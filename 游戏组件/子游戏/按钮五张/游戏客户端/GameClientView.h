#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "JettonControl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//消息定义
#define IDM_START					WM_USER+100							//开始消息
#define IDM_FOLLOW					WM_USER+102							//跟注消息
#define IDM_GIVE_UP					WM_USER+103							//放弃消息
#define IDM_ADD_SCORE				WM_USER+104							//加注消息
#define IDM_SHOW_HAND				WM_USER+105							//梭哈消息
#define IDM_SEND_CARD_FINISH		WM_USER+106							//发牌完成

#define MYSELF_VIEW_ID				2									//自己位置

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//数据变量
protected:
	LONGLONG						m_lUserScore[GAME_PLAYER];			//
	LONGLONG						m_lTableScore[GAME_PLAYER];			//下注数目
	LONGLONG						m_lCellScore;						//单元注
	bool							m_bUserShowHand;					//

	//动画变量
protected:
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_SendCardCurPos;					//发牌当前位置
	INT								m_nStepCount;						//步数
	INT								m_nXStep;							//步长
	INT								m_nYStep;							//步长
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btFollow;							//跟注按钮
	CSkinButton						m_btShowHand;						//梭哈按钮
	CSkinButton						m_btAddTimes3;						//
	CSkinButton						m_btAddTimes2;						//
	CSkinButton						m_btAddTimes1;						//

	//位置信息
protected:
	CPoint							m_ptTableScore[GAME_PLAYER];		//当前下注信息

	//控件变量
public:
	CScoreView						m_ScoreView;						//成绩窗口
	CCardControl					m_CardControl[GAME_PLAYER];			//用户扑克
	CJettonControl					m_PlayerJeton[GAME_PLAYER+1];		//玩家筹码

	//界面变量
protected:
	CBitImage						m_ImageCard;						//扑克资源
	CBitImage						m_ImageViewBack;					//背景资源
	CBitImage						m_ImageViewFill;					//填充资源
	CPngImage						m_PngNumber;						//数字位图
	CPngImage						m_PngShowHand;						//

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//继承函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(int nWidth, int nHeight);
	//绘制接口
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//功能函数
public:
	//动画判断
	bool IsDispatchCard();
	//完成发牌
	void FinishDispatchCard();
	//发送扑克
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//设置下注
	void SetUserTableScore(WORD wChairID, LONGLONG lTableScore);
	//设置单元注
	void SetCellScore( LONGLONG lCellScore );
	//
	void SetUserShowHand( bool bShowHand );
	//
	void SetUserScore( WORD wChairId, LONGLONG lScore );

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//
	VOID OnLButtonUp(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
