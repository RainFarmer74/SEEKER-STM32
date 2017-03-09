#ifndef __PID_H
#define __PID_H	
#include "stm32f10x.h"


//extern u16 ferp;
//extern u16 duty2;


u16 pidcontrol(u16 ValueSet);
void PID_GPIO_Config(void);
void PID_init(void);
void Restart(void);

#endif
