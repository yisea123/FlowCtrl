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
** ABCC command sequencer API used by the the application.
********************************************************************************
********************************************************************************
** Services provided by ABCC driver:
**    ABCC_AddCmdSeq()                    - Add a command sequence
**    ABCC_AbortCmdSeq()                  - Terminate a command sequence
********************************************************************************
********************************************************************************
*/
#ifndef ABCC_CMD_SEQ_IF_H_
#define ABCC_CMD_SEQ_IF_H_

#include "abcc_drv_cfg.h"
#include "abcc_port.h"
#include "abcc_td.h"
#include "abp.h"

/*******************************************************************************
** Constants
********************************************************************************
*/
#ifndef ABCC_CFG_DRV_CMD_SEQ_ENABLE
#define ABCC_CFG_DRV_CMD_SEQ_ENABLE TRUE
#endif
/*
** Defines used to build the command sequence.
** The debug option can be enabled in abcc_drv_cfg.h or in abcc_platform.h
** Example:
** static const ABCC_CmdSeqType ExampleSequence[] =
** {
**    ABCC_CMD_SEQ( CmdBuilder1,      RespHandler1 ),
**    ABCC_CMD_SEQ( CmdBuilder2,      NULL         ),
**    ABCC_CMD_SEQ( CmdBuilder3,      RespHandler3 ),
**    ABCC_CMD_SEQ_END()
** };
**
*/
#if( ABCC_CFG_DEBUG_CMD_SEQ_ENABLED )
#define ABCC_CMD_SEQ( cmd, resp ) { cmd, resp, #cmd, #resp }
#else
#define ABCC_CMD_SEQ( cmd, resp ) { cmd, resp }
#endif

#if( ABCC_CFG_DEBUG_CMD_SEQ_ENABLED )
#define ABCC_CMD_SEQ_END()    { NULL, NULL, NULL, NULL }
#else
#define ABCC_CMD_SEQ_END()    { NULL, NULL }
#endif

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*
** Response types for command callback
*/
typedef enum ABCC_CmdSeqCmdStatus
{
   ABCC_SEND_COMMAND,
   ABCC_SKIP_COMMAND,
   ABCC_CMD_ABORT_SEQ
}
ABCC_CmdSeqCmdStatusType;

/*
** Response types for response callback
*/
typedef enum ABCC_CmdSeqRespStatus
{
   ABCC_EXEC_NEXT_COMMAND,
   ABCC_EXEC_CURR_COMMAND,
   ABCC_RESP_ABORT_SEQ
}
ABCC_CmdSeqRespStatusType;

/*------------------------------------------------------------------------------
** Type for command callback used by command sequencer.
** See also description of ABCC_AddCmdSeq() and ABCC_AbortCmdSeq().
**------------------------------------------------------------------------------
** Arguments:
**    psCmdMsg    - Pointer to command buffer
**
** Returns:
**    ABCC_CmdSeqCmdStatusType:
**    ABCC_SEND_COMMAND       - Send the command to ABCC
**    ABCC_SKIP_COMMAND       - Skip and move to next command in sequence
**    ABCC_CMD_ABORT_SEQ      - Abort whole sequence
**------------------------------------------------------------------------------
*/
typedef ABCC_CmdSeqCmdStatusType (*ABCC_CmdSeqCmdHandler)( ABP_MsgType* psCmdMsg );

/*------------------------------------------------------------------------------
** Type for response callback used by command sequencer.
** See also description of ABCC_AddCmdSeq() and ABCC_AbortCmdSeq().
**------------------------------------------------------------------------------
** Arguments:
**    psRespMsg      - Pointer to response buffer
**
** Returns:
**    ABCC_CmdSeqRespStatusType:
**    ABCC_EXEC_NEXT_COMMAND  - Move to next command in sequence
**    ABCC_EXEC_CURR_COMMAND  - Execute current command again.
**    ABCC_RESP_ABORT_SEQ     - Abort whole sequence
**------------------------------------------------------------------------------
*/
typedef ABCC_CmdSeqRespStatusType (*ABCC_CmdSeqRespHandler)( ABP_MsgType* psRespMsg );

/*------------------------------------------------------------------------------
** Type for sequence-done callback used by command sequencer.
** See also description of ABCC_AddCmdSeq().
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
typedef void (*ABCC_CmdSeqDoneHandler)( void );

/*------------------------------------------------------------------------------
** Type used by command sequencer to define command-response callback pairs.
** See also description of ABCC_AddCmdSeq().
**------------------------------------------------------------------------------
*/
typedef struct ABCC_CmdSeq
{
   ABCC_CmdSeqCmdHandler   pnCmdHandler;
   ABCC_CmdSeqRespHandler  pnRespHandler;
#if( ABCC_CFG_DEBUG_CMD_SEQ_ENABLED)
   char*                   pcCmdName;
   char*                   pcRespName;
#endif
}
ABCC_CmdSeqType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/
#if ABCC_CFG_DRV_CMD_SEQ_ENABLE
/*------------------------------------------------------------------------------
** Add a command sequence.
**
** This is an alternative way of sending commands to the ABCC module. The driver
** provides support for command buffer allocation, resource control and
** sequencing of messages. The user has to provide functions to build messages
** and handle responses (See description of ABCC_CmdSeqCmdHandler,
** ABCC_CmdSeqRespHandler and ABCC_CmdSeqDoneHandler callback functions).
**
** An array of ABCC_CmdSeqType's is provided and defines the command sequence to
** be executed. The last entry in the array is indicated by NULL pointers.
** The next command in the sequence will be executed when the previous command
** has successfully received a response.
**
** If a command sequence response handler exists the response will be passed to
** the application.
**
** If the command sequence response handler is NULL the application will not be
** notified. If the error bit is set the application will be notified by the
** ABCC_CbfDriverError() callback.
**
** If the pnCmdSeqDone function callback exists the application will be
** notified when the whole command sequence has finished.
**
** The number of concurrent command sequences is limited by
** ABCC_CFG_MAX_NUM_CMD_SEQ defined in abcc_drv_cfg.h.
**
** Example:
** ABCC_AddCmdSeq( ExampleSequence, CbfDone );
**
**------------------------------------------------------------------------------
** Arguments:
**    pasCmdSeq         - Pointer to command sequence to be executed.
**    pnCmdSeqDone      - Function pointer for notification when sequence is
**                        done.
**                        Set to NULL to skip notification to application.
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void ABCC_AddCmdSeq( const ABCC_CmdSeqType* pasCmdSeq,
                     const ABCC_CmdSeqDoneHandler pnCmdSeqDone );

/*------------------------------------------------------------------------------
** Performs immediate termination of the specified command sequence.
**
** Note! This function shall only be used if there is no possibility to use the
** ABORT return value of the command or response handler.
** (see description of ABCC_CmdSeqCmdHandler and ABCC_CmdSeqRespHandler)
**
** Note! Calling this function has some restrictions to perform safe abortion.
** 1. The call has to be done in a context that has the same or lower priority
**    than the ABCC driver main loop context.
** 2. The call has to be done in a context that has the same or lower priority
**    than the ABCC receive message context.
**------------------------------------------------------------------------------
** Arguments:
**   pasCmdSeq       - Pointer to sequence to be aborted. If NULL all
**                     active sequences will be aborted.
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void ABCC_AbortCmdSeq( const ABCC_CmdSeqType* pasCmdSeq );
#endif

#endif  /* inclusion lock */
