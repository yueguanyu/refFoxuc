#pragma once
#include "resource.h"
#include "stdafx.h"

//操作结果
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};

//记录信息
struct tagClientGameRecord
{
	enOperateResult					enOperateFlags;						//操作标识
	BYTE							cbResult;							//结果
	BYTE							cbLong;								//龙点
	BYTE							cbHu;								//虎点
};

//历史记录
#define MAX_SCORE_HISTORY			65									//历史个数

//////////////////////////////////////////////////////////////////////////////////////
class CDlgViewChart : public CDialog
{
	//历史信息
public:
	LONGLONG							m_lMeStatisticScore;				//游戏成绩
	tagClientGameRecord				m_TraceGameRecordArray[100];		//路单记录
	tagClientGameRecord				m_GameRecordArray[100];				//结果记录					
	int								m_TraceGameRecordCount;				//路单数目
	int								m_GameRecordCount;					//记录数目
	bool							m_bFillTrace[6][29];				//填充标识

	//资源变量
protected:
	CBitImage						m_ImageBack;						//背景图片
	CPngImage						m_PngPlayerFlag;					//闲家图片
	CPngImage						m_PngPlayerEXFlag;					//闲家图片
	CPngImage						m_PngBankerFlag;					//庄家图片
	CPngImage						m_PngBankerEXFlag;					//庄家图片
	CPngImage						m_PngTieFlag;						//平家图片
	CPngImage						m_PngTieEXFlag;						//平家图片
	CPngImage						m_PngTwopairFlag;					//平家图片

	//控件变量
protected:
	CSkinButton							m_btClose;							//关闭按钮

	//界面函数
public:
	//更新界面
	void UpdateChart(){InvalidateRect(NULL);}
	//绘画百分比
	void DrawPrecent(CDC *pDC);
	//绘画表格
	void DrawChart(CDC *pDC);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//信息接口
public:
	//设置结果
	void SetGameRecord(const tagClientGameRecord &ClientGameRecord);

public:
	CDlgViewChart(); 
	virtual ~CDlgViewChart();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_DYNAMIC(CDlgViewChart)
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
