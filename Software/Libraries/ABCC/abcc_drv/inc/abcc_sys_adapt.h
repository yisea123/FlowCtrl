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
** Defines system specific interface.
********************************************************************************
********************************************************************************
** Services:
**    ABCC_SYS_AbccInterruptEnable  - Enable interrupts.
**    ABCC_SYS_AbccInterruptDisable - Disable interrupts.
**    ABCC_SYS_HWReset()            - Puts Anybus HW into reset.
**    ABCC_SYS_HWReleaseReset       - Pulls Anybus HW out of reset.
**    ABCC_SYS_IsAbccInterruptActive- Check if interrupt is active.
**    ABCC_SYS_ReadModuleId         - Read Module Identification pins from the
**                                    ABCC interface.
**    ABCC_SYS_SetOpmode            - Sets ABCC Operating Mode pins from the
**                                    ABCC interface
**    ABCC_SYS_GetOpmode            - Reads ABCC Operating Mode from hardware
**    ABCC_SYS_ModuleDetect         - Detects if a module is present by reading
**                                    the Module Detection pins.
**    ABCC_SYS_Init()               - Hardware or system dependent
**                                    initialization.
**    ABCC_SYS_Close()              - Close or free all resources allocated in
**                                    ABCC_SYS_Init
********************************************************************************
********************************************************************************
*/
#ifndef ABCC_SYS_ADAPT
#define ABCC_SYS_ADAPT
#include "abcc_drv_cfg.h"
#include "abcc_td.h"

/*******************************************************************************
** Defines
********************************************************************************
*/

/*******************************************************************************
** Public Services Definitions
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Enable the ABCC HW interrupt (IRQ_N pin on the application interface)
** This function will be called by the driver when the ABCC interrupt shall be
** enabled.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_INT_ENABLED )
EXTFUNC void ABCC_SYS_AbccInterruptEnable( void );
#endif

/*------------------------------------------------------------------------------
** Disable ABCC HW interrupt (IRQ_N pin on the application interface)
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_INT_ENABLED )
EXTFUNC void ABCC_SYS_AbccInterruptDisable( void );
#endif

/*------------------------------------------------------------------------------
** Enable the sync interrupt triggered by the sync pin on the application
** interface (MI0/SYNC).
** This function will be called by the driver when the sync interrupt shall be
** enabled.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
EXTFUNC void ABCC_SYS_SyncInterruptEnable( void );
#endif

/*------------------------------------------------------------------------------
** Disable sync interrupt
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_SYNC_ENABLE && ABCC_CFG_USE_ABCC_SYNC_SIGNAL )
EXTFUNC void ABCC_SYS_SyncInterruptDisable( void );
#endif

/*------------------------------------------------------------------------------
** Reset ABCC. Set the reset pin on the ABCC interface to low.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_HWReset( void );

/*------------------------------------------------------------------------------
** Release reset of ABCC. Sets the reset pin on the ABCC_ interface to high.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**-------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_HWReleaseReset( void );

/*------------------------------------------------------------------------------
** This function shall be able to read the interrupt signal from the ABCC. It is
** used to enable polling of interrupts if they should not be enabled.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Returns TRUE if an interrupt is active, otherwise it returns FALSE.
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
EXTFUNC BOOL ABCC_SYS_IsAbccInterruptActive( void );
#endif

/*------------------------------------------------------------------------------
** Read Module Identification pins on the host connector.
** If the identification pins are not connected the ABCC_CFG_MODULE_ID_PINS_CONN
** definition must be set to FALSE in abcc_drv_cfg.h. The definition
** ABCC_CFG_ABCC_MODULE_ID in abcc_dev_cfg.h must also be set with the correct
** value corresponding to the used device.
**
** Valid return values:
**    00b (0) Active CompactCom 30-series
**    01b (1) Passive CompactCom
**    10b (2) Active CompactCom 40-series
**    11b (3) Customer specific
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    Module identification value
**
**------------------------------------------------------------------------------
*/
#ifndef ABCC_CFG_ABCC_MODULE_ID
EXTFUNC UINT8 ABCC_SYS_ReadModuleId( void );
#endif

