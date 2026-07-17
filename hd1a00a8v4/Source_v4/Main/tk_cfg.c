#include "SH79F326.h"
#include "api_ext.h"
#include "cpu.h"
#include "myiic.h"
#include "string.h"
#include "main.h"
#include "intrins.h"
#include	"tk.h"

#if (PRODUCE_TK_ENABLE  ==0 )	
	#pragma message "打开支持手直接摸功能----调试使用版本"		
	#define	EFT_INJECT_PARA 65535		
	#define	HAND_EN			1		
#else	
	#pragma message "关闭支持手直接摸功能----量产使用版本 ，EFT_INJECT = 1500"			
	#define	EFT_INJECT_PARA 1500		
	#define	HAND_EN			0	
#endif		

#ifdef	TEST_TK_DATA	
	#pragma message "打开串口调试 波特率115200bps"				
#else	
	#pragma message "关闭串口调试"					
#endif


extern	void 	InitTKPara(void) ;

// 开放触摸中断出来  20181120
extern	void 	 TK_Isr (void);
void TK_Interrupt (void) interrupt  1
{
    TK_Isr ();
}
// 默认不修改该表格
unsigned int code Tab_TK_CODE[] = 
{
	0x0000,		//没键	
	//*** 如下面的信息没有填，当非法键按下的时候，不会自动复位，可调用复位reset_TK 手动复位
	
};




#define		FREQ_3M				4			//2.89 -- 5.8
#define		FREQ_2_4M			5			//4  -- 7.5
#define		FREQ_2M				6			//5.37 -- 9.13
#define		FREQ_1_7M			7			//6.5 -- 10.6
#define		FREQ_1_5M			8			//7.21 -- 12.13
#define		FREQ_1_33M			9
#define		FREQ_1_2M			10
#define		FREQ_1_09M			11
#define		FREQ_1M				12
#define		FREQ_923K			13
#define		FREQ_857K			14
#define		FREQ_800K			15
#define		FREQ_750K			16
		

#define TK(n)   (n-1)

