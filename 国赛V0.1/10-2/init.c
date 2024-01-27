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
     //CMOD|=0x08;//���ö�ʱ��ʱ��Ϊϵͳʱ��
	   CMOD&=0XF7;
     CCON=0x00;//�����λcf�������־λ������cr����ͣλ������ֹͣ����
}

void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR &= 0xFB;		//��ʱ��2ʱ��ΪFosc/12,��12T
	T2L = 0xCC;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	ES=1;
}
