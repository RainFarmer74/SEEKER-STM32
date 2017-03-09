#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"

void    WriteUsartBuff3(char ch);
void uart2_init(u32 bound);
void ClearUart2(void) ;
void UART2_send_byte(uint8_t byte) ;
void UART2_Send(uint8_t *Buffer, uint32_t Length);
void USART2_Puts(char * str);
int strcmp_str(char *str1,char *str2,int count);
#endif
