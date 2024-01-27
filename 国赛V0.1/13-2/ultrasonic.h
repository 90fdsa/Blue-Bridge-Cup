#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#include "STC15F2K60S2.h"
#include "intrins.h"

void Delay13us();		//@12.000MHz
void Send_Wave(); 
unsigned char sonic();
//void sonic_qidong(unsigned char a);

#endif