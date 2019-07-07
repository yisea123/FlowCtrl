/*
*********************************************************************************************************
*
*	模块名称 :  
*	文件名称 : bsp_io.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#ifndef __BSP__IO_H
#define __BSP__IO_H

#include <stdint.h>


#define RELAY_OFF	0
#define RELAY_ON	1

/* 供外部调用的函数声明 */
void bsp_RelayInit(void);
void bsp_SetRelay1State(uint8_t state);
void bsp_SetRelay2State(uint8_t state);
void bsp_InputStateInit(void);
void bsp_GetInputState(uint8_t *state);

#endif

/*****************************************  (END OF FILE) **********************************************/

