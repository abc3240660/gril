#include "sys.h"
#include "usart5.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h"
#include "ucos_ii.h"

__align(8) u8 USART5_TX_BUF[USART5_MAX_SEND_LEN]; 	//发送缓冲,最大USART5_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART5_RX_BUF[USART5_MAX_RECV_LEN]; 				//接收缓冲,最大USART5_MAX_RECV_LEN个字节.
u8 USART5_RX_BUF_BAK[USART5_MAX_RECV_LEN];

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART5_RX_STA=0;   	 
vu16 USART5_RX_STA_BAK=0;
void USART5_IRQHandler(void)
{
	u8 res;	    
	OSIntEnter();    
	if(USART_GetFlagStatus(USART5, USART_FLAG_RXNE) != RESET)//接收到数据
	{	 
		res=USART_ReceiveData(USART5); 			 
//		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//		USART1->DR = (u8) res;
		if((USART5_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART5_RX_STA<USART5_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM6,0);       				//计数器清空
				if(USART5_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM_Cmd(TIM6, ENABLE); 	    			//使能定时器7
				}
				USART5_RX_BUF[USART5_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART5_RX_STA|=1<<15;				//强制标记接收完成
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
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void usart5_init(u32 bound)
{  	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART5);  //复位串口3
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5,ENABLE);//使能USART5时钟
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOB11和GPIOB10初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);   
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_USART5); //GPIOB11复用为USART5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_USART5); //GPIOB10复用为USART5	  
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART5, &USART_InitStructure); //初始化串口3
	
	USART_ITConfig(USART5, USART_IT_RXNE, ENABLE);//开启中断  
		
	USART_Cmd(USART5, ENABLE);                    //使能串口 
	
 
	NVIC_InitStructure.NVIC_IRQChannel = USART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM6_Int_Init(100-1,8400-1);	//10ms中断一次
	
  	TIM_Cmd(TIM6, DISABLE); //关闭定时器7
	
	USART5_RX_STA=0;				//清零 
}

//串口3,printf 函数
//确保一次发送数据不超过USART5_MAX_SEND_LEN字节
void u5_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART5_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
		while(USART_GetFlagStatus(USART5,USART_FLAG_TC)==RESET);//循环发送,直到发送完毕   
		USART_SendData(USART5,(uint8_t)USART5_TX_BUF[j]);   
	}
}



































