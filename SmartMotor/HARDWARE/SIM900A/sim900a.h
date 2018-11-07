#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
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
 
#define SIM900_MAX_NEWMSG	10		//���10������Ϣ

typedef struct 
{							  
 	u8 status;		//SIM900A״̬
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
	u8 newmsgindex[SIM900_MAX_NEWMSG];//�¶�����SIM���ڵ�����,���¼SIM900_MAX_NEWMSG���¶���
	u8 incallnum[20];//������뻺����,�20λ
}__sim900dev; 

extern __sim900dev sim900dev;	//sim900������


#define PROTOCOL_HEAD		"^MOBIT"
#define DEV_TAG			"ECAR"
#define SW_VERSION		"V1.0"
#define HW_VERSION		"V1.0"

#define CMD_DEV_ACK		"Re"// DEV ACK

#define CMD_DEV_REGISTER	"R0"// DEV CMD
#define CMD_HEART_BEAT		"H0"// DEV CMD
#define CMD_INQUIRE_PARAM	"C0"// DEV ACK
#define CMD_RING_ALARM		"R1"// DEV ACK
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

 

u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void sim900a_cmd_over(void);
u8 sim900a_chr2hex(u8 chr);
u8 sim900a_hex2chr(u8 hex);
void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void sim900a_cmsgin_check(void);
void sim900a_status_check(void);
#endif





