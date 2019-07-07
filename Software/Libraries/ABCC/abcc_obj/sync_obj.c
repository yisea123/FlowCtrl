/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.02.02 (2016-11-10)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**    ABCC Driver    5.02.01 (2016-11-02)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
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
** Implementation of the SYNC object.
********************************************************************************
********************************************************************************
*/
#include "abcc_td.h"
#include "abcc_sw_port.h"
#include "abcc_drv_cfg.h"
#include "abcc_obj_cfg.h"
#include "abp.h"
#include "abp_sync.h"
#include "abcc_ad_if.h"
#include "abcc.h"
#include "abcc_port.h"
#include "appl_abcc_handler.h"
#include "abcc_sys_adapt.h"
#include "sync_obj.h"

#if SYNC_OBJ_ENABLE

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define SYNC_OA_NAME_VALUE                          "Sync"
#define SYNC_OA_REV_VALUE                           1
#define SYNC_OA_NUM_INST_VALUE                      1
#define SYNC_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing a SYNC Object.
**------------------------------------------------------------------------------
*/
typedef struct sync_Object
{
   const  char* pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
sync_ObjectType;

/*------------------------------------------------------------------------------
** Structure describing an SYNC Instance.
**------------------------------------------------------------------------------
*/
typedef struct sync_Instance
{
#if SYNC_IA_CYCLE_TIME_ENABLE
   UINT32 lCycleTime;
#endif
#if SYNC_IA_OUTPUT_VALID_ENABLE
   UINT32 lOutputValidTime;
#endif
#if SYNC_IA_INPUT_CAPTURE_ENABLE
   UINT32 lInputCaptureTime;
#endif
#if SYNC_IA_OUTPUT_PROCESSING_ENABLE
   UINT32 lOutputProcessingTime;
#endif
#if SYNC_IA_INPUT_PROCESSING_ENABLE
   UINT32 lInputProcessingTime;
#endif
#if SYNC_IA_MIN_CYCLE_TIME_ENABLE
   UINT32 lMinCycleTime;
#endif
#if SYNC_IA_SYNC_MODE_ENABLE
   UINT16 iSyncMode;
#endif
}
sync_InstanceType;

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static const sync_ObjectType sync_sObject =
{
   SYNC_OA_NAME_VALUE,                           /* Name.                     */
   SYNC_OA_REV_VALUE,                            /* Revision.                 */
   SYNC_OA_NUM_INST_VALUE,                       /* Number of instances.      */
   SYNC_OA_HIGHEST_INST_VALUE                    /* Highest instance number.  */
};

static sync_InstanceType sync_sInstance =
{
#if SYNC_IA_CYCLE_TIME_ENABLE
   SYNC_IA_CYCLE_TIME_VALUE,
#endif
#if SYNC_IA_OUTPUT_VALID_ENABLE
   SYNC_IA_OUTPUT_VALID_VALUE,
#endif
#if SYNC_IA_INPUT_CAPTURE_ENABLE
   SYNC_IA_INPUT_CAPTURE_VALUE,
#endif
#if SYNC_IA_OUTPUT_PROCESSING_ENABLE
   SYNC_IA_OUTPUT_PROCESSING_VALUE,
#endif
#if SYNC_IA_INPUT_PROCESSING_ENABLE
   SYNC_IA_INPUT_PROCESSING_VALUE,
#endif
#if SYNC_IA_MIN_CYCLE_TIME_ENABLE
   SYNC_IA_MIN_CYCLE_TIME_VALUE,
#endif
#if SYNC_IA_SYNC_MODE_ENABLE
   SYNC_IA_SYNC_MODE_VALUE,
#endif
};

/*------------------------------------------------------------------------------
** Forward declarations
**------------------------------------------------------------------------------
*/
static void UpdateAppStatus( void );

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Processes commands to the Sync Instance
**------------------------------------------------------------------------------
** Arguments:
**    psNewMessage - Pointer to a ABP_MsgType message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void InstanceCommand( ABP_MsgType* psNewMessage )
{
   UINT32 lCycleTimeNs;
   UINT16 iSyncMode;
   UINT8  bAnbState;

   ABCC_PORT_UseCritical();

   if( ABCC_GetMsgInstance( psNewMessage ) != 1 )
   {
      /*
      ** The requested instance does not exist.
      ** Respond with a error.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_INST );

      return;
   }

   switch( ABCC_GetMsgCmdBits( psNewMessage ) )
   {
   case ABP_CMD_GET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
#if SYNC_IA_CYCLE_TIME_ENABLE
      case ABP_SYNC_IA_CYCLE_TIME:

         /*
         ** The 'cycle time' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lCycleTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_CYCLE_TIME_DS );
         break;
#endif
#if SYNC_IA_OUTPUT_VALID_ENABLE
      case ABP_SYNC_IA_OUTPUT_VALID:

         /*
         ** The 'output valid' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lOutputValidTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_OUTPUT_VALID_DS );

         break;
#endif
#if SYNC_IA_INPUT_CAPTURE_ENABLE
      case ABP_SYNC_IA_INPUT_CAPTURE:

         /*
         ** The 'input capture' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lInputCaptureTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_INPUT_CAPTURE_DS );

         break;
#endif
#if SYNC_IA_OUTPUT_PROCESSING_ENABLE
      case ABP_SYNC_IA_OUTPUT_PROCESSING:

         /*
         ** The 'output processing' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lOutputProcessingTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_OUTPUT_PROCESSING_DS );

         break;
#endif
#if SYNC_IA_INPUT_PROCESSING_ENABLE
      case ABP_SYNC_IA_INPUT_PROCESSING:

         /*
         ** The 'input processing' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lInputProcessingTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_INPUT_PROCESSING_DS );

         break;
#endif
#if SYNC_IA_MIN_CYCLE_TIME_ENABLE
      case ABP_SYNC_IA_MIN_CYCLE_TIME:

         /*
         ** The 'min cycle time' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData32( psNewMessage, sync_sInstance.lMinCycleTime, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_MIN_CYCLE_TIME_DS );

         break;
#endif
#if SYNC_IA_SYNC_MODE_ENABLE
      case ABP_SYNC_IA_SYNC_MODE:

         /*
         ** The 'sync mode' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData16( psNewMessage, sync_sInstance.iSyncMode, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, (UINT8)ABP_SYNC_IA_SYNC_MODE_DS );

         break;
#endif
#if SYNC_IA_SUPPORTED_SYNC_MODES_ENABLE
      case ABP_SYNC_IA_SUPPORTED_SYNC_MODES:

         /*
         ** The 'supported sync modes' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_SetMsgData16( psNewMessage, SYNC_IA_SUPPORTED_SYNC_MODES_VALUE, 0 );
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, ABP_SYNC_IA_SUPPORTED_SYNC_MODES_DS );

         break;
#endif
      default:
         /*
         ** Unsupported attribute.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );

         break;
      }
      break;
   }
   case ABP_CMD_SET_ATTR:
   {
      bAnbState = ABCC_AnbState();

      /*
      ** It shall only be possible to set the attributes if the Anybus state
      ** is not equal to Idle or Process active.
      */
      if( ( bAnbState == ABP_ANB_STATE_IDLE ) ||
          ( bAnbState == ABP_ANB_STATE_PROCESS_ACTIVE ) )
      {
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_STATE );

         break;
      }

      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
#if SYNC_IA_CYCLE_TIME_ENABLE
      case ABP_SYNC_IA_CYCLE_TIME:

         ABCC_GetMsgData32( psNewMessage, &lCycleTimeNs, 0 );

         ABCC_PORT_EnterCritical();
         {
            if( lCycleTimeNs < sync_sInstance.lMinCycleTime )
            {
               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_VAL_TOO_LOW );
            }
            else
            {
               /*
               ** Set the 'cycle time' attribute
               */
               sync_sInstance.lCycleTime = lCycleTimeNs;
               ABP_SetMsgResponse( psNewMessage, 0 );
               UpdateAppStatus();
            }
         }
         ABCC_PORT_ExitCritical();

         break;
#endif
#if SYNC_IA_OUTPUT_VALID_ENABLE
      case ABP_SYNC_IA_OUTPUT_VALID:

         /*
         ** Set the 'output valid' attribute
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_GetMsgData32( psNewMessage, &sync_sInstance.lOutputValidTime, 0 );
            UpdateAppStatus();
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, 0 );

         break;
#endif
#if SYNC_IA_INPUT_CAPTURE_ENABLE
      case ABP_SYNC_IA_INPUT_CAPTURE:

         /*
         ** Set the 'input capture' attribute
         */
         ABCC_PORT_EnterCritical();
         {
            ABCC_GetMsgData32( psNewMessage, &sync_sInstance.lInputCaptureTime, 0 );
            UpdateAppStatus();
         }
         ABCC_PORT_ExitCritical();

         ABP_SetMsgResponse( psNewMessage, 0 );

         break;
#endif
#if SYNC_IA_SYNC_MODE_ENABLE
      case ABP_SYNC_IA_SYNC_MODE:

         ABCC_GetMsgData16( psNewMessage, &iSyncMode, 0 );

         if( (SYNC_SyncModeType)iSyncMode > SYNC_MODE_SYNCHRONOUS )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_VAL_TOO_HIGH );
         }
         else if( !( SYNC_IA_SUPPORTED_SYNC_MODES_VALUE & ( 1 << iSyncMode ) ) )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_OUT_OF_RANGE );
         }
         else
         {
            /*
            ** Set the 'sync mode' attribute
            */
            ABCC_PORT_EnterCritical();
            {
               sync_sInstance.iSyncMode = iSyncMode;
               UpdateAppStatus();
            }
            ABCC_PORT_ExitCritical();

            ABP_SetMsgResponse( psNewMessage, 0 );
         }

         break;
