#include "ds1302.h"  									

//unsigned char write_address[]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
//unsigned char read_address[]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};


//写字节
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//向DS1302寄存器写入数据
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//从DS1302寄存器读出数据
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

void set_time(unsigned char hour,unsigned char minute,unsigned char second)
{
	  
	  hour=hour/10*16+hour%10;
	  minute=minute/10*16+minute%10;
	  second=second/10*16+second%10;
	  
	  Write_Ds1302_Byte(0x8e,0x00);
		Write_Ds1302_Byte(0x84,hour);
	  Write_Ds1302_Byte(0x82,minute);
	  Write_Ds1302_Byte(0x80,second);
	  Write_Ds1302_Byte(0x8e,0x80);
	  
}