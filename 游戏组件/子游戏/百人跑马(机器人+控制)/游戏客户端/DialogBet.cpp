// DialogBet.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogBet.h"
#include "DialogMessage.h"
#include ".\dialogbet.h"



// CDialogPlayBet 对话框
#define  MAX_BET		9999999l


IMPLEMENT_DYNAMIC(CDialogPlayBet, CDialog)
CDialogPlayBet::CDialogPlayBet(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPlayBet::IDD, pParent)
{
	//区域倍数
	for ( int i = 0; i < CountArray(m_nMultiple); ++i)
		m_nMultiple[i] = 1;

	m_InfoFont.CreateFont(12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	m_MultipleFont.CreateFont(13,0,0,0,800,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	m_InfoBrush.CreateSolidBrush(RGB(79,87,100));
	ZeroMemory(m_lLastRound,sizeof(m_lLastRound));
	m_lPlayerScore = 0l;
	m_lTheNote = 1000l;

}

CDialogPlayBet::~CDialogPlayBet()
{
	m_InfoFont.DeleteObject();
	m_MultipleFont.DeleteObject();
	m_InfoBrush.DeleteObject();
}

void CDialogPlayBet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_1_6, m_editInput[AREA_1_6]);
	DDX_Control(pDX, IDC_EDIT_1_5, m_editInput[AREA_1_5]);
	DDX_Control(pDX, IDC_EDIT_1_4, m_editInput[AREA_1_4]);
	DDX_Control(pDX, IDC_EDIT_1_3, m_editInput[AREA_1_3]);
	DDX_Control(pDX, IDC_EDIT_1_2, m_editInput[AREA_1_2]);
	DDX_Control(pDX, IDC_EDIT_2_6, m_editInput[AREA_2_6]);
	DDX_Control(pDX, IDC_EDIT_2_5, m_editInput[AREA_2_5]);
	DDX_Control(pDX, IDC_EDIT_2_4, m_editInput[AREA_2_4]);
	DDX_Control(pDX, IDC_EDIT_2_3, m_editInput[AREA_2_3]);
	DDX_Control(pDX, IDC_EDIT_3_6, m_editInput[AREA_3_6]);
	DDX_Control(pDX, IDC_EDIT_3_5, m_editInput[AREA_3_5]);
	DDX_Control(pDX, IDC_EDIT_3_4, m_editInput[AREA_3_4]);
	DDX_Control(pDX, IDC_EDIT_4_6, m_editInput[AREA_4_6]);
	DDX_Control(pDX, IDC_EDIT_4_5, m_editInput[AREA_4_5]);
	DDX_Control(pDX, IDC_EDIT_5_6, m_editInput[AREA_5_6]);

	DDX_Control(pDX, IDC_BUTTON_ADD_1_6, m_btAdd[AREA_1_6]);
	DDX_Control(pDX, IDC_BUTTON_ADD_1_5, m_btAdd[AREA_1_5]);
	DDX_Control(pDX, IDC_BUTTON_ADD_1_4, m_btAdd[AREA_1_4]);
	DDX_Control(pDX, IDC_BUTTON_ADD_1_3, m_btAdd[AREA_1_3]);
	DDX_Control(pDX, IDC_BUTTON_ADD_1_2, m_btAdd[AREA_1_2]);
	DDX_Control(pDX, IDC_BUTTON_ADD_2_6, m_btAdd[AREA_2_6]);
	DDX_Control(pDX, IDC_BUTTON_ADD_2_5, m_btAdd[AREA_2_5]);
	DDX_Control(pDX, IDC_BUTTON_ADD_2_4, m_btAdd[AREA_2_4]);
	DDX_Control(pDX, IDC_BUTTON_ADD_2_3, m_btAdd[AREA_2_3]);
	DDX_Control(pDX, IDC_BUTTON_ADD_3_6, m_btAdd[AREA_3_6]);
	DDX_Control(pDX, IDC_BUTTON_ADD_3_5, m_btAdd[AREA_3_5]);
	DDX_Control(pDX, IDC_BUTTON_ADD_3_4, m_btAdd[AREA_3_4]);
	DDX_Control(pDX, IDC_BUTTON_ADD_4_6, m_btAdd[AREA_4_6]);
	DDX_Control(pDX, IDC_BUTTON_ADD_4_5, m_btAdd[AREA_4_5]);
	DDX_Control(pDX, IDC_BUTTON_ADD_5_6, m_btAdd[AREA_5_6]);

	DDX_Control(pDX, IDC_BUTTON_REDUCE_1_6, m_btReduce[AREA_1_6]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_1_5, m_btReduce[AREA_1_5]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_1_4, m_btReduce[AREA_1_4]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_1_3, m_btReduce[AREA_1_3]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_1_2, m_btReduce[AREA_1_2]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_2_6, m_btReduce[AREA_2_6]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_2_5, m_btReduce[AREA_2_5]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_2_4, m_btReduce[AREA_2_4]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_2_3, m_btReduce[AREA_2_3]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_3_6, m_btReduce[AREA_3_6]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_3_5, m_btReduce[AREA_3_5]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_3_4, m_btReduce[AREA_3_4]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_4_6, m_btReduce[AREA_4_6]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_4_5, m_btReduce[AREA_4_5]);
	DDX_Control(pDX, IDC_BUTTON_REDUCE_5_6, m_btReduce[AREA_5_6]);

	DDX_Control(pDX, IDC_BUTTON_1000, m_btBet1000);
	DDX_Control(pDX, IDC_BUTTON_1W, m_btBet1W);
	DDX_Control(pDX, IDC_BUTTON_10W, m_btBet10W);
	DDX_Control(pDX, IDC_BUTTON_100W, m_btBet100W);
	DDX_Control(pDX, IDC_BUTTON_DETERMINE, m_btDetermine);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_btReset);
	DDX_Control(pDX, IDC_BUTTON_REPEAT, m_btRepeat);
	DDX_Control(pDX, IDC_BUTTON_CLOSE_BET, m_btClosee);

}


