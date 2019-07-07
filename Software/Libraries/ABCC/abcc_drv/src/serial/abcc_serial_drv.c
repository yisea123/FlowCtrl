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
** Implementation of serial driver ping protocol
********************************************************************************
********************************************************************************
*/
#include "abcc_drv_cfg.h"

#if( ABCC_CFG_DRV_SERIAL )

#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "../abcc_debug_err.h"
#include "abcc_sys_adapt.h"
#include "../abcc_timer.h"
#include "../abcc_drv_if.h"
#include "../abcc_mem.h"
#include "abcc_crc16.h"
#include "abcc_sys_adapt_ser.h"
#include "abcc_port.h"
#include "abcc_drv_ser_if.h"

#if ( ABCC_CFG_MAX_MSG_SIZE < 16 )
#error "ABCC_CFG_MAX_MSG_SIZE must be at least a 16 bytes"
#endif

/*******************************************************************************
** Constants
********************************************************************************
*/

#define SER_CRC_LEN ( 2 )
#define SER_MSG_FRAG_LEN ( 16 )

/*******************************************************************************
** Typedefs
********************************************************************************
*/

typedef struct
{
  UINT8*             pbCurrPtr;           /* Pointer to the current position in the send buffer. */
  INT16              iNumBytesLeft;       /* Number of bytes left to send. */
  UINT16             iFragLength;         /* Current fragmentation block length. */
} WrMsgFragType;


typedef struct
{
  UINT8*             pbCurrPtr;           /* Pointer to the current position in receive buffer. */
  UINT16             iNumBytesReceived;   /* Number of bytes received. */
  UINT16             iFragLength;         /* Current fragmentation block length. */
  UINT16             iMaxLength;          /* Max num bytes to receive*/
} RdMsgFragType;


typedef struct SerTxTelegramType
{
   UINT8 bControl;
   UINT8 abWrMsg[ SER_MSG_FRAG_LEN ];
   UINT8 abData[ ABCC_CFG_MAX_PROCESS_DATA_SIZE + SER_CRC_LEN ];
} SerTxTelegramType;


typedef struct SerRxTelegramType
{
   UINT8 bStatus;
   UINT8 abRdMsg[ SER_MSG_FRAG_LEN ];
   UINT8 abData[ ABCC_CFG_MAX_PROCESS_DATA_SIZE + SER_CRC_LEN ];
} SerRxTelegramType;


/*------------------------------------------------------------------------------
** Internal states.
**------------------------------------------------------------------------------
*/
typedef enum
{
   SM_SER_INIT = 0,
   SM_SER_RDY_TO_SEND_PING,
   SM_SER_WAITING_FOR_PONG
} drv_SerStateType;

/*******************************************************************************
** Private Globals
********************************************************************************
*/


/*------------------------------------------------------------------------------
** General privates.
**------------------------------------------------------------------------------
*/


static drv_SerStateType             drv_eState;               /* Serial driver state. */
static UINT8                        drv_bStatus;              /* Latest received status        */


static ABP_MsgType*                 drv_psWriteMessage;       /* Pointer to active write message. */

static ABP_MsgType*                 drv_psReadMessage;        /* Pointer to the read message. */
static BOOL                         drv_fNewReadMessage;      /* Indicate that new message has arrived. */

static UINT16                       drv_iWritePdSize;         /* Current write PD size. */
static UINT16                       drv_iReadPdSize;          /* Current read PD size. */

static UINT16                       drv_iTxFrameSize;        /* Current ping frame size. */
static UINT16                       drv_iRxFrameSize;        /* Current ping frame size. */


static UINT8                        drv_bNbrOfCmds;           /* Number of commands that can be received by the application */

static SerRxTelegramType            drv_sRxTelegram;          /* Place holder for Rx telegram */
static SerTxTelegramType            drv_sTxTelegram;          /* Place holder for Tx telegram */

static WrMsgFragType                sTxFragHandle;
static RdMsgFragType                sRxFragHandle;
static BOOL                         fSendWriteMessageEndMark;  /* Indicate end of message */


static BOOL                         drv_fNewRxTelegramReceived;   /* Serail driver has a complete message */
static UINT8*                       drv_bpRdPd;                 /* Pointer to valid read process data */


/*
 ** Timers and watchdogs
 */
static ABCC_TimerHandle xWdTmoHandle;
static BOOL                         fWdTmo;                          /* Current wd timeout status */
static ABCC_TimerHandle                 xTelegramTmoHandle;
static BOOL                         fTelegramTmo;                    /* Current telegram tmo status */
static UINT16                       iTelegramTmoMs;                  /* Telegram timeout  */


/*******************************************************************************
** Private forward declarations.
********************************************************************************
*/
static void DrvSerSetMsgReceiverBuffer( ABP_MsgType* const psReadMsg );

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Callback from the physical layer to indicate that a RX telegran was received.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
static void drv_RxTelegramReceived( void )
{
   drv_fNewRxTelegramReceived = TRUE;
}


/*------------------------------------------------------------------------------
** Init write message fragmentation
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
**       psMsg          Pointer t0 start of message
**       iFragLength    Fragment length
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
static void drv_WriteFragInit( WrMsgFragType* psFragHandle, UINT8* psMsg , UINT16 iMsgSize, UINT16 iFragLength )
{
    psFragHandle->iNumBytesLeft = iMsgSize;
    psFragHandle->iFragLength = iFragLength;
    psFragHandle->pbCurrPtr = psMsg;
}


/*------------------------------------------------------------------------------
** Get current write message fragment
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
**       pbBuffer       Pointer to destination buffer
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
static void drv_GetWriteFrag( WrMsgFragType* const psFragHandle , UINT8* const pbBuffer )
{
   /*
   ** Copy the message into the MOSI frame buffer.
   */
   ABCC_ASSERT( psFragHandle->iNumBytesLeft > 0 );

   ABCC_PORT_MemCpy( pbBuffer,
                     psFragHandle->pbCurrPtr,
                     psFragHandle->iFragLength );
}


/*------------------------------------------------------------------------------
** Update to next Write fragment. Returns TRUE if if nothing to send.
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
**       pbBuffer       Pointer to destination buffer
** Returns:
**       TRUE if the whole message is sent .
**------------------------------------------------------------------------------
*/
static BOOL drv_PrepareNextWriteFrag( WrMsgFragType* const psFragHandle )
{
   BOOL fFragDone = FALSE;

   if( psFragHandle->iNumBytesLeft > 0 )
   {
      psFragHandle->pbCurrPtr         += psFragHandle->iFragLength;
      psFragHandle->iNumBytesLeft     -= psFragHandle->iFragLength;
   }

   if ( psFragHandle->iNumBytesLeft <= 0 )
   {
      fFragDone = TRUE;
   }
   return fFragDone;
}

/*------------------------------------------------------------------------------
** Check if write message sending is in progress
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
** Returns:
**       TRUE if sending is ongoing.
**------------------------------------------------------------------------------
*/
static BOOL drv_isWrMsgSendingInprogress( WrMsgFragType* const psFragHandle )
{
   return ( psFragHandle->pbCurrPtr != 0 );
}


/*------------------------------------------------------------------------------
** Init read message fragmentation.
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
**       psMsg          Pointer to start of message
**       iFragLength    Fragment length
**       iMaxMsgLength  MAximum lenth of message
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
static void drv_InitReadFrag( RdMsgFragType* psFragHandle, UINT8* psMsg , UINT16 iFragLength, UINT16 iMaxMsgLength )
{
    psFragHandle->iNumBytesReceived = 0;
    psFragHandle->iFragLength = iFragLength;
    psFragHandle->pbCurrPtr = psMsg;
    psFragHandle->iMaxLength = iMaxMsgLength;
}


/*------------------------------------------------------------------------------
** Add read message fragment
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to write fragmentation information
**       pbBuffer       Pointer to source buffer
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
static void drv_AddReadFrag( RdMsgFragType* const psFragHandle , UINT8* const pbBuffer )
{
   if( ( psFragHandle->iNumBytesReceived + psFragHandle->iFragLength ) > psFragHandle->iMaxLength )
   {
      /*
      ** Message size exceeds buffer. Don't fill up the buffer, this is handled
      ** in higher layers.
      */
      return;
   }

   /*
   ** Copy the message into buffer.
   */
   psFragHandle->iNumBytesReceived += psFragHandle->iFragLength;

   ABCC_PORT_MemCpy( psFragHandle->pbCurrPtr,
                     pbBuffer,
                     psFragHandle->iFragLength );

   psFragHandle->pbCurrPtr += psFragHandle->iFragLength;
}


