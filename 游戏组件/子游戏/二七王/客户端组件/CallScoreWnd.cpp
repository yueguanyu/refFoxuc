#include "StdAfx.h"
#include "CallScoreWnd.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCallScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCallScoreView::CCallScoreView()
{
	m_lCurrentCallScore=120;

	//加载资源
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_CALL_BACK);

	return;
}

//析构函数
CCallScoreView::~CCallScoreView()
{
}

//建立消息
int CCallScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	int nWidth=m_ImageBack.GetWidth();
	int nHeight=m_ImageBack.GetHeight();

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btGiveUp.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_GIVE_UP);
	m_btGiveUp.ShowWindow(SW_SHOW);

	for(BYTE i=0;i<25;i++) 
	{
		m_btScore[i].Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_CALL_SCORE+i);
		m_btScore[i].ShowWindow(SW_SHOW);
	}

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btGiveUp.SetButtonImage(IDB_CALL_GIVEUP,hInstance,false,false);
	m_btScore[ 0 ].SetButtonImage(	IDB_CALL_0		,hInstance,false,false);
	m_btScore[ 1 ].SetButtonImage(	IDB_CALL_5		,hInstance,false,false);
	m_btScore[ 2 ].SetButtonImage(	IDB_CALL_10		,hInstance,false,false);
	m_btScore[ 3 ].SetButtonImage(	IDB_CALL_15		,hInstance,false,false);
	m_btScore[ 4 ].SetButtonImage(	IDB_CALL_20		,hInstance,false,false);
	m_btScore[ 5 ].SetButtonImage(	IDB_CALL_25		,hInstance,false,false);
	m_btScore[ 6 ].SetButtonImage(	IDB_CALL_30		,hInstance,false,false);
	m_btScore[ 7 ].SetButtonImage(	IDB_CALL_35		,hInstance,false,false);
	m_btScore[ 8 ].SetButtonImage(	IDB_CALL_40		,hInstance,false,false);
	m_btScore[ 9 ].SetButtonImage(	IDB_CALL_45		,hInstance,false,false);
	m_btScore[ 10 ].SetButtonImage(	IDB_CALL_50		,hInstance,false,false);
	m_btScore[ 11 ].SetButtonImage(	IDB_CALL_55		,hInstance,false,false);
	m_btScore[ 12 ].SetButtonImage(	IDB_CALL_60		,hInstance,false,false);
	m_btScore[ 13 ].SetButtonImage(	IDB_CALL_65		,hInstance,false,false);
	m_btScore[ 14 ].SetButtonImage(	IDB_CALL_70		,hInstance,false,false);
	m_btScore[ 15 ].SetButtonImage(	IDB_CALL_75		,hInstance,false,false);
	m_btScore[ 16 ].SetButtonImage(	IDB_CALL_80		,hInstance,false,false);
	m_btScore[ 17 ].SetButtonImage(	IDB_CALL_85		,hInstance,false,false);
	m_btScore[ 18 ].SetButtonImage(	IDB_CALL_90		,hInstance,false,false);
	m_btScore[ 19 ].SetButtonImage(	IDB_CALL_95		,hInstance,false,false);
	m_btScore[ 20 ].SetButtonImage(	IDB_CALL_100	,hInstance,false,false);
	m_btScore[ 21 ].SetButtonImage(	IDB_CALL_105	,hInstance,false,false);
	m_btScore[ 22 ].SetButtonImage(	IDB_CALL_110	,hInstance,false,false);
	m_btScore[ 23 ].SetButtonImage(	IDB_CALL_115	,hInstance,false,false);
	m_btScore[ 24 ].SetButtonImage(	IDB_CALL_120	,hInstance,false,false);

	//移动按钮
	int baseX=20;
	int baseY=45;
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//按钮
	m_btGiveUp.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,(nWidth-rcButton.Width())/2+126,nHeight-rcButton.Height()-23,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 0 ]	,NULL,	baseX+rcButton.Width()*4	,		baseY+rcButton.Height()*4			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 1 ]	,NULL,	baseX+rcButton.Width()*3	,		baseY+rcButton.Height()*4			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 2 ]	,NULL,	baseX+rcButton.Width()*2	,		baseY+rcButton.Height()*4			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 3 ]	,NULL,	baseX+rcButton.Width()*1	,		baseY+rcButton.Height()*4			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 4 ]	,NULL,	baseX						,		baseY+rcButton.Height()*4			,0,0,uFlags);
																						
	DeferWindowPos(hDwp,  m_btScore[ 5 ]	,NULL,	baseX+rcButton.Width()*4	,		baseY+rcButton.Height()*3			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 6 ]	,NULL,	baseX+rcButton.Width()*3	,		baseY+rcButton.Height()*3			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 7 ]	,NULL,	baseX+rcButton.Width()*2	,		baseY+rcButton.Height()*3			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 8 ]	,NULL,	baseX+rcButton.Width()*1	,		baseY+rcButton.Height()*3			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 9 ]	,NULL,	baseX						,		baseY+rcButton.Height()*3			,0,0,uFlags);
																						
	DeferWindowPos(hDwp,  m_btScore[ 10 ]	,NULL,	baseX+rcButton.Width()*4	,		baseY+rcButton.Height()*2			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 11 ]	,NULL,	baseX+rcButton.Width()*3	,		baseY+rcButton.Height()*2			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 12 ]	,NULL,	baseX+rcButton.Width()*2	,		baseY+rcButton.Height()*2			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 13 ]	,NULL,	baseX+rcButton.Width()*1	,		baseY+rcButton.Height()*2			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 14 ]	,NULL,	baseX						,		baseY+rcButton.Height()*2			,0,0,uFlags);

	DeferWindowPos(hDwp,  m_btScore[ 15 ]	,NULL,	baseX+rcButton.Width()*4	,		baseY+rcButton.Height()*1			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 16 ]	,NULL,	baseX+rcButton.Width()*3	,		baseY+rcButton.Height()*1			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 17 ]	,NULL,	baseX+rcButton.Width()*2	,		baseY+rcButton.Height()*1			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 18 ]	,NULL,	baseX+rcButton.Width()*1	,		baseY+rcButton.Height()*1			,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 19 ]	,NULL,	baseX						,		baseY+rcButton.Height()*1			,0,0,uFlags);

	DeferWindowPos(hDwp,  m_btScore[ 20 ]	,NULL,	baseX+rcButton.Width()*4	,		baseY								,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 21 ]	,NULL,	baseX+rcButton.Width()*3	,		baseY								,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 22 ]	,NULL,	baseX+rcButton.Width()*2	,		baseY								,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 23 ]	,NULL,  baseX+rcButton.Width()*1	,		baseY								,0,0,uFlags);
	DeferWindowPos(hDwp,  m_btScore[ 24 ]	,NULL,  baseX						,		baseY								,0,0,uFlags);

	//移动控件
	EndDeferWindowPos(hDwp);

	m_btScore[24].EnableWindow(false);
	return 0;
}

