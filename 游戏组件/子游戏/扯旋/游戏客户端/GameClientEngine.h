#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//数组说明
//typedef CWHArray<CMD_Buffer,CMD_Buffer &> CSocketPacketArray;

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine: public CGameFrameEngine
{
	//游戏变量
protected:
	WORD							m_wViewChairID[GAME_PLAYER];			//视图位置
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//用户数据
	TCHAR							m_szAccounts[GAME_PLAYER][LEN_NICKNAME];	//玩家名字
	LONGLONG						m_lUserScore[GAME_PLAYER];				//用户金币
	LONGLONG						m_lCellScore;							//单元积分
	LONGLONG						m_lUserGetScore[GAME_PLAYER];			//用户得分
	LONGLONG						m_lUserTax[GAME_PLAYER];				//用户税收

	//加注信息
protected:
	LONGLONG						m_lTurnMaxScore;						//最大下注
	LONGLONG						m_lTurnMinScore;						//最小下注
	LONGLONG						m_lTableScore[GAME_PLAYER];				//下注数目
	LONGLONG						m_lTotalScore[GAME_PLAYER];				//总注数目

	//缓冲变量
protected:
	//CSocketPacketArray				m_SocketPacketArray;					//数据缓存

	//控件变量
public:
	CGameLogic						m_GameLogic;							//游戏逻辑
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
	//用户下本
	bool OnSubUserInvest(VOID * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(VOID * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubUserGiveUp(VOID * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(VOID * pBuffer, WORD wDataSize);
	//开始分牌
	bool OnSubStartOpen(VOID * pBuffer, WORD wDataSize);
	//用户开牌
	bool OnSubOpenCard(VOID * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(VOID * pBuffer, WORD wDataSize);

	//辅助函数
public:
	//获取牌型
	void GetHeapTailType(WORD wChairID,TCHAR szHeapString[],TCHAR szTailString[]);
	//获取牌型
	void GetCardType(BYTE cbCardData[],BYTE cbCardCount,TCHAR szResult[]);
	//获取名字
	void GetViewUserName(WORD wViewChairID,TCHAR szNametring[]);
	//视窗位置
	WORD GetViewChairID(WORD wChairID);
	//更新控制
	void UpdateScoreControl();
	//更新控制
	void UpdateInvestControl();

	//消息映射
protected:
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//清理按钮 
	LRESULT OnClearScore(WPARAM wParam, LPARAM lParam);
	//减注点数
	LRESULT OnDecreaseJetton(WPARAM wParam, LPARAM lParam);
	//开牌按钮
	LRESULT	OnOpenCard(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//开牌完成
	LRESULT OnOpenCardFinish(WPARAM wParam, LPARAM lParam);

public:
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
