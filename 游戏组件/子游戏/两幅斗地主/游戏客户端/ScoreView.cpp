#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView()
{
	//设置数据
	m_lGameTax=0L;
	ZeroMemory(m_lGameScore,sizeof(m_lGameScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_GAME_END);
	m_ImageWinLose.LoadFromResource(AfxGetInstanceHandle(),IDB_WIN_LOSE);
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//初始化函数
BOOL CScoreView::OnInitDialog()
{
	__super::OnInitDialog();

	//设置数据
	m_lGameTax=0L;
	ZeroMemory(m_lGameScore,sizeof(m_lGameScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//居中窗口
	CenterWindow(GetParent());

	//设置区域
	CRgn wndRgn;
	m_ImageBack.CreateImageRegion(wndRgn, RGB(255,0,255));
	
	if ( (( HRGN )wndRgn) !=NULL)
	{
		//移动窗口
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);
		SetWindowRgn(wndRgn, true);
		wndRgn.DeleteObject();
	}


	//创建资源管理器
	m_pSkinResourceManager= (CSkinResourceManager *)CSkinResourceManager::GetInstance();
	ASSERT(m_pSkinResourceManager!=NULL);


	return TRUE;
}

//重画函数
void CScoreView::OnPaint() {

	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	if(m_pSkinResourceManager!=NULL)
	dc.SelectObject(m_pSkinResourceManager->GetDefaultFont());

	m_ImageBack.BitBlt(dc,0,0);

	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		//用户名字
		rcDraw.left=26;
		rcDraw.right=117;
		rcDraw.top=i*28+87;
		rcDraw.bottom=rcDraw.top+28;
		dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=117;
		rcDraw.right=206;
		_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),m_lGameScore[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//绘画标志
		int nWinLoseWidth = m_ImageWinLose.GetWidth() / 3;
		int nWinLoseHeight = m_ImageWinLose.GetHeight();
		int nDesPosX = ( 296 - 206 - nWinLoseWidth ) / 2 + 200;
		int nDesPoxY = ( 28 - nWinLoseHeight ) / 2 + 79 + i * 29;
		if ( m_lGameScore[ i ] == 0 ) {
			m_ImageWinLose.TransDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 2 * nWinLoseWidth, 0, RGB( 255, 0, 255 ) );
		}
		else if ( 0 < m_lGameScore[ i ] ) {
			m_ImageWinLose.TransDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 0, 0, RGB( 255, 0, 255 ) );
		}
		else {
			m_ImageWinLose.TransDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, nWinLoseWidth, 0, RGB( 255, 0, 255 ) );
		}
	}

	////显示税收
	//rcDraw.left=26;
	//rcDraw.right=117;
	//rcDraw.top=i*28+88;
	//rcDraw.bottom=rcDraw.top+28;
	//LPCTSTR pszTax=TEXT("交易税");
	//dc.DrawText(pszTax,lstrlen(pszTax),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	////显示税收
	//rcDraw.left=117;
	//rcDraw.right=206;
	//rcDraw.top=i*28+88;
	//rcDraw.bottom=rcDraw.top+28;
	//_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),m_lGameTax);
	//dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);	

	return;
}

//绘画背景
BOOL CScoreView::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lGameTax=0L;
	ZeroMemory(m_lGameScore,sizeof(m_lGameScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//绘画视图
	if (m_hWnd!=NULL) Invalidate(NULL);

	return;
}

//设置税收
void CScoreView::SetGameTax(LONGLONG lGameTax)
{
	//设置变量
	if (m_lGameTax!=lGameTax)
	{
		m_lGameTax=lGameTax;
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore)
{
	//设置变量
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
