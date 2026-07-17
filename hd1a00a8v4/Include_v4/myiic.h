#ifndef _MYIIC_H
#define _MYIIC_H
#include "SH79F326.h"
#include "cpu.h"

#if 1
//p4.3 output
#define SCL_OUT()  P4CR |= 0X8
//IO方向设置
//p4.2 output
#define SDA_OUT()   P4CR |= 0X4
//_pcc0 = 0; _pcpu0 = 0
//IO操作
#define IIC_SCL(n)  P4_3 = n //SCL
#define IIC_SDA(n)  P4_2 = n //SDA

#else 


#endif

/*TM1640B总线接口*/
void bus_Init(void);
void bus_start(void);
void bus_end(void);
void bus_sendbyte(unsigned char txd);
extern unsigned char sm1616_data_0[16];
int display(unsigned char * buf);
#endif

