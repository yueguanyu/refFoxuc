#ifndef CMD_PAIJIU_HEAD_FILE
#define CMD_PAIJIU_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID							226									//游戏 I D
#define GAME_NAME						TEXT("干瞪眼")						//游戏名字
#define GAME_PLAYER						6									//游戏人数
#define MAX_COUNT						27									//最大数目

#define VERSION_SERVER			    	PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				    PROCESS_VERSION(6,0,3)				//程序版本

#define HAND_CARD_COUNT					2									//手牌数目

//游戏状态
#define GS_WK_FREE				    	GAME_STATUS_FREE					//空闲状态
#define GS_WK_SCORE						GAME_STATUS_PLAY					//游戏状态
#define GS_WK_PLAYING					GAME_STATUS_PLAY+1					//等待状态
/////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_LAND_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lBaseScore;							//基础积分

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusScore
{
	BYTE							bLandScore;							//地主分数
	LONGLONG						lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[3][20];					//手上扑克
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//坑主玩家
	WORD							wBombTime;							//炸弹倍数
	LONGLONG						lBaseScore;							//基础积分
	BYTE							bLandScore;							//地主分数
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[MAX_COUNT];			//出牌列表
	BYTE							bMagicData[MAX_COUNT];
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bLeftCardCount;

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[17];						//扑克列表
};

//发送扑克
struct CMD_S_SendAllCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][MAX_COUNT];			//扑克列表
	BYTE							bBackCardData[3];					//底牌扑克
};

//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bLandScore;							//上次叫分
	BYTE							bCurrentScore;						//当前叫分
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wLandUser;							//地主玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[MAX_COUNT];
	BYTE							bCardCount;
	BYTE							bLeftCardCount;
	BYTE							bUserCardCount[GAME_PLAYER];
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bUserCardCount[GAME_PLAYER];
	BYTE							bMagicData[MAX_COUNT];
	BYTE							bCardData[MAX_COUNT];				//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	bool							bCanOutSingleKing;
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bUserCardCount[GAME_PLAYER];
	BYTE							bCardData[GAME_PLAYER][MAX_COUNT];
	BYTE							bLeftCardCount;
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax;							//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏积分
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bCardData[162];						//扑克列表 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息

//托管结构
struct CMD_C_UserTrustee {
	WORD							wUserChairID;						//玩家椅子
	bool							bTrustee;							//托管标识
};

//用户叫分
struct CMD_C_LandScore
{
	BYTE							bLandScore;							//地主分数
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[MAX_COUNT];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif
