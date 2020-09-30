#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	//加注信息
protected:
	LONG							m_lCellScore;						//游戏底注
	LONGLONG						m_lServiceCharge;					//服务费
	LONGLONG						m_lDrawMaxScore;					//最大下注
	LONGLONG						m_lTurnMaxScore;					//最大下注
	LONGLONG						m_lTurnLessScore;					//最小下注
	LONGLONG						m_lUserScore[GAME_PLAYER];			//用户下注
	LONGLONG						m_lTableScore[GAME_PLAYER];			//桌面下注

	//状态变量
protected:
	bool							m_bShowHand;						//梭哈标志
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];		//游戏状态
	TCHAR							m_szAccounts[GAME_PLAYER][32];		//玩家名字

	//辅助变量
protected:
	CMD_S_GameEnd					m_GameEndPacket;					//结束数据

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CHistoryScore					m_HistoryScore;						//历史积分
	CGameClientView					m_GameClientView;					//游戏视图

	//函数定义
public:
	//构造函数
	CGameClientEngine();
	//析构函数
	virtual ~CGameClientEngine();

	//控制接口
public:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置引擎
	virtual bool OnResetGameEngine();

	//事件接口
public:
	//旁观状态
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户积分
	virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(VOID * pData, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(VOID * pData, WORD wDataSize);
	//发送扑克
	bool OnSubSendCard(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(VOID * pData, WORD wDataSize);
	//获取信息
	bool OnSubGetWinner( VOID * pData, WORD wDataSize );

	//辅助函数
protected:
	//自动开始
	bool PerformAutoStart();
	//隐藏控制
	VOID HideScoreControl();
	//更新控制
	VOID UpdateScoreControl();
	//执行结束
	VOID PerformGameConclude();

	//消息映射
protected:
	//开始消息
	LRESULT OnMessageStart(WPARAM wParam, LPARAM lParam);
	//跟注消息
	LRESULT OnMessageFollow(WPARAM wParam, LPARAM lParam);
	//放弃消息
	LRESULT OnMessageGiveUp(WPARAM wParam, LPARAM lParam);
	//加注消息
	LRESULT OnMessageAddScore(WPARAM wParam, LPARAM lParam);
	//梭哈消息
	LRESULT OnMessageShowHand(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnMessageSendCardFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif