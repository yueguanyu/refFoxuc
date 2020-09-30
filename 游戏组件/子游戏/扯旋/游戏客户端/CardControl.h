#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl : public CWnd
{
	//配置变量
protected:
	bool							m_bFlash;							//闪烁标志
	bool							m_bDisable;							//无效标志
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayHead;						//显示扑克
	DWORD							m_dwCardHSpace;						//横向间隔

	//状态变量
protected:
	bool							m_bCaptureMouse;					//捕获标志
	CWHArray<BYTE>			m_CardDataArray;					//扑克数组

	//位置信息
protected:
	CRgn							m_CardRegion;						//扑克区域
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//静态变量
protected:
	static bool						m_bLoad;							//初始标志
	static CSize					m_CardSize;							//扑克大小
	static CBitImage				m_ImageCard;						//图片资源
	CBitImage						m_ImageMark;						//图片资源

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//控制函数
public:
	//设置闪烁
	void SetCardFlash(bool bFlash);
	//设置无效
	void SetDisableCardBack(bool bDisable);
	//设置间距
	void SetCardSpace(DWORD dwCardSpace);
	//设置响应
	void SetPositively(bool bPositively);
	//显示扑克
	void SetDisplayHead(bool bDisplayHead);
	//基准位置
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//获取位置
	CPoint GetCardPos(bool bMaxCount=TRUE);
	//获取标志
	bool GetCardDisable(){return m_bDisable;}
	//获取标志
	bool GetPositively(){return m_bPositively;}
	//获取标志
	bool GetDisplayHead(){return m_bDisplayHead;}
	//获取标志
	bool GetCardFlash(){return m_bFlash;}
	//分析暗牌
	void AnalyseMingCard();

	//功能函数
public:
	//获取数目
	DWORD GetCardCount();
	//获取数目
	DWORD GetUnDisableCardCount();
	//设置扑克
	DWORD GetCardData(BYTE cbCardData[], DWORD dwMaxCount);
	//设置扑克
	BYTE GetCardData(BYTE cbIndex);
	//设置扑克
	DWORD SetCardData(const BYTE bCardData[], DWORD dwCardCount);
	//设置扑克
	DWORD SetNullityCardData(BYTE cbIndex,BYTE cbCardData=0xff);
	//设置扑克
	DWORD GetNullityCardData(BYTE cbCardData=0xff);

	//内部函数
private:
	//调整位置
	void RectifyControl();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
