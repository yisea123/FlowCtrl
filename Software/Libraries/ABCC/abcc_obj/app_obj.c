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
** Source file for the APP object, containing the Application Object and
** Application Instance.
********************************************************************************
********************************************************************************
*/
#include "abcc_td.h"
#include "abcc.h"
#include "abcc_obj_cfg.h"
#include "app_obj.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if APP_OBJ_ENABLE

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define APP_OA_NAME_VALUE                          "Application"
#define APP_OA_REV_VALUE                           2
#define APP_OA_NUM_INST_VALUE                      1
#define APP_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing an Application Object.
**------------------------------------------------------------------------------
*/
typedef struct app_Object
{
  const char* pcName;
  UINT8  bRevision;
  UINT16 iNumberOfInstances;
  UINT16 iHighestInstanceNo;
}
app_ObjectType;

/*------------------------------------------------------------------------------
** Structure describing an Application Data Instance.
**------------------------------------------------------------------------------
*/
#if APP_IA_CONFIGURED_ENABLE ||   \
    APP_IA_SUP_LANG_ENABLE ||     \
    APP_IA_PAR_CRTL_SUM_ENABLE || \
    APP_IA_HW_CONF_ADDR_ENABLE
typedef struct app_Instance
{
#if APP_IA_CONFIGURED_ENABLE
   BOOL   fConfigured;
#endif
#if APP_IA_SUP_LANG_ENABLE
   UINT8  abSupportedLanguages[ APP_IA_SUP_LANG_ARRAY_SIZE ];
#endif
#if APP_IA_PAR_CRTL_SUM_ENABLE
   UINT8  abParameterControlSum[ 16 ];
#endif
#if APP_IA_HW_CONF_ADDR_ENABLE
   BOOL   fHardwareConfigurableAddress;
#endif
}
app_InstanceType;
#endif

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

#if APP_IA_SUP_LANG_ENABLE
static const char* app_aacLanguages[ 5 ] =
{
   "English",
   "Deutsch",
   "Español",
   "Italiano",
   "Français"
};
#endif

static const app_ObjectType app_sObject =
{
   APP_OA_NAME_VALUE,                           /* Name.                                              */
   APP_OA_REV_VALUE,                            /* Revision.                                          */
   APP_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   APP_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

#if APP_IA_CONFIGURED_ENABLE ||   \
    APP_IA_SUP_LANG_ENABLE ||     \
    APP_IA_PAR_CRTL_SUM_ENABLE || \
    APP_IA_HW_CONF_ADDR_ENABLE
static app_InstanceType app_sInstance =
{
#if APP_IA_CONFIGURED_ENABLE
   APP_IA_CONFIGURED_VALUE,                     /* Configured.                                        */
#endif
#if APP_IA_SUP_LANG_ENABLE
   APP_IA_SUP_LANG_VALUE,                       /* Supported languages.                               */
#endif
#if APP_IA_PAR_CRTL_SUM_ENABLE
   APP_IA_PAR_CRTL_SUM_VALUE,                   /* Parameter control sum                              */
#endif
#if APP_IA_HW_CONF_ADDR_ENABLE
   APP_IA_HW_CONF_ADDR_VALUE,                   /* Hardware configurable address                      */
#endif
};
#endif

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Called to check if the requested reset is permitted by the application.
**------------------------------------------------------------------------------
** Arguments:
**    bResetType           - Type of reset, see ABP_RESET_XXX defines.
**
** Returns:
**    BOOL                 - TRUE: Reset request is allowed.
**                           FALSE: Reset request NOT allowed.
**------------------------------------------------------------------------------
*/
BOOL IsResetRequestAllowed( UINT8 bResetType )
{
   switch( (ABP_ResetType)bResetType )
   {
   case ABP_RESET_POWER_ON:
   case ABP_RESET_FACTORY_DEFAULT:
   case ABP_RESET_POWER_ON_FACTORY_DEFAULT:

      return( TRUE );

   default:

      return( FALSE );
   }
}

/*------------------------------------------------------------------------------
** This function will restore NVS parameters to their default values
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void RestoreToDefault( void )
{
   /*
   ** Todo: PORTING ALERT!
   ** Restore parameters stored in NVS to their default values
   */
}

