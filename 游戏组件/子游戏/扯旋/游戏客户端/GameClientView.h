#pragma once

#include "Stdafx.h"
#include "GoldView.h"
#include "ScoreView.h"
#include "GameOption.h"
#include "CardControl.h"
#include "JetonControl.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_START						100								//开始按钮
#define	IDC_INVEST						101								//下本按钮
#define	IDC_ADD_SCORE					102								//加注按钮
#define	IDC_FOLLOW						103								//跟注按钮
#define	IDC_SHOW_HAND					104								//全压按钮
#define	IDC_PASS						105								//不加按钮
#define	IDC_GIVE_UP						106								//放弃按钮
#define	IDC_CLEAR_SCORE					107								//清理按钮
#define	IDC_OPEN_CARD					108								//开牌按钮
#define	IDC_DECREASE1					109								//减注按钮
#define	IDC_DECREASE2					110								//减注按钮
#define	IDC_DECREASE3					111								//减注按钮
#define	IDC_DECREASE4					112								//减注按钮

//消息定义
#define IDM_OPEN_CARD_FINISH		WM_USER+121							//开牌完成
#define IDM_SEND_CARD_FINISH		WM_USER+120							//发牌完成
#define IDM_START					WM_USER+100							//开始消息
#define IDM_ADD_SCORE				WM_USER+102							//加注消息
#define IDM_CLEAR_SCORE				WM_USER+112							//清理消息
#define IDM_OPEN_CARD				WM_USER+113							//开牌消息
#define IDM_DECREASE				WM_USER+114							//减注消息

#define	MY_VIEW_CHAIR_ID				2								//间隔速度
//////////////////////////////////////////////////////////////////////////

//移动类型
#define MOVE_USER_ADD				0								//底注筹码下注
#define MOVE_POOL_ALLSCORE			1								//加注筹码移至中间
#define MOVE_USER_GETSCORE			2								//中加筹码移至底注
#define MOVE_BACK_SCORE				3								//返回筹码

//结构定义
//////////////////////////////////////////////////////////////////////////

//筹码状态
struct tagJettonStatus
{
	//属性信息
	WORD							wMoveCount;							//移动次数
	WORD							wMoveIndex;							//移动索引

	//筹码信息
	CPoint							ptFrom;								//出发位置
	CPoint							ptDest;								//目的位置
	CPoint							ptCourse;							//过程位置
	LONGLONG						lGold;								//筹码数目

	//移动形式
	INT								iMoveType;							//移动形式
};

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//游戏变量
protected:
	bool							m_bUserOffLine[GAME_PLAYER];		//用户断线				
	bool							m_bWaitCompareHeap;					//开牌比较
	bool							m_bWaitCompareTail;					//开牌比较
	bool							m_bWaitInvest;						//等待下本
	bool							m_bWaitOpenCard;					//等待摊牌
	bool							m_bHitPositively;					//点击响应
	bool							m_bUserOperate[GAME_PLAYER];		//操作用户
	WORD							m_wBankerUser;						//庄家用户
	BYTE							m_cbHitCardData[6];					//点击信息
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//用户数据
	LONGLONG						m_lCellScore;						//单元注数
	LONGLONG						m_lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG						m_lTableScore[GAME_PLAYER];			//桌面金币
	LONGLONG						m_lCenterScore;						//总金币数
	TCHAR							m_szHeapCard[GAME_PLAYER][LEN_NICKNAME];//头牌牌型
	TCHAR							m_szTailCard[GAME_PLAYER][LEN_NICKNAME];//尾牌牌型

	//动作动画
protected:
	WORD							m_wActionUser;						//动作用户
	WORD							m_wUserAction;						//动作标识

	//动画变量
protected:
	WORD							m_wSendCount;						//移动次数
	WORD							m_wSendIndex;						//移动次数
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//动画变量
protected:
	bool							m_bFlashCard;						//散射标志
	WORD							m_wDelayCount;						//延迟次数
	WORD							m_wDelayIndex;						//延迟次数
	WORD							m_wCurrentOpenUser;					//开牌用户
	WORD							m_wHeapMaxUser;						//头大用户

	//动画变量
public:
	bool							m_bJettonAction;					//动画标志
	tagJettonStatus					m_JettonStatus[GAME_PLAYER];		//筹码信息

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btInvest;							//下本按钮
	CSkinButton						m_btAddScore;						//加注按钮
	CSkinButton						m_btFollow;							//跟注按钮
	CSkinButton						m_btShowHand;						//全压按钮
	CSkinButton						m_btPass;							//不加按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btClearScore;						//清理按钮
	CSkinButton						m_btOpenCard;						//开牌按钮
	CSkinButton						m_btDecrease[CONTROL_COUNT];		//减注按钮

	//控件变量
