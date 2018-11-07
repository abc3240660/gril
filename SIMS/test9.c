#include "sim900a.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "lcd.h" 
#include "w25qxx.h" 	 
#include "touch.h" 	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"

void sim_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;
		printf("%s",USART3_RX_BUF);
		if(mode)USART3_RX_STA=0;
	} 
}

u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);
	if(ack&&waittime)
	{
		while(--waittime)
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)
			{
				if(sim900a_check_cmd(ack))break;
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}

u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}

void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
	u16 temp; 
	u8 buf[2];
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else
			{
				buf[1]=*src++;
				buf[0]=*src++; 
				temp=(u16)ff_convert((WCHAR)*(u16*)buf,1); 
			}
			*dst++=sim900a_hex2chr((temp>>12)&0X0F);
			*dst++=sim900a_hex2chr((temp>>8)&0X0F);
			*dst++=sim900a_hex2chr((temp>>4)&0X0F);
			*dst++=sim900a_hex2chr(temp&0X0F);
		}
	}else	//unicode 2 gbk
	{ 
		while(*src!=0)
		{
			buf[1]=sim900a_chr2hex(*src++)*16;
			buf[1]+=sim900a_chr2hex(*src++);
			buf[0]=sim900a_chr2hex(*src++)*16;
			buf[0]+=sim900a_chr2hex(*src++);
 			temp=(u16)ff_convert((WCHAR)*(u16*)buf,0);
			if(temp<0X80){*dst=temp;dst++;}
			else {*(u16*)dst=swap16(temp);dst+=2;}
		} 
	}
	*dst=0;
} 
const u8* kbd_tbl1[13]={"1","2","3","4","5","6","7","8","9","*","0","#","DEL"};
const u8* kbd_tbl2[13]={"1","2","3","4","5","6","7","8","9",".","0","#","DEL"};
u8** kbd_tbl;
u8* kbd_fn_tbl[2];
void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl)
{
	u16 i;
	POINT_COLOR=RED;
	kbd_tbl=kbtbl;
	LCD_Fill(x,y,x+240,y+140,WHITE);
	LCD_DrawRectangle(x,y,x+240,y+140);						   
	LCD_DrawRectangle(x+80,y,x+160,y+140);	 
	LCD_DrawRectangle(x,y+28,x+240,y+56);
	LCD_DrawRectangle(x,y+84,x+240,y+112);
	POINT_COLOR=BLUE;
	for(i=0;i<15;i++)
	{
		if(i<13)Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),(u8*)kbd_tbl[i],16,80);
		else Show_Str_Mid(x+(i%3)*80,y+6+28*(i/3),kbd_fn_tbl[i-13],16,80); 
	}  		 					   
}
void sim900a_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>15)return;
	if(sta)LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,GREEN);
	else LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,WHITE); 
	if(j&&(i>3))Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_fn_tbl[keyx-13],16,80);
	else Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_tbl[keyx],16,80);		 		 
}
u8 sim900a_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;
	u8 key=0;
	tp_dev.scan(0); 
	if(tp_dev.sta&TP_PRES_DOWN)
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*28+28)&&tp_dev.y[0]>(y+i*28))
				{	
					key=i*3+j+1;	 
					break;	 		   
				}
			}
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					sim900a_key_staset(x,y,key_x-1,0);
					key_x=key;
					sim900a_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}  
	}else if(key_x) 
	{
		sim900a_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}
