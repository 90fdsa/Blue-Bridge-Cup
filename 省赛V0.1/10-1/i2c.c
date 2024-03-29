#include "reg52.h"
#include "intrins.h"

#define DELAY_TIME 5

/** 定义I2C总线时钟线和数据线 */
sbit scl = P2^0;
sbit sda = P2^1;

/**
* @brief I2C总线中一些必要的延时
*
* @param[in] i - 延时时间调整.
* @return none
*/
void i2c_delay(unsigned char i)
{
    do
    {
        _nop_();
    }
    while(i--);        
}

/**
* @brief 产生I2C总线启动条件.
*
* @param[in] none
* @param[out] none
* @return none
*/
void i2c_start(void)
{
    sda = 1;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 0;
    i2c_delay(DELAY_TIME);
    scl = 0;    
}

/**
* @brief 产生I2C总线停止条件
*
* @param[in] none
* @param[out] none.
* @return none
*/
void i2c_stop(void)
{
    sda = 0;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 1;
    i2c_delay(DELAY_TIME);       
}

/**
* @brief I2C发送一个字节的数据
*
* @param[in] byt - 待发送的字节
* @return none
*/
void i2c_sendbyte(unsigned char byt)
{
    unsigned char i;
//
	EA = 0;
    for(i=0; i<8; i++){
        scl = 0;
        i2c_delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
        i2c_delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
        i2c_delay(DELAY_TIME);
    }
	EA = 1;
//
    scl = 0;  
}

/**
* @brief 等待应答
*
* @param[in] none
* @param[out] none
* @return none
*/
unsigned char i2c_waitack(void)
{
	unsigned char ackbit;
	
    scl = 1;
    i2c_delay(DELAY_TIME);
    ackbit = sda; //while(sda);  //wait ack
    scl = 0;
    i2c_delay(DELAY_TIME);
	
	return ackbit;
}

/**
* @brief I2C接收一个字节数据
*
* @param[in] none
* @param[out] da
* @return da - 从I2C总线上接收到得数据
*/
unsigned char i2c_receivebyte(void)
{
	unsigned char da;
	unsigned char i;
	EA = 0;	
	for(i=0;i<8;i++){   
		scl = 1;
		i2c_delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		i2c_delay(DELAY_TIME);
	}
	EA = 1;
	return da;    
}

/**
* @brief 发送应答
*
* @param[in] ackbit - 设定是否发送应答
* @return - none
*/
void i2c_sendack(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit;  //0：发送应答信号；1：发送非应答信号
    i2c_delay(DELAY_TIME);
    scl = 1;
    i2c_delay(DELAY_TIME);
    scl = 0; 
	  sda = 1;
    i2c_delay(DELAY_TIME);
}

/**
* @brief 读写操作过程中一些必要的延时
*
* @param[in] i - 指定延时时间
* @return - none
*/
void operate_delay(unsigned char t)
{
	unsigned char i;
	
	while(t--){
		for(i=0; i<112; i++);
	}
}

/**
* @brief PCF8591初始化
*
* @param[in] none
* @param[out] none
* @return none
*/
void init_pcf8591(void)
{
	i2c_start();
	i2c_sendbyte(0x90);
	i2c_waitack();
	i2c_sendbyte(0x03);  //ADC通道3
	i2c_waitack();
	i2c_stop();
	operate_delay(10);
}

/**
* @brief 通过I2C总线读取ADC结果
*
* @param[in] none
* @param[out] temp - AD转换结果
* @return - temp
*/
unsigned char adc_pcf8591(void)
{
	unsigned char temp;
	
//	i2c_start();             //开始
//	i2c_sendbyte(0x90);      //我告诉你
//	i2c_waitack();           //诶，您说
//	i2c_sendbyte(0x03);      //我要读AIN3的值
//	i2c_waitack();           //诶，好的
//	i2c_stop();              //结束
//  如果是底下dac的sendbyte是0x43那就不用上面的语句
	
	i2c_start();             //开始
	i2c_sendbyte(0x91);      //照我说的读
	i2c_waitack();           //诶，好的
	temp = i2c_receivebyte();//正在读
	i2c_sendack(1);          //我读完了
	i2c_stop();              //结束
	
	return temp;
}

/**
* @brief DAC输出
*
* @param[in] dat - 输入进行数模转换的数据
* @param[out] none 
* @return - temp
*/
void dac_pcf8591(unsigned char dat)
{
	i2c_start();         //开始
	i2c_sendbyte(0x90);  //我告诉你
	i2c_waitack();       //诶，您说
	//i2c_sendbyte(0x40);  //我要输出dac
	i2c_sendbyte(0x43);  //边输入adc边输出dac的语句
	i2c_waitack();       //诶，好的
	
	i2c_sendbyte(dat);   //我要输出dat的DAC值
	i2c_waitack();       //诶，好的
	i2c_stop();          //结束
}


