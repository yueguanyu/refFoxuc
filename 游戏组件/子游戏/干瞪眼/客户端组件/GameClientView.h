#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "HistoryScore.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_LAND_SCORE				(WM_USER+103)						//叫分消息
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //提示消息
#define IDM_LAND_AUTOPLAY			(WM_USER+105)                       //托管消息
#define IDM_SORT_HAND_CARD			(WM_USER+106)						//排列扑克
#define IDM_MUSIC					(WM_USER+110)						//声音消息

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//游戏变量
protected:
	WORD							m_wLandUser;						//地主用户
	WORD							m_wBombTime;						//炸弹倍数
	LONGLONG							m_lBaseScore;						//基础分数
	BYTE							m_cbLandScore;						//地主分数
	BYTE							m_bLeftCardCount;

	//状态变量
public:
	bool							m_bLandTitle;						//地主标志
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bScore[GAME_PLAYER];				//用户叫分
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	bool							m_bUserTrustee[GAME_PLAYER];		//玩家托管
	bool							m_bShowScore;						//积分状态
	tagHistoryScore	*				m_pHistoryScore[GAME_PLAYER];		//积分信息

	//爆炸动画
protected:
	bool							m_bBombEffect;						//爆炸效果
	BYTE							m_cbBombFrameIndex;					//帧数索引

	//位置信息
public:
	CSize							m_LandSize;							//挖坑标志
	CPoint							m_ptPass[GAME_PLAYER];
	CPoint							m_ptWarningCard[GAME_PLAYER];		//报牌位置

	//位图变量
protected:
	//CBitImageEx						m_ImageBomb;						//炸弹图片
	CBitImageEx						m_ImageBack;						//背景资源
	CBitImageEx						m_ImageCenter;						//背景资源
	CBitImageEx						m_ImageBombEffect;					//炸弹效果
	CBitImageEx						m_ImageHistoryScore;				//历史积分
	CBitImageEx						m_ImageNumber;						//数字资源
	CBitImageEx						m_ImageLeftCard;
	CBitImageEx						m_ImageLeftCardText;
	CBitImageEx						m_ImageFrame;

	CPngImageEx						m_ImageScore;						//分数图片
	CPngImageEx						m_ImageLand;						//庄家图片
	CPngImageEx						m_ImageUserTrustee;					//玩家托管
	CPngImageEx						m_ImageCardCount;

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btAutoOutCard;					//提示按钮
	CSkinButton						m_btSortCard;						//排序按钮
	CSkinButton						m_btAutoPlayOn;						// 开启自动托管
	CSkinButton						m_btAutoPlayOff;					// 关闭自动托管
	CSkinButton						m_btScore;							//看分按钮
	CSkinButton						m_btMusic;

	//扑克控件
public:
	CCardControl					m_BackCardControl;					//底牌扑克
	CCardControl					m_UserCardControl[GAME_PLAYER];		//扑克视图
	CCardControl					m_HandCardControl[GAME_PLAYER];		//手上扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CSize							m_sizeHistory;						//积分大小


	//加载资源
	CPngImageEx ImageTimeBack;
	CPngImageEx ImageTimeNumber;
	CPngImageEx ImageUserReady;

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
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

	//重载函数
public:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//功能函数
public:
	//炸弹倍数
	void SetBombTime(WORD wBombTime);
	//基础分数
	void SetBaseScore(LONGLONG lBaseScore);
	//显示提示
	void ShowLandTitle(bool bLandTitle);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//挖坑分数
	void SetLandUser(WORD wChairID, BYTE bLandScore);
	//挖坑分数
	void SetLandScore(WORD wChairID, BYTE bLandScore);
	//设置爆炸
	bool SetBombEffect(bool bBombEffect);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos,int nWidth, int nWeight,UINT nFormat=DT_LEFT);
	//玩家托管
	void SetUserTrustee( WORD wChairID, bool bTrustee );
	//设置积分
	bool SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore);
	//设置剩余牌
	void SetLeftCardCount(BYTE bCount);

	void UpdateGameView(CRect *rc);

	//内部函数
private:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//绘画时间
	void DrawUserTimerEx(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea=99);
	//绘画准备
	void DrawUserReadyEx(CDC * pDC, int nXPos, int nYPos);
	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//叫分按钮
	afx_msg void OnOneScore();
	//叫分按钮
	afx_msg void OnTwoScore();
	//放弃按钮
	afx_msg void OnGiveUpScore();
	//出牌提示
	afx_msg void OnAutoOutCard();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//叫分按钮
	afx_msg void OnThreeScore();
	// 自动托管
	afx_msg void OnAutoPlayerOn();
	// 取消托管
	afx_msg void OnAutoPlayerOff();
	//排序按钮
	VOID OnBnClickedSortCard();
	//积分按钮
	VOID OnBnClickedScore();
	//背景音乐
	VOID OnBnClickedMusic();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif