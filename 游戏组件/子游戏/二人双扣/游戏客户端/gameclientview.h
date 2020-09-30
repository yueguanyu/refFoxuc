#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
//常量定义
#define ME_VIEW_CHAIR				1									//自己位置


//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_REQ_HUNTER				(WM_USER+103)						//申请抄底
#define IDM_OUT_PROMPT				(WM_USER+104)						//提示出牌
#define IDM_SORT_BY_COUNT			(WM_USER+105)						//数目排序
#define IDM_TRUSTEE_CONTROL			(WM_USER+106)						//托管控制
#define IDM_CARD_TYPE				(WM_USER+107)						//牌型提示		
#define IDM_LAST_TURN_CARD			(WM_USER+108)						//上一轮标志

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//位置信息
protected:
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置
	CPoint							m_PointUserPass[GAME_PLAYER];		//放弃位置
	CPoint							m_PointWinOrder[GAME_PLAYER];		//输赢名次
	CPoint                          m_PointMessageDlg[GAME_PLAYER];		//语音消息
	INT								m_nXFace;
	INT								m_nYFace;
	INT								m_nXTimer;
	INT								m_nYTimer;
	INT								m_nXBorder;
	INT								m_nYBorder;
	//移动变量
protected:
	bool							m_bMoveMouse;						//移动标志
	bool							m_bSwitchCard;						//交换标志
	bool							m_bSelectCard;						//选择标志
	WORD							m_wHoverCardItem;					//盘旋索引
	WORD							m_wMouseDownItem;					//点击索引
	//上轮标志
protected:
	bool							m_bLastTurnCard;					//上轮扑克
	//状态变量
public:
	bool							m_bShowScore;						//积分状态
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目	
	
	//历史积分
public:
	LONGLONG                        m_lAllTurnScore[GAME_PLAYER];		//总局得分
	LONGLONG                        m_lLastTurnScore[GAME_PLAYER];		//上局得分

	//爆炸动画
protected:
	bool							m_bBombEffect;						//爆炸效果
	BYTE							m_cbBombFrameIndex;					//帧数索引

	//位图变量
protected:
	CPngImage						m_ImageTrustee;						//托管标志	
	CBitImage						m_ImageViewFill;					//背景位图
	CBitImage						m_ImageViewBack;					//背景位图
	CBitImage						m_ImageUserPass;					//放弃标志
	CBitImage						m_ImageWinOrder;						//名次位图
	CBitImage						m_ImageLastTurn;					//上轮标志
	CPngImage						m_ImageBombEffect;					//爆炸位图
	CBitImage						m_ImageHistoryScore;				//积分位图

	//图片大小
private:
	CSize							m_sizeHistory;						//积分大小

	//按钮控件
public:
	CSkinButton						m_btScore;							//看分按钮
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btHunter;							//抄底按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btLastTurnCard;					//上轮扑克
	CSkinButton						m_btStusteeControl;					//拖管控制
	//游戏控件
public:
	CCardControl					m_HandCardControl[GAME_PLAYER];		//手上扑克
	CCardControl					m_UserCardControl[GAME_PLAYER];		//扑克视图
	CScoreView						m_ScoreView;						//积分视图
#ifdef VIDEO_GAME
	//视频组件
private:
	CVideoServiceControl 			m_DlgVedioService[GAME_PLAYER];				//视频窗口
#endif

	//辅助函数
protected:
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame,CRect*rect);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//是否特殊绘制
	virtual bool RealizeWIN7() { return true; }

	//功能函数
public:
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//设置名次
	bool SetUserWinOrder(WORD wChairID, WORD wWinOrder);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);
	//设置爆炸
	bool SetBombEffect(bool bBombEffect);
	//设置上轮
	void SetLastTurnCard(bool bLastTurnCard);
	//更新视图
	void RefreshGameView();

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//申请抄底
	afx_msg void OnReqHunter();
	//放弃按钮
	afx_msg void OnPassCard();
	//上轮扑克
	afx_msg void OnLastTurnCard();
	//拖管控制
	afx_msg void OnStusteeControl(); 
	//积分按钮
	afx_msg void OnBnClickedScore();
	//关闭按钮
	afx_msg void OnBnClickedScoreClose();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	//鼠标消息
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
