#include "sim900a.h" 
#include "delay.h"	
#include "led.h"     
#include "w25qxx.h"  
#include "malloc.h"
#include "string.h"
#include "usart3.h" 
#include "ff.h" 
#include "ucos_ii.h" 

//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SIM900A 初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/1
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140810
//1,新增__sim900dev结构体
//2,修改sim900a_send_cmd函数,新增非预期结果返回
////////////////////////////////////////////////////////////////////////////////// 	
 

__sim900dev sim900dev;	//sim900控制器

const char* cmd_list[] = {
	CMD_DEV_REGISTER,
	CMD_HEART_BEAT,
	CMD_INQUIRE_PARAM,
	CMD_RING_ALARM,
	CMD_DOOR_OPEN,
	CMD_DOOR_CLOSE,
	CMD_JUMP_LAMP,
	NULL
};

char send_buf[LEN_MAX_SEND] = "";
char recv_buf[LEN_MAX_RECV] = "";

char sync_sys_time[LEN_SYS_TIME+1] = "";

int door_state = 0;
int ring_times = 0;
int lamp_times = 0;
int lock_state = 0;
int hbeat_time = 0;
char bat_vol[LEN_BAT_VOL] = "88";// defaut is fake
char imei[LEN_IMEI_NO] = "88888888";// defaut is fake
char rssi[LEN_RSSI_VAL] = "88";// defaut is fake
char dev_time[LEN_SYS_TIME] = "20181105151955";// defaut is fake

int get_cmd_cnt()
{
	int cnt = 0;
	while(1) {
		if (NULL == cmd_list[cnt]) {
			break;
		}
		cnt++;
	}

	return cnt;
}

int is_supported_cmd(int cnt, char* str)
{
	int i = 0;

	for (i=0; i<cnt; i++) {
		if (0 == strncmp(str, cmd_list[i], strlen(cmd_list[i]))) {
			break;
		}
	}

	if (i != UNKNOWN_CMD) {
		printf("Recved CMD/ACK %s\n", str);
	}

	return i;
}

//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,收到非预期结果
//       2,没收到任何回复
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;  
	USART3_RX_STA=0;
	sim900dev.cmdon=1;//进入指令等待状态
	if((u32)cmd<=0XFF)
	{   
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//是否接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))res=0;//收到期待的结果了
				else res=1;//不是期待的结果
				break; 
			} 
		}
		if(waittime==0)res=2; 
	}
	return res;
}
//命令处理完时调用,与sim900a_send_cmd成对使用/多个与sim900a_send_cmd后调用.
void sim900a_cmd_over(void)
{
	USART3_RX_STA=0;
	sim900dev.cmdon=0;//退出指令等待状态
}
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//unicode gbk 转换函数
//src:输入字符串
//dst:输出(uni2gbk时为gbk内码,gbk2uni时,为unicode字符串)
//mode:0,unicode到gbk转换;
//     1,gbk到unicode转换;
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
    OS_CPU_SR cpu_sr=0;
	u16 temp; 
	u8 buf[2];
	OS_ENTER_CRITICAL();//进入临界区(无法被中断打断)  
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//非汉字
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//汉字,占2个字节
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
	*dst=0;//添加结束符
	OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断)	
} 

//extern void sms_remind_msg(u8 mode);
//extern void phone_incall_task_creat(void);
//电话呼入/检测到短信 检测
void sim900a_cmsgin_check(void)
{
	u8 *p1,*p2; 
	u8 num;
	if(sim900dev.cmdon==0&&sim900dev.mode==0)//非指令等待状态,.拨号/短信模式,才检测数据
	{
		if(USART3_RX_STA&0X8000)//收到数据了
		{
			if(sim900a_check_cmd("+CLIP:"))//接收到来电?
			{
				p1=sim900a_check_cmd("+CLIP:");
				p1+=8;
				p2=(u8*)strstr((const char *)p1,"\"");
				p2[0]=0;//添加结束符 
				strcpy((char*)sim900dev.incallnum,(char*)p1);//拷贝号码
				sim900dev.mode=3;			//标记来电了
//				phone_incall_task_creat();	//创建来电任务
			}
			if(sim900a_check_cmd("+CMGS:"))//短信发送成功
			{
//				sms_remind_msg(1);//提示发送信息成功
			}
			if(sim900a_check_cmd("+CMTI:"))//收到新消息
			{
				if(sim900dev.newmsg<SIM900_MAX_NEWMSG)
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");   
					p2=(u8*)strstr((const char*)(p1+1),"\r\n");
					if((p2-p1)==2)num=p1[1]-'0';//1位
					else if((p2-p1)==3)num=(p1[1]-'0')*10+p1[2]-'0';//2位
					else if((p2-p1)==4)num=(p1[1]-'0')*100+(p1[2]-'0')*10+p1[2]-'0';//3位 
					sim900dev.newmsgindex[sim900dev.newmsg]=num;
					sim900dev.newmsg++;
				}
//				sms_remind_msg(0);//提示收到新消息
			}			
			USART3_RX_STA=0;
			printf("rev:%s\r\n",USART3_RX_BUF);	
		}
	}
}
//sim900a状态监测
void sim900a_status_check(void)
{
	u8 *p1; 
	if(sim900dev.cmdon==0&&sim900dev.mode==0&&USART3_RX_STA==0)//非指令等待状态.拨号/短信模式/且没有收到任何数据,才允许查询
	{
		if(sim900a_send_cmd("AT+CSQ","OK",25)==0)//查询信号质量,顺便检测GSM模块状态
		{
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),":"); 
			p1+=2;
			sim900dev.csq=(p1[0]-'0')*10+p1[1]-'0';//信号质量
			if(sim900dev.csq>30)sim900dev.csq=30;		
			sim900dev.status|=1<<7;	//查询GSM模块是否在位?
		}else 
		{ 
			sim900dev.csq=0;	
			sim900dev.status=0;	//重新查找
		} 
		if((sim900dev.status&0XC0)==0X80)//CPIN状态,未获取?
		{ 
			sim900a_send_cmd("ATE0","OK",100);//不回显(必须关闭,否则接收数据可能异常)
			if(sim900a_send_cmd("AT+CPIN?","OK",25)==0)sim900dev.status|=1<<6;//SIM卡在位
			else sim900dev.status&=~(1<<6);//SIM卡错误 
		} 
		if((sim900dev.status&0XE0)==0XC0)//运营商名字,未获取?
		{ 
			if(sim900a_send_cmd("AT+COPS?","OK",25)==0)//查询运营商名字
			{ 
				p1=(u8*)strstr((const char*)(USART3_RX_BUF),"MOBILE");//查找MOBILE,看看是不是中国移动?
				if(p1)sim900dev.status&=~(1<<4); //中国移动 
				else 
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),"UNICOM");//查找UNICOM,看看是不是中国联通?
					if(p1)sim900dev.status|=1<<4;	//中国联通 
				}
				if(p1)
				{
					sim900dev.status|=1<<5;	//得到运营商名字了 
					//phone部分通用设置
					sim900a_send_cmd("AT+CLIP=1","OK",100);	//设置来电显示 
					sim900a_send_cmd("AT+COLP=1","OK",100);	//设置被叫号码显示
					//sms部分通用设置
					sim900a_send_cmd("AT+CMGF=1","OK",100);			//设置文本模式 
					sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",100);	//设置TE字符集为UCS2 
					sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",100);	//设置短消息文本模式参数 
				}
			}else sim900dev.status&=~(1<<5);	//未获得运营商名字
		}
		sim900a_cmd_over();//处理完毕
	}
}

static void sim900a_long_return_check(void)
{
	char* str = 0;
	USART3_RX_BUF_BAK[USART3_RX_STA_BAK&0X7FFF]=0;//添加结束符
	USART3_RX_STA_BAK = 0;
	printf("bak recved %s\n", USART3_RX_BUF_BAK);
	
	// +CIPOPEN: 0,0
	str = strstr(USART3_RX_BUF_BAK, "+CIPOPEN");
	if (str) {
		if (('0' == *(str+10)) && ('0' == *(str+12))) {
			sim900dev.status |= 0x08;
		} else {
			sim900dev.status &= 0xF7;
		}
	}
}

// DEV ACK
void do_door_open(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_DOOR_OPEN, door_state);

	printf("SEND:%s\n", send);
}

// DEV ACK
void do_jump_lamp(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_JUMP_LAMP);

	printf("SEND:%s\n", send);
}

// DEV ACK
void do_ring_alarm(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_RING_ALARM);

	printf("SEND:%s\n", send);
}

// DEV Auto Send
void do_dev_register(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_REGISTER, HW_VERSION, SW_VERSION, bat_vol);

	printf("SEND:%s\n", send);
}

// DEV Auto Send
void do_heart_beat(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_HEART_BEAT, dev_time, lock_state, rssi, bat_vol);

	printf("SEND:%s\n", send);
}

// DEV Auto SEND
void do_door_close(char* send)
{
	// FuncCall TBD

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DOOR_CLOSE);

	printf("SEND:%s\n", send);
}

