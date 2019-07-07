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
*/

#include "abcc_drv_cfg.h"
#include "abcc_port.h"
#include "abcc_sys_adapt.h"
#include "abcc_sys_adapt_spi.h"
#include "abcc_sys_adapt_par.h"
#include "abcc_sys_adapt_ser.h"

#include "bsp.h"

#if( ABCC_CFG_DRV_SPI )
/*
** Callback for SPI data ready indication.
*/
static ABCC_SYS_SpiDataReceivedCbfType pnDataReadyCbf;
#endif


#if( ABCC_CFG_DRV_SPI )
static void ABCC_SYS_SpiInit(void)
{
   
}
#endif


BOOL ABCC_SYS_HwInit( void )
{
	return TRUE;
}

BOOL ABCC_SYS_Init( void )
{
   return TRUE;
}

void ABCC_SYS_Close( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
}

#if( ABCC_CFG_OP_MODE_SETTABLE )
void ABCC_SYS_SetOpmode( UINT8 bOpmode )
{
   
}
#endif

void ABCC_SYS_HWReset( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	ABCC_RST_0();
	OSTimeDly( 5 );
}

void ABCC_SYS_HWReleaseReset( void )
{
   /*
   ** Implement according to abcc_sys_adapt.h
   */
    GPIO_SetBits(GPIOB,GPIO_Pin_5);
	ABCC_RST_1();
	OSTimeDly( 5 );
}


#ifndef ABCC_CFG_ABCC_MODULE_ID
UINT8 ABCC_SYS_ReadModuleId( void )
{
   UINT8 bRet = 0x00;

   if( HAL_GPIO_ReadPin( ABCC_SYS_MI0 ) == GPIO_PIN_SET )
   {
      bRet |= 0x01;
   }

   if( HAL_GPIO_ReadPin( ABCC_SYS_MI1 ) == GPIO_PIN_SET )
   {
      bRet |= 0x02;
   }

   return bRet;
}
#endif

#if( ABCC_CFG_MOD_DETECT_PINS_CONN )
BOOL ABCC_SYS_ModuleDetect( void )
{
  
}
#endif

#if( ABCC_CFG_INT_ENABLED )
void ABCC_SYS_AbccInterruptEnable( void )
{
   
}


void ABCC_SYS_AbccInterruptDisable( void )
{
   
}
#endif


#if( ABCC_CFG_POLL_ABCC_IRQ_PIN )
BOOL ABCC_SYS_IsAbccInterruptActive( void )
{

}
#endif


#if( ABCC_CFG_DRV_SPI )
static ABCC_SYS_SpiDataReceivedCbfType pnDataReadyCbf;


void ABCC_SYS_SpiRegDataReceived( ABCC_SYS_SpiDataReceivedCbfType pnDataReceived  )
{
   pnDataReadyCbf = pnDataReceived;
}

void ABCC_SYS_SpiSendReceive( void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength )
{
  /*
   ** Implement according to abcc_sys_adapt_spi.h
   */
	   /*
   ** Implement according to abcc_sys_adapt_spi.h
   */
	 uint8_t ret;


   ret = HAL_SPI_TransmitReceive( ( uint8_t* )pxSendDataBuffer, ( uint8_t* )pxReceiveDataBuffer, iLength, 1000 );


   if( ret != 0 )
   {
      /* Error */
      return;
   }
   /* call back function for SPI recieve */
   pnDataReadyCbf();
}

#endif
