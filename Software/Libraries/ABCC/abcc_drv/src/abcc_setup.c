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
** Implementation of abcc setup state machine
********************************************************************************
********************************************************************************
*/

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_cmd_seq_if.h"
#include "abcc_handler.h"
#include "abcc_drv_if.h"
#include "abcc_debug_err.h"
/*******************************************************************************
** Constants
********************************************************************************
*/

/*
** Invalid ADI index.
*/
#define AD_INVALID_ADI_INDEX           ( 0xffff )

/*******************************************************************************
** Typedefs
********************************************************************************
*/

#if !ABCC_CFG_DRV_CMD_SEQ_ENABLE
typedef enum CmdSetupState
{
   SETUP_BEFORE_USER_INIT,
   SETUP_USER_INIT,
   SETUP_AFTER_USER_INIT,
   SETUP_DONE
}
CmdSetupStateType;
#endif

/*******************************************************************************
** Private Globals
********************************************************************************
*/
#if !ABCC_CFG_DRV_CMD_SEQ_ENABLE
static void SendSetupCommand( ABP_MsgType* psMsg );
#endif
static ABCC_CmdSeqCmdStatusType DataFormatCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType DataFormatResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType ParamSupportCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType ParamSupportResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType ModuleTypeCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType ModuleTypeResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType NetworkTypeCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType NetworkTypeResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType PreparePdMapping( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType ReadWriteMapCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType ReadWriteMapResp( ABP_MsgType* psMsg );

static void TriggerUserInit( void );

static ABCC_CmdSeqCmdStatusType RdPdSizeCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType RdPdSizeResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType WrPdSizeCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType WrPdSizeResp( ABP_MsgType* psMsg );

static ABCC_CmdSeqCmdStatusType SetupCompleteCmd( ABP_MsgType* psMsg );
static ABCC_CmdSeqRespStatusType SetupCompleteResp( ABP_MsgType* psMsg );

static void SetupDone( void );

/*
** Command sequence until user setup.
*/
static const ABCC_CmdSeqType SetupSeqBeforeUserInit[] =
{
   ABCC_CMD_SEQ( DataFormatCmd,    DataFormatResp ),
   ABCC_CMD_SEQ( ParamSupportCmd,  ParamSupportResp ),
   ABCC_CMD_SEQ( ModuleTypeCmd,    ModuleTypeResp ),
   ABCC_CMD_SEQ( NetworkTypeCmd,   NetworkTypeResp ),
   ABCC_CMD_SEQ( PreparePdMapping, NULL ),
   ABCC_CMD_SEQ( ReadWriteMapCmd,  ReadWriteMapResp ),
   ABCC_CMD_SEQ_END()
};

/*
** Command sequence after user setup.
*/
static const ABCC_CmdSeqType SetupSeqAfterUserInit[] =
{
   ABCC_CMD_SEQ( RdPdSizeCmd,      RdPdSizeResp ),
   ABCC_CMD_SEQ( WrPdSizeCmd,      WrPdSizeResp ),
   ABCC_CMD_SEQ( SetupCompleteCmd, SetupCompleteResp ),
   ABCC_CMD_SEQ_END()
};


/*------------------------------------------------------------------------------
** abcc_iModuleType       - ABCC module type (read out during SETUP state)
** abcc_iNetworkType      - ABCC network type (read out during SETUP state)
** abcc_eNetFormat        - Data endian format of the network
**                          (read out during SETUP state)
** abcc_eParameterSupport - Parameter support (read out during SETUP state)
** abcc_eCmdState         - Current command message state during initialization
** abcc_psAdiEntry        - Pointer to list of ADIs
** abcc_psDefaultMap      - Pointer to list of default mapped ADIs
** abcc_iNumAdi           - Number of ADIs in abcc_psAdiEntry list
** abcc_MappingIndex      - Index of next ADI to map from abcc_psDefaultMap
** abcc_iPdReadSize       - Read process data size in octets
** abcc_iPdWriteSize      - Write process data size in octets
** abcc_iPdWriteBitSize   - Write process data size in bits
** abcc_iPdReadBitSize    - Read process data size in bits
**------------------------------------------------------------------------------
*/
static UINT16               abcc_iModuleType;
static UINT16               abcc_iNetworkType;
static NetFormatType        abcc_eNetFormat;
static ParameterSupportType abcc_eParameterSupport;

/*
** Help varibales for ADI mapping servcie
*/
static AD_AdiEntryType*     abcc_psAdiEntry    = NULL;
static AD_DefaultMapType*   abcc_psDefaultMap  = NULL;
static UINT16               abcc_iNumAdi       = 0;
static UINT16               abcc_iMappingIndex = 0;

/*
** Currently used process data sizes
*/
static UINT16               abcc_iPdReadSize   = 0;
static UINT16               abcc_iPdWriteSize  = 0;

/*
** Current wrpd sizes divided in octets and bits
*/
static UINT16   abcc_iPdWriteBitSize = 0;

/*
** Current rdpd sizes divided in octets and bits
*/
static UINT16   abcc_iPdReadBitSize = 0;

#if !ABCC_CFG_DRV_CMD_SEQ_ENABLE

/*
** Current state main setup state
*/
static CmdSetupStateType eSetupState;

/*
** Sub states for main setup state: SETUP_BEFORE_USER_INIT:
** DataFormatCmd        0
** ParamSupportCmd      1
** ModuleTypeCmd        2
** NetworkTypeCmd       3
** ReadWriteMapCmd      4
** PreparePdMapping     5
**
** SETUP_USER_INIT:     Has no sub states
**
** Sub states for main setup state: SETUP_AFTER_USER_INIT:
** RdPdSizeCmd          0
** WrPdSizeCmd          1
** SetupCompleteCmd     2
**
** SETUP_DONE:       Has no sub states
**
*/
static UINT8 bSetupSubState = 0;

/*
** Pointer to current main setup state sequence.
** SETUP_BEFORE_USER_INIT: SetupSeqBeforeUserInit[]
** SETUP_AFTER_USER_INIT:  SetupSeqAfterUserInit[]
*/
static const ABCC_CmdSeqType* pasSetupSeq;
#endif

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Find ADI entry table index for the specified instance number.
**------------------------------------------------------------------------------
** Arguments:
**    iInstance         -  Instance number.
**
** Returns:
**    AD_INVALID_ADI_INDEX      - Instance was not found.
**------------------------------------------------------------------------------
*/
static UINT16 GetAdiIndex( UINT16 iInstance )
{
   UINT16 i;
   UINT16  iIndex;

   iIndex = AD_INVALID_ADI_INDEX;

   for( i = 0; i < abcc_iNumAdi; i++ )
   {
      if( abcc_psAdiEntry[ i ].iInstance == iInstance )
      {
         iIndex = i;
         break;
      }
   }

   return( iIndex );
}

static UINT16 abcc_GetAdiMapSizeInBits(const AD_AdiEntryType* psAdiEntry, UINT8 bNumElem, UINT8 bElemStartIndex )
{
   UINT16 iSize;
#if( ABCC_CFG_STRUCT_DATA_TYPE )
   UINT16 i;
   if( psAdiEntry->psStruct == NULL )
   {
      iSize = ABCC_GetDataTypeSizeInBits( psAdiEntry->bDataType ) * bNumElem ;
   }
   else
   {
      iSize = 0;
      for( i = bElemStartIndex; i < ( bNumElem + bElemStartIndex ) ; i++ )
      {
         iSize += ABCC_GetDataTypeSizeInBits( psAdiEntry->psStruct[ i ].bDataType );
      }
   }
#else
      (void)bElemStartIndex;
      iSize = ABCC_GetDataTypeSizeInBits( psAdiEntry->bDataType ) * bNumElem ;
#endif

   return iSize;
}

static void abcc_FillMapExtCommand( ABP_MsgType16* psMsg16, UINT16 iAdi, UINT8 bAdiTotNumElem, UINT8 bElemStartIndex, UINT8 bNumElem, UINT8 bDataType )
{
   psMsg16->aiData[ 0 ] = iTOiLe( iAdi );                               /* ADI Instance number. */
   ABCC_SetLowAddrOct( psMsg16->aiData[ 1 ], bAdiTotNumElem);           /* Total number of elements in ADI. */
   ABCC_SetHighAddrOct( psMsg16->aiData[ 1 ], bElemStartIndex );
   ABCC_SetLowAddrOct( psMsg16->aiData[ 2 ], bNumElem);
   ABCC_SetHighAddrOct( psMsg16->aiData[ 2 ], 1 );                      /* Number of type descriptors. */
   ABCC_SetLowAddrOct( psMsg16->aiData[ 3 ], bDataType );               /* ADI element data type. */
   psMsg16->sHeader.iDataSize = iTOiLe( 7 );                            /* The number of used octets in aiData. (The bytes written below). */
}

/*******************************************************************************
** Public Services
********************************************************************************
*/

void ABCC_SetupInit( void )
{
   abcc_iModuleType = 0xFFFF;
   abcc_iNetworkType = 0xFFFF;
   abcc_eNetFormat = NET_UNKNOWN;
   abcc_eParameterSupport = PARAMETER_UNKNOWN;

   abcc_psAdiEntry     = NULL;
   abcc_psDefaultMap   = NULL;
   abcc_iNumAdi = 0;
   abcc_iMappingIndex  = 0;
   abcc_iPdReadSize    = 0;
   abcc_iPdWriteSize   = 0;
   abcc_iPdWriteBitSize  = 0;
   abcc_iPdReadBitSize   = 0;
}

/*------------------------------------------------------------------------------
** Data format command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType DataFormatCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1,
                      ABP_NW_IA_DATA_FORMAT, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Data format response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType DataFormatResp( ABP_MsgType* psMsg )
{
   UINT8 bFormat;
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    ABCC_GetErrorCode( psMsg ) );

   ABCC_GetMsgData8( psMsg, &bFormat, 0 );
   abcc_eNetFormat = (NetFormatType)bFormat;
   ABCC_ASSERT( abcc_eNetFormat < NET_UNKNOWN );
   DEBUG_EVENT(  "RSP MSG_DATA_FORMAT: %d\n", abcc_eNetFormat  );
   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Parameter support command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType ParamSupportCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1,
                      ABP_NW_IA_PARAM_SUPPORT, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Parameter support response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType ParamSupportResp( ABP_MsgType* psMsg )
{
   UINT8 bParamSupport;
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );

   ABCC_GetMsgData8( psMsg, &bParamSupport, 0 );
   abcc_eParameterSupport = (ParameterSupportType)bParamSupport;
   DEBUG_EVENT(  "RSP MSG_GET_PARAM_SUPPORT: %d\n", abcc_eParameterSupport  );
   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Module type command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType ModuleTypeCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_ANB, 1,
                      ABP_ANB_IA_MODULE_TYPE, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Module type response.
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType ModuleTypeResp( ABP_MsgType* psMsg )
{
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );
   ABCC_GetMsgData16( psMsg, &abcc_iModuleType, 0 );
   DEBUG_EVENT(  "RSP MSG_GET_MODULE_ID: 0x%x\n",abcc_iModuleType  );
   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Network type command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType NetworkTypeCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1,
                      ABP_NW_IA_NW_TYPE, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Network type response.
** Part of a command sequence and implements function callback
** ABCC_CmdRespCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType NetworkTypeResp( ABP_MsgType* psMsg )
{
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );
   ABCC_GetMsgData16( psMsg, &abcc_iNetworkType, 0 );
   DEBUG_EVENT(  "RSP MSG_GET_NETWORK_ID :0x%x\n", abcc_iNetworkType  );
   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Actions required before mapping.
** Part of a command sequence and implements function callback.
** ABCC_CmdRespCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType PreparePdMapping( ABP_MsgType* psMsg )
{
   (void)psMsg;
   abcc_iNumAdi = ABCC_CbfAdiMappingReq( (const AD_AdiEntryType**)&abcc_psAdiEntry,
                                         (const AD_DefaultMapType**)&abcc_psDefaultMap );
   /*
   ** No command shall be sent.
   */
   return( ABCC_SKIP_COMMAND );
}

