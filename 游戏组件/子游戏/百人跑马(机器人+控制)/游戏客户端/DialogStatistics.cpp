// DialogStatistics.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogStatistics.h"
#include ".\dialogstatistics.h"


// CDialogStatistics 对话框

IMPLEMENT_DYNAMIC(CDialogStatistics, CDialog)
CDialogStatistics::CDialogStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogStatistics::IDD, pParent)
{
	m_InfoFont.CreateFont(12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	ZeroMemory(m_nWinCount, sizeof(m_nWinCount));
}

CDialogStatistics::~CDialogStatistics()
{
	m_InfoFont.DeleteObject();
}

void CDialogStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_TJ_CLOSE, m_btClose);
	DDX_Control(pDX, IDC_BUTTON_TJ_CLOSE_2, m_btDetermine);
}


BEGIN_MESSAGE_MAP(CDialogStatistics, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_TJ_CLOSE, OnBnClickedButtonTjClose)
	ON_BN_CLICKED(IDC_BUTTON_TJ_CLOSE_2, OnBnClickedButtonTjClose2)
END_MESSAGE_MAP()


// CDialogStatistics 消息处理程序

BOOL CDialogStatistics::OnInitDialog()
{
	CDialog::OnInitDialog();

	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_ImageBackdrop.LoadFromResource( hInstance,IDB_BTONGJI_FRAME );
	m_ImageLine.LoadFromResource( hInstance,IDB_LINE );
	m_btClose.SetButtonImage( IDB_BT_CLOSE,  hInstance , false,false);
	m_btDetermine.SetButtonImage( IDB_BT_MESSAGE,  hInstance , false,false);

	ModifyStyle(0, WS_CLIPCHILDREN, 0);

	//CImageHandle HandleBack(&m_ImageBackdrop);
	SetWindowPos(NULL, 0, 0, m_ImageBackdrop.GetWidth(), m_ImageBackdrop.GetHeight(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOMOVE);

	m_btClose.MoveWindow(m_ImageBackdrop.GetWidth() - 26, 1, 25, 25);
	m_btDetermine.MoveWindow(m_ImageBackdrop.GetWidth()/2 - 30, 330, 60, 22);
	return TRUE; 
}

BOOL CDialogStatistics::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDialogStatistics::OnPaint()
{
	CPaintDC dc(this); 

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	CBitmap* oldBitmap = DCBuffer.SelectObject(&ImageBuffer);
	CFont* oldFont = DCBuffer.SelectObject(&m_InfoFont);
	UINT nTextAlign = DCBuffer.SetTextAlign(TA_TOP|TA_LEFT);
	COLORREF oldColor = DCBuffer.SetTextColor(RGB(250,250,255));

	//背景
	m_ImageBackdrop.BitBlt(DCBuffer.GetSafeHdc(), 0, 0);

	//线
	CPoint point;
	point.SetPoint(51, 284);
	for ( int i = 0; i < HORSES_ALL; ++i )
	{
		for ( int j = 0 ; j < m_nWinCount[i]; ++j)
		{
			m_ImageLine.BitBlt(DCBuffer.GetSafeHdc(), point.x, point.y);
			point.y -= 1;
		}

		TCHAR szInfo[64] = TEXT("");
		CRect rect;
		rect.SetRect( point.x - 2, point.y - 15, point.x + 17, point.y);
		_sntprintf(szInfo, sizeof(szInfo), TEXT("%d"), m_nWinCount[i]);
		DCBuffer.DrawText(szInfo, rect , DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );

		point.x += 21;
		point.y = 284;
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.SetTextColor(oldColor);
	DCBuffer.SetTextAlign(nTextAlign);
	DCBuffer.SelectObject(oldBitmap);
	DCBuffer.SelectObject(oldFont);
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();
}

void CDialogStatistics::SetWinCount( INT nWinCount[HORSES_ALL] )
{
	memcpy(m_nWinCount, nWinCount, sizeof(m_nWinCount));

	if ( GetSafeHwnd() && IsWindowVisible())
	{
		Invalidate(FALSE);
	}
}
void CDialogStatistics::OnBnClickedButtonTjClose()
{
	ShowWindow(SW_HIDE);
}

void CDialogStatistics::OnBnClickedButtonTjClose2()
{
	ShowWindow(SW_HIDE);
}
