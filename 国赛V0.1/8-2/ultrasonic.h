#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#include "stc15f2k60s2.h"
#include "intrins.h"

void Delay13us();		//@12.000MHz
void send_wave();
unsigned char distance_massure();


#endif