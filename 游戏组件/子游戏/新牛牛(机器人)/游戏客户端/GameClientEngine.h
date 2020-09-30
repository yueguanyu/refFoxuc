#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "HistoryRecord.h"

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<UDP_Buffer,UDP_Buffer &> CSocketPacketArray;

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
	//配置变量
protected:
	DWORD							m_dwCardHSpace;							//扑克象素
	WORD							m_wViewChairID[GAME_PLAYER];			//视图位置

	//加注信息
protected:
	LONGLONG						m_lTurnMaxScore;						//最大下注
	LONGLONG						m_lTableScore[GAME_PLAYER];				//下注数目

	//状态变量
protected:
	BYTE                            m_bDynamicJoin;                         //动态加入 
	WORD							m_wBankerUser;							//庄家用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	TCHAR							m_szAccounts[GAME_PLAYER][LEN_ACCOUNTS];	//玩家名字

	//用户扑克
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//用户数据
	BYTE							m_bUserOxCard[GAME_PLAYER];				//牛牛数据								

	//缓冲变量
protected:
	CSocketPacketArray				m_SocketPacketArray;					//数据缓存

	//组件变量
protected:
	CHistoryScore					m_HistoryScore;						//历史积分

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

	//控制接口
public:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置框架
	virtual bool OnResetGameEngine();

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	//事件接口
public:
	//旁观状态
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//网络消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏场景
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);

	//消息处理
protected:
	//用户叫庄
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//用户摊牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//控制
	bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//是否当前操作用户
	bool IsCurrentUser(WORD wCurrentUser){return (!IsLookonMode() && wCurrentUser==GetMeChairID());}
	//更新控制
	void UpdateScoreControl(LONGLONG lScore[],BOOL bShow);
	//转换信息
	void ChangeUserInfo(BYTE bCardData[],BYTE bCardCount,CString &CardInfo);

	//消息映射
protected:
	//提示按钮
	LRESULT	OnHintOx(WPARAM wParam, LPARAM lParam);
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//牛牛按钮
	LRESULT	OnOx(WPARAM wParam, LPARAM lParam);
	//快捷键按钮 
	LRESULT OnShortcut(WPARAM wParam, LPARAM lParam);
	//摊牌按钮
	LRESULT	OnOpenCard(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//显示点数
	LRESULT OnSetUserOxValue(WPARAM wParam, LPARAM lParam);
	//提示信息
	LRESULT OnShowInfo(WPARAM wParam, LPARAM lParam);
	//牛BT为真
	LRESULT OnOxEnable(WPARAM wParam, LPARAM lParam);
	//扑克分类
	LRESULT OnSortCard(WPARAM wParam, LPARAM lParam);
	//扑克分类
	LRESULT OnReSortCard(WPARAM wParam, LPARAM lParam);
	//做庄信息
	LRESULT OnBanker(WPARAM wParam, LPARAM lParam);
	//点击消息
	LRESULT OnClickCard(WPARAM wParam, LPARAM lParam);
	//语音消息
	LRESULT OnMessageYuYin(WPARAM wParam, LPARAM lParam);
	//控制
	LRESULT  OnAdminControl(WPARAM wParam, LPARAM lParam);
	//特殊控制
	LRESULT OnSpeCommdControl( WPARAM wParam, LPARAM lParam );

public:
	afx_msg void OnTimer(UINT nIDEvent);
	//插入消息
	void InsertMessageToDlg(CString strInfo);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
