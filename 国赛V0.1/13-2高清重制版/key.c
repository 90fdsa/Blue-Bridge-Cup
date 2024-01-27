#include "key.h"

unsigned char key_value,old_key_value;
extern unsigned char show_mod,show_sequence;
unsigned char distance_danwei,frequency_danwei;

extern unsigned char himid_can;
extern unsigned char distance_can;
extern unsigned int frequence_can;



sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;

unsigned char read_key()
{
	  unsigned char key;
		if(!S7){key=1;}
		else if(!S6){key=2;}
		else if(!S5){key=3;}
		else if(!S4){key=4;}
		else{key=0;}
		return key;
}
void key_proc()
{
		switch(key_value)
		{
			case 1:
			{   //frequency
					if(show_mod==0){frequency_danwei=~frequency_danwei;}
					if(show_mod==3)
					{
							switch(show_sequence)
							{
								case 0:frequence_can-=500;if(frequence_can<1000){frequence_can=12000;}break;
								case 1:himid_can-=10;if(himid_can<10){himid_can=60;}break;
								case 2:distance_can-=10;if(distance_can<10){distance_can=120;}break;
							}
					}
			}break;
			case 2:
			{   //distance
					if(show_mod==2){distance_danwei=~distance_danwei;}
					if(show_mod==3)
					{
							switch(show_sequence)
							{
								case 0:frequence_can+=500;if(frequence_can>12000){frequence_can=1000;}break;
								case 1:himid_can+=10;if(himid_can>60){himid_can=10;}break;
								case 2:distance_can+=10;if(distance_can>120){distance_can=10;}break;
							}
					}
			}break;
			case 3:
			{
					if(show_mod==3)
					{
							show_sequence++;
						  if(show_sequence==3){show_sequence=0;}
					}
			}break;
			case 4:{show_mod++;if(show_mod==4){show_mod=0;}}break;
		}
}