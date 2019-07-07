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
** Interface for driver internal interface to the abcc_handler
********************************************************************************
********************************************************************************
** Services:
** ABCC_SetReadyForCommunication()  : Indicate anybus ready for communication
** ABCC_SetMainStateError()         : Set driver in ABCC_ERROR state
** ABCC_GetMainState()              : Get current state

********************************************************************************
********************************************************************************
*/

#ifndef ABCC_HANDLER_H_
#define ABCC_HANDLER_H_

#include "abcc_drv_cfg.h"
#include "abp.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*
 ** Set default interrupt mask if not defined in abcc_drv_cfg.h
 */
#if( ABCC_CFG_DRV_PARALLEL )
#ifndef ABCC_CFG_INT_ENABLE_MASK_PAR
#define ABCC_CFG_INT_ENABLE_MASK_PAR          ( 0 )
#endif
#endif

#if( ABCC_CFG_DRV_PARALLEL_30 )
#ifndef ABCC_CFG_INT_ENABLE_MASK_PAR30
#define ABCC_CFG_INT_ENABLE_MASK_PAR30          ( 0 )
#endif
#endif

#if( ABCC_CFG_DRV_SPI )
#ifndef ABCC_CFG_INT_ENABLE_MASK_SPI
#define ABCC_CFG_INT_ENABLE_MASK_SPI          ( 0 )
#endif
#endif

/*******************************************************************************
** Typedefs
********************************************************************************
*/

typedef union
{
   ABP_MsgType*    psMsg;
   ABP_MsgType16*  psMsg16;
} ABCC_MsgType;

/*
** Type for ABCC main states
*/
typedef enum ABCC_MainStateType
{
   ABCC_DRV_INIT,
   ABCC_DRV_SHUTDOWN,
   ABCC_DRV_ERROR,
   ABCC_DRV_WAIT_COMMUNICATION_RDY,
   ABCC_DRV_SETUP,
   ABCC_DRV_RUNNING
} ABCC_MainStateType;


/*******************************************************************************
** Public Globals
********************************************************************************
*/

#if( ABCC_CFG_SYNC_MEASUREMENT_IP )
/*------------------------------------------------------------------------------
** Flag used for sync measurement
**------------------------------------------------------------------------------
*/
extern BOOL fAbccUserSyncMeasurementIp;
#endif

/*
** The interrupt mask that has been set to the ABCC at start up.
*/
extern UINT16 ABCC_iInterruptEnableMask;

/*******************************************************************************
** Public Services
********************************************************************************
*/


/*------------------------------------------------------------------------------
** ABCC_SetPdSize()
** Sets the new process data sizes.
**------------------------------------------------------------------------------
** Arguments:
**       iReadPdSize       - Size of the read process data (in bytes), used from
**                           this point on.
**       iWritePdSize      - Size of the write process data (in bytes), used from
**                           this point on.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetPdSize( const UINT16 iReadPdSize, const UINT16 iWritePdSize );


/*------------------------------------------------------------------------------
** The anybus is ready for communication. This function shall be called either
** due to power up interrupt or initial handshake timeout
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetReadyForCommunication( void );

/*------------------------------------------------------------------------------
** Set main state machine into error state. This will stop ABCC_ISR()
** and ABCC_RunDriver to perform any action towards application or anybus
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SetMainStateError( void );

/*------------------------------------------------------------------------------
** Gets currents state
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Current state ( ABCC_MainStateType )
**------------------------------------------------------------------------------
*/
EXTFUNC ABCC_MainStateType ABCC_GetMainState( void );

/*------------------------------------------------------------------------------
** Checks if update write process data is requested.
** If requested, the update is performed.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None:
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_CheckWrPdUpdate( void );

#endif  /* inclusion lock */
