/*******************************************************************************  
* �ļ����ƣ��������ʵ��
* ʵ��Ŀ�ģ�1.���վ������ɨ��Ļ���ԭ��
*           2.���հ���ɨ�輰���������Ļ���ԭ��
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪKBD��J2����Ϊ1-3��2-4
* ʵ�����󣺰��°������������ʾ��0~15֮��仯
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ����˵����1.ת�Ӱ���ʹ��P4.4��P4.2����P3.6 P3.7
* 			2.����״̬�����а���ɨ�裬ϵͳʵʱ�Ժã�
* 			��ÿ10msɨ����Է�ֹ������
* ���ڰ汾��2019-6-24/V2.0
*******************************************************************************/
#include "STC15F2K60S2.H"  //�ô�ͷ�ļ�ֱ�Ӵ���REG52.H�����������ͷ�ļ��Ա�
#include "absacc.h"
#include "i2c.h"
#include "intrins.h"
#include "i2c_eeprom.h"
                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0xbf,0xc6};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
char up=0;
unsigned int duty=1000;
char k=0;char l=0;
unsigned char lun=1;char ci=10;
char end=0;char start=1;
char mod=0;char step=0;
unsigned char interval=10;
char show=0;
unsigned char adc_value;
char liang;
/******��������***********************/
void read_keyboard(void);
void display();
void type1(char i);
void type2(char i);
void type3(char i);
void type4(char i);
void read_keyboard(void);
void key_proc();
void blink(unsigned char a);
void close_display();
void display_liang();
void adc_pan();
void delay();
/************************************/

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
	TR1 = 0;
	TR1 = 1;
	ET1 = 1;
	
//	lun=read_eeprom(0x00);
//	delay();
//	interval=read_eeprom(0x10);
//	delay();
	
	
	EA = 1; 
	
	P2 = ((P2 &0x1f)|0x80);
	P0=0xff;
	P2 &= 0x1f;

	
	
	init_pcf8591();  //PCF8591��ʼ��
	
	
  while(1)
  {
		read_keyboard();
		key_proc();
		adc_pan();
  }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //���ڿ��ƵƵ�����
{
	  static unsigned int time;
		static unsigned int kong=0;
		static unsigned int jie=0;
		static unsigned char adc_ce=0;
		if(++adc_ce==50)
		{
			adc_ce=0;
			adc_value = adc_pcf8591();
		}
		if(mod==0)
		{
			if(show==1)
			{display_liang();}
			else
			{close_display();}
			if(end==0&&start==1)
			{
				time++;
				if(time==400)
				{
					time=0;
					k=k+1;
					if(k==ci)
					{k=0;}
				}
			
			}
			else if(end==1)
			{	
				k=0;
				kong++;
				if(kong==interval*100)
				{
					end=0;
					lun++;
					if(lun==2)
					{ci=6;}
					else if(lun==4)
					{ci=10;lun=0;}
					kong=0;
				}
			}
		}
		else if(mod==1)
		{
			if(++jie==800)
			{
				jie=0;
				blink(step);
			}
		}
		display();
}

void isr_timer_1(void)  interrupt 3  //���ڵ�������
{
    static unsigned int intr;
		intr++;
		if(intr <= duty)
		{
				P2 = ((P2&0x1f)|0x80);
				if(end==0)
				{
					switch(lun)
					{
						case 0:type1(k);break;
						case 1:type2(k);break;
						case 2:type3(k);break;
						case 3:type4(k);break;
					}
				}
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
		case 8:
		{
			if(start==1)
			{start=0;}
			else
			{start++;}
			key_value=0xff;
			break;
		}
		case 12:
		{
			mod=1;
			step++;
			if(step==3)
			{
				step=0;
				mod=0;
//				write_eeprom(0x00,lun);
//				delay();
//				write_eeprom(0x10,interval);
//				delay();
			}
			key_value=0xff;
			break;
		}
		case 9:
		{
			if(mod==1)
			{
				if(step==1)
				{
					lun++;
					if(lun==4)
					{lun=0;}

				}
				else if(step==2)
				{
					interval+=1;
					if(interval==13)
					{interval=4;}
				}
			}

			key_value=0xff;
			break;
		}
		case 13:
		{
			if(mod==1)
			{
				if(step==1)
				{
					lun--;
					if(lun<0)
					{lun=3;}
				}
				else if(step==2)
				{
					interval-=1;
					if(interval<4)
					{interval=12;}
				}
			}
			if(mod==0)
			{
				if(show==1)
				{show=0;}
				else
				{show++;}
			}
			key_value=0xff;
			break;
		}
	}
}

