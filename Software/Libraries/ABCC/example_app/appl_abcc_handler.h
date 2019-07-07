/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.02.02 (2016-11-10)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**    ABCC Driver    5.02.01 (2016-11-02)                                     **
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
** This is an example implementation of an application handler to control the
** ABCC module. It has support to initialize, run and reset/shutdown the ABCC.
** The state machine shall be executed cyclically and it returns its current
** status after every execution.
********************************************************************************
********************************************************************************
** Services:
**    APPL_HandleAbcc()              - Runs the state machine controlling the
**                                     ABCC module
**    APPL_RestartAbcc()             - Forces the state machine to restart the
**                                     ABCC module
**    APPL_Shutdown()                - Forces the state machine to shut down the
**                                     ABCC module
**    APPL_Reset()                   - Forces the state machine to reset the
**                                     ABCC module
**    APPL_RestoreToDefault()        - Restores any NVS parameter to its default
**                                     value
**    APPL_GetCandidateFwAvailable() - Call to check if there is a firmware in
**                                     the candidate area
**    APPL_SetCandidateFwAvailable() - Sets whether firmware is available in the
**                                     candidate area
**    APPL_IsResetRequestAllowed()   - Check if a reset is allowed
**
**    APPL_SetHwSwitch1Value()       - Set switch 1 value
********************************************************************************
********************************************************************************
*/

#ifndef APPL_ABCC_HANDLER_H
#define APPL_ABCC_HANDLER_H
#include "abcc_td.h"
#include "string.h"

/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Status reported by the ABCC handler controlling the ABCC module
**------------------------------------------------------------------------------
*/
typedef enum APPL_AbccHandlerStatus
{
   APPL_MODULE_NO_ERROR,         /* Module OK */
   APPL_MODULE_NOT_DETECTED,     /* No module plugged */
   APPL_MODULE_NOT_SUPPORTED,    /* Unsupported module detected */
   APPL_MODULE_NOT_ANSWERING,    /* Possible reasons: Wrong API selected, defect module */
   APPL_MODULE_RESET,            /* Reset requested from ABCC */
   APPL_MODULE_SHUTDOWN,         /* Shutdown requested */
   APPL_MODULE_UNEXPECTED_ERROR  /* Unexpected error occurred */
}
APPL_AbccHandlerStatusType;


/*------------------------------------------------------------------------------
**  Error codes for example application:
**
**  APPL_NO_ERROR                  - No error
**  APPL_AD_PD_REDA_SIZE_ERR       - Total process data read size too large
**                                   Check ABCC_CFG_MAX_PROCESS_DATA_SIZE
**  APPL_AD_PD_WRITE_SIZE_ERR      - Total process data write too large
**                                   Check ABCC_CFG_MAX_PROCESS_DATA_SIZE
**  APPL_AD_TOO_MANY_READ_MAPPINGS - Read process data map has too many entries
**                                   Check AD_MAX_OF_READ_WRITE_TO_MAP.
**  APPL_AD_TOO_MANY_WRITE_MAPPINGS- Write process data map has too many entries
**                                   Check AD_MAX_OF_WRITE_WRITE_TO_MAP
**  APPL_AD_UNKNOWN_ADI            - Requested ADI could not be found
**------------------------------------------------------------------------------
*/
typedef enum APPL_ErrCode
{
   APPL_NO_ERROR = 0,
   APPL_AD_PD_READ_SIZE_ERR,
   APPL_AD_PD_WRITE_SIZE_ERR,
   APPL_AD_TOO_MANY_READ_MAPPINGS,
   APPL_AD_TOO_MANY_WRITE_MAPPINGS,
   APPL_AD_UNKNOWN_ADI
}
APPL_ErrCodeType;

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** This function shall be called on cyclic bases from the main loop to handle
** the ABCC. It includes a state machine for handling reset, run, and shutdown
** of the driver.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    State of the ABCC handler
**------------------------------------------------------------------------------
*/
EXTFUNC APPL_AbccHandlerStatusType APPL_HandleAbcc( void );

/*------------------------------------------------------------------------------
** This function will force the ABCC handler to restart the ABCC module
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_RestartAbcc( void );

/*------------------------------------------------------------------------------
** This function will force the ABCC handler to shutdown the ABCC module
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_Shutdown( void );

/*------------------------------------------------------------------------------
** This function will force the ABCC handler to reset the ABCC
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_Reset( void );

/*------------------------------------------------------------------------------
** Application has encountered an unexpected error.
** The abcc main state machine will halt and indicate error.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_UnexpectedError( void );

/*------------------------------------------------------------------------------
** Called to check if the requested reset is permitted by the application.
**------------------------------------------------------------------------------
** Arguments:
**    bResetType           - Type of reset, see ABP_RESET_XXX defines.
**
** Returns:
**    BOOL                 - TRUE: Reset request is allowed.
**                           FALSE: Reset request NOT allowed.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL IsResetRequestAllowed( UINT8 bResetType );

/*------------------------------------------------------------------------------
** Set HW switch1 value to the application. The value is used to generate the
** the node address or the IP address (192.168.0.X) depending on network.
**
** NOTE: For networks that uses an IP address and if this function is called
**       with address 0 no address will be set to the ABCC.
**------------------------------------------------------------------------------
** Arguments:
**    bSwitchValue - Switch 1 value
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_SetAddress( UINT8 bSwitchValue );

/*------------------------------------------------------------------------------
** Set HW switch2 value to the application. The value is used to generate the
** baud rate for networks that implements network configuration object instance
** 2.
**------------------------------------------------------------------------------
** Arguments:
**    bSwitchValue - Switch 2 value
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APPL_SetBaudrate( UINT8 bSwitchValue );

#endif  /* inclusion lock */
