/*******************************************************************************  
* �ļ����ƣ�����ܿ���ʵ��
* ʵ��Ŀ�ģ�1.���������������·����Ʒ���
*           2.�����������ʾ�Ļ���ԭ��
* ʵ�����ã�J3��������ΪMM��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* ���������������ʾ
* Ӳ��������IAP15F2K61S2@11.0592MHz
* ע�����J13����ΪMMģʽʱ��ͨ���洢��ӳ�䷽ʽ��̣����洢ӳ���ԭ��
* ���ڰ汾��2019-6-24/V2.0
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"
#include "intrins.h"
                       
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void cls_buzz()
{
	XBYTE[0xA000] = 0x00;
}

void delay()	//delay 200ms @ 11.0592M
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

//������
void main(void)
{ 
	unsigned char i  = 10;	
	cls_buzz();	
    while(1)
	{
		(i== 10)?(i=0):(i++);
		XBYTE[0xE000] = tab[i];  //��ʾ��ֵ1   ��ѡ
		XBYTE[0xC000] = 0x55;    //λѡ
		delay();delay();delay();
		XBYTE[0xE000] = 0X00;
		
		XBYTE[0xE000] = tab[i+1];
		XBYTE[0xC000] = 0xAA;
		delay();delay();delay();
		XBYTE[0xE000] = 0X00;
    }
}


