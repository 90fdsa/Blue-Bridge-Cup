#include "key.h"
#include "iic.h"

extern unsigned char jilu_distance[10];
extern unsigned char key_value,old_key_value;
extern unsigned char show_mod;
unsigned char aux1,aux2,aux3;//辅助切换
unsigned char start_distance,end_distance;//测测你的
extern unsigned char old_distance_mod;//加和模式或过去模式
extern unsigned char distance_standard;//标准值
unsigned char jilu_count=0;
unsigned char reme_count;
//测量值
extern unsigned char distance;

unsigned char read_keyboard()
{
		unsigned char key;
	  unsigned int a;
	  P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  a=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  a=(a<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  a=(a<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  a=(a<<4)|(P3&0x0f);
	
	  switch(~a)
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
			   aux1=~aux1;
         if(aux1!=0){show_mod=1;}
				 else{show_mod=0;}
			}break;
			case 10:
			{
				 aux2=~aux2;
         if(aux2!=0){show_mod=2;}
				 else{show_mod=0;}
			}break;
			case 13:
			{
				 start_distance=1;
				 if(jilu_count<10)
         {
					  jilu_distance[jilu_count]=distance;
				    eeprom_write(jilu_count,distance);Delay5ms();
				 }
				 else if(jilu_count>=10)
				 {
					  unsigned char i,zan;
						for(i=0;i<9;i++)
            {  
							 jilu_distance[i]=jilu_distance[i+1];
							 zan=eeprom_read(i+1);
							 Delay5ms();
							 eeprom_write(i,zan);
						   Delay5ms();
						}
						jilu_distance[9]=distance;
						eeprom_write(0x09,distance);Delay5ms();
				 }
				 jilu_count++;
				 eeprom_write(0x10,jilu_count);Delay5ms();
				 eeprom_write(0x11,distance_standard);
			}break;
			case 14:
			{
				  if(show_mod==0)
					{
							aux3=~aux3;
						  if(aux3==0){old_distance_mod=0;}
					    else{old_distance_mod=1;}
					}
				  if(show_mod==1)
          {
					    reme_count++;
						  if(reme_count==10){reme_count=0;}
					}
					if(show_mod==2){distance_standard+=10;if(distance_standard>90){distance_standard=0;}}
			}break;
		}
}