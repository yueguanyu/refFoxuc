#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
//#include "GameLogic.h"


//////////////////////////////////////////////////////////////////////////

//筹码定义
#define JETTON_COUNT				9									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//筹码信息
struct tagJettonInfo
{
	int								nXPos;								//筹码位置
	int								nYPos;								//筹码位置
	BYTE							cbJettonIndex;						//筹码索引
};

//筹码数组
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//类声明
class CGameClientDlg;
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//限制信息
protected:
	__int64							m_iMeMaxScore;							//最大下注
	__int64							m_iMeAreaScore[JETTON_AREA_COUNT];		//玩家下注					
	__int64							m_iAllAreaScore[JETTON_AREA_COUNT];		//全体下注
	__int64							m_iAreaScoreLimit[JETTON_AREA_COUNT];	//各区域当前可下分
	CRect							m_rcJettonArea[JETTON_AREA_COUNT];		//各下注区域
	CRect							m_rcScoreLimit[JETTON_AREA_COUNT];		//各区域最大可下分书写点
	CRect							m_rcApplayBankerNo[4];					//申请上庄玩家列表
	CRect							m_rcApplayBankerName[4];				//申请上庄玩家名字
	CRect							m_rcApplayBankerScore[4];				//申请上庄玩家金币
	CRect							m_rcHistoryRecord[MAX_SHOW_HISTORY];
	int								m_nCurCartoonIndex;						//当前应该绘制哪个卡通图案
	int								m_CartoonTimeElapseArray[72];//96			//控制卡通旋转的时间间隔
	int								m_CartoonElapsIndex;					//卡通定时器数组下标
	BOOL							m_bCanShowResult;						//是否显示结算框
	BYTE							m_cbGoalAnimal;							//每次旋转停止的位置

	//位置信息
protected:
	int								m_nScoreHead;							//成绩位置


	bool							m_bisLButtonDown;						//是否鼠标左键按下

	BYTE							m_bEndCount;							//游戏结束记数


	//动画变量
protected:
	bool							m_bBombEffect[JETTON_AREA_COUNT];		//爆炸效果
	BYTE							m_cbBombFrameIndex[JETTON_AREA_COUNT];	//帧数索引

	//历史信息
protected:
	__int64							m_iMeStatisticScore;					//游戏成绩
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录

	//状态变量
protected:
	WORD							m_wMeChairID;						//我的位置
	BYTE							m_cbWinnerSide;						//胜利玩家
	BYTE							m_cbAreaFlash;						//胜利玩家
	__int64							m_iCurrentJetton;					//当前筹码
	CString							m_strDispatchCardTips;				//发牌提示
	bool							m_bShowChangeBanker;				//轮换庄家
	bool							m_bNeedSetGameRecord;				//完成设置

	//庄家信息
protected:	
	WORD							m_wBankerUser;						//当前庄家
	WORD							m_wBankerTime;						//做庄次数
	__int64							m_iBankerScore;						//庄家积分
	__int64							m_iBankerWinScore;					//庄家成绩	
	bool							m_bEnableSysBanker;					//系统做庄
	CFont							m_fontScoreLimit;					//专门绘制可下分的画笔

	//当局成绩
public:
	__int64							m_iMeCurGameScore;					//我的成绩
	__int64							m_iMeCurGameReturnScore;			//我的成绩(返还下注分)
	__int64							m_iBankerCurGameScore;				//庄家成绩
	__int64							m_iGameRevenue;						//游戏税收

	//数据变量
protected:
	CPoint							m_PointJetton[JETTON_AREA_COUNT];					//筹码位置
	CPoint							m_PointJettonNumber[JETTON_AREA_COUNT];				//数字位置
	CJettonInfoArray				m_JettonInfoArray[JETTON_AREA_COUNT];				//筹码数组
	CPoint							m_PtCardArray[24];					//旋转路径上各区域的中心点

	//控件变量
