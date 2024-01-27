#include "show.h"
#include "ds1302.h"

code unsigned char duanma[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xbf,0xc6,0x86,0x8c,};
                           //  -    C    E    P
unsigned char buffer[]={10,10,10,10,10,10,10,10};

unsigned char weixuan;
unsigned char show_mod,show_sequence;
unsigned char hour,minute,second;

extern float temperature,voltage;
extern unsigned char liangmie;
extern unsigned char led_can;
extern char shi_can,tem_can;

void display()
{
		P0=0xff;
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  P0=1<<weixuan;
	  P2=(P2&0x1f)|0xc0;
	  P2&=0x1f;
	 
	  P0=duanma[buffer[weixuan]];
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  if(++weixuan==8){weixuan=0;}
}

void fen()
{
		switch(show_mod)
		{
			case 0:
			{
					switch(show_sequence)
					{
						case 0:show_time();break;
						case 1:show_temperature();break;
						case 2:show_liang();break;
					}
			}break;
			case 1:
			{
					switch(show_sequence)
					{
						case 0:show_time_can();break;
						case 1:show_temperature_can();break;
						case 2:show_led_can();break;
					}
			}break;
		}
}

void show_time()
{
	  hour=Read_Ds1302_Byte(0x85);
	  minute=Read_Ds1302_Byte(0x83);
	  second=Read_Ds1302_Byte(0x81);
	
	  hour=hour/16*10+hour%16;
	  minute=minute/16*10+minute%16;
	  second=second/16*10+second%16;
	
		buffer[0]=hour/10;buffer[1]=hour%10;
	  buffer[2]=22;
	  buffer[3]=minute/10;buffer[4]=minute%10;
	  buffer[5]=22;
	  buffer[6]=second/10;buffer[7]=second%10;
}

void show_temperature()
{
		buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;
	  buffer[5]=((int)(temperature*10))/100;
	  buffer[6]=((int)(temperature*10))%100/10+11;
	  buffer[7]=((int)(temperature*10))%10;
}

void show_liang()
{
		buffer[0]=24;buffer[1]=10;
	  buffer[2]=((int)(voltage*100))/100+11;
	  buffer[3]=((int)(voltage*100))%100/10;
	  buffer[4]=((int)(voltage*100))%10;
	  buffer[5]=10;buffer[6]=10;
	  buffer[7]=liangmie;
}

void show_time_can()
{
		buffer[0]=25;buffer[1]=1;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=shi_can/10;
	  buffer[7]=shi_can%10;
}

void show_temperature_can()
{
		buffer[0]=25;buffer[1]=2;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=tem_can/10;
	  buffer[7]=tem_can%10;
}

void show_led_can()
{
		buffer[0]=25;buffer[1]=3;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;buffer[6]=10;
	  buffer[7]=led_can;
}