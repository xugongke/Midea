/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          gpio_init.c
*\Description   
*\Note          
*\Log           2017.11.09    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#include "stm8s.h"
#include "gpio_cfg.h"

/***************************************************************************************************
*\Function      GpioInit
*\Description   所有用到的通用输入输出功能引脚初始化
*\Parameter     
*\Return        void
*\Note          
*\Log           2017.11.09    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
void GpioInit(void)
{
    //继电器控制
    GPIO_Init(RELAY_CTRL1_PORT  , RELAY_CTRL1_PIN  , GPIO_MODE_OUT_PP_LOW_SLOW); //继电器
    GPIO_Init(RELAY_CTRL1_1_PORT, RELAY_CTRL1_1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW); //继电器
    GPIO_Init(RELAY_CTRL2_PORT  , RELAY_CTRL2_PIN  , GPIO_MODE_OUT_PP_LOW_SLOW); //MOS管
    GPIO_Init(VOL_TEST_PORT  , VOL_TEST_PIN  , GPIO_MODE_OUT_PP_LOW_SLOW);       //光伏电压检测切换输入引脚

    VOL_100V_IN;// 检测光伏输入电压
    RELAY_IDLE();
    GPIO_Init(MOS_CTRL2_PORT  , MOS_CTRL2_PIN  , GPIO_MODE_OUT_PP_LOW_SLOW);   //MOS管

    RELAY_CTRL2_START();
    MOS_CTRL2_STOP();
    //调试用LED
  //  GPIO_Init(DEBUG_LED1_PORT , DEBUG_LED1_PIN , GPIO_MODE_OUT_PP_LOW_FAST); //ctrl1

    AC_DECTECT_PIN_INIT; //交流是否存在检测
    AC_RELAY_INIT; //交流继电器
    //GPIO_Init(DEBUG_LED2_PORT , DEBUG_LED2_PIN , GPIO_MODE_OUT_PP_LOW_SLOW); //ctrl2
    GF_12V_DECTECT_PIN_INIT;//直流输出12V是否存在

    //未使用引脚统一初始化为:推挽输出，输出低电平  @todo 硬件引脚发生变化时,需要重新梳理空闲引脚
#if 0
//     GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
// 
//     //GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
//     //GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);
// 
//     GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
//     GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);

//     GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
//     GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);
//     GPIO_Init(GPIOA, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
#endif
}
