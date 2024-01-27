#include "iic.h"

#define DELAY_TIME	5

sbit scl=P2^0;
sbit sda=P2^1;

//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

void operate_delay(unsigned char a)
{
	  unsigned char i;
		while(a--){for(i=0;i<112;i++);}
}
void pcf8591_init()
{
		I2CStart();
	  I2CSendByte(0x90);
	  I2CWaitAck();
	  I2CSendByte(0x03);
	  I2CWaitAck();
	  I2CStop();
	  operate_delay(10);
}
void pcf8591_dac(unsigned char da)
{
		I2CStart();
	  I2CSendByte(0x90);
	  I2CWaitAck();
	  I2CSendByte(0x43);
	  I2CWaitAck();
	  I2CSendByte(da);
	  I2CWaitAck();
	  I2CStop();
}

void write_eeprom(unsigned char add,unsigned char value)
{
		I2CStart();
	  I2CSendByte(0xa0);
	  I2CWaitAck();
	  I2CSendByte(add);
	  I2CWaitAck();
	  I2CSendByte(value);
	  I2CWaitAck();
	  I2CStop();
	  operate_delay(10);
}
unsigned char read_eeprom(unsigned char add)
{
		unsigned char a;
	  
	  I2CStart();
	  I2CSendByte(0xa0);
	  I2CWaitAck();
	  I2CSendByte(add);
	  I2CWaitAck();
	  I2CStop();
	
	  I2CStart();
	  I2CSendByte(0xa1);
	  I2CWaitAck();
	  a=I2CReceiveByte();
	  I2CSendAck(1);
	  I2CStop();
	
	  return a;
}