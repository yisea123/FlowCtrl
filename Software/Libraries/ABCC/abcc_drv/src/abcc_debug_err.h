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
** Interface for error handling and debugging.
********************************************************************************
********************************************************************************
** Services :
** ABCC_DebugPrintMsg()             - Debug print of ABCC message
** ABCC_DebugPrintMsgEvent()        - Debug print of ABCC message event
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_DEBUG_ERR_H_
#define ABCC_DEBUG_ERR_H_

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abp.h"
#include "abcc_sys_adapt.h"
#include "abcc.h"
#include "abcc_port.h"
#include "abcc_handler.h"

/*
** DEBUG Level macros.
*/
#if( ABCC_CFG_DEBUG_EVENT_ENABLED )
#define DEBUG_EVENT  ABCC_PORT_DebugPrint
#else
#define DEBUG_EVENT( args )
#endif


#if ABCC_CFG_DEBUG_MESSAGING
/*------------------------------------------------------------------------------
** Prints ABCC message content using ABCC_PORT_DebugPrint().
** Prints: Message buffer address, message header and message data
**------------------------------------------------------------------------------
** Arguments:
**    pcInfo - General information about the debug print.
**    psMsg  - ABCC message
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void ABCC_DebugPrintMsg( char* pcInfo, ABP_MsgType* psMsg );

/*------------------------------------------------------------------------------
** Prints buffer address and source id for an ABCC message.
**------------------------------------------------------------------------------
** Arguments:
**    pcInfo - General information about the debug print.
**    psMsg  - ABCC message
** Returns:
**    None
**------------------------------------------------------------------------------
*/
void ABCC_DebugPrintMsgEvent( char* pcInfo, ABP_MsgType* psMsg );

#define ABCC_DEBUG_MSG_DATA( pcInfo, psMsg )  ABCC_DebugPrintMsg( ( pcInfo ), ( psMsg ) )
#define ABCC_DEBUG_MSG_EVENT( pcInfo, psMsg ) ABCC_DebugPrintMsgEvent( ( pcInfo ), ( psMsg ) )
#define ABCC_DEBUG_MSG_GENERAL      ABCC_PORT_DebugPrint
#else
#define ABCC_DEBUG_MSG_DATA( pcInfo, psMsg )
#define ABCC_DEBUG_MSG_EVENT( pcInfo, psMsg )
#define ABCC_DEBUG_MSG_GENERAL( pcInfo, ... )
#endif

#endif