//��ʾ����
void display(void)
{   
	static unsigned char dspcom = 0;
	P0 = 0xff;
	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;

	P0 = 1<<dspcom;
	P2 = ((P2&0x1f)|0xC0); 
	P2 &= 0x1f;

	P0 = tab[dspbuf[dspcom]];	
  	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  
    if(++dspcom == 8){
        dspcom = 0;
    }    
}

void type1(char i)
{
	switch(i)
	{
		case 0:P0=0xff;break;
		case 1:P0=0xfe;break;
		case 2:P0=0xfc;break;
		case 3:P0=0xf8;break;
		case 4:P0=0xf0;break;
		case 5:P0=0xe0;break;
		case 6:P0=0xc0;break;
		case 7:P0=0x80;break;
		case 8:P0=0x00;break;
		case 9:end=1;break;
	}	
}

void type2(char i)
{
	switch(i)
	{
		case 0:P0=0xff;break;
		case 1:P0=0x7f;break;
		case 2:P0=0x3f;break;
		case 3:P0=0x1f;break;
		case 4:P0=0x0f;break;
		case 5:P0=0x07;break;
		case 6:P0=0x03;break;
		case 7:P0=0x01;break;
		case 8:P0=0x00;break;
		case 9:end=1;break;
	}	
}

void type3(char i)
{
	switch(i)
	{
		case 0:P0=0xff;break;
		case 1:P0=0x7e;break;
		case 2:P0=0x3c;break;
		case 3:P0=0x18;break;
		case 4:P0=0x00;break;
		case 5:end=1;break;
	}	
}

void type4(char i)
{
	switch(i)
	{
		case 0:P0=0xff;break;
		case 1:P0=0xe7;break;
		case 2:P0=0xc3;break;
		case 3:P0=0x81;break;
		case 4:P0=0x00;break;
		case 5:end=1;break;
	}	
}

void blink(unsigned char a)
{
	static unsigned char count=0;
	if(mod==1)
	{
		if(a==1)
		{
			if(count==0)
			{
				dspbuf[0] = 11;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = (interval*100)/1000;dspbuf[5] = (interval*100)%1000/100;
				dspbuf[6] = (interval*100)%100/10;dspbuf[7] = (interval*100)%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = 11;dspbuf[1] =lun ;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = (interval*100)/1000;dspbuf[5] = (interval*100)%1000/100;
				dspbuf[6] = (interval*100)%100/10;dspbuf[7] = (interval*100)%10;
				count=0;
			}
		}
		else if(a==2)
		{
			if(count==0)
			{
				dspbuf[0] = 11;dspbuf[1] =lun ;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = 10;dspbuf[5] = 10;dspbuf[6] = 10;dspbuf[7] = 10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = 11;dspbuf[1] =lun ;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = (interval*100)/1000;dspbuf[5] = (interval*100)%1000/100;
				dspbuf[6] = (interval*100)%100/10;dspbuf[7] = (interval*100)%10;
				count=0;
			}
		}
	}	
}

void close_display()
{
	dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 10;dspbuf[3] = 10;
	dspbuf[4] = 10;dspbuf[5] = 10;dspbuf[6] = 10;dspbuf[7] = 10;
}
void display_liang()
{
	dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 10;dspbuf[3] = 10;
	dspbuf[4] = 10;dspbuf[5] = 10;dspbuf[6] = 11;dspbuf[7] = liang;
}

void adc_pan()
{
	if(adc_value>=0&&adc_value<=63)
	{duty=25;liang=1;}
	else if(adc_value>=64&&adc_value<=127)
	{duty=100;liang=2;}
	else if(adc_value>=128&&adc_value<=191)
	{duty=500;liang=3;}
	else
	{duty=1000;liang=4;}
}

void delay()		//10ms @11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}