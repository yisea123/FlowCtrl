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
** Memory allocation implementation.
********************************************************************************
********************************************************************************
*/

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abcc.h"
#include "abcc_mem.h"
#include "abcc_sys_adapt.h"
#include "abcc_port.h"
#include "abcc_debug_err.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*
** Set default value for maximum number of resources
*/
#ifndef ABCC_CFG_MAX_NUM_MSG_RESOURCES
#define ABCC_CFG_MAX_NUM_MSG_RESOURCES          ( ABCC_CFG_MAX_NUM_APPL_CMDS + ABCC_CFG_MAX_NUM_ABCC_CMDS )
#endif

/*
** Magic cookie
*/
#define ABCC_MEM_MAGIC_COOKIE  0x5CC5

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Structure for defining size of memory message allocation
**
** The magic cookie field is used to evaluate if the buffer status field
** is broken. The buffer status could be broken if the user writes outside the
** bounds of the message data area.
**------------------------------------------------------------------------------
*/
typedef struct
{
   ABP_MsgHeaderType16 sHeader;
   UINT32   alData[ ( ABCC_CFG_MAX_MSG_SIZE + 3 ) >> 2 ];
   UINT16   iMagicCookie;
   UINT16   iBufferStatus;
}
PACKED_STRUCT ABCC_MemAllocType;

/*------------------------------------------------------------------------------
** Union used for casting between memory ABCC_MemAllocType and ABP_MsgType.
**------------------------------------------------------------------------------
*/
typedef union
{
   ABCC_MemAllocType* psAllocMsg;
   ABP_MsgType* psMsg;
}
ABCC_MemAllocUnion;

/*------------------------------------------------------------------------------
** Memory pool structure
**
** ------------------
** iNumFreeMsg = 3  |
** ------------------
** Msg 0 pointer    |---|
** ------------------   |
** Msg 1 pointer    |---+--|
** ------------------   |  |
** Msg 2 pointer    |---+--+--|
** ------------------   |  |  |
** Msg 0           |<--|  |  |
** ------------------      |  |
** Msg 1           |<-----|  |
** ------------------         |
** Msg 2           |<--------|
** ------------------
**------------------------------------------------------------------------------
*/
static UINT16 abcc_iNumFreeMsg;
static ABCC_MemAllocUnion abcc_uFreeMsgStack[ ABCC_CFG_MAX_NUM_MSG_RESOURCES ];
static ABCC_MemAllocType  abcc_asMsgPool[ ABCC_CFG_MAX_NUM_MSG_RESOURCES ];



/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

void ABCC_MemCreatePool( void )
{
   UINT16 i;

   abcc_iNumFreeMsg = ABCC_CFG_MAX_NUM_MSG_RESOURCES;

   for( i = 0;  i < ABCC_CFG_MAX_NUM_MSG_RESOURCES ; i++ )
   {
      abcc_uFreeMsgStack[ i ].psAllocMsg = &abcc_asMsgPool[ i ];
      abcc_asMsgPool[ i ].iMagicCookie = ABCC_MEM_MAGIC_COOKIE;
      abcc_asMsgPool[ i ].iBufferStatus = ABCC_MEM_BUFSTAT_FREE;
   }
}

ABP_MsgType* ABCC_MemAlloc( void )
{
   ABP_MsgType* pxItem = NULL;
   ABCC_PORT_UseCritical();
   ABCC_PORT_EnterCritical();
   if( abcc_iNumFreeMsg > 0 )
   {
      abcc_iNumFreeMsg--;
      pxItem = abcc_uFreeMsgStack[ abcc_iNumFreeMsg ].psMsg;
      ( (ABCC_MemAllocType*)pxItem )->iBufferStatus = ABCC_MEM_BUFSTAT_ALLOCATED;
   }

   ABCC_PORT_ExitCritical();

   ABCC_DEBUG_MSG_GENERAL(  "Mem: Buffer allocated: 0x%08x\n", (UINT32)pxItem  );

   return( pxItem );
}

void ABCC_MemFree( ABP_MsgType** pxItem)
{
   ABCC_MemAllocType* const psBuf = (ABCC_MemAllocType*)(*pxItem);
   ABCC_PORT_UseCritical();

   ABCC_DEBUG_MSG_GENERAL(  "Mem: Buffer returned:  0x%08x\n", (UINT32)*pxItem  );

   if( psBuf->iMagicCookie != ABCC_MEM_MAGIC_COOKIE )
   {
      ABCC_ERROR( ABCC_SEV_FATAL,
                  ABCC_EC_MSG_BUFFER_CORRUPTED,
                  (UINT32)psBuf );
      return;
   }

   if( psBuf->iBufferStatus == ABCC_MEM_BUFSTAT_FREE )
   {
      ABCC_ERROR( ABCC_SEV_FATAL,
                  ABCC_EC_MSG_BUFFER_ALREADY_FREED,
                  (UINT32)psBuf );
      return;
   }

   ABCC_PORT_EnterCritical();

   abcc_uFreeMsgStack[ abcc_iNumFreeMsg ].psAllocMsg = psBuf;
   abcc_iNumFreeMsg++;
   psBuf->iBufferStatus = ABCC_MEM_BUFSTAT_FREE;
   *pxItem = NULL;

   ABCC_PORT_ExitCritical();
}

ABCC_MemBufferStatusType ABCC_MemGetBufferStatus( ABP_MsgType* psMsg )
{
   const ABCC_MemAllocType* const psBuf = (ABCC_MemAllocType*)psMsg;

   if( psBuf->iMagicCookie != ABCC_MEM_MAGIC_COOKIE )
   {
      ABCC_ERROR( ABCC_SEV_FATAL,
                  ABCC_EC_MSG_BUFFER_CORRUPTED,
                  (UINT32)psBuf );

      return( ABCC_MEM_BUFSTAT_UNKNOWN );
   }

   return( (ABCC_MemBufferStatusType)psBuf->iBufferStatus );
}

void ABCC_MemSetBufferStatus( ABP_MsgType* psMsg,
                              ABCC_MemBufferStatusType eStatus )
{
   ABCC_MemAllocType* const psBuf = (ABCC_MemAllocType*)psMsg;

   if( psBuf->iMagicCookie != ABCC_MEM_MAGIC_COOKIE )
   {
      ABCC_ERROR( ABCC_SEV_FATAL,
                  ABCC_EC_MSG_BUFFER_CORRUPTED,
                  (UINT32)psMsg );
      return;
   }

   psBuf->iBufferStatus = eStatus;
}

/*******************************************************************************
** Tasks
********************************************************************************
*/
