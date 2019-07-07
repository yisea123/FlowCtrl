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
** Example of an ADI setup with 32 ADIs each one holding a 16 bit value.
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( TRUE )
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SEPARATE_16 )

#if( ABCC_CFG_STRUCT_DATA_TYPE || !ABCC_CFG_ADI_GET_SET_CALLBACK  )
   #error ABCC_CFG_ADI_GET_SET_CALLBACK must be set to TRUE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example
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
** Forward declarations
**------------------------------------------------------------------------------
*/
static void SetAdi10Value( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void GetAdi11Value( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/
static UINT16  appl_aiUint16_10[ 32 ];
static UINT16  appl_aiUint16_11[ 32 ];
static UINT16  appl_Uint16_12 = 0;
/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** 32 16-bit values individually
** See abcc_ad_if.h for a more detailed description.
**------------------------------------------------------------------------------
*/
/*-----------------------------------------------------------------------------------------------------------------------
** iInstance | pabName | bDataType | bNumOfElements | bDesc | pxValuePtr | pxValuePropPtr| pnGetAdiValue | pnSetAdiValue
**-----------------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   { 10,   "ABP_UINT16_SET",        ABP_UINT16,   32, APPL_READ_MAP_WRITE_ACCESS_DESC, { { appl_aiUint16_10 ,NULL } }, NULL, SetAdi10Value },//有回调函数  NULL, SetAdi10Value 
   { 11,   "ABP_UINT16_GET",        ABP_UINT16,   32, APPL_WRITE_MAP_READ_ACCESS_DESC, { { appl_aiUint16_11 ,NULL } }, GetAdi11Value, NULL },
   { 12,   "ABP_UINT16_COUNTER",    ABP_UINT16,   1,  APPL_NOT_MAP_WRITE_ACCESS_DESC,  { { &appl_Uint16_12 ,NULL  } }, NULL, NULL         },
};

/*
**------------------------------------------------------------------------------
** Map all adi:s in both directions
** See abcc_ad_if.h for a more detailed description.
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 10,  PD_READ,   AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 11,  PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_DEFAULT_MAP_END_ENTRY }
};
/*******************************************************************************
** Private Services
********************************************************************************
*/
/*------------------------------------------------------------------------------
** Callback of type ABCC_GetAdiValueFuncType. The function will be called when
** the network reads ADI #11. It will increment the value of ADI#12 every time
** this is done.
**
** ABCC_GetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void GetAdi11Value( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
   appl_Uint16_12++;
}
/*------------------------------------------------------------------------------
** Callback of type ABCC_SetAdiValueFuncType. The function will be called when
** the network writes to ADI#10. It copies the changed values from ADI#10 to ADI#11.
**
** ABCC_SetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void SetAdi10Value( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
   UINT8 index;
   for( index = bStartIndex ; index < bStartIndex+bNumElements ; index++ )
   {
      appl_aiUint16_11[ index ] = appl_aiUint16_10[ index ];
   }
}
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

