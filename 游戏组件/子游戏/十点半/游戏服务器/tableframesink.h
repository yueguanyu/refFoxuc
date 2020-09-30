#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	LONGLONG						m_lCellScore;						//底注
	WORD							m_wCurrentUser;						//当前玩家
	WORD							m_wBankerUser;						//庄家
	LONGLONG						m_lGameScore[GAME_PLAYER];			//得分

	//状态变量
	BYTE							m_byUserStatus[GAME_PLAYER];		//玩家状态
	bool							m_bAddScore[GAME_PLAYER];			//是否已加倍
	
	//下注变量
	LONGLONG						m_lTableScore[GAME_PLAYER];			//当前总下注

	//扑克变量
	BYTE							m_cbRepertoryCard[FULL_COUNT];		//库存扑克
	BYTE							m_bySendCardCount;					//发牌数目
	BYTE							m_byCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_byHandCardData[GAME_PLAYER][MAX_COUNT];	//桌面扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame						* m_pITableFrame;					//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性
	tagCustomRule *					m_pGameCustomRule;						//自定规则

	//属性变量
protected:
	static const WORD				m_wPlayerCount;						//游戏人数
	static const BYTE		m_GameStartMode;					//开始模式

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
	//初始化
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//信息接口
public:
	//开始模式
	virtual BYTE  GetGameStartMode();
	//游戏状态
	virtual bool  IsUserPlaying(WORD wChairID);

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};


	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	
	//游戏事件
protected:
	//玩家加倍
	bool OnSubUserAddScore( WORD wChariId );
	//玩家停牌
	bool OnSubUserGiveUp( WORD wChairId );
	//玩家要牌
	bool OnSubUserGiveCard( WORD wChairId );
	//玩家下注
	bool OnSubUserScore(WORD wChairId,LONGLONG lScore);

	//辅助函数
protected:
	//开始发牌
	VOID SendGamePlay();
};

//////////////////////////////////////////////////////////////////////////

#endif