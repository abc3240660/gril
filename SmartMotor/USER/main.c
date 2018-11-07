#include "includes.h"
#include "malloc.h"
#include "common.h"
#include "usart3.h"
#include "usart5.h"
#include "sim900a.h"
#include "can1.h"
#include "can2.h"

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ��8�ֽڶ���	
__align(8) static OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//��������
//�����������ȼ�
#define USART_TASK_PRIO       			7 
//���������ջ��С
#define USART_STK_SIZE  		    	128
//�����ջ��8�ֽڶ���	
__align(8) static OS_STK USART_TASK_STK[USART_STK_SIZE];
//������
void usart_task(void *pdata);
							 
//������
//�����������ȼ�
#define MAIN_TASK_PRIO       			6 
//���������ջ��С
#define MAIN_STK_SIZE  					1200
//�����ջ��8�ֽڶ���	
__align(8) static OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//��������
//�����������ȼ�
#define WATCH_TASK_PRIO       			3 
//���������ջ��С
#define WATCH_STK_SIZE  		   		256
//�����ջ��8�ֽڶ���	
__align(8) static OS_STK WATCH_TASK_STK[WATCH_STK_SIZE];
//������
void watch_task(void *pdata);
//////////////////////////////////////////////////////////////////////////////	 

//ϵͳ��ʼ��
void system_init(void)
{
	u8 CAN1_mode=0; //CAN����ģʽ;0,��ͨģʽ;1,����ģʽ
	u8 CAN2_mode=0; //CAN����ģʽ;0,��ͨģʽ;1,����ģʽ	
	
	delay_init(168);			//��ʱ��ʼ��  
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);		//��ʼ������3������Ϊ115200
	usart5_init(38400);
 	LED_Init();					//��ʼ��LED 
// 	KEY_Init();					//������ʼ�� 
//	W25QXX_Init();				//��ʼ��W25Q128

	CAN1_Mode_Init(CAN1_mode);//CAN��ʼ����ͨģʽ,������250Kbps
	CAN2_Mode_Init(CAN2_mode);//CAN��ʼ����ͨģʽ,������500Kbps 
  
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	
//	exfuns_init();				//FATFS �����ڴ�

	delay_ms(1500);
}   

//main����	  					
int main(void)
{ 	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
  system_init();		//ϵͳ��ʼ�� 
 	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	  						    
}
//extern OS_EVENT * audiombox;	//��Ƶ������������
//��ʼ����
void start_task(void *pdata)
{  
	OS_CPU_SR cpu_sr=0;
	pdata = pdata; 	   
	OSStatInit();		//��ʼ��ͳ������.�������ʱ1��������	
// 	app_srand(OSTime);
	
	OS_ENTER_CRITICAL();//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);						   
 	OSTaskCreate(usart_task,(void *)0,(OS_STK*)&USART_TASK_STK[USART_STK_SIZE-1],USART_TASK_PRIO);						   
	OSTaskCreate(watch_task,(void *)0,(OS_STK*)&WATCH_TASK_STK[WATCH_STK_SIZE-1],WATCH_TASK_PRIO); 					   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();	//�˳��ٽ���(���Ա��жϴ��)
} 
	
//������
void main_task(void *pdata)
{
//	while(1) {
//		u3_printf("Hello Uart3 TTL\n");
//		sim900a_send_cmd("AT","OK",100);
//		delay_ms(1000);
//	}
//	sim7500e_tcp_connect(0,NULL,NULL);
	delay_ms(5000);
	cpr74_read_calypso();
}

//ִ�����ҪʱЧ�ԵĴ���
void usart_task(void *pdata)
{	    
	while(1)
	{			  
		if (0x08 == sim7500dev.status&0x08) {
			LED0=!LED0;
		}
//		printf("USART3_RX_STA_BAK = %d\n", USART3_RX_STA_BAK);
		delay_ms(1000);	 
	}
}

//��������
void watch_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0; 
	
	while(1) {
		delay_ms(100);
	}
}

//Ӳ��������
void HardFault_Handler(void)
{
	u32 i;
	u8 t=0;
	u32 temp;
	temp=SCB->CFSR;					//fault״̬�Ĵ���(@0XE000ED28)����:MMSR,BFSR,UFSR
 	printf("CFSR:%8X\r\n",temp);	//��ʾ����ֵ
	temp=SCB->HFSR;					//Ӳ��fault״̬�Ĵ���
 	printf("HFSR:%8X\r\n",temp);	//��ʾ����ֵ
 	temp=SCB->DFSR;					//����fault״̬�Ĵ���
 	printf("DFSR:%8X\r\n",temp);	//��ʾ����ֵ
   	temp=SCB->AFSR;					//����fault״̬�Ĵ���
 	printf("AFSR:%8X\r\n",temp);	//��ʾ����ֵ
 	LED1=!LED1;
 	while(t<5)
	{
		t++;
		LED0=!LED0;
		//BEEP=!BEEP;
		for(i=0;i<0X1FFFFF;i++);
 	}
}
