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
** permission. When used together with a product from HMS, this code can be   **
** modified, reproduced and distributed in binary form without any            **
** restrictions.                                                              **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** Defines the generic driver interface implemented by each specific driver.
********************************************************************************
********************************************************************************
** Driver services:
**    pnABCC_DrvInit()                      - Initialize driver privates and
**                                            states to default values.
**    pnABCC_DrvISR()                       - Calls in the interrupt context to
**                                            acknowledge received
**                                            interrupts.
**    pnABCC_DrvRunDriverTx()               - Drives the internal send process
**                                            if applicable
**    pnABCC_DrvRunDriverRx()               - Drives the internal receive
**                                            process if applicable
**    pnABCC_DrvPrepareWriteMessage()       - Copy message data
**    pnABCC_DrvWriteMessage()              - Writes a message.
**    pnABCC_DrvWriteProcessData()          - Writes current process data.
**    pnABCC_DrvISReadyForWriteMessage()    - Checks if the driver is ready
**                                            to send a new write message.
**    pnABCC_DrvISReadyForCmd()             - Checks if the Anybus is ready to
**                                            receive a new command message.
**    pnABCC_DrvSetNbrOfCmds()              - Sets the number of simultaneous
**                                            commands that is supported by
**                                            the application.
**    pnABCC_DrvSetAppStatus()              - Sets the current application
**                                            status.
**    pnABCC_DrvSetPdSize()                 - Sets the current process data
**                                            size.
**    pnABCC_DrvSetMsgReceiverBuffer()      - Sets the message receiver buffer.
**    pnABCC_DrvSetIntMask()                - Set interrupt mask
**    pnABCC_DrvGetWrPdBuffer()             - Get wrpd buffer
**    pnABCC_DrvGetModCap()                 - Read module capability
**    pnABCC_DrvGetLedStatus()              - Read led status
**    pnABCC_DrvGetIntStatus()              - Get current interrupt status
**    pnABCC_DrvGetAnybusState()            - Get current Anybus state
**    pnABCC_DrvReadProcessData()           - Get read process data if any.
**    pnABCC_DrvReadMessage()               - Get read message if any.
**    pnABCC_DrvIsSuperviced()              - Is the network supervised
********************************************************************************
********************************************************************************
*/

#ifndef PHY_DRV_IF_H_
#define PHY_DRV_IF_H_

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abcc_debug_err.h"
#include "abp.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Reads an amount of bytes from the ABCC memory.
** This function/macro will be used by the driver when reading process data or
** message data from the ABCC memory.
** See also the ABCC_SYS_READ_RDPD macro description below.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset  - Memory offset to start writing to.
**                  8 bit char platforms  : iMemOffset in octets
**                  16 bit char platforms : iMemOffset in 16 bit words
**    pxData      - Pointer to the data to be written.
**    iLength     - The amount of data to write in octets.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvParallelRead( iMemOffset, pxData, iLength )                    \
        ABCC_PORT_MemCpy( (pxData), (void*)( ABCC_CFG_PARALLEL_BASE_ADR + (iMemOffset) ), (iLength) )
#else
#define ABCC_DrvParallelRead( iMemOffset, pxData, iLength )                    \
        ABCC_SYS_ParallelRead( iMemOffset, pxData, iLength )
#endif

