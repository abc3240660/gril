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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//SIM900A ��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/1
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1 20140810
//1,����__sim900dev�ṹ��
//2,�޸�sim900a_send_cmd����,������Ԥ�ڽ������
////////////////////////////////////////////////////////////////////////////////// 	
 

__sim900dev sim900dev;	//sim900������

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

//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,�յ���Ԥ�ڽ��
//       2,û�յ��κλظ�
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;  
	USART3_RX_STA=0;
	sim900dev.cmdon=1;//����ָ��ȴ�״̬
	if((u32)cmd<=0XFF)
	{   
		while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//�Ƿ���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack))res=0;//�յ��ڴ��Ľ����
				else res=1;//�����ڴ��Ľ��
				break; 
			} 
		}
		if(waittime==0)res=2; 
	}
	return res;
}
//�������ʱ����,��sim900a_send_cmd�ɶ�ʹ��/�����sim900a_send_cmd�����.
void sim900a_cmd_over(void)
{
	USART3_RX_STA=0;
	sim900dev.cmdon=0;//�˳�ָ��ȴ�״̬
}
//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//unicode gbk ת������
//src:�����ַ���
//dst:���(uni2gbkʱΪgbk����,gbk2uniʱ,Ϊunicode�ַ���)
//mode:0,unicode��gbkת��;
//     1,gbk��unicodeת��;
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
    OS_CPU_SR cpu_sr=0;
	u16 temp; 
	u8 buf[2];
	OS_ENTER_CRITICAL();//�����ٽ���(�޷����жϴ��)  
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//�Ǻ���
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//����,ռ2���ֽ�
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
	*dst=0;//��ӽ�����
	OS_EXIT_CRITICAL();	//�˳��ٽ���(���Ա��жϴ��)	
} 

//extern void sms_remind_msg(u8 mode);
//extern void phone_incall_task_creat(void);
//�绰����/��⵽���� ���
void sim900a_cmsgin_check(void)
{
	u8 *p1,*p2; 
	u8 num;
	if(sim900dev.cmdon==0&&sim900dev.mode==0)//��ָ��ȴ�״̬,.����/����ģʽ,�ż������
	{
		if(USART3_RX_STA&0X8000)//�յ�������
		{
			if(sim900a_check_cmd("+CLIP:"))//���յ�����?
			{
				p1=sim900a_check_cmd("+CLIP:");
				p1+=8;
				p2=(u8*)strstr((const char *)p1,"\"");
				p2[0]=0;//��ӽ����� 
				strcpy((char*)sim900dev.incallnum,(char*)p1);//��������
				sim900dev.mode=3;			//���������
//				phone_incall_task_creat();	//������������
			}
			if(sim900a_check_cmd("+CMGS:"))//���ŷ��ͳɹ�
			{
//				sms_remind_msg(1);//��ʾ������Ϣ�ɹ�
			}
			if(sim900a_check_cmd("+CMTI:"))//�յ�����Ϣ
			{
				if(sim900dev.newmsg<SIM900_MAX_NEWMSG)
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");   
					p2=(u8*)strstr((const char*)(p1+1),"\r\n");
					if((p2-p1)==2)num=p1[1]-'0';//1λ
					else if((p2-p1)==3)num=(p1[1]-'0')*10+p1[2]-'0';//2λ
					else if((p2-p1)==4)num=(p1[1]-'0')*100+(p1[2]-'0')*10+p1[2]-'0';//3λ 
					sim900dev.newmsgindex[sim900dev.newmsg]=num;
					sim900dev.newmsg++;
				}
//				sms_remind_msg(0);//��ʾ�յ�����Ϣ
			}			
			USART3_RX_STA=0;
			printf("rev:%s\r\n",USART3_RX_BUF);	
		}
	}
}
//sim900a״̬���
void sim900a_status_check(void)
{
	u8 *p1; 
	if(sim900dev.cmdon==0&&sim900dev.mode==0&&USART3_RX_STA==0)//��ָ��ȴ�״̬.����/����ģʽ/��û���յ��κ�����,�������ѯ
	{
		if(sim900a_send_cmd("AT+CSQ","OK",25)==0)//��ѯ�ź�����,˳����GSMģ��״̬
		{
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),":"); 
			p1+=2;
			sim900dev.csq=(p1[0]-'0')*10+p1[1]-'0';//�ź�����
			if(sim900dev.csq>30)sim900dev.csq=30;		
			sim900dev.status|=1<<7;	//��ѯGSMģ���Ƿ���λ?
		}else 
		{ 
			sim900dev.csq=0;	
			sim900dev.status=0;	//���²���
		} 
		if((sim900dev.status&0XC0)==0X80)//CPIN״̬,δ��ȡ?
		{ 
			sim900a_send_cmd("ATE0","OK",100);//������(����ر�,����������ݿ����쳣)
			if(sim900a_send_cmd("AT+CPIN?","OK",25)==0)sim900dev.status|=1<<6;//SIM����λ
			else sim900dev.status&=~(1<<6);//SIM������ 
		} 
		if((sim900dev.status&0XE0)==0XC0)//��Ӫ������,δ��ȡ?
		{ 
			if(sim900a_send_cmd("AT+COPS?","OK",25)==0)//��ѯ��Ӫ������
			{ 
				p1=(u8*)strstr((const char*)(USART3_RX_BUF),"MOBILE");//����MOBILE,�����ǲ����й��ƶ�?
				if(p1)sim900dev.status&=~(1<<4); //�й��ƶ� 
				else 
				{
					p1=(u8*)strstr((const char*)(USART3_RX_BUF),"UNICOM");//����UNICOM,�����ǲ����й���ͨ?
					if(p1)sim900dev.status|=1<<4;	//�й���ͨ 
				}
				if(p1)
				{
					sim900dev.status|=1<<5;	//�õ���Ӫ�������� 
					//phone����ͨ������
					sim900a_send_cmd("AT+CLIP=1","OK",100);	//����������ʾ 
					sim900a_send_cmd("AT+COLP=1","OK",100);	//���ñ��к�����ʾ
					//sms����ͨ������
					sim900a_send_cmd("AT+CMGF=1","OK",100);			//�����ı�ģʽ 
					sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",100);	//����TE�ַ���ΪUCS2 
					sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",100);	//���ö���Ϣ�ı�ģʽ���� 
				}
			}else sim900dev.status&=~(1<<5);	//δ�����Ӫ������
		}
		sim900a_cmd_over();//�������
	}
}

