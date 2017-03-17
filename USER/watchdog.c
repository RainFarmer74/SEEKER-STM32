#include "watchdog.h"
/*****************************************************
函数：       void IWDG_Init(void)
用途：       独立看门狗初始化
 返回值： 无
 ****************************************************/
void IWDG_Init(void)
{  
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);    //写入0x5555，用于允许狗狗寄存器写入功能

    IWDG_SetPrescaler(IWDG_Prescaler_256);                            //狗狗时钟分频，40K/256=156HZ(6.4ms)

 //   IWDG_SetReload(625);                                                                        //喂狗时间5s/6.4ms=781(不能大于0xfff)
    IWDG_SetReload(781); 

    IWDG_ReloadCounter();                                                                    //喂狗

    IWDG_Enable();                                                                                       //使能狗狗
}
/*****************************************************
函数：       void IWDG_Feed(void)
用途：       味独立狗
 返回值： 无
 ****************************************************/
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();           
}
