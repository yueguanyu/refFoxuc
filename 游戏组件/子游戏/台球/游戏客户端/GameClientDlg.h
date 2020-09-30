#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

 
//游戏对话框
class CGameClientDlg : public CGameFrameEngine
{
	//游戏变量
public:
	WORD							m_wCurrentUser;						//当前玩家 
	BYTE                            m_bOutTimeTimes;                    //超时次数
	WORD                            m_serverType;                       //服务器类型
	
	bool							m_bInited;
  
	//控件变量
protected: 
	CGameClientView					m_GameClientView;					//游戏视图

	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置框架
	virtual bool OnResetGameEngine();
	//游戏设置
	virtual void OnGameOptionSet();
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时间消息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//旁观状态
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);
	//声音接口
public:
	//背景音乐
	virtual bool AllowBackGroundSound(bool bAllowSound){ return true; }
	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart( void * pBuffer, WORD wDataSize);
	// 游戏结束
	bool OnSubGameEnd( void *pBuffer,WORD wDataSize); 
	//设置母球
	bool OnSubSetBaseBall( void * pBuffer, WORD wDataSize);
	//设置结束
	bool OnSubEndSetBaseBall( void * pBuffer, WORD wDataSize);
	//设置球杆
	bool OnSubBallClubPos( void * pBuffer, WORD wDataSize);
	//击球
	bool OnSubHitBall( void * pBuffer, WORD wDataSize);
	//击球结果
	bool OnSubHitResult( void *pBuffer,WORD wDataSize);
 
	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
