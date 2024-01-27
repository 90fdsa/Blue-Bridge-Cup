#include "uart.h"


void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR &= 0xFB;		//��ʱ��2ʱ��ΪFosc/12,��12T
	T2L = 0xE6;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
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

