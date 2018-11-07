#include "sim900a.h" 
#include "delay.h"	
#include "led.h"     
#include "w25qxx.h"  
#include "malloc.h"
#include "string.h"
#include "usart3.h" 
#include "ff.h" 
#include "ucos_ii.h" 
#include "can1.h"

__sim7500dev sim7500dev;	//sim7500������

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

u8 sim7500e_get_cmd_count()
{
	u8 cnt = 0;
	while(1) {
		if (NULL == cmd_list[cnt]) {
			break;
		}
		cnt++;
	}

	return cnt;
}

u8 sim7500e_is_supported_cmd(u8 cnt, char* str)
{
	u8 i = 0;

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
u8* sim7500e_check_cmd(u8 *str)
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
u8 sim7500e_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;  
	USART3_RX_STA=0;
	sim7500dev.cmdon=1;//����ָ��ȴ�״̬
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
				if(sim7500e_check_cmd(ack))res=0;//�յ��ڴ��Ľ����
				else res=1;//�����ڴ��Ľ��
				break; 
			} 
		}
		if(waittime==0)res=2; 
	}
	return res;
}
//�������ʱ����,��sim7500e_send_cmd�ɶ�ʹ��/�����sim7500e_send_cmd�����.
void sim7500e_cmd_over(void)
{
	USART3_RX_STA=0;
	sim7500dev.cmdon=0;//�˳�ָ��ȴ�״̬
}

static void sim7500e_long_return_check(void)
{
	char* str = 0;
	USART3_RX_BUF_BAK[USART3_RX_STA_BAK&0X7FFF]=0;//��ӽ�����
	USART3_RX_STA_BAK = 0;
	printf("bak recved %s\n", USART3_RX_BUF_BAK);
	
	// +CIPOPEN: 0,0
	str = strstr((char*)USART3_RX_BUF_BAK, "+CIPOPEN");
	if (str) {
		if (('0' == *(str+10)) && ('0' == *(str+12))) {
			sim7500dev.status |= 0x08;
		} else {
			sim7500dev.status &= 0xF7;
		}
	}
}

void sim7500e_tcp_send(char* send)
{
	if(sim7500e_send_cmd("AT+CIPSEND=0,",">",200)==0)//��������
	{
		sim7500e_send_cmd((u8*)send,0,500);	//��������:0X00  
		delay_ms(20);						//�������ʱ
		sim7500e_send_cmd((u8*)0X1A,0,0);	//CTRL+Z,�������ݷ���,����һ�δ���	
	}else sim7500e_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 	
	sim7500e_cmd_over();	
}

// DEV ACK
void sim7500e_do_door_open(char* send)
{
	CAN1_OpenDoor();

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_DOOR_OPEN, door_state);

	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
}

// DEV ACK
void sim7500e_do_jump_lamp(char* send)
{
	CAN1_JumpLamp();

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_JUMP_LAMP);

	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
}

// DEV ACK
void sim7500e_do_ring_alarm(char* send)
{
	CAN1_RingAlarm();

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_ACK, CMD_RING_ALARM);

	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
}

// DEV Auto Send
u8 sim7500e_do_dev_register(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DEV_REGISTER, HW_VERSION, SW_VERSION, bat_vol);

	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
	
	return 0;
}

// DEV Auto Send
void sim7500e_do_heart_beat(char* send)
{
	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s,%s,%d,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_HEART_BEAT, dev_time, lock_state, rssi, bat_vol);
	
	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
}

// DEV Auto SEND
void sim7500e_do_door_close(char* send)
{
	CAN1_CloseDoor();

	memset(send, 0, LEN_MAX_SEND);
	sprintf(send, "%s,%s,%s,%s\n", PROTOCOL_HEAD, DEV_TAG, imei, CMD_DOOR_CLOSE);

	printf("SEND:%s\n", send);
	
	sim7500e_tcp_send(send);
}

void sim7500e_parse_msg(char* msg, char* send)
{
	int index = 0;
	int data_pos = 0;
	char delims[] = ",";
	char* split_str = NULL;

	enum CMD_TYPE cmd_type = UNKNOWN_CMD;

	int cmd_count = sim7500e_get_cmd_count();

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
				cmd_type = (enum CMD_TYPE)sim7500e_is_supported_cmd(cmd_count, split_str);

				if (cmd_type != UNKNOWN_CMD) {
					if (0 == data_pos) {
						data_pos = index;
					}
					
					if (DOOR_OPEN == cmd_type) {
						sim7500e_do_door_open(send);
					}
				} else {
					// TBD
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
				sim7500e_do_ring_alarm(send);
			} else if (JUMP_LAMP == cmd_type) {
				lamp_times = atoi(split_str);
				printf("lamp_times = %d\n", lamp_times);
				sim7500e_do_jump_lamp(send);
			}
		}
		split_str = strtok(NULL, delims);
		index++;
	};
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
const u8 *modetbl[2]={"TCP","UDP"};
void sim7500e_tcp_connect(u8 mode,u8* ipaddr,u8* port)
{ 
	u8 i = 0;
	u8 *p,*p1,*p2;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������
	u8 oldsta=0XFF;
	p=mymalloc(SRAMIN,100);		//����100�ֽ��ڴ�
	p1=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�
	USART3_RX_STA=0;
	
	for (i=0; i<5; i++) {
		if (0 == sim7500e_send_cmd("AT","OK",100))break;
		if (4 == i) return;
		delay_ms(50);
	}
	
#if 1
	if(sim7500e_send_cmd("ATE0","OK",200)) {
		if(sim7500e_send_cmd("ATE0","OK",200))return;//������
	}
#endif

	sim7500e_send_cmd("AT+NETCLOSE","OK",200);
	sim7500e_send_cmd("AT+NETCLOSE","OK",200);
	
	delay_ms(1000);
	
	if(sim7500e_send_cmd("AT+NETOPEN","OK",200)) {
		if(sim7500e_send_cmd("AT+NETOPEN","OK",200))return;
	}
	delay_ms(100);
	if(sim7500e_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500))return;//��������
	//sim7500e_cmd_over();
	
	delay_ms(500);
	
	sim7500e_long_return_check();
	
	delay_ms(100);
	if(sim7500e_send_cmd("AT+CIPSEND=0,5",">",200))return;
	delay_ms(100);
	delay_ms(100);
	if(sim7500e_send_cmd("Hello","OK",200))return;
	delay_ms(100);
	delay_ms(100);
	if(sim7500e_do_dev_register(send_buf))return;
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
				sim7500e_send_cmd("AT+CIPCLOSE=0","OK",500);	//�ر�����
				sim7500e_send_cmd("AT+NETCLOSE","OK",500);		//�ر��ƶ�����
				sim7500e_send_cmd("AT+NETOPEN","OK",200);
				sim7500e_send_cmd("AT+CIPOPEN=0,\"TCP\",\"122.4.233.119\",9001","OK",500);						//������������
				sim7500e_cmd_over();
				connectsta=0;	
 				hbeaterrcnt=0;
			}
		}
		if(connectsta==0&&(timex%200)==0)//���ӻ�û������ʱ��,ÿ2���ѯһ��CIPSTATUS.
		{
			//sim7500e_send_cmd("AT+CIPSTATUS","OK",500);	//��ѯ����״̬
			//if(strstr((const char*)USART3_RX_BUF,"CLOSED"))connectsta=2;
			//if(strstr((const char*)USART3_RX_BUF,"CONNECT OK"))connectsta=1;
			connectsta=1;
		}
		if(connectsta==1&&timex>=600)//����������ʱ��,ÿ6�뷢��һ������
		{
			timex=0;
			
			sim7500e_do_heart_beat(send_buf);
				
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
			
			// Received User Data
			p2 = (u8*)strstr((const char*)USART3_RX_BUF, "+IPD");
			if (p2) {
				u8 num_cnt = 0;
				
				while(1) {
					if ((*(p2+4+num_cnt) < '0') || (*(p2+4+num_cnt) > '9')) {
						break;
					}
					num_cnt++;
				}
				
				data_lenth = 0;
				for (i=0; i<num_cnt; i++) {
					data_lenth += *(p2+4+i) - '0';
				}
				
				memset(recv_buf, 0, LEN_MAX_RECV);
				memcpy(recv_buf, p2+4+2+lenth_cnt, LEN_MAX_RECV);
				
				USART3_RX_STA=0;// Let Interrupt Go On Saving DATA
				
				printf("RECVED MSG: %s\n", recv_buf);
				
				sim7500e_parse_msg(recv_buf, send_buf);
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
