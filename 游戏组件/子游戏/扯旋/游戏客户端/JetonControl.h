#ifndef JETTON_CONTROL_HEAD_FILE
#define JETTON_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

struct JettonLayerInfo
{
	BYTE cbLayerCount;
	BYTE cbDrawIndex;
};
//////////////////////////////////////////////////////////////////////////

//筹码控件
class CJettonControl
{
	//筹码变量
protected:
	LONGLONG						m_lUserHaveCount;					//现有筹码
	LONGLONG						m_lCellJetton;						//单元筹码
	LONGLONG						m_lUserJetton;						//筹码数目
	LONGLONG						m_lUserAddJetton;					//加注数目
	LONGLONG						m_lMinJetton;						//最少筹码
	LONGLONG						m_lScoreIndex[JETTON_COUNT];		//筹码数目
	BYTE							m_cbChangeStatus[CONTROL_COUNT];	//背景状态
	BYTE							m_cbFirstIndex;						//当前所引
	BYTE							m_cbMaxLayerCount;					//筹码层数
	JettonLayerInfo					m_AddLayerInfo[CONTROL_COUNT];		//筹码信息
	JettonLayerInfo					m_UserLayerInfo[CONTROL_COUNT];		//筹码信息

	//绘画变量
protected:
	CSize							m_BigJettonSize;					//筹码大小
	CSize							m_SmaJettonSize; 					//筹码大小
	CSize							m_JettonBackSize; 					//背景大小
	CPoint							m_UserJettonlPost;					//基准位置
	CPoint							m_AddJettonPost;					//加注位置

	//辅助变量
protected:
	CBitImage						m_ImageBigJetton;					//筹码图片
	CBitImage						m_ImageSmaJetton;					//筹码图片
	CBitImage						m_ImageJettonBack;					//筹码背景
	CSkinButton						*m_pbtDecrease;						//减注按钮
	CSkinButton						*m_pbtClearJetton;					//清理按钮
	CSkinButton						*m_pbtAdd;							//加注按钮
	
	//函数定义
public:
	//构造函数
	CJettonControl();
	//析构函数
	virtual ~CJettonControl();

	//功能函数
public:
	//设置现有
	void SetUserHaveCount(LONGLONG lHaveCound);
	//设置按钮
	void SetButton(CSkinButton *btDecrease,CSkinButton *pbtClearJetton,CSkinButton *pbtAdd);
	//更新控件
	void UpdataButton();
	//绘画位置
	void SetControlPost(CPoint ControlPost,CPoint AddPost);
	//单元层数
	void SetJettonLayer(BYTE cbJettonLayer);
	//设置单元
	void SetCellJetton(LONGLONG lCellJetton);
	//最小筹码
	void SetMinJetton(LONGLONG lMinJetton);
	//设置筹码
	void SetUserJetton(LONGLONG lUserJetton);
	//设置筹码
	void SetAddJetton(LONGLONG lAddJetton);
	//计算筹码
	void CountJettonInfo(LONGLONG lScore,JettonLayerInfo LayerInfo[CONTROL_COUNT],BYTE cbChangeIndex=0);
	//点击判断
	bool EstimateHitJetton(CPoint CurrentPoint);
	//移动判断
	bool EstimateMove(CPoint CurrentPoint);
	//减少筹码
	bool DecreaseJetton(WORD wControlID);
	//清理加注
	void ClearAddJetton();
	//获取筹码
	LONGLONG GetUserJetton(){return m_lUserJetton;}
	//获取筹码
	LONGLONG GetAddJetton(){return m_lUserAddJetton;}
	//获取筹码
	LONGLONG GetMinJetton(){return m_lMinJetton;}
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//绘画控制
public:
	//绘画筹码
	void DrawJettonControl(CDC * pDC);
};

//////////////////////////////////////////////////////////////////////////

#endif
