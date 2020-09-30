// DialogMessage.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogMessage.h"
#include ".\dialogmessage.h"


// CDialogMessage 对话框

IMPLEMENT_DYNAMIC(CDialogMessage, CDialog)
CDialogMessage::CDialogMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMessage::IDD, pParent)
{
	m_InfoFont.CreateFont(12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	ZeroMemory(m_szMessage,sizeof(m_szMessage));
}

CDialogMessage::~CDialogMessage()
{
	m_InfoFont.DeleteObject();
}

void CDialogMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btDetermine);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btClosee);
	
}


BEGIN_MESSAGE_MAP(CDialogMessage, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDialogMessage 消息处理程序

BOOL CDialogMessage::OnInitDialog()
{
	CDialog::OnInitDialog();

	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_ImageBackdrop.LoadFromResource(hInstance, IDB_BACK_MESSAGE );
	//CImageHandle HandleBack(&m_ImageBackdrop);
	SetWindowPos(NULL, 0, 0,m_ImageBackdrop.GetWidth(), m_ImageBackdrop.GetHeight(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOMOVE);

	m_btDetermine.SetButtonImage( IDB_BT_MESSAGE,  hInstance , false, false);
	m_btClosee.SetButtonImage( IDB_BT_CLOSE,  hInstance , false, false);
	CRect rcButton;
	m_btDetermine.GetWindowRect(&rcButton);
	m_btDetermine.MoveWindow( m_ImageBackdrop.GetWidth()/2 - rcButton.Width()/2 , m_ImageBackdrop.GetHeight() - 50, rcButton.Width(), rcButton.Height());
	m_btClosee.MoveWindow( m_ImageBackdrop.GetWidth() - 26, 1, 25,25);
	return TRUE; 
}

BOOL CDialogMessage::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDialogMessage::OnPaint()
{
	CPaintDC dc(this); // device context for painting

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
	COLORREF oldColor = DCBuffer.SetTextColor(RGB(250,250,0));

	//CImageHandle HandleBack(&m_ImageBackdrop);
	m_ImageBackdrop.BitBlt(DCBuffer.GetSafeHdc(), 0, 0);

	TCHAR szInfo[128] = {0};
	DCBuffer.DrawText(m_szMessage, rcClient , DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );

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

//设置消息
void CDialogMessage::SetMessage( LPCTSTR lpszString )
{
	memcpy(m_szMessage, lpszString, sizeof(m_szMessage));
}
void CDialogMessage::OnBnClickedButtonClose()
{
	CDialog::OnOK();
}
