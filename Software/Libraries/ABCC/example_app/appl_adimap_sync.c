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
** Example of how to handle ADI values in a sync application.
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
**
** In abcc_obj_cfg.h make sure that the following definitions are set to:
** SYNC_OBJ_ENABLE     ( TRUE )
**
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc.h"
#include "appl_adi_config.h"
#include "appl_abcc_handler.h"


#if( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SYNC )

#if(  ABCC_CFG_STRUCT_DATA_TYPE || ABCC_CFG_ADI_GET_SET_CALLBACK )
   #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to FALSE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
#endif

#if( !SYNC_OBJ_ENABLE )
   #error SYNC_OBJ_ENABLE must be set to TRUE in abcc_obj_cfg.h.h
#endif


#if( !ABCC_CFG_SYNC_ENABLE )
   #error ABCC_CFG_SYNC_ENABLE in abcc_drv_cfg.h must be enabled
#endif


/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_READ_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_READ_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_SET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |            \
                                        ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_SET_ACCESS )

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Private Globals
********************************************************************************
*/
static void triggerAdiSyncOutputValid( void );
static void triggerAdiSyncInputCapture( void );


/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/

/*
** Latest received read process data linked to the ADI entry table.
*/
static UINT32 appl_lPendingOutput;

/*
** Represent the current value used by the hardware actuated at the latest sync
** pulse.
*/
static UINT32 appl_lSyncOutput;

/*
** Represent the current input from the HW.
*/
static UINT32 appl_lSensorValue;

/*
** Represent the sampled appl_lSensorInput at the sync pulse. The variable is
** linked to the ADI entry table.
*/
static UINT32 appl_lSyncInput;


/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
**  32 bit input/output.
**------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   { 50,  "SyncOutput",   ABP_UINT32,   1, APPL_READ_MAP_WRITE_ACCESS_DESC,  { { &appl_lPendingOutput, NULL } } },
   { 51,  "SyncInput",    ABP_UINT32,   1, APPL_WRITE_MAP_READ_ACCESS_DESC,  { { &appl_lSyncInput, NULL } } },
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 50, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 51, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_DEFAULT_MAP_END_ENTRY }
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** When sync is used this function is called to indicate that the read
** process data ADI:s shall be copied to the synchronized outputs.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void triggerAdiSyncOutputValid( void )
{
   /*
   **  Copy the pending output to the SyncOutput.
   **  In a real case the appl_lSyncOutput is not variable but a function
   **  controlling e.g. a motor.
   */
   appl_lSyncOutput = appl_lPendingOutput;
   (void)appl_lSyncOutput;
}

/*------------------------------------------------------------------------------
** If sync is used this function is called to indicate that write process data
** ADI:s shall be updated with the captured input.
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
static void triggerAdiSyncInputCapture( void )
{
   /*
   **  Copy the sensor to the sync input.
   **  appl_lSensorValue represent for example a measured speed.
   **
   */
   appl_lSyncInput = appl_lSensorValue;

   /*
   ** Always update the ABCC with the latest write process data at the end of
   ** this function.
   */
   ABCC_TriggerWrPdUpdate();
}


void APPL_SyncIsr( void )
{
   /*
   ** This is the the start of the sync cycle. This point is as close to the
   ** SYNC Input Capture Point as this example gets. The measurement pin for
   ** input processing measurements is set here.
   ** Optimally this measurement should be done on the interrupt pin, but the
   ** GPIO needs to be toggled so that ABCC_GpioReset() can cause a sloping
   ** flank at the end point of input processing.
   */
#if ABCC_CFG_SYNC_MEASUREMENT_IP
   ABCC_GpioSet();
#endif

   /*
   ** PORTING ALERT!
   ** Some applications require a PLL being locked to the sync signal before
   ** PROCESS_ACTIVE is entered. It is also possible to do measurement to verify
   ** the cycle time before allowing PROCESS_ACTIVE.
   ** Only ABP_APPSTAT_NO_ERROR will allow the ABCC to enter PROCESS_ACTIVE.
   **
   ** In those cases this call must be relocated.
   ** In this example PROCESS_ACTIVE will be allowed as soon as the first sync
   ** interrupt appears.
   */
   if( ABCC_GetAppStatus() != ABP_APPSTAT_NO_ERROR )
   {
      if( ABCC_GetAppStatus() == ABP_APPSTAT_NOT_SYNCED )
      {
         ABCC_SetAppStatus( ABP_APPSTAT_NO_ERROR );
      }
      else
      {
         return;
      }
   }

   /*
   ** PORTING ALERT!
   ** The OutputValidTime attribute in the sync object defines the time in
   ** nano seconds that shall be waited from this point before actuating the ADI
   ** values to the synchronized output.
   ** This means that a timer shall be started here and when it expires
   ** triggerAdiSyncOutputValid() shall be called.
   ** In this example the output valid time is ignored and the function is
   ** called directly (OutputValidTime = 0).
   */
   triggerAdiSyncOutputValid();

   /*
   ** PORTING ALERT!
   ** The InputCaptureTime attribute in the sync object defines the time in
   ** nano seconds that shall be waited from this point before capturing the
   ** input data and send it to the ABCC.
   ** This means that a timer shall be started here, and when it expires
   ** triggerAdiSyncInputCapture() shall be called.
   ** In this example the input capture  time is ignored and the
   ** function is called directly (InputCaptureTime = 0).
   */
   triggerAdiSyncInputCapture();
}



UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}

void APPL_CyclicalProcessing( void )
{
   /*
   ** This function is called when read and write data have been updated. It
   ** could for example be used for operations on the ADI data.
   ** Not used in this example.
   */
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
