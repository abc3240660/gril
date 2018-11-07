#include "can1.h"
#include "led.h"



u8 CAN1_Mode_Init(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1);
	  
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
  	CAN_Init(CAN1, &CAN_InitStructure);
    
 	CAN_FilterInitStructure.CAN_FilterNumber=0;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  	CAN_FilterInit(&CAN_FilterInitStructure);
		
#if CAN1_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN1_RX0_INT_ENABLE
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	int i=0;
	CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
		printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

u8 CAN1_Send_Msg(u8* msg,u8 len)
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
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;
	if(i>=0XFFF)return 1;
	return 0;		
}
	
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}
