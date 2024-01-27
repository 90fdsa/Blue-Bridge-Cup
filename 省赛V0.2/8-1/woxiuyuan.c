#include "STC15F2K60S2.H"
#include "onewire.h"
#include "ds1302.h"

#define uchar unsigned char
#define uint unsigned int

code uchar duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,0xbf,0xc6};
uchar buffer[]={10,10,10,10,10,10,10,10};
char time_stand[3];
char alarm_stand[3]={0,0,0};

uchar weixuan;
uchar key_value=0;
uchar old_key_value=0;
uchar hours,minutes,seconds;
uchar tiaozheng_mod=0;
uchar time_tiaozheng=0;
uchar one_second=0;
uchar pause_flag=0;
uchar alarm_action=0;
uchar led_flag=0;
uchar blink_times=0;
uchar tem_show=0;
float tempera;

uchar read_keyboard();
void display();
void key_proc();
void read_time();
void how_to_display();
void palse_time();
void start_time();
void alarm_do();
void led_do();
void key_trigger();

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0X1F)|0XA0;
	  P2&=0X1F;
}

void close_led()
{
		P0=0xff;
	  P2=(P2&0X1F)|0X80;
	  P2&=0X1F;
}

void Timer0Init(void)		//1000微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}


void main()
{
		close_buzz();
	  close_led();
	  Timer0Init();
	  EA=1;
	  set_time(23,59,55);
	
	  while(1)
		{
				read_time();
			  how_to_display();
			  
			  
		}
}

void timer0_proc() interrupt 1
{
	  static uchar i=0;
	  static uint dumiao=0;
	  static uint blink=0;
	  static uint tem=0;
		display();
	  if(++i==10)
		{
				i=0;
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  key_trigger();
			  if(old_key_value<key_value){key_proc();}
				if(old_key_value>key_value){tem_show=0;}
				alarm_do();
				led_do();
		}
		if(++dumiao==1000)
		{
				dumiao=0;
			  one_second=~one_second;
		}
		if(++tem==300)
		{
				tem=0;
			  tempera=rd_temperature();
		}
		if(alarm_action==1)
		{
				blink++;
			  if(blink==200)
				{
						blink=0;
					  led_flag=~led_flag;
					  blink_times++;
					  if(blink_times==25 || key_value!=0)
						{
						    alarm_action=0;
							  blink=0;
							  blink_times=0;
							  led_flag=0;
						}
				}
		}
}
uchar read_keyboard()
{
	  uint key;
	  uchar key_va;  
	
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  key=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  key=(key<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  key=(key<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  key=(key<<4)|(P3&0x0f);
	
	  switch(~key)
		{
			case 0x1000:key_va=1;break;
			case 0x0100:key_va=2;break;
			case 0x0010:key_va=3;break;
			case 0x0001:key_va=4;break;
			case 0x2000:key_va=5;break;
			case 0x0200:key_va=6;break;
			case 0x0020:key_va=7;break;
			case 0x0002:key_va=8;break;
			case 0x4000:key_va=9;break;
			case 0x0400:key_va=10;break;
			case 0x0040:key_va=11;break;
			case 0x0004:key_va=12;break;
			case 0x8000:key_va=13;break;
			case 0x0800:key_va=14;break;
			case 0x0080:key_va=15;break;
			case 0x0008:key_va=16;break;
			default:key_va=0;break;
		}
		
		return key_va;
}

void display()
{
		P0=0xff;
	  P2=(P2&0X1F)|0Xe0;
	  P2&=0X1F;
	
	  P0=1<<weixuan;
	  P2=(P2&0X1F)|0Xc0;
	  P2&=0X1F;
	
	  P0=duanma[buffer[weixuan]];
	  P2=(P2&0X1F)|0Xe0;
	  P2&=0X1F;
	
	  if(++weixuan==8){weixuan=0;}
}

void key_proc()
{
		switch(key_value)
		{
			case 9:
			{  
				  tiaozheng_mod=1;
					time_tiaozheng++;
				  if(time_tiaozheng==1){palse_time();}
				  if(time_tiaozheng==4)
          {
					    time_tiaozheng=0;
							tiaozheng_mod=0;
						  set_time(time_stand[0],time_stand[1],time_stand[2]);
					    start_time();
					}
			}break;
			case 13:
			{
					tiaozheng_mod=2;
					time_tiaozheng++;
				  if(time_tiaozheng==4)
          {
					    time_tiaozheng=0;
							tiaozheng_mod=0;
					}
			}break;
			case 10:
			{
					if(tiaozheng_mod==1)
					{
							if(time_tiaozheng==1)
							{
									time_stand[0]++;
								  if(time_stand[0]==24){time_stand[0]=0;}
							}
							if(time_tiaozheng==2)
							{
									time_stand[1]++;
								  if(time_stand[1]==60){time_stand[1]=0;}
							}
							if(time_tiaozheng==3)
							{
									time_stand[2]++;
								  if(time_stand[2]==60){time_stand[2]=0;}
							}
					}
					if(tiaozheng_mod==2)
					{
							if(time_tiaozheng==1)
							{
									alarm_stand[0]++;
								  if(alarm_stand[0]==24){alarm_stand[0]=0;}
							}
							if(time_tiaozheng==2)
							{
									alarm_stand[1]++;
								  if(alarm_stand[1]==60){alarm_stand[1]=0;}
							}
							if(time_tiaozheng==3)
							{
									alarm_stand[2]++;
								  if(alarm_stand[2]==60){alarm_stand[2]=0;}
							}
					}
			}break;
		  case 14:
			{
					if(tiaozheng_mod==1)
					{
							if(time_tiaozheng==1)
							{
									time_stand[0]--;
								  if(time_stand[0]<0){time_stand[0]=23;}
							}
							if(time_tiaozheng==2)
							{
									time_stand[1]--;
								  if(time_stand[1]<0){time_stand[1]=59;}
							}
							if(time_tiaozheng==3)
							{
									time_stand[2]--;
								  if(time_stand[2]<0){time_stand[2]=59;}
							}
					}
					if(tiaozheng_mod==2)
					{
							if(time_tiaozheng==1)
							{
									alarm_stand[0]--;
								  if(alarm_stand[0]<0){alarm_stand[0]=23;}
							}
							if(time_tiaozheng==2)
							{
									alarm_stand[1]--;
								  if(alarm_stand[1]<0){alarm_stand[1]=59;}
							}
							if(time_tiaozheng==3)
							{
									alarm_stand[2]--;
								  if(alarm_stand[2]<0){alarm_stand[2]=59;}
							}
					}
			}break;
		}
}

void read_time()
{
	  uchar a,b,c;   
	
		a=Read_Ds1302_Byte(0x85);
		b=Read_Ds1302_Byte(0x83);
		c=Read_Ds1302_Byte(0x81);
	
	  if(pause_flag==1)
    {c&=0x7f;}
	
	  hours=a/16*10+a%16;
	  minutes=b/16*10+b%16;
	  seconds=c/16*10+c%16;
		
		if(pause_flag==1)
		{
				time_stand[0]=hours;
				time_stand[1]=minutes;
				time_stand[2]=seconds;
			  pause_flag=0;
		}
			
}

void palse_time()
{
		uchar a;
	  a=Read_Ds1302_Byte(0x81);
	  Write_Ds1302_Byte(0x8e,0x00);
	  Write_Ds1302_Byte(0x80,a|0x80);pause_flag=1;
	  Write_Ds1302_Byte(0x8e,0x80);
}

void start_time()
{
		uchar a;
	  a=Read_Ds1302_Byte(0x81);
	  Write_Ds1302_Byte(0x8e,0x00);
	  Write_Ds1302_Byte(0x80,a&0x7f);//pause_flag=0;
	  Write_Ds1302_Byte(0x8e,0x80);
}

void how_to_display()
{
	  if(tiaozheng_mod==0)
		{
			  if(tem_show==0)
				{
						buffer[0]=hours/10;buffer[1]=hours%10;
						buffer[2]=11;
						buffer[3]=minutes/10;buffer[4]=minutes%10;
						buffer[5]=11;
						buffer[6]=seconds/10;buffer[7]=seconds%10;
				}
				else
				{  
					  buffer[0]=10;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
						buffer[5]=((char)tempera)/10;
						buffer[6]=((char)tempera)%10;
					  buffer[7]=12;
				}
		}
		if(tiaozheng_mod==1)
		{
				if(time_tiaozheng==1)
				{
						if(one_second!=0)
						{
								buffer[0]=time_stand[0]/10;buffer[1]=time_stand[0]%10;
								buffer[2]=11;
								buffer[3]=time_stand[1]/10;buffer[4]=time_stand[1]%10;
								buffer[5]=11;
								buffer[6]=time_stand[2]/10;buffer[7]=time_stand[2]%10;
						}
						else
						{
								buffer[0]=10;buffer[1]=10;
								buffer[2]=11;
								buffer[3]=time_stand[1]/10;buffer[4]=time_stand[1]%10;
								buffer[5]=11;
								buffer[6]=time_stand[2]/10;buffer[7]=time_stand[2]%10;
						}
				}
				if(time_tiaozheng==2)
				{
						if(one_second!=0)
						{
								buffer[0]=time_stand[0]/10;buffer[1]=time_stand[0]%10;
								buffer[2]=11;
								buffer[3]=time_stand[1]/10;buffer[4]=time_stand[1]%10;
								buffer[5]=11;
								buffer[6]=time_stand[2]/10;buffer[7]=time_stand[2]%10;
						}
						else
						{
								buffer[0]=time_stand[0]/10;buffer[1]=time_stand[0]%10;
								buffer[2]=11;
								buffer[3]=10;buffer[4]=10;
								buffer[5]=11;
								buffer[6]=time_stand[2]/10;buffer[7]=time_stand[2]%10;
						}
				}
				if(time_tiaozheng==3)
				{
						if(one_second!=0)
						{
								buffer[0]=time_stand[0]/10;buffer[1]=time_stand[0]%10;
								buffer[2]=11;
								buffer[3]=time_stand[1]/10;buffer[4]=time_stand[1]%10;
								buffer[5]=11;
								buffer[6]=time_stand[2]/10;buffer[7]=time_stand[2]%10;
						}
						else
						{
								buffer[0]=time_stand[0]/10;buffer[1]=time_stand[0]%10;
								buffer[2]=11;
								buffer[3]=time_stand[1]/10;buffer[4]=time_stand[1]%10;
								buffer[5]=11;
								buffer[6]=10;buffer[7]=10;
						}
				}
		}
		if(tiaozheng_mod==2)
		{
				if(time_tiaozheng==1)
				{
						if(one_second!=0)
						{
								buffer[0]=alarm_stand[0]/10;buffer[1]=alarm_stand[0]%10;
								buffer[2]=11;
								buffer[3]=alarm_stand[1]/10;buffer[4]=alarm_stand[1]%10;
								buffer[5]=11;
								buffer[6]=alarm_stand[2]/10;buffer[7]=alarm_stand[2]%10;
						}
						else
						{
								buffer[0]=10;buffer[1]=10;
								buffer[2]=11;
								buffer[3]=alarm_stand[1]/10;buffer[4]=alarm_stand[1]%10;
								buffer[5]=11;
								buffer[6]=alarm_stand[2]/10;buffer[7]=alarm_stand[2]%10;
						}
				}
				if(time_tiaozheng==2)
				{
						if(one_second!=0)
						{
								buffer[0]=alarm_stand[0]/10;buffer[1]=alarm_stand[0]%10;
								buffer[2]=11;
								buffer[3]=alarm_stand[1]/10;buffer[4]=alarm_stand[1]%10;
								buffer[5]=11;
								buffer[6]=alarm_stand[2]/10;buffer[7]=alarm_stand[2]%10;
						}
						else
						{
								buffer[0]=alarm_stand[0]/10;buffer[1]=alarm_stand[0]%10;
								buffer[2]=11;
								buffer[3]=10;buffer[4]=10;
								buffer[5]=11;
								buffer[6]=alarm_stand[2]/10;buffer[7]=alarm_stand[2]%10;
						}
				}
				if(time_tiaozheng==3)
				{
						if(one_second!=0)
						{
								buffer[0]=alarm_stand[0]/10;buffer[1]=alarm_stand[0]%10;
								buffer[2]=11;
								buffer[3]=alarm_stand[1]/10;buffer[4]=alarm_stand[1]%10;
								buffer[5]=11;
								buffer[6]=alarm_stand[2]/10;buffer[7]=alarm_stand[2]%10;
						}
						else
						{
								buffer[0]=alarm_stand[0]/10;buffer[1]=alarm_stand[0]%10;
								buffer[2]=11;
								buffer[3]=alarm_stand[1]/10;buffer[4]=alarm_stand[1]%10;
								buffer[5]=11;
								buffer[6]=10;buffer[7]=10;
						}
				}
		}
}

void alarm_do()
{
		if(hours==alarm_stand[0] & minutes==alarm_stand[1] & seconds==alarm_stand[2])
		{
				alarm_action=1;
		}
}

void led_do()
{
	  uchar led_value;
		if(led_flag!=0){led_value|=0x01;}
		else{led_value&=0xfe;}
		
		P0=~led_value;
		P2=(P2&0X1F)|0X80;
	  P2&=0X1F;
}

void key_trigger()
{
		switch(key_value)
		{
		  case 14:
			{
					if(tiaozheng_mod==0)
					{
							tem_show=1;
					}
			}break;
		}
}