#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//辅助变量
protected:
	DWORD							m_dwPlayTimeCount;					//游戏时间

	//离开请求
protected:
	BYTE							m_cbResponses[GAME_PLAYER];			//回应状态
	WORD							m_wRequestTimes[GAME_PLAYER];		//请求离开

	//游戏变量
protected:
	WORD							m_wHeadUser;						//首出用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_cbContinue[GAME_PLAYER];			//继续标志
	WORD							m_wPersistInfo[GAME_PLAYER][2];		//游戏信息
	bool							m_bAutoPlayer[4];	
	LONGLONG						m_TurnScore;
	LONGLONG						m_PlayerScore[4];
	BYTE							m_510KCardRecord[3][8];				//510K计数
	bool							m_bFirstUserLastTurn;				//头名玩家最后一轮。。。过手分...囧rz

	//游戏等级
protected:
	WORD							m_wOrderIndex;						//等级索引
	BYTE							m_cbMainValue;						//主牌数值
	BYTE							m_cbValueOrder[GAME_PLAYER];		//等级数值

	//胜利信息
protected:
	WORD							m_wWinCount;						//胜利人数
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表

	//出牌信息
protected:
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_cbTurnCardType;					//扑克类型
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];		//出牌数据
	BYTE							m_cbMagicCardData[MAX_COUNT];		//变幻扑克

	//扑克信息
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
	//ITableFrameControl *			m_pITableFrameControl;
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;						//游戏人数

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};
	//查询接口
public:
	//查询限额
	virtual LONGLONG QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual LONGLONG QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID,VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID,VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//请求事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; };
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);
	//继续游戏
	bool OnUserContinueGame(WORD wChairID);

protected:
	//是否都有分
	bool IsAllUserHaveScore(BYTE bCardData[GAME_PLAYER][MAX_COUNT]);
	//设置对家
	void SearchFriend(BYTE bFirstCard,const BYTE bHandCardData[4][28]);
	//转换文字
	CString TranslateCardText(BYTE bCardData);
};

//////////////////////////////////////////////////////////////////////////////////

#endif