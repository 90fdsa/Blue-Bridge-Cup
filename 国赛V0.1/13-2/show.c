#include "show.h"

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
														 0x88,0x89,0x8e,0x8c,};
unsigned char buffer[]={10,10,10,10,10,10,10,10};
//用于正常运作
unsigned char weixuan;
unsigned char show_state;
unsigned char show_canshu;
unsigned char distance_huan;
unsigned char freq_huan;
//从主函数接过来的
//extern unsigned int xdata frequency;
//extern unsigned char xdata adc_value;
//extern unsigned char xdata distance;
extern unsigned int frequency;
extern unsigned char adc_value;
extern unsigned char distance;
extern unsigned char qiehuan;
extern unsigned int fre_standard;
extern unsigned char him_standard;
extern unsigned char dis_standard;
//临时变量
extern float him;

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
		switch(show_state)
		{
			case 0:show_frequency();break;
			case 1:show_himid();break;
			case 2:show_distance();break;
			case 3:
			{
					switch(show_canshu)
					{
						case 0:show_frequency_parameter();break;
						case 1:show_himid_parameter();break;
						case 2:show_distance_parameter();break;
					}
			}break;
			
		}
}

void show_frequency()
{
	  if(freq_huan==0)
		{
				buffer[0]=24;buffer[1]=10;buffer[2]=10;
				buffer[3]=frequency/10000;
				buffer[4]=frequency%10000/1000;
				buffer[5]=frequency%1000/100;
				buffer[6]=frequency%100/10;
				buffer[7]=frequency%10;
		}
		if(freq_huan==1)
		{
				buffer[0]=24;buffer[1]=10;buffer[2]=10;
				buffer[3]=10;
				buffer[4]=10;
				buffer[5]=frequency/10000;
				buffer[6]=frequency%10000/1000+11;
				buffer[7]=frequency%1000/100;
		}
}

void show_himid()
{
	  
		buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=((char)him)/10;
	  buffer[7]=((char)him)%10;
}

void show_distance()
{
	  if(distance_huan==0)
		{
			buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;
			buffer[4]=10;
			buffer[5]=distance/100;
			buffer[6]=distance%100/10;
			buffer[7]=distance%10;
		}
		if(distance_huan==1)
		{
			buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;
			buffer[4]=10;
			buffer[5]=distance/100+11;
			buffer[6]=distance%100/10;
			buffer[7]=distance%10;
		}
}

void show_frequency_parameter()
{
		buffer[0]=25;buffer[1]=1;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;
	  buffer[5]=fre_standard/10000;
	  buffer[6]=fre_standard%10000/1000+11;
	  buffer[7]=fre_standard%1000/100;
}

void show_himid_parameter()
{
		buffer[0]=25;buffer[1]=2;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=him_standard/10;
	  buffer[7]=him_standard%10;
}

void show_distance_parameter()
{
		buffer[0]=25;buffer[1]=3;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=dis_standard/100+11;
	  buffer[7]=dis_standard%100/10;
}
