#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "MyImage.h"
#include "DialogBet.h"
#include "DialogRecord.h"
#include "DialogStatistics.h"
#include "DialogBetRecord.h"
#include "UserFaceWnd.h"
#include "DirectSound.h"
#include "DialogControl.h"


//////////////////////////////////////////////////////////////////////////


//游戏视图
class CGameClientView : public CGameFrameViewD3D
{
	//基础信息
private:
	INT						m_nStreak;							//场次
	WORD					m_wKindID;							//游戏ID
	tagBASE					m_tagBenchmark;						//基准结构
	INT						m_nBetEndTime;						//下注结束时间

	//游戏变量
private:
	WORD					m_wMeChairID;						//我的位置
	BYTE					m_cbGameStatus;						//设置状态
	bool					m_bRacing;							//正在赛马
	bool					m_bMeOrAll;							//显示自己还是全部(true为Me, false为All)

	//游戏信息
private:
	INT						m_nMultiple[AREA_ALL];				//区域倍数
	BYTE					m_bRanking[RANKING_NULL];			//名次

	//分数
protected:
	INT						m_nBetPlayerCount;					//下注人数
	LONGLONG				m_lPlayerBet[AREA_ALL];				//玩家下注
	LONGLONG				m_lPlayerBetAll[AREA_ALL];			//所有下注
	LONGLONG				m_lPlayerWinning;					//玩家输赢

	//大小
private:
	CSize					m_szTotalSize;						//总大小
	CSize					m_szBackdrop;						//背景中大小
	CSize					m_szBackdropHand;					//背景头大小
	CSize					m_szBackdropTai;					//背景尾大小
	
	//马匹
public:	
	INT						m_nHorsesSpeed[HORSES_ALL][STEP_SPEED];	//每匹马的每秒速度(标准值)
	INT						m_nHorsesSpeedIndex[HORSES_ALL];	//每匹马速度索引
	INT						m_nHorsesBasicSpeed;				//马匹基础速度
	INT						m_nInFrameDelay;					//马匹换帧延迟
	BYTE					m_cbHorsesIndex[HORSES_ALL];		//马索引
	TCHAR					m_szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字
	CPoint					m_ptHorsesPos[HORSES_ALL];			//马位置
	CMyD3DTexture			m_ImageHorses[HORSES_ALL];			//马
	CDirectSound			m_HorsesSound;						//马匹声音
	CDirectSound			m_GameOverSound;					//马匹声音

	//游戏结束
public:
	bool					m_bGameOver;						//游戏结束
	CPoint					m_ptGameOver;						//位置
	CD3DTexture				m_ImageGameOver;					//地图
	CSkinButton				m_btGameOver;						//按钮

	//背景
public:
	CMyD3DTexture			m_ImageBackdropHand;				//背景头
	CMyD3DTexture			m_ImageBackdropTail;				//背景尾
	CMyD3DTexture			m_ImageBackdrop;					//背景中
	CMyD3DTexture			m_ImageDistanceFrame;				//距离

	//状态栏
public:
	CD3DTexture				m_ImageUserInfoL;					//左
	CD3DTexture				m_ImageUserInfoM;					//中
	CD3DTexture				m_ImageUserInfoR;					//右
	CD3DTexture				m_ImageUserInfoShu;					//中竖线

	//遮掩栏
public:
	CMyD3DTexture			m_ImagePenHand;						//围栏头
	CMyD3DTexture			m_ImagePenNum;						//围栏数字
	CMyD3DTexture			m_ImagePenTail;						//围栏尾

	//时间
public:
	CD3DTexture				m_ImageTimeNumer;					//数字
	CD3DTexture				m_ImageTimeBet;						//下注时间
	CD3DTexture				m_ImageTimeBetEnd;					//下注结束
	CD3DTexture				m_ImageTimeFree;					//空闲时间
	CD3DTexture				m_ImageTimeHorseEnd;				//跑马结束
	CD3DTexture				m_ImageTimeHorse;					//跑马

