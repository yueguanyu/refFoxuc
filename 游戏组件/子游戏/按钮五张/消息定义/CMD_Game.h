#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							1000								//游戏 I D
#define GAME_PLAYER						5									//游戏人数
#define GAME_NAME						TEXT("按钮五张")					//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

#define MAX_COUNT						5									//最大数目
#define VERSION_SERVER					PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(6,0,3)				//程序版本
//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_SCORE					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	//标志变量
	BYTE								bShowHand;							//梭哈标志

	//加注信息
	LONGLONG							lCellScore;							//单元下注
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnLessScore;						//最小下注
	LONGLONG							lShowHandScore;						//

	//状态信息
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	LONGLONG							lTableScore[GAME_PLAYER*2];			//下注数目

	//扑克信息
	BYTE								cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE								cbHandCardData[GAME_PLAYER][5];		//桌面扑克
};

//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	LONGLONG							lCellScore;							//单元下注
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnLessScore;						//最小下注
	LONGLONG							lShowHandScore;						//

	//用户信息
	WORD				 				wCurrentUser;						//当前玩家

	//扑克数据
	BYTE								cbObscureCard;						//底牌扑克
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	LONGLONG							lAddScoreCount;						//加注数目
	LONGLONG							lTurnLessScore;						//最少加注
	bool								bShowHand;							//是否梭哈
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
	LONGLONG							lTurnMaxScore;						//
};

//发牌数据包
struct CMD_S_SendCard
{
	WORD								wCurrentUser;						//当前用户
	WORD								wLastMostUser;						//上次最大用户
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnLessScore;						//最小下注
	BYTE								cbSendCardCount;					//发牌数目
	BYTE								cbCardData[GAME_PLAYER][2];			//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃跟注

//用户加注
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//加注数目
};

//////////////////////////////////////////////////////////////////////////

#endif