public:
	CScoreView						m_ScoreView;						//成绩窗口
	CCardControl					m_CardControl[GAME_PLAYER];			//用户扑克
	CJettonControl					m_JetonControl;						//筹码控件
	CGoldView						m_MoveGoldView[GAME_PLAYER];		//筹码控件
	CGoldView						m_AddGoldView[GAME_PLAYER];			//筹码控件
	CGoldView						m_UserGoldView[GAME_PLAYER];		//筹码控件
	CGoldView						m_CenterGoldView;					//中心筹码

	//位置信息
protected:
	CPoint							m_ptBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptCenterJetton;					//筹码位置
	CPoint							m_ptUserJetton[GAME_PLAYER];		//筹码位置
	CPoint							m_ptMyJetton;						//筹码位置
	CPoint							m_ptUserAction[GAME_PLAYER];		//动作位置
	CPoint							m_SendEndingPos[GAME_PLAYER];		//桌牌位置
	CPoint							m_SendStartPos;						//始发位置
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_ptOperateBack[GAME_PLAYER];		//操作提示
	CPoint							m_ptJettonBack[GAME_PLAYER];		//筹码背景
	CPoint							m_ptAddJetton[GAME_PLAYER];			//筹码位置
	CPoint							m_ptHitCard[6];						//点击位置
	CPoint							m_ptControlBack;					//点击位置

	int								m_nXFace;
	int								m_nYFace;
	int								m_nXBorder;
	int								m_nYBorder;
	int								m_nYTimer;
	int								m_nXTimer;
	//界面变量
protected:
	CBitImage						m_ImageBanker;						//庄家资源
	CBitImage						m_ImageCard;						//扑克资源
	CBitImage						m_ImageViewBack;					//背景资源
	CBitImage						m_ImageControlBack;					//背景资源

	//提示变量
protected:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

	//WIN7支持
	virtual bool RealizeWIN7() { return true; }

	//功能函数
public:
	//设置断线
	void SetOffLinkUser(WORD wChairID,bool bOffLink);
	//设置牌型
	void SetCardType(WORD wChairID,LPCTSTR pszHeapString,LPCTSTR pszTailString);
	//获取扑克
	bool GetHitCardData(BYTE cbCardData[],BYTE cbCardCount);
	//设置操作
	void SetUserOperate(WORD wChairID,bool bOperate);
	//扑克数据
	void SetHitCardData(BYTE cbCardData[],BYTE cbCardCount);
	//设置点击
	void SetHitPositively(bool bPositively);
	//用户积分
	void SetUserJetton(WORD wChairID,LONGLONG lJetton);
	//设置单元
	void SetCellScore(LONGLONG lCellScore);
	//设置下注
	void SetUserTableScore(WORD wChairID, LONGLONG lTableScore);
	//中心筹码
	void SetCenterScore(LONGLONG lCenterScore);

	//比较信息
	void SetOpenCardInfo(BYTE cbCardData[GAME_PLAYER][MAX_COUNT],WORD wCurrentOpenUser,WORD wHeapMaxUser);
	//设置动作
	bool SetUserAction(WORD wActionUser, WORD cbUserAction);
	//移动信息
	void SetJettonMoveInfo(WORD wChairID, int iMoveType, LONGLONG lTableScore);
	//开牌动画
	bool CartoonOpenCard();
	//移动筹码
	bool CartoonMoveJetton();
	//移动扑克
	bool CartoonMoveCard();
	//完成发牌
	void FinishDispatchCard();

	//动画标志
	bool IsMoveing(){return (m_bJettonAction || m_SendCardItemArray.GetCount()>0);}
	//移动标志
	bool IsCardMoveing(){return (m_SendCardItemArray.GetCount()>0);}
	//移动标志
	bool IsJettonMoveing(){return m_bJettonAction;}
	//发送扑克
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//庄家标志
	void SetBankerUser(WORD wBankerUser);
	//等待下本
	void SetWaitInvest(bool bWaitInvest);
	//等待开牌
	void SetWaitOpenCard(bool bWaitOpenCard);
	//等待比较
	void SetWiatCompare(bool bWaitCompareHeap,bool bWaitCompareTail);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);
	//更新视图
	void RefreshGameView();
	//更新视图
	void RefreshGameView(CRect &rect);

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//左击消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
