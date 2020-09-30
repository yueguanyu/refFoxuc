#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameTax=0L;
	m_wPlayTime=0;
	m_lGameResult=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));
	memset(m_bUserAdd,0,sizeof(m_bUserAdd));
	memset(m_iOtherAdd,0,sizeof(m_iOtherAdd));

	//加载资源
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_GAME_END);
	m_ImageWinLose.LoadFromResource(AfxGetInstanceHandle(),IDB_WIN_LOSE);
	m_PngBack.LoadImage(AfxGetInstanceHandle(),TEXT("GAME_END"));
	m_PngWinLose.LoadImage(AfxGetInstanceHandle(),TEXT("WINLOSR"));
	m_ImageNumber.LoadImage(AfxGetInstanceHandle(),TEXT("NUMBER"));
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
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));
	memset(m_bUserAdd,0,sizeof(m_bUserAdd));
	memset(m_iOtherAdd,0,sizeof(m_iOtherAdd));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//居中窗口
	CenterWindow(GetParent());

	//创建资源管理器
	m_pSkinResourceManager= (CSkinResourceManager *)CSkinResourceManager::GetInstance();
	ASSERT(m_pSkinResourceManager!=NULL);

	//if (ImageHandle.IsResourceValid())
	{
		//创建临时图
		CBitImage BufferImage;
		//CBitImage ImageHandle;
		int nImageWidth=m_PngBack.GetWidth();
		int nImageHeight=m_PngBack.GetHeight();
		BufferImage.Create(nImageWidth,nImageHeight,32);
		//ImageHandle.BitBlt(BufferImage.GetDC(),0,0,nImageWidth,nImageHeight,0,0);
		if(BufferImage.IsNull() != NULL)
			BufferImage.ReleaseDC();

		//创建区域
		CRgn ImageRgn;
		BufferImage.CreateImageRegion( ImageRgn, RGB( 255, 255, 255 ) );
		if ( (( HRGN )ImageRgn) !=NULL)
		{
			SetWindowPos(NULL,0,0,nImageWidth,nImageHeight-20,SWP_NOMOVE);
			SetWindowRgn(( HRGN )ImageRgn,TRUE);

			//设置透明
			ModifyStyleEx(0,0x00080000);
			HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL")); 
			if(hInst) 
			{ 
				typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
				MYFUNC fun = NULL;
				//取得SetLayeredWindowAttributes（）函数指针 
				fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
				if(fun)fun(this->GetSafeHwnd(),0,250,1); 
				FreeLibrary(hInst); 
			}   

			ImageRgn.DeleteObject();

			return true;
		}
	}

	return TRUE;
}

//重画函数
void CScoreView::OnPaint() {

	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(m_pSkinResourceManager->GetDefaultFont());

	//绘画背景
	//CImageHandle ImageHandle(&m_ImageBack);
	//m_ImageBack.BitBlt(dc,0,0);
	m_PngBack.DrawImage(&dc,0,0);

	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		//用户名字
		rcDraw.left=176;
		rcDraw.right=247;
		rcDraw.top=i*45+137;
		rcDraw.bottom=rcDraw.top+168;
		dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),"111");
		//dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=367;
		rcDraw.right=426;
		_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),m_lGameScore[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户加倍
		rcDraw.left=267;
		rcDraw.right=426;
		_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_bUserAdd[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//绘画标志
		int nWinLoseWidth = m_PngWinLose.GetWidth() / 6;
		int nWinLoseHeight = m_PngWinLose.GetHeight();
		int nDesPosX = ( 296 - 206 - nWinLoseWidth ) / 2 + 235;
		int nDesPoxY = ( 28 - nWinLoseHeight ) / 2 + 135 + i * 45;
		if ( m_lGameScore[ i ] == 0 ) 
		{
			m_PngWinLose.DrawImage(&dc,nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 0 * nWinLoseWidth, 0);
		}
		else if ( 0 < m_lGameScore[ i ] ) 
		{
			m_PngWinLose.DrawImage(&dc,nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 0 * nWinLoseWidth, 0);
		}
		else 
		{
			m_PngWinLose.DrawImage(&dc,nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 1 * nWinLoseWidth, 0);
		}
	}


	//其他加倍
	for(BYTE i=0;i<5;i++)
	{
		rcDraw.left=96;
		rcDraw.right=147;
		rcDraw.top=i*36+118;
		rcDraw.bottom=rcDraw.top+68;
		_sntprintf(szBuffer,sizeof(szBuffer),TEXT("X %d"),m_iOtherAdd[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	}

	//局数
	rcDraw.left=226;
	rcDraw.right=307;
	rcDraw.top=i*36+105;
	rcDraw.bottom=rcDraw.top+68;
	//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%Id"),m_wPlayTime);
	//dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	DrawNumber(&dc,rcDraw.left,rcDraw.top,m_wPlayTime);

	//成绩
	rcDraw.left=356;
	rcDraw.right=397;
	rcDraw.top=i*36+105;
	rcDraw.bottom=rcDraw.top+68;
	_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),m_lGameResult);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
	//DrawNumber(&dc,rcDraw.left,rcDraw.top,m_lGameResult);

	//显示税收
	rcDraw.left=26;
	rcDraw.right=117;
	rcDraw.top=i*28+88;
	rcDraw.bottom=rcDraw.top+28;
	LPCTSTR pszTax=TEXT("交易税");
//	dc.DrawText(pszTax,lstrlen(pszTax),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//显示税收
	rcDraw.left=117;
	rcDraw.right=206;
	rcDraw.top=i*28+88;
	rcDraw.bottom=rcDraw.top+28;
	_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),m_lGameTax);
//	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);	

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
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

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
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,BYTE *pUserAdd,int *iOtherAdd,WORD wPlayTime,LONGLONG lGameResult)
{
	//设置变量
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		m_wPlayTime=wPlayTime;
		m_lGameResult=lGameResult;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		CopyMemory(m_bUserAdd,pUserAdd,sizeof(m_bUserAdd));
		CopyMemory(m_iOtherAdd,iOtherAdd,sizeof(m_iOtherAdd));
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//数字
void CScoreView::DrawNumber(CDC *pDC,int x,int y,int iNum)
{
	int iWidth=m_ImageNumber.GetWidth()/10;
	int iNumber=1;
	BYTE NumberSize=0;
	while(iNum >= iNumber)
	{
		if(iNum > 10000)
			m_ImageNumber.DrawImage(pDC,x+iWidth*5-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
		else if(iNum > 1000)
			m_ImageNumber.DrawImage(pDC,x+iWidth*4-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
		else if(iNum > 100)
			m_ImageNumber.DrawImage(pDC,x+iWidth*3-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
		else if(iNum > 10)
			m_ImageNumber.DrawImage(pDC,x+iWidth*2-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);
		else if(iNum > 0)
			m_ImageNumber.DrawImage(pDC,x-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),iWidth*((iNum/iNumber)%10),0);

		iNumber*=10;
		NumberSize++;
	}
	//m_ImageNumber.DrawImage(pDC,x-NumberSize*iWidth,y,iWidth,m_ImageNumber.GetHeight(),0,0);
}
//////////////////////////////////////////////////////////////////////////
