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
** ABCC driver API used by the the application.
********************************************************************************
********************************************************************************
** Services provided by ABCC driver:
**    ABCC_StartDriver()                  - Make the driver ready for use.
**    ABCC_isReadyForCommunication()      - Check if the driver is ready to
**                                          communicate.
**    ABCC_ShutdownDriver()               - Close the driver.
**    ABCC_HWReset                        - Reset ABCC.
**    ABCC_HWReleaseReset                 - Release the reset on ABCC.
**    ABCC_RunTimerSystem()               - Timer information to ABCC.
**    ABCC_RunDriver()                    - Main routine to be called cyclically
**                                          during polling.
**    ABCC_UserInitComplete()             - End of user specific setup sequence.
**    ABCC_SendCmdMsg()                   - Sends a message command to ABCC.
**    ABCC_SendRespMsg()                  - Sends a message response to ABCC.
**    ABCC_SendRemapRespMsg()             - Send remap response message
**    ABCC_SetAppStatus()                 - Sets the application status.
**    ABCC_GetCmdMsgBuffer()              - Allocates the command message buffer.
**    ABCC_ReturnMsgBuffer()              - Frees the message buffer.
**    ABCC_TakeMsgBufferOwnership()       - Takes the ownership of the message buffer.
**    ABCC_ISR()                          - ABCC interrupt service routine.
**    ABCC_TriggerRdPdUpdate()            - Triggers the RdPd read.
**    ABCC_TriggerReceiveMessage()        - Triggers the message read.
**    ABCC_TriggerWrPdUpdate()            - Triggers the WrPd update.
**    ABCC_TriggerAnbStatusUpdate()       - Check for Anybus status change
**    ABCC_TriggerTransmitMessage()       - Check sending queue
**    ABCC_ModCap()                       - Reads the module capability.
**    ABCC_LedStatus()                    - Reads the led status.
**    ABCC_AnbState()                     - Reads the Anybus state.
**    ABCC_NetworkType()                  - Get network type.
**    ABCC_ModuleType()                   - Get module type.
**    ABCC_DataFormatType()               - Get network endian.
**    ABCC_ParameterSupport()             - Get parameter support.
**    ABCC_GetOpmode()                    - Get operating mode.
**    ABCC_GetAttribute()                 - Fills "Get Attribute" message
**    ABCC_SetByteAttribute()             - Fills "Set Attribute" message
**    ABCC_VerifyMessage()                - Checks if E-bit is set
**    ABCC_GetDataTypeSize()              - Returns size of ABCC data type
**    ABCC_ErrorHandler()                 - Common error handler
**
** Services to be implemented by user:
** ABCC_CbfReceiveMsg()                - Delivery of the received message.
** ABCC_CbfAdiMappingReq()             - Request of the ADI mapping information.
** ABCC_CbfUserInitReq()               - User specific setup made by the application.
** ABCC_CbfUpdateWriteProcessData()    - Request of the latest write process data.
** ABCC_CbfNewReadPd()                 - Delivery of the latest read process data.
** ABCC_CbfWdTimeout()                 - Communication lost.
** ABCC_CbfWdTimeoutRecovered()        - Communication restored.
** ABCC_CbfEvent()                     - Events received. Called from ISR.
** ABCC_CbfRemapDone()                 - Acknowledge of remap has been sent.
** ABCC_CbfAnbStateChanged()           - The anybus state has changed.
** ABCC_CbfSyncIsr()                   - Callback for sync event.
********************************************************************************
********************************************************************************
*/
#ifndef ABCC_H_
#define ABCC_H_

#include "abcc_drv_cfg.h"
#include "abcc_port.h"
#include "abcc_td.h"
#include "abp.h"
#include "abcc_ad_if.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Bit definitions of ABCC events.
** These bit definitions are used in the bit mask forwarded to the
** ABCC_CbfEvent() callback.
**------------------------------------------------------------------------------
*/
#define ABCC_ISR_EVENT_RDPD       0x01
#define ABCC_ISR_EVENT_RDMSG      0x02
#define ABCC_ISR_EVENT_WRMSG      0x04
#define ABCC_ISR_EVENT_STATUS     0x08

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Function types used by user to deliver messages to the application.
**------------------------------------------------------------------------------
*/
typedef void (*ABCC_MsgHandlerFuncType)( ABP_MsgType* psMsg );

/*------------------------------------------------------------------------------
** Data format type.
**------------------------------------------------------------------------------
*/
typedef enum ABCC_DataFormatType
{
   ABCC_DF_LSB_FIRST = 0,
   ABCC_DF_MSB_FIRST = 1
}
ABCC_DataFormatType;

/*------------------------------------------------------------------------------
** Driver severity codes indicated by ABCC_CbfDriverError.
**------------------------------------------------------------------------------
*/
typedef enum ABCC_SeverityType
{
   /*
   ** Information about an event that has occurred (e.g., serial message lost).
   */
   ABCC_SEV_INFORMATION = 0,

   /*
   ** An error of minor importance has occurred. The system can recover from
   ** this error.
   */
   ABCC_SEV_WARNING,

   /*
   ** A fatal event has occurred, the system cannot recover (e.g., driver is
   ** out of timers).
   */
   ABCC_SEV_FATAL,

   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */
   ABCC_SEV_SET_ENUM_ANSI_SIZE = 0x7FFF

}
ABCC_SeverityType;

/*------------------------------------------------------------------------------
**  Driver error codes indicated by ABCC_CbfDriverError.
**------------------------------------------------------------------------------
*/
typedef enum ABCC_ErrorCodeType
{
   ABCC_EC_NO_ERROR,
   ABCC_EC_INTERNAL_ERROR,
   ABCC_EC_LINK_CMD_QUEUE_FULL,
   ABCC_EC_LINK_RESP_QUEUE_FULL,
   ABCC_EC_OUT_OF_MSG_BUFFERS,
   ABCC_EC_TRYING_TO_FREE_NULL_POINTER,
   ABCC_EC_INCORRECT_OPERATING_MODE,
   ABCC_EC_INCORRECT_STATE,
   ABCC_EC_RESP_MSG_E_BIT_SET,
   ABCC_EC_WRPD_SIZE_ERR,
   ABCC_EC_RDPD_SIZE_ERR,
   ABCC_EC_RDMSG_SIZE_ERR,
   ABCC_EC_INVALID_RESP_SOURCE_ID,
   ABCC_EC_MODULE_NOT_DECTECTED,
   ABCC_EC_PARAMETER_NOT_VALID,
   ABCC_EC_MODULE_ID_NOT_SUPPORTED,
   ABCC_EC_DEFAULT_MAP_ERR,
   ABCC_EC_ERROR_IN_READ_MAP_CONFIG,
   ABCC_EC_ERROR_IN_WRITE_MAP_CONFIG,
   ABCC_EC_INTSTATUS_NOT_SUPPORTED_BY_DRV_IMPL,
   ABCC_EC_MODCAP_NOT_SUPPORTED_BY_DRV_IMPL,
   ABCC_EC_SPI_OP_NOT_ALLOWED_DURING_SPI_TRANSACTION,
   ABCC_EC_WRMSG_SIZE_ERR,
   ABCC_EC_MSG_BUFFER_CORRUPTED,
   ABCC_EC_MSG_BUFFER_ALREADY_FREED,
   ABCC_EC_NO_RESOURCES,
   ABCC_EC_HW_INIT_FAILED,
   ABCC_EC_RCV_CMD_SIZE_EXCEEDS_BUFFER,
   ABCC_EC_RCV_RESP_SIZE_EXCEEDS_BUFFER,
   ABCC_EC_UNEXPECTED_NULL_PTR,
   ABCC_EC_OUT_OF_CMD_SEQ_RESOURCES,
   ABCC_EC_SYS_ADAPTION_ERR,
   ABCC_EC_APPLICATION_SPECIFIC,
   ABCC_EC_SET_ENUM_ANSI_SIZE       = 0x7FFF
}
ABCC_ErrorCodeType;

/*------------------------------------------------------------------------------
**  Error information structure used by ABCC_GetDriverError()
**------------------------------------------------------------------------------
*/
typedef struct ABCC_ErrInfo
{
   ABCC_ErrorCodeType   eErrorCode;
   ABCC_SeverityType    eSeverity;
   UINT32               lAddInfo;
#if ABCC_CFG_DEBUG_ERR_ENABLED
   char*                pacSeverity;
   char*                pacErrorCode;
   char*                pacAddInfo;
   char*                pacLocation;
#endif
}
ABCC_ErrInfoType;

/*------------------------------------------------------------------------------
** ABCC_CommunicationStateType:
**
** ABCC_NOT_READY_FOR_COMMUNICATION: Nothing is wrong but it
**                                   is not yet possible to
**                                   communicate with the ABCC.
** ABCC_READY_FOR_COMMUNICATION:     Possible to communicate with ABCC
** ABCC_COMMUNICATION_ERROR:         ABCC module is not responding
**
**------------------------------------------------------------------------------
*/
typedef enum ABCC_CommunicationState
{
   ABCC_NOT_READY_FOR_COMMUNICATION = 0,
   ABCC_READY_FOR_COMMUNICATION = 1,
   ABCC_COMMUNICATION_ERROR = 2
}
ABCC_CommunicationStateType;


/*------------------------------------------------------------------------------
** Used for storing the data format of the field bus.
** NET_UNKNOWN means that the Anybus-CC has not yet responded to our command to
** read the fieldbus data format.
**------------------------------------------------------------------------------
*/
typedef enum NetFormatType
{
   NET_LITTLEENDIAN,
   NET_BIGENDIAN,
   NET_UNKNOWN
}
NetFormatType;

/*------------------------------------------------------------------------------
** Type for indicate if parameter support is available or not.
**------------------------------------------------------------------------------
*/
typedef enum ParameterSupportType
{
   NOT_PARAMETER_SUPPORT,
   PARAMETER_SUPPORT,
   PARAMETER_UNKNOWN
}
ParameterSupportType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Macros for basic endian swap. Used by conversion macros below.
**------------------------------------------------------------------------------
*/
#ifndef ABCC_iEndianSwap
#define ABCC_iEndianSwap( iFoo )    (UINT16)( ( (UINT16)(iFoo) >> 8 ) | ( (UINT16)(iFoo) << 8 ) )
#endif

#ifndef ABCC_lEndianSwap
#define ABCC_lEndianSwap( lFoo )      (UINT32)( ( (UINT32)ABCC_iEndianSwap( (UINT16)( (UINT32)(lFoo) ) ) << 16 ) | (UINT32)ABCC_iEndianSwap( (UINT32)(lFoo) >> 16 ) )
#endif

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
#ifndef ABCC_l64EndianSwap
#define ABCC_l64EndianSwap( lFoo )  (UINT64)( ( (UINT64)ABCC_lEndianSwap( (UINT32)( (UINT64)(lFoo) ) ) << 32 ) | (UINT64)ABCC_lEndianSwap( (UINT64)(lFoo) >> 32 ) )
#endif
#endif

/*------------------------------------------------------------------------------
** Macros for reading/writing byte to/from a 16 bit word.
**------------------------------------------------------------------------------
*/
#ifndef ABCC_iSetLSB
#define ABCC_iSetLSB( iDest, iSrc )                               \
do                                                                \
{                                                                 \
   (iDest) &= (UINT16)0xFF00;                                     \
   (iDest) |= (UINT16)( iSrc ) & (UINT16)0x00FF;                  \
}                                                                 \
while( 0 )
#endif

#ifndef ABCC_iSetMSB
#define ABCC_iSetMSB( iDest, iSrc )                               \
do                                                                \
{                                                                 \
   (iDest) &=  (UINT16)0x00FF;                                    \
   (iDest) |=  (UINT16)(iSrc) << 8;                               \
}                                                                 \
while( 0 )
#endif

#ifndef ABCC_iLSB
#define ABCC_iLSB( iFoo )           (UINT16)( (iFoo) & 0x00FF)
#endif

#ifndef ABCC_iMSB
#define ABCC_iMSB( iFoo )           (UINT16)( (UINT16)(iFoo) >> 8 )
#endif

/*------------------------------------------------------------------------------
** Endian dependent macros:
**------------------------------------------------------------------------------
** Macros to convert to/from native endian to/from specified endian:
**
** iBeTOi( iBeFoo )   - 16 bit big endian    -> native endian
** iTOiBe( iFoo )     - 16 bit native endian -> big endian
** iLeTOi( iLeFoo )   - 16 bit little endian -> native endian
** iTOiLe( iFoo )     - 16 bit native endian -> little endian
**                    -
** lBeTOl( lBeFoo )   - 32 bit big endian    -> native endian
** lTOlBe( lFoo )     - 32 bit native endian -> big endian
** lLeTOl( lLeFoo )   - 32 bit little endian -> native endian
** lTOlLe( lFoo )     - 32 bit native endian -> little endian
**                    -
** lBeTOl64( lBeFoo ) - 64 bit big endian    -> native endian
** lTOlBe64( lFoo )   - 64 bit native endian -> big endian
** lLeTOl64( lLeFoo ) - 64 bit little endian -> native endian
** lTOlLe64( lFoo )   - 64 bit native endian -> little endian
**------------------------------------------------------------------------------
** Macros to set/get low/high address octet from a word:
**
** ABCC_GetLowAddrOct( iFoo )
** ABCC_GetHighAddrOct( iFoo )
** ABCC_SetLowAddrOct( iDest, iSrc )
** ABCC_SetHighAddrOct( iDest, iSrc )
**------------------------------------------------------------------------------
*/
#ifdef ABCC_SYS_BIG_ENDIAN

   #define iBeTOi( iBeFoo )                     (UINT16)(iBeFoo)
   #define iTOiBe( iFoo )                       (UINT16)(iFoo)
   #define iLeTOi( iLeFoo )                     ABCC_iEndianSwap( iLeFoo )
   #define iTOiLe( iFoo )                       ABCC_iEndianSwap( iFoo )

   #define lBeTOl( lBeFoo )                     (UINT32)( lBeFoo )
   #define lTOlBe( lFoo )                       (UINT32)( lFoo )
   #define lLeTOl( lLeFoo )                     ABCC_lEndianSwap( lLeFoo )
   #define lTOlLe( lFoo )                       ABCC_lEndianSwap( lFoo )


   #define ABCC_GetLowAddrOct( iFoo )           ABCC_iMSB( iFoo )
   #define ABCC_GetHighAddrOct( iFoo )          ABCC_iLSB( iFoo )

   #define ABCC_SetLowAddrOct( iDest, iSrc )    ABCC_iSetMSB( iDest, iSrc )
   #define ABCC_SetHighAddrOct( iDest, iSrc )   ABCC_iSetLSB( iDest, iSrc )

#else
   #define iBeTOi( iBeFoo )                     ABCC_iEndianSwap( iBeFoo )
   #define iTOiBe( iFoo )                       ABCC_iEndianSwap( iFoo )
   #define iLeTOi( iLeFoo )                     (UINT16)(iLeFoo)
   #define iTOiLe( iFoo )                       (UINT16)(iFoo)
   #define lBeTOl( lBeFoo )                     ABCC_lEndianSwap( lBeFoo )
   #define lTOlBe( lFoo )                       ABCC_lEndianSwap( lFoo )
   #define lLeTOl( lLeFoo )                     (UINT32)( lLeFoo )
   #define lTOlLe( lFoo )                       (UINT32)( lFoo )

   #define ABCC_GetLowAddrOct( iFoo )           ABCC_iLSB( iFoo )
   #define ABCC_GetHighAddrOct( iFoo )          ABCC_iMSB( iFoo )

   #define ABCC_SetLowAddrOct( iDest, iSrc )    ABCC_iSetLSB( iDest, iSrc )
   #define ABCC_SetHighAddrOct( iDest, iSrc )   ABCC_iSetMSB( iDest, iSrc )
#endif

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
#ifdef ABCC_SYS_BIG_ENDIAN
#define lBeTOl64( lBeFoo )  (UINT64)( lBeFoo )
#define lTOlBe64( lFoo )    (UINT64)( lFoo )
#define lLeTOl64( lLeFoo )  ABCC_l64EndianSwap( lLeFoo )
#define lTOlLe64( lFoo )    ABCC_l64EndianSwap( lFoo )
#else
#define lBeTOl64( lBeFoo )  ABCC_l64EndianSwap( lBeFoo )
#define lTOlBe64( lFoo )    ABCC_l64EndianSwap( lFoo )
#define lLeTOl64( lLeFoo )  (UINT64)( lLeFoo )
#define lTOlLe64( lFoo )    (UINT64)( lFoo )
#endif
#endif

/*------------------------------------------------------------------------------
** 8/16 bit char platform dependent macros for reading ABP message type members
**------------------------------------------------------------------------------
** ABCC_GetMsgDataSize( psMsg ) - Message data size (in octets)
** ABCC_GetMsgInstance( psMsg ) - Message instance
** ABCC_GetMsgSourceId( psMsg ) - Message source id
** ABCC_GetMsgDestObj( psMsg )  - Destination object
** ABCC_IsCmdMsg( psMsg )       - Message command bit
** ABCC_GetMsgCmdBits( psMsg )  - Message command
** ABCC_GetMsgCmdExt0( psMsg )  - Command extension 0
** ABCC_GetMsgCmdExt1( psMsg )  - Command extension 1
** ABCC_GetMsgCmdExt( psMsg )   - Get extension 0 and 1 16 bit type
** ABCC_SetMsgCmdExt( psMsg )   - Set extension 0 and 1 16 bit type
**------------------------------------------------------------------------------
*/

#define ABCC_GetMsgDataSize( psMsg )   ( iLeTOi( (psMsg)->sHeader.iDataSize ) )
#define ABCC_GetMsgInstance( psMsg )   ( iLeTOi( (psMsg)->sHeader.iInstance ) )

#ifdef ABCC_SYS_16_BIT_CHAR
#define ABCC_GetMsgSourceId( psMsg )   ( ABCC_GetLowAddrOct( (psMsg)->sHeader.iSourceIdDestObj ) )
#define ABCC_GetMsgDestObj( psMsg )    ( ABCC_GetHighAddrOct( (psMsg)->sHeader.iSourceIdDestObj ) )
#define ABCC_IsCmdMsg( psMsg )         ( ABCC_GetLowAddrOct( (psMsg)->sHeader.iCmdReserved ) & ABP_MSG_HEADER_C_BIT )
#define ABCC_GetMsgCmdBits( psMsg )    ( ABCC_GetLowAddrOct( (psMsg)->sHeader.iCmdReserved ) & ABP_MSG_HEADER_CMD_BITS )
#define ABCC_GetMsgCmdExt0( psMsg )    ( ABCC_GetLowAddrOct( (psMsg)->sHeader.iCmdExt0CmdExt1)  )
#define ABCC_GetMsgCmdExt1( psMsg )    ( ABCC_GetHighAddrOct( (psMsg)->sHeader.iCmdExt0CmdExt1)  )
#define ABCC_GetMsgCmdExt( psMsg )     ( iLeTOi( (psMsg)->sHeader.iCmdExt0CmdExt1 ) )
#define ABCC_SetMsgCmdExt( psMsg, iExt ) ( (psMsg)->sHeader.iCmdExt0CmdExt1 = iTOiLe( iExt ) )
#define ABCC_GetMsgDataPtr( psMsg )    ( (psMsg)->aiData )
#define ABCC_GetErrorCode( psMsg )     ( ABCC_GetLowAddrOct( (psMsg)->aiData[ 0 ] ) )
#else
#define ABCC_GetMsgSourceId( psMsg )    ( (psMsg)->sHeader.bSourceId )
#define ABCC_GetMsgDestObj( psMsg )    (psMsg)->sHeader.bDestObj
#define ABCC_IsCmdMsg( psMsg )         ( (psMsg)->sHeader.bCmd & ABP_MSG_HEADER_C_BIT )
#define ABCC_GetMsgCmdBits( psMsg )    ( (psMsg)->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS )
#define ABCC_GetMsgCmdExt0( psMsg )    ( (psMsg)->sHeader.bCmdExt0 )
#define ABCC_GetMsgCmdExt1( psMsg )    ( (psMsg)->sHeader.bCmdExt1 )
#define ABCC_GetMsgCmdExt( psMsg )     ( iLeTOi( ((ABP_MsgHeaderType16*)(psMsg))->iCmdExt0CmdExt1 ) )
#define ABCC_SetMsgCmdExt( psMsg, iExt ) ((ABP_MsgHeaderType16*)(psMsg))->iCmdExt0CmdExt1 = iTOiLe( iExt ) )
#define ABCC_GetMsgDataPtr( psMsg )    ( (psMsg)->abData )
#define ABCC_GetErrorCode( psMsg )     ( (psMsg)->abData[ 0 ] )
#endif

