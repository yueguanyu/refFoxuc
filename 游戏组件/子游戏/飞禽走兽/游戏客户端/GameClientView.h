#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "..\游戏服务器\GameLogic.h"
#include "ClientControl.h"

//////////////////////////////////////////////////////////////////////////

//筹码定义
#define JETTON_COUNT				6									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_PLACE_JETTON			WM_USER+200							//加住信息
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息		
#define IDM_SOUND					WM_USER+206		
#define IDM_ADMIN_COMMDN			WM_USER+207


//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//历史记录
#define MAX_SCORE_HISTORY			65									//历史个数
#define MAX_FALG_COUNT				12									//标识个数
#define COUNT_DRAWINFO              18

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
	enOperateResult					enOperateMen[AREA_COUNT+1];			//操作标识
	bool							bWinMen[AREA_COUNT+1];				//顺门胜利

};

struct tagAndroidBet
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	WORD							wChairID;							//玩家位置
	int								nLeftTime;							//剩余时间 (100ms为单位)
};

//筹码数组
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//累声明
class CGameClientEngine;

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//限制信息
protected:
	LONGLONG						m_lMeMaxScore;						//最大下注
    LONGLONG						m_lAreaLimitScore;					//区域限制
	LONGLONG						m_lRobotMaxJetton;					//最大筹码

	//下注信息
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1];	//个人总注
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];	//全体总注
	LONGLONG						m_lUserAddScore[AREA_COUNT];		//加注信息
	
	//位置信息
protected:
	int								m_nWinFlagsExcursionX;				//偏移位置
	int								m_nWinFlagsExcursionY;				//偏移位置
	int								m_nScoreHead;						//成绩位置
	CPoint							m_PointJetton[AREA_COUNT];			//筹码位置
	CPoint							m_PointAnimalRand[AREA_ALL];		//动物几率
	CPoint                          m_PointAddButton[AREA_COUNT];		//加注位置
	CPoint                          m_PointJettonScore[AREA_COUNT];		//数字位置
	CPoint                          m_LuZiStart;						//路子信息
	int								m_TopHeight;						//位置信息
	int								m_LifeWidth;						//位置信息
	CRect							m_MeInfoRect;						//位置信息						
	CRect							m_CarRect[ANIMAL_COUNT];			//位置信息	

	//扑克信息
public:	
    BYTE							m_cbTableCardArray[2];				//桌面扑克
	
				
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
	bool							m_bFlashResult;						//显示结果
	bool							m_bShowGameResult;					//显示结果
	bool                            m_bShowShaYuResult;					//鲨鱼结算
	bool                            m_bShowAnotherResult;				//其它结算
	int                             m_nPlayGameTimes;					//已完盘数
	bool							m_DrawBack;							//绘画背景
	

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
	bool							m_blMoveFinish;						//移动结束
	//数据变量
protected:
	
	bool							m_bWinFlag[AREA_COUNT];				//胜利标识
	INT								m_nAnimalPercent[AREA_ALL];			//区域几率
	int								m_CarIndex ;						//当前index
	int                             m_ShaYuIndex;                       //鲨鱼index
	BYTE							m_cbShaYuAddMulti;                  //鲨鱼奖励
	BYTE							m_cbOneDigit;						//奖励个位
	BYTE							m_cbTwoDigit;						//奖励十位
	BYTE							m_cbRandOneDigit;					//奖励个位
	BYTE							m_cbRandTwoDigit;					//奖励十位

	BYTE                            m_cbRandAddMulti;                   //当前变量
	BYTE                            m_cbAnimalIndex;					//动画索引
	bool                            m_bShowCartoon;						//显示卡通
	
	CWHArray<tagAndroidBet>			m_ArrayAndroid;						//机器下注

	//移动随机数
	BYTE							m_nNumberOpenSide;
	BYTE							m_nCurRunIndex1;
	BYTE							m_NumberIndex1;
	bool							m_blRungingNumber1;

	BYTE							m_nCurRunIndex2;
	BYTE							m_NumberIndex2;
	bool							m_blRungingNumber2;

	//控件变量
