#include "jidianqi.h"

extern unsigned char distance_can,distance;

void jidianqi_go()
{	
	  unsigned char c;
	  if(distance>distance_can){c|=0x10;}else{c&=0xef;}
		c&=0xbf;
		
		P0=c;
	  P2=(P2&0x1f)|0xa0;
	  P2&=0x1f;
}