/*------------------------------------------------------------------------------
** 8/16 bit char platform dependent macros to read and write message data
**------------------------------------------------------------------------------
** ABCC_SetMsgString()  - Copy native string to ABCC message
** ABCC_GetMsgString()  - Copy ABCC message string to native string
** ABCC_SetMsgData8()   - Write 8 bit data to ABCC message
** ABCC_SetMsgData16()  - Write 16 bit data to ABCC message
** ABCC_SetMsgData32()  - Write 32 bit data to ABCC message
** ABCC_GetMsgData8()   - Read 8 bit data from an ABCC message
** ABCC_GetMsgData16()  - Read 16 bit data from an ABCC message
** ABCC_GetMsgData32()  - Read 32 bit data from an ABCC message
**------------------------------------------------------------------------------
** ABCC_SetMsgString( psMsg, pcString, iNumChar, iOctetOffset )
**    psMsg - Pointer to message
**    pcString - String to be written
**    iNumChar - Number of chars in the string
**    iOctetOffset - Offset to where the string shall be written.
**
** ABCC_GetMsgString( psMsg, pcString, iNumChar, iOctetOffset )
**    psMsg - Pointer to message
**    pcString - String to be written
**    iNumChar - Number of chars in the string
**    iOctetOffset - Offset to where the string shall be read.
**
** ABCC_SetMsgDataX( psMsg, Data, iOctetOffset )
**    psMsg - Pointer to message
**    Data  - Data to be set
**    iOctetOffset - Offset to where data shall be written.
**
** ABCC_GetMsgDataX( psMsg, Data, iOctetOffset )
**    psMsg - Pointer to message
**    Data  - Read data variable
**    iOctetOffset - Offset to where data shall be read.
**------------------------------------------------------------------------------
*/
void ABCC_SetMsgString( ABP_MsgType* psMsg, const char* pcString, UINT16 iNumChar, UINT16 iOctetOffset );
void ABCC_GetMsgString( ABP_MsgType* psMsg, char* pcString, UINT16 iNumChar, UINT16 iOctetOffset );

void ABCC_GetMsgData8( ABP_MsgType* psMsg, UINT8* pbData, UINT16 iOctetOffset );
void ABCC_SetMsgData8( ABP_MsgType* psMsg, UINT8 bData, UINT16 iOctetOffset );

void ABCC_GetMsgData16( ABP_MsgType* psMsg, UINT16* piData, UINT16 iOctetOffset );
void ABCC_SetMsgData16( ABP_MsgType* psMsg, UINT16 iData, UINT16 iOctetOffset );

void ABCC_GetMsgData32( ABP_MsgType* psMsg, UINT32* plData, UINT16 iOctetOffset );
void ABCC_SetMsgData32( ABP_MsgType* psMsg, UINT32 lData, UINT16 iOctetOffset );

#if( ABCC_CFG_64BIT_ADI_SUPPORT )
UINT64 ABCC_GetMsgData64( ABP_MsgType* psMsg, UINT16 iOctetOffset );
void ABCC_SetMsgData64( ABP_MsgType* psMsg, UINT64 lData, UINT16 iOctetOffset );
#endif

/*------------------------------------------------------------------------------
** Copy a number of bytes, from the source pointer to the destination pointer.
** This function can be modified to use performance enhancing platform specific
** instructions. The default implementation is memcpy().
**------------------------------------------------------------------------------
** Arguments:
**    pbDestinationPtr - Pointer to the destination.
**    pbSourcePtr      - Pointer to source data.
**    iNbrOfBytes      - The number of bytes that shall be copied.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#define ABCC_MemCpy( pbDestinationPtr, pbSourcePtr, iNbrOfBytes )             \
        ABCC_PORT_MemCpy( pbDestinationPtr, pbSourcePtr, iNbrOfBytes )

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Called when an error is detected. Used by the following support macros:
** ABCC_ASSERT()
** ABCC_ASSERT_ERR()
** ABCC_ERROR()
**
** The function will log and report error to application and take necessary
** action depending on severity. More human readable information will be
** available if ABCC_CFG_DEBUG_ERR_ENABLED is set to TRUE (see ABCC_ErrInfoType)
** but this will have significant impact on the code size.
**------------------------------------------------------------------------------
** Arguments:
**    eSeverity         - Severity (see ABCC_SeverityType)
**    eErrorCode        - Error code (see ABCC_ErrorCodeType)
**    lAddInfo          - Additional info relevant for error
**    pacSeverity       - Pointer to severity in text format
**    pacErrorCode      - Pointer to error code in text format
**    pacAddInfo        - Pointer to additional info in text format
**    pacLoaction       - Pointer to file and line info in text format
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if ABCC_CFG_DEBUG_ERR_ENABLED
void ABCC_ErrorHandler( ABCC_SeverityType eSeverity,
                        ABCC_ErrorCodeType eErrorCode,
                        UINT32 lAddInfo,
                        char* pacSeverity,
                        char* pacErrorCode,
                        char* pacAddInfo,
                        char* pacLocation);
#else
void ABCC_ErrorHandler( ABCC_SeverityType eSeverity,
                        ABCC_ErrorCodeType eErrorCode,
                        UINT32 lAddInfo );
#endif

/*
** Create constant strings for file and line information.
** Used by error macros
*/
#define STR( x )        #x
#define XSTR( x )       STR( x )
#define FileLine        "File: " __FILE__" (Line:" XSTR( __LINE__ )")"

