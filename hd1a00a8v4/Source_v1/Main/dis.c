/***************************************************************************************************
*                    (c) Copyright 1992-2019 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          dis.c
*\Description   显示界面
*\Note          
***************************************************************************************************/

#include "main.h"
#include "string.h"

vu8 dis_index;   /*进度条闪烁索引*/
u8 temp;
//const u16 LEDFlashArray[11] = {0x0000,0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00FF, 0x01ff, 0x03ff};
/***************************************************************************************************
*\Function      DisInitState
*\Description   故障显示
*\Parameter     
***************************************************************************************************/
void DisErrState(void) 
{
    memset(sm1616_data_0, 0x00, 16);
    if (revert == 0) { //实际温度字段
        sm1616_data_0[0] = BCD_decode_tab[0x0e]; /*温度 十位*/
        sm1616_data_0[1] = BCD_decode_tab[ErrCode%10]; /*温度 个位   摄氏度*/
    }
}

extern u8 last_data_buf;
extern u8 cur_key;
/***************************************************************************************************
*\Function      DisInitState
*\Description   待机状态
*\Parameter     
*\Return        void
*\Note          
***************************************************************************************************/
void DisInitState(void) 
{
     memset(sm1616_data_0, 0x00, 16);  //待机状态下只点亮开机按钮
}

/***************************************************************************************************
*\Function      DisInitState
*\Description   开机状态
*\Parameter     
*\Return        void
*\Note          
***************************************************************************************************/
void DisOpenState(void) 
{
//     if(key_valid != 0){
//         code_index = key_valid;
//     }
    //code_index++;
   // code_index %= 16;
   // sm1616_data_0[13] = 0; //按钮指示灯 高亮控制
   // sm1616_data_0[14] = LED_BOOAST_BIT|LED_MODE_BIT|LED_SET_BIT|LED_DOWN_BIT|LED_UP_BIT|LED_SWITCH_BIT; //半亮控制

    sm1616_data_0[0] = 0;
    sm1616_data_0[1] = 0;
    sm1616_data_0[2] = 0; 
    sm1616_data_0[3] = 0; 

    if (GFHeatFlag == 1){
        /*光伏加热*/
        sm1616_data_0[2] |= LED_DC;
    }
    //if ((RunStateFlag & HEATING_FLAG) != 0){
   // if ((ACHeatFlag != 0) && ((RunStateFlag & HEATING_FLAG) != 0)){
    if (ACHeatFlag != 0) {
        /*市电加热*/
        sm1616_data_0[2] |= LED_AC;
    }
 
    switch (Mode)
    {
    case MODE_BOOAST: //boost 模式  图标全亮
        sm1616_data_0[2] |= LED_MD_AC; //DC图标 市电模式
        break;
//     case MODE_TIMEING: //预约模式  图标全亮
//        // sm1616_data_0[13] |= LED_SET_BIT;
//         break;
//     case MODE_ECO: //ECO模式
//         sm1616_data_0[4] |= LED_ECO;
//         break;
    case MODE_HIGH_TEMP: //高温杀菌模式
        sm1616_data_0[3] |= LED_MD_HT;
        break;
    default:
        break;
    }
   
    if (TempCur < 0)  {
        temp = 0; //零下温度显示0
    }
    else{
        temp = TempCur;	
    }
    if (temp > 99) {
        temp = 99;
    }

    /*实际温度*/
    sm1616_data_0[0] = BCD_decode_tab[temp/10]; /*温度 十位*/
    sm1616_data_0[1] = BCD_decode_tab[temp%10]; /*温度 个位*/
    sm1616_data_0[3] |= LED_DEGREE;//℃图标

    if ((SetingIndex == 1)||(SetingIndex == 3)){ /*设置温度闪烁*/
        if ((revert == 0) && (key_down == 0)){
            sm1616_data_0[0] = 0;
            sm1616_data_0[1] = 0;
        }
        else {
            sm1616_data_0[0] = BCD_decode_tab[TempHeatUp/10];
            sm1616_data_0[1] = BCD_decode_tab[TempHeatUp%10];
        }
    }
//     if (SetingIndex == 10){//设置直流设置温度
//         if (revert == 1){
//             sm1616_data_0[0] = BCD_decode_tab[DCTempHeatUp/10]; /*设置温度 十位*/
//             sm1616_data_0[1] = BCD_decode_tab[DCTempHeatUp%10] | 0x80; /*设置温度 个位*/
//         }
//     }
   
    /*光伏电存在*/
    if (GFFlag != 0){
        sm1616_data_0[2] |= LED_MD_DC; //DC图标
    }
     /*市电存在*/
    if (JLFlag != 0){
        //sm1616_data_0[2] |= LED_MD_AC; //DC图标
    }
//     if (revert == 0)
//     {
//         sm1616_data_0[0] = BCD_decode_tab[key_valid%10];
//         sm1616_data_0[1] = BCD_decode_tab[9%10];
//     }
//     else {
//         sm1616_data_0[0] = BCD_decode_tab[TempHeatUp/10];
//         sm1616_data_0[1] = BCD_decode_tab[TempHeatUp%10];
// 
//     }
}

