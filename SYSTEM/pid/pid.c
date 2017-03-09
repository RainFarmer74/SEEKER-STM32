#include "pid.h"
#include "math.h"
#include "delay.h"

long ParameterVA,ParameterVB;
long DCVolEk[4];
unsigned int Control;
extern u16 period2;
u16 DCVolOutput=500;
//*******************************************************************************************************
//
void PID_GPIO_Config(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		    // 开漏输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStructure);

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_8 ;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	           //上拉输入	    
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStructure);

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_1 ;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    // 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void PID_init(void)
{
	 GPIO_SetBits(GPIOA,GPIO_Pin_0);
//	 GPIO_SetBits(GPIOA,GPIO_Pin_2);
	// GPIO_SetBits(GPIOA,GPIO_Pin_0);
	// GPIO_SetBits(GPIOA,GPIO_Pin_3);
    //GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}
void Restart(void)
{
     GPIO_ResetBits(GPIOA,GPIO_Pin_1);
     delay_ms(5);
    // GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
u16 pidcontrol(u16 ValueSet)
{ 
    ParameterVA=3;
   	ParameterVB=-1;
    DCVolEk[1]=DCVolEk[0];					  //上次偏差
    DCVolEk[0]=(ValueSet - 1000000/period2)/4;	//本次偏差

	DCVolOutput += DCVolEk[0]*ParameterVA;//+ DCVolEk[1]*ParameterVB;  //PID计算
	if(DCVolOutput > 1440)
	{
			 DCVolOutput = 1440;
	}

		if(DCVolOutput < 1000)
		{
				DCVolOutput = 1000;
		}
	
			
	

   return DCVolOutput; 		
}

