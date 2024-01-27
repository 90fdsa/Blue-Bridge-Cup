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
#include "i2c.h"

sfr AUXR = 0x8E;
unsigned char adc_value;
unsigned int duty=0;
char up=0;
char k=0;
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//��ʾ������

//��������
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void type1(char i);
/**********************************/

//������
void main(void)
{ 
		cls_buzz();
		cls_led();   
		AUXR |= 0xC0;
		TMOD &= 0xFF;
		TL0 = 0xCD;	//1ms,1Tmoshi
		TH0 = 0xD4;	
		TF0 = 0;	
		TR0 = 1;
		ET0 = 1;
		TL1=0XF5;
		TH1=0XFF;
		TF1 = 0;	
		TR1 = 1;
		ET1 = 1;
		EA = 1;
		init_pcf8591();  //PCF8591��ʼ��   
    while(1)
    {
			//��ʾ���ݸ��� 
			//dspbuf[0]=up;
			dspbuf[0]=k;
			dspbuf[5]=adc_value/100;
			dspbuf[6]=adc_value%100/10;      
			dspbuf[7]=adc_value%100%10; 
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
		static unsigned char eihei;
	  static unsigned int time;
		if(++eihei==2)
		{
				eihei=0;
				if(up==0)
				{duty++;}
				else
				{duty--;}
				if(duty==500)
				{up=1;}
				else if(duty==0)
				{up=0;}
		}
		if(++intr == 50)  //1msִ��һ��
		{
				intr = 0;
				adc_value = adc_pcf8591();
    }
		if(++time==1000)
		{
			time=0;
			k=k+1;
			if(k==8)
			{k=0;}
		}
		display();
}

void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{
    static unsigned int intr;
		intr++;
		if(intr <= duty)  //200us�ߵ�ƽ
		{
				P2 = ((P2&0x1f)|0x80);
				type1(k);
				P2 &= 0x1f;
    }
		if(intr>duty&&intr<1000)
		{
				P2 = ((P2&0x1f)|0x80);
				P0 = 0xff;  //LED��
				P2 &= 0x1f;
		}
		if(intr==1000)
		{
			 intr=0;
		}
}

void cls_buzz()
{
	P2 = ((P2&0x1f)|0xA0); 
	P0 = 0x00;
	P2 &= 0x1f;
}
void cls_led()
{
	P2 = ((P2&0x1f)|0x80); 
	P0 = 0xFF;
	P2 &= 0x1f;
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

void type1(char i)
{
	switch(i)
	{
		case 0:P0=0xfe;break;
		case 1:P0=0xfd;break;
		case 2:P0=0xfb;break;
		case 3:P0=0xf7;break;
		case 4:P0=0xef;break;
		case 5:P0=0xdf;break;
		case 6:P0=0xbf;break;
		case 7:P0=0x7f;break;
	}	
}