#include "cpu.h"
extern UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL); 
extern void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData);
extern void EEPromSectorErase(UCHAR nAddrH);
//=====================================================================================
//
//ssp_flag为EEPROM防跑飞判断标志位，防止程序因干扰跳转flash的操作函数中， 
//对flash执行非法操作，用户在使用过程中最好将读操作的标志位和写操作的标志位设为不同的值
//
//=====================================================================================
extern UCHAR ssp_flag;  