/*------------------------------------------------------------------------------
** Reads a byte from the ABCC memory.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset - Offset from ABCC base address.
**                 8 bit char platforms  : iMemOffset in octets
**                 16 bit char platforms : iMemOffset in 16 bit words
**
** Returns:
**    Read UINT8
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvRead8( iMemOffset )                                            \
        *(volatile UINT8*)( ABCC_CFG_PARALLEL_BASE_ADR + ( iMemOffset ) )
#define ABCC_DrvRead16( iMemOffset )                                           \
        *(volatile UINT16*)( ABCC_CFG_PARALLEL_BASE_ADR + ( iMemOffset ) )
#else
#define ABCC_DrvRead8( iMemOffset )  ABCC_SYS_ParallelRead8( iMemOffset )
#define ABCC_DrvRead16( iMemOffset ) ABCC_SYS_ParallelRead16( iMemOffset )
#endif

/*------------------------------------------------------------------------------
** Writes a word to the ABCC memory.
** In case of a memory mapped system this function does not need not be
** implemented. See ABCC_SYS_WRITE16 macro description below.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset - Offset from ABCC base address.
**                 8 bit char platforms  : iMemOffset in octets
**                 16 bit char platforms : iMemOffset in 16 bit words
**    iData      - Data to be written to ABCC
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvParallelWrite( iMemOffset, pxData, iLength )                   \
        ABCC_PORT_MemCpy( (void*)( ABCC_CFG_PARALLEL_BASE_ADR + (iMemOffset) ), (pxData), (iLength) )
#else
#define ABCC_DrvParallelWrite( iMemOffset, pxData, iLength )                   \
        ABCC_SYS_ParallelWrite( iMemOffset, pxData, iLength )
#endif

/*------------------------------------------------------------------------------
** The driver will use the ABCC_SYS_WRITE8 and ABCC_SYS_WRITE16 macros to
** access the ABCC registers. In case of a memory mapped system,
** MEMORY_MAPPED_ACCESS is defined an direct memory access will be done
** using ABCC_CFG_PARALLEL_BASE_ADR to calculate the absolute address. In case
** of a non memory mapped system the ABCC_SYS_ParallelWrite8/16 will be called.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset - Offset from ABCC base address.
**                 8 bit char platforms  : iMemOffset in octets
**                 16 bit char platforms : iMemOffset in 16 bit words
**    pbData     - Data to be written to ABCC
**
** Returns:
**    None
**
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvWrite8( iMemOffset, pbData )                                   \
        *(volatile UINT8*)( ABCC_CFG_PARALLEL_BASE_ADR + ( iMemOffset ) ) = pbData
#define ABCC_DrvWrite16( iMemOffset, pbData )                                  \
        *(volatile UINT16*)( ABCC_CFG_PARALLEL_BASE_ADR + ( iMemOffset ) ) = pbData
#else
#define ABCC_DrvWrite8( iMemOffset, pbData )                                   \
        ABCC_SYS_ParallelWrite8( iMemOffset, pbData )
#define ABCC_DrvWrite16( iMemOffset, pbData )                                   \
        ABCC_SYS_ParallelWrite16( iMemOffset, pbData )
#endif

/*------------------------------------------------------------------------------
** Get the address to the received read process data for parallel 30 operating
** mode.
** For a non memory mapped system the system adaption layer need to provide a
** buffer where the read process data can be stored.
** No implementation is needed for a memory mapped system since the macro
** provides the information.
**------------------------------------------------------------------------------
** Argument:
**    None
**
** Returns:
**    Address to RdPdBuffer.
**
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL_30 )
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvParallel30GetRdPdBuffer()                                      \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ABP_RDPDM_ADR_OFFSET )
#else
#define ABCC_DrvParallel30GetRdPdBuffer()                                      \
        ABCC_SYS_ParallelGetRdPdBuffer()
#endif
#endif

/*------------------------------------------------------------------------------
** Get the address to the received read process data for event parallel
** operating mode.
** For a non memory mapped system the system adaption layer need to provide a
** buffer where the read process data can be stored.
** No implementation is needed for a memory mapped system since the macro
** provides the information.
**------------------------------------------------------------------------------
** Argument:
**    None
**
** Returns:
**    Address to RdPdBuffer.
**
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL )
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#ifdef ABCC_SYS_16_BIT_CHAR
#define ABCC_DrvParallelGetRdPdBuffer()                                        \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ( ABP_RDPD_ADR_OFFSET >> 1 ) )
#else
#define ABCC_DrvParallelGetRdPdBuffer()                                        \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ABP_RDPD_ADR_OFFSET )
#endif
#else
#define ABCC_DrvParallelGetRdPdBuffer()                                        \
        ABCC_SYS_ParallelGetRdPdBuffer()
#endif
#endif

/*------------------------------------------------------------------------------
** Get the address to store the write process data for parallel 30 operating
** mode.
** For a non memory mapped system the system adaption layer need to provide a
** buffer where the write process data can be stored.
** No implementation is needed for a memory mapped system since the macro
** provides the information.
**------------------------------------------------------------------------------
** Argument:
**    None
**
** Returns:
**    Address to WrPdBuffer
**
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL_30 )
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#define ABCC_DrvParallel30GetWrPdBuffer()                                      \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ABP_WRPDM_ADR_OFFSET )
#else
#define ABCC_DrvParallel30GetWrPdBuffer()   ABCC_SYS_ParallelGetWrPdBuffer()
#endif
#endif

/*------------------------------------------------------------------------------
** Get the address to store the write process data for event parallel operating
** mode.
** For a non memory mapped system the system adaption layer need to provide a
** buffer where the write process data can be stored.
** No implementation is needed for a memory mapped system since the macro
** provides the information.
**------------------------------------------------------------------------------
** Argument:
**    None
**
** Returns:
**    Address to WrPdBuffer
**
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_DRV_PARALLEL )
#if( ABCC_CFG_MEMORY_MAPPED_ACCESS )
#ifdef ABCC_SYS_16_BIT_CHAR
#define ABCC_DrvParallelGetWrPdBuffer()                                        \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ( ABP_WRPD_ADR_OFFSET >> 1 ) )
#else
#define ABCC_DrvParallelGetWrPdBuffer()                                        \
        (void*)( ABCC_CFG_PARALLEL_BASE_ADR + ABP_WRPD_ADR_OFFSET )
#endif
#else
#define ABCC_DrvParallelGetWrPdBuffer()   ABCC_SYS_ParallelGetWrPdBuffer()
#endif
#endif

/*------------------------------------------------------------------------------
** Initializes the driver to default values.
** Must be called before the driver is used.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvInit )( UINT8 bOpmode );

/*------------------------------------------------------------------------------
** Calls in the interrupt context to acknowledge received interrupts.The ISR
** routine will clear all pending interrupts.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    Acknowledged interrupts.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ( *pnABCC_DrvISR )( void );

/*------------------------------------------------------------------------------
** Drives the internal send process.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvRunDriverTx )( void );

/*------------------------------------------------------------------------------
** Drives the internal receive process.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    Pointer to successfully sent write message.
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ( *pnABCC_DrvRunDriverRx )( void );

/*------------------------------------------------------------------------------
** Copy message to the ABCC40 interface if applicable.
** The actual write trigger is done by pnABCC_DrvPrepareWriteMessage. This
** function is only implemented for the parallel operating modes. Check function
** pointer for NULL before calling.
** Note! It is only allowed to call this functions if the driver is ready to
** handle a new message. Use the following functions to ensure this:
** ABCC_DrvParIsReadyForWriteMessage()
** ABCC_DrvParIsReadyForCmd()
**------------------------------------------------------------------------------
** Arguments:
**    psWriteMsg   - Pointer to message.
**
** Returns:
**    -
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvPrepareWriteMessage) ( ABP_MsgType* psWriteMsg );

/*------------------------------------------------------------------------------
** Writes a message to the driver.
**------------------------------------------------------------------------------
** Arguments:
**    psWriteMsg   - Pointer to message.
**
** Returns:
**    True         - Message was successfully written and can be deallocated
**                   immediately.
**    False        - Message was not yet written and cannot be deallocated.
**                   The psWriteMsg pointer is owned by the driver until the
**                   message is written and the pointer is returned in the
**                   driver execution response.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ( *pnABCC_DrvWriteMessage) ( ABP_MsgType* psWriteMsg );

/*------------------------------------------------------------------------------
** Writes current process data.
** The data is copied before returning from the method.
**------------------------------------------------------------------------------
** Arguments:
**    pbProcessData - Pointer to process data to be sent.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvWriteProcessData )( void* pbProcessData );

/*------------------------------------------------------------------------------
** Checks if the driver is in the correct state for writing process data to the
** anybus
**------------------------------------------------------------------------------
** Arguments:
**       None
**
** Returns:
**       True        - Driver is in correct state to send WrPd
**       False:      - Driver is not in correct state to send Wrpd
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ( *pnABCC_DrvISReadyForWrPd )( void );

/*------------------------------------------------------------------------------
** Checks if the driver is ready to send a new write message.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    True        - Driver is ready to send a new write message.
**    False       - Driver is not ready to send a new write message.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ( *pnABCC_DrvISReadyForWriteMessage )( void );

/*------------------------------------------------------------------------------
** The host application checks if the Anybus is ready to receive a new command
** message.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    True         - OK to send new command.
**    False        - NOK to send new command.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ( *pnABCC_DrvISReadyForCmd )( void );

/*------------------------------------------------------------------------------
** Sets the number of simultaneous commands that is supported by the
** application.
**------------------------------------------------------------------------------
** Arguments:
**    bNbrOfCmds  - Number of commands that the application is ready to receive.
**
** Returns:
**       None
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvSetNbrOfCmds )( UINT8 bNbrOfCmds );

/*------------------------------------------------------------------------------
**  Sets the current application status.
**  Note! This information is not supported by all protocols.
**------------------------------------------------------------------------------
** Arguments:
**    eAppStatus   - Current application status.
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvSetAppStatus )( ABP_AppStatusType eAppStatus );

/*------------------------------------------------------------------------------
** Sets the current process data size.
**------------------------------------------------------------------------------
** Arguments:
**    iReadPdSize  - Size of read process data (bytes)
**    iWritePdSize - Size of write process data (bytes)
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvSetPdSize )( const UINT16 iReadPdSize,
                                       const UINT16 iWritePdSize );

/*------------------------------------------------------------------------------
** Sets Interrupt mask according to h_aci.h.
**------------------------------------------------------------------------------
** Arguments:
**    iIntMask     - Interrupt mask set according to h_aci.h.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ( *pnABCC_DrvSetIntMask )( const UINT16 iIntMask );

/*------------------------------------------------------------------------------
** Get WrpdBuffer for the user to update.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Pointer to WrPd buffer.
**------------------------------------------------------------------------------
*/
EXTFUNC void* ( *pnABCC_DrvGetWrPdBuffer )( void );