/*------------------------------------------------------------------------------
** Sets ABCC Operating Mode pins on the ABCC interface. If the operating mode is
** fixed the definition ABCC_CFG_ABCC_OP_MODE_X shall be set to the configured
** operating mode instead of implementing this function. If it is hardware
** configurable ABCC_CFG_OP_MODE_HW_CONF must be defined.
**------------------------------------------------------------------------------
** Arguments:
**    bOpMode - 1 SPI
**            - 2 Shift Register ( not supported )
**            - 3-6 Reserved
**            - 7 16 bit parallel
**            - 8 8 bit parallel
**            - 9 Serial 19.2 kbit/s
**            - 10 Serial 57.6 kbit/s
**            - 11 Serial 115.2 kbit/s
**            - 12 Serial 625 kbit/s
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_OP_MODE_SETTABLE )
EXTFUNC void ABCC_SYS_SetOpmode( UINT8 bOpMode );
#endif

/*------------------------------------------------------------------------------
** Read the configured operating mode to be used from hardware. It could be
** either e.g. a switch or the operating mode pins of the host connector.
** This function needs to be implemented if the operating mode is hardware
** configurable.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    bOpMode - 1 SPI
**            - 2 Shift Register ( not supported )
**            - 3-6 Reserved
**            - 7 16 bit parallel
**            - 8 8 bit parallel
**            - 9 Serial 19.2 kbit/s
**            - 10 Serial 57.6 kbit/s
**            - 11 Serial 115.2 kbit/s
**            - 12 Serial 625 kbit/s
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_OP_MODE_GETTABLE )
EXTFUNC UINT8 ABCC_SYS_GetOpmode( void );
#endif

/*------------------------------------------------------------------------------
** Detects if a module is present by reading the Module Detection pins on the
** ABCC interface.
** If the ABCC Module detection pins are not connected
** ABCC_CFG_MOD_DETECT_PINS_CONN must be defined.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE  - Module is detected.
**    FALSE - Module is not detected.
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
EXTFUNC BOOL ABCC_SYS_ModuleDetect( void );
#endif

/*------------------------------------------------------------------------------
** This function is called by the driver from the ABCC_HwInit() interface.
** If there is any hardware or system dependent initialization required
** to be done at the power up initialization it shall be done here.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE  - Initialization succeeded.
**    FALSE - Initialization failed.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_SYS_HwInit( void );

/*------------------------------------------------------------------------------
** This function is used to measure sync timings. ABCC_CFG_SYNC_MEASUREMENT_OP
** is used when measuring the output processing time and
** ABCC_CFG_SYNC_MEASUREMENT_IP is used to measure the input processing time.
** It should reset an output signal that can be measured to ascertain
** aforementioned sync times.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if ( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
EXTFUNC void ABCC_SYS_GpioReset( void );
#endif

/*------------------------------------------------------------------------------
** This function is used to measure sync timings. ABCC_CFG_SYNC_MEASUREMENT_OP
** is used when measuring the output processing time and
** ABCC_CFG_SYNC_MEASUREMENT_IP is used to measure the input processing time.
** It should set an output signal that can be measured to ascertain
** aforementioned sync times.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if ( ABCC_CFG_SYNC_MEASUREMENT_OP || ABCC_CFG_SYNC_MEASUREMENT_IP )
EXTFUNC void ABCC_SYS_GpioSet( void );
#endif

/*------------------------------------------------------------------------------
** This function is called by the driver at the beginning ABCC_StartDriver().
** If there is any hardware specific tasks required to be done every time the
** driver starts it shall be done here. Note that ABCC_StartDriver() will also
** be called during restart of the driver.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    TRUE  - Initialization succeeded.
**    FALSE - Initialization failed.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_SYS_Init( void );

/*------------------------------------------------------------------------------
** Called from driver at the end of ABCC_ShutDown(). Any hardware specific
** tasks that is required to be done every time the driver is stopped it shall
** be done here. Note that the driver could be started  again by calling
** ABCC_StartDriver().
**------------------------------------------------------------------------------
** Arguments:
**    None
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_Close( void );

#endif  /* inclusion lock */
