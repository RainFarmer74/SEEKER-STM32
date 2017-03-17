#ifndef __TIMER_H
#define	__TIMER_H

#include "stm32f10x.h"


void TIM4_PWM_Init(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);

void TIM2_config(u32 Cycle);
void TIM3_config(u32 PulseNum);
void Pulse_output(u32 Cycle,u32 PulseNum);
void Scan_back(u32 Cycle);
#endif /* __PWM_OUTPUT_H */