/*------------------------------------------------------------------------------
** Read write mapping command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType ReadWriteMapCmd( ABP_MsgType* psMsg )
{
   UINT16 iLocalMapIndex;
   UINT16 iLocalSize;
   ABCC_MsgType pMsgSendBuffer;

   pMsgSendBuffer.psMsg = psMsg;
   iLocalMapIndex = 0;

   /*
   ** Unique source id for each mapping command
   */
   ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iSourceIdDestObj, ABCC_GetNewSourceId() );


   if( abcc_psAdiEntry && abcc_psDefaultMap && ( abcc_psDefaultMap[ abcc_iMappingIndex ].eDir != PD_END_MAP ) )
   {
      if( abcc_psDefaultMap[ abcc_iMappingIndex ].iInstance != AD_MAP_PAD_ADI )
      {
         iLocalMapIndex = GetAdiIndex( abcc_psDefaultMap[ abcc_iMappingIndex ].iInstance );

         if( iLocalMapIndex == AD_INVALID_ADI_INDEX )
         {
            ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_DEFAULT_MAP_ERR,
                        (UINT32)abcc_psDefaultMap[ abcc_iMappingIndex ].iInstance );
            ABCC_SetMainStateError();
            return( ABCC_CMD_ABORT_SEQ );
         }
      }
   }
   else
   {
      return( ABCC_SKIP_COMMAND );
   }

   if( ABCC_ReadModuleId() == ABP_MODULE_ID_ACTIVE_ABCC40 )
   {
      UINT8 bNumElemToMap;
      UINT8 bElemMapStartIndex;

      /*
      ** Implement mapping according to the extended command for ABCC40.
      */
      ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->sHeader.iSourceIdDestObj, ABP_OBJ_NUM_NW );
      pMsgSendBuffer.psMsg16->sHeader.iInstance            = iTOiLe( 1 );

      /*
      ** Number of mapping items to add.
      */
      ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdExt0CmdExt1, 1 );

      /*
      ** Reserved
      */
      ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdExt0CmdExt1, 0 );

      if ( abcc_psDefaultMap[ abcc_iMappingIndex ].iInstance != AD_MAP_PAD_ADI )
      {
         if( abcc_psDefaultMap[ abcc_iMappingIndex ].bNumElem == AD_DEFAULT_MAP_ALL_ELEM )
         {
            bNumElemToMap = abcc_psAdiEntry[ iLocalMapIndex ].bNumOfElements;
            bElemMapStartIndex = 0;
         }
         else
         {
            bNumElemToMap = abcc_psDefaultMap[ abcc_iMappingIndex ].bNumElem;
            bElemMapStartIndex = abcc_psDefaultMap[ abcc_iMappingIndex ].bElemStartIndex;
         }

         abcc_FillMapExtCommand( pMsgSendBuffer.psMsg16,
                                 abcc_psAdiEntry[ iLocalMapIndex ].iInstance ,      /* Adi */
                                 abcc_psAdiEntry[ iLocalMapIndex ].bNumOfElements , /* Adi total num elements */
                                 bElemMapStartIndex,                                /* Mapping  start index */
                                 bNumElemToMap,                                     /* Num elements to map */
                                 abcc_psAdiEntry[ iLocalMapIndex ].bDataType  );    /* Data type */
         iLocalSize = abcc_GetAdiMapSizeInBits( &abcc_psAdiEntry[ iLocalMapIndex ],
                                                bNumElemToMap, bElemMapStartIndex );

#if( ABCC_CFG_STRUCT_DATA_TYPE )
         if ( abcc_psAdiEntry[ iLocalMapIndex ].psStruct != NULL )
         {
            UINT16 iDescOffset;
            iDescOffset = 0;
            ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->aiData[ 2 ], bNumElemToMap );

            while ( iDescOffset < bNumElemToMap  )
            {
               ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->aiData[ ( iDescOffset >> 1) + 3 ], abcc_psAdiEntry[ iLocalMapIndex ].psStruct[ iDescOffset + bElemMapStartIndex].bDataType );
               iDescOffset++;
               if( iDescOffset < bNumElemToMap )
               {
                  ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->aiData[ ( iDescOffset >> 1) + 3 ], abcc_psAdiEntry[ iLocalMapIndex ].psStruct[ iDescOffset + bElemMapStartIndex].bDataType );
                  iDescOffset++;
               }
            }
            pMsgSendBuffer.psMsg16->sHeader.iDataSize = iTOiLe( 6 + iDescOffset );
         }
