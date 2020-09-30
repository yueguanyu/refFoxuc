#include "StdAfx.h"
#include ".\shbutton.h"

//时间标识
#define IDI_SEND_MESSAGE				21									//标识
#define TIME_SEND_MESSAGE				30									//时间

CSHButton::CSHButton(void)
{
	m_bMouseDown = false;
}

CSHButton::~CSHButton(void)
{
}
BEGIN_MESSAGE_MAP(CSHButton, CSkinButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CSHButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSkinButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer( IDI_SEND_MESSAGE, TIME_SEND_MESSAGE, NULL);

	return 0;
}


// 绘画函数
void CSHButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_bMouseDown = ((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);
	CSkinButton::DrawItem(lpDrawItemStruct);
}


// 单击函数
void CSHButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = true;
	m_dwDownTime = GetTickCount();
	CSkinButton::OnLButtonDown(nFlags, point);
}

// 失去焦点
void CSHButton::OnKillFocus(CWnd* pNewWnd)
{
	m_bMouseDown = false;
	CSkinButton::OnKillFocus(pNewWnd);
}

// 左键弹起
void CSHButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = false;
	CSkinButton::OnLButtonUp(nFlags, point);
}

// 定时器
void CSHButton::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == IDI_SEND_MESSAGE && m_bHovering && m_bMouseDown && GetTickCount() - m_dwDownTime > 800 )
	{
		UINT uControlID = GetWindowLong(m_hWnd,GWL_ID);
		::SendMessage(GetParent()->GetSafeHwnd(),WM_COMMAND,MAKELPARAM(uControlID,BN_CLICKED),(LPARAM)(::GetDlgItem(GetParent()->GetSafeHwnd(),uControlID)));
	}

	CSkinButton::OnTimer(nIDEvent);
}



