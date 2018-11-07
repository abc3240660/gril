#ifndef __UART5_H
#define __UART5_H 
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

#define UART5_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define UART5_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define UART5_RX_EN 			1					//0,������;1,����.

extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//���ջ���,���UART5_MAX_RECV_LEN�ֽ�
extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//���ͻ���,���UART5_MAX_SEND_LEN�ֽ�
extern vu16 UART5_RX_STA;   						//��������״̬

extern u8  UART5_RX_BUF_BAK[UART5_MAX_RECV_LEN];
extern vu16 UART5_RX_STA_BAK;

void usart5_init(u32 bound);
void u5_printf(char* fmt,...);
#endif	   
















