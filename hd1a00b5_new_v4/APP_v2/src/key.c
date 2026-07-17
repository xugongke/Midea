/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          key.c
*\Description   
*\Note          
*\Log           2017.11.13    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#include "key.h"

u8 KeyDetectCntStop;
u8 KeyDetectCntQidong;
u8 KeyDetectCntDiandong;

u8 KeyStateStop;
u8 KeyStateQidong;
u8 KeyStateDiandong;
#if 0
#define KEY_DECT_CNT  2
/***************************************************************************************************
*\Function      KeyDetect
*\Description   按键检测,该函数中仅检测对外的停止、启动和点动按钮
*\Parameter     
*\Return        u8
*\Note          1)定时功能由应用层保证
*\Log           2017.11.13    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 KeyDetect(void)
{
    u8 key = KEY_INVALID;
    if (GPIO_READ_INPUT_PIN(TINGZHI_INPUT_PORT, TINGZHI_INPUT_PIN) == 0)
    {
        /*停止按钮被按下*/
        if (KeyDetectCntStop < KEY_DECT_CNT)
        {
            KeyDetectCntStop++;
        }
        else if (KeyDetectCntStop == KEY_DECT_CNT)
        {
            key = KEY_STOP_DOWN;
            //KeyDetectCntStop++;
        }
    }
    else
    {
        KeyDetectCntStop = 0;
    }

    if (GPIO_READ_INPUT_PIN(QIDONG_INPUT_PORT, QIDONG_INPUT_PIN) == 0)
    {
        /*启动按钮被按下*/
        if (KeyDetectCntQidong < KEY_DECT_CNT)
        {
            KeyDetectCntQidong++;
        }
        else
        {
            if (KeyDetectCntQidong == KEY_DECT_CNT)
            {
                if (key == KEY_INVALID)
                {
                    key = KEY_QIDONG_DOWN;
                }
                KeyDetectCntQidong++;
            }
        }
    }
    else
    {
        KeyDetectCntQidong = 0;
    }

    if (key == KEY_INVALID)
    {
        if ((KeyDetectCntDiandong == 0)  && (KeyDetectCntQidong == 0) && (KeyDetectCntStop == 0))
        {
            key = KEY_ALL_UP;
        }
    }

    return key;
}
#endif