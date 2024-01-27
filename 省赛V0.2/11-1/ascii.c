#include "STC15F2K60S2.h"
#include "iic.h"

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0x8c,0xab};
unsigned char buffer[]={10,10,10,10,10,10,10,10};

unsigned char weima;
unsigned char key_value;
unsigned char old_key_value;
unsigned char show_mod=1;
unsigned char old_show_mod;
unsigned char adc_value;
unsigned char adc_flag=0;
unsigned char times=0;
unsigned char write_flag=0;
float voltage;
float old_voltage;
float voltage_standard=3;
unsigned char voltage_standard_stand;

unsigned char read_key();
void display();
void key_proc();
void show_fen();
void show_adc();
void show_standard();
void show_times();

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0x1f)|0xa0;
	  P2&=0x1f;
}

void close_led()
{
		P0=0xff;
	  P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}

void Timer0Init(void)		//1000微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}

void main()
{
		close_buzz();
	  close_led();
	  Timer0Init();
	  
	  voltage_standard_stand=read_eeprom(0x00);
	  voltage_standard=((float)voltage_standard_stand)/10;
	
	  pfc8591_init();
	  EA=1;
	  while(1)
		{
			  if(adc_flag)
				{
					  adc_flag=0;
						adc_value=adc();
					  old_voltage=voltage;
						voltage=adc_value*0.0196;
					  if(old_voltage>voltage_standard&&voltage<voltage_standard)
						{times++;}
				}
			  show_fen();
		}
}

void timer0_proc() interrupt 1
{
		static unsigned char a;
	  static unsigned char adc_time;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
		}
		if(++adc_time==50)
		{
				adc_time=0;
			  adc_flag=1;
		}
		if(voltage>voltage_standard)
		{
				
		}
}

unsigned char read_key()
{
	  unsigned char key;
	  unsigned int stand;
	  
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  stand=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  stand=(stand<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  stand=(stand<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  stand=(stand<<4)|(P3&0x0f);
	
	  switch(~stand)
		{
			case 0x1000:key=1;break;
			case 0x0100:key=2;break;
			case 0x0010:key=3;break;
			case 0x0001:key=4;break;
			case 0x2000:key=5;break;
			case 0x0200:key=6;break;
			case 0x0020:key=7;break;
			case 0x0002:key=8;break;
			case 0x4000:key=9;break;
			case 0x0400:key=10;break;
			case 0x0040:key=11;break;
			case 0x0004:key=12;break;
			case 0x8000:key=13;break;
			case 0x0800:key=14;break;
			case 0x0080:key=15;break;
			case 0x0008:key=16;break;
			default:key=0;break;
		}
		
		return key;
}

void display()
{
		P0=0xff;
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  P0=1<<weima;
	  P2=(P2&0x1f)|0xc0;
	  P2&=0x1f;
	
	  P0=duanma[buffer[weima]];
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  if(++weima==8){weima=0;}
}

void key_proc()
{
		switch(key_value)
		{
			case 15:
			{
					show_mod++;
				  if(show_mod==4)
          {
							show_mod=1;
						  voltage_standard_stand=voltage_standard*10;
					    write_eeprom(0x00,voltage_standard_stand);
					}
			}break;
			case 16:
			{
					if(show_mod==2)
					{
							voltage_standard+=0.5;
						  if(voltage_standard==5.5){voltage_standard=0;}
					}
			}break;
			case 12:
			{
					if(show_mod==2)
					{
							voltage_standard-=0.5;
						  if(voltage_standard<0){voltage_standard=5;}
					}
			}break;
			case 11:
			{
					if(show_mod==3)
					{
							times=0;
					}
			}break;
		}
}

void show_fen()
{
		switch(show_mod)
		{
			case 1:show_adc();break;
			case 2:show_standard();break;
			case 3:show_times();break;
		}
}

void show_adc()
{
	  buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
		buffer[5]=((int)(voltage*100))/100+11;
	  buffer[6]=((int)(voltage*100))%100/10;
	  buffer[7]=((int)(voltage*100))%10;
}

void show_standard()
{
	  buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
		buffer[5]=((int)(voltage_standard*100))/100+11;
	  buffer[6]=((int)(voltage_standard*100))%100/10;
	  buffer[7]=((int)(voltage_standard*100))%10;
//		buffer[5]=voltage_standard_stand/100;
//	  buffer[6]=voltage_standard_stand%100/10;
//	  buffer[7]=voltage_standard_stand%10;
}

void show_times()
{
	  buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
	  buffer[5]=10;
	  buffer[6]=times/10;
	  buffer[7]=times%10;
}