#endif
      default:

         /*
         ** The attribute does not exist, or the attribute is not settable.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
   }
   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
   }
}

/*------------------------------------------------------------------------------
** Processes commands to the SYNC Object (Instance 0)
**------------------------------------------------------------------------------
** Arguments:
**    psNewMessage - Pointer to a ABP_MsgType message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;

   /*
   ** This function processes commands to the Sync Object (Instance 0).
   */
   switch ( ABCC_GetMsgCmdBits( psNewMessage ) )
   {
   case ABP_CMD_GET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
      case ABP_OA_NAME:

         /*
         ** The 'name' attribute is requested.
         ** Copy the attribute to a response message.
         */
         iStrLength = (UINT16)strlen( sync_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, sync_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:

         /*
         ** The 'revision' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, sync_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** The 'Number of Instances' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, sync_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** The 'Highest Instance Number' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, sync_sObject.iHighestInstanceNo, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_HIGHEST_INST_DS );
         break;

      default:

         /*
         ** Unsupported attribute.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
   }
   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch( Command number ) */
}

/*------------------------------------------------------------------------------
** Validates the timing parameters set by the Anybus module and sets the
** correct value to the Application status register.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void UpdateAppStatus( void )
{
   if( (SYNC_SyncModeType)sync_sInstance.iSyncMode == SYNC_MODE_NONSYNCHRONOUS )
   {
      ABCC_SetAppStatus( ABP_APPSTAT_NO_ERROR );
   }
   else if( ( sync_sInstance.lCycleTime < sync_sInstance.lOutputValidTime ) ||
            ( sync_sInstance.lCycleTime < ( sync_sInstance.lInputCaptureTime +
                                            sync_sInstance.lInputProcessingTime) ) )
   {
      ABCC_SetAppStatus( ABP_APPSTAT_SYNC_CFG_ERR );
   }
   else
   {
      ABCC_SetAppStatus( ABP_APPSTAT_NOT_SYNCED );
   }
}

/*******************************************************************************
** Public Services
********************************************************************************
*/

UINT32 SYNC_GetCycleTime( void )
{
   UINT32 lCycleTimeNs;

   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      lCycleTimeNs = sync_sInstance.lCycleTime;
   }
   ABCC_PORT_ExitCritical();

   return( lCycleTimeNs );
}

