/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.02.02 (2016-11-10)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**    ABCC Driver    5.02.01 (2016-11-02)                                     **
**                                                                            */
/*----------------------------------------------------------------------------
 * Name:    Serial.h
 * Purpose: Low level serial definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f4xx_hal.h"
#include "usart.h"

extern void SER_LinkStdio ( UART_HandleTypeDef *huart );
extern int  SER_GetChar   ( void );
extern int  SER_PutChar   ( int c );

#endif
