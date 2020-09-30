#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "DirectSound.h"

//////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	//友元声明
	friend class CGameClientView;


	//游戏变量
protected:
	INT								m_nMultiple[AREA_ALL];				//区域倍数

	//限制变量
protected:
	LONGLONG						m_lAreaLimitScore;					//区域总限制
	LONGLONG						m_lUserLimitScore;					//个人区域限制

	//分数
protected:
	LONGLONG						m_lPlayerBet[AREA_ALL];				//玩家下注
	LONGLONG						m_lPlayerBetAll[AREA_ALL];			//所有下注

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
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound);

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//内部函数
private:
	//更新控件
	void UpdateControls();
	//自己最大下分
	LONGLONG GetMeMaxBet( BYTE cbArea = AREA_ALL );

	//消息映射
protected:
	//加注消息
	LRESULT OnPlayerBet(WPARAM wParam, LPARAM lParam);
	//控制
	LRESULT OnAdminControl(WPARAM wParam, LPARAM lParam);

	//消息映射
public:
	//下注开始
	bool OnSubBetStart(const void * pBuffer, WORD wDataSize);
	//下注结束
	bool OnSubBetEnd(const void * pBuffer, WORD wDataSize);
	//跑马开始
	bool OnSubHorsesStart(const void * pBuffer, WORD wDataSize);
	//跑马结束
	bool OnSubHorsesEnd(const void * pBuffer, WORD wDataSize);
	//玩家下注
	bool OnSubPlayerBet(const void * pBuffer, WORD wDataSize);
	//下注失败
	bool OnSubPlayerBetFail(const void * pBuffer, WORD wDataSize);
	//强制结束
	bool OnSubMandatoryEnd(const void * pBuffer, WORD wDataSize);
	//控制
	bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
