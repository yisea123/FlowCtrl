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
** Source file for the Functional Safety object.
********************************************************************************
********************************************************************************
*/
#include "abcc_td.h"
#include "abcc.h"
#include "abcc_obj_cfg.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"
#include "abp_safe.h"

#if SAFE_OBJ_ENABLE

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define SAFE_OA_NAME_VALUE                          "Functional Safety"
#define SAFE_OA_REV_VALUE                           1
#define SAFE_OA_NUM_INST_VALUE                      1
#define SAFE_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing a Functional Safety object.
**------------------------------------------------------------------------------
*/
typedef struct safe_Object
{
  const char* pcName;
  UINT8  bRevision;
  UINT16 iNumberOfInstances;
  UINT16 iHighestInstanceNo;
}
safe_ObjectType;

/*------------------------------------------------------------------------------
** Structure describing an Functional Safety instance.
**------------------------------------------------------------------------------
*/
#if SAFE_IA_SAFETY_ENABLED_ENABLE ||                                           \
    SAFE_IA_BAUD_RATE_ENABLE ||                                                \
    SAFE_IA_IO_CONFIGURATION_ENABLE ||                                         \
    SAFE_IA_CYCLE_TIME_ENABLE
typedef struct safe_Instance
{
#if SAFE_IA_SAFETY_ENABLED_ENABLE
   BOOL    fSafetyEnabled;
#endif
#if SAFE_IA_BAUD_RATE_ENABLE
   UINT32  lBaudRate;
#endif
#if SAFE_IA_IO_CONFIGURATION_ENABLE
   UINT8   abIOConfiguration[ SAFE_IA_IO_CONFIGURATION_ARRAY_SIZE ];
#endif
#if SAFE_IA_CYCLE_TIME_ENABLE
   UINT8   bCycleTime;
#endif
}
safe_InstanceType;
#endif

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static const safe_ObjectType safe_sObject =
{
   SAFE_OA_NAME_VALUE,                           /* Name.                                              */
   SAFE_OA_REV_VALUE,                            /* Revision.                                          */
   SAFE_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   SAFE_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

#if SAFE_IA_SAFETY_ENABLED_ENABLE ||                                           \
    SAFE_IA_BAUD_RATE_ENABLE ||                                                \
    SAFE_IA_IO_CONFIGURATION_ENABLE ||                                         \
    SAFE_IA_CYCLE_TIME_ENABLE
static safe_InstanceType safe_sInstance =
{
#if SAFE_IA_SAFETY_ENABLED_ENABLE
   SAFE_IA_SAFETY_ENABLED_VALUE,                 /* Safety enabled.                                    */
#endif
#if SAFE_IA_BAUD_RATE_ENABLE
   SAFE_IA_BAUD_RATE_VALUE,                      /* Baud rate.                                         */
#endif
#if SAFE_IA_IO_CONFIGURATION_ENABLE
   SAFE_IA_IO_CONFIGURATION_VALUE,               /* IO Configuration                                   */
#endif
#if SAFE_IA_CYCLE_TIME_ENABLE
   SAFE_IA_CYCLE_TIME_VALUE                      /* Cycle time                                         */
#endif
};
#endif

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** The function that processes the commands to the Functional Safety Instance.
**------------------------------------------------------------------------------
** Arguments:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void InstanceCommand( ABP_MsgType* psNewMessage )
{
#if SAFE_IA_IO_CONFIGURATION_ENABLE
   UINT16 iIndex;
#endif

   if( ABCC_GetMsgInstance( psNewMessage ) != 1 )
   {
      /*
      ** The requested instance does not exist.
      ** Respond with a error.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_INST );

      return;
   }

   switch ( ABCC_GetMsgCmdBits( psNewMessage ) )
   {
   case ABP_CMD_GET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
#if SAFE_IA_SAFETY_ENABLED_ENABLE
      case ABP_SAFE_IA_SAFETY_ENABLED:

         /*
         ** The 'safety enable' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, safe_sInstance.fSafetyEnabled, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_SAFE_IA_SAFETY_ENABLED_DS );
         break;
#endif
#if SAFE_IA_BAUD_RATE_ENABLE
      case ABP_SAFE_IA_BAUD_RATE:

         /*
         ** The 'baud rate' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData32( psNewMessage, safe_sInstance.lBaudRate, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_SAFE_IA_BAUD_RATE_DS );
         break;
#endif
#if SAFE_IA_IO_CONFIGURATION_ENABLE
      case ABP_SAFE_IA_IO_CONFIG:

         /*
         ** The 'io configuration' attribute is requested.
         ** Copy the attribute to a response message.
         */
         for( iIndex = 0; iIndex < SAFE_IA_IO_CONFIGURATION_ARRAY_SIZE; iIndex++ )
         {
            ABCC_SetMsgData8( psNewMessage, safe_sInstance.abIOConfiguration[ iIndex ], iIndex );
         }
         ABP_SetMsgResponse( psNewMessage, SAFE_IA_IO_CONFIGURATION_ARRAY_SIZE );
         break;
#endif
#if SAFE_IA_CYCLE_TIME_ENABLE
      case ABP_SAFE_IA_CYCLE_TIME:

         /*
         ** The 'parameter control sum' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, safe_sInstance.bCycleTime, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_SAFE_IA_CYCLE_TIME_DS );
         break;
#endif
      default:

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

   } /* End switch( command number ) */
}

/*------------------------------------------------------------------------------
** The function that processes the commands to the Functional Safety Object
** (instance 0).
**------------------------------------------------------------------------------
** Arguments:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;

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
         iStrLength = (UINT16)strlen( safe_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, safe_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:

         /*
         ** The 'revision' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, safe_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** The 'Number of Instances' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, safe_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** The 'Highest Instance Number' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, safe_sObject.iHighestInstanceNo, 0 );
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
      ** Respond with an error.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End switch( command number ) */

}

/*******************************************************************************
** Public Services
********************************************************************************
*/

void SAFE_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Process the Functional Safety Object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Process the Functional Safety Instance Command.
      */
      InstanceCommand( psNewMessage );
   }

   ABCC_SendRespMsg( psNewMessage );

}

void SAFE_SetSafetyEnable( BOOL fEnabled )
{
   safe_sInstance.fSafetyEnabled = fEnabled;
}

#endif /* SAFE_OBJ_ENABLE */
