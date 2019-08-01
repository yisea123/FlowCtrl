/*
*********************************************************************************************************
*	                                  
*	模块名称 : uCOS-II
*	文件名称 : app.c
*	版    本 : V1.0
*	说    明 : 本实验主要实现线程安全的printf方式
*              实验目的：
*                1. 学习如何在uCOS-II上实现线程安全的printf方式。
*              实验内容：
*                1. 共创建了如下几个任务，通过按下按键K1可以通过串口打印任务堆栈使用情况
*                   优先级   使用栈  剩余栈  百分比   任务名
*                    Prio     Used    Free    Per      Taskname
*                     63       17      111    13%     uC/OS-II Idle
*                     62       21      107    16%     uC/OS-II Stat
*                      0       34      222    13%     Start Task
*                      2       88      168    34%     User Interface
*                     60       27      229    10%     LED
*                      3       27      229    10%     COM
*                      1       29      227    11%     KeyScan
*                    串口软件建议使用SecureCRT（V4光盘里面有此软件）查看打印信息。
*                    Start Task任务    ：实现LED闪烁。
*                    User Interface任务：根据用户的按键消息，实现相应的操作。
*                    LED任务           ：实现LED闪烁。
*                    KeyScan任务       ：主要是实现按键扫描。
*                    COM任务           ：实现LED闪烁。
*                2. (1) 凡是用到printf函数的全部通过函数App_Printf实现。
*                   (2) App_Printf函数做了信号量的互斥操作，解决资源共享问题。
*
*              注意事项：
*                 1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                    V4开发板光盘里面有。
*                 2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2015-08-02   Eric2013    1. ST固件库到V3.6.1版本
*                                        2. BSP驱动包V1.2
*                                        3. uCOS-II版本V2.92.11
*                                        4. uC/CPU版本V1.30.02
*                                        5. uC/LIB版本V1.38.01
*                                       
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              任务堆栈
********************************************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskLEDStk[APP_TASK_LED_STK_SIZE];	
static OS_STK AppTaskCOMStk[APP_TASK_COM_STK_SIZE];	
static OS_STK AppTaskFlowStk[APP_TASK_FLOW_STK_SIZE];	
static OS_STK AppTaskKeyScanStk[APP_TASK_KeyScan_STK_SIZE];	
			
/*
*******************************************************************************************************
*                              函数声明
*******************************************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTaskUserIF(void *p_arg);
static void AppTaskLED(void *p_arg);
static void DispTaskInfo(void);
static void AppTaskCom(void *p_arg);
static void App_Printf (CPU_CHAR *format, ...);

static void writeParamFromFlash(void);
static void readParamFromFlash(void);
/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/

#define KEY_SELECT    KEY_Line1_K1
#define KEY_EXIT      KEY_Line1_K2
#define KEY_UP        KEY_Line1_K3
#define KEY_DOWN      KEY_Line1_K4

#define KEY_CtrlInput KEY_Line1_K5
#define KEY_CtrlPump  KEY_Line1_K6



/* 定义一个邮箱， 这只是一个邮箱指针， OSMboxCreate函数会创建邮箱必需的资源 */
static OS_EVENT *AppUserIFMbox;

/*创建一个信号量*/
static OS_EVENT *AppPrintfSemp;

struct FlashParam
{
	uint32_t Flg; /* 防止意外断电数据出错 */
	uint8_t ValueClosed;
	uint8_t Bypass;
	uint8_t FlowWarningValue;
	uint8_t FlowFaultValue;
	uint8_t LeakResponseValue;
	uint8_t DelayToDetect;
	uint8_t LeakFlowDifference;
};

struct FlashParam FlashParam;

