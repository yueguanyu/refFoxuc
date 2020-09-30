#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "../游戏服务器/GameLogic.h"
//#include "DlgBank.h"
#include "ClientControl.h"

//////////////////////////////////////////////////////////////////////////
//时间标识
#define IDI_FLASH_WINNER			100									//闪动标识
#define IDI_SHOW_CHANGE_BANKER		101									//轮换庄家
#define IDI_DISPATCH_CARD			102									//发牌标识
#define IDI_SHOWDISPATCH_CARD_TIP	103									//发牌提示
#define IDI_OPENCARD	104												//发牌提示
#define IDI_MOVECARD_END			105									//移動牌結束
#define IDI_POSTCARD	106											   //发牌提示
#define IDI_CHECK_TIMER			107

//筹码定义
#define JETTON_COUNT				7									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_PLACE_JETTON			WM_USER+200							//加住信息
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息
#define IDM_CONTINUE_CARD			WM_USER+202							//继续发牌
#define IDM_AUTO_OPEN_CARD			WM_USER+203							//按钮标识
#define IDM_OPEN_CARD				WM_USER+204		
#define IDM_MANAGE_CONTROL			WM_USER+205							//申请信息
#define IDM_OPEN_SOUND			    WM_USER+206							//按钮标识
#define IDM_CLOSE_SOUND				WM_USER+207		
#define IDM_POSTCARD_SOUND               (WM_USER+208)	                    //发牌完成


//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//历史记录
//#ifdef _DEBUG
//#define MAX_SCORE_HISTORY			18									//历史个数
//#else
//#define MAX_SCORE_HISTORY			65									//历史个数
//#endif

#define MAX_SCORE_HISTORY			65									//历史个数

#define MAX_FALG_COUNT				14									//标识个数

//筹码信息
struct tagJettonInfo
{
	int								nXPos;								//筹码位置
	int								nYPos;								//筹码位置
	BYTE							cbJettonIndex;						//筹码索引
};

//操作结果
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};

//记录信息
struct tagClientGameRecord
{
	enOperateResult					enOperateShunMen;					//操作标识
	enOperateResult					enOperateDuiMen;					//操作标识
	enOperateResult					enOperateDaoMen;					//操作标识
	bool							bWinShunMen;						//顺门胜利
	bool							bWinDuiMen;							//对门胜利
	bool							bWinDaoMen;							//倒门胜利
	bool							bWinShunMenSame;						//顺门胜利
	bool							bWinDuiMenSame;							//对门胜利
	bool							bWinDaoMenSame;							//倒门胜利
};

//发牌提示
enum enDispatchCardTip
{
	enDispatchCardTip_NULL,
	enDispatchCardTip_Continue,											//继续发牌
	enDispatchCardTip_Dispatch											//重新洗牌
};

//筹码数组
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//累声明
class CGameClientEngine;
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//限制信息
protected:
	LONGLONG						m_lMeMaxScore;						//最大下注
    LONGLONG						m_lAreaLimitScore;					//区域限制

	//下注信息
protected:
	LONGLONG						m_lUserJettonScore[AREA_ARRY_COUNT];	//个人总注
	LONGLONG						m_lAllJettonScore[AREA_ARRY_COUNT];	//全体总注

	//位置信息
protected:
	int								m_nWinFlagsExcursionX;				//偏移位置
	int								m_nWinFlagsExcursionY;				//偏移位置
	int								m_nScoreHead;						//成绩位置
	CRect							m_rcShunMen;						//闲家区域
	CRect							m_rcJiaoL;							//闲天王区域
	CRect							m_rcQiao;							//对子区域
	CRect							m_rcDuiMen;							//平区域
	CRect							m_rcDaoMen;							//同点平区域
	CRect							m_rcJiaoR;							//庄家区域
	int								m_OpenCardIndex;					//開牌順序
	int								m_PostCardIndex;					//發牌順序
	int								m_PostStartIndex;					//发牌起始位置


	//扑克信息
public:	
    BYTE							m_cbTableCardArray[4][2];			//桌面扑克
	bool							m_blMoveFinish;
	BYTE							m_bcfirstShowCard;
	BYTE							m_bcShowCount;
	bool							m_blAutoOpenCard;					//手自动

	//历史信息
protected:
	LONGLONG						m_lMeStatisticScore;				//游戏成绩
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//游戏记录
	int								m_nRecordFirst;						//开始记录
	int								m_nRecordLast;						//最后记录

	//状态变量
