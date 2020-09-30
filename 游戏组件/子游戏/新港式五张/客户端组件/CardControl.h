#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//属性定义
#define MAX_CARD_COUNT				5									//扑克数目
#define DEF_X_DISTANCE				21									//默认间距

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//枚举定义

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
class CCardControl
{
	//状态变量
protected:
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayHead;						//显示标志

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[MAX_CARD_COUNT];		//扑克数据

	//间隔变量
protected:
	UINT							m_nXDistance;						//横向间隔

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//资源变量
protected:
	CSize							m_CardSize;							//扑克大小
	CPngImage						m_ImageCard;						//图片资源

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//扑克控制
public:
	//扑克数目
	WORD GetCardCount() { return m_wCardCount; }
	//获取扑克
	WORD GetCardData(BYTE cbCardData[], WORD wBufferCount);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//状态查询
public:
	//获取大小
	CSize GetCardSize() { return m_CardSize; }
	//查询响应
	bool GetPositively() { return m_bPositively; }
	//查询显示
	bool GetDisplayHead() { return m_bDisplayHead; }

	//状态控制
public:
	//设置距离
	VOID SetXDistance(UINT nXDistance) { m_nXDistance=nXDistance; }
	//设置响应
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//设置显示
	VOID SetDisplayHead(bool bDisplayHead) { m_bDisplayHead=bDisplayHead; }
	//获取牌尾位置
	CPoint GetTailPos();
	//获取中心点
	VOID GetCenterPoint(CPoint & CenterPoint);
	
	//控件控制
public:
	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	VOID SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//事件控制
public:
	//初始化
	VOID Initialize(bool bSmall);
	//绘画扑克
	VOID DrawCardControl(CDC * pDC);
	//光标消息
	bool OnEventSetCursor(CPoint Point);
	//鼠标消息
	bool OnEventLeftMouseDown(CPoint Point);

	//内部函数
private:
	//获取大小
	VOID GetControlSize(CSize & ControlSize);
	//索引切换
	WORD SwitchCardPoint(CPoint & MousePoint);
	//获取原点
	VOID GetOriginPoint(CPoint & OriginPoint);
};

//////////////////////////////////////////////////////////////////////////

#endif