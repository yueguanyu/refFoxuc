#pragma once

#include "Stdafx.h"
#include "JettonControl.h"
#include "NumberControl.h"
#include "CardControl.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//视图位置
#define MYSELF_VIEW_ID				2									//视图位置

#define AC_GIVE_CARD				1									//要牌动作
#define AC_STOP_CARD				2									//停牌动作
#define AC_ADD_SCORE				3									//加倍动作

//控制消息
#define IDM_START					(WM_USER+200)						//开始消息
#define IDM_GIVE_CARD				(WM_USER+201)						//要牌消息
#define IDM_STOP_CARD				(WM_USER+202)						//停牌消息
#define IDM_ADD_SCORE				(WM_USER+207)						//加倍消息
#define IDM_USER_SCORE				(WM_USER+208)						//下注消息

//通知消息
#define IDM_SEND_CARD_FINISH		(WM_USER+300)						//发牌完成
#define IDM_MOVE_JET_FINISH			(WM_USER+301)						//移动筹码完成

//////////////////////////////////////////////////////////////////////////

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

///////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//状态变量
protected:
	LONGLONG						m_lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG						m_lUserAddScore[GAME_PLAYER];		//用户加注分
	FLOAT							m_fUserCardScore[GAME_PLAYER];		//用户扑克分
	LONGLONG						m_lCellScore;						//单元积分
	LONGLONG						m_lJettonScore;						//总筹码数
	WORD							m_wBankerUser;						//庄家
	bool							m_bWaitUserScore;					//等待下注
	bool							m_bShowAddJetton;					//显示标识

	//发牌动画变量
protected:
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_SendCardCurPos;					//发牌当前位置
	INT								m_nStepCount;						//步数
	INT								m_nXStep;							//步长
	INT								m_nYStep;							//步长
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//动作变量
protected:
	WORD							m_wActionUser;						//动作用户
	BYTE							m_cbUserAction;						//动作标识

	//位置变量
protected:
	CPoint							m_ptUserAction[GAME_PLAYER];		//动作位置
	CPoint							m_ptJettons[GAME_PLAYER];			//筹码位置
	CPoint							m_ptBanker[GAME_PLAYER];			//庄家位置

	INT								m_nXFace;
	INT								m_nYFace;
	INT								m_nXTimer;
	INT								m_nYTimer;

	//大小变量
	CSize							m_SizeGameCard;						//扑克大小

	//资源变量
protected:
	CBitImage						m_ImageGameCard;					//扑克资源
	CBitImage						m_ImageViewFill;					//背景资源
	CBitImage						m_ImageViewBack;					//背景资源
	CPngImage						m_PngActionBack;					//操作背景
	CPngImage						m_PngActionFont;					//操作
	CPngImage						m_PngNumber;						//扑克分数字
	CPngImage						m_PngScoreNum;						//筹码分数字
	CPngImage						m_PngActionScoreBack;				//加注提示
	CPngImage						m_PngActionScoreNum;				//加注提示
	CPngImage						m_PngBkTopLeft;						//
	CPngImage						m_PngBkTopRight;					//
	CPngImage						m_PngBkBottomLeft;					//
	CPngImage						m_PngBkBottomRight;					//
	CBitImage						m_ImageTitle;						//标题位图
	CPngImage						m_PngCardScore;						//牌点
	CPngImage						m_PngBanker;						//庄家标志
	CPngImage						m_PngWaitScore;						//等待下注

	//控制按钮
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btGiveCard;						//要牌按钮
	CSkinButton						m_btStopCard;						//停牌按钮
	CSkinButton						m_btAddScore;						//加倍按钮
	CSkinButton						m_btOneScore;						//一倍按钮
	CSkinButton						m_btTwoScore;						//两倍按钮
	CSkinButton						m_btThreeScore;						//三倍按钮
	CSkinButton						m_btFourScore;						//四倍按钮

	//控件变量
public:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CCardControl					m_CardControl[GAME_PLAYER];			//扑克控件
	CJettonControl					m_JettonControl;
	CNumberControl					m_NumberControl[GAME_PLAYER];

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//继承函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }

	//功能函数
public:
	//设置积分
	bool SetScoreInfo(WORD wChairID, LONGLONG lUserScore);
	//设置动作
	bool SetUserAction(WORD wActionUser, BYTE cbUserAction);
	//设置牌面分
	VOID SetCardScore( WORD wChairId, FLOAT fCardScore );
	//设置单元分
	VOID SetCellScore( LONGLONG lScore );
	//设置游戏结束信息
	VOID SetGameEndInfo( WORD wWinner );
	//设置庄家
	VOID SetBankerUser( WORD wBankerUser );
	//设置等待
	VOID SetWaitUserScore( bool bWaitUserScore );
	//设置显示
	VOID ShowAddJettonInfo( bool bShow );

	//动画函数
public:
	//是否发牌
	bool IsDispatchStatus();
	//停止发牌
	VOID ConcludeDispatch();
	//发送扑克
	VOID DispatchUserCard(WORD wChairID, BYTE cbCardData);

	//玩家加注
	VOID OnUserAddJettons( WORD wChairId, LONGLONG lScore );
	//玩家得分
	VOID OnUserRemoveJettons( WORD wChairId, LONGLONG lScore );
	//开始筹码动画
	VOID BeginMoveJettons();
	//停止筹码动画
	VOID StopMoveJettons();

	//开始数字动画
	VOID BeginMoveNumber();
	//停止数字动画
	VOID StopMoveNumber();

	//按钮消息
public:
	//开始按钮
	VOID OnBnClickedStart();
	//跟注
	VOID OnBnClickedGiveCard();
	//放弃
	VOID OnBnClickedStopCard();
	//加倍
	VOID OnBnClickedAddScore();
	//1倍底注
	VOID OnBnClickedOneScore();
	//2倍底注
	VOID OnBnClickedTwoScore();
	//3倍底注
	VOID OnBnClickedThreeScore();
	//4倍底注
	VOID OnBnClickedFourScore();

	//消息函数
public:
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器
	VOID OnTimer(UINT nIDEvent);
	//左击
	VOID OnLButtonDown(UINT nFlags, CPoint point);
	//光标设置
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//更新视图
	void RefreshGameView();
	//更新视图
	void RefreshGameView(CRect rect);

	//内部函数
protected:
	//绘画数字,参数bHasSign表示位图是否有正负
	VOID DrawNumberString( CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, CPngImage &PngNumber, bool bHasSign );
	//绘画字符
	bool DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, INT nXPos, INT nYPos);
	//绘画牌点
	VOID DrawCardScore( CDC * pDC, INT nXPos, INT nYPos, FLOAT fCardScore, bool bLeftAlign );

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////