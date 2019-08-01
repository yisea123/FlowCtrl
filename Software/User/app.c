/*
*********************************************************************************************************
*	                                  
*	ģ������ : uCOS-II
*	�ļ����� : app.c
*	��    �� : V1.0
*	˵    �� : ��ʵ����Ҫʵ���̰߳�ȫ��printf��ʽ
*              ʵ��Ŀ�ģ�
*                1. ѧϰ�����uCOS-II��ʵ���̰߳�ȫ��printf��ʽ��
*              ʵ�����ݣ�
*                1. �����������¼�������ͨ�����°���K1����ͨ�����ڴ�ӡ�����ջʹ�����
*                   ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������
*                    Prio     Used    Free    Per      Taskname
*                     63       17      111    13%     uC/OS-II Idle
*                     62       21      107    16%     uC/OS-II Stat
*                      0       34      222    13%     Start Task
*                      2       88      168    34%     User Interface
*                     60       27      229    10%     LED
*                      3       27      229    10%     COM
*                      1       29      227    11%     KeyScan
*                    �����������ʹ��SecureCRT��V4���������д�������鿴��ӡ��Ϣ��
*                    Start Task����    ��ʵ��LED��˸��
*                    User Interface���񣺸����û��İ�����Ϣ��ʵ����Ӧ�Ĳ�����
*                    LED����           ��ʵ��LED��˸��
*                    KeyScan����       ����Ҫ��ʵ�ְ���ɨ�衣
*                    COM����           ��ʵ��LED��˸��
*                2. (1) �����õ�printf������ȫ��ͨ������App_Printfʵ�֡�
*                   (2) App_Printf���������ź����Ļ�������������Դ�������⡣
*
*              ע�����
*                 1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                    V4��������������С�
*                 2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��   ����         ����            ˵��
*       V1.0    2015-08-02   Eric2013    1. ST�̼��⵽V3.6.1�汾
*                                        2. BSP������V1.2
*                                        3. uCOS-II�汾V2.92.11
*                                        4. uC/CPU�汾V1.30.02
*                                        5. uC/LIB�汾V1.38.01
*                                       
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              �����ջ
********************************************************************************************************
*/

/* ����ÿ������Ķ�ջ�ռ䣬app_cfg.h�ļ��к궨��ջ��С */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskLEDStk[APP_TASK_LED_STK_SIZE];	
static OS_STK AppTaskCOMStk[APP_TASK_COM_STK_SIZE];	
static OS_STK AppTaskFlowStk[APP_TASK_FLOW_STK_SIZE];	
static OS_STK AppTaskKeyScanStk[APP_TASK_KeyScan_STK_SIZE];	
			
/*
*******************************************************************************************************
*                              ��������
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
*                               ����
*******************************************************************************************************
*/

#define KEY_SELECT    KEY_Line1_K1
#define KEY_EXIT      KEY_Line1_K2
#define KEY_UP        KEY_Line1_K3
#define KEY_DOWN      KEY_Line1_K4

#define KEY_CtrlInput KEY_Line1_K5
#define KEY_CtrlPump  KEY_Line1_K6



/* ����һ�����䣬 ��ֻ��һ������ָ�룬 OSMboxCreate�����ᴴ������������Դ */
static OS_EVENT *AppUserIFMbox;

/*����һ���ź���*/
static OS_EVENT *AppPrintfSemp;

