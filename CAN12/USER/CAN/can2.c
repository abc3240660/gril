#include "can2.h"
#include "led.h"



u8 CAN2_Mode_Init(u8 mode)
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN2_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1|RCC_APB1Periph_CAN2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//PA11,PA12
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2);
	  
   	CAN_InitStructure.CAN_TTCM=DISABLE;
  	CAN_InitStructure.CAN_ABOM=DISABLE;
  	CAN_InitStructure.CAN_AWUM=DISABLE;
  	CAN_InitStructure.CAN_NART=ENABLE;
  	CAN_InitStructure.CAN_RFLM=DISABLE;
  	CAN_InitStructure.CAN_TXFP=DISABLE;
		
  	CAN_InitStructure.CAN_Mode= mode;
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;
  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;
  	CAN_InitStructure.CAN_Prescaler=6;
  	CAN_Init(CAN2, &CAN_InitStructure);
    
 	CAN_FilterInitStructure.CAN_FilterNumber=14;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  	CAN_FilterInit(&CAN_FilterInitStructure);
		
#if CAN2_RX0_INT_ENABLE
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN2_RX0_INT_ENABLE
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	int i=0;
	CAN_Receive(CAN2, 0, &RxMessage);
	for(i=0;i<8;i++)
		printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

u8 CAN2_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;
	TxMessage.ExtId=0x12;
	TxMessage.IDE=0;
	TxMessage.RTR=0;
	TxMessage.DLC=len;
	for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;
	if(i>=0XFFF)return 1;
	return 0;		
}

u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
	if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)return 0;
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}
