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

#include "STC15F2K60S2.H"
//#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"
#include "i2c.h"
#include "intrins.h"

//sfr AUXR = 0x8E;

 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//��ʾ������
unsigned char key_value;
unsigned char start;

//��������
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void delay();
void read_keyboard(void);
void key_proc();
void delay_200ms();		//@11.0592MHz
/**********************************/

//������
void main(void)
{ 

		cls_buzz();
		cls_led();   
		AUXR |= 0x80;
		TMOD &= 0xF0;
		TL0 = 0xCD;	//1ms,1Tmoshi
		TH0 = 0xD4;	
		TF0 = 0;	
		TR0 = 1;
		ET0 = 1; 
		
	  start = read_eeprom(0x00);  //��AT24C02��ַ0x00�ж�ȡ����
    start++;
	  delay();	//��ʱ10ms
    write_eeprom(0x00,start);  //��AT24C02��ַ0x00��д������
	  delay();
	
		EA=1;

    while(1)
    {
			//��ʾ���ݸ��� 
			//dspbuf[0]=up;	
//			read_keyboard();
//			key_proc();
//			delay_200ms();delay_200ms();delay_200ms();delay_200ms();delay_200ms();
//			write_eeprom(0x00,start);  //��AT24C02��ַ0x00��д������
//			delay();
			dspbuf[5]=start/100;
			dspbuf[6]=start%100/10;      
			dspbuf[7]=start%100%10; 
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{
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

//��ȡ������̼�ֵ:ת�Ӱ���ʹ��P42��P44����8051����
//˳���е�P36��P37����
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

void key_proc()
{
		switch(key_value)
		{
			case 0:
			{
//				start++;
//				write_eeprom(0x00,start);
//				delay();
				key_value=0xff;
				break;
			}
		}
}

void delay_200ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}