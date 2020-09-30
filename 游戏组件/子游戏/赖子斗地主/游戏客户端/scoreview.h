#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CDialog
{
	//变量定义
protected:
	LONGLONG						m_lGameTax;							//游戏税收
	LONGLONG						m_lGameScore[GAME_PLAYER];			//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][LEN_NICKNAME];//用户名字
	BYTE							m_bUserAdd[GAME_PLAYER];			//用户加倍
	int								m_iOtherAdd[5];		//其他加倍 0总倍，1明牌，2抢地主，3炸弹，4春天
	WORD							m_wPlayTime;			//局数
	LONGLONG						m_lGameResult;			//成绩

	//资源变量
protected:
	CBitImage						m_ImageBack;						//背景图案
	CBitImage						m_ImageWinLose;						//胜负标识
	CPngImage						m_ImageNumber;						//数字
	CPngImage						m_PngBack;							//背景
	CPngImage						m_PngWinLose;						//胜负标识
	CSkinResourceManager *          m_pSkinResourceManager;             //资源管理器

	//函数定义
public:
	//构造函数
	CScoreView(void);
	//析构函数
	virtual ~CScoreView(void);
	
	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel() { return; }

	//功能函数
public:
	//重置积分
	void ResetScore();
	//设置税收
	void SetGameTax(LONGLONG lGameTax);
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,BYTE *pUserAdd,int *iOtherAdd,WORD wPlayTime,LONGLONG lGameResult);
	//数字
	void DrawNumber(CDC *pDC,int x,int y,int bNum);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
