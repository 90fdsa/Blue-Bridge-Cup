#include "STC15F2K60S2.H"

unsigned char key_value=0;
unsigned char old_key_value=0;
unsigned char a=0,b=0,c=0;

void key_scan();
void delay(unsigned int a);
void key_proc_short();
void led_deal();

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

void timer0init()
{
		AUXR&=0x7f;
	  TMOD&=0xf0;
	  TH0=(65536-1000)/256;
	  TL0=(65536-1000)%256;
	  TF0=0;
	  TR0=1;
	  ET0=1;
}

void main()
{
		close_buzz();
		close_led();
	  timer0init();
	  EA=1;
	  while(1)
		{
			  key_proc_short();
				led_deal();
		}
}
	
void time0() interrupt 1
{
		static unsigned int a;
	  if(++a==10)
		{
			  a=0;
				key_scan();
		}
}

void key_scan()
{
	  static unsigned char hang;
	  static unsigned char lie;
		P3=0x0f;P42=0;P44=0;
	  if(P3!=0x0f)
		{
				delay(20);
			  if(P3!=0x0f)
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
				}
				else
				{
				    hang=0;lie=0;key_value=0;
					  P3=0x0f;P42=0;P44=0;
				}
		}
}
	
void delay(unsigned int a)
{
		while(a--);
}

void key_proc_short()
{
		switch(key_value)
		{
			  case 1:
				{
				    a=~a;
				}break;
				case 2:
				{
				    b=~b;
				}break;
				case 3:
				{
				    c=~c;
				}break;
		}
}
void led_deal()
{
	  unsigned char led;
		if(a==0){led&=0xfe;}
		else{led|=0x01;}
		if(b==0){led&=0xfd;}
		else{led|=0x02;}
		if(c==0){led&=0xfb;}
		else{led|=0x04;}
		
		P0=led;
		P2=(P2&0x1f)|0x80;
		P2&=0x1f;
}