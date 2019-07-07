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
** Target Platform:                                                           **
**    STM3240G-EVAL Board                                                     **
**                                                                            **
** Platform BSP Version:                                                      **
**    STM32Cube FW_F4 V1.7.0                                                  **
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
** Example of an ADI setup with 8 ADIs that interface with various peripheral
** that exist on the STM32 Eval Board as well as the Option Card. These
** peripherals include: 3 HEX Switches, 12 LEDs, 8 I/Os (all used as outputs)
** 1 Momentary Pushbutton, and 1 Potentiometer. By defining __USE_LCD in the
** project settings, the STM3240G-EVAL board's LCD screen can display some of
** variables in this demo.
**
** In abcc_drv_cfg.h make sure that the following definitions are set to:
** ABCC_CFG_STRUCT_DATA_TYPE     ( FALSE )
** ABCC_CFG_ADI_GET_SET_CALLBACK ( TRUE )
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"
#include "stm32f4xx_hal.h"
#ifdef __USE_LCD
#include "GLCD.h"
#endif

#if ( APPL_ACTIVE_ADI_SETUP == APPL_ADI_SETUP_BOARD_SPECIFIC )

#if( ABCC_CFG_STRUCT_DATA_TYPE || !ABCC_CFG_ADI_GET_SET_CALLBACK  )
   #error "ABCC_CFG_ADI_GET_SET_CALLBACK must be set to TRUE and ABCC_CFG_STRUCT_DATA_TYPE set to FALSE in order to run this example"
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
static void appl_GetTim5Cnt( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void appl_Get3HexSw( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void appl_Set4Leds( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void appl_Set8Leds8IOs( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void appl_GetKey( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );
static void appl_GetPot( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex );

/*------------------------------------------------------------------------------
** Data holder for the ADI instances
**------------------------------------------------------------------------------
*/

static UINT32  appl_iRdCnt = 0;    /* Simple counter that increments each time one of the "get" callbacks is called */
static UINT32  appl_iWrCnt = 0;    /* Simple counter that increments each time one of the "set" callbacks is called */
static UINT32  appl_lTimer = 0;    /* Contains the TIM5 counter */
static UINT16  appl_i3HexSwitches; /* Contains the state of the 3 I2C Hex Switches on the Option Card */
static UINT8   appl_b4Leds;        /* Contains the state of the 4 LEDs on the Option Card */
static UINT16  appl_i8Leds8IOs;    /* Contains the state of the 8 LEDs + 4 I/Os on the Option Card */
static BOOL    appl_fKey;          /* Contains the state of the "Key" pushbutton on the host EVAL-Board */
static FLOAT32 appl_rPot;          /* Contains the value of the potentiometer on the host EVAL-Board */

/*******************************************************************************
** Public Globals
********************************************************************************
*/
extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc3;

/*------------------------------------------------------------------------------
** Various hardware and software variables
** See abcc_ad_if.h for a more detailed description.
**------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   {  100,   "Read TIM5",                  ABP_UINT32,   1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_lTimer        ,NULL } }, appl_GetTim5Cnt,     NULL },
   {  101,   "Read RD Counter",            ABP_UINT16,   1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iRdCnt        ,NULL } }, NULL,                NULL },
   {  102,   "Read WR Counter",            ABP_UINT16,   1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_iWrCnt        ,NULL } }, NULL,                NULL },
   {  103,   "Read 3 Hex Switches",        ABP_UINT16,   1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_i3HexSwitches ,NULL } }, appl_Get3HexSw,      NULL },
   {  104,   "Read/Write 4 LEDs",          ABP_UINT8,    1,  APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_b4Leds        ,NULL } }, NULL,                appl_Set4Leds },
   {  105,   "Read/Write 8 LEDs + 8 I/Os", ABP_UINT16,   1,  APPL_READ_MAP_WRITE_ACCESS_DESC, { { &appl_i8Leds8IOs    ,NULL } }, NULL,                appl_Set8Leds8IOs },
   {  106,   "Read Key",                   ABP_BOOL,     1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_fKey          ,NULL } }, appl_GetKey,         NULL },
   {  107,   "Read Potentiometer",         ABP_FLOAT,    1,  APPL_WRITE_MAP_READ_ACCESS_DESC, { { &appl_rPot          ,NULL } }, appl_GetPot,         NULL },
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
   { 100,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 101,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 102,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 103,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 104,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 105,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 106,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 107,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_DEFAULT_MAP_END_ENTRY }
};
/*******************************************************************************
** Private Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Callback of type ABCC_GetAdiValueFuncType. The function will be called when
** the network reads ADI #100. It will poll the TIM5's current counter value.
** This value will be copied to ADI #100 every time this is done.
**
** ABCC_GetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_GetTim5Cnt( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
   #ifdef __USE_LCD   
   /*
   ** Temp string including NULL termination
   */
   char text[11] = {'\0'};
   #endif
   appl_lTimer = TIM5->CNT;
   #ifdef __USE_LCD
   snprintf(text, sizeof(text), "0x%08X", appl_lTimer);
   GLCD_DisplayString(9, 9, 1, (unsigned char *)text);
   #endif
   appl_iRdCnt++;
}


/*------------------------------------------------------------------------------
** Callback of type ABCC_GetAdiValueFuncType. The function will be called when
** the network reads ADI #103. It will poll the state of the HEX switches on the
** "Option Card" that are connected via I2C. This value will be copied to the
** ADI#103 every time this is done.
**
** ABCC_GetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_Get3HexSw( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
  UINT8 abData[2] = {0xFF, 0x0F};
  UINT16 iVal = 0, iMask = 1;
  UINT8 bCnt;

  /* Read the inputs */
  HAL_I2C_Master_Receive(&hi2c1, 0x40 , abData, 2, 1000);

  /* Nibble flip the hex switches to present them in left-to-right reading order */
  for(bCnt=0;bCnt<3;bCnt++)
  {
    iVal = ((iVal<<4) | (((*((UINT16*)abData))>>(bCnt*4))&0x0F));
    iMask *= 256;
  }

  appl_i3HexSwitches = iVal&(iMask-1);
  appl_iRdCnt++;
}


/*------------------------------------------------------------------------------
** Callback of type ABCC_SetAdiValueFuncType. The function will be called when
** the network writes ADI #104. It will set the state of the 4 LEDs (shared with
** with the 3 HEX switches) on the "Option Card" that are connected via I2C.
** This value will be copied to the ADI#104 every time this is done.
**
** ABCC_SetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_Set4Leds( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
  static UINT8 bLedStatusLast = 0xF0;

  if(bLedStatusLast != appl_b4Leds)
  {
    UINT8 abData[2] = {0xFF, 0x0F};
    #ifdef __USE_LCD
    /*
    ** Temp string including NULL termination
    */
    char text[4];
    #endif
    bLedStatusLast = appl_b4Leds;

    (*(UINT16*)abData) = ~(((UINT16)(appl_b4Leds&0x0F))<<12);
    HAL_I2C_Master_Transmit(&hi2c1, 0x40 , abData, 2, 1000);

    #ifdef __USE_LCD
    snprintf(text, sizeof(text), "0x%1X", appl_b4Leds & 0x0F );
    GLCD_DisplayString(6, 13, 1, (unsigned char *)text);
    #endif
  }
  appl_iWrCnt++;
}


/*------------------------------------------------------------------------------
** Callback of type ABCC_SetAdiValueFuncType. The function will be called when
** the network writes ADI #105. It will set the state of 8 LEDs and 8 I/Os on the
** "Option Card" that are connected via I2C. This value will be copied to the
** ADI#105 every time this is done.
**
** ABCC_SetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_Set8Leds8IOs( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
  static UINT16 iLastState = 0xF0F0;

  if((iLastState != appl_i8Leds8IOs))
  {
    UINT8 abData[2];
    #ifdef __USE_LCD
    /*
    ** Temp string including NULL termination
    */
    char text[5];
    #endif
    iLastState = appl_i8Leds8IOs;
    /* LEDs are active low but I/Os are active high.
    ** Logically invert "just" the LEDs to be consistent */
    (*(UINT16*)abData) = (appl_i8Leds8IOs^0x00FF) | (appl_i8Leds8IOs&0xFF00);
    HAL_I2C_Master_Transmit(&hi2c1, 0x44 , abData, 2, 1000);

    #ifdef __USE_LCD
    sprintf(text, "0x%02X", (UINT8)((appl_i8Leds8IOs&0x00FF)));
    GLCD_DisplayString(7, 13, 1, (unsigned char *)text);
    snprintf(text, sizeof(text), "0x%02X", (UINT8)((appl_i8Leds8IOs)>>8));
    GLCD_DisplayString(8, 13, 1, (unsigned char *)text);
    #endif
  }
  appl_iWrCnt++;
}


