#include "led.h"
#include "lcd.h"
#include "key.h"
#include "can1.h"
#include "can2.h"
#include "usart1.h"

int main(void)
{ 
	u8 CAN1_mode=0;
	u8 CAN2_mode=0;
	
	u8 can1_sendbuf[8]="CAN1SEND";
	u8 can2_sendbuf[8]="CAN2SEND";
	
	u8 can1_RECbuf[8]={0};
	u8 can2_RECbuf[8]={0};
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LED_Init();
 	LCD_Init();
	KEY_Init();
	uart1_init(9600);
	
	CAN1_Mode_Init(CAN1_mode);
	CAN2_Mode_Init(CAN2_mode);
 	
 	BRUSH_COLOR=RED;
	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
	LCD_DisplayString(10,40,16,"Author:Clever");
	LCD_DisplayString(30,60,24,"13.CAN TEST");

	CAN1_Send_Msg(can1_sendbuf,8);
	delay_ms(10);
	CAN2_Receive_Msg(can2_RECbuf); 
	LCD_DisplayString(30,120,24,can2_RECbuf);
	uart1SendChars(can2_RECbuf,8);
  
	CAN2_Send_Msg(can2_sendbuf,8);
	delay_ms(10);	
	CAN1_Receive_Msg(can1_RECbuf); 
	LCD_DisplayString(30,180,24,can1_RECbuf);
	uart1SendChars(can1_RECbuf,8);
 									  
	while(1)
	{
		delay_ms(100);
		LED0=!LED0;
	} 
}
