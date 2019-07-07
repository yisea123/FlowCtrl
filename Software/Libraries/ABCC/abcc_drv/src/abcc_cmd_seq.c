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
** Implements a command sequencer for ABCC command messages.
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_cmd_seq_if.h"
#include "abcc_sw_port.h"
#include "abcc_debug_err.h"
#include "abcc_link.h"
#include "abcc_mem.h"

#if ABCC_CFG_DRV_CMD_SEQ_ENABLE
/*******************************************************************************
** Constants
********************************************************************************
*/

/*
** Set maximum number of retries for trying to retrieve a message buffer.
** One try will be done each ABCC_RunDriver() call.
** Note that when setting this to more than 0 it will handle temporary
** message resource problems, but all retries will be done from
** ABCC_RunDriver() context.
*/
#ifndef ABCC_CFG_CMD_SEQ_MAX_NUM_RETRIES
#define ABCC_CFG_CMD_SEQ_MAX_NUM_RETRIES 0
#endif

/*
** Set to TRUE if the first message in a sequence shall be sent
** immediately when the sequence is added. This is to ensure that all messaging
** is done from the same context if no resource problems occurs.
*/
#ifndef ABCC_CFG_CMD_SEQ_IMMEDIATE_SEND
#define ABCC_CFG_CMD_SEQ_IMMEDIATE_SEND   TRUE
#endif

#ifndef ABCC_CFG_MAX_NUM_CMD_SEQ
#define ABCC_CFG_MAX_NUM_CMD_SEQ    2
#endif

#if( ABCC_CFG_MAX_NUM_CMD_SEQ > 255 )
#error "ABCC_CFG_MAX_NUM_CMD_SEQ larger than 255 not supported"
#endif

/*******************************************************************************
** Typedefs
********************************************************************************
*/
typedef enum CmdSeqState
{
   CMD_SEQ_NOT_STARTED = 0,
   CMD_SEQ_BUSY,
   CMD_SEQ_WAITING_FOR_RESP,
   CMD_SEQ_RETRIGGER,
   CMD_SEQ_ANY_STATE
}
CmdSeqStateType;

typedef struct CmdSeqHandler
{
   const ABCC_CmdSeqType*  pasCmdSeq;
   ABCC_CmdSeqDoneHandler  pnSeqDone;
   CmdSeqStateType         eCmdSeqState;
   UINT8                   bCurrSeqIndex;
   UINT8                   bSourceId;
   UINT8                   bTriggerCount;
}
CmdSeqHandlerType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/


/*******************************************************************************
** Private Globals
********************************************************************************
*/

static UINT16 abcc_iNeedReTriggerCount;
static CmdSeqHandlerType abcc_asCmdSeq[ ABCC_CFG_MAX_NUM_CMD_SEQ ];

static BOOL ExecCmdSequence( CmdSeqHandlerType* psCmdSeqHandler, ABP_MsgType* psMsg );
static void HandleResponse( ABP_MsgType* psMsg );
static CmdSeqHandlerType* AllocCmdSeqHandler( const ABCC_CmdSeqType* pasCmdSeq );

#define SetState( psCmdSeqHandler, eNewState ) \
if( !CheckAndSetState( psCmdSeqHandler, CMD_SEQ_ANY_STATE, eNewState ) ) { ABCC_ASSERT( FALSE ); }

#define GetState( psCmdSeqHandler ) ( psCmdSeqHandler->eCmdSeqState )

static BOOL CheckAndSetState( CmdSeqHandlerType* psCmdSeqHandler,
                              CmdSeqStateType eCheckState,
                              CmdSeqStateType eNewState );


/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Allocate command sequence handler. Returns NULL if no handler is available.
**------------------------------------------------------------------------------
** Arguments:
**    pasCmdSeq            - Pointer to command sequence.
**
** Returns:
**    CmdSeqHandlerType*   - Pointer to allocated handler. NULL if no resource
**                           is allocated.
**------------------------------------------------------------------------------
*/
static CmdSeqHandlerType* AllocCmdSeqHandler( const ABCC_CmdSeqType* pasCmdSeq )
{
   UINT8 i;
   CmdSeqHandlerType* psCmdSeqHandler;
   ABCC_PORT_UseCritical();

   psCmdSeqHandler = NULL;

   ABCC_PORT_EnterCritical();

   for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
   {
      if( abcc_asCmdSeq[ i ].pasCmdSeq == NULL )
      {
         psCmdSeqHandler = &abcc_asCmdSeq[ i ];
         psCmdSeqHandler->pasCmdSeq = pasCmdSeq;
         break;
      }
   }

   ABCC_PORT_ExitCritical();
   return( psCmdSeqHandler );
}

