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
** This program is the property of HMS Industrial Networks AB.                **
** It may not be reproduced, distributed, or used without permission          **
** of an authorized company official.                                         **
********************************************************************************
********************************************************************************
** Memory management of the driver
********************************************************************************
********************************************************************************
** Services:
** ABCC_MemCreatePool()        - Create memory resource pool
** ABCC_MemAlloc()             - Alloc memory resource
** ABCC_MemFree()              - Free memory resource
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_MEM_H_
#define ABCC_MEM_H_

#include "abcc_drv_cfg.h"
#include "abcc_td.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Buffer status used to keep track of a memory buffer's current state
**------------------------------------------------------------------------------
*/
typedef enum ABCC_MemBufferStatusType
{
   ABCC_MEM_BUFSTAT_FREE = 0,
   ABCC_MEM_BUFSTAT_ALLOCATED = 1,
   ABCC_MEM_BUFSTAT_IN_APPL_HANDLER = 2,
   ABCC_MEM_BUFSTAT_SENT = 3,
   ABCC_MEM_BUFSTAT_OWNED = 4,

   ABCC_MEM_BUFSTAT_UNKNOWN = 0x7FFF
}
ABCC_MemBufferStatusType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Creates a memory pool of buffers with a specific size.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void ABCC_MemCreatePool( void );

/*------------------------------------------------------------------------------
** Allocates and return pointer to memory of predefined size
** (ABCC_MemCreatePool)
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Pointer to allocated memory. NULL if pool is empty.
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ABCC_MemAlloc( void );

/*------------------------------------------------------------------------------
** Return memory to the pool. Note that it is important that the returned memory
** is belonging to the pool from the beginning.
**------------------------------------------------------------------------------
** Arguments:
**    pxItem       - Pointer to the memory to be returned. The pointer is set to
**                   NULL.
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_MemFree( ABP_MsgType** pxItem );

/*------------------------------------------------------------------------------
** Get the currently status of the memory buffer
**------------------------------------------------------------------------------
** Arguments:
**    psMsg - Message buffer to check the status of
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_MemBufferStatusType ABCC_MemGetBufferStatus( ABP_MsgType* psMsg );

/*------------------------------------------------------------------------------
** Set a new status to the memory buffer
**------------------------------------------------------------------------------
** Arguments:
**    psMsg   - Memory buffer to set the status to
**    eStatus - Status to be set to the message buffer
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_MemSetBufferStatus( ABP_MsgType* psMsg,
                                      ABCC_MemBufferStatusType eStatus );

#endif  /* inclusion lock */
