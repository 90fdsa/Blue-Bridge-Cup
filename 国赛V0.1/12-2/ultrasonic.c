#include "ultrasonic.h"
#include "init.h"

sbit TX=P1^0;
sbit RX=P1^1;

unsigned char sonic()
{
		unsigned char dis=0,num=10;
	  
	  Timer0Init();
	  TX=0;
	
	  while(num--)  //����10��
    {
      TR0 = 1;
	    while(!TF0);  //��ʱʱ�䵽����������
	    TF0 = 0;      //�ֶ������־λ	
	    TX ^= 1;      //ȡ��
      Timer0Init();  //��ʼ����ʱ��0����ʼ����ʱ����ֵ	
    }


    //���շ���
	  TR0 = 0;		//��ʱ��0ֹͣ��ʱ
	  TF0 = 0;		//���TF0��־
	  TL0 = 0x00;	//���ö�ʱ��ֵ
	  TH0 = 0x00;	//���ö�ʱ��ֵ
	  TR0 = 1;

    while(RX && !TF0);  //�ȴ��յ�����	��ʱ���
    TR0 = 0;  //��ʱ��0ֹͣ��ʱ
	
    if(TF0)   //��ʱ���
    {
	     TF0 = 0; //�����־λ		
	     dis = 255;
    }
    else  //RX�������Ž��յ��͵�ƽ
    {
	    dis = ((TH0<<8)+TL0)*0.017;
    }
	
    return dis;
}