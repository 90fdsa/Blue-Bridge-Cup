#include "reg52.h"
#include "SMG.h"

unsigned int count_f=0;
unsigned int count_dat=0;
unsigned int count_t=0; 


void cls_buzz()
{
	P0=0;
	Select573(5);
	Select573(0);
	
}

void cls_led()
{
	P0=0XFF;
	Select573(4);
	Select573(0);
	
}



void InitSystem()
{
	cls_buzz();
	cls_led();

}
void InitT()
{
	TH0=0XFF;
	TL0=0XFF;
	TH1=(65535-1000)/256;
	TL1=(65535-1000)%256;
	TMOD=0X04;
	ET0=1;
	ET1=1;
	EA=1;
	TR0=1;
	TR1=1;

}

void Service_T0() interrupt 1
{
	count_f++;
}

void Service_T1() interrupt 3
{
//	TH1=(65535-1000)/256;
//	TL1=(65535-1000)%256;
	count_t++;
	if(count_t==1000)
	{
		count_dat=count_f*4;
		count_f=0;
		count_t=0;
	}
	
	
}
void Display()
{
	
	SMG_bit(0,0x8e);
	SMG_delay(500);

	if(count_dat>=1000)
	{
	SMG_bit(4,SMG_duanma[count_dat/1000%10]);
	SMG_delay(500);
	}
	if(count_dat>=10000)
	{
	SMG_bit(3,SMG_duanma[count_dat/10000]);
	SMG_delay(500);
	}
	SMG_bit(5,SMG_duanma[count_dat/100%10]);
	SMG_delay(500);	
	SMG_bit(6,SMG_duanma[count_dat/10%10]);
	SMG_delay(500);	
	SMG_bit(7,SMG_duanma[count_dat%10]);
	SMG_delay(500);
	SMG_bit(7,0xff);	
}

void main()
{
	InitSystem();
	InitT();
	while(1)
	{
		Display();
	}
	
	
}