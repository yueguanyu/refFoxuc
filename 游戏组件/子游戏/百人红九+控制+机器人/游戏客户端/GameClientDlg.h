#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

#include "DirectSound.h"
#include "afxtempl.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameEngine
{
	//友元声明
	friend class CGameClientView;

	//限制信息
protected:
	LONGLONG						m_lMeMaxScore;						//最大下注
	LONGLONG						m_lAreaLimitScore;					//区域限制
	LONGLONG						m_lApplyBankerCondition;			//申请条件

	//个人下注
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1];	//个人总注

	//庄家信息
protected:
	LONGLONG						m_lBankerScore;						//庄家积分
	WORD							m_wCurrentBanker;					//当前庄家
	BYTE							m_cbLeftCardCount;					//扑克数目
	bool							m_bEnableSysBanker;					//系统做庄

	//状态变量
protected:
	bool							m_bMeApplyBanker;					//申请标识

	//机器变量
protected:
	CList<tagAndroidBet,tagAndroidBet>		m_ListAndroid;				//机器人下注链表

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图

	//声音资源
protected:
	CDirectSound					m_DTSDBackground;					//背景音乐
	CDirectSound					m_DTSDCheer[3];						//背景音乐

	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置框架
	virtual bool OnResetGameEngine();
	//游戏设置
	virtual void OnGameOptionSet();
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//旁观状态
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);
	virtual bool AllowBackGroundSound(bool bAllowSound);

	bool OnFrameMessage(WORD wSubCmdID,  void * pBuffer, WORD wDataSize);
	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart( void * pBuffer, WORD wDataSize);
	//游戏空闲
	bool OnSubGameFree( void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubPlaceJetton( void * pBuffer, WORD wDataSize,bool bGameMes);
	//游戏结束
	bool OnSubGameEnd( void * pBuffer, WORD wDataSize);
	//申请做庄
	bool OnSubUserApplyBanker( void * pBuffer, WORD wDataSize);
	//取消做庄
	bool OnSubUserCancelBanker( void * pBuffer, WORD wDataSize);
	//切换庄家
	bool OnSubChangeBanker( void * pBuffer, WORD wDataSize);
	//游戏记录
	bool OnSubGameRecord( void * pBuffer, WORD wDataSize);
	//下注失败
	bool OnSubPlaceJettonFail( void * pBuffer, WORD wDataSize);
	//玩家作弊
	bool OnSubCheat( void * pBuffer, WORD wDataSize);
	//申请结果
	bool OnSubReqResult(const void * pBuffer, WORD wDataSize);
	bool OnSubScoreResult(void * pBuffer, WORD wDataSize);
	bool OnSubAccountResult(void * pBuffer, WORD wDataSize);
	//赋值函数
protected:
	//设置庄家
	void SetBankerInfo(WORD wBanker,LONGLONG lScore);
	//个人下注
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);

	//功能函数
protected:
	//更新控制
	void UpdateButtonContron();
	//跟新银行控件
	void ReSetGameCtr();

	//消息映射
protected:
	//加注消息
	LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
	//申请消息
	LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
	//继续发牌
	LRESULT OnContinueCard(WPARAM wParam, LPARAM lParam);
	//手工搓牌
	LRESULT  OnOpenCard(WPARAM wParam, LPARAM lParam);
	//自动搓牌
	LRESULT  OnAutoOpenCard(WPARAM wParam, LPARAM lParam);
	//管理员请求
	LRESULT OnAdminCommand(WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetAccount(WPARAM wParam, LPARAM lParam);
	LRESULT	 OnCheckAccount(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