/*------------------------------------------------------------------------------
** Check if read message receiving is in progress
**------------------------------------------------------------------------------
** Arguments:
**       psFragHandle   Pointer to read fragmentation information
** Returns:
**       TRUE if the.
**------------------------------------------------------------------------------
*/
static BOOL drv_isRdMsgReceiveInprogress( RdMsgFragType* const psFragHandle )
{
   return (psFragHandle->pbCurrPtr != 0 );
}


static void drv_WdTimeoutHandler( void )
{
   fWdTmo = TRUE;
   ABCC_CbfWdTimeout();
}


static void drv_TelegramTimeoutHandler( void )
{
   fTelegramTmo = TRUE;
}


/*******************************************************************************
** Public Services
********************************************************************************
*/
void ABCC_DrvSerInit( UINT8 bOpmode )
{
   fSendWriteMessageEndMark = FALSE;

   /*
   ** Initialize privates and states.
   */
   ABCC_ASSERT_ERR( ( bOpmode > 8 ) && ( bOpmode < 13 ), ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)bOpmode );

   drv_psReadMessage = 0;
   drv_psWriteMessage = 0;

   drv_bNbrOfCmds = 0;
   drv_eState = SM_SER_INIT;

   drv_iWritePdSize = 0;
   drv_iReadPdSize = 0;

   drv_iTxFrameSize = 17 + drv_iWritePdSize;
   drv_iRxFrameSize = 17 + drv_iReadPdSize;


   drv_bpRdPd = NULL;

   drv_sTxTelegram.bControl = 0;
   drv_sRxTelegram.bStatus = 0;
   drv_bStatus = 0;

   drv_InitReadFrag(&sRxFragHandle,0,0,0);
   drv_WriteFragInit( &sTxFragHandle ,0 ,0 ,0 );

   drv_fNewRxTelegramReceived = FALSE;
   drv_fNewReadMessage = FALSE;

   xTelegramTmoHandle = ABCC_TimerCreate( drv_TelegramTimeoutHandler );
   fTelegramTmo = FALSE;

   switch ( bOpmode )
   {
   case 9: /* 19200 baud */
      iTelegramTmoMs = 350;
   break;
   case 10: /* 57600 baud */
      iTelegramTmoMs = 120;
   break;
   case 11: /* 115200 baud */
      iTelegramTmoMs = 60;
   break;
   case 12: /* 625000 baud */
      iTelegramTmoMs = 20;
   break;
   default:
      ABCC_ASSERT(FALSE);
   }

      xWdTmoHandle = ABCC_TimerCreate( drv_WdTimeoutHandler );
   fWdTmo = FALSE;

   /*
   ** Register the PONG indicator for the physical serial driver.
   */
   ABCC_SYS_SerRegDataReceived( drv_RxTelegramReceived );
}

