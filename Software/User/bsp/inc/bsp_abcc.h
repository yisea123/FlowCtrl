/*
*********************************************************************************************************
*
*	模块名称 : ABCC模块 
*	文件名称 : bsp_abcc.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#ifndef __BSP_ABCC_H
#define __BSP_ABCC_H

#include <stdint.h>
#include "abcc_td.h"
#include "abcc.h"
#include "abcc_sys_adapt.h"
#include "ad_obj.h"
#include "appl_abcc_handler.h"
#include "appl_adi_config.h"

#define APPL_TIMER_MS         5
#define USE_TIMER_INTERRUPT   0

/* 供外部调用的函数声明 */
/* 定义SPI总线的 GPIO端口 */
#define SPI1_RCC_SCK 	RCC_APB2Periph_GPIOA
#define SPI1_PORT_SCK	GPIOA
#define SPI1_PIN_SCK	GPIO_Pin_5

#define SPI1_RCC_MISO 	RCC_APB2Periph_GPIOA
#define SPI1_PORT_MISO	GPIOA
#define SPI1_PIN_MISO	GPIO_Pin_6

#define SPI1_RCC_MOSI 	RCC_APB2Periph_GPIOA
#define SPI1_PORT_MOSI	GPIOA
#define SPI1_PIN_MOSI	GPIO_Pin_7

#define SPI1_RCC_CS 	RCC_APB2Periph_GPIOA
#define SPI1_PORT_CS	GPIOA
#define SPI1_PIN_CS		GPIO_Pin_4

#define SPI1_CS_0()		SPI1_PORT_CS->BRR = SPI1_PIN_CS
#define SPI1_CS_1()		SPI1_PORT_CS->BSRR = SPI1_PIN_CS

#define ABCC_RST_GPIO   GPIOB
#define ABCC_RST_PIN    GPIO_Pin_5

#define ABCC_RST_0()	ABCC_RST_GPIO->BRR = ABCC_RST_PIN
#define ABCC_RST_1()	ABCC_RST_GPIO->BSRR = ABCC_RST_PIN

int bsp_abccInit(void);
uint8_t HAL_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);


#endif

/*****************************************  (END OF FILE) **********************************************/


