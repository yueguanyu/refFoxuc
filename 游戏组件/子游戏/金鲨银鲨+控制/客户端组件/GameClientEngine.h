#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	//游戏变量
protected:
	BOOL							m_lHasInChip;						//已经换筹码
	LONG							m_lCellScore;						//底分
	LONGLONG						m_lPlayScore;						//玩家积分
	LONGLONG						m_lPlayChip;						//玩家筹码
	int								m_nTurnTableRecord;					//游戏记录

	//下注变量
protected:
	int								m_nCurrentNote;						//当前注
	int								m_nUnitNote;						//单位注
	LONGLONG						m_lPlayBet[ANIMAL_MAX];				//玩家下注
	LONGLONG						m_lPlayAllBet[ANIMAL_MAX];			//所有玩家下注
	LONGLONG						m_lPlayLastBet[ANIMAL_MAX];			//上次压住
	LONGLONG                        m_hPlayAllBet[ANIMAL_MAX];           //会员玩家下注
	LONGLONG						m_lPlayWin;							//玩家赢的钱

	//设置变量
protected:
	LONGLONG						m_lAreaLimitScore;					//区域限制
	LONGLONG						m_lPlayLimitScore;					//玩家限制

	//组件变量
protected:
	CGameClientView					m_GameClientView;					//游戏视图

	//函数定义
public:
	//构造函数
	CGameClientEngine();
	//析构函数
	virtual ~CGameClientEngine();

	//系统事件
public:
	//创建函数
	virtual bool OnInitGameEngine();
	//重置函数
	virtual bool OnResetGameEngine();

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

	//设置事件
public:
	//设置事件
	virtual bool OnGameOptionChange();
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound);

	//消息处理
protected:
	//空闲时间
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//游戏结算
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//玩家下注
	bool OnSubPlayBet(const void * pBuffer, WORD wDataSize);
	//下注失败
	bool OnSubPlayBetFail(const void * pBuffer, WORD wDataSize);
	//清除下注
	bool OnSubBetClear(const void * pBuffer, WORD wDataSize);
	//控制
	bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);

	//系统消息
public:
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);

	//消息映射
protected:
	//更新筹码
	LRESULT OnMessageUpDateChip(WPARAM wParam, LPARAM lParam);
	//兑换筹码消息
	LRESULT OnMessageChip(WPARAM wParam, LPARAM lParam);
	//下注消息
	LRESULT OnMessagePlayBet(WPARAM wParam, LPARAM lParam);
	//续压消息
	LRESULT OnMessageOperationRenewal(WPARAM wParam, LPARAM lParam);
	//取消消息
	LRESULT OnMessageOperationCancel(WPARAM wParam, LPARAM lParam);
	//切换消息
	LRESULT OnMessageOperationSwitch(WPARAM wParam, LPARAM lParam);
	//控制
	LRESULT  OnAdminControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif