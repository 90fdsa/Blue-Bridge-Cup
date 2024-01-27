#include "STC15F2K60S2.H"
#include "onewire.h"
#include "iic.h"

code unsigned char duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,
														 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc6,0x8c,0x88};
unsigned char buffer[8]={10,10,10,10,10,10,10,10};

unsigned char weixuan;
unsigned char key_value;
unsigned char old_key_value;
unsigned char show_mod=1;
unsigned char tem_standard=20;
unsigned char dac_value;
unsigned char dac_output_mod=0;
float true_dac_value;
float temperature;

unsigned char read_key();
void key_proc();
void display();
void display_fen();
void display_tem();
void display_standard();
void display_dac();
void dac_choose();
void dacmod1();
void dacmod2();
void led_show();

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
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}

void main()
{
		close_buzz();
	  close_led();
	  init_pcf8591();
	  Timer0Init();
    EA=1;
    while(1)
		{
			  display_fen();
			  dac_choose();
		}			
}
	
void timer0_proc() interrupt 1
{
	  static unsigned char a;
	  static unsigned int tem;
		display();
	  if(++a==10)
		{
				a=0;
			  led_show();
			  old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
		}
		if(++tem==300)
		{
				tem=0;
			  temperature=rd_temperature();
		}
}

unsigned char read_key()
{
	  unsigned char key;
	  unsigned int value;
	  
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  value=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  value=(value<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  value=(value<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  value=(value<<4)|(P3&0x0f);
	
	  switch(~value)
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

void key_proc()
{
		switch(key_value)
		{
			case 13:
			{
					show_mod++;
				  if(show_mod==4){show_mod=1;}
			}break;
			case 9:
			{
					dac_output_mod=~dac_output_mod;
			}break;
			case 10:
			{
					if(show_mod==2)
					{
							tem_standard++;
						  if(tem_standard==100){tem_standard=10;}
					}
			}break;
			case 14:
			{
					if(show_mod==2)
					{
							tem_standard--;
						  if(tem_standard==9){tem_standard=99;}
					}
			}break;
		}
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

void display_fen()
{
		switch(show_mod)
		{
			case 1:display_tem();break;
			case 2:display_standard();break;
			case 3:display_dac();break;
		}
}

void display_tem()
{
		buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;
		buffer[4]=((int)(temperature*100))/1000;
	  buffer[5]=((int)(temperature*100))%1000/100+11;
		buffer[6]=((int)(temperature*100))%100/10;
		buffer[7]=((int)(temperature*100))%10;
}

void display_standard()
{
		buffer[0]=23;buffer[1]=10;buffer[2]=10;buffer[3]=10;
		buffer[4]=10;
	  buffer[5]=10;
		buffer[6]=tem_standard/10;
		buffer[7]=tem_standard%10;
}

void display_dac()
{
	  true_dac_value=dac_value*0.0196;
		buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=10;
		buffer[4]=10;
	  buffer[5]=((int)(true_dac_value*100))/100+11;
		buffer[6]=((int)(true_dac_value*100))%100/10;
		buffer[7]=((int)(true_dac_value*100))%10;
}

void dac_choose()
{
		if(dac_output_mod==0){dacmod1();}
		else{dacmod2();}
}

void dacmod1()
{
		if(tem_standard<temperature)
		{
			dac_value=255;
			DAC(dac_value);
		}
		else
		{
			dac_value=0;
			DAC(dac_value);
		}
}

void dacmod2()
{
		if(temperature<20)
		{
				dac_value=51;
			  DAC(dac_value);
		}
		if(temperature>20&&temperature<40)
		{
				dac_value=51+((char)(temperature)-20)*8;
			  DAC(dac_value);
		}
		if(temperature>40)
		{
				dac_value=204;
			  DAC(dac_value);
		}
}

void led_show()
{
	  unsigned char b;
	
		if(dac_output_mod==0){b|=0x01;}
		else{b&=0xfe;}
		if(show_mod==1){b|=0x02;}
		else{b&=0xfd;}
		if(show_mod==2){b|=0x04;}
		else{b&=0xfb;}
		if(show_mod==3){b|=0x08;}
		else{b&=0xf7;}
		
		P0=~b;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}