#endif
      }
      else
      {
          abcc_FillMapExtCommand( pMsgSendBuffer.psMsg16,
                                  0 ,                                                /* Adi */
                                  abcc_psDefaultMap[ abcc_iMappingIndex ].bNumElem , /* Adi total num elements */
                                  0,                                                 /* Mapping  start index */
                                  abcc_psDefaultMap[ abcc_iMappingIndex ].bNumElem , /* Num elements to map */
                                  ABP_PAD1 );                                        /* Data type */
         iLocalSize = abcc_psDefaultMap[ abcc_iMappingIndex ].bNumElem;

      }

      if( abcc_psDefaultMap[ abcc_iMappingIndex ].eDir == PD_READ )
      {
         ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdReserved, ABP_MSG_HEADER_C_BIT | ABP_NW_CMD_MAP_ADI_READ_EXT_AREA );
         abcc_iPdReadBitSize += iLocalSize;
         abcc_iPdReadSize = ( abcc_iPdReadBitSize + 7 )/8;
      }
      else
      {
         ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdReserved, ABP_MSG_HEADER_C_BIT | ABP_NW_CMD_MAP_ADI_WRITE_EXT_AREA );
         abcc_iPdWriteBitSize += iLocalSize;
         abcc_iPdWriteSize = ( abcc_iPdWriteBitSize + 7 )/8;
      }
      abcc_iMappingIndex++;
   }
   else
   {
      /*
      ** If an ABCC30 is attached.
      */
      iLocalSize = ABCC_GetDataTypeSize( abcc_psAdiEntry[ iLocalMapIndex ].bDataType ) * abcc_psAdiEntry[ iLocalMapIndex ].bNumOfElements;

      ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->sHeader.iSourceIdDestObj, ABP_OBJ_NUM_NW );
      pMsgSendBuffer.psMsg16->sHeader.iInstance            = iTOiLe( 1 );
      pMsgSendBuffer.psMsg16->sHeader.iDataSize            = iTOiLe( 4 );
      pMsgSendBuffer.psMsg16->sHeader.iCmdExt0CmdExt1     = iTOiLe( abcc_psAdiEntry[ iLocalMapIndex ].iInstance );  /* ADI Instance number. */

      ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->aiData[ 0 ], abcc_psAdiEntry[ iLocalMapIndex ].bDataType );       /* ADI data type. */
      ABCC_SetHighAddrOct( pMsgSendBuffer.psMsg16->aiData[ 0 ], abcc_psAdiEntry[ iLocalMapIndex ].bNumOfElements ); /* Number of elements in ADI. */
      pMsgSendBuffer.psMsg16->aiData[ 1 ]       = iTOiLe( iLocalMapIndex + 1 );                                     /* ADI order number. */

      if( abcc_psDefaultMap[ abcc_iMappingIndex ].eDir == PD_READ )
      {
         ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdReserved, ABP_MSG_HEADER_C_BIT | ABP_NW_CMD_MAP_ADI_READ_AREA );
         abcc_iPdReadSize             += iLocalSize;
      }
      else
      {
         ABCC_SetLowAddrOct( pMsgSendBuffer.psMsg16->sHeader.iCmdReserved, ABP_MSG_HEADER_C_BIT | ABP_NW_CMD_MAP_ADI_WRITE_AREA );
         abcc_iPdWriteSize            += iLocalSize;
      }
      abcc_iMappingIndex++;
   }

   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Read write mapping response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType ReadWriteMapResp( ABP_MsgType* psMsg )
{
   DEBUG_EVENT(  "RSP MSG_MAP_IO_****\n"  );
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );

   if( abcc_psAdiEntry && abcc_psDefaultMap && ( abcc_psDefaultMap[ abcc_iMappingIndex ].eDir == PD_END_MAP ) )
   {
      /*
      ** This was the last mapping command. Proceed with next command.
      */
      return( ABCC_EXEC_NEXT_COMMAND );
   }

   /*
   ** Next mapping
   */
   return( ABCC_EXEC_CURR_COMMAND );
}

