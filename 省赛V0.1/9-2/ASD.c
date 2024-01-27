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
#include "onewire.h"
                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,
                             0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,
													   0xc1,0x8e,0xc6,0x89,0x8c};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区


unsigned char key_value=0xFF;//按键值
unsigned char select=1;
unsigned char small_mod=0;
unsigned char s12_flag=0;

unsigned int pluse=0,pluse_zan=0;//将他们分开存

float temperature;
unsigned int temperature100;//将他们分开存
unsigned char shi_wei,ge_wei,yi_wei,er_wei;

unsigned char adc_value;//将他们分开存
double voltage;
char ge,dianyi;
double standard=2;

unsigned char adc_1;
unsigned char tem_1,tem_2;
unsigned char fre_1,fre_2,fre_3;

double voltage_stand;
unsigned int temperature_stand;
unsigned int frequency_stand;

/******函数声明***********************/
void read_keyboard();
void display();
void show_adc();
void show_NE();
void show_tem();
void fen();
void key_proc();
void deal();
void deal_adc();
void deal_tem();
void delay10ms();
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

void Timer0Init(void)		//12微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟12T模式
	TMOD &= 0xF0; 	//设置定时器模式
	TMOD |= 0x04;
	TL0 = 0xff;		//设置定时初值
	TH0 = 0xff;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;    //打开定时器中断
}

void Timer1Init(void)		//12微秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x20;		//设置定时初值
	TH1 = 0xD1;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;    //打开定时器中断
}
//主函数
void main(void)
{  
	cls_buzz();
	cls_led();

	Timer0Init();	
  Timer1Init();
	
	EA=0;
	
	adc_1=read_eeprom(0x00);delay10ms();
	tem_1=read_eeprom(0x01);delay10ms();
	tem_2=read_eeprom(0x02);delay10ms();
	fre_1=read_eeprom(0x03);delay10ms();
	fre_2=read_eeprom(0x04);delay10ms();
	fre_3=read_eeprom(0x05);delay10ms();
	
	EA = 1;   
	init_pcf8591();
	voltage_stand=adc_1*0.0196;
	temperature_stand=tem_1*100+tem_2;
	frequency_stand=fre_1*1000+fre_2*10+fre_3;
	
  while(1)
  {
		  read_keyboard();
			key_proc();
		  deal();
		  fen();
  } 
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 3
{
		pluse_zan++;
}

void isr_timer_1(void)  interrupt 3  //默认中断优先级 3
{     
		static unsigned char time=0;
		static unsigned int tem=0;
	  static unsigned int jian=0;
	  static unsigned int a=0;
	  static unsigned int b=0;
		display(); //1ms执行一次
		if(++time==10)
		{
				time=0;
				adc_value=adc_pcf8591();
		}
	  if(++tem==300)
	  {
			  tem=0;
		    temperature=rd_temperature_f();
	  }
	  if(++jian==1000)
		{
			  jian=0;
			  TR0=0;
				pluse=pluse_zan;
			  pluse_zan=0;
			  TR0=1;
		}
		if(s12_flag==1)
		{
			  standard=standard+0.1;
				if(standard>=5.1){standard=0;}
			  if(++a>=800)
				{
					  if(++b==200)
						{
								b=0;
							  standard=standard+0.1;
							  if(standard>=5.1){standard=0;}
						}
				}
		}
		else
		{
				a=0;b=0;
		}
		s12_flag=0;
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
		case 12://选择
		{
			if(select!=5)
			{
					select++;
					if(select==4){select=1;}
			}	
      else
			{
			    s12_flag=1;
			}				
			key_value=0xff;
			break;
		}
		case 8://存储
		{
			write_eeprom(0x00,adc_value);
			delay10ms();
			write_eeprom(0x01,temperature100/100);
			delay10ms();
			write_eeprom(0x02,temperature100%100);
			delay10ms();
			write_eeprom(0x03,pluse/1000);
			delay10ms();
			write_eeprom(0x04,pluse%1000/10);
			delay10ms();
			write_eeprom(0x05,pluse/10);
			delay10ms();
			voltage_stand=voltage;
			temperature_stand=temperature100;
			frequency_stand=pluse;
			key_value=0xff;
			break;
		}
		case 9:
		{
			if(select!=5){select=5;}
			else{select=1;}
			key_value=0xff;
			break;
		}
		case 13://回显
		{
			small_mod=~small_mod;
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

void show_adc()
{
	  if(small_mod==0)
		{
				dspbuf[0]=22;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=10;
				dspbuf[5]=10;
				dspbuf[6]=(ge+11);
				dspbuf[7]=dianyi;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=22;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=10;
				dspbuf[5]=10;
				dspbuf[6]=((char)(voltage_stand*10)/10)+11;
				dspbuf[7]=(char)(voltage_stand*10)%10;
		}
		    
}

void show_NE()
{
	  if(small_mod==0)
		{
				dspbuf[0]=23;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=pluse/10000;
				dspbuf[4]=pluse%10000/1000;
				dspbuf[5]=pluse%1000/100;
				dspbuf[6]=pluse%100/10;
				dspbuf[7]=pluse%10;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=23;
				dspbuf[2]=10;
				dspbuf[3]=frequency_stand/10000;
				dspbuf[4]=frequency_stand%10000/1000;
				dspbuf[5]=frequency_stand%1000/100;
				dspbuf[6]=frequency_stand%100/10;
				dspbuf[7]=frequency_stand%10;
		}
}	

void show_tem()
{
	  if(small_mod==0)
		{
				dspbuf[0]=24;
				dspbuf[1]=10;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=shi_wei;
				dspbuf[5]=ge_wei+11;
				dspbuf[6]=yi_wei;
				dspbuf[7]=er_wei;
		}
		else
		{
				dspbuf[0]=25;
				dspbuf[1]=24;
				dspbuf[2]=10;
				dspbuf[3]=10;
				dspbuf[4]=temperature_stand/1000;
				dspbuf[5]=(temperature_stand%1000/100)+11;
				dspbuf[6]=temperature_stand/100%10;
				dspbuf[7]=temperature_stand%10;
		}			
}

void show_standard()
{
	  dspbuf[0]=26;
		dspbuf[1]=key_value/100;
		dspbuf[2]=key_value%100/10;
		dspbuf[3]=key_value%10;
    dspbuf[4]=10;
		dspbuf[5]=10;
		dspbuf[6]=((char)(standard*10))/10+11;
		dspbuf[7]=((char)(standard*10))%10;
}
void fen()
{
		if(select==1)
		{show_adc();}
		if(select==2)
		{show_NE();}
		if(select==3)
		{show_tem();}
		if(select==5)
		{show_standard();}
}

void deal()
{
	  deal_adc();
	  deal_tem();
}

void deal_adc()
{
		voltage=adc_value*0.0196;
		ge=(char)voltage;
		dianyi=((char)(voltage*10))%10;
}

void deal_tem()
{
		temperature100=(int)(temperature*100);
		shi_wei=(char)temperature/10;
		ge_wei=(char)temperature%10;
		yi_wei=(char)(temperature100%100/10);
	  er_wei=(char)(temperature100%10);
}

void delay10ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
