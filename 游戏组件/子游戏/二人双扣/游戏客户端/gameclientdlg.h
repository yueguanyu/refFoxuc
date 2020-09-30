#pragma once

#include "Stdafx.h"
#include "..\游戏服务器\GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	bool							m_bReqHumter;						//抄底申请
	//开始信息
protected:
	WORD							m_wGetRandCardID;					//拿牌用户
	BYTE							m_cbGetCardPosition;				//拿牌位置
	BYTE							m_cbRandCardData;					//随机扑克

	//游戏变量
protected:
	WORD							m_wCurrentUser;						//当前用户

	//胜利信息
protected:
	WORD							m_wWinOrder;						//胜利玩家

	//扑克变量
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目
	//出牌变量
protected:
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];		//出牌列表
	tagAnalyseResult				m_OutCardInfo;						//出牌类型
	tagAnalyseResult				m_LastOutCard;						//最后出牌

	//上轮扑克
protected:
	bool							m_bLastTurn;								//上轮标志
	BYTE							m_cbOutCardCount[GAME_PLAYER][2];			//出牌数目
	BYTE							m_cbOutCardData[GAME_PLAYER][2][MAX_COUNT];	//出牌列表

	//配置变量
protected:
	bool							m_bStustee;							//托管标志
	bool							m_bSortCount;						//数目排序	
	bool							m_bHaveVoiceCard;                   //声卡检测
	DWORD							m_dwCardHSpace;						//扑克象素

	//辅助变量
protected:
	WORD							m_wMostUser;						//最大玩家
	WORD							m_wTimeOutCount;					//超时次数
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[MAX_COUNT];	//派发扑克
	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	//CGlobalUnits *                  m_pGlobalUnits;                         //全局单元

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

	//定时器处理
protected:
	bool  OnTimerDispatchCard();
	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//放弃出牌
	bool OnSubPassCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户托管
	bool OnSubTrustee(const void * pBuffer,WORD wDataSize);
	
	//辅助函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//放弃判断
	bool VerdictPassCard();
	//自动出牌
	bool AutomatismOutCard();
	//当前扑克
	bool SwitchToCurrentCard();
	//停止发牌
	bool DoncludeDispatchCard();
	//炸弹声音
	void PlayBombSound(WORD wChairID, BYTE cbStarLevel);
	//派发扑克
	bool DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount);
	//抄底判定
	bool OnCanReqHunter(const BYTE cbCardData[],BYTE cbCardCount);
	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//牌型提示
	LRESULT OnCardType(WPARAM wParam,LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//放弃出牌
	LRESULT OnPassCard(WPARAM wParam, LPARAM lParam);
	//出牌提示
	LRESULT OnOutPrompt(WPARAM wParam, LPARAM lParam);
	//申请抄底
	LRESULT OnReqHunter(WPARAM wParam, LPARAM lParam);
	//数目排序
	LRESULT OnSortByCount(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);
	//上轮扑克
	LRESULT OnLastTurnCard(WPARAM wParam, LPARAM lParam);
	//音频创建失败
	//LRESULT OnVoiceCreateFail(WPARAM wParam,LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