/*------------------------------------------------------------------------------
** Trigger user init when mapping is done.
** Part of a command sequence and implements function callback
** ABCC_CmdSeqDoneHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static void TriggerUserInit( void )
{
   ABCC_CbfUserInitReq();
}

/*------------------------------------------------------------------------------
** Read map size command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType RdPdSizeCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1,
                      ABP_NW_IA_READ_PD_SIZE, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Read map size response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType RdPdSizeResp( ABP_MsgType* psMsg )
{
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );

   if( abcc_psDefaultMap == NULL )
   {
      /*
      ** Use received read size
      */
      ABCC_GetMsgData16( psMsg, &abcc_iPdReadSize, 0 );
   }
   else
   {
      UINT16 iSize;

      /*
      ** Verify that ABCC and driver has the same view
      */
      ABCC_GetMsgData16( psMsg, &iSize, 0 );
      ABCC_ASSERT( abcc_iPdReadSize == iSize );
   }

   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Write map size command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType WrPdSizeCmd( ABP_MsgType* psMsg )
{
   ABCC_GetAttribute( psMsg, ABP_OBJ_NUM_NW, 1,
                      ABP_NW_IA_WRITE_PD_SIZE, ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Write map size response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType WrPdSizeResp( ABP_MsgType* psMsg )
{
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );

   if( abcc_psDefaultMap == NULL )
   {
      /*
      ** Use received write size
      */
      ABCC_GetMsgData16( psMsg, &abcc_iPdWriteSize, 0 );
   }
   else
   {
      UINT16 iSize;

      /*
      ** Verify that ABCC and driver has the same view
      */
      ABCC_GetMsgData16( psMsg, &iSize, 0 );
      ABCC_ASSERT( abcc_iPdWriteSize == iSize );
   }

   return( ABCC_EXEC_NEXT_COMMAND );
}

/*------------------------------------------------------------------------------
** Setup complete command
** Part of a command sequence and implements function callback
** ABCC_CmdSeqCmdHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqCmdStatusType SetupCompleteCmd( ABP_MsgType* psMsg )
{
   ABCC_SetByteAttribute(  psMsg, ABP_OBJ_NUM_ANB, 1,
                           ABP_ANB_IA_SETUP_COMPLETE, TRUE,
                           ABCC_GetNewSourceId() );
   return( ABCC_SEND_COMMAND );
}

/*------------------------------------------------------------------------------
** Setup complete response
** Part of a command sequence and implements function callback
** ABCC_CmdSeqRespHandler (abcc.h)
**------------------------------------------------------------------------------
*/
static ABCC_CmdSeqRespStatusType SetupCompleteResp( ABP_MsgType* psMsg )
{
   ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                    ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET,
                    (UINT32)ABCC_GetErrorCode( psMsg ) );
   pnABCC_DrvSetPdSize(abcc_iPdReadSize, abcc_iPdWriteSize );
   DEBUG_EVENT(  "RSP MSG_SETUP_COMPLETE\n"  );
   return( ABCC_EXEC_NEXT_COMMAND );
}

