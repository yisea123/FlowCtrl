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
** Implementation of operation mode independent parts of the abcc handler.
********************************************************************************
********************************************************************************
*/

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abcc_drv_if.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_link.h"
#include "abcc_cmd_seq.h"
#include "abcc_mem.h"
#include "abcc_sys_adapt.h"
#include "abcc_debug_err.h"
#include "abcc_handler.h"
#include "abcc_timer.h"
#include "abcc_setup.h"
#include "abcc_port.h"

#if( ABCC_CFG_DRV_SPI )
#include "spi/abcc_drv_spi_if.h"
#endif
#if( ABCC_CFG_DRV_PARALLEL )
#include "par/abcc_drv_par_if.h"
#endif
#if( ABCC_CFG_DRV_PARALLEL_30 )
#include "par30/abcc_drv_par30_if.h"
#endif
#if( ABCC_CFG_DRV_SERIAL )
#include "serial/abcc_drv_ser_if.h"
#endif

#if !ABCC_CFG_OP_MODE_GETTABLE
   #ifdef ABCC_CFG_ABCC_OP_MODE_30
      #if( ( ABCC_CFG_ABCC_OP_MODE_30 != ABP_OP_MODE_8_BIT_PARALLEL ) && \
           ( ABCC_CFG_ABCC_OP_MODE_30 != ABP_OP_MODE_SERIAL_19_2 ) &&    \
           ( ABCC_CFG_ABCC_OP_MODE_30 != ABP_OP_MODE_SERIAL_57_6 ) &&    \
           ( ABCC_CFG_ABCC_OP_MODE_30 != ABP_OP_MODE_SERIAL_115_2 ) &&   \
           ( ABCC_CFG_ABCC_OP_MODE_30 != ABP_OP_MODE_SERIAL_625 ) )
         #error No valid Operation Mode has been selected for ABCC30 operation! Check abcc_drv_cfg.h.
      #endif
   #endif

   #ifdef ABCC_CFG_ABCC_OP_MODE_40
      #if( ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_SPI ) &&             \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_16_BIT_PARALLEL ) && \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_8_BIT_PARALLEL ) &&  \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_SERIAL_19_2 ) &&     \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_SERIAL_57_6 ) &&     \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_SERIAL_115_2 ) &&    \
           ( ABCC_CFG_ABCC_OP_MODE_40 != ABP_OP_MODE_SERIAL_625 ) )
         #error No valid Operation Mode has been selected for ABCC40 operation! Check abcc_drv_cfg.h.
      #endif
   #endif
#endif

/*
** Registerd handler functions
*/
EXTFUNC ABCC_ErrorCodeType ABCC_SpiRunDriver( void );
EXTFUNC void ABCC_SpiISR( void );

EXTFUNC ABCC_ErrorCodeType ABCC_ParRunDriver( void );
EXTFUNC void ABCC_ParISR( void );

EXTFUNC ABCC_ErrorCodeType ABCC_Par30RunDriver( void );
EXTFUNC void ABCC_Par30ISR( void );

EXTFUNC ABCC_ErrorCodeType ABCC_SerRunDriver( void );
EXTFUNC void ABCC_SerISR( void );




/*
 ** Default USER defines
 */
#ifndef ABCC_CFG_STARTUP_TIME_MS
#define ABCC_CFG_STARTUP_TIME_MS ( 1500 )
#endif

/********************************************************************************
** Public Globals
********************************************************************************
*/

/*
** Registered handler functions
*/

ABCC_ErrorCodeType ( *ABCC_RunDriver )( void );
void ( *ABCC_ISR )( void );
void ( *ABCC_TriggerWrPdUpdate )( void );

/*
** The interrupt mask that has been set to the ABCC at start up.
*/
UINT16 ABCC_iInterruptEnableMask;

/*
** Registerd driver functions
*/

void  ( *pnABCC_DrvInit )( UINT8 bOpmode );
UINT16 ( *pnABCC_DrvISR )( void );
void ( *pnABCC_DrvRunDriverTx )( void );
ABP_MsgType* ( *pnABCC_DrvRunDriverRx )( void );
void ( *pnABCC_DrvPrepareWriteMessage) ( ABP_MsgType* psWriteMsg );
BOOL ( *pnABCC_DrvWriteMessage) ( ABP_MsgType* psWriteMsg );
void ( *pnABCC_DrvWriteProcessData )( void* pbProcessData );
BOOL ( *pnABCC_DrvISReadyForWrPd )( void );
BOOL ( *pnABCC_DrvISReadyForWriteMessage )( void );
BOOL ( *pnABCC_DrvISReadyForCmd )( void );
void ( *pnABCC_DrvSetNbrOfCmds )( UINT8 bNbrOfCmds );
void ( *pnABCC_DrvSetAppStatus )( ABP_AppStatusType eAppStatus );
void ( *pnABCC_DrvSetPdSize )( const UINT16 iReadPdSize, const UINT16 iWritePdSize );
void ( *pnABCC_DrvSetIntMask )( const UINT16 iIntMask );
void* ( *pnABCC_DrvGetWrPdBuffer )( void );
UINT16 ( *pnABCC_DrvGetModCap )( void );
UINT16 ( *pnABCC_DrvGetLedStatus )( void );
UINT16 ( *pnABCC_DrvGetIntStatus )( void );
UINT8 ( *pnABCC_DrvGetAnybusState )( void );
void* ( *pnABCC_DrvReadProcessData )( void );
ABP_MsgType* ( *pnABCC_DrvReadMessage )( void );
BOOL ( *pnABCC_DrvIsSupervised )( void );
UINT8 ( *pnABCC_DrvGetAnbStatus )( void );

#if( ABCC_CFG_SYNC_MEASUREMENT_IP )
BOOL fAbccUserSyncMeasurementIp;
#endif

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static volatile UINT8 abcc_bAnbState = 0xff;

static ABCC_MainStateType abcc_eMainState = ABCC_DRV_INIT;

/*
** Pointer to WRPD buffer.
*/
static void* abcc_pbWrPdBuffer;

/*
 ** Tmo handler for
 */
static ABCC_TimerHandle abcc_TmoHandle;

