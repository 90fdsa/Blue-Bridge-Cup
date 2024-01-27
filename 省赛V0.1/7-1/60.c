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
#include "onewire.h"

sfr AUXR = 0x8E;

sbit S7  = P3^0;
sbit S6  = P3^1;
sbit S5  = P3^2;
sbit S4  = P3^3;
 
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbF,0xc6};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};//显示缓冲区

//函数声明
/**********************************/
void display(void);
void cls_buzz();
void cls_led();
void button(unsigned char a);
void delay();
void key_proc(unsigned char key);
unsigned char read_key(void);
void how_to_display();
/**********************************/

unsigned char second=0;
unsigned char ordre=1;
unsigned char temperature=0;
unsigned int duty=0;
unsigned char shu=0;
char mod=1;

//主函数
void main(void)
{ 
    unsigned char temp;
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
		//init_ds18b20();
    while(1)
    {
			//显示数据更新 
			
			button(temp);
			
//			dspbuf[1]=mod;
//			dspbuf[2]=temp;
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1,用于读秒
{
    static unsigned int intr=0;
		intr++;
		if(intr == 1000)  //1s执行一次
		{
				if(second>0)
				{
						second--;
				}
				else
				{
					duty=0;
				}
				intr = 0;
				temperature=rd_temperature();
    }
		display();
}

void isr_timer_1(void)  interrupt 3  //默认中断优先级 3，用于产生pwm
{
    static unsigned int intr;
		intr++;
		if(intr <= duty)  //200us高电平
		{
				shu=1;
				//intr = 0;
    }
		if(intr>duty&&intr<1000)
		{
				shu=0;
		}
		if(intr==1000)
		{
			 intr=0;
		}
		how_to_display();
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

//延时函数: 10ms@11.0592MHz
void delay(void)
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

void how_to_display()
{
		if(mod==1)
		{
			dspbuf[0]=11;
			dspbuf[1]=ordre;
			dspbuf[2]=11;
			//dspbuf[4]=0;
			dspbuf[5]=second/100;
			dspbuf[6]=second%100/10;      
			dspbuf[7]=second%100%10;
		}
		if(mod==0)
		{
			dspbuf[0]=11;
			dspbuf[1]=ordre;
			dspbuf[2]=11;
			dspbuf[4]=0;
			dspbuf[5]=temperature/10;      
			dspbuf[6]=temperature%10;
			dspbuf[7]=12;
		}
		 
}

//按键扫描函数
unsigned char read_key(void)
{
    unsigned char temp;
    unsigned char key_value = 0xff;
    
    temp = P3&0x0f;
    switch(temp)
    {
        case 0x0e:
            key_value = 1;  //S7
            break;    
        case 0x0d:
            key_value = 2;  //S6
            break;    
        case 0x0b:
            key_value = 3;  //S5
            break;    
        case 0x07:
            key_value = 4;  //S4
            break;    
    }
    return key_value;
}
//按键消抖函数
void button(unsigned char a)
{
		a = read_key();
    if(a != 0xff)
    {
        delay();  //按键消抖
        a = read_key();
        if(a != 0xff)
        {
            //进入按键处理函数
            key_proc(a);
            while(P3 != 0xff);  //等待按键释放                        
        }
    }
}

//按键处理函数
void key_proc(unsigned char key)
{
		static unsigned char time=0;//控制时间的变量
		static unsigned char order=1;//控制占空比的变量
    switch(key)
    {
      case 1://控制模式
				P2 = (P2&0x1f|0x80);
				switch(order)
				{
					case 1:duty=200;P0=0XFE;break;
					case 2:duty=500;P0=0XFD;break;
					case 3:duty=700;P0=0XFB;break;
				}
				ordre=order;
				order++;
				if(order==4)
				{order=1;}
				P2 &= 0x1f;
        break;
			case 2://控制时间
				P2 = (P2&0x1f|0x80);
				switch(time)
				{
					case 0:second=0;break;
					case 1:second=60;break;
					case 2:second=120;break;
				}
				time++;
				if(time>2)
				{time=0;}
				P2 &= 0x1f;
				break;
      case 3:
				P2 = (P2&0x1f|0x80);
				second=0;
				duty=0;
				P2 &= 0x1f;
        break;
      case 4:
				P2 = (P2&0x1f|0x80);
				if(mod==1)
				{mod=0;}
				else
				{mod++;}
				P2 &= 0x1f;
        break;
    }    
}

