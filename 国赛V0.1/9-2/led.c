#include "led.h"

extern unsigned char show_mod,show_sequence;

extern float voltage,voltage_standard;
unsigned char led1,led2,led3,led4,aux4;

void led_pan()
{
		if(show_mod==0&show_sequence==0){led1=1;}else{led1=0;}
		if(show_mod==0&show_sequence==1){led2=1;}else{led2=0;}
		if(show_mod==0&show_sequence==2){led3=1;}else{led3=0;}
		if(voltage>voltage_standard){led4=1;}else{led4=0;}
}
void led()
{
	  unsigned char c;
		if(led1==1){c|=0x01;}else{c&=0xfe;}
		if(led2==1){c|=0x02;}else{c&=0xfd;}
		if(led3==1){c|=0x04;}else{c&=0xfb;}
		if(aux4!=0){c|=0x08;}else{c&=0xf7;}
		
		P0=~c;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}