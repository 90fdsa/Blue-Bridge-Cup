#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#include "stc15f2k60s2.h"
#include "init.h"
#include "intrins.h"

void Delay13us();		//@12.000MHz
void Send_Wave(); 
unsigned char sonic();

#endif