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
** Source file for the EtherCAT ABCC Object.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "abcc_obj_cfg.h"
#include "ect.h"
#include "abp.h"
#include "abp_ect.h"
#include "string.h"
#include "appl_abcc_handler.h"
#include "abcc_port.h"

#if ECT_OBJ_ENABLE

/*******************************************************************************
** Defines
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Object attribute values
**------------------------------------------------------------------------------
*/
#define ECT_OA_NAME_VALUE                          "EtherCAT"
#define ECT_OA_REV_VALUE                           1
#define ECT_OA_NUM_INST_VALUE                      1
#define ECT_OA_HIGHEST_INST_VALUE                  1

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure describing the EtherCAT Instance 1 attribute 13 ADI Translation
**------------------------------------------------------------------------------
*/
#if ECT_IA_ADI_TRANS_ENABLE
typedef struct ect_Instance1AdiTranslation
{
   UINT16 iADIInstanceNumber;
   UINT16 iADIObjectIndex;
}
ect_Instance1AdiTranslation;
#endif

/*------------------------------------------------------------------------------
** Structure describing the EtherCAT Instance 1 attribute 15 Object Subindex
** Translation
**------------------------------------------------------------------------------
*/
#if ECT_IA_ADI_TRANS_ENABLE
typedef struct ect_Instance1ObjSubTrans
{
   UINT16 iADIInstanceNumber;
   UINT16 iADIObjectIndex;
   UINT8  bADIObjectIndexSubindex;
}
ect_Instance1ObjSubTrans;
#endif

/*------------------------------------------------------------------------------
** Structure describing the EtherCAT Instance 1 attributes.
**------------------------------------------------------------------------------
*/
#if( ECT_IA_ENUM_ADIS_ENABLE || ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE || ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE || ECT_IA_ADI_TRANS_ENABLE || ECT_IA_OBJ_SUB_TRANS_ENABLE )
typedef struct ect_Instance
{
#if ECT_IA_ENUM_ADIS_ENABLE
   UINT16 aiEnumADIs[ ECT_IA_ENUM_ADIS_ARRAY_SIZE ];
#endif
#if ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE
   UINT16 aiWrPdAssyInstTrans[ ECT_IA_WR_PD_ASSY_INST_TRANS_SIZE ];
#endif
#if ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE
   UINT16 aiRdPdAssyInstTrans[ ECT_IA_RD_PD_ASSY_INST_TRANS_SIZE ];
#endif
#if ECT_IA_ADI_TRANS_ENABLE
   ect_Instance1AdiTranslation asADITrans[ ECT_IA_ADI_TRANS_SIZE ];
#endif
#if ECT_IA_OBJ_SUB_TRANS_ENABLE
   ect_Instance1ObjSubTrans asObjSubTrans[ ECT_IA_OBJ_SUB_TRANS_SIZE ];
#endif
}
ect_InstanceType;
#endif /* if( ECT_IA_ENUM_ADIS_ENABLE... */

/*------------------------------------------------------------------------------
** Structure describing a Powerlink Object.
**------------------------------------------------------------------------------
*/
typedef struct ect_Object
{
    const  char* pcName;
    UINT8  bRevision;
    UINT16 iNumberOfInstances;
    UINT16 iHighestInstanceNo;
}
ect_ObjectType;

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static const ect_ObjectType ect_sObject =
{
   ECT_OA_NAME_VALUE,                           /* Name.                                              */
   ECT_OA_REV_VALUE,                            /* Revision.                                          */
   ECT_OA_NUM_INST_VALUE,                       /* Number of instances.                               */
   ECT_OA_HIGHEST_INST_VALUE                    /* Highest instance number.                           */
};

#if( ECT_IA_ENUM_ADIS_ENABLE || ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE || ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE || ECT_IA_ADI_TRANS_ENABLE || ECT_IA_OBJ_SUB_TRANS_ENABLE )
static const ect_InstanceType ect_sInstance =
{
#if ECT_IA_ENUM_ADIS_ENABLE
   ECT_IA_ENUM_ADIS_VALUE,
#endif
#if ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE
   ECT_IA_WR_PD_ASSY_INST_TRANS_VALUE,
#endif
#if ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE
   ECT_IA_RD_PD_ASSY_INST_TRANS_VALUE,
#endif
#if ECT_IA_ADI_TRANS_ENABLE
   ECT_IA_ADI_TRANS_VALUE,
#endif
#if ECT_IA_OBJ_SUB_TRANS_ENABLE
   ECT_IA_OBJ_SUB_TRANS_VALUE
#endif
};

