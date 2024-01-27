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
#include "intrins.h"
#include "onewire.h"


                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0xbf,0xc6};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
char hour=23,minute=59,second=50;//ʱ��
char hours=0,minutes=0,seconds=0;//����ʱ��
char stand,mod=0;
char time=0,order=0;//�������ӵı���
char alart=0;
char ordre=0;//������ʾ�¶ȵı���
char temper;
/******��������***********************/
void read_keyboard(void);
void display();
void how_to_display();
void flow();
void blink(unsigned char a);
void key_proc();
void delay2ms(void);
void alarm();
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
	//init_ds18b20();
    while(1)
    {
			read_keyboard();
			if(key_value != 0xFF)
			{key_proc();}
			alarm();  
    }
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{     
	static unsigned int intr = 0;
	static unsigned int tem = 0;
	display(); //1msִ��һ��
	if(++tem==50)
	{
		temper=rd_temperature();
		
		tem=0;
	}
	if(mod==0)
	{
		how_to_display();
		if(++intr == 1000) 
		{
			intr = 0;
			flow();
			alart=0;
		}
	}
	else if(mod==1)
	{
		if(++intr == 1000) 
		{
			intr = 0;
			blink(time);
		}
	}
	else if(mod==2)
	{
		if(++intr == 1000) 
		{
			intr = 0;
			blink(time);
		}
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

void how_to_display()
{
	if(ordre==1)
	{
		dspbuf[0] = 10;
		dspbuf[1] = 10;
		dspbuf[2] = 10;
		dspbuf[3] = 10;
		dspbuf[4] = 10;
		dspbuf[5] = temper/10;
		dspbuf[6] = temper%10;
		dspbuf[7] = 12;
	}
	else
	{
		dspbuf[0] = hour/10;
		dspbuf[1] = hour%10;
		dspbuf[2] = 11;
		dspbuf[3] = minute/10;
		dspbuf[4] = minute%10;
		dspbuf[5] = 11;
		dspbuf[6] = second/10;
		dspbuf[7] = second%10;
	}
	
}

void key_proc()
{
	static unsigned char teim=0;//����ʱ��ı���
	switch(key_value)
	{
		case 8:
		{
			mod=1;
			switch(teim)
			{
				case 0:time=0;teim++;break;
				case 1:time=1;teim++;break;
				case 2:time=2;teim++;break;
				case 3:mod=0;time=0;teim=0;break;
			}
			key_value=0xff;alart=1;
			break;
		}
		case 12:
		{
			mod=2;
			switch(teim)
			{
				case 0:time=0;teim++;break;
				case 1:time=1;teim++;break;
				case 2:time=2;teim++;break;
				case 3:mod=0;time=0;teim=0;break;
			}
			key_value=0xff;alart=1;
			break;
		}
		case 9:
		{
			if(mod==1)
			{
				switch(time)
				{
					case 0:
					{	hour++;
						if(hour==24)
						{hour=0;}
						break;}
					case 1:
					{	minute++;
						if(minute==60)
						{minute=0;}
						break;}
					case 2:
					{	second++;
						if(second==60)
						{second=0;}
						break;}
				}
			}
			else if(mod==2)
			{
				switch(time)
				{
					case 0:
					{	hours++;
						if(hours==24)
						{hours=0;}
						break;}
					case 1:
					{	minutes++;
						if(minutes==60)
						{minutes=0;}
						break;}
					case 2:
					{	seconds++;
						if(seconds==60)
						{seconds=0;}
						break;}
				}
			}
			key_value=0xff;alart=1;
			break;
		}
		case 13:
		{
			if(mod==0)
			{
				if(ordre==1)
				{ordre=0;}
				else
				{ordre++;}
			}
			if(mod==1)
			{
				switch(time)
				{
					case 0:
					{	hour--;
						if(hour<0)
						{hour=23;}
						break;}
					case 1:
					{	minute--;
						if(minute<0)
						{minute=59;}
						break;}
					case 2:
					{	second--;
						if(second<0)
						{second=59;}
						break;}
				}
			}
			else if(mod==2)
			{
				switch(time)
				{
					case 0:
					{	hours--;
						if(hours<0)
						{hours=23;}
						break;}
					case 1:
					{	minutes--;
						if(minutes<0)
						{minutes=59;}
						break;}
					case 2:
					{	seconds--;
						if(seconds<0)
						{seconds=59;}
						break;}
				}
			}
			key_value=0xff;alart=1;
			break;
		}
	}
}
void flow()
{
	second++;
	if(second==60)
	{
		second=0;
		minute++;
		if(minute==60)
		{
			minute=0;
			hour++;
			if(hour==24)
			{
				second=0;
				minute=0;
				hour=0;
			}
		}
	}
}

void blink(unsigned char a)
{
	static unsigned char count=0;
	if(mod==1)
	{
		if(a==0)
		{
			if(count==0)
			{
				dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
		else if(a==1)
		{
			if(count==0)
			{
				dspbuf[0] = hour / 10;dspbuf[1] = hour % 10;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = 10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
		else if(a==2)
		{
			if(count==0)
			{
				dspbuf[0] = hour / 10;dspbuf[1] = hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = 10;dspbuf[7] = 10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
	}
	if(mod==2)
	{
		if(a==0)
		{
			if(count==0)
			{
				dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
		else if(a==1)
		{
			if(count==0)
			{
				dspbuf[0] = hours / 10;dspbuf[1] = hours % 10;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = 10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
		else if(a==2)
		{
			if(count==0)
			{
				dspbuf[0] = hours / 10;dspbuf[1] = hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = 10;dspbuf[7] = 10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
	}
}
void alarm()
{
	char i;
	if(hour==hours&&minute==minutes&&second==seconds&&alart==0)
	{
		for(i=0;i<13;i++)
		{
			P2 = ((P2&0x1f)|0x80);
			P0 = 0xff;  //LED��
			P2 &= 0x1f;
      delay2ms();
        
      P2 = ((P2&0x1f)|0x80);
			P0 = 0xfe;  //LED��
			P2 &= 0x1f;
      delay2ms();
		}
		P2 = ((P2&0x1f)|0x80);
		P0 = 0xff;  //LED��
		P2 &= 0x1f;
	}
}

void delay2ms(void)   //��� -0.036168981482us
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