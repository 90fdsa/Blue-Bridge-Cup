#ifndef _INIT_H
#define _INIT_H

#include "stc15f2k60s2.h"
void close_buzz();
void close_led();
void Timer1Init(void);
void PCA_init();
void UartInit(void);		//4800bps@12.000MHz



#endif