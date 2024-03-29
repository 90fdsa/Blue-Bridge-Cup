/*******************************************************************************  
* 文件名称：DAC转换实验
* 实验目的：1.掌握I2C总线通讯基本特点和工作时序
*           2.掌握51单片机模拟I2C总线时序的程序设计方法
*           3.掌握PCF8591 ADC的操作方法
* 实验配置：J3跳线配置为MM方式，J5配置为BTN、J2配置为1-3和2-4
* 程序现象：按S7、S6按键，实现转换值变化，其中前面四位数码管显
            示数模转换的数字值，后四位显示模拟电压值，单位uV
* 硬件环境：IAP15F2K61S2@11.0592MHz
* 注意事项：用万用表测量J3排针上面的D\A引脚，测量电压与显示电压值进行对比，
            分析误差原因
* 日期版本：2019-6-24/V2.0
*******************************************************************************/

#include "reg52.h"  //定义51单片机特殊功能寄存器
#include "i2c.h"    //I2C总线驱动库
#include "absacc.h"

#define VREF  4850  //PCF8591参考电压

sfr AUXR = 0x8E; 
							 //0,1,2,3,4,5,6,7,8,9,熄灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
                             0xff
                           };
unsigned char dspbuf[8] = {10,10,10,10,0,0,0,0};  //显示缓冲区
unsigned char dspcom = 0;
bit key_re;
unsigned char key_press;
unsigned char key_value;

bit key_flag;
unsigned char intr;

void display(void);
unsigned char read_key(void);

void cls_buzz()
{
	XBYTE[0xA000] = 0;
}
void cls_led()
{
	XBYTE[0x8000] = 0xFF;
}


//主函数
void main(void)
{ 
    unsigned char key_temp;
	unsigned char dac_value;  //DAC转换数据
	unsigned int Vaout;  //输出电压
	
	cls_buzz();cls_led();
    init_pcf8591();
	AUXR |= 0x80;
	TMOD &= 0xF0;
	TL0 = 0xCD;	
	TH0 = 0xD4;	
	TF0 = 0;	
	TR0 = 1;
	ET0 = 1;
	EA = 1;
	
    while(1)
    {
        if(key_flag)
        {
            key_flag = 0;
            key_temp = read_key();
        }        
        //进入按键处理程序部分
        switch(key_temp)
        {
            case 1:
			{
				dac_value=dac_value+5;
				if(dac_value>255)dac_value=255;
				dac_pcf8591(dac_value);		
			}     			
                break;
            case 2:
			{
                dac_value=dac_value-5;
				if(dac_value<0)dac_value=0;
				dac_pcf8591(dac_value);		
			}
                break;      
        }
		key_temp = 0;  //完成按键处理程序后，清除键值
		
		Vaout = VREF/255*dac_value;
		
		dspbuf[0] = 10;
		(Vaout>=100)?(dspbuf[1] = dac_value/100):(dspbuf[1] = 10);    
		(Vaout>=10)?(dspbuf[2] = dac_value%100/10):(dspbuf[2] = 10);      
		dspbuf[3] = dac_value%10; 
				
		//显示数据更新
		(Vaout>=1000)?(dspbuf[4] = Vaout/1000):(dspbuf[4] = 10);  
		(Vaout>=100)?(dspbuf[5] = Vaout%1000/100):(dspbuf[5] = 10);    
		(Vaout>=10)?(dspbuf[6] = Vaout%100/10):(dspbuf[6] = 10);      
		dspbuf[7] = Vaout%10; 
    }
}

//定时器中断服务函数
void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
    display();
	if(++intr == 10)  //1ms执行一次
	{
        intr = 0;
		key_flag = 1;  //10ms按键扫描标志位置1
    }
}

//显示函数
void display(void)
{   
	XBYTE[0xE000] = 0xff;  //消隐
	
	XBYTE[0xC000] = (1<<dspcom);
    XBYTE[0xE000] = tab[dspbuf[dspcom]]; //段码
   
    if(++dspcom == 8){
        dspcom = 0;
    }   
}

//读取键值
unsigned char read_key(void)
{
	
    unsigned char key_temp;
    
	key_temp = (P3&0x0f);
	
    if(key_temp != 0x0f) //有按键按下
        key_press++;
	else
		key_press = 0;  //抖动
    
    if(key_press == 5)
    {
		key_press = 0;
		key_re = 1;
		switch(key_temp)
        {
            case 0x0e:
                key_value = 1;  //S1
                break;                
            case 0x0d:
                key_value = 2;  //S5
                break;    
            case 0x0b:
                key_value = 3;  //S9
                break;    
            case 0x07:
                key_value = 4;  //S13
                break;
        }
    }
    //连续三次检测到按键被按下，并且该按键已经释放
    if((key_re == 1) && (key_temp == 0x0f))
    {
        key_re = 0;
        return key_value;
    }
    
    return 0xff;  //无按键按下或被按下的按键未被释放    
}