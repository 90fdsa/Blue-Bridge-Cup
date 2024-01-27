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

                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0,old_key_value=0;//����ֵ
unsigned char led_1,led_2,led_3,Led_Flag;
unsigned char ok=0;
unsigned char done=0;
unsigned int chang = 0;
unsigned char times=0;
unsigned char jiesuan=0;
/******��������***********************/
void display();
unsigned char Key_Scan(); 
void key_proc_short();
void led_show();
void key_proc_long();
void key_proc_dcl();
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
    while(1)
    {
			  //key_proc_short();
				dspbuf[7] = key_value%10;
				dspbuf[6] = key_value/10;
			
				led_show();
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;
  static unsigned char b=0;
	display(); //1msִ��һ��
	if(++intr == 10) 
	{
     intr = 0;
		 old_key_value=key_value;
     key_value=Key_Scan();
		 if(old_key_value<key_value){ok=1;times++;}//�̰�����˫������ʵӦ�ýа��¼��
		 //���ֻ�ж̰���������������д�̰�����������
		 if(old_key_value>key_value){done=0;}//����������ʵӦ�ý����ּ��
		 //���������оݱ���Ҫ�����ּ��
  }
	
	if(ok==1)//��ʱ200ms����������˫���͵���
	{
			b++;
		  if(b==200){jiesuan=1;b=0;}
	}
	
	if(jiesuan==1)//��ʱ֮������㣬�ж�˫���򵥻�
	{
			if(times==2){key_proc_dcl();ok=0;times=0;jiesuan=0;}
			if(times==1){key_proc_short();ok=0;times=0;jiesuan=0;}
	}
	
  if(key_value!=0)//���ifelse�ǳ���������
	{
		 chang++;
		 if(chang>1000 & done==0){key_proc_long();}
		 //��ס���key_proc_long()��������Ҫ�ں�����β�����ֵΪ�㣬doneΪ1�����
	}
	else{chang=0;}
}

unsigned char Key_Scan(void)
{
	unsigned int Key_New;
	unsigned char Key_Value;
	P3 |= 0x0F;
	P44 = 0; P42 = 1; P35 = 1; P34 = 1;  // ��һ��
	Key_New = P3 & 0X0F;
	P44 = 1; P42 = 0; P35 = 1; P34 = 1;  // �ڶ���
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;  // ������
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 1; P34 = 0;  // ������
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	switch(~Key_New)
	{
		case 0x8000: Key_Value = 13; break;
		case 0x4000: Key_Value = 9; break;
		case 0x2000: Key_Value = 5; break;
		case 0x1000: Key_Value = 1; break;		
		case 0x0800: Key_Value = 14; break;
		case 0x0400: Key_Value = 10; break;
		case 0x0200: Key_Value = 6; break;
		case 0x0100: Key_Value = 2; break;		
		case 0x0080: Key_Value = 15; break;
		case 0x0040: Key_Value = 11; break;
		case 0x0020: Key_Value = 7; break;
		case 0x0010: Key_Value = 3; break;			
		case 0x0008: Key_Value = 16; break;
		case 0x0004: Key_Value = 12; break;
		case 0x0002: Key_Value = 8; break;
		case 0x0001: Key_Value = 4; break;			
		default : Key_Value = 0;
	}
	return Key_Value;
}


//��ʾ����
void display(void)
{   
	static unsigned char dspcom = 0;
	//������Ӱ
	P0 = 0xff;
	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  //дλ��
	P0 = 1<<dspcom;
	P2 = ((P2&0x1f)|0xC0); 
	P2 &= 0x1f;
  //д����
	P0 = tab[dspbuf[dspcom]];	
  	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  
    if(++dspcom == 8){
        dspcom = 0;
    }    
}

void key_proc_short()
{
	switch(key_value)
	{
		case 1://ѡ��
		{
			led_1=~led_1;
			break;
		}
		case 2://�洢
		{
			led_2=~led_2;
			break;
		}
		case 3:
		{
			led_3=~led_3;
			break;
		}
	}
}

void key_proc_long()
{
	switch(key_value)
	{
		case 4://����ѭ��֮��ֻ��һ��
		{
			led_2=~led_2;
			key_value=0;done=1;
			break;
		}
		case 1://����ѭ��֮��һֱ��
		{
			led_1=~led_1;
			//key_value=0;done=1;
			break;
		}
	}
}

void key_proc_dcl()
{
	switch(key_value)
	{
		case 6://����ѭ��֮��ֻ��һ��
		{
			led_2=~led_2;
			//key_value=0;done=1;
			break;
		}
	}
}

void led_show()
{
	  //led1
	  if(led_1!=0)
	  {
		  Led_Flag |= 0x01;
		}
		else
		{
			Led_Flag &= 0xfe;
		}
	
		//led7
		if(led_2!=0)
		{
			Led_Flag |= 0x40;
		}
		else
		{
			Led_Flag &= 0xbf;
		}
	
		//led8
		if(led_3!=0)
		{
			Led_Flag |= 0x80;
	  }
	  else
	  {
		  Led_Flag &= 0x7f;
	  }
		P0 = ~Led_Flag;
		P2 = ((P2&0x1f)|0x80);
		P2 &= 0x1f;
}