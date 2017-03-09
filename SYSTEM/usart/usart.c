#include "sys.h"
#include "usart.h"
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{  
	USART_SendData(USART1,(unsigned char) ch);    
	//while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
   // USART1->DR = (u8) ch;
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记

void uart_init(u32 bound){
    //GPIO端口设置
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
   //USART 初始化设置
   
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
  
   //使能串口接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 
   
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}
//extern char RxBuffer[15];
//extern u8 RxCount;
//void    WriteUsartBuff(char ch)
//{
//     RxBuffer[RxCount]=ch;
//	 RxCount++;
//	if(RxCount>=15) 
//	{
//		RxCount=0;
//	}
//}
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//    {
//            WriteUsartBuff(USART_ReceiveData(USART1));//(USART1->DR);	//读取接收到的数据
//             USART_ClearFlag(USART1,USART_FLAG_RXNE);
//    } 
//} 
///*********清除串口1接收数据缓存*****************/
//void ClearUart1(void)  //
//{
//	unsigned char i;
//	for(i = 0; i <15;i++)
//	{
//		 RxBuffer[i] = 0;
//	}
//}
//int strcmp_str(char *str1,char *str2,int count)
//{
//	int i=0;
//	while(*str1 == *str2)
//	{
//		i++;
//		if(i==count)
//		{
//			return 0;
//		}
//		str1++;
//		str2++;
//	}
//	return -1;
//}
