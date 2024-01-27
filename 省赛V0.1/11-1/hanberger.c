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

                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0x8C,0xab};
                           //  U   P    n
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
unsigned char adc_value;
unsigned char mod=1;
float voltage_standard=3;
unsigned char voltage_standard10;
float voltage;
unsigned char count=0;
unsigned char error=0;
unsigned char key_led=0;
/******��������***********************/
void read_keyboard(void);
void display();
void deal_adc();
void key_proc();
void fen();
void zhunze();
void jishu();
void error_led(unsigned char a);
void led_pan();
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
  voltage_standard10=read_eeprom(0x00);
	voltage_standard=((float)voltage_standard10)/10;
		
  while(1)
  {
		  read_keyboard();
		  key_proc();
		  fen();
		  led_pan();
		  error_led(key_led);
  }         
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;
	static float voltage_old=0;
	static unsigned int dingshi = 0;
	display(); //1msִ��һ��
	if(++intr == 10) 
	{
			intr=0;
			adc_value=adc_pcf8591();
		  voltage=adc_value*0.0196;
		  if((voltage_old>voltage_standard)&&(voltage<voltage_standard))
			{count++;}
			voltage_old=voltage;
  }
	if(voltage_old<voltage_standard)
	{
			dingshi++;
		  if(dingshi>=5000)
			{
					key_led=1;
				  dingshi=5000;
			}
	}
	else
	{
			dingshi=0;
		  key_led=0;
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

void key_proc()
{
	switch(key_value)
	{
		case 14://S12
		{
			error=0;
			mod++;
			if(mod==4)
      {
				mod=1;
				voltage_standard10=voltage_standard*10;
				write_eeprom(0X00,voltage_standard10);
			}
			key_value=0xff;
			break;
		}
		case 10://S13
		{
			if(mod==3)
			{
				error=0;
				count=0;
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
		case 15://S16,��
		{
			if(mod==2)
			{
				  error=0;
					voltage_standard+=0.5;
				  if(voltage_standard>5)
					{voltage_standard=0;}
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
		case 11://S17
		{
			if(mod==2)
			{
				  error=0;
					voltage_standard-=0.5;
				  if(voltage_standard<0)
					{voltage_standard=5;}
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
	}
}

void deal_adc()//д�������鷳
{
		//float voltage;
	  int voltage100;
		unsigned char ge,dianyi,dianer;
		//voltage=adc_value*0.0196;
		voltage100=voltage*100;
		ge=voltage100/100;
		dianyi=voltage100%100/10;
		dianer=voltage100%10;
    
		dspbuf[0]=22;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=(ge+11);dspbuf[6]=dianyi;dspbuf[7]=dianer;
}

void zhunze()
{
	  int voltage_100;
		unsigned char ge1,dianyi1,dianer1;
		voltage_100=voltage_standard*100;
		ge1=voltage_100/100;
		dianyi1=voltage_100%100/10;
		dianer1=voltage_100%10;
	
		dspbuf[0]=23;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=(ge1+11);dspbuf[6]=dianyi1;dspbuf[7]=dianer1;
}

void jishu()
{
		dspbuf[0]=24;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=count/100;
	  dspbuf[6]=count%100/10;
	  dspbuf[7]=count%10;
}


void smg_off()//�ر�����ܣ���ʱ��
{
		dspbuf[0]=10;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;dspbuf[5]=10;dspbuf[6]=10;dspbuf[7]=10;
}

void fen()
{
		switch(mod)
		{
			case 1:deal_adc();break;
			case 2:zhunze();break;
			case 3:jishu();break;
		}
}

void error_led(unsigned char a)
{
		P2 = ((P2&0x1f)|0x80);
	  switch(a)
		{
			case 0:P0 = 0xff;break;
			case 1:P0 = 0xfe;break;
			case 2:P0 = 0xfd;break;
			case 3:P0 = 0xfc;break;
			case 4:P0 = 0xfb;break;
			case 5:P0 = 0xfa;break;
			case 6:P0 = 0xf9;break;
			case 7:P0 = 0xf8;break;
		}
		P2 &= 0x1f;
}

void led_pan()
{
	  if(key_led==1)
		{
				if(error>=3&&count%2==0)
		    {key_led=1;key_led+=4;}
				
		    if(error>=3&&count%2==1)
		    {key_led=1;key_led+=6;}
				
		    if(error<3&&count%2==0)
		    {key_led=1;}
				
		    if(error<3&&count%2==1)
		    {key_led=1;key_led+=2;}
		}
		else
		{
				if(error>=3&&count%2==0)
		    {key_led=0;key_led+=4;}
				
		    if(error>=3&&count%2==1)
		    {key_led=0;key_led+=6;}
				
		    if(error<3&&count%2==0)
		    {key_led=0;}
				
		    if(error<3&&count%2==1)
		    {key_led=0;key_led+=2;}
		}
		
}	