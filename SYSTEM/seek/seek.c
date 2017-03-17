#include "seek.h"
#include "timer.h"
#include "delay.h"
extern u32 speed,angle,dir,scanflag;
//开机回归零点函数
void zeroback(void)
{
        GPIO_ResetBits(GPIOB,GPIO_Pin_12) ;    
       speed=10;
       angle=60;
        scanflag=1;
        Pulse_output(1000000/1600,160*20);    
    //    motor_state=1;    
       delay_ms(2000);
}

//外部端口初始化
void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                             //方向引脚
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_OD;   //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;                     //网口复位引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//零点中断配置
void EXIT_ZERO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE); 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;       
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_EXTILineConfig (GPIO_PortSourceGPIOB,GPIO_PinSource13);
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =   EXTI_Trigger_Falling;
     EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
   
}


