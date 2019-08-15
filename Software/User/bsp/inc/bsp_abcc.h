/*
*********************************************************************************************************
*
*	ģ������ : ABCCģ�� 
*	�ļ����� : bsp_abcc.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
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

/* ���ⲿ���õĺ������� */
/* ����SPI���ߵ� GPIO�˿� */
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


