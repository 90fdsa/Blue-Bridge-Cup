#include "STC15F2K60S2.H"

#define uchar unsigned char 
#define uint unsigned int

code uchar duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff};
uchar buffer[8]={10,10,10,10,10,10,10,10};
uchar mod1[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
uchar mod2[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
uchar mod3[]={0x7e,0xbd,0xdb,0xe7};
uchar mod4[]={0xe7,0xdb,0xbd,0x7e};

uchar weixuan=0;
uchar key_value=0;
uint jiange=200;
uchar mod_choose=1;
uchar order1=0;
uchar order2=0;
uchar do_state=1;
uint true_duty=50;



void display();
uchar key_scan();
void mode1();
void mode2();
void mode3();
void mode4();

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0X1F)|0XA0;
	  P2&=0x1f;
}

void close_led()
{
		P0=0xff;
	  P2=(P2&0X1F)|0X80;
	  P2&=0x1f;
}

void Timer0Init(void)		//1微秒@12.000MHz
{
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}


void Timer1Init(void)		//1000微秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
}



void main()
{
		close_buzz();
	  close_led();
	  Timer0Init();
	  Timer1Init();
	  EA=1;
	  while(1)
		{
//				buffer[0]=duty/1000;
//			  buffer[1]=duty%1000/100;
//			  buffer[2]=duty%100/10;
//			  buffer[3]=duty%10;
			  buffer[6]=key_value/10;
			  buffer[7]=key_value%10;
		}
}

void timer0_proc() interrupt 1
{
	  static uint duty=0;
	  duty++;
    if(duty>0&duty<true_duty)
    {
			  P2=(P2&0X1F)|0X80;
				switch(mod_choose)
				{
					case 1:mode1();break;
					case 2:mode2();break;
					case 3:mode3();break;
					case 4:mode4();break;
				}
				P2&=0x1f;
		}
		if(duty>=true_duty&duty<1000)
    {
			  P2=(P2&0X1F)|0X80;
				P0=0xff;
			  P2&=0x1f;
		}
		if(duty==1000){duty=0;}

}

void timer1_proc() interrupt 3
{
	  
	  static uchar a=0;
		static uint jiangejishi=0;
	  display();
	  
	  if(++a==10)
		{
			  a=0;
				key_value=key_scan();
		}
		if(++jiangejishi==jiange)
		{
				jiangejishi=0;
        if(mod_choose==1||mod_choose==2)
				{
						order1++;
					  if(order1==8){order1=0;mod_choose++;}
				}
				if(mod_choose==3||mod_choose==4)	
				{
						order1++;
					  if(order1==4)
            {
						    order1=0;mod_choose++;
							  if(mod_choose==5){mod_choose=1;}
						}
				}
		}
		
	  
	  
}

uchar key_scan()
{
	  uchar key;
	  uint ma;
	  
		P3|=0x0f;
	  P44=0;P42=1;P35=1;P34=1;
	  ma=P3&0x0f;
	  P44=1;P42=0;P35=1;P34=1;
	  ma=(ma<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=0;P34=1;
	  ma=(ma<<4)|(P3&0x0f);
	  P44=1;P42=1;P35=1;P34=0;
	  ma=(ma<<4)|(P3&0x0f);
	
	  switch(~ma)
		{
			case 0x1000:key=1;break;
			case 0x0100:key=2;break;
			case 0x0010:key=3;break;
			case 0x0001:key=4;break;
			case 0x2000:key=5;break;
			case 0x0200:key=6;break;
			case 0x0020:key=7;break;
			case 0x0002:key=8;break;
			case 0x4000:key=9;break;
			case 0x0400:key=10;break;
			case 0x0040:key=11;break;
			case 0x0004:key=12;break;
			case 0x8000:key=13;break;
			case 0x0800:key=14;break;
			case 0x0080:key=15;break;
			case 0x0008:key=16;break;
			default:key=0;break;
		}
		
		return key;
}

void display()
{
		P0=0xff;
	  P2=(P2&0X1F)|0Xe0;
	  P2&=0x1f;
	
	  P0=1<<weixuan;
	  P2=(P2&0X1F)|0Xc0;
	  P2&=0x1f;
	
	  P0=duanma[buffer[weixuan]];
	  P2=(P2&0X1F)|0Xe0;
	  P2&=0x1f;
	
	  if(++weixuan==8){weixuan=0;}
}

void mode1()
{
	  P0=mod1[order1];
		
		
}

void mode2()
{
		P0=mod2[order1];
		
		
}
	
void mode3()
{
		P0=mod3[order1];
		
		
}

void mode4()
{
		P0=mod4[order1];
		

		

}