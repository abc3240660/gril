#ifndef __SIM7500E_H__
#define __SIM7500E_H__	 
#include "sys.h"

#define SIM7500E_MAX_NEWMSG	10		//���10������Ϣ

typedef struct 
{							  
 	u8 status;		//SIM7500EA״̬
					//bit7:0,û���ҵ�ģ��;1,�ҵ�ģ����
					//bit6:0,SIM��������;1,SIM������
					//bit5:0,δ�����Ӫ������;1,�ѻ����Ӫ������
					//bit4:0,�й��ƶ�;1,�й���ͨ
					//bit3:0,TCP Connect NG;1,TCP Connect OK
					//[2:0]:����
	
	u8 mode;		//��ǰ����ģʽ
					//0,��������ģʽ/����ģʽ
					//1,������
					//2,ͨ����
					//3,������Ӧ��
	
	vu8 cmdon;		//����Ƿ���ָ���ڷ��͵ȴ�״̬
					//0,û��ָ���ڵȴ���Ӧ
					//1,��ָ���ڵȴ���Ӧ
	
	u8 csq;			//�ź�����
	
	vu8 newmsg;		//����Ϣ����,0,û������Ϣ;����,����Ϣ����
	u8 newmsgindex[SIM7500E_MAX_NEWMSG];//�¶�����SIM���ڵ�����,���¼SIM7500E_MAX_NEWMSG���¶���
	u8 incallnum[20];//������뻺����,�20λ
}__sim7500dev; 

extern __sim7500dev sim7500dev;	//sim900������


#define PROTOCOL_HEAD	"^MOBIT"
#define DEV_TAG			"ECAR"
#define SW_VERSION		"V1.0"
#define HW_VERSION		"V1.0"

#define CMD_DEV_ACK		"Re"// DEV ACK

#define CMD_DEV_REGISTER	"R0"// DEV CMD
#define CMD_HEART_BEAT		"H0"// DEV CMD
#define CMD_INQUIRE_PARAM	"C0"// DEV ACK
#define CMD_RING_ALARM		"R2"// DEV ACK
#define CMD_DOOR_OPEN		"O0"// DEV ACK
#define CMD_DOOR_CLOSE		"C1"// DEV CMD
#define CMD_JUMP_LAMP		"S2"// DEV ACK

#define LEN_SYS_TIME	32
#define LEN_IMEI_NO	32
#define LEN_BAT_VOL	32
#define LEN_RSSI_VAL	32
#define LEN_MAX_SEND	32
#define LEN_MAX_RECV	32

#define DEBUG_USE 1

enum CMD_TYPE {
	DEV_REGISTER = 0,
	HEART_BEAT,
	INQUIRE_PARAM,
	RING_ALARM,
	DOOR_OPEN,
	DOOR_CLOSE,
	JUMP_LAMP,
	UNKNOWN_CMD
};
 
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//�ߵ��ֽڽ����궨��

u8* sim7500e_check_cmd(u8 *str);
u8 sim7500e_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void sim7500e_cmd_over(void);
u8 sim7500e_chr2hex(u8 chr);
u8 sim7500e_hex2chr(u8 hex);
void sim7500e_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim7500e_cmsgin_check(void);
void sim7500e_status_check(void);
void sim7500e_tcp_connect(u8 mode,u8* ipaddr,u8* port);

#endif/* __SIM7500E_H__ */





