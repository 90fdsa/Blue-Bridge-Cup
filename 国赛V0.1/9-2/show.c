#include "show.h"

code unsigned char duanma[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0x8e,0xc6,0x89,0x8c};
//                             U    F    C    H    P 
unsigned char buffer[8]={10,10,10,10,10,10,10,10,};
unsigned char weixuan;
unsigned char show_mod,show_sequence;
//主函数传过来的测量值
extern float voltage,temperature;
extern unsigned int freq,frequency;
//主函数传过来的记录值
extern unsigned char voltage_record;
extern unsigned int temperature_record,frequency_record;
//主函数传过来的标准值
extern float voltage_standard;

void display()
{
		P0=0xff;
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  P0=1<<weixuan;
	  P2=(P2&0x1f)|0xc0;
	  P2&=0x1f;
	
	  P0=duanma[buffer[weixuan]];
	  P2=(P2&0x1f)|0xe0;
	  P2&=0x1f;
	
	  if(++weixuan==8){weixuan=0;}
}

void fen()
{
		switch(show_mod)
		{
			case 0:
			{
				 switch(show_sequence)
				 {
					 case 0:voltage_show();break;
					 case 1:frequency_show();break;
					 case 2:temperature_show();break;
				 }
			}break;
			case 1:
			{
				 switch(show_sequence)
				 {
					 case 0:voltage_history_show();break;
					 case 1:frequency_history_show();break;
					 case 2:temperature_history_show();break;
				 }
			}break;
		  case 2:
			{
			    voltage_yu();
			}break;
		}
}
void voltage_show()
{
		buffer[0]=22;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=((int)(voltage*10))/10+11;
	  buffer[7]=((int)(voltage*10))%10;
}
void frequency_show()
{
    buffer[0]=23;buffer[1]=10;buffer[2]=10;
	  (frequency>=10000)?(buffer[3]=frequency/10000):(buffer[3]=10);
	  (frequency>=1000)?(buffer[4]=frequency%10000/1000):(buffer[4]=10);
	  (frequency>=100)?(buffer[5]=frequency%1000/100):(buffer[5]=10);
	  buffer[6]=frequency%100/10;
	  buffer[7]=frequency%10;
}
void temperature_show()
{
		buffer[0]=24;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=((int)(temperature*100))/1000;
	  buffer[5]=((int)(temperature*100))%1000/100+11;
	  buffer[6]=((int)(temperature*100))%100/10;
	  buffer[7]=((int)(temperature*100))%10;
}
void voltage_history_show()
{
		buffer[0]=25;buffer[1]=22;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=voltage_record/10+11;
	  buffer[7]=voltage_record%10;
}
void frequency_history_show()
{
		buffer[0]=25;buffer[1]=23;buffer[2]=10;
	  (frequency_record>=10000)?(buffer[3]=frequency_record/10000):(buffer[3]=10);
	  (frequency_record>=1000)?(buffer[4]=frequency_record%10000/1000):(buffer[4]=10);
	  (frequency_record>=100)?(buffer[5]=frequency_record%1000/100):(buffer[5]=10);
	  buffer[6]=frequency_record%100/10;
	  buffer[7]=frequency_record%10;
}
void temperature_history_show()
{
		buffer[0]=25;buffer[1]=24;buffer[2]=10;buffer[3]=10;
	  buffer[4]=temperature_record/1000;
	  buffer[5]=temperature_record%1000/100+11;
	  buffer[6]=temperature_record%100/10;
	  buffer[7]=temperature_record%10;
}
void voltage_yu()
{
    buffer[0]=26;buffer[1]=10;buffer[2]=10;buffer[3]=10;
	  buffer[4]=10;buffer[5]=10;
	  buffer[6]=((char)(voltage_standard*10))/10+11;
	  buffer[7]=((char)(voltage_standard*10))%10;
}