/*
 ** Indicate ready for communication
 */
static BOOL abcc_fReadyForCommunicationTmo = FALSE;
static BOOL abcc_fReadyForCommunication = FALSE;

/*
 ** Current operation mode
 */
static UINT8 abcc_bOpmode = 0;

#if( ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_PARALLEL_30 || ABCC_CFG_DRV_SERIAL )
/*
 ** Flag to indicate that WrPD update shall be done
 */
static BOOL abcc_fDoWrPdUpdate = FALSE;
#endif

/*
** The Application status register value of the Anybus module
*/
static volatile ABP_AppStatusType abcc_eAppStatus = ABP_APPSTAT_NO_ERROR;

/*******************************************************************************
** Private Services
********************************************************************************
*/

static void TriggerWrPdUpdateNow( void )
{
   if( ABCC_GetMainState() == ABCC_DRV_RUNNING )
   {
      /*
      ** Send new "write process data" to the Anybus-CC.
      ** The data format of the process data is network specific.
      ** The application converts the data accordingly.
      */
      if( pnABCC_DrvISReadyForWrPd() )
      {
         if( ABCC_CbfUpdateWriteProcessData( abcc_pbWrPdBuffer ) )
         {
            pnABCC_DrvWriteProcessData( abcc_pbWrPdBuffer );
#if( ABCC_CFG_SYNC_MEASUREMENT_IP )
            if( ABCC_GetOpmode() == ABP_OP_MODE_SPI )
            {
               fAbccUserSyncMeasurementIp = TRUE;
            }
            else
            {
               ABCC_SYS_GpioReset();
            }
#endif
         }
      }
   }
}

#if( ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_PARALLEL_30 || ABCC_CFG_DRV_SERIAL )
static void TriggerWrPdUpdateLater( void )
{
   abcc_fDoWrPdUpdate = TRUE;
}
#endif

static BOOL IsInterruptInUse( void )
{
   BOOL fReturn;

   fReturn = FALSE;
#if( ABCC_CFG_INT_ENABLED )
   switch( abcc_bOpmode )
   {
   case ABP_OP_MODE_16_BIT_PARALLEL:
   case ABP_OP_MODE_8_BIT_PARALLEL:
   case ABP_OP_MODE_SPI:
      fReturn = TRUE;
      break;

   default:
      break;
   }

   return( fReturn );
#else
   return( fReturn );
#endif /* End of #if ABCC_CFG_INT_ENABLED */
}

static BOOL IsPolledInterruptInUse( void )
{
   BOOL fReturn;

   fReturn = FALSE;
#if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
   switch( abcc_bOpmode )
   {
   case ABP_OP_MODE_16_BIT_PARALLEL:
   case ABP_OP_MODE_8_BIT_PARALLEL:
   case ABP_OP_MODE_SPI:
      fReturn = TRUE;
      break;

   default:

      break;
   }

   return( fReturn );
#else
   return( fReturn );
#endif /* End of #if ABCC_CFG_POLL_ABCC_IRQ_PIN */
}

static void SetReadyForCommunicationTmo( void )
{
   abcc_fReadyForCommunicationTmo = TRUE;
}

/*******************************************************************************
** Public Service
********************************************************************************
*/

#if( ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_PARALLEL_30 || ABCC_CFG_DRV_SERIAL )
void ABCC_CheckWrPdUpdate( void )
{
   if( abcc_fDoWrPdUpdate )
   {
      abcc_fDoWrPdUpdate = FALSE;
      TriggerWrPdUpdateNow();
   }
}
#endif

void ABCC_SetReadyForCommunication( void )
{
   abcc_fReadyForCommunication = TRUE;
}

void ABCC_SetMainStateError( void )
{
   abcc_eMainState = ABCC_DRV_ERROR;
}

ABCC_MainStateType ABCC_GetMainState( void )
{
   return( abcc_eMainState );
}

void ABCC_TriggerAnbStatusUpdate( void )
{
   UINT8 bAnbState;

   bAnbState = pnABCC_DrvGetAnybusState();
   if( bAnbState != abcc_bAnbState )
   {
      abcc_bAnbState = bAnbState;
      ABCC_CbfAnbStateChanged( (ABP_AnbStateType)bAnbState );
   }
}

void ABCC_TriggerTransmitMessage( void )
{
   ABCC_LinkCheckSendMessage();
}

#if ( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
void ABCC_GpioReset( void )
{
   ABCC_SYS_GpioReset();
}
#endif

#if ( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
void ABCC_GpioSet( void )
{
   ABCC_SYS_GpioSet();
}
#endif

ABCC_ErrorCodeType ABCC_HwInit( void )
{
   if( !ABCC_SYS_HwInit() )
   {
      return( ABCC_EC_HW_INIT_FAILED );
   }
   return( ABCC_EC_NO_ERROR );
}


