#ifndef CMD_LIARSDICE_HEAD_FILE
#define CMD_LIARSDICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							13									//游戏 I D
#define GAME_PLAYER						2									//游戏人数
#define GAME_NAME						TEXT("摇骰子")					    //游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define HAND_DICE_COUNT					5									//骰子数目

#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				    //程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				    //程序版本

//游戏状态
#define GS_WK_FREE				    	GAME_STATUS_FREE					//等待开始
#define GS_WK_SETCHIP					GAME_STATUS_PLAY+1					//玩家下注
#define GS_WK_THROWDICE			    	GAME_STATUS_PLAY+2					//玩家摇骰
#define GS_WK_PLAYING			    	GAME_STATUS_PLAY+3					//游戏进行
#define GS_WK_OPENDICE					GAME_STATUS_PLAY+4					//玩家开骰

//结构体定义

//用户喊话
struct	tagDiceYell
{
	BYTE	bDiceCount;		//骰子数目
	BYTE	bDicePoint;		//骰子点数
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_START_CHIP				100									//开始下注
#define SUB_S_CHIP_RESULT				101									//下注结果
#define SUB_S_SEND_DICE					102									//发送骰值
#define SUB_S_THROW_DICE				103									//玩家摇骰
#define SUB_S_YELL_DICE					104									//玩家喊话
#define SUB_S_YELL_RESULT				105									//喊话结果
#define SUB_S_OPEN_DICE					106									//玩家开骰
#define SUB_S_GAME_START				107									//游戏开始
#define SUB_S_GAME_END					108									//游戏结束
//////////////////////////////////////////////////////////////////////////

//游戏状态
struct CMD_S_StatusFree
{
	WORD							wLeftTimer;					
};


//开始下注
struct CMD_S_Chip_Start
{
	WORD			wUserID;							//玩家椅号
	LONGLONG		lMaxChip;							//最大额度
};

//下注结果
struct CMD_S_Chip_Result
{
	WORD			wUserID;							//玩家椅号
	LONGLONG		lChip;								//下注额度
};

//发送骰值
struct CMD_S_SendDice
{
	WORD			wCurUser;							//用户椅号
	BYTE			bDiceData[HAND_DICE_COUNT];			//用户骰值
};

//玩家摇骰
struct CMD_S_ThrowDice
{
	WORD			wThrowUser;							//用户椅号
};

//玩家喊话
struct CMD_S_YellDice
{
	WORD			wCurUser;							//当前用户
	WORD			wGameTurn;							//游戏轮数
	tagDiceYell		maxYell;							//最大喊话
};

//喊话结果
struct CMD_S_YellDice_Result
{
	WORD			wYellUser;							//喊话用户
	tagDiceYell		curYell;							//喊话数据				
};

//玩家开骰
struct CMD_S_OpenDice
{
	WORD			wOpenUser;							//开骰用户
	BYTE			bDiceData[2][HAND_DICE_COUNT];		//用户骰值
	BYTE			bDicePoint;							//被开点数
	int				nDiceOpenCount;						//被开数目
	int				nDiceRealCount;						//实际数目 
	bool			bOpenRight;							//是否开对	(即对手猜错数目)
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG		lGameTax;							//游戏税收
	LONGLONG		lGameScore[GAME_PLAYER];			//游戏积分
	BYTE			bEndMode;							//结束方式
};

//断线重连
//下注状态
struct CMD_S_StatusSetChip
{
	WORD			wChipUser;							//下注玩家
	LONGLONG		lMaxChip;							//最高下注
};

//摇骰状态
struct CMD_S_StatusThrow
{
	BYTE			bDiceData[HAND_DICE_COUNT];			//用户骰值
	LONGLONG		lChip;								//下注额度
	bool			bHaveThrow[GAME_PLAYER];			//是否摇骰
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD			wCurUser;							//当前玩家
	BYTE			bDiceData[HAND_DICE_COUNT];			//用户骰值
	LONGLONG		lChip;								//下注额度
	WORD			wGameTurn;							//游戏轮数
	tagDiceYell		userYell[GAME_PLAYER];				//用户喊话
};

//开骰状态
struct CMD_S_StatusOpen
{
	WORD			wOpenUser;							//开骰用户
	BYTE			bDiceData[2][HAND_DICE_COUNT];		//用户骰值
	LONGLONG		lChip;								//下注额度
	tagDiceYell		curYell;							//当前喊话
	int				nDiceRealCount;						//实际数目 
	bool			bOpenRight;							//是否开对	(即对手猜错数目)
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_THROWDICE					300									//摇骰开始
#define SUB_C_THROWFINISH				301									//摇骰完毕
#define SUB_C_CHIP						302									//下注命令
#define SUB_C_YELLDICE					303									//喊话命令
#define SUB_C_OPENDICE					304									//开骰命令
#define SUB_C_COMPLETEOPEN				305									//开骰完毕	(开骰信息显示完毕)

//玩家下注
struct CMD_C_Chip
{
	WORD			wUserID;							//玩家椅号
	LONGLONG		lChip;								//下注额度
};

//玩家喊话
struct CMD_C_YellDice
{
	WORD			wUserID;							//玩家椅号
	tagDiceYell		curYell;							//当前喊话
};

//开骰场景 (此场景非断线重连场景)
struct OpenDice_Scene
{
	WORD wOpenUser;		//开骰玩家
	BYTE bRealDiceCount;//实际数目
	BYTE bDicePoint;	//开的点数
	bool bOpenRight;	//是否开中
	BYTE bODTime;		//开骰时间
	void Init()
	{
		wOpenUser=0;
		bRealDiceCount = 0;
		bDicePoint = 0;
		bOpenRight = false;
		bODTime = 0;
	}
};

//////////////////////////////////////////////////////////////////////////

#endif
