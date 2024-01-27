#include "key.h"

extern unsigned char key_value;
extern unsigned char show_mod,show_sequence;
extern unsigned char led_can;
extern char shi_can,tem_can;

unsigned char read_key()
{
	  unsigned char key;
	  unsigned int dai;
	
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  dai=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  dai=(dai<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  dai=(dai<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  dai=(dai<<4)|(P3&0x0f);
	
	  switch(~dai)
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
			case 13://s4
			{
					show_mod++;
				  show_sequence=0;
				  if(show_mod==2){show_mod=0;}
			}break;
			case 9:show_sequence++;if(show_sequence==3){show_sequence=0;}break;//s5
			case 14://s8
			{
				  if(show_mod==1)
					{
							switch(show_sequence)
							{
								case 0:shi_can--;if(shi_can<0){shi_can=23;}break;
								case 1:tem_can--;if(tem_can<0){tem_can=99;}break;
								case 2:led_can--;if(led_can<4){led_can=8;}break;
							}
					}
			}break;
			case 10://s9
			{
				  if(show_mod==1)
					{
							switch(show_sequence)
							{
								case 0:shi_can++;if(shi_can>23){shi_can=0;}break;
								case 1:tem_can++;if(tem_can>99){tem_can=0;}break;
								case 2:led_can++;if(led_can>8){led_can=4;}break;
							}
					}
			}break;
		}
}