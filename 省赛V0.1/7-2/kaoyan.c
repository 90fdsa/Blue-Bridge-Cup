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
//#include "STC15F2K60S2.H"  //�ô�ͷ�ļ�ֱ�Ӵ���REG52.H�����������ͷ�ļ��Ա�
#include "absacc.h"
#include "ds1302.h"
#include "i2c.h"
                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0xbf};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
unsigned char *p_rtc;
unsigned char clock_setting[6];//���ڱ���Ҫ������ʱ��
unsigned char chache[4];
unsigned char clock_flag=1,voltage_flag=1;//���������߻��ǵ���ʱ��
unsigned char liangmie=0;
unsigned char time_choose=0,voltage_choose=0,frequency_choose,chache_choose;
//��һ����ʱ����˸��־λ���ڶ�����ʱ���޸�ѡ���־λ
char hour,minute,second;
extern unsigned char set[6];
unsigned char mod=1;
unsigned char adc_value;
int voltage_now,voltage_old,voltage_high=2000,voltage_low=1000;
unsigned char high,low;
/******��������***********************/
void read_keyboard(void);
void display();
void Delay100ms();		//@11.0592MHz
void delay10ms();
void fen();
void clock();
void key_proc();
void blink();
void clock_normal();
void voltage_ce();
void blink_voltage();
void voltage();
void frequency();
void chakan();
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
	  AUXR |= 0xc0;
		TMOD = 0x04;
	
		TL0 = 0;	//1ms,1Tmoshi
		TH0 = 0;	
		TR0 = 1;
	  
		TL1 = 0XCD;
		TH1 = 0XD4;
		TR1 = 1;
		ET1 = 1;
		EA = 1; 
	  
		Delay100ms();
    SetRTC(); 
    	
	  init_pcf8591();
	
	  voltage_high=read_eeprom(0x00)*100;delay10ms();
		voltage_low=read_eeprom(0x08)*100;delay10ms();
		chache[0]=read_eeprom(0x10);delay10ms();
		chache[1]=read_eeprom(0x11);delay10ms();
		chache[2]=read_eeprom(0x12);delay10ms();
		chache[3]=read_eeprom(0x13);delay10ms();
		
    while(1)
    {
			  key_proc();
        fen();
    }
}

