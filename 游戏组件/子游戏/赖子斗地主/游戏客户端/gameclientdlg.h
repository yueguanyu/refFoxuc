#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "HistoryRecord.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//游戏变量
protected:
	WORD							m_wLandUser;						//挖坑用户
	WORD							m_wBombTime;						//倍数
	BYTE							m_bCardCount[3];					//扑克数目
	BYTE							m_bHandCardCount;					//扑克数目
	BYTE							m_bHandCardData[20];				//手上扑克
	bool							m_bAutoPlay ;						//托管变量
	//
	BYTE							m_bMingCard[GAME_PLAYER];			//明牌标识
	BYTE							m_bHandCardUser[3][20];				//玩家手上扑克
	BYTE							m_bLaiZiData;						//赖子

	//出牌变量
protected:
	DWORD							m_bTurnOutType;						//出牌类型
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[20];				//出牌列表
	BYTE							m_bChangeCard[20];					//变后牌
	BYTE							m_bChangeTurnOut[20];					//变后牌

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素
	bool							m_bAllowLookon;						//允许旁观

	//辅助变量
protected:
	WORD							m_wMostUser;						//最大玩家
	WORD							m_wCurrentUser;						//当前用户
	WORD							m_wTimeOutCount;					//超时次数
	BYTE							m_cbSortType;						//排序类型
	bool							m_bAddScore[GAME_PLAYER];						//加倍
	bool							m_bIsAdd;							//加倍状态
	BYTE							m_bLaiZiFrame;						//帧数

	//辅助变量
protected:
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[3][20];			//派发扑克

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	CHistoryScore					m_HistoryScore;						//历史积分

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
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//用户叫分
	bool OnSubLandScore(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//放弃出牌
	bool OnSubPassCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//加倍
	bool OnSubAdd(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//自动出牌
	bool AutomatismOutCard();
	//停止发牌
	bool DoncludeDispatchCard();
	//派发扑克
	bool DispatchUserCard(BYTE [GAME_PLAYER][20], BYTE cbCardCount);
	//赖子动画停止
	bool DoncludeFrame();

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//放弃出牌
	LRESULT OnPassCard(WPARAM wParam, LPARAM lParam);
	//叫分消息
	LRESULT OnLandScore(WPARAM wParam, LPARAM lParam);
	//出牌提示
	LRESULT OnAutoOutCard(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);
	//托管消息
	LRESULT OnAutoPlay(WPARAM wParam, LPARAM lParam);
	//排列扑克
	LRESULT OnMessageSortCard(WPARAM wParam, LPARAM lParam);
	//明牌开始
	LRESULT OnMingCardState(WPARAM wParam,LPARAM lParam);
	//明牌
	LRESULT OnMingCard(WPARAM wParam,LPARAM lParam);
	//抢地主
	LRESULT OnQiangLand(WPARAM wParam,LPARAM lParam);
	//出牌明牌
	LRESULT OnMingOut(WPARAM wParam,LPARAM lParam);
	//加倍
	LRESULT OnAdd(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
