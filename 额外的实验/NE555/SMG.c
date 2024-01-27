#include "SMG.h"

void Select573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
			P2 = ( P2 &0X1F ) | 0X80;
		break;
		case 5:
			P2 = ( P2 &0X1F ) | 0Xa0;
		break;		
		case 6:
			P2 = ( P2 &0X1F ) | 0Xc0;
		break;		
		case 7:
			P2 = ( P2 &0X1F ) | 0Xe0;
		break;		
		case 0:
			P2 = ( P2 &0X1F ) | 0X00;
		break;		
	}
	
}


void SMG_bit(unsigned char whe,unsigned char dat)
{
	P0=0x01<<whe;
	Select573(6);
	Select573(0);
	P0=dat;
	Select573(7);
	Select573(0);
	
}

void SMG_delay(unsigned int t)
{
	
	while(t--);

}