BEGIN_MESSAGE_MAP(CDialogPlayBet, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_1000, OnBnClickedButton1000)
	ON_BN_CLICKED(IDC_BUTTON_1W, OnBnClickedButton1w)
	ON_BN_CLICKED(IDC_BUTTON_10W, OnBnClickedButton10w)
	ON_BN_CLICKED(IDC_BUTTON_100W, OnBnClickedButton100w)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_1_6, OnBnClickedButtonReduce16)
	ON_BN_CLICKED(IDC_BUTTON_ADD_1_6, OnBnClickedButtonAdd16)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_1_5, OnBnClickedButtonReduce15)
	ON_BN_CLICKED(IDC_BUTTON_ADD_1_5, OnBnClickedButtonAdd15)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_1_4, OnBnClickedButtonReduce14)
	ON_BN_CLICKED(IDC_BUTTON_ADD_1_4, OnBnClickedButtonAdd14)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_1_3, OnBnClickedButtonReduce13)
	ON_BN_CLICKED(IDC_BUTTON_ADD_1_3, OnBnClickedButtonAdd13)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_1_2, OnBnClickedButtonReduce12)
	ON_BN_CLICKED(IDC_BUTTON_ADD_1_2, OnBnClickedButtonAdd12)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_2_6, OnBnClickedButtonReduce26)
	ON_BN_CLICKED(IDC_BUTTON_ADD_2_6, OnBnClickedButtonAdd26)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_2_5, OnBnClickedButtonReduce25)
	ON_BN_CLICKED(IDC_BUTTON_ADD_2_5, OnBnClickedButtonAdd25)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_2_4, OnBnClickedButtonReduce24)
	ON_BN_CLICKED(IDC_BUTTON_ADD_2_4, OnBnClickedButtonAdd24)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_2_3, OnBnClickedButtonReduce23)
	ON_BN_CLICKED(IDC_BUTTON_ADD_2_3, OnBnClickedButtonAdd23)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_3_6, OnBnClickedButtonReduce36)
	ON_BN_CLICKED(IDC_BUTTON_ADD_3_6, OnBnClickedButtonAdd36)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_3_5, OnBnClickedButtonReduce35)
	ON_BN_CLICKED(IDC_BUTTON_ADD_3_5, OnBnClickedButtonAdd35)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_3_4, OnBnClickedButtonReduce34)
	ON_BN_CLICKED(IDC_BUTTON_ADD_3_4, OnBnClickedButtonAdd34)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_4_6, OnBnClickedButtonReduce46)
	ON_BN_CLICKED(IDC_BUTTON_ADD_4_6, OnBnClickedButtonAdd46)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_4_5, OnBnClickedButtonReduce45)
	ON_BN_CLICKED(IDC_BUTTON_ADD_4_5, OnBnClickedButtonAdd45)
	ON_BN_CLICKED(IDC_BUTTON_REDUCE_5_6, OnBnClickedButtonReduce56)
	ON_BN_CLICKED(IDC_BUTTON_ADD_5_6, OnBnClickedButtonAdd56)
	ON_BN_CLICKED(IDC_BUTTON_DETERMINE, OnBnClickedButtonDetermine)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_REPEAT, OnBnClickedButtonRepeat)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_BET, OnBnClickedButtonCloseBet)
	ON_EN_CHANGE(IDC_EDIT_1_6, OnEnChangeEdit16)
	ON_EN_CHANGE(IDC_EDIT_1_5, OnEnChangeEdit15)
	ON_EN_CHANGE(IDC_EDIT_1_4, OnEnChangeEdit14)
	ON_EN_CHANGE(IDC_EDIT_1_3, OnEnChangeEdit13)
	ON_EN_CHANGE(IDC_EDIT_1_2, OnEnChangeEdit12)
	ON_EN_CHANGE(IDC_EDIT_2_6, OnEnChangeEdit26)
	ON_EN_CHANGE(IDC_EDIT_2_5, OnEnChangeEdit25)
	ON_EN_CHANGE(IDC_EDIT_2_4, OnEnChangeEdit24)
	ON_EN_CHANGE(IDC_EDIT_2_3, OnEnChangeEdit23)
	ON_EN_CHANGE(IDC_EDIT_3_6, OnEnChangeEdit36)
	ON_EN_CHANGE(IDC_EDIT_3_5, OnEnChangeEdit35)
	ON_EN_CHANGE(IDC_EDIT_3_4, OnEnChangeEdit34)
	ON_EN_CHANGE(IDC_EDIT_4_6, OnEnChangeEdit46)
	ON_EN_CHANGE(IDC_EDIT_4_5, OnEnChangeEdit45)
	ON_EN_CHANGE(IDC_EDIT_5_6, OnEnChangeEdit56)
END_MESSAGE_MAP()


// CDialogBet 消息处理程序

BOOL CDialogPlayBet::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_ImageBackdrop.LoadImage( hInstance, TEXT("XIAZHU_FRAME")  );

	for ( int i = 0; i < AREA_ALL; ++i)
	{
		m_btAdd[i].SetButtonImage( IDB_BT_JIA,  hInstance , false,false);
		m_btReduce[i].SetButtonImage( IDB_BT_JIAN,  hInstance , false,false);
	}

	m_btBet1000.SetButtonImage( IDB_BT_1000,  hInstance , false,false);
	m_btBet1W.SetButtonImage( IDB_BT_1W,  hInstance , false,false);
	m_btBet10W.SetButtonImage( IDB_BT_10W,  hInstance , false,false);
	m_btBet100W.SetButtonImage( IDB_BT_100W,  hInstance , false,false);

	m_btDetermine.SetButtonImage( IDB_BT_ENTER,  hInstance , false,false);
	m_btReset.SetButtonImage( IDB_BT_REPLAY,  hInstance , false,false);
	m_btRepeat.SetButtonImage( IDB_BT_REPLAYLASTROUND,  hInstance , false,false);
	m_btDetermine.EnableWindow(FALSE);
	m_btRepeat.EnableWindow(FALSE);

	m_btClosee.SetButtonImage( IDB_BT_CLOSE,  hInstance , false,false);

	ModifyStyle(0, WS_CLIPCHILDREN, 0);
	SetWindowPos(NULL, 0, 0,m_ImageBackdrop.GetWidth(), m_ImageBackdrop.GetHeight(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOMOVE);
	m_btReduce[AREA_1_6].MoveWindow( 98,		87 + 1 , 23, 23 );
	m_editInput[AREA_1_6].MoveWindow( 98 + 23,	92 + 1 , 42, 13 );
	m_btAdd[AREA_1_6].MoveWindow( 98 + 23 + 42,	87 + 1 , 23, 23 );

	m_btReduce[AREA_1_5].MoveWindow( 186,		87 + 1 , 23, 23 );
	m_editInput[AREA_1_5].MoveWindow( 186 + 23,	92 + 1 , 42, 13 );
	m_btAdd[AREA_1_5].MoveWindow( 186 + 23 + 42,87 + 1 , 23, 23 );

	m_btReduce[AREA_1_4].MoveWindow( 275,		87 + 1 , 23, 23 );
	m_editInput[AREA_1_4].MoveWindow( 275 + 23,	92 + 1 , 42, 13 );
	m_btAdd[AREA_1_4].MoveWindow( 275 + 23 + 42,87 + 1 , 23, 23 );

	m_btReduce[AREA_1_3].MoveWindow( 363,		87 + 1 , 23, 23 );
	m_editInput[AREA_1_3].MoveWindow( 363 + 23,	92 + 1 , 42, 13 );
	m_btAdd[AREA_1_3].MoveWindow( 363 + 23 + 42,87 + 1 , 23, 23 );

	m_btReduce[AREA_1_2].MoveWindow( 451,		87 + 1 , 23, 23 );
	m_editInput[AREA_1_2].MoveWindow( 451 + 23,	92 + 1 , 42, 13 );
	m_btAdd[AREA_1_2].MoveWindow( 451 + 23 + 42,87 + 1 , 23, 23 );


	m_btReduce[AREA_2_6].MoveWindow( 98,		87 + 47 + 1, 23, 23 );
	m_editInput[AREA_2_6].MoveWindow( 98 + 23,	92 + 47 + 1 , 42, 13 );
	m_btAdd[AREA_2_6].MoveWindow( 98 + 23 + 42,	87 + 47 + 1 , 23, 23 );

	m_btReduce[AREA_2_5].MoveWindow( 186,		87 + 47 + 1 , 23, 23 );
	m_editInput[AREA_2_5].MoveWindow( 186 + 23,	92 + 47 + 1 , 42, 13 );
	m_btAdd[AREA_2_5].MoveWindow( 186 + 23 + 42,87 + 47 + 1 , 23, 23 );

	m_btReduce[AREA_2_4].MoveWindow( 275,		87 + 47 + 1 , 23, 23 );
	m_editInput[AREA_2_4].MoveWindow( 275 + 23,	92 + 47 + 1 , 42, 13 );
	m_btAdd[AREA_2_4].MoveWindow( 275 + 23 + 42,87 + 47 + 1 , 23, 23 );

	m_btReduce[AREA_2_3].MoveWindow( 363,		87 + 47 + 1 , 23, 23 );
	m_editInput[AREA_2_3].MoveWindow( 363 + 23,	92 + 47 + 1 , 42, 13 );
	m_btAdd[AREA_2_3].MoveWindow( 363 + 23 + 42,87 + 47 + 1 , 23, 23 );


	m_btReduce[AREA_3_6].MoveWindow( 98,		87 + 47 * 2 + 1 , 23, 23 );
	m_editInput[AREA_3_6].MoveWindow( 98 + 23,	92 + 47 * 2 + 1 , 42, 13 );
	m_btAdd[AREA_3_6].MoveWindow( 98 + 23 + 42,	87 + 47 * 2 + 1 , 23, 23 );

	m_btReduce[AREA_3_5].MoveWindow( 186,		87 + 47 * 2 + 1 , 23, 23 );
	m_editInput[AREA_3_5].MoveWindow( 186 + 23,	92 + 47 * 2 + 1 , 42, 13 );
	m_btAdd[AREA_3_5].MoveWindow( 186 + 23 + 42,87 + 47 * 2 + 1 , 23, 23 );

	m_btReduce[AREA_3_4].MoveWindow( 275,		87 + 47 * 2 + 1 , 23, 23 );
	m_editInput[AREA_3_4].MoveWindow( 275 + 23,	92 + 47 * 2 + 1 , 42, 13 );
	m_btAdd[AREA_3_4].MoveWindow( 275 + 23 + 42,87 + 47 * 2 + 1 , 23, 23 );

	m_btReduce[AREA_4_6].MoveWindow( 98,		87 + 47 * 3 + 1, 23, 23 );
	m_editInput[AREA_4_6].MoveWindow( 98 + 23,	92 + 47 * 3 + 1, 42, 13 );
	m_btAdd[AREA_4_6].MoveWindow( 98 + 23 + 42,	87 + 47 * 3 + 1, 23, 23 );

	m_btReduce[AREA_4_5].MoveWindow( 186,		87 + 47 * 3 + 1, 23, 23 );
	m_editInput[AREA_4_5].MoveWindow( 186 + 23,	92 + 47 * 3 + 1, 42, 13 );
	m_btAdd[AREA_4_5].MoveWindow( 186 + 23 + 42,87 + 47 * 3 + 1, 23, 23 );

	m_btReduce[AREA_5_6].MoveWindow( 98,		87 + 47 * 4 + 2, 23, 23 );
	m_editInput[AREA_5_6].MoveWindow( 98 + 23,	92 + 47 * 4 + 2, 42, 13 );
	m_btAdd[AREA_5_6].MoveWindow( 98 + 23 + 42,	87 + 47 * 4 + 2, 23, 23 );

	m_rcBet1000.SetRect(310			, 220, 310 + 58		, 220 + 59);
	m_rcBet1W.SetRect(310 + 58		, 220, 310 + 58 * 2	, 220 + 59);
	m_rcBet10W.SetRect(310 + 58 * 2	, 220, 310 + 58 * 3 , 220 + 59);
	m_rcBet100W.SetRect(310 + 58 * 3, 220, 310 + 58 * 4 , 220 + 59);
	m_btBet1000.MoveWindow(m_rcBet1000);
	m_btBet1W.MoveWindow(m_rcBet1W);
	m_btBet10W.MoveWindow(m_rcBet10W);
	m_btBet100W.MoveWindow(m_rcBet100W);

	m_btDetermine.MoveWindow(320	,288, 60, 22);
	m_btReset.MoveWindow(320 + 65	,288, 60, 22);
	m_btRepeat.MoveWindow(320 + 130	,288, 79, 22);

	m_btClosee.MoveWindow( m_ImageBackdrop.GetWidth() - 26, 1, 25,25);

	return TRUE; 
}

