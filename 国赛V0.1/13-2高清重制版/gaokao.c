#include "stc15f2k60s2.h"
#include "init.h"
#include "key.h"
#include "show.h"
#include "iic.h"
#include "ultrasonic.h"
#include "jidianqi.h"
#include "led.h"

//extern unsigned char buffer[];
extern unsigned char key_value,old_key_value;
//一系列标志位
unsigned char adc_flag,sonic_flag;
//测量量
unsigned int freq,frequency;
unsigned char himid,distance;
//参数
unsigned char himid_can=20;
unsigned char distance_can=50;
unsigned int frequence_can=1500;
//led标记位
extern unsigned char qi1,qi2,qi3,aux1,aux2,aux3;

void adc_start();
void sonic_start();
void dac_start();

void main()
{
    close_buzz();
    close_led();
	  pcf8591_init();
	  PCA_init();
	  Timer0Init();	
	  Timer1Init();		//100微秒@12.000MHz
	  Timer2Init();
	  EA=1;
	  while(1)
		{
				fen();
			  adc_start();
			  sonic_start();
			  led_pan();
		}
}

void timer0_proc() interrupt 1
{
		freq++;
}

void timer1_proc() interrupt 3
{
		static unsigned char a,ad;
	  static unsigned int fre,son;
	  static unsigned char ok;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
				//dac
				dac_start();
				jidianqi_go();
				led();
				
		}
		if(++fre==500){fre=0;frequency=freq*2;freq=0;}
		if(++ad==50){ad=0;adc_flag=1;}
		if(++son==300){son=0;sonic_flag=1;}
		
		if(qi1==1){if(++ok==100){ok=0;aux1=~aux1;}}else{aux1=0;}
		if(qi2==1){if(++ok==100){ok=0;aux2=~aux2;}}else{aux2=0;}
		if(qi3==1){if(++ok==100){ok=0;aux3=~aux3;}}else{aux3=0;}
}

void timer2_proc() interrupt 12
{
		static unsigned int ji;
	  unsigned char d;
	  if(frequency<frequence_can)
		{
			  ji++;
				if(ji<=200)
        {
						d|=0x20;d&=0xbf;
						P0=d;
						P2=(P2&0x1f)|0xa0;
						P2&=0x1f;
				}
				else if(ji>200&ji<1000)
				{
						d&=0xdf;d&=0xbf;
						P0=d;
						P2=(P2&0x1f)|0xa0;
						P2&=0x1f;
				}
				else if(ji==1000){ji=0;}
		}
		else if(frequency>=frequence_can)
		{
				ji++;
				if(ji<=800)
        {
						d|=0x20;d&=0xbf;
						P0=d;
						P2=(P2&0x1f)|0xa0;
						P2&=0x1f;
				}
				else if(ji>800&ji<1000)
				{
						d&=0xdf;d&=0xbf;
						P0=d;
						P2=(P2&0x1f)|0xa0;
						P2&=0x1f;
				}
				else if(ji==1000){ji=0;}
		}
}

void adc_start()
{
   if(adc_flag==1){adc_flag=0;himid=pcf8591_adc()*0.39;}
}

void sonic_start()
{
	 if(sonic_flag==1){sonic_flag=0;distance=sonic();}
}

void dac_start()
{
	if(himid<himid_can){pcf8591_dac(51);}
	else if(himid>80){pcf8591_dac(255);}
	else{pcf8591_dac(51+(204/(80-himid_can))*(himid-himid_can));}
	
}