u8 sim900a_call_test(void)
{
	u8 key;
	u16 lenx;
	u8 callbuf[20]; 
	u8 pohnenumlen=0;
	u8 *p,*p1,*p2;
	u8 oldmode=0;
	u8 cmode=0;

	LCD_Clear(WHITE);
	if(sim900a_send_cmd("AT+CLIP=1","OK",200))return 1;
	if(sim900a_send_cmd("AT+COLP=1","OK",200))return 2;
 	p1=mymalloc(SRAMIN,20);
	if(p1==NULL)return 2;	
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-SIM900A 拨号测试",16,240);				    	 
	Show_Str(40,70,200,16,"请拨号:",16,0); 
	kbd_fn_tbl[0]="拨号";
	kbd_fn_tbl[1]="返回"; 
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);
	POINT_COLOR=BLUE; 
	while(1)
	{
		delay_ms(10);
		if(USART3_RX_STA&0X8000)
		{
			sim_at_response(0);
			if(cmode==1||cmode==2)
			{
				if(cmode==1)if(sim900a_check_cmd("+COLP:"))cmode=2;
				if(sim900a_check_cmd("NO CARRIER"))cmode=0;
				if(sim900a_check_cmd("NO ANSWER"))cmode=0;
				if(sim900a_check_cmd("ERROR"))cmode=0;
			}
			if(sim900a_check_cmd("+CLIP:"))
			{
				cmode=3;
				p=sim900a_check_cmd("+CLIP:");
				p+=8;
				p2=(u8*)strstr((const char *)p,"\"");
				p2[0]=0;
				strcpy((char*)p1,(char*)p);
			}
			USART3_RX_STA=0;
		}
		key=sim900a_get_keynum(0,180);
		if(key)
		{ 
			if(key<13)
			{
				if(cmode==0&&pohnenumlen<15)
				{ 
					callbuf[pohnenumlen++]=kbd_tbl[key-1][0];
					u3_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]); 
				}else if(cmode==2)
				{ 
					u3_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]);
					delay_ms(100);
					u3_printf("AT+VTS=%c\r\n",kbd_tbl[key-1][0]); 
					LCD_ShowChar(40+56,90,kbd_tbl[key-1][0],16,0);
				}
			}else
			{
				if(key==13)if(pohnenumlen&&cmode==0)pohnenumlen--;
				if(key==14)
				{
					if(cmode==0)
					{
						callbuf[pohnenumlen]=0;
						u3_printf("ATD%s;\r\n",callbuf);
						cmode=1;
					}else 
					{
						sim900a_send_cmd("ATH","OK",200);
						cmode=0;
					}
				}
				if(key==15)
				{
					if(cmode==3)
					{
						sim900a_send_cmd("ATA","OK",200);
						Show_Str(40+56,70,200,16,callbuf,16,0);
						cmode=2;
					}else
					{
						sim900a_send_cmd("ATH",0,0);
						break;
					}
				}
			} 
			if(cmode==0)
			{
				callbuf[pohnenumlen]=0; 
				LCD_Fill(40+56,70,239,70+16,WHITE);
				Show_Str(40+56,70,200,16,callbuf,16,0);  	
			}				
		}
		if(oldmode!=cmode)
		{
			switch(cmode)
			{
				case 0: 
					kbd_fn_tbl[0]="拨号";
					kbd_fn_tbl[1]="返回"; 
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"请拨号:",16,0);  
					LCD_Fill(40+56,70,239,70+16,WHITE);
					if(pohnenumlen)
					{
						POINT_COLOR=BLUE;
						Show_Str(40+56,70,200,16,callbuf,16,0);
					}
					break;
				case 1:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"拨号中:",16,0); 
					pohnenumlen=0;
				case 2:
					POINT_COLOR=RED;
					if(cmode==2)Show_Str(40,70,200,16,"通话中:",16,0); 
					kbd_fn_tbl[0]="挂断";
					kbd_fn_tbl[1]="返回"; 	
					break;
				case 3:
					POINT_COLOR=RED;
					Show_Str(40,70,200,16,"有来电:",16,0); 
					POINT_COLOR=BLUE;
					Show_Str(40+56,70,200,16,p1,16,0); 
					kbd_fn_tbl[0]="挂断";
					kbd_fn_tbl[1]="接听"; 
					break;				
			}
			if(cmode==2)Show_Str(40,90,200,16,"DTMF音:",16,0);
			else LCD_Fill(40,90,120,90+16,WHITE);
			sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);
			oldmode=cmode; 
		}
		if((lenx%50)==0)LED0=!LED0; 	    				 
		lenx++;	 
	} 
	myfree(SRAMIN,p1);
	return 0;
}

