#include "led.h"
#include "delay.h"
#include "sys.h"
#include "Tiky_LCD.h" 
#include "usart.h"
#include "sram.h"
#include "timer.h"
#include "malloc.h"
#include "GUI.h"
#include "GUIDEMO.h"
#include "includes.h"
#include "ec11key.h"
#include "adc.h"
#include "temp.h"
#include "control.h"
#include "PID.h"
#include "common.h"
#include "dma.h"

//START����
//������������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С 
#define START_STK_SIZE			  256
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//TEMP����
//�����������ȼ�
#define TEMP_TASK_PRIO 				5
//�����ջ��С
#define TEMP_STK_SIZE				64
//������ƿ�
OS_TCB TempTaskTCB;
//�����ջ
CPU_STK TEMP_TASK_STK[TEMP_STK_SIZE];
//led0����
void temp_watch_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			6
//�����ջ��С
#define EMWINDEMO_STK_SIZE			2048
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

//Run����
//�����������ȼ�
#define RUN_TASK_PRIO 				7
//�����ջ��С
#define RUN_STK_SIZE				64
//������ƿ�
OS_TCB RUNTaskTCB;
//�����ջ
CPU_STK RUN_TASK_STK[RUN_STK_SIZE];
//led0����
void run_task(void *p_arg);


////////////////////////////////////////////////////////
OS_TMR 	tmr1;		//��ʱ��1
OS_TMR	tmr2;		//��ʱ��2
void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����
void tmr2_callback(void *p_tmr, void *p_arg);	//��ʱ��2�ص�����

u32 Check_Sys=0;

// Target�¶�ֵ
extern const short baseTempMax[];
extern EVENT_VAL g_event_val_new;// ��ǰ״̬
extern EVENT_VAL g_event_val_last;// ��һ��״̬
extern TEMP_VAL g_temp_val_new;// ��ǰ�¶�
extern TEMP_VAL g_temp_val_last;// ��һ���¶�

//
// User Interface
//

// �Ƿ������ش���
// 0-OK 1-FLAME ERROR 2-SENSOR ERROR
extern int g_fatal_error;

// ����ģʽ�Ŀ�ʼ�ͽ�����־
// 0-end 1-start
extern int g_startup_mode;
extern int g_run_mode;
extern int g_feed_mode;
extern int g_shutdown_mode;

int g_startup_mode_last;
int g_run_mode_last;
int g_feed_mode_last;
int g_shutdown_mode_last;

// ����ģʽ�ļ�ʱ��
extern u16 g_startup_mode_counter;
extern u16 g_run_mode_counter_mins;// minutes
extern u16 g_run_mode_counter_sec;// seconds
extern u16 g_run_mode_counter_hour;// hours
extern u16 g_feed_mode_counter;
extern u16 g_shutdown_mode_counter;


// RUNģʽ����ʱ��ʱ�������Լ�ʹ��
// ��λseconds(Ĭ��480s)
extern int g_run_timer_setting;

// 0-���϶�(Ĭ��) 1���϶�
// g_temp_val_new.temp_unit

// �Ҳ��¶���(��Ӧ�¶Ȱ�1234)
// g_temp_val_new.temp1
// g_temp_val_new.temp2
// g_temp_val_new.temp3
// g_temp_val_new.temp4

// RUN�������������¶�
// g_temp_val_new.temp5

// �¶Ȱ��Ƿ���쳣
// 0-OK(default) 1-�¶Ȱ��쳣
extern int g_temp1_error;
extern int g_temp2_error;
extern int g_temp3_error;
extern int g_temp4_error;
extern int g_temp5_error;

// ��ʾ�Ƿ��ʼ��
// 0-δ��ʼ��(default), 1-�ѳ�ʼ��
extern int g_factory_reseted;

// ����startup����run��־
// 0-run, 1-startup mode(default)
extern int g_startup_enable;

// smoke�������ٷֱȣ�Ĭ��ֵΪ50(%)
extern int g_smoke_val_percent;
extern int g_target_temp_val;
extern int g_set_temp;

extern int g_temp_center;

