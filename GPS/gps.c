#include "gps.h" 
#include "led.h" 
#include "delay.h" 								   
#include "usart3.h" 								   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"

u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1)
	{
		if(*p=='-'){mask|=0X02;p++;}
		if(*p==','||(*p=='*'))break;
		if(*p=='.'){mask|=0X01;p++;}
		else if(*p>'9'||(*p<'0'))
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;
	for(i=0;i<ilen;i++)
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;
	*dx=flen;
	for(i=0;i<flen;i++)
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}  							 

void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';
	posx=NMEA_Comma_Pos(p1,3);
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"$GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);
			else break;
			slx++;	   
		}   
 		p=p1+1;
	}   
}
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPGGA");
	posx=NMEA_Comma_Pos(p1,6);
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}

void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;   
	p1=(u8*)strstr((const char *)buf,"$GPGSA");
	posx=NMEA_Comma_Pos(p1,2);
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<12;i++)
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}

void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs;  
	p1=(u8*)strstr((const char *)buf,"GPRMC");
	posx=NMEA_Comma_Pos(p1,1);
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);
		rs=temp%NMEA_Pow(10,dx+2);
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
	}
	posx=NMEA_Comma_Pos(p1,4);
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);
 	posx=NMEA_Comma_Pos(p1,5);
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);
		rs=temp%NMEA_Pow(10,dx+2);
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
	}
	posx=NMEA_Comma_Pos(p1,6);
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);
	posx=NMEA_Comma_Pos(p1,9);
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
}

void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GPVTG");
	posx=NMEA_Comma_Pos(p1,7);
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);
	}
}  

void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	NMEA_GPGSV_Analysis(gpsx,buf);
	NMEA_GPGGA_Analysis(gpsx,buf);
	NMEA_GPGSA_Analysis(gpsx,buf);
	NMEA_GPRMC_Analysis(gpsx,buf);
	NMEA_GPVTG_Analysis(gpsx,buf);
}

void Ublox_CheckSum(u8 *buf,u16 len,u8* cka,u8*ckb)
{
	u16 i;
	*cka=0;*ckb=0;
	for(i=0;i<len;i++)
	{
		*cka=*cka+buf[i];
		*ckb=*ckb+*cka;
	}
}

u8 Ublox_Cfg_Ack_Check(void)
{			 
	u16 len=0,i;
	u8 rval=0;
	while((USART3_RX_STA&0X8000)==0 && len<100)
	{
		len++;
		delay_ms(5);
	}		 
	if(len<100)
	{
		len=USART3_RX_STA&0X7FFF;
		for(i=0;i<len;i++)if(USART3_RX_BUF[i]==0XB5)break;
		if(i==len)rval=2;
		else if(USART3_RX_BUF[i+3]==0X00)rval=3;
		else rval=0;
	}else rval=1;
    USART3_RX_STA=0;
	return rval;  
}

u8 Ublox_Cfg_Cfg_Save(void)
{
	u8 i;
	_ublox_cfg_cfg *cfg_cfg=(_ublox_cfg_cfg *)USART3_TX_BUF;
	cfg_cfg->header=0X62B5;		//cfg header
	cfg_cfg->id=0X0906;			//cfg cfg id
	cfg_cfg->dlength=13;
	cfg_cfg->clearmask=0;
	cfg_cfg->savemask=0XFFFF;
	cfg_cfg->loadmask=0;
	cfg_cfg->devicemask=4;
	Ublox_CheckSum((u8*)(&cfg_cfg->id),sizeof(_ublox_cfg_cfg)-4,&cfg_cfg->cka,&cfg_cfg->ckb);
	Ublox_Send_Date((u8*)cfg_cfg,sizeof(_ublox_cfg_cfg));
	for(i=0;i<6;i++)if(Ublox_Cfg_Ack_Check()==0)break;
	return i==6?1:0;
}

