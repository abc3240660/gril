#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 

#define WK_UP 		PAin(0)		//PA0
#define XFS_RDY 	PAin(7)		//PA7

#define KEY0_PRES 	1	//KEY0����
#define KEY1_PRES		2	//KEY1����

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��					    
#endif
