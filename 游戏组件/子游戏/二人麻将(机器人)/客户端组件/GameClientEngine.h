#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "MoveCardItem.h"
#include "CHistoryScore.h"

//////////////////////////////////////////////////////////////////////////////////
typedef CWHArray<CMoveCardItem *>		MoveCardItemArray;			//动画项目
//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{

	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	WORD							m_wReplaceUser;						//补花用户
	BYTE							m_cbActionMask;						//玩家动作
	BYTE							m_cbActionCard;						//动作牌
	BYTE							m_cbListenStatus;					//听牌状态,0指未听牌,1指只胡牌,2指只自摸,3指胡牌和自摸
	bool							m_bWillHearStatus;					//即将听牌
	LONG							m_lSiceCount;						//色子数值

	//辅助变量
	TCHAR							m_szAccounts[GAME_PLAYER][LEN_ACCOUNTS];	//玩家名称

	//动画变量
	MoveCardItemArray				m_MoveCardItemArray;				//扑克缓存

	//托管变量
protected:
	bool							m_bStustee;							//托管标志
	WORD							m_wTimeOutCount;					//超时次数

	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中扑克
	WORD							m_wHeapHead;						//牌堆头部
	WORD							m_wHeapTail;						//牌堆尾部
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];		//堆立信息
	BYTE							m_cbTailCount[GAME_PLAYER];				//堆牌尾数

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合扑克

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

	//系统事件
public:
	//创建函数
	virtual bool OnInitGameEngine();
	//重置函数
	virtual bool OnResetGameEngine();
	//游戏设置
//	virtual void OnGameOptionSet();

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
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户托管
	bool OnSubTrustee(const void * pBuffer,WORD wDataSize);
	//用户听牌
	bool OnSubListen(const void * pBuffer,WORD wDataSize);

	//辅助函数
protected:
	//播放声音
	void PlayCardSound(WORD wChairID,BYTE cbCardData);
	//播放声音
	void PlayActionSound(WORD wChairID,BYTE cbAction);
	//出牌判断
	bool VerdictOutCard(BYTE cbCardData);
	//设置扑克
	void SetHandCardControl(BYTE cbCardIndex[MAX_INDEX], BYTE cbAdvanceCard);
	//获取操作信息
	BYTE GetSelectCardInfo( WORD wOperateCode, tagSelectCardInfo SelectInfo[] );
	//扑克动画
	bool BeginMoveCard();
	//停止动画
	void StopMoveCard();
	//出牌动画
	bool BeginMoveOutCard( const CMoveCardItem *pMoveCardItem );
	//发牌动画
	bool BeginMoveSendCard( const CMoveCardItem *pMoveCardItem );
	//开局发牌动画
	bool BeginMoveStartCard( const CMoveCardItem *pMoveCardItem );
	//出牌动画完成
	bool OnMoveOutCardFinish( const COutCardItem *pOutCardItem );
	//发牌动画完成
	bool OnMoveSendCardFinish( const CSendCardItem *pSendCardItem );
	//开局动画完成
	bool OnMoveStartCardFinish( const CStartCardItem *pStartCardItem );

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//扑克操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//摇色子结束消息
	LRESULT OnSiceFinish(WPARAM wParam, LPARAM lParam);
	//玩家操作
	LRESULT OnUserAction( WPARAM wParam, LPARAM lParam );
	//动画完成消息
	LRESULT OnMoveCardFinish( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif