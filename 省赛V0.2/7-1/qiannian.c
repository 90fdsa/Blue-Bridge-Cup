#include "STC15F2K60S2.H"
#include "onewire.h"

#define uchar unsigned char 
#define uint unsigned int 

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,0xbf,0xc6};
uchar buffer[]={10,10,10,10,10,10,10,10};
uint working_duty[]={200,500,700};
uchar working_time[]={0,60,120};
uchar working_teim=0;
uchar working_mod=0;
uchar start=0;
uchar time=0;
uchar weixuan=0;
uchar display_mod=0;
uchar LED;
float temperature=0;

uchar key_value=0;
uchar old_key_value=0;

void display();
void read_keyboard();
void key_proc();
void display1();
void display2();
void fen();
void led_show();

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
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}

void Timer1Init(void)		//1微秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xFF;		//设置定时初值
	TH1 = 0xFF;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
}

void main()
{
		close_led();
	  close_buzz();
	  Timer0Init();
	  Timer1Init();
	  EA=1;
	  while(1)
		{
				fen();
		}
}

void timer0_proc() interrupt 1
{
	  static uchar intr=0;
	  static uint sec=0;
	  static uint tem_t=0;
	  if(++intr==10)
		{
				intr=0;
				led_show();
			  old_key_value=key_value;
			  read_keyboard();
			  if(old_key_value<key_value){key_proc();}
		}
		if(++sec==1000)
		{
				sec=0;
			  if(time>0){time--;}
		}
		if(++tem_t==1000)
		{
				tem_t=0;
			  temperature=rd_temperature();
		}
		display();
}

void timer1_proc() interrupt 3
{
	  static unsigned int du=0;
	  du++;
		if(time>0)
		{
				if(du>0 && du<working_duty[working_mod]){P34=1;}
				if(du>=du<working_duty[working_mod] && du<1000){P34=0;}
				if(du==1000){du=0;}
		}
}

void display()
{
		P0=0xff;
	  P2=(P2&0x1f)|0xE0;
	  P2&=0x1f;
	
	  P0=1<<weixuan;
	  P2=(P2&0x1f)|0xC0;
	  P2&=0x1f;
	  
	  P0=duanma[buffer[weixuan]];
	  P2=(P2&0x1f)|0xE0;
	  P2&=0x1f;
	
	  weixuan++;
	  if(weixuan==8){weixuan=0;}
}

void read_keyboard()
{
	  uint key_zan;
	  
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  key_zan=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  key_zan=(key_zan<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  key_zan=(key_zan<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  key_zan=(key_zan<<4)|(P3&0x0f);
	  
	  switch(~key_zan)
		{
			case 0x1000:key_value=1;break;
			case 0x0100:key_value=2;break;
			case 0x0010:key_value=3;break;
			case 0x0001:key_value=4;break;
			case 0x2000:key_value=5;break;
			case 0x0200:key_value=6;break;
			case 0x0020:key_value=7;break;
			case 0x0002:key_value=8;break;
			case 0x4000:key_value=9;break;
			case 0x0400:key_value=10;break;
			case 0x0040:key_value=11;break;
			case 0x0004:key_value=12;break;
			case 0x8000:key_value=13;break;
			case 0x0800:key_value=14;break;
			case 0x0080:key_value=15;break;
			case 0x0008:key_value=16;break;
			default:key_value=0;break;
		}
}

void key_proc()
{
		switch(key_value)
		{
			case 13://S4
			{
					working_mod++;
				  if(working_mod==3){working_mod=0;}
			}break;
			case 9://S5
			{
			    working_teim++;
				  if(working_teim==3){working_teim=0;}
					time=working_time[working_teim];
			}break;
			case 10://S9
			{
					time=0;
			}break;
			case 14://S8
			{
			    display_mod=~display_mod;
			}break;
		}
}

void fen()
{
		if(display_mod==0){display1();}
		else{display2();}
}

void display1()
{
		buffer[0]=11;
	  buffer[1]=working_mod+1;
		buffer[2]=11;
	  buffer[3]=10;
		buffer[4]=time/1000;
	  buffer[5]=time%1000/100;
		buffer[6]=time%100/10;
	  buffer[7]=time%10;
}

void display2()
{
		buffer[0]=11;
	  buffer[1]=4;
		buffer[2]=11;
	  buffer[3]=10;
		buffer[4]=10;
	  buffer[5]=((char)temperature)/10;
		buffer[6]=((char)temperature)%10;
	  buffer[7]=12;
}

void led_show()
{
	  if(time>0)
		{
				if(working_mod==0){LED&=0xfe;}
				else{LED|=0x01;}
				if(working_mod==1){LED&=0xfd;}
				else{LED|=0x02;}
				if(working_mod==2){LED&=0xfb;}
				else{LED|=0x04;}
		}
		else{LED=0xff;}
		
		P0=LED;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}