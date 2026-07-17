/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          adc.h
*\Description   ADC相关函数和接口定义
*\Note          1)互感器采用规格固定为5A/2.5ma 即2000：1
                2)ADC采样到的电流最大值为:(ADCm*5/1024)/200   
                                         其中5表示ADC参考工作电压，200表示采样电阻为200欧姆 1024是10位ADC的满量程值
                3)参数配置中使用额定电流，额定电流*根下2（1.414）等于最大值，
                输入额定电流对应的ADC采样值为：ADCm=(额定电流*1.414)/2000 * 200 * 1024/5 =(额定电流*1.414)/10 *  1024/5 
                在后面的计算过程中直接对ADCm进行比较即可
                根据ADC采样最大值计算当前电流(单位:A):有效电流= (0.707*ADCm*5/1024)/200 * 2000= (0.707*ADCm*5/1024) * 10
                4)P=0.85*1.732UI=0.85*1.732*380*I  0.85为功率因数,额定电流:I=P/(0.85*1.732*380)
*\Log           2017.11.08    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#ifndef _ADC_H
#define _ADC_H
#include "stm8s.h"
#include "board.h"

#define ADC_FREQ_HZ   5000 /*ADC采样频率，使用定时器3实现频率控制 ADC1单次扫描模式*/

#define CURR_A_INDEX 2   /*A相电流数组索引*/
#define CURR_B_INDEX 3   /*B相电流数组索引*/
#define CURR_C_INDEX 4   /*C相电流数组索引*/

#define VOL_AB_INDEX 1  /*AB相间电压数组索引*/
#define VOL_BC_INDEX 0  /*AB相间电压数组索引*/

#define ADC_TOTAL_NUM   4
#define ADC_BUF_LEN     4   /*最底层采样平均次数*/
extern u16 AdcScanValue[ADC_TOTAL_NUM]; //单次扫描模式中的转换值

#define ADC_MAX_VALUE_NUM   50      //各个通道每个采样周期20ms的最大值, 存储连续1s的数据


extern u16 AdcValueArray[ADC_BUF_LEN][ADC_TOTAL_NUM-1/*ADC_TOTAL_NUM/2*/];  /*记录最近连续几次的最底层数据，用于计算平均值*/
extern u8  AdcBufIndex;

extern u16 ADCHistory[ADC_FREQ_HZ/50][ADC_TOTAL_NUM-1/*ADC_TOTAL_NUM/2*/]; //ADC历史数据,存储1个20ms的数据
extern u16 MaxValueCurr[ADC_TOTAL_NUM-1/*ADC_TOTAL_NUM/2*/];               //当前各个通道的最大值
extern u8  AdcHistoryIndex;  /*中断中计算20ms中的最大值*/

extern u16 MaxValueHistory[ADC_MAX_VALUE_NUM][ADC_TOTAL_NUM-1/*ADC_TOTAL_NUM/2*/];     //各个通道每个采样周期20ms的最大值, 存储连续1s的数据
extern u8  MaxValueHistoryIndex;   /*当前最新有效的电流最大值, 中断中维护，主循环中只读*/

extern u8 adc_err_cnt;

/*******************************************************************************
**函数名称：void ADC_Init()
**功能描述：初始化ADC
**入口参数：无
**输出：无
*******************************************************************************/
void ADC_Init(void);

/*******************************************************************************
**函数名称：void ADC_SCAN_Init()
**功能描述：初始化ADC,单次扫描模式，一次扫描所有的6个通道
**入口参数：无
**输出：无
*******************************************************************************/
void ADC_SCAN_Init(void);

/*******************************************************************************
**函数名称：void Get_ADC_AnyChannal_Value(u16 *AD_Value , u16 SamplesTimes)
**功能描述：读取ADC完成多次模数转换结果
**入口参数：u16 *AD_Value  存放多冷采样的平均值
            u16 SamplesTimes  采样次数
**输出：无
*******************************************************************************/
void Get_ADC_AnyChannal_Value(u16 *AD_Value, u16 SamplesTimes);

/***************************************************************************************************
*\Function      GetAllScanValue
*\Description   获取单次扫描模式中所有的ADC转换值
*\Parameter     
*\Return        void
*\Note          
***************************************************************************************************/
void GetAllScanValue(void);

void AdcMaxValueCalc(void);

#endif /*_ADC_H*/