/*
** Call ABCC_ErrorHandler with different arguments depending on
** ABCC_CFG_DEBUG_ERR_ENABLED.
*/
#if ABCC_CFG_ERR_REPORTING_ENABLED
#if ABCC_CFG_DEBUG_ERR_ENABLED
#define ABCC_ERROR_HANDLER( eSeverity, eErrorCode, lAddInfo )                  \
        ABCC_ErrorHandler( eSeverity, eErrorCode, lAddInfo,                    \
                           #eSeverity, #eErrorCode, #lAddInfo, FileLine)
#else
#define ABCC_ERROR_HANDLER( eSeverity, eErrorCode, lAddInfo )                  \
        ABCC_ErrorHandler( eSeverity, eErrorCode, lAddInfo )
#endif
#else
#define ABCC_ERROR_HANDLER( eSeverity, eErrorCode, lAddInfo )
#endif

/*------------------------------------------------------------------------------
** Assert boolean expression. ABCC_SEV_FATAL will be reported if assertion
** fails.
**------------------------------------------------------------------------------
** Arguments:
**    x                 - Boolean expression to be asserted.
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#define ABCC_ASSERT( x )                                                       \
   if ( !( x ) )                                                               \
   {                                                                           \
      ABCC_ERROR_HANDLER( ABCC_SEV_FATAL, ABCC_EC_INTERNAL_ERROR, 0 );         \
   }

/*------------------------------------------------------------------------------
** Assert boolean expression with user defined error information if the
** assertion fails.
**------------------------------------------------------------------------------
** Arguments:
**    x                 - Boolean expression to be asserted.
**    eSeverity         - Severity (see ABCC_SeverityType)
**    eErrorCode        - Error code (see ABCC_ErrorCodeType)
**    lAddInfo          - Additional info relevant for error
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#define ABCC_ASSERT_ERR(x, eSeverity, eErrorCode, lAddInfo )                   \
   if ( !( x ) )                                                               \
   {                                                                           \
      ABCC_ERROR_HANDLER( eSeverity, eErrorCode, lAddInfo );                   \
   }

/*------------------------------------------------------------------------------
** Report error with user defined error information.
**------------------------------------------------------------------------------
** Arguments:
**    eSeverity         - Severity (see ABCC_SeverityType)
**    eErrorCode        - Error code (see ABCC_ErrorCodeType)
**    lAddInfo          - Additional info relevant for error
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#define ABCC_ERROR( eSeverity, eErrorCode, lAddInfo )                          \
        ABCC_ERROR_HANDLER( eSeverity, eErrorCode, lAddInfo )

/*------------------------------------------------------------------------------
** Debug user defined error information. Set ABCC_CFG_DEBUG_ERR_ENABLED to
** enable debug print.
**------------------------------------------------------------------------------
** Arguments:
**    x                 - Boolean expression to be asserted.
**    eSeverity         - Severity (see ABCC_SeverityType)
**    eErrorCode        - Error code (see ABCC_ErrorCodeType)
**    lAddInfo          - Additional info relevant for error
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if ABCC_CFG_DEBUG_ERR_ENABLED
#define ABCC_DEBUG_ERR  ABCC_PORT_DebugPrint
#else
#define ABCC_DEBUG_ERR( args )
#endif

/*------------------------------------------------------------------------------
** Get the error log for errors that have occurred so far. The maximum
** number of errors is controlled by ABCC_DRV_CFG_MAX_NUM_ERR_LOG. If the log is
** full the the last error will be overwritten i.e. the last error in the array
** will always be the latest error.
** More human readable information will be available if
** ABCC_CFG_DEBUG_ERR_ENABLED is set to TRUE (see ABCC_ErrInfoType) but this
** will have significant impact on the code size.
**------------------------------------------------------------------------------
** Arguments:
**    ppacErrInfo       - Pointer to array of ABCC_ErrInfoType
** Returns:
**    Number of errors in array.
**------------------------------------------------------------------------------
*/
UINT8 ABCC_GetDriverError( ABCC_ErrInfoType** ppacErrInfo );

/*------------------------------------------------------------------------------
** This function is used to measure sync timings. ABCC_CFG_SYNC_MEASUREMENT_OP
** is used when measuring the output processing time and
** ABCC_CFG_SYNC_MEASUREMENT_IP is used to measure the input processing time.
**------------------------------------------------------------------------------
** Arguments:
**    None
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
EXTFUNC void ABCC_GpioReset( void );
#endif

/*------------------------------------------------------------------------------
** This function is used to measure sync timings. ABCC_CFG_SYNC_MEASUREMENT_OP
** is used when measuring the output processing time and
** ABCC_CFG_SYNC_MEASUREMENT_IP is used to measure the input processing time.
**------------------------------------------------------------------------------
** Arguments:
**    None
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
EXTFUNC void ABCC_GpioSet( void );
#endif

/*------------------------------------------------------------------------------
** This function will initiate the hardware required to communicate with the
** ABCC. This interface shall be called once during the power up initialization.
** The driver can be restarted without calling this interface again.
**------------------------------------------------------------------------------
** Arguments:
**    None
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_HwInit( void );

/*------------------------------------------------------------------------------
** This function will initiate the driver, enable interrupt, and set the
** operation mode. If a firmware update is pending a delay, iMaxStartupTime, can
** be defined describing how long the driver is to wait for the startup
** interrupt. iMaxStartupTime set to Zero (0) makes the driver use the
** ABCC_CFG_STARTUP_TIME_MS time.
** When this function has been called the timer system could be started,
** see ABCC_RunTimerSystem().
** Note! This function will NOT release the reset of the ABCC.
** To release reset, ABCC_HwReleaseReset() has to be called by the application.
**------------------------------------------------------------------------------
** Arguments:
**    lMaxStartupTimeMs -    Max startup time for ABCC
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_StartDriver( UINT32 lMaxStartupTimeMs );

/*------------------------------------------------------------------------------
** Stops the driver and puts it into SHUTDOWN state. The ABCC will be reset.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_ShutdownDriver( void );

/*------------------------------------------------------------------------------
** This function shall be polled after ABCC_StartDriver() has been executed
** until ABCC_READY_FOR_COMMUNICATION is returned. This indicates that the ABCC
** is ready for communication and the ABCC setup sequence is started.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    ABCC_CommunicationStateType:
**    ( see description of ABCC_CommunicationStateType )
**
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_CommunicationStateType ABCC_isReadyForCommunication( void );

/*------------------------------------------------------------------------------
** This function should be called from inside the ABCC interrupt routine to
** acknowledge and handle received ABCC events (Triggered by IRQ_n on the
** abcc application interface).
** The user configuration defines, ABCC_CFG_INT_ENABLE_MASK and
** ABCC_CFG_HANDLE_IN_ABCC_ISR_MASK, allows configuration of which events to
** handle by the ISR and which events to pass on to the application
** (ABCC_CbfEvent()).
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *ABCC_ISR )( void );

/*------------------------------------------------------------------------------
** This function is responsible for handling all timers for the ABCC-driver. It
** is recommended to call this function on a regular basis from a timer
** interrupt. Without this function no timeout and watchdog functionality will
** work. This function can be called after ABCC_StartDriver() has been called.
**------------------------------------------------------------------------------
** Arguments:
**    iDeltaTimeMs - Milliseconds since last call.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_RunTimerSystem( const INT16 iDeltaTimeMs );

/*------------------------------------------------------------------------------
** ABCC hardware reset.
** Note! This function will only set reset pin to low. It the responsibility of
** the caller to make sure that the reset time is long enough.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_HWReset( void );

/*------------------------------------------------------------------------------
** Releases the ABCC reset.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_HWReleaseReset( void );

/*------------------------------------------------------------------------------
** This function drives the ABCC driver sending and receiving mechanism.
** The driver must be ready for communication before this function is called
** (ABCC_isReadyForCommunication() must be TRUE). This function could be called
** cyclically or be based on events from the ABCC. If all events are handled in
** the interrupt context then there is no need to call this function.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType (*ABCC_RunDriver)( void );

/*------------------------------------------------------------------------------
** This function should be called by the application when the last response from
** the user specific setup has been received. This will end the ABCC setup
** sequence and ABCC_SETUP_COMPLETE will be sent. The user specific setup is a
** part of the ABCC setup sequence and it is initiated by the driver by calling
** the ABCC_CbfUserInitReq() function.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_UserInitComplete( void );

/*------------------------------------------------------------------------------
** Sends a command message to the ABCC.
** The function ABCC_GetCmdMsgBuffer() must be used to allocate the message
** buffer. It is OK to re-use a previously received response buffer
** The driver will use the sourceId to map the response to the correct response
** handler. ABCC_GetNewSourceId() could be used to provide an new source id.
** Example where ABCC_CbfMessageReceived() function is used as response handler:
**
** eResp = ABCC_SendCmdMsg( psMsg, ABCC_CbfMessageReceived );
**------------------------------------------------------------------------------
** Arguments:
**    psCmdMsg     - Pointer to the command message.
**    pnMsgHandler - Pointer to the function to handle the response
**                   message.
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_SendCmdMsg( ABP_MsgType* psCmdMsg,
                                            ABCC_MsgHandlerFuncType pnMsgHandler );

/*------------------------------------------------------------------------------
** Retrieves the number of entries left in the command queue.
** Note! When sending a message the returned status must always be checked to
** verify that the message has in fact been sent.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Number of queue entries left in the command queue
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_GetCmdQueueSize( void );

/*------------------------------------------------------------------------------
** Sends a response message to the ABCC.
** Note! The received command buffer can be reused as a response buffer. If a
** new buffer is used then the function ABCC_GetCmdMsgBuffer() must be used to
** allocate the buffer.
**------------------------------------------------------------------------------
** Arguments:
**    psMsgResp - Pointer to the message.
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_SendRespMsg( ABP_MsgType* psMsgResp );

/*------------------------------------------------------------------------------
** Sends a remap response to the ABCC. When the response is sent the new process
** data sizes will be set and the function ABCC_CbfRemapDone() will be called to
** indicate the change.
**------------------------------------------------------------------------------
** Arguments:
**    psMsgResp       - Pointer to the response message.
**    iNewReadPdSize  - RdPd size when the remap is done.
**    iNewWritePdSize - WrPd size when the remap is done.
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )
EXTFUNC ABCC_ErrorCodeType ABCC_SendRemapRespMsg( ABP_MsgType* psMsgResp,
                                                  UINT16 iNewReadPdSize,
                                                  const UINT16 iNewWritePdSize );
#endif

/*------------------------------------------------------------------------------
** Get the current application status.
** Note! This information is only supported in SPI and parallel operating mode.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    The application status of the ABCC
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_AppStatusType ABCC_GetAppStatus( void );

/*------------------------------------------------------------------------------
** Sets the current application status.
** Note! This information is only supported in SPI and parallel operating mode.
** When used for other operating modes the call has no effect.
**------------------------------------------------------------------------------
** Arguments:
**    eAppStatus        - Current application status
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetAppStatus( ABP_AppStatusType eAppStatus  );

/*------------------------------------------------------------------------------
** Retrieves a message buffer. This function MUST be used when allocating
** message buffers. The size of the buffer is controlled by
** ABCC_CFG_MAX_MSG_SIZE.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    ABP_MsgType* - Pointer to the message buffer.
**                   NULL is returned if no resource is available.
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ABCC_GetCmdMsgBuffer( void );

/*------------------------------------------------------------------------------
** Returns the message buffer to the driver's message pool.
** Note! This function may only be used in combination with
** ABCC_TakeMsgBufferOwnership().
**------------------------------------------------------------------------------
** Arguments:
**    ppsBuffer - Pointer to the message buffer that was freed.
**                The buffer pointer will be set to NULL.
**
** Returns:
**    ABCC_ErrorCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_ReturnMsgBuffer( ABP_MsgType** ppsBuffer );

/*------------------------------------------------------------------------------
** Takes the ownership of the message buffer. The driver will not free this
** buffer when returning from e.g. a response callback. It is the user's
** responsibility to free this buffer when it is not needed anymore by using
** ABCC_ReturnMsgBuffer().
**------------------------------------------------------------------------------
** Arguments:
**   psMsg - Pointer to the message buffer to take ownership of
**
** Returns:
**    ABCC_StatusType
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_TakeMsgBufferOwnership( ABP_MsgType* psMsg );

/*------------------------------------------------------------------------------
** Reads the module ID.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    ModuleId
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_ReadModuleId( void );

/*------------------------------------------------------------------------------
** Detects if a module is present. If the ABCC Module detect pins on the host
** connector is not connected (ABCC_CFG_MOD_DETECT_PINS_CONN shall be defined)
** this interface will always return TRUE.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE  - Module detected.
**    FALSE - No module detected
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_ModuleDetect( void );

/*------------------------------------------------------------------------------
** Reads the module capability. This function is only supported by the ABCC40
** parallel operating mode.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Module capability
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL )
EXTFUNC UINT16 ABCC_ModCap( void );
#endif

/*------------------------------------------------------------------------------
** Reads the LED status. Only supported in SPI and parallel operating mode.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    LED status according to the software design guide.
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL || ABCC_CFG_DRV_SPI )
EXTFUNC UINT16 ABCC_LedStatus( void );
#endif

/*------------------------------------------------------------------------------
** Reads the current Anybus state.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Anybus state
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_AnbState( void);

/*------------------------------------------------------------------------------
** Returns the current status of the supervision bit.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE  - Is supervised by another network device.
**    FALSE - Not supervised.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_IsSupervised( void );

/*------------------------------------------------------------------------------
** Retrieves the network type.
** This function will return a valid value after ABCC_CbfAdiMappingReq has been
** called by the driver. If called earlier the function will return 0xFFFF which
** indicates that the network is unknown. The different newtwork types could
** be found in abp.h.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Network type (0xFFFF if the network is unknown)
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_NetworkType( void );

/*------------------------------------------------------------------------------
** Retrieves the module type.
** This function will return a valid value after ABCC_CbfAdiMappingReq has been
** called by the driver. If called earlier the function will return 0xFFFF which
** indicates that the moduleType is unknown. The different module types types
** could be found in abp.h.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Module type (0x04XX for Anybus-CC modules).
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_ModuleType( void );

/*------------------------------------------------------------------------------
** Retrieves the network format.
** This function will return a valid value after ABCC_CbfAdiMappingReq has been
** called by the driver. If called earlier the function will return NET_UNKNOWN.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Network format type (NET_LITTLEENDIAN, NET_BIGENDIAN).
**------------------------------------------------------------------------------
*/
EXTFUNC NetFormatType ABCC_NetFormatType( void );

/*------------------------------------------------------------------------------
** Retrieves the parameter support.
** This function will return a valid value after ABCC_CbfAdiMappingReq has been
** called by the driver. If called earlier PARAMETR_UNKNOWN will be returned.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    ParamemterSupportType
**------------------------------------------------------------------------------
*/
EXTFUNC ParameterSupportType ABCC_ParameterSupport( void );

/*------------------------------------------------------------------------------
** This function will call ABCC_SYS_GetOpmode() to read the operating mode from
** HW. If the operation is known and fixed or in any other way decided by the
** application this function could be ignored.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    The ABCC40 4 bit operating mode according to abp.h
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_GetOpmode( void );

/*******************************************************************************
** Callback Functions.
** These function must be implemented by the application. The context of the
** callback will differ depending on implementation.
** If, for example, the read process data is chosen to be interrupt driven and
** the message handling chosen to be polled ( see ABCC_CFG_INT_ENABLE_MASK and
** HANDLE_ABCC_CFG_IN_ABCC_ISR in ABCC_CFG_def.h ), the  ABCC_CbfNewReadPd()
** will be called from interrupt context and ABCC_CbfReceiveMsg() will be called
** from the same context as ABCC_RunDriver().
********************************************************************************
*/

/*------------------------------------------------------------------------------
** This function is called from ABCC_ISR() when events specified in
** ABCC_CFG_INT_ENABLE_MASK_X have occurred. The function returns a mask of
** ABCC_ISR_EVENT_X bits with the currently active events that has not already
** been handled by the ISR itself. What interrupt to be handled by the ISR is
** defined in the ABCC_CFG_HANDLE_INT_IN_ISR_MASK.
** This function is always called from interrupt context.
**------------------------------------------------------------------------------
** Arguments:
**    iEvents - Mask according to the ISR event bits ABCC_ISR_EVENT_X
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfEvent( UINT16 iEvents);

/*------------------------------------------------------------------------------
** If sync is supported this function will be invoked at the sync event.
** The function is executed in interrupt context. If the separate sync pin in
** the ABCC interface is used this function shall be called from the interrupt
** handler. If the ABCC interrupt is used the driver will call this function.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if ABCC_CFG_SYNC_ENABLE
EXTFUNC void ABCC_CbfSyncIsr( void );
#endif


/*------------------------------------------------------------------------------
** This function needs to be implemented by the application. The function is
** called to trigger a user specific setup during the ABCC setup state. The ABCC
** driver will remain in ABCC_CFG_INIT state until ABCC_UserInitComplete() is called
** by the application.  If no user specific setup is required,
** ABCC_UserInitComplete() must be called inside this function otherwise setup
** complete will never be sent.
**
** This function call will be invoked in same context as the read message handling.
** (See comment for callback section above)
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfUserInitReq( void );

/*------------------------------------------------------------------------------
** A message has been received from the ABCC. This is the receive function for
** all received commands from the ABCC. It could also be used as a response
** handler if passed on as an argument to the ABCC_SendCmdMsg() function.
** Regarding callback context, see comment for callback section above.
**------------------------------------------------------------------------------
** Arguments:
**    psReceivedMsg       - Pointer to received message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfReceiveMsg( ABP_MsgType* psReceivedMsg );

/*------------------------------------------------------------------------------
** This function needs to be implemented by the application. The function
** updates the current write process data. The data must be copied into the
** buffer before returning from the function.
** The data will only be sent to the ABCC if the return value is TRUE.
** Regarding callback context, see comment for callback section above.
**------------------------------------------------------------------------------
** Arguments:
**    pxWritePd - Pointer to the process data to be sent.
**
** Returns:
**    TRUE  - If the process data has been changed since last call.
**    FALSE - Process data not changed.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_CbfUpdateWriteProcessData( void* pxWritePd );

/*------------------------------------------------------------------------------
** This function needs to be implemented by the application. The function is
** called when new process data has been received. The process data needs to
** be copied to the application ADI:s before returning from the function. Note
** that the data may not be changed since last time.
** Regarding callback context, see comment for callback section above.
**------------------------------------------------------------------------------
** Arguments:
**    pxReadPd - Pointer to the received process data.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfNewReadPd( void* pxReadPd );

/*------------------------------------------------------------------------------
** This function needs to be implemented by the application. The function is
** called when communication with the ABCC module has been lost. The watchdog
** timeout is defined by ABCC_CFG_WD_TIMEOUT_MS.
** Note! No watch functionality is provided for parallel 8/16 bit operation
** mode.
** This function is invoked in the same context as ABCC_RunTimerSystem().
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_SERIAL ||           \
     ABCC_CFG_DRV_PARALLEL_30 )
EXTFUNC void ABCC_CbfWdTimeout( void );
#endif

/*------------------------------------------------------------------------------
** This function needs to be implemented by the application. The function
** indicates that we recently had an ABCC watchdog timeout but now the
** communication is working again.
** This function will be invoked from same context as the receive handling.
** (see comment for callback section above).
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if(  ABCC_CFG_DRV_SPI || ABCC_CFG_DRV_SERIAL ||           \
      ABCC_CFG_DRV_PARALLEL_30 )
EXTFUNC void ABCC_CbfWdTimeoutRecovered( void );
#endif

/*------------------------------------------------------------------------------
** This function need to be implemented by the application. The function is
** called when the driver is about to start the automatic process data mapping.
** If no automatic configuration is desired then the pointers are set to NULL.
** Otherwise the pointers are set to point at the structures containing mapping
** information. The mapping structures are defined in abcc_ad_if.h.
** This function will be invoked in same context as the read message handling.
** (See comment for callback section above)
**------------------------------------------------------------------------------
** Arguments:
**    ppsAdiEntry   - Pointer to the requested configuration structure pointer.
**    ppsDefaultMap - Pointer to default mapping table.
**
** Returns:
**    Number of Adi:s in the psAdiEntry table.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_CbfAdiMappingReq( const AD_AdiEntryType** const ppsAdiEntry,
                                      const AD_DefaultMapType**  const ppsDefaultMap );

/*------------------------------------------------------------------------------
** This function needs to be implemented by the application.
** The context of the call is depending on where the error has occured.
**------------------------------------------------------------------------------
** Arguments:
**    eSeverity  - Severity of the event (see ABCC_SeverityType).
**    iErrorCode - Error code.
**    lAddInfo   - Depending on error different additional information can be
**                 added.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfDriverError(  ABCC_SeverityType eSeverity,
                                   ABCC_ErrorCodeType  iErrorCode,
                                   UINT32  lAddInfo );

/*------------------------------------------------------------------------------
** This callback is invoked if the anybus changes state
** See ABP_AnbStateType in abp.h for more information.
**
** Regarding callback context, see comment for callback section above.
**------------------------------------------------------------------------------
** Arguments:
**    bNewAnbState   - New anybus state
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CbfAnbStateChanged( ABP_AnbStateType bNewAnbState );

/*******************************************************************************
** REMAP Related functions
********************************************************************************
*/

/*------------------------------------------------------------------------------
** This callback is invoked when REMAP response is successfully sent to the
** ABCC.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )
EXTFUNC void ABCC_CbfRemapDone( void );
#endif

/*******************************************************************************
** Event related functions
********************************************************************************
*/

/*------------------------------------------------------------------------------
** This function is called to trigger a RdPd read. If the read process data is
** available then a call to the function ABCC_CbfNewReadPd() will be triggered.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_TriggerRdPdUpdate( void );

/*------------------------------------------------------------------------------
** This function is called to trigger a message receive read. If a read message
** is available then the corresponding message handler will be called.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_TriggerReceiveMessage( void );

/*------------------------------------------------------------------------------
** This function indicates that new process data from the application is
** available and will be sent to the ABCC.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void (*ABCC_TriggerWrPdUpdate)( void );

/*------------------------------------------------------------------------------
** Check if current anybus status has changed.
** If the status  is changed  ABCC_CbfAnbStatusChanged() will be invoked.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_TriggerAnbStatusUpdate( void );

/*------------------------------------------------------------------------------
** Checks if there are any messages to send.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_TriggerTransmitMessage( void );

/*******************************************************************************
** Message support functions
********************************************************************************
*/

/*------------------------------------------------------------------------------
** This function fills an ABCC message with parameters to get an attribute.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg      - Pointer to message buffer.
**    bObject    - Object number.
**    iInstance  - Instance number.
**    bAttribute - Attribute number.
**    bSourceId  - Source identifier
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_GetAttribute( ABP_MsgType* psMsg,
                                UINT8 bObject,
                                UINT16 iInstance,
                                UINT8 bAttribute,
                                UINT8 bSourceId );

/*------------------------------------------------------------------------------
** This function fills an ABCC message with parameters in order to set an
** attribute.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg      - Pointer to message buffer.
**    bObject    - Object number.
**    iInstance  - Instance number.
**    bAttribute - Attribute number.
**    bVal       - Value to set.
**    bSourceId  - Source identifier.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetByteAttribute( ABP_MsgType* psMsg,
                                    UINT8 bObject,
                                    UINT16 iInstance,
                                    UINT8 bAttribute,
                                    UINT8 bVal,
                                    UINT8 bSourceId );

/*------------------------------------------------------------------------------
** This function sets the input arguments to the ABCC message header correctly.
** The data must be copied to message data buffer separately.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg      - Pointer to message buffer.
**    bObject    - Object number.
**    iInstance  - Instance number.
**    bAttribute - Attribute number.
**    eService   - Message command
**    iDataSize  - Size of the message data in bytes
**    bSourceId  - Source identifier.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetMsgHeader( ABP_MsgType* psMsg,
                                UINT8 bObject,
                                UINT16 iInstance,
                                UINT8 bAttribute,
                                ABP_MsgCmdType eService,
                                UINT16 iDataSize,
                                UINT8 bSourceId );

/*------------------------------------------------------------------------------
** This function verifies an ABCC response message.
**------------------------------------------------------------------------------
** Arguments:
**    psMsg - Pointer to message buffer.
**
** Returns:
**    ABCC_ErrorCodeType.
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_ErrorCodeType ABCC_VerifyMessage( const ABP_MsgType* psMsg );

/*------------------------------------------------------------------------------
** This function returns a new source id that could used when sending a command
** message. It is guaranteed be unique if this function is used every time a new
** command is sent. The alternative would be that the user uses fixed source
** id:s.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    New SourceId
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_GetNewSourceId( void );

/*------------------------------------------------------------------------------
** This function returns the size of an ABP data type.
**------------------------------------------------------------------------------
** Arguments:
**    bDataType - Data type number.
**
** Returns:
**    Data type size in bytes.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_GetDataTypeSize( UINT8 bDataType );

/*------------------------------------------------------------------------------
** This function returns the size of an ABP data type in bits.
**------------------------------------------------------------------------------
** Arguments:
**    bDataType - Data type number.
**
** Returns:
**    Data type size in bits.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_GetDataTypeSizeInBits( UINT8 bDataType );

#endif  /* inclusion lock */
