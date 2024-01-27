/*******************************************************************************  
* �ļ����ƣ�DS18B20�¶ȴ�����ʵ��
* ʵ��Ŀ�ģ�1.���յ�����ͨѶ�����ص�͹���ʱ��
*           2.����51��Ƭ��ģ�ⵥ����ʱ��ĳ�����Ʒ���
*           3.����DS18B20�¶ȴ������Ĳ�������
* ʵ�����ã�J3��������ΪIO��ʽ��J5����ΪBTN��J2����Ϊ1-3��2-4
* �����������ִ����¶ȴ��������������ʾ�б仯
* Ӳ��˵����IAP15F2K61S2@11.0592MHz
* ע�������������Ϊ1���϶�
* ���ڰ汾��2019-6-24/V2.0
*******************************************************************************/

#include "reg52.h"  //����51��Ƭ�����⹦�ܼĴ���
#include "absacc.h"

//sbit S1  = P0^0;
//sbit S2  = P0^1;
//sbit S3  = P0^2;
//sbit S4  = P0^3;
 
//��������
/**********************************/
void cls_buzz();
void cls_led();
void button(unsigned char a);
void key_proc(unsigned char key);
unsigned char read_key(void);
void delay();
/**********************************/

//������
void main(void)
{ 
		unsigned char temp=0;
		cls_buzz();
		cls_led();   
    while(1)
    {
			//��ʾ���ݸ��� 
			button(temp); 
    }
}
void cls_buzz()
{
	P2 = ((P2&0x1f)|0xA0); 
	P0 = 0x00;
	P2 &= 0x1f;
}
void cls_led()
{
	P2 = ((P2&0x1f)|0x80); 
	P0 = 0xFF;
	P2 &= 0x1f;
}
void delay(void)
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}
//����ɨ�躯��
unsigned char read_key(void)
{
    unsigned char temp;
    unsigned char key_value = 0xff;
    
    temp = P3&0x0f;
    switch(temp)
    {
        case 0x0e:
            key_value = 1;  //S7
            break;    
        case 0x0d:
            key_value = 2;  //S6
            break;    
        case 0x0b:
            key_value = 3;  //S5
            break;    
        case 0x07:
            key_value = 4;  //S4
            break;    
    }
    return key_value;
}
//������������
void button(unsigned char a)
{
		a = read_key();
    if(a != 0xff)
    {
        delay();  //��������
        a = read_key();
        if(a != 0xff)
        {
            //���밴��������
            key_proc(a);
            while(P3 != 0xff);  //�ȴ������ͷ�                        
        }
    }
}

//����������
void key_proc(unsigned char key)
{
		static unsigned char time=1;
    switch(key)
    {
      case 1:
				P2 = (P2&0x1f|0x80);
				switch(time)
				{
					case 1:P0=0XFE;break;
					case 2:P0=0XFD;break;
					case 3:P0=0XFB;break;
					case 4:P0=0XF7;break;
				}
				time++;
				if(time==5)
				{time=1;}
				P2 &= 0x1f;
        break;
			case 2:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
				break;
      case 3:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
        break;
      case 4:
				P2 = (P2&0x1f|0x80);
				P2 &= 0x1f;
        break;
    }    
}

