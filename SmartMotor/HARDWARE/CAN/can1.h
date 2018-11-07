#ifndef __CAN1_H
#define __CAN1_H	 
#include "common.h"	 

//////////////////////////////////////////////////////////////////////////////////	 


	
//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    
										 							 				    
u8 CAN1_Mode_Init(u8 mode);//CAN��ʼ��
 
u8 CAN1_Send_Msg(u8* msg,u8 len);						//��������

u8 CAN1_Receive_Msg(u8 *buf);							//��������

u8 CAN1_StartEngine(void);
u8 CAN1_StopEngine(void);

u8 CAN1_OpenDoor(void);
u8 CAN1_CloseDoor(void);

u8 CAN1_JumpLamp(void);
u8 CAN1_RingAlarm(void);

#endif

















