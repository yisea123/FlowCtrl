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
** Source file for the EtherNet/IP Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "abcc_obj_cfg.h"
#include "eip.h"
#include "abp.h"
#include "abp_eip.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if EIP_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define EIP_OA_NAME_VALUE                          "EtherNet/IP"
#define EIP_OA_REV_VALUE                           2
#define EIP_OA_NUM_INST_VALUE                      1
#define EIP_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing an Ethernet/IP Object.
**------------------------------------------------------------------------------
*/
typedef struct eip_Object
{
   const  char* pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
eip_ObjectType;

/*------------------------------------------------------------------------------
** Structure describing an EtherNet/IP Instance Map used for attribute 27 and 28.
**------------------------------------------------------------------------------
*/
typedef struct eip_InstanceMap
{
   UINT16 iInstanceNumber;
   UINT16 iInstanceSize;
}
eip_InstanceMapType;

/*------------------------------------------------------------------------------
** Structure describing the EtherNet/IP Instance 1 attributes.
**------------------------------------------------------------------------------
*/
#if( EIP_IA_PROD_INSTANCE_ENABLE || EIP_IA_CONS_INSTANCE_ENABLE || EIP_IA_PROD_INSTANCE_MAP_ENABLE || EIP_IA_CONS_INSTANCE_MAP_ENABLE )
typedef struct eip_Instance
{
#if EIP_IA_PROD_INSTANCE_ENABLE
   UINT16 aiProducingInstanceNumber[ EIP_IA_PROD_INSTANCE_ARRAY_SIZE ];
#endif
#if EIP_IA_CONS_INSTANCE_ENABLE
   UINT16 aiConsumingInstanceNumber[ EIP_IA_CONS_INSTANCE_ARRAY_SIZE ];
#endif
#if EIP_IA_PROD_INSTANCE_MAP_ENABLE
   eip_InstanceMapType asProducingInstanceMap[ EIP_IA_PROD_INSTANCE_MAP_ARRAY_SIZE ];
#endif
#if EIP_IA_CONS_INSTANCE_MAP_ENABLE
   eip_InstanceMapType asConsumingInstanceMap[ EIP_IA_CONS_INSTANCE_MAP_ARRAY_SIZE ];
#endif
}
eip_InstanceType;
#endif

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

static const eip_ObjectType eip_sObject =
{
   EIP_OA_NAME_VALUE,                           /* Name.                                              */
   EIP_OA_REV_VALUE,                            /* Revision.                                          */
   EIP_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   EIP_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

#if( EIP_IA_PROD_INSTANCE_ENABLE || EIP_IA_CONS_INSTANCE_ENABLE || EIP_IA_PROD_INSTANCE_MAP_ENABLE || EIP_IA_CONS_INSTANCE_MAP_ENABLE )
static const eip_InstanceType eip_sInstance =
{
#if EIP_IA_PROD_INSTANCE_ENABLE
   EIP_IA_PROD_INSTANCE_VALUE,                  /* 7  Producing instance number.                      */
#endif
#if EIP_IA_CONS_INSTANCE_ENABLE
   EIP_IA_CONS_INSTANCE_VALUE,                  /* 8  Consuming instance number.                      */
#endif
#if EIP_IA_PROD_INSTANCE_MAP_ENABLE
   EIP_IA_PROD_INSTANCE_MAP_VALUE,              /* 27 Producing Instance Map.                         */
#endif
#if EIP_IA_CONS_INSTANCE_MAP_ENABLE
   EIP_IA_CONS_INSTANCE_MAP_VALUE,              /* 28 Consuming Instance Map.                         */
#endif
};
#endif
/*******************************************************************************
** Public Services
********************************************************************************
*/

void EIP_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the Ethernet/IP Object and it's Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Ethernet/IP object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Ethernet/IP instance Command.
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
** Processes commands to EIP Instances
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
   ** This function processes commands to the Ethernet/IP Instance.
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
#if EIP_IA_VENDOR_ID_ENABLE
      case ABP_EIP_IA_VENDOR_ID:

         /*
         ** Copy the 1st Instance 1 attribute (Vendor ID) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_VENDOR_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_VENDOR_ID_DS );
         break;
#endif
#if EIP_IA_DEVICE_TYPE_ENABLE
      case ABP_EIP_IA_DEVICE_TYPE:

         /*
         ** Copy the 2nd Instance 1 attribute (Device type) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_DEVICE_TYPE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_DEVICE_TYPE_DS );
         break;
#endif
#if EIP_IA_PRODUCT_CODE_ENABLE
      case ABP_EIP_IA_PRODUCT_CODE:

         /*
         ** Copy the 3rd Instance 1 attribute (Product code) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_PRODUCT_CODE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_PRODUCT_CODE_DS );
         break;
#endif
#if EIP_IA_REVISION_ENABLE
      case ABP_EIP_IA_REVISION:

         /*
         ** Copy the 4th Instance 1 attribute (Revision) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_REVISION_MAJOR_VALUE, 0 );
         ABCC_SetMsgData8( psNewMessage, EIP_IA_REVISION_MINOR_VALUE, 1 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_REVISION_DS );
         break;
#endif
#if EIP_IA_SERIAL_NUMBER_ENABLE
      case ABP_EIP_IA_SERIAL_NUMBER:

         /*
         ** Copy the 5th Instance 1 attribute (Serial number) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, EIP_IA_SERIAL_NUMBER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_SERIAL_NUMBER_DS );
         break;
#endif
#if EIP_IA_PRODUCT_NAME_ENABLE
      case ABP_EIP_IA_PRODUCT_NAME:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( EIP_IA_PRODUCT_NAME_VALUE );

         /*
         ** Copy the 6th Instance 1 attribute (Product name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, EIP_IA_PRODUCT_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if EIP_IA_PROD_INSTANCE_ENABLE
      case ABP_EIP_IA_PROD_INSTANCE:
      {
         UINT16  iIndex;

         /*
         ** Copy the 7th Instance 1 attribute (Producing instance number) to the message.
         */
         for( iIndex = 0; iIndex < EIP_IA_PROD_INSTANCE_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.aiProducingInstanceNumber[ iIndex ], 2 * iIndex );
         }

         /*
         ** The value of ABP_EIP_IA_PROD_INSTANCE_DS is counted per element, and
         ** should therefore be divided by 6 as long as ABP_EIP_IA_PROD_INSTANCE_DS
         ** is defined to be multiplied by 6 in abp_eip.h as ( 6 * ABP_UINT16_SIZEOF ).
         */
         ABP_SetMsgResponse( psNewMessage, EIP_IA_PROD_INSTANCE_ARRAY_SIZE * ( ABP_EIP_IA_PROD_INSTANCE_DS / 6 ) );
         break;
      }
#endif
#if EIP_IA_CONS_INSTANCE_ENABLE
      case ABP_EIP_IA_CONS_INSTANCE:
      {
         UINT16  iIndex;

         /*
         ** Copy the 8th Instance 1 attribute (Consuming instance number) to the message.
         */
         for( iIndex = 0; iIndex < EIP_IA_CONS_INSTANCE_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.aiConsumingInstanceNumber[ iIndex ], 2 * iIndex );
         }

         /*
         ** The value of ABP_EIP_IA_CONS_INSTANCE_DS is counted per element, and
         ** should therefore be divided by 6 as long as ABP_EIP_IA_CONS_INSTANCE_DS
         ** is defined to be multiplied by 6 in abp_eip.h as ( 6 * ABP_UINT16_SIZEOF ).
         */
         ABP_SetMsgResponse( psNewMessage, EIP_IA_CONS_INSTANCE_ARRAY_SIZE * ( ABP_EIP_IA_CONS_INSTANCE_DS / 6 ) );
         break;
      }
#endif
#if EIP_IA_COMM_SETTINGS_FROM_NET_ENABLE
      case ABP_EIP_IA_COMM_SETTINGS_FROM_NET:

         /*
         ** Copy the 9th Instance 1 attribute (Enable communication settings from net) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_COMM_SETTINGS_FROM_NET_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_COMM_SETTINGS_FROM_NET_DS );
         break;
#endif
#if EIP_IA_ENABLE_APP_CIP_OBJECTS_ENABLE
      case ABP_EIP_IA_ENABLE_APP_CIP_OBJECTS:

         /*
         ** Copy the 11th Instance 1 attribute (Enable CIP request forwarding) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_ENABLE_APP_CIP_OBJECTS_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ENABLE_APP_CIP_OBJECTS_DS );
         break;
#endif
#if EIP_IA_ENABLE_PARAM_OBJECT_ENABLE
      case ABP_EIP_IA_ENABLE_PARAM_OBJECT:

         /*
         ** Copy the 12th Instance 1 attribute (Enable Parameter object) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_ENABLE_PARAM_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ENABLE_PARAM_OBJECT_DS );
         break;
#endif
#if EIP_IA_INPUT_INSTANCE_OBJECT_ENABLE
      case ABP_EIP_IA_INPUT_INSTANCE_OBJECT:

         /*
         ** Copy the 13th Instance 1 attribute (Input only heartbeat instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_INPUT_INSTANCE_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_INPUT_INSTANCE_DS );
         break;
#endif
#if EIP_IA_LISTEN_INSTANCE_OBJECT_ENABLE
      case ABP_EIP_IA_LISTEN_INSTANCE_OBJECT:

         /*
         ** Copy the 14th Instance 1 attribute (Listen only heartbeat instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_LISTEN_INSTANCE_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_LISTEN_INSTANCE_DS );
         break;
#endif
#if EIP_IA_CONFIG_INSTANCE_ENABLE
      case ABP_EIP_IA_CONFIG_INSTANCE:

         /*
         ** Copy the 15th Instance 1 attribute (Assembly object Configuration instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_CONFIG_INSTANCE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_CONFIG_INSTANCE_DS );
         break;
#endif
#if EIP_IA_DISABLE_STRICT_IO_MATCH_ENABLE
      case ABP_EIP_IA_DISABLE_STRICT_IO_MATCH:

         /*
         ** Copy the 16th Instance 1 attribute (Disable Strict IO match) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_DISABLE_STRICT_IO_MATCH_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_DISABLE_STRICT_IO_MATCH_DS );
         break;
#endif
#if EIP_IA_ENABLE_UNCONNECTED_SEND_ENABLE
      case ABP_EIP_IA_ENABLE_UNCONNECTED_SEND:

         /*
         ** Copy the 17th Instance 1 attribute (Enable unconnected routing) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_ENABLE_UNCONNECTED_SEND_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ENABLE_UNCONNECTED_SEND_DS );
         break;
#endif
#if EIP_IA_INPUT_EXT_INSTANCE_OBJECT_ENABLE
      case ABP_EIP_IA_INPUT_EXT_INSTANCE_OBJECT:

         /*
         ** Copy the 18th Instance 1 attribute (Input only extended heartbeat instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_INPUT_EXT_INSTANCE_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_INPUT_EXT_INSTANCE_DS );
         break;
#endif
#if EIP_IA_LISTEN_EXT_INSTANCE_OBJECT_ENABLE
      case ABP_EIP_IA_LISTEN_EXT_INSTANCE_OBJECT:

         /*
         ** Copy the 19th Instance 1 attribute (Listen only extended heartbeat instance number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_LISTEN_EXT_INSTANCE_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_LISTEN_EXT_INSTANCE_DS );
         break;
#endif
#if EIP_IA_IF_LABEL_PORT_1_ENABLE
      case ABP_EIP_IA_IF_LABEL_PORT_1:
      {
         UINT16 iStrLength;

         /*
         ** Copy the 20th Instance 1 attribute (Interface label port 1) to the message.
         */
         iStrLength = (UINT16)strlen( EIP_IA_IF_LABEL_PORT_1_VALUE );
         ABCC_SetMsgString( psNewMessage, EIP_IA_IF_LABEL_PORT_1_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if EIP_IA_IF_LABEL_PORT_2_ENABLE
      case ABP_EIP_IA_IF_LABEL_PORT_2:
      {
         UINT16 iStrLength;

         /*
         ** Copy the 21st Instance 1 attribute (Interface label port 2) to the message.
         */
         iStrLength = (UINT16)strlen( EIP_IA_IF_LABEL_PORT_2_VALUE );
         ABCC_SetMsgString( psNewMessage, EIP_IA_IF_LABEL_PORT_2_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if EIP_IA_IF_LABEL_PORT_INT_ENABLE
      case ABP_EIP_IA_IF_LABEL_PORT_INT:
      {
         UINT16 iStrLength;

         /*
         ** Copy the 22nd Instance 1 attribute (Interface label internal port) to the message.
         */
         iStrLength = (UINT16)strlen( EIP_IA_IF_LABEL_PORT_INT_VALUE );
         ABCC_SetMsgString( psNewMessage, EIP_IA_IF_LABEL_PORT_INT_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if EIP_IA_ENABLE_APP_CIP_OBJECTS_EXT_ENABLE
      case ABP_EIP_IA_ENABLE_APP_CIP_OBJECTS_EXT:

         /*
         ** Copy the 23rd Instance 1 attribute (Enable CIP request forwarding extended) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_ENABLE_APP_CIP_OBJECTS_EXT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ENABLE_APP_CIP_OBJECTS_EXT_DS );
         break;
#endif
#if EIP_IA_PREPEND_PRODUCING_ENABLE
      case ABP_EIP_IA_PREPEND_PRODUCING:

         /*
         ** This attribute was deprecated and removed from Anybus CompactCom 40.
         ** Copy the 24th Instance 1 attribute (Prepend producing profile instance) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_PREPEND_PRODUCING_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_PREPEND_PRODUCING_DS );
         break;
#endif
#if EIP_IA_PREPEND_CONSUMING_ENABLE
      case ABP_EIP_IA_PREPEND_CONSUMING:

         /*
         ** This attribute was deprecated and removed from Anybus CompactCom 40.
         ** Copy the 25th Instance 1 attribute (Prepend consuming profile instance) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_PREPEND_CONSUMING_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_PREPEND_CONSUMING_DS );
         break;
#endif
#if EIP_IA_ENABLE_EIP_QC_ENABLE
      case ABP_EIP_IA_ENABLE_EIP_QC:

         /*
         ** Copy the 26th Instance 1 attribute (Enable EtherNet/IP QuickConnect) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_ENABLE_EIP_QC_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ENABLE_EIP_QC_DS );
         break;
#endif
#if EIP_IA_PROD_INSTANCE_MAP_ENABLE
      case ABP_EIP_IA_PROD_INSTANCE_MAP:
      {
         UINT16  iIndex;

         /*
         ** This attribute was deprecated and removed from Anybus CompactCom 40.
         ** Copy the 27th Instance 1 attribute (Producing Instance Map) to the message.
         */
         for( iIndex = 0; iIndex < EIP_IA_PROD_INSTANCE_MAP_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.asProducingInstanceMap[ iIndex ].iInstanceNumber, 4 * iIndex );
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.asProducingInstanceMap[ iIndex ].iInstanceSize, ( 4 * iIndex ) + 2 );
         }

         /*
         ** The value of ABP_EIP_IA_PROD_INSTANCE_MAP_DS is counted per element, and
         ** should therefore be divided by 6 as long as ABP_EIP_IA_PROD_INSTANCE_MAP_DS
         ** is defined to be multiplied by 6 in abp_eip.h as ( 6 * 2 * ABP_UINT16_SIZEOF ).
         */
         ABP_SetMsgResponse( psNewMessage, EIP_IA_PROD_INSTANCE_MAP_ARRAY_SIZE * ( ABP_EIP_IA_PROD_INSTANCE_MAP_DS / 6 ) );
         break;
      }
#endif
#if EIP_IA_CONS_INSTANCE_MAP_ENABLE
      case ABP_EIP_IA_CONS_INSTANCE_MAP:
      {
         UINT16  iIndex;

         /*
         ** This attribute was deprecated and removed from Anybus CompactCom 40.
         ** Copy the 28th Instance 1 attribute (Consuming Instance Map) to the message.
         */
         for( iIndex = 0; iIndex < EIP_IA_CONS_INSTANCE_MAP_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.asConsumingInstanceMap[ iIndex ].iInstanceNumber, 4 * iIndex );
            ABCC_SetMsgData16( psNewMessage, eip_sInstance.asConsumingInstanceMap[ iIndex ].iInstanceSize, ( 4 * iIndex ) + 2 );
         }

         /*
         ** The value of ABP_EIP_IA_CONS_INSTANCE_MAP_DS is counted per element, and
         ** should therefore be divided by 6 as long as ABP_EIP_IA_CONS_INSTANCE_MAP_DS
         ** is defined to be multiplied by 6 in abp_eip.h as ( 6 * 2 * ABP_UINT16_SIZEOF ).
         */
         ABP_SetMsgResponse( psNewMessage, EIP_IA_CONS_INSTANCE_MAP_ARRAY_SIZE * ( ABP_EIP_IA_CONS_INSTANCE_MAP_DS / 6 ) );
         break;
      }
#endif
#if EIP_IA_IGNORE_SEQ_COUNT_CHECK_ENABLE
      case ABP_EIP_IA_IGNORE_SEQ_COUNT_CHECK:

         /*
         ** Copy the 29th Instance 1 attribute (Ignore Sequence Count Check) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, EIP_IA_IGNORE_SEQ_COUNT_CHECK_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_IGNORE_SEQ_COUNT_CHECK_DS );
         break;
#endif
#if EIP_IA_ABCC_ADI_OBJECT_ENABLE
      case ABP_EIP_IA_ABCC_ADI_OBJECT:

         /*
         ** Copy the 30th Instance 1 attribute (ABCC ADI Object number) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, EIP_IA_ABCC_ADI_OBJECT_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_EIP_IA_ABCC_ADI_OBJECT_DS );
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
** Processes commands to the EIP Object (Instance 0)
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
   ** This function processes commands to the Ethernet/IP Object (Instance 0).
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
         iStrLength = (UINT16)strlen( eip_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, eip_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }

      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData8( psNewMessage, eip_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, eip_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, eip_sObject.iHighestInstanceNo, 0 );
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
   case ABP_EIP_CMD_PROCESS_CIP_OBJ_REQUEST:

      /*
      ** Optionally implement the Process CIP Object Request service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_EIP_CMD_SET_CONFIG_DATA:

      /*
      ** Optionally implement the Set Config Data service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_EIP_CMD_PROCESS_CIP_ROUTING_REQUEST:

      /*
      ** Optionally implement the CIP Routing Request service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_EIP_CMD_GET_CONFIG_DATA:

      /*
      ** Optionally implement the Get Config Data service here.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_EIP_CMD_PROCESS_CIP_OBJ_REQUEST_EXT:

      /*
      ** Optionally implement the Process CIP Object Request Extended service
      ** here.
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

#endif /* EIP_OBJ_ENABLE */
