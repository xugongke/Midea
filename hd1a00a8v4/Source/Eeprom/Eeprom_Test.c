#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
#define BUF_LEN  5
u8 buf[BUF_LEN];
#include "eeprom_ext.h"
#include "main.h"
#if 0
void Eeprom_Test()
{
  UCHAR val;
//   CLKCON = 0x08;
//   for(i=0;i<10;i++);
//   CLKCON |= 0x04;
//======EEPROM 扇区X擦除========
//ssp_flag = 0xA5用于防止程序跑飞跳转到EEPROM擦除函数，造成对EEPROM的误擦除操作  
  ssp_flag = 0xA5;   //	该语句最好紧接着放到擦除之前函数前
  EEPromSectorErase(0);
//=====EEPROM 0x0096地址写值======
//ssp_flag=0x5A用于防止程序跑飞跳转到EEPROM读函数，造成对EEPROM的非法读操作
  ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
  EEPromByteProgram(0x0,0x96,0x3c);
//=====读取EEPROM 0x0096地址的值
  val=EEPromByteRead(0x0,0x96);
//====操作结束，恢复总中断=====
  while(1);
}
#endif

u8 EEPROM_ByteRead(volatile u8 Addr)
{
    Addr %= BUF_LEN;
    return EEPromByteRead(0x0,Addr+0x96);
}

void EEPROM_ByteWrite(u8 ADDR,u8 da)
{
    ADDR %= BUF_LEN;
    buf[0] = EEPROM_ByteRead(0);
    buf[1] = EEPROM_ByteRead(1);
    buf[2] = EEPROM_ByteRead(2);
#if BUF_LEN > 3
    buf[3] = EEPROM_ByteRead(3);
#endif

#if BUF_LEN > 4 //开机状态
     buf[4] = EEPROM_ByteRead(4);
#endif

    buf[ADDR] = da;
    ssp_flag = 0xA5;   //	该语句最好紧接着放到擦除之前函数前
    EEPromSectorErase(0);
    ssp_flag = 0xA5;   //	该语句最好紧接着放到擦除之前函数前
    //=====EEPROM 0x0096地址写值======
    //ssp_flag=0x5A用于防止程序跑飞跳转到EEPROM读函数，造成对EEPROM的非法读操作
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+0,buf[0]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+1,buf[1]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+2,buf[2]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#if BUF_LEN > 3
    EEPromByteProgram(0x0,0x96+3,buf[3]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#endif
#if BUF_LEN > 4 //开机状态
    EEPromByteProgram(0x0,0x96+4,buf[4]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#endif
    buf[0] = EEPROM_ByteRead(0);

}


void ResetAllPara(u8 user_flag)
{
    buf[AC_MODE_TEMP_ADDR] = 75; // 市电模式及光伏默认75℃
    buf[MIX_MODE_TEMP_ADDR] = 55; // 混动模式阶梯温度默认55 (原50)
    buf[WORK_MODE_ADDR]     = MODE_MIX; //默认混动模式
    buf[OPEN_STATE_ADDR]    = 0xa3;     //出厂默认关机 0x55:开机 0xa3:关机
    buf[FACTORY_MODE_ADDR]  = user_flag; //工厂模式 
    ssp_flag = 0xA5;   //	该语句最好紧接着放到擦除之前函数前
    EEPromSectorErase(0);
    ssp_flag = 0xA5;   //	该语句最好紧接着放到擦除之前函数前
    //=====EEPROM 0x0096地址写值======
    //ssp_flag=0x5A用于防止程序跑飞跳转到EEPROM读函数，造成对EEPROM的非法读操作
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+0,buf[0]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+1,buf[1]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
    EEPromByteProgram(0x0,0x96+2,buf[2]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#if BUF_LEN > 3
    EEPromByteProgram(0x0,0x96+3,buf[3]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#endif
#if BUF_LEN > 4 //开机状态
    EEPromByteProgram(0x0,0x96+4,buf[4]);
    ssp_flag = 0x5A; 	  //该语句最好紧接着放到读取函数前
#endif
    buf[0] = EEPROM_ByteRead(0);
}