#ifndef CMD_RUNFAST_HEAD_FILE
#define CMD_RUNFAST_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						15									//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("十点半")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_GOLD)					//游戏类型

#define MAX_COUNT					9									//最大数目
#define FULL_COUNT					54									//

#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//牌型定义
#define CT_ERROR					0									//错误牌型
#define CT_WU_XIAO					11									//五小
#define CT_REN_WU_XIAO				12									//人五小
#define CT_TIAN_WANG				13									//天王
#define CT_LIU_XIAO					14									//六小
#define CT_QI_XIAO					15									//七小
#define CT_BA_XIAO					16									//八小
#define CT_JIU_XIAO					17									//九小
#define CT_DA_TIAN_WANG				18									//大天王

//游戏状态
#define GS_TH_FREE					GAME_STATUS_FREE								//空闲状态
#define GS_TH_SCORE					GAME_STATUS_PLAY							//下注状态
#define GS_TH_PLAY					(GAME_STATUS_PLAY+1)						//游戏状态

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_ADD_SCORE				103									//玩家操作命令
#define SUB_S_SEND_CARD				104									//发牌命令
#define SUB_S_GIVE_UP				106									//停牌命令
#define SUB_S_USER_LEFT				105									//玩家逃跑
#define SUB_S_GAME_PLAY				107									//游戏开始

//空闲状态
struct CMD_S_StatusFree
{
	//单元积分
	LONGLONG						lCellScore;							//单元积分
};

//下注状态
struct CMD_S_StatusScore
{
	//单元积分
	LONGLONG						lCellScore;							//单元积分

	//状态变量
	WORD							wBankerUser;						//庄家
	BYTE							cbHadScore;							//已下注
	bool							bUserStatus[GAME_PLAYER];			//用户状态
};

//游戏状态
struct CMD_S_StatusPlay
{
	//单元积分
	LONGLONG						lCellScore;							//单元积分

	//状态变量
	WORD							wCurrentUser;						//当前玩家
	WORD							wBankerUser;						//庄家
	BYTE							byUserStatus[GAME_PLAYER];			//用户状态

	//下注变量
	LONGLONG						lTableScore[GAME_PLAYER];			//底注

	//扑克变量
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT];		//桌面扑克
};

//游戏开始
struct CMD_S_GameStart
{
	//单元积分
	LONGLONG						lCellScore;							//单元积分

	//状态变量
	WORD							wBankerUser;						//庄家
};

//游戏开始
struct CMD_S_GamePlay
{
	//状态变量
	WORD							wCurrentUser;						//当前玩家

	//扑克变量
	BYTE							byCardData[GAME_PLAYER];			//明牌

	//下注变量
	LONGLONG						lTableScore[GAME_PLAYER];			//底注
};

//玩家操作
struct CMD_S_AddScore
{
	WORD							wAddScoreUser;						//操作玩家
	WORD							wCurrentUser;						//当前玩家
	BYTE							cbCardData;							//玩家牌
};

//发送扑克
struct CMD_S_SendCard
{
	//游戏信息
	WORD							wCurrentUser;						//当前玩家
	WORD							wSendCardUser;						//发牌玩家
	BYTE							cbCardData;							//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							byCardData[GAME_PLAYER];			//底牌数据
	LONGLONG						lGameScore[GAME_PLAYER];			//玩家得分
};

//停牌
struct CMD_S_GiveUp
{
	WORD							wGiveUpUser;						//停牌玩家
	WORD							wCurrentUser;						//当前玩家
};

//玩家逃跑
struct CMD_S_UserLeft
{
	WORD							wLeftUser;							//逃跑玩家
	WORD							wCurrentUser;						//当前玩家
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_ADD_SCORE			1									//加倍
#define SUB_C_GIVE_UP			2									//停牌
#define SUB_C_GIVE_CARD			3									//要牌
#define SUB_C_SCORE				4									//下注

//////////////////////////////////////////////////////////////////////////

//下注
struct CMD_C_Score
{
	LONGLONG						lScore;								//下注数
};

//////////////////////////////////////////////////////////////////////////

#endif