void sim900a_sms_read_test(void)
{ 
	u8 *p,*p1,*p2;
	u8 timex=0;
	u8 msgindex[3];
	u8 msglen=0;
	u8 msgmaxnum=0;
	u8 key=0;
	u8 smsreadsta=0;
	p=mymalloc(SRAMIN,200);
	LCD_Clear(WHITE); 
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-SIM900A 读短信测试",16,240);				    	 
	Show_Str(30,50,200,16,"读取:     总信息:",16,0); 	
	kbd_fn_tbl[0]="读取";
	kbd_fn_tbl[1]="返回"; 
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);
	while(1)
	{
		key=sim900a_get_keynum(0,180);
		if(key)
		{  
			if(smsreadsta)
			{
				LCD_Fill(30,75,239,179,WHITE);
				smsreadsta=0;
			}
			if(key<10||key==11)
			{
				if(msglen<2)
				{ 
					msgindex[msglen++]=kbd_tbl[key-1][0];
					u3_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]); 
				} 
				if(msglen==2)
				{
					key=(msgindex[0]-'0')*10+msgindex[1]-'0';
					if(key>msgmaxnum)
					{
						msgindex[0]=msgmaxnum/10+'0';
						msgindex[1]=msgmaxnum%10+'0';					
					}
				} 
			}else
			{
				if(key==13)if(msglen)msglen--;
				if(key==14&&msglen)
				{ 
					LCD_Fill(30,75,239,179,WHITE);
					sprintf((char*)p,"AT+CMGR=%s",msgindex);
					if(sim900a_send_cmd(p,"+CMGR:",200)==0)
					{
						POINT_COLOR=RED;
						Show_Str(30,75,200,12,"状态:",12,0);
						Show_Str(30+75,75,200,12,"来自:",12,0);
						Show_Str(30,90,200,12,"接收时间:",12,0);
						Show_Str(30,105,200,12,"内容:",12,0);
						POINT_COLOR=BLUE;
						if(strstr((const char*)(USART3_RX_BUF),"UNREAD")==0)Show_Str(30+30,75,200,12,"已读",12,0);
						else Show_Str(30+30,75,200,12,"未读",12,0);
						p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
						p2=(u8*)strstr((const char*)(p1+2),"\"");
						p2[0]=0;
						sim900a_unigbk_exchange(p1+2,p,0);
						Show_Str(30+75+30,75,200,12,p,12,0);
						p1=(u8*)strstr((const char*)(p2+1),"/");
						p2=(u8*)strstr((const char*)(p1),"+");
						p2[0]=0;
						Show_Str(30+54,90,200,12,p1-2,12,0);
						p1=(u8*)strstr((const char*)(p2+1),"\r");
						sim900a_unigbk_exchange(p1+2,p,0);
						Show_Str(30+30,105,180,75,p,12,0);
						smsreadsta=1;
					}else
					{
						Show_Str(30,75,200,12,"无短信内容!!!请检查!!",12,0);
						delay_ms(1000);
						LCD_Fill(30,75,239,75+12,WHITE);
					}	  
					USART3_RX_STA=0;
				}
				if(key==15)break;
			} 
			msgindex[msglen]=0; 
			LCD_Fill(30+40,50,86,50+16,WHITE);
			Show_Str(30+40,50,86,16,msgindex,16,0);  	
		}
		if(timex==0)
		{
			if(sim900a_send_cmd("AT+CPMS?","+CPMS:",200)==0)
			{ 
				p1=(u8*)strstr((const char*)(USART3_RX_BUF),","); 
				p2=(u8*)strstr((const char*)(p1+1),",");
				p2[0]='/'; 
				if(p2[3]==',')
				{
					msgmaxnum=(p2[1]-'0')*10+p2[2]-'0';
					p2[3]=0;
				}else
				{
					msgmaxnum=(p2[1]-'0')*100+(p2[2]-'0')*10+p2[3]-'0';
					p2[4]=0;
				}
				sprintf((char*)p,"%s",p1+1);
				Show_Str(30+17*8,50,200,16,p,16,0);
				USART3_RX_STA=0;		
			}
		}	
		if((timex%20)==0)LED0=!LED0;
		timex++;
		delay_ms(10);
		if(USART3_RX_STA&0X8000)sim_at_response(1);
	}
	myfree(SRAMIN,p); 
}
const u8* sim900a_test_msg="您好，这是一条测试短信，由ATK-SIM900A GSM模块发送，模块购买地址:http://eboard.taobao.com，谢谢支持！";
void sim900a_sms_send_test(void)
{
	u8 *p,*p1,*p2;
	u8 phonebuf[20];
	u8 pohnenumlen=0;
	u8 timex=0;
	u8 key=0;
	u8 smssendsta=0;
	p=mymalloc(SRAMIN,100);
	p1=mymalloc(SRAMIN,300);
	p2=mymalloc(SRAMIN,100);
	LCD_Clear(WHITE);  
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-SIM900A 发短信测试",16,240);				    	 
	Show_Str(30,50,200,16,"发送给:",16,0); 	 
	Show_Str(30,70,200,16,"状态:",16,0);
	Show_Str(30,90,200,16,"内容:",16,0);  
	POINT_COLOR=BLUE;
	Show_Str(30+40,70,170,90,"等待发送",16,0);
	Show_Str(30+40,90,170,90,(u8*)sim900a_test_msg,16,0);
	kbd_fn_tbl[0]="发送";
	kbd_fn_tbl[1]="返回"; 
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl1);
	while(1)
	{
		key=sim900a_get_keynum(0,180);
		if(key)
		{   
			if(smssendsta)
			{
				smssendsta=0;
				Show_Str(30+40,70,170,90,"等待发送",16,0);
			}
			if(key<10||key==11)
			{
				if(pohnenumlen<15)
				{ 
					phonebuf[pohnenumlen++]=kbd_tbl[key-1][0];
					u3_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]); 
				}
			}else
			{
				if(key==13)if(pohnenumlen)pohnenumlen--;
				if(key==14&&pohnenumlen)
				{  
					Show_Str(30+40,70,170,90,"正在发送",16,0);
					smssendsta=1;		 
					sim900a_unigbk_exchange(phonebuf,p,1);
					sim900a_unigbk_exchange((u8*)sim900a_test_msg,p1,1);
					sprintf((char*)p2,"AT+CMGS=\"%s\"",p); 
					if(sim900a_send_cmd(p2,">",200)==0)
					{ 		 				 													 
						u3_printf("%s",p1);
 						if(sim900a_send_cmd((u8*)0X1A,"+CMGS:",1000)==0)smssendsta=2;
					}  
					if(smssendsta==1)Show_Str(30+40,70,170,90,"发送失败",16,0);
					else Show_Str(30+40,70,170,90,"发送成功",16,0);
					USART3_RX_STA=0;
				}
				if(key==15)break;
			} 
			phonebuf[pohnenumlen]=0; 
			LCD_Fill(30+54,50,239,50+16,WHITE);
			Show_Str(30+54,50,156,16,phonebuf,16,0);  	
		}
		if((timex%20)==0)LED0=!LED0;
		timex++;
		delay_ms(10);
		if(USART3_RX_STA&0X8000)sim_at_response(1);
	}
	myfree(SRAMIN,p);
	myfree(SRAMIN,p1);
	myfree(SRAMIN,p2); 
} 

