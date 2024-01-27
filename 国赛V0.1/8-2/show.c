#include "show.h"

code unsigned char duanma[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x8e};
unsigned char buffer[8]={10,10,10,10,10,10,10,10};
extern unsigned char jilu_distance[10];

unsigned char weixuan;
unsigned char show_mod;
extern unsigned char old_distance_mod;//加和模式或过去模式
extern unsigned char jilu_count,reme_count;

//测量值
extern unsigned char distance,old_distance;
//标准值
extern unsigned char distance_standard;

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
			case 0:show_distance();break;
			case 1:show_history();break;
			case 2:show_parameter();break;
		}
}
void show_distance()
{
		buffer[0]=old_distance_mod;buffer[1]=10;
	  buffer[2]=old_distance/100;
	  buffer[3]=old_distance%100/10;
	  buffer[4]=old_distance%10;
	  buffer[5]=distance/100;
	  buffer[6]=distance%100/10;
	  buffer[7]=distance%10;
}
void show_history()
{
		buffer[0]=(reme_count+1)/10;
	  buffer[1]=(reme_count+1)%10;
	  buffer[2]=10;buffer[3]=10;buffer[4]=10;
	  buffer[5]=jilu_distance[reme_count]/100;
	  buffer[6]=jilu_distance[reme_count]%100/10;
	  buffer[7]=jilu_distance[reme_count]%10;
}
void show_parameter()
{
		buffer[0]=11;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=distance_standard/10;
	  buffer[7]=distance_standard%10;
}