// EMWIN�������
extern void GUIDEMO_AfterLogo(void);
extern void GUIDEMO_UpdateTemp(int *temp_val);

extern int g_time_remain;

#if 1
// LOGOͼƬת������
extern const unsigned char gImage_camp[1308];

// ��ȡÿ�����ص��2�ֽ���ɫ��Ϣ
u16 image_getcolor(u8 mode,u8 *str)
{
	u16 color;
	if(mode)
	{
		color=((u16)*str++)<<8;
		color|=*str;
	}else
	{
		color=*str++;
		color|=((u16)*str)<<8;
	}
	return color;	
}

// д��һ������յ����꣬Ȼ��д����������(��ɫ��Ϣ)
void image_show(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 scan,u8 *p)
{  
	u32 i;
	u32 len=0;
	
	BlockWrite(xsta,xend,ysta,yend);
	
	len=(xend-xsta+1)*(yend-ysta+1);	//д������ݳ���
	for(i=0;i<len;i++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = image_getcolor(scan&(1<<4),p);
		p+=2;
	}	    					  	    
} 

void image_display(u16 x,u16 y,u8 * imgx)
{
	HEADCOLOR *imginfo;
 	u8 ifosize=sizeof(HEADCOLOR);//�õ�HEADCOLOR�ṹ��Ĵ�С
	imginfo=(HEADCOLOR*)imgx;
 	image_show(x,y,x+imginfo->w-1,y+imginfo->h-1,imginfo->scan,imgx+ifosize);
}

// ��ָ��λ�ÿ�ʼ��ʾһ��ͼƬ
void disp_img(u16 x, u16 y)
{
	image_display(x,y,(u8*)gImage_camp);//��ָ����ַ��ʾͼƬ
}
#endif

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();	
	
	delay_init();	    	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
#ifdef SRAM_MEMDEV
	FSMC_SRAM_Init();		//��ʼ��SRAM
#endif
 	LED_Init();			    //LED�˿ڳ�ʼ��
	Lcd_Initialize();
	
	// ������ɨ�跽ʽ����
	LCD_Display_Dir(HORIZON_DISPLAY);

	EC11_EXTI_Init();//EC11��������ʼ��
	my_mem_init(SRAMIN); 	//��ʼ���ڲ��ڴ��
	
	Adc_Init();
	Control_Init();

	//PID_Init();
	
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	Usart_DMA_Init();
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
							 
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
#if 1
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  			//STemWin��ʼ��
#if 0
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//0
                (OS_TICK	 )10,          //100*10=1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
				
				
	//������ʱ��2
	OSTmrCreate((OS_TMR		*)&tmr2,		
                (CPU_CHAR	*)"tmr2",		
                (OS_TICK	 )200,			//200*10=2000ms	
                (OS_TICK	 )0,   					
                (OS_OPT		 )OS_OPT_TMR_ONE_SHOT, 	//���ζ�ʱ��
                (OS_TMR_CALLBACK_PTR)tmr2_callback,	//��ʱ��2�ص�����
                (void	    *)0,			
                (OS_ERR	    *)&err);
	OSTmrStart(&tmr1,&err);	//������ʱ��1
	OSTmrStart(&tmr2,&err);	//������ʱ��2
