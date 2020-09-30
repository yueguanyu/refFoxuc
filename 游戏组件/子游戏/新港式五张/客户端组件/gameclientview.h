#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "HistoryRecord.h"
#include "GameLogic.h"
#include "PlayOperate.h"
#include "MoveDraw.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//视图位置
#define MYSELF_VIEW_ID				2									//视图位置

//动作标识
#define AC_NO_ADD					0									//不加动作
#define AC_FOLLOW					1									//跟注动作
#define AC_ADD_SCORE				2									//加注动作
#define AC_SHOW_HAND				3									//梭哈动作
#define AC_GIVE_UP					4									//放弃动作

//控制消息
#define IDM_START					(WM_USER+200)						//开始消息
#define IDM_FOLLOW					(WM_USER+201)						//跟注消息
#define IDM_GIVE_UP					(WM_USER+202)						//放弃消息
#define IDM_ADD_SCORE				(WM_USER+203)						//加注消息
#define IDM_SHOW_HAND				(WM_USER+204)						//梭哈消息

//通知消息
#define IDM_SEND_CARD_FINISH		(WM_USER+300)						//发牌完成

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &>	CSendCardItemArray;
typedef CWHArray<stuMoveChip*>						CChipArray;


//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//状态变量
protected:
	BOOL							m_bFirstChange;				
	LONG							m_lCellScore;						//游戏底注
	LONGLONG						m_lDrawMaxScore;					//最大下
	LONGLONG						m_lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG						m_lUserAddScore[GAME_PLAYER];		//用户结束增加值
	LONGLONG						m_lUserAddAllScore[GAME_PLAYER];	//用户结束增加值
	LONGLONG						m_lUserSaveScore[GAME_PLAYER];		//用户结束积分
	LONGLONG						m_lTableScore[GAME_PLAYER];			//下注数目
	LONGLONG						m_lOverScore[GAME_PLAYER];			//结束结算
	LONGLONG						m_lOverSaveScore[GAME_PLAYER];		//结束保存结算

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

	//界面状态
protected:
	bool							m_bShowScore;						//积分状态
	tagHistoryScore	*				m_pHistoryScore[GAME_PLAYER];		//积分信息

	//控制按钮
public:
	CSkinButton						m_btStart;							//开始按钮
	CPlayOperate					m_PlayOperate;						//玩家操作

	//游戏控件
public:
	CGameLogic						m_GameLogic;
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件

	//扑克筹码
public:
	CCardControl					m_CardControl[GAME_PLAYER];			//扑克控件
	LONGLONG						m_lPalyBetChip[GAME_PLAYER];		//玩家下注
	LONGLONG						m_lALLBetChip;						//桌面下注
	CChipArray						m_ArrayPlayChip;					//玩家下注数组
	CChipArray						m_ArrayBetChip;						//下注数组
	CChipArray						m_ArrayOverChip;					//回收数组

	CRect							m_rectBetChip;						//下注区域
	CSize							m_sizeWin;							//窗口大小


	//位置变量
protected:
	CPoint							m_ptUserInfo[GAME_PLAYER];			//玩家信息
	CPoint							m_ptUserAction[GAME_PLAYER];		//动作位置
	CPoint							m_ptPalyBetChip[GAME_PLAYER];		//玩家下注
	CPoint							m_ptPalyPos[GAME_PLAYER];			//玩家下注
	CPoint							m_ptOverScore[GAME_PLAYER];			//结束积分

	//资源大小
protected:
	CSize							m_SizeHistory;						//积分大小
	CSize							m_SizeGameCard;						//扑克大小

	//资源变量