void CDialogPlayBet::OnPaint()
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
	CFont* oldFont = DCBuffer.SelectObject(&m_MultipleFont);
	UINT nTextAlign = DCBuffer.SetTextAlign(TA_TOP|TA_LEFT);
	COLORREF oldColor = DCBuffer.SetTextColor(RGB(20,255,50));

	m_ImageBackdrop.DrawImage(&DCBuffer, 0, 0);

	TCHAR szInfo[128] = {0};
	for (int i = 0; i < AREA_ALL; ++i )
	{
		CRect rect;
		m_btReduce[i].GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.OffsetRect(0, - 23);
		rect.right += 66;

		_sntprintf(szInfo, sizeof(szInfo), TEXT("%d"), m_nMultiple[i]);
		DCBuffer.DrawText(szInfo, rect , DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );
	}

	DCBuffer.SelectObject(&m_InfoFont);
	DCBuffer.SetTextColor(RGB(250,250,0));
	_sntprintf(szInfo, sizeof(szInfo), TEXT("您的余额为：%I64d"), m_lPlayerScore);
	DCBuffer.DrawText(szInfo, CRect( 188, 286, 188 + 130, 286 + 25) , DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );

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

HBRUSH CDialogPlayBet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nDlgCtrlID = pWnd->GetDlgCtrlID();
	if(nDlgCtrlID == IDC_EDIT_1_6 || nDlgCtrlID == IDC_EDIT_1_5 || nDlgCtrlID == IDC_EDIT_1_4 || nDlgCtrlID == IDC_EDIT_1_3 || nDlgCtrlID == IDC_EDIT_1_2  
		|| nDlgCtrlID == IDC_EDIT_2_6 || nDlgCtrlID == IDC_EDIT_2_5 || nDlgCtrlID == IDC_EDIT_2_4 || nDlgCtrlID == IDC_EDIT_2_3
		|| nDlgCtrlID == IDC_EDIT_3_6 || nDlgCtrlID == IDC_EDIT_3_5 || nDlgCtrlID == IDC_EDIT_3_4
		|| nDlgCtrlID == IDC_EDIT_4_6 || nDlgCtrlID == IDC_EDIT_4_5
		|| nDlgCtrlID == IDC_EDIT_5_6)
	{
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextColor(RGB(255,255,255));
		return m_InfoBrush;
	}
	return hbr;
}

