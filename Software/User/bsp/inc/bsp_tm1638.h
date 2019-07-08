/*
*********************************************************************************************************
*
*	模块名称 : TM1638驱动模块
*	文件名称 : bsp_tm1638.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#ifndef __BSP_TM1638_H
#define __BSP_TM1638_H

#include <stdint.h>

#define KEY_Line1_K1   0x01
#define KEY_Line1_K2   0x02
#define KEY_Line1_K3   0x03
#define KEY_Line1_K4   0x04
#define KEY_Line1_K5   0x05
#define KEY_Line1_K6   0x06

/* 供外部调用的函数声明 */
void bsp_InitTm1638(void);
void bsp_Tm1638WriteByte(uint8_t byte);
void bsp_Tm1638WriteData(uint8_t addr, uint8_t data);
void bsp_Tm1638Disp(uint8_t num1, uint8_t num2, uint8_t num3, uint8_t num4, uint8_t point);
uint8_t bsp_Tm1638ReadKey(void);

#endif

/*****************************************  (END OF FILE) **********************************************/
