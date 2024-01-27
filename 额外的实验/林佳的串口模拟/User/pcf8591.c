#include "pcf8591.h"

unsigned char ADC()
{
	unsigned char dat;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x43);
	IIC_WaitAck();	
	IIC_Start();
	IIC_SendByte(0x91);	
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	return dat;
	
}


