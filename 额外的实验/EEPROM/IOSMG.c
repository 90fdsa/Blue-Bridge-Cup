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

#include "STC15F2K60S2.H"
//#include "reg52.h"  //定义51单片机特殊功能寄存器
#include "absacc.h"
#include "i2c.h"
#include "intrins.h"

//sfr AUXR = 0x8E;

 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//显示缓冲区
unsigned char key_value;
unsigned char start;

//函数声明
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void delay();
void read_keyboard(void);
void key_proc();
void delay_200ms();		//@11.0592MHz
/**********************************/

//主函数
void main(void)
{ 

		cls_buzz();
		cls_led();   
		AUXR |= 0x80;
		TMOD &= 0xF0;
		TL0 = 0xCD;	//1ms,1Tmoshi
		TH0 = 0xD4;	
		TF0 = 0;	
		TR0 = 1;
		ET0 = 1; 
		
	  start = read_eeprom(0x00);  //从AT24C02地址0x00中读取数据
    start++;
	  delay();	//延时10ms
    write_eeprom(0x00,start);  //向AT24C02地址0x00中写入数据
	  delay();
	
		EA=1;

    while(1)
    {
			//显示数据更新 
			//dspbuf[0]=up;	
//			read_keyboard();
//			key_proc();
//			delay_200ms();delay_200ms();delay_200ms();delay_200ms();delay_200ms();
//			write_eeprom(0x00,start);  //向AT24C02地址0x00中写入数据
//			delay();
			dspbuf[5]=start/100;
			dspbuf[6]=start%100/10;      
			dspbuf[7]=start%100%10; 
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
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

void key_proc()
{
		switch(key_value)
		{
			case 0:
			{
//				start++;
//				write_eeprom(0x00,start);
//				delay();
				key_value=0xff;
				break;
			}
		}
}

void delay_200ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}