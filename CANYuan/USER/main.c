#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "can.h"

int main(void)
{ 
	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
	LED_Init();
 	LCD_Init();
	KEY_Init();
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);//CAN500Kbps    
 	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"CAN TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/7");
	LCD_ShowString(30,130,200,16,16,"LoopBack Mode");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");
  	POINT_COLOR=BLUE;
	LCD_ShowString(30,170,200,16,16,"Count:");
	LCD_ShowString(30,190,200,16,16,"Send Data:");
	LCD_ShowString(30,250,200,16,16,"Receive Data:");
 									  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;
				if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);
 			}
			res=CAN1_Send_Msg(canbuf,8);
			if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");
			else LCD_ShowString(30+80,190,200,16,16,"OK    ");
		}else if(key==WKUP_PRES)
		{	   
			mode=!mode;
			CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,mode);
  			POINT_COLOR=RED;
			if(mode==0)
			{
				LCD_ShowString(30,130,200,16,16,"Nnormal Mode ");	    
			}else
			{
 				LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;
		}		 
		key=CAN1_Receive_Msg(canbuf);
		if(key)
		{			
			LCD_Fill(30,270,160,310,WHITE);
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0X80);
		}		   
	} 
	
}
