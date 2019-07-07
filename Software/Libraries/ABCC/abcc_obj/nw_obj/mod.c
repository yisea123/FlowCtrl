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
** Source file for the Modbus Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_obj_cfg.h"
#include "mod.h"
#include "abp.h"
#include "abp_mod.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if MOD_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define MOD_OA_NAME_VALUE                          "Modbus"
#define MOD_OA_REV_VALUE                           1
#define MOD_OA_NUM_INST_VALUE                      1
#define MOD_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing an Modbus Object.
**------------------------------------------------------------------------------
*/
typedef struct mod_Object
{
   const  char* pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
mod_ObjectType;

/*------------------------------------------------------------------------------
** Forward declarations
**------------------------------------------------------------------------------
*/
static void InstanceCommand( ABP_MsgType* psNewMessage );
static void ObjectCommand( ABP_MsgType* psNewMessage );

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static const mod_ObjectType mod_sObject =
{
   MOD_OA_NAME_VALUE,                           /* Name.                                              */
   MOD_OA_REV_VALUE,                            /* Revision.                                          */
   MOD_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   MOD_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

/*******************************************************************************
** Public Services
********************************************************************************
*/

void MOD_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the Modbus Object and its Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Modbus object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Modbus instance Command.
      */
      InstanceCommand( psNewMessage );
   }

   ABCC_SendRespMsg( psNewMessage );
}

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Processes commands to Modbus Instances
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
   UINT16 iIndex;
   UINT16 iStrLength;

   (void)iIndex;
   (void)iStrLength;

   /*
   ** This function processes commands to the Modbus Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) != 1 )
   {
      /*
      ** The Instance does not exist.
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
#if MOD_IA_VENDOR_NAME_ENABLE
      case ABP_MOD_IA_VENDOR_NAME:

         iStrLength = (UINT16)strlen( MOD_IA_VENDOR_NAME_VALUE );

         /*
         ** Copy the 1st Instance 1 attribute (Vendor name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_VENDOR_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_PRODUCT_CODE_ENABLE
      case ABP_MOD_IA_PRODUCT_CODE:

         iStrLength = (UINT16)strlen( MOD_IA_PRODUCT_CODE_VALUE );

         /*
         ** Copy the 2nd Instance 1 attribute (Product code) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_PRODUCT_CODE_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_REVISION_ENABLE
      case ABP_MOD_IA_REVISION:

         iStrLength = (UINT16)strlen( MOD_IA_REVISION_VALUE );

         /*
         ** Copy the 3rd Instance 1 attribute (Revision) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_REVISION_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_VENDOR_URL_ENABLE
      case ABP_MOD_IA_VENDOR_URL:

         iStrLength = (UINT16)strlen( MOD_IA_VENDOR_URL_VALUE );

         /*
         ** Copy the 4th Instance 1 attribute (Vendor URL) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_VENDOR_URL_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_PRODUCT_NAME_ENABLE
      case ABP_MOD_IA_PRODUCT_NAME:

         iStrLength = (UINT16)strlen( MOD_IA_PRODUCT_NAME_VALUE );

         /*
         ** Copy the 5th Instance 1 attribute (Product name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_PRODUCT_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_MODEL_NAME_ENABLE
      case ABP_MOD_IA_MODEL_NAME:

         iStrLength = (UINT16)strlen( MOD_IA_MODEL_NAME_VALUE );

         /*
         ** Copy the 6th Instance 1 attribute (Model name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_MODEL_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_USER_APP_NAME_ENABLE
      case ABP_MOD_IA_USER_APP_NAME:

         iStrLength = (UINT16)strlen( MOD_IA_USER_APP_NAME_VALUE );

         /*
         ** Copy the 7th Instance 1 attribute (User application name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, MOD_IA_USER_APP_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
#endif
#if MOD_IA_DEVICE_ID_ENABLE
      case ABP_MOD_IA_DEVICE_ID:

         /*
         ** Copy the 8th Instance 1 attribute (Device ID) to the message.
         */
         for( iIndex = 0; iIndex < MOD_IA_DEVICE_ID_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData8( psNewMessage, MOD_IA_DEVICE_ID_VALUE[ iIndex ], iIndex );
         }

         ABP_SetMsgResponse( psNewMessage, MOD_IA_DEVICE_ID_ARRAY_SIZE );
         break;
#endif
#if MOD_IA_ADI_INDEXING_BITS_ENABLE
      case ABP_MOD_IA_ADI_INDEXING_BITS:
         /*
         ** Copy the 9th Instance 1 attribute (No. of ADI indexing bits) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, MOD_IA_ADI_INDEXING_BITS_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_MOD_IA_ADI_INDEXING_BITS_DS );
         break;
#endif
#if MOD_IA_MESSAGE_FORWARDING_ENABLE
      case ABP_MOD_IA_MESSAGE_FORWARDING:
         /*
         ** Copy the 10th Instance 1 attribute (Enable Modbus Message forwarding) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, MOD_IA_MESSAGE_FORWARDING_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_MOD_IA_MESSAGE_FORWARDING_DS );
         break;
#endif
#if MOD_IA_RW_OFFSET_ENABLE
      case ABP_MOD_IA_RW_OFFSET:
         /*
         ** Copy the 11th Instance 1 attribute (Modbus read/write registers command offset) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, MOD_IA_RW_OFFSET_READ_VALUE, 0 );
         ABCC_SetMsgData16( psNewMessage, MOD_IA_RW_OFFSET_WRITE_VALUE, 2 );
         ABP_SetMsgResponse( psNewMessage, ABP_MOD_IA_RW_OFFSET_DS );
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
   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch( Command number ) */
}


/*------------------------------------------------------------------------------
** Processes commands to the Modbus Object (Instance 0)
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
   /*
   ** This function processes commands to the Modbus Object (Instance 0).
   */
   switch ( ABCC_GetMsgCmdBits( psNewMessage ) )
   {
   case ABP_CMD_GET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
      case ABP_OA_NAME:
      {
         UINT16 iStrLength;

         /*
         ** Copy the attribute to a message.
         */
         iStrLength = (UINT16)strlen( mod_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, mod_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }

      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData8( psNewMessage, mod_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, mod_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, mod_sObject.iHighestInstanceNo, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_HIGHEST_INST_DS );
         break;

      default:

         /*
         ** Unsupported attribute.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;

      } /* End of switch( Attribute number ) */

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

#endif /* MOD_OBJ_ENABLE */
