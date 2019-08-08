/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.03.02 (2017-03-28)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.39.01 (2017-03-22)                                     **
**    ABCC Driver    5.03.02 (2017-03-28)                                     **
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
** Example of an ADI setup with an array of 16 bit values
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( FALSE )
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_SIMPLE_16 )

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
UINT8  	OkToWeld = 1;       /* 0:fail 1:ok */
UINT8 	ValueClosed = 0;   /* 0:open, 1:close */
UINT8 	Bypass = 0;          /* 0:null 1:bypass */
UINT8  	MinimalFlow = 0;     /* 0: Flow is minimal below minflow */
UINT8 	Leak = 0;            /* 0:Null 1:Cap lost */
UINT8   ValueFault = 0;      /* 0:Null 1:Fault */
UINT8   FlowSensorFault = 0; /* 0:ok 1:fault */
UINT8   PowerOK = 1; /* 0:fault  1:ok */
UINT8   FlowWarning = 0; /* 0:ok  1:Warning */
UINT8   FlowFault = 0;   /* 0:ok  1:Fault */
UINT8   FlowOff = 0;     /* 0:ok  1:off  */
UINT8   OKToExchangeCap = 0; /* 0:fail  1:ok */
UINT8   UserDI01 = 0;
UINT8   UserDI02 = 0;
UINT8   UserDI03 = 0;
UINT8   UserDI04 = 0;        
UINT8   SupplyFlowInLPM = 0;   /* = 模拟量 *10  进水流量 */
UINT8   ReturnFlowInLPM = 0;   /* = 模拟量 *10  回水流量 */
UINT8   FlowWarningValue = 0;  /* = 模拟量 *10  设定的流量报警参数  */
UINT8   FlowFaultValue = 0;    /* = 模拟量 *10  设定的流量错误参数 */
UINT8   LeakResponseValue = 0; /* = 模拟量 *10  设定的泄露检测持续参数 */
UINT8   DelayToDetect = 0;     /* = 单位s   设定阀门打开之后的延时检测参数 */
UINT8   LeakFlowDifference =0; /* = 模拟量 *10  设定的泄露的流量差值测参数 */

UINT8   SetFlowReset = 0;        /* 0:Null 1:Reset */
UINT8   SetCloseValue = 0;       /* 0:Open 1:Close */
UINT8   SetByPass = 0;         
UINT8   SetExchangeCapReset = 0; /* 0:Null 1:Exchange */
UINT8   SetParameterRequest=0;   /* 0:Null 1:Set */
UINT8   Reserve1 = 0;
UINT8   Reserve2 = 0;
UINT8   Reserve3 = 0;
UINT8   SetFlowWarningValue = 0; /* = 模拟量 *10   */
UINT8   SetFlowFaultValue = 0;   /* = 模拟量 *10   */
UINT8   SetLeakResponseValue = 0;/* = 模拟量 *10   */
UINT8   SetDelayToDetect = 0;    /* = 单位为s    */
UINT8   SetLeakFlowDifference =0;/* = 模拟量 *10   */


/*------------------------------------------------------------------------------
** Min, max and default value for appl_aiUint16
**------------------------------------------------------------------------------
*/
static AD_UINT8Type 	appl_sUint8Prop  = { { 0,  0xFF, 0 } };
static AD_UINT16Type 	appl_sUint16Prop = { { 0, 0xFFFF, 0 } };
static AD_UINT32Type 	appl_sUint32Prop = { { 0, 0xFFFFFFFF, 0 } };

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*------------------------------------------------------------------------------
** 32 16-bit values as an array
**------------------------------------------------------------------------------
*/

