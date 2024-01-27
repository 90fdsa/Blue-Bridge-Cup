#include "display.h"

code unsigned char duanma[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc6,0xc7,0x8c,0xc8};
//                             C    L    P    N
unsigned char buffer[8]={10,10,10,10,10,10,10,10};
unsigned char weixuan;

extern unsigned char key_value,old_key_value;
//传过来的测量值
extern float true_temperature;
extern unsigned char distance;
extern unsigned int change_time_of_parameter;
//传过来的参数值
extern char temperature_parameter;
extern char distance_parameter;

unsigned char show_mod,show_sequence;

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
					 case 0:temperature_show();break;
					 case 1:distance_show();break;
					 case 2:changetime_show();break;
				 }
			}break;
			case 1:
			{
					switch(show_sequence)
					{
					  case 0:temperature_parameter_show();break;
					  case 1:distance_parameter_show();break;
					} 
			}break;
		}
}

void temperature_show()
{
		buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=((int)(true_temperature*100))/1000;
	  buffer[5]=((int)(true_temperature*100))%1000/100+11;
	  buffer[6]=((int)(true_temperature*100))%100/10;
	  buffer[7]=((int)(true_temperature*100))%10;
}

void distance_show()
{
    buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=distance/10;
	  buffer[7]=distance%10;
}

void changetime_show()
{
		buffer[0]=25;buffer[1]=10;buffer[2]=10;
	  (change_time_of_parameter>10000)?(buffer[3]=change_time_of_parameter/10000):(buffer[3]=10);
	  (change_time_of_parameter>1000)?(buffer[4]=change_time_of_parameter%10000/1000):(buffer[4]=10);
	  (change_time_of_parameter>100)?(buffer[5]=change_time_of_parameter%1000/100):(buffer[5]=10);
	  (change_time_of_parameter>10)?(buffer[6]=change_time_of_parameter%100/10):(buffer[6]=10);
	  buffer[7]=change_time_of_parameter%10;
}

void temperature_parameter_show()
{
		buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=1;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=temperature_parameter/10;
	  buffer[7]=temperature_parameter%10;
}

void distance_parameter_show()
{
		buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=2;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=distance_parameter/10;
	  buffer[7]=distance_parameter%10;
}