#include "seg.h"


void Seg_Tran(unsigned char *seg_string,unsigned char *seg_buf)
{
	unsigned char i;
	unsigned char j;
	unsigned char temp;
	
	for(i=0,j=0;i<8;i++,j++)
	{
		switch(seg_string[j])
		{
			case '0':temp=0xc0;break;
			case '1':temp=0xf9;break;			
			case '2':temp=0xa4;break;			
			case '3':temp=0xb0;break;			
			case '4':temp=0x99;break;		
			case '5':temp=0x92;break;		
			case '6':temp=0x82;break;		
			case '7':temp=0xF8;break;		
			case '8':temp=0x80;break;
			case '9':temp=0x90;break;
			case 'A':temp=0x88;break;
			case 'B':temp=0x83;break;
			case 'C':temp=0xC6;break;
			case 'D':temp=0xA1;break;
			case 'E':temp=0x86;break;
			
			case 'U':temp=0xC1;break;			
			
			
			case ' ':temp=0xFF;break;			
			case '-':temp=0xBF;break;
			default :temp=0xFF;break;
		}
		
		if(seg_string[j+1]=='.')
		{
			temp&=0x7f;
			j++;
			
		}
		seg_buf[i]=temp;
	}
	
}


void Seg_Disp(unsigned char *seg_buf,unsigned char pos)
{
	P0=0X01<<pos;
	P2=(P2&0x1f)|0xc0;
	P2&=0x1f;
	
	
	P0=seg_buf[pos];
	P2=(P2&0x1f)|0xe0;
	P2&=0x1f;	
	

}




