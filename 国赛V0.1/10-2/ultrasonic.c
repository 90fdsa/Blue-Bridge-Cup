#include "ultrasonic.h"

sbit TX=P1^0;
sbit RX=P1^1;

void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void Send_Wave()
{
	unsigned char i;
	for(i=0;i<10;i++)
	{
		TX=1;
		Delay13us();
		TX=0;
		Delay13us();
	}
}

unsigned char sonic()
{
	unsigned char dis=0;
	 
	Send_Wave();
	 
	CH=0;
	CL=0;
	CR=1;
	while((RX)&&(!CF));
	CR=0;
	
	if(CF)
	{
			CF=0;
		  dis=255;
	}
	else
	{
			dis=((CH<<8)+CL)*0.017;
	}
	
	return dis;
}