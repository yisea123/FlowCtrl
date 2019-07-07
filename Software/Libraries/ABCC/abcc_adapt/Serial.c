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
 * Name:    Serial.c
 * Purpose: Low level serial routines
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "Serial.h"

/* Important Note: this project is based on STM32CubeMX and therefore it is expected
** that the UART to be used for STDIO will have been intialized accordingly prior to
** utilizing these calls */

static UART_HandleTypeDef *huart_stdio = NULL;

/*-----------------------------------------------------------------------------
 *       SER_AttachStdio:  Link a UART to the STDIO routines
 *----------------------------------------------------------------------------*/
void SER_LinkStdio( UART_HandleTypeDef *huart )
{
  huart_stdio = huart;
}


/*-----------------------------------------------------------------------------
 *       SER_PutChar:  Write a character to Serial Port
 *----------------------------------------------------------------------------*/
int32_t SER_PutChar ( const int32_t ch )
{
  if(huart_stdio)
  {
    uint8_t c = ch;
    if(HAL_UART_Transmit(huart_stdio, &c, 1, 1000) == HAL_OK)
    {
      return (ch);
    }
  }
  return ( -1 );
}


/*-----------------------------------------------------------------------------
 *       SER_GetChar:  Read a character from Serial Port
 *----------------------------------------------------------------------------*/
int32_t SER_GetChar ( void )
{
  if(huart_stdio)
  {
    uint8_t ch;
    if(HAL_UART_Receive(huart_stdio, &ch, 1, 1000) == HAL_OK)
    {
      return (ch);
    }
  }
  return ( -1 );
}

/*-----------------------------------------------------------------------------
 * end of file
 *----------------------------------------------------------------------------*/
