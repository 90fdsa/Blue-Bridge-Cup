#ifndef _SMG_H
#define _SMG_H

#include "reg52.h"
static unsigned char SMG_duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xbf};

void Select573(unsigned char channel);
void SMG_bit(unsigned char whe,unsigned char dat);
void SMG_delay(unsigned int t);
#endif
