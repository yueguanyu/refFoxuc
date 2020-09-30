#pragma once

#include "Stdafx.h"
#include "..\游戏服务器\GameLogic.h"
#include "GameClientView.h"
#include "DlgRequestLeave.h"
#include "DlgResponseLeave.h"

//////////////////////////////////////////////////////////////////////////

//离开请求数组
typedef	CWHArray<CDlgResponseLeave *> DlgResLeaveArray;

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{

	//友元声明
	friend class CGameClientView;

	//逻辑变量
protected:
	BYTE							m_cbPackCount;						//牌副数目
	BYTE							m_cbMainColor;						//主牌花色
	BYTE							m_cbMainValue;						//主牌数值

	//叫牌信息
protected:
	BYTE							m_cbCallCard;						//叫牌扑克
	BYTE							m_cbCallCount;						//叫牌数目
	WORD							m_wCallCardUser;					//叫牌用户

	//状态变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	bool							m_bBatchCard;						//批量出牌

	//得分信息
protected:
	BYTE							m_cbScoreCardCount;					//分牌数目
	BYTE							m_cbScoreCardData[12*PACK_COUNT];	//得分扑克

	//甩牌变量
protected:
	bool							m_bThrowCard;						//甩牌标志
	WORD							m_wThrowCardUser;					//甩牌用户
	BYTE							m_cbResultCardCount;				//还原扑克
	BYTE							m_cbResultCardData[MAX_COUNT];		//还原扑克

	//出牌变量
protected:
	bool							m_bLastTurn;						//上轮标志
	WORD							m_wFirstOutUser;					//出牌用户
	BYTE							m_cbOutCardCount[4][2];				//出牌数目
	BYTE							m_cbOutCardData[4][2][MAX_COUNT];	//出牌列表

	//底牌扑克
protected:
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//用户扑克
protected:
	BYTE							m_cbHandCardCount;					//扑克数目
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克

	//配置变量
protected:
	bool							m_bStustee;							//托管标志
	DWORD							m_dwCardHSpace;						//扑克象素
	WORD							m_wTimeOutCount;					//超时次数

	//辅助变量
protected:
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[MAX_COUNT];	//派发扑克

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	CDlgRequestLeave				m_DlgRequestLeave;					//请求离开
	DlgResLeaveArray				m_DlgResponseArray;					//用户请求数组
	
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
	//时间消息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID);

	//旁观状态
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pBuffer, WORD wDataSize);
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound){return true;}

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID * pBuffer, WORD wDataSize);
	//发送扑克
	bool OnSubSendCard(VOID * pBuffer, WORD wDataSize);
	//用户叫牌
	bool OnSubCallCard(VOID * pBuffer, WORD wDataSize);
	//发送底牌
	bool OnSubSendConceal(VOID * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGamePlay(VOID * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(VOID * pBuffer, WORD wDataSize);
	//甩牌结果
	bool OnSubThrowResult(VOID * pBuffer, WORD wDataSize);
	//一轮结算
	bool OnSubTurnBalance(VOID * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(VOID * pBuffer, WORD wDataSize);
	//用户托管
	bool OnSubStrustee(VOID * pBuffer, WORD wDataSize);
	//请求离开
	bool OnSubRequestLeave(VOID * pBuffer, WORD wDataSize);
	//批量出牌
	bool OnSubBatchCard(VOID * pBuffer, WORD wDataSize);

	//功能函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//自动出牌
	bool AutomatismOutCard();
	//当前扑克
	bool SwitchToCurrentCard();
	//甩牌结果
	bool SwitchToThrowResult();
	//更新叫牌
	bool UpdateCallCardControl();
	//派发扑克
	bool DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount);
	//设置扑克
	bool SetHandCardControl(BYTE cbCardData[], BYTE cbCardCount);
	//停止发牌
	void StopDispatchCard();

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//叫牌消息
	LRESULT OnCallCard(WPARAM wParam, LPARAM lParam);
	//出牌提示
	LRESULT OnOutPrompt(WPARAM wParam, LPARAM lParam);
	//留底消息
	LRESULT OnSendConceal(WPARAM wParam, LPARAM lParam);
	//查看底牌
	LRESULT OnConcealCard(WPARAM wParam, LPARAM lParam);
	//请求离开
	LRESULT OnRuquestLeave(WPARAM wParam, LPARAM lParam);
	//上轮扑克
	LRESULT OnLastTurnCard(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);
	//请求离开消息
	LRESULT OnMessageResponseLeave(WPARAM wParam,LPARAM lParam);
	//test
	LRESULT OnTest(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
