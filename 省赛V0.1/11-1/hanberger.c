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
#include "i2c.h"

                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
                             0xc1,0x8C,0xab};
                           //  U   P    n
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0xFF;//按键值
unsigned char adc_value;
unsigned char mod=1;
float voltage_standard=3;
unsigned char voltage_standard10;
float voltage;
unsigned char count=0;
unsigned char error=0;
unsigned char key_led=0;
/******函数声明***********************/
void read_keyboard(void);
void display();
void deal_adc();
void key_proc();
void fen();
void zhunze();
void jishu();
void error_led(unsigned char a);
void led_pan();
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
	init_pcf8591();
  voltage_standard10=read_eeprom(0x00);
	voltage_standard=((float)voltage_standard10)/10;
		
  while(1)
  {
		  read_keyboard();
		  key_proc();
		  fen();
		  led_pan();
		  error_led(key_led);
  }         
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{     
	static unsigned char intr = 0;
	static float voltage_old=0;
	static unsigned int dingshi = 0;
	display(); //1ms执行一次
	if(++intr == 10) 
	{
			intr=0;
			adc_value=adc_pcf8591();
		  voltage=adc_value*0.0196;
		  if((voltage_old>voltage_standard)&&(voltage<voltage_standard))
			{count++;}
			voltage_old=voltage;
  }
	if(voltage_old<voltage_standard)
	{
			dingshi++;
		  if(dingshi>=5000)
			{
					key_led=1;
				  dingshi=5000;
			}
	}
	else
	{
			dingshi=0;
		  key_led=0;
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

void key_proc()
{
	switch(key_value)
	{
		case 14://S12
		{
			error=0;
			mod++;
			if(mod==4)
      {
				mod=1;
				voltage_standard10=voltage_standard*10;
				write_eeprom(0X00,voltage_standard10);
			}
			key_value=0xff;
			break;
		}
		case 10://S13
		{
			if(mod==3)
			{
				error=0;
				count=0;
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
		case 15://S16,加
		{
			if(mod==2)
			{
				  error=0;
					voltage_standard+=0.5;
				  if(voltage_standard>5)
					{voltage_standard=0;}
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
		case 11://S17
		{
			if(mod==2)
			{
				  error=0;
					voltage_standard-=0.5;
				  if(voltage_standard<0)
					{voltage_standard=5;}
			}
			else
			{error++;}
			key_value=0xff;
			break;
		}
	}
}

void deal_adc()//写的略有麻烦
{
		//float voltage;
	  int voltage100;
		unsigned char ge,dianyi,dianer;
		//voltage=adc_value*0.0196;
		voltage100=voltage*100;
		ge=voltage100/100;
		dianyi=voltage100%100/10;
		dianer=voltage100%10;
    
		dspbuf[0]=22;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=(ge+11);dspbuf[6]=dianyi;dspbuf[7]=dianer;
}

void zhunze()
{
	  int voltage_100;
		unsigned char ge1,dianyi1,dianer1;
		voltage_100=voltage_standard*100;
		ge1=voltage_100/100;
		dianyi1=voltage_100%100/10;
		dianer1=voltage_100%10;
	
		dspbuf[0]=23;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=(ge1+11);dspbuf[6]=dianyi1;dspbuf[7]=dianer1;
}

void jishu()
{
		dspbuf[0]=24;dspbuf[1]=10;dspbuf[2]=10;dspbuf[3]=10;dspbuf[4]=10;
	  dspbuf[5]=count/100;
	  dspbuf[6]=count%100/10;
	  dspbuf[7]=count%10;
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
			case 1:deal_adc();break;
			case 2:zhunze();break;
			case 3:jishu();break;
		}
}

void error_led(unsigned char a)
{
		P2 = ((P2&0x1f)|0x80);
	  switch(a)
		{
			case 0:P0 = 0xff;break;
			case 1:P0 = 0xfe;break;
			case 2:P0 = 0xfd;break;
			case 3:P0 = 0xfc;break;
			case 4:P0 = 0xfb;break;
			case 5:P0 = 0xfa;break;
			case 6:P0 = 0xf9;break;
			case 7:P0 = 0xf8;break;
		}
		P2 &= 0x1f;
}

void led_pan()
{
	  if(key_led==1)
		{
				if(error>=3&&count%2==0)
		    {key_led=1;key_led+=4;}
				
		    if(error>=3&&count%2==1)
		    {key_led=1;key_led+=6;}
				
		    if(error<3&&count%2==0)
		    {key_led=1;}
				
		    if(error<3&&count%2==1)
		    {key_led=1;key_led+=2;}
		}
		else
		{
				if(error>=3&&count%2==0)
		    {key_led=0;key_led+=4;}
				
		    if(error>=3&&count%2==1)
		    {key_led=0;key_led+=6;}
				
		    if(error<3&&count%2==0)
		    {key_led=0;}
				
		    if(error<3&&count%2==1)
		    {key_led=0;key_led+=2;}
		}
		
}	