ABCC_ErrorCodeType ABCC_StartDriver( UINT32 lMaxStartupTimeMs )
{
   UINT8 bModuleId;

   if( lMaxStartupTimeMs == 0 )
   {
      lMaxStartupTimeMs = ABCC_CFG_STARTUP_TIME_MS;
   }

   bModuleId = ABCC_ReadModuleId();

#if( ABCC_CFG_DRV_SERIAL || ABCC_CFG_DRV_PARALLEL_30 )
   if( ( bModuleId != ABP_MODULE_ID_ACTIVE_ABCC40 ) && ( bModuleId != ABP_MODULE_ID_ACTIVE_ABCC30 ) )
#elif( ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_PARALLEL )
   if( bModuleId != ABP_MODULE_ID_ACTIVE_ABCC40 )
#endif
   {
      ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_MODULE_ID_NOT_SUPPORTED, (UINT32)bModuleId );

      return( ABCC_EC_MODULE_ID_NOT_SUPPORTED );
   }

   abcc_bOpmode = ABCC_GetOpmode();

   switch( abcc_bOpmode )
   {
#if( ABCC_CFG_DRV_SERIAL )
   case ABP_OP_MODE_SERIAL_19_2:
   case ABP_OP_MODE_SERIAL_57_6:
   case ABP_OP_MODE_SERIAL_115_2:
   case ABP_OP_MODE_SERIAL_625:

      ABCC_ISR                   = NULL;
      ABCC_RunDriver             = &ABCC_SerRunDriver;
      ABCC_TriggerWrPdUpdate     = &TriggerWrPdUpdateLater;

      pnABCC_DrvInit               = &ABCC_DrvSerInit;
      pnABCC_DrvISR                = &ABCC_DrvSerISR;
      pnABCC_DrvRunDriverTx        = &ABCC_DrvSerRunDriverTx;
      pnABCC_DrvRunDriverRx        = &ABCC_DrvSerRunDriverRx;
      pnABCC_DrvPrepareWriteMessage = NULL;
      pnABCC_DrvWriteMessage       = &ABCC_DrvSerWriteMessage;
      pnABCC_DrvWriteProcessData   = &ABCC_DrvSerWriteProcessData;
      pnABCC_DrvISReadyForWrPd     = &ABCC_DrvSerIsReadyForWrPd;
      pnABCC_DrvISReadyForWriteMessage = &ABCC_DrvSerIsReadyForWriteMessage;
      pnABCC_DrvISReadyForCmd      = &ABCC_DrvSerIsReadyForCmd;
      pnABCC_DrvSetNbrOfCmds       = &ABCC_DrvSerSetNbrOfCmds;
      pnABCC_DrvSetAppStatus       = &ABCC_DrvSerSetAppStatus;
      pnABCC_DrvSetPdSize          = &ABCC_DrvSerSetPdSize;
      pnABCC_DrvSetIntMask         = &ABCC_DrvSerSetIntMask;
      pnABCC_DrvGetWrPdBuffer      = &ABCC_DrvSerGetWrPdBuffer;
      pnABCC_DrvGetModCap          = &ABCC_DrvSerGetModCap;
      pnABCC_DrvGetLedStatus       = &ABCC_DrvSerGetLedStatus;
      pnABCC_DrvGetIntStatus       = &ABCC_DrvSerGetIntStatus;
      pnABCC_DrvGetAnybusState     = &ABCC_DrvSerGetAnybusState;
      pnABCC_DrvReadProcessData    = &ABCC_DrvSerReadProcessData;
      pnABCC_DrvReadMessage        = &ABCC_DrvSerReadMessage;
      pnABCC_DrvIsSupervised       = &ABCC_DrvSerIsSupervised;
      pnABCC_DrvGetAnbStatus       = &ABCC_DrvSerGetAnbStatus;

      ABCC_iInterruptEnableMask = 0;

      break;
#endif /* End of #if ABCC_CFG_DRV_SERIAL */
#if( ABCC_CFG_DRV_SPI )
   case ABP_OP_MODE_SPI:

      if( bModuleId == ABP_MODULE_ID_ACTIVE_ABCC40 )
      {
         ABCC_ISR                   = &ABCC_SpiISR;
         ABCC_RunDriver             = &ABCC_SpiRunDriver;
         ABCC_TriggerWrPdUpdate     = &TriggerWrPdUpdateLater;

         pnABCC_DrvInit               = &ABCC_DrvSpiInit;
         pnABCC_DrvISR                = NULL;
         pnABCC_DrvRunDriverTx        = &ABCC_DrvSpiRunDriverTx;
         pnABCC_DrvRunDriverRx        = &ABCC_DrvSpiRunDriverRx;
         pnABCC_DrvPrepareWriteMessage = NULL;
         pnABCC_DrvWriteMessage       = &ABCC_DrvSpiWriteMessage;
         pnABCC_DrvWriteProcessData   = &ABCC_DrvSpiWriteProcessData;
         pnABCC_DrvISReadyForWrPd     = &ABCC_DrvSpiIsReadyForWrPd;
         pnABCC_DrvISReadyForWriteMessage = &ABCC_DrvSpiIsReadyForWriteMessage;
         pnABCC_DrvISReadyForCmd      = &ABCC_DrvSpiIsReadyForCmd;
         pnABCC_DrvSetNbrOfCmds       = &ABCC_DrvSpiSetNbrOfCmds;
         pnABCC_DrvSetAppStatus       = &ABCC_DrvSpiSetAppStatus;
         pnABCC_DrvSetPdSize          = &ABCC_DrvSpiSetPdSize;
         pnABCC_DrvSetIntMask         = &ABCC_DrvSpiSetIntMask;
         pnABCC_DrvGetWrPdBuffer      = &ABCC_DrvSpiGetWrPdBuffer;
         pnABCC_DrvGetModCap          = &ABCC_DrvSpiGetModCap;
         pnABCC_DrvGetLedStatus       = &ABCC_DrvSpiGetLedStatus;
         pnABCC_DrvGetIntStatus       = &ABCC_DrvSpiGetIntStatus;
         pnABCC_DrvGetAnybusState     = &ABCC_DrvSpiGetAnybusState;
         pnABCC_DrvReadProcessData    = &ABCC_DrvSpiReadProcessData;
         pnABCC_DrvReadMessage        = &ABCC_DrvSpiReadMessage;
         pnABCC_DrvIsSupervised       = &ABCC_DrvSpiIsSupervised;
         pnABCC_DrvGetAnbStatus       = &ABCC_DrvSpiGetAnbStatus;

         ABCC_iInterruptEnableMask = ABCC_CFG_INT_ENABLE_MASK_SPI;
      }
      else
      {
         ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)abcc_bOpmode );
      }

      break;
#endif /* End of #if ABCC_CFG_DRV_SPI */
#if( ABCC_CFG_DRV_PARALLEL || ABCC_CFG_DRV_PARALLEL_30 )
   case ABP_OP_MODE_8_BIT_PARALLEL:
#if( ABCC_CFG_DRV_PARALLEL )
      if( bModuleId == ABP_MODULE_ID_ACTIVE_ABCC30 )
