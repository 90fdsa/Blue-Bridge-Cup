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
                           //  0    1    2    3    4    5    6    7    8    9  Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
														 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0x8e};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

sbit L1 = P0^0;
sbit L2 = P0^1;
sbit L3 = P0^2;
sbit L4 = P0^3;
sbit L5 = P0^4;

bit key_flag = 0;//����ɨ���־λ 
unsigned char key_value=0xFF;//����ֵ
unsigned char small_mod=0;
unsigned char adc_value;
unsigned char dac_value=104;
unsigned char high,low;
unsigned char dac_mod=0;
unsigned int frequency=0;
unsigned int frequency_stand=0;
unsigned char key_led=1;
unsigned char led_c=0,smg_c=0;

/******��������***********************/
void read_keyboard(void);
void display();
void key_proc();
void fen();
void deal_adc();
void deal_F();
void LED(unsigned char a);
void how_to_display();
void smg_off();
void LED_off();
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
	AUXR |= 0xC0;	//1Tģʽ��IAP15F2K61S2��Ƭ�����⹦�ܼĴ���	
	TMOD = 0x04;
	TL0 = 0;	
	TH0 = 0;
	TR0 = 1;
	
	TL1 = 0xCD;	
	TH1 = 0xD4;		
	TR1 = 1;
	ET1 = 1;
	
	EA = 1;  
	init_pcf8591();
	
//	P2 = ((P2&0x1f)|0x80);
//	P0 = 0xff;  //LED��
//	P2 &= 0x1f;
	
  while(1)
  {
			read_keyboard();
			key_proc();
			fen();
	}
}       


//��ʱ���жϷ�����
void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr=0;
	static unsigned int pluse=0;
	display(); //1msִ��һ��
	dac_pcf8591(dac_value);
	if(++intr==100)
	{
		intr=0;
		adc_value=adc_pcf8591();
	}
	if(++pluse==1000)
	{
		pluse=0;
		TR0=0;
		high=TH0;low=TL0;
		TH0=0;TL0=0;
		TR0=1;
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
			dac_mod=~dac_mod;
			if(dac_mod==0)
			{dac_value=104;}
			else
			{dac_value=adc_value;}
			key_value=0xff;
			break;
		}
		case 12:
		{
			small_mod=~small_mod;
			key_value=0xff;
			break;
		}
		case 9:
		{
			led_c=~led_c;
			key_value=0xff;
			break;
		}
		case 13:
		{
			smg_c=~smg_c;
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

void fen()
{
		if(small_mod==0)
		{
				deal_adc();
			  if(led_c==0)
				{LED(key_led);}
				else
				{LED_off();}
				
		}
		else
		{		
				deal_F();
				if(led_c==0)
				{LED(key_led);}
				else
				{LED_off();}
		}
}

void deal_adc()//д�������鷳
{
	  float voltage;
	  int voltage100;
		unsigned char ge,dianyi,dianer;
		voltage=adc_value*0.0196;
		voltage100=voltage*100;
		ge=voltage100/100;
		dianyi=voltage100%100/10;
		dianer=voltage100%10;
	  
		key_led=1;
	  if(dac_mod!=0)
		{key_led+=2;}
		if((voltage>=1.5&&voltage<2.5)||(voltage>=3.5))
		{key_led++;}
		
		if(smg_c==0)
		{
				dspbuf[0]=22;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	      dspbuf[5]=(ge+11);dspbuf[6]=dianyi;dspbuf[7]=dianer;
		}
		else
		{smg_off();}
		
}

void deal_F()//д�������鷳
{
	  unsigned int high_high,high_low,low_high,low_low;
 
		high_high=(high&0xf0)>>4;
		high_low=high&0x0f;
	  low_high=(low&0xf0)>>4;
		low_low=(low&0x0f);
		frequency_stand=(high_high*16*16*16+high_low*16*16+low_high*16+low_low);
	  
	
	 	key_led=5;
		if((frequency_stand>=1000&&frequency_stand<5000)||(frequency_stand>=10000))
		{key_led++;}
	
		if(smg_c==0)
		{
			  dspbuf[0]=23;
		    dspbuf[1]=10;
		    dspbuf[2]=10;
				(frequency_stand>=10000)?(dspbuf[3]=frequency_stand/10000):(dspbuf[3]=10);
				(frequency_stand>=1000)?(dspbuf[4]=frequency_stand%10000/1000):(dspbuf[4]=10);
				(frequency_stand>=100)?(dspbuf[5]=frequency_stand%1000/100):(dspbuf[5]=10);
				dspbuf[6]=frequency_stand%100/10;
				dspbuf[7]=frequency_stand%10;
		}
		else
		{smg_off();}
	
		
}


void LED(unsigned char a)
{
		P2 = ((P2&0x1f)|0x80);
	  switch(a)
		{
			case 1:P0 = 0xfe;break;
			case 2:P0 = 0xfa;break;
			case 3:P0 = 0xee;break;
			case 4:P0 = 0xea;break;
			case 5:P0 = 0xfd;break;
			case 6:P0 = 0xf3;break;
		}
		P2 &= 0x1f;
}

void LED_off()
{
		P2 = ((P2&0x1f)|0x80);
	  P0 = 0xff;
		P2 &= 0x1f;
}

void smg_off()
{
		dspbuf[0]=10;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;dspbuf[5]=10;dspbuf[6]=10;dspbuf[7]=10;
}