u8 Ublox_Cfg_Msg(u8 msgid,u8 uart1set)
{
	_ublox_cfg_msg *cfg_msg=(_ublox_cfg_msg *)USART3_TX_BUF;
	cfg_msg->header=0X62B5;		//cfg header
	cfg_msg->id=0X0106;			//cfg msg id
	cfg_msg->dlength=8;
	cfg_msg->msgclass=0XF0;
	cfg_msg->msgid=msgid;
	cfg_msg->iicset=1;
	cfg_msg->uart1set=uart1set;
	cfg_msg->uart2set=1;
	cfg_msg->usbset=1;
	cfg_msg->spiset=1;
	cfg_msg->ncset=1;
	Ublox_CheckSum((u8*)(&cfg_msg->id),sizeof(_ublox_cfg_msg)-4,&cfg_msg->cka,&cfg_msg->ckb);
	Ublox_Send_Date((u8*)cfg_msg,sizeof(_ublox_cfg_msg));
	return Ublox_Cfg_Ack_Check();
}

u8 Ublox_Cfg_Prt(u32 baudrate)
{
	_ublox_cfg_prt *cfg_prt=(_ublox_cfg_prt *)USART3_TX_BUF;
	cfg_prt->header=0X62B5;		//cfg header
	cfg_prt->id=0X0006;			//cfg prt id
	cfg_prt->dlength=20;
	cfg_prt->portid=1;
	cfg_prt->reserved=0;
	cfg_prt->txready=0;
	cfg_prt->mode=0X08D0;
	cfg_prt->baudrate=baudrate;
	cfg_prt->inprotomask=0X0007;
	cfg_prt->outprotomask=0X0007;
 	cfg_prt->reserved4=0;
 	cfg_prt->reserved5=0;
	Ublox_CheckSum((u8*)(&cfg_prt->id),sizeof(_ublox_cfg_prt)-4,&cfg_prt->cka,&cfg_prt->ckb);
	Ublox_Send_Date((u8*)cfg_prt,sizeof(_ublox_cfg_prt));
	delay_ms(200);
	usart3_init(36,baudrate);
	return Ublox_Cfg_Ack_Check();
} 

u8 Ublox_Cfg_Tp(u32 interval,u32 length,signed char status)
{
	_ublox_cfg_tp *cfg_tp=(_ublox_cfg_tp *)USART3_TX_BUF;
	cfg_tp->header=0X62B5;		//cfg header
	cfg_tp->id=0X0706;			//cfg tp id
	cfg_tp->dlength=20;
	cfg_tp->interval=interval;
	cfg_tp->length=length;
	cfg_tp->status=status;
	cfg_tp->timeref=0;
	cfg_tp->flags=0;
	cfg_tp->reserved=0;
	cfg_tp->antdelay=820;
	cfg_tp->rfdelay=0;
	cfg_tp->userdelay=0;
	Ublox_CheckSum((u8*)(&cfg_tp->id),sizeof(_ublox_cfg_tp)-4,&cfg_tp->cka,&cfg_tp->ckb);
	Ublox_Send_Date((u8*)cfg_tp,sizeof(_ublox_cfg_tp));
	return Ublox_Cfg_Ack_Check();
}

u8 Ublox_Cfg_Rate(u16 measrate,u8 reftime)
{
	_ublox_cfg_rate *cfg_rate=(_ublox_cfg_rate *)USART3_TX_BUF;
 	if(measrate<200)return 1;
 	cfg_rate->header=0X62B5;	//cfg header
	cfg_rate->id=0X0806;	 	//cfg rate id
	cfg_rate->dlength=6;
	cfg_rate->measrate=measrate;
	cfg_rate->navrate=1;
	cfg_rate->timeref=reftime;
	Ublox_CheckSum((u8*)(&cfg_rate->id),sizeof(_ublox_cfg_rate)-4,&cfg_rate->cka,&cfg_rate->ckb);
	Ublox_Send_Date((u8*)cfg_rate,sizeof(_ublox_cfg_rate));
	return Ublox_Cfg_Ack_Check();
}

void Ublox_Send_Date(u8* dbuf,u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR=dbuf[j];  
	}	
}







