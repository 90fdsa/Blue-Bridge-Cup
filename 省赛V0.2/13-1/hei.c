#include "STC15F2K60S2.H"
#include "ds1302.h"
#include "onewire.h"
#include "intrins.h"

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
														 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0xbf};
unsigned char buffer[]={10,10,10,10,10,10,10,10};

unsigned char weixuan=0;
unsigned char key_value=0;
unsigned char old_key_value=0;
float temperature;
unsigned char hours=1,minutes=1,seconds=1;
unsigned char show_mod=1;
unsigned char temperature_standard=25;
unsigned char jidian_state=0;
unsigned char control_state=0;
unsigned char led1=0;
unsigned char led2=0;
unsigned char led3=0;
unsigned char pause=0;
unsigned char jidian_ok=0;
unsigned char time_show_mod;

unsigned char read_keyboard();
void display();
void get_time();
void fen();
void show_temperature();
void show_time1();
void show_time2();
void show_standard();
void key_proc();
void key_proc_short();
void temp_control();
void time_control();
void control_fen();
void led_control();

void close_led()
{
		P0=0xff;
	  P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0x1f)|0xa0;
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
	  set_time(23,59,40);
		close_led();
	  close_buzz();
	  
	  Timer0Init();
    EA=1;
	  
    while(1)
		{
				
		}			
}

void timer0_proc() interrupt 1
{
		static unsigned char a;
	  static unsigned int tem;
	  static unsigned int jidian;
	  static unsigned int led_1hao;
	  static unsigned char blink;
	  display();
	  
	  if(++a==10)
		{
				a=0;
			  led_control();
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  if(old_key_value<key_value){key_proc();}
				if(old_key_value>key_value){time_show_mod=0;}
				
				
			  get_time();
				fen();
				control_fen();
				key_proc_short();
		}
		if(++tem==300)
		{
				tem=0;
			  temperature=rd_temperature();
		}
		if(jidian_state==1)
		{
				jidian++;
			  if(jidian==5000){jidian=0;jidian_state=0;}
		}
		if(led1==1)
		{
				led_1hao++;
			  if(led_1hao==5000){led_1hao=0;led1=0;}
		}
		if(jidian_ok==1)
		{
				blink++;
			  if(blink==100)
				{
					  blink=0;
						led3=~led3;
				}
		}
		else
		{
				blink=0;
		}
}

unsigned char read_keyboard()
{
	  static unsigned char key_val;
	  static unsigned int deal;
	  
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  deal=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  deal=(deal<<4)|(P3&0x0f);
		P44=1;P42=1;P35=0;P34=1;
	  deal=(deal<<4)|(P3&0x0f);
		P44=1;P42=1;P35=1;P34=0;
	  deal=(deal<<4)|(P3&0x0f);
	  switch(~deal)
		{
			case 0x1000:key_val=1;break;
			case 0x0100:key_val=2;break;
			case 0x0010:key_val=3;break;
			case 0x0001:key_val=4;break;
			case 0x2000:key_val=5;break;
			case 0x0200:key_val=6;break;
			case 0x0020:key_val=7;break;
			case 0x0002:key_val=8;break;
			case 0x4000:key_val=9;break;
			case 0x0400:key_val=10;break;
			case 0x0040:key_val=11;break;
			case 0x0004:key_val=12;break;
			case 0x8000:key_val=13;break;
			case 0x0800:key_val=14;break;
			case 0x0080:key_val=15;break;
			case 0x0008:key_val=16;break;
			default:key_val=0;break;
		}
		return key_val;
}

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

void key_proc()
{
		switch(key_value)
		{
			case 15:
			{
					show_mod++;
				  if(show_mod==4){show_mod=1;}
			}break;
			case 11:
			{
					control_state=~control_state;
			}break;
			case 16:
			{
					if(show_mod==3)
					{
							temperature_standard++;
						  if(temperature_standard==100){temperature_standard=10;}
					}
			}break;
			case 12:
			{
					if(show_mod==3)
					{
							temperature_standard--;
						  if(temperature_standard==9){temperature_standard=99;}
					}
			}break;
		}
}

void key_proc_short()
{
		switch(key_value)
		{
			case 12:
			{
					if(show_mod==2)
					{
							time_show_mod=1;
					}
			}break;
		}
}
void get_time()
{
		seconds=Read_Ds1302_Byte(0x81);
		minutes=Read_Ds1302_Byte(0x83);
		hours=Read_Ds1302_Byte(0x85);
	  
	  hours=hours/16*10+hours%16;
		minutes=minutes/16*10+minutes%16;
		seconds=seconds/16*10+seconds%16;
}

void fen()
{
		switch(show_mod)
		{
			case 1:show_temperature();break;
			case 2:
			{
				  if(time_show_mod==0){show_time1();}
					else{show_time2();}
				  break;
			}
			case 3:show_standard();break;
		}
}

void show_temperature()
{
	  buffer[0]=22;buffer[1]=1;buffer[2]=10;buffer[3]=10;buffer[4]=10;
	  buffer[5]=((int)(temperature*10))/100;
		buffer[6]=((int)(temperature*10))%100/10+11;
		buffer[7]=((int)(temperature*10))%10;
}

void show_time1()
{
		buffer[0]=22;buffer[1]=2;buffer[2]=10;
	  buffer[3]=hours/10;
	  buffer[4]=hours%10;
	  buffer[5]=23;
		buffer[6]=minutes/10;
		buffer[7]=minutes%10;
}

void show_time2()
{
		buffer[0]=22;buffer[1]=2;buffer[2]=10;
	  buffer[3]=minutes/10;
	  buffer[4]=minutes%10;
	  buffer[5]=23;
		buffer[6]=seconds/10;
		buffer[7]=seconds%10;
}


void show_standard()
{
		buffer[0]=22;buffer[1]=3;buffer[2]=10;buffer[3]=10;
		buffer[4]=10;buffer[5]=10;
	  buffer[6]=temperature_standard/10;
	  buffer[7]=temperature_standard%10;
}

void temp_control()
{
	  unsigned char a;
		if(temperature>temperature_standard){a=0x10;jidian_ok=1;}
		else{a=0x00;jidian_ok=0;}

		P0=a;
		P2=(P2&0x1f)|0xa0;
		P2&=0x1f;
}

void time_control()
{
	  unsigned char b;
		if(minutes==0&&seconds==0)
    {jidian_state=1;}
		if(jidian_state==1){b=0x10;jidian_ok=1;}
		else{b=0x00;jidian_ok=0;}
		
		
		P0=b;
		P2=(P2&0x1f)|0xa0;
		P2&=0x1f;
}

void control_fen()
{
		if(control_state==0){temp_control();}
		else{time_control();}
}

void led_control()
{
	  unsigned char c;
		if(minutes==0&&seconds==0){led1=1;}
		if(control_state==0){led2=1;}
		else{led2=0;}
		
		if(led1==1){c|=0x01;}
		else{c&=0xfe;}
		if(led2==1){c|=0x02;}
		else{c&=0xfd;}
		if(led3!=0){c|=0x04;}
		else{c&=0xfb;}
		
		P0=~c;
		P2=(P2&0x1f)|0x80;
		P2&=0x1f;
}