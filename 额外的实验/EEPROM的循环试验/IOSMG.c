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

//#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"
#include "i2c.h"
#include "STC15F2K60S2.H"

//sfr AUXR = 0x8E;

 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//��ʾ������

unsigned char start=1;
unsigned char key_value=0;
unsigned char up;

//��������
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void delay();
void read_keyboard(void);
void key_proc();
/**********************************/

//������
void main(void)
{ 
    unsigned int a;
		cls_buzz();
		cls_led();   
//		AUXR |= 0xC0;
//		TMOD &= 0xFF;
//		TL0 = 0xCD;	//1ms,1Tmoshi
//		TH0 = 0xD4;	
//		TF0 = 0;	
//		TR0 = 1;
//		ET0 = 1; 
	
		
		
	  
		EA=0;
	  for(a=0;a<255;a++)
	  {
				write_eeprom(a,0);
			  delay();	//��ʱ10ms
		}
		  //��AT24C02��ַ0x00��д������
		
	
//		start++;
//	
//		start = read_eeprom(0x00);  //��AT24C02��ַ0x00�ж�ȡ����
//	  delay();	//��ʱ10ms
		
		EA=1;
	
    while(1)
    {
			//��ʾ���ݸ��� 
			
//			read_keyboard();
//			key_proc();
//			dspbuf[0]=stop/100;
//			dspbuf[1]=stop%100/10;      
//			dspbuf[2]=stop%100%10;
			dspbuf[2]=1;
			dspbuf[3]=1;      
			dspbuf[4]=4;
			dspbuf[5]=5;
			dspbuf[6]=1;      
			dspbuf[7]=4; 
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
//		static unsigned int a;
//		if(++a==1000)
//		{
//				a=0;
//				start++;
//				write_eeprom(0x00,start);  //��AT24C02��ַ0x00��д������
//				delay();	//��ʱ10ms
//		}
		display();
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

void delay()//10ms
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
