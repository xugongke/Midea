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

u16  time_s_dis;
#if TEST_ZHANHUI_EN
/***************************************************************************************************
*\Function      DisOpenStateDemo
*\Description   样机演示程序开机状态
*\Parameter     
*\Return        void
*\Note          
***************************************************************************************************/
void DisOpenStateDemo(void) 
{
    if (TempCur < 0)  {
        temp = 0; //零下温度显示0
    }
    else{
        temp = TempCur;	
        if ((time_boot - time_s_dis) > 1){
            TempLast = (TempCur+TempLast)/2;
        }
    }

    if (temp > 99) {
        temp = 99;
    }
    sm1616_data_0[0] = 0;
    sm1616_data_0[1] = 0;
    sm1616_data_0[2] = 0; 
    sm1616_data_0[3] = 0; 

    if (GFHeatFlag == 1){
        /*光伏加热*/
        sm1616_data_0[2] |= LED_DC;
        sm1616_data_0[2] |= LED_MD_DC; //光伏模式指示灯
    }
    if (ACHeatFlag != 0) {
        /*市电加热*/
        sm1616_data_0[2] |= LED_AC;
        sm1616_data_0[2] |= LED_MD_AC; // 市电模式
    }

    if (Mode == MODE_CHECK)
    {
        if (ErrCode == 9){
            sm1616_data_0[0] = BCD_decode_tab[0xe]; /*温度 十位*/
            sm1616_data_0[1] = BCD_decode_tab[9]; /*温度 个位*/
        }
        else {
            sm1616_data_0[0] = BCD_decode_tab[temp/10]; /*温度 十位*/
            sm1616_data_0[1] = BCD_decode_tab[temp%10]; /*温度 个位*/
            sm1616_data_0[3] |= LED_DEGREE;//℃图标
        }
        return;
    }
}
#endif
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
    if (TempCur < 0)  {
        temp = 0; //零下温度显示0
    }
    else{
        temp = TempCur;	
        if ((time_boot - time_s_dis) > 1){
            TempLast = (TempCur+TempLast)/2;
        }
    }

    if (temp > 99) {
        temp = 99;
    }
    sm1616_data_0[0] = 0;
    sm1616_data_0[1] = 0;
    sm1616_data_0[2] = 0; 
    sm1616_data_0[3] = 0; 

    if (FactoryLEDOff == 1){
        /*工厂自检模式 不点亮屏幕*/
        return;
    }
    if (Mode == MODE_CHECK)
    {
        if (ErrCode == 9){
            sm1616_data_0[0] = BCD_decode_tab[0xe]; /*温度 十位*/
            sm1616_data_0[1] = BCD_decode_tab[9]; /*温度 个位*/
            //sm1616_data_0[3] |= LED_DEGREE;//℃图标
        }
        else {
            sm1616_data_0[0] = BCD_decode_tab[temp/10]; /*温度 十位*/
            sm1616_data_0[1] = BCD_decode_tab[temp%10]; /*温度 个位*/
            sm1616_data_0[3] |= LED_DEGREE;//℃图标
        }
        //sm1616_data_0[0] = BCD_decode_tab[8]; /*温度 十位*/

        return;
    }
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
    case MODE_AC: //市电模式
        sm1616_data_0[2] |= LED_MD_AC; // 市电模式
        break;
    case MODE_GF: //纯光伏模式
        sm1616_data_0[2] |= LED_MD_DC; //光伏模式指示灯
        break;
    case MODE_MIX: //混动模式  图标全亮
        if ((revert == 0) &&  (SetingIndex == 3)){
            // sm1616_data_0[2] = 0; //光伏模式和市电模式指示灯闪烁指示
        }
        else {
            sm1616_data_0[2] |= LED_MD_DC|LED_MD_AC; //光伏模式和市电模式指示灯全亮
        }
        break;
    case MODE_HIGH_TEMP: //高温杀菌模式
        sm1616_data_0[3] |= LED_MD_HT;
       
        break;
    default:
        break;
    }
   
    /*实际温度*/
    if ((Mode == MODE_HIGH_TEMP) && (time_flash < 5))
    {
        if (revert == 0){
            sm1616_data_0[0] = 0; /*温度 十位*/
            sm1616_data_0[1] = 0; /*温度 个位*/
        }
        else {
            sm1616_data_0[0] = BCD_decode_tab[8]; /*温度 十位*/
            sm1616_data_0[1] = BCD_decode_tab[0]; /*温度 个位*/
        }
    }
    else {
        sm1616_data_0[0] = BCD_decode_tab[temp/10]; /*温度 十位*/
        sm1616_data_0[1] = BCD_decode_tab[temp%10]; /*温度 个位*/
    }
    sm1616_data_0[3] |= LED_DEGREE;//℃图标

    if ((SetingIndex == 1)||(SetingIndex == 3)){ /*设置温度闪烁*/
        if ((revert == 0) && (key_down == 0)){
            sm1616_data_0[0] = 0;
            sm1616_data_0[1] = 0;
        }
        else {
            if (SetingIndex == 3){//设置混合模式阶梯温度
                sm1616_data_0[0] = BCD_decode_tab[MixHeatUp/10];
                sm1616_data_0[1] = BCD_decode_tab[MixHeatUp%10];
            }
            else {
                sm1616_data_0[0] = BCD_decode_tab[TempHeatUp/10];
                sm1616_data_0[1] = BCD_decode_tab[TempHeatUp%10];
            }
        }
    }

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
        time_flash = 0; //闪烁控制
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
//         if (FactorMode != 0){
//             if (time_boot <= 16){
//                 StateCnt = 0;
//                 StateStartTick = tick;
//                 Mode = MODE_CHECK; //切换到自检模式
//                 break;
//             }
//         }
        SetingIndex = 0;
        switch (Mode)
        {
        case MODE_GF:
            Mode = MODE_MIX; //切换到混动模式
            EEPROM_ByteWrite(WORK_MODE_ADDR, Mode); //切换到混动模式
            ModeExchange();
            if (JLFlag == 1){
                if (TempCur < TempHeatUp){
                    HEAT_ON(); //立即启动加热
                    fast_cnt = 0;
                }
            }
            break;
        case MODE_AC:
            Mode = MODE_GF;
            EEPROM_ByteWrite(WORK_MODE_ADDR, Mode); //切换到光伏模式
            ModeExchange();			   
			HEAT_OFF(); //
			break;
        case MODE_MIX:
            Mode = MODE_AC;
            EEPROM_ByteWrite(WORK_MODE_ADDR, Mode); //切换到市电模式
            ModeExchange();
		    if (JLFlag == 1){
                if (TempCur < TempHeatUp){
                    HEAT_ON(); //立即启动加热
                    fast_cnt = 0;
                }
            }
            break;
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
        if (FactorMode != 0){
            if (time_boot <= 20){ //关机状态下也可以响应功率自检模式
                StateCnt = 0;
                StateStartTick = tick;
                Mode = MODE_CHECK; //切换到自检模式
                break;
            }
        }
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }

        updown_flag = 1; 
        switch (Mode)
        {
        case MODE_MIX:
            
        case MODE_AC:
        case MODE_GF:
            /*设置温度*/
            if (SetingIndex == 1){
                TempHeatUp += 1; //第一次按调整值时不改变温度值
            }
            if (SetingIndex == 0){
                SetingIndex = 1;
            }
            time_sec = 0;
            if (SetingIndex == 1){
                if (Mode == MODE_MIX){
                    if (TempHeatUp > 75) { //混动模式市电温度上限为75
                        TempHeatUp = HEAT_LOW_TEMP;
                    }
                }
                else 
                    if (TempHeatUp > 75) {
                        TempHeatUp = HEAT_LOW_TEMP;
                    }
                    TempHeatDown = TempHeatUp - AC_DOWN_TEMP;
                    if ((JLFlag == 1) && ((Mode == MODE_AC) /*|| ((Mode == MODE_MIX))*/)){
                        if (TempCur < TempHeatUp){
                            HEAT_ON(); //立即启动加热
                        }
                    }
            }
            else if (SetingIndex == 3){
                MixHeatUp++;
                if (MixHeatUp > 60){
                    MixHeatUp = HEAT_LOW_TEMP;
                }
                if ((JLFlag == 1) && (Mode == MODE_MIX)){
                    if (TempCur < MixHeatUp){
                        //HEAT_ON(); //立即启动加热
                    }
                }
            }
            break;
        default:
            break;
        }
        break;
    case KEY_VALUE_DOWN:
        if (FactorMode != 0){
            if (time_boot <= 20){
                StateCnt = 0;
                StateStartTick = tick;
                Mode = MODE_CHECK; //切换到自检模式
                break;
            }
        }
        if (RunState != FSM_OPEN){
            /*不是开机状态下不响应按钮*/
            break;
        }
       
        updown_flag = 1; 
        switch (Mode)
        {
        case MODE_MIX:
        case MODE_AC:
        case MODE_GF:
            /*设置温度*/
            if (SetingIndex == 1){
                TempHeatUp -= 1; //第一次按调整值时不改变温度值
            }
            if (SetingIndex == 0){
                SetingIndex = 1;
            }
            time_sec = 0;
            if (SetingIndex == 1)
            {
                if (TempHeatUp < HEAT_LOW_TEMP) {
                    TempHeatUp = 75;
//                     if (Mode == MODE_MIX){
//                         TempHeatUp = 60;//混动模式市电温度上限为60
//                     }
                }
                TempHeatDown = TempHeatUp - AC_DOWN_TEMP;
                if ((JLFlag == 1) && ((Mode == MODE_AC) /*|| ((Mode == MODE_MIX))*/)){
                    if (TempCur < TempHeatUp){
                        HEAT_ON(); //立即启动加热
                    }
                }
            }
            else if (SetingIndex == 3){
                MixHeatUp--;
                if (MixHeatUp < HEAT_LOW_TEMP){
                    MixHeatUp = 60;
                }
                if ((JLFlag == 1) && (Mode == MODE_MIX)){
                    if (TempCur < MixHeatUp){
                        //HEAT_ON(); //立即启动加热
                    }
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
