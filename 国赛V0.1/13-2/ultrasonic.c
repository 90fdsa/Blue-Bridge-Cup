#include "ultrasonic.h"
#include "init.h"

sbit TX=P1^0;
sbit RX=P1^1;

unsigned char qidong_flag;

void Delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

void Send_Wave()                 //产生8个40KHx超声波信号
{
	unsigned char i;
	for(i = 0; i < 10; i++)
	{
		TX = 1;
		Delay13us();	
		TX = 0;
		Delay13us();
	}
}

unsigned char sonic(void)
{
	//unsigned int time = 0;
	unsigned char dis=0;
	
	Send_Wave();		        //发送超声波信号
	
  CH=0;
  CL=0;
  CR=1;
  while((RX)&&(CF==0));
  CR=0;
	
  if(CF)   //定时溢出
  {
	   CF = 0; //清除标志位		
	   dis = 255;
  }
  else  //RX接收引脚接收到低电平
  {
	  dis = ((CH<<8)+CL)*0.017;
  }
	
  return dis;
	}


