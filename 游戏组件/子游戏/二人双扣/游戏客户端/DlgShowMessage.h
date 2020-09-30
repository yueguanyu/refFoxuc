#pragma once

#include "Stdafx.h"
#include "resource.h"
// CDlgShowMessage 对话框

class CDlgShowMessage : public CWnd
{

public:
	CDlgShowMessage(CWnd* pParent = NULL);   // 标准构造函数
	//加载位图
	bool LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans);
	//加载位图
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);
    //鼠标拖拽
	void IsMouseDrag(bool bFlags);
	//显示窗体
	void ShowWindowEx(bool bFlags,int nTimer,CString&str);
	//设置字符
	void SetPrintStr(CString&str);
	//获取宽度
	int GetWidth();
	//获取高度
	int GetHight();
	//设置位置
	bool SetWndPos(CPoint&point);
	//获取位置
	bool GetWndPos(CPoint&point);
	//文字位置
	void SetTextRect(CRect&rect);
    //艺术字体
	void DrawTextString(CDC * pDC, CString&szString, COLORREF crText, COLORREF crFrame);

	//析构函数
	virtual ~CDlgShowMessage();
public:
	CRgn							m_DialogRgn;						//窗口区域
	CSkinImage						m_ImageBack;						//背景位图
	bool                            m_bMouseDrag;
	bool                            m_bIsShow;
	CString                         m_szStr;
	int                             m_nXBGImage;
	int                             m_nYBGImage;
    CRect                           m_TextRect;
	CPoint                          m_WndPos;
	//内部函数 
protected:
	//创建区域
	bool CreateControlRgn(COLORREF crTrans);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};
