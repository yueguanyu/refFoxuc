#ifndef GOLD_VIEW_HEAD_FILE
#define GOLD_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"

struct JettonHeapInfo
{
	BYTE cbLayerCount;
	BYTE cbDrawIndex;
};

//////////////////////////////////////////////////////////////////////////

//筹码视图类
class CGoldView
{
	//变量定义
public:
	bool								m_bBeelinePut;					//直线摆放
	LONGLONG							m_lGold;						//筹码数目
	LONGLONG							m_lMaxLayer;					//最大层数
	LONGLONG							m_lCellScore;					//单元注数
	LONGLONG							m_lScoreIndex[JETTON_COUNT];	//筹码数目
	LONGLONG							m_lDrawOrder[JETTON_COUNT];		//绘画顺序
	JettonHeapInfo						m_JettonHeapInfo[JETTON_COUNT];	//筹码数目
	BYTE								m_cbFirstIndex;					//低注位置
	CSize								m_JettonSize;					//筹码大小
	CPoint								m_ptDrawJetton[JETTON_COUNT];	//绘画位置

	//辅助变量
protected:
	static bool							m_bInit;						//初始标志
	static CBitImage					m_ImageGold;					//筹码图片

	//函数定义
public:
	//构造函数
	CGoldView();
	//析构函数
	virtual ~CGoldView();

	//功能函数
public:
	//设置摆放
	void SetBeelinePut(bool bBeelinePut);
	//设置筹码
	void SetGold(LONGLONG lGold);
	//设置单元
	void SetCellJetton(LONGLONG lCellJetton);
	//获取筹码
	LONGLONG GetGold() { return m_lGold; };
	//设置层数
	void SetMaxGoldLayer(LONGLONG lMaxLayer);
	//获取层数
	LONGLONG GetMaxGoldLayer(){return m_lMaxLayer;}
	//绘画筹码
	void DrawGoldView(CDC * pDC, int nXPos, int nYPos, bool bCount,bool bCenter=false);
	//整性变字符
	LPCTSTR GetGlodString(LONGLONG lGold, TCHAR szString[]);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//内部函数
private:
	//调整筹码层
	void RectifyGoldLayer();
};

//////////////////////////////////////////////////////////////////////////

#endif
