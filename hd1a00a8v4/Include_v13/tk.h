#ifndef	_TK_H
#define	_TK_H

//===============================================================================================
//								中颖 SH79F326 触摸库 V1
//							-----------------------------
//	触摸按键子程序接口
//	
//===============================================================================================

//打***可以改

//封装:
#define	SH79F328_48PIN	0
#define	SH79F328_44PIN	1
#define	SH79F326_28PIN	2
#define	SH79F326_32PIN	3
#define	SH79F9260_32PIN	4

//#define	PACKAGE		SH79F326_32PIN			//***
#define	PACKAGE		SH79F328_48PIN


//////20,									//原始数据去抖 建议计算的去抖时间 320~400ms左右
//////5,									//基准去抖		建议去抖时间 80ms以上

//////2+1,								//单键去抖
//////2+2,								//所有键去抖  两个加起来 建议触摸键去抖时间80~ 100ms左右

 // 16ms  会影响去抖时间，需相应修改去抖次数，周期改短的，需把去抖次数加大，反之也是。
#define	TK_SCAN_PERIOD		(7)

//手直接摸
#define	PRODUCE_TK_ENABLE	0			//***0--支持手摸 1--不支持

//#define TEST_TK_DATA  1 /*打开串口调试*/

#ifdef TEST_TK_DATA
extern unsigned short tick_tk_send;
void UART_Txd_TKDEBUG(void);
#endif

#define	bit0	0x01
#define	bit1	0x02
#define	bit2	0x04
#define	bit3	0x08
#define	bit4	0x10
#define	bit5	0x20
#define	bit6	0x40
#define	bit7	0x80

#define GHG_T(n)		((((unsigned long)n)*3)/2)			//Timer4已经占用  12M 8分频	 n是 us 转 Timer4的计数值  (n*12)/8 => (n*3)/2 
															//n取10ms n*3=30000 小于0xffff
															//        n*12=120000 大于0xfff 所以(n*12)/8要改成(n*3)/2

//-------------------------------------------------------------------------------------
//								配置
//-------------------------------------------------------------------------------------
extern	unsigned int code Tab_TK_CODE[];

