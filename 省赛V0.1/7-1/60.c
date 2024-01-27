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
#include "onewire.h"

sfr AUXR = 0x8E;

sbit S7  = P3^0;
sbit S6  = P3^1;
sbit S5  = P3^2;
sbit S4  = P3^3;
 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbF,0xc6};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//��ʾ������

//��������
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void button(unsigned char a);
void delay();
void key_proc(unsigned char key);
unsigned char read_key(void);
void how_to_display();
/**********************************/

unsigned char second=0;
unsigned char ordre=1;
unsigned char temperature=0;
unsigned int duty=0;
unsigned char shu=0;
char mod=1;

//������
void main(void)
{ 
    unsigned char temp;
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
		//init_ds18b20();
    while(1)
    {
			//��ʾ���ݸ��� 
			
			button(temp);
			
//			dspbuf[1]=mod;
//			dspbuf[2]=temp;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1,���ڶ���
{
    static unsigned int intr=0;
		intr++;
		if(intr == 1000)  //1sִ��һ��
		{
				if(second>0)
				{
						second--;
				}
				else
				{
					duty=0;
				}
				intr = 0;
				temperature=rd_temperature();
    }
		display();
}

void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 3�����ڲ���pwm
{
    static unsigned int intr;
		intr++;
		if(intr <= duty)  //200us�ߵ�ƽ
		{
				shu=1;
				//intr = 0;
    }
		if(intr>duty&&intr<1000)
		{
				shu=0;
		}
		if(intr==1000)
		{
			 intr=0;
		}
		how_to_display();
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

//��ʱ����: 10ms@11.0592MHz
void delay(void)
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
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

void how_to_display()
{
		if(mod==1)
		{
			dspbuf[0]=11;
			dspbuf[1]=ordre;
			dspbuf[2]=11;
			//dspbuf[4]=0;
			dspbuf[5]=second/100;
			dspbuf[6]=second%100/10;      
			dspbuf[7]=second%100%10;
		}
		if(mod==0)
		{
			dspbuf[0]=11;
			dspbuf[1]=ordre;
			dspbuf[2]=11;
			dspbuf[4]=0;
			dspbuf[5]=temperature/10;      
			dspbuf[6]=temperature%10;
			dspbuf[7]=12;
		}
		 
}

//����ɨ�躯��
unsigned char read_key(void)
{
    unsigned char temp;
    unsigned char key_value = 0xff;
    
    temp = P3&0x0f;
    switch(temp)
    {
        case 0x0e:
            key_value = 1;  //S7
            break;    
        case 0x0d:
            key_value = 2;  //S6
            break;    
        case 0x0b:
            key_value = 3;  //S5
            break;    
        case 0x07:
            key_value = 4;  //S4
            break;    
    }
    return key_value;
}
//������������
void button(unsigned char a)
{
		a = read_key();
    if(a != 0xff)
    {
        delay();  //��������
        a = read_key();
        if(a != 0xff)
        {
            //���밴��������
            key_proc(a);
            while(P3 != 0xff);  //�ȴ������ͷ�                        
        }
    }
}

//����������
void key_proc(unsigned char key)
{
		static unsigned char time=0;//����ʱ��ı���
		static unsigned char order=1;//����ռ�ձȵı���
    switch(key)
    {
      case 1://����ģʽ
				P2 = (P2&0x1f|0x80);
				switch(order)
				{
					case 1:duty=200;P0=0XFE;break;
					case 2:duty=500;P0=0XFD;break;
					case 3:duty=700;P0=0XFB;break;
				}
				ordre=order;
				order++;
				if(order==4)
				{order=1;}
				P2 &= 0x1f;
        break;
			case 2://����ʱ��
				P2 = (P2&0x1f|0x80);
				switch(time)
				{
					case 0:second=0;break;
					case 1:second=60;break;
					case 2:second=120;break;
				}
				time++;
				if(time>2)
				{time=0;}
				P2 &= 0x1f;
				break;
      case 3:
				P2 = (P2&0x1f|0x80);
				second=0;
				duty=0;
				P2 &= 0x1f;
        break;
      case 4:
				P2 = (P2&0x1f|0x80);
				if(mod==1)
				{mod=0;}
				else
				{mod++;}
				P2 &= 0x1f;
        break;
    }    
}