static void sim900a_long_return_check(void)
{
	char* str = 0;
	USART3_RX_BUF_BAK[USART3_RX_STA_BAK&0X7FFF]=0;//��ӽ�����
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
const u8 *modetbl[2]={"TCP","UDP"};//����ģʽ
//tcp/udp����
//����������,��ά������
//mode:0:TCP����;1,UDP����)
//ipaddr:ip��ַ
//port:�˿� 
#if 1
void sim900a_tcpudp_test(u8 mode,u8* ipaddr,u8* port)
{ 
	u8 i = 0;
	u8 *p,*p1,*p2,*p3;
	u8 key;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);		//����100�ֽ��ڴ�
	p1=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�
	USART3_RX_STA=0;
	
	for (i=0; i<5; i++) {
		if (0 == sim900a_send_cmd("AT","OK",100))break;
		if (4 == i) return;
		delay_ms(50);
	}
	
	if(sim900a_send_cmd("ATE0","OK",200)) {
		if(sim900a_send_cmd("ATE0","OK",200))return;//������
	}
	
	if(sim900a_send_cmd("AT+NETCLOSE","OK",200)) {
		if(sim900a_send_cmd("AT+NETCLOSE","OK",200))return;
	}
	
	delay_ms(1000);
	
	if(sim900a_send_cmd("AT+NETOPEN","OK",200)) {
		if(sim900a_send_cmd("AT+NETOPEN","OK",200))return;
	}
	delay_ms(100);
	if(sim900a_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500))return;//��������
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
			if(connectsta==2||hbeaterrcnt>8)//�����ж���,��������8������û����ȷ���ͳɹ�,����������
			{
				sim900a_send_cmd("AT+CIPCLOSE=0","OK",500);	//�ر�����
				sim900a_send_cmd("AT+NETCLOSE","OK",500);		//�ر��ƶ�����
				sim900a_send_cmd("AT+NETOPEN","OK",200);
				sim900a_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500);						//������������
				sim900a_cmd_over();
				connectsta=0;	
 				hbeaterrcnt=0;
			}
		}
		if(connectsta==0&&(timex%200)==0)//���ӻ�û������ʱ��,ÿ2���ѯһ��CIPSTATUS.
		{
			//sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//��ѯ����״̬
			//if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			//if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
			connectsta=1;
		}
		if(connectsta==1&&timex>=600)//����������ʱ��,ÿ6�뷢��һ������
		{
			timex=0;
			if(sim900a_send_cmd("AT+CIPSEND=0,",">",200)==0)//��������
			{
				sim900a_send_cmd("12345",0,500);	//��������:0X00  
				//sim900a_send_cmd((u8*)0X00,0,0);	//��������:0X00  
				delay_ms(20);						//�������ʱ
				sim900a_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,�������ݷ���,����һ�δ���	
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 	
			sim900a_cmd_over();
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//������Դ���
		} 
		delay_ms(10);
		if(USART3_RX_STA&0X8000)		//���յ�һ��������
		{
			u8 lenth_cnt = 0;
			u8 data_lenth = 0;
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;	//��ӽ����� 
			//printf("RECVED %s",USART3_RX_BUF);				//���͵�����  
			if(hbeaterrcnt)							//��Ҫ�������Ӧ��
			{
				if(strstr((const char*)USART3_RX_BUF,"OK"))hbeaterrcnt=0;//��������
			}
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPCLOSE");
			if (p2) {
				// Error Process TBD
			}
			
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPD");
			if(p2)//���յ�TCP/UDP����
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
	u8 connectsta=0;			//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);		//����100�ֽ��ڴ�
	p1=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�
	USART3_RX_STA=0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim900a_send_cmd(p,"OK",500))return;		//��������
	while(1)
	{ 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)//�˳�����		 
		{  
			sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//�ر�����
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//�ر��ƶ����� 
			break;												 
		}else if(key==KEY0_PRES&(hbeaterrcnt==0))				//��������(��������ʱ����)
		{
			if(sim900a_send_cmd("AT+CIPSEND",">",500)==0)		//��������
			{ 
 				printf("CIPSEND DATA:%s\r\n",p1);	 			//�������ݴ�ӡ������
				u3_printf("%s\r\n",p1);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0X1A,"SEND OK",1000)==0);//Show_Str(30+30,80,200,12,"���ݷ��ͳɹ�!",12,0);//��ȴ�10s
//				else Show_Str(30+30,80,200,12,"���ݷ���ʧ��!",12,0);
				delay_ms(1000); 
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 
			oldsta=0XFF;			
		}
		if((timex%20)==0)
		{
			LED0=!LED0;
			count++;	
			if(connectsta==2||hbeaterrcnt>8)//�����ж���,��������8������û����ȷ���ͳɹ�,����������
			{
				sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//�ر�����
				sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//�ر��ƶ����� 
				sim900a_send_cmd(p,"OK",500);						//������������
				connectsta=0;	
 				hbeaterrcnt=0;
			}
		}
		if(connectsta==0&&(timex%200)==0)//���ӻ�û������ʱ��,ÿ2���ѯһ��CIPSTATUS.
		{
			sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//��ѯ����״̬
			if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
		}
		if(connectsta==1&&timex>=600)//����������ʱ��,ÿ6�뷢��һ������
		{
			timex=0;
			if(sim900a_send_cmd("AT+CIPSEND",">",200)==0)//��������
			{
				sim900a_send_cmd((u8*)0X00,0,0);	//��������:0X00  
				delay_ms(20);						//�������ʱ
				sim900a_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,�������ݷ���,����һ�δ���	
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 		
				
			hbeaterrcnt++; 
			printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//������Դ���
		} 
		delay_ms(10);
		if(USART3_RX_STA&0X8000)		//���յ�һ��������
		{ 
			USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;	//��ӽ����� 
			printf("%s",USART3_RX_BUF);				//���͵�����  
			if(hbeaterrcnt)							//��Ҫ�������Ӧ��
			{
				if(strstr((const char*)USART3_RX_BUF,"SEND OK"))hbeaterrcnt=0;//��������
			}				
			p2=(u8*)strstr((const char*)USART3_RX_BUF,"+IPD");
			if(p2)//���յ�TCP/UDP����
			{
				p3=(u8*)strstr((const char*)p2,",");
				p2=(u8*)strstr((const char*)p2,":");
				p2[0]=0;//���������
//				sprintf((char*)p1,"�յ�%s�ֽ�,��������",p3+1);//���յ����ֽ���
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