	//控件变量
public:
	CDialogPlayBet			m_DlgPlayBet;						//下注窗口
	CDialogRecord			m_DlgRecord;						//游戏记录
	CDialogStatistics		m_DlgStatistics;					//统计窗口
	CDialogBetRecord		m_DlgBetRecord;						//记录窗口
	//CUserFaceWnd			m_WndUserFace;						//玩家脸部
	CDialogControl			m_DlgControl;						//控制窗口
	CButton					m_btOpenControl;					//系统控制

	//按钮
public:
	CSkinButton				m_btPlayerBetShow;					//显示个人投注
	CSkinButton				m_btAllBetShow;						//显示全场投注

	CSkinButton				m_btPlayerBet;						//个人投注
	CSkinButton				m_btStatistics;						//统计
	CSkinButton				m_btExplain;						//说明

	CSkinButton				m_btRecord;							//历史记录
	CSkinButton				m_btBetRecord;						//下注记录

	//背景资源
protected:
	CD3DTexture				m_TextureBack;						//背景资源

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

	//界面函数
protected:
	//动画驱动
	virtual VOID CartoonMovie();
	//配置设备
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);



	//设置函数
public:
	//设置ID
	void SetKindID( WORD wKindID ) { m_wKindID = wKindID; }
	//设置场次
	void SetStreak( INT nStreak ) { m_nStreak = nStreak; }
	//设置我的位置
	void SetMeChairID( WORD wMeChairID ) { m_wMeChairID = wMeChairID; }
	//设置状态
	void SetGameStatus(BYTE bGameStatus) { m_cbGameStatus = bGameStatus; }
	//设置输赢
	void SetPlayerWinning( LONGLONG lPlayerWinning ) { m_lPlayerWinning = lPlayerWinning; }
	//设置下注结束时间
	void SetBetEndTime( INT	nBetEndTime ) { m_nBetEndTime = nBetEndTime ;}
	//设置下注人数
	void SetBetPlayerCount( INT nBetPlayerCount) { m_nBetPlayerCount = nBetPlayerCount; }
	//设置马名字
	void SetHorsesName( WORD wMeChairID, TCHAR szName[HORSES_NAME_LENGTH]);
	//设置所有下注
	void SetAllBet(BYTE cbArea, LONGLONG lScore);
	//设置单人下注加注
	void SetPlayerBet(WORD wMeChairID, BYTE cbArea, LONGLONG lScore);
	//设置名次
	void SetRanking(BYTE bRanking[RANKING_NULL]);



	//绘画函数
private:
	//绘画背景
	void DrawBackdrop( CD3DDevice * pD3DDevice );
	//绘画马
	void DrawHorses( CD3DDevice * pD3DDevice );
	//绘画时间提示
	void DrawTimeClew( CD3DDevice * pD3DDevice );
	//绘画玩家信息
	void DrawUserInfo( CD3DDevice * pD3DDevice );
	//绘画结束信息
	void DrawGameOver( CD3DDevice * pD3DDevice );

	//界面函数
private:
	//艺术字体
	void DrawTextString( CD3DDevice * pD3DDevice, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawNumber(CD3DDevice * pD3DDevice , int nXPos, int nYPos, int nNumer, bool bTime = false );

	//跑马函数
public:
	//赛马开始
	void HorsesStart(INT nHorsesSpeed[HORSES_ALL][COMPLETION_TIME]);
	//赛马结束
	void HorsesEnd();
	//新一局开始
	void NweHorses();

	//按钮消息
public:
	//个人下注显示
	afx_msg void OnPlayerBetShow();
	//全场下注显示
	afx_msg void OnAllBetShow();
	//个人下注
	afx_msg void OnPlayerBet();
	//历史记录
	afx_msg void OnRecordShow();
	//统计窗口
	afx_msg void OnStatistics();
	//说明
	afx_msg void OnExplain();
	//成绩关闭
	afx_msg void OnGameOverClose();
	//下注成绩
	afx_msg void OnBetRecordShow();
	//管理员控制
	afx_msg void OpenControlWnd();

	//消息映射
public:
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);
	//创建窗口
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//窗口销毁
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
