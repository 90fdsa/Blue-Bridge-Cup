/*******************************************************************************  
* 文件名称：矩阵键盘实验
* 实验目的：1.掌握矩阵键盘扫描的基本原理
*           2.掌握按键扫描及消除抖动的基本原理
* 实验配置：J3跳线配置为IO方式，J5配置为KBD、J2配置为1-3和2-4
* 实验现象：按下按键，数码管显示在0~15之间变化
* 硬件说明：IAP15F2K61S2@11.0592MHz
* 程序说明：1.转接板上使用P4.4和P4.2代替P3.6 P3.7
* 			2.采用状态法进行按键扫描，系统实时性好，
* 			且每10ms扫描可以防止抖动。
* 日期版本：2019-6-24/V2.0
*******************************************************************************/
#include "STC15F2K60S2.H"  //用此头文件直接代替REG52.H，请进行两个头文件对比
#include "absacc.h"

                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区
sbit TX = P1^0;   //接收引脚
sbit RX = P1^1;   //发射引脚

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0xFF;//按键值
unsigned char ui_Sonic_flag;
unsigned char uc_Dist; 
/******函数声明***********************/
void read_keyboard(void);
void display();
unsigned char Sonic(void);
void Sonic_Proc(void);
/************************************/

void cls_buzz(void)	//关闭蜂鸣器
{
	P2 = (P2&0x1F|0xA0);
	P0 = 0x00;
	P2 &= 0x1F;
}

void cls_led(void)//关闭所有LED
{
    P2 = ((P2&0x1f)|0x80);
	P0 = 0xff;  
	P2 &= 0x1f;
}

void Timer0Init(void)		//12微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0停止计时
}

void Timer1Init(void)		//12微秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;    //打开定时器中断
}

//主函数
void main(void)
{  
	  cls_buzz();
	  cls_led();
    Timer0Init();
	  Timer1Init();
	  EA=1;
    while(1)
    {   
			  Sonic_Proc();
       	dspbuf[5]=uc_Dist/100;
			  dspbuf[6]=uc_Dist%100/10;
			  dspbuf[7]=uc_Dist%10;
    }
}

//定时器中断服务函数
void isr_timer_1(void)  interrupt 3  //默认中断优先级 1
{     
	static unsigned int intr = 0;
	if(++intr==300)
	{
		intr=0;
		ui_Sonic_flag = 1;   //超声波刷新频率 300ms一次
	}
	display(); //1ms执行一次
}

//读取矩阵键盘键值:转接板中使用P42和P44代替8051引脚
//顺序中的P36和P37引脚
void read_keyboard(void)
{
    static unsigned char hang;
	static unsigned char key_state=0;	
	switch(key_state)
	{
		case 0:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //有按键按下
			key_state=1;	
		}break;
		case 1:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //有按键按下
			{
				if(P30 == 0)hang = 1;
				if(P31 == 0)hang = 2;
				if(P32 == 0)hang = 3;
				if(P33 == 0)hang = 4;//确定行	    
				switch(hang){
					case 1:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=0;key_state=2;}
						else if(P42 == 0) {key_value=1;key_state=2;}
						else if(P35 == 0) {key_value=2;key_state=2;}
						else if(P34 == 0) {key_value=3;key_state=2;}
					}break;
					case 2:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=4;key_state=2;}
						else if(P42 == 0) {key_value=5;key_state=2;}
						else if(P35 == 0) {key_value=6;key_state=2;}
						else if(P34 == 0) {key_value=7;key_state=2;}
					}break;
					case 3:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=8;key_state=2;}
						else if(P42 == 0) {key_value=9;key_state=2;}
						else if(P35 == 0) {key_value=10;key_state=2;}
						else if(P34 == 0) {key_value=11;key_state=2;}
					}break;
					case 4:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=12;key_state=2;}
						else if(P42 == 0) {key_value=13;key_state=2;}
						else if(P35 == 0) {key_value=14;key_state=2;}
						else if(P34 == 0) {key_value=15;key_state=2;}
					}break;
				}	
			}
			else
			{
				key_state=0;	
			}  	   
		}break;
		case 2:     
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 == 0x0f) //按键放开
			key_state=0;	
		}break;
						
    } 
	
}


//显示函数
void display(void)
{   
	static unsigned char dspcom = 0;
	P0 = 0xff;
	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;

	P0 = 1<<dspcom;
	P2 = ((P2&0x1f)|0xC0); 
	P2 &= 0x1f;

	P0 = tab[dspbuf[dspcom]];	
  	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  
    if(++dspcom == 8){
        dspcom = 0;
    }    
}

unsigned char Sonic(void)
{
  unsigned char Dist=0,num=10;
	
  //TX引脚发送10个频率为40MHz方波
  Timer0Init();  //初始化定时器0，初始化定时器初值
  TX = 0;        //发射引脚拉为低电平
  
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
	   Dist = 255;
  }
  else  //RX接收引脚接收到低电平
  {
	  Dist = ((TH0<<8)+TL0)*0.017;
  }
	
  return Dist;
}

void Sonic_Proc(void)
{
  if(ui_Sonic_flag==1)
	{
			ui_Sonic_flag = 0;
		  uc_Dist = Sonic();  //测距
	}
}