void parse_msg(char* msg, char* send)
{
	int i = 0;
	int index = 0;
	int data_pos = 0;
	char delims[] = ",";
	char* split_str = NULL;

	enum CMD_TYPE cmd_type = UNKNOWN_CMD;

	int cmd_count = get_cmd_cnt();

#ifdef DEBUG_USE
	//printf("Support %d CMDs\n", cmd_count);
#endif

	split_str = strtok(msg, delims);
	while(split_str != NULL) {
#ifdef DEBUG_USE
		//printf("split_str = %s\n", split_str);
#endif
		// index = 3: SVR CMD
		// index = 4: SVR ACK
		if ((3 == index) || (4 == index)) {
			if (UNKNOWN_CMD == cmd_type) {
				cmd_type = is_supported_cmd(cmd_count, split_str);

				if (cmd_type != UNKNOWN_CMD) {
					if (0 == data_pos) {
						data_pos = index;
					}
				} else {
					if (DOOR_OPEN == cmd_type) {
						do_door_open(send);
					}
				}
			}
		}

		if (index > data_pos) {
			if (DEV_REGISTER == cmd_type) {
				if (5 == index) {
					strncpy(sync_sys_time, split_str, LEN_SYS_TIME);
					sync_sys_time[LEN_SYS_TIME] = '\0';
					printf("sync_sys_time = %s\n", sync_sys_time);
				} else if (6 == index) {
					hbeat_time = atoi(split_str);
					printf("hbeat_time = %d\n", hbeat_time);
				}
			} else if (HEART_BEAT == cmd_type) {
			} else if (INQUIRE_PARAM == cmd_type) {
			} else if (RING_ALARM == cmd_type) {
				ring_times = atoi(split_str);
				printf("ring_times = %d\n", ring_times);
				do_ring_alarm(send);
			} else if (JUMP_LAMP == cmd_type) {
				lamp_times = atoi(split_str);
				printf("lamp_times = %d\n", lamp_times);
				do_jump_lamp(send);
			}
		}
		split_str = strtok(NULL, delims);
		index++;
	};
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
const u8 *modetbl[2]={"TCP","UDP"};//连接模式
//tcp/udp测试
//带心跳功能,以维持连接
//mode:0:TCP测试;1,UDP测试)
//ipaddr:ip地址
//port:端口 
#if 1
void sim900a_tcpudp_test(u8 mode,u8* ipaddr,u8* port)
{ 
	u8 i = 0;
	u8 *p,*p1,*p2,*p3;
	u8 key;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,正在连接;1,连接成功;2,连接关闭; 
	u8 hbeaterrcnt=0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);		//申请100字节内存
	p1=mymalloc(SRAMIN,100);	//申请100字节内存
	USART3_RX_STA=0;
	
	for (i=0; i<5; i++) {
		if (0 == sim900a_send_cmd("AT","OK",100))break;
		if (4 == i) return;
		delay_ms(50);
	}
	
	if(sim900a_send_cmd("ATE0","OK",200)) {
		if(sim900a_send_cmd("ATE0","OK",200))return;//不回显
	}
	
	if(sim900a_send_cmd("AT+NETCLOSE","OK",200)) {
		if(sim900a_send_cmd("AT+NETCLOSE","OK",200))return;
	}
	
	delay_ms(1000);
	
	if(sim900a_send_cmd("AT+NETOPEN","OK",200)) {
		if(sim900a_send_cmd("AT+NETOPEN","OK",200))return;
	}
	delay_ms(100);
	if(sim900a_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500))return;//发起连接
	//sim900a_cmd_over();
	
	delay_ms(500);
	
	sim900a_long_return_check();
	
	delay_ms(100);
	if(sim900a_send_cmd("AT+CIPSEND=0,5",">",200))return;
	delay_ms(100);
	delay_ms(100);
	if(sim900a_send_cmd("Hello","OK",200))return;
	delay_ms(100);
	delay_ms(100);
	while(1)
	{ 
		if((timex%20)==0)
		{
			LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)//连接中断了,或者连续8次心跳没有正确发送成功,则重新连接
			{
				sim900a_send_cmd("AT+CIPCLOSE=0","OK",500);	//关闭连接
				sim900a_send_cmd("AT+NETCLOSE","OK",500);		//关闭移动场景
				sim900a_send_cmd("AT+NETOPEN","OK",200);
				sim900a_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500);						//尝试重新连接
				sim900a_cmd_over();
				connectsta=0;	
 				hbeaterrcnt=0;
			}
		}
		if(connectsta==0&&(timex%200)==0)//连接还没建立的时候,每2秒查询一次CIPSTATUS.
		{
			//sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
			//if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			//if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
			connectsta=1;
		}
		if(connectsta==1&&timex>=600)//连接正常的时候,每6秒发送一次心跳
		{
			timex=0;
			if(sim900a_send_cmd("AT+CIPSEND=0,",">",200)==0)//发送数据
			{
				sim900a_send_cmd("12345",0,500);	//发送数据:0X00  
				//sim900a_send_cmd((u8*)0X00,0,0);	//发送数据:0X00  
				delay_ms(20);						//必须加延时
				sim900a_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,结束数据发送,启动一次传输	
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 	
			sim900a_cmd_over();
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//方便调试代码
		} 
		delay_ms(10);
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
			u8 lenth_cnt = 0;
			u8 data_lenth = 0;
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;	//添加结束符 
			//printf("RECVED %s",USART3_RX_BUF);				//发送到串口  
			if(hbeaterrcnt)							//需要检测心跳应答
			{
				if(strstr((const char*)USART3_RX_BUF,"OK"))hbeaterrcnt=0;//心跳正常
			}
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPCLOSE");
			if (p2) {
				// Error Process TBD
			}
			
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPD");
			if(p2)//接收到TCP/UDP数据
			{
				if ((*(p2+4) >= 0) && (*(p2+4) <= 9)) {
					lenth_cnt++;
					data_lenth = *(p2+4) - '0';
				}
				
				if ((*(p2+5) >= 0) && (*(p2+5) <= 9)) {
					lenth_cnt++;
					data_lenth *= 10;
					data_lenth += (*(p2+5) - '0');
				}
				
				if ((*(p2+6) >= 0) && (*(p2+6) <= 9)) {
					lenth_cnt++;
					data_lenth *= 10;
					data_lenth += (*(p2+6) - '0');
				}
				
				memset(recv_buf, 0, LEN_MAX_RECV);
				memcpy(recv_buf, p2+4+2+lenth_cnt, LEN_MAX_RECV);
				printf("RECVED MSG: %s\n", recv_buf);
			}
			USART3_RX_STA=0;
		}
		if(oldsta!=connectsta)
		{
			oldsta=connectsta;
		} 
		timex++; 
	} 
	myfree(SRAMIN,p);
	myfree(SRAMIN,p1);
}
#else
void sim900a_tcpudp_test(u8 mode,u8* ipaddr,u8* port)
{ 
	u8 *p,*p1,*p2,*p3;
	u8 key;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,正在连接;1,连接成功;2,连接关闭; 
	u8 hbeaterrcnt=0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);		//申请100字节内存
	p1=mymalloc(SRAMIN,100);	//申请100字节内存
	USART3_RX_STA=0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim900a_send_cmd(p,"OK",500))return;		//发起连接
	while(1)
	{ 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)//退出测试		 
		{  
			sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景 
			break;												 
		}else if(key==KEY0_PRES&(hbeaterrcnt==0))				//发送数据(心跳正常时发送)
		{
			if(sim900a_send_cmd("AT+CIPSEND",">",500)==0)		//发送数据
			{ 
 				printf("CIPSEND DATA:%s\r\n",p1);	 			//发送数据打印到串口
				u3_printf("%s\r\n",p1);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0X1A,"SEND OK",1000)==0);//Show_Str(30+30,80,200,12,"数据发送成功!",12,0);//最长等待10s