//鼠标消息
void CCallScoreView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//重画函数
void CCallScoreView::OnPaint() 
{
	CPaintDC dc(this); 

	//绘画背景
	m_ImageBack.BitBlt(dc,0,0);

	//设置区域
	CRect rt;
	GetClientRect(&rt);
	CRgn WndRgn;
	WndRgn.CreateRoundRectRgn(rt.left,rt.top,rt.right+1,rt.bottom+1,10,10);
	SetWindowRgn(WndRgn,TRUE);
	WndRgn.DeleteObject();

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
//	dc.SelectObject(CSkinResourceManager::GetDefaultFont());


	return;
}

//设置当前叫分
void CCallScoreView::SetCurrentCallScore(LONGLONG lScore)
{
	m_lCurrentCallScore=lScore;

	//禁用按钮
	for(BYTE i=0;i<24;i++)
	{
		if(i*5>=m_lCurrentCallScore)	m_btScore[i].EnableWindow(false);
		else m_btScore[i].EnableWindow(true);
	}

	return;
}

//命令函数
BOOL CCallScoreView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_GIVE_UP:				//开始按钮
		{
			//发送消息
			if(m_pSinkWindow!=NULL)
				m_pSinkWindow->SendEngineMessage(IDM_LAND_SCORE,255,255);
			ShowWindow(SW_HIDE);
			return TRUE;
		}
	default:
		{
			BYTE bLandScore=(LOWORD(wParam)-IDC_CALL_SCORE)*5;
			ASSERT(bLandScore>=0&&bLandScore<=120);
			//发送消息
			if(m_pSinkWindow!=NULL)
				m_pSinkWindow->SendEngineMessage(IDM_LAND_SCORE,bLandScore,bLandScore);
			ShowWindow(SW_HIDE);
			return TRUE;
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
