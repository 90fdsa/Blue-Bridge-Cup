#ifndef _IIC_H
#define _IIC_H

#include "stc15f2k60s2.h"
#include "intrins.h"

static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);

void operate_delay(unsigned char a);
void pcf8591_init();
unsigned char pcf8591_adc();

void eeprom_write(unsigned char add,unsigned char value);
unsigned char eeprom_read(unsigned char add);
void Delay5ms();		//@12.000MHz



#endif