protected:
	WORD							m_wMeChairID;						//我的位置
	BYTE							m_cbAreaFlash;						//胜利玩家
	LONGLONG						m_lCurrentJetton;					//当前筹码
	bool							m_bShowChangeBanker;				//轮换庄家
	bool							m_bNeedSetGameRecord;				//完成设置
	bool							m_bWinShunMen;						//胜利标识
	bool							m_bWinDuiMen;						//胜利标识
	bool							m_bWinDaoMen;						//胜利标识
	bool							m_bFlashResult;						//显示结果
	bool							m_bShowGameResult;					//显示结果
	enDispatchCardTip				m_enDispatchCardTip;				//发牌提示

	//庄家信息
protected:	
	WORD							m_wBankerUser;						//当前庄家
	WORD							m_wBankerTime;						//做庄次数
	LONGLONG						m_lBankerScore;						//庄家积分
	LONGLONG						m_lBankerWinScore;					//庄家成绩	
	LONGLONG						m_lTmpBankerWinScore;				//庄家成绩	
	bool							m_bEnableSysBanker;					//系统做庄

	//当局成绩
public:
	LONGLONG						m_lMeCurGameScore;					//我的成绩
	LONGLONG						m_lMeCurGameReturnScore;			//我的成绩
	LONGLONG						m_lBankerCurGameScore;				//庄家成绩
	LONGLONG						m_lGameRevenue;						//游戏税收

	//数据变量
protected:
	CPoint							m_PointJetton[AREA_COUNT];			//筹码位置
	CPoint							m_PointJettonNumber[AREA_COUNT];	//数字位置
	CJettonInfoArray				m_JettonInfoArray[AREA_COUNT];		//筹码数组

	//控件变量
public:
	CSkinButton						m_btJetton100;						//筹码按钮
	CSkinButton						m_btJetton1000;						//筹码按钮
	CSkinButton						m_btJetton10000;					//筹码按钮
	CSkinButton						m_btJetton100000;					//筹码按钮
	CSkinButton						m_btJetton500000;					//筹码按钮
	CSkinButton						m_btJetton1000000;					//筹码按钮
	CSkinButton						m_btJetton5000000;					//筹码按钮
	CSkinButton						m_btJetton10000000;					//筹码按钮
	CSkinButton						m_btApplyBanker;					//申请庄家
	CSkinButton						m_btCancelBanker;					//取消庄家
	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩
	CSkinButton						m_btAutoOpenCard;					//自动开牌
	CSkinButton						m_btOpenCard;						//手动开牌
	CSkinButton						m_btBank;							//银行按钮
	CSkinButton						m_btContinueCard;					//继续发牌
	CSkinButton				        m_btBankerStorage;					//存款按钮
	CSkinButton						m_btBankerDraw;						//取款按钮
	CSkinButton						m_btSoundOpen;	
	CSkinButton						m_btSoundClose;	

	CSkinButton						m_btUp;					            //存款按钮
	CSkinButton						m_btDown;						    //取款按钮	
	CButton							m_btOpenAdmin;                      //管理员控制

	CBitImageEx						m_btImageJetton1000;					//筹码按钮
	CBitImageEx						m_btImageJetton10000;					//筹码按钮
	CBitImageEx						m_btImageJetton100000;					//筹码按钮
	CBitImageEx						m_btImageJetton500000;					//筹码按钮
	CBitImageEx						m_btImageJetton1000000;					//筹码按钮
	CBitImageEx						m_btImageJetton5000000;					//筹码按钮
	CBitImageEx						m_btImageJetton10000000;				//筹码按钮



	//控件变量
public:
	HINSTANCE						m_hInst;
	IClientControlDlg			   *m_pAdminControl;						//控制
	CApplyUser						m_ApplyUser;						//申请列表
	CGameRecord						m_GameRecord;						//记录列表
	CCardControl					m_CardControl[5];					//扑克控件	
	CGameClientEngine					*m_pGameClientDlg;					//父类指针
	CGameLogic						m_GameLogic;						//游戏逻辑
	//CDlgBank						m_DlgBank;							//银行控件
	//CAdminControl				    m_AdminDlg;							//管理员控制
	

	//界面变量
protected:
	CBitImageEx						m_ImageViewFill;					//背景位图
	CBitImageEx						m_ImageViewBack;					//背景位图
	CBitImageEx						m_ImageWinFlags;					//标志位图
	CBitImageEx						m_ImageJettonView;					//筹码视图
	CBitImageEx						m_ImageScoreNumber;					//数字视图
	CBitImageEx						m_ImageMeScoreNumber;				//数字视图
	CBitImageEx						m_ImageTimeFlag;					//时间标识
	CBitImageEx						m_ImageDispatchCardTip;				//发牌提示

	struct TotoalPress
	{
		CPngImage n_Png_tatoalflag;
		CPngImage n_Png_tipC;
		CPngImage n_Png_tipN;
		
	};


	CPngImage						m_PngScoreNumber;				//数字视图

	TotoalPress                     m_TotoalPress;               

	//边框资源
protected:
	CBitImageEx						m_ImageFrameShunMen;				//边框图片
	CBitImageEx						m_ImageFrameJiaoL;					//边框图片
	CBitImageEx						m_ImageFrameQiao;					//边框图片
	CBitImageEx						m_ImageFrameDuiMen;					//边框图片
	CBitImageEx						m_ImageFrameDaoMen;					//边框图片
	CBitImageEx						m_ImageFrameJiaoR;					//边框图片

	CBitImageEx						m_ImageMeBanker;					//切换庄家
	CBitImageEx						m_ImageChangeBanker;				//切换庄家
	CBitImageEx						m_ImageNoBanker;					//切换庄家

	CPngImage					    m_ImageTimer;					//切换庄家
	CBitImageEx                      m_ImageTimerNumber;

	//结束资源
protected:
	CBitImageEx						m_ImageGameEnd;						//成绩图片

	//银行
public:
	BOOL							m_blCanStore;                       //是否能保存

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
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }

	//设置函数
public:
	//设置信息
	void SetMeMaxScore(LONGLONG lMeMaxScore);
	//我的位置
	void SetMeChairID(DWORD dwMeUserID);
	//个人下注
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);
	//庄家信息
	void SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore);
	//庄家成绩
	void SetBankerScore(WORD wBankerTime, LONGLONG lWinScore) {m_wBankerTime=wBankerTime; m_lTmpBankerWinScore=lWinScore;}
	//当局成绩
	void SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue);
	//设置筹码
	void PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount);
	//区域限制
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);	
	//设置扑克
	void SetCardInfo(BYTE cbTableCardArray[4][2]);
	//设置筹码
	void SetCurrentJetton(LONGLONG lCurrentJetton);
	//历史记录
	void SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//成绩设置
	void SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore);
	//允许系统做庄
	void EnableSysBanker(bool bEnableSysBanker) {m_bEnableSysBanker=bEnableSysBanker;}
	//设置提示
	void SetDispatchCardTip(enDispatchCardTip DispatchCardTip);

	void SetFirstShowCard(BYTE bcCard);

	//更新视图
	void RefreshGameView();
	
	void ClearBank();
	//
	void SetBankerAllResult(LONGLONG lAllWinScore){m_lBankerWinScore = lAllWinScore;}

	//动画函数
public:	
	//开始发牌
	void DispatchCard();
	//结束发牌
	void FinishDispatchCard();
	//设置爆炸
	bool SetBombEffect(bool bBombEffect, WORD wAreaIndex);

	//计算限制
public:
	//最大下注
	LONGLONG GetUserMaxJetton();

	//绘画函数
protected:
	//闪烁边框
	void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置胜方
	void SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);
	//绘画庄家
	void DrawBankerInfo(CDC *pDC,int nWidth,int nHeight);
	//绘画玩家
	void DrawMeInfo(CDC *pDC,int nWidth,int nHeight);

	//内联函数
public:
	//当前筹码
	inline LONGLONG GetCurrentJetton() { return m_lCurrentJetton; }	
	
	void SetMoveCardTimer();
	//接收场景时结束发牌
	void SetFinishDiscard();

	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false,bool blTimer=false);
	//绘画数字
	void DrawNumberStringWithPng(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat=-1);
	//绘画标识
	void DrawWinFlags(CDC * pDC);
	//显示结果
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//透明绘画
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);
	//推断赢家
	void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);

	void DrawJettonPress(CDC* pDC,int iWidth,int iHeight,LONGLONG lTotal,LONGLONG lHaveJetton);

	void DrawGameTimer(CDC* pDC,int iWidth,int iHeight,int iTimer);

	LONGLONG GetBankMaxJetton();

	//按钮消息
protected:
	//上庄按钮
	afx_msg void OnApplyBanker();
	//下庄按钮
	afx_msg void OnCancelBanker();
	//移动按钮
	afx_msg void OnScoreMoveL();
	//移动按钮
	afx_msg void OnScoreMoveR();
	//手工搓牌
	afx_msg void OnOpenCard();
	//自动搓牌
	afx_msg void OnAutoOpenCard();

	afx_msg void OnOpenSound();

	afx_msg void OnCloseSound();
	//银行存款
	afx_msg void OnBankStorage();
	//银行取款
	afx_msg void OnBankDraw();
	//上翻
	afx_msg void OnUp();
	//下翻
	afx_msg void OnDown();
	//管理员控制
	afx_msg void OpenAdminWnd();
	//输赢控制
	afx_msg void OnControl();

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
    //鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//控件命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//其他控件鼠标UP消息
	LRESULT OnViLBtUp(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()	

};
#endif