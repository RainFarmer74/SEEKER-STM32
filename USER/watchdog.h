#ifndef __WATCHDOG_H
#define	__WATCHDOG_H

#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
void IWDG_Init(void);
void IWDG_Feed(void);

#endif
