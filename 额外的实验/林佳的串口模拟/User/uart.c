#include "uart.h"


void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xE6;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES=1;
	EA=1;
}

void sendByte(unsigned char dat)
{
	SBUF=dat;
	while(!TI);
	TI=0;

}

void sendString(unsigned char *uart_T_sting)
{
	while(*uart_T_sting!='\0')
	{
	sendByte(*uart_T_sting++);
	}

	
	
}

