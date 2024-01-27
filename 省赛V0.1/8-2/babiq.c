#include "STC15F2K60S2.H"  //用此头文件直接代替REG52.H，请进行两个头文件对比
#include "absacc.h"
#include "i2c.h"
#include "intrins.h"
                           //  0    1    2    3    4    5    6    7    8    9   熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0x8e};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //显示缓冲区

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
sbit TX = P1^0;  //发射引脚
sbit RX = P1^1;  //接收引脚

bit key_flag = 0;//按键扫描标志位                                                                                          
unsigned char key_value=0xFF;//按键值
unsigned char s_flag=0;
unsigned int distance;
unsigned int time=0;
unsigned char mod=1;
unsigned char mod_show=0;
unsigned int distance_copy=0;
unsigned char standard=50;
unsigned char count=0;
unsigned char count_look=0;
unsigned char record;
unsigned char recording[10];
unsigned char ok,ok_2;
unsigned char led_1=0,led_2=0,led_3=0;
unsigned char Led_Flag;
/******函数声明***********************/
void read_keyboard(void);
void display();
void Timer0Init(void);		//12微秒@12.000MHz
void Timer1Init(void);		//12微秒@12.000MHz
unsigned char Sonic();
void fen();
void show_data();
void show_eeprom();
void can();
void kong();
void key_proc();
void voltage();
void eeprom_write();
void delay();	//10ms @11.0592MHz
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

void Timer0Init(void)		//12微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0停止计时
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
  char q;	
	cls_buzz();
	cls_led();
  Timer0Init();
	Timer1Init();
	init_pcf8591();
	EA=0;
	standard=read_eeprom(0x10);
	delay();
	count=read_eeprom(0x11);
	delay();
  for(q=0;q<10;q++)
	{
			recording[q]=read_eeprom(q);
		  delay();
	}
	EA=1;
  while(1)
  {    
		  key_proc();
		  fen();
		  if(distance-standard>0){voltage();}
		  led_show();
  }
}

//定时器中断服务函数
void isr_timer_1(void)  interrupt 3  //默认中断优先级 1
{     
	static unsigned char intr = 0;
	static unsigned char le=0;
	static unsigned char ji=0;
	if(++intr == 10) 
	{
     intr = 0;
		 read_keyboard(); 
  }
	if(ok_2==1)
	{
			if(++le==200)
			{
				  le=0;
					led_1=~led_1;
				  ji++;
				  if(ji==10)
					{ok_2=0;ji=0;le=0;}
			}
	}
  display(); //1ms执行一次
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
	static unsigned char a=0;
	static unsigned char b=0;
	switch(key_value)
	{
		case 12://S4
		{
			distance_copy=distance;
			distance = Sonic();
			if(mod_show!=0)
      {distance_copy=distance+distance_copy;}
			count++;
			EA=0;write_eeprom(0x11,count);delay();EA=1;
			ok=1;ok_2=1;
			eeprom_write();
			key_value=0xff;
			break;
		}
		case 8://S5,数据回显
		{
			a=~a;led_2=~led_2;
			if(a!=0){mod=2;}
			else{mod=1;}
			
			key_value=0xff;
			break;
		}
		case 9://S9
		{
			b=~b;led_3=~led_3;
			if(b!=0){mod=3;}
			else
      {
			  mod=1;
				EA=0;write_eeprom(0x10,standard);delay();EA=1;
				delay();
			}
			key_value=0xff;
			break;
		}
		case 13://S8
		{
			if(mod==1){mod_show=~mod_show;}
			if(mod==2)
			{
					count_look++;
				  if(count_look==10)
					{
							count_look=0;
					}
//					EA=0;
//					record=read_eeprom(count_look);
//					EA=1;
					delay();
			}
			if(mod==3)
			{
					standard+=10;
				  if(standard==100){standard=0;}
			}
			key_value=0xff;
			break;
		}
		case 0:
		{
		  write_eeprom(0x11,0);
			write_eeprom(0x00,0);
			write_eeprom(0x01,0);
			write_eeprom(0x02,0);
			write_eeprom(0x03,0);
			write_eeprom(0x04,0);
			write_eeprom(0x05,0);
			write_eeprom(0x06,0);
			write_eeprom(0x07,0);
			write_eeprom(0x08,0);
			write_eeprom(0x09,0);
			write_eeprom(0x0a,0);
			write_eeprom(0x0b,0);
			count=0;
		}
	}
}

unsigned char Sonic(void)
{
  unsigned char Dist=0,num=10;
	
//TX引脚发送10个频率为40Hz方波
  Timer0Init();  //初始化定时器0，初始化定时器初值
  TX = 0;        //发射引脚拉为低电平
  
  while(num--)  //发送10个
  {
    TR0 = 1;
	  while(!TF0);  //计时时间到，控制周期
	  TF0 = 0;      //手动清除标志位	
	  TX ^= 1;      //取反
    Timer0Init();  //初始化定时器0，初始化定时器初值	
  }


//接收方波
	TR0 = 0;		//定时器0停止计时
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;	//设置定时初值
	TH0 = 0x00;	//设置定时初值
	TR0 = 1;

  while(RX && !TF0);  //等待收到脉冲	或定时溢出
  TR0 = 0;  //定时器0停止计时
	
  if(TF0)   //定时溢出
  {
	   TF0 = 0; //清除标志位		
	   Dist = 255;
  }
  else  //RX接收引脚接收到低电平
  {
	  Dist = ((TH0<<8)+TL0)*0.017;
  }
	
  return Dist;
}

void fen()
{
    switch(mod)
		{
			case 1:show_data();break;
			case 2:show_eeprom();break;
			case 3:can();break;
		}
}

void show_data()
{
	  if(mod_show==0){dspbuf[0]=0;}
		else{dspbuf[0]=1;}
		dspbuf[1] = 10;
	  dspbuf[2] = distance_copy/100;
		dspbuf[3] = distance_copy%100/10;
		dspbuf[4] = distance_copy%10;
	  dspbuf[5] = distance/100;
		dspbuf[6] = distance%100/10;
		dspbuf[7] = distance%10;	
}

void show_eeprom()
{
		dspbuf[0]=(count_look+1)/10;
	  dspbuf[1]=(count_look+1)%10;
	  dspbuf[2]=count/100;
	  dspbuf[3]=count%100/10;
	  dspbuf[4]=count%10;
	  dspbuf[5]=recording[count_look]/100;
	  dspbuf[6]=recording[count_look]%100/10;
	  dspbuf[7]=recording[count_look]%10;
}

void can()
{
		dspbuf[0]=11;dspbuf[1]=10;dspbuf[2]=10;
	  dspbuf[3]=10;dspbuf[4]=10;dspbuf[5]=10;
	  dspbuf[6]=standard/10;
	  dspbuf[7]=standard%10;
}

void voltage()
{
	  
		dac_pcf8591(distance-standard);
		if(distance-standard>255)
		{
				dac_pcf8591(255);
		}	
}

void eeprom_write()
{
		char i,j;
	  //unsigned char linshi; 
	  if(ok==1)
		{
				ok=0;
			  if(count<10)
				{recording[count]=distance;}
				else
				{
						for(i=0;i<9;i++)
					  {
								recording[i]=recording[i+1];
						}
						recording[9]=distance;
				}
				for(j=0;j<10;j++)
				{
						EA=0;write_eeprom(j,recording[j]);delay();EA=1;
				}
		}		
}

void delay()		//10ms @12MHz
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
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
		P2 = ((P2&0x1f)|0x80);
    P0 = ~Led_Flag;
		P2 &= 0x1f;
}