BOOL CDialogPlayBet::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDialogPlayBet::OnBnClickedButton1000()
{
	m_lTheNote = 1000l;
	m_btBet1000.MoveWindow( m_rcBet1000.left, m_rcBet1000.top - 5, m_rcBet1000.Width(), m_rcBet1000.Height());
	m_btBet1W.MoveWindow(m_rcBet1W);
	m_btBet10W.MoveWindow(m_rcBet10W);
	m_btBet100W.MoveWindow(m_rcBet100W);
}

void CDialogPlayBet::OnBnClickedButton1w()
{
	m_lTheNote = 10000l;
	m_btBet1000.MoveWindow( m_rcBet1000 );
	m_btBet1W.MoveWindow( m_rcBet1W.left, m_rcBet1W.top - 5, m_rcBet1W.Width(), m_rcBet1W.Height());
	m_btBet10W.MoveWindow( m_rcBet10W );
	m_btBet100W.MoveWindow( m_rcBet100W );
}

void CDialogPlayBet::OnBnClickedButton10w()
{
	m_lTheNote = 100000l;
	m_btBet1000.MoveWindow( m_rcBet1000 );
	m_btBet1W.MoveWindow( m_rcBet1W );
	m_btBet10W.MoveWindow( m_rcBet10W.left, m_rcBet10W.top - 5, m_rcBet10W.Width(), m_rcBet10W.Height() );
	m_btBet100W.MoveWindow( m_rcBet100W );

}