struct FlashParam
{
	uint32_t Flg; /* ��ֹ����ϵ����ݳ��� */
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
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*******************************************************************************************************
*/
int main(void)
{
	INT8U  err;

	/* ��ʼ��"uC/OS-II"�ں� */
	OSInit();
  
	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	OSTaskCreateExt(AppTaskStart,	/* ����������ָ�� */
                    (void *)0,		/* ���ݸ�����Ĳ��� */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1], /* ָ������ջջ����ָ�� */
                    APP_TASK_START_PRIO,	/* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                    APP_TASK_START_PRIO,	/* ����ID��һ����������ȼ���ͬ */
                    (OS_STK *)&AppTaskStartStk[0],/* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                    APP_TASK_START_STK_SIZE, /* ����ջ��С */
                    (void *)0,	/* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                       ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
					
					/*  �������£�
						OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
						OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
						OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
					*/                  

	/* ָ����������ƣ����ڵ��ԡ���������ǿ�ѡ�� */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
	
	/*ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295*/ 
	OSTimeSet(0);	
	
	/* ����������ϵͳ������Ȩ����uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
	uint32_t key = 0;
	uint8_t i = 0;
	
    /* �����ڱ���������澯����������������κ�Ŀ����� */	
    (void)p_arg;  
	
	 
	/* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
	bsp_Init();
    CPU_Init();          
	BSP_Tick_Init();   
	

	/* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ���� */
	#if (OS_TASK_STAT_EN > 0)
		OSStatInit();
	#endif	
		
	/* ����Ӧ�ó�������� */
	AppTaskCreate();
	
	OSTimeDlyHMSM(0, 0, 5, 0);
	APPL_RestartAbcc();  /* ��λABCC */
	
	while (1)     
	{ 	
		OSTimeDlyHMSM(0, 0, 1, 0);
		printf("input:%dhz  output:%dhz\n", 1000000/(g_usHuba1*10), 1000000/(g_usHuba2*10));
	}      
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskKeyScan
*	����˵��: ����ɨ��	
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 1
*********************************************************************************************************
*/
static void AppTaskKeyScan(void *p_arg)
{
	uint8_t ucKeyCode = 0;	/* �������� */
	uint8_t ucTemp = 0;
	(void)p_arg;
		  
	while(1)
	{    
		ucTemp = bsp_Tm1638ReadKey();;   /* ��ȡ���� */
		if (ucTemp > 0)
		{	
			ucKeyCode = ucTemp;
			/* ���������뷢�͵����� */
			OSMboxPost(AppUserIFMbox, &ucKeyCode);
		}

		/* �ӳ�10ms�������ͷ�CPU����������ȼ���������� */    	
		OSTimeDlyHMSM(0, 0, 0, 300);	 /* Ҳ���Ե��� OSTimeDly() ����ʵ���ӳ� */						  	 	       											  
	}   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ��������Ҫ���ڵõ������ļ�ֵ��
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 2
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
		if (err == OS_ERR_NONE)                             /* �޴��ʾ�ɹ����յ�һ����Ϣ */
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
				bsp_Tm1638Disp(0, SupplyFlowInLPM/100%10, SupplyFlowInLPM/10%10, SupplyFlowInLPM%10, 1);   /* ��ʾ���� */
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
*	�� �� ��: AppTaskCom
*	����˵��: LED4��˸�ʹ��ڴ�ӡ
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
*********************************************************************************************************
*/
static void AppTaskCom(void *p_arg)
{	
	int cnt = 0;
	APPL_AbccHandlerStatusType eAbccHandlerStatus = APPL_MODULE_NO_ERROR;
	(void)p_arg;

	bsp_abccInit();     /* abcc��ʼ�� */
	
	
	
	while(1)
	{		 		 
		eAbccHandlerStatus = APPL_HandleAbcc();
		//B40ʱ����MCU����APPL_TIMER_MS,��Ҫ֪ͨB40
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
*	�� �� ��: AppTaskFlow
*	����˵��: Flow����ģ��		  			  
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
static void AppTaskFlow(void *p_arg)
{
	uint8_t timeout = 0;
	uint32_t leakTime = 0;
    (void)p_arg;		/* ����������澯 */

	/* ��ȡ��ر��� */
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
	
	if(timeout == 10) /* �޷�����������ͨ�� */
	{
		OSTimeDlyHMSM(0, 0, DelayToDetect, 0);
		CtrlInputWaterCtrl(RELAY_ON);   /* ͨ���ˮ�� */
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
			SetFlowReset = 0; /* ������ */
			NVIC_SystemReset();
		}
		
		if((SetCloseValue == 1) && (ValueClosed == 0))             /* 1:�رյ�ŷ�/���ϵ� - �رս�ˮ */
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
				CtrlInputWaterCtrl(RELAY_ON);  /* �򿪵�ŷ�  - �򿪽�ˮ */
				FlowOff = 0;
			}
			Bypass = 1;                    /* BypassΪ1����Relay1���ٹر�  */			
		}
		else if((SetByPass == 0) && (Bypass == 1))
		{
			Bypass = 0;    
		}

		/* �Զ������缫ñ */
		if(SetExchangeCapReset == 1)	 
		{
			ValueClosed = 1;
			CtrlInputWaterCtrl(RELAY_OFF); /* ��ŷ��ϵ� */
			FlowOff = 1;
			OSTimeDlyHMSM(0, 0, 0, 500);   /* ��ʱ0.5s */
			CtrlPumpWater(RELAY_ON);       /* ������ˮ�� */ 
			OSTimeDlyHMSM(0, 0, 5, 0);	   /* ��ʱ1min �ȴ���ˮ�ù������ */
			OKToExchangeCap = 1;		   /* OK_To_Exchange �ź���ΪOK�����Կ�ʼ�����缫ñ */
			while(SetExchangeCapReset == 1)
			{
				OSTimeDlyHMSM(0, 0, 1, 0);	/* ��ʱ�ȴ���ñ��� */	  
			}
			OKToExchangeCap = 0;			/* ���ɸ����缫ñ */ 
			ValueClosed = 0;
			CtrlInputWaterCtrl(RELAY_ON);    /* �򿪽�ˮ�� */
			FlowOff = 0;
		}
		
		CalcFlowValue();  /* ����Flow Rate */
		
		/* ��ˮ�����쳣 */
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
		
		/* �缫ñ��ʧ��� */
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
		
		/* flash �������� */
		writeParamFromFlash();
		
		OSTimeDlyHMSM(0, 0, 0, 5);		
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLED
*	����˵��: LED2��˸�ʹ��ڴ�ӡ		  			  
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/
static void AppTaskLED(void *p_arg)
{
	uint16_t PowerVal = 0;
	
    (void)p_arg;		/* ����������澯 */
		
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
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
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
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	INT8U      err;


	/* ��������(MBOX) */
	AppUserIFMbox = OSMboxCreate((void *)0);
	AppPrintfSemp = OSSemCreate(1);	  /* ����һ���ź��� ʵ���ź������� */


	/* ����AppTaskUserIF���� */
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
    
	/* ����AppTaskLED���� */
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
    
	/* ����AppTaskCom���� */
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
					
	/* ����AppTaskFlow���� */
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

    /* ����AppTaskKeyScan���� */
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
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-II������Ϣ��ӡ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *ptcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	OS_STK_DATA stk;	        /* ���ڶ�ȡ����ջʹ����� */

	ptcb    = &OSTCBTbl[0];		/* ָ���1��������ƿ�(TCB) */

	/* ��ӡ���� */
	App_Printf("==================================================\r\n");
	App_Printf("  ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������\r\n");
	App_Printf("  Prio     Used    Free    Per      Taskname\r\n");

	OSTimeDly(10);
	
	/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
	while (ptcb != NULL)
	{
		/* 
			ptcb->OSTCBPrio : ������ƿ��б������������ȼ�
			ptcb->OSTCBTaskName : ������ƿ��б�����������ƣ���Ҫ�ڴ��������ʱ��
			����OSTaskNameSet()ָ���������ƣ����磺
			OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
		*/
		OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* �������ջ���ÿռ� */
		App_Printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio, 
			stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
			ptcb->OSTCBTaskName);		
		ptcb = ptcb->OSTCBPrev;		            /* ָ����һ��������ƿ� */
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
