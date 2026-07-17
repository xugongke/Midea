/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          para.h
*\Description   
*\Note          
*\Log           2017.11.14    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#ifndef _PARA_H
#define _PARA_H
#include "stm8s.h"
#include "board.h"

#define OVERLOAD_CURRENT_NUM 8

extern const u8 MotorPowerArray[OVERLOAD_CURRENT_NUM];
extern const u16 OverloadCurrent[OVERLOAD_CURRENT_NUM];
extern const u16 OverloadCurrentADC[OVERLOAD_CURRENT_NUM];

#define END_CODE1 0xf5
#define END_CODE2 0xaf
typedef struct ParaType_st
{
    u8 StartTime;               /*启动时间，单位s. 最大启动时间暂定为10s, 最小暂定为2s*/
    u8 OverLoadCurrentIndex;    /*过载电流选择索引， OverloadCurrent*/
    u8 StopTime;                /*停止时间，单位s 最小5s 最大200s*/
    u8 Res1;
    u8 Res2;
    u8 Res3;
    u8 end_code1;      //固定为0xf5
    u8 end_code2;      //固定为0xaf
}ParaType;

extern ParaType  SysPara;
extern u8 ParaSetIndex;  /*0:设置启动时间 1:设置过载电流  2:设置停止时间*/
void ReadPara(void);
void SavePara(void);
//检查系统参数的合法性
void SysParaCheck(void);
#endif /*_PARA_H*/
