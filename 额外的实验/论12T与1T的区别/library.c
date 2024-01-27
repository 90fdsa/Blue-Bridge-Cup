#include "STC15F2K60S2.H"

unsigned char a=0;
unsigned char key_value=0;

void keyboard_scan();

void close_buzz()
{
		P0=0x00;
	  P2=(P2&0x1f)|0xa0;
	  P2&=0x1f;
}

void close_led()
{
	  P0=0xff;
	  P2=(P2&0x1f)|0x80;
	  P2&=0x1f;
}

void timer_0_init()
{
		AUXR&=0X7F;  //12T
	  //AUXR|=0X80;  //1T,因无法准确定时而弃用
	  TMOD&=0xF0;
	  TH0=(65536-1000)/256;
	  TL0=(65535-1000)%256;
	  TF0=0;
	  TR0=1;
	  ET0=1;
}

void main()
{
    close_buzz();
	  close_led();
	  timer_0_init();
	  EA=1;
	  while(1)
		{
				
		}
}

void timer_0() interrupt 1
{
	  static unsigned int i;
	  if(++i==1000)
		{
			  i=0;
				a=~a;
				if(a!=0)
				{
						P0=0x00;
						P2=(P2&0x1f)|0x80;
						P2&=0x1f;
				}
				else
				{
						P0=0xff;
						P2=(P2&0x1f)|0x80;
						P2&=0x1f;
				}
		}	
}

void keyboard_scan()
{
		static unsigned char key_state;
	  static unsigned char hang;
	  static unsigned char lie;
	  switch(key_state)
		{
			  case 0:
			  {
				    P3=0x0f;P42=0;P44=0;
					  if(key_state!=0x0f){key_state=1;}
				}break;
				case 1:
				{
						P3=0x0f;P42=0;P44=0;
					  if(key_state!=0x0f)
						{
								if(P30==0){hang=1;}
								if(P31==0){hang=5;}
								if(P32==0){hang=9;}
								if(P33==0){hang=13;}
								P3=0xf0;P42=1;P44=1;
								if(P34==0){lie=3;}
								if(P35==0){lie=2;}
								if(P42==0){lie=1;}
								if(P44==0){lie=0;}
								key_value=hang+lie;
								key_state=2;
						}
						else
						{
								key_state=0;
						}
				}break;
				case 2:
				{
						P3=0x0f;P42=0;P44=0;
					  if(P3==0x0f)
						{
								key_state=0;
							  key_value=0;
						}
				}break;
		}
}