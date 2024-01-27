#include "init.h"
#include "show.h"
#include "key.h"
#include "led.h"

extern unsigned char buffer[8];

extern unsigned char key_value,old_key_value;
extern unsigned char led1;
//unsigned char n; 用于串口计数
//unsigned char txstring[],rxstring[];

void main()
{
		close_buzz();
		close_led();
		Timer1Init();
	  EA=1;
	  while(1)
		{
				buffer[6]=key_value/10;
			  buffer[7]=key_value%10;
		}
}

void timer1_proc() interrupt 3
{
		static unsigned char a;
	  static unsigned char dis;
	  static unsigned int hao;
//	  static unsigned int fre;
	  if(++dis==10){dis=0;display();}
	  if(++a==100)
		{
				a=0;
			  old_key_value=key_value;
			  key_value=read_key_juzhen();
			  if(old_key_value<key_value){key_proc();}
				led();
		}
		if(++hao==1000)//100ms
		{
				hao=0;
			  led1=~led1;
		}
//		hao++;
//		if(hao<8){/*给个高电平*/}
//		else if(hao>=8&hao<10){/*给个低电平*/}
//		else if(hao==10){hao=0;}
//		if(++fre=500)
//		{
//				fre=0;
//			  frequency=freq*2;
//			  freq=0;
//		}
}

//void timer0_proc() interrupt 1
//{
//		freq++;
//}

//void uart_proc() interrupt 4
//{
//		unsigned char temp;
//	  if(RI==1)
//		{
//				temp=SBUF;
//			  if(temp=='\n')
//				{
//						rxstring[n]='\0';
//					  uart_flag=1;
//					  n=0;
//				}
//				else
//				{
//					  rxstring[n]=temp;
//					  n++;
//				}
//				RI=0;
//		}
//}

//void start_send()
//{
//		if((uart_flag==1)&(strcmp(rxstring,"fuckyou\r")==0))
//		{
//				sprintf(txstring,"wo_ce_ni_de_ma");
//			  send_string(txstring);
//			  uart_flag=0;
//		}
//}