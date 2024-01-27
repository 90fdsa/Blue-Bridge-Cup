#ifndef __UART_H__
#define __UART_H__
#include <STC15F2K60S2.H>


void UartInit(void);

void sendString(unsigned char *uart_T_sting);

#endif

