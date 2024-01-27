#include "STC15F2K60S2.H"  //�ô�ͷ�ļ�ֱ�Ӵ���REG52.H�����������ͷ�ļ��Ա�
#include "absacc.h"
#include "i2c.h"
#include "intrins.h"
                           //  0    1    2    3    4    5    6    7    8    9   Ϩ��
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xFF,0x8e};
unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};  //��ʾ������

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
sbit TX = P1^0;  //��������
sbit RX = P1^1;  //��������

bit key_flag = 0;//����ɨ���־λ                                                                                          
unsigned char key_value=0xFF;//����ֵ
unsigned char s_flag=0;
unsigned int distance;
unsigned int time=0;
unsigned char mod=1;
unsigned char mod_show=0;
unsigned int distance_copy=0;
unsigned char standard=50;
unsigned char count=0;
unsigned char count_look=0;
unsigned char record;
unsigned char recording[10];
unsigned char ok,ok_2;
unsigned char led_1=0,led_2=0,led_3=0;
unsigned char Led_Flag;
/******��������***********************/
void read_keyboard(void);
void display();
void Timer0Init(void);		//12΢��@12.000MHz
void Timer1Init(void);		//12΢��@12.000MHz
unsigned char Sonic();
void fen();
void show_data();
void show_eeprom();
void can();
void kong();
void key_proc();
void voltage();
void eeprom_write();
void delay();	//10ms @11.0592MHz
void led_show();
/************************************/
void cls_buzz(void)	//�رշ�����
{
	P2 = (P2&0x1F|0xA0);
	P0 = 0x00;
	P2 &= 0x1F;
}

void cls_led(void)//�ر�����LED
{
    P2 = ((P2&0x1f)|0x80);
	P0 = 0xff;  
	P2 &= 0x1f;
}

void Timer0Init(void)		//12΢��@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xF4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;		//��ʱ��0ֹͣ��ʱ
}

void Timer1Init(void)		//12΢��@12.000MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x20;		//���ö�ʱ��ֵ
	TH1 = 0xD1;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF0��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;    //�򿪶�ʱ���ж�
}

//������
void main(void)
{ 
  char q;	
	cls_buzz();
	cls_led();
  Timer0Init();
	Timer1Init();
	init_pcf8591();
	EA=0;
	standard=read_eeprom(0x10);
	delay();
	count=read_eeprom(0x11);
	delay();
  for(q=0;q<10;q++)
	{
			recording[q]=read_eeprom(q);
		  delay();
	}
	EA=1;
  while(1)
  {    
		  key_proc();
		  fen();
		  if(distance-standard>0){voltage();}
		  led_show();
  }
}

//��ʱ���жϷ�����
void isr_timer_1(void)  interrupt 3  //Ĭ���ж����ȼ� 1
{     
	static unsigned char intr = 0;
	static unsigned char le=0;
	static unsigned char ji=0;
	if(++intr == 10) 
	{
     intr = 0;
		 read_keyboard(); 
  }
	if(ok_2==1)
	{
			if(++le==200)
			{
				  le=0;
					led_1=~led_1;
				  ji++;
				  if(ji==10)
					{ok_2=0;ji=0;le=0;}
			}
	}
  display(); //1msִ��һ��
}

//��ȡ������̼�ֵ:ת�Ӱ���ʹ��P42��P44����8051����
//˳���е�P36��P37����
void read_keyboard(void)
{
    static unsigned char hang;
	static unsigned char key_state=0;	
	switch(key_state)
	{
		case 0:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //�а�������
			key_state=1;	
		}break;
		case 1:
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 != 0x0f) //�а�������
			{
				if(P30 == 0)hang = 1;
				if(P31 == 0)hang = 2;
				if(P32 == 0)hang = 3;
				if(P33 == 0)hang = 4;//ȷ����	    
				switch(hang){
					case 1:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=0;key_state=2;}
						else if(P42 == 0) {key_value=1;key_state=2;}
						else if(P35 == 0) {key_value=2;key_state=2;}
						else if(P34 == 0) {key_value=3;key_state=2;}
					}break;
					case 2:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=4;key_state=2;}
						else if(P42 == 0) {key_value=5;key_state=2;}
						else if(P35 == 0) {key_value=6;key_state=2;}
						else if(P34 == 0) {key_value=7;key_state=2;}
					}break;
					case 3:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=8;key_state=2;}
						else if(P42 == 0) {key_value=9;key_state=2;}
						else if(P35 == 0) {key_value=10;key_state=2;}
						else if(P34 == 0) {key_value=11;key_state=2;}
					}break;
					case 4:{P3 = 0xf0; P42 = 1; P44 = 1;
						if(P44 == 0) {key_value=12;key_state=2;}
						else if(P42 == 0) {key_value=13;key_state=2;}
						else if(P35 == 0) {key_value=14;key_state=2;}
						else if(P34 == 0) {key_value=15;key_state=2;}
					}break;
				}	
			}
			else
			{
				key_state=0;	
			}  	   
		}break;
		case 2:     
		{
			P3 = 0x0f; P42 = 0; P44 = 0;
			if(P3 == 0x0f) //�����ſ�
			key_state=0;	
		}break;
						
    } 
	
}


//��ʾ����
void display(void)
{   
	static unsigned char dspcom = 0;
	P0 = 0xff;
	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;

	P0 = 1<<dspcom;
	P2 = ((P2&0x1f)|0xC0); 
	P2 &= 0x1f;

	P0 = tab[dspbuf[dspcom]];	
  	P2 = ((P2&0x1f)|0xE0); 
	P2 &= 0x1f;
  
    if(++dspcom == 8){
        dspcom = 0;
    }    
}

void key_proc()
{
	static unsigned char a=0;
	static unsigned char b=0;
	switch(key_value)
	{
		case 12://S4
		{
			distance_copy=distance;
			distance = Sonic();
			if(mod_show!=0)
      {distance_copy=distance+distance_copy;}
			count++;
			EA=0;write_eeprom(0x11,count);delay();EA=1;
			ok=1;ok_2=1;
			eeprom_write();
			key_value=0xff;
			break;
		}
		case 8://S5,���ݻ���
		{
			a=~a;led_2=~led_2;
			if(a!=0){mod=2;}
			else{mod=1;}
			
			key_value=0xff;
			break;
		}
		case 9://S9
		{
			b=~b;led_3=~led_3;
			if(b!=0){mod=3;}
			else
      {
			  mod=1;
				EA=0;write_eeprom(0x10,standard);delay();EA=1;
				delay();
			}
			key_value=0xff;
			break;
		}
		case 13://S8
		{
			if(mod==1){mod_show=~mod_show;}
			if(mod==2)
			{
					count_look++;
				  if(count_look==10)
					{
							count_look=0;
					}
//					EA=0;
//					record=read_eeprom(count_look);
//					EA=1;
					delay();
			}
			if(mod==3)
			{
					standard+=10;
				  if(standard==100){standard=0;}
			}
			key_value=0xff;
			break;
		}
		case 0:
		{
		  write_eeprom(0x11,0);
			write_eeprom(0x00,0);
			write_eeprom(0x01,0);
			write_eeprom(0x02,0);
			write_eeprom(0x03,0);
			write_eeprom(0x04,0);
			write_eeprom(0x05,0);
			write_eeprom(0x06,0);
			write_eeprom(0x07,0);
			write_eeprom(0x08,0);
			write_eeprom(0x09,0);
			write_eeprom(0x0a,0);
			write_eeprom(0x0b,0);
			count=0;
		}
	}
}

unsigned char Sonic(void)
{
  unsigned char Dist=0,num=10;
	
//TX���ŷ���10��Ƶ��Ϊ40Hz����
  Timer0Init();  //��ʼ����ʱ��0����ʼ����ʱ����ֵ
  TX = 0;        //����������Ϊ�͵�ƽ
  
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
	   Dist = 255;
  }
  else  //RX�������Ž��յ��͵�ƽ
  {
	  Dist = ((TH0<<8)+TL0)*0.017;
  }
	
  return Dist;
}

void fen()
{
    switch(mod)
		{
			case 1:show_data();break;
			case 2:show_eeprom();break;
			case 3:can();break;
		}
}

void show_data()
{
	  if(mod_show==0){dspbuf[0]=0;}
		else{dspbuf[0]=1;}
		dspbuf[1] = 10;
	  dspbuf[2] = distance_copy/100;
		dspbuf[3] = distance_copy%100/10;
		dspbuf[4] = distance_copy%10;
	  dspbuf[5] = distance/100;
		dspbuf[6] = distance%100/10;
		dspbuf[7] = distance%10;	
}

void show_eeprom()
{
		dspbuf[0]=(count_look+1)/10;
	  dspbuf[1]=(count_look+1)%10;
	  dspbuf[2]=count/100;
	  dspbuf[3]=count%100/10;
	  dspbuf[4]=count%10;
	  dspbuf[5]=recording[count_look]/100;
	  dspbuf[6]=recording[count_look]%100/10;
	  dspbuf[7]=recording[count_look]%10;
}

void can()
{
		dspbuf[0]=11;dspbuf[1]=10;dspbuf[2]=10;
	  dspbuf[3]=10;dspbuf[4]=10;dspbuf[5]=10;
	  dspbuf[6]=standard/10;
	  dspbuf[7]=standard%10;
}

void voltage()
{
	  
		dac_pcf8591(distance-standard);
		if(distance-standard>255)
		{
				dac_pcf8591(255);
		}	
}

void eeprom_write()
{
		char i,j;
	  //unsigned char linshi; 
	  if(ok==1)
		{
				ok=0;
			  if(count<10)
				{recording[count]=distance;}
				else
				{
						for(i=0;i<9;i++)
					  {
								recording[i]=recording[i+1];
						}
						recording[9]=distance;
				}
				for(j=0;j<10;j++)
				{
						EA=0;write_eeprom(j,recording[j]);delay();EA=1;
				}
		}		
}

void delay()		//10ms @12MHz
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}

void led_show()
{
	  //led1
	  if(led_1!=0)
	  {
		  Led_Flag |= 0x01;
		}
		else
		{
			Led_Flag &= 0xfe;
		}
	
		//led7
		if(led_2!=0)
		{
			Led_Flag |= 0x40;
		}
		else
		{
			Led_Flag &= 0xbf;
		}
	
		//led8
		if(led_3!=0)
		{
			Led_Flag |= 0x80;
	  }
	  else
	  {
		  Led_Flag &= 0x7f;
	  }
		P2 = ((P2&0x1f)|0x80);
    P0 = ~Led_Flag;
		P2 &= 0x1f;
}