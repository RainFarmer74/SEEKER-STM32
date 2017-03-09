#include "sys.h"
#include "usart.h"
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{  
	USART_SendData(USART1,(unsigned char) ch);    
	//while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
   // USART1->DR = (u8) ch;
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
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
 
u8 USART_RX_BUF[64];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART_RX_STA=0;       //����״̬���

void uart_init(u32 bound){
    //GPIO�˿�����
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

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
   
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
  
   //ʹ�ܴ��ڽ����ж�
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 
   
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

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
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//    {
//            WriteUsartBuff(USART_ReceiveData(USART1));//(USART1->DR);	//��ȡ���յ�������
//             USART_ClearFlag(USART1,USART_FLAG_RXNE);
//    } 
//} 
///*********�������1�������ݻ���*****************/
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