/*------------------------------------------------------------------------------
** Callback of type ABCC_GetAdiValueFuncType. The function will be called when
** the network reads ADI #106. It will poll the state of the EVAL-Board's "Key"
** push-button. This value will be copied to the ADI#106 every time this is done.
**
** ABCC_GetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_GetKey( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
  /* Invert the logic since the 'Key' input is active low */
  if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_15) == GPIO_PIN_SET)
  {
    appl_fKey = FALSE;
    #ifdef __USE_LCD
    GLCD_DisplayString(5, 13, 1, (unsigned char *)"FALSE");
    #endif
  }
  else
  {
    appl_fKey = TRUE;
    #ifdef __USE_LCD
    GLCD_SetBackColor(Red);
    GLCD_SetTextColor(White);
    GLCD_DisplayString(5, 13, 1, (unsigned char *)"TRUE");
    GLCD_SetBackColor(White);
    GLCD_SetTextColor(Red);
    GLCD_DisplayString(5, 17, 1, (unsigned char *)" ");
    #endif
  }
  appl_iRdCnt++;
}

/*------------------------------------------------------------------------------
** Callback of type ABCC_GetAdiValueFuncType. The function will be called when
** the network reads ADI #107. It will poll the ADC value of the EVAL-Board's
** potentiometer. This value is converted to a 32-bit floating point value and
** will be copied to the ADI#107 every time this is done.
**
** ABCC_GetAdiValueFuncType is declared in abcc_ad_if.h
**------------------------------------------------------------------------------
*/
static void appl_GetPot( const struct AD_AdiEntry* psAdiEntry, UINT8 bNumElements, UINT8 bStartIndex )
{
  UINT16 iRawAdcVal;
  /* For simplicity of this demo, we interact with the ADC in a blocking-polling mode.
  ** There are better alternatives to this such as DMA or interrupt driven */
  HAL_ADC_Start(&hadc3);

  while( HAL_BUSY == HAL_ADC_PollForConversion(&hadc3, 1000) ){};

  iRawAdcVal = HAL_ADC_GetValue(&hadc3);
  /* Convert the value into a 32-bit floating point
  ** number whose scale ranges from 0.0f to 3.3f*/
  appl_rPot = ((FLOAT32)iRawAdcVal*3.3f/4095);
  HAL_ADC_Stop(&hadc3);

  #ifdef __USE_LCD
  /* Write to the LCD screen */
  GLCD_SetBackColor(LightGrey);
  GLCD_Bargraph (9 * 16, 4 * 24, 10 * 16, 24 - 2, (iRawAdcVal>>2));
  GLCD_SetBackColor(White);
  #endif
  appl_iRdCnt++;
}


/*******************************************************************************
** Public Services
********************************************************************************
*/

void APPL_CyclicalProcessing( void )
{
   /*
   ** This function is called when read and write data have been updated. It
   ** could for example be used for operations on the ADI data.
   ** Not used in this example.
   */
}

UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}
/*******************************************************************************
** Tasks
********************************************************************************
*/
#endif
