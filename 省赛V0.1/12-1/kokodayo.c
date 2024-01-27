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
#include "onewire.h"
#include "i2c.h"

                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc6,0x8C,0x88};
                           //  C    P    A
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
float true_temperature;
unsigned char temperature_standard=25;
unsigned char mod=1;
unsigned char dac_out=102;
unsigned char dac_mod=0;
unsigned char led_mod;
/******��������***********************/
void read_keyboard(void);
void display();
void key_proc();
void temperature_deal();
void temperature_show_standard();
void show_dac();
void fen();
void smg_off();
void dac_pan();
void led_pan();
void led_show();
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
	AUXR |= 0x80;	//1Tģʽ��IAP15F2K61S2��Ƭ�����⹦�ܼĴ���	
	TMOD &= 0xF0;
	TL0 = 0xCD;	
	TH0 = 0xD4;	
	TF0 = 0;	
	TR0 = 1;
	ET0 = 1;
	EA = 1;
  init_pcf8591();
		
    while(1)
    {
			 true_temperature=rd_temperature_f();
			 dac_pcf8591(dac_out);
       key_proc();
			 fen();
			 led_pan();
			 led_show();
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;
	static unsigned char dac_count=0;
	display(); //1msִ��һ��

	if(++intr == 10) 
	{
     intr = 0;
		 read_keyboard(); 
  }
	if(++dac_count==150)
	{
			dac_count=0;
		  dac_pan();
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
		case 12://S4
		{
			mod++;
			if(mod==4){mod=1;}
			key_value=0xff;
			break;
		}
		case 8://S5
		{
			dac_mod=~dac_mod;
			key_value=0xff;
			break;
		}
		case 13://S8
		{
			if(mod==2){temperature_standard-=1;}
			key_value=0xff;
			break;
		}
		case 9://S9
		{
			if(mod==2){temperature_standard+=1;}
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

void smg_off()//�ر�����ܣ���ʱ��
{
		dspbuf[0]=10;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;dspbuf[5]=10;dspbuf[6]=10;dspbuf[7]=10;
}

void temperature_deal()
{
		unsigned int temperature100;
	  temperature100=true_temperature*100;
	  dspbuf[0]=22;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=temperature100/1000;
	  dspbuf[5]=(temperature100%1000/100)+11;
	  dspbuf[6]=temperature100%100/10;
	  dspbuf[7]=temperature100%10;
	  led_mod=1;
}

void temperature_show_standard()
{
//		unsigned int temperature100;
//	  temperature100=temperature_standard*100;
	  dspbuf[0]=23;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;
	  dspbuf[5]=10;
	  dspbuf[6]=temperature_standard/10;
	  dspbuf[7]=temperature_standard%10;
	  led_mod=3;
}

void show_dac()
{
		float voltage;
	  unsigned int voltage100;
	  voltage=dac_out*0.0196;
	  voltage100=voltage*100;
	  dspbuf[0]=24;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;
	  dspbuf[5]=(voltage100/100)+11;
	  dspbuf[6]=voltage100%100/10;
	  dspbuf[7]=voltage100%10;
	  led_mod=5;
}

void fen()
{
		switch(mod)
		{
			case 1:temperature_deal();break;
			case 2:temperature_show_standard();break;
			case 3:show_dac();break;
		}
}

void dac_pan()
{
		if(dac_mod==0)
		{
				if(true_temperature<temperature_standard){dac_out=0;}
				else{dac_out=255;}
		}
		else
		{
				if(true_temperature<=20){dac_out=51;}
				if(true_temperature>20&&true_temperature<40){dac_out=51+((char)(true_temperature-20)*8);}
				if(true_temperature>=40){dac_out=204;}
		}
}

void led_pan()
{
		if(led_mod==1)
		{
				if(dac_mod==0){led_mod=1;led_mod++;}
				else{led_mod=1;}
		}
		if(led_mod==3)
		{
				if(dac_mod==0){led_mod=3;led_mod++;}
				else{led_mod=3;}
		}
		if(led_mod==5)
		{
				if(dac_mod==0){led_mod=5;led_mod++;}
				else{led_mod=5;}
		}
}

void led_show()
{
		P2 = ((P2&0x1f)|0x80);
	  switch(led_mod)
		{
			//case 0:P0 = 0xff;break;
			case 1:P0 = 0xfd;break;
			case 2:P0 = 0xfc;break;
			case 3:P0 = 0xfb;break;
			case 4:P0 = 0xfa;break;
			case 5:P0 = 0xf7;break;
			case 6:P0 = 0xf6;break;
			//case 7:P0 = 0xf8;break;
		}
		P2 &= 0x1f;
}