//一般项目需要修改参数:
/* 主要参数
1:触摸键个数
2:触摸通道配置

//  灵敏度相关的三个参数
3:灵敏度阀值
4:中放大倍数,调试灵敏度

5:中频率及基准电压、OP电压 (一般默认 先不修改该参数，主要调改3、4参数为主)
*/
Struct_TK_Para	code	TK_Para = 
{
	/*--------------------------------------------------------------*/
	/*		以下部分由项目开发工程师修改							*/
	/*	note ：此部分，可以根据项目需要进行修改。					*/
	/*--------------------------------------------------------------*/
//	//按键个数(1~16)		16个按键(max)		*** ??????
//	4,										

//	//触摸通道配置，按键个数配置相应的通道个数	，多余的可以默认填写0 *** ??????
//	TK(18),TK(22),TK(23),TK(24),  0,0,0,0,   0,0,0,0,  0,0,0,0,

//	//此参数默认0 默认不用修改
//	0,0,0,0,0,0,0,0,0,0,						0,0,0,0,0,0,	//复用情况***
//	//此参数默认0 默认不用修改
//	0,0,0,0,0,0,0,0,0,0,						0,0,0,0,0,0,	//邻键					   ***

//	//原始纹波，一般范围(30~45) ,建议默认值35				  ***??????
//	// 调试触摸数据，建议触摸纹波小于35，如存在个别通道一直长时间大于35情况, 建议查看vdd是否有波动，或者通道受到干扰
//	35, 35, 35, 35, 35, 35, 35,35, 35, 35, 35,35, 35,35,35,35,	//原始纹波				  ***??????
//	
//	//灵敏度阈值，一般取人手操作时变化量的60%~70%,  建议大于 ***	>=110??????
//	130,130,130,130,	130,130,130,130,	130,130,130,130,  130,130,130,130,	//灵敏度		 ***	>=110??????
 
 
 	//按键个数(1~16)		16个按键(max)		*** ??????
	4,										

	//触摸通道配置，按键个数配置相应的通道个数	，多余的可以默认填写0 *** ??????
	//PUMP	,未使用	, Mode , 未使用 ,  未使用 , 
	TK(5)	, TK(6), TK(7), TK(8), 0, 0,   0, 0,   0,0,0,0,  

	//此参数默认0 默认不用修改
	0,0,0,0,  0,0,0,0,   0,0,0,0,	//复用情况***
	//此参数默认0 默认不用修改
	0,0,0,0,  0,0,0,0,   0,0,0,0,	//邻键					   ***

	//原始纹波，一般范围(30~45) ,建议默认值35				  ***??????
	// 调试触摸数据，建议触摸纹波小于35，如存在个别通道一直长时间大于35情况, 建议查看vdd是否有波动，或者通道受到干扰
	35, 35, 35, 35, 35, 35, 35,35, 35, 35, 35,35, //原始纹波				  ***??????
	
	//灵敏度阈值，一般取人手操作时变化量的60%~70%,  建议大于 ***	>=110??????
//	400,400,400,600,	600,600,600,600,	600,600,600,600,	 	//灵敏度		 ***	>=110??????
	220,120,190,120,	120,600,600,600,	600,600,600,600,	 	//灵敏度		20240412 libq3
	
	/*--------------------------------------------------------------*/
	/*		以下部分仅限中颖原厂/代理商调试使用						         */
	/*	note ：此部分修改，需要跟代理商/原厂工程师确定	,默认不修改		*/
	/*--------------------------------------------------------------*/ 
	1,									//需要调试三角形时间 ***  		默认不用修改
	GHG_T(10305),						//高转中  *** 测试时间		默认不用修改
	GHG_T(12500),						//中转高  *** 测试时间+		默认不用修改
	GHG_T(7000),						//中转低  *** 测试时间-		默认不用修改
	GHG_T(10305),						//低转中  *** 测试时间		默认不用修改	

	
	/*--------------------------------------------------------------*/
	/*		以下部分由项目开发工程师修改							*/
	/*	note ：此部分，可以根据项目需要进行修改。					*/
	/*--------------------------------------------------------------*/
	FREQ_3M,							//高频率	 			//默认不用修改
	2,//3									//高放大倍数	  //默认不用修改

//目前只使用到中频率，频率是触摸扫描的频率，频率越快，扫描触摸越快，相对灵敏度会提高
// 一般范围可以 (FREQ_1_5M~FREQ_2_4M)
	FREQ_2M,	//	,		//中频率				//FS17 1.84M
//目前只使用到中放大倍数，倍数越大，灵敏度越高
// 一般范围可以(1~5)
	5,									//中放大倍数	  ??????

	FREQ_1_7M,							//低频率			//默认不用修改
	2,//3									//低放大倍数	  //默认不用修改
	
	/*--------------------------------------------------------------*/
	/*		以下部分仅限中颖原厂/代理商调试使用						*/
	/*	note ：此部分修改，需要跟代理商/原厂工程师确定				*/
	/*--------------------------------------------------------------*/	
	20+5,									//原始数据去抖
	5+3,									//基准去抖
	2+1,									//单键去抖
	2+2,									//所有键去抖
		
	//跟随设置
	0,									//COM1~COM8	不跟随  默认不修改
	0,									//S1~S8		不跟随  默认不修改

//触摸基准电压选择4V 及OP电压设置 2V   :0x33
//触摸基准电压选择3V 及OP电压设置 1.5V :0x33+0x04+0x40
//触摸基准电压选择3V 及OP电压设置 1V   :0x33+0x04+0x80
	0x33+0x04+0x40,			//TKVREF，填0x33 使用 4V, 再加上0x04 使用 3V
							//4V比3V 采集数据花的时间短，相同放大倍数得到的数据更大，按键灵敏度更高
							//		 电源比较差的就用3V,一般情况下用4V	
	/*--------------------------------------------------------------*/
	/*		以下部分仅限中颖原厂调试使用							*/
	/*	note ：禁止修改！！！！					,默认不修改					*/
	/*--------------------------------------------------------------*/	
	//抗干扰补丁
	HAND_EN,							//支持手直接摸弹簧		
	0,									//高温补丁1允许位 					 		***
	GHG_T(1000),						//高温补丁1参数：环境突变马上重新适应		***
	0,								 	//高温补丁2允许位  默认	屏蔽 						***
	0,	  								//滑条补丁
	EFT_INJECT_PARA,					//大变化量忽略	
	((sizeof(Tab_TK_CODE))/2),			//有效列表组合数量	
	0xff,									//数据异常补丁	
	100,							    //数据异常参数
};

