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
** Source file for the PROFIBUS DPV1 IO Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc_sys_adapt.h"
#include "abcc_obj_cfg.h"
#include "abcc.h"
#include "dpv1.h"
#include "abp_dpv1.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if DPV1_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

#define DPV1_PRM_STANDARD_BYTES                     7
#define DPV1_PRM_DPV1_STATUS_BYTES                  3

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define DPV1_OA_NAME_VALUE                          "PROFIBUS DP-V1"
#define DPV1_OA_REV_VALUE                           4
#define DPV1_OA_NUM_INST_VALUE                      1
#define DPV1_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** dpv1_ObjectType.
** Structure describing an Profibus Object.
**------------------------------------------------------------------------------
*/
typedef struct dpv1_Object
{
    const  char* pcName;
    UINT8  bRevision;
    UINT16 iNumberOfInstances;
    UINT16 iHighestInstanceNo;
}
dpv1_ObjectType;

/*------------------------------------------------------------------------------
** Structure describing the EtherNet/IP Instance 1 attributes.
**------------------------------------------------------------------------------
*/
#if DPV1_IA_PRM_DATA_ENABLE ||          \
    DPV1_IA_EXPECTED_CFG_DATA_ENABLE || \
    DPV1_IA_IM_HEADER_ENABLE
typedef struct dpv1_Instance
{
#if DPV1_IA_PRM_DATA_ENABLE
   UINT8 abParamData[ DPV1_IA_PRM_DATA_ARRAY_SIZE ];
#endif
#if DPV1_IA_EXPECTED_CFG_DATA_ENABLE
   UINT8 abExpectedCfgData[ DPV1_IA_EXPECTED_CFG_DATA_ARRAY_SIZE ];
#endif
#if DPV1_IA_IM_HEADER_ENABLE
   UINT8 abIMheader[ DPV1_IA_IM_HEADER_ARRAY_SIZE];
#endif
}
dpv1_InstanceType;
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

static const dpv1_ObjectType dpv1_sObject =
{
   DPV1_OA_NAME_VALUE,                           /* Name.                                              */
   DPV1_OA_REV_VALUE,                            /* Revision.                                          */
   DPV1_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   DPV1_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

#if DPV1_IA_PRM_DATA_ENABLE ||          \
    DPV1_IA_EXPECTED_CFG_DATA_ENABLE || \
    DPV1_IA_IM_HEADER_ENABLE
static dpv1_InstanceType dpv1_sInstance =
{
#if DPV1_IA_PRM_DATA_ENABLE
   DPV1_IA_PRM_DATA_VALUE,
#endif
#if DPV1_IA_EXPECTED_CFG_DATA_ENABLE
   DPV1_IA_EXPECTED_CFG_DATA_VALUE,
#endif
#if DPV1_IA_IM_HEADER_ENABLE
   DPV1_IA_IM_HEADER_VALUE,
#endif
};
#endif

/*******************************************************************************
** Public Services
********************************************************************************
*/

void DPV1_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the Profibus Object and it's Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** Profibus object Command
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Profibus instance Command
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
** Processes commands to DPV1 Instances
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
   ** This function processes commands to the Profibus Instance.
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
#if DPV1_IA_IDENT_NUMBER_ENABLE
      case ABP_DPV1_IA_IDENT_NUMBER:

         /*
         ** Copy the 1st Instance 1 attribute (Device ID) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_IDENT_NUMBER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_IDENT_NUMBER_DS );
         break;
#endif
#if DPV1_IA_PRM_DATA_ENABLE
      case ABP_DPV1_IA_PRM_DATA:
      
         /*
         ** The Parameter data is not getable. It's only setable.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_GETABLE );
         break;
#endif
#if DPV1_IA_EXPECTED_CFG_DATA_ENABLE
      case ABP_DPV1_IA_EXPECTED_CFG_DATA:
     {
         UINT16  iIndex;

         /*
         ** Copy the 3rd Instance 1 attribute (Expected configuration) to the message.
         */
         for( iIndex = 0; iIndex < DPV1_IA_EXPECTED_CFG_DATA_ARRAY_SIZE; iIndex++ )
         {
            ABCC_SetMsgData8( psNewMessage, dpv1_sInstance.abExpectedCfgData[ iIndex ], iIndex );
         }
         ABP_SetMsgResponse( psNewMessage, DPV1_IA_EXPECTED_CFG_DATA_ARRAY_SIZE );
         break;
      }
#endif
#if DPV1_IA_SSA_ENABLED_ENABLE
      case ABP_DPV1_IA_SSA_ENABLED:

         /*
         ** Copy the 4th Instance 1 attribute (SSA enabled) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DPV1_IA_SSA_ENABLED_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_SSA_ENABLED_DS );
         break;
#endif
#if DPV1_IA_MANUFACTURER_ID_ENABLE
      case ABP_DPV1_IA_MANUFACTURER_ID:

         /*
         ** Copy the 8th Instance 1 attribute (Manufacturer ID) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_MANUFACTURER_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_MANUFACTURER_ID_DS );
         break;
#endif
#if DPV1_IA_ORDER_ID_ENABLE
      case ABP_DPV1_IA_ORDER_ID:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( DPV1_IA_ORDER_ID_VALUE );

         /*
         ** The maximum number of elements allowed in the array should not be
         ** checked here. It should be checked earlier...
         */
         if( iStrLength > ABP_DPV1_IA_ORDER_ID_DS )
         {
            iStrLength = ABP_DPV1_IA_ORDER_ID_DS;
         }

         /*
         ** Copy the 9th Instance 1 attribute (IM Order ID) to the message.
         */
         ABCC_SetMsgString( psNewMessage, DPV1_IA_ORDER_ID_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
      }
#endif
#if DPV1_IA_SERIAL_NO_ENABLE
      case ABP_DPV1_IA_SERIAL_NO:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( DPV1_IA_SERIAL_NO_VALUE );

         /*
         ** The maximum number of elements allowed in the array should not be
         ** checked here. It should be checked earlier...
         */
         if( iStrLength > ABP_DPV1_IA_SERIAL_NO_DS )
         {
            iStrLength = ABP_DPV1_IA_SERIAL_NO_DS;
         }

         /*
         ** Copy the 10th Instance 1 attribute (IM Serial number) to the message.
         */
         ABCC_SetMsgString( psNewMessage, DPV1_IA_SERIAL_NO_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, iStrLength );
         break;
      }
#endif
#if DPV1_IA_HW_REV_ENABLE
      case ABP_DPV1_IA_HW_REV:

         /*
         ** Copy the 11th Instance 1 attribute (IM Hardware revision) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_HW_REV_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_HW_REV_DS );
         break;
#endif
#if DPV1_IA_SW_REV_ENABLE
      case ABP_DPV1_IA_SW_REV:

         /*
         ** Copy the 12th Instance 1 attribute (Software revision) to the message.
         */
         ABCC_SetMsgData8( psNewMessage, DPV1_IA_SW_REV_SYMBOL_VALUE, 0 );
         ABCC_SetMsgData8( psNewMessage, DPV1_IA_SW_REV_MAJOR_VALUE, 1 );
         ABCC_SetMsgData8( psNewMessage, DPV1_IA_SW_REV_MINOR_VALUE, 2 );
         ABCC_SetMsgData8( psNewMessage, DPV1_IA_SW_REV_BUILD_VALUE, 3 );

         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_SW_REV_DS );
         break;
