#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//数组说明
//typedef CWHArray<CMD_Buffer,CMD_Buffer &> CSocketPacketArray;

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//加注信息
protected:
	LONGLONG						m_lCellScore;							//单元下注
	LONGLONG						m_lTurnMaxScore;						//最大下注
	LONGLONG						m_lTurnLessScore;						//最小下注
	LONGLONG						m_lTableScore[GAME_PLAYER*2];			//下注数目
	LONGLONG						m_lShowHandScore;						//限制最高分
	LONGLONG						m_lUserScore[GAME_PLAYER];				//

	//状态变量
protected:
	bool							m_bShowHand;								//梭哈标志
	WORD							m_wCurrentUser;								//当前用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];				//游戏状态
	TCHAR							m_szAccounts[GAME_PLAYER][LEN_ACCOUNTS];	//玩家名字

	//控件变量
public:
	CGameClientView					m_GameClientView;						//游戏视图
	
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
	//时间消息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID);
	//删除定时器
	virtual bool OnEventGameClockKill(WORD wChairID);
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

public:
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound){return true;}

	//辅助函数
protected:
	//隐藏控制
	void HideScoreControl();
	//更新控制
	void UpdateScoreControl();

	//消息映射
protected:
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//放弃按钮
	LRESULT OnGiveUp(WPARAM wParam, LPARAM lParam);
	//跟注按钮
	LRESULT OnFollow(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//梭哈按钮
	LRESULT OnShowHand(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif
//////////////////////////////////////////////////////////////////////////
