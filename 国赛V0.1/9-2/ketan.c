#include "stc15f2k60s2.h"
#include "init.h"
#include "key.h"
#include "show.h"
#include "iic.h"
#include "onewire.h"
#include "led.h"

//标记
extern unsigned char led4,aux4;
extern unsigned char key_value,old_key_value;
unsigned char done=0;
unsigned char kuai=0;
unsigned char temperature_start_flag;
//值
float voltage,temperature;
unsigned int freq,frequency;
//记录值
unsigned char voltage_record;
unsigned int temperature_record,frequency_record;
//标准值
float voltage_standard=2;

void temperature_start();
void read_value();

void main()
{
    close_buzz();
    close_led();
	  read_value();
	  pcf8591_init();
	  Timer0Init();
    Timer1Init();
	  EA=1;
	  while(1)
		{
		    fen();
			  temperature_start();
			  led_pan();
		}
}

void timer0_proc() interrupt 1
{
   freq++;
}

void timer1_proc() interrupt 3
{
		static unsigned char a,ad,blink;
	  static unsigned int fre,chang,zeng;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  if(old_key_value<key_value){key_proc();done=1;}
				if(old_key_value>key_value){done=0;kuai=0;}
				led();
		}
		if(++ad==50)
		{
				ad=0;
        voltage=pcf8591_adc()*0.0196;
		}
		if(++fre==500)
		{
				fre=0;
			  frequency=freq*2;
			  freq=0;
			  temperature_start_flag=1;
		}
		if(key_value!=0)
		{
				chang++;
			  if(chang==800&done==1)
				{
					  chang=0;
						key_proc_long();
					  kuai=1;
				}
		}
		if(kuai==1)
		{
			  if(++zeng==200)
				{
						zeng=0;
					  key_proc_long();
				}
		}
		if(led4==1)
		{
				blink++;
			  if(blink==200)
				{
						blink=0;
					  aux4=~aux4;
				}
		}
		else{aux4=0;blink=0;}
}

void temperature_start()
{
		if(temperature_start_flag==1)
		{
				temperature_start_flag=0;
			  temperature=read_temperature();
		}
}

void read_value()
{
	  unsigned char high,low;
		voltage_record=eeprom_read(0x00);Delay5ms();
	  high=0;low=0;
	  high=eeprom_read(0x01);Delay5ms();
	  low=eeprom_read(0x02);Delay5ms();
	  frequency_record=high*256+low;
	  high=0;low=0;
	  high=eeprom_read(0x03);Delay5ms();
	  low=eeprom_read(0x04);Delay5ms();
	  temperature_record=high*256+low;
}