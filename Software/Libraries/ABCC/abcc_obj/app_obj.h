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
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
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
** This is the public header file for the APP object.
********************************************************************************
********************************************************************************
** Services:
**    APP_GetCandidateFwAvailable() - Checks if there is an firmware available
**                                    in the module's candidate area.
**    APP_ProcResetRequest()        - Performs a reset of the application
**    APP_ProcessCmdMsg()           - Processes commands sent to the APP object.
**    APP_HwConfAddress()           - Set attribute ABP_APP_IA_HW_CONF_ADDR.
********************************************************************************
********************************************************************************
*/
#ifndef APP_OBJ_H
#define APP_OBJ_H


/*******************************************************************************
** Constants
********************************************************************************
*/

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
** Call to check if there is firmware available in the candidate area. This
** function retrieves the value from a NVS.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    BOOL                 - TRUE: Firmware available in candidate area.
**                           FALSE: Firmware NOT available in candidate area.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL APP_GetCandidateFwAvailable( void );


/*------------------------------------------------------------------------------
** Set attribute ABP_APP_IA_HW_CONF_ADDR to indicate if the address is set by
** hardware switches or not.
**------------------------------------------------------------------------------
** Arguments:
**    fhwConfAddress:   - TRUE if address is set by HW switches.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void APP_HwConfAddress( BOOL fhwConfAddress );


/*------------------------------------------------------------------------------
** Called to perform a reset upon request.
**------------------------------------------------------------------------------
** Arguments:
**    bResetType        - Type of reset, see ABP_RESET_XXX defines.
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
EXTFUNC void APP_ProcResetRequest( UINT8 bResetType );

/*------------------------------------------------------------------------------
** The function that processes the commands sent to the APP object.
**------------------------------------------------------------------------------
** Arguments:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Returns:
**    None.
**------------------------------------------------------------------------------
*/
EXTFUNC void APP_ProcessCmdMsg( ABP_MsgType* psNewMessage );




#endif  /* inclusion lock */