#endif
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	// STemWin UI����	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
#endif
#if 1
	// EC11��������
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);		
#endif
#if 1
	// �¶ȶ�ȡ����
	OSTaskCreate((OS_TCB*     )&TempTaskTCB,		
				 (CPU_CHAR*   )"Temp task", 		
                 (OS_TASK_PTR )temp_watch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TEMP_TASK_PRIO,     
                 (CPU_STK*    )&TEMP_TASK_STK[0],	
                 (CPU_STK_SIZE)TEMP_STK_SIZE/10,	
                 (CPU_STK_SIZE)TEMP_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
#endif	
#if 1
	// RUN����
	OSTaskCreate((OS_TCB*     )&RUNTaskTCB,		
				 (CPU_CHAR*   )"RUN task", 		
                 (OS_TASK_PTR )run_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )RUN_TASK_PRIO,     
                 (CPU_STK*    )&RUN_TASK_STK[0],	
                 (CPU_STK_SIZE)RUN_STK_SIZE/10,	
                 (CPU_STK_SIZE)RUN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
#endif								 
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//��ʱ��1�Ļص����� 100ms
void tmr1_callback(void *p_tmr, void *p_arg)
{
	static u8 tmr1_num_s=0;//1S
	tmr1_num_s++;
	if(tmr1_num_s>=10){
//		if(g_startup_mode) g_startup_mode_counter++;
//		else g_startup_mode_counter=0;
		
//		if(g_feed_mode) g_feed_mode_counter++;
//		else g_feed_mode_counter=0;
		
//		if(g_shutdown_mode) g_shutdown_mode_counter++;
//		else g_shutdown_mode_counter=0;
		
		tmr1_num_s=0;
	}

/*	static u32 tmr1_num=0;
//	LCD_ShowxNum(62,111,tmr1_num,3,16,0x80); //��ʾ��ʱ��1��ִ�д���
//	LCD_Fill(6,131,114,313,lcd_discolor[tmr1_num%14]);//�������
	tmr1_num++;		//��ʱ��1ִ�д�����1*/
//	Check_Sys++;
}

//��ʱ��2�Ļص�����
void tmr2_callback(void *p_tmr,void *p_arg)
{
/*	static u8 tmr2_num = 0;
	tmr2_num++;		//��ʱ��2ִ�д�����1
	LCD_ShowxNum(182,111,tmr2_num,3,16,0x80);  //��ʾ��ʱ��1ִ�д���
	LCD_Fill(126,131,233,313,lcd_discolor[tmr2_num%14]); //�������
	LED1 = ~LED1;
	printf("��ʱ��2���н���\r\n");*/
}
extern void GUIDEMO_StartupTimerUI(void);
extern void GUIDEMO_MainMenu(int sel);
extern void GUIDEMO_DayNightSwitch(void);
extern void GUIDEMO_DayModeSet(void);
extern void GUIDEMO_ResetInitialUI(void);
extern void GUIDEMO_MainMenu(int sel);
extern void GUIDEMO_StartupInitialUI(int mode);
extern void GUIDEMO_BigCircleUI(int is_smoke_ui);
extern void GUIDEMO_StartupTimerBypassedUI(void);
extern void GUIDEMO_LeftOneExitUI(int time1, int time2,int ui_sel);
// EMWIN�������
void emwindemo_task(void *p_arg)
{
	EVENT_VAL evt_val = g_event_val_last;
	TEMP_VAL temp_val = g_temp_val_last;
		EC11_STA ec_sta = EC11_IDLE;	
	int flag=0;
	int time_remain = 10;
		int temp_val1[5]={11,12,13,14};
	// �ô��¶�Ӧ��Ϊ���¶Ȱ�����
	// ����¶Ȱ�û�ӻ��߶�������������Ϊ0���¶Ȼ���ʾΪ"-"
	// �ô�Ϊ��һ����ʾ��������temp_watch_task�и����¶���Ϣ����
	temp_val.temp1 = 0;
	temp_val.temp2 = 0;
	temp_val.temp3 = 0;
	temp_val.temp4 = 0;
	temp_val.temp5 = 0;
	
	// Ĭ��ֵ������������

	g_temp_val_new.target_smoke=1;
	g_temp_val_new.target_val=155;

	//g_temp_val_new = temp_val;
	g_event_val_new = evt_val;
	
	LCD_Clear(WHITE);

#if 1
	disp_img(50, 100);
	delay_ms(5000);
#endif

	while(1)
	{
		
		GUIDEMO_AfterLogo();
	if(g_fatal_error!=0)
	{
		flag =0;
		time_remain=10;
		if(g_fatal_error==1)//flame
		{
			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_FLAME_ERROR);
		}else if(g_fatal_error==2)//sensor
		{
			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_SENSOR);
		}else if(g_fatal_error==3)//overtemp	
		{
			GUIDEMO_LeftOneExitUI(12, 0, UI_EXIT_OVER_TEMP);
		}
		
		//GUIDEMO_RightTempUI(temp_val1, 1, big, 0);//?4??
		
		//g_fatal_error=0;
		g_startup_mode=0;
		g_run_mode=0;
		EC11_Clear();
		
		do{
			// ??EC11???????
			ec_sta = EC11_KEY_Scan(0);

			if ((0 == time_remain%10)) {
				if(flag==0)
				{
					GUIDEMO_ClearScreen(243,0,480,320);
					GUIDEMO_RightTempUI(temp_val1, 1, 0, 0);//????4???	
					flag =1;
				}
				GUIDEMO_UpdateTemp(temp_val1);
				
				GUIDEMO_RightTempUI(temp_val1, 1, 0, 1);//????4???

				GUIDEMO_RightTempUI(temp_val1, 1, 0, 2);//??big??
			}
			time_remain--;
			
			if (0 == time_remain) {
				time_remain = 10;
			}
		
			if (EC11_BUT == ec_sta)
			{
				ec_sta = EC11_IDLE;
				//GUIDEMO_BigCircleUI(2);
				g_fatal_error=0;//�������
				break;
				
			}
			delay_ms(100);
		} while(EC11_BUT != ec_sta);
	}
	}
}

