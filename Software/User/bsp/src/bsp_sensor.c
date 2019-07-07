/*
*********************************************************************************************************
*
*	模块名称 : bsp_sensor.h
*	文件名称 : 
*	版    本 : V1.0
*	说    明 : 包含涡街流量传感器
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2019-06-12  zwx  正式发布
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#include "bsp.h"

#define HUBA_RCC   RCC_APB2Periph_GPIOB
#define HUBA1_GPIO GPIOB
#define HUBA1_PIH  GPIO_Pin_0
#define HUBA2_GPIO GPIOB
#define HUBA2_PIN  GPIO_Pin_1

uint16_t g_usHuba1 = 0;
uint16_t g_usHuba2 = 0;
static uint16_t usPeriod = 0;


uint16_t Huba1Buf[HUBA_BUF_LEN] = {0};
uint16_t Huba2Buf[HUBA_BUF_LEN] = {0};
static uint8_t  Huba1Index = 0;
static uint8_t  Huba2Index = 0;

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 频率范围大概是 20Hz-270Hz
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SensorInit(uint16_t psc, uint16_t arr)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM3_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);          
	RCC_APB2PeriphClockCmd(HUBA_RCC, ENABLE);
      
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = HUBA1_PIH;
	GPIO_Init(HUBA1_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = HUBA2_PIN;
	GPIO_Init(HUBA2_GPIO, &GPIO_InitStructure);


	TIM_TimeBaseStructure.TIM_Prescaler = psc;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period = arr;   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	usPeriod = arr;

	// TIM3 设置
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3;      
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;        
	TIM3_ICInitStructure.TIM_ICFilter = 0x00; 
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
	
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4;      
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;        
	TIM3_ICInitStructure.TIM_ICFilter = 0x00; 
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update|TIM_IT_CC3|TIM_IT_CC4, ENABLE);     

	TIM_Cmd(TIM3, ENABLE );       

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                      
	NVIC_Init(&NVIC_InitStructure);        
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM3_IRQHandler(void)
{
	static uint16_t last_huba1 = 0, last_huba2 = 0, temp = 0;
	static uint8_t last_update = 0;
	static uint8_t cc3flg = 0, cc4flg = 0;
	
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
//	}
//	if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)  
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
//	}
//	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)  
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
//	}
	
	if(TIM3->SR & TIM_IT_CC3)
	{
		temp = TIM3->CCR3;
		if(last_huba1 != 0)
		{
			if(temp < last_huba1)
				g_usHuba1 = usPeriod - last_huba1 + temp;
			else
				g_usHuba1 = temp - last_huba1;		
		}
		
		last_huba1 = TIM3->CCR3;
		cc3flg = 1;
		
		Huba1Buf[Huba1Index] = g_usHuba1;
		Huba1Index++;
		if(Huba1Index == HUBA_BUF_LEN)
		{
			Huba1Index = 0;
		}
	}
	
	if(TIM3->SR & TIM_IT_CC4)
	{
		temp = TIM3->CCR4;
		if(last_huba2 != 0)
		{
			if(temp < last_huba2)
				g_usHuba2 = usPeriod - last_huba2 + temp;
			else
				g_usHuba2 = temp - last_huba2;			
		}

		last_huba2 = TIM3->CCR4;
		cc4flg = 1;
		
		Huba2Buf[Huba2Index] = g_usHuba2;
		Huba2Index++;
		if(Huba2Index == HUBA_BUF_LEN)
		{
			Huba2Index = 0;
		}
	}	
	
	if(TIM3->SR & TIM_IT_Update)
	{
		if(cc3flg == 0)
		{
			g_usHuba1 = 0;
			last_huba1 = 0;
			Huba1Buf[Huba1Index] = g_usHuba1;
			Huba1Index++;
			if(Huba1Index == HUBA_BUF_LEN)
			{
				Huba1Index = 0;
			}			
		}
		if(cc4flg == 0)
		{
			g_usHuba2 = 0;
			last_huba2 = 0;
			Huba2Buf[Huba2Index] = g_usHuba2;
			Huba2Index++;
			if(Huba2Index == HUBA_BUF_LEN)
			{
				Huba2Index = 0;
			}			
		}		
		cc3flg = 0;
		cc4flg = 0;
	}
	
	TIM3->SR = 0;
}



/*****************************************  (END OF FILE) **********************************************/

