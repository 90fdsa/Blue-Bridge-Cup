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
#include "onewire.h"
#include "ds1302.h"
                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0xbf};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0xFF;//按键值
unsigned char mod=1;
unsigned char *p_rtc;
float true_temperature;
unsigned char temperature_standard=23;
unsigned char jidianqi_mod=0;
unsigned char time_jidianqi=0;
unsigned char time_show_mod=0;
unsigned char Led_Flag;
unsigned char zhengdian=0;
unsigned char Timer_Flag;
unsigned char relay_flag;
/******函数声明***********************/
void read_keyboard(void);
void display();
void temperature_deal();
void smg_off();
void fen();
void key_proc();
void Delay100ms();		//@11.0592MHz
void clock_deal();
void temperature_biao();
void jidianqi_kong();
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

  Delay100ms();
  SetRTC();  
    while(1)
    {
			 key_proc();
			 fen();
			 jidianqi_kong();
			 led_show();
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{     
	static unsigned char intr = 0;
	static unsigned char clock = 0;
	static unsigned int tem = 0;
	static unsigned int deng=0;
	static unsigned int a=0;
	display(); //1ms执行一次
	if(++intr == 10) 
	{
     intr = 0;
		 read_keyboard();
  }
	if(++clock == 200) 
	{
     clock = 0;
		 p_rtc = ReadRTC();
  }
	if(++tem==400)
	{
		 tem = 0;
		 true_temperature=rd_temperature_f();
	}
	if(time_jidianqi==1)
	{
		 deng++;
		 P2 = (P2&0x1F|0xA0);
	   P0 = 0x10;
	   P2 &= 0x1F;
		 relay_flag=1;
		 if(deng==5000)
		 {
			  deng=0;
				time_jidianqi=0;
		 }
	}
	if(++a==100)
	{	 a=0;Timer_Flag=1;}
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
		case 14://S12
		{
			mod++;
			if(mod==4){mod=1;}
			key_value=0xff;
			break;
		}
		case 10://S13
		{
			jidianqi_mod=~jidianqi_mod;
			key_value=0xff;
			break;
		}
		case 15://S16
		{
			if(mod==3)
			{
			   temperature_standard++;
				 if(temperature_standard==100){temperature_standard=10;}
			}
			key_value=0xff;
			break;
		}
		case 11://S17
		{
			if(mod==3)
			{
			   temperature_standard--;
				 if(temperature_standard==9){temperature_standard=99;}
			}
			if(mod==2)
			{
			   time_show_mod=~time_show_mod;
			}
			key_value=0xff;
			break;
		}
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

void temperature_deal()
{
		unsigned int temperature10;
	  temperature10=true_temperature*10;
	  dspbuf[0]=22;dspbuf[1]=1;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=temperature10/100;
	  dspbuf[6]=(temperature10%100/10)+11;
	  dspbuf[7]=temperature10%10;
}

void clock_deal()
{
	  if(time_show_mod==0)
		{
				dspbuf[0]=22;dspbuf[1]=2;dspbuf[2]=10;
	      dspbuf[3]=p_rtc[0];
	      dspbuf[4]=p_rtc[1];
	      dspbuf[5]=23;
	      dspbuf[6]=p_rtc[2];
	      dspbuf[7]=p_rtc[3];
		}
	  else
		{
	      dspbuf[0]=22;dspbuf[1]=2;dspbuf[2]=10;
				dspbuf[3]=p_rtc[2];
				dspbuf[4]=p_rtc[3];
				dspbuf[5]=23;
				dspbuf[6]=p_rtc[4];
				dspbuf[7]=p_rtc[5];
		}
}

void temperature_biao()
{
		dspbuf[0]=22;dspbuf[1]=3;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;dspbuf[5]=10;
	  dspbuf[6]=temperature_standard/10;
	  dspbuf[7]=temperature_standard%10;
}

void smg_off()//关闭数码管，暂时的
{
		dspbuf[0]=10;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;
	  dspbuf[4]=10;dspbuf[5]=10;dspbuf[6]=10;dspbuf[7]=10;
}

void fen()
{
		switch(mod)
		{
			case 1:temperature_deal();break;
			case 2:clock_deal();break;
			case 3:temperature_biao();break;
		}
}

void jidianqi_kong()
{
		if(jidianqi_mod==0)//温控
		{
			  if(true_temperature>temperature_standard)
				{
						P2 = (P2&0x1F|0xA0);
	          P0 = 0x10;
	          P2 &= 0x1F;
					  relay_flag=1;
				}
				else
				{
						P2 = (P2&0x1F|0xA0);
	          P0 = 0x00;
	          P2 &= 0x1F;
					  relay_flag=0;
				}
		}
		else
		{
			  if(time_jidianqi==0)
				{
						P2 = (P2&0x1F|0xA0);
	          P0 = 0x00;
	          P2 &= 0x1F;
					  relay_flag=0;
				}
			  if(p_rtc[2]==0&&p_rtc[3]==0&&p_rtc[4]==0&&p_rtc[5]==0&&time_jidianqi==0)
				{
					  time_jidianqi=1;
				}
		}
}


void led_show()
{
	  //led1
	  if(p_rtc[2]==0&&p_rtc[3]==0&&p_rtc[4]==0&&p_rtc[5]==0&&zhengdian==0)
	  {
		  Led_Flag |= 0x01;zhengdian=1;
		}
		if(p_rtc[5]>5&&zhengdian==1)
		{
			Led_Flag &= 0xfe;zhengdian=0;
		}
	
		//led2
		if(jidianqi_mod == 0)
		{
			Led_Flag |= 0x02;
		}
		else
		{
			Led_Flag &= 0xfd;
		}
	
		//led3
		if(relay_flag == 1)
		{
			if(Timer_Flag == 1)
			{
				Timer_Flag=0;
			
			  if((Led_Flag&0x04) == 0x04)
			  {
				  Led_Flag &= 0xfb;
			  }
			  else
			  {
				  Led_Flag |= 0x04;
			  }
		  }
	  }
	  else
	  {
		  Led_Flag &= 0xfb;
	  }
		P2 = ((P2&0x1f)|0x80);
    P0 = ~Led_Flag;
		P2 &= 0x1f;
}

void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

