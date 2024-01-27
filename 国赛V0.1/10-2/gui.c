#include "stc15f2k60s2.h"
#include "init.h"
#include "display.h"
#include "key.h"
#include "onewire.h"
#include "ultrasonic.h"
#include "iic.h"
#include "led.h"
#include "series.h"
#include "string.h"
#include "stdio.h"

unsigned char rxstring[20];
unsigned char txstring[20];

extern unsigned char key_value,old_key_value;
unsigned char temperature_start_flag;
unsigned char sonic_start_flag;
unsigned char button_flag;
unsigned char dac_start_flag=1;
unsigned char done=0;
unsigned char n=0;//串口接收数据变量
unsigned char uart_flag=0;//串口接收完毕
//测量值
float true_temperature;
unsigned char distance;
unsigned int change_time_of_parameter;
//参数值
char temperature_parameter=30;
char distance_parameter=35;
char old_temperature_parameter=30;
char old_distance_parameter=35;


void temperature_start();
void sonic_start();
void uart_start();

void main()
{
	  change_time_of_parameter=read_eeprom(0x00);
		close_buzz();
    close_led(); 
	  pcf8591_init();
	  PCA_init();
    Timer1Init();
	  UartInit();
	  EA=1;
	  while(1)
		{
				fen();
			  temperature_start();
			  sonic_start();
			  uart_start();
		}
}

void timer1_proc() interrupt 3
{
		static unsigned char a;
	  static unsigned char tem;
	  static unsigned int son;
	  static unsigned int chang;
	  display();
	  if(++a==10)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_keyboard();
			  if(old_key_value<key_value){key_proc();}
				if(old_key_value>key_value){done=0;}
				//dac
				if(dac_start_flag==1)
				{
						if(distance<=distance_parameter){pcf8591_dac(102);}
						if(distance>distance_parameter){pcf8591_dac(204);}
				}
				if(dac_start_flag==0){pcf8591_dac(20);}
				//led
				led_pan();
				led();
		}
		if(++tem==100)
		{
				tem=0;
			  temperature_start_flag=1;
		}
		if(++son==300)
		{
				son=0;
			  sonic_start_flag=1;
		}
		if(key_value!=0)//判断长按键
		{
				chang++;
			  if(chang>=1000&&done==0)
        {
				   key_proc_long();
					 done=1;
				   chang=0;
				}
		}
}

void uart() interrupt 4//接受
{
	  unsigned char temp;
	  if(RI)
		{
				temp=SBUF;
			  if(temp=='\n')
				{
						rxstring[n]='\n';
						uart_flag=1;//收完了
						n=0;
				}
			  else
				{rxstring[n++]=temp;}
				RI=0;
				
//				P0|=0x80;
//	      P2=(P2&0x1f)|0x80;
//	      P2&=0x1f;
		}
}

void temperature_start()
{
		if(temperature_start_flag==1)
		{
				temperature_start_flag=0;
			  true_temperature=temperature_massure();
		}
}	

void sonic_start()
{
		if(sonic_start_flag==1)
		{
				sonic_start_flag=0;
			  distance=sonic();
			  if(distance>99){distance=99;}
		}
}	

void uart_start()
{
		if((uart_flag==1)&&(strcmp(rxstring,"ST\r\n")==0))
		{
				sprintf(txstring,"$%d,%3.2f\r\n",(int)distance,true_temperature);
				Uart_Send(txstring);
				uart_flag=0;
		}
		if((uart_flag==1)&&(strcmp(rxstring,"PARA\r\n")==0))
		{
				sprintf(txstring,"$%d,%d\r\n",(int)distance_parameter,(int)temperature_parameter);
				Uart_Send(txstring);
				uart_flag=0;
		}
}