/*------------------------------------------------------------------------------
** Function to set whether firmware is available or not in the candidate area.
** This function sets the value to a NVS.
**------------------------------------------------------------------------------
** Arguments:
**    fFirmwareAvailable    - TRUE/FALSE
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void SetCandidateFirmwareAvailable( BOOL fFirmwareAvailable )
{
   /*
   ** Todo: PORTING ALERT!
   ** Store value to NVS.
   */
   (void)fFirmwareAvailable;
}

/*------------------------------------------------------------------------------
** The function that processes the commands to the Application Instance.
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
   BOOL8  fFirmwareAvailable;
   UINT16 iIndex;

   fFirmwareAvailable = 0;
   iIndex = 0;

   (void)(fFirmwareAvailable);
   (void)(iIndex);

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
#if APP_IA_CONFIGURED_ENABLE
      case ABP_APP_IA_CONFIGURED:

         /*
         ** The 'configured' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, app_sInstance.fConfigured, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_APP_IA_CONFIGURED_DS );
         break;
#endif
#if APP_IA_SUP_LANG_ENABLE
      case ABP_APP_IA_SUP_LANG:

         /*
         ** The 'supported languages' attribute is requested.
         ** Copy the attribute to a response message.
         */
         for( iIndex = 0; iIndex < APP_IA_SUP_LANG_ARRAY_SIZE; iIndex++ )
         {
            ABCC_SetMsgData8( psNewMessage, app_sInstance.abSupportedLanguages[ iIndex ], iIndex );
         }
         ABP_SetMsgResponse( psNewMessage, APP_IA_SUP_LANG_ARRAY_SIZE );
         break;
#endif
#if APP_IA_SER_NUM_ENABLE
      case ABP_APP_IA_SER_NUM:

         /*
         ** The 'serial number' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData32( psNewMessage, APP_IA_SER_NUM_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_APP_IA_SER_NUM_DS );
         break;
#endif
#if APP_IA_PAR_CRTL_SUM_ENABLE
      case ABP_APP_IA_PAR_CRTL_SUM:

         /*
         ** The 'parameter control sum' attribute is requested.
         ** Copy the attribute to a response message.
         */
         for( iIndex = 0; iIndex < 16; iIndex++ )
         {
            ABCC_SetMsgData8( psNewMessage, app_sInstance.abParameterControlSum[ iIndex ], iIndex );
         }
         ABP_SetMsgResponse( psNewMessage, ABP_APP_IA_PAR_CRTL_SUM_DS );
         break;
#endif
#if APP_IA_FW_AVAILABLE_ENABLE
      case ABP_APP_IA_FW_AVAILABLE:

         /*
         ** The 'candidate firmware available' attribute is requested.
         ** Copy the attribute to a response message.
         */
         fFirmwareAvailable = APP_GetCandidateFwAvailable();
         ABCC_SetMsgData8( psNewMessage, fFirmwareAvailable, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_APP_IA_FW_AVAILABLE_DS );
         break;
#endif
#if APP_IA_HW_CONF_ADDR_ENABLE
      case ABP_APP_IA_HW_CONF_ADDR:

         /*
         ** The 'hardware configurable address' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, app_sInstance.fHardwareConfigurableAddress, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_APP_IA_HW_CONF_ADDR_DS );
         break;
#endif
      default:

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
   }
#if APP_IA_FW_AVAILABLE_ENABLE
   case ABP_CMD_SET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
      case ABP_APP_IA_FW_AVAILABLE:

         /*
         ** Set the 'candidate firmware available' attribute
         */
          ABCC_GetMsgData8( psNewMessage, &fFirmwareAvailable, 0 );
         SetCandidateFirmwareAvailable( fFirmwareAvailable );
         ABP_SetMsgResponse( psNewMessage, 0 );
         break;

      default:

         /*
         ** The attribute does not exist, or the attribute is not settable.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
   }
#endif
#if APP_IA_SUP_LANG_ENABLE
   case ABP_CMD_GET_ENUM_STR:
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
      case ABP_APP_IA_SUP_LANG:
      {
         BOOL8  fLanguageSupported = FALSE;
         UINT16 iStrLength;

         for( iIndex = 0; iIndex < APP_IA_SUP_LANG_ARRAY_SIZE; iIndex++ )
         {
            if( app_sInstance.abSupportedLanguages[ iIndex ] == ABCC_GetMsgCmdExt1( psNewMessage ) )
            {
               /*
               ** Copy the ENUM STR to a message.
               */
               fLanguageSupported = TRUE;
               iStrLength = (UINT16)strlen( app_aacLanguages[ ABCC_GetMsgCmdExt1( psNewMessage ) ] );
               ABCC_SetMsgString( psNewMessage,
                                  app_aacLanguages[ ABCC_GetMsgCmdExt1( psNewMessage ) ],
                                  iStrLength,
                                  0 );
               ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
            }
         }

         if( !fLanguageSupported )
         {
            /*
            ** The ENUM value was out of range.
            */
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_1 );
         }
         break;
      }
      default:

         /*
         ** The attribute does not exist, or the attribute is not an ENUM.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
#endif

   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End switch( command number ) */
}

/*------------------------------------------------------------------------------
** The function that processes the commands to the Application Object
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
         iStrLength = (UINT16)strlen( app_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, app_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:

         /*
         ** The 'revision' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData8( psNewMessage, app_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** The 'Number of Instances' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, app_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** The 'Highest Instance Number' attribute is requested.
         ** Copy the attribute to a response message.
         */
         ABCC_SetMsgData16( psNewMessage, app_sObject.iHighestInstanceNo, 0 );
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
   case ABP_APP_CMD_RESET_REQUEST:
   {
      /*
      ** Request a command reset.
      */
      if( !IsResetRequestAllowed( ABCC_GetMsgCmdExt1( psNewMessage ) ) )
      {
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_STATE );
      }
      else
      {
         ABP_SetMsgResponse( psNewMessage, 0 );
      }
      break;
   }
   case ABP_CMD_RESET:
   {
      /*
      ** Perform a reset.
      */
      APP_ProcResetRequest( ABCC_GetMsgCmdExt1( psNewMessage ) );
      ABP_SetMsgResponse( psNewMessage, 0 );
      break;
   }
   case ABP_APP_CMD_CHANGE_LANG_REQUEST:

      /*
      ** Optionally implement the Change Language Request service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_APP_CMD_RESET_DIAGNOSTIC:

      /*
      ** Optionally implement the Reset Diagnostic service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

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

void APP_HwConfAddress( BOOL fhwConfAddress )
{
#if APP_IA_HW_CONF_ADDR_ENABLE
   app_sInstance.fHardwareConfigurableAddress = fhwConfAddress;
#endif
}

BOOL APP_GetCandidateFwAvailable( void )
{
   /*
   ** Todo: PORTING ALERT!
   ** Read value from NVS.
   */
   return( FALSE );
}

void APP_ProcResetRequest( UINT8 bResetType )
{
   switch( bResetType )
   {
   case ABP_RESET_FACTORY_DEFAULT:
      RestoreToDefault();
      break;

   case ABP_RESET_POWER_ON_FACTORY_DEFAULT:
      RestoreToDefault();
      APPL_Reset();
      break;

   case ABP_RESET_POWER_ON:
      APPL_Reset();
      break;

   default:
      break;
   }

}

void APP_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Process the Application data Object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Process the Application data Instance Command.
      */
      InstanceCommand( psNewMessage );
   }

   ABCC_SendRespMsg( psNewMessage );

}

#endif /* APP_OBJ_ENABLE */