#define	KEY1_OUT_L	{P1_0 = 0;	P1CR  |=  bit0;}	
#define	KEY2_OUT_L	{P1_1 = 0;	P1CR  |=  bit1;}	
#define	KEY3_OUT_L	{P1_2 = 0;	P1CR  |=  bit2;}	
#define	KEY4_OUT_L	{P1_3 = 0;	P1CR  |=  bit3;}	
#define	KEY5_OUT_L	{P1_4 = 0;	P1CR  |=  bit4;}	
#define	KEY6_OUT_L	{P1_5 = 0;	P1CR  |=  bit5;}	
#define	KEY7_OUT_L	{P1_6 = 0;	P1CR  |=  bit6;}	
#define	KEY8_OUT_L	{P1_7 = 0;	P1CR  |=  bit7;}	
#define	KEY9_OUT_L	{P0_0 = 0;	P0CR  |=  bit0;}
#define	KEY10_OUT_L	{P0_1 = 0;	P0CR  |=  bit1;}
#define	KEY11_OUT_L	{P0_2 = 0;	P0CR  |=  bit2;}
#define	KEY12_OUT_L	{P0_3 = 0;	P0CR  |=  bit3;}
#define	KEY13_OUT_L	{P0_4 = 0;	P0CR  |=  bit4;}
#define	KEY14_OUT_L	{P0_5 = 0;	P0CR  |=  bit5;}
#define	KEY15_OUT_L	{P0_6 = 0;	P0CR  |=  bit6;}												
#define	KEY16_OUT_L	{INSCON |= 0x40;	P5_0 = 0;P5CR |=  bit0;	INSCON &= 0xBF; }			
#define	KEY17_OUT_L	{INSCON |= 0x40;	P5_1 = 0;P5CR |=  bit1;	INSCON &= 0xBF; }			
#define	KEY18_OUT_L	{INSCON |= 0x40;	P5_2 = 0;P5CR |=  bit2;	INSCON &= 0xBF;	}			
#define	KEY19_OUT_L	{INSCON |= 0x40;	P5_3 = 0;P5CR |=  bit3;	INSCON &= 0xBF;	}			
#define	KEY20_OUT_L	{P4_5 = 0;	P4CR  |=  bit5;}			
#define	KEY21_OUT_L	{P4_4 = 0;	P4CR  |=  bit4;}												
#define	KEY22_OUT_L	{P4_3 = 0;	P4CR  |=  bit3;}												
#define	KEY23_OUT_L	{P4_2 = 0;	P4CR  |=  bit2;}
#define	KEY24_OUT_L	{P4_1 = 0;	P4CR  |=  bit1;}

#define	CX_OUT_L	 {P0_7 = 0;	P0CR   |=  bit7;}	

///----------------------------------------end  20190910------------------------------------
char idata counterTKPeriod=0;
//触摸扫描函数，建议主循环一直调用调用
void ScanTKPress(void)
{
    unsigned char i;
    //	if(_testbit_(gbFlag1ms_ScanTK))/// gbFlag1ms_ScanTK每1ms 置1
    //{
    if((++counterTKPeriod) >= (TK_SCAN_PERIOD)) //   一个周期
    {
        counterTKPeriod = 0; 		
        stop_scan_tk();				
        gWrCurKey = catchKey();
        if(gWrCurKey)
            i=gWrCurKey;
    }	
    //	}
    SanTk();	
}

