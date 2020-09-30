#ifndef CALL_SCORE_VIEW_HEAD_FILE
#define CALL_SCORE_VIEW_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "Resource.h"

#define IDC_CALL_SCORE			700								//放弃叫分
#define IDC_GIVE_UP				800								//放弃叫分

#define IDM_LAND_SCORE			(WM_USER+700)					//叫分消息

//////////////////////////////////////////////////////////////////////////


//积分视图类
class CCallScoreView : public CWnd
{
	//分数信息
protected:
	LONGLONG							m_lCurrentCallScore;
	CSkinButton						m_btGiveUp;
	CSkinButton						m_btScore[25];
	CGameFrameViewGDI *				m_pSinkWindow;						//回调窗口
	//资源变量
protected:
	CBitImage						m_ImageBack;						//背景图案

	//函数定义
public:
	//构造函数
	CCallScoreView();
	//析构函数
	virtual ~CCallScoreView();
	
	//功能函数
public:
	void SetCurrentCallScore(LONGLONG lScore);
	void SetSinkWindow(CGameFrameViewGDI * pSinkWindow){m_pSinkWindow = pSinkWindow;}
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif