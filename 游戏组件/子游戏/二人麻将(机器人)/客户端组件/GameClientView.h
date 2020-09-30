#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once
#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "DrawSiceWnd.h"
#include "DlgListen.h"
#include "GameScore.h"
#include "MoveCardItem.h"
#include "CHistoryScore.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_TRUSTEE_CONTROL			(WM_USER+102)						//托管控制
#define IDM_CARD_OPERATE			(WM_USER+106)						//玩家操作牌
#define IDM_MOVE_CARD_FINISH		(WM_USER+107)						//扑克动画结束消息

//////////////////////////////////////////////////////////////////////////

//视图位置
#define MYSELF_VIEW_ID				1									//视图位置


//////////////////////////////////////////////////////////////////////////////////
struct tagMoveCardItem
{
	CPoint			ptFrom;												//开始位置
	CPoint			ptTo;												//到达位置
	WORD			wViewChairId;										//出牌或发牌视图位置
	BYTE			cbCardData;											//出牌或发牌数据
	BYTE			cbCardCount;										//牌数
};

//游戏视图                     CGameFrameViewGDI
class CGameClientView : public CGameFrameViewGDI 
{
	//标志变量
protected:
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	bool							m_bHuangZhuang;						//荒庄标志
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
	bool							m_bListenStatus[GAME_PLAYER];		//听牌标志

	//游戏属性
protected:
	LONGLONG							m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbFengQuan;						//风圈

	//动作动画
protected:
	bool							m_bBombEffect;						//动作效果
	BYTE							m_cbBombFrameIndex;					//帧数索引

	//丢弃效果
	WORD							m_wDiscUser;						//丢弃用户
	BYTE							m_cbDiscFrameIndex;					//帧数索引	

	//发牌或出牌动画变量
protected:
	bool							m_bEnableAnimate;					//是否显示发牌动画
	bool							m_bCardMoving;						//是否在动画
	int								m_nXStep;							//X步长
	int								m_nYStep;							//Y步长
	int								m_nStepCount;						//移动步数
	enDirection						m_enMoveDirection;					//移动方向
	CRect							m_rcCardMove;						//绘画区域
	tagMoveCardItem					m_MoveCardItem;						//动画项目

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbUserAction[GAME_PLAYER];		//用户动作
	tagHistoryScore	*				m_pHistoryScore[GAME_PLAYER];		//积分信息

	//位置变量
protected:
	CPoint							m_UserFlagPos[GAME_PLAYER];			//标志位置
	CPoint							m_UserListenPos[GAME_PLAYER];		//标志位置
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置

	//位图变量
protected:
	CBitImageEx						m_ImageHistoryScore;				//历史积分
	CBitImageEx						m_ImageBack;						//背景图案
	CBitImageEx						m_ImageCenter;						//LOGO图	
	
	CPngImageEx						m_ImageHuangZhuang;					//荒庄标志
	CPngImageEx						m_ImageWait;						//等待提示
	CPngImageEx						m_ImageOutCard;						//出牌提示
	CPngImageEx						m_ImageUserFlag;					//用户标志
	CPngImageEx						m_ImageActionBack;					//动作背景
	CPngImageEx						m_ImageTrustee;						//托管标志
	CPngImageEx						m_PngFengQuan;						//圈风标志
	CPngImageEx						m_PngListenFlag;					//听牌标志
	CPngImageEx						m_ImageActionAni;					//吃牌动画资源
	CPngImageEx						m_ImageDisc;						//丢弃效果
	CPngImageEx						m_ImageArrow;						//定时器箭头	

	CPngImageEx						ImageTimeBack;
	CPngImageEx						ImageTimeNumber;
	CPngImageEx						ImageUserReady;
	//扑克控件
public:
	CHeapCard						m_HeapCard[GAME_PLAYER];			//堆立扑克
	CUserCard						m_UserCard;							//用户扑克
	CTableCard						m_TableCard[GAME_PLAYER];					//桌面扑克
	CWeaveCard						m_WeaveCard[GAME_PLAYER][MAX_WEAVE];		//组合扑克
	CDiscardCard					m_DiscardCard[GAME_PLAYER];					//丢弃扑克
	CCardControl					m_HandCardControl;					//手上扑克

	//控件变量
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btStusteeControl;					//拖管控制
	CControlWnd						m_ControlWnd;						//控制窗口
	CDrawSiceWnd					m_DrawSiceWnd;						//色子控件
	CDlgListen						m_DlgListen;						//听牌对话框
	CGameScore						m_GameScore;						//成绩框
	CSize							m_sizeHistory;						//积分大小
	CSkinButton						m_btScore;							//看分按钮
	bool							m_bShowScore;						//积分状态

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
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//设置积分
	bool SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore);
	//基础积分
	void SetCellScore(LONGLONG lCellScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//荒庄设置
	void SetHuangZhuang(bool bHuangZhuang);
	//状态标志
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);
	//设置动作
	bool SetBombEffect(bool bBombEffect);
	//丢弃用户
	void SetDiscUser(WORD wDiscUser);
	//定时玩家
	void SetCurrentUser(WORD wCurrentUser);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);
	//听牌标志
	void SetUserListenStatus(WORD wViewChairID, bool bListenStatus);
	//设置风圈
	void SetFengQuan( BYTE cbFengQuan );
	//停止出牌或发牌动画
	bool StopMoveCard();
	//是否动画进行中
	bool IsMovingCard() { return m_bCardMoving; }
	//允许动画
	void EnableAnimate( bool bAnimate );
	//是否允许动画
	bool IsEnableAnimate() { return m_bEnableAnimate; }
	//扑克动画
	void OnMoveCardItem( const tagMoveCardItem &MoveCardItem ,bool bdebug=false);
	//单元积分
	LONGLONG GetlCellScore(){return m_lCellScore;}

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos,int nWidth, int nWeight,UINT nFormat=DT_LEFT);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,int nWidth, int nWeight,UINT nFormat=DT_LEFT);
	//绘画时间
	void DrawUserTimerEx(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea=99);
	//绘画准备
	void DrawUserReadyEx(CDC * pDC, int nXPos, int nYPos);
	//绘画扑克动画
	void DrawMoveCardItem( CDC *pDC );

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif