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

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0;//按键值
unsigned char led_1,led_2,led_3,Led_Flag;
unsigned char flag;
unsigned int flagnum=0;
/******函数声明***********************/
void display();
void Key_Scan(); 
void key_proc();
void led_show();
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

//主函数
void main(void)
{  
	cls_buzz();
	cls_led();
	AUXR |= 0x80;	//1T模式，IAP15F2K61S2单片机特殊功能寄存器	
	TMOD &= 0xF0;
	TL0 = 0xCD;	
	TH0 = 0xD4;	
	TF0 = 0;	
	TR0 = 1;
	ET0 = 1;
	EA = 1;   
    while(1)
    {
				dspbuf[7] = key_value%10;
				dspbuf[6] = key_value/10;
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{     
	static unsigned char intr = 0;

	display(); //1ms执行一次
	if(++intr == 10) 
	{
     intr = 0;
     Key_Scan();
  }
	if(++flagnum==2000)
  {flagnum=0;flag=1;}
}

void Key_Scan(void)
{
		unsigned char key,hang,lie,keyvalue,yi=0;
		P3&=0X0F;P42=0;P44=0;key=P3&0X0F;
		if(key!=0x0f)
		{
	     switch(key)
       { 
					case 0x0e: hang=4;break;
          case 0x0d: hang=3;break;
				  case 0x0b: hang=2;break;
          case 0x07: hang=1;break;
			 }
    }
    P3|=0XF0;P42=1;P44=1;P36=P42;P37=P44;key=P3&0xf0;
    if(key!=0xf0)
    {
       switch(key)
       {
          case 0xe0: lie=4;break;
          case 0xd0: lie=3;break;
          case 0xb0: lie=2;break;
          case 0x70: lie=1;break;
       }
		}
    keyvalue=lie*4+hang-1;key_value=keyvalue;
    P3=0X0F;P42=0;P44=0;key=P3&0X0F; //松手检测
    flagnum=0;flag=0;yi=0;
    while(P3!=0X0F)
    {
			 if(flag==1&&yi==0)
       { 
			     switch(keyvalue)
			     {case  1:   break;   }   
			 }  //长按
    }
    if(flag==0)
    { 
				switch(keyvalue)
				{case  1:   break;   }  
		}   //短按

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