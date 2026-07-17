#include "myiic.h"

//—” ±nus								   
void delay_us(void) {
  char i = 1;
  do {
   	i--;
  }while(i);
}

//IIC≥ı ºªØ
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


