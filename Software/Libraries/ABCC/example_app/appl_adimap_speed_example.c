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
** Example of an ADI setup with two simple UINT16 ADIs simulating speed and
** reference speed.
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"
#include "abcc.h"

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SPEED_EXAMPLE )

#if (  ABCC_CFG_STRUCT_DATA_TYPE || ABCC_CFG_ADI_GET_SET_CALLBACK )
   #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to FALSE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
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

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
static UINT16 appl_iSpeed;
static UINT16 appl_iRefSpeed;

/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT16Type appl_sUint16Prop = { { 0, 0xFFFF, 0 } };

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  0x1,  "SPEED",     ABP_UINT16,   1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iSpeed,    &appl_sUint16Prop } } },
   {  0x2,  "REF_SPEED", ABP_UINT16,   1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_iRefSpeed, &appl_sUint16Prop } } }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 1, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 2, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
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

UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}

void APPL_CyclicalProcessing( void )
{
   if( ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE )
   {
      /*
      ** An example of ADI data handling.
      */
      if( appl_iSpeed > appl_iRefSpeed )
      {
         /*
         ** Do something that lowers speed.
         */
         appl_iSpeed -= 1;
      }
      else if( appl_iSpeed < appl_iRefSpeed )
      {
         /*
         ** Do something that increases speed.
         */
         appl_iSpeed += 1;
      }
   }
   else
   {
      /*
      ** We are not in process active, the default should be that the motor
      ** should not run.
      */
      appl_iSpeed = 0;
   }
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
