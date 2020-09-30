#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义


#define KIND_ID							11									//游戏 I D
#define GAME_PLAYER						5									//游戏人数
#define GAME_NAME						TEXT("扯旋")						//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define MAX_COUNT						4									//最大数目

//结束原因						
#define GER_NO_PLAYER						0x10							//没有玩家

//游戏状态						
#define GS_TK_FREE						GAME_STATUS_FREE					//等待开始
#define GS_TK_INVEST					GAME_STATUS_PLAY					//下本钱注
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//下注状态
#define GS_TK_OPEN_CARD					GAME_STATUS_PLAY+2				    //游戏进行

#define VERSION_SERVER					PROCESS_VERSION(6,0,3)			    //程序版本
#define VERSION_CLIENT					PROCESS_VERSION(6,0,3)			    //程序版本

//结束原因						
#define END_REASON_EXIT					1									//强退结束
#define END_REASON_PASS					2									//让牌结束
#define END_REASON_NORMAL				3									//正常结束
#define END_REASON_GIVEUP				4									//放弃结束

//////////////////////////////////////////////////////////////////////////

#define JETTON_COUNT					8								//筹码个数
#define LAYER_HEIGHT					4								//层叠高度
#define JETTON_SPECE					3								//筹码间距
#define CONTROL_COUNT					4								//控件个数

//////////////////////////////////////////////////////////////////////////

//服务器命令结构
#define SUB_S_GAME_START				101									//游戏开始
#define SUB_S_USER_INVEST				107									//用户下本
#define SUB_S_ADD_SCORE					102									//加注结果
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_OPEN_START				115									//开始分牌
#define SUB_S_OPEN_CARD					105									//用户分牌
#define SUB_S_GIVE_UP					106									//用户放弃


#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusInvest
{
	WORD								wBankerUser;						//庄家用户

	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool								bInvestFinish[GAME_PLAYER];			//完成标志

	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
};

//游戏状态
struct CMD_S_StatusScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wBankerUser;						//庄家用户

	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态

	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG							lTotalScore[GAME_PLAYER];			//总注数目
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnMinScore;						//最小下注
};

//游戏状态
struct CMD_S_StatusOpenCard
{
	WORD								wBankerUser;						//庄家用户

	BYTE								cbOpenFinish[GAME_PLAYER];			//完成标志
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态

	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克

	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG							lTotalScore[GAME_PLAYER];			//总注数目
};

//////////////////////////////////////////////////////////////////////////

//游戏开始
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//庄家用户
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
};

//用户下本
struct CMD_S_UserInvest
{
	WORD								wChairID;							//用户玩家
	LONGLONG							lScore;								//用户筹码	
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGameStatus;						//游戏状态
	WORD								wGiveUpUser;						//放弃用户
	LONGLONG							lLostScore;							//输掉金币	
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//加注用户
	WORD								wCurrentUser;						//当前用户
	LONGLONG							lAddScoreCount;						//加注数目
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnMinScore;						//最小下注
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
	BYTE								cbEndReason;						//结束原因
};

//发牌数据包
struct CMD_S_SendCard
{
	BYTE								cbCardCount;						//扑克数目
	WORD								wCurrentUser;						//当前用户
	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克
	//bool								bStartOpen;							//开始分牌
};

//用户摊牌
struct CMD_S_Open_Card
{
	WORD								wChairID;							//摊牌用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_USER_INVEST				1									//用户下本
#define SUB_C_ADD_SCORE					2									//用户加注
#define SUB_C_OPEN_CARD					3									//用户摊牌
#define SUB_C_GIVE_UP					4									//用户放弃

//用户叫庄
struct CMD_C_UserInvest
{
	LONGLONG							lInvestGold;						//下本数目
};

//用户加注
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//加注数目
};

//用户开牌
struct CMD_C_OpenCard
{
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
};

//////////////////////////////////////////////////////////////////////////

#endif
