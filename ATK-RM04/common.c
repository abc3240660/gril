#include "common.h"

const u8* portnum="8086";		 

const u8* wifista_ssid="ALIENTEK";
const u8* wifista_encryption="wpawpa2_aes";
const u8* wifista_password="15902020353";

const u8* wifiap_ssid="ATK-RM04";
const u8* wifiap_encryption="wpawpa2_aes";
const u8* wifiap_password="12345678";

const u8 *ATK_RM04_NETMODE_TBL[4]={" ROUTER ","ETH-COM ","WIFI-STA","WIFI-AP "};
const u8 *ATK_RM04_WORKMODE_TBL[4]={"TCP服务器","TCP客户端","UDP服务器","UDP客户端"};

void atk_rm04_init(void)
{
#if AT_MODE_CONFIG==0
	RCC->APB2ENR|=1<<2;
	GPIOA->CRL&=0XFFF0FFFF;	 
	GPIOA->CRL|=3<<4*4;
	ES_CTRL=1;
#endif
 	usart3_init(36,115200);
} 
void atk_rm04_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;
		printf("%s",USART3_RX_BUF);
		if(mode)USART3_RX_STA=0;
	} 
}
u8* atk_rm04_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r",cmd);
	if(ack&&waittime)
	{
		while(--waittime)
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)
			{
				if(atk_rm04_check_cmd(ack))break;
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
u8 atk_rm04_quit_trans(void)
{
#if AT_MODE_CONFIG==1
 	delay_ms(15);
	while((USART3->SR&0X40)==0);
	USART3->DR='+';      
	delay_ms(15);
	while((USART3->SR&0X40)==0);
	USART3->DR='+';      
	delay_ms(15);
	while((USART3->SR&0X40)==0);
	USART3->DR='+';      
	delay_ms(500);
	while((USART3->SR&0X40)==0);
	USART3->DR=0X1B;      
	delay_ms(15);
	while((USART3->SR&0X40)==0);
	USART3->DR=0X1B;      
	delay_ms(15);
	while((USART3->SR&0X40)==0);
	USART3->DR=0X1B;      
	delay_ms(15);
	return atk_rm04_send_cmd("at","at",20);
#else
	ES_CTRL=0;
	delay_ms(120);
	ES_CTRL=1;
	return 0;
#endif
}
u8 atk_rm04_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_rm04_quit_trans())return 0;
	atk_rm04_send_cmd("at+S2N_Stat=?","?",50);
	p=atk_rm04_check_cmd("\r\n");
	res=*(p+2);
	atk_rm04_send_cmd("at+out_trans=0","ok",50);
	return res;
}
const u8* kbd_tbl[13]={"1","2","3","4","5","6","7","8","9",".","0","#","DEL"}; 
u8* kbd_fn_tbl[2];
void atk_rm04_load_keyboard(u16 x,u16 y)
{
	u16 i;
	POINT_COLOR=RED; 
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
void atk_rm04_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{		  
	u16 i=keyx/3,j=keyx%3;
	if(keyx>15)return;
	if(sta)LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,GREEN);
	else LCD_Fill(x+j*80+1,y+i*28+1,x+j*80+78,y+i*28+26,WHITE); 
	if(j&&(i>3))Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_fn_tbl[keyx-13],16,80);
	else Show_Str_Mid(x+j*80,y+6+28*i,(u8*)kbd_tbl[keyx],16,80);		 		 
}
u8 atk_rm04_get_keynum(u16 x,u16 y)
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
					atk_rm04_key_staset(x,y,key_x-1,0);
					key_x=key;
					atk_rm04_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}  
	}else if(key_x) 
	{
		atk_rm04_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}
