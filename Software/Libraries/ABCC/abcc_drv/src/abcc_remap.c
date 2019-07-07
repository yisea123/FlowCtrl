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
** Implementation of remap function
********************************************************************************
********************************************************************************
*/


#include "abcc_drv_cfg.h"

#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )

#include "abcc_td.h"
#include "abp.h"
#include "abcc.h"
#include "abcc_handler.h"
#include "abcc_link.h"

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/

static UINT16   abcc_iNewPdReadSize    = 0;
static UINT16   abcc_iNewPdWriteSize   = 0;


static void abcc_RemapRespMsgSent( void )
{
   ABCC_SetPdSize( abcc_iNewPdReadSize, abcc_iNewPdWriteSize );
   ABCC_CbfRemapDone();
}

ABCC_ErrorCodeType ABCC_SendRemapRespMsg( ABP_MsgType* psMsgResp, UINT16 iNewReadPdSize, const UINT16 iNewWritePdSize )
{
   ABCC_ErrorCodeType eResult;
   abcc_iNewPdReadSize = iNewReadPdSize;
   abcc_iNewPdWriteSize = iNewWritePdSize;

   /*
    ** When ack is sent abcc_RemapRespMsgSent will be called.
    */
   eResult = ABCC_LinkWrMsgWithNotification( psMsgResp, abcc_RemapRespMsgSent );

   return( eResult );
}

#endif

