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

#include "reg52.h"  //定义51单片机特殊功能寄存器
#include "absacc.h"
#include "i2c.h"

sfr AUXR = 0x8E;
unsigned char adc_value;
unsigned int duty=0;
char up=0;
char k=0;
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//显示缓冲区

//函数声明
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void type1(char i);
/**********************************/

//主函数
void main(void)
{ 
		cls_buzz();
		cls_led();   
		AUXR |= 0xC0;
		TMOD &= 0xFF;
		TL0 = 0xCD;	//1ms,1Tmoshi
		TH0 = 0xD4;	
		TF0 = 0;	
		TR0 = 1;
		ET0 = 1;
		TL1=0XF5;
		TH1=0XFF;
		TF1 = 0;	
		TR1 = 1;
		ET1 = 1;
		EA = 1;
		init_pcf8591();  //PCF8591初始化   
    while(1)
    {
			//显示数据更新 
			//dspbuf[0]=up;
			dspbuf[0]=k;
			dspbuf[5]=adc_value/100;
			dspbuf[6]=adc_value%100/10;      
			dspbuf[7]=adc_value%100%10; 
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
		static unsigned char eihei;
	  static unsigned int time;
		if(++eihei==2)
		{
				eihei=0;
				if(up==0)
				{duty++;}
				else
				{duty--;}
				if(duty==500)
				{up=1;}
				else if(duty==0)
				{up=0;}
		}
		if(++intr == 50)  //1ms执行一次
		{
				intr = 0;
				adc_value = adc_pcf8591();
    }
		if(++time==1000)
		{
			time=0;
			k=k+1;
			if(k==8)
			{k=0;}
		}
		display();
}

void isr_timer_1(void)  interrupt 3  //默认中断优先级 1
{
    static unsigned int intr;
		intr++;
		if(intr <= duty)  //200us高电平
		{
				P2 = ((P2&0x1f)|0x80);
				type1(k);
				P2 &= 0x1f;
    }
		if(intr>duty&&intr<1000)
		{
				P2 = ((P2&0x1f)|0x80);
				P0 = 0xff;  //LED关
				P2 &= 0x1f;
		}
		if(intr==1000)
		{
			 intr=0;
		}
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

void type1(char i)
{
	switch(i)
	{
		case 0:P0=0xfe;break;
		case 1:P0=0xfd;break;
		case 2:P0=0xfb;break;
		case 3:P0=0xf7;break;
		case 4:P0=0xef;break;
		case 5:P0=0xdf;break;
		case 6:P0=0xbf;break;
		case 7:P0=0x7f;break;
	}	
}