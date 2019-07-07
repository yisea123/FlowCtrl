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
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <rt_misc.h>
#include "Serial.h"

#pragma import(__use_no_semihosting_swi)

struct __FILE 
{ 
  int handle; 
  /* Add whatever you need here */
};

enum
{
  STDIN_HANDLE = 0,
  STDOUT_HANDLE,
  STDERR_HANDLE
};

FILE __stdin = {STDIN_HANDLE};
FILE __stdout = {STDOUT_HANDLE};
FILE __stderr = {STDERR_HANDLE};

int fputc(int c, FILE * f)
{
  int ret = EOF;
  switch(f->handle)
  {
    case STDOUT_HANDLE:
    case STDERR_HANDLE:
      ret = SER_PutChar(c);
      break;
    default:
      break;
  }
  return (ret);
}


int fgetc(FILE *f)
{
  return (SER_GetChar());
}


int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int c)
{
  SER_PutChar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