UINT32 SYNC_GetInputCaptureTime( void )
{
   UINT32 lInputCaptureTimeNs;

   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      lInputCaptureTimeNs = sync_sInstance.lInputCaptureTime;
   }
   ABCC_PORT_ExitCritical();

   return( lInputCaptureTimeNs );
}

SYNC_SyncModeType SYNC_GetMode( void )
{
   return( (SYNC_SyncModeType)sync_sInstance.iSyncMode );
}

UINT32 SYNC_GetOutputValidTime( void )
{
   UINT32 lOutputValidTimeNs;

   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      lOutputValidTimeNs = sync_sInstance.lOutputValidTime;
   }
   ABCC_PORT_ExitCritical();

   return( lOutputValidTimeNs );
}

void SYNC_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Process the Sync object command
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Process the Sync instance command
      */
      InstanceCommand( psNewMessage );
   }

   ABCC_SendRespMsg( psNewMessage );
}

void SYNC_SetInputProcessingTime( UINT32 lInputProcTimeNs )
{
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      sync_sInstance.lInputProcessingTime = lInputProcTimeNs;
   }
   ABCC_PORT_ExitCritical();
}

void SYNC_SetMinCycleTime( UINT32 lMinCycleTimeNs )
{
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      sync_sInstance.lMinCycleTime = lMinCycleTimeNs;
   }
   ABCC_PORT_ExitCritical();
}

void SYNC_SetOutputProcessingTime( UINT32 lOutputProcTimeNs )
{
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   {
      sync_sInstance.lOutputProcessingTime = lOutputProcTimeNs;
   }
   ABCC_PORT_ExitCritical();
}

#endif /* SYNC_OBJ_ENABLE */