#if 1
void InitKey(void) 
{
    unsigned char i;

	CLKCON = 0x20;		   				//326 内部RC 24分频 默认 2分频，= 12M 如果外接晶振8M 无需分频
	_nop_();
	_nop_();
	_nop_();
	_nop_();

	for(i=0;i<TK_Para.num;i++)
	{
		switch(TK_Para.chn[i])
		{
//					case 0:		KEY1_OUT_L		break;		   //TK1--P1.0
//					case 1:		KEY2_OUT_L		break;		   //TK2--P1.1
//					case 2:		KEY3_OUT_L		break;		   //TK3--P1.2
//					case 3:		KEY4_OUT_L		break;		   //TK4--P1.3
					case 4:		KEY5_OUT_L		break;		   //TK5--P1.4
					case 5:		KEY6_OUT_L		break;		   //TK6--P1.5
					case 6:		KEY7_OUT_L		break;		   //TK7--P1.6
					case 7:		KEY8_OUT_L		break;		   //TK8--P1.7
//					case 8:		KEY9_OUT_L		break;		   //TK9 --P0.0	  	
//					case 9:		KEY10_OUT_L		break;		   //TK10--P0.1
//					case 10:	KEY11_OUT_L		break;		   //TK11--P0.2
//					case 11:	KEY12_OUT_L		break;		   //TK12--P0.3
//					case 12:	KEY13_OUT_L		break;		   //TK13--P0.4
//					case 13:	KEY14_OUT_L		break;		   //TK14--P0.5
//					case 14:	KEY15_OUT_L		break;		   //TK15--P0.6	 
//					case 15:	KEY16_OUT_L		break;		   //TK16--P5.0
//					case 16:	KEY17_OUT_L		break;		   //TK17--P5.1
//					case 17:	KEY18_OUT_L		break;		   //TK18--P5.2
//					case 18:	KEY19_OUT_L		break;		   //TK19--P5.3
//					case 19:	KEY20_OUT_L		break;		   //TK20--P4.5
//					case 20:	KEY21_OUT_L		break;		   //TK21--P4.4	 
//					case 21:	KEY22_OUT_L		break;		   //TK22--P4.3
//					case 22:	KEY23_OUT_L		break;		   //TK23--P4.2
//					case 23:	KEY24_OUT_L		break;		   //TK24--P4.1	
		}	
	}
	
	CX_OUT_L

	
	//隐藏寄存器配置
	#if	(PACKAGE == SH79F326_32PIN)
#pragma message "芯片封装:SH79F326----32PIN"		
			P0	 &= (~(bit0+bit1+bit2+bit3+bit4+bit5));
			P0CR |=   (bit0+bit1+bit2+bit3+bit4+bit5) ;
			
			P2	 &= (~(bit0+bit1+bit2+bit3));
			P2CR |=   (bit0+bit1+bit2+bit3) ;
			
			P4	 &= (~(bit5+bit6+bit7));
			P4CR |=   (bit5+bit6+bit7) ;
	
			INSCON |= 0x40;
			P5	 &= (~(bit3+bit4+bit5+bit6+bit7));
			P5CR |=   (bit3+bit4+bit5+bit6+bit7) ;
			INSCON &= 0xBF; 	
	#endif
	
	#if (PACKAGE == SH79F326_28PIN)
#pragma message "芯片封装:SH79F326----28PIN"		
			P0	 &= (~(bit3+bit4+bit5+bit6));
			P0CR |=   (bit3+bit4+bit5+bit6) ;
			
			P2	 &= (~(bit0+bit1+bit2+bit3+bit4+bit5+bit6+bit7));
			P2CR |=   (bit0+bit1+bit2+bit3+bit4+bit5+bit6+bit7) ;
		
			P4	 &= (~(bit4+bit5+bit6+bit7));
			P4CR |=   (bit4+bit5+bit6+bit7) ;
	
			INSCON |= 0x40;
			P5	 &= (~(bit2+bit3+bit4+bit5+bit6+bit7));
			P5CR |=   (bit2+bit3+bit4+bit5+bit6+bit7) ;
			INSCON &= 0xBF; 				 
	#endif
	
	#if	(PACKAGE == SH79F328_48PIN)
#pragma message "芯片封装:SH79F328----48PIN"				
			//无隐藏IO
	#endif
	
	#if (PACKAGE == SH79F328_44PIN)
#pragma message "芯片封装:SH79F328----44PIN"			
			P4	 &= (~(bit6+bit7));
			P4CR |=   (bit6+bit7) ;
	
			INSCON |= 0x40;
			P5	 &= (~(bit4+bit5));
			P5CR &= (~(bit4+bit5)) ;
			INSCON &= 0xBF; 				 
	#endif
	
		
			
	#if (PACKAGE == SH79F9260_32PIN)
#pragma message "芯片封装:SH79F9260----32PIN"			
		
	//SH79f9260 隐藏寄存器配置
			P0	 &= (~(bit0+bit1+bit2+bit3+bit4+bit5+bit6));
			P0CR |=   (bit0+bit1+bit2+bit3+bit4+bit5+bit6) ;
			
			P2	 &= (~(bit3+bit4+bit5));
			P2CR |=   (bit3+bit4+bit5) ;
			
			P4	 &= (~(bit5+bit6+bit7));
			P4CR |=   (bit5+bit6+bit7) ;
	
			INSCON |= 0x40;
			P5	 &= (~(bit3+bit4+bit5+bit6+bit7));
			P5CR |=   (bit3+bit4+bit5+bit6+bit7) ;
			INSCON &= 0xBF; 				 
	#endif


	InitTKPara();

// 解决警告处理，
	GetTKNum();
	getTKCxt();//getTKResult();// getTKCxt();			
	GetTKSample(0);// 触摸原始数据
	GetTKBase(0);	 //触摸基准值	
	GetTKRaw(0);
	GetTKVersion();
}
#endif

