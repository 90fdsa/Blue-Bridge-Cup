#ifndef _DS1302_H
#define _DS1302_H

#include "STC15F2K60S2.h"
#include "intrins.h"

void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );
void set_time(unsigned char hour,unsigned char minute,unsigned char second);



#endif