public:
	HINSTANCE						m_hInst;
	IClientControlDlg			    *m_pAdminControl;					//控制控件
	CGameRecord						m_GameRecord;						//记录列表
	CGameClientEngine				*m_pGameClientDlg;					//父类指针
	CGameLogic						m_GameLogic;						//游戏逻辑
	
	//银行
public:
	bool							m_blCanStore;                       //是否保存

	//结束控制
protected:
	bool							m_bFlashrandShow;					//随机亮灯
	bool							m_blRungingCar;						//是否移动
	int								m_nTimerStep;						//移动步值
	int								m_nCarOpenSide;						//开中动物
	int								m_nCurRunIndex;						//当前索引
	int								m_nTotoalRun;						//移动圈数
	CRect							m_RectArea[8];						//区域位置
	int                             m_nAllRunTimes;

	//按钮变量
public:
	CSkinButton						m_btJetton10;						//筹码按钮
	CSkinButton						m_btJetton100;						//筹码按钮
	CSkinButton						m_btJetton1000;						//筹码按钮
	CSkinButton						m_btJetton10000;					//筹码按钮
	CSkinButton						m_btJetton100000;					//筹码按钮
	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩
	CSkinButton						m_btBankStorage;					//存款按钮
	CSkinButton						m_btBankDraw;						//取款按钮
	CButton							m_btOpenAdmin;						//系统控制
	CSkinButton						m_btAdd[AREA_COUNT];				//增加按钮
	
	//界面变量
protected:
	CBitImage						m_ImageViewFill;					//背景位图
	CBitImage						m_ImageScoreNumber;					//数字视图
	CBitImage						m_ImageMeScoreNumber;				//数字视图
	CPngImage						m_ImageTimeFlagPng;					//时间标识
	CPngImage						m_ViewBackPng;						//背景图片
	CPngImage						m_idb_selPng;						//图标亮灯
	CPngImage						m_ImageWinFlags;					//标志位图
	CPngImage                       m_pngTimeBack;						//图片信息
	CPngImage                       m_pngResultFShayu;					//图片信息
	CPngImage                       m_pngResultFOther;					//图片信息
	CPngImage                       m_pngResultAnimal;					//图片信息
	CPngImage                       m_pngNumberASymbol;					//图片信息
	CPngImage                       m_pngLuZiR;							//图片信息
	CPngImage                       m_pngLuZiM;							//图片信息
	CPngImage                       m_pngLuZiL;							//图片信息
	CPngImage                       m_pngLuZiAnimal;					//图片信息
	CPngImage                       m_pngCoinNumber;					//图片信息
	CPngImage                       m_pngCarton[AREA_ALL];				//图片信息
	CPngImage                       m_pngAnimalLight;					//图片信息
	CPngImage                       m_pngTimeNumber;					//图片信息
	
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
	//取款操作
	void OnBankDraw();
	//存款操作
	void OnBankStorage();
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
	//机器人下注
	void AndroidBet(BYTE cbViewIndex, LONGLONG lScoreCount);
	//区域限制
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);	
	//设置扑克
	void SetCardInfo(BYTE cbTableCardArray[2]);
	//历史记录
	void SetGameHistory(BYTE *bcResulte);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//成绩设置
	void SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore);
	//允许系统做庄
	void EnableSysBanker(bool bEnableSysBanker) {m_bEnableSysBanker=bEnableSysBanker;}
		
	//执行剩余所有的缓冲动画
	void PerformAllBetAnimation();
	//设置机器人最大筹码
	void SetRobotMaxJetton(LONGLONG lRobotMaxJetton){ m_lRobotMaxJetton = lRobotMaxJetton; };
	//设置鲨鱼奖励
	void SetShaYuAddMulti(BYTE cbShaYuAddMulti);//{ m_cbShaYuAddMulti = cbShaYuAddMulti; };  
	//更新视图
	void RefreshGameView();
	//设置几率
	void SetAnimalPercent( INT nAnimalPercent[AREA_ALL] );
	//设置移动
	void StartRunCar(int m_nTimerStep);
	//另外移动
	void StartRunAnotherCar(int iTimer);
	//进行移动
	void RuningCar(int m_nTimerStep);
	//另外移动
	void RuningAnotherCar(int iTimer);
	//随机亮灯
	void StartRandShowSide();
		
	//动画函数
