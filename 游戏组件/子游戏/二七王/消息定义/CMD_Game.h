#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						165									//游戏 I D
#define GAME_NAME					TEXT("二七王游戏")					//游戏名字

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

//游戏状态
#define GS_UG_FREE					GAME_STATUS_FREE								//空闲状态
#define GS_UG_SCORE					(GAME_STATUS_PLAY+5)						//叫分状态
#define GS_UG_CALL					(GAME_STATUS_PLAY+1)						//叫牌状态
#define GS_UG_BACK					(GAME_STATUS_PLAY+2)						//留底状态
#define GS_UG_PLAY					(GAME_STATUS_PLAY+3)						//游戏状态
#define GS_UG_WAIT					(GAME_STATUS_PLAY+4)						//等待状态


#define GER_GIVE_UP					GER_NORMAL+5
#define MAX_COUNT					60		

//////////////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_SEND_CARD				101									//发送扑克
#define SUB_S_CALL_CARD				102									//用户叫牌
#define SUB_S_SEND_CONCEAL			103									//底牌扑克
#define SUB_S_GAME_PLAY				104									//游戏开始
#define SUB_S_OUT_CARD				105									//用户出牌
#define SUB_S_THROW_RESULT			106									//甩牌结果
#define SUB_S_TURN_BALANCE			107									//一轮统计
#define SUB_S_GAME_END				108									//游戏结束
#define SUB_S_LAND_SCORE			109									//叫分命令

#define SUB_S_GM_CARD				110

//GM功能
struct CMD_S_GMCard
{
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];
	BYTE							cbCardCount[GAME_PLAYER];
};

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lBaseScore;							//基础积分
};
//叫分状态
struct CMD_S_StatusScore
{
	LONGLONG							lBaseScore;							//基础积分
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	WORD							bLandScore;							//上次叫分
	BYTE							bCurrentScore;						//当前叫分
	BYTE							cbBackCardData[8];					//底牌扑克
	BYTE							cbCardData[52];						//扑克列表
};
//叫牌状态
struct CMD_S_StatusCall
{
	LONGLONG							lBaseScore;							//基础积分
	//游戏变量
	WORD							wBankerUser;						//庄家用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值
	WORD							wLandScore;

	//发牌变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	//叫牌信息
	BYTE							cbComplete;							//完成标志
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
};

//留底状态
struct CMD_S_StatusBack
{
	LONGLONG							lBaseScore;							//基础积分
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
	WORD							wLandScore;

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[60];						//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONGLONG							lBaseScore;							//基础积分
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	WORD							wFirstOutUser;						//出牌用户

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
	WORD							wLandScore;

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	//出牌变量
	BYTE							cbOutCardCount[4];					//出牌数目
	BYTE							cbOutCardData[4][52];				//出牌列表

	//得分变量
	BYTE							cbScoreCardCount;					//扑克数目
	BYTE							cbScoreCardData[48];				//得分扑克

	int								BankerCatchRed5Score;				//抓红五分数
};

//等待状态
struct CMD_S_StatusWait
{
	LONGLONG							lBaseScore;							//基础积分
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家用户
	WORD							bLandScore;							//所叫分数
	WORD							wCurrentUser;						//当前用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表
	WORD							wCurrentUser;						//当前用户
	BYTE							cbBackCard[8];						//底牌扑克
};

//用户叫牌
struct CMD_S_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
};

//底牌扑克
struct CMD_S_SendConceal
{
	BYTE							cbMainColor;						//主牌数值
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
};

//游戏开始
struct CMD_S_GamePlay
{
	WORD							wCurrentUser;						//当前用户
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//扑克数目
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[52];						//扑克列表
	bool							bNoMainUser[GAME_PLAYER];
	BYTE							bDoubleCount[GAME_PLAYER];
	BYTE							bMainCount[GAME_PLAYER];
	bool							bLastTurn;
	bool							bFirstOut;
};

//甩牌结果
struct CMD_S_ThrowResult
{
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbThrowCardCount;					//扑克数目
	BYTE							cbResultCardCount;					//扑克数目
	BYTE							cbCardDataArray[104];				//扑克数组
	int								PlayerScore[4];
};

//一轮统计
struct CMD_TurnBalance
{
	WORD							wTurnWinner;						//一轮胜者
	WORD				 			wCurrentUser;						//当前玩家
	int								PlayerScore[4];
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lScore[4];							//用户得分
	LONGLONG							lKingScore[4];						//冲关得分
	int								wGameScore;							//游戏得分
	WORD							wConcealTime;						//扣底倍数
	WORD							wConcealScore;						//底牌积分
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
	bool							bAddConceal;						//是否庄扣
	BYTE							bEndStatus;							//结束情况
};
//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							bLandScore;							//上次叫分
	WORD							bCurrentScore;						//当前叫分
	bool							bGiveUpScore[4];
	WORD							wUserScore[4];
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_CALL_CARD				1									//用户叫牌
#define SUB_C_CALL_FINISH			2									//叫牌完成
#define SUB_C_CONCEAL_CARD			3									//底牌扑克
#define SUB_C_OUT_CARD				4									//用户出牌
#define SUB_C_LAND_SCORE			5									//用户叫分
#define SUB_C_EJECT					6									//超时强退
#define SUB_C_GIVEUP_GAME			7
#define SUB_C_GM_REQUEST			9

//用户叫分
struct CMD_C_LandScore
{
	BYTE							bLandScore;							//地主分数
};

//用户叫牌
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
};

//底牌扑克
struct CMD_C_ConcealCard
{
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[52];						//扑克列表
};


//////////////////////////////////////////////////////////////////////////

#endif