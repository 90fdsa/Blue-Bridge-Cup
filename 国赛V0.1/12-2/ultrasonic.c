#include "ultrasonic.h"
#include "init.h"

sbit TX=P1^0;
sbit RX=P1^1;

unsigned char sonic()
{
		unsigned char dis=0,num=10;
	  
	  Timer0Init();
	  TX=0;
	
	  while(num--)  //发送10个
    {
      TR0 = 1;
	    while(!TF0);  //计时时间到，控制周期
	    TF0 = 0;      //手动清除标志位	
	    TX ^= 1;      //取反
      Timer0Init();  //初始化定时器0，初始化定时器初值	
    }


    //接收方波
	  TR0 = 0;		//定时器0停止计时
	  TF0 = 0;		//清除TF0标志
	  TL0 = 0x00;	//设置定时初值
	  TH0 = 0x00;	//设置定时初值
	  TR0 = 1;

    while(RX && !TF0);  //等待收到脉冲	或定时溢出
    TR0 = 0;  //定时器0停止计时
	
    if(TF0)   //定时溢出
    {
	     TF0 = 0; //清除标志位		
	     dis = 255;
    }
    else  //RX接收引脚接收到低电平
    {
	    dis = ((TH0<<8)+TL0)*0.017;
    }
	
    return dis;
}