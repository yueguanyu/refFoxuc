#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//结束信息
struct tagScoreViewInfo
{
	int								wGameScore;							//游戏得分
	WORD							wConcealTime;						//扣底倍数
	WORD							wConcealScore;						//底牌积分
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
	bool							bAddConceal;						//是否庄扣
	BYTE							bEndStatus;
};

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CWnd
{
	//结束信息
protected:
	int								m_wGameScore;						//游戏得分
	WORD							m_wLandUser;
	WORD							m_wLandScore;
	WORD							m_wConcealTime;						//扣底倍数
	WORD							m_wConcealScore;					//底牌积分
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];				//暗藏扑克
	bool							m_bAddConceal;
	BYTE							m_bEndStatus;

	//积分信息
protected:
	LONGLONG							m_lScore[GAME_PLAYER];				//游戏得分
	LONGLONG							m_lKingScore[GAME_PLAYER];			//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][LEN_NICKNAME];//用户名字

	//资源变量
protected:
	CSize							m_CardSize;							//扑克大小
	CBitImage						m_ImageBack;						//背景图案
	CBitImage						m_ImageCard;						//扑克位图
	CBitImage						m_ImageWinLose;						//胜负标志

	CSkinButton						m_btOkStart;
	CGameFrameViewGDI *				m_pSinkWindow;						//回调窗口
	//函数定义
public:
	//构造函数
	CScoreView();
	//析构函数
	virtual ~CScoreView();
	
	//功能函数
public:
	//设置信息
	void SetScoreViewInfo(tagScoreViewInfo & ScoreViewInfo);
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,LONGLONG lKingScore);
	//设置叫分
	void SetLandScore(WORD wLandUser,WORD wLandScore);
	void SetSinkWindow(CGameFrameViewGDI * pSinkWindow){m_pSinkWindow = pSinkWindow;}
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	BOOL OnCommand(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif