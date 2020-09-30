#ifndef JETTON_CONTROL_HEAD_FILE
#define JETTON_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//属性定义
#define MAX_JETTON_INDEX				14									//筹码数目

//待决筹码
struct tagJetPending
{
	LONGLONG	lScore;													//筹码分
	CPoint		ptJetton;												//筹码位置
};

//////////////////////////////////////////////////////////////////////////

//筹码控件
class CJettonControl
{
	//变量定义
protected:
	static LONGLONG					m_lJetonIndex[MAX_JETTON_INDEX];	//筹码索引

	LONGLONG						m_lScore;							//筹码数目
	CPoint							m_BenchmarkPos;						//基准位置

	//位图变量
protected:
	CPngImage						m_PngJetton;						//筹码位图
	INT								m_nJettonWidth;						//筹码宽
	INT								m_nJettonHeight;					//筹码高

	//位置变量
protected:
	CWHArray<WORD>			m_arJetIndex;						//绘画筹码
	CWHArray<CPoint>			m_arJetExcusions;					//绘画位置
	

	//动画变量
protected:
	CWHArray<tagJetPending>	m_arJetPending;						//待决筹码

	CWHArray<WORD>			m_arIndexPending;					//待决筹码索引
	CWHArray<CPoint>			m_arExcusionsPend;					//偏移
	CWHArray<INT>				m_arStepCount;						//步数
	CWHArray<INT>				m_arXStep;							//X步长
	CWHArray<INT>				m_arYStep;							//Y步长
	LONGLONG						m_lScorePending;					//待决筹码分
	CRect							m_rcMove;							//移动筹码区域
	CRect							m_rcDraw;							//移动绘画区域

	//辅助函数
protected:
	//计算移动区域
	VOID		ComputeMoveRect();
	//构造动画
	BOOL		ConstructCartoon();
	//提取筹码索引
	BOOL		DistillJetIndex( LONGLONG lScore, INT_PTR &nJetIndex);
	//压缩筹码,把小值筹码组合成大值筹码
	VOID		CompactJetIndex();

public:
	//构造函数
	CJettonControl();
	//析构函数
	virtual ~CJettonControl();

	//功能函数
public:
	//获取筹码
	LONGLONG	GetScore() { return m_lScore; }
	//重置控件
	VOID		ResetControl();
	//加筹码
	VOID		AddScore( LONGLONG lScore, CPoint ptFrom = CPoint(0,0) );
	//移除筹码
	VOID		RemoveScore( LONGLONG lScore, CPoint ptTo = CPoint(0,0) );
	//移除所有筹码
	VOID		RemoveAllScore( CPoint ptTo = CPoint(0,0) );
	
	//功能函数
public:
	//开始移动筹码
	BOOL		BeginMoveJettons();
	//移动筹码
	BOOL		PlayMoveJettons();
	//停止移动筹码
	BOOL		FinishMoveJettons();
	//是否在移动
	BOOL		IsPlayMoving();

	//位置函数
public:
	//基准位置
	VOID		SetBenchmarkPos(INT nXPos, INT nYPos);
	//获取位置
	CPoint		GetBenchmarkPos() { return m_BenchmarkPos; }
	//获取更新区域
	VOID		GetDrawRect(CRect &rc);

	//控制函数
public:
	//绘画控件
	VOID		DrawJettonControl(CDC * pDC);
};

//////////////////////////////////////////////////////////////////////////

#endif