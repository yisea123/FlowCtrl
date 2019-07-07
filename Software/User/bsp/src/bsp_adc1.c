/*
*********************************************************************************************************
*
*	ģ������ : ADC1����ģ��
*	�ļ����� : bsp_adc1.h
*	��    �� : V1.0
*	˵    �� : ��������ܺͰ���
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-06-13  zwx  ��ʽ����
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#include "bsp.h"




/* ADC1 �������� */
#define ADC1_IN1_PORT  GPIOA
#define ADC1_IN1_PIN   GPIO_Pin_1

/*
*********************************************************************************************************
*	�� �� ��: bsp_Adc1Init
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Adc1Init(void)
{
    GPIO_InitTypeDef         gpio;
    ADC_InitTypeDef          adc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
    
    gpio.GPIO_Pin   = ADC1_IN1_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADC1_IN1_PORT,&gpio);		
    
	ADC_DeInit(ADC1); 
    adc.ADC_Mode               = ADC_Mode_Independent;	             
    adc.ADC_ScanConvMode       = DISABLE; 		             
    adc.ADC_ContinuousConvMode = DISABLE;	               
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; 
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc);
   
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
    ADC_Cmd(ADC1, ENABLE);
	
  	ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}


/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t Adc1_Collect(void)
{   	 
	uint16_t PowerVal = 0;
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	PowerVal = (float)3.3 * ADC_GetConversionValue(ADC1) / 4096 * 11;	
	return PowerVal;
}




/*****************************************  (END OF FILE) **********************************************/
