#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"

u8 HC05_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	HC05_KEY=1;
	HC05_LED=1; 
	
	usart3_init(9600);
	
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("AT\r\n");
		HC05_KEY=0;
		for(t=0;t<10;t++)
		{
			if(USART3_RX_STA&0X8000)break;
			delay_ms(5);
		}		
		if(USART3_RX_STA&0X8000)
		{
			temp=USART3_RX_STA&0X7FFF;
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O'&&USART3_RX_BUF[1]=='K')
			{
				temp=0;
				break;
			}
		}			    		
	}		    
	if(retry==0)temp=1;
	return temp;	  
}

u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("AT+ROLE?\r\n");
		for(t=0;t<20;t++)
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		HC05_KEY=0;
		if(USART3_RX_STA&0X8000)
		{
			temp=USART3_RX_STA&0X7FFF;
			USART3_RX_STA=0;			 
			if(temp==13&&USART3_RX_BUF[0]=='+')
			{
				temp=USART3_RX_BUF[6]-'0';
				break;
			}
		}		
	}
	if(retry==0)temp=0XFF;
	return temp;
}

u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("%s\r\n",atstr);
		HC05_KEY=0;
		for(t=0;t<20;t++)
		{
			if(USART3_RX_STA&0X8000)break;
			delay_ms(5);
		}		
		if(USART3_RX_STA&0X8000)
		{
			temp=USART3_RX_STA&0X7FFF;
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O')
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;
	return temp;
}

void HC05_CFG_CMD(u8 *str)
{					  
	u8 temp;
	u8 t;		  
	HC05_KEY=1;
	delay_ms(10);
	u3_printf("%s\r\n",(char*)str);
	for(t=0;t<50;t++)
	{
		if(USART3_RX_STA&0X8000)break;
		delay_ms(10);
	}									    
	HC05_KEY=0;
	if(USART3_RX_STA&0X8000)
	{
		temp=USART3_RX_STA&0X7FFF;
		USART3_RX_STA=0;
		USART3_RX_BUF[temp]=0;
		printf("\r\n%s",USART3_RX_BUF);
	} 				 
}
