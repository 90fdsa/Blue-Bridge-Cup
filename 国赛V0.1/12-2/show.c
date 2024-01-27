#include "show.h"
#include "ds1302.h"

code unsigned char duanma[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xbf,0xc7,0xc6,0x8e,0x89,0xfe,0xf7,0x8c};
                             //-    L    C    F    H   up-   dn-  P
unsigned char buffer[]={10,10,10,10,10,10,10,10};
extern unsigned char period[];
unsigned char weixuan;

extern unsigned char distance;
extern unsigned char adc_value;
extern unsigned char max,min;
extern float average;

unsigned char qiehuan,shuju,canshu,jilu;
unsigned char h,m,s;
unsigned char sonic_mod_flag=0;
unsigned char jishi_sonic_flag=0;
unsigned char miaoshu;
unsigned char dis_canshu=20;

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
	  s=Read_Ds1302_Byte(0x81);
		m=Read_Ds1302_Byte(0x83);
		h=Read_Ds1302_Byte(0x85);
	
		s=s/16*10+s%16;
		m=m/16*10+m%16;
		h=h/16*10+h%16;
		switch(qiehuan)
		{
			case 0:
			{
				switch(shuju)
				{
					case 0:show_time();break;
					case 1:show_distance();break;
					case 2:
					{
							switch(jilu)
							{
								case 0:show_data_max();break;
								case 1:show_data_avg();break;
								case 2:show_data_min();break;
							}
					}break;
				}
			}break;
			case 1:
			{
				switch(canshu)
				{
					case 0:show_time_canshu();break;
					case 1:show_distance_canshu();break;
				}
			}break;
		}
		
		
}

void show_time()
{
	
	
	buffer[0]=h/10;buffer[1]=h%10;
	buffer[2]=22;
	buffer[3]=m/10;buffer[4]=m%10;
	buffer[5]=22;
	buffer[6]=s/10;buffer[7]=s%10;
}

void show_distance()
{
	if(sonic_mod_flag==0)
	{
			buffer[0]=23;buffer[1]=24;buffer[2]=10;buffer[3]=10;
			buffer[4]=10;
			buffer[5]=distance/100;
			buffer[6]=distance%100/10;
			buffer[7]=distance%10;
	}
	else
	{
			buffer[0]=23;buffer[1]=25;
		  buffer[2]=10;buffer[3]=10;
			buffer[4]=10;
			buffer[5]=distance/100;
			buffer[6]=distance%100/10;
			buffer[7]=distance%10;
	}
}
void show_data_max()
{
	buffer[0]=26;buffer[1]=27;buffer[2]=10;buffer[3]=10;
	buffer[4]=10;
	buffer[5]=max/100;
	buffer[6]=max%100/10;
	buffer[7]=max%10;
}

void show_data_avg()
{
	buffer[0]=26;buffer[1]=22;buffer[2]=10;buffer[3]=10;
	buffer[4]=((int)(average*10))/1000;
	buffer[5]=((int)(average*10))%1000/100;
	buffer[6]=(((int)(average*10))%100/10)+11;
	buffer[7]=((int)(average*10))%10;
}

void show_data_min()
{
	buffer[0]=26;buffer[1]=28;buffer[2]=10;buffer[3]=10;
	buffer[4]=10;
	buffer[5]=min/100;
	buffer[6]=min%100/10;
	buffer[7]=min%10;
}

void show_time_canshu()
{
	buffer[0]=29;buffer[1]=1;buffer[2]=10;buffer[3]=10;
	buffer[4]=10;buffer[5]=10;
	buffer[6]=period[miaoshu]/10;
	buffer[7]=period[miaoshu]%10;
}

void show_distance_canshu()
{
	buffer[0]=29;buffer[1]=2;buffer[2]=10;buffer[3]=10;
	buffer[4]=10;buffer[5]=10;
	buffer[6]=dis_canshu/10;
	buffer[7]=dis_canshu%10;
}