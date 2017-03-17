
#include "timer.h"
#include "usart.h"
//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  					//使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 									//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  								//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);					//初始化TIM3
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);									 //允许定时器3更新中断
	TIM_Cmd(TIM2,ENABLE); 																			//使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;							 //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//定时器4 主模式
//PWM输出
void TIM4_config(u32 Cycle)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                   //TIM2_CH2 PB7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = Cycle-1;                                                   //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =71;                                                     //设置用来作为TIMx时钟频率除数的预分频值  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                       //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                     //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                                                       //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;              //输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);                                                         //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);

    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);                             //CH2预装载使能	 
    TIM_ARRPreloadConfig(TIM4, ENABLE);                                                               //使能TIMx在ARR上的预装载寄存器
}
//定时器3 从模式
void TIM3_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// 

    TIM_TimeBaseStructure.TIM_Period = PulseNum-1;   //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
   TIM_TimeBaseStructure.TIM_Prescaler =0;    //设置用来作为TIMx时钟频率除数的预分频值  不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位  

    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);
    //TIM_InternalClockConfig(TIM3);
    TIM3->SMCR|=0x07;	                               //设定从模式控制寄存器	 
    //TIM_ARRPreloadConfig(TIM3, ENABLE);              //使能TIMx在ARR上的预装载寄存器
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;       //TIM3捕获中断   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}

void Pulse_output(u32 Cycle,u32 PulseNum)
{
	TIM3_config(PulseNum);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM4_config(Cycle);
	TIM_Cmd(TIM4, ENABLE);
}
void Scan_back(u32 Cycle)
{
    TIM4_config(Cycle);
	TIM_Cmd(TIM4 , ENABLE);
}
