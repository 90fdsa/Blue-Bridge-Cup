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
#include "intrins.h"
#include "onewire.h"


                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0xbf,0xc6};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0xFF;//按键值
char hour=23,minute=59,second=50;//时间
char hours=0,minutes=0,seconds=0;//闹钟时间
char stand,mod=0;
char time=0,order=0;//控制闹钟的变量
char alart=0;
char ordre=0;//控制显示温度的变量
char temper;
/******函数声明***********************/
void read_keyboard(void);
void display();
void how_to_display();
void flow();
void blink(unsigned char a);
void key_proc();
void delay2ms(void);
void alarm();
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
	//init_ds18b20();
    while(1)
    {
			read_keyboard();
			if(key_value != 0xFF)
			{key_proc();}
			alarm();  
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{     
	static unsigned int intr = 0;
	static unsigned int tem = 0;
	display(); //1ms执行一次
	if(++tem==50)
	{
		temper=rd_temperature();
		
		tem=0;
	}
	if(mod==0)
	{
		how_to_display();
		if(++intr == 1000) 
		{
			intr = 0;
			flow();
			alart=0;
		}
	}
	else if(mod==1)
	{
		if(++intr == 1000) 
		{
			intr = 0;
			blink(time);
		}
	}
	else if(mod==2)
	{
		if(++intr == 1000) 
		{
			intr = 0;
			blink(time);
		}
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

void how_to_display()
{
	if(ordre==1)
	{
		dspbuf[0] = 10;
		dspbuf[1] = 10;
		dspbuf[2] = 10;
		dspbuf[3] = 10;
		dspbuf[4] = 10;
		dspbuf[5] = temper/10;
		dspbuf[6] = temper%10;
		dspbuf[7] = 12;
	}
	else
	{
		dspbuf[0] = hour/10;
		dspbuf[1] = hour%10;
		dspbuf[2] = 11;
		dspbuf[3] = minute/10;
		dspbuf[4] = minute%10;
		dspbuf[5] = 11;
		dspbuf[6] = second/10;
		dspbuf[7] = second%10;
	}
	
}

void key_proc()
{
	static unsigned char teim=0;//控制时间的变量
	switch(key_value)
	{
		case 8:
		{
			mod=1;
			switch(teim)
			{
				case 0:time=0;teim++;break;
				case 1:time=1;teim++;break;
				case 2:time=2;teim++;break;
				case 3:mod=0;time=0;teim=0;break;
			}
			key_value=0xff;alart=1;
			break;
		}
		case 12:
		{
			mod=2;
			switch(teim)
			{
				case 0:time=0;teim++;break;
				case 1:time=1;teim++;break;
				case 2:time=2;teim++;break;
				case 3:mod=0;time=0;teim=0;break;
			}
			key_value=0xff;alart=1;
			break;
		}
		case 9:
		{
			if(mod==1)
			{
				switch(time)
				{
					case 0:
					{	hour++;
						if(hour==24)
						{hour=0;}
						break;}
					case 1:
					{	minute++;
						if(minute==60)
						{minute=0;}
						break;}
					case 2:
					{	second++;
						if(second==60)
						{second=0;}
						break;}
				}
			}
			else if(mod==2)
			{
				switch(time)
				{
					case 0:
					{	hours++;
						if(hours==24)
						{hours=0;}
						break;}
					case 1:
					{	minutes++;
						if(minutes==60)
						{minutes=0;}
						break;}
					case 2:
					{	seconds++;
						if(seconds==60)
						{seconds=0;}
						break;}
				}
			}
			key_value=0xff;alart=1;
			break;
		}
		case 13:
		{
			if(mod==0)
			{
				if(ordre==1)
				{ordre=0;}
				else
				{ordre++;}
			}
			if(mod==1)
			{
				switch(time)
				{
					case 0:
					{	hour--;
						if(hour<0)
						{hour=23;}
						break;}
					case 1:
					{	minute--;
						if(minute<0)
						{minute=59;}
						break;}
					case 2:
					{	second--;
						if(second<0)
						{second=59;}
						break;}
				}
			}
			else if(mod==2)
			{
				switch(time)
				{
					case 0:
					{	hours--;
						if(hours<0)
						{hours=23;}
						break;}
					case 1:
					{	minutes--;
						if(minutes<0)
						{minutes=59;}
						break;}
					case 2:
					{	seconds--;
						if(seconds<0)
						{seconds=59;}
						break;}
				}
			}
			key_value=0xff;alart=1;
			break;
		}
	}
}
void flow()
{
	second++;
	if(second==60)
	{
		second=0;
		minute++;
		if(minute==60)
		{
			minute=0;
			hour++;
			if(hour==24)
			{
				second=0;
				minute=0;
				hour=0;
			}
		}
	}
}

void blink(unsigned char a)
{
	static unsigned char count=0;
	if(mod==1)
	{
		if(a==0)
		{
			if(count==0)
			{
				dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
		else if(a==1)
		{
			if(count==0)
			{
				dspbuf[0] = hour / 10;dspbuf[1] = hour % 10;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = 10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
		else if(a==2)
		{
			if(count==0)
			{
				dspbuf[0] = hour / 10;dspbuf[1] = hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = 10;dspbuf[7] = 10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hour / 10;dspbuf[1] =hour % 10;dspbuf[2] = 11;dspbuf[3] = minute/10;
				dspbuf[4] = minute%10;dspbuf[5] = 11;dspbuf[6] = second/10;dspbuf[7] = second%10;
				count=0;
			}
		}
	}
	if(mod==2)
	{
		if(a==0)
		{
			if(count==0)
			{
				dspbuf[0] = 10;dspbuf[1] = 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
		else if(a==1)
		{
			if(count==0)
			{
				dspbuf[0] = hours / 10;dspbuf[1] = hours % 10;dspbuf[2] = 11;dspbuf[3] = 10;
				dspbuf[4] = 10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
		else if(a==2)
		{
			if(count==0)
			{
				dspbuf[0] = hours / 10;dspbuf[1] = hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = 10;dspbuf[7] = 10;
				count++;
			}
			else if(count==1)
			{
				dspbuf[0] = hours / 10;dspbuf[1] =hours % 10;dspbuf[2] = 11;dspbuf[3] = minutes/10;
				dspbuf[4] = minutes%10;dspbuf[5] = 11;dspbuf[6] = seconds/10;dspbuf[7] = seconds%10;
				count=0;
			}
		}
	}
}
void alarm()
{
	char i;
	if(hour==hours&&minute==minutes&&second==seconds&&alart==0)
	{
		for(i=0;i<13;i++)
		{
			P2 = ((P2&0x1f)|0x80);
			P0 = 0xff;  //LED关
			P2 &= 0x1f;
      delay2ms();
        
      P2 = ((P2&0x1f)|0x80);
			P0 = 0xfe;  //LED开
			P2 &= 0x1f;
      delay2ms();
		}
		P2 = ((P2&0x1f)|0x80);
		P0 = 0xff;  //LED关
		P2 &= 0x1f;
	}
}

void delay2ms(void)   //误差 -0.036168981482us
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