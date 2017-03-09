#include "stm32f10x.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "seek.h"
#define  SuperPort_RST()  { GPIO_ResetBits(GPIOB,GPIO_Pin_14);delay_ms(500); GPIO_SetBits(GPIOB,GPIO_Pin_14);}  //���ڸ�λ
#define dir_shunshi  GPIO_SetBits(GPIOB,GPIO_Pin_12)      //���ת������  ˳ʱ��
#define dir_nishi        GPIO_ResetBits(GPIOB,GPIO_Pin_12)  //���ת������  ��ʱ��
char RxBuffer[15];
u8 RxCount;
u8 pre_cnt_rs;                //���ݱ�־
u8 tim3_count;               //��ʱ��������־
u8 Mk_UsartAll;              //���ڽ���һ��������ɱ�־
u32 speed;                           //����ٶ�
u32 angle;                          //����Ƕ�
//u8 motor_state;       //���״̬��־λ   
u8 zero_flag;                  //����־λ
u8 scanflag;                    //ɨ���־λ
//����ж���������
//en:1,������0���ر�
void ZERO_Set_Int(u8 en)
{
    EXTI->PR=1<<13;                         //���LINE13�ϵ��жϱ�־λ
    if(en)EXTI->IMR|=1<<13;         //������LINE13�ϵ��ж�
    else EXTI->IMR&=~(1<<13);  //����LINE13�ϵ��ж�
}
//���ڽ����ж�ʹ�ܺ͹رղ�������
void usart2_receivestart(u8 en)
{
    if(en) USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
    else  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    
}
//�������ݽ�������
void UsartRece_Data(void)
{
    if(!(strcmp_str(RxBuffer,"M+",2)) || !(strcmp_str(RxBuffer,"M-",2)))
    {
        USART2_Puts("M=GET\n");
        usart2_receivestart(0);
        speed = (( RxBuffer[2]-'0')*100 + ( RxBuffer[3]-'0')*10 + ( RxBuffer[4]-'0'))*16;
        angle = (( RxBuffer[5]-'0')*100 + ( RxBuffer[6]-'0')*10 + ( RxBuffer[7]-'0'))*160;
        if(RxBuffer[1]=='+')
        {
            dir_shunshi;
        }
        else dir_nishi;
        if(RxBuffer[8]=='B' && RxBuffer[11]=='K')    //ɨ��ָ��
        {
             scanflag=1;
             ZERO_Set_Int(0);                                                    //�ر�����ж�
             Pulse_output(1000000/speed,angle );
        }
        //ת̨����ָ��
        else     
        {
            scanflag=0;
            ZERO_Set_Int(0);                                                  //�ر�����ж�
             Pulse_output(1000000/speed,angle );
        }
    }
    //�ع����ָ��
    else if(!(strcmp_str(RxBuffer,"BACKZERO",8)))
    {
        zeroback();                                                               //�������
    }
    //����δʶ��ָ��
    else USART2_Puts("E=3\n");
}

int main(void)
{
  	/* ����ϵͳʱ��Ϊ72M */
	 SystemInit();
 	delay_init(72);	                	        //��ʱ��ʼ��
	NVIC_Configuration();         	//�ж�����
	uart_init(115200);		              //���ڳ�ʼ��
    uart2_init(115200);
    GPIO_init();                                   //�ⲿ�˿ڳ�ʼ��
    SuperPort_RST();                      //���ڸ�λ
    TIM4_Int_Init(50-1,7200-1);//5ms��ʱ
    EXIT_ZERO_Config();             //����ж�����
     ZERO_Set_Int(0);                   //����LINE13�ϵ��ж�
    zero_flag=0;
    scanflag=0;
    Mk_UsartAll=0;
   // zeroback();                                     //�ع����
     Scan_back(1000000/(10*160));
	while (1)
	{
		 if(Mk_UsartAll==1 )
		{
            UsartRece_Data();     //����2�������ݴ���		   
            ClearUart2();                 //�建��
            Mk_UsartAll=0;
            RxCount=0;
		}           
        if(zero_flag==1)
        {
           zero_flag=0;
            dir_shunshi;
            ZERO_Set_Int(1);              //��LINE13�ϵ��ж�
            delay_ms(3000);
            Scan_back(1000000/(10*160));//10��ÿ������
       }
	}										   
}
////�ⲿ����ж�
//void EXTI15_10_IRQHandler(void)
//{
//    if(EXTI_GetITStatus (EXTI_Line13) != RESET)
//    {
//            TIM_Cmd(TIM2, DISABLE);       
//            ZERO_Set_Int(0);                              //�ر�����ж�
//            usart2_receivestart(1);                   //���ڽ���ʹ��
//            EXTI_ClearITPendingBit( EXTI_Line13);
//            USART2_Puts("M=OK\n");
//    }
//   
//}
//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{	
		TIM_Cmd(TIM2, DISABLE);
		TIM_Cmd(TIM3, DISABLE);
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
        TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
        if(scanflag==1)
        {
            zero_flag=1;
        }
        else
        {
            usart2_receivestart(1);
            USART2_Puts("M=OK\n");
        }	
	}
}
//��ʱ��3�жϷ�����
//���ڽ��ճ�ʱ����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		if(RxCount!=0) 														 //�����ݽ���
		{
			if(RxCount == pre_cnt_rs)    					//�������
			{
				tim3_count++;
				if(tim3_count>=4)
				{
					  Mk_UsartAll=1;             			//��������������
					  tim3_count=0;
				}
			}
			else
			{
				tim3_count = 0;
				pre_cnt_rs = RxCount;
			}
		}
          TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ
    }
}

