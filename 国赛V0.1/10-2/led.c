#include "led.h"

extern char temperature_parameter;
extern char distance_parameter;

extern float true_temperature;
extern unsigned char distance;
extern unsigned char dac_start_flag;

unsigned char led1,led2,led3;

void led_pan()
{
		if(true_temperature>temperature_parameter){led1=1;}else{led1=0;}
		if(distance>distance_parameter){led2=1;}else{led2=0;}
		if(dac_start_flag==1){led3=1;}else{led3=0;}
}
void led()
{
		unsigned char c;
	  if(led1==1){c|=0x01;}else{c&=0xfe;}
		if(led2==1){c|=0x02;}else{c&=0xfd;}
		if(led3==1){c|=0x04;}else{c&=0xfb;}
		
		P0=~c;
	  P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}