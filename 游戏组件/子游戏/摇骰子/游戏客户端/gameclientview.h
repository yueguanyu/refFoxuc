#pragma once

#include "Stdafx.h"
#include "ScoreView.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//消息定义
#define IDM_START					(WM_APP+100)						//开始消息
#define IDM_SETCHIP					(WM_APP+101)						//压注消息
#define IDM_THROW_DICE				(WM_APP+102)						//摇骰消息
#define IDM_LOOK_DICE				(WM_APP+103)						//看骰消息
#define IDM_SCROLL					(WM_APP+104)						//传递骰数	
#define IDM_DICE_NUM				(WM_APP+105)						//传递点数
#define IDM_YELL_DICE				(WM_APP+106)						//喊话消息
#define IDM_OPEN_DICE				(WM_APP+107)						//开骰消息
////////////////////////////////////////////////////////////////////////////////////////////////

#define SET_CHIP_COUNT				4									//下注档次	

//动画动作
enum enAnimalAction
{
	enTABegin = 1,	//动画开始
	enTAPlay,		//动画进行中
	enTAEnd			//动画结束
};

//按钮操作
enum enUpdateButton
{
	enUBClickBegin = 1,    //点击开始
	enUBChip,			   //下注
	enUBThrow,			   //摇骰
	enUBYellFirst,		   //首轮喊话
	enUBYell,			   //喊话
	enUBOpen,			   //开骰
	enUBEnd,			   //结束
	enTest				   //测试
};

//按钮结构体
struct tagButtonState
{	// 0 隐藏 不可用 1 隐藏 可用 2 显示 不可用 3 显示 可用
	BYTE bStart;			//开始按钮
	BYTE bSetChip;			//下注按钮
	BYTE bThrowDice;		//摇骰按钮
	BYTE bLookDice;			//看骰按钮
	BYTE bYellOK;			//喊话按钮
	BYTE bDiceScroll;		//翻页按钮
	BYTE bDiceNum;			//骰点按钮
	BYTE bOpenDice;			//开骰按钮
	void Set(BYTE bst, BYTE bsc, BYTE btd, BYTE bld, BYTE byo, BYTE bds, BYTE bdn, BYTE bod)
	{bStart = bst;bSetChip = bsc;bThrowDice = btd;bLookDice = bld;bYellOK = byo;bDiceScroll = bds;bDiceNum = bdn;bOpenDice = bod;}
};

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//界面变量
protected:
	BYTE							m_bDiceData[GAME_PLAYER][5];		//骰子数组
	bool							m_bShowDice[GAME_PLAYER];			//显示骰子
public:
	tagDiceYell						m_MaxYell;							//最大喊话
	bool							m_bShowDiceCup[GAME_PLAYER];		//显示骰盅 
	BYTE							m_bDiceCount;						//骰子数目 (喊话面板上当前选择的数目)
	BYTE							m_bNowDicePoint;					//当前骰点 (喊话面板上选择的骰点)
	int								m_nAnimalTime[GAME_PLAYER];			//动画时间
	OpenDice_Scene					m_SceneOpenDice;					//开骰场景
	LONGLONG						m_lChip;							//下注额度

	//位置变量
private:
	POINT							m_ptDiceCup[GAME_PLAYER];			//玩家骰盅
	POINT							m_ptHandDice[GAME_PLAYER];			//手中骰子
	POINT							m_ptYellPanel;						//喊话框位
	POINT							m_ptWordOpen[GAME_PLAYER];			//开骰文字
	POINT							m_ptMaxYell;						//最大喊话
	POINT							m_ptChip[GAME_PLAYER];				//下注筹码

	INT								m_nXFace;
	INT								m_nYFace;
	INT								m_nXTimer;
	INT								m_nYTimer;

	//控件变量
	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btSetChip[4];						//下注按钮
	CSkinButton						m_btThrowDice;						//摇骰按钮
	CSkinButton						m_btLookDice;						//看骰按钮
	CSkinButton						m_btYellOK;							//喊话按钮
	CSkinButton						m_btDiceScroll[2];					//骰数按钮 (上下翻的2个箭头)
	CSkinButton						m_btDiceNum[6];						//骰点按钮	
	CSkinButton						m_btOpenDice;						//开骰按钮

	//位图变量
protected:
	CBitImage						m_ImageBack;						//背景资源
	CBitImage						m_ImageCenter;						//背景资源
	CBitImage						m_ImageChipBack;					//下注背景
	CBitImage						m_ImageSezi;						//骰子资源	
	CBitImage						m_ImageThrowDiceAnimal[GAME_PLAYER];//摇骰动画
	CBitImage						m_ImageOpenDiceAnimal[GAME_PLAYER]; //开盖动画
			
	CPngImage						m_PngImageUserFrame;				//用户信息
	CPngImage						m_PngImageYellFrame;				//喊话背景	
	CPngImage						m_PngImageJetton;					//筹码资源
	CPngImage						m_PngImageWord;						//文字资源
	CPngImage						m_PngImageJettonSmall;				//筹码资源
	CPngImage						m_PngImageNum1;						//喊话数字
	CPngImage						m_PngImageNum2;						//选择数字
	CPngImage						m_PngImageDiceCup;					//骰盅资源
	CPngImage						m_PngImageWordSucc;					//开中对手
	CPngImage						m_PngImageWordFail;					//没有开中
	CPngImage						m_PngImageSeziSmall;				//小型骰子

	CPngImage						m_PngOpenDice0;						//开盖动画
	CPngImage						m_PngOpenDice1;						//开盖动画

	CPngImage						m_PngOpenDiceTop[29];				//开盖动画
	CPngImage						m_PngOpenDiceBottom[29];			//开盖动画

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//摇骰动画
	void DrawThrowDice(CDC *pDC, WORD wViewChair, int nTime, int nX, int nY);
	//绘画骰盅
	void DrawDiceCup(CDC * pDC);
	//绘画下注
	void DrawChip(CDC *pDC);
	//绘画骰子
	void DrawSezi(CDC *pDC);
	//绘画喊话
	inline void DrawYell(CDC *pDC, BYTE bDiceCount,  BYTE bDicePoint, int nX, int nY);
	//最大喊话
	void DrawMaxYell(CDC *pDC, int nWidth, int nHeight);
	//喊话面板
	void DrawYellPanel(CDC *pDC);
	//开骰场景
	void DrawOpenDice(CDC *pDC, int nX, int nY);
	//绘画数字
	void DrawPicNum(CDC *pDC, CPngImage *pImage, BYTE bNum, int nX, int nY);

	//功能函数
public:
	//设置骰子
	void SetHandDice(WORD wChairID, bool bShow, BYTE bDiceData[]);
	//摇骰动画
	void OnThrowAnimal(WORD wChairID, enAnimalAction enAction);
	//更新按钮
	void UpdateButton(enUpdateButton nType);
	//更新视图
	void RefreshGameView();

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

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//开始按钮
	afx_msg void OnStart();
	//设置压注
	afx_msg void OnSetChip(UINT nCtrlID);
	//摇骰按钮
	afx_msg void OnThrowDice();
	//看骰按钮
	afx_msg void OnLookDice();
	//上翻按钮
	afx_msg void OnScrollUp();
	//下翻按钮
	afx_msg void OnScrollDown();
	//骰点按钮
	afx_msg void OnDiceNum(UINT nCtrlID);
	//喊话按钮
	afx_msg void OnYellOk();
	//开骰按钮
	afx_msg void OnOpenDice();
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
