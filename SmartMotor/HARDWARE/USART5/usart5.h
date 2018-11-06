#ifndef __USART5_H
#define __USART5_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����3��ʼ������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 	

#define USART5_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART5_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART5_RX_EN 			1					//0,������;1,����.

extern u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//���ջ���,���USART5_MAX_RECV_LEN�ֽ�
extern u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//���ͻ���,���USART5_MAX_SEND_LEN�ֽ�
extern vu16 USART5_RX_STA;   						//��������״̬

extern u8  USART5_RX_BUF_BAK[USART5_MAX_RECV_LEN];
extern vu16 USART5_RX_STA_BAK;

void usart5_init(u32 bound);
void u5_printf(char* fmt,...);
#endif	   
















