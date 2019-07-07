/*
*********************************************************************************************************
*
*	ģ������ :  
*	�ļ����� : bsp_io.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
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

/* ���ⲿ���õĺ������� */
void bsp_RelayInit(void);
void bsp_SetRelay1State(uint8_t state);
void bsp_SetRelay2State(uint8_t state);
void bsp_InputStateInit(void);
void bsp_GetInputState(uint8_t *state);

#endif

/*****************************************  (END OF FILE) **********************************************/

