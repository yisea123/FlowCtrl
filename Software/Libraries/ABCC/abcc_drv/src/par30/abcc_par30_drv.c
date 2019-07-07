/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Driver version 5.02.01 (2016-11-02)                                   **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** File Description:
** Parallel (PARI) driver implementation.
********************************************************************************
********************************************************************************
*/
#include "abcc_drv_cfg.h"

#if( ABCC_CFG_DRV_PARALLEL_30 )

#include "abcc_td.h"
#include "../abcc_debug_err.h"
#include "abcc_sys_adapt.h"
#include "../abcc_timer.h"
#include "../abcc_drv_if.h"
#include "../abcc_mem.h"
#include "abp.h"
#include "abcc_sys_adapt_par.h"
#include "abcc_port.h"

#if ( ABCC_CFG_MAX_MSG_SIZE < 16 )
#error "ABCC_CFG_MAX_MSG_SIZE must be at least a 16 bytes"
#endif

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

typedef enum ABCC_DrvPar30State
{
   SM_RDY_TO_PING,
   SM_WAITING_FOR_PONG,
   SM_SER_INIT
}
ABCC_DrvPar30StateType;

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static ABP_MsgType* par_drv_pbReadMessageData;
static  UINT8* par_drv_pbRdPdBuffer;

static UINT16   par_drv_iSizeOfReadPd;
static UINT16   par_drv_iSizeOfWritePd;

static   UINT8    par_drv_bNbrOfCmds;          /* Number of commands supported by the application. */
static ABCC_TimerHandle xWdTmoHandle;
static BOOL  fWdTmo;                          /* Current wd timeout status */
static BOOL drv_fLegacy;                      /* USe legacy format         */

/*
** Working copies of the status and control registers
*/
static volatile UINT8   bControlReg = ABP_CTRL_R_BIT;
static volatile UINT8   bStatusReg  = 0x80;
static volatile ABCC_DrvPar30StateType eState = SM_SER_INIT;



/*******************************************************************************
** Private forward declarations
********************************************************************************
*/


/*******************************************************************************
** Private Services
********************************************************************************
*/
/*
** Converting to/from legacy message format.
**
**  ABCC40 msg header      Leagcy msg header
** --------------------
** | UINT16 iDataSize |
** --------------------
** | UINT16 iReserved |
** --------------------    --------------------
** | UINT8 bSourceId  |    | UINT8 bSourceId  |
** --------------------    --------------------
** | UINT8 bDestObj   |    | UINT8 bDestObj   |
** --------------------    --------------------
** | UINT8 bSourceId  |    | UINT8 bSourceId  |
** --------------------    --------------------
** | UINT16 iInstance |    | UINT16 iInstance |
** --------------------    --------------------
** | UINT8 bReserved  |    | UINT8 bDataSize  |
** --------------------    --------------------
** | UINT8 bCmdExt0   |    | UINT8 bCmdExt0   |
** --------------------    --------------------
** | UINT8 bCmdExt1   |    | UINT8 bCmdExt1   |
** --------------------    --------------------
**
** The last 8 bytes in the new message format
** structured the same way as the legacy message format
** except for bReserved which is placed on the legacy
** bDataSize position.
**
** This means that the conversion from "new" meessage format
** to legacy format
** is simply done by copying the iDataSize to the bReserved
** filed and the message is sent from the bsourceId address position.
**
**
** Read message are converted the same way. The data from
** ACI are copied to the bSourceId address position. The bReserved
** field are copied to the iDataSize filed.
**
** Note that the iDataSize need to be converted to/from little endian.
**
*/
static void drv_DoMsgWrite( ABP_MsgType* psMsg )
{
   /*
   ** First Calculate total message size
   */
   UINT16 iMsgSize = iLeTOi( psMsg->sHeader.iDataSize );

   ABCC_ASSERT( iMsgSize < ABP_MAX_MSG_DATA_BYTES );
   iMsgSize += sizeof( ABP_MsgHeaderType );

   ABCC_DrvParallelWrite( ABP_WRMSG_ADR_OFFSET, (UINT8*)psMsg, iMsgSize );
}


