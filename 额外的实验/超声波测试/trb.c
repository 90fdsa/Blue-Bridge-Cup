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
sbit TX = P1^0;   //��������
sbit RX = P1^1;   //��������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
unsigned char ui_Sonic_flag;
unsigned char uc_Dist; 
/******��������***********************/
void read_keyboard(void);
void display();
unsigned char Sonic(void);
void Sonic_Proc(void);
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

void Timer0Init(void)		//12΢��@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xF4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0ֹͣ��ʱ
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

//������
void main(void)
{  
	  cls_buzz();
	  cls_led();
    Timer0Init();
	  Timer1Init();
	  EA=1;
    while(1)
    {   
			  Sonic_Proc();
       	dspbuf[5]=uc_Dist/100;
			  dspbuf[6]=uc_Dist%100/10;
			  dspbuf[7]=uc_Dist%10;
    }
}

//��ʱ���жϷ�����
void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{     
	static unsigned int intr = 0;
	if(++intr==300)
	{
		intr=0;
		ui_Sonic_flag = 1;   //������ˢ��Ƶ�� 300msһ��
	}
	display(); //1msִ��һ��
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

unsigned char Sonic(void)
{
  unsigned char Dist=0,num=10;
	
  //TX���ŷ���10��Ƶ��Ϊ40MHz����
  Timer0Init();  //��ʼ����ʱ��0����ʼ����ʱ����ֵ
  TX = 0;        //����������Ϊ�͵�ƽ
  
  while(num--)  //����10��
  {
    TR0 = 1;
	  while(!TF0);  //��ʱʱ�䵽����������
	  TF0 = 0;      //�ֶ������־λ	
	  TX ^= 1;      //ȡ��
    Timer0Init();  //��ʼ����ʱ��0����ʼ����ʱ����ֵ	
  }


//���շ���
	TR0 = 0;		//��ʱ��0ֹͣ��ʱ
	TF0 = 0;		//���TF0��־
	TL0 = 0x00;	//���ö�ʱ��ֵ
	TH0 = 0x00;	//���ö�ʱ��ֵ
	TR0 = 1;

  while(RX && !TF0);  //�ȴ��յ�����	��ʱ���
  TR0 = 0;  //��ʱ��0ֹͣ��ʱ
	
  if(TF0)   //��ʱ���
  {
	   TF0 = 0; //�����־λ		
	   Dist = 255;
  }
  else  //RX�������Ž��յ��͵�ƽ
  {
	  Dist = ((TH0<<8)+TL0)*0.017;
  }
	
  return Dist;
}

void Sonic_Proc(void)
{
  if(ui_Sonic_flag==1)
	{
			ui_Sonic_flag = 0;
		  uc_Dist = Sonic();  //���
	}
}