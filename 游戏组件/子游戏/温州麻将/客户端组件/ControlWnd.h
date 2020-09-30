#ifndef CONTROL_WND_HEAD_FILE
#define CONTROL_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_USER_ACTION				(WM_USER+300)						//用户动作

//////////////////////////////////////////////////////////////////////////

//控制窗口
class CControlWnd : public CWnd
{
	//控件变量
protected:
	CSkinButton						m_btPeng;							//控制按钮
	CSkinButton						m_btGang;							//控制按钮
	CSkinButton						m_btHear;							//控制按钮
	CSkinButton						m_btChiHu;							//控制按钮
	CSkinButton						m_btGiveUp;							//控制按钮
	CSkinButton						m_btChi;							//控件按钮

	//资源变量
protected:
	CBitImage						m_ImageControlBack;					//窗口背景

	//函数定义
public:
	//构造函数
	CControlWnd();
	//析构函数
	virtual ~CControlWnd();

	//功能函数
public:
	//设置状态
	VOID SetControlInfo(BYTE cbAcitonMask);

	//内部函数
protected:
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);

	//按钮消息
protected:
	//碰牌按钮
	VOID OnBnClickedPeng();
	//杆牌按钮
	VOID OnBnClickedGang();
	//听牌按钮
	VOID OnBnClickedHear();
	//吃胡按钮
	VOID OnBnClickedChiHu();
	//放弃按钮
	VOID OnBnClickedGiveUp();
	//吃牌按钮
	VOID OnBnClickedChi();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
