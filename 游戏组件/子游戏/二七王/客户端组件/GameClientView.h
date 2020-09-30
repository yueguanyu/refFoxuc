#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CallCardWnd.h"
#include "CardControl.h"
#include "ConcealCardView.h"
#include "CallScoreWnd.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//按钮消息
#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_OUT_PROMPT				(WM_USER+102)						//提示消息
#define IDM_SEND_CONCEAL			(WM_USER+103)						//留底消息

//功能按钮
#define IDM_CONCEAL_CARD			(WM_USER+110)						//查看底牌
#define IDM_REQUEST_LEAVE			(WM_USER+111)						//请求离开
#define IDM_LAST_TURN_CARD			(WM_USER+112)						//上轮扑克
#define IDM_TRUSTEE_CONTROL			(WM_USER+113)						//托管控制
#define IDM_GIVEUP_GAME				(WM_USER+114)						//托管控制

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//显示变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	bool							m_bWaitConceal;						//等待标志
	bool							m_bLastTurnCard;					//上轮扑克
	bool							m_bUserNoMain[GAME_PLAYER];			//无主用户
	BYTE							m_bMainCount[GAME_PLAYER];	
	BYTE							m_bDoubleCount[GAME_PLAYER];	
	LONGLONG							m_lWidth,m_lHeight;

	//游戏变量
protected:
	int								m_wPlayerScore[GAME_PLAYER];		//各自得分
	bool							m_bGiveUpPlayer[GAME_PLAYER];
	WORD							m_wCardScore;						//玩家得分
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_bScore[GAME_PLAYER];				//用户叫分
	WORD							m_wCurrentCallScore;				//当前叫分
	WORD							m_wUserCallScore;					//用户叫分

	//叫牌信息
protected:
	BYTE							m_cbCallColor;						//叫牌花色
	BYTE							m_cbCallCount;						//叫牌数目
	BYTE							m_cbMainValue;						//主牌数值
	WORD							m_wCallCardUser;					//叫牌用户
	BYTE							m_cbValueOrder[2];					//主牌数值

	//位置信息
protected:
	CSize							m_SizeBanker;						//庄家大小
	CSize							m_SizeStation;						//状态大小
	CPoint							m_ptScore[GAME_PLAYER];				//叫分位置
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptAuto[GAME_PLAYER];				//托管位置
	CPoint							m_ptNoMain[GAME_PLAYER];			//无主位置

	//位图变量
protected:
	CBitImageEx						m_ImageBack;						//背景资源
	CBitImageEx						m_ImageCenter;
	CBitImageEx						m_ImageBanker;						//庄家资源
	CBitImageEx						m_ImageLastTurn;					//上轮标志
	CBitImageEx						m_ImageColorFlag;					//花色标志
	CBitImageEx						m_ImageWaitConceal;					//等待资源
	CBitImageEx						m_ImageStationTitle;				//状态资源

	CBitImageEx						m_ImageNoMain;						//无主资源
	CPngImageEx						m_ImageNoCall;
	CPngImageEx						m_ImageNumber;						//数字资源
	CBitImageEx						m_ImageScoreCard;					//得分牌

	CPngImageEx						m_ImageMain1;
	CPngImageEx						m_ImageMain2;

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btOutPrompt;						//提示按钮
	CSkinButton						m_btSendConceal;					//留底按钮
	CSkinButton						m_btGiveUp;

	//功能按钮
public:
	CSkinButton						m_btConcealCard;					//查看底牌
	//CSkinButton						m_btRuquestLeave;					//请求离开
	CSkinButton						m_btLastTurnCard;					//上轮扑克
	CSkinButton						m_btStusteeControl;					//拖管控制

	//扑克控件
public:
	CCardControl					m_CardScore;						//得分扑克
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[4];				//扑克视图
	CCardControl					m_BackCardControl;					//底牌扑克


	CCardControl					m_GmCardControl[4];

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CCallScoreView					m_CallScoreView;					
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CCallCardWnd					m_CallCardWnd;						//叫牌窗口
	CConcealCardView				m_ConcealCardView;					//底牌窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }
	//重载函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//游戏信息
public:
	//设置得分
	void SetCardScore(WORD wCardScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//设置等待
	void SetWaitConceal(bool bWaitConceal);
	//设置上轮
	void SetLastTurnCard(bool bLastTurnCard);
	//设置主牌
	void SetValueOrder(BYTE cbMainValue, BYTE cbValueMySelf, BYTE cbValueOther);
	//设置亮主
	void SetCallCardInfo(WORD wCallCardUser, BYTE cbCallColor, BYTE cbCallCount);
	//挖坑分数
	void SetLandScore(WORD wChairID, WORD bLandScore);
	//设置叫分
	void SetCurrentCallScore(WORD wCurrentCallScore);
	//玩家分数
	void SetPlayerScore(int wPlayerScore[4]);
	//放弃叫分
	void SetGiveUpPlayer(bool bGiveUpPlayer[4]);
	void SetUserScore(WORD wUserScore[4]);
	//设置无主
	void SetNoMainUser(bool bNoMainUser[4]);

	void SetGMCard();
	//设置数量
	void SetMainCount(BYTE bMainCount[4],BYTE bDoubleCount[4]);

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos,int nWidth, int nWeight,UINT nFormat=DT_LEFT);

	//内部函数
private:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);

	//消息映射
protected:

	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif