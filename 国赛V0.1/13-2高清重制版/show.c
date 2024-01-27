#include "show.h"

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0x8e,0x89,0x88,0x8c};
//                             F    H    A    P

unsigned char buffer[]={10,10,10,10,10,10,10,10};
unsigned char weixuan;
unsigned char show_mod,show_sequence;
extern unsigned char distance_danwei,frequency_danwei;
//测量量
extern unsigned int freq,frequency;
extern unsigned char himid,distance;
//参数
extern unsigned char himid_can;
extern unsigned char distance_can;
extern unsigned int frequence_can;


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
			case 0:show_frequency();break;
			case 1:show_himid();break;
			case 2:show_distance();break;
			case 3:
			{
			    switch(show_sequence)
					{
						case 0:show_frequency_para();break;
						case 1:show_himid_para();break;
						case 2:show_distance_para();break;
					}
			}break;
		}
}

void show_frequency()
{
	  if(frequency_danwei==0)
		{
				buffer[0]=22;buffer[1]=10;buffer[2]=10;
				(frequency>10000)?(buffer[3]=frequency/10000):(buffer[3]=10);
				(frequency>1000)?(buffer[4]=frequency%10000/1000):(buffer[4]=10);
				(frequency>100)?(buffer[5]=frequency%1000/100):(buffer[5]=10);
				buffer[6]=frequency%100/10;
				buffer[7]=frequency%10;
		}
		else
		{
				buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				buffer[5]=frequency/10000;
				buffer[6]=frequency%10000/1000+11;
				buffer[7]=frequency%1000/100;
		}
		
}
void show_himid()
{
		buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=himid/10;
	  buffer[7]=himid%10;
}
void show_distance()
{
	  if(distance_danwei==0)
		{
				buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=10;
				buffer[4]=10;
			  (distance>100)?(buffer[5]=distance/100):(buffer[5]=10);
				(distance>10)?(buffer[6]=distance%100/10):(buffer[6]=10);
				buffer[7]=distance%10;
		}
		else
		{
				buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=10;
				buffer[4]=10;
				buffer[5]=distance/100+11;
				buffer[6]=distance%100/10;
				buffer[7]=distance%10;
		}
		
}
void show_frequency_para()
{
		buffer[0]=25;buffer[1]=1;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;
	  (frequence_can>10000)?(buffer[5]=frequence_can/10000):(buffer[5]=10);
	  buffer[6]=frequence_can%10000/1000+11;
	  buffer[7]=frequence_can%1000/100;
}
void show_himid_para()
{
		buffer[0]=25;buffer[1]=2;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=himid_can/10;
	  buffer[7]=himid_can%10;
}
void show_distance_para()
{
		buffer[0]=25;buffer[1]=3;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=distance_can/100+11;
	  buffer[7]=distance_can%100/10;
}