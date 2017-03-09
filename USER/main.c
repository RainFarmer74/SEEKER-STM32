#include "stm32f10x.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "seek.h"
#define  SuperPort_RST()  { GPIO_ResetBits(GPIOB,GPIO_Pin_14);delay_ms(500); GPIO_SetBits(GPIOB,GPIO_Pin_14);}  //网口复位
#define dir_shunshi  GPIO_SetBits(GPIOB,GPIO_Pin_12)      //电机转动方向  顺时针
#define dir_nishi        GPIO_ResetBits(GPIOB,GPIO_Pin_12)  //电机转动方向  逆时针
char RxBuffer[15];
u8 RxCount;
u8 pre_cnt_rs;                //数据标志
u8 tim3_count;               //定时器计数标志
u8 Mk_UsartAll;              //串口接收一组数据完成标志
u32 speed;                           //电机速度
u32 angle;                          //电机角度
//u8 motor_state;       //电机状态标志位   
u8 zero_flag;                  //零点标志位
u8 scanflag;                    //扫描标志位
//零点中断屏蔽设置
//en:1,开启；0：关闭
void ZERO_Set_Int(u8 en)
{
    EXTI->PR=1<<13;                         //清除LINE13上的中断标志位
    if(en)EXTI->IMR|=1<<13;         //不屏蔽LINE13上的中断
    else EXTI->IMR&=~(1<<13);  //屏蔽LINE13上的中断
}
//串口接收中断使能和关闭操作函数
void usart2_receivestart(u8 en)
{
    if(en) USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
    else  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    
}
//串口数据解析函数
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
        if(RxBuffer[8]=='B' && RxBuffer[11]=='K')    //扫描指令
        {
             scanflag=1;
             ZERO_Set_Int(0);                                                    //关闭零点中断
             Pulse_output(1000000/speed,angle );
        }
        //转台调整指令
        else     
        {
            scanflag=0;
            ZERO_Set_Int(0);                                                  //关闭零点中断
             Pulse_output(1000000/speed,angle );
        }
    }
    //回归零点指令
    else if(!(strcmp_str(RxBuffer,"BACKZERO",8)))
    {
        zeroback();                                                               //返回零点
    }
    //其它未识别指令
    else USART2_Puts("E=3\n");
}

int main(void)
{
  	/* 配置系统时钟为72M */
	 SystemInit();
 	delay_init(72);	                	        //延时初始化
	NVIC_Configuration();         	//中断配置
	uart_init(115200);		              //串口初始化
    uart2_init(115200);
    GPIO_init();                                   //外部端口初始化
    SuperPort_RST();                      //网口复位
    TIM4_Int_Init(50-1,7200-1);//5ms计时
    EXIT_ZERO_Config();             //零点中断配置
     ZERO_Set_Int(0);                   //屏蔽LINE13上的中断
    zero_flag=0;
    scanflag=0;
    Mk_UsartAll=0;
   // zeroback();                                     //回归零点
     Scan_back(1000000/(10*160));
	while (1)
	{
		 if(Mk_UsartAll==1 )
		{
            UsartRece_Data();     //串口2接受数据处理		   
            ClearUart2();                 //清缓存
            Mk_UsartAll=0;
            RxCount=0;
		}           
        if(zero_flag==1)
        {
           zero_flag=0;
            dir_shunshi;
            ZERO_Set_Int(1);              //打开LINE13上的中断
            delay_ms(3000);
            Scan_back(1000000/(10*160));//10度每秒回零点
       }
	}										   
}
////外部零点中断
//void EXTI15_10_IRQHandler(void)
//{
//    if(EXTI_GetITStatus (EXTI_Line13) != RESET)
//    {
//            TIM_Cmd(TIM2, DISABLE);       
//            ZERO_Set_Int(0);                              //关闭零点中断
//            usart2_receivestart(1);                   //串口接收使能
//            EXTI_ClearITPendingBit( EXTI_Line13);
//            USART2_Puts("M=OK\n");
//    }
//   
//}
//定时器3中断服务函数
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
//定时器3中断服务函数
//串口接收超时处理
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
		if(RxCount!=0) 														 //有数据接收
		{
			if(RxCount == pre_cnt_rs)    					//接收完成
			{
				tim3_count++;
				if(tim3_count>=4)
				{
					  Mk_UsartAll=1;             			//交给主函数处理
					  tim3_count=0;
				}
			}
			else
			{
				tim3_count = 0;
				pre_cnt_rs = RxCount;
			}
		}
          TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
    }
}

