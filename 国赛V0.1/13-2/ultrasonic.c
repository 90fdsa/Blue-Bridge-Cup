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

void Send_Wave()                 //����8��40KHx�������ź�
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
	
	Send_Wave();		        //���ͳ������ź�
	
  CH=0;
  CL=0;
  CR=1;
  while((RX)&&(CF==0));
  CR=0;
	
  if(CF)   //��ʱ���
  {
	   CF = 0; //�����־λ		
	   dis = 255;
  }
  else  //RX�������Ž��յ��͵�ƽ
  {
	  dis = ((CH<<8)+CL)*0.017;
  }
	
  return dis;
	}


