#include "key.h"
#include "iic.h"

extern unsigned char show_mod,show_sequence;
extern float voltage,temperature;
extern unsigned int frequency;
extern float voltage_standard;

unsigned char key_value,old_key_value;


unsigned char read_keyboard()
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
			case 13:
			{
					if(show_mod==0|show_mod==1)
					{
							show_sequence++;
						  if(show_sequence==3){show_sequence=0;}
					}
			}break;
			case 14:
			{
					if(show_mod==0){show_mod=1;}
					if(show_mod==2)
          {
					   voltage_standard+=0.1;
						 if(voltage_standard>5){voltage_standard=0.1;}
					}
			}break;
			case 10:
			{
			    show_mod&=0xfe;
				  show_mod^=0x02;
			}break;
			case 9:
			{
			    eeprom_write(0x00,(char)(voltage*10));Delay5ms();
				  eeprom_write(0x01,(char)(frequency/256));Delay5ms();
				  eeprom_write(0x02,(char)(frequency%256));Delay5ms();
				  eeprom_write(0x03,((int)(temperature*100))/256);Delay5ms();
				  eeprom_write(0x04,((int)(temperature*100))%256);
			}break;
		}
}

void key_proc_long()
{
		switch(key_value)
		{
			case 14:
			{
					if(show_mod==2)
					{
					   voltage_standard+=0.1;
						 if(voltage_standard>5){voltage_standard=0.1;}
					}
			}break;
		}
}