#endif /* End of #if ABCC_CFG_DRV_PARALLEL */
      {
#if( ABCC_CFG_DRV_PARALLEL_30 )
         ABCC_ISR                   = &ABCC_Par30ISR;
         ABCC_RunDriver             = &ABCC_Par30RunDriver;
         ABCC_TriggerWrPdUpdate     = &TriggerWrPdUpdateLater;

         pnABCC_DrvInit               = &ABCC_DrvPar30Init;
         pnABCC_DrvISR                = &ABCC_DrvPar30ISR;
         pnABCC_DrvRunDriverTx        = &ABCC_DrvPar30RunDriverTx;
         pnABCC_DrvRunDriverRx        = &ABCC_DrvPar30RunDriverRx;
         pnABCC_DrvPrepareWriteMessage = &ABCC_DrvPar30PrepareWriteMessage;
         pnABCC_DrvWriteMessage       = &ABCC_DrvPar30WriteMessage;
         pnABCC_DrvWriteProcessData   = &ABCC_DrvPar30WriteProcessData;
         pnABCC_DrvISReadyForWrPd     = &ABCC_DrvPar30IsReadyForWrPd;
         pnABCC_DrvISReadyForWriteMessage = &ABCC_DrvPar30IsReadyForWriteMessage;
         pnABCC_DrvISReadyForCmd      = &ABCC_DrvPar30IsReadyForCmd;
         pnABCC_DrvSetNbrOfCmds       = &ABCC_DrvPar30SetNbrOfCmds;
         pnABCC_DrvSetAppStatus       = &ABCC_DrvPar30SetAppStatus;
         pnABCC_DrvSetPdSize          = &ABCC_DrvPar30SetPdSize;
         pnABCC_DrvSetIntMask         = &ABCC_DrvPar30SetIntMask;
         pnABCC_DrvGetWrPdBuffer      = &ABCC_DrvPar30GetWrPdBuffer;
         pnABCC_DrvGetModCap          = &ABCC_DrvPar30GetModCap;
         pnABCC_DrvGetLedStatus       = &ABCC_DrvPar30GetLedStatus;
         pnABCC_DrvGetIntStatus       = &ABCC_DrvPar30GetIntStatus;
         pnABCC_DrvGetAnybusState     = &ABCC_DrvPar30GetAnybusState;
         pnABCC_DrvReadProcessData    = &ABCC_DrvPar30ReadProcessData;
         pnABCC_DrvReadMessage        = &ABCC_DrvPar30ReadMessage;
         pnABCC_DrvIsSupervised       = &ABCC_DrvPar30IsSupervised;
         pnABCC_DrvGetAnbStatus       = &ABCC_DrvPar30GetAnbStatus;

         ABCC_iInterruptEnableMask = ABCC_CFG_INT_ENABLE_MASK_PAR30;
#else
         ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)abcc_bOpmode );
#endif /* End of #if ABCC_CFG_DRV_PARALLEL_30 */
         break;
      }

      /*
      ** If event driven parallel operating mode is enabled and an ABCC 40
      ** module is mounted fall through to the 16-bit parallel operating mode
      ** case which sets up the event driven parallel operating mode.
      */
#endif /* End of #if ABCC_CFG_DRV_PARALLEL or ABCC_CFG_DRV_PARALLEL_30 */
#if( ABCC_CFG_DRV_PARALLEL )
   case ABP_OP_MODE_16_BIT_PARALLEL:

      if( bModuleId == ABP_MODULE_ID_ACTIVE_ABCC30 )
      {
         ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)abcc_bOpmode );

         break;
      }

      ABCC_ISR                   = &ABCC_ParISR;
      ABCC_RunDriver             = &ABCC_ParRunDriver;
      ABCC_TriggerWrPdUpdate     = &TriggerWrPdUpdateNow;

      pnABCC_DrvInit               = &ABCC_DrvParInit;
      pnABCC_DrvISR                = &ABCC_DrvParISR;
      pnABCC_DrvRunDriverTx        = NULL;
      pnABCC_DrvRunDriverRx        = &ABCC_DrvParRunDriverRx;
      pnABCC_DrvPrepareWriteMessage = &ABCC_DrvParPrepareWriteMessage;
      pnABCC_DrvWriteMessage       = &ABCC_DrvParWriteMessage;
      pnABCC_DrvWriteProcessData   = &ABCC_DrvParWriteProcessData;
      pnABCC_DrvISReadyForWrPd     = &ABCC_DrvParIsReadyForWrPd;
      pnABCC_DrvISReadyForWriteMessage = &ABCC_DrvParIsReadyForWriteMessage;
      pnABCC_DrvISReadyForCmd      = &ABCC_DrvParIsReadyForCmd;
      pnABCC_DrvSetNbrOfCmds       = &ABCC_DrvParSetNbrOfCmds;
      pnABCC_DrvSetAppStatus       = &ABCC_DrvParSetAppStatus;
      pnABCC_DrvSetPdSize          = &ABCC_DrvParSetPdSize;
      pnABCC_DrvSetIntMask         = &ABCC_DrvParSetIntMask;
      pnABCC_DrvGetWrPdBuffer      = &ABCC_DrvParGetWrPdBuffer;
      pnABCC_DrvGetModCap          = &ABCC_DrvParGetModCap;
      pnABCC_DrvGetLedStatus       = &ABCC_DrvParGetLedStatus;
      pnABCC_DrvGetIntStatus       = &ABCC_DrvParGetIntStatus;
      pnABCC_DrvGetAnybusState     = &ABCC_DrvParGetAnybusState;
      pnABCC_DrvReadProcessData    = &ABCC_DrvParReadProcessData;
      pnABCC_DrvReadMessage        = &ABCC_DrvParReadMessage;
      pnABCC_DrvIsSupervised       = &ABCC_DrvParIsSupervised;
      pnABCC_DrvGetAnbStatus       = &ABCC_DrvParGetAnbStatus;

#if ABCC_CFG_INT_ENABLED
      ABCC_iInterruptEnableMask = ABCC_CFG_INT_ENABLE_MASK_PAR;

#if ABCC_CFG_SYNC_ENABLE && !ABCC_CFG_USE_ABCC_SYNC_SIGNAL
      ABCC_iInterruptEnableMask |= ABP_INTMASK_SYNCIEN;
#endif
#else
      ABCC_iInterruptEnableMask = 0;
#endif

      break;