static void SetupDone( void )
{
   DEBUG_EVENT(  "Mapped PD size, RdPd %d WrPd: %d", abcc_iPdReadSize, abcc_iPdWriteSize  );
}

#if !ABCC_CFG_DRV_CMD_SEQ_ENABLE
/*------------------------------------------------------------------------------
** Handles responses for setup messages.
** Depending on main setup state and sub state the corresponding response
** handler is called.
** In state SETUP_BEFORE_USER_INIT SetupSeqBeforeUserInit[] sequence is used.
** In state SETUP_AFTER_USER_INIT SetupSeqAfterUserInit[] sequence is used.
** bSetupSubState is used as index in the sequence array.
**
** When the response is handled the next setup command is triggered.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg                   - Pointer to response buffer
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
static void HandleSetupRespone( ABP_MsgType* psMsg )
{
   if( pasSetupSeq[ bSetupSubState ].pnRespHandler( psMsg ) == ABCC_EXEC_NEXT_COMMAND )
   {
      bSetupSubState++;
   }
   SendSetupCommand( psMsg );
}

/*------------------------------------------------------------------------------
** Send the next command in the setup sequence.
** Depending on main setup state and sub state the corresponding command handler
** is called.
** In state SETUP_BEFORE_USER_INIT SetupSeqBeforeUserInit[] sequence is used.
** In state SETUP_AFTER_USER_INIT SetupSeqAfterUserInit[] sequence is used.
** bSetupSubState is used as index in the sequence array.
**
** At the end of SETUP_BEFORE_USER_INIT ABCC_CbfUserInitReq() callback
** is triggered.
** At the end of SETUP_AFTER_USER_INIT the setup is done.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg                   - Pointer to response buffer
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
static void SendSetupCommand( ABP_MsgType* psMsg )
{
   while( pasSetupSeq[ bSetupSubState ].pnCmdHandler != NULL )
   {
      if( pasSetupSeq[ bSetupSubState ].pnCmdHandler( psMsg ) == ABCC_SKIP_COMMAND )
      {
         bSetupSubState++;
      }
      else
      {
         ABCC_SendCmdMsg( psMsg, HandleSetupRespone );
         break;
      }
   }

   if( pasSetupSeq[ bSetupSubState ].pnCmdHandler == NULL )
   {
      if( eSetupState == SETUP_BEFORE_USER_INIT )
      {
         eSetupState = SETUP_USER_INIT;
         TriggerUserInit();
      }
      else
      {
         eSetupState = SETUP_DONE;
         SetupDone();
      }
   }
}
#endif

#if ABCC_CFG_DRV_CMD_SEQ_ENABLE
void ABCC_StartSetup( void )
{
   ABCC_AddCmdSeq( SetupSeqBeforeUserInit, TriggerUserInit );
}

void ABCC_UserInitComplete( void )
{
   ABCC_AddCmdSeq( SetupSeqAfterUserInit, SetupDone );
}
#else
void  ABCC_StartSetup( void )
{
   ABP_MsgType* psMsg;
   eSetupState = SETUP_BEFORE_USER_INIT;
   bSetupSubState = 0;
   pasSetupSeq = SetupSeqBeforeUserInit;
   psMsg = ABCC_GetCmdMsgBuffer();
   ABCC_ASSERT( psMsg );
   SendSetupCommand( psMsg );
}

void ABCC_UserInitComplete( void )
{
   ABP_MsgType* psMsg;
   psMsg = ABCC_GetCmdMsgBuffer();
   ABCC_ASSERT( psMsg );
   eSetupState = SETUP_AFTER_USER_INIT;
   bSetupSubState = 0;
   pasSetupSeq = SetupSeqAfterUserInit;
   SendSetupCommand( psMsg );
}
#endif

UINT16 ABCC_NetworkType( void )
{
   return abcc_iNetworkType;
}

UINT16 ABCC_ModuleType( void )
{
   return abcc_iModuleType;
}

NetFormatType ABCC_NetFormatType( void )
{
    return abcc_eNetFormat;
}

ParameterSupportType ABCC_ParameterSupport( void )
{
   return abcc_eParameterSupport;
}
