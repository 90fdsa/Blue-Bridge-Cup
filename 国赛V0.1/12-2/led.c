#include "led.h"

extern unsigned char qiehuan,shuju,canshu,jilu;
extern unsigned char sonic_mod_flag;
extern unsigned char adc_value;

void led()
{
		unsigned char a;
		if(shuju==0){a|=0x01;}
		else{a&=0xfe;}
		
		if(shuju==1){a|=0x02;}
		else{a&=0xfd;}
		
		if(shuju==2){a|=0x04;}
		else{a&=0xfb;}
		
		if(sonic_mod_flag==0){a|=0x08;}
		else{a&=0xf7;}
		
		if(adc_value<=20){a|=0x20;}
		else{a&=0xdf;}
		
		P0=~a;
		P2=(P2&0X1F)|0X80;
	  P2&=0x1f;
}