#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//游戏变量
protected:
	LONGLONG						m_lCellScore;						//单元积分
	LONGLONG						m_lTableScore[GAME_PLAYER];			//桌面分
	bool							m_bAddScore;						//已加倍
	bool							m_bGameStart;						//游戏开始

	//玩家变量
	WORD							m_wCurrentUser;						//当前玩家
	WORD							m_wBankerUser;						//庄家
	BYTE							m_byUserStatus[GAME_PLAYER];		//玩家状态

	//辅助变量
	TCHAR							m_szAccounts[GAME_PLAYER][32];		//玩家名字

	BYTE							m_bySendCardCount;							//发牌数目
	WORD							m_wStartChairId;							//发牌起始位置
	BYTE							m_bySendCardData[GAME_PLAYER][MAX_COUNT];	//发牌数据

	CMD_S_GameStart					m_GameStart;						//游戏开始消息
	CMD_S_GameEnd					m_GameEnd;							//游戏结束消息

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	
	//函数定义
public:
	//构造函数
	CGameClientEngine();
	//析构函数
	virtual ~CGameClientEngine();

	//常规继承
private:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置框架
	virtual bool OnResetGameEngine();
	//游戏设置
	virtual void OnGameOptionSet();

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pData, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard( const void *pData, WORD wDataSize );
	//玩家加倍
	bool OnSubUserAddScore( const void *pData, WORD wDataSize );
	//玩家停牌
	bool OnSubUserStopCard( const void *pData, WORD wDataSize );
	//玩家逃跑
	bool OnSubUserLeft( const void *pData, WORD wDataSize );
	//游戏开始
	bool OnSubGamePlay( const void *pData, WORD wDataSize );

	//消息映射
protected:
	//开始消息
	LRESULT OnStart( WPARAM wParam, LPARAM lParam );
	//下注消息
	LRESULT OnAddScore( WPARAM wParam, LPARAM lParam );
	//跟注消息
	LRESULT OnGiveCard( WPARAM wParam, LPARAM lParam );
	//放弃消息
	LRESULT OnStopCard( WPARAM wParam, LPARAM lParam );
	//下注消息
	LRESULT OnUserScore( WPARAM wParam, LPARAM lParam );

	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	//辅助函数
protected:
	//隐藏控制
	VOID HideScoreControl();
	//更新控制
	VOID UpdateScoreControl();
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//筹码移动完成
	LRESULT OnMoveJetFinish(WPARAM wParam, LPARAM lParam);
	//播放操作声音
	VOID PlayActionSound( WORD wChairId, BYTE byAction );
	//执行结束
	VOID PerformGameConclude();
	//执行开始
	VOID PerformGameStart();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
