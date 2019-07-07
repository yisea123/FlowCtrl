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
**  Target dependent interface for parallel 8 / 16 operation mode.
********************************************************************************
********************************************************************************
** Services:
**    ABCC_SYS_ParallelRead()          - Reads an amount of bytes from the
**                                       ABCC
**    ABCC_SYS_ParallelRead8()         - Reads a byte from the ABCC
**    ABCC_SYS_ParallelRead16()        - Reads a word from the ABCC
**    ABCC_SYS_ParallelWrite()         - Writes an amount of bytes
**    ABCC_SYS_ParallelWrite8()        - Writes a byte to the ABCC
**    ABCC_SYS_ParallelWrite16()       - Writes a word to the ABCC
**    ABCC_SYS_ParallelGetRdPdBuffer() - Get the address to the received read
**                                       process data.
**    ABCC_SYS_ParallelGetWrPdBuffer() - Get the address to the write process
**                                       data.
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_SYS_ADAPT_PARI_
#define ABCC_SYS_ADAPT_PARI_
#include "abcc_drv_cfg.h"
#include "abcc_td.h"
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
** Reads an amount of bytes from the ABCC memory. Implementation is not needed
** for a memory mapped system.
** This function/macro will be used by the driver when reading process data or
** message data from the ABCC memory.
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
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC void ABCC_SYS_ParallelRead( UINT16 iMemOffset, void* pxData, UINT16 iLength );
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
**    Read UINT8s
**------------------------------------------------------------------------------
*/
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC UINT8 ABCC_SYS_ParallelRead8( UINT16 iMemOffset );
#endif

/*------------------------------------------------------------------------------
** Reads a word from the ABCC memory.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset - Offset from ABCC base address
**                 8 bit char platforms  : iMemOffset in octets
**                 16 bit char platforms : iMemOffset in 16 bit words
**
** Returns:
**    Read UINT16
**------------------------------------------------------------------------------
*/
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC UINT16 ABCC_SYS_ParallelRead16( UINT16 iMemOffset );
#endif

/*------------------------------------------------------------------------------
** Writes an amount of bytes to the ABCC memory
** This function will be used by the driver when writing process data or message
** data to the ABCC memory.
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
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC void ABCC_SYS_ParallelWrite( UINT16 iMemOffset,
                                     void* pxData,
                                     UINT16 iLength );
#endif

/*------------------------------------------------------------------------------
** Writes a byte to the ABCC memory.
**------------------------------------------------------------------------------
** Arguments:
**    iMemOffset - Offset from ABCC base address.
**                 8 bit char platforms  : iMemOffset in octets
**                 16 bit char platforms : iMemOffset in 16 bit words
**     pbData    - Data to be written to ABCC
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC void ABCC_SYS_ParallelWrite8( UINT16 iMemOffset, UINT8 pbData );
#endif

/*------------------------------------------------------------------------------
**  Writes a word to the ABCC memory.
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
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
EXTFUNC void ABCC_SYS_ParallelWrite16( UINT16 iMemOffset, UINT16 iData );
#endif

/*------------------------------------------------------------------------------
** Get the address to the received read process data.
** For a non memory mapped system the system adaption layer need to provide a
** buffer where the read process data can be stored.
**------------------------------------------------------------------------------
** Argument:
**    None
**
** Returns:
**    Address to RdPdBuffer.
**
**------------------------------------------------------------------------------
*/
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
   EXTFUNC void* ABCC_SYS_ParallelGetRdPdBuffer( void );
#endif

/*------------------------------------------------------------------------------
** Get the address to store the write process data.
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
#if( !ABCC_CFG_MEMORY_MAPPED_ACCESS )
   EXTFUNC void* ABCC_SYS_ParallelGetWrPdBuffer( void );
#endif

#endif  /* inclusion lock */
