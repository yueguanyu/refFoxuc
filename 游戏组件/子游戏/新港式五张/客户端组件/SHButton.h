#pragma once

class CSHButton : public CSkinButton
{
public:
	bool				m_bMouseDown;			// 鼠标点下
	DWORD				m_dwDownTime;			// 点击时间

public:
	CSHButton(void);
	~CSHButton(void);

	// 消息函数
public:
	// 创建函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// 绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	// 左键点击
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// 失去焦点
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	// 左键弹起
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	// 定时器
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()

};