static void drv_DoLegacyMsgWrite( ABP_MsgType* psMsg )
{
   UINT16 iMsgSize = iLeTOi( psMsg->sHeader.iDataSize );

   ABCC_ASSERT( iMsgSize <= ABP_MAX_MSG_255_DATA_BYTES );
   iMsgSize += sizeof( ABP_Msg255HeaderType );
   psMsg->sHeader.bReserved = (UINT8)psMsg->sHeader.iDataSize;
   ABCC_DrvParallelWrite( ABP_WRMSG_LEGACY_ADR_OFFSET, (UINT8*)&psMsg->sHeader.bSourceId, iMsgSize );
}

static void drv_DoMsgRead( ABP_MsgType* psMsg )
{
   UINT16 iMsgSize;

   ABCC_DrvParallelRead( ABP_RDMSG_ADR_OFFSET,
                         (UINT8*)psMsg,
                         sizeof( ABP_MsgHeaderType ) );

   iMsgSize = iLeTOi( psMsg->sHeader.iDataSize );

   if( ( iMsgSize <= ABCC_CFG_MAX_MSG_SIZE ) &&
       ( iMsgSize > 0 ) )
   {
      /*
      ** There is data and it fits in buffer size, so the message is read.
      */
      ABCC_DrvParallelRead( ABP_RDMSG_ADR_OFFSET + sizeof( ABP_MsgHeaderType ),
                            psMsg->abData,
                            iMsgSize );
   }
}


void drv_DoLegacyMsgRead( ABP_MsgType* psMsg )
{
   UINT16 iMsgSize;

   ABCC_DrvParallelRead( ABP_RDMSG_LEGACY_ADR_OFFSET,
                         (UINT8*)&psMsg->sHeader.bSourceId,
                         sizeof( ABP_Msg255HeaderType ) );
   iMsgSize = (UINT16)( psMsg->sHeader.bReserved );

   psMsg->sHeader.iDataSize = iTOiLe( iMsgSize );

   if( ( iMsgSize <= ABCC_CFG_MAX_MSG_SIZE ) &&
       ( iMsgSize > 0 ) )
   {
      /*
      ** There is data and it fits in buffer size, so the message is read.
      */
      ABCC_DrvParallelRead( ABP_RDMSG_LEGACY_ADR_OFFSET + sizeof( ABP_Msg255HeaderType ),
                            psMsg->abData,
                            iMsgSize );
   }
}


static void drv_WdTimeoutHandler( void )
{
   fWdTmo = TRUE;
   ABCC_CbfWdTimeout();
}

/*******************************************************************************
** Public Services
********************************************************************************
*/
void ABCC_DrvPar30Init( UINT8 bOpmode )
{
   /*
   ** Initialize privates and states.
   */
   ABCC_ASSERT_ERR( bOpmode == 8 , ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)bOpmode );

   par_drv_iSizeOfReadPd  = 0;
   par_drv_iSizeOfWritePd = 0;
   par_drv_bNbrOfCmds     = 0;
   par_drv_pbRdPdBuffer = ABCC_DrvParallel30GetRdPdBuffer();
   bControlReg = ABP_CTRL_R_BIT | ABP_CTRL_T_BIT;
   bStatusReg  = 0;
   eState = SM_SER_INIT;
   xWdTmoHandle = ABCC_TimerCreate( drv_WdTimeoutHandler );
   fWdTmo = FALSE;

   /* If an ABCC40 is attached then use big messages */
   if ( ABCC_ReadModuleId() == ABP_MODULE_ID_ACTIVE_ABCC40 )
   {
      drv_fLegacy = FALSE;
   }
   else
   {
      drv_fLegacy = TRUE;
   }
}

void ABCC_DrvPar30SetIntMask( const UINT16 iIntMask )
{
 /*
  ** Not possible to set interrupt mask.
  */
 (void)iIntMask;
}

#if( ABCC_CFG_INT_ENABLED )
UINT16 ABCC_DrvPar30ISR( void )
{
   /*
    ** Interrupt is acknowledged when status register is read
    */
   ABCC_DrvRead8( ABP_STATUS_ADR_OFFSET );

   return 0;
}
#else
UINT16 ABCC_DrvPar30ISR( void )
{
   ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_INTERNAL_ERROR, 0);
   return 0;
}
#endif