/*------------------------------------------------------------------------------
** Read module capabillity
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    Module capability.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ( *pnABCC_DrvGetModCap )( void );

/*------------------------------------------------------------------------------
** Read module capability
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    Module capability.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ( *pnABCC_DrvGetLedStatus )( void );

/*------------------------------------------------------------------------------
** Gets the Anybus interrupt status. The pnABCC_DrvISR() function will clear all
** pending interrupts. This function must be called before pnABCC_DrvISR() or it
** will always return 0.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    The Anybus interrupt status.
**------------------------------------------------------------------------------
*/
/*EXTFUNC UINT16 ( *pnABCC_DrvGetIntStatus )( void );*/

/*------------------------------------------------------------------------------
** Gets the Anybus state.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    The Anybus state
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ( *pnABCC_DrvGetAnybusState )( void );

/*------------------------------------------------------------------------------
** Reads the read process data.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    A pointer to the read process data; or NULL if no process data to read
**    was available.
**------------------------------------------------------------------------------
*/
EXTFUNC void* ( *pnABCC_DrvReadProcessData )( void );

/*------------------------------------------------------------------------------
** Reads the read message.
**------------------------------------------------------------------------------
** Arguments:
**    None.
**
** Returns:
**    A pointer to the read message; or NULL if no message is available.
**    The pointer, if not NULL, will point to the buffer previously set by
**    calling pnABCC_DrvSetMsgReceiverBuffer().
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ( *pnABCC_DrvReadMessage )( void );

/*------------------------------------------------------------------------------
** Returns supervision bit in status register.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE: The device is supervised by another network device.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ( *pnABCC_DrvIsSupervised )( void );

/*------------------------------------------------------------------------------
**  Returns anybus status register.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Anybus status register
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ( *pnABCC_DrvGetAnbStatus )( void );

#endif  /* inclusion lock */

/*******************************************************************************
** End of drv_if.h
********************************************************************************
*/
