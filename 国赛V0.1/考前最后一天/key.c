#include "key.h"

unsigned char key_value,old_key_value;

unsigned char read_key_juzhen()
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
			case 9:break;
			case 10:break;
			case 13:break;
			case 14:break;
		}	
}

//sbit S7=P3^0;
//sbit S6=P3^1;
//sbit S5=P3^2;
//sbit S4=P3^3;
//unsigned char read_key_duli()
//{
//	  unsigned char key;
//		if(!S7){key=4;}
//		else if(!S6){key=3;}
//		else if(!S5){key=2;}
//		else if(!S4){key=1;}
//		else{key=0;}
//		return key;
//}