/*------------------------------------------------------------------------------
**  Handles preparation and sending of TX telegram
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       -
**------------------------------------------------------------------------------
*/
void ABCC_DrvSerRunDriverTx( void )
{
   UINT16 iCrc;
   BOOL   fHandleWriteMsg = FALSE;

   ABCC_PORT_UseCritical();

   if( drv_eState == SM_SER_RDY_TO_SEND_PING )
   {
      drv_eState = SM_SER_WAITING_FOR_PONG;
      drv_sTxTelegram.bControl &= ABP_CTRL_T_BIT;

      if ( !fTelegramTmo )
      {
         /*
         ** Everything is OK. Reset retransmission and toggle the T bit.
         */
         drv_sTxTelegram.bControl ^= ABP_CTRL_T_BIT;

         ABCC_PORT_EnterCritical();

         if ( ( drv_psWriteMessage != 0 ) && !drv_isWrMsgSendingInprogress( &sTxFragHandle ) )
         {
            drv_psWriteMessage->sHeader.bReserved = (UINT8)( iLeTOi( drv_psWriteMessage->sHeader.iDataSize ) );
            drv_WriteFragInit(&sTxFragHandle ,&drv_psWriteMessage->sHeader.bSourceId, (UINT16)drv_psWriteMessage->sHeader.bReserved + 8, SER_MSG_FRAG_LEN );
         }

         ABCC_PORT_ExitCritical();
      }

      fTelegramTmo = FALSE;

      /*---------------------------------------------------------------------------
      ** Write message handling.
      **---------------------------------------------------------------------------
      */

      if ( drv_isWrMsgSendingInprogress( &sTxFragHandle ) )
      {
         fHandleWriteMsg = TRUE;
      }


     drv_sTxTelegram.bControl &= ~ABP_CTRL_M_BIT;

     if ( fHandleWriteMsg )
      {
         if ( !fSendWriteMessageEndMark )
         {
            drv_GetWriteFrag( &sTxFragHandle, drv_sTxTelegram.abWrMsg );
            drv_sTxTelegram.bControl |= ABP_CTRL_M_BIT;
         }
         else
         {
            /*
            ** Position to update the rx frame size to match the length of
            ** the new RdPd size after a read remap.
            ** The last fragment of the remap response has been sent
            ** and the ABCC will adjust the length in the next frame.
            */
            if( ( drv_psWriteMessage->sHeader.bDestObj == ABP_OBJ_NUM_APPD ) &&
                ( drv_psWriteMessage->sHeader.bCmd == ABP_APPD_REMAP_ADI_READ_AREA ) )
            {
               if( pnABCC_DrvCbfReadRemapDone != NULL )
               {
                  pnABCC_DrvCbfReadRemapDone( drv_psWriteMessage );
               }
            }
         }
      }

      if( drv_bNbrOfCmds > 0 )
      {
         drv_sTxTelegram.bControl |= ABP_CTRL_R_BIT;
      }

      drv_bpRdPd = NULL;

      /*
      ** Apply the CRC checksum.
      */
      iCrc = CRC_Crc16( (UINT8*)&drv_sTxTelegram, drv_iTxFrameSize );

      drv_sTxTelegram.abData[ drv_iWritePdSize + 1] = (UINT8)(iCrc & 0xFF );
      drv_sTxTelegram.abData[ drv_iWritePdSize  ] = (UINT8)(iCrc >> 8 );

      /*
      ** Send  TX telegram and received Rx telegram.
      */
      ABCC_TimerStart( xTelegramTmoHandle, iTelegramTmoMs );
      ABCC_SYS_SerSendReceive( (UINT8*)&drv_sTxTelegram,  (UINT8*)&drv_sRxTelegram, drv_iTxFrameSize + SER_CRC_LEN, drv_iRxFrameSize + SER_CRC_LEN );
   }
}