/*-------------------------------------------------------------------------------------------------------------
** 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6. pxValuePtr | 7. pxValuePropPtr
**--------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
//	{ 0x1, "ControlWord", ABP_UINT16, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &CtrolWord, &appl_sUint16Prop } } },  /* read from plc */
//	{ 0x2, "SpeedRef",    ABP_UINT32, 1, APPL_READ_MAP_WRITE_ACCESS_DESC,{ { &SpeedRef, &appl_sUint32Prop } } },
//	{ 0x3, "StatusWord",  ABP_UINT16, 1, APPL_WRITE_MAP_READ_ACCESS_DESC,{ { &StatusWord, &appl_sUint16Prop } } },  /* write to plc */

	{ 0x1, "OkToWeld",          ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &OkToWeld,          &appl_sUint8Prop } } },
	{ 0x2, "ValueClosed",       ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &ValueClosed,       &appl_sUint8Prop } } },
	{ 0x3, "Bypass",            ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Bypass,            &appl_sUint8Prop } } },
	{ 0x4, "MinimalFlow",       ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &MinimalFlow,       &appl_sUint8Prop } } },
	{ 0x5, "Leak",              ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &Leak,              &appl_sUint8Prop } } },
	{ 0x6, "ValueFault",        ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &ValueFault,        &appl_sUint8Prop } } },
	{ 0x7, "FlowSensorFault",   ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowSensorFault,   &appl_sUint8Prop } } },
	{ 0x8, "PowerOK",           ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &PowerOK,           &appl_sUint8Prop } } }, 
	{ 0x9, "FlowWarning",       ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowWarning,       &appl_sUint8Prop } } },
	{ 0x0A,"FlowFault",         ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowFault,         &appl_sUint8Prop } } },
	{ 0x0B,"FlowOff",           ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowOff,           &appl_sUint8Prop } } },
	{ 0x0C,"OKToExchangeCap",   ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &OKToExchangeCap,   &appl_sUint8Prop } } },
	{ 0x0D,"UserDI01",          ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &UserDI01,          &appl_sUint8Prop } } },
	{ 0x0E,"UserDI02",          ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &UserDI02,          &appl_sUint8Prop } } },
	{ 0x0F,"UserDI03",          ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &UserDI03,          &appl_sUint8Prop } } },
	{ 0x10,"UserDI04",          ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &UserDI04,          &appl_sUint8Prop } } },
	{ 0x11,"SupplyFlowInLPM",   ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &SupplyFlowInLPM,   &appl_sUint8Prop } } },
	{ 0x12,"ReturnFlowInLPM",   ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &ReturnFlowInLPM,   &appl_sUint8Prop } } },
	{ 0x13,"FlowWarningValue",  ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowWarningValue,  &appl_sUint8Prop } } },
	{ 0x14,"FlowFaultValue",    ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &FlowFaultValue,    &appl_sUint8Prop } } },
	{ 0x15,"LeakResponseValue", ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &LeakResponseValue, &appl_sUint8Prop } } },
	{ 0x16,"DelayToDetect",     ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &DelayToDetect,     &appl_sUint8Prop } } },
	{ 0x17,"LeakFlowDifference",ABP_UINT8, 1, APPL_WRITE_MAP_READ_ACCESS_DESC, { { &LeakFlowDifference,&appl_sUint8Prop } } },


	{ 0x18,"SetFlowReset",		    ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetFlowReset,		   &appl_sUint8Prop } } },
	{ 0x19,"SetCloseValue",         ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetCloseValue,        &appl_sUint8Prop } } },
	{ 0x1A,"SetByPass",             ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetByPass,            &appl_sUint8Prop } } },
	{ 0x1B,"SetExchangeCapReset",   ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetExchangeCapReset,  &appl_sUint8Prop } } },
	{ 0x1C,"SetParameterRequest",   ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetParameterRequest,  &appl_sUint8Prop } } },
	{ 0x1D,"Reserve1",	            ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &Reserve1,             &appl_sUint8Prop } } },
	{ 0x1E,"Reserve2",              ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &Reserve1,             &appl_sUint8Prop } } },
	{ 0x1F,"Reserve3", 	            ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &Reserve1,	           &appl_sUint8Prop } } },
	{ 0x20,"SetFlowWarningValue",   ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetFlowWarningValue,  &appl_sUint8Prop } } },
	{ 0x21,"SetFlowFaultValue",     ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetFlowFaultValue,    &appl_sUint8Prop } } },
	{ 0x22,"SetLeakResponseValue",  ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetLeakResponseValue, &appl_sUint8Prop } } },
	{ 0x23,"SetDelayToDetect",      ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetDelayToDetect,     &appl_sUint8Prop } } },
	{ 0x24,"SetLeakFlowDifference", ABP_UINT8,  1, APPL_READ_MAP_WRITE_ACCESS_DESC, { { &SetLeakFlowDifference,&appl_sUint8Prop } } }
};

/*------------------------------------------------------------------------------
** Map all adi:s in both directions   周期性数据
**------------------------------------------------------------------------------
** 1. AD instance | 2. Direction | 3. Num elements | 4. Start index |
**------------------------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
//    { 1, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
//	{ 2, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

	{ 1,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 2,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 3,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 4,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 5,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 6,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 7,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 8,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 9,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 10, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 11, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 12, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 13, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 14, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 15, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 16, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 17, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 18, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 19, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 20, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 21, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 22, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 23, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	
	{ 24, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 25, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 26, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 27, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 28, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 29, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 30, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 31, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 32, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 33, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 34, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },
	{ 35, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },	
	{ 36, PD_READ, AD_DEFAULT_MAP_ALL_ELEM, 0 },

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
