/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*	模块名称 : uCOS-II的应用配置
*	文件名称 : app_cfg.c
*	版    本 : V1.0
*	说    明 : ucos-ii的应用配置
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-08-02 Eric2013  正式发布
*
*	Copyright (C), 2015-2016, 
*
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  APP_TASK_START_NAME                          ((INT8U *)"Start Task")
#define  APP_TASK_LED_NAME                            ((INT8U *)"LED")
#define  APP_TASK_USER_IF_NAME                        ((INT8U *)"User Interface")
#define  APP_TASK_COM_NAME                            ((INT8U *)"COM")
#define  APP_TASK_FLOW_NAME                           ((INT8U *)"Flow")
#define  APP_TASK_KeyScan_NAME                        ((INT8U *)"KeyScan")


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
/* 启动任务 */
#define  APP_TASK_START_PRIO                               0

#define  APP_TASK_KeyScan_PRIO							   1

#define  APP_TASK_USER_IF_PRIO                             2

#define  APP_TASK_COM_PRIO								   3

#define  APP_TASK_FLOW_PRIO								   4

#define  APP_TASK_LED_PRIO               (OS_LOWEST_PRIO - 3)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         64
#define  APP_TASK_LED_STK_SIZE                           64
#define  APP_TASK_USER_IF_STK_SIZE                       128
#define  APP_TASK_COM_STK_SIZE							 256
#define  APP_TASK_FLOW_STK_SIZE							 256
#define  APP_TASK_KeyScan_STK_SIZE						 64

#define  OS_CPU_EXCEPT_STK_SIZE                          512     

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

#include <cpu.h>
void  App_SerPrintf  (CPU_CHAR *format, ...);

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_DBG
#else
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#endif
#define  APP_TRACE                              App_SerPrintf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
