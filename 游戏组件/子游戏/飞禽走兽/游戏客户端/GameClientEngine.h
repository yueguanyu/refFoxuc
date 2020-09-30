#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//友元声明
	friend class CGameClientView;


	//限制信息
protected:
	LONGLONG						m_lMeMaxScore;						//最大下注
	LONGLONG						m_lAreaLimitScore;					//区域限制
	LONGLONG						m_lApplyBankerCondition;			//申请条件
	int                             m_GameEndTime;						//结束时间
	bool							m_blUsing;							//是否使用
	INT								m_nAnimalPercent[AREA_ALL];				//区域几率
	bool                            m_bPlaceEnd;						//下注结束
	//个人下注
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT];	    //个人总注
	LONGLONG                        m_lPlayerBetAll[AREA_COUNT];	    //全部总注

	//庄家信息
protected:
	LONGLONG						m_lBankerScore;						//庄家积分
	WORD							m_wCurrentBanker;					//当前庄家
	bool							m_bEnableSysBanker;					//系统做庄


	//状态变量
protected:
	bool							m_bMeApplyBanker;					//申请标识

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	CGlobalUnits *                  m_pGlobalUnits;                     //全局单元
	//控件变量
protected:
	CList<tagAndroidBet,tagAndroidBet>	m_ListAndroid;					//机器下注

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
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound);


	//用户事件
public:
	//用户进入
	virtual VOID  OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户离开
	virtual VOID  OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户状态
	virtual VOID  OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);


	//短语语音回调
public:
	//选择事件
	virtual VOID  OnEventPhraseSelect(INT nPhraseIndex);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//游戏空闲
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//申请做庄
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//取消做庄
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//切换庄家
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
	//游戏记录
	bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);
	//下注失败
	bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
	//扑克牌
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);

	/*bool OnSubCheckImageIndex(const void * pBuffer, WORD wDataSize);*/
	//控制
	bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);


	//赋值函数
protected:
	//设置庄家
	void SetBankerInfo(WORD wBanker,LONGLONG lScore);
	//个人下注
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);

	//功能函数
protected:
	void ReSetBankCtrol(int nGameState);
	//更新控制
	void UpdateButtonContron();	
	//自己最大下分
	LONGLONG GetMeMaxBet( BYTE cbArea );

	//消息映射
protected:
	//加注消息
	LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
	//申请消息
	LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
	
	LRESULT  OnPlaySound(WPARAM wParam, LPARAM lParam);
	//控制
	LRESULT  OnAdminControl(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
