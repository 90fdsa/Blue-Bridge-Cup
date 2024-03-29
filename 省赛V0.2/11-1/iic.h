#ifndef _IIC_H
#define _IIC_H

#include "STC15F2K60S2.h"
#include "intrins.h"

sbit SDA = P2^1;
sbit SCL = P2^0;

void IIC_Delay(unsigned char i);
void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 
void operate_delay(unsigned char a);
void pfc8591_init();
unsigned char adc();
void write_eeprom(unsigned char add,unsigned char value);
unsigned char read_eeprom(unsigned char add);

#endif