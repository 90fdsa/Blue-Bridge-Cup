#include "led.h"

extern unsigned int frequency;
extern float him;
extern unsigned char distance;

//extern unsigned int xdata frequency;
//extern unsigned char xdata adc_value;
//extern unsigned char xdata distance;

extern unsigned int fre_standard;
extern unsigned char him_standard;
extern unsigned char dis_standard;

extern unsigned char show_state;
extern unsigned char show_canshu;

unsigned char led1_chang,led2_chang,led3_chang,led4_chang,led5_chang,led6_chang;
unsigned char led1_shan,led2_shan;led3_shan;
unsigned char aux1,aux2,aux3;


void tiaojian()
{
		if(show_state==0){led1_chang=1;}else{led1_chang=0;}
		if(show_state==3&&show_canshu==0){led1_shan=1;}else{led1_shan=0;}
		
		if(show_state==1){led2_chang=1;}else{led2_chang=0;}
		if(show_state==3&&show_canshu==1){led2_shan=1;}else{led2_shan=0;}
		
		if(show_state==2){led3_chang=1;}else{led3_chang=0;}
		if(show_state==3&&show_canshu==2){led3_shan=1;}else{led3_shan=0;}
		
		if(frequency>fre_standard){led4_chang=1;}else{led4_chang=0;}
		if(him>him_standard){led5_chang=1;}else{led5_chang=0;}
		if(distance>dis_standard){led6_chang=1;}else{led6_chang=0;}
}
void led()
{
	  unsigned char c;
		if(led1_chang==1|aux1!=0){c|=0x01;}else{c&=0xfe;}
		if(led2_chang==1|aux2!=0){c|=0x02;}else{c&=0xfd;}
		if(led3_chang==1|aux3!=0){c|=0x04;}else{c&=0xfb;}
		if(led4_chang==1){c|=0x08;}else{c&=0xf7;}
		if(led5_chang==1){c|=0x10;}else{c&=0xef;}
		if(led6_chang==1){c|=0x20;}else{c&=0xdf;}
		
		P0=~c;
		P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}