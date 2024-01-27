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
#include "onewire.h"
                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
													   0xc1,0x8e,0xc6,0x89,0x8c};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������


unsigned char key_value=0xFF;//����ֵ
unsigned char select=1;
unsigned char small_mod=0;
unsigned char s12_flag=0;

unsigned int pluse=0,pluse_zan=0;//�����Ƿֿ���

float temperature;
unsigned int temperature100;//�����Ƿֿ���
unsigned char shi_wei,ge_wei,yi_wei,er_wei;

unsigned char adc_value;//�����Ƿֿ���
double voltage;
char ge,dianyi;
double standard=2;

unsigned char adc_1;
unsigned char tem_1,tem_2;
unsigned char fre_1,fre_2,fre_3;

double voltage_stand;
unsigned int temperature_stand;
unsigned int frequency_stand;

/******��������***********************/
void read_keyboard();
void display();
void show_adc();
void show_NE();
void show_tem();
void fen();
void key_proc();
void deal();
void deal_adc();
void deal_tem();
void delay10ms();
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
	AUXR |= 0x80;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0; 	//���ö�ʱ��ģʽ
	TMOD |= 0x04;
	TL0 = 0xff;		//���ö�ʱ��ֵ
	TH0 = 0xff;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;    //�򿪶�ʱ���ж�
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
	
	EA=0;
	
	adc_1=read_eeprom(0x00);delay10ms();
	tem_1=read_eeprom(0x01);delay10ms();
	tem_2=read_eeprom(0x02);delay10ms();
	fre_1=read_eeprom(0x03);delay10ms();
	fre_2=read_eeprom(0x04);delay10ms();
	fre_3=read_eeprom(0x05);delay10ms();
	
	EA = 1;   
	init_pcf8591();
	voltage_stand=adc_1*0.0196;
	temperature_stand=tem_1*100+tem_2;
	frequency_stand=fre_1*1000+fre_2*10+fre_3;
	
  while(1)
  {
		  read_keyboard();
			key_proc();
		  deal();
		  fen();
  } 
}

//��ʱ���жϷ�����
void isr_timer_0(void)  interrupt 1  //Ĭ���ж����ȼ� 3
{
		pluse_zan++;
}

void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 3
{     
		static unsigned char time=0;
		static unsigned int tem=0;
	  static unsigned int jian=0;
	  static unsigned int a=0;
	  static unsigned int b=0;
		display(); //1msִ��һ��
		if(++time==10)
		{
				time=0;
				adc_value=adc_pcf8591();
		}
	  if(++tem==300)
	  {
			  tem=0;
		    temperature=rd_temperature_f();
	  }
	  if(++jian==1000)
		{
			  jian=0;
			  TR0=0;
				pluse=pluse_zan;
			  pluse_zan=0;
			  TR0=1;
		}
		if(s12_flag==1)
		{
			  standard=standard+0.1;
				if(standard>=5.1){standard=0;}
			  if(++a>=800)
				{
					  if(++b==200)
						{
								b=0;
							  standard=standard+0.1;
							  if(standard>=5.1){standard=0;}
						}
				}
		}
		else
		{
				a=0;b=0;
		}
		s12_flag=0;
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
		case 12://ѡ��
		{
			if(select!=5)
			{
					select++;
					if(select==4){select=1;}
			}	
      else
			{
			    s12_flag=1;
			}				
			key_value=0xff;
			break;
		}
		case 8://�洢
		{
			write_eeprom(0x00,adc_value);
			delay10ms();
			write_eeprom(0x01,temperature100/100);
			delay10ms();
			write_eeprom(0x02,temperature100%100);
			delay10ms();
			write_eeprom(0x03,pluse/1000);
			delay10ms();
			write_eeprom(0x04,pluse%1000/10);
			delay10ms();
			write_eeprom(0x05,pluse/10);
			delay10ms();
			voltage_stand=voltage;
			temperature_stand=temperature100;
			frequency_stand=pluse;
			key_value=0xff;
			break;
		}
		case 9:
		{
			if(select!=5){select=5;}
			else{select=1;}
			key_value=0xff;
			break;
		}
		case 13://����
		{
			small_mod=~small_mod;
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

void show_adc()
{
	  if(small_mod==0)
		{
				dspbuf[0]=22;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=10;
				dspbuf[5]=10;
				dspbuf[6]=(ge+11);
				dspbuf[7]=dianyi;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=22;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=10;
				dspbuf[5]=10;
				dspbuf[6]=((char)(voltage_stand*10)/10)+11;
				dspbuf[7]=(char)(voltage_stand*10)%10;
		}
		    
}

void show_NE()
{
	  if(small_mod==0)
		{
				dspbuf[0]=23;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=pluse/10000;
				dspbuf[4]=pluse%10000/1000;
				dspbuf[5]=pluse%1000/100;
				dspbuf[6]=pluse%100/10;
				dspbuf[7]=pluse%10;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=23;
				dspbuf[2]=10;
				dspbuf[3]=frequency_stand/10000;
				dspbuf[4]=frequency_stand%10000/1000;
				dspbuf[5]=frequency_stand%1000/100;
				dspbuf[6]=frequency_stand%100/10;
				dspbuf[7]=frequency_stand%10;
		}
}	

void show_tem()
{
	  if(small_mod==0)
		{
				dspbuf[0]=24;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=shi_wei;
				dspbuf[5]=ge_wei+11;
				dspbuf[6]=yi_wei;
				dspbuf[7]=er_wei;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=24;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=temperature_stand/1000;
				dspbuf[5]=(temperature_stand%1000/100)+11;
				dspbuf[6]=temperature_stand/100%10;
				dspbuf[7]=temperature_stand%10;
		}			
}

void show_standard()
{
	  dspbuf[0]=26;
		dspbuf[1]=key_value/100;
		dspbuf[2]=key_value%100/10;
		dspbuf[3]=key_value%10;
    dspbuf[4]=10;
		dspbuf[5]=10;
		dspbuf[6]=((char)(standard*10))/10+11;
		dspbuf[7]=((char)(standard*10))%10;
}
void fen()
{
		if(select==1)
		{show_adc();}
		if(select==2)
		{show_NE();}
		if(select==3)
		{show_tem();}
		if(select==5)
		{show_standard();}
}

void deal()
{
	  deal_adc();
	  deal_tem();
}

void deal_adc()
{
		voltage=adc_value*0.0196;
		ge=(char)voltage;
		dianyi=((char)(voltage*10))%10;
}

void deal_tem()
{
		temperature100=(int)(temperature*100);
		shi_wei=(char)temperature/10;
		ge_wei=(char)temperature%10;
		yi_wei=(char)(temperature100%100/10);
	  er_wei=(char)(temperature100%10);
}

void delay10ms(void)   //��� 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
