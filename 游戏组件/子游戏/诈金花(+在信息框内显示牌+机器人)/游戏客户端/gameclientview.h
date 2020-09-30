#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "GoldControl.h"
#include "JettonControl.h"
#include "NumberControl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//消息定义
#define IDM_START					WM_USER+100							//开始消息
#define IDM_ADD_SCORE				WM_USER+101							//加注消息
#define	IDM_CONFIRM					WM_USER+109							//确定消息	
#define	IDM_CANCEL_ADD				WM_USER+119							//取消消息	
#define IDM_MIN_SCORE				WM_USER+102							//最少加注
#define IDM_MAX_SCORE				WM_USER+103							//最大加注
#define IDM_LOOK_CARD				WM_USER+104							//看牌消息
#define IDM_COMPARE_CARD			WM_USER+105							//比牌消息
#define IDM_OPEN_CARD				WM_USER+106							//开牌消息
#define IDM_GIVE_UP					WM_USER+107							//放弃消息
#define IDM_SEND_CARD_FINISH		WM_USER+108							//发牌完成
#define IDM_COMPARE_USER			WM_USER+110							//被选用户
#define IDM_FALSH_CARD_FINISH		WM_USER+114							//闪牌完成

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//筹码信息
struct tagGoldMes
{
	LONGLONG						lGoldType;							//筹码类型
	LONGLONG						lDrawCount;							//绘画次数
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//数据变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	LONGLONG						m_lMaxCellScore;					//最大下注
	LONGLONG						m_lCellScore;						//最少下注
	LONGLONG						m_lTableScore[GAME_PLAYER];			//下注数目
	LONGLONG						m_lStopUpdataScore[GAME_PLAYER];	//保持数目
	bool							m_bInvest;							//等待下注
	bool							m_bCompareCard;						//选比标志
	WORD							m_wWaitUserChoice;					//等待标志

	//牌型数据
public:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool							m_bShow;							//显示牌型
	TCHAR							m_tcBuffer[GAME_PLAYER][64];		//牌的类型

	//比牌变量
protected:
	WORD							m_wConmareIndex;					//比牌索引
	WORD							m_wConmareCount;					//比牌数目
	WORD							m_wCompareChairID[2];				//比牌用户

	//闪牌变量
protected:
	bool							m_bFalsh;							//闪牌参数
	WORD							m_wFlashUser[GAME_PLAYER];			//闪牌用户
	WORD							m_wFalshCount;						//闪牌次数

	//动画标志
protected:
	bool							m_bCartoon;							//动画标志
	WORD							m_wLoserUser;						//比牌败户
	bool							m_bStopDraw;						//闪牌标志
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//位置变量
protected:
	CPoint							m_ptUserCompare;					//比牌位置
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_ptUserCard[GAME_PLAYER];			//桌牌位置
	CPoint							m_KeepPos;							//保存位置

	INT								m_nXFace;
	INT								m_nYFace;
	INT								m_nXTimer;
	INT								m_nYTimer;
	//用户状态
protected:
	BOOL							m_bCompareUser[GAME_PLAYER];		//被选比牌用户

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btMaxScore;						//最大按钮
	CSkinButton						m_btMinScore;						//清理按钮
	CSkinButton						m_btCancel;							//清理按钮
	CSkinButton						m_btConfirm;						//取消按钮
	CSkinButton						m_btAddScore;						//加注按钮
	CSkinButton						m_btFollow;							//跟注按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btLookCard;						//看牌按钮
	CSkinButton						m_btCompareCard;					//比牌按钮
	CSkinButton						m_btOpenCard;						//开牌按钮

	//控件变量
public:
	CScoreView						m_ScoreView;						//成绩窗口
	CGoldControl					m_GoldControl;						//加注视图
	CCardControl					m_CardControl[GAME_PLAYER];			//用户扑克
	CJettonControl					m_JettonControl;					//筹码控件
	CNumberControl					m_NumberControl[GAME_PLAYER];		//数字控件
	CDFontEx						m_DFontEx;							// 字体

	//位置信息
protected:
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptJettons[GAME_PLAYER];			//筹码位置
	CWHArray<tagGoldMes>		m_lDrawGold[GAME_PLAYER];			//绘画数目
	CWHArray<CPoint>			m_ptKeepJeton[GAME_PLAYER];			//金币位置

	//界面变量
protected:
	CBitImage						m_ImageBanker;						//庄家资源
	CBitImage						m_ImageCard;						//扑克资源
	CBitImage						m_ImageTitle;						//标题资源
	CBitImage						m_ImageViewBack;					//背景资源
	CBitImage						m_ImageViewCenter;					//背景资源
	CBitImage						m_ImageJeton;						//筹码资源
	CBitImage						m_ImageArrowhead;					//箭头资源
	CBitImage						m_ImageReady;						//准备资源
	CBitImage						m_ImagePocket;						//金袋资源
	CBitImage						m_ImageNumber;						//数字资源
	CBitImage						m_ImageBalance;						//结算资源

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
	//开始数字滚动
	void BeginMoveNumber();
	//停止数字滚动
	void StopMoveNumber();
	//设置下注
	void SetUserTableScore(WORD wChairID, LONGLONG lTableScore,LONGLONG lCurrentScore);
	//开始筹码动画
	void BeginMoveJettons();
	//停止筹码动画
	void StopMoveJettons();
	//胜利玩家
	void SetGameEndInfo( WORD wWinner );
	//等待选择
	void SetWaitUserChoice(WORD wChoice);
	//庄家标志
	void SetBankerUser(WORD wBankerUse);
	//左上信息
	void SetScoreInfo(LONGLONG lMaxCellScore,LONGLONG lCellScore);
	//选比标志
	void SetCompareCard(bool bCompareCard,BOOL bCompareUser[]);
	//停止更新
	void StopUpdataScore(bool bStop);
	//绘画扑克
	void DrawCompareCard(CDC * pDC, INT nXPos, INT nYPos, bool bChapped);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos,UINT nFormat);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,UINT nFormat=DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	//动画判断
	bool IsDispatchCard(){return m_bCartoon;}
	//获取信息
	bool GetCompareInfo(){return m_bCompareCard;};
	//更新视图
	void RefreshGameView();

	//动画函数
public:
	//比牌动画
	void PerformCompareCard(WORD wCompareUser[2], WORD wLoserUser);
	//发牌动画
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//闪牌动画
	bool bFalshCard(WORD wFalshUser[]);
	//发牌处理
	bool SendCard();
	//闪牌处理
	bool FlashCard();
	//比牌处理
	bool CompareCard();
	//停止闪牌
	void StopFlashCard();
	//停止比牌
	void StopCompareCard();
	//完成发牌
	void FinishDispatchCard();

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
