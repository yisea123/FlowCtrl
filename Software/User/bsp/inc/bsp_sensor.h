/*
*********************************************************************************************************
*
*	ģ������ : bsp_sensor.h 
*	�ļ����� : 
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#ifndef __BSP_SENSOR_H
#define __BSP_SENSOR_H

#include <stdint.h>

#define HUBA_BUF_LEN   10

extern uint16_t g_usHuba1;
extern uint16_t g_usHuba2;

extern uint16_t Huba1Buf[HUBA_BUF_LEN];
extern uint16_t Huba2Buf[HUBA_BUF_LEN];

/* ���ⲿ���õĺ������� */
void bsp_SensorInit(uint16_t psc, uint16_t arr);

#endif

/*****************************************  (END OF FILE) **********************************************/