typedef	struct
{
	unsigned char	num;				  //按键个数   				1~16个
//	unsigned char	chn[16];			  //通道					0表示TK1....23表示TK24
//	unsigned char	shareFg[16];		  //通道是否与LED SEG复用	1--复用 0--单独
//	unsigned char	nearBy[16];			  //邻键	  				1表示右侧与其相邻  2表示左侧与其相邻 3表示左右都有键它处于中间
//	unsigned int	ripple[16];			  //原始纹波
//	unsigned int	sensitive[16];		  //灵敏度		
	
	unsigned char		chn[12];			  //通道					0表示TK1....23表示TK24
	unsigned char	shareFg[12];		  //通道是否与LED SEG复用	1--复用 0--单独
	unsigned char	nearBy[12];			  //邻键	  				1表示右侧与其相邻  2表示左侧与其相邻 3表示左右都有键它处于中间
	unsigned int	ripple[12];			  //原始纹波
	unsigned int	sensitive[12];		  //灵敏度		
	
	unsigned char	testChgTEn;			  //调试三角形时间
	unsigned int	high_mid_jmpT;		  //高转中
	unsigned int	mid_high_jmpT;		  //中转高
	unsigned int	mid_low_jmpT;		  //中转低
	unsigned int	low_mid_jmpT;		  //低转中
	unsigned char	highFreq;			  //高频率
	unsigned char	highDiv;			  //高放大倍数	
	unsigned char	midFreq;			  //中频率
	unsigned char	midDiv;			 	  //中放大倍数	
	unsigned char	lowFreq;			  //低频率
	unsigned char	lowDiv;				  //低放大倍数	
	unsigned char	inputFilter;		  //原始数据去抖
	unsigned char	baseFineTune;		  //基准去抖
	unsigned char	pressEntry;			  //单键去抖
	unsigned char	outputFilter;		  //所有键去抖
	unsigned char	shareCom;			  //跟随COM设置	
	unsigned char	shareSeg;			  //跟随SEG设置			
	unsigned char	tkvref;				  //触摸电压
		
	//自动重新适应环境补丁:	
	unsigned char	handDirectTouchEn;	  //是否支持手直接摸弹簧(填1支持,填0不支持)				
	unsigned char	patch1;				  //补丁1: 环境突变导致按键采集时间变化了，则自动重新适应(此补丁不支持手直接摸的情况)(填1有效,填0此补丁无效)
	unsigned int	patch1_Para;		  //       参数:采集时间跳变量
										  //	   加入原因: 把面板突然拿走，或者把面板重新盖上去
										  //
	unsigned char	patch2;				  //补丁2: 从无键到有键的去抖过程中,如果其他按键通道数据波动大，则重置去抖时间(填1有效,填0此补丁无效)
										  //	   加入原因: 受热，热风吹烧录口，烧录口同时又是触摸通道
										  //
	unsigned char	slideSlowMove;		  //补丁3: 滑条慢慢摸的时候，手指2侧未摸到的地方停止刷基准(填1有效,填0此补丁无效)
										  //	   加入原因: 快速滑动滑条没问题，超级缓慢滑动滑条有问题
										  //	   
	unsigned int	eftInject;			  //补丁4: 数据突然出现跳变超过此值,则自动重新适应(此补丁不支持手直接摸的情况,填65535此补丁无效)
										  //	   加入原因: 出现过读到的触摸按键数据值为0的情况
										  //		
	unsigned char	validNum;			  //补丁5: 有效列表组合数量, 按键值不属于支持表格内包含的情况,则自动重新适应(不填表格默认补丁无效)
										  //	   加入原因: 面板倒水导致多个按键同时判断为按下了
										  //		
	unsigned char	noiseErr;			  //补丁6: 在连续8采集中，通道出现N次数据异常,则自动重新适应(大于8补丁无效)		
	unsigned char	noiseErr_Para;		  //	   参数:采集数据突然小于基准值N个单位
										  //	   加入原因: EFT 4KV 数据偶尔出现比基准值小好多的情况,基准跟着往下调，干扰突然平稳了，数据恢复正常
										  //				 由于刚才基准调低了，导致误动作了
										  //	   注：不加此补丁4V改3V也可以改好
}Struct_TK_Para;


extern	 Struct_TK_Para	code	TK_Para;

void	UART_INIT(void);

//-------------------------------------------------------------------------------------
//								应用程序接口
//-------------------------------------------------------------------------------------
//接口
extern	void	InitKey(void);						//初始化
extern	void	stop_scan_tk(void);					//一个扫描周期开始的时候调用一次
extern	void	stop_scan_tk_1(void);				//开始扫描LED的时候调用一次(仅针对通道与LED复用的情况)
extern	void	SanTk(void); 						//在一个周期内至少调用N次 (N=触摸按键个数)

//结果
extern	unsigned int	getTKResult(void) ;	  		//返回按键值
extern	unsigned int	catchKey(void);				//返回按键值(带屏蔽无效键功能)

//手动重新适应环境补丁
extern	void	reset_TK(void);						//note1:(如发现按键按下后一直没有释放，可以调用一次)
													//note2:(如发现非法按键组合一直没有释放，可以调用一次)


extern	void ScanTKPress(void);


extern	unsigned int	getTKCxt(void) ;		  		//获得按键采样时间
extern	unsigned char	GetTKNum(void) ;				//获得按键总个数
extern  unsigned int	GetTKRaw(unsigned char idx) ;	//获得原始值eg: 	GetTKRaw(0) 对应K1		
														//           		GetTKRaw(5) 对应K6
extern	unsigned int	GetTKBase(unsigned char idx) ;	//获得基准值
extern	unsigned int	GetTKSample(unsigned char idx) ;//获得稳定值
extern	unsigned long	GetTKVersion(void);				//查询版本号


extern	unsigned char	gBKeyValidFg;
extern unsigned int	gWrCurKey;	 

#endif