protected:
	CPngImage						m_ImageReady;						// 准备

	CPngImage						m_ImageGameCard;					// 扑克资源
	CBitImage						m_ImageViewFill;					// 背景资源
	CPngImage						m_ImageViewBack;					// 背景资源

	CPngImage						m_ImageActionBack;					// 操作背景
	CPngImage						m_ImageActionFont;					// 操作
	CPngImage						m_ImageShowHand;					// 梭哈

	CPngImage						m_ImageUserInfoH;					// 玩家背景横
	CPngImage						m_ImageUserInfoV;					// 玩家背景竖
	CPngImage						m_ImageUserPalyH;					// 玩家背景横
	CPngImage						m_ImageUserPalyV;					// 玩家背景竖
	CPngImage						m_ImageChip;						// 筹码位图

	CPngImage						m_ImageCellScore;					// 低分背景
	CPngImage						m_ImgaeScoreTotal;					// 桌面积分
	CPngImage						m_ImageTipInfo;						// 小贴士

	CPngImage						m_ImageCellNumber;					// 低分数字
	CPngImage						m_ImgaeScoreTotalNumber;			// 总注数字

	CPngImage						m_ImgaeScoreBackR;					// 结束背景
	CPngImage						m_ImgaeScoreBackL;					// 结束背景
	CPngImage						m_ImgaeScoreBackM;					// 结束背景

	CPngImage						m_ImgaeScoreSum;					// 结束符号
	CPngImage						m_ImgaeScoreWin;					// 结束分数
	CPngImage						m_ImgaeScoreLose;					// 结束分数

	CPngImage						m_ImageTime;						// 时间数字

	CPngImage						m_ImageCradType;					// 牌型
	CDFontEx						m_DFontEx;							// 字体

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

	//控制接口
public:
	//重置界面
	virtual VOID ResetGameView();

	//继承函数
protected:
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//是否特殊绘制
	virtual bool RealizeWIN7() { return true; }

	//发牌函数
public:
	//是否发牌
	bool IsDispatchStatus();
	//停止发牌
	VOID ConcludeDispatch();
	//发送扑克
	VOID DispatchUserCard(WORD wChairID, BYTE cbCardData);

	//功能函数
public:
	//游戏底注
	bool SetCellScore(LONG lCellScore);
	//最大下注
	bool SetDrawMaxScore(LONGLONG lDrawMaxScore);
	//设置积分
	bool SetUserScore(WORD wChairID, LONGLONG lUserScore);
	//设置积分
	bool SetUserSaveScore(WORD wChairID, LONGLONG lUserScore);
	//设置结束积分
	bool SetOverScore(WORD wChairID, LONGLONG lUserScore);
	//设置动作
	bool SetUserAction(WORD wActionUser, BYTE cbUserAction);
	//设置积分
	bool SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore);
	//设置下注
	void SetUserTableScore(WORD wChairID, LONGLONG lTableScore);
	// 添加桌面下注
	void AddDeskChip( LONGLONG lScore, WORD wChairID );
	// 添加玩家下注
	void AddPlayChip( LONGLONG lScore, WORD wChairID );
	// 添加玩家加注
	void AddPlayBetChip( LONGLONG lScore, WORD wChairID );
	// 筹码回收
	void ChioRecovery( WORD wChairID );

	
	//内部函数
private:
	// 绘画数字
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// 绘画数字
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	// 绘画字符
	bool DrawTextStringEx(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, INT nDrawFormat);
	// 绘画字符
	bool DrawTextStringEx(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, INT nXPos, INT nYPos);
	// 绘画结束分数
	void DrawOverScore(CDC * pDC, LONGLONG lOutNum, INT nXPos, INT nYPos );
	// 添加逗号
	CString AddComma( LONGLONG lScore );

	//按钮消息
public:
	//开始按钮
	VOID OnBnClickedStart();
	//跟注按钮
	VOID OnBnClickedFollow();
	//放弃按钮
	VOID OnBnClickedGiveUp();
	//加注按钮
	VOID OnBnClickedAddScore();
	//加注按钮
	VOID OnBnClickedAddScoreOne();
	//加注按钮
	VOID OnBnClickedAddScoreTwo();
	//加注按钮
	VOID OnBnClickedAddScoreThree();
	//加注按钮
	VOID OnBnClickedAddScoreFour();
	//梭哈按钮
	VOID OnBnClickedShowHand();
	//积分按钮
	VOID OnBnClickedADD();
	//积分按钮
	VOID OnBnClickedSUB();

	//消息函数
public:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	// 消息函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif