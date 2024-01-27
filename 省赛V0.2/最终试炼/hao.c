#include "stc15f2k60s2.h"
#include "iic.h"
#include <stdio.h>
#include <string.h>

code unsigned char weima[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,
                            0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};
unsigned char buffer[]={10,10,10,10,10,10,10,10};
unsigned char rxstring[18];
unsigned char txstring[18];


unsigned char key_value;
unsigned char weixuan;
unsigned char adc_value;
float voltage;
float db;
unsigned char n=0;//串口接收数据变量
unsigned char uart_flag;

void display();
unsigned char read_key();
void send_byte(unsigned char date);
void send_string(unsigned char *dat);


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

void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xE6;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES = 1;
}


void main()
{
		close_buzz();
	  close_led();
	  Timer0Init();
	  pcf8591_init();
	  UartInit();
	  EA=1;
	  while(1)
		{
			  buffer[5]=((int)(db*10))/100;
				buffer[6]=((int)(db*10))%100/10+11;
			  buffer[7]=((int)(db*10))%10;
			  if((uart_flag==1)&&(strcmp(rxstring,"fuckyou\r")==0))
				{
						sprintf(txstring,"Noises:%3.1fdb\r\n",db);
						send_string(txstring);
						uart_flag=0;
				}
		}
}

void timer0_proc() interrupt 1
{
	  static unsigned char a;
	  static unsigned char ad;
		display();
	  if(++a==10)
		{
				a=0;
			  key_value=read_key();
		}
		if(++ad==50)
		{
				ad=0;
			  adc_value=pcf8591_adc();
			  voltage=adc_value*0.0196;
			  db=voltage*18;
		}
}
	
void uart_proc() interrupt 4
{
		unsigned char temp;
	  if(RI)
		{
				temp=SBUF;
			  if(temp=='\n')
				{
						rxstring[n]='\0';
						uart_flag=1;
						n=0;
				}
			  else
				{
						rxstring[n++]=temp;
				}
				RI=0;
		}
}

unsigned char read_key()
{ 
	  unsigned char key;
	  unsigned int ji;
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
		ji=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  ji=(ji<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  ji=(ji<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  ji=(ji<<4)|(P3&0x0f);
	
	  switch(~ji)
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
	  P2=P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  P0=1<<weixuan;
	  P2=(P2&0x1f)|0xc0;
	  P2&=0x1f;
	
	  P0=weima[buffer[weixuan]];
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  if(++weixuan==8){weixuan=0;}
}

void send_byte(unsigned char date)
{
		SBUF=date;
	  while(!TI);
	  TI=0;
}

void send_string(unsigned char *dat)
{
		while(*dat!='\0')
		{
				send_byte(*dat++);
		}
}