/*------------------------------------------------------------------------------
**  Handle the reception of the Rx telegram
**------------------------------------------------------------------------------
** Arguments:
**       psResp:  Pointer to the response message.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
ABP_MsgType* ABCC_DrvSerRunDriverRx( void )
{
   UINT16 iCalcCrc;
   UINT16 iReceivedCrc;

   ABP_MsgType* psWriteMsg = NULL;

   if ( drv_eState == SM_SER_WAITING_FOR_PONG )
   {
      if( !drv_fNewRxTelegramReceived )
      {
         if ( fTelegramTmo )
         {
            ABCC_SYS_SerRestart();
            drv_eState = SM_SER_RDY_TO_SEND_PING;
         }

         /*
         ** Nothing has happened. No pong was received.
         */
         return NULL;
      }

      /*
       ** Telegram received
       */
      drv_fNewRxTelegramReceived = FALSE;


      iReceivedCrc = CRC_Crc16( (UINT8*)&drv_sRxTelegram, drv_iRxFrameSize );

      /*
      ** Read the CRC that is sent with the telegram.
      */
      iCalcCrc  = (UINT16)drv_sRxTelegram.abData[ drv_iReadPdSize ] << 8 ;
      iCalcCrc |= (UINT16)drv_sRxTelegram.abData[ drv_iReadPdSize + 1];

      if( ( ( drv_bStatus & ABP_CTRL_T_BIT ) ==
            ( drv_sRxTelegram.bStatus & ABP_CTRL_T_BIT ) ) ||
          ( iCalcCrc != iReceivedCrc ) )
      {
         ABCC_SYS_SerRestart();
         return NULL;
      }

      if( fWdTmo )
      {
        ABCC_CbfWdTimeoutRecovered();
      }

      /*
      ** Correct telgram received
      */
      ABCC_TimerStop( xTelegramTmoHandle );
      fTelegramTmo = FALSE;

      ABCC_TimerStop( xWdTmoHandle );
      fWdTmo = FALSE;

      /*
      ** Restart watchdog
      */
      ABCC_TimerStart( xWdTmoHandle, ABCC_CFG_WD_TIMEOUT_MS );

      /*
      ** Save the current anybus status.
      */
      drv_bStatus =  drv_sRxTelegram.bStatus;
      drv_bpRdPd = drv_sRxTelegram.abData;

      /*---------------------------------------------------------------------------
      ** Write message handling.
      **---------------------------------------------------------------------------
      */
      if ( drv_isWrMsgSendingInprogress( &sTxFragHandle ) )
      {
         /*
          ** End mark is succesfully sent
          */
         if ( fSendWriteMessageEndMark )
         {
            fSendWriteMessageEndMark = FALSE;
            drv_WriteFragInit(&sTxFragHandle ,0 ,0 ,0);
            psWriteMsg = drv_psWriteMessage;

            /*
            ** Update the application flow control.
            */
            if ( ( drv_psWriteMessage->sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) == 0 )
            {
               drv_bNbrOfCmds++;
            }

            drv_psWriteMessage = 0;
         }
         else
         {
            fSendWriteMessageEndMark = drv_PrepareNextWriteFrag(&sTxFragHandle );
         }
      }
      /*---------------------------------------------------------------------------
      ** Read message handling
      ** --------------------------------------------------------------------------
      */

      if( drv_bStatus & ABP_STAT_M_BIT  )
      {

         if ( !drv_isRdMsgReceiveInprogress( &sRxFragHandle ) )
         {
            if( drv_psReadMessage == NULL )
            {
               DrvSerSetMsgReceiverBuffer( ABCC_MemAlloc() );

               if( drv_psReadMessage == NULL )
               {
                  ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_OUT_OF_MSG_BUFFERS, 0 );
                  return( NULL );
               }
            }

            /*
             ** Start receiving on legacy start position which corresponds to &drv_psReadMessage->sHeader.bSourceId
             */
            drv_InitReadFrag(&sRxFragHandle, &drv_psReadMessage->sHeader.bSourceId, SER_MSG_FRAG_LEN, ABCC_CFG_MAX_MSG_SIZE + 8 );
         }

         drv_AddReadFrag(&sRxFragHandle, drv_sRxTelegram.abRdMsg);

      }
      else
      {
         if ( drv_isRdMsgReceiveInprogress( &sRxFragHandle ) )
         {
            /*
            ** Empty message endmarker received
            ** Copy old message format size parameter to Large message format used by the driver
            */
            drv_psReadMessage->sHeader.iDataSize = iTOiLe( (UINT16)drv_psReadMessage->sHeader.bReserved );

            /*
            ** Update the application flow control.
            */
            if ( drv_psReadMessage->sHeader.bCmd & ABP_MSG_HEADER_C_BIT )
            {
               drv_bNbrOfCmds--;
            }
            drv_fNewReadMessage = TRUE;
            drv_InitReadFrag(&sRxFragHandle ,0,0,0);
         }
      }
      drv_eState = SM_SER_RDY_TO_SEND_PING;
   }
   else if (drv_eState == SM_SER_INIT )
   {
      ABCC_TimerStart( xWdTmoHandle, ABCC_CFG_WD_TIMEOUT_MS );
      drv_eState = SM_SER_RDY_TO_SEND_PING;
   }
   return psWriteMsg;
}


UINT16 ABCC_DrvSerISR( void )
{
   return 0;
}


BOOL ABCC_DrvSerWriteMessage( ABP_MsgType* psWriteMsg )
{
   ABCC_PORT_UseCritical();
   ABCC_ASSERT_ERR( psWriteMsg, ABCC_SEV_FATAL, ABCC_EC_UNEXPECTED_NULL_PTR, (UINT32)psWriteMsg );

   ABCC_PORT_EnterCritical();
   ABCC_ASSERT_ERR( drv_psWriteMessage == NULL, ABCC_SEV_FATAL, ABCC_EC_UNEXPECTED_NULL_PTR, (UINT32)drv_psWriteMessage );

   drv_psWriteMessage = psWriteMsg;
   ABCC_PORT_ExitCritical();

   /*
   ** The serial driver owns the buffer.
   */
   return( FALSE );
}