uint8_t isConnectOk(void)
{
	if((ABCC_AnbState() == ABP_ANB_STATE_PROCESS_ACTIVE) || (ABCC_AnbState() == ABP_ANB_STATE_IDLE))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static void readParamFromFlash(void)
{
	sf_ReadBuffer((uint8_t*)&FlashParam, 0, sizeof(FlashParam));
	if(FlashParam.Flg == 0x55AA6699)
	{
		ValueClosed = FlashParam.ValueClosed;
		Bypass = FlashParam.Bypass;
		FlowWarningValue = FlashParam.FlowWarningValue;
		FlowFaultValue = FlashParam.FlowFaultValue;
		LeakResponseValue = FlashParam.LeakResponseValue;
		DelayToDetect = FlashParam.DelayToDetect;
		LeakFlowDifference = FlashParam.LeakFlowDifference;
	}
	else
	{
		writeParamFromFlash();
	}
}

static void writeParamFromFlash(void)
{
	uint8_t flashUpdate = 0;
	
	FlashParam.Flg = 0x55AA6699;
	if(FlashParam.ValueClosed != ValueClosed)
	{
		FlashParam.ValueClosed = ValueClosed;
		flashUpdate = 1;
	}
	if(FlashParam.Bypass != Bypass)
	{
		FlashParam.Bypass = Bypass;
		flashUpdate = 1;
	}	
	if(FlashParam.FlowWarningValue != FlowWarningValue)
	{
		FlashParam.FlowWarningValue = FlowWarningValue;
		flashUpdate = 1;
	}
	if(FlashParam.FlowFaultValue != FlowFaultValue)
	{
		FlashParam.FlowFaultValue = FlowFaultValue;
		flashUpdate = 1;
	}		
	if(FlashParam.LeakResponseValue != LeakResponseValue)
	{
		FlashParam.LeakResponseValue = LeakResponseValue;
		flashUpdate = 1;
	}
	if(FlashParam.DelayToDetect != DelayToDetect)
	{
		FlashParam.DelayToDetect = DelayToDetect;
		flashUpdate = 1;
	}		
	if(FlashParam.LeakFlowDifference != LeakFlowDifference)
	{
		FlashParam.LeakFlowDifference = LeakFlowDifference;
		flashUpdate = 1;
	}	
	
	if(flashUpdate == 1)
	{
		if(1 == sf_WriteBuffer((uint8_t*)&FlashParam, 0, sizeof(FlashParam)))
		{
			
		}		
		else
		{
			printf("flash write error\n");
		}
	}
}

void CtrlInputWaterCtrl(uint8_t state)
{
	bsp_SetRelay2State(state);  
}

void CtrlPumpWater(uint8_t state)
{
	bsp_SetRelay1State(state);  
}

void CalcFlowValue(void)
{
	uint8_t index = 0;
	uint32_t sum = 0;

	sum = 0;
	for(index=0; index<HUBA_BUF_LEN; index++)
	{
		sum += Huba1Buf[index];
	}
	SupplyFlowInLPM = sum / HUBA_BUF_LEN * 10 * 0.186;
	
	sum = 0;
	for(index=0; index<HUBA_BUF_LEN; index++)
	{
		sum += Huba2Buf[index];
	}
	ReturnFlowInLPM = sum / HUBA_BUF_LEN * 10 * 0.186;	
}


/*
*******************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
int main(void)
{
	INT8U  err;

	/* 初始化"uC/OS-II"内核 */
	OSInit();
  
	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                    (void *)0,		/* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1], /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                    (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                       （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
					
					/*  定义如下：
						OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
						OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
						OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
					*/                  

	/* 指定任务的名称，用于调试。这个函数是可选的 */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
	
	/*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/ 
	OSTimeSet(0);	
	
	/* 启动多任务系统，控制权交给uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
	uint32_t key = 0;
	uint8_t i = 0;
	
    /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */	
    (void)p_arg;  
	
	 
	/* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
	bsp_Init();
    CPU_Init();          
	BSP_Tick_Init();   
	

	/* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率 */
	#if (OS_TASK_STAT_EN > 0)
		OSStatInit();
	#endif	
		
	/* 创建应用程序的任务 */
	AppTaskCreate();
	
	OSTimeDlyHMSM(0, 0, 5, 0);
	APPL_RestartAbcc();  /* 复位ABCC */
	
	while (1)     
	{ 	
		OSTimeDlyHMSM(0, 0, 1, 0);
		printf("input:%dhz  output:%dhz\n", 1000000/(g_usHuba1*10), 1000000/(g_usHuba2*10));
	}      
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskKeyScan
*	功能说明: 按键扫描	
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 1
*********************************************************************************************************
*/
static void AppTaskKeyScan(void *p_arg)
{
	uint8_t ucKeyCode = 0;	/* 按键代码 */
	uint8_t ucTemp = 0;
	(void)p_arg;
		  
	while(1)
	{    
		ucTemp = bsp_Tm1638ReadKey();;   /* 读取键盘 */
		if (ucTemp > 0)
		{	
			ucKeyCode = ucTemp;
			/* 将按键代码发送到邮箱 */
			OSMboxPost(AppUserIFMbox, &ucKeyCode);
		}

		/* 延迟10ms。必须释放CPU，否则低优先级任务会阻塞 */    	
		OSTimeDlyHMSM(0, 0, 0, 300);	 /* 也可以调用 OSTimeDly() 函数实现延迟 */						  	 	       											  
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 本任务主要用于得到按键的键值。
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
#define FLOW_VALUE					0
#define FLOW_WARNING_VALUE			1
#define FLOW_FAULT_VALUE			2
#define LEAK_RESPONSE_VALUE			3
#define DELAY_TO_DETECT				4
#define LEAK_FLOW_DIFFERENCE		5

static void AppTaskUserIF(void *p_arg)
{
	INT8U   msg;
	INT8U   err;
	INT8U   timeout = 0;
	INT8U   input_state = RELAY_OFF;
	INT8U   pump_state = RELAY_OFF;
	INT8U   key_state = 0;
	INT8U   select_state = 0;
	INT8U   flow_value = 0;
	
   (void)p_arg;	 
	
	while (1) 
	{
		msg = *(INT8U *)(OSMboxPend(AppUserIFMbox, OS_TICKS_PER_SEC , &err));
		if (err == OS_ERR_NONE)                             /* 无错表示成功接收到一个消息 */
		{
			printf("key_state = %d, select %d\n", KEY_SELECT, select_state);
			if (msg == KEY_SELECT)		
			{    
//				DispTaskInfo();
				key_state = KEY_SELECT;
				timeout = 0;
				select_state++;
				if(select_state == 6)
				{
					select_state = 1;
				}
			}
			else if (msg == KEY_EXIT)
			{
				key_state =  0;
				select_state = 0;
			}
			else if (msg == KEY_UP)		
			{ 
				switch(select_state)
				{
					case FLOW_WARNING_VALUE:
						FlowWarningValue++;
						break;
					case FLOW_FAULT_VALUE:
						FlowFaultValue++;
						break;
					case LEAK_RESPONSE_VALUE:
						LeakResponseValue++;
						break;
					case DELAY_TO_DETECT:
						DelayToDetect++;
						break;
					case LEAK_FLOW_DIFFERENCE:
						LeakFlowDifference++;
						break;
					default:
						break;
				}
			}	
			else if (msg == KEY_DOWN)
			{
				switch(select_state)
				{
					case FLOW_WARNING_VALUE:
						FlowWarningValue--;
						break;
					case FLOW_FAULT_VALUE:
						FlowFaultValue--;
						break;
					case LEAK_RESPONSE_VALUE:
						LeakResponseValue--;
						break;
					case DELAY_TO_DETECT:
						DelayToDetect--;
						break;
					case LEAK_FLOW_DIFFERENCE:
						LeakFlowDifference--;
						break;
					default:
						break;
				}
			}
			else if (msg == KEY_CtrlInput)		
			{ 
				if(input_state == RELAY_OFF)
					input_state = RELAY_ON;
				else
					input_state = RELAY_OFF;

				CtrlInputWaterCtrl(input_state);
			}	
			else if (msg == KEY_CtrlPump)		
			{ 
				if(pump_state == RELAY_OFF)
					pump_state = RELAY_ON;
				else
					pump_state = RELAY_OFF;

				CtrlPumpWater(pump_state);
			}	
			else
			{
				if(key_state == KEY_SELECT)
				{
					timeout++;
					if(timeout >= 5)
					{
						key_state = 0;
						select_state = 0;
					}
				}
				
			}
		}
		
		switch(select_state)
		{
			case FLOW_VALUE:
				bsp_Tm1638Disp(0, SupplyFlowInLPM/100%10, SupplyFlowInLPM/10%10, SupplyFlowInLPM%10, 1);   /* 显示数据 */
				break;
			case FLOW_WARNING_VALUE:
				bsp_Tm1638Disp(1, FlowWarningValue/100%10, FlowWarningValue/10%10, FlowWarningValue%10, 1);  
				break;
			case FLOW_FAULT_VALUE:
				bsp_Tm1638Disp(2, FlowFaultValue/100%10, FlowFaultValue/10%10, FlowFaultValue%10, 1);  
				break;
			case LEAK_RESPONSE_VALUE:
				bsp_Tm1638Disp(3, LeakResponseValue/100%10, LeakResponseValue/10%10, LeakResponseValue%10, 1);  
				break;
			case DELAY_TO_DETECT:
				bsp_Tm1638Disp(4, DelayToDetect/100%10, DelayToDetect/10%10, DelayToDetect%10, 1); 
				break;
			case LEAK_FLOW_DIFFERENCE:
				bsp_Tm1638Disp(5, LeakFlowDifference/100%10, LeakFlowDifference/10%10, LeakFlowDifference%10, 1); 
				break;
			default:
				break;
		}		
		
		
	}		
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: LED4闪烁和串口打印
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskCom(void *p_arg)
{	
	int cnt = 0;
	APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
	(void)p_arg;

	bsp_abccInit();     /* abcc初始化 */
	
	
	
	while(1)
	{		 		 
		eAbccHandlerStatus = APPL_HandleAbcc();
		//B40时基，MCU过了APPL_TIMER_MS,需要通知B40
		ABCC_RunTimerSystem( APPL_TIMER_MS );
		OSTimeDly( APPL_TIMER_MS );
		switch( eAbccHandlerStatus )
		{
			case APPL_MODULE_RESET:
				NVIC_SystemReset();
				break;
			default:
				break;
		}
   } 						  	 	       											  
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskFlow
*	功能说明: Flow控制模块		  			  
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
static void AppTaskFlow(void *p_arg)
{
	uint8_t timeout = 0;
	uint32_t leakTime = 0;
    (void)p_arg;		/* 避免编译器告警 */

	/* 读取相关变量 */
	readParamFromFlash();
		
	for(timeout = 0; timeout < 10; timeout++)
	{
		if(isConnectOk() == 1)
		{
			break;
		}
		else
		{
			OSTimeDlyHMSM(0, 0, 1, 0);
		}
	}
	
	if(timeout == 10) /* 无法与主机建立通信 */
	{
		OSTimeDlyHMSM(0, 0, DelayToDetect, 0);
		CtrlInputWaterCtrl(RELAY_ON);   /* 通电打开水阀 */
		FlowOff = 0;
	}
	else
	{
		if(ValueClosed == 1)
		{
			bsp_SetRelay2State(RELAY_OFF);
		}
		if(Bypass == 1)
		{
			
		}
	}
	
	while (1) 
	{
		if(SetParameterRequest == 1)
		{
			FlowWarningValue   = SetFlowWarningValue; 
			FlowFaultValue     = SetFlowFaultValue;
			LeakResponseValue  = SetLeakResponseValue;
			DelayToDetect      = SetDelayToDetect;
			LeakFlowDifference = SetLeakFlowDifference; 
		}
		
		if(SetFlowReset == 1)
		{
			SetFlowReset = 0; /* 软重启 */
			NVIC_SystemReset();
		}
		
		if((SetCloseValue == 1) && (ValueClosed == 0))             /* 1:关闭电磁阀/阀断电 - 关闭进水 */
		{
			CtrlInputWaterCtrl(RELAY_OFF);
			FlowOff = 1;
			ValueClosed = 1;
		}
		else if((SetCloseValue == 0) && (ValueClosed == 1))
		{
			ValueClosed = 0;			
		}
		
		if((SetByPass == 1) && (Bypass == 0))
		{
			if(ValueClosed != 1)
			{
				CtrlInputWaterCtrl(RELAY_ON);  /* 打开电磁阀  - 打开进水 */
				FlowOff = 0;
			}
			Bypass = 1;                    /* Bypass为1，则Relay1不再关闭  */			
		}
		else if((SetByPass == 0) && (Bypass == 1))
		{
			Bypass = 0;    
		}

		/* 自动更换电极帽 */
		if(SetExchangeCapReset == 1)	 
		{
			ValueClosed = 1;
			CtrlInputWaterCtrl(RELAY_OFF); /* 电磁阀断电 */
			FlowOff = 1;
			OSTimeDlyHMSM(0, 0, 0, 500);   /* 延时0.5s */
			CtrlPumpWater(RELAY_ON);       /* 启动抽水泵 */ 
			OSTimeDlyHMSM(0, 0, 5, 0);	   /* 延时1min 等待抽水泵工作完毕 */
			OKToExchangeCap = 1;		   /* OK_To_Exchange 信号置为OK，可以开始更换电极帽 */
			while(SetExchangeCapReset == 1)
			{
				OSTimeDlyHMSM(0, 0, 1, 0);	/* 延时等待换帽完毕 */	  
			}
			OKToExchangeCap = 0;			/* 不可更换电极帽 */ 
			ValueClosed = 0;
			CtrlInputWaterCtrl(RELAY_ON);    /* 打开进水阀 */
			FlowOff = 0;
		}
		
		CalcFlowValue();  /* 计算Flow Rate */
		
		/* 进水流量异常 */
		if(SupplyFlowInLPM > FlowWarningValue)
		{
			OkToWeld = 1;
			MinimalFlow = 1;
			FlowWarning = 0;
			FlowFault = 0;
		}
		if((SupplyFlowInLPM <= FlowWarningValue) && (SupplyFlowInLPM > FlowFaultValue))
		{
			OkToWeld = 1; 
			MinimalFlow = 0;
			FlowWarning = 1;
			FlowFault = 0;
		}
		if(ReturnFlowInLPM <= FlowFaultValue)
		{
			OkToWeld = 0;
			MinimalFlow = 0;
			FlowWarning = 0;
			FlowFault = 1;
		}
		
		/* 电极帽丢失检测 */
		if((SupplyFlowInLPM - ReturnFlowInLPM) > LeakFlowDifference)
		{
			leakTime += 5;
			if(leakTime >= LeakResponseValue * 1000)
			{
				if((ValueClosed != 1) && (Bypass != 1))
				{
					CtrlInputWaterCtrl(RELAY_OFF); 
					FlowOff = 0;
					CtrlPumpWater(RELAY_ON);					
				}
   
				Leak = 1;
			}
		}
		else
		{
			Leak = 0;
			leakTime = 0;
		}
		
		/* flash 参数更新 */
		writeParamFromFlash();
		
		OSTimeDlyHMSM(0, 0, 0, 5);		
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED2闪烁和串口打印		  			  
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
static void AppTaskLED(void *p_arg)
{
	uint16_t PowerVal = 0;
	
    (void)p_arg;		/* 避免编译器告警 */
		
	while (1) 
	{
		if(OkToWeld == 1)
		{
			bsp_LedToggle(LED_FLOW_OK);
		}
		else
		{
			bsp_LedOff(LED_FLOW_OK);
		}
		
		if((FlowWarning == 1) || (Leak == 1))
		{
			bsp_LedToggle(LED_FAULT);
		}
		else
		{
			bsp_LedOff(LED_FAULT);
		}		
		
		if(Bypass == 1)
		{
			bsp_LedToggle(LED_BYPASS);
		}
		else
		{
			bsp_LedOff(LED_BYPASS);
		}
		
		PowerVal = Adc1_Collect();  /* collect power */
		OSTimeDlyHMSM(0, 0, 0, 500);		
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static void  App_Printf (CPU_CHAR *format, ...)
{
    CPU_CHAR  buf_str[80 + 1];
    va_list   v_args;
    CPU_INT08U  os_err;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

    OSSemPend((OS_EVENT  *)AppPrintfSemp,
              (INT32U     )0,
              (INT8U     *)&os_err);

    printf("%s", buf_str);

    os_err = OSSemPost((OS_EVENT *)AppPrintfSemp);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	INT8U      err;


	/* 创建邮箱(MBOX) */
	AppUserIFMbox = OSMboxCreate((void *)0);
	AppPrintfSemp = OSSemCreate(1);	  /* 创建一个信号量 实现信号量互斥 */


	/* 创建AppTaskUserIF任务 */
	OSTaskCreateExt(AppTaskUserIF,
                    (void *)0,
                    (OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE - 1],
                    APP_TASK_USER_IF_PRIO,
                    APP_TASK_USER_IF_PRIO,
                    (OS_STK *)&AppTaskUserIFStk[0],
                    APP_TASK_USER_IF_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USER_IF_PRIO, APP_TASK_USER_IF_NAME, &err);
    
	/* 创建AppTaskLED任务 */
	OSTaskCreateExt(AppTaskLED,
                    (void *)0,
                    (OS_STK *)&AppTaskLEDStk[APP_TASK_LED_STK_SIZE - 1],
                    APP_TASK_LED_PRIO,
                    APP_TASK_LED_PRIO,
                    (OS_STK *)&AppTaskLEDStk[0],
                    APP_TASK_LED_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_LED_PRIO, APP_TASK_LED_NAME, &err);
    
	/* 创建AppTaskCom任务 */
	OSTaskCreateExt(AppTaskCom,
                    (void *)0,
                    (OS_STK *)&AppTaskCOMStk[APP_TASK_COM_STK_SIZE-1],
                    APP_TASK_COM_PRIO,
                    APP_TASK_COM_PRIO,
                    (OS_STK *)&AppTaskCOMStk[0],
                    APP_TASK_COM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_COM_PRIO, APP_TASK_COM_NAME, &err);
					
	/* 创建AppTaskFlow任务 */
	OSTaskCreateExt(AppTaskFlow,
                    (void *)0,
                    (OS_STK *)&AppTaskFlowStk[APP_TASK_FLOW_STK_SIZE-1],
                    APP_TASK_FLOW_PRIO,
                    APP_TASK_FLOW_PRIO,
                    (OS_STK *)&AppTaskFlowStk[0],
                    APP_TASK_FLOW_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_FLOW_PRIO, APP_TASK_FLOW_NAME, &err);					

    /* 创建AppTaskKeyScan任务 */
	OSTaskCreateExt(AppTaskKeyScan,
                    (void *)0,
                    (OS_STK *)&AppTaskKeyScanStk[APP_TASK_KeyScan_STK_SIZE-1],
                    APP_TASK_KeyScan_PRIO,
                    APP_TASK_KeyScan_PRIO,
                    (OS_STK *)&AppTaskKeyScanStk[0],
                    APP_TASK_KeyScan_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_KeyScan_PRIO, APP_TASK_KeyScan_NAME, &err);
}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-II任务信息打印到串口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *ptcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	OS_STK_DATA stk;	        /* 用于读取任务栈使用情况 */

	ptcb    = &OSTCBTbl[0];		/* 指向第1个任务控制块(TCB) */

	/* 打印标题 */
	App_Printf("==================================================\r\n");
	App_Printf("  优先级   使用栈  剩余栈  百分比   任务名\r\n");
	App_Printf("  Prio     Used    Free    Per      Taskname\r\n");

	OSTimeDly(10);
	
	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (ptcb != NULL)
	{
		/* 
			ptcb->OSTCBPrio : 任务控制块中保存的任务的优先级
			ptcb->OSTCBTaskName : 任务控制块中保存的任务名称，需要在创建任务的时候
			调用OSTaskNameSet()指定任务名称，比如：
			OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
		*/
		OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* 获得任务栈已用空间 */
		App_Printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
			stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
			ptcb->OSTCBTaskName);		
		ptcb = ptcb->OSTCBPrev;		            /* 指向上一个任务控制块 */
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
