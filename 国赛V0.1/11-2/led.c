#include "led.h"
#include "ds1302.h"

unsigned char ho;
unsigned char led1,led2,led3;
unsigned char led3_aux;

extern float temperature,voltage;
extern unsigned char liangmie;
extern unsigned char led_can;
extern char shi_can,tem_can;



void led_pan()
{
		ho=Read_Ds1302_Byte(0x85);
	  ho=ho/16*10+ho%16;
	  if(ho>=shi_can|ho<8){led1=1;}else{led1=0;}
		if(temperature>tem_can){led2=1;}else{led2=0;}
		if(liangmie==1){led3=1;}
}

void led_run()
{
	  unsigned char a;
		if(led1==1){a|=0x01;}else{a&=0xfe;}
		if(led2==1){a|=0x02;}else{a&=0xfd;}
		if(led3_aux==1){a|=0x04;}else{a&=0xfb;}
		if(led_can==4&liangmie==1){a|=0x08;}else{a&=0xf7;}
		if(led_can==5&liangmie==1){a|=0x10;}else{a&=0xef;}
		if(led_can==6&liangmie==1){a|=0x20;}else{a&=0xdf;}
		if(led_can==7&liangmie==1){a|=0x40;}else{a&=0xbf;}
		if(led_can==8&liangmie==1){a|=0x80;}else{a&=0x7f;}
		
		P0=~a;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}