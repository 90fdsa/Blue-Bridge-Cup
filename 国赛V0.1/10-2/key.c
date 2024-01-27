#include "key.h"
#include "iic.h"

unsigned char key_value,old_key_value;
extern unsigned char show_mod,show_sequence;
extern unsigned char dac_start_flag;
//传过来的参数值
extern char temperature_parameter;
extern char distance_parameter;
extern char old_temperature_parameter;
extern char old_distance_parameter;
extern unsigned int change_time_of_parameter;

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
			case 9:
			{
				show_mod++;
        show_sequence=0;
        if(show_mod==2)
        {
				  show_mod=0;
					if((old_temperature_parameter!=temperature_parameter)|(old_distance_parameter!=distance_parameter))
          {
						change_time_of_parameter++;
						write_eeprom(0x00,change_time_of_parameter);
						old_temperature_parameter=temperature_parameter;
						old_distance_parameter=distance_parameter;
					}						
				}
      }break;
			case 13:
			{
			   switch(show_mod)
				 {
					 case 0:show_sequence++;if(show_sequence==3){show_sequence=0;}break;
					 case 1:show_sequence++;if(show_sequence==2){show_sequence=0;}break;
				 }
			}break;
			case 10:
			{
					if(show_mod==1&show_sequence==0)
          {
					   temperature_parameter+=2;
						 if(temperature_parameter>99){temperature_parameter=0;}
					}
					if(show_mod==1&show_sequence==1)
          {
					   distance_parameter+=5;
						 if(distance_parameter>99){distance_parameter=0;}
					}
			}break;
			case 14:
			{
			    if(show_mod==1&show_sequence==0)
          {
					   temperature_parameter-=2;
						 if(temperature_parameter<0){temperature_parameter=99;}
					}
					if(show_mod==1&show_sequence==1)
          {
					   distance_parameter-=5;
						 if(distance_parameter<0){distance_parameter=99;}
					}
			}break;
		}
}

void key_proc_long()
{
		switch(key_value)
		{
			case 10:
			{
				dac_start_flag++;if(dac_start_flag==2){dac_start_flag=0;}
			}break;
			case 14:
			{
				change_time_of_parameter=0;
			}break;
			case 15:
			{
				change_time_of_parameter=0;
				write_eeprom(0x00,0);
			}break;	
		}
}