/*******************************************************************************  
* �ļ����ƣ�DS18B20�¶ȴ�����ʵ��
* ʵ��Ŀ�ģ�1.���յ�����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ�ⵥ����ʱ��ĳ�����Ʒ���
*           3.����DS18B20�¶ȴ������Ĳ�������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �����������ִ����¶ȴ��������������ʾ�б仯
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ע�������������Ϊ1���϶�
* ���ڰ汾��2019-6-24/V2.0
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"

sfr AUXR = 0x8E;
sbit shu=P3^4;
 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//��ʾ������

void display(void);

unsigned char show=0;
unsigned char wei=0;
unsigned char weishu=0;
unsigned char wei_flag=0;

void cls_led()
{
	P2 = ((P2&0x1f)|0x80); 
	P0 = 0xFF;
	P2 &= 0x1f;
}

void cls_buzz()
{
	P2 = ((P2&0x1f)|0xA0); 
	P0 = 0x00;
	P2 &= 0x1f;
}


void Timer0Init(void)		//1΢��@12.000MHz
{
	//AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD = 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xFF;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
}

void Timer1Init(void)		//1000΢��@12.000MHz
{
	//AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x18;		//���ö�ʱ��ֵ
	TH1 = 0xFC;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
}

//������
void main(void)
{ 
		cls_buzz();
		cls_led();   
	  Timer0Init();
	  Timer1Init();
	  EA=1;
    while(1)
    {
			if(wei_flag==1)
			{
					wei_flag=0;
				  weishu=1<<wei;
				  if(++wei==8){wei=0;}
			}
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
    static unsigned int intr;
		intr++;
		if(intr <= 100)  //200us�ߵ�ƽ
		{
				P0=weishu;
				P2 = ((P2&0x1f)|0x80); 
				P2 &= 0x1f;
    }
		if(intr>100&&intr<1000)
		{
				P0=0xff;
				P2 = ((P2&0x1f)|0x80); 
				P2 &= 0x1f;
		}
		if(intr==1000)
		{
			 intr=0;
		}
}

void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{
		static unsigned int a;
	  if(++a==1000)
		{
				a=0;
			  wei_flag=1;
		}
}

//��ʾ����
void display(void)
{   
	static unsigned char dspcom = 0;
	P2 = ((P2&0x1f)|0xE0);   //��ѡ573
	P0 = 0xff;               //��������
	P2 &= 0x1f; 

	P0 = 1<<dspcom;	         //ѭ��λѡ
	P2 = ((P2&0x1f)|0xC0);   //λѡ573
	P2 &= 0x1f;
	
	P0 = tab[dspbuf[dspcom]];//��ʾ����
  P2 = ((P2&0x1f)|0xE0);   //��ѡ573
	P2 &= 0x1f;               
	
    if(++dspcom == 8){
        dspcom = 0;
    }    
}