#endif /* #if( ECT_IA_ENUM_ADIS_ENABLE...*/

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Processes commands to ECT Instances
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
   ** This function processes commands to the EtherCAT Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) != 1 )
   {
      /*
      ** The Instance does not exist.
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
#if ECT_IA_VENDOR_ID_ENABLE
      case ABP_ECT_IA_VENDOR_ID:

         /*
         ** Copy the 1st Instance 1 attribute (Vendor ID) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, ECT_IA_VENDOR_ID_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_VENDOR_ID_DS );
         break;
#endif
#if ECT_IA_PRODUCT_CODE_ENABLE
      case ABP_ECT_IA_PRODUCT_CODE:

         /*
         ** Copy the 2nd Instance 1 attribute (Product code) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, ECT_IA_PRODUCT_CODE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_PRODUCT_CODE_DS );
         break;
#endif
#if ECT_IA_MAJOR_REV_ENABLE
      case ABP_ECT_IA_MAJOR_REV:

         /*
         ** Copy the 3rd Instance 1 attribute (Revision high word) to the
         ** message.
         */
         ABCC_SetMsgData16( psNewMessage, ECT_IA_REVISION_HW_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_MAJOR_REV_DS );
         break;
#endif
#if ECT_IA_MINOR_REV_ENABLE
      case ABP_ECT_IA_MINOR_REV:

         /*
         ** Copy the 4th Instance 1 attribute (Revision low word) to the message.
         */
         ABCC_SetMsgData16( psNewMessage, ECT_IA_REVISION_LW_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_MINOR_REV_DS );
         break;
#endif
#if ECT_IA_SERIAL_NUMBER_ENABLE
      case ABP_ECT_IA_SERIAL_NUMBER:

         /*
         ** Copy the 5th Instance 1 attribute (Serial number) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, ECT_IA_SERIAL_NUMBER_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_SERIAL_NUMBER_DS );
         break;
#endif
#if ECT_IA_MANF_DEVICE_NAME_ENABLE
      case ABP_ECT_IA_MANF_DEV_NAME:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( ECT_IA_MANF_DEVICE_NAME_VALUE );

         /*
         ** Copy the 6th Instance 1 attribute (Manufacturer device name) to the
         ** message.
         */
         ABCC_SetMsgString( psNewMessage, ECT_IA_MANF_DEVICE_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
#endif
#if ECT_IA_MANF_HW_VERSION_ENABLE
      case ABP_ECT_IA_MANF_HW_VER:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( ECT_IA_MANF_HW_VERSION_VALUE );

         /*
         ** Copy the 7th Instance 1 attribute (Manufacturer hardware version) to
         ** the message.
         */
         ABCC_SetMsgString( psNewMessage, ECT_IA_MANF_HW_VERSION_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
      }
      break;
#endif
#if ECT_IA_MANF_SW_VERSION_ENABLE
      case ABP_ECT_IA_MANF_SW_VER:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( ECT_IA_MANF_SW_VERSION_VALUE );

         /*
         ** Copy the 8th Instance 1 attribute (Manufacturer software version) to
         ** the message.
         */
         ABCC_SetMsgString( psNewMessage, ECT_IA_MANF_SW_VERSION_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
      }
      break;
#endif
#if ECT_IA_ENUM_ADIS_ENABLE
      case ABP_ECT_IA_ENUM_ADIS:
      {
         UINT16  iIndex;

         /*
         ** Copy the 9th Instance 1 attribute (ENUM ADIs) to the message.
         */
         for( iIndex = 0; iIndex < ECT_IA_ENUM_ADIS_ARRAY_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.aiEnumADIs[ iIndex ],
                               ( ABP_UINT16_SIZEOF * iIndex ) );
         }

         ABP_SetMsgResponse( psNewMessage, ( ECT_IA_ENUM_ADIS_ARRAY_SIZE * ABP_UINT16_SIZEOF ) );
         break;
      }
#endif
#if ECT_IA_DEVICE_TYPE_ENABLE
      case ABP_ECT_IA_DEVICE_TYPE:

         /*
         ** Copy the 10th Instance 1 attribute (Device type) to the message.
         */
         ABCC_SetMsgData32( psNewMessage, ECT_IA_DEVICE_TYPE_VALUE, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_ECT_IA_DEVICE_TYPE_DS );
         break;
#endif
#if ECT_IA_WR_PD_ASSY_INST_TRANS_ENABLE
      case ABP_ECT_IA_WR_PD_ASSY_INST_TRANS:
      {
         UINT16  iIndex;

         /*
         ** Copy the 11th Instance 1 attribute (Write PD assembly instance
         ** translation) to the message.
         */
         for( iIndex = 0; iIndex < ECT_IA_WR_PD_ASSY_INST_TRANS_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.aiWrPdAssyInstTrans[ iIndex ],
                               ( ABP_UINT16_SIZEOF * iIndex ) );
         }

         ABP_SetMsgResponse( psNewMessage, ( ECT_IA_WR_PD_ASSY_INST_TRANS_SIZE * ABP_UINT16_SIZEOF ) );
         break;
      }
