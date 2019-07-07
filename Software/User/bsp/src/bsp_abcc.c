/*
*********************************************************************************************************
*
*	ģ������ : ABCCģ��
*	�ļ����� : bsp_abcc.h
*	��    �� : V1.0
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-06-12  zwx  ��ʽ����
*
*	Copyright (C), 2019-2029
*
*********************************************************************************************************
*/

#include "bsp.h"


/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_spi1Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStruct;

	//ʹ������
	RCC_APB2PeriphClockCmd(SPI1_RCC_CS | SPI1_RCC_SCK | SPI1_RCC_MOSI | SPI1_RCC_MISO, ENABLE);	

	//ʹ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_I2S_DeInit(SPI1);

	//GPIO����  
	/* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_SCK;	
	GPIO_Init(SPI1_PORT_SCK, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_MISO;	
	GPIO_Init(SPI1_PORT_MISO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_MOSI;	
	GPIO_Init(SPI1_PORT_MOSI, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI1_PIN_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_PORT_CS, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ABCC_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ABCC_RST_GPIO, &GPIO_InitStructure);

	SPI1_CS_1();

	SPI_Cmd(SPI1,DISABLE);

	//SPI��ʼ��
	//SPI_StructInit(&SPI_InitStruct);

	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

	// ABCC SPI max  18MHz
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;  // 
	SPI_Init(SPI1,&SPI_InitStruct);

	SPI_Cmd(SPI1,ENABLE);
}



/*
*********************************************************************************************************
*	�� �� ��: 
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int bsp_abccInit(void)
{
	bsp_spi1Init();
	if( ABCC_HwInit() != ABCC_EC_NO_ERROR )
	{
		return( 0 );
	}	
}

uint8_t SPI_SENDBYTE(uint8_t dat)
{
	
	uint8_t res;
	
	while ( SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)== RESET);
	//SPI_I2S_SendData(SPI1,dat);
	SPI1->DR = dat;
	
	while ( SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)== RESET);
	//res = SPI_I2S_ReceiveData(SPI1);
	res = SPI1->DR;
	
	return res;

}

uint8_t HAL_SPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	//uint32_t TimeWasted = 0;
	uint16_t itemp=0;
	
//	printf("TX: ");
//	for(itemp=0;itemp<Size;itemp++)
//	{
//		 printf("%02X ", pTxData[itemp]);
//	}
//	printf("\n\r");
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);	
	/*SPI SEND AND RECEIVE DATA*/
	for(itemp=0;itemp<Size;itemp++)
	{
		//pRxData[itemp] = SPI_SENDBYTE(pTxData[itemp]);
		*((uint8_t *)(pRxData+itemp)) =  SPI_SENDBYTE(*((uint8_t *)(pTxData+itemp)));
	}
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
//	printf("RX: ");
//	for(itemp=0;itemp<Size;itemp++)
//	{
//		 printf("%02X ", pRxData[itemp]);
//	}
//	printf("\n\r");
	
	return 0;
	 
}




/*****************************************  (END OF FILE) **********************************************/