void CDialogPlayBet::OnBnClickedButton100w()
{
	m_lTheNote = 1000000l;
	m_btBet1000.MoveWindow( m_rcBet1000 );
	m_btBet1W.MoveWindow( m_rcBet1W );
	m_btBet10W.MoveWindow(m_rcBet10W);
	m_btBet100W.MoveWindow(m_rcBet100W.left, m_rcBet100W.top - 5, m_rcBet100W.Width(), m_rcBet100W.Height());
}


//区域加
void CDialogPlayBet::EditAdd(BYTE cbArea)
{
	if( cbArea >= AREA_ALL)
	{
		ASSERT(FALSE);
		return;
	}

	TCHAR szCount[64] = TEXT("");
	m_editInput[cbArea].GetWindowText(szCount, sizeof(szCount));

	LONGLONG lTemp = 0l;
	if (szCount[0] != TEXT('\0'))
		lTemp = _tstoi64(szCount);

	lTemp += m_lTheNote;
	TCHAR szBet[64] = TEXT("");
	_sntprintf(szBet, sizeof(szBet), TEXT("%I64d"), lTemp);
	m_editInput[cbArea].SetWindowText(szBet);
}
//区域减
void CDialogPlayBet::EditReduce(BYTE cbArea)
{
	if( cbArea >= AREA_ALL)
	{
		ASSERT(FALSE);
		return;
	}

	TCHAR szCount[64] = TEXT("");
	m_editInput[cbArea].GetWindowText(szCount, sizeof(szCount));

	LONGLONG lTemp = 0l;
	if (szCount[0] != TEXT('\0'))
		lTemp = _tstoi64(szCount);

	lTemp -= m_lTheNote;
	if( lTemp < 0 )
		lTemp = 0;
	TCHAR szBet[64] = TEXT("");
	if( lTemp > 0 )
	{
		_sntprintf(szBet, sizeof(szBet), TEXT("%I64d"), lTemp);
	}
	m_editInput[cbArea].SetWindowText(szBet);
}

//区域限制
void CDialogPlayBet::EditLimit(BYTE cbArea)
{
	if( cbArea >= AREA_ALL)
	{
		ASSERT(FALSE);
		return;
	}

	TCHAR szCount[64] = TEXT("");
	m_editInput[cbArea].GetWindowText(szCount, sizeof(szCount));
	
	LONGLONG lTemp = 0l;
	if (szCount[0] != TEXT('\0'))
		lTemp = _tstoi64(szCount);
	
	if ( lTemp > MAX_BET || lTemp < 0l )
	{
		if( lTemp > MAX_BET )
			lTemp = MAX_BET;
		else if ( lTemp < 0l )
			lTemp = 0l;

		TCHAR szBet[64] = TEXT("");
		_sntprintf(szBet, sizeof(szBet), TEXT("%I64d"), lTemp);
		m_editInput[cbArea].SetWindowText(szBet);
	}
}


