#include "onewire.h"

sbit DQ=P1^4;
//
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

float temperature_massure()
{
		unsigned char low,high;
	  unsigned int jie;
	  float tem;
	
	  init_ds18b20();
	  Write_DS18B20(0xcc);
	  Write_DS18B20(0x44);
	  Delay_OneWire(200);
	  
	  init_ds18b20();
	  Write_DS18B20(0xcc);
	  Write_DS18B20(0xbe);
	
	  low=Read_DS18B20();
	  high=Read_DS18B20();
	  
	  jie=high&0x0f;
	  jie=jie<<8;
	  jie=jie|low;
	  tem=jie*0.0625;
	
	  return tem;
	  
}