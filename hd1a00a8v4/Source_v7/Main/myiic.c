#include "myiic.h"

//延时nus								   
void delay_us(void) {
  char i = 1;
  do {
   	i--;
  }while(i);
}

//IIC初始化
void bus_Init(void)
{
    SCL_OUT();
    IIC_SCL(1);
    SDA_OUT();
    IIC_SDA(1);
}

void bus_start(void)
{
    IIC_SCL(1);
    IIC_SDA(0);
    delay_us();
    IIC_SCL(0);
}

void bus_sendbyte(unsigned char txd)
{   char i;
    for (i = 0; i < 8; i++)
    {
        IIC_SDA((txd&0x01));
        //delay_us();
        txd>>=1;
        IIC_SCL(1);
        delay_us();
        IIC_SCL(0);
        delay_us();
    }
}

void bus_end(void)
{
    IIC_SCL(0);
    IIC_SDA(0);
    delay_us();
    IIC_SCL(1);
    delay_us();
    IIC_SDA(1);
}



unsigned char sm1616_data_0[16] = {0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
 //================================================
//***********延时函数*****************************
//================================================
int display(unsigned char * buf)
{
    int i;
    bus_start();
    bus_sendbyte(0x40); /*地址加1模式*/
    bus_end();

    bus_start();  /*刷新显示数据*/
    bus_sendbyte(0xc0); //设置地址
    
    for(i=0; i< 16; i++){
        bus_sendbyte(buf[i]);
    }
    bus_end();
    bus_start();  /*刷新显示数据*/
    bus_sendbyte(0x8f); //显示开命令
    bus_end();
    //     bus_sendbyte(0x88); //显示开命令
    return 0;	
}