void CDialogPlayBet::OnBnClickedButtonReduce16()
{
	EditReduce(AREA_1_6);
}

void CDialogPlayBet::OnBnClickedButtonAdd16()
{
	EditAdd(AREA_1_6);
}

void CDialogPlayBet::OnBnClickedButtonReduce15()
{
	EditReduce(AREA_1_5);
}

void CDialogPlayBet::OnBnClickedButtonAdd15()
{
	EditAdd(AREA_1_5);
}

void CDialogPlayBet::OnBnClickedButtonReduce14()
{
	EditReduce(AREA_1_4);
}

void CDialogPlayBet::OnBnClickedButtonAdd14()
{
	EditAdd(AREA_1_4);
}

void CDialogPlayBet::OnBnClickedButtonReduce13()
{
	EditReduce(AREA_1_3);
}

void CDialogPlayBet::OnBnClickedButtonAdd13()
{
	EditAdd(AREA_1_3);
}

void CDialogPlayBet::OnBnClickedButtonReduce12()
{
	EditReduce(AREA_1_2);
}

void CDialogPlayBet::OnBnClickedButtonAdd12()
{
	EditAdd(AREA_1_2);
}

void CDialogPlayBet::OnBnClickedButtonReduce26()
{
	EditReduce(AREA_2_6);
}

void CDialogPlayBet::OnBnClickedButtonAdd26()
{
	EditAdd(AREA_2_6);
}

void CDialogPlayBet::OnBnClickedButtonReduce25()
{
	EditReduce(AREA_2_5);
}

void CDialogPlayBet::OnBnClickedButtonAdd25()
{
	EditAdd(AREA_2_5);
}

void CDialogPlayBet::OnBnClickedButtonReduce24()
{
	EditReduce(AREA_2_4);
}

void CDialogPlayBet::OnBnClickedButtonAdd24()
{
	EditAdd(AREA_2_4);
}

void CDialogPlayBet::OnBnClickedButtonReduce23()
{
	EditReduce(AREA_2_3);
}

void CDialogPlayBet::OnBnClickedButtonAdd23()
{
	EditAdd(AREA_2_3);
}

void CDialogPlayBet::OnBnClickedButtonReduce36()
{
	EditReduce(AREA_3_6);
}

void CDialogPlayBet::OnBnClickedButtonAdd36()
{
	EditAdd(AREA_3_6);
}

void CDialogPlayBet::OnBnClickedButtonReduce35()
{
	EditReduce(AREA_3_5);
}

void CDialogPlayBet::OnBnClickedButtonAdd35()
{
	EditAdd(AREA_3_5);
}

void CDialogPlayBet::OnBnClickedButtonReduce34()
{
	EditReduce(AREA_3_4);
}

void CDialogPlayBet::OnBnClickedButtonAdd34()
{
	EditAdd(AREA_3_4);
}

void CDialogPlayBet::OnBnClickedButtonReduce46()
{
	EditReduce(AREA_4_6);
}

void CDialogPlayBet::OnBnClickedButtonAdd46()
{
	EditAdd(AREA_4_6);
}

void CDialogPlayBet::OnBnClickedButtonReduce45()
{
	EditReduce(AREA_4_5);
}

void CDialogPlayBet::OnBnClickedButtonAdd45()
{
	EditAdd(AREA_4_5);
}

void CDialogPlayBet::OnBnClickedButtonReduce56()
{
	EditReduce(AREA_5_6);
}

void CDialogPlayBet::OnBnClickedButtonAdd56()
{
	EditAdd(AREA_5_6);
}

void CDialogPlayBet::OnEnChangeEdit16()
{
	EditLimit(AREA_1_6);
}

void CDialogPlayBet::OnEnChangeEdit15()
{
	EditLimit(AREA_1_5);
}

void CDialogPlayBet::OnEnChangeEdit14()
{
	EditLimit(AREA_1_4);
}