#endif
#if DPV1_IA_REV_COUNTER_ENABLE
      case ABP_DPV1_IA_REV_COUNTER:

         /*
         ** Copy the 13th Instance 1 attribute (Revision counter) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_REV_COUNTER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_REV_COUNTER_DS );
         break;
#endif
#if DPV1_IA_PROFILE_ID_ENABLE
      case ABP_DPV1_IA_PROFILE_ID:

         /*
         ** Copy the 14th Instance 1 attribute (Profile ID) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_PROFILE_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_PROFILE_ID_DS );
         break;
#endif
#if DPV1_IA_PROFILE_SPEC_TYPE_ENABLE
      case ABP_DPV1_IA_PROFILE_SPEC_TYPE:

         /*
         ** Copy the 15th Instance 1 attribute (Profile specific type) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, DPV1_IA_PROFILE_SPEC_TYPE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_DPV1_IA_PROFILE_SPEC_TYPE_DS );
         break;
#endif
#if DPV1_IA_IM_HEADER_ENABLE
      case ABP_DPV1_IA_IM_HEADER:
     {
         UINT16  iIndex;

         /*
         ** Copy the 18th Instance 1 attribute (IM header) to the message.
         */
         for( iIndex = 0; iIndex < DPV1_IA_IM_HEADER_ARRAY_SIZE; iIndex++ )
         {
            ABCC_SetMsgData8( psNewMessage, dpv1_sInstance.abIMheader[ iIndex ], iIndex );
         }
         ABP_SetMsgResponse( psNewMessage, DPV1_IA_IM_HEADER_ARRAY_SIZE );
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
#if DPV1_IA_PRM_DATA_ENABLE || DPV1_IA_EXPECTED_CFG_DATA_ENABLE
   case ABP_CMD_SET_ATTR:
   {
      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
#if DPV1_IA_PRM_DATA_ENABLE
      case ABP_DPV1_IA_PRM_DATA:

         /*
         ** The data size is verified.
         */
         if( psNewMessage->sHeader.iDataSize > DPV1_IA_PRM_DATA_ARRAY_SIZE )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );
         }
         else if( psNewMessage->sHeader.iDataSize < DPV1_PRM_STANDARD_BYTES )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_NOT_ENOUGH_DATA );
         }
         else
         {
            /*
            ** If the application has defined user specific parameter data,
            ** this is the place to check the data (from byte 10 and forth)
            ** In this example we only update the attribute.
            */
            ABCC_PORT_MemCpy( dpv1_sInstance.abParamData, psNewMessage->abData, DPV1_PRM_STANDARD_BYTES + DPV1_PRM_DPV1_STATUS_BYTES );

            ABP_SetMsgResponse( psNewMessage, 0 );
         }
         break;
#endif
#if DPV1_IA_EXPECTED_CFG_DATA_ENABLE
      case ABP_DPV1_IA_EXPECTED_CFG_DATA:

         /*
         ** The data size is verified.
         */
         if( psNewMessage->sHeader.iDataSize > DPV1_IA_EXPECTED_CFG_DATA_ARRAY_SIZE )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );
         }
         else
         {
            ABCC_PORT_MemCpy( dpv1_sInstance.abExpectedCfgData, psNewMessage->abData, psNewMessage->sHeader.iDataSize );

            ABP_SetMsgResponse( psNewMessage, 0 );
         }
         break;
#endif

#if DPV1_IA_IDENT_NUMBER_ENABLE
      case ABP_DPV1_IA_IDENT_NUMBER:
#endif
#if DPV1_IA_SSA_ENABLED_ENABLE
      case ABP_DPV1_IA_SSA_ENABLED:
#endif
#if DPV1_IA_MANUFACTURER_ID_ENABLE
      case ABP_DPV1_IA_MANUFACTURER_ID:
#endif
#if DPV1_IA_ORDER_ID_ENABLE
      case ABP_DPV1_IA_ORDER_ID:
#endif
#if DPV1_IA_SERIAL_NO_ENABLE
      case ABP_DPV1_IA_SERIAL_NO:
#endif
#if DPV1_IA_HW_REV_ENABLE
      case ABP_DPV1_IA_HW_REV:
#endif
#if DPV1_IA_SW_REV_ENABLE
      case ABP_DPV1_IA_SW_REV:
#endif
#if DPV1_IA_REV_COUNTER_ENABLE
      case ABP_DPV1_IA_REV_COUNTER:
#endif
#if DPV1_IA_PROFILE_ID_ENABLE
      case ABP_DPV1_IA_PROFILE_ID:
#endif
#if DPV1_IA_PROFILE_SPEC_TYPE_ENABLE
      case ABP_DPV1_IA_PROFILE_SPEC_TYPE:
#endif
         /*
         ** Unsettable attribute.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
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
#endif
   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch( Command number ) */
}

/*------------------------------------------------------------------------------
** Processes commands to the DPV1 Object (Instance 0)
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
   ** This function processes commands to the Profibus Object (Instance 0).
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
         iStrLength = (UINT16)strlen( dpv1_sObject.pcName );
         ABCC_SetMsgString( psNewMessage, dpv1_sObject.pcName, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }

      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData8( psNewMessage, dpv1_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_REV_DS );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, dpv1_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_OA_NUM_INST_DS );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, dpv1_sObject.iHighestInstanceNo, 0 );
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

#endif /* DPV1_OBJ_ENABLE */