#endif /* End of #if ABCC_CFG_DRV_PARALLEL */
   default:

      ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE, (UINT32)abcc_bOpmode );

      return( ABCC_EC_INCORRECT_OPERATING_MODE );
   }

   if ( !( ( abcc_eMainState == ABCC_DRV_INIT )  ||
           ( abcc_eMainState == ABCC_DRV_SHUTDOWN ) ) )
   {
      ABCC_ERROR(ABCC_SEV_FATAL, ABCC_EC_INCORRECT_STATE, (UINT32)abcc_eMainState );
      abcc_eMainState = ABCC_DRV_ERROR;

      return( ABCC_EC_INCORRECT_STATE );
   }

   if ( !ABCC_SYS_Init() )
   {
      return( ABCC_EC_INTERNAL_ERROR );
   }

   ABCC_TimerInit();
   pnABCC_DrvInit( abcc_bOpmode );

   ABCC_LinkInit();
#if ABCC_CFG_DRV_CMD_SEQ_ENABLE
   ABCC_InitCmdSequencer();
#endif
   ABCC_SetupInit();

   abcc_bAnbState = 0xff;

   abcc_TmoHandle = ABCC_TimerCreate( SetReadyForCommunicationTmo );

   abcc_pbWrPdBuffer = pnABCC_DrvGetWrPdBuffer();

   if( !ABCC_ModuleDetect() )
   {
      ABCC_ERROR(ABCC_SEV_WARNING, ABCC_EC_MODULE_NOT_DECTECTED, 0);

      return( ABCC_EC_MODULE_NOT_DECTECTED );
   }

#if( ABCC_CFG_OP_MODE_SETTABLE )
   ABCC_SYS_SetOpmode( abcc_bOpmode );
#endif

   abcc_fReadyForCommunicationTmo = FALSE;
   abcc_fReadyForCommunication = FALSE;

#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
   ABCC_SYS_SyncInterruptEnable();
#endif

#if( ABCC_CFG_INT_ENABLED )
   if( IsInterruptInUse() )
   {
         ABCC_SYS_AbccInterruptEnable();
   }
#endif /* End of #if ABCC_CFG_INT_ENABLED */

   abcc_eMainState = ABCC_DRV_WAIT_COMMUNICATION_RDY;

   ABCC_TimerStart( abcc_TmoHandle, lMaxStartupTimeMs );

   return( ABCC_EC_NO_ERROR );
}

ABCC_CommunicationStateType ABCC_isReadyForCommunication( void )
{
   if( abcc_eMainState > ABCC_DRV_WAIT_COMMUNICATION_RDY )
   {
      return( ABCC_READY_FOR_COMMUNICATION );
   }

   if( abcc_eMainState < ABCC_DRV_WAIT_COMMUNICATION_RDY )
   {
      return( ABCC_NOT_READY_FOR_COMMUNICATION );
   }

   if( abcc_fReadyForCommunicationTmo == TRUE )
   {
      if( IsInterruptInUse() || IsPolledInterruptInUse() )
      {
         return( ABCC_COMMUNICATION_ERROR );
      }
      else
      {
         abcc_fReadyForCommunication = TRUE;
      }
   }

#if( !ABCC_CFG_INT_ENABLED ) && ( ABCC_CFG_POLL_ABCC_IRQ_PIN )
   if( IsPolledInterruptInUse() )
   {
      abcc_fReadyForCommunication = ABCC_SYS_IsAbccInterruptActive();
   }
#endif

   if( abcc_fReadyForCommunication == TRUE )
   {
      pnABCC_DrvSetIntMask( ABCC_iInterruptEnableMask );
      abcc_eMainState = ABCC_DRV_RUNNING;
      pnABCC_DrvSetNbrOfCmds( ABCC_CFG_MAX_NUM_APPL_CMDS );

      ABCC_StartSetup();
      return( ABCC_READY_FOR_COMMUNICATION );
   }

   return( ABCC_NOT_READY_FOR_COMMUNICATION );
}


void ABCC_NewWrPdEvent( void )
{
   if( ABCC_GetMainState() == ABCC_DRV_RUNNING )
   {
      /*
      ** Send new "write process data" to the Anybus-CC.
      ** The data format of the process data is network specific.
      ** The application converts the data accordingly.
      */
      if ( pnABCC_DrvISReadyForWrPd() )
      {
         if( ABCC_CbfUpdateWriteProcessData( abcc_pbWrPdBuffer ) )
         {
            pnABCC_DrvWriteProcessData( abcc_pbWrPdBuffer );
#if( ABCC_CFG_SYNC_MEASUREMENT_IP )
            if( ABCC_GetOpmode() == ABP_OP_MODE_SPI )
            {
               fAbccUserSyncMeasurementIp = TRUE;
            }
            else
            {
               ABCC_SYS_GpioReset();
            }
#endif
         }
      }
   }
}



void ABCC_TriggerRdPdUpdate( void )
{
   void* bpRdPd;

#if( ABCC_CFG_SYNC_MEASUREMENT_OP )
   ABCC_SYS_GpioSet();
#endif

   bpRdPd = pnABCC_DrvReadProcessData();

   if( bpRdPd )
   {
      if( pnABCC_DrvGetAnybusState() == ABP_ANB_STATE_PROCESS_ACTIVE  )
      {
         /*
         ** The "read process data" is only valid in the PROCESS_ACTIVE state.
         ** Retrieve the new "read process data" from the Anybus-CC.
         ** The data format of the process data is network specific.
         ** Convert it to our native format.
         */
         ABCC_CbfNewReadPd( bpRdPd );
      }
   }

#if( ABCC_CFG_SYNC_MEASUREMENT_OP )
   /*
   ** This is the Output Valid point (for OuputValidTime = 0). The
   ** applications has received data and handled it. Thus we reset the
   ** ABCC_CFG_SYNC_MEASUREMENT_OP measurement.
   */
   ABCC_SYS_GpioReset();
#endif
}

