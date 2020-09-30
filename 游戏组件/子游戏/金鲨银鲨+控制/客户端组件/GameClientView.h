#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ObjectMove.h"
#include "D3DTextureIndex.h"
#include "GameLogic.h"
#include "WindowChip.h"
#include "WindowOver.h"
#include "FishDraw.h"
#include "WindowTip.h"
#include "ConstantlyButton.h"
#include "BetWnd.h"
#include "ClientControl.h"

//////////////////////////////////////////////////////////////////////////////////

//视图位置
#define MYSELF_VIEW_ID				0									//视图位置

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewD3D
{
	//游戏变量
protected:
	int										m_nCurrentNote;						//当前注
	int										m_nAnimalMultiple[ANIMAL_MAX];		//动物倍数
	BYTE									m_cbGameStatus;						//游戏状态

	//游戏记录
protected:
	CWHArray<int>							m_ArrayTurnTableRecord;				//转盘纪录
	BOOL									m_bShowTurnTableRecord;				//显示游戏记录

	//玩家变量
protected:
	WORD									m_wMineChairID;						//自己位置
	LONGLONG								m_lPlayChip;						//玩家筹码
	LONGLONG								m_lPlayBet[ANIMAL_MAX];				//玩家下注
	LONGLONG								m_lPlayAllBet[ANIMAL_MAX];			//所有玩家下注
	
	//按钮点击
protected:
	BOOL									m_bKeyDown[ANIMAL_MAX];				//按钮点击
	BOOL									m_bKeySend[ANIMAL_MAX];				//按钮点击
	CLapseCount								m_KeyLapse[ANIMAL_MAX];				//按钮时间

	//得分变量
protected:
	LONGLONG								m_lPlayWin[2];						//玩家得分
	LONGLONG								m_lPlayPrizes;						//玩家彩金
	LONGLONG								m_lPlayShowWin;						//玩家显示得分
	LONGLONG								m_lPlayShowPrizes;					//玩家显示彩金
	CLapseCount								m_PlayPrizesLapse;					//彩金时间

	//位置变量
protected:
	CPoint									m_ptBenchmark;						//基准位置
	CSize									m_sizeStage;						//舞台大小
	CPoint									m_ptAnimalMultiple[ANIMAL_MAX];		//动物倍数位置
	CPoint									m_ptPlayBet[ANIMAL_MAX];			//玩家下注位置

	//转盘动画
protected:
	BOOL									m_bTurnTwoTime;						//转2次
	int										m_nTurnTarget[2];					//转盘目标
	int										m_nPrizesMultiple;					//彩金	

	int										m_nBrightCount;						//亮个数
	int										m_nTurnTableCurrent;				//当前转盘索引
	int										m_nTurnTableTarget;					//目标转盘索引 
	int										m_nTurnTableCount;					//延时计数 
	int										m_nTurnTableDelay;					//转盘延时
	int										m_nTurnCount;						//转动数量
	int										m_nTurnTransit;						//中转点
	int										m_nTurnTableLaps;					//圈数
	BOOL									m_bTurnTable;						//开始转圈
	CLapseCount								m_TurnTableLapse;					//转圈流失时间

	//赢动画
protected:
	BOOL									m_nWinShow;							//赢显示
	int										m_nWinIndex;						//赢索引
	int										m_nWinFrame;						//赢索引
	CLapseCount								m_nWinFrameTime;					//赢换帧时间
	CLapseCount								m_nWinContinueTime;					//赢持续时间

	BOOL									m_nPrizesShow;						//派彩显示
	CLapseCount								m_nPrizesContinueTime;				//派彩持续时间
public:
	CButton							        m_btOpenAdmin;						//系统控制
	//控制
public:
	HINSTANCE						        m_hInst;
	IClientControlDlg*			           	m_pClientControlDlg;

	BOOL									m_nGoldSilverShow;					//显示金银鲨
	CLapseCount								m_nGoldSilverContinueTime;			//金银鲨持续时间

	//闪电动画
protected:
	BOOL									m_nLaserShow;						//显示激光
	BOOL									m_nLaserDown;						//激光下落
	int										m_nLaserY;							//激光位置

	//动画变量
protected:
	tgaAnimalsDraw							m_AnimalsDraw[TURAN_TABLE_MAX];		//动画矩形位置

	//资源变量
protected:
	CD3DTexture								m_ImageKeepOut;						//遮挡
	CD3DTexture								m_ImageMainBack;					//背景图片
	CD3DTextureIndex						m_ImageAnimals;						//动物图片
	CD3DTextureIndex						m_ImageAnimalsBack;					//动物背景
	CD3DTextureIndex						m_ImageAnimalFlicker[3];			//动物背景闪动
	CD3DTexture								m_ImageNumberGreen;					//绿色数字
	CD3DTexture								m_ImageNumberRed;					//红色数字
	CD3DTexture								m_ImageNumberYellow;				//黄色数字
	CD3DTexture								m_ImageNumberBlue;					//蓝色数字

	CD3DTexture								m_ImageTimeBack;					//时间背景
	CD3DTexture								m_ImageTimeNumber;					//时间数字
	CD3DTextureIndex						m_ImageAnimalsWin[ANIMAL_MAX];		//动物赢动画
	CD3DTextureIndex						m_ImageWinPrizes;					//彩金赢动画
	CD3DTextureIndex						m_ImageWinBright;					//赢闪光
	CD3DTextureIndex						m_ImageAnimalsWinS[ANIMAL_MAX];		//动物赢动画
	CD3DTexture								m_ImagePrizesNumber;				//彩金数字
	CD3DTextureIndex						m_ImageAnimalRecord;				//游戏记录

	CD3DTexture								m_ImageLaser[2];					//激光动画

	CD3DTextureIndex						m_ImageSharkWin[2];					//鲨鱼赢动画


	//按钮变量
public:
	CConstantlyButton						m_btSelectBet[ANIMAL_MAX];			//选择下注
	CVirtualButton							m_btOperationRenewal;				//续压
	CVirtualButton							m_btOperationCancel;				//取消
	CVirtualButton							m_btOperationSwitch;				//切换

	//窗口变量
public:
	CWindowChip								m_WindowChip;						//筹码兑换界面
	CWindowOver								m_WindowOver;						//结束界面
	CFishDraw								m_FishDraw;							//捕鱼绘画
	CWindowTip								m_WindowTip;						//顶层窗口
	CBetWnd									m_BetWnd;							//下注显示窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//界面函数
protected:
	//动画驱动
	virtual VOID CartoonMovie();
	//配置设备
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);


	//绘画界面
