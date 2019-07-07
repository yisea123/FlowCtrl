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
** ** This file implements the ABCC_DunDriver() and ABCC_ISR() routine
** for parallel ping/pong operating mode.
********************************************************************************
********************************************************************************
*/

#include "abcc_drv_cfg.h"
#include "abcc_port.h"

#if( ABCC_CFG_DRV_PARALLEL_30 )

#include "abcc_td.h"
#include "../abcc_drv_if.h"
#include "abp.h"
#include "abcc.h"
#include "../abcc_link.h"
#include "abcc_sys_adapt.h"
#include "../abcc_debug_err.h"
#include "../abcc_handler.h"
#include "../abcc_timer.h"
#include "../abcc_cmd_seq.h"

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

/*******************************************************************************
** Public Functions
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_RunDriver()
**------------------------------------------------------------------------------
*/
ABCC_ErrorCodeType ABCC_Par30RunDriver( void )
{
   ABCC_MainStateType eMainState = ABCC_GetMainState();
   if (  eMainState < ABCC_DRV_SETUP )
   {
      if ( eMainState != ABCC_DRV_ERROR )
      {
         ABCC_ERROR(ABCC_SEV_WARNING, ABCC_EC_INCORRECT_STATE, 0);
         ABCC_SetMainStateError();
      }
      return( ABCC_EC_INCORRECT_STATE );
   }

#if( !ABCC_CFG_INT_ENABLED )
   (void)pnABCC_DrvRunDriverRx();
   ABCC_TriggerRdPdUpdate();
   ABCC_TriggerAnbStatusUpdate();
   ABCC_TriggerReceiveMessage();
#endif
#if ABCC_CFG_DRV_CMD_SEQ_ENABLE
   ABCC_ExecCmdSequencer();
#endif
   ABCC_CheckWrPdUpdate();
   ABCC_LinkCheckSendMessage();
   pnABCC_DrvRunDriverTx();

   return( ABCC_EC_NO_ERROR );
   /* end of AnybusMain() */
}


#if( ABCC_CFG_INT_ENABLED )
void ABCC_Par30ISR()
{
   ABCC_MainStateType eMainState;

   eMainState = ABCC_GetMainState();

   if ( eMainState < ABCC_DRV_WAIT_COMMUNICATION_RDY )
   {
      return;
   }

   if ( eMainState == ABCC_DRV_WAIT_COMMUNICATION_RDY )
   {
      ABCC_SetReadyForCommunication();
   }
   /*
   ** Let the driver handle the interrupt and clear the interrupt register.
   */
   (void)pnABCC_DrvISR();

   ABCC_LinkRunDriverRx();
   ABCC_TriggerRdPdUpdate();
   ABCC_TriggerAnbStatusUpdate();
   ABCC_TriggerReceiveMessage();
}
#else
void ABCC_Par30ISR()
{
   ABCC_ERROR( ABCC_SEV_WARNING, ABCC_EC_INTERNAL_ERROR, 0);
}
#endif

#endif /* ABCC_CFG_DRV_PARALLEL_30 */


/*******************************************************************************
** Public Services
********************************************************************************
*/

/*******************************************************************************
** Tasks
********************************************************************************
*/