//��ʱ���жϷ�����
void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;
	static unsigned int blin = 0;
	if(++intr == 10) 
	{
     intr = 0;
		 read_keyboard();
		 adc_value=adc_pcf8591();
		 
		 if(voltage_now<voltage_low&&voltage_old>voltage_low)
		 {
				chache[0]=0;
			  chache[1]=p_rtc[0]*10+p_rtc[1];
			  chache[2]=p_rtc[2]*10+p_rtc[3];
			  chache[3]=p_rtc[4]*10+p_rtc[5];
			  write_eeprom(0x10,chache[0]);delay10ms();write_eeprom(0x11,chache[1]);delay10ms();
			  write_eeprom(0x12,chache[2]);delay10ms();write_eeprom(0x13,chache[3]);delay10ms();
		 }
		 if(voltage_now>voltage_high&&voltage_old<voltage_high)
		 {
				chache[0]=1;
			  chache[1]=p_rtc[0]*10+p_rtc[1];
			  chache[2]=p_rtc[2]*10+p_rtc[3];
			  chache[3]=p_rtc[4]*10+p_rtc[5];
			 	write_eeprom(0x10,chache[0]);delay10ms();write_eeprom(0x11,chache[1]);delay10ms();
			  write_eeprom(0x12,chache[2]);delay10ms();write_eeprom(0x13,chache[3]);delay10ms();
		 }
		 voltage_old=voltage_now;
  }
	if(++blin == 1000)
	{
			blin=0;
		  liangmie=~liangmie;
		    
		  TR0=0;
		  high=TH0;low=TL0;
		  TH0=0;TL0=0;
		  TR0=1;
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

void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void delay10ms()		//10ms @11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}

void key_proc()
{
	static unsigned char count=0;
//	static unsigned char count_v=0;
	switch(key_value)
	{
		case 12://S4
		{
			if(mod==1)//ʱ�����
			{
					clock_flag=0;
					count++;
					if(count==1)
					{
							clock_setting[0]=p_rtc[0];clock_setting[1]=p_rtc[1];clock_setting[2]=p_rtc[2];
							clock_setting[3]=p_rtc[3];clock_setting[4]=p_rtc[4];clock_setting[5]=p_rtc[5];
							hour=clock_setting[0]*10+clock_setting[1];
							minute=clock_setting[2]*10+clock_setting[3];
							second=clock_setting[4]*10+clock_setting[5];
					}
			
					time_choose++;
					if(time_choose==4){time_choose=1;}
			}
			else if(mod==2)//��ѹ����
			{
					voltage_flag=0;
				  voltage_choose++;
					if(voltage_choose==3){voltage_choose=1;}
			}
			else if(mod==3)//Ƶ�ʿ���
			{
					frequency_choose++;
				  if(frequency_choose==3){frequency_choose=1;}
			}
			else if(mod==4)//��ѹ�鿴
			{
					chache_choose++;
				  if(chache_choose==3){chache_choose=1;}
			}
			key_value=0xff;
			break;
		}
		case 0://S7��ʱ��
		{			
			mod=1;
			if(count>=1)
			{
				set[0]=hour/10;set[1]=hour%10;set[2]=minute/10;
			  set[3]=minute%10;set[4]=second/10;set[5]=second%10;
			  SetRTC();
			}
			clock_flag=1;count=0;
			key_value=0xff;
			break;
		}
		case 4://S6����ѹ
		{
			mod=2;voltage_flag=1;
			write_eeprom(0x00,voltage_high/100);
			write_eeprom(0x08,voltage_low/100);
			key_value=0xff;
			break;
		}
		case 8://S5��Ƶ��
		{
			mod=3;frequency_choose=1;
			key_value=0xff;
			break;
		}
		case 9://S9���鿴
		{
			mod=4;chache_choose=1;
			key_value=0xff;
			break;
		}
		case 1://S11������
		{
			if(mod==1)//ʱ�����
			{
				  if(clock_flag==0)
					{
							switch(time_choose)
							{
									case 1:hour++;if(hour==24){hour=0;}break;
									case 2:minute++;if(minute==60){minute=0;}break;
									case 3:second++;if(second==60){second=0;}break;
						  }	
					}
			}
			if(mod==2)
			{
					if(voltage_flag==0)
					{
							switch(voltage_choose)
							{
								case 1:voltage_high+=500;if(voltage_high==10000){voltage_high=9500;}break;
								case 2:voltage_low+=500;
								if(voltage_low==10000){voltage_low=9500;}
								if(voltage_low>=voltage_high){voltage_low-=500;}break;
							}
					}
			}
			key_value=0xff;
			break;
		}
		case 5://S10���½�
		{
			if(mod==1)//ʱ�����
			{
				  if(clock_flag==0)
			    {
					    switch(time_choose)
							{
									case 1:hour--;if(hour<0){hour=23;}break;
									case 2:minute--;if(minute<0){minute=59;}break;
									case 3:second--;if(second<0){second=59;}break;
						  }	
					}
			}
			if(mod==2)
			{
					if(voltage_flag==0)
					{
							switch(voltage_choose)
							{
								case 1:voltage_high-=500;
                if(voltage_high<0){voltage_high=0;}
								if(voltage_high<=voltage_low){voltage_high+=500;}break;
								case 2:voltage_low-=500;if(voltage_low<0){voltage_low=0;}break;
							}
					}
			}
			key_value=0xff;
			break;
		}
		
	}
}
void fen()
{
		switch(mod)
		{
			case 1:clock();break;
			case 2:voltage();break;
			case 3:frequency();break;
			case 4:chakan();break;
		}
}

void clock()
{
	 if(clock_flag==1){clock_normal();}
	 else{blink();}
}

void clock_normal()
{
		p_rtc = ReadRTC();	
	  dspbuf[0] = p_rtc[0];
	  dspbuf[1] = p_rtc[1];
    dspbuf[2] = 11;
	  dspbuf[3] = p_rtc[2];
	  dspbuf[4] = p_rtc[3];
	  dspbuf[5] = 11;
	  dspbuf[6] = p_rtc[4];
	  dspbuf[7] = p_rtc[5];
}

void blink()
{
		if(time_choose==1)
		{
				if(liangmie==0)
				{
						dspbuf[0] = hour/10;dspbuf[1] = hour%10;dspbuf[2] = 11;dspbuf[3] = minute/10;
					  dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				}
				else
				{
						dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
					  dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				}
		}
		if(time_choose==2)
		{
				if(liangmie==0)
				{
						dspbuf[0] = hour/10;dspbuf[1] = hour%10;dspbuf[2] = 11;dspbuf[3] = minute/10;
					  dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				}
				else
				{
						dspbuf[0] = hour/10;dspbuf[1] = hour%10;dspbuf[2] = 11;dspbuf[3] = 10;
					  dspbuf[4] = 10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				}
		}
		if(time_choose==3)
		{
				if(liangmie==0)
				{
						dspbuf[0] = hour/10;dspbuf[1] = hour%10;dspbuf[2] = 11;dspbuf[3] = minute/10;
					  dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				}
				else
				{
						dspbuf[0] = hour/10;dspbuf[1] = hour%10;dspbuf[2] = 11;dspbuf[3] = minute/10;
					  dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = 10;dspbuf[7] = 10;
				}
		}
}

void voltage()
{
		if(voltage_flag==1){voltage_ce();}
	  else{blink_voltage();}
}

void voltage_ce()
{
		float a=19.6;
	  voltage_now=a*adc_value;
	  dspbuf[0] = 11;dspbuf[1] = 1;dspbuf[2] = 11;dspbuf[3] = 10;
	  dspbuf[4] = voltage_now/1000;
	  dspbuf[5] = voltage_now%1000/100;
	  dspbuf[6] = voltage_now%100/10;
	  dspbuf[7] = voltage_now%10;
}

void blink_voltage()
{
		if(voltage_choose==1)
		{
				if(liangmie==0)
				{
						dspbuf[0] = voltage_high/1000;dspbuf[1] = voltage_high%1000/100;
					  dspbuf[2] = voltage_high%100/10;dspbuf[3] = voltage_high%10;
					  dspbuf[4] = voltage_low/1000;dspbuf[5] = voltage_low%1000/100;
					  dspbuf[6] = voltage_low%100/10;dspbuf[7] = voltage_low%10;
				}
				else
				{
						dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 10;dspbuf[3] = 10;
					  dspbuf[4] = voltage_low/1000;dspbuf[5] = voltage_low%1000/100;
					  dspbuf[6] = voltage_low%100/10;dspbuf[7] = voltage_low%10;
				}
		}
		if(voltage_choose==2)
		{
				if(liangmie==0)
				{
						dspbuf[0] = voltage_high/1000;dspbuf[1] = voltage_high%1000/100;
					  dspbuf[2] = voltage_high%100/10;dspbuf[3] = voltage_high%10;
					  dspbuf[4] = voltage_low/1000;dspbuf[5] = voltage_low%1000/100;
					  dspbuf[6] = voltage_low%100/10;dspbuf[7] = voltage_low%10;
				}
				else
				{
						dspbuf[0] = voltage_high/1000;dspbuf[1] = voltage_high%1000/100;
					  dspbuf[2] = voltage_high%100/10;dspbuf[3] = voltage_high%10;
					  dspbuf[4] = 10;dspbuf[5] = 10;dspbuf[6] = 10;dspbuf[7] = 10;
				}
		}
}

void frequency()
{
	  unsigned int high_high,high_low,low_high,low_low;
	  float frequency_stand;
    float period;
		high_high=(high&0xf0)>>4;
		high_low=high&0x0f;
	  low_high=(low&0xf0)>>4;
		low_low=(low&0x0f);
	
		frequency_stand=(high_high*16*16*16+high_low*16*16+low_high*16+low_low);
	  period=(1/frequency_stand)*1000000;
	  
	  if(frequency_choose==1)
		{
				 dspbuf[0] = 11;dspbuf[1] = 2;dspbuf[2] = 11; 
			   dspbuf[3] = ((int)frequency_stand)/10000;
	       dspbuf[4] = ((int)frequency_stand)%10000/1000;
	       dspbuf[5] = ((int)frequency_stand)%1000/100;
	       dspbuf[6] = ((int)frequency_stand)%100/10;
	       dspbuf[7] = ((int)frequency_stand)%10;
		}
		else if(frequency_choose==2)
		{
				 dspbuf[0] = 11;dspbuf[1] = 2;dspbuf[2] = 11;
			   dspbuf[3] = ((int)period)/10000;
	       dspbuf[4] = ((int)period)%10000/1000;
	       dspbuf[5] = ((int)period)%1000/100;
	       dspbuf[6] = ((int)period)%100/10;
	       dspbuf[7] = ((int)period)%10;
		}
}

void chakan()
{
		if(chache_choose==1)
		{
				 dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 10; 
			   dspbuf[3] = 10;dspbuf[4] = 10;dspbuf[5] = 10;
	       dspbuf[6] = chache[0]/10;
	       dspbuf[7] = chache[0]%10;
		}
		else if(chache_choose==2)
		{
				 dspbuf[0] = chache[1]/10;
	       dspbuf[1] = chache[1]%10;
         dspbuf[2] = 11;
	       dspbuf[3] = chache[2]/10;
	       dspbuf[4] = chache[2]%10;
	       dspbuf[5] = 11;
	       dspbuf[6] = chache[3]/10;
	       dspbuf[7] = chache[3]%10;
		}
}