#include "STC15F2K60S2.h"
#include "init.h"
#include "show.h"
#include "BTN.h"
#include "iic.h"
#include "ultrasonic.h"
#include "jidianqi.h"
#include "led.h"

extern unsigned char show_state;
extern unsigned char key_value;
extern unsigned char old_key_value;
extern unsigned char qidong_flag;
extern float him;
//实际数据
//xdata unsigned int freq,frequency;
//xdata unsigned char adc_value;
//xdata unsigned char distance;
unsigned int freq,frequency;
unsigned char adc_value;
unsigned char distance;
float him;
//参数
unsigned int fre_standard=9000;
unsigned char him_standard=40;
unsigned char dis_standard=60;
//led辅助
extern unsigned char led1_shan,led2_shan,led3_shan;
extern unsigned char aux1,aux2,aux3;

void sonic_qidong();

void main()
{
		close_buzz();
		close_led();
	  pcf8591_init();
	  Timer0Init_fre();		//1000微秒@12.000MHz
		Timer1Init();
	  PCA_init();
	  EA=1;
	  while(1)
		{
			  sonic_qidong();
			  fen();
			  tiaojian();
		}
}

void sonic_qidong()
{
		if(qidong_flag==1)
		{
				qidong_flag=0;
			  distance=sonic();
			  if(him<=dis_standard){pcf8591_dac(51);}
				if(him>dis_standard&him<80){pcf8591_dac(51+(204/(80-dis_standard))*(him-dis_standard));}
				if(him>=80){pcf8591_dac(255);}
		}
}

void timer0_proc() interrupt 1
{
		freq++;
}

void timer1_proc() interrupt 3
{
	  static unsigned char a;
	  static unsigned int gong;
	  static unsigned char ad;
	  static unsigned int so;
	  static unsigned char blink;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_key();
			  if(old_key_value<key_value){key_proc();}
				//继电器
				if(distance>dis_standard){open_jidian();}
				else{close_buzz();}
				//LED
				led();
		}
		if(++so==200)
		{
				so=0;
	      qidong_flag=1;
		}
		if(++gong==500)
		{
				gong=0;
			  frequency=freq*2;
			  freq=0;
		}
		if(++ad==50)
		{
				ad=0;
			  adc_value=pcf8591_adc();
			  him=adc_value*0.392;
		}
		if(++blink==100)
		{
				blink=0;
			  if(led1_shan==1){aux1=~aux1;}else{aux1=0;}
				if(led2_shan==1){aux2=~aux2;}else{aux2=0;}
				if(led3_shan==1){aux3=~aux3;}else{aux3=0;}
		}
}