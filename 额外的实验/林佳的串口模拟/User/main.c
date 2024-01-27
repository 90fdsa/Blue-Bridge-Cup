#include <STC15F2K60S2.H>
#include "stdio.h"
#include "Timer.h"
#include "seg.h"
#include "bsp_key.h"
#include "pcf8591.h"
#include "bsp_led.h"
#include "uart.h"
#include <string.h>

unsigned int seg_slow_down;
unsigned int key_slow_down;
unsigned int led_slow_down;

unsigned char ucled;

unsigned char Key_Value;
unsigned char Key_Old,Key_Down,Key_Up;

unsigned char pos;
unsigned char seg_buf[8];
unsigned char seg_string[10];


void Seg_Proc();
void Key_Proc();
void Led_Proc();


//======参数区
xdata float noise;//噪声
unsigned char noise_c=65;//噪声设定参数
unsigned char t_L3=0;
xdata unsigned char uart_T_string[18];//串口发送
xdata unsigned char uart_R_string[18];//串口接受
unsigned char temp;//串口中间变量
unsigned char n=0;//串口接受数据数量


//==标志位
bit stat_flag=0;//界面标志位
bit stat_more_flag=0;//分贝过大标志位
bit uart_flag=0;//串口接受标志位


void main()
{
	Timer0Init();
	Initsystem();
	UartInit();

	while(1)
	{
		
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}

}
void ServiceUart() interrupt 4
{
	
	if(RI==1)
	{
		temp=SBUF;
		if(temp == 'n')
		{
			uart_R_string[n++]=temp;
			uart_R_string[n]='\0';
			uart_flag=1;
			n=0;
		}
		else
		{
		uart_R_string[n++]=temp;

		}
		
		
		RI=0;
	}

	
}

void SeviceT0() interrupt 1
{
	
	if(++seg_slow_down==300) seg_slow_down=0;
	if(++key_slow_down==10) key_slow_down=0;
	if(++led_slow_down==300) led_slow_down=0;
	Seg_Disp(seg_buf,pos);
	if(++pos==8) pos=0;

	if(stat_more_flag==1)
	{
		t_L3++;
	}
	if(t_L3==100)
	{
		t_L3=0;
		ucled^=0x04;
	}
	Key_Disp(ucled);
	
}



void Seg_Proc()
{
	if(seg_slow_down) return;
	noise=(ADC()*18.0/51.0);
	
	if(stat_flag==0)
	{
	if((uart_flag==1)&&(strcmp(uart_R_string,"Return")==0))
	{
		
		sprintf(uart_T_string,"Noises:%3.1fdb\r\n",noise);
		sendString(uart_T_string);
		uart_flag=0;
	}
	
	sprintf(seg_string,"U1 %6.1f",noise);
	}
	
	
	else
	sprintf(seg_string,"U2%6d",(int)noise_c);
	
	
	
	
	Seg_Tran(seg_string,seg_buf);
	
}

void Key_Proc()
{
	if(key_slow_down) return;	
	
	Key_Value=Key_Read();
	Key_Down=Key_Value&(Key_Old^Key_Value);
	Key_Up=~Key_Value&(Key_Old^Key_Value);
	Key_Old=Key_Value;
	
	
	switch(Key_Down)
	{
		case 12:
		stat_flag^=1;
		
		break;
		case 16:
		if((noise_c<90)&&(stat_flag==1))
		{
			noise_c+=5;

		}			
		
		break;		
		
		case 17:
		if((noise_c>0)&&(stat_flag==1))
		{
			noise_c-=5;

		}				
		
		break;	
		
		
		
	}
	
}


void Led_Proc()
{
	if(led_slow_down) return;	
	
	if(stat_flag==0)
	{
		ucled|=0x01;
		ucled&=0xfd;

	}
	else
	{

		ucled|=0x02;
		ucled&=0xfe;
	}
	
	if(noise>noise_c)
	{
		stat_more_flag=1;
		
	}
	else
	{
		stat_more_flag=0;
		t_L3=0;
		ucled&=0xfb;
	}
	
	
}