public:
	
	CSkinButton						m_btApplyBanker;					//申请庄家
	CSkinButton						m_btCancelBanker;					//取消庄家

	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩
	CSkinButton						m_btBankerMoveUp;					//向上滚动申请庄家列表
	CSkinButton						m_btBankerMoveDown;					//向下滚动申请庄家列表
	CSkinButton						m_btBankButton;						//银行按钮

	CSkinButton						m_btAddStep1;						//1倍投注
	CSkinButton						m_btAddStep10;						//10倍投注
	CSkinButton						m_btAddStep100;						//100倍投注
	CSkinButton						m_btAddStep1_H;						//1倍投注
	CSkinButton						m_btAddStep10_H;					//10倍投注
	CSkinButton						m_btAddStep100_H;					//100倍投注

	CSkinButton						m_btBetLast;						//续押按钮
	CSkinButton						m_btBetSmall;						//押小按钮
	CSkinButton						m_btBetBig;							//押大按钮


	//控件变量
public:
	CApplyUser						m_ApplyUser;						//申请列表
	CGameRecord						m_GameRecord;						//记录列表	
	CGameClientDlg					*m_pGameClientDlg;					//父类指针

	//界面变量
protected:
	CBitImage						m_ImageViewFill;					//背景位图
	CBitImage						m_ImageViewBack;					//背景位图
	CBitImage						m_ImageJettonView;					//筹码视图
	CBitImage						m_ImageScoreNumber;					//数字视图
	CBitImage						m_ImageMeScoreNumber;				//数字视图
	CBitImage						m_ImageTimeFlag;					//时间标识
	CBitImage						m_ImageHistoryRecord;				//历史记录图片
	CPngImage						m_ImageBombEffect;					//动画图片
	CPngImage						m_ImageCardCartoon;					//旋转动画

	CPngImage						m_ImageApple;						//苹果动画
	CPngImage						m_ImageApple2;						//苹果X2动画
	CPngImage						m_IamgeBar;							//BAR动画
	CPngImage						m_IamgeBar2;						//BARX2动画
	CPngImage						m_IamgeBell;						//铃声动画
	CPngImage						m_IamgeBell2;						//铃声X2动画
	CPngImage						m_IamgeSeven;						//SEVEN
	CPngImage						m_IamgeSeven2;						//SEVENX2
	CPngImage						m_IamgeStar;						//STAR
	CPngImage						m_IamgeStar2;						//STARX2
	CPngImage						m_IamgeOrange;						//ORANGE
	CPngImage						m_IamgeOrange2;						//ORANGEX2
	CPngImage						m_IamgePaw;							//PAW
	CPngImage						m_IamgePaw2;						//PAWX2
	CPngImage						m_ImageWaterMelon;					//WATERMELON
	CPngImage						m_ImageWaterMelon2;					//WATERMELONX2

	CPngImage						m_ImageBetNumber;					//数字视图
	CPngImage						m_ImageBetBigNumber;				//时间视图

	CPngImage						m_ImageFreeTime;					//空闲时间
	CPngImage						m_ImageBitSmallTime;				//押大小时间
	CPngImage						m_ImageJettonTime;					//下注时间



	CBitImage						m_ImageKeyPress;					//按下加注按钮

	//边框资源
protected:
	CBitImage						m_ImageWinFrame;					//下注买中边框

	CBitImage						m_ImageMeBanker;					//切换庄家
	CBitImage						m_ImageChangeBanker;				//切换庄家
	CBitImage						m_ImageNoBanker;					//切换庄家

	//结束资源
protected:
	CBitImage						m_ImageGameEnd;						//成绩图片

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//设置函数
public:
	//设置信息
	void SetMeMaxScore(__int64 iMeMaxScore);
	//我的位置
	inline void SetMeChairID(WORD wMeChairID) { m_wMeChairID=wMeChairID;}
	//个人下注
	void SetMePlaceJetton(BYTE cbViewIndex, __int64 iJettonCount);
	//庄家信息
	void SetBankerInfo(WORD wBankerUser, __int64 iBankerScore);
	//庄家成绩
	void SetBankerScore(WORD wBankerTime, __int64 iWinScore) {m_wBankerTime=wBankerTime; m_iBankerWinScore=iWinScore;}
	//当局成绩
	void SetCurGameScore(__int64 iMeCurGameScore, __int64 iMeCurGameReturnScore, __int64 iBankerCurGameScore, __int64 iGameRevenue);
	//设置筹码
	void PlaceUserJetton(BYTE cbViewIndex, __int64 iScoreCount);
	//区域限制
	void SetAreaLimitScore(__int64 * pLimitArray, int ArraySize);	

	//设置筹码
	//void SetCurrentJetton(__int64 iCurrentJetton);
	//历史记录
	void SetGameHistory(tagGameRecord * pGameRecord, int nRecordCount);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//成绩设置
	void SetGameScore(__int64 iMeCurGameScore, __int64 iMeCurGameReturnScore, __int64 iBankerCurGameScore);
	//允许系统做庄
	void EnableSysBanker(bool bEnableSysBanker) {m_bEnableSysBanker=bEnableSysBanker;}

	void CreateApplayBankerListRect(int nBaseX, int nBaseY);
	void CreateHistoryRecordRect(int nWidth, int nHeight);
	void UpdateCartoonTimeElapse(BYTE cbStopStep);	
	void CreateTurnPathAreaBasePoint(int nWidth, int nHeight);

	//动画函数
public:	
	//设置爆炸
	bool SetBombEffect(bool bBombEffect, WORD wAreaIndex);
	//绘画爆炸
	void DrawBombEffect(CDC *pDC);

	//绘制申请上庄玩家列表
	void DrawApplyBankerUserList(CDC * pDC);

	//绘制历史记录
	void DrawHistoryRecord(CDC * pDC);

	//绘制时间提示
	void DrawTimeTip(CDC * pDC, int nWidth, int nHeight);

	//绘制区域限注
	void DrawAreaLimitScore(CDC * pDC);

	//绘制筹码
	void DrawAreaJetton(CDC * pDC);

	//绘制庄家信息
	void DrawBankerInfo(CDC * pDC, int nWidth, int nHeight);

	//绘制闲家信息
	void DrawUserInfo(CDC * pDC, int nWidth, int nHeight);

	//切换庄家
	void DrawChangeBanker(CDC * pDC, int nWidth, int nHeight);

	//绘制下注按钮
	void DrawAddMoney(CDC * pDC, int nWidth, int nHeight);

	//绘制时间
	virtual void DrawUserTimer(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea);

	//计算限制
protected:

	//获取当前玩家还能下注的数额
	__int64 GetUserCurrentLeaveScore();
	
	//绘画函数
protected:
	//闪烁边框
	void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

	//开始旋转
	void PlayingCartoonEffect();
	
	//结束旋转
	void EndCartoonEffect();


	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置胜方
	void SetWinnerSide(BYTE cbWinnerSide);

	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);

	//内联函数
public:
	//当前筹码
	inline __int64 GetCurrentJetton() { return m_iCurrentJetton; }	

	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, __int64 lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, __int64 iNumber, INT nXPos, INT nYPos);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, __int64 iNumber, CRect rcRect, INT nFormat=-1);
	//显示结果
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//绘制我的分数
	void DrawMeScore(CDC *pDC, int nWidth, int nHeight);
	//绘制当前成绩
	void DrawCurrentScore(CDC *pDC, int nWidth, int nHeight);
	//透明绘画
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);

	//绘制动画
	void DrawCardCartoon(CDC * pDC);

	//绘制结束动画
	void DrawEndCartoon(CDC * pDC,int nWidth,int nHeight,BYTE bGoalAnimal);

	//按钮消息
protected:
	//倍投按钮
	afx_msg void OnAddStep1();
	afx_msg void OnAddStep10();
	afx_msg void OnAddStep100();
	afx_msg void OnAddStep1_H();
	afx_msg void OnAddStep10_H();
	afx_msg void OnAddStep100_H(); 

	//上庄按钮
	afx_msg void OnApplyBanker();
	//下庄按钮
	afx_msg void OnCancelBanker();
	//移动按钮
	afx_msg void OnScoreMoveL();
	//移动按钮
	afx_msg void OnScoreMoveR();

	afx_msg void OnBankerListMoveUp();
	afx_msg void OnBankerListMoveDown();

	afx_msg void OnClickedBank();

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
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
