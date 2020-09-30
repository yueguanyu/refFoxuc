#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID						24									//游戏 I D
#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("癞子斗地主")						//游戏名字

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//游戏状态
#define GS_WK_FREE					GAME_STATUS_FREE								//等待开始
#define GS_WK_SCORE					GAME_STATUS_PLAY							//叫分状态
#define GS_WK_PLAYING				GAME_STATUS_PLAY+1						//游戏进行
#define GS_WK_QIANG_LAND			GAME_STATUS_PLAY+2						//抢地主
#define GS_WK_ADD					GAME_STATUS_PLAY+3						//加倍

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_LAND_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
//
#define SUB_S_MING_CARD				106									//明牌
#define SUB_S_QIANG_LAND			107									//抢地主
#define SUB_S_MING_OUT				108									//出牌明牌
#define SUB_S_ADD					109									//加倍

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG						lBaseScore;							//基础积分

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
	WORD							wBombTime;							//倍数
	bool							bMingCardStatus[GAME_PLAYER];		//名牌状态

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusQiangLand
{
	LONGLONG						lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[3][20];					//手上扑克
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	WORD							wBombTime;							//倍数
	bool							bMingCard[GAME_PLAYER];				//明牌用户
	bool							bMingCardStatus[GAME_PLAYER];		//名牌状态

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};
//游戏状态
struct CMD_S_StatusAdd
{
	WORD							wLandUser;							//坑主玩家
	LONGLONG						lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[3][20];					//手上扑克
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	WORD							wBombTime;							//倍数
	bool							bMingCard[GAME_PLAYER];				//明牌用户
	bool							bAddScore[GAME_PLAYER];				//加倍判断
	BYTE							bLaiZiCard;							//赖子
	bool							bMingCardStatus[GAME_PLAYER];		//名牌状态

	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};
//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//坑主玩家
	WORD							wBombTime;							//倍数
	LONGLONG						lBaseScore;							//基础积分
	BYTE							bLandScore;							//地主分数
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[3][20];					//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[20];					//出牌列表
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bLaiZiCard;							//赖子牌
	bool							bMingCardStatus[GAME_PLAYER];		//名牌状态

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
	//
	BYTE							bMingCard[GAME_PLAYER];				//明牌信息

	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][20];			//扑克列表
	BYTE							bBackCardData[3];					//底牌扑克
	BYTE							bLaiZiCard;							//赖子牌
	WORD							wBombTime;							//倍数			
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
	bool							bAddScore[GAME_PLAYER];				//加倍判断
	bool							bIsAdd;								//是否加倍
	WORD				 			wLandUser;							//地主玩家
	BYTE							bLandScore;							//地主分数
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	WORD							wBombTime;							//倍数	
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[20];						//扑克列表
	BYTE							bChangeCard[20];					//变后牌
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG						lGameTax;							//游戏税收
	LONGLONG						lGameScore[3];						//游戏积分
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bCardData[54];						//扑克列表 
	BYTE							bUserAdd[GAME_PLAYER];				//用户加倍
	int								iOtherAdd[5];						//其他加倍
	WORD							wPlayTime[GAME_PLAYER];							//局数
	LONGLONG						lGameResult[GAME_PLAYER];						//成绩
};
//明牌
struct CMD_S_MingCard
{
	WORD							wMingUser;							//明牌用户
	BYTE							bMingCardStatus;							//明牌状态
	WORD							wBombTime;							//倍数
};
//抢地主
struct CMD_S_QiangLand
{
	BYTE							bQiangLand;							//抢地主
	WORD							wQiangUser;							//抢地主用户
	WORD							wBombTime;							//倍数 
};
//加倍
struct CMD_S_Add
{
	bool						bAdd;							//加倍
	bool						bIsAdd;							//加倍判断
	WORD						wAddUser;						//加倍用户
	WORD						wCurrentUser;					//当前用户
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息
//
#define SUB_C_MINGSTATE				5									//明牌开始
#define SUB_C_QIANGLAND				6									//抢地主
#define SUB_C_MINGOUT				7									//出牌明牌
#define SUB_C_ADD				8										//加倍

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
	BYTE							bCardData[20];						//扑克列表
	BYTE							bChangeCard[20];					//变后牌
};
//明牌开始
struct CMD_C_MingState
{
	WORD							wUserChairID;						//玩家椅子
	BYTE							bMingState;							//明牌状态
};
//抢地主
struct CMD_C_QiangLand
{
	bool							bQiangLand;						//抢地主
};
//出牌明牌
struct CMD_C_MingOut
{
	bool							bMingOut;						//出牌明牌
};
//加倍
struct CMD_C_ADD
{
	bool							bAdd;						//加倍
};
//////////////////////////////////////////////////////////////////////////

#endif
