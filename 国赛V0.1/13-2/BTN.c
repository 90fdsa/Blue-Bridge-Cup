#include "BTN.h"

unsigned char key_value,old_key_value;

extern unsigned char show_state;
extern unsigned char show_canshu;
extern unsigned char distance_huan;
extern unsigned char freq_huan;

extern unsigned int fre_standard;
extern unsigned char him_standard;
extern unsigned char dis_standard;

sbit S7  = P3^0;
sbit S6  = P3^1;
sbit S5  = P3^2;
sbit S4  = P3^3;

unsigned char read_key()
{
	  unsigned char key;
		if(S7==0){key=1;}
		else if(S6==0){key=2;}
		else if(S5==0){key=3;}
		else if(S4==0){key=4;}
		else{key=0;}
		
		return key;
}

void key_proc()
{
    switch(key_value)
		{
			case 4:
			{
				  show_state++;
				  if(show_state==4){show_state=0;}
			}break;
			case 3:
			{
					if(show_state==3)
					{
							show_canshu++;
						  if(show_canshu==3){show_canshu=0;}
					}
			}break;
			case 2:
			{
					if(show_state==2){distance_huan++;if(distance_huan==2){distance_huan=0;}}
					if(show_state==3)
					{
							switch(show_canshu)
							{
								case 0:fre_standard+=500;if(fre_standard==12500){fre_standard=1000;}break;
								case 1:him_standard+=10;if(him_standard==70){him_standard=10;}break;
								case 2:dis_standard+=10;if(dis_standard==130){dis_standard=10;}break;
							}
					}
			}break;
			case 1:
			{
					if(show_state==0){freq_huan++;if(freq_huan==2){freq_huan=0;}}
					if(show_state==3)
					{
							switch(show_canshu)
							{
								case 0:fre_standard-=500;if(fre_standard==500){fre_standard=12000;}break;
								case 1:him_standard-=10;if(him_standard==0){him_standard=60;}break;
								case 2:dis_standard-=10;if(dis_standard==0){dis_standard=120;}break;
							}
					}
			}break;
		}			
}