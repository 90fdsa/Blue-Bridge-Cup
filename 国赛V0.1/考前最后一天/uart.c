#include "uart.h"

void send_string(unsigned char *uart_string)
{
		while(*uart_string!='\0')
		{
				SBUF=*uart_string;
			  while(TI==0);
			  TI=0;
			  uart_string++;
		}
}