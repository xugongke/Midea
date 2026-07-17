/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          gpio_cfg.h
*\Description   电路中所有用到的通用GPIO引脚相关定义
*\Note          
*\Log           2017.11.09    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#ifndef _GPIO_CFG_H
#define _GPIO_CFG_H
#include "stm8s.h"

#define  VolHighPower  60 /*双加热管加热阈值电压*/

extern u8 mos2_start; //第2路MOS是否启动加热  1:启动  0:未启动
extern u8 ResetRelayFlag; //是否需要复位继电器状态 1:需要复位
extern volatile unsigned char CommTimeout; //总线通讯超时计数
extern u8 MCtrl;       /*主显示板控制:  bit[0]:1:交流加热; 0:交流不加热   bit[1~7]暂时不使用*/
extern u8 CurState;    /*反馈给主显示板当前状态  bit[0]:1:交流加热; 0:交流不加热; bit[1]:直流加热状态  
                bit[2]:交流电是否存在  bit[3]:直流电是否存在   BIT[7]:光伏电源接反
                */
extern s8 ACTempHigh; /*交流温度阈值上限*/
extern s8 TempHigh; /*温度高阈值, 高于该阈值时停止加热*/
extern s8 TempLow;  /*温度低阈值, 低于该阈值时启动加热*/

extern s8 Temperature; /*2个50KNTC的温度值, 分压电阻采用10K 单位:℃*/
extern s8 TemperatureSend; /*2个50KNTC的温度值, 分压电阻采用10K 单位:℃*/
/*uart 半双工模式引脚切换*/
#define UART1_RX_PIN GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); 
#define UART1_TX_PIN GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW); 


#define GPIO_WRITE_HIGH(GPIOx, PortPins)        GPIOx->ODR |= (uint8_t)PortPins
#define GPIO_WRITE_LOW(GPIOx, PortPins)         GPIOx->ODR &= (uint8_t)(~PortPins)
#define GPIO_WRITE_REVERSE(GPIOx, PortPins)     GPIOx->ODR ^= (uint8_t)PortPins

#define GPIO_READ_INPUT_PIN(GPIOx, PortPins)  ((GPIOx->IDR & (uint8_t)PortPins)==0?0:1)

//220V输入检测
#define AC_DECTECT_PIN_INIT GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);   /*交流是否存在*/
#define AC_DECTECT_ST       GPIO_READ_INPUT_PIN(GPIOB, GPIO_PIN_5)

#define GF_12V_DECTECT_PIN_INIT    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);   /*光伏转换输出的12V是否存在*/
#define GF_12V_DECTECT_PIN         GPIO_READ_INPUT_PIN(GPIOB, GPIO_PIN_4)

/*交流加热继电器*/
#define AC_RELAY_INIT   GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW)
#define AC_RELAY_ON     GPIO_WRITE_HIGH(GPIOD, GPIO_PIN_4)
#define AC_RELAY_OFF    GPIO_WRITE_LOW(GPIOD, GPIO_PIN_4)

//继电器控制
#define RELAY_CTRL1_PORT    GPIOC        /*继电器 1脚 动作引脚*/
#define RELAY_CTRL1_1_PORT  GPIOC        /*继电器 2脚 保持引脚*/
#define RELAY_CTRL1_PIN     GPIO_PIN_6
#define RELAY_CTRL1_1_PIN   GPIO_PIN_5

#define RELAY_CTRL2_PORT GPIOC          /*MOS管*/
#define RELAY_CTRL2_PIN  GPIO_PIN_7

#define MOS_CTRL2_PORT GPIOC       /*新增一路MOS控制*/
#define MOS_CTRL2_PIN  GPIO_PIN_3  

#define  VOL_TEST_PORT  GPIOA  /* 控制电源检测切换*/
#define  VOL_TEST_PIN   GPIO_PIN_2
#define  VOL_100V_TEST  GPIO_WRITE_HIGH(VOL_TEST_PORT, VOL_TEST_PIN)    /*电压测试100输入*/
#define  VOL_100V_IN    GPIO_WRITE_LOW(VOL_TEST_PORT, VOL_TEST_PIN)     /*电压测试100 TEST输入*/

#define RELAY_CTRL1_START()   GPIO_WRITE_HIGH(RELAY_CTRL1_PORT, RELAY_CTRL1_PIN) 
#define RELAY_CTRL1_STOP()    GPIO_WRITE_LOW( RELAY_CTRL1_PORT, RELAY_CTRL1_PIN)

#define RELAY_CTRL1_1_START()   GPIO_WRITE_HIGH(RELAY_CTRL1_1_PORT, RELAY_CTRL1_1_PIN) 
#define RELAY_CTRL1_1_STOP()    GPIO_WRITE_LOW( RELAY_CTRL1_1_PORT, RELAY_CTRL1_1_PIN)

#define RELAY_CLOSE()       RELAY_CTRL1_STOP();RELAY_CTRL1_1_STOP()      /*磁保持继电器闭合*/
#define RELAY_DISCONNECT()  RELAY_CTRL1_START();RELAY_CTRL1_1_START()    /*磁保持继电器断开*/

#define RELAY_IDLE()   RELAY_CTRL1_STOP()   /*继电器保持当前状态, 动作引脚停止*/

#define RELAY_CTRL2_START() GPIO_WRITE_HIGH(RELAY_CTRL2_PORT, RELAY_CTRL2_PIN) 
#define RELAY_CTRL2_STOP()  GPIO_WRITE_LOW( RELAY_CTRL2_PORT, RELAY_CTRL2_PIN)

#define MOS_CTRL2_START() mos2_start = 1;GPIO_WRITE_HIGH(MOS_CTRL2_PORT, MOS_CTRL2_PIN) 
#define MOS_CTRL2_STOP()  mos2_start = 0;GPIO_WRITE_LOW( MOS_CTRL2_PORT, MOS_CTRL2_PIN) 

#define DEBUG_LED1_ON()     
#define DEBUG_LED1_OFF()    


#define DEBUG_LED1_EVERT()  

/***************************************************************************************************
*\Function      GpioInit
*\Description   所有用到的通用输入输出功能引脚初始化
*\Parameter     
*\Return        void
*\Note          
*\Log           2017.11.09    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
void GpioInit(void);
/***************************************************************************************************
*\Function      OpenHeaterRAW
*\Description   启动直流加热功能,不进行任何状态维护
*\Parameter     
*\Return        u8 1:启动成功  0:启动失败
*\Note          1)用于直流电源不正常时复位继电器状态
*\Log           2019.02.26    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 StartHeaterRAW(void);
#endif /*_GPIO_CFG_H*/
