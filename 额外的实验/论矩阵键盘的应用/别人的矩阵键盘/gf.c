#include <STC15F2K60S2.H>
code unsigned char smg_DM[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
unsigned char Key_OutPut,IFLong_OutPut;
void Delay(unsigned int xms)		//@11.0592MHz
{
		while(xms--)
		{
		unsigned char i, j;
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}
 
void smg_bit(unsigned char com,unsigned char num)//数码管显示函数
{
	P2 = (P2 & 0x1f) | 0xC0;
	P0 = 1 << (com - 1);
	P2 = (P2 & 0x1f) | 0xE0;
	P0 = smg_DM[num];
	Delay(2);
	P0 = 0xff;
}
 
unsigned char Key_Scan(void)
{
	unsigned int Key_New;
	unsigned char Key_Value;
	P3 |= 0x0F;
	P44 = 0; P42 = 1; P35 = 1; P34 = 1;  // 第一列
	Key_New = P3 & 0X0F;
	P44 = 1; P42 = 0; P35 = 1; P34 = 1;  // 第二列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;  // 第三列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	P44 = 1; P42 = 1; P35 = 1; P34 = 0;  // 第四列
	Key_New = (Key_New << 4) | (P3 & 0X0F); 
	switch(~Key_New)
	{
		case 0x8000: Key_Value = 1; break;
		case 0x4000: Key_Value = 2; break;
		case 0x2000: Key_Value = 3; break;
		case 0x1000: Key_Value = 4; break;		
		case 0x0800: Key_Value = 5; break;
		case 0x0400: Key_Value = 6; break;
		case 0x0200: Key_Value = 7; break;
		case 0x0100: Key_Value = 8; break;		
		case 0x0080: Key_Value = 9; break;
		case 0x0040: Key_Value = 10; break;
		case 0x0020: Key_Value = 11; break;
		case 0x0010: Key_Value = 12; break;			
		case 0x0008: Key_Value = 13; break;
		case 0x0004: Key_Value = 14; break;
		case 0x0002: Key_Value = 15; break;
		case 0x0001: Key_Value = 16; break;			
		default : Key_Value = 0;
	}
	return Key_Value;
}
	
unsigned char LongOrShort()
{
	unsigned int TimeCount = 0;
	unsigned int EnterFlag = 0;
	while(Key_Scan() != 0)
	{
		TimeCount++;
		EnterFlag = 1;
		Delay(1);
	}
	if(TimeCount<500 & EnterFlag == 1)
	{
		TimeCount = 0;
		EnterFlag = 0;
		return 0;
	}
	else if(TimeCount>500 & EnterFlag == 1)
	{
		TimeCount = 0;
		EnterFlag = 0;
		return 1;
	}
	return IFLong_OutPut;
}
 
void main()
{
	while(1)
	{
		if(Key_Scan()) Key_OutPut = Key_Scan();
		IFLong_OutPut = LongOrShort();
		if(IFLong_OutPut){P2 = (P2 & 0x1f) | 0x80;P0 = 0;}//长按点亮所有LED，反之关闭所有
		else {P2 = (P2 & 0x1f) | 0x80;P0 = 0xFF;}
		smg_bit(1,Key_OutPut/10);//显示按键编号
		smg_bit(2,Key_OutPut%10);
	}
}