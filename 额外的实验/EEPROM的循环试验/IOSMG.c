/*******************************************************************************  
* 文件名称：DS18B20温度传感器实验
* 实验目的：1.掌握单总线通讯基本特点和工作时序
*           2.掌握51单片机模拟单总线时序的程序设计方法
*           3.掌握DS18B20温度传感器的操作方法
* 实验配置：J3跳线配置为IO方式，J5配置为BTN、J2配置为1-3和2-4
* 程序现象：用手触摸温度传感器，数码管显示有变化
* 硬件说明：IAP15F2K61S2@11.0592MHz
* 注意事项：测量精度为1摄氏度
* 日期版本：2019-6-24/V2.0
*******************************************************************************/

//#include "reg52.h"  //定义51单片机特殊功能寄存器
#include "absacc.h"
#include "i2c.h"
#include "STC15F2K60S2.H"

//sfr AUXR = 0x8E;

 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//显示缓冲区

unsigned char start=1;
unsigned char key_value=0;
unsigned char up;

//函数声明
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void delay();
void read_keyboard(void);
void key_proc();
/**********************************/

//主函数
void main(void)
{ 
    unsigned int a;
		cls_buzz();
		cls_led();   
//		AUXR |= 0xC0;
//		TMOD &= 0xFF;
//		TL0 = 0xCD;	//1ms,1Tmoshi
//		TH0 = 0xD4;	
//		TF0 = 0;	
//		TR0 = 1;
//		ET0 = 1; 
	
		
		
	  
		EA=0;
	  for(a=0;a<255;a++)
	  {
				write_eeprom(a,0);
			  delay();	//延时10ms
		}
		  //向AT24C02地址0x00中写入数据
		
	
//		start++;
//	
//		start = read_eeprom(0x00);  //从AT24C02地址0x00中读取数据
//	  delay();	//延时10ms
		
		EA=1;
	
    while(1)
    {
			//显示数据更新 
			
//			read_keyboard();
//			key_proc();
//			dspbuf[0]=stop/100;
//			dspbuf[1]=stop%100/10;      
//			dspbuf[2]=stop%100%10;
			dspbuf[2]=1;
			dspbuf[3]=1;      
			dspbuf[4]=4;
			dspbuf[5]=5;
			dspbuf[6]=1;      
			dspbuf[7]=4; 
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
//		static unsigned int a;
//		if(++a==1000)
//		{
//				a=0;
//				start++;
//				write_eeprom(0x00,start);  //向AT24C02地址0x00中写入数据
//				delay();	//延时10ms
//		}
		display();
}

void cls_buzz()
{
	P2 = ((P2&0x1f)|0xA0); 
	P0 = 0x00;
	P2 &= 0x1f;
}
void cls_led()
{
	P2 = ((P2&0x1f)|0x80); 
	P0 = 0xFF;
	P2 &= 0x1f;
}

void delay()//10ms
{
		unsigned char i, j;

		i = 108;
		j = 145;
		do
		{
			while (--j);
		} while (--i);
}
//显示函数
void display(void)
{   
	static unsigned char dspcom = 0;
	P2 = ((P2&0x1f)|0xE0);   //段选573
	P0 = 0xff;               //消隐段码
	P2 &= 0x1f; 

	P0 = 1<<dspcom;	         //循环位选
	P2 = ((P2&0x1f)|0xC0);   //位选573
	P2 &= 0x1f;
	
	P0 = tab[dspbuf[dspcom]];//显示段码
  P2 = ((P2&0x1f)|0xE0);   //段选573
	P2 &= 0x1f;               
	
    if(++dspcom == 8){
        dspcom = 0;
    }    
}
