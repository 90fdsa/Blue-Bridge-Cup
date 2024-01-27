#include "jidianqi.h"


void open_jidian()
{
		P0=0x10;
	  P2=(P2&0x1f)|0xa0;
	  P2&=0x1f;
}
