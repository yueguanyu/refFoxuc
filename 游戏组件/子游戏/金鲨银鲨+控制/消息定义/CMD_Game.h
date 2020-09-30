#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						127									//游戏 I D
#define GAME_NAME					TEXT("金鲨银鲨")					//游戏名字

//组件属性
#define GAME_PLAYER					MAX_CHAIR							//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//下注状态
#define GAME_SCENE_END				(GAME_STATUS_PLAY+1)				//开奖状态

//////////////////////////////////////////////////////////////////////////////////
//命令定义
#define GEM_FIRST							4							//第一关数值
#define GEM_SECOND							5							//第二关数值
#define GEM_THIRD							6							//第三关数值
#define GEM_MAX								7							//最大数值

//////////////////////////////////////////////////////////////////////////////////
//转盘索引
#define TURAN_TABLE_MAX						28							//转盘索引
#define TURAN_TABLE_JUMP					30							//转盘跳转

//动物索引
#define ANIMAL_LION							0							//狮子
#define ANIMAL_PANDA						1							//熊猫
#define ANIMAL_MONKEY						2							//猴子
#define ANIMAL_RABBIT						3							//兔子
#define ANIMAL_EAGLE						4							//老鹰
#define ANIMAL_PEACOCK						5							//孔雀
#define ANIMAL_PIGEON						6							//鸽子
#define ANIMAL_SWALLOW						7							//燕子
#define ANIMAL_SLIVER_SHARK					8							//银鲨
#define ANIMAL_GOLD_SHARK					9							//金鲨
#define ANIMAL_BIRD							10							//飞禽
#define ANIMAL_BEAST						11							//走兽
#define ANIMAL_MAX							12

//分类信息
#define ANIMAL_TYPE_NULL					0							//无
#define ANIMAL_TYPE_BEAST					1							//走兽
#define ANIMAL_TYPE_BIRD					2							//飞禽
#define ANIMAL_TYPE_GOLD					3							//黄金
#define ANIMAL_TYPE_SLIVER					4							//白银


//游戏记录
#define RECORD_COUNT_MAX					20							//最大数量

#define BULLET_MAX_COUNT                    6                           //兑换筹码最大数目

//////////////////////////////////////////////////////////////////////////////////

//控制消息
#define IDM_UPDATE_CHIP						(WM_USER+2000)				//更新筹码消息
#define IDM_EXCHANGE_CHIP					(WM_USER+200)				//兑换筹码消息

//下注按钮
#define IDM_PLAY_BET						(WM_USER+201)				//狮子

//控制按钮
#define IDM_OPERATION_RENEWAL				(WM_USER+213)				//续压
#define IDM_OPERATION_CANCEL				(WM_USER+214)				//取消
#define IDM_OPERATION_SWITCH				(WM_USER+215)				//切换
#define IDM_ADMIN_COMMDN WM_USER+1000

//////////////////////////////////////////////////////////////////////////////////
//音量
#define	MAX_VOLUME (3000)
#define SUB_C_AMDIN_COMMAND			4									//系统控制
//////////////////////////////////////////////////////////////////////////////////
//命令定义-服务器
#define SUB_S_AMDIN_COMMAND			109									//系统控制


//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA  	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			//接受
#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};
//控制区域信息
struct tagControlInfo
{
	INT  nAreaWin[ANIMAL_MAX];		//控制区域
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//游戏状态
struct CMD_S_StatusFree
{
	BYTE							cbTimeLeave;						//剩余时间

	LONG							lCellScore;							//底分
	LONGLONG						lPlayScore;							//玩家分数
	LONGLONG						lStorageStart;						//库存（彩池）

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制
	
	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
};

//游戏状态
struct CMD_S_StatusPlay
{
	BYTE							cbTimeLeave;						//剩余时间

	LONG							lCellScore;							//底分
	LONGLONG						lPlayScore;							//玩家分数
	LONGLONG						lPlayChip;							//玩家筹码
	LONGLONG						lStorageStart;						//库存（彩池）		

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lAllBet[ANIMAL_MAX];				//总下注
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家下注

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
};

//游戏状态
struct CMD_S_StatusEnd
{
	BYTE							cbTimeLeave;						//剩余时间

	LONG							lCellScore;							//底分
	LONGLONG						lPlayScore;							//玩家分数
	LONGLONG						lPlayChip;							//玩家筹码
	LONGLONG						lStorageStart;						//库存（彩池）

	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG						lPlayLimitScore;					//玩家限制

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lAllBet[ANIMAL_MAX];				//总下注
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家下注

	int								nTurnTableRecord[RECORD_COUNT_MAX];	//游戏记录
};


//////////////////////////////////////////////////////////////////////////////////
//服务器命令结构
enum
{
	SUB_S_GAME_FREE = 101,						//游戏空闲
	SUB_S_GAME_START,							//游戏开始
	SUB_S_GAME_END,								//游戏结束
	SUB_S_PLAY_BET,								//用户下注
	SUB_S_PLAY_BET_FAIL,						//用户下注失败
	SUB_S_BET_CLEAR,							//清除下注
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//剩余时间

	int								nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	LONGLONG						lStorageStart;						//库存（彩池）
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbTimeLeave;						//剩余时间
	BOOL							bTurnTwoTime;						//转2次
	int								nTurnTableTarget[2];				//转盘目标
	int								nPrizesMultiple;					//彩金	
	LONGLONG						lPlayWin[2];						//玩家输赢
	LONGLONG						lPlayPrizes;						//玩家彩金

	LONGLONG						lPlayShowPrizes;					//显示彩金
};

//用户下注
struct CMD_S_PlayBet
{
	WORD							wChairID;							//玩家位置
	LONGLONG                        mHBet;								//会员玩家下注
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
};

//用户下注失败
struct CMD_S_PlayBetFail
{
	WORD							wChairID;							//玩家位置
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
};

//清除下注
struct CMD_S_BetClear
{
	WORD							wChairID;							//玩家位置
	LONGLONG						lPlayBet[ANIMAL_MAX];				//玩家清除数量
};
//////////////////////////////////////////////////////////////////////////////////
//客户端命令结构
enum
{
	SUB_C_EXCHANGE_CHIP = 101,						//兑换筹码
	SUB_C_PLAY_BET,									//下注消息
	SUB_C_BET_CLEAR,								//清除下注
};

//兑换筹码
struct CMD_C_Chip
{
	LONGLONG						lChip;								//筹码数量
};

//下注消息
struct CMD_C_PlayBet
{
	int								nAnimalIndex;						//下注动物
	LONGLONG						lBetChip;							//筹码数量
};

//清除下注
struct CMD_C_BetClear
{
};
//机器人信息
struct tagRobotInfo
{
	int nChip[3];														//筹码定义
	int nAreaChance[ANIMAL_MAX];										//区域几率
	TCHAR szCfgFileName[MAX_PATH];										//配置文件
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		int nTmpChip[3] = {10,100,1000};
		int nTmpAreaChance[ANIMAL_MAX] = {11, 1, 11, 11, 4, 11, 8, 8, 8, 8, 8};
		TCHAR szTmpCfgFileName[MAX_PATH] = _T("SharkBattleConfig.ini");

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
	}
};

#endif