void sim900a_sms_ui(u16 x,u16 y)
{ 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,y,"ATK-SIM900A 短信测试",16,240);  
	Show_Str(x,y+40,200,16,"请选择:",16,0); 				    	 
	Show_Str(x,y+60,200,16,"KEY0:读短信测试",16,0); 				    	 
	Show_Str(x,y+80,200,16,"KEY1:发短信测试",16,0);				    	 
	Show_Str(x,y+100,200,16,"KEY_UP:返回上级菜单",16,0);
}

u8 sim900a_sms_test(void)
{
	u8 key;
	u8 timex=0;
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 1;
	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",200))return 2;
	if(sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",200))return 3;
	sim900a_sms_ui(40,30);
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{ 
			sim900a_sms_read_test();
			sim900a_sms_ui(40,30);
			timex=0;
		}else if(key==KEY1_PRES)
		{ 
			sim900a_sms_send_test();
			sim900a_sms_ui(40,30);
			timex=0;			
		}else if(key==WKUP_PRES)break;
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);
	}
	sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200);
	return 0;
} 
const u8 *modetbl[2]={"TCP","UDP"};
void sim900a_tcpudp_test(u8 mode,u8* ipaddr,u8* port)
{ 
	u8 *p,*p1,*p2,*p3;
	u8 key;
	u16 timex=0;
	u8 count=0;
	const u8* cnttbl[3]={"正在连接","连接成功","连接关闭"};
	u8 connectsta=0;
	u8 hbeaterrcnt=0;
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);
	p1=mymalloc(SRAMIN,100);
	LCD_Clear(WHITE);  
	POINT_COLOR=RED; 
	if(mode)Show_Str_Mid(0,30,"ATK-SIM900A UDP连接测试",16,240);
	else Show_Str_Mid(0,30,"ATK-SIM900A TCP连接测试",16,240); 
	Show_Str(30,50,200,16,"KEY_UP:退出测试  KEY0:发送数据",12,0); 	
	sprintf((char*)p,"IP地址:%s 端口:%s",ipaddr,port);
	Show_Str(30,65,200,12,p,12,0);
	Show_Str(30,80,200,12,"状态:",12,0);
	Show_Str(30,100,200,12,"发送数据:",12,0);
	Show_Str(30,115,200,12,"接收数据:",12,0);
	POINT_COLOR=BLUE;
	USART3_RX_STA=0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim900a_send_cmd(p,"OK",500))return;
	while(1)
	{ 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{  
			sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);
			break;												 
		}else if(key==KEY0_PRES&(hbeaterrcnt==0))
		{
			Show_Str(30+30,80,200,12,"数据发送中...",12,0);
			if(sim900a_send_cmd("AT+CIPSEND",">",500)==0)
			{ 
 				printf("CIPSEND DATA:%s\r\n",p1);
				u3_printf("%s\r\n",p1);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0X1A,"SEND OK",1000)==0)Show_Str(30+30,80,200,12,"数据发送成功!",12,0);
				else Show_Str(30+30,80,200,12,"数据发送失败!",12,0);
				delay_ms(1000); 
			}else sim900a_send_cmd((u8*)0X1B,0,0);
			oldsta=0XFF;			
		}
		if((timex%20)==0)
		{
			LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)
			{
				sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);
				sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);
				sim900a_send_cmd(p,"OK",500);
				connectsta=0;	
 				hbeaterrcnt=0;
			}
			sprintf((char*)p1,"ATK-SIM900A %s测试 %d  ",modetbl[mode],count);
			Show_Str(30+54,100,200,12,p1,12,0); 
		}
		if(connectsta==0&&(timex%200)==0)
		{
			sim900a_send_cmd("AT+CIPSTATUS","OK",500);
			if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
		}
		if(connectsta==1&&timex>=600)
		{
			timex=0;
			if(sim900a_send_cmd("AT+CIPSEND",">",200)==0)
			{
				sim900a_send_cmd((u8*)0X00,0,0);
				delay_ms(20);
				sim900a_send_cmd((u8*)0X1A,0,0);
			}else sim900a_send_cmd((u8*)0X1B,0,0);
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);
		} 
		delay_ms(10);
		if(USART3_RX_STA&0X8000)
		{ 
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;
			printf("%s",USART3_RX_BUF);
			if(hbeaterrcnt)
			{
				if(strstr((const char*)USART3_RX_BUF,"SEND OK"))hbeaterrcnt=0;
			}				
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPD");
			if(p2)
			{
				p3=(u8*)strstr((const char*)p2,",");
				p2=(u8*)strstr((const char*)p2,":");
				p2[0]=0;
				sprintf((char*)p1,"收到%s字节,内容如下",p3+1);
				LCD_Fill(30+54,115,239,130,WHITE);
				POINT_COLOR=BRED;
				Show_Str(30+54,115,156,12,p1,12,0);
				POINT_COLOR=BLUE;
				LCD_Fill(30,130,210,319,WHITE);
				Show_Str(30,130,180,190,p2+1,12,0);
			}
			USART3_RX_STA=0;
		}
		if(oldsta!=connectsta)
		{
			oldsta=connectsta;
			LCD_Fill(30+30,80,239,80+12,WHITE);
			Show_Str(30+30,80,200,12,(u8*)cnttbl[connectsta],12,0);
		} 
		timex++; 
	} 
	myfree(SRAMIN,p);
	myfree(SRAMIN,p1);
}
void sim900a_gprs_ui(void)
{
	LCD_Clear(WHITE);  
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-SIM900A GPRS通信测试",16,240);	 
	Show_Str(30,50,200,16,"KEY_UP:连接方式切换",16,0); 	 	
	Show_Str(30,90,200,16,"连接方式:",16,0);
	Show_Str(30,110,200,16,"IP地址:",16,0);
	Show_Str(30,130,200,16,"端口:",16,0);
	kbd_fn_tbl[0]="连接";
	kbd_fn_tbl[1]="返回"; 
	sim900a_load_keyboard(0,180,(u8**)kbd_tbl2);
} 
u8 sim900a_gprs_test(void)
{
	const u8 *port="8086";
	u8 mode=0;
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16];
	u8 iplen=0;
	sim900a_gprs_ui();
	Show_Str(30+72,90,200,16,(u8*)modetbl[mode],16,0);
	Show_Str(30+40,130,200,16,(u8*)port,16,0);
 	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);
	sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",1000))return 1;
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 2;
	if(sim900a_send_cmd("AT+CGATT=1","OK",500))return 3;
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))return 4;
	if(sim900a_send_cmd("AT+CIPHEAD=1","OK",500))return 5;
	ipbuf[0]=0; 		
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)		 
		{  
			mode=!mode;
			Show_Str(30+72,90,200,16,(u8*)modetbl[mode],16,0);
		} 
		key=sim900a_get_keynum(0,180);
		if(key)
		{   
			if(key<12)
			{
				if(iplen<15)
				{ 
					ipbuf[iplen++]=kbd_tbl[key-1][0];
					u3_printf("AT+CLDTMF=2,\"%c\"\r\n",kbd_tbl[key-1][0]); 
				}
			}else
			{
				if(key==13)if(iplen)iplen--;
				if(key==14&&iplen)
				{    
					sim900a_tcpudp_test(mode,ipbuf,(u8*)port);
					sim900a_gprs_ui();
					Show_Str(30+72,90,200,16,(u8*)modetbl[mode],16,0);
					Show_Str(30+40,130,200,16,(u8*)port,16,0);
					USART3_RX_STA=0;
				}
				if(key==15)break;
			} 
			ipbuf[iplen]=0; 
			LCD_Fill(30+56,110,239,110+16,WHITE);
			Show_Str(30+56,110,200,16,ipbuf,16,0);
		} 
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);
	}
	return 0;
} 
void sim900a_mtest_ui(u16 x,u16 y)
{
	u8 *p,*p1,*p2; 
	p=mymalloc(SRAMIN,50);
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,y,"ATK-SIM900A 测试程序",16,240);  
	Show_Str(x,y+25,200,16,"请选择:",16,0); 				    	 
	Show_Str(x,y+45,200,16,"KEY0:拨号测试",16,0); 				    	 
	Show_Str(x,y+65,200,16,"KEY1:短信测试",16,0);				    	 
	Show_Str(x,y+85,200,16,"KEY_UP:GPRS测试",16,0);
	POINT_COLOR=BLUE; 	
	USART3_RX_STA=0;
	if(sim900a_send_cmd("AT+CGMI","OK",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");
		p1[0]=0;
		sprintf((char*)p,"制造商:%s",USART3_RX_BUF+2);
		Show_Str(x,y+110,200,16,p,16,0);
		USART3_RX_STA=0;		
	} 
	if(sim900a_send_cmd("AT+CGMM","OK",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n"); 
		p1[0]=0;
		sprintf((char*)p,"模块型号:%s",USART3_RX_BUF+2);
		Show_Str(x,y+130,200,16,p,16,0);
		USART3_RX_STA=0;		
	} 
	if(sim900a_send_cmd("AT+CGSN","OK",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");
		p1[0]=0;
		sprintf((char*)p,"序列号:%s",USART3_RX_BUF+2);
		Show_Str(x,y+150,200,16,p,16,0);
		USART3_RX_STA=0;		
	}
	if(sim900a_send_cmd("AT+CNUM","+CNUM",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+2),"\"");
		p2[0]=0;
		sprintf((char*)p,"本机号码:%s",p1+2);
		Show_Str(x,y+170,200,16,p,16,0);
		USART3_RX_STA=0;		
	}
	myfree(SRAMIN,p); 
}
u8 sim900a_gsminfo_show(u16 x,u16 y)
{
	u8 *p,*p1,*p2;
	u8 res=0;
	p=mymalloc(SRAMIN,50);
	POINT_COLOR=BLUE; 	
	USART3_RX_STA=0;
	if(sim900a_send_cmd("AT+CPIN?","OK",200))res|=1<<0;
	USART3_RX_STA=0;  
	if(sim900a_send_cmd("AT+COPS?","OK",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\""); 
		if(p1)
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;
			sprintf((char*)p,"运营商:%s",p1+1);
			Show_Str(x,y,200,16,p,16,0);
		} 
		USART3_RX_STA=0;		
	}else res|=1<<1;
	if(sim900a_send_cmd("AT+CSQ","+CSQ:",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;
		sprintf((char*)p,"信号质量:%s",p1+2);
		Show_Str(x,y+20,200,16,p,16,0);
		USART3_RX_STA=0;		
	}else res|=1<<2;
	if(sim900a_send_cmd("AT+CBC","+CBC:",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;
		sprintf((char*)p,"电池电量:%s%%  %smV",p1+1,p2+1);
		Show_Str(x,y+40,200,16,p,16,0);
		USART3_RX_STA=0;		
	}else res|=1<<3; 
	if(sim900a_send_cmd("AT+CCLK?","+CCLK:",200)==0)
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;
		sprintf((char*)p,"日期时间:%s",p1+1);
		Show_Str(x,y+60,200,16,p,16,0);
		USART3_RX_STA=0;		
	}else res|=1<<4; 
	myfree(SRAMIN,p); 
	return res;
} 
void sim900a_test(void)
{
	u8 key=0; 
	u8 timex=0;
	u8 sim_ready=0;
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-SIM900A 测试程序",16,240); 
	while(sim900a_send_cmd("AT","OK",100))
	{
		Show_Str(40,55,200,16,"未检测到模块!!!",16,0);
		delay_ms(800);
		LCD_Fill(40,55,200,55+16,WHITE);
		Show_Str(40,55,200,16,"尝试连接模块...",16,0);
		delay_ms(400);  
	} 	 
	LCD_Fill(40,55,200,55+16,WHITE);
	key+=sim900a_send_cmd("ATE0","OK",200);
	sim900a_mtest_ui(40,30);
	while(1)
	{
		delay_ms(10); 
		sim_at_response(1);
		if(sim_ready)
		{
			key=KEY_Scan(0); 
			if(key)
			{
				switch(key)
				{
					case KEY0_PRES:
						sim900a_call_test();
						break;
					case KEY1_PRES:
						sim900a_sms_test();
						break; 
					case WKUP_PRES:
						sim900a_gprs_test();
						break;
				}
				sim900a_mtest_ui(40,30);
				timex=0;
			} 			
		}
		if(timex==0)
		{
			if(sim900a_gsminfo_show(40,225)==0)sim_ready=1;
			else sim_ready=0;
		}	
		if((timex%20)==0)LED0=!LED0;
		timex++;	 
	} 	
}
