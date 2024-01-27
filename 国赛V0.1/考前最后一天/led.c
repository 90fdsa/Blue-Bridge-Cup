#include "led.h"

unsigned char led1;

void led()
{
	  unsigned char c;
		if(led1!=0){c&=0xfe;}else{c|=0x01;}
		
		P0=~c;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}