void ABCC_DrvSerWriteProcessData( void* pxProcessData )
{
   (void)pxProcessData;
   /*
    ** Nothing needs to be done here since the buffer is already updated by the application
   */
}

/*
** This function must be called from a critical section.
*/
BOOL ABCC_DrvSerIsReadyForWriteMessage( void )
{
   BOOL fRdyForWrMsg = FALSE;

   if ( drv_psWriteMessage == NULL )
   {
      fRdyForWrMsg = TRUE;
   }
   return fRdyForWrMsg;
}


/*
** This function must be called from a critical section.
*/
BOOL ABCC_DrvSerIsReadyForCmd( void )
{
   return ( pnABCC_DrvISReadyForWriteMessage() &&  ( drv_bStatus & ABP_STAT_R_BIT ) );
}

void ABCC_DrvSerSetNbrOfCmds( UINT8 bNbrOfCmds )
{
   drv_bNbrOfCmds = bNbrOfCmds;
}

void ABCC_DrvSerSetAppStatus( ABP_AppStatusType eAppStatus )
{
   (void)eAppStatus;
}

void ABCC_DrvSerSetPdSize( const UINT16  iReadPdSize, const UINT16  iWritePdSize)
{
   /*
   **  Update lengths dependent on pd sizes
   */
   drv_iWritePdSize = iWritePdSize;
   drv_iReadPdSize = iReadPdSize;
   drv_iTxFrameSize = 17 + drv_iWritePdSize;
   drv_iRxFrameSize = 17 + drv_iReadPdSize;
}

static void DrvSerSetMsgReceiverBuffer( ABP_MsgType* const psReadMsg )
{
   /*
   ** The buffer can be NULL if we are out of msg resources.
   ** We can not start a new ping-pong before this is resolved.
   */
   drv_psReadMessage = psReadMsg;
}


UINT16 ABCC_DrvSerGetIntStatus( void )
{
   ABCC_ERROR(ABCC_SEV_FATAL, ABCC_EC_INTSTATUS_NOT_SUPPORTED_BY_DRV_IMPL, 0);

   return 0;
}

UINT8 ABCC_DrvSerGetAnybusState( void )
{
   return drv_bStatus & 0x7;
}


void* ABCC_DrvSerReadProcessData( void )
{
    return drv_bpRdPd;
}


ABP_MsgType* ABCC_DrvSerReadMessage( void )
{
   ABP_MsgType* psMsg = NULL;

   if( drv_fNewReadMessage )
   {
      psMsg = drv_psReadMessage;
      drv_fNewReadMessage = FALSE;
      drv_psReadMessage = NULL;
   }

   return( psMsg );
}


void ABCC_DrvSerSetIntMask( const UINT16 iIntMask )
{
   (void)iIntMask;
   /*
    ** Not possible to set interrupt mask for serial driver.
    */
}

void* ABCC_DrvSerGetWrPdBuffer( void )
{
   /*
    ** Return position to WrPd position in tx telegraam
    */
   return drv_sTxTelegram.abData;
}


UINT16 ABCC_DrvSerGetModCap( void )
{
   ABCC_ERROR( ABCC_SEV_WARNING , ABCC_EC_MODCAP_NOT_SUPPORTED_BY_DRV_IMPL, 0);
   return 0;
}

UINT16 ABCC_DrvSerGetLedStatus( void )
{
   ABCC_ERROR( ABCC_SEV_WARNING , ABCC_EC_MODCAP_NOT_SUPPORTED_BY_DRV_IMPL, 0);
   return 0;
}

BOOL ABCC_DrvSerIsReadyForWrPd( void )
{
   if ( drv_eState == SM_SER_RDY_TO_SEND_PING )
   {
      return TRUE;
   }
   return FALSE;
}

BOOL ABCC_DrvSerIsSupervised( void )
{
   /*
   ** The Anybus supervision bit is stored in bit 3
   */
   return( ( drv_bStatus >> 3 ) & 1 );
}

UINT8 ABCC_DrvSerGetAnbStatus( void )
{
   return drv_bStatus & 0xf ;
}
#endif /* End of #if( ABCC_CFG_DRV_SERIAL ) */

