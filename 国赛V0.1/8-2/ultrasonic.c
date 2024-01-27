#include "ultrasonic.h"
#include "init.h"

sbit tx=P1^0;
sbit rx=P1^1;

void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void send_wave()
{
	unsigned char i;
	for(i=0;i<10;i++)
	{
			tx=1;
		  Delay13us();
		  tx=0;
		  Delay13us();
	}
}

unsigned char distance_massure()
{
	unsigned char dis;
	send_wave();
	CL=0;CH=0;CR=1;
	while((rx)&(!CF));
	CR=0;
	if(CF==1){CF=0;dis=255;}
	else{dis=((CH<<8)+CL)*0.017;}
	return dis;
}