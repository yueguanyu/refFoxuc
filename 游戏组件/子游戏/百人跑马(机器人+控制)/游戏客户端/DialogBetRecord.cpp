// DialogBetRecord.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogBetRecord.h"
#include ".\dialogbetrecord.h"


BEGIN_MESSAGE_MAP(CColorHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//标题栏重绘
void CColorHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,RGB(70,76,79));		//重绘标题栏颜色

	int nItems = GetItemCount();
	CRect rectItem;

	CPen m_pen(PS_SOLID,1,RGB(200,200,200));		//分隔线颜色
	CFont m_font;
	m_font.CreatePointFont(90,TEXT("宋体"));				//字体
	CPen * pOldPen = dc.SelectObject(&m_pen);
	CFont * pOldFont=dc.SelectObject(&m_font);

	dc.SetTextColor(RGB(200, 243, 39));				//字体颜色

	for(int i = 0; i <nItems; i++)					//对标题的每个列进行重绘
	{  
		GetItemRect(i, &rectItem);
		rectItem.top+=2;
		rectItem.bottom+=2; 
		dc.MoveTo(rectItem.right,rect.top);         //重绘分隔栏
		dc.LineTo(rectItem.right,rectItem.bottom);

		TCHAR buf[256];
		HD_ITEM hditem;

		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( i, &hditem );                      //获取当然列的文字

		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP |DT_CENTER | DT_END_ELLIPSIS ;
		dc.DrawText(buf, &rectItem, uFormat);       //重绘标题栏的文字
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
	m_pen.DeleteObject();
	m_font.DeleteObject();
}

BOOL CColorHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//列表
BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CMyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//屏蔽鼠标操作
	return;
}


// CDialogBetRecord 对话框

IMPLEMENT_DYNAMIC(CDialogBetRecord, CDialog)
CDialogBetRecord::CDialogBetRecord(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogBetRecord::IDD, pParent)
{
	m_InfoFont.CreateFont(12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
}

CDialogBetRecord::~CDialogBetRecord()
{
	m_InfoFont.DeleteObject();
}

void CDialogBetRecord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BET_LOG, m_listBetRecord);
	DDX_Control(pDX, IDC_BUTTON_BR_OK, m_btClosee);
}


BEGIN_MESSAGE_MAP(CDialogBetRecord, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_BR_OK, OnBnClickedButtonBrOk)
END_MESSAGE_MAP()


// CDialogBetRecord 消息处理程序


BOOL CDialogBetRecord::OnInitDialog()
{
	CDialog::OnInitDialog();

	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_ImageBackdrop.LoadFromResource(hInstance,IDB_BET_RECORD);
	m_btClosee.SetButtonImage( IDB_BT_CLOSE_BL,  hInstance , false , false);
	
	SetWindowPos(NULL, 0, 0, m_ImageBackdrop.GetWidth(), m_ImageBackdrop.GetHeight(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOMOVE);

	m_listBetRecord.MoveWindow(4, 40, m_ImageBackdrop.GetWidth()-8, m_ImageBackdrop.GetHeight() - 44);
	m_btClosee.MoveWindow(m_ImageBackdrop.GetWidth() - 24, 2, 22, 22);

	m_ColorHeader.SubclassWindow(m_listBetRecord.GetHeaderCtrl()->GetSafeHwnd());

	m_listBetRecord.SetBkColor(RGB(40,46,59));
	m_listBetRecord.SetTextBkColor(RGB(40,46,59));
	m_listBetRecord.SetTextColor(RGB(255,255,255));
	m_listBetRecord.InsertColumn(0,TEXT("场次组合"),LVCFMT_CENTER, 60); 
	m_listBetRecord.InsertColumn(1,TEXT("下注"),LVCFMT_CENTER, 40); 
	m_listBetRecord.InsertColumn(2,TEXT("得分"),LVCFMT_CENTER, 40); 
	m_listBetRecord.InsertColumn(3,TEXT("时间"),LVCFMT_CENTER, m_ImageBackdrop.GetWidth() - 8 - 140); 

	return TRUE;  
}

BOOL CDialogBetRecord::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDialogBetRecord::OnPaint()
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
	COLORREF oldColor = DCBuffer.SetTextColor(RGB(250,250,0));

	m_ImageBackdrop.BitBlt(DCBuffer.GetSafeHdc(), 0, 0);

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

//区域辨认
CString CDialogBetRecord::IdentifyAreas( BYTE cbArea )
{
	CString str = TEXT("");
	if ( cbArea == AREA_1_6 )
	{
		str = TEXT("1-6");
	}
	else if( cbArea == AREA_1_5 )
	{
		str = TEXT("1-5");
	}
	else if( cbArea == AREA_1_4 )
	{
		str = TEXT("1-4");
	}
	else if( cbArea == AREA_1_3 )
	{
		str = TEXT("1-3");
	}
	else if( cbArea == AREA_1_2 )
	{
		str = TEXT("1-2");
	}
	else if( cbArea == AREA_2_6 )
	{
		str = TEXT("2-6");
	}
	else if( cbArea == AREA_2_5 )
	{
		str = TEXT("2-5");
	}
	else if( cbArea == AREA_2_4 )
	{
		str = TEXT("2-4");
	}
	else if( cbArea == AREA_2_3 )
	{
		str = TEXT("2-3");
	}
	else if( cbArea == AREA_3_6 )
	{
		str = TEXT("3-6");
	}
	else if( cbArea == AREA_3_5 )
	{
		str = TEXT("3-5");
	}
	else if( cbArea == AREA_3_4 )
	{
		str = TEXT("3-4");
	}
	else if( cbArea == AREA_4_6 )
	{
		str = TEXT("4-6");
	}
	else if( cbArea == AREA_4_5 )
	{
		str = TEXT("4-5");
	}
	else if( cbArea == AREA_5_6 )
	{
		str = TEXT("5-6");
	}
	else 
	{
		ASSERT(FALSE);
	}
	return str;
}

//添加信息
void CDialogBetRecord::AddInfo( BetRecordInfo* pInfo )
{
	
	TCHAR szInfo[64] = TEXT("");
	_sntprintf(szInfo, sizeof(szInfo), TEXT("%d：%s"), pInfo->nStreak, IdentifyAreas(pInfo->nRanking));
	m_listBetRecord.InsertItem(m_listBetRecord.GetItemCount(),szInfo);

	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), pInfo->lBet);
	m_listBetRecord.SetItemText(m_listBetRecord.GetItemCount()-1, 1, szInfo);

	_sntprintf(szInfo, sizeof(szInfo), TEXT("%I64d"), pInfo->lWin);
	m_listBetRecord.SetItemText(m_listBetRecord.GetItemCount()-1, 2, szInfo);

	_sntprintf(szInfo, sizeof(szInfo), TEXT("%d:%d"), pInfo->nHours, pInfo->nMinutes);
	m_listBetRecord.SetItemText(m_listBetRecord.GetItemCount()-1, 3, szInfo);

	if(m_listBetRecord.GetSafeHwnd() && IsWindowVisible())
	{
		m_listBetRecord.Invalidate(FALSE);
	}
}


void CDialogBetRecord::OnBnClickedButtonBrOk()
{
	ShowWindow(SW_HIDE);
}

