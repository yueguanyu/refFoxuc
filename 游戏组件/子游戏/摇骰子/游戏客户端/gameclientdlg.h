#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图

	//配置变量
protected:
	BYTE							m_bSetChipTime;						//下注时间
	BYTE							m_bThrowDiceTime;					//摇骰时间
	BYTE							m_bYellDiceTime;					//喊话时间
	BYTE							m_bOpenInterval;					//开骰时间 (开骰动作里各个状态之间的间隔)

	//游戏变量
protected:
	BYTE							m_bDiceData[GAME_PLAYER][5];		//骰子数组
	WORD							m_wTurnCount;						//喊话轮数	
	LONGLONG						m_lChip;							//下注大小
	LONGLONG						m_lMaxChip;							//最大下注
	WORD							m_wCurUser;							//当前玩家
	tagDiceYell						m_UserYell[GAME_PLAYER];			//最后喊话
	UINT_PTR						m_nTimer;							//计时变量
	
	//函数定义
public:
	//构造函数
	CGameClientEngine();
	//析构函数
	virtual ~CGameClientEngine();

	//消息处理
protected:
	//开始下注
	bool OnSubStartChip(const void * pBuffer, WORD wDataSize);
	//下注结果
	bool OnSubChipResult(const void * pBuffer, WORD wDataSize);
	//骰子数值
	bool OnSubSendDice(const void * pBuffer, WORD wDataSize);
	//玩家摇骰
	bool OnSubThrowDice(const void * pBuffer, WORD wDataSize);
	//玩家喊话
	bool OnSubYellDice(const void * pBuffer, WORD wDataSize);
	//喊话结果
	bool OnSubYellResult(const void * pBuffer, WORD wDataSize);
	//玩家开骰
	bool OnSubOpenDice(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

    //辅助函数
protected:
	//设置喊话
	void SetYellPanel(BYTE bDiceCount);
	//播放声音
	void PlaySoundBySex(BYTE bDiceCount, BYTE bDicePoint, bool bBoy = true);

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
	virtual bool AllowBackGroundSound(bool bAllowSound){return true;}

	//消息函数
protected:
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//压注消息
	LRESULT OnSetChip(WPARAM wParam, LPARAM lParam);
	//摇骰消息
	LRESULT OnThrowDice(WPARAM wParam, LPARAM lParam);
	//摇骰结束

	//看骰消息
	LRESULT OnLookDice(WPARAM wParam, LPARAM lParam);
	//骰数消息
	LRESULT OnScroll(WPARAM wParam, LPARAM lParam);
	//骰点消息
	LRESULT OnDiceNum(WPARAM wParam, LPARAM lParam);
	//喊话消息
	LRESULT OnYellOk(WPARAM wParam, LPARAM lParam);
	//开骰消息
	LRESULT OnOpenDice(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
