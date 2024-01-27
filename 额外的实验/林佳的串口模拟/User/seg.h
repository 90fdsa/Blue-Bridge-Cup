#ifndef __SEG_H__
#define __SEG_H__
#include <STC15F2K60S2.H>


void Seg_Tran(unsigned char *seg_string,unsigned char *seg_buf);
void Seg_Disp(unsigned char *seg_buf,unsigned char pos);


#endif
