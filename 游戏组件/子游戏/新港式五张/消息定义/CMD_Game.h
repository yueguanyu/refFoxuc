#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						25									//游戏 I D
#define GAME_NAME					TEXT("新版五张")						//游戏名字

//组件属性
#define GAME_PLAYER					5									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//////////////////////////////////////////////////////////////////////////

//结束原因
#define GER_NO_PLAYER				0x10								//没有玩家

//状态定义
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//游戏进行


//////////////////////////////////////////////////////////////////////////
//按钮标识 

//控制按钮
#define IDC_START						100								//开始按钮
#define IDC_ADD_SCORE_ONE				101
#define IDC_ADD_SCORE_TWO				102
#define IDC_ADD_SCORE_THREE				103
#define IDC_ADD_SCORE_FOUR				104
#define IDC_ADD_SCORE					105
#define IDC_CONTROL_FOLLOW				106
#define IDC_CONTROL_GIVEUP				107 
#define IDC_CONTROL_SHOWHAND			108	
#define IDC_CONTROL_ADD					109	
#define IDC_CONTROL_SUB     			110
//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_ADD_SCORE				101									//用户加注
#define SUB_S_GIVE_UP				102									//用户放弃
#define SUB_S_SEND_CARD				103									//发送扑克
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_GET_WINNER			105									//获取信息

//空闲状态
struct CMD_S_StatusFree
{
	//游戏属性
	LONG							lCellScore;							//基础积分

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏属性
	LONG							lCellScore;							//基础积分
	LONG							lServiceCharge;						//服务费

	//加注信息
	LONGLONG						lDrawMaxScore;						//最大下注
	LONGLONG						lTurnMaxScore;						//最大下注
	LONGLONG						lTurnLessScore;						//最小下注
	LONGLONG						lUserScore[GAME_PLAYER];			//用户下注
	LONGLONG						lTableScore[GAME_PLAYER];			//桌面下注

	//状态信息
	BYTE							cbShowHand;							//梭哈标志
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbPlayStatus[GAME_PLAYER];			//游戏状态

	//扑克信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbHandCardData[GAME_PLAYER][5];		//桌面扑克

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏开始
struct CMD_S_GameStart
{
	//游戏属性
	LONG							lCellScore;							//单元下注
	LONG							lServiceCharge;						//服务费

	//下注信息
	LONGLONG						lDrawMaxScore;						//最大下注
	LONGLONG						lTurnMaxScore;						//最大下注
	LONGLONG						lTurnLessScore;						//最小下注

	//用户信息
	WORD				 			wCurrentUser;						//当前玩家

	//扑克数据
	BYTE							cbObscureCard;						//底牌扑克
	BYTE							cbCardData[GAME_PLAYER];			//用户扑克
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD							wGiveUpUser;						//放弃用户
	WORD							wCurrentUser;						//当前用户
	LONGLONG						lDrawMaxScore;						//最大下注
	LONGLONG						lTrunMaxScore;						//最大下注
};

//用户下注
struct CMD_S_AddScore
{
	WORD							wCurrentUser;						//当前用户
	WORD							wAddScoreUser;						//加注用户
	LONGLONG						lTurnLessScore;						//最少加注
	LONGLONG						lUserScoreCount;					//加注数目
};

//发送扑克
struct CMD_S_SendCard
{
	//游戏信息
	WORD							wCurrentUser;						//当前用户
	WORD							wStartChairID;						//开始用户
	LONGLONG						lTurnMaxScore;						//最大下注

	//扑克信息
	BYTE							cbSendCardCount;					//发牌数目
	BYTE							cbCardData[2][GAME_PLAYER];			//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbCardData[GAME_PLAYER];			//用户扑克
	LONGLONG						lGameScore[GAME_PLAYER];			//游戏积分
};

//获取信息
struct CMD_S_GetWinner
{
	WORD								wOrderCount;					//玩家数
	WORD								wChairOrder[GAME_PLAYER];		//玩家名次
};

//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_GIVE_UP				1									//用户放弃
#define SUB_C_ADD_SCORE				2									//用户加注
#define SUB_C_GET_WINNER			3									//获取信息

//用户加注
struct CMD_C_AddScore
{
	LONGLONG						lScore;								//加注数目
};

//////////////////////////////////////////////////////////////////////////

#endif