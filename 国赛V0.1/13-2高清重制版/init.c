#include "init.h"

void Timer0Init(void)		//1000微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD = 0xf4;		//设置定时器模式
	TL0 = 0xff;		//设置定时初值
	TH0 = 0xFf;		//设置定时初值
	TF0 = 0;		//清除TF1标志
	TR0 = 1;		//定时器1开始计时
	ET0 = 1;
}

void Timer1Init(void)		//1000微秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
}

void PCA_init()
{
		CMOD&=0xf7;
	  CCON=0x00;
}

void Timer2Init(void)		//1微秒@12.000MHz
{
	AUXR &= 0xFB;		//定时器时钟12T模式
	T2L = 0xFF;		//设置定时初值
	T2H = 0xFF;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;    //开定时器2中断
}


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