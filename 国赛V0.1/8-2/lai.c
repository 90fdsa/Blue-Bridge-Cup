#include "show.h"
#include "key.h"
#include "init.h"
#include "ultrasonic.h"
#include "iic.h"
#include "led.h"

unsigned char jilu_distance[10];
unsigned char key_value,old_key_value;
extern unsigned char start_distance,end_distance;
unsigned char old_distance_mod;
extern unsigned char jilu_count,reme_count;
extern unsigned char led1;
//测量值
unsigned char distance,old_distance;
//标准值
unsigned char distance_standard=20;

void ce_distance();

void main()
{
	  unsigned char b;
		close_buzz();
	  close_led();
	  pcl8591_init();
	  PCAInit();
	  Timer1Init();	
	  for(b=0;b<10;b++)
	  {
				jilu_distance[b]=eeprom_read(b);
			  Delay5ms();
		}
		jilu_count=eeprom_read(0x10);Delay5ms();
		distance_standard=eeprom_read(0x11);Delay5ms();
	  EA=1;
	  while(1)
		{
				fen();
			  ce_distance();
			  led_pan();
		}
}

void timer1_proc() interrupt 3
{
		static unsigned char a;
	  static unsigned char blink;
	  static unsigned char jishu;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  if(old_key_value<key_value){key_proc();}
				//dac
				if(distance<=distance_standard){pcl8591_dac(0);}
				else{pcl8591_dac(distance-distance_standard);}
				//led
				led();
		}
		//led1
		if(end_distance==1)
		{
				if(++blink==200)
				{
						led1=~led1;
					  jishu++;
					  if(jishu==20)
						{
								end_distance=0;
							  jishu=0;
						}
				}
		}
}

void ce_distance()
{
		if(start_distance==1)
		{
				start_distance=0;
			  old_distance=distance;
			  distance=distance_massure();
			  if(old_distance_mod!=0){old_distance+=distance;}
				end_distance=1;
		}
}