void KeyProcess(u8 key) {
    time_sec = 0;
    switch (key)
    {
    case KEY_VALUE_HT:
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }
        TempHeatUp = 80;
        TempHeatDown = TempHeatUp-AC_DOWN_TEMP;
        DCTempHeatUp = 80;
        Mode = MODE_HIGH_TEMP; //高温杀菌模式
        ht_done_flag = 0;
        done_flag = 0;
        if (JLFlag == 1){
            if (TempCur < TempHeatUp){
                HEAT_ON(); //立即启动加热
            }
        }
        break; 
    case  KEY_VALUE_MODE:
        /*@TODO模式选择处理*/
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }
        SetingIndex = 0;
        switch (Mode)
        {
        case MODE_OPEN:
            Mode = MODE_BOOAST; //切换到速热模式
            done_flag = 0; //加热未完成
            if (TempHeatUp > 75){
                 TempHeatUp =  EEPROM_ByteRead(00); //获取存储温度
            }
            if (TempHeatUp != EEPROM_ByteRead(00)){
                EEPROM_ByteWrite(00, TempHeatUp);
            }
            TempHeatDown = TempHeatUp-AC_DOWN_TEMP;
            if (JLFlag == 1){
                if (TempCur < TempHeatUp){
                    HEAT_ON(); //立即启动加热
                    fast_cnt = 0;
                }
            }
            break;
        case MODE_BOOAST:
        case MODE_HIGH_TEMP:
            TempHeatUp = EEPROM_ByteRead(00); //获取存储温度
            TempHeatDown = TempHeatUp-AC_DOWN_TEMP;
            Mode = MODE_OPEN; //回到开机默认状态
            done_flag = 0;
            HEAT_OFF();
            fast_cnt = 0;
            break;
        default:
            break;
        }
        break;
   
    case KEY_VALUE_UP:
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }
        updown_flag = 1; 
        switch (Mode)
        {
        case MODE_BOOAST:
        case MODE_OPEN:
           /*只在非模式下响应*/
            /*设置温度*/
            TempHeatUp += 1;
            SetingIndex = 1;
            time_sec = 0;
            if (TempHeatUp > 75) {
                TempHeatUp = HEAT_LOW_TEMP;
            }
            TempHeatDown = TempHeatUp - AC_DOWN_TEMP;
             if ((JLFlag == 1) && (Mode == MODE_BOOAST)){
                if (TempCur < TempHeatUp){
                    HEAT_ON(); //立即启动加热
                }
            }
            break;
        default:
            break;
        }
        break;
    case KEY_VALUE_DOWN:
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }
        updown_flag = 1; 
        switch (Mode)
        {
        case MODE_BOOAST:
        case MODE_OPEN:
            /*只在非模式下响应*/
            /*设置温度*/
            SetingIndex = 1;
            time_sec = 0;
            /*设置温度*/
            TempHeatUp -= 1;
            if (TempHeatUp < HEAT_LOW_TEMP) {
                TempHeatUp = 75;
            }
            TempHeatDown = TempHeatUp - AC_DOWN_TEMP;
            if ((JLFlag == 1) && (Mode == MODE_BOOAST)){
                if (TempCur < TempHeatUp){
                    HEAT_ON(); //立即启动加热
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        SetingIndex = 0;
        break;
    }
}
