#include "onewire.h"

//单总线内部延时函数
void Delay_OneWire(unsigned int t)  
{
	unsigned char a;
	while(t--)
	{
			for(a=0;a<12;a++);
	}
}

//单总线写操作
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

//单总线读操作
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

//DS18B20初始化
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

float rd_temperature(void)
{
	  unsigned char low,high;
	  unsigned int st;
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
	  
	  st=high&0x0f;
	  st=st<<8;
	  st=st|low;
	  tem=st*0.0625;
	
	  return tem;
}
