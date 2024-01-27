#include "init.h"
#include "show.h"

extern unsigned char buffer[];

void main()
{
		close_buzz();
    close_led();
		Timer2Init();
	  EA=1;
	  while(1)
		{
				buffer[2]=1;
			  buffer[3]=1;
			  buffer[4]=4;
			  buffer[5]=5;
        buffer[6]=1;   			  
			  buffer[7]=4;
		}
}

void timer2_proc() interrupt 12
{
	  display();
}