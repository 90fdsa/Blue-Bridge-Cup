#include "STC15f2K60S2.h"
#include "iic.h"

code unsigned char weima[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
                            0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                            0xc1,0x8e};
unsigned char buffer[]={10,10,10,10,10,10,10,10};

unsigned char ad_flag=0;
unsigned char weixuan;
unsigned char old_key_value;
unsigned char key_value;
unsigned char adc_value;
float voltage;
unsigned int fr_count;
unsigned int frequent=0;
unsigned char show_mod=0;
unsigned char dac_mod=0;
unsigned char led_flag=0;
unsigned char smg_flag=0;

unsigned char read_keyboard();
void display();
void key_proc();
void fen();
void show_frequency();
void show_voltage();
void show_nothing();
void dac_out();
void led_proc();
void led_choose();

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0X1F)|0XA0;
	  P2&=0x1f;
}

void close_led()
{
		P0=0xff;
	  P2=(P2&0X1F)|0X80;
	  P2&=0x1f;
}

void Timer0Init(void)		//100微秒@12.000MHz
{
	TMOD = 0xF4;		//设置定时器模式
	TL0 = 0xff;		//设置定时初值
	TH0 = 0xff;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;
	ET0 = 1;		//定时器0开始中断
}

void Timer1Init(void)		//1000微秒@12.000MHz
{
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xfc;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;
	ET1 = 1;
}

void main()
{
		close_buzz();
	  close_led();
	  Timer0Init();
	  Timer1Init();
	  pcf8591_init();
    EA=1;
	  
	  while(1)
		{
			  fen();
		}
}

void Timer0_proc() interrupt 1
{
    fr_count++;
}

void Timer1_proc() interrupt 3
{
	  static unsigned char a;
	  static unsigned char ad;
	  static unsigned int freq;
		display();led_choose();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  if(old_key_value<key_value){key_proc();}
		}
		if(++ad==50)
		{
		    ad=0;
			  adc_value=pcf8591_adc();
			  voltage=adc_value*0.0196;
			  dac_out();
		}
		if(++freq==1000)
		{
			  frequent=fr_count;
			  fr_count=0;
			  freq=0;
		}
}

unsigned char read_keyboard()
{
	  unsigned char key;
	  unsigned int hao;
	
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  hao=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  hao=(hao<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  hao=(hao<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  hao=(hao<<4)|(P3&0x0f);
	
	  switch(~hao)
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
		P0=0Xff;
	  P2=(P2|0X1F)|0XE0;
	  P2&=0X1F;
	
	  P0=1<<weixuan;
	  P2=(P2|0X1F)|0Xc0;
	  P2&=0X1F;
	
	  P0=weima[buffer[weixuan]];
	  P2=(P2|0X1F)|0XE0;
	  P2&=0X1F;
	
	  if(++weixuan==8){weixuan=0;}
}

void key_proc()
{
		switch(key_value)
		{
			case 13:
			{
					show_mod++;
				  if(show_mod==2){show_mod=0;}
			}break;
			case 9:
			{
					dac_mod=~dac_mod;
			}break;
			case 14:
			{
					led_flag=~led_flag;
			}break;
			case 10:
			{
					smg_flag=~smg_flag;
			}break;
			
		}
}

void fen()
{
	  if(smg_flag==0)
		{
				switch(show_mod)
				{
					case 0:show_voltage();break;
					case 1:show_frequency();break;
				}
		}
		else
		{
				show_nothing();
		}
}

void show_voltage()
{
		buffer[0]=22;
	  buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
	  buffer[5]=((int)(voltage*100))/100+11;
		buffer[6]=((int)(voltage*100))%100/10;
	  buffer[7]=((int)(voltage*100))%10;
}

void show_frequency()
{
		buffer[0]=23;
	  buffer[1]=10;buffer[2]=10;
	  (frequent>10000)?(buffer[3]=frequent/10000):(buffer[3]=10);
	  (frequent>1000)?(buffer[4]=frequent%10000/1000):(buffer[4]=10);
	  (frequent>100)?(buffer[5]=frequent%1000/100):(buffer[5]=10);
		buffer[6]=frequent%100/10;
	  buffer[7]=frequent%10;
}

void show_nothing()
{
		buffer[0]=10;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;buffer[6]=10;buffer[7]=10;  
}

void dac_out()
{
		if(dac_mod==0){pcf8591_dac(102);}
		else{pcf8591_dac(adc_value);}
}

void led_choose()
{
		if(led_flag==0){led_proc();}
		else{close_led();}
}

void led_proc()
{
	  unsigned char a;
		if(show_mod==0){a|=0x01;}
		else{a&=0xfe;}
		if(show_mod==1){a|=0x02;}
		else{a&=0xfd;}
		if((voltage>3.5)||(voltage>=1.5&&voltage<2.5)){a|=0x04;}
		else{a&=0xfb;}
		if((frequent>10000)||(frequent>=1000&&frequent<5000)){a|=0x08;}
		else{a&=0xf7;}
		if(dac_mod!=0){a|=0x10;}
		else{a&=0xef;}
		
		P0=~a;
		P2=(P2&0X1F)|0X80;
	  P2&=0x1f;
}