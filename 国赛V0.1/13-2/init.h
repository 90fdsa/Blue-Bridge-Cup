#ifndef _INIT_H
#define _INIT_H

void close_buzz();
void close_led();
void Timer0Init_fre(void);			//1000΢��@12.000MHz
void PCA_init();
void Timer1Init(void);
//void Timer2Init(void);        //1����@12.000MHz

#endif