void CDialogPlayBet::OnEnChangeEdit13()
{
	EditLimit(AREA_1_3);
}

void CDialogPlayBet::OnEnChangeEdit12()
{
	EditLimit(AREA_1_2);
}

void CDialogPlayBet::OnEnChangeEdit26()
{
	EditLimit(AREA_2_6);
}


void CDialogPlayBet::OnEnChangeEdit25()
{
	EditLimit(AREA_2_5);
}

void CDialogPlayBet::OnEnChangeEdit24()
{
	EditLimit(AREA_2_4);
}

void CDialogPlayBet::OnEnChangeEdit23()
{
	EditLimit(AREA_2_3);
}

void CDialogPlayBet::OnEnChangeEdit36()
{
	EditLimit(AREA_3_6);
}

void CDialogPlayBet::OnEnChangeEdit35()
{
	EditLimit(AREA_3_5);
}

void CDialogPlayBet::OnEnChangeEdit34()
{
	EditLimit(AREA_3_4);
}

void CDialogPlayBet::OnEnChangeEdit46()
{
	EditLimit(AREA_4_6);
}

void CDialogPlayBet::OnEnChangeEdit45()
{
	EditLimit(AREA_4_5);
}

void CDialogPlayBet::OnEnChangeEdit56()
{
	EditLimit(AREA_5_6);
}


//确认
void CDialogPlayBet::OnBnClickedButtonDetermine()
{
	LONGLONG lAllScore = 0l;
	for ( int i = 0 ; i < AREA_ALL; ++i)
	{
		TCHAR szCount[64] = {0};
		m_editInput[i].GetWindowText(szCount, sizeof(szCount));
		m_lLastRound[i] = _tstoi64(szCount);
		lAllScore += m_lLastRound[i];
	}
	if ( lAllScore > m_lPlayerScore )
	{
		CDialogMessage Message;
		Message.SetMessage(TEXT("您的余额不足，请充值！"));
		Message.DoModal();
		return;
	}

	if ( lAllScore > 0 )
	{
		for ( int i = 0 ; i < AREA_ALL; ++i)
		{
			m_editInput[i].SetWindowText(TEXT(""));
		}

		CMD_C_PlayerBet stPlayerBet;
		memcpy(stPlayerBet.lBetScore, m_lLastRound, sizeof(m_lLastRound));
		CGameFrameEngine::GetInstance()->SendMessage(IDM_PLAYER_BET, 0, (LPARAM)(&stPlayerBet));
	}
}

//重置
void CDialogPlayBet::OnBnClickedButtonReset()
{
	for ( int i = 0 ; i < AREA_ALL; ++i)
	{
		m_editInput[i].SetWindowText(TEXT(""));
	}
}

//重复
void CDialogPlayBet::OnBnClickedButtonRepeat()
{
	for ( int i = 0 ; i < AREA_ALL; ++i)
	{
		TCHAR szBet[64] = {0};
		if( m_lLastRound[i] > 0 )
		{
			_sntprintf(szBet, sizeof(szBet), TEXT("%I64d"), m_lLastRound[i]);
		}
		m_editInput[i].SetWindowText(szBet);
	}
}

//关闭
void CDialogPlayBet::OnBnClickedButtonCloseBet()
{
	ShowWindow(SW_HIDE);
}

//设置积分
void CDialogPlayBet::SetScore( LONGLONG lScore )
{
	if( m_lPlayerScore != lScore )
	{
		m_lPlayerScore = lScore;

		if(GetSafeHwnd() && IsWindowVisible())
		{
			Invalidate(FALSE);
		}
	}
}

//设置是否能下注
void CDialogPlayBet::SetCanBet( BOOL bCanBet)
{
	if( m_btDetermine.GetSafeHwnd() && m_btRepeat.GetSafeHwnd() )
	{
		m_btDetermine.EnableWindow(bCanBet);
		m_btRepeat.EnableWindow(bCanBet);
	}
}

//设置倍数
void CDialogPlayBet::SetMultiple( INT nMultiple[AREA_ALL] )
{
	memcpy(m_nMultiple, nMultiple, sizeof(m_nMultiple));
	if(GetSafeHwnd() && IsWindowVisible())
	{
		Invalidate(FALSE);
	}
}