//				else Show_Str(30+30,80,200,12,"数据发送失败!",12,0);
				delay_ms(1000); 
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 
			oldsta=0XFF;			
		}
		if((timex%20)==0)
		{
			LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)//连接中断了,或者连续8次心跳没有正确发送成功,则重新连接
			{
				sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
				sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景 
				sim900a_send_cmd(p,"OK",500);						//尝试重新连接
				connectsta=0;	
 				hbeaterrcnt=0;
			}
		}
		if(connectsta==0&&(timex%200)==0)//连接还没建立的时候,每2秒查询一次CIPSTATUS.
		{
			sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
			if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
		}
		if(connectsta==1&&timex>=600)//连接正常的时候,每6秒发送一次心跳
		{
			timex=0;
			if(sim900a_send_cmd("AT+CIPSEND",">",200)==0)//发送数据
			{
				sim900a_send_cmd((u8*)0X00,0,0);	//发送数据:0X00  
				delay_ms(20);						//必须加延时
				sim900a_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,结束数据发送,启动一次传输	
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 		
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//方便调试代码
		} 
		delay_ms(10);
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{ 
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;	//添加结束符 
			printf("%s",USART3_RX_BUF);				//发送到串口  
			if(hbeaterrcnt)							//需要检测心跳应答
			{
				if(strstr((const char*)USART3_RX_BUF,"SEND OK"))hbeaterrcnt=0;//心跳正常
			}				
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPD");
			if(p2)//接收到TCP/UDP数据
			{
				p3=(u8*)strstr((const char*)p2,",");
				p2=(u8*)strstr((const char*)p2,":");
				p2[0]=0;//加入结束符
//				sprintf((char*)p1,"收到%s字节,内容如下",p3+1);//接收到的字节数
			}
			USART3_RX_STA=0;
		}
		if(oldsta!=connectsta)
		{
			oldsta=connectsta;
		} 
		timex++; 
	} 
	myfree(SRAMIN,p);
	myfree(SRAMIN,p1);
}
#endif

