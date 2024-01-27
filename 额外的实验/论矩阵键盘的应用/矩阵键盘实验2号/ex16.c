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
unsigned char key_value=0;//����ֵ
unsigned char led_1,led_2,led_3,Led_Flag;
unsigned char flag;
unsigned int flagnum=0;
/******��������***********************/
void display();
void Key_Scan(); 
void key_proc();
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
    while(1)
    {
				dspbuf[7] = key_value%10;
				dspbuf[6] = key_value/10;
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;

	display(); //1msִ��һ��
	if(++intr == 10) 
	{
     intr = 0;
     Key_Scan();
  }
	if(++flagnum==2000)
  {flagnum=0;flag=1;}
}

void Key_Scan(void)
{
		unsigned char key,hang,lie,keyvalue,yi=0;
		P3&=0X0F;P42=0;P44=0;key=P3&0X0F;
		if(key!=0x0f)
		{
	     switch(key)
       { 
					case 0x0e: hang=4;break;
          case 0x0d: hang=3;break;
				  case 0x0b: hang=2;break;
          case 0x07: hang=1;break;
			 }
    }
    P3|=0XF0;P42=1;P44=1;P36=P42;P37=P44;key=P3&0xf0;
    if(key!=0xf0)
    {
       switch(key)
       {
          case 0xe0: lie=4;break;
          case 0xd0: lie=3;break;
          case 0xb0: lie=2;break;
          case 0x70: lie=1;break;
       }
		}
    keyvalue=lie*4+hang-1;key_value=keyvalue;
    P3=0X0F;P42=0;P44=0;key=P3&0X0F; //���ּ��
    flagnum=0;flag=0;yi=0;
    while(P3!=0X0F)
    {
			 if(flag==1&&yi==0)
       { 
			     switch(keyvalue)
			     {case  1:   break;   }   
			 }  //����
    }
    if(flag==0)
    { 
				switch(keyvalue)
				{case  1:   break;   }  
		}   //�̰�

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