/*------------------------------------------------------------------------------
** Resets handler to initial state. If it's the initial reset after startup the
** reset of state will have no side effect.
**------------------------------------------------------------------------------
** Arguments:
**    fInitial            - Set to TRUE if it's the initial reset after startup.
**    psCmdSeqHandler     - Pointer to handler
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void ResetCmdSeqHandler( CmdSeqHandlerType* psCmdSeqHandler, BOOL fInitial )
{
   if( psCmdSeqHandler != NULL )
   {
      if( fInitial )
      {
         psCmdSeqHandler->eCmdSeqState = CMD_SEQ_NOT_STARTED;
      }
      else
      {
         /*
         ** This call may affect file global abcc_iNeedReTriggerCount
         */
         SetState( psCmdSeqHandler, CMD_SEQ_NOT_STARTED );
      }
      psCmdSeqHandler->pasCmdSeq = NULL;
      psCmdSeqHandler->pnSeqDone = NULL;
      psCmdSeqHandler->bCurrSeqIndex = 0;
      psCmdSeqHandler->bSourceId = 0;
      psCmdSeqHandler->bTriggerCount = 0;
   }
}

/*------------------------------------------------------------------------------
** Find handler mapped to source id. Used when response message is mapped to
** command sequence.
**------------------------------------------------------------------------------
** Arguments:
**    bSourceId            - Source id
**
** Returns:
**    CmdSeqHandlerType*   - Mapped handler. NULL if not found.
**------------------------------------------------------------------------------
*/
static CmdSeqHandlerType* FindCmdSeqHandlerFromSourceId( UINT8 bSourceId )
{
   UINT8 i;

   for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
   {
      if( ( abcc_asCmdSeq[ i ].eCmdSeqState == CMD_SEQ_WAITING_FOR_RESP ) &&
          ( abcc_asCmdSeq[ i ].pasCmdSeq != NULL ) &&
            abcc_asCmdSeq[ i ].bSourceId == bSourceId )
      {
         return( &abcc_asCmdSeq[ i ] );
      }
   }

   return( NULL );
}

/*------------------------------------------------------------------------------
** Find handler mapped to command sequence provided by user.
**------------------------------------------------------------------------------
** Arguments:
**    pasCmdSeq            - Pointer to command sequence.
**
** Returns:
**    CmdSeqHandlerType*   - Mapped handler. NULL if not found.
**------------------------------------------------------------------------------
*/
static CmdSeqHandlerType* FindCmdSeqHandler( const ABCC_CmdSeqType* pasCmdSeq )
{
   UINT8 i;

   for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
   {
      if( pasCmdSeq == abcc_asCmdSeq[ i ].pasCmdSeq )
      {
         return( &abcc_asCmdSeq[ i ] );
      }
   }

   return( NULL );
}


/*------------------------------------------------------------------------------
** Set handler in new state. May have side effects when entering or leaving
** CMD_SEQ_RETRIGGER state (see ABCC_ExecCmdSequencer()).
**
**------------------------------------------------------------------------------
** Arguments:
**    psCmdSeqHandler      - Pointer to handler
**    eNewState            - New state
**
** Returns:
**    TRUE: New state set
**    FALSE: Old state kept
**------------------------------------------------------------------------------
*/
static BOOL CheckAndSetState( CmdSeqHandlerType* psCmdSeqHandler,
                              CmdSeqStateType eCheckState,
                              CmdSeqStateType eNewState )
{
   BOOL fRet;
   ABCC_PORT_UseCritical();

   fRet = FALSE;
   ABCC_PORT_EnterCritical();

   if( ( eCheckState == CMD_SEQ_ANY_STATE ) ||
       ( eCheckState == psCmdSeqHandler->eCmdSeqState ) )
   {
      if( ( psCmdSeqHandler->eCmdSeqState == CMD_SEQ_RETRIGGER ) &&
            eNewState != CMD_SEQ_RETRIGGER )
      {
         abcc_iNeedReTriggerCount--;
      }

      if( ( psCmdSeqHandler->eCmdSeqState != CMD_SEQ_RETRIGGER ) &&
            eNewState == CMD_SEQ_RETRIGGER )
      {
         abcc_iNeedReTriggerCount++;
      }

      psCmdSeqHandler->eCmdSeqState = eNewState;
      fRet = TRUE;
   }

   ABCC_PORT_ExitCritical();

   return( fRet );
}

/*------------------------------------------------------------------------------
** Performs abort on handler if it is active.
**------------------------------------------------------------------------------
** Arguments:
**    psCmdSeqHandler      - Pointer to handler
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void DoAbort( CmdSeqHandlerType* psCmdSeqHandler )
{
   BOOL fFreeSourceId;
   UINT8 bSourceId;
   ABCC_PORT_UseCritical();

   fFreeSourceId = FALSE;
   ABCC_PORT_EnterCritical();

   if( psCmdSeqHandler->eCmdSeqState == CMD_SEQ_BUSY )
   {
      /*
      ** State not allowed. See header file documentation for
      ** ABCC_AbortCmdSeq()
      */
      ABCC_ERROR( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_STATE, psCmdSeqHandler->eCmdSeqState );
   }
   else if( psCmdSeqHandler->eCmdSeqState != CMD_SEQ_NOT_STARTED )
   {
      if( psCmdSeqHandler->eCmdSeqState == CMD_SEQ_RETRIGGER )
      {
         abcc_iNeedReTriggerCount--;
      }
      if( psCmdSeqHandler->eCmdSeqState == CMD_SEQ_WAITING_FOR_RESP )
      {
         bSourceId = psCmdSeqHandler->bSourceId;
         fFreeSourceId = TRUE;
      }
      /*
      ** Do reset of handler.
      */
      ResetCmdSeqHandler( psCmdSeqHandler, TRUE );
   }
   ABCC_PORT_ExitCritical();

   /*
   ** Free of sourceId is done outside critical section to avoid nested
   ** critical sections. Result can be ignored
   */
   if( fFreeSourceId )
   {
      (void)ABCC_LinkGetMsgHandler( bSourceId );
   }
}

/*------------------------------------------------------------------------------
** Common response handler for all response messages routed to the command
** sequencer. Implements ABCC_MsgHandlerFuncType function callback (abcc.h)
**------------------------------------------------------------------------------
** Arguments:
**    psMsg       - Pointer to response message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void HandleResponse( ABP_MsgType* psMsg )
{
   UINT8 bSourceId;
   CmdSeqHandlerType* psCmdSeqHandler;
   ABCC_CmdSeqRespStatusType eStatus;


   bSourceId = ABCC_GetMsgSourceId( psMsg );
   psCmdSeqHandler =  FindCmdSeqHandlerFromSourceId( bSourceId );

   if( psCmdSeqHandler != NULL )
   {
      /*
      ** The corresponding command sequence found
      */
      if( CheckAndSetState( psCmdSeqHandler, CMD_SEQ_WAITING_FOR_RESP, CMD_SEQ_BUSY ) )
      {
         if( psCmdSeqHandler->pasCmdSeq[ psCmdSeqHandler->bCurrSeqIndex ].pnRespHandler != NULL )
         {
            /*
            ** Pass the response message to the application.
            */
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->%s()\n",
               (UINT32)psCmdSeqHandler->pasCmdSeq,
               psCmdSeqHandler->pasCmdSeq[ psCmdSeqHandler->bCurrSeqIndex ].pcRespName  );
#endif
            eStatus = psCmdSeqHandler->pasCmdSeq[ psCmdSeqHandler->bCurrSeqIndex ].pnRespHandler( psMsg );

            if( eStatus == ABCC_EXEC_NEXT_COMMAND )
            {
               /*
               ** Move to next command in sequence
               */
               psCmdSeqHandler->bCurrSeqIndex++;
            }
            else if( eStatus == ABCC_RESP_ABORT_SEQ )
            {
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
               ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Aborted\n",
                                       (UINT32)psCmdSeqHandler->pasCmdSeq  );
#endif
               /*
               ** Loop until end of sequence and skip done callback.
               */
               while( &psCmdSeqHandler->pasCmdSeq[ ++psCmdSeqHandler->bCurrSeqIndex ] != NULL )
               {
               };
               psCmdSeqHandler->pnSeqDone = NULL;
            }
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            else
            {
               ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Executing same sequence step again\n",
                                       (UINT32)psCmdSeqHandler->pasCmdSeq  );
            }
