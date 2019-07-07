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
** Implements command sequenser.
**
********************************************************************************
********************************************************************************
** Services :
**    ABCC_InitCmdSequencer()    - Initiate command sequencer at start-up
**    ABCC_ExecCmdSequencer()    - Cyclic execution of command sequencer
**
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_CMD_SEQ_H_
#define ABCC_CMD_SEQ_H_
#include "abcc_drv_cfg.h"
#include "abp.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

#ifndef ABCC_CFG_DRV_CMD_SEQ_ENABLE
#define ABCC_CFG_DRV_CMD_SEQ_ENABLE TRUE
#endif

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
** Initiate command sequencer.
**------------------------------------------------------------------------------
** Arguments:
**       None
**
** Returns:
**       None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_InitCmdSequencer( void );

/*------------------------------------------------------------------------------
** Cyclic execution of command sequencer. Checks for command sequences that for
** some reason needs re-triggering (ex. previous lack of resources)
**------------------------------------------------------------------------------
** Arguments:
**       None
**
** Returns:
**       None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_ExecCmdSequencer( void );

#endif
