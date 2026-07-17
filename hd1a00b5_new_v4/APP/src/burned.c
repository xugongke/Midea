/*干烧判断逻辑*/
#include "stm8s.h"
#include "gpio_cfg.h"
#include "key.h"
#include "para.h"

u32 power_on_flag;
u32 last_heat_stop_time; /*上次实际断开加热时间 s*/

u32 start_sec_time; /*起始时间 s*/
u8 burned_st; /*干烧判断*/
u8 burned_cnt; /*干烧判断次数*/
u8 filter_cnt; /*温度滤波*/
s8 StartTemp; /*启动时温度*/
/*1:在加热状态 0:不在加热状态*/
#define HEAT_STATE_ON 1
u8 heatStateGet(void)
{
    u8 ex_st = 0;
    /*判断是否在加热状态*/
    /*交流部分*/
    if ((AC_HEAT_ON & CurState) != 0){
        ex_st = 1;
    }
    /*直流部分是否在加热*/
    if (((CurState & GF_HEAT_ON) != 0) && ((DC_VOL_EXIST&CurState) != 0)){
        ex_st = 1;
    }
    return ex_st;
}

void NormalHeatStart(void)
{
    /*交流启动加热*/
    if (((MCtrl & 0x01) != 0) 
        && ((CurState & AC_POWER_ON) != 0)
        && (Temperature < ACTempHigh)
       /* && (CommTimeout < 15)*/
        ){
        /*启动交流加热*/
        CurState |= AC_HEAT_ON;
        AC_RELAY_ON();
    }
    else{
        CurState &= ~AC_HEAT_ON; //交流加热指示
        MCtrl &= ~0x01;
        AC_RELAY_OFF;
    }

    //温度传感器异常 则不进行加热
    if ((Temperature <= TempLow)&&(Temperature > ERR_TEMP)) {
        /*当前温度低于阈值, 且不处于加热状态, 则启动加热*/
        if ((GetRelayState() == STATE_DISCONNECT) || (RELAY_CTRL_STATE() != RELAY_CLOSE_CTRL_LEVEL))
        {
            if ((power_on_flag == 0) ||
                ((sec_time - last_heat_stop_time)  > 29)
                ){/*增加一个延迟，防止过于频繁启停控制*/
                StartHeater();
                power_on_flag = 1;
            }
        }
    }
}

/*干烧判断*/
void burned_judge(void)
{
    //u8 ex_st = 0;
    if (((CurState & GF_VOL_HIGH) != 0) ||
        ((CurState & BURNED_PROTECT) != 0) || 
        ((CurState & GF_VOL_REV) != 0) 
        ){
            /*光伏电压过高 或者发生过干烧报警*/
            StopHeater();
            AC_RELAY_OFF;
            CurState &= ~AC_HEAT_ON; //交流加热指示
            MCtrl    &= ~0x01;
            burned_st = BN_ST_IDLE;
            return ;
    }

    if ((Temperature < ERR_TEMP) || (Temperature >= TempHigh)){ /*传感器异常或温度过高都停止直流加热*/
        if ((CurState & 0x08) != 0){ // 直流电压
            StopHeater();
        }
        CurState &= ~AC_HEAT_ON; //交流加热指示
        MCtrl &= ~0x01;
        AC_RELAY_OFF;//停止交流加热
    }
    else {
        /*温度正常时则启动加热*/
        if ((CurState & DC_VOL_EXIST) == 0){
            /*直流电不存在时,使能MOS加热部分*/
            MOS_CTRL1_START();
        }
    }

    switch (burned_st)
    {
    case BN_ST_IDLE:
        burned_cnt = 0;
        start_sec_time = sec_time;
        StartTemp = Temperature;
        filter_cnt = 0;
        NormalHeatStart();
        if (heatStateGet() == HEAT_STATE_ON) {
            burned_st = BN_ST_30S_HEAT; //切换到下个状态, 加热30s
        }
        break;
    case BN_ST_30S_HEAT:
        NormalHeatStart();
        if ((sec_time - start_sec_time) >= 30){
            /*加热时间超过30s开始下个阶段, 120s周期判断是否温度超过4℃*/
            StartTemp = Temperature;
            start_sec_time = sec_time;
            filter_cnt = 0;
            burned_st = BN_ST_120S_HEAT;
        }
        if (heatStateGet() != HEAT_STATE_ON){
            /*没有处在加热状态, 则终止干烧判断*/
            burned_st = BN_ST_IDLE;
        }
        break;
    case BN_ST_120S_HEAT:
        NormalHeatStart();
        if ((sec_time - start_sec_time) >= 120){
            /*120s内温度正常*/
             /*本次干烧判断圆满结束,且为正常, 则半小时内不在判断干烧*/
             burned_st = BN_ST_30MIN_WAIT;
             start_sec_time = sec_time;
        }

        if ((Temperature > StartTemp) && ((Temperature - StartTemp) >= 4)){
            /*大于等于4度以上则判断为干烧*/
            filter_cnt++;
            if (filter_cnt > 3){
                burned_cnt++;
                if (burned_cnt >= 3){
                    /*连续3次以上干烧,则彻底报警*/
                    StartTemp = Temperature;
                    start_sec_time = sec_time;
                    burned_st = BN_ST_BURNED;
                }
                else {
                    /*还未到连续3次触发条件*/
                    StartTemp = Temperature;
                    start_sec_time = sec_time;
                    burned_st = BN_ST_WAIT_150S;
                }
            }
        }

        if (heatStateGet() != HEAT_STATE_ON){
            /*没有处在加热状态, 则终止干烧判断*/
            burned_st = BN_ST_IDLE;
        }
        break;
    case BN_ST_30MIN_WAIT:
        NormalHeatStart();
        /*该段时间内 不再判断干烧*/
        if ((sec_time - start_sec_time) >= (u32)1800){
            burned_st = BN_ST_IDLE;
        }
        if (heatStateGet() != HEAT_STATE_ON){
            /*退出加热状态了, 则退出干烧等待期*/
            burned_st = BN_ST_IDLE;
        }
        break;
    case BN_ST_WAIT_150S:
         /*等待150s不加热,到达150s后重新到BN_ST_30S_HEAT*/
        if ((sec_time - start_sec_time) >= 150){
            StartTemp = Temperature;
            start_sec_time = sec_time;
            burned_st = BN_ST_30S_HEAT;
        }
        else {/*不到时间不启动加热*/
            StopHeater();
            CurState &= ~AC_HEAT_ON; //交流加热指示
            AC_RELAY_OFF;
        }
        break;
    case BN_ST_BURNED: //干烧状态, 不再允许任何加热操作
        StopHeater();
        AC_RELAY_OFF;
        CurState &= ~AC_HEAT_ON; //交流加热指示
        CurState |= BURNED_PROTECT;
        start_sec_time = sec_time;
        break;
    default:
        burned_st = BN_ST_IDLE;
        break;
    }
}
