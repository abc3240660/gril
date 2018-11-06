#include "sys.h"
#include "usart5.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h"
#include "ucos_ii.h"

__align(8) u8 USART5_TX_BUF[USART5_MAX_SEND_LEN]; 	//���ͻ���,���USART5_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 USART5_RX_BUF[USART5_MAX_RECV_LEN]; 				//���ջ���,���USART5_MAX_RECV_LEN���ֽ�.
u8 USART5_RX_BUF_BAK[USART5_MAX_RECV_LEN];

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART5_RX_STA=0;   	 
vu16 USART5_RX_STA_BAK=0;
void USART5_IRQHandler(void)
{
	u8 res;	    
	OSIntEnter();    
	if(USART_GetFlagStatus(USART5, USART_FLAG_RXNE) != RESET)//���յ�����
	{	 
		res=USART_ReceiveData(USART5); 			 
//		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
//		USART1->DR = (u8) res;
		if((USART5_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART5_RX_STA<USART5_MAX_RECV_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM6,0);       				//���������
				if(USART5_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
					TIM_Cmd(TIM6, ENABLE); 	    			//ʹ�ܶ�ʱ��7
				}
				USART5_RX_BUF[USART5_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART5_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		} else {
			LED1 = 0;
			LED2 = 0;
			
			if (USART5_RX_STA_BAK >= USART5_MAX_RECV_LEN) {
				USART5_RX_STA_BAK = 0;
			}
			
			USART5_RX_BUF_BAK[USART5_RX_STA_BAK++]=res;
		}
	}  											 
	OSIntExit();  											 
}   
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void usart5_init(u32 bound)
{  	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART5);  //��λ����3
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5,ENABLE);//ʹ��USART5ʱ��
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOB11��GPIOB10��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��GPIOB11����GPIOB10
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);   
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_USART5); //GPIOB11����ΪUSART5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_USART5); //GPIOB10����ΪUSART5	  
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART5, &USART_InitStructure); //��ʼ������3
	
	USART_ITConfig(USART5, USART_IT_RXNE, ENABLE);//�����ж�  
		
	USART_Cmd(USART5, ENABLE);                    //ʹ�ܴ��� 
	
 
	NVIC_InitStructure.NVIC_IRQChannel = USART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM6_Int_Init(100-1,8400-1);	//10ms�ж�һ��
	
  	TIM_Cmd(TIM6, DISABLE); //�رն�ʱ��7
	
	USART5_RX_STA=0;				//���� 
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART5_MAX_SEND_LEN�ֽ�
void u5_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART5_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
		while(USART_GetFlagStatus(USART5,USART_FLAG_TC)==RESET);//ѭ������,ֱ���������   
		USART_SendData(USART5,(uint8_t)USART5_TX_BUF[j]);   
	}
}



































