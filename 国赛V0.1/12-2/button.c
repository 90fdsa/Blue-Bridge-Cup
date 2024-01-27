#include "button.h"

extern unsigned char key_value;
extern unsigned char old_key_value;

extern unsigned char qiehuan,shuju,canshu,jilu;
extern unsigned char sonic_mod_flag;
extern unsigned char miaoshu;
extern unsigned char dis_canshu;

unsigned char read_key()
{
	unsigned char key;
	unsigned int zan;
	
	P3|=0x0f;
  P44=0;P42=1;P35=1;P34=1;
  zan=P3&0x0f;
  P44=1;P42=0;P35=1;P34=1;
  zan=(zan<<4)|(P3&0x0f);		 
	P44=1;P42=1;P35=0;P34=1;
  zan=(zan<<4)|(P3&0x0f);
	P44=1;P42=1;P35=1;P34=0;
  zan=(zan<<4)|(P3&0x0f);
	
	switch(~zan)
	{
		case 0x1000:key=1;break;
		case 0x0100:key=2;break;
		case 0x0010:key=3;break;
		case 0x0001:key=4;break;
		case 0x2000:key=5;break;
		case 0x0200:key=6;break;
		case 0x0020:key=7;break;
		case 0x0002:key=8;break;
		case 0x4000:key=9;break;
		case 0x0400:key=10;break;
		case 0x0040:key=11;break;
		case 0x0004:key=12;break;
		case 0x8000:key=13;break;
		case 0x0800:key=14;break;
		case 0x0080:key=15;break;
		case 0x0008:key=16;break;
		default:key=0;break;
	}
	
	return key;
}

void key_proc()
{
		switch(key_value)
		{
			case 13:qiehuan++;if(qiehuan==2){qiehuan=0;}break;
			case 9:
			{
					if(qiehuan==0){shuju++;if(shuju==3){shuju=0;}}
					if(qiehuan==1){canshu++;if(canshu==2){canshu=0;}}
			}break;
			case 14:
			{
					if(qiehuan==0&&shuju==1){sonic_mod_flag=~sonic_mod_flag;}
					if(qiehuan==0&&shuju==2){jilu++;if(jilu==3){jilu=0;}}
			}break;
			case 10:
			{
					if(qiehuan==1&&canshu==0){miaoshu++;if(miaoshu==5){miaoshu=0;}}
					if(qiehuan==1&&canshu==1){dis_canshu+=10;if(dis_canshu==90){dis_canshu=10;}}
			}
		}
}