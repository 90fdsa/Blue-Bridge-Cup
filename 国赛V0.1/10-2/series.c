#include "series.h"

void Uart_Send(unsigned char *Uart_String)//串口发送字符函数
{
	while(*Uart_String != '\0')
	{
		SBUF = *Uart_String;
		while(TI == 0);//等待发送完成
		TI = 0;
		Uart_String++;
	}
}