#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						306									//游戏 I D
#define GAME_NAME					TEXT("二人麻将")					//游戏名字

//组件属性
#define GAME_PLAYER					2									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLAY				(GAME_STATUS_PLAY+1)				//游戏进行

#define GS_MJ_FREE					GAME_STATUS_FREE					//空闲状态
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+1)				//游戏状态


//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					16									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				64									//最大库存
#define MAX_HUA_CARD				0									//花牌个数

//扑克定义
#define HEAP_FULL_COUNT				32									//堆立全牌

#define MAX_RIGHT_COUNT				3									//最大权位DWORD个数			

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];						//组合数据
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_OPERATE_NOTIFY		104									//操作提示
#define SUB_S_OPERATE_RESULT		105									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TRUSTEE				107									//用户托管
#define SUB_S_LISTEN				103									//用户听牌
#define SUB_S_REPLACE_CARD			108									//用户补牌


//游戏状态
struct CMD_S_StatusFree
{
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];					//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];					//积分信息

	LONGLONG						lCellScore;									//基础金币
	WORD							wBankerUser;								//庄家用户
	bool							bTrustee[GAME_PLAYER];						//是否托管
};

//游戏状态
struct CMD_S_StatusPlay
{
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];					//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];					//积分信息

	//游戏变量
	LONGLONG						lCellScore;									//单元积分
	WORD							wBankerUser;								//庄家用户
	WORD							wCurrentUser;								//当前用户
	WORD							wReplaceUser;								//花牌替换用户
	BYTE							cbQuanFeng;									//圈风

	//状态变量
	BYTE							cbActionCard;								//动作扑克
	BYTE							cbActionMask;								//动作掩码
	BYTE							cbHearStatus[GAME_PLAYER];					//听牌状态
	BYTE							cbLeftCardCount;							//剩余数目
	bool							bTrustee[GAME_PLAYER];						//是否托管

	//出牌信息
	WORD							wOutCardUser;								//出牌用户
	BYTE							cbOutCardData;								//出牌扑克
	BYTE							cbDiscardCount[GAME_PLAYER];				//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//丢弃记录

	//扑克数据
	BYTE							cbCardCount;								//扑克数目
	BYTE							cbCardData[MAX_COUNT];						//扑克列表
	BYTE							cbSendCardData;								//发送扑克

	//组合扑克
	BYTE							cbWeaveCount[GAME_PLAYER];					//组合数目
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合扑克

	//堆立信息
	WORD							wHeapHead;									//堆立头部
	WORD							wHeapTail;									//堆立尾部
	BYTE							cbTailCount[GAME_PLAYER];					//尾部数目
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];				//堆牌信息
};

//游戏开始
struct CMD_S_GameStart
{
	LONG							lSiceCount;									//骰子点数
	WORD							wBankerUser;								//庄家用户
	WORD							wCurrentUser;								//当前用户
	WORD							wReplaceUser;								//补牌用户
	BYTE							cbUserAction;								//用户动作
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];			//扑克列表
	BYTE							cbQuanFeng;									//圈风
	WORD							wHeapHead;									//堆立牌头
	WORD							wHeapTail;									//堆立牌尾
	BYTE							cbTailCount[GAME_PLAYER];					//尾部数目
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];				//堆立信息
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	WORD							wReplaceUser;						//补牌用户
	WORD							wSendCardUser;						//发牌用户
	bool							bTail;								//末尾发牌
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard[3];					//操作扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	WORD							wExitUser;							//强退用户
	LONGLONG						lGameTax;							//游戏税收
	//结束信息
	WORD							wProvideUser;						//供应用户
	BYTE							cbProvideCard;						//供应扑克
	DWORD							dwChiHuKind[GAME_PLAYER];			//胡牌类型
	DWORD							dwChiHuRight[MAX_RIGHT_COUNT];		//胡牌类型
	BYTE							cbFanCount;							//总番数

	//积分信息
	LONGLONG						lGameScore[GAME_PLAYER];			//游戏积分

	//扑克信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克数据
};

//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};

//用户听牌
struct CMD_S_Listen
{
	WORD							wChairId;							//听牌用户
};

//补牌命令
struct CMD_S_ReplaceCard
{
	WORD							wReplaceUser;						//补牌用户
	BYTE							cbReplaceCard;						//补牌扑克
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			3									//操作扑克
#define SUB_C_TRUSTEE				4									//用户托管
#define SUB_C_LISTEN				2									//用户听牌
#define SUB_C_REPLACE_CARD			5									//用户补牌

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard[3];					//操作扑克
};

//用户听牌
struct CMD_C_Listen
{
	BYTE							cbListen;							//听牌用户
};

//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//补牌命令
struct CMD_C_ReplaceCard
{
	BYTE							cbCardData;							//扑克数据
};

//////////////////////////////////////////////////////////////////////////

#endif