// ÿ100ms��ɨ��һ��EC11������״̬
void touch_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		EC11_BUT_Scan();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
	}
}

// ÿ��2�����һ���¶���Ϣ
// �ú����е��¶���ϢӦȫ����Ϊ���¶Ȱ���ȡ
void temp_watch_task(void *p_arg)
{
	OS_ERR err;
	u8 pid_counter = 0;
	TEMP_VAL temp_val = g_temp_val_last;
	
#if FOR_DEBUG_USE
	int tmpval  = 100;
	int testcnt = 0;
#endif
		
	// �ô��¶�Ӧ��Ϊ���¶Ȱ�����
	// ����¶Ȱ�û�ӻ��߶�������������Ϊ0���¶Ȼ���ʾΪ"-"
	temp_val.temp1 = 0;
	temp_val.temp2 = 0;
	temp_val.temp3 = 0;
	temp_val.temp4 = 0;
	temp_val.temp5 = 0;
	
	// Ĭ��ֵ������������
	//temp_val.target_val = 450;
	
	while(1)
	{
		LED0 = !LED0;
		
		// �ô��¶�Ӧ��Ϊ���¶Ȱ�����
		// ����¶Ȱ�û�ӻ��߶�������������Ϊ0���¶Ȼ���ʾΪ"-"
/*		temp_val.temp1 += 10;
		temp_val.temp2 += 15;
		temp_val.temp3 += 20;
		temp_val.temp4 += 25;
		temp_val.temp5 += 30;
		
		if (temp_val.temp1 > 500) {
			temp_val.temp1 = 0;
		}
		if (temp_val.temp2 > 500) {
			temp_val.temp2 = 0;
		}
		if (temp_val.temp3 > 500) {
			temp_val.temp3 = 0;
		}
		if (temp_val.temp4 > 500) {
			temp_val.temp4 = 0;
		}
		if (temp_val.temp5 > 500) {
			temp_val.temp5 = 0;
		}
		
		temp_val.temp1 %= baseTempMax[1] + 1;
		temp_val.temp2 %= baseTempMax[1] + 1;
		temp_val.temp3 %= baseTempMax[1] + 1;
		temp_val.temp4 %= baseTempMax[1] + 1;
		temp_val.temp5 %= baseTempMax[1] + 1;
		
		// ������ȫ�ֱ���
		g_temp_val_new.temp1 = temp_val.temp1;
		g_temp_val_new.temp2 = temp_val.temp2;
		g_temp_val_new.temp3 = temp_val.temp3;
		g_temp_val_new.temp4 = temp_val.temp4;
		g_temp_val_new.temp5 = temp_val.temp5;*/
		
		// ������ȫ�ֱ���
#if FOR_DEBUG_USE	
		g_temp_val_new.temp1 = (tmpval + testcnt * 1) % 501;
		g_temp_val_new.temp2 = (tmpval + testcnt * 2) % 501;
		g_temp_val_new.temp3 = (tmpval + testcnt * 3) % 501;
		g_temp_val_new.temp4 = (tmpval + testcnt * 4) % 501;
		g_temp_val_new.temp5 = (tmpval + testcnt * 5) % 701;
		
		testcnt++;
		
		if ((g_temp_val_new.temp5 > 20) && (g_temp_val_new.temp5 < 100)) {
//			g_temp1_error=1;
//			g_temp2_error=1;
//			g_temp3_error=1;
//			g_temp4_error=1;
//			g_temp5_error=1;
		}else
		
		//if (testcnt > 50) 
			{
			g_temp1_error=0;
			g_temp2_error=0;
			g_temp3_error=0;
			g_temp4_error=0;
			g_temp5_error=0;
		}
#else
		g_temp_val_new.temp1 = (int)Get_Temperature(ADC_Channel_6);
		g_temp_val_new.temp2 = (int)Get_Temperature(ADC_Channel_5);
		g_temp_val_new.temp3 = (int)Get_Temperature(ADC_Channel_4);
		g_temp_val_new.temp4 = (int)Get_Temperature(ADC_Channel_3);
		g_temp_val_new.temp5 = (int)Get_Temperature(ADC_Channel_2);
#endif

		if (0 == pid_counter) {
			PID_dt++;
			S1_Counter_TempAvg++;
		}
		
#if	FOR_PID_USE
		PID_Ctr(g_temp_val_new.temp5, g_set_temp);
		// �ú�����õ���2��ֵ:Auger_On, Auger_Off,�ֱ��ʾ����ȿ�Auger_On�룬�ٹر�Auger_Off��
#endif		
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
		
		pid_counter++;
		if (10 == pid_counter) {
			pid_counter = 0;
		}
	}
}

// ����ģʽ
void run_task(void *p_arg)
{
	OS_ERR err;
	int cur_temp=0;
//	int last_temp=0;
	int total_sec=0;
//	int sec_T1=0;
//	int sec_T2=0;
	int sec_ON=0;
	int sec_OFF=0;
	int sec_ON_left=0;
	int sec_OFF_left=0;
	int temp=0;
	
	int flame_run_first_in=1;
	int flame_run_sec=0;
	int flame_run_hot_status=0;
	
	g_startup_mode_last = 0;
	g_run_mode_last = 0;
	g_feed_mode_last = 0;
	g_shutdown_mode_last = 0;
	
	MOT = Control_OFF;		
	HOT = Control_OFF;
	FAN = Control_OFF;
//	MOT = Control_ON;		
//	HOT = Control_ON;
//	FAN = Control_ON;

	while(1)
	{
		if (1 == g_startup_mode) {	//start mode 
			Startup_Mode(360 - g_startup_mode_counter);//g_startup_mode_counter 360s in GUIDEMO.c 
			if (0 == g_startup_mode_counter) {
				g_startup_mode = 0;
			}
			if (g_startup_mode_counter > 0) {
				g_startup_mode_counter--;
			}
		}else{
			g_startup_mode_counter=0;
		}
		
		if (1 == g_run_mode) {
			
			if (59 == g_run_mode_counter_sec) {// running timing
				g_run_mode_counter_sec = 0;
				if (59 == g_run_mode_counter_mins) {
					g_run_mode_counter_mins = 0;
					g_run_mode_counter_hour++;
				} else {
					g_run_mode_counter_mins++;
				}
			} else {
				g_run_mode_counter_sec++;
			}
			
			Run_Mode_Start();//fan is always on
			
			total_sec=(g_run_mode_counter_hour*3600 + g_run_mode_counter_mins* 60 + g_run_mode_counter_sec);
			
			
			if(g_flame_update_status == 1) //	 Set temperature changes, update flame
			{
				g_flame_update_status=0;
				flame_run_sec=0;
				flame_run_first_in=1;
				
				Run_Mode_HOT_ON(0);//Turn off the hot
				flame_run_hot_status=0;
		
			}
			if(((g_set_temp<190)||(g_set_temp>250))&&(g_temp_val_new.temp5 < 135))//	Check for Flame-Out condition.
			{
				flame_run_sec++;
				if((flame_run_sec > 15*60) && (flame_run_first_in!=0))//Record temp at 15 min
				{
					cur_temp  = g_temp_val_new.temp5;
					flame_run_sec=0;
					flame_run_first_in=0;
				}else if((flame_run_sec> 3*60) && (!flame_run_first_in) )//Compare the temperature every 3 minutes
				{
					if(g_temp_val_new.temp5>cur_temp) 
					{
					cur_temp  = g_temp_val_new.temp5;
					flame_run_sec=0;
					}else g_fatal_error =1;//flame err ����
				}
			}else if((g_set_temp>=190)&&(g_set_temp<=200)&&(g_set_temp-g_temp_val_new.temp5>=10))//�²��10��
			{
				flame_run_sec++;
				if(flame_run_first_in == 1)
				{
					cur_temp  = g_temp_val_new.temp5;
					flame_run_first_in=0;
				}
				
				if(flame_run_sec>18*60)// 18*60Flame-out error and shutdown after 18 min
				{	
						g_fatal_error =1;//flame err ����
				}else if(flame_run_sec>12*60)  // 12*60Start sampling for increase in temp again, 12 min
				{
					if(g_temp_val_new.temp5>cur_temp) 
					{
						flame_run_sec=0;
						flame_run_first_in = 1;

					}
				}else if(flame_run_sec>9*60) // 9*60 turn off hotrod after 6 min
				{
					Run_Mode_HOT_ON(0);//Turn off the hot
					flame_run_hot_status=0;

				}else if((flame_run_sec>3*60)&&(flame_run_hot_status == 0))//3*60
				{
					if(g_temp_val_new.temp5>cur_temp) flame_run_sec=0;
					else
					{
						Run_Mode_HOT_ON(1);//Turn on the hot
						flame_run_hot_status=1;
					}
					
						flame_run_first_in = 1;
				}
			}else if((g_set_temp>200)&&(g_set_temp<=250)&&(g_set_temp-g_temp_val_new.temp5>=20))//�²��20��
			{
				flame_run_sec++;
				if(flame_run_first_in == 1)
				{
					cur_temp  = g_temp_val_new.temp5;
					flame_run_first_in=0;
				}
				
				if(flame_run_sec> 18*60)// Flame-out error and shutdown after 18 min
				{	
						g_fatal_error =1;//flame err ����
				}else if(flame_run_sec> 12*60)  // Start sampling for increase in temp again, 12 min
				{
					if(g_temp_val_new.temp5>cur_temp) 
					{
						flame_run_sec=0;
						flame_run_first_in = 1;

					}
				}else if(flame_run_sec> 9*60) // turn off hotrod after 6 min
				{
					Run_Mode_HOT_ON(0);//Turn off the hot
					flame_run_hot_status=0;

				}else if((flame_run_sec> 3*60)&&(flame_run_hot_status == 0))
				{
					if(g_temp_val_new.temp5>cur_temp) flame_run_sec=0;
					else
					{
						Run_Mode_HOT_ON(1);//Turn on the hot
						flame_run_hot_status=1;
					}
					
					flame_run_first_in = 1;

				}
			}
			else
			{
					flame_run_sec=0;
					flame_run_first_in=1;
				
					Run_Mode_HOT_ON(0);//Turn off the hot
					flame_run_hot_status=0;
			}
			
			
			
#if FOR_Time_USE  //time control
		if(g_target_temp_val == 150)//low smoke
		{
			if(g_temp_val_new.temp5 > 160)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=10;
				Auger_Off=25;
			}
		}else if(g_target_temp_val == 155)//high smoke
		{
			if(g_temp_val_new.temp5 > 220)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=20;
				Auger_Off=25;
			}
		}else if((g_target_temp_val >= 160) &&(g_target_temp_val <= 210))
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=6;
				Auger_Off=10;
			}
		}else if((g_target_temp_val > 210) &&(g_target_temp_val <= 260))
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=10;
				Auger_Off=10;
			}
		}else if((g_target_temp_val > 260) &&(g_target_temp_val <= 310))
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=20;
				Auger_Off=10;
			}
		}else if((g_target_temp_val > 310) &&(g_target_temp_val <= 360))
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=30;
				Auger_Off=10;
			}
		}else if((g_target_temp_val > 360) &&(g_target_temp_val <= 450))
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=40;
				Auger_Off=10;
			}
		}else if(g_target_temp_val > 450)
		{
			if(g_temp_val_new.temp5 > g_target_temp_val)
			{
				Auger_On=6;
				Auger_Off=40;
			}else
			{
				Auger_On=0;
				Auger_Off=0;
			}
		}
#endif 			
			
			
			
				if((sec_ON != Auger_On) || (sec_OFF != Auger_Off))
				{//�����ر�ʱ��仯
					sec_ON_left = Auger_On;
					sec_OFF_left = Auger_Off;
					
					sec_ON = Auger_On;
					sec_OFF= Auger_Off;
				}
				
				if(Auger_On!=0)
				{
					if(sec_ON_left>0)
					{
						Run_Mode_MOT_ON(1);
						sec_ON_left--;
					}else if(sec_OFF_left>0)
					{
						Run_Mode_MOT_ON(0);
						sec_OFF_left--;
					}
					
					if((sec_ON_left==0) && (sec_OFF_left==0))
					{
						sec_ON_left=sec_ON;
						sec_OFF_left=sec_OFF;
					}
				}
				else Run_Mode_MOT_ON(1);//The motor continues to keep going
			
		 
		} else 
		{
			
			flame_run_sec=0;
			flame_run_first_in=1;
			flame_run_hot_status=0;
	
			g_run_mode_counter_mins = 0;
			g_run_mode_counter_sec = 0;
			g_run_mode_counter_hour = 0;
		}
		if((g_temp_val_new.temp5 > 600)&&((1 == g_run_mode)||(1 == g_startup_mode)))
		{
			g_fatal_error =3;//flame over temp ���±���
			Run_Mode_FLAME_OVER();
		}	
				
		if((g_temp5_error == 1)&&((1 == g_run_mode)||(1 == g_startup_mode)))
		{
			g_fatal_error =2;//flame sensor error RTD ����
			Run_Mode_FLAME_OVER();
		}	
		if (1 == g_feed_mode) {
			Feed_Mode(420 - g_feed_mode_counter);// 7 minutes
			if (0 == g_feed_mode_counter) {
				g_feed_mode = 0;
			}
			if (g_feed_mode_counter > 0) {
				g_feed_mode_counter--;
			}
		} else {
			g_feed_mode_counter = 0;
		}
		
		if (1 == g_shutdown_mode) {
			Shutdown_Mode(1200 - g_shutdown_mode_counter);// 20 minutes
			temp = 1200 - g_shutdown_mode_counter;
			if((temp<5*60)||(temp>15*60))
				Run_Mode_MOT_ON(0);
			else
			{
				if((temp-5*60)%210 == 0)
					Run_Mode_MOT_ON(1);//����10s
				if((temp-5*60)%210 == 10)
					Run_Mode_MOT_ON(0);//�ر�200s
			}
			
			if (0 == g_shutdown_mode_counter) {
				g_shutdown_mode = 0;
			}
			if (g_shutdown_mode_counter > 0) {
				g_shutdown_mode_counter--;
			}
		} else {
			g_shutdown_mode_counter = 0;
		}
	
		if (g_startup_mode_last != g_startup_mode)
		{
			g_startup_mode_last = g_startup_mode;
			if(g_startup_mode==0)
				Run_Mode_FLAME_OVER();
		}
		if (g_run_mode_last != g_run_mode)
			g_run_mode_last = g_run_mode;
		if (g_feed_mode_last != g_feed_mode)
		{
			g_feed_mode_last = g_feed_mode;
			if(g_feed_mode==0)
				Run_Mode_MOT_ON(0);
		}
		if (g_shutdown_mode_last != g_shutdown_mode)
		{
			g_shutdown_mode_last = g_shutdown_mode;
			if(g_shutdown_mode==0)
				Run_Mode_FLAME_OVER();
		}
	
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
	}
}

