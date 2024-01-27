#include "init.h"


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

void Timer2Init(void)		//1000微秒@12.000MHz
{
	AUXR &= 0xFB;		//定时器时钟12T模式
	T2L = 0x18;		//设置定时初值
	T2H = 0xFC;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;    //开定时器2中断
}