void ABCC_TriggerReceiveMessage ( void )
{
   ABCC_MsgType sRdMsg;

   sRdMsg.psMsg = ABCC_LinkReadMessage();

   if( sRdMsg.psMsg == NULL )
   {
      return;
   }

   ABCC_DEBUG_MSG_DATA( "Msg received", sRdMsg.psMsg );
   /*
   ** Set buffer status to indicate that the buffer is handed over to the
   ** application.
   */
   ABCC_MemSetBufferStatus( sRdMsg.psMsg, ABCC_MEM_BUFSTAT_IN_APPL_HANDLER );
   /*
   ** A new message is available.
   */
   if( ABCC_GetLowAddrOct( sRdMsg.psMsg16->sHeader.iCmdReserved ) & ABP_MSG_HEADER_C_BIT )
   {
      /*
      ** Check so that messages exceeding ABCC_CFG_MAX_MSG_SIZE are handled. The
      ** actual buffer protection is done in the operating mode driver.
      */
      if( ABCC_GetMsgDataSize( sRdMsg.psMsg ) > ABCC_CFG_MAX_MSG_SIZE )
      {
         ABCC_ERROR( ABCC_SEV_INFORMATION,
                     ABCC_EC_RCV_CMD_SIZE_EXCEEDS_BUFFER,
                     (UINT32)ABCC_GetMsgDataSize( sRdMsg.psMsg ) );
         ABP_SetMsgErrorResponse( sRdMsg.psMsg, 1, ABP_ERR_NO_RESOURCES );
         ABCC_SendRespMsg( sRdMsg.psMsg );
      }
      else
      {
         /*
         ** The message is a command, let the application respond.
         */
         ABCC_CbfReceiveMsg( sRdMsg.psMsg );
      }
   }
   else
   {
      /*
      ** Check so that messages exceeding ABCC_CFG_MAX_MSG_SIZE are handled.
      */
      if( ABCC_GetMsgDataSize( sRdMsg.psMsg ) > ABCC_CFG_MAX_MSG_SIZE )
      {
         (void)ABCC_LinkGetMsgHandler( ABCC_GetLowAddrOct( sRdMsg.psMsg16->sHeader.iSourceIdDestObj ) );
         ABCC_ERROR( ABCC_SEV_INFORMATION,
                     ABCC_EC_RCV_RESP_SIZE_EXCEEDS_BUFFER,
                     (UINT32)ABCC_GetMsgDataSize( sRdMsg.psMsg ) );
      }
      else
      {
         ABCC_MsgHandlerFuncType pnMsgHandler = 0;
         pnMsgHandler = ABCC_LinkGetMsgHandler( ABCC_GetLowAddrOct( sRdMsg.psMsg16->sHeader.iSourceIdDestObj ) );

         if( pnMsgHandler )
         {
            ABCC_DEBUG_MSG_EVENT( "Routing response to registered response handler", sRdMsg.psMsg );
            pnMsgHandler( sRdMsg.psMsg );
         }
         else
         {
            ABCC_DEBUG_MSG_EVENT( "No response handler found", sRdMsg.psMsg );
            ABCC_CbfReceiveMsg( sRdMsg.psMsg );
         }
      }
   }

   if( ABCC_MemGetBufferStatus( sRdMsg.psMsg ) == ABCC_MEM_BUFSTAT_IN_APPL_HANDLER )
   {
      /*
      ** The status has not been changed while the user processed the response
      ** message. Then this buffer shall be freed by the driver.
      */
      ABCC_ReturnMsgBuffer( &sRdMsg.psMsg );
   }
}

ABCC_ErrorCodeType ABCC_SendCmdMsg( ABP_MsgType*  psCmdMsg, ABCC_MsgHandlerFuncType pnMsgHandler )
{
   ABCC_ErrorCodeType eResult;
   ABCC_MsgType sMsg;

   sMsg.psMsg = psCmdMsg;

   /*
   ** Register function to handle response.
   ** Must be done before sending the message to avoid race condition.
   */
   if( ABCC_LinkMapMsgHandler( ABCC_GetLowAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj ),
                               pnMsgHandler ) == ABCC_EC_NO_ERROR )
   {
      eResult = ABCC_LinkWriteMessage( sMsg.psMsg );
      if( eResult != ABCC_EC_NO_ERROR )
      {
         /*
         ** Free message handler resource
         */
         (void)ABCC_LinkGetMsgHandler( ABCC_GetLowAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj ) );
      }
   }
   else
   {
      eResult = ABCC_EC_NO_RESOURCES;

      /*
      ** Report error
      */
      ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_NO_RESOURCES, 0 );
   }

   return( eResult );
}

UINT16 ABCC_GetCmdQueueSize( void )
{
   return( ABCC_LinkGetNumCmdQueueEntries() );
}


ABCC_ErrorCodeType ABCC_SendRespMsg( ABP_MsgType* psMsgResp )
{
   return( ABCC_LinkWriteMessage( psMsgResp ) );
}

ABP_MsgType* ABCC_GetCmdMsgBuffer( void )
{
   if( ABCC_GetCmdQueueSize() == 0 )
   {
      return( NULL );
   }
   return( ABCC_MemAlloc() );
}

ABCC_ErrorCodeType ABCC_ReturnMsgBuffer( ABP_MsgType** ppsBuffer )
{
   ABCC_LinkFree( ppsBuffer );

   return( ABCC_EC_NO_ERROR );
}

void ABCC_TakeMsgBufferOwnership( ABP_MsgType* psMsg )
{
   ABCC_MemSetBufferStatus( psMsg, ABCC_MEM_BUFSTAT_OWNED );
}

void ABCC_SetPdSize( const UINT16 iReadPdSize, const UINT16 iWritePdSize )
{
   DEBUG_EVENT(  "New process data sizes RdPd %d WrPd %d\n", iReadPdSize, iWritePdSize  );
   pnABCC_DrvSetPdSize( iReadPdSize, iWritePdSize );
}


void ABCC_HWReset( void )
{
   DEBUG_EVENT(  "HW Reset\n"  );
   ABCC_ShutdownDriver();
   ABCC_SYS_HWReset();
}


void ABCC_ShutdownDriver( void )
{
   DEBUG_EVENT(  " Enter Shutdown state\n"  );

#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
   ABCC_SYS_SyncInterruptDisable();
#endif

#if( ABCC_CFG_INT_ENABLED )
   ABCC_SYS_AbccInterruptDisable();
#endif
   ABCC_SYS_Close();
   ABCC_TimerDisable();
   abcc_eMainState = ABCC_DRV_SHUTDOWN;
}