#endif

#if ECT_IA_RD_PD_ASSY_INST_TRANS_ENABLE
      case ABP_ECT_IA_RD_PD_ASSY_INST_TRANS:
      {
         UINT16  iIndex;

         /*
         ** Copy the 12th Instance 1 attribute (Read PD assembly instance
         ** translation) to the message.
         */
         for( iIndex = 0; iIndex < ECT_IA_RD_PD_ASSY_INST_TRANS_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.aiRdPdAssyInstTrans[ iIndex ],
                               ( ABP_UINT16_SIZEOF * iIndex ) );
         }

         ABP_SetMsgResponse( psNewMessage, ( ECT_IA_RD_PD_ASSY_INST_TRANS_SIZE * ABP_UINT16_SIZEOF ) );
         break;
      }
#endif
#if ECT_IA_ADI_TRANS_ENABLE
      case ABP_ECT_IA_ADI_TRANS:
      {
         UINT16  iIndex;

         /*
         ** Copy the 13th Instance 1 attribute (ADI translation) to the message.
         */
         for( iIndex = 0; iIndex < ECT_IA_ADI_TRANS_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.asADITrans[ iIndex ].iADIInstanceNumber,
                               ( ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF ) * iIndex ) );
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.asADITrans[ iIndex ].iADIObjectIndex,
                               ( ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF ) * iIndex ) + ABP_UINT16_SIZEOF );
         }

         ABP_SetMsgResponse( psNewMessage, ( ECT_IA_ADI_TRANS_SIZE * ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF ) ) );
         break;
      }
#endif
#if ECT_IA_OBJ_SUB_TRANS_ENABLE
      case ABP_ECT_IA_OBJ_SUB_TRANS:
      {
         UINT16  iIndex;

         /*
         ** Copy the 13th Instance 1 attribute (ADI translation) to the message.
         */
         for( iIndex = 0; iIndex < ECT_IA_OBJ_SUB_TRANS_SIZE; ++iIndex )
         {
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.asObjSubTrans[ iIndex ].iADIInstanceNumber,
                               ( ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF + ABP_UINT8_SIZEOF ) * iIndex ) );
            ABCC_SetMsgData16( psNewMessage,
                               ect_sInstance.asObjSubTrans[ iIndex ].iADIObjectIndex,
                               ( ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF + ABP_UINT8_SIZEOF ) * iIndex ) + ABP_UINT16_SIZEOF );
            ABCC_SetMsgData8( psNewMessage,
                               ect_sInstance.asObjSubTrans[ iIndex ].bADIObjectIndexSubindex,
                               ( ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF + ABP_UINT8_SIZEOF ) * iIndex ) + ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF );
         }

         ABP_SetMsgResponse( psNewMessage, ( ECT_IA_OBJ_SUB_TRANS_SIZE * ( ABP_UINT16_SIZEOF + ABP_UINT16_SIZEOF + ABP_UINT8_SIZEOF ) ) );
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
   }
}

/*------------------------------------------------------------------------------
** Processes commands to the ECT Object (Instance 0)
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
   ** This function processes commands to the EtherCAT Object
   ** (Instance 0).
   */
   switch( ABCC_GetMsgCmdBits( psNewMessage ) )
   {
   case ABP_CMD_GET_ATTR:

      switch( ABCC_GetMsgCmdExt0( psNewMessage ) )
      {
      case ABP_OA_NAME:
      {
         UINT16 iStrLength;

         iStrLength = (UINT16)strlen( ECT_OA_NAME_VALUE );

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgString( psNewMessage, ECT_OA_NAME_VALUE, iStrLength, 0 );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;
      }
      case ABP_OA_REV:

         /*
         ** Copy the attribute to a message.
         */

         ABCC_SetMsgData8( psNewMessage, ect_sObject.bRevision, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_OA_NUM_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, ect_sObject.iNumberOfInstances, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_OA_HIGHEST_INST:

         /*
         ** Copy the attribute to a message.
         */
         ABCC_SetMsgData16( psNewMessage, ect_sObject.iHighestInstanceNo, 0 );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      default:

         /*
         ** Unsupported attribute.
         */
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;

      } /* End of switch( Attribute number ) */

      break;

   default:

      /*
      ** Unsupported command.
      */
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch( Command number ) */
}

/*******************************************************************************
** Public Services
********************************************************************************
*/

void ECT_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the EtherCAT Object and its Instance.
   */
   if( ABCC_GetMsgInstance( psNewMessage ) == ABP_INST_OBJ )
   {
      /*
      ** EtherCAT object Command.
      */
      ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** EtherCAT instance Command.
      */
      InstanceCommand( psNewMessage );
   }

   ABCC_SendRespMsg( psNewMessage );
}

#endif
