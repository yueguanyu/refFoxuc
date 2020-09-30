#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//游戏变量
protected:
	WORD							m_wBankerUser;							//庄家用户
	bool							m_bMingZhu[GAME_PLAYER];
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏玩家
	BYTE							m_cbPlayerCount;
	LONGLONG						m_lCurrentTimes;						//当前倍数
	LONGLONG						m_lTableScore[GAME_PLAYER];				//玩家下注
	LONGLONG						m_lMaxScore;							//封顶数目
	LONGLONG						m_lMaxCellScore;						//单元上限
	LONGLONG						m_lCellScore;							//单元下注
	LONGLONG						m_lUserMaxScore;						//最大分数

	//手上扑克
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克
	BYTE							m_cbAllUserCardData[GAME_PLAYER][MAX_COUNT];//所有扑克
	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//消息处理
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID,bool bExit=false);
	//看牌事件
	bool OnUserLookCard(WORD wChairID);
	//比牌事件
	bool OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID);
	//开牌事件
	bool OnUserOpenCard(WORD wUserID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareUser);
	
	//接受消息
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	bool OnSubAddScore(VOID * pData, WORD wDataSize);
	bool OnSubCompareCard(VOID * pData, WORD wDataSize);
	bool OnSubLookCard(VOID * pData, WORD wDataSize);

	bool OnSubGiveUp(VOID * pData, WORD wDataSize);
	bool OnSubPlayerExit(VOID * pData, WORD wDataSize);
	bool OnSubGameEnd(VOID * pData,WORD wDataSize);
	void UpdataControl(WORD ChairID);

};

//////////////////////////////////////////////////////////////////////////

#endif