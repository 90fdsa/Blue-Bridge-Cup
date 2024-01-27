#include "led.h"

unsigned char led1,led2,led3,led4,led5,led6;
unsigned char qi1,qi2,qi3;
unsigned char aux1,aux2,aux3;
//测量量
extern unsigned int freq,frequency;
extern unsigned char himid,distance;
//参数
extern unsigned char himid_can;
extern unsigned char distance_can;
extern unsigned int frequence_can;
extern unsigned char show_mod,show_sequence;

void led_pan()
{
		if(show_mod==0){led1=1;}else{led1=0;}
		if(show_mod==1){led2=1;}else{led2=0;}
		if(show_mod==2){led3=1;}else{led3=0;}
		if(frequency>frequence_can){led4=1;}else{led4=0;}
		if(himid>himid_can){led5=1;}else{led5=0;}
		if(distance>distance_can){led6=1;}else{led6=0;}
		if(show_mod==3&show_sequence==0){qi1=1;}else{qi1=0;}
		if(show_mod==3&show_sequence==1){qi2=1;}else{qi2=0;}
		if(show_mod==3&show_sequence==2){qi3=1;}else{qi3=0;}
}
void led()
{
		unsigned char c;
	  if(led1==1|aux1!=0){c|=0x01;}else{c&=0xfe;}
		if(led2==1|aux2!=0){c|=0x02;}else{c&=0xfd;}
		if(led3==1|aux3!=0){c|=0x04;}else{c&=0xfb;}
		if(led4==1){c|=0x08;}else{c&=0xf7;}
		if(led5==1){c|=0x10;}else{c&=0xef;}
		if(led6==1){c|=0x20;}else{c&=0xdf;}
		c&=0xbf;c&=0x7f;
		
		P0=~c;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}
