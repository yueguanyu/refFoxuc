#ifndef GOLD_VIEW_HEAD_FILE
#define GOLD_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
//宏定义
#define GOLD_IMAGE_WIDTH		20										//筹码宽
#define GOLD_IMAGE_HEIGHT		19										//筹码高

//////////////////////////////////////////////////////////////////////////

//筹码视图类
class CGoldView
{
	//变量定义
public:
	LONGLONG							m_lGold;						//筹码数目
	LONGLONG							m_lMaxLayer;					//最大层数
	LONGLONG							m_lGoldCount[7];	 			//筹码数目

	//辅助变量
protected:
	static bool							m_bInit;						//初始标志
	static CPngImageEx					m_ImageGold;					//筹码图片
	CBitImageEx							m_ImageNumber;					//数字图片
	CWnd								*m_pGameViewWnd;				//视图窗口
	//函数定义
public:
	//构造函数
	CGoldView();
	//析构函数
	virtual ~CGoldView();

	//功能函数
public:
	//加载资源
	bool LoadResource(CWnd* pWnd);
	//设置筹码
	void SetGold(LONGLONG lGold);
	//获取筹码
	LONGLONG GetGold() { return m_lGold; };
	//设置层数
	void SetMaxGoldLayer(LONGLONG lMaxLayer);
	//获取层数
	LONGLONG GetMaxGoldLayer(){return m_lMaxLayer;}
	//绘画筹码
	void DrawGoldView(CDC * pDC, int nXPos, int nYPos, bool bCount,bool bCenter=false,BYTE bDUser=0);
	//整性变字符
	LPCTSTR GetGlodString(LONGLONG lGold, TCHAR szString[]);

	//内部函数
private:
	//调整筹码层
	void RectifyGoldLayer();
};

//////////////////////////////////////////////////////////////////////////

#endif