void atk_rm04_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_rm04_send_cmd("at+net_wanip=?","?",50))
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+2);
}
u8 atk_rm04_get_wifista_state(void)
{
	u8 *p; 
	atk_rm04_send_cmd("at+wifi_ConState=?","?",20);
	p=atk_rm04_check_cmd("\r\n"); 
	return strstr((const char*)p,"Connected")?1:0;
}
void atk_rm04_msg_show(u16 x,u16 y,u8 wanip)
{
	u8 *p,*p1;
	POINT_COLOR=BLUE;
	if(wanip==0)
	{
		atk_rm04_send_cmd("at+ver=?","?",20);
		p=atk_rm04_check_cmd("\r\n");
		Show_Str(x,y,240,16,"版本:",16,0);Show_Str(x+40,y,240,16,p+2,16,0);
		atk_rm04_send_cmd("at+netmode=?","?",20);
		p=atk_rm04_check_cmd("\r\n");
		Show_Str(x,y+16,240,16,"网络模式:",16,0);Show_Str(x+72,y+16,240,16,(u8*)ATK_RM04_NETMODE_TBL[*(p+2)-'0'],16,0);
		atk_rm04_send_cmd("at+wifi_conf=?","?",20);
		p=atk_rm04_check_cmd("\r\n");
		p1=(u8*)strstr((const char*)p,",");
		*p1=0;p1++;
		Show_Str(x,y+32,240,16,"SSID号:",16,0);Show_Str(x+56,y+32,240,16,p+2,16,0);
		p=p1;
		p1=(u8*)strstr((const char*)p,",");
		*p1=0;p1++;
		Show_Str(x,y+48,240,16,"加密方式:",16,0);Show_Str(x+72,y+48,240,16,p,16,0);
		Show_Str(x,y+64,240,16,"密码:",16,0);Show_Str(x+40,y+64,240,16,p1,16,0);
	}
	atk_rm04_send_cmd("at+net_wanip=?","?",20);
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0; 
	Show_Str(x,y+80,240,16,"WAN IP:",16,0);Show_Str(x+56,y+80,240,16,p+2,16,0);
	POINT_COLOR=RED;
}
void atk_rm04_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password)
{ 
	POINT_COLOR=RED;
	Show_Str(x,y,240,12,rmd,12,0);
	Show_Str(x,y+20,240,12,"SSID:",12,0);
	Show_Str(x,y+36,240,12,"加密方式:",12,0);
	Show_Str(x,y+52,240,12,"密码:",12,0); 
	POINT_COLOR=BLUE;
	Show_Str(x+30,y+20,240,12,ssid,12,0);
	Show_Str(x+54,y+36,240,12,encryption,12,0);
	Show_Str(x+30,y+52,240,12,password,12,0); 	  
}
u8 atk_rm04_netpro_sel(u16 x,u16 y,u8* name) 
{
	u8 key,t=0,*p;
	u8 netpro=0;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	p=mymalloc(SRAMIN,50);
	sprintf((char*)p,"%s 工作模式选择",name);
	Show_Str_Mid(0,y,p,16,240); 				    	 
	Show_Str(x,y+30,200,16,"KEY0:下一个",16,0); 				    	 
	Show_Str(x,y+50,200,16,"KEY1:上一个",16,0);				    	 
	Show_Str(x,y+70,200,16,"KEY_UP:确定",16,0); 
	Show_Str(x,y+100,200,16,"请选择:",16,0); 
	POINT_COLOR=BLUE;
 	Show_Str(x+16,y+120,200,16,"TCP服务器",16,0); 				    	 
	Show_Str(x+16,y+140,200,16,"TCP客户端",16,0);				    	 
	Show_Str(x+16,y+160,200,16,"UDP服务器",16,0); 
	Show_Str(x+16,y+180,200,16,"UDP客户端",16,0); 
	POINT_COLOR=RED;
	Show_Str(x,y+120,200,16,"→",16,0); 
	while(1)
	{
		key=KEY_Scan(0); 
		if(key)
		{
			if(key==4)break; 
			Show_Str(x,y+120+netpro*20,200,16,"  ",16,0);
			if(key==1)
			{
				if(netpro<3)netpro++;
				else netpro=0;
			}else if(key==2)
			{
				if(netpro>0)netpro--;
				else netpro=3; 
			}
			Show_Str(x,y+120+netpro*20,200,16,"→",16,0);
				
		} 
		delay_ms(10);
		atk_rm04_at_response(1);
		if((t++)==20){t=0;LED0=!LED0;}
	} 
	myfree(SRAMIN,p);
	return netpro;
} 
u8 atk_rm04_ip_set(u8* title,u8* mode,u8* port,u8* ip) 
{
	u8 res=0;
 	u8 key;
	u8 timex=0;  
	u8 iplen=0;
	LCD_Clear(WHITE);  
	POINT_COLOR=RED; 
	Show_Str_Mid(0,30,title,16,240);
	Show_Str(30,90,200,16,"工作模式:",16,0);
	Show_Str(30,110,200,16,"IP地址:",16,0);
	Show_Str(30,130,200,16,"端口:",16,0);
	kbd_fn_tbl[0]="连接";
	kbd_fn_tbl[1]="返回"; 
	atk_rm04_load_keyboard(0,180);
	POINT_COLOR=BLUE;
 	Show_Str(30+72,90,200,16,mode,16,0);
 	Show_Str(30+40,130,200,16,port,16,0);
	ip[0]=0; 		
	while(1)
	{ 
		key=atk_rm04_get_keynum(0,180);
		if(key)
		{   
			if(key<12)
			{
				if(iplen<15)
				{ 
					ip[iplen++]=kbd_tbl[key-1][0];
				}
			}else
			{
				if(key==13)if(iplen)iplen--;
				if(key==14&&iplen)break;
				if(key==15){res=1;break;}
			} 
			ip[iplen]=0; 
			LCD_Fill(30+56,110,239,110+16,WHITE);
			Show_Str(30+56,110,200,16,ip,16,0);
		} 
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		atk_rm04_at_response(1);
	} 
	return res;
}
void atk_rm04_mtest_ui(u16 x,u16 y)
{ 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,y,"ATK-RM04 WIFI模块测试",16,240); 
	Show_Str(x,y+25,200,16,"请选择:",16,0); 				    	 
	Show_Str(x,y+45,200,16,"KEY0:串口以太网",16,0); 				    	 
	Show_Str(x,y+65,200,16,"KEY1:WIFI STA",16,0);				    	 
	Show_Str(x,y+85,200,16,"KEY_UP:WIFI AP",16,0); 
 	atk_rm04_msg_show(x,y+125,0);
}
void atk_rm04_test(void)
{
	u8 key;
	u8 timex;
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-RM04 WIFI模块测试",16,240); 
	atk_rm04_quit_trans();
	while(atk_rm04_send_cmd("at","\r\n",20))
	{
		Show_Str(40,55,200,16,"未检测到模块!!!",16,0);
		delay_ms(800);
		LCD_Fill(40,55,200,55+16,WHITE);
		Show_Str(40,55,200,16,"尝试连接模块...",16,0);
		atk_rm04_quit_trans();
	} 	   
	atk_rm04_mtest_ui(32,30);
	while(1)
	{
		delay_ms(10); 
		atk_rm04_at_response(1);
		key=KEY_Scan(0); 
		if(key)
		{
			switch(key)
			{
				case KEY0_PRES://KEY0
					atk_rm04_cometh_test();
					break;
				case KEY1_PRES://KEY1
					atk_rm04_wifista_test();
					break;
				case WKUP_PRES://KEY_UP
					atk_rm04_wifiap_test();
					break;
			}
			atk_rm04_mtest_ui(32,30);
			timex=0;
		} 	
		if(timex==0)atk_rm04_msg_show(32,155,1);
		if((timex%20)==0)LED0=!LED0;
		timex++;	 
	} 
}
