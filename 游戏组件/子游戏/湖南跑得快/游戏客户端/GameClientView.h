#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_OUT_PROMPT				(WM_USER+103)						//提示出牌

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//游戏变量
protected:
	LONGLONG							m_lCellScore;						//基础分数

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bBombCount[GAME_PLAYER];			//炸弹数目
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	CPoint							m_PointUserPass[GAME_PLAYER];		//放弃位置
	CPoint							m_PointUserBomb[GAME_PLAYER];		//炸弹位置

	//历史积分
public:
	LONGLONG                            m_lAllTurnScore[3];					//总局得分
	LONGLONG                            m_lLastTurnScore[3];				//上局得分

	//位图变量
protected:
	CBitImageEx						m_ImageBomb;						//炸弹资源
	CBitImageEx						m_ImageBack;						//背景资源
	CBitImageEx						m_ImageFill;						//背景资源
	CBitImageEx						m_ImageGameInfo;					//游戏信息
	CBitImageEx						m_ImageUserPass;					//放弃标志

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btOutPrompt;						//提示按钮

	//扑克控件
public:
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[3];				//扑克视图
	CCardControl					m_LeaveCardControl[2];				//结束扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
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
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//功能函数
public:
	//基础分数
	void SetCellScore(LONGLONG lCellScore);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//炸弹数目
	void SetBombCount(WORD wChairID, BYTE bBombCount);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);

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