public:	
	//开始发牌
	void DispatchCard();
	//结束发牌
	void FinishDispatchCard( bool bRecord = true );
	
	//计算限制
public:
	//最大下注
	LONGLONG GetUserMaxJetton(BYTE cbJettonArea = 0);
	//区域倍率
	void  GetAllWinArea(BYTE bcWinArea[],BYTE bcAreaCount,BYTE InArea);
	//降低筹码
	void ReduceJettonNumber();
	//绘画函数
protected:
	//闪烁边框
	void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置胜方
	void SetWinnerSide(bool blWin[],bool bSet);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);
	//绘画庄家
	void DrawBankerInfo(CDC *pDC,int nWidth,int nHeight);
	//绘画玩家
	void DrawMeInfo(CDC *pDC,int nWidth,int nHeight);
	//取消闪动
	void ClearAreaFlash() { m_cbAreaFlash = 0xFF; }

	//内联函数
public:
	//当前筹码
	inline LONGLONG GetCurrentJetton() { return m_lCurrentJetton; }	

	void KillCardTime();

	void SwitchToCheck();

	void SwithToNormalView();

	void StartHandle_Leave();

	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos,bool blTimer= false,bool bMeScore = false);
	//绘画数字
	void DrawNumStrWithSpace(CDC * pDC, LONGLONG lNumber,CRect&rcPrint,INT nFormat=-1);
	//绘画数字
	void DrawNumStrWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//绘画数字
	void DrawSymbolNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos,int nHalf=0);
	//绘画数字
	void DrawPercentNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//图片数字
	int DrawPicNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos,enXCollocateMode xMode=enXCenter);
	//绘画标识
	void DrawWinFlags(CDC * pDC);
	//显示结果
	void DrawShaYuResult(CDC *pDC, int nWidth, int nHeight);
	//显示结果
	void DrawAnotherResult(CDC *pDC, int nWidth, int nHeight);
	//获取描述
	CString GetAnimalStrInfo(BYTE cbAllIndex);
	
	//推断赢家
	void DeduceWinner(bool bWinMen[]);
	//增加下注
	void ButtonAddScore(BYTE cbJettonArea);
	//提交下注
	void OnPlaceJetton();
	//获取索引
	BYTE GetAnimalInfo(BYTE cbAllIndex);
	//获取索引
	BYTE GetCartoonIndex(BYTE cbAllIndex);
	//获取赔率
	BYTE GetAnimalMulti(BYTE cbAllIndex);
	//重置状态
	void ResetButtonState();
	

	void StartRunNumber1(int iTimer);
	void StartRunNumber2(int iTimer);
	void RuningNumber1(int iTimer);
	void RuningNumber2(int iTimer);

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

	afx_msg void OnUp();

	afx_msg void OnDown();

	afx_msg void OnAddScore0();
	afx_msg void OnAddScore1();
	afx_msg void OnAddScore2();
	afx_msg void OnAddScore3();
	afx_msg void OnAddScore4();
	afx_msg void OnAddScore5();
	afx_msg void OnAddScore6();
	afx_msg void OnAddScore7();
	afx_msg void OnAddScore8();
	afx_msg void OnAddScore9();
	afx_msg void OnAddScore10();


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
	//管理员控制
	afx_msg void OpenAdminWnd();

	DECLARE_MESSAGE_MAP()	

};

//////////////////////////////////////////////////////////////////////////

#endif
