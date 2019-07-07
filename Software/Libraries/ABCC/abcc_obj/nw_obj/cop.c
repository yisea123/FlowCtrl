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
** Source file for the CANopen Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_obj_cfg.h"
#include "cop.h"
#include "abp.h"
#include "abp_cop.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if COP_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define COP_OA_NAME_VALUE                          "CANopen"
#define COP_OA_REV_VALUE                           1
#define COP_OA_NUM_INST_VALUE                      1
#define COP_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing an CANopen Object.
**------------------------------------------------------------------------------
*/
typedef struct cop_Object
{
   const  char* pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
cop_ObjectType;


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

static const cop_ObjectType cop_sObject =
{
   COP_OA_NAME_VALUE,                           /* Name.                                              */
   COP_OA_REV_VALUE,                            /* Revision.                                          */
   COP_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   COP_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

/*******************************************************************************
** Public Services
********************************************************************************
*/

void COP_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the CANopen Object and its Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** The CANopen object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** The CANopen instance Command.
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
** Processes commands to COP Instances
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
#ifdef COP_IA_VENDOR_ID_ENABLE
      case ABP_COP_IA_VENDOR_ID:

         /*
         ** Copy the 1st Instance 1 attribute (Vendor ID) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, COP_IA_VENDOR_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_COP_IA_VENDOR_ID_DS );
         break;
#endif
#ifdef COP_IA_PRODUCT_CODE_ENABLE
      case ABP_COP_IA_PRODUCT_CODE:

         /*
         ** Copy the 2nd Instance 1 attribute (Product code) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, COP_IA_PRODUCT_CODE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_COP_IA_PRODUCT_CODE_DS );
         break;
#endif
#ifdef COP_IA_REV_MAJOR_ENABLE
      case ABP_COP_IA_MAJOR_REV:

         /*
         ** Copy the 3rd Instance 1 attribute (Major revision) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, COP_IA_REV_MAJOR_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_COP_IA_MAJOR_REV_DS );
         break;
#endif
#ifdef COP_IA_REV_MINOR_ENABLE
      case ABP_COP_IA_MINOR_REV:

         /*
         ** Copy the 4th Instance 1 attribute (Minor revision) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, COP_IA_REV_MINOR_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_COP_IA_MINOR_REV_DS );
         break;
#endif
#ifdef COP_IA_SERIAL_NUMBER_ENABLE
      case ABP_COP_IA_SERIAL_NUMBER:

         /*
         ** Copy the 5th Instance 1 attribute (Serial number) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, COP_IA_SERIAL_NUMBER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_COP_IA_SERIAL_NUMBER_DS );
         break;
#endif
#ifdef COP_IA_MANF_DEV_NAME_ENABLE
      case ABP_COP_IA_MANF_DEV_NAME:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( COP_IA_MANF_DEV_NAME_VALUE );

         /*
         ** Copy the 6th Instance 1 attribute (Manufacture device name) to the message.
         */
         ABCC_SetMsgString( psNewMessage, COP_IA_MANF_DEV_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
      }
#endif
#ifdef COP_IA_MANF_HW_VER_ENABLE
      case ABP_COP_IA_MANF_HW_VER:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( COP_IA_MANF_HW_VER_VALUE );

         /*
         ** Copy the 7th Instance 1 attribute (Manufacture hardware version) to the message.
         */
         ABCC_SetMsgString( psNewMessage, COP_IA_MANF_HW_VER_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
      }
#endif
#ifdef COP_IA_MANF_SW_VER_ENABLE
      case ABP_COP_IA_MANF_SW_VER:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( COP_IA_MANF_SW_VER_VALUE );

         /*
         ** Copy the 8th Instance 1 attribute (Manufacture software version) to the message.
         */
         ABCC_SetMsgString( psNewMessage, COP_IA_MANF_SW_VER_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
      }
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
** Processes commands to the COP Object (Instance 0)
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
   ** This function processes commands to the CANopen Object (Instance 0).
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
         iStrLength = (UINT16)strlen( cop_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, cop_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }

      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData8( psNewMessage, cop_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, cop_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, cop_sObject.iHighestInstanceNo, 0 );
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

#endif /* COP_OBJ_ENABLE */
