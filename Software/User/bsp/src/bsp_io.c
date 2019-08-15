/*
*********************************************************************************************************
*
*	模块名称 : 
*	文件名称 : bsp_io.c
*	版    本 : V1.0
*	说    明 : IO引脚控制模块
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2019-06-15  zwx  正式发布
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#include "bsp.h"

#define RELAY_OUT_RCC   (RCC_APB2Periph_GPIOA)
#define RELAY_OUT1_GPIO GPIOA 
#define RELAY_OUT1_PIN  GPIO_Pin_11
#define RELAY_OUT2_GPIO GPIOA
#define RELAY_OUT2_PIN  GPIO_Pin_8

#define RELAY_ON	0
#define RELAY_OFF	1

#define STATE_IN_RCC    (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB)
#if 0 /* 硬件版本1 */
	#define STATE_IN1_GPIO  GPIOA 
	#define STATE_IN1_PIN   GPIO_Pin_12
	#define STATE_IN2_GPIO  GPIOB 
	#define STATE_IN2_PIN   GPIO_Pin_3
	#define STATE_IN3_GPIO  GPIOA
	#define STATE_IN3_PIN   GPIO_Pin_15
#else /* 硬件版本2 */
	#define STATE_IN1_GPIO  GPIOA 
	#define STATE_IN1_PIN   GPIO_Pin_14
	#define STATE_IN2_GPIO  GPIOA 
	#define STATE_IN2_PIN   GPIO_Pin_15
	#define STATE_IN3_GPIO  GPIOB
	#define STATE_IN3_PIN   GPIO_Pin_3
#endif


/*
*********************************************************************************************************
*	函 数 名: bsp_RelayInit
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RelayInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RELAY_OUT_RCC, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = RELAY_OUT1_PIN;
	GPIO_Init(RELAY_OUT1_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RELAY_OUT2_PIN;
	GPIO_Init(RELAY_OUT2_GPIO, &GPIO_InitStructure);	
}

/*
*********************************************************************************************************
*	函 数 名:  bsp_SetRelay1State
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetRelay1State(uint8_t state)
{
	if(state == RELAY_ON)
	{
		RELAY_OUT1_GPIO->BRR = RELAY_OUT1_PIN;
	}
	else 
	{
		RELAY_OUT1_GPIO->BSRR = RELAY_OUT1_PIN;
	}
}

/*
*********************************************************************************************************
*	函 数 名:  bsp_SetRelay1State
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetRelay2State(uint8_t state)
{
	if(state == RELAY_ON)
	{
		RELAY_OUT2_GPIO->BRR = RELAY_OUT2_PIN;
	}
	else 
	{
		RELAY_OUT2_GPIO->BSRR = RELAY_OUT2_PIN;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InputStateInit
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InputStateInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(STATE_IN_RCC, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	
	GPIO_InitStructure.GPIO_Pin = STATE_IN1_PIN;
	GPIO_Init(STATE_IN1_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = STATE_IN2_PIN;
	GPIO_Init(STATE_IN2_GPIO, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = STATE_IN3_PIN;
	GPIO_Init(STATE_IN3_GPIO, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetInputState
*	功能说明: 
*	形    参: state : state[0-2]: 三个输入的状态
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_GetInputState(uint8_t *state)
{
	state[0] = ((STATE_IN1_GPIO->IDR & STATE_IN1_PIN) == 0)? 0:1;
	state[1] = ((STATE_IN2_GPIO->IDR & STATE_IN2_PIN) == 0)? 0:1;
	state[2] = ((STATE_IN3_GPIO->IDR & STATE_IN3_PIN) == 0)? 0:1;
}

/*****************************************  (END OF FILE) **********************************************/