unsigned int	gWrCurKey;	 
unsigned char	gBKeyValidFg;
unsigned int	gWRestT;
unsigned int	gWFailT;

unsigned char	gBLatchErrFg;


unsigned int	Key_DataInput(void)
{
	unsigned int	val;
	
	gBKeyValidFg = 1;

	val = getTKResult();

  if(val != 0)
	{
		_nop_();
		
	}
	switch(val)
	{	 
		//支持的按键值
		case 0x0000:	
			gBLatchErrFg = 0;					//等到按键释放后清楚无效键标志
		case 0x0001:	 //	Power
		case 0x0002:	 //	Up
		case 0x0004:	 //	Down
		case 0x0008:	 //	WIFI
		case 0x0010:	 //	Zero
		case 0x0020:	 //	AItmp
		case 0x0040:	 //
		case 0x0080:	 //
		case 0x0100:	 //
		case 0x0200:	 //	
    	case 0x0400:	 //
		case 0x0800:	 // 
		case 0x0005:	 // Power+Down
		case 0x0006:	 // Up+Down
        case 0x0c: //mode+'-' 
		//case 0x0014:	 //	Zero+Down
		//case 0x0011:	 //	Zero+Power
		//case 0x1000:	
		//case 0x2000:	
		//case 0x4000:
		//case 0x8000:
	//TBD 
	// 有效键值
			if( gBLatchErrFg == 0 )
			{
				return val;
			}
			else
			{
				return 0xffff;
			}
		break;	
// 无效键值
		default:	
			gBLatchErrFg = 1;
			gBKeyValidFg = 0; 
			return 0xffff;
		break;	

	}	
}


unsigned int catchKey(void)
{
	unsigned int KeyRead;
	KeyRead =  Key_DataInput();


	if(gBKeyValidFg == 0)
	{	  
		//非法键1.8s复位	
		gWRestT = 0;
		if((++gWFailT)>=1800/TK_SCAN_PERIOD)  		
		{
			gWFailT = 0;
			reset_TK();
			KeyRead = 0;
		}
	}
	
	else if(KeyRead !=0 )
	{
		//长按36s复位
		gWFailT = 0;
			
		if((++gWRestT)>=36000/TK_SCAN_PERIOD)		//比亚迪为36S常按下 20180115
		{
			gWRestT = 0;
			reset_TK();	
			KeyRead = 0;
		}
	}
	else
	{
		//无键
		gWRestT = 0;
		gWFailT = 0;
	}
    //-----------------------------------------------------------
	return KeyRead;
}




