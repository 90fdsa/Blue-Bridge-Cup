#include "STC15F2K60S2.h"
#include "init.h"
#include "show.h"
#include "button.h"
#include "ds1302.h"
#include "ultrasonic.h"
#include "iic.h"
#include "led.h"

unsigned char period[]={2,3,5,7,9};

extern unsigned char sonic_mod_flag;
extern unsigned char h,m,s;
extern unsigned char miaoshu;


unsigned char key_value,old_key_value;
unsigned char distance;
unsigned char adc_value,old_adc_value;
unsigned char now_second,new_second,old_second;
unsigned char sonic_time=0;
unsigned char max,min,xiangjiawei=0;
unsigned int sum;
float average;


void main()
{
		close_buzz();
    close_led();
	  set_time(15,0,0);
	  pcf8591_init();
	  Timer1Init();
	  EA=1;
	  while(1)
		{
				fen();
			  now_second=Read_Ds1302_Byte(0x81);
		}
}

void Timer1_proc() interrupt 3
{
	  static unsigned char a;
		display();
	  led();
	  if(++a==10)
		{
				a=0;
			
				old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
				
				//Dac计算
				if(distance<=10){pcf8591_dac(51);}
				if(distance>=80){pcf8591_dac(255);}
				if(distance>10&distance<80){pcf8591_dac((distance-10)*2.91+51);}
				
				//测量距离
				if(sonic_mod_flag==0)
				{
						old_adc_value=adc_value;
						adc_value=pcf8591_adc();
						if(old_adc_value>20&adc_value<20){distance=sonic();sonic_time++;xiangjiawei=1;}
				}
				if(sonic_mod_flag!=0)
				{
					  old_second=new_second;
					  new_second=now_second/16*10+now_second%16;
						if((new_second%period[miaoshu]==0)&&(old_second%period[0]!=0))
		        {distance=sonic();sonic_time++;xiangjiawei=1;}	
				}
				//计算最大值那三个
				if(sonic_time==1){max=distance;min=distance;average=distance;sum=distance;xiangjiawei=0;}
				if(sonic_time>1)
				{
						if(distance>=max){max=distance;}
						if(distance<=min){min=distance;}
						if(xiangjiawei==1)
						{
								sum=sum+distance;
								average=sum/sonic_time;
							  xiangjiawei=0;
						}
				}
		}
}