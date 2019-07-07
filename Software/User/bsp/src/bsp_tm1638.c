/*
*********************************************************************************************************
*
*	模块名称 : TM1638驱动模块
*	文件名称 : bsp_tm1638.h
*	版    本 : V1.0
*	说    明 : 驱动数码管和按键
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

void bsp_Tm1638WriteByte(uint8_t byte);
void bsp_Tm1638WriteCmd(uint8_t cmd);
void bsp_Tm1638WriteData(uint8_t addr, uint8_t data);

#define RCC_ALL_TM1638 	  (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC)
#define TM1638_STB_GPIO   GPIOB
#define TM1638_STB_PIN    GPIO_Pin_9
#define TM1638_CLK_GPIO   GPIOC
#define TM1638_CLK_PIN    GPIO_Pin_13
#define TM1638_DIO_GPIO   GPIOA
#define TM1638_DIO_PIN    GPIO_Pin_0

const uint8_t Seg[] = {                                                                      
        0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,                // 0
        0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 1
        0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x00,                // 2
        0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x00,                // 3
        0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x00,                // 4
        0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00,                // 5
        0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x00,                // 6
        0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,                // 7
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,                // 8
        0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x00,                // 9
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                // 10
};


/*
*********************************************************************************************************
*	函 数 名: bsp_InitTm1638
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTm1638(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_ALL_TM1638, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = TM1638_STB_PIN;
	GPIO_Init(TM1638_STB_GPIO,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = TM1638_CLK_PIN;
	GPIO_Init(TM1638_CLK_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Pin = TM1638_DIO_PIN;
	GPIO_Init(TM1638_DIO_GPIO,&GPIO_InitStructure);	
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Tm1638WriteByte(uint8_t byte)
{
	uint8_t i = 0;
	
	for(i=0; i<8; i++)
	{
		GPIO_ResetBits(TM1638_CLK_GPIO, TM1638_CLK_PIN);
		if(byte & 0x01)
		{
			GPIO_SetBits(TM1638_DIO_GPIO, TM1638_DIO_PIN);
		}
		else
		{
			GPIO_ResetBits(TM1638_DIO_GPIO, TM1638_DIO_PIN);
		}
		GPIO_SetBits(TM1638_CLK_GPIO, TM1638_CLK_PIN);
		byte >>= 1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Tm1638WriteCmd(uint8_t cmd)
{
	GPIO_SetBits(TM1638_STB_GPIO, TM1638_STB_PIN);
	GPIO_ResetBits(TM1638_STB_GPIO, TM1638_STB_PIN);
	bsp_Tm1638WriteByte(cmd);
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Tm1638WriteData(uint8_t addr, uint8_t data)
{
	bsp_Tm1638WriteCmd(0x44);      // 设置数据命令-固定地址
	bsp_Tm1638WriteCmd(0xc0|addr); // 设置显示地址
	bsp_Tm1638WriteByte(0xff&data);     // data
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Tm1638Disp
*	功能说明: uint8_t num1, uint8_t num2, uint8_t num3, uint8_t num4 : 数码管显示的数字 0 - 9
              uint8_t point ：小数点所在位置 0 - 4，其中 0表示不显示小数点
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Tm1638Disp(uint8_t num1, uint8_t num2, uint8_t num3, uint8_t num4, uint8_t point)
{
	uint8_t i = 0;
	
	for(i=0; i<7; i++)
	{
		bsp_Tm1638WriteData(i<<1, Seg[num4*8+i] | Seg[num3*8+i]<<1 | Seg[num2*8+i]<<2 | Seg[num1*8+i]<<3);		
	}
	if(point > 0)
	{
		bsp_Tm1638WriteData(i<<1, 1<<(4-point));
	}
	else
	{
		bsp_Tm1638WriteData(i<<1, 0);
	}
	
	bsp_Tm1638WriteCmd(0x8F); /* 显示开 最高亮度 */
}

/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_Tm1638ReadByte(void)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	
	GPIO_SetBits(TM1638_DIO_GPIO, TM1638_DIO_PIN);	
	
	for(i=0; i<8; i++)
	{
		GPIO_ResetBits(TM1638_CLK_GPIO, TM1638_CLK_PIN);
		temp >>= 1;
		GPIO_SetBits(TM1638_CLK_GPIO, TM1638_CLK_PIN);

		if(GPIO_ReadInputDataBit(TM1638_DIO_GPIO, TM1638_DIO_PIN) == Bit_SET)
		{
			temp |= 0x80;
		}
	}

	return temp;
}



/*
*********************************************************************************************************
*	函 数 名: 
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_Tm1638ReadKey(void)
{
	uint8_t i = 0;
	uint8_t key[4] = {0};
	uint8_t keyval = 0;
	uint32_t key_code = 0;

	bsp_Tm1638WriteCmd(0x42); /* 读键扫数据 */
	
	for(i=0; i<4; i++)
	{
		key[i] = bsp_Tm1638ReadByte();
	}
	
	key_code = ((key[0]&0x66)<<24) | ((key[1]&0x66)<<16) | ((key[2]&0x66)<<8) | (key[3]&0x66);
	
	if(key_code == 0x4000000)
	{
		return 0x01;
	}
	else if(key_code == 0x40000000)
	{
		return 0x02;
	}
	else if(key_code == 0x40000)
	{
		return 0x03;
	}
	else if(key_code == 0x400000)
	{
		return 0x04;
	}
	else if(key_code == 0x400)
	{
		return 0x05;
	}
	else if(key_code == 0x4000)
	{
		return 0x06;
	}
	else
	{
		return 0;
	}
}


/*****************************************  (END OF FILE) **********************************************/
