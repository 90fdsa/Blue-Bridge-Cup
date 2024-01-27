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

//sbit S1  = P0^0;
//sbit S2  = P0^1;
//sbit S3  = P0^2;
//sbit S4  = P0^3;
 
//函数声明
/**********************************/
void cls_buzz();
void cls_led();
void button(unsigned char a);
void key_proc(unsigned char key);
unsigned char read_key(void);
void delay();
/**********************************/

//主函数
void main(void)
{ 
		unsigned char temp=0;
		cls_buzz();
		cls_led();   
    while(1)
    {
			//显示数据更新 
			button(temp); 
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
		static unsigned char time=1;
    switch(key)
    {
      case 1:
				P2 = (P2&0x1f|0x80);
				switch(time)
				{
					case 1:P0=0XFE;break;
					case 2:P0=0XFD;break;
					case 3:P0=0XFB;break;
					case 4:P0=0XF7;break;
				}
				time++;
				if(time==5)
				{time=1;}
				P2 &= 0x1f;
        break;
			case 2:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
				break;
      case 3:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
        break;
      case 4:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
        break;
    }    
}

