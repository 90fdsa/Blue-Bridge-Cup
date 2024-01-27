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
unsigned char key_value=0,old_key_value=0;//按键值
unsigned char led_1,led_2,led_3,Led_Flag;
unsigned char ok=0;
unsigned char done=0;
unsigned int chang = 0;
unsigned char times=0;
unsigned char jiesuan=0;
/******函数声明***********************/
void display();
unsigned char Key_Scan(); 
void key_proc_short();
void led_show();
void key_proc_long();
void key_proc_dcl();
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
			  //key_proc_short();
				dspbuf[7] = key_value%10;
				dspbuf[6] = key_value/10;
			
				led_show();
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{     
	static unsigned char intr = 0;
  static unsigned char b=0;
	display(); //1ms执行一次
	if(++intr == 10) 
	{
     intr = 0;
		 old_key_value=key_value;
     key_value=Key_Scan();
		 if(old_key_value<key_value){ok=1;times++;}//短按键或双击，其实应该叫按下检测
		 //如果只有短按键，就在括号里写短按键处理函数。
		 if(old_key_value>key_value){done=0;}//长按键，其实应该叫松手检测
		 //长按键的判据必须要有松手检测
  }
	
	if(ok==1)//定时200ms，用于区别双击和单击
	{
			b++;
		  if(b==200){jiesuan=1;b=0;}
	}
	
	if(jiesuan==1)//定时之后的清算，判断双击或单击
	{
			if(times==2){key_proc_dcl();ok=0;times=0;jiesuan=0;}
			if(times==1){key_proc_short();ok=0;times=0;jiesuan=0;}
	}
	
  if(key_value!=0)//这个ifelse是长按键部分
	{
		 chang++;
		 if(chang>1000 & done==0){key_proc_long();}
		 //记住这个key_proc_long()函数必须要在函数结尾输入键值为零，done为1的语句
	}
	else{chang=0;}
}

unsigned char Key_Scan(void)
{
	unsigned int Key_New;
	unsigned char Key_Value;
	P3 |= 0x0F;
	P44 = 0; P42 = 1; P35 = 1; P34 = 1;  // 第一列
	Key_New = P3 & 0X0F;
	P44 = 1; P42 = 0; P35 = 1; P34 = 1;  // 第二列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;  // 第三列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 1; P34 = 0;  // 第四列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	switch(~Key_New)
	{
		case 0x8000: Key_Value = 13; break;
		case 0x4000: Key_Value = 9; break;
		case 0x2000: Key_Value = 5; break;
		case 0x1000: Key_Value = 1; break;		
		case 0x0800: Key_Value = 14; break;
		case 0x0400: Key_Value = 10; break;
		case 0x0200: Key_Value = 6; break;
		case 0x0100: Key_Value = 2; break;		
		case 0x0080: Key_Value = 15; break;
		case 0x0040: Key_Value = 11; break;
		case 0x0020: Key_Value = 7; break;
		case 0x0010: Key_Value = 3; break;			
		case 0x0008: Key_Value = 16; break;
		case 0x0004: Key_Value = 12; break;
		case 0x0002: Key_Value = 8; break;
		case 0x0001: Key_Value = 4; break;			
		default : Key_Value = 0;
	}
	return Key_Value;
}


//显示函数
void display(void)
{   
	static unsigned char dspcom = 0;
	//消除鬼影
	P0 = 0xff;
	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  //写位码
	P0 = 1<<dspcom;
	P2 = ((P2&0x1f)|0xC0); 
	P2 &= 0x1f;
  //写段码
	P0 = tab[dspbuf[dspcom]];	
  	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  
    if(++dspcom == 8){
        dspcom = 0;
    }    
}

void key_proc_short()
{
	switch(key_value)
	{
		case 1://选择
		{
			led_1=~led_1;
			break;
		}
		case 2://存储
		{
			led_2=~led_2;
			break;
		}
		case 3:
		{
			led_3=~led_3;
			break;
		}
	}
}

void key_proc_long()
{
	switch(key_value)
	{
		case 4://进入循环之后只做一次
		{
			led_2=~led_2;
			key_value=0;done=1;
			break;
		}
		case 1://进入循环之后一直做
		{
			led_1=~led_1;
			//key_value=0;done=1;
			break;
		}
	}
}

void key_proc_dcl()
{
	switch(key_value)
	{
		case 6://进入循环之后只做一次
		{
			led_2=~led_2;
			//key_value=0;done=1;
			break;
		}
	}
}

void led_show()
{
	  //led1
	  if(led_1!=0)
	  {
		  Led_Flag |= 0x01;
		}
		else
		{
			Led_Flag &= 0xfe;
		}
	
		//led7
		if(led_2!=0)
		{
			Led_Flag |= 0x40;
		}
		else
		{
			Led_Flag &= 0xbf;
		}
	
		//led8
		if(led_3!=0)
		{
			Led_Flag |= 0x80;
	  }
	  else
	  {
		  Led_Flag &= 0x7f;
	  }
		P0 = ~Led_Flag;
		P2 = ((P2&0x1f)|0x80);
		P2 &= 0x1f;
}