ABP_MsgType* ABCC_DrvPar30RunDriverRx( void )
{
   UINT8 bNewStatusReg1;
   UINT8 bNewStatusReg2;

   if ( eState == SM_WAITING_FOR_PONG )
   {

      do
      {
          bNewStatusReg1 = ABCC_DrvRead8( ABP_STATUS_ADR_OFFSET );
          bNewStatusReg2 = ABCC_DrvRead8( ABP_STATUS_ADR_OFFSET );
      } while ( bNewStatusReg1 != bNewStatusReg2 );


      if( ( bNewStatusReg1 & ABP_STAT_T_BIT ) != ( bStatusReg & ABP_STAT_T_BIT ) )
      {
          bStatusReg = bNewStatusReg1;
          bControlReg &= ~ABP_CTRL_M_BIT;
          bControlReg ^= ABP_CTRL_T_BIT;

          ABCC_TimerStop( xWdTmoHandle );
          /*
           ** Check if timeout has occurred.
           */
          if( fWdTmo == TRUE )
          {
             ABCC_CbfWdTimeoutRecovered();
          }
          fWdTmo = FALSE;
          eState = SM_RDY_TO_PING;
      }
   }
   else if ( eState == SM_SER_INIT )
   {
      eState = SM_RDY_TO_PING;
   }

   return NULL;
}


void ABCC_DrvPar30RunDriverTx( void )
{
   if(eState == SM_RDY_TO_PING )
    {
       ABCC_DrvWrite8( ABP_CONTROL_ADR_OFFSET, bControlReg );
       /*
       ** Start WD timer
       */
       ABCC_TimerStart(xWdTmoHandle, ABCC_CFG_WD_TIMEOUT_MS );
       eState = SM_WAITING_FOR_PONG;
    }
}


void ABCC_DrvPar30PrepareWriteMessage( ABP_MsgType* psWriteMsg )
{
   ABCC_ASSERT_ERR( psWriteMsg, ABCC_SEV_FATAL, ABCC_EC_UNEXPECTED_NULL_PTR, (UINT32)psWriteMsg );


   if( drv_fLegacy )
   {
     drv_DoLegacyMsgWrite( psWriteMsg );
   }
   else
   {
      drv_DoMsgWrite( psWriteMsg );
   }
}

BOOL ABCC_DrvPar30WriteMessage( ABP_MsgType* psWriteMsg )
{
   ABCC_ASSERT_ERR( psWriteMsg, ABCC_SEV_FATAL, ABCC_EC_UNEXPECTED_NULL_PTR, (UINT32)psWriteMsg );

   bControlReg |= ABP_CTRL_M_BIT;

   /*
   ** Determine if command messages (instead of response messages) can be sent.
   */
   if( !( psWriteMsg->sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) )
   {
      /*
      ** A command message has been received by the host application and a
      ** response message (not a command message) will be sent back to the
      ** Anybus. The number of commands the host application can receive
      ** shall be increased by one, as a previous received command is now
      ** handled.
      */
      par_drv_bNbrOfCmds++;

      /*
      ** When a change of the number of commands which the host application
      ** can receive is made from 0 to 1, it means that we can set the APPRF
      ** flag again to indicate for the Anybus that the host is now ready to
      ** receive a new command.
      */
       bControlReg |= ABP_CTRL_R_BIT;;
   }

   /*
   ** Update the buffer control register.
   */
    return( TRUE);
}


void ABCC_DrvPar30WriteProcessData( void* pxProcessData )
{

   if(  par_drv_iSizeOfWritePd )
   {
      /*
      ** Write process data.
      */
#if !( ABCC_CFG_MEMORY_MAPPED_ACCESS )
      ABCC_SYS_ParallelWrite( ABP_WRPDM_ADR_OFFSET,
                              pxProcessData,
                              par_drv_iSizeOfWritePd );
#else
      (void)pxProcessData;
#endif
   }
}


BOOL ABCC_DrvPar30IsReadyForWriteMessage( void )
{
   BOOL fRdyForCmd =  FALSE;

   if( eState == SM_RDY_TO_PING  )
   {
      if( !( bControlReg & ABP_CTRL_M_BIT ) )
      {
         fRdyForCmd = TRUE;
      }
   }
   return fRdyForCmd;
}


BOOL ABCC_DrvPar30IsReadyForCmd( void )
{
   BOOL fRdyForCmd =  FALSE;

   if( eState == SM_RDY_TO_PING )
   {
      if( ( bStatusReg & ABP_STAT_R_BIT ) && !( bControlReg & ABP_CTRL_M_BIT ) )
      {
         fRdyForCmd = TRUE;
      }
   }

   return fRdyForCmd;
}