BOOL ABCC_ModuleDetect( void )
{
#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
   return( ABCC_SYS_ModuleDetect() );
#else
   return( TRUE );
#endif
}

UINT16 ABCC_ModCap( void )
{
   return( pnABCC_DrvGetModCap() );
}

UINT16 ABCC_LedStatus()
{
   return( pnABCC_DrvGetLedStatus() );
}

UINT8 ABCC_AnbState( void )
{
   return( pnABCC_DrvGetAnybusState() );
}

BOOL ABCC_IsSupervised( void )
{
   return( pnABCC_DrvIsSupervised() );
}

void ABCC_HWReleaseReset( void )
{
   ABCC_SYS_HWReleaseReset();
}

ABP_AppStatusType ABCC_GetAppStatus( void )
{
   return( abcc_eAppStatus );
}

void ABCC_SetAppStatus( ABP_AppStatusType eAppStatus )
{
   if( abcc_eAppStatus != eAppStatus )
   {
      abcc_eAppStatus = eAppStatus;
      pnABCC_DrvSetAppStatus( eAppStatus );
   }
}

UINT8 ABCC_ReadModuleId( void )
{
#ifdef ABCC_CFG_ABCC_MODULE_ID
   return( ABCC_CFG_ABCC_MODULE_ID );
#else
   return( ABCC_SYS_ReadModuleId() );
#endif
}

void ABCC_RunTimerSystem( const INT16 iDeltaTimeMs )
{
   ABCC_TimerTick( iDeltaTimeMs );
}


UINT8 ABCC_GetNewSourceId( void )
{
   static UINT8 bSourceId = 0;
   UINT8 bTempSrcId;
   ABCC_PORT_UseCritical();

   do
   {
      ABCC_PORT_EnterCritical();
      bTempSrcId = ++bSourceId;
      ABCC_PORT_ExitCritical();
   }  while (  ABCC_LinkIsSrcIdUsed( bTempSrcId ) );

   return( bTempSrcId );
}

UINT8 ABCC_GetOpmode( void )
{
#if( ABCC_CFG_OP_MODE_GETTABLE )
   return( ABCC_SYS_GetOpmode() );
#elif ( defined( ABCC_CFG_ABCC_OP_MODE_30 ) &&                                \
        defined( ABCC_CFG_ABCC_OP_MODE_40 ) )
   UINT8 bModuleId;

   bModuleId = ABCC_ReadModuleId();

   if( bModuleId == ABP_MODULE_ID_ACTIVE_ABCC30 )
   {
      return( ABCC_CFG_ABCC_OP_MODE_30 );
   }
   else
   {
      return( ABCC_CFG_ABCC_OP_MODE_40 );
   }
#elif defined( ABCC_CFG_ABCC_OP_MODE_30 )
   return( ABCC_CFG_ABCC_OP_MODE_30 );
#elif defined( ABCC_CFG_ABCC_OP_MODE_40 )
   return( ABCC_CFG_ABCC_OP_MODE_40 );
#else
   /*
   ** The user has not configured any way to determine the operating mode
   */
   #error "No method to determine the operating mode is available. Either set ABCC_CFG_OP_MODE_GETTABLE to TRUE or any of ABCC_CFG_ABCC_OP_MODE_X. See descriptions in abcc_cfg.h for details."
#endif /* End of #if defined( ABCC_CFG_OP_MODE_HW_CONF ) */
}


void ABCC_GetAttribute( ABP_MsgType* psMsg,
                        UINT8 bObject,
                        UINT16 iInstance,
                        UINT8 bAttribute,
                        UINT8 bSourceId )
{
   ABCC_MsgType sMsg;
   sMsg.psMsg = psMsg;

   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bSourceId ); /* SourceId */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bObject );  /* bObject */
   psMsg->sHeader.iInstance = iTOiLe( iInstance );                          /* Instance */
   ABCC_SetLowAddrOct(  sMsg.psMsg16->sHeader.iCmdReserved,
                  ABP_MSG_HEADER_C_BIT | ABP_CMD_GET_ATTR );                /* Command */

   sMsg.psMsg16->sHeader.iDataSize = 0;                                     /* Data size           */
   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, bAttribute ); /* CmdExt0 (Attribute) */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, 0 );         /* CmdExt1 (reserved)  */
}

void ABCC_SetByteAttribute(ABP_MsgType* psMsg,
                           UINT8 bObject,
                           UINT16 iInstance,
                           UINT8 bAttribute,
                           UINT8 bVal,
                           UINT8 bSourceId )
{
   ABCC_MsgType sMsg;
   sMsg.psMsg = psMsg;

   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bSourceId );  /* SourceId */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bObject );   /* bObject */
   psMsg->sHeader.iInstance = iTOiLe( iInstance );                           /* Instance */
   ABCC_SetLowAddrOct(  sMsg.psMsg16->sHeader.iCmdReserved,
                  ABP_MSG_HEADER_C_BIT | ABP_CMD_SET_ATTR );                 /* Command */

   sMsg.psMsg16->sHeader.iDataSize = iTOiLe( 1 );                            /* Data size           */
   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, bAttribute );  /* CmdExt0 (Attribute) */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, 0 );          /* CmdExt1 (reserved)  */
   ABCC_SetLowAddrOct( sMsg.psMsg16->aiData[ 0 ], bVal );                    /* Data                */
}

void ABCC_SetMsgHeader( ABP_MsgType* psMsg,
                        UINT8 bObject,
                        UINT16 iInstance,
                        UINT8 bAttribute,
                        ABP_MsgCmdType eService,
                        UINT16 iDataSize,
                        UINT8 bSourceId )
{
   ABCC_MsgType sMsg;
   sMsg.psMsg = psMsg;

   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bSourceId );  /* SourceId */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iSourceIdDestObj, bObject );   /* bObject */
   psMsg->sHeader.iInstance = iTOiLe( iInstance );                           /* Instance */
   ABCC_SetLowAddrOct(  sMsg.psMsg16->sHeader.iCmdReserved,
                        ABP_MSG_HEADER_C_BIT | eService );                   /* Command */

   sMsg.psMsg16->sHeader.iDataSize = iTOiLe( iDataSize );                    /* Data size           */
   ABCC_SetLowAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, bAttribute );  /* CmdExt0 (Attribute) */
   ABCC_SetHighAddrOct( sMsg.psMsg16->sHeader.iCmdExt0CmdExt1, 0 );          /* CmdExt1 (reserved)  */
}

