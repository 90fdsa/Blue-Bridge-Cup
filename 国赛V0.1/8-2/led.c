#include "led.h"

unsigned char led1,led7,led8,aux8;
extern unsigned char show_mod;

void led_pan()
{
		if(show_mod==1){led7=1;}else{led7=0;}
		if(show_mod==2){led8=1;}else{led8=0;}
}
void led()
{
		unsigned char c;
	
	  if(led1!=0){c&=0xfe;}else{c|=0x01;}
	  if(led7==1){c&=0xbf;}else{c|=0x40;}
		if(led8==1){c&=0x7f;}else{c|=0x80;}
		
		P0=c;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}