#endif
         }
         else
         {
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Response received\n",
                                    (UINT32)psCmdSeqHandler->pasCmdSeq  );
#endif
            /*
            ** No response handler exists. Check that message is OK and move to next command.
            ** Move to next command in sequence
            */
            ABCC_ASSERT_ERR( ABCC_VerifyMessage( psMsg ) == ABCC_EC_NO_ERROR,
                             ABCC_SEV_WARNING, ABCC_EC_RESP_MSG_E_BIT_SET, 0 );
            psCmdSeqHandler->bCurrSeqIndex++;
         }

         if( ABCC_MemGetBufferStatus( psMsg ) != ABCC_MEM_BUFSTAT_IN_APPL_HANDLER )
         {
            /*
            ** The application has used the buffer for other things.
            */
            psMsg = ABCC_GetCmdMsgBuffer();
         }

         /*
         ** Execute next command. The return value is ignored
         ** since the message deallocation will be handled after return of this
         ** function
         */
         (void)ExecCmdSequence( psCmdSeqHandler, psMsg );
      }
   }
}

/*------------------------------------------------------------------------------
** Execute the command sequence
**------------------------------------------------------------------------------
** Arguments:
**    psCmdSeqHandler   - Pointer to handler
**    psMsg             - Pointer to allocated command message buffer.
**
** Returns:
**    TRUE              - Command buffer sent or freed.
**    FALSE             - Command buffer not sent or freed.
**------------------------------------------------------------------------------
*/
static BOOL ExecCmdSequence( CmdSeqHandlerType* psCmdSeqHandler, ABP_MsgType* psMsg )
{
   BOOL fCmdBufferConsumed;
   const ABCC_CmdSeqType* psCmdSeq;
   ABCC_CmdSeqCmdStatusType eStatus;

   fCmdBufferConsumed = FALSE;

   if( psMsg != NULL )
   {
      psCmdSeqHandler->bTriggerCount = 0;
      psCmdSeq = &psCmdSeqHandler->pasCmdSeq[ psCmdSeqHandler->bCurrSeqIndex ];

      while( ( psCmdSeq->pnCmdHandler != NULL ) && !fCmdBufferConsumed )
      {
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->%s()\n", (UINT32)psCmdSeqHandler->pasCmdSeq,
                                               psCmdSeqHandler->pasCmdSeq[ psCmdSeqHandler->bCurrSeqIndex ].pcCmdName  );

#endif
         eStatus = psCmdSeq->pnCmdHandler( psMsg );
         if( eStatus == ABCC_SKIP_COMMAND )
         {
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Command not sent, jump to next sequence step\n",
                                    (UINT32)psCmdSeqHandler->pasCmdSeq  );
#endif
            /*
            ** User has chosen not to execute this command. Move to next.
            */
            psCmdSeq++;
            psCmdSeqHandler->bCurrSeqIndex++;
         }
         else if( eStatus == ABCC_SEND_COMMAND )
         {
            psCmdSeqHandler->bSourceId = ABCC_GetMsgSourceId( psMsg );
            SetState( psCmdSeqHandler, CMD_SEQ_WAITING_FOR_RESP );
            (void)ABCC_SendCmdMsg( psMsg, HandleResponse );
            fCmdBufferConsumed = TRUE;
         }
         else
         {
            /*
            ** Abort move to end of sequence
            */
            while( (++psCmdSeq)->pnCmdHandler != NULL )
            {
            };
            psCmdSeqHandler->pnSeqDone = NULL;
#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Aborted\n",
                                    (UINT32)psCmdSeqHandler->pasCmdSeq  );
#endif
         }
      }

      /*
      ** Check end of sequence
      */
      if( psCmdSeq->pnCmdHandler == NULL )
      {
         ABCC_CmdSeqDoneHandler pnSeqDone;

         /*
         ** Free resource before calling done callback
         */
         ABCC_ReturnMsgBuffer( &psMsg );
         fCmdBufferConsumed = TRUE;

#if ABCC_CFG_DEBUG_CMD_SEQ_ENABLED
            ABCC_PORT_DebugPrint(  "CmdSeq(%x)->Done\n",
                                    (UINT32)psCmdSeqHandler->pasCmdSeq  );
#endif
         pnSeqDone = psCmdSeqHandler->pnSeqDone;
         ResetCmdSeqHandler( psCmdSeqHandler, FALSE );

         if( pnSeqDone != NULL )
         {
            pnSeqDone();
         }
      }
   }
   else
   {
      /*
      ** Currently out of resource. Try again at next call of
      ** ABCC_ExecCmdSequencer().
      */
      SetState( psCmdSeqHandler, CMD_SEQ_RETRIGGER );
   }

   return( fCmdBufferConsumed );
}

/*******************************************************************************
** Public Services
********************************************************************************
*/

void ABCC_AddCmdSeq( const ABCC_CmdSeqType* pasCmdSeq,
                     const ABCC_CmdSeqDoneHandler pnCmdSeqDone )
{
   CmdSeqHandlerType* psCmdSeqHandler;
#if ABCC_CFG_CMD_SEQ_IMMEDIATE_SEND
   ABP_MsgType* psMsg;
#endif

   if( pasCmdSeq != NULL )
   {
      /*
      ** Allocate and init handler.
      */
      psCmdSeqHandler = AllocCmdSeqHandler( pasCmdSeq );
      if( psCmdSeqHandler != NULL )
      {
         psCmdSeqHandler->pnSeqDone = pnCmdSeqDone;

#if ABCC_CFG_CMD_SEQ_IMMEDIATE_SEND
         psMsg = ABCC_GetCmdMsgBuffer();
         (void)ExecCmdSequence( psCmdSeqHandler, psMsg );
#else
         SetState( psCmdSeqHandler, CMD_SEQ_RETRIGGER );
#endif
      }
   }
   else
   {
      ABCC_ERROR( ABCC_SEV_WARNING,
                  ABCC_EC_OUT_OF_CMD_SEQ_RESOURCES,
                  ABCC_CFG_MAX_NUM_CMD_SEQ );
   }
}

void ABCC_AbortCmdSeq( const ABCC_CmdSeqType* pasCmdSeq )
{
   UINT8 i;
   CmdSeqHandlerType* psCmdSeqHandler;

   if( pasCmdSeq == NULL )
   {
      for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
      {
         DoAbort( &abcc_asCmdSeq[ i ] );
      }
   }
   else
   {
      psCmdSeqHandler = FindCmdSeqHandler( pasCmdSeq );

      while( psCmdSeqHandler != NULL )
      {
         DoAbort( psCmdSeqHandler );
         psCmdSeqHandler = FindCmdSeqHandler( pasCmdSeq );
      }
   }
}

void ABCC_InitCmdSequencer( void )
{
   UINT8 i;
   for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
   {
      ResetCmdSeqHandler( &abcc_asCmdSeq[ i ], TRUE );
   }
   abcc_iNeedReTriggerCount = 0;
}

void ABCC_ExecCmdSequencer( void )
{
   UINT8 i;
   ABP_MsgType* psMsg;

   /*
   ** Only execute if any sequence requires re-trigger.
   */
   if( abcc_iNeedReTriggerCount > 0 )
   {
      psMsg = NULL;

      for( i = 0; i < ABCC_CFG_MAX_NUM_CMD_SEQ; i++ )
      {
         if( CheckAndSetState( &abcc_asCmdSeq[ i ], CMD_SEQ_RETRIGGER, CMD_SEQ_BUSY ) )
         {
            abcc_asCmdSeq[ i ].bTriggerCount++;
            if( abcc_asCmdSeq[ i ].bTriggerCount > ABCC_CFG_CMD_SEQ_MAX_NUM_RETRIES )
            {
               ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_OUT_OF_MSG_BUFFERS, (UINT32)abcc_asCmdSeq[ i ].pasCmdSeq );
            }

            if( psMsg == NULL )
            {
               psMsg = ABCC_GetCmdMsgBuffer();
            }

            if( ExecCmdSequence( &abcc_asCmdSeq[ i ], psMsg ) == TRUE )
            {
               /*
               ** Message buffer consumed
               */
               psMsg = NULL;
            }
         }
      }

      if( psMsg != NULL )
      {
         ABCC_ReturnMsgBuffer( &psMsg );
      }
   }
}
#endif
/*******************************************************************************
** Tasks
********************************************************************************
*/