ABCC_ErrorCodeType ABCC_VerifyMessage( const ABP_MsgType* psMsg )
{
   const ABP_MsgType16* psMsg16 = (const ABP_MsgType16* )psMsg;
   if( ABCC_GetLowAddrOct( psMsg16->sHeader.iCmdReserved ) & ABP_MSG_HEADER_E_BIT )
   {
      return( ABCC_EC_RESP_MSG_E_BIT_SET );
   }
   return( ABCC_EC_NO_ERROR );
}

/*------------------------------------------------------------------------------
** ABCC_GetDataTypeSizeInBits()
**------------------------------------------------------------------------------
*/
UINT16 ABCC_GetDataTypeSizeInBits( UINT8 bDataType )
{
   UINT16 iSetBitSize;

   if( ABP_Is_PADx( bDataType ) )
   {
      iSetBitSize = bDataType - ABP_PAD0;
   }
   else if ( ABP_Is_BITx( bDataType ) )
   {
      iSetBitSize = ( ( bDataType - ABP_BIT1 ) + 1 );
   }
   else
   {
      iSetBitSize = (UINT16)ABCC_GetDataTypeSize( bDataType );
      iSetBitSize *= 8;
   }

   return( iSetBitSize );
}



/*------------------------------------------------------------------------------
** ABCC_GetDataTypeSize()
**------------------------------------------------------------------------------
*/

UINT8 ABCC_GetDataTypeSize( UINT8 bDataType )
{
   UINT8 bSize;
   switch( bDataType )
   {

   case ABP_UINT8:
   case ABP_BOOL:
   case ABP_SINT8:
   case ABP_ENUM:
   case ABP_BITS8:
   case ABP_CHAR:
   case ABP_OCTET:
      bSize = ABP_UINT8_SIZEOF;
      break;

   case ABP_UINT16:
   case ABP_BITS16:
   case ABP_SINT16:
      bSize = ABP_UINT16_SIZEOF;
      break;
   case ABP_UINT32:
   case ABP_SINT32:
   case ABP_BITS32:
   case ABP_FLOAT:
      bSize = ABP_UINT32_SIZEOF;
      break;

   case ABP_SINT64:
   case ABP_UINT64:
      bSize = ABP_UINT64_SIZEOF;
      break;

   case ABP_BIT1:
   case ABP_BIT2:
   case ABP_BIT3:
   case ABP_BIT4:
   case ABP_BIT5:
   case ABP_BIT6:
   case ABP_BIT7:
      bSize = ABP_UINT8_SIZEOF;
      break;


   case ABP_PAD1:
   case ABP_PAD2:
   case ABP_PAD3:
   case ABP_PAD4:
   case ABP_PAD5:
   case ABP_PAD6:
   case ABP_PAD7:
   case ABP_PAD8:
      bSize = ABP_UINT8_SIZEOF;
      break;

   case ABP_PAD9:
   case ABP_PAD10:
   case ABP_PAD11:
   case ABP_PAD12:
   case ABP_PAD13:
   case ABP_PAD14:
   case ABP_PAD15:
   case ABP_PAD16:
      bSize = ABP_UINT16_SIZEOF;
      break;
   default:
      bSize = 0;
      break;
   }

   return( bSize );
}

void ABCC_GetMsgString( ABP_MsgType* psMsg, char* pcString, UINT16 iNumChar, UINT16 iOctetOffset )
{
   ABCC_PORT_StrCpyToNative( pcString,
                             ABCC_GetMsgDataPtr( psMsg ),
                             iOctetOffset,
                             iNumChar );
}

void ABCC_SetMsgString( ABP_MsgType* psMsg, const char* pcString, UINT16 iNumChar, UINT16 iOctetOffset )
{
   ABCC_PORT_StrCpyToPacked( ABCC_GetMsgDataPtr( psMsg ),
                             iOctetOffset,
                             pcString,
                             iNumChar );
}

void ABCC_GetMsgData8( ABP_MsgType* psMsg, UINT8* pbData, UINT16 iOctetOffset )
{
#ifdef ABCC_SYS_16_BIT_CHAR
   *pbData = 0;
#endif
   ABCC_PORT_Copy8( pbData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
}

void ABCC_SetMsgData8( ABP_MsgType* psMsg, UINT8 bData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy8( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &bData, 0 );
}

void ABCC_GetMsgData16( ABP_MsgType* psMsg, UINT16* piData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy16( piData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *piData = iLeTOi( *piData );
}

void ABCC_SetMsgData16( ABP_MsgType* psMsg, UINT16 iData, UINT16 iOctetOffset )
{
   iData = iTOiLe( iData );
   ABCC_PORT_Copy16( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &iData, 0 );
}

void ABCC_GetMsgData32( ABP_MsgType* psMsg, UINT32* plData, UINT16 iOctetOffset )
{
   ABCC_PORT_Copy32( plData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *plData = lLeTOl( *plData );
}

void ABCC_SetMsgData32( ABP_MsgType* psMsg, UINT32 lData, UINT16 iOctetOffset )
{
   lData = lTOlLe( lData );
   ABCC_PORT_Copy32( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &lData, 0 );
}

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
void ABCC_GetMsgData64( ABP_MsgType* psMsg, UINT64* plData, UINT16 iOctetOffset );
{
   ABCC_PORT_Copy64( plData, 0, ABCC_GetMsgDataPtr( psMsg ), iOctetOffset );
   *plData = lLeTOl64( *plData );
}

void ABCC_SetMsgData64( ABP_MsgType* psMsg, UINT64 lData, UINT16 iOctetOffset );
{
   lData = lTOlLe64( lData );
   ABCC_PORT_Copy64( ABCC_GetMsgDataPtr( psMsg ), iOctetOffset, &lData, 0 );
}
#endif
