#ifndef NUMBER_CONTROL_HEAD_FILE
#define NUMBER_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//数字滚动控件
class CNumberControl
{
	//控件变量
protected:
	CPoint					m_BenchmarkPos;							//基准位置
	enXCollocateMode		m_XCollocateMode;						//显示模式
	CRect					m_rcDraw;								//更新绘画区域

	//数字变量
	LONGLONG				m_lScore;								//滚动数字

	//动画变量
	INT						m_nYExcusion;							//偏移
	INT_PTR					m_nScrollIndex;							//滚动索引
	INT						m_nScrollCount;							//滚动次数
	CWHArray<BYTE>			m_arScoreNum;							//滚动数字
	BOOL					m_bMoving;								//滚动标识
	
	//位图变量
protected:
	CPngImage				m_PngScoreNum;							//滚动数字图
	CPngImage				m_PngScoreBack;							//滚动背景图
	
public:
	//构造函数
	CNumberControl();
	//析构函数
	virtual ~CNumberControl();

	//功能函数
public:
	//设置基准位置
	VOID		SetBencbmarkPos( INT nXPos, INT nYPos, enXCollocateMode XCollocateMode );
	//设置数字
	VOID		SetScore( LONGLONG lScore );
	//开始滚动
	BOOL		BeginScrollNumber();
	//滚动数字
	BOOL		PlayScrollNumber();
	//停止滚动
	BOOL		FinishScrollNumber();
	//重置控件
	VOID		ResetControl();

	//绘画函数
public:
	//绘画控件
	VOID		DrawNumberControl( CDC *pDC );
	//获取更新绘画区域
	VOID		GetDrawRect( CRect &rc );
	//获取数字
	LONGLONG	GetScore() { return m_lScore; };

	//辅助函数
protected:
	//获取原点
	VOID GetOriginPoint(CPoint & OriginPoint);
};

#endif

//////////////////////////////////////////////////////////////////////////