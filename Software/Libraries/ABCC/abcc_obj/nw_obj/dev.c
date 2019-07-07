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
** Source file for the DeviceNet Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_obj_cfg.h"
#include "dev.h"
#include "abp.h"
#include "abp_dev.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if DEV_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define DEV_OA_NAME_VALUE                          "DeviceNet"
#define DEV_OA_REV_VALUE                           1
#define DEV_OA_NUM_INST_VALUE                      1
#define DEV_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing an DeviceNet Object.
**------------------------------------------------------------------------------
*/
typedef struct dev_Object
{
   const  char* pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
dev_ObjectType;

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

static const dev_ObjectType dev_sObject =
{
   DEV_OA_NAME_VALUE,                           /* Name.                                              */
   DEV_OA_REV_VALUE,                            /* Revision.                                          */
   DEV_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   DEV_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

/*******************************************************************************
** Public Services
********************************************************************************
*/

void DEV_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the DeviceNet Object and it's Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** DeviceNet object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** DeviceNet instance Command.
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
** Processes commands to DEV Instances
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
   /*
   ** This function processes commands to the DeviceNet Instance.
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
#if DEV_IA_VENDOR_ID_ENABLE
      case ABP_DEV_IA_VENDOR_ID:

         /*
         ** Copy the 1st Instance 1 attribute (Vendor ID) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_VENDOR_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_VENDOR_ID_DS );
         break;
#endif
#if DEV_IA_DEVICE_TYPE_ENABLE
      case ABP_DEV_IA_DEVICE_TYPE:

         /*
         ** Copy the 2nd Instance 1 attribute (Device type) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_DEVICE_TYPE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_DEVICE_TYPE_DS );
         break;
#endif
#if DEV_IA_PRODUCT_CODE_ENABLE
      case ABP_DEV_IA_PRODUCT_CODE:

         /*
         ** Copy the 3rd Instance 1 attribute (Product code) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_PRODUCT_CODE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_PRODUCT_CODE_DS );
         break;
#endif
#if DEV_IA_REVISION_ENABLE
      case ABP_DEV_IA_REVISION:

         /*
         ** Copy the 4th Instance 1 attribute (Revision) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_REVISION_MAJOR_VALUE, 0 );
         ABCC_SetMsgData8( psNewMessage, DEV_IA_REVISION_MINOR_VALUE, 1 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_REVISION_DS );
         break;
#endif
#if DEV_IA_SERIAL_NUMBER_ENABLE
      case ABP_DEV_IA_SERIAL_NUMBER:

         /*
         ** Copy the 5th Instance 1 attribute (Serial number) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, DEV_IA_SERIAL_NUMBER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_SERIAL_NUMBER_DS );
         break;
#endif
#if DEV_IA_PRODUCT_NAME_ENABLE
      case ABP_DEV_IA_PRODUCT_NAME:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( DEV_IA_PRODUCT_NAME_VALUE );

         /*
         ** Copy the 6th Instance 1 attribute (Product name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, DEV_IA_PRODUCT_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if DEV_IA_PROD_INSTANCE_ENABLE
      case ABP_DEV_IA_PROD_INSTANCE:

         /*
         ** Copy the 7th Instance 1 attribute (Producing instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_PROD_INSTANCE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_PROD_INSTANCE_DS );
         break;
#endif
#if DEV_IA_CONS_INSTANCE_ENABLE
      case ABP_DEV_IA_CONS_INSTANCE:

         /*
         ** Copy the 8th Instance 1 attribute (Consuming instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_CONS_INSTANCE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_CONS_INSTANCE_DS );
         break;
#endif
#if DEV_IA_ADDRESS_FROM_NET_ENABLE
      case ABP_DEV_IA_ADDRESS_FROM_NET:

         /*
         ** Copy the 9th Instance 1 attribute (Enable address from net) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_ADDRESS_FROM_NET_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_ADDRESS_FROM_NET_DS );
         break;
#endif
#if DEV_IA_BAUD_RATE_FROM_NET_ENABLE
      case ABP_DEV_IA_BAUD_RATE_FROM_NET:

         /*
         ** Copy the 10th Instance 1 attribute (Enable baud rate from net) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_BAUD_RATE_FROM_NET_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_BAUD_RATE_FROM_NET_DS );
         break;
#endif
#if DEV_IA_ENABLE_APP_CIP_OBJECTS_ENABLE
      case ABP_DEV_IA_ENABLE_APP_CIP_OBJECTS:

         /*
         ** Copy the 11th Instance 1 attribute (Enable CIP request forwarding) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_ENABLE_APP_CIP_OBJECTS_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_ENABLE_APP_CIP_OBJECTS_DS );
         break;
#endif
#if DEV_IA_ENABLE_PARAM_OBJECT_ENABLE
      case ABP_DEV_IA_ENABLE_PARAM_OBJECT:

         /*
         ** Copy the 12th Instance 1 attribute (Enable Parameter object) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_ENABLE_PARAM_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_ENABLE_PARAM_OBJECT_DS );
         break;
#endif
#if DEV_IA_ENABLE_QUICK_CONNECT_ENABLE
      case ABP_DEV_IA_ENABLE_QUICK_CONNECT:

         /*
         ** Copy the 13th Instance 1 attribute (Enable Quick Connect) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DEV_IA_ENABLE_QUICK_CONNECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_ENABLE_PARAM_OBJECT_DS );
         break;
#endif
#if DEV_IA_PREPEND_PRODUCING_ENABLE
      case ABP_DEV_IA_PREPEND_PRODUCING:

         /*
         ** Copy the 18th Instance 1 attribute (Prepend producing profile instance) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_PREPEND_PRODUCING_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_PREPEND_PRODUCING_DS );
         break;
#endif
#if DEV_IA_PREPEND_CONSUMING_ENABLE
      case ABP_DEV_IA_PREPEND_CONSUMING:

         /*
         ** Copy the 19th Instance 1 attribute (Prepend consuming profile instance) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_PREPEND_CONSUMING_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_PREPEND_CONSUMING_DS );
         break;
#endif
#if DEV_IA_ABCC_ADI_OBJECT_ENABLE
      case ABP_DEV_IA_ABCC_ADI_OBJECT:

         /*
         ** Copy the 20th Instance 1 attribute (ABCC ADI Object Number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DEV_IA_ABCC_ADI_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_ABCC_ADI_OBJECT_DS );
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
** Processes commands to the DEV Object (Instance 0)
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
   ** This function processes commands to the DeviceNet Object (Instance 0).
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
         iStrLength = (UINT16)strlen( dev_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, dev_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }

      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData8( psNewMessage, dev_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, dev_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, dev_sObject.iHighestInstanceNo, 0 );
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
   case ABP_DEV_CMD_PROCESS_CIP_OBJ_REQUEST:

      /*
      ** Optionally implement the Process CIP Object Request service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch( Command number ) */
}

#endif /* DEV_OBJ_ENABLE */
