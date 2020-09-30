#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	//逻辑变量
protected:
	BYTE							m_cbPackCount;						//牌副数目
	BYTE							m_cbMainColor;						//主牌花色
	BYTE							m_cbMainValue;						//主牌数值

	LONGLONG							m_lCellScore;

	//叫牌信息
protected:
	BYTE							m_cbCallCard;						//叫牌扑克
	BYTE							m_cbCallCount;						//叫牌数目
	WORD							m_wCallCardUser;					//叫牌用户

	//状态变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wLandScore;
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_bOverTimes;
	bool							m_bUserNoMain[GAME_PLAYER];

	//得分信息
protected:
	BYTE							m_cbScoreCardCount;					//分牌数目
	BYTE							m_cbScoreCardData[12*MAX_PACK];		//得分扑克

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
	BYTE							m_cbBackCard[8];					//底牌扑克
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];				//暗藏扑克

	//用户扑克
protected:
	BYTE							m_cbHandCardCount;					//扑克数目
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克

	//配置变量
protected:
	bool							m_bStustee;							//托管标志
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素

	//辅助变量
protected:
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[MAX_COUNT];	//派发扑克

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

	//取消消息
	//virtual VOID OnCancel();

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//用户叫牌
	bool OnSubCallCard(const void * pBuffer, WORD wDataSize);
	//发送底牌
	bool OnSubSendConceal(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGamePlay(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//甩牌结果
	bool OnSubThrowResult(const void * pBuffer, WORD wDataSize);
	//一轮结算
	bool OnSubTurnBalance(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户叫分
	bool OnSubLandScore(const void * pBuffer, WORD wDataSize);

	bool OnSubGMCard(const void * pBuffer, WORD wDataSize);
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

	void PlayLandScoreSound(IClientUserItem *pUserItem,WORD wLandScore);

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
	//LRESULT OnRuquestLeave(WPARAM wParam, LPARAM lParam);
	//上轮扑克
	LRESULT OnLastTurnCard(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);

	//叫分消息
	LRESULT OnLandScore(WPARAM wParam, LPARAM lParam);

	LRESULT OnGiveUpGame(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif