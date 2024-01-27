#include "stc15f2k60s2.h"
#include "init.h"
#include "show.h"
#include "key.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
#include "led.h"

extern unsigned char ho;
extern unsigned char led1,led2,led3,ledn;
extern unsigned char led3_aux;

unsigned char key_value,old_key_value;
unsigned char tem_flag;
//变量值
float temperature,voltage;
unsigned char adc_value,liangmie;
//参数
char shi_can=17;
char tem_can=25;
unsigned char led_can=4;
void tem_start();

void main()
{
   close_buzz();
   close_led();
	 pcf8591_init();
	 set_time(16,59,50);
   Timer1Init();
   EA=1;
   while(1)
	 {
			fen();
		  tem_start();
		  led_pan();
	 }		 
}

void timer1_proc() interrupt 3
{
		static unsigned char a;
	  static unsigned int tem;
	  static unsigned char ad;
	  static unsigned int led3a;
	  static unsigned int led3b;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
				led_run();
		}
		if(++ad==50)
		{
				ad=0; 
			  adc_value=pcf8591_adc();
			  voltage=adc_value*0.0196;
			  if(adc_value<30){liangmie=1;}else{liangmie=0;}
		}
		if(++tem==300)
		{
				tem=0;
			  tem_flag=1;
		}
		if(liangmie==1)
		{
			  led3b=0;
				if(++led3a==3000)
				{
						led3a=0;
					  led3_aux=1;
				}
		}
		if(liangmie==0)
		{
				led3a=0;
			  if(++led3b==3000)
				{
						led3b=0;
					  led3_aux=0;
				}
		}
}

void tem_start()
{
		if(tem_flag==1)
		{
				tem_flag=0;
			  temperature=rd_temperature();
		}
}