protected:
	//绘画个人信息
	VOID DrawPlayInfo(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	// 界面函数
private:
	// 绘画数字
	void DrawNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT, BYTE cbAlpha = 255);	
	// 绘画数字
	void DrawRotateNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat);	
	//绘画字符
	VOID DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont,  LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);

	//设置函数
public:
	//设置状态
	VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus = cbGameStatus; }
	//设置自己位置
	void SetMineChairID( WORD wMineChairID ) { m_wMineChairID = wMineChairID; }
	//设置当前注
	void SetCurrentNote( int nCurrentNote ) { m_nCurrentNote = nCurrentNote; }
	//设置玩家筹码
	void SetPlayChip( LONGLONG lPlayChip ) { m_lPlayChip = lPlayChip; }
	//设置玩家下注
	void SetPlayBet( LONGLONG lPlayBet[ANIMAL_MAX] ) { CopyMemory(m_lPlayBet, lPlayBet, sizeof(m_lPlayBet)); }
	//设置玩家下注
	void SetPlayBet( LONGLONG lPlayBet, int nAnimalIndex ) { m_lPlayBet[nAnimalIndex] = lPlayBet; }
	//设置所有玩家下注
	void SetPlayAllBet( LONGLONG lPlayAllBet[ANIMAL_MAX] ) { CopyMemory(m_lPlayAllBet, lPlayAllBet, sizeof(m_lPlayAllBet)); }
	//设置所有玩家下注
	void SetPlayAllBet( LONGLONG lPlayAllBet, int nAnimalIndex ) { m_lPlayAllBet[nAnimalIndex] = lPlayAllBet; }
	//设置倍数
	void SetAnimalMultiple( int nAnimalMultiple[ANIMAL_MAX] ) { CopyMemory(m_nAnimalMultiple, nAnimalMultiple, sizeof(m_nAnimalMultiple)); }
	//设置显示记录
	void SetShowTurnTableRecord( BOOL bShowTurnTableRecord ) { m_bShowTurnTableRecord = bShowTurnTableRecord; }
	//设置显示得分
	void SetPlayShowWin( LONGLONG lPlayShowWin ) { m_lPlayShowWin = lPlayShowWin; }
	//设置显示彩金
	void SetPlayShowPrizes( LONGLONG lPlayShowPrizes ) { m_lPlayShowPrizes = lPlayShowPrizes; }
	//添加记录
	void AddTurnTableRecord( int nAnimalIndex );

	//功能函数
public:
	//游戏结束
	void GameOver( BOOL bTurnTwoTime, int nTurnTableTarget[2], int nPrizesMultiple, LONGLONG lPlayWin[2], LONGLONG lPlayPrizes );
	//开始转圈
	void BeginTurnTable(  bool bFirst, int nTurnTableTarget );
	//结束转圈
	void EndTurnTable();
	//显示赢
	void BeginWin( int nAnimalIndex );
	//结束赢
	void EndWin();
	//显示激光
	void BeginLaser();
	//结束激光
	void EndLaser();

	//显示金银鲨
	void BeginGoldSilver();
	//结束金银鲨
	void EndGoldSilver();


	// 按钮消息
public:
	// 狮子按钮
	VOID OnBnClickedAnimalLion();
	// 熊猫按钮
	VOID OnBnClickedAnimalPanda();
	// 猴子按钮
	VOID OnBnClickedAnimalMonkey();
	// 兔子按钮
	VOID OnBnClickedAnimalRabbit();
	// 老鹰按钮
	VOID OnBnClickedAnimalEagle();
	// 孔雀按钮
	VOID OnBnClickedAnimalPeacock();
	// 鸽子按钮
	VOID OnBnClickedAnimalPigeon();
	// 燕子按钮
	VOID OnBnClickedAnimalSwallow();
	// 银鲨按钮
	VOID OnBnClickedAnimalSliverShark();
	// 金鲨按钮
	VOID OnBnClickedAnimalGoldShark();
	// 飞禽按钮
	VOID OnBnClickedAnimalBird();
	// 走兽按钮
	VOID OnBnClickedAnimalBeast();
	// 续压按钮
	VOID OnBnClickedOperationRenewal();
	// 取消按钮
	VOID OnBnClickedOperationCancel();
	// 切换按钮
	VOID OnBnClickedOperationSwitch();
	//允许控制
	void AllowControl(BYTE cbStatus);
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


	//消息函数
public:
	//点击鼠标
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//点击鼠标
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//系统消息
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);
	//键盘按下
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//按键弹起
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//失去焦点
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//管理员控制
	afx_msg void OpenAdminWnd();


	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif