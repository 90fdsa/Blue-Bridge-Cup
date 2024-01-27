#include "init.h"

void Timer0Init(void)		//1000΢��@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD = 0xf4;		//���ö�ʱ��ģʽ
	TL0 = 0xff;		//���ö�ʱ��ֵ
	TH0 = 0xFf;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF1��־
	TR0 = 1;		//��ʱ��1��ʼ��ʱ
	ET0 = 1;
}

void Timer1Init(void)		//1000΢��@12.000MHz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x18;		//���ö�ʱ��ֵ
	TH1 = 0xFC;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
}

void PCA_init()
{
		CMOD&=0xf7;
	  CCON=0x00;
}

void Timer2Init(void)		//1΢��@12.000MHz
{
	AUXR &= 0xFB;		//��ʱ��ʱ��12Tģʽ
	T2L = 0xFF;		//���ö�ʱ��ֵ
	T2H = 0xFF;		//���ö�ʱ��ֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	IE2 |= 0x04;    //����ʱ��2�ж�
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