void ABCC_DrvPar30SetNbrOfCmds( UINT8 bNbrOfCmds )
{
   par_drv_bNbrOfCmds = bNbrOfCmds;

   /*
   ** Acknowledge that we are ready to accept the first command message.
   */
   bControlReg |= ABP_CTRL_R_BIT;
}


void ABCC_DrvPar30SetAppStatus( ABP_AppStatusType eAppStatus )
{
   (void)eAppStatus;
}

void ABCC_DrvPar30SetPdSize( const UINT16 iReadPdSize, const UINT16 iWritePdSize )
{
   par_drv_iSizeOfReadPd = iReadPdSize;
   par_drv_iSizeOfWritePd = iWritePdSize;
}


static void DrvPar30SetMsgReceiverBuffer( ABP_MsgType* const psReadMsg )
{
   /*
   ** The buffer can be NULL if we are out of msg resources.
   ** We can not start a new ping-pong before this is resolved.
   */
   par_drv_pbReadMessageData = psReadMsg;
}


UINT16 ABCC_DrvPar30GetIntStatus( void )
{
   return  0;
}

UINT8 ABCC_DrvPar30GetAnybusState( void )
{
   /*
   ** The Anybus state is stored in bits 0-2 of the status register.
   */
   return( bStatusReg & 0x07 );
}


void* ABCC_DrvPar30ReadProcessData( void )
{
   /*
    ** TODO Check valid data
    */

   if ( ( par_drv_iSizeOfReadPd == 0)  || ( eState  == SM_WAITING_FOR_PONG ) )
   {
      return NULL;
   }

#if !( ABCC_CFG_MEMORY_MAPPED_ACCESS )
   ABCC_SYS_ParallelRead( ABP_RDPDM_ADR_OFFSET,
                          par_drv_pbRdPdBuffer,
                          par_drv_iSizeOfReadPd );
#endif

   return( par_drv_pbRdPdBuffer );
}

ABP_MsgType* ABCC_DrvPar30ReadMessage( void )
{
   if( eState == SM_RDY_TO_PING )
   {
      if( bStatusReg & ABP_STAT_M_BIT  )
      {
         DrvPar30SetMsgReceiverBuffer( ABCC_MemAlloc() );

         if( par_drv_pbReadMessageData == NULL )
         {
            ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_OUT_OF_MSG_BUFFERS, 0 );
            return( NULL );
         }

         if( drv_fLegacy )
         {
           drv_DoLegacyMsgRead( par_drv_pbReadMessageData );
         }
         else
         {
            drv_DoMsgRead( par_drv_pbReadMessageData );
         }

         /*
         ** Determine if command messages (instead of response messages) can be read.
         */
         if( par_drv_pbReadMessageData->sHeader.bCmd & ABP_MSG_HEADER_C_BIT )
         {
            /*
            ** A command messages has been sent by the Anybus and it has been read
            ** by the host application. The number of commands allowed by the host
            ** application must be decreased by one.
            */
            par_drv_bNbrOfCmds--;

            /*
            ** Indicates that the host application is not ready to receive a new
            ** command from the Anybus. Writing to this register must only be done
            ** when the RDMSG bit is set to 1. A check is not required however,
            ** since the RDMSG bit is set to 1 a few lines higher up in the code.
            */
            if( par_drv_bNbrOfCmds == 0 )
            {
               /*
               ** Update the buffer control register.
               */
               bControlReg &= ~ABP_CTRL_R_BIT;
            }
         }

         return( par_drv_pbReadMessageData );
      }
   }
   return( NULL );
}

BOOL ABCC_DrvPar30IsReadyForWrPd( void )
{
   if ( eState == SM_RDY_TO_PING )
   {
      return TRUE;
   }

   return FALSE;
}

UINT8* ABCC_DrvPar30GetWrPdBuffer( void )
{
   return ABCC_DrvParallel30GetWrPdBuffer();
}


UINT16 ABCC_DrvPar30GetModCap( void )
{
   return 0xFFFF;
}

UINT16 ABCC_DrvPar30GetLedStatus( void )
{
   return 0;
}

BOOL ABCC_DrvPar30IsSupervised( void )
{
   /*
   ** The Anybus supervision bis is stored in bit 3
   */
   return( ( bStatusReg >> 3 ) & 1 );
}

UINT8 ABCC_DrvPar30GetAnbStatus( void )
{
   return bStatusReg & 0xf ;
}



#endif
/*******************************************************************************
** End of par_drv.c
********************************************************************************
*/
