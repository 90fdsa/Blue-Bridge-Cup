#include "STC15F2K60S2.H"  //�ô�ͷ�ļ�ֱ�Ӵ���REG52.H�����������ͷ�ļ��Ա�
#include "absacc.h"

unsigned char key_value;
unsigned char i=0;
unsigned char ok=0;
unsigned char Led_Flag;

void read_keyboard(void);
//void key_proc();
void led_show();

void cls_buzz(void)	//�رշ�����
{
	P2 = (P2&0x1F|0xA0);
	P0 = 0x00;
	P2 &= 0x1F;
}

void cls_led(void)//�ر�����LED
{
    P2 = ((P2&0x1f)|0x80);
	P0 = 0xff;  
	P2 &= 0x1f;
}

void Timer0Init(void)		//12΢��@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0; 	//���ö�ʱ��ģʽ
	TMOD |= 0x04;
	TL0 = 0xff;		//���ö�ʱ��ֵ
	TH0 = 0xff;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;    //�򿪶�ʱ���ж�
}

void Timer1Init(void)		//12΢��@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF0��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;    //�򿪶�ʱ���ж�
}

void main()
{
	  cls_buzz();
	  cls_led();
	  Timer1Init();
		EA=1;
	  while(1)
		{
				read_keyboard();
			  //key_proc(); 
			  led_show();
		}
}

void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 3
{     
		static unsigned int a;
	  if(key_value==12)
		{
				a++;
			  if(a>=2000)
				{
						ok=~ok;
				}
		}
		else
		{
				a=0;
		}
}
	
void read_keyboard(void)
{
    static unsigned char hang;
	static unsigned char key_state=0;	
	switch(key_state)
	{
		case 0:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //�а�������
			key_state=1;	
		}break;
		case 1:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //�а�������
			{
				if(P30 == 0)hang = 1;
				if(P31 == 0)hang = 2;
				if(P32 == 0)hang = 3;
				if(P33 == 0)hang = 4;//ȷ����	    
				switch(hang){
					case 1:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=0;key_state=2;}
						else if(P42 == 0) {key_value=1;key_state=2;}
						else if(P35 == 0) {key_value=2;key_state=2;}
						else if(P34 == 0) {key_value=3;key_state=2;}
					}break;
					case 2:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=4;key_state=2;}
						else if(P42 == 0) {key_value=5;key_state=2;}
						else if(P35 == 0) {key_value=6;key_state=2;}
						else if(P34 == 0) {key_value=7;key_state=2;}
					}break;
					case 3:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=8;key_state=2;}
						else if(P42 == 0) {key_value=9;key_state=2;}
						else if(P35 == 0) {key_value=10;key_state=2;}
						else if(P34 == 0) {key_value=11;key_state=2;}
					}break;
					case 4:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=12;key_state=2;}
						else if(P42 == 0) {key_value=13;key_state=2;}
						else if(P35 == 0) {key_value=14;key_state=2;}
						else if(P34 == 0) {key_value=15;key_state=2;}
					}break;
				}	
			}
			else
			{
				key_state=0;	
			}  	   
		}break;
		case 2:     
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 == 0x0f) //�����ſ�
			key_state=0;	
		}break;
						
    } 
	
}

//void key_proc()
//{
//	
//	switch(key_value)
//	{
//		case 12:
//		{
//			i=1;
//			key_value=0xff;
//			break;
//		}
//	}
//}

void led_show()
{
		//ledall
		if(ok!=0)
		{
			Led_Flag |= 0xff;
	  }
	  else
	  {
		  Led_Flag &= 0x00;
	  }
		P2 = ((P2&0x1f)|0x80);
    P0 = ~Led_Flag;
		P2 &= 0x1f;
}