#include "series.h"

void Uart_Send(unsigned char *Uart_String)//���ڷ����ַ�����
{
	while(*Uart_String != '\0')
	{
		SBUF = *Uart_String;
		while(TI == 0);//�ȴ��������
		TI = 0;
		Uart_String++;
	}
}