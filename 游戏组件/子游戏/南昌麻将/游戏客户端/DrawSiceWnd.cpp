#include "StdAfx.h"
#include "DrawSiceWnd.h"

//////////////////////////////////////////////////////////////////////////
#define IDI_SICE_POS				203									//移动定时器
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDrawSiceWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDrawSiceWnd::CDrawSiceWnd()
{
	//加载资源
	m_bSicePos=0;
	m_nXPos=0;
	m_nYPos=0;
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadFromResource(hInstance,IDB_SICE_BACK);
	
	return;
}

//析构函数
CDrawSiceWnd::~CDrawSiceWnd()
{
}

//重画函数
void CDrawSiceWnd::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap BufferImage;
	BufferDC.CreateCompatibleDC(&dc);
	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&BufferImage);
	//绘画背景
	//CImageHandle BackImageHandle(&m_ImageBack);
	//CImageHandle SiceImageFirstHandle(&m_ImageDrawSiceFirst);
	//CImageHandle SiceImageSecondHandle(&m_ImageDrawSiceSecond);
	//CImageHandle SiceImageFirstUserHandle(&m_ImageDrawSiceUserFirst);
	//CImageHandle SiceImageSecondUserHandle(&m_ImageDrawSiceUserSecond);
	//CImageHandle KingImageFirstHandle(&m_ImageKingSiceFirst);
	//CImageHandle KingImageSecondHandle(&m_ImageKingSiceSecond);

	int nImageWidth=m_ImageBack.GetWidth();
	int nImageHeight=m_ImageBack.GetHeight();
	for (int nXPos=0;nXPos<rcClient.right;nXPos+=nImageWidth)
	{
		for (int nYPos=0;nYPos<rcClient.bottom;nYPos+=nImageHeight)
		{
			m_ImageBack.BitBlt(BufferDC,nXPos,nYPos);
		}
	}
	if(m_bSicePos<9)
	{
		m_ImageDrawSiceFirst.TransDrawImage(&BufferDC,30,0,m_ImageDrawSiceFirst.GetWidth()/9,m_ImageDrawSiceFirst.GetHeight(),
			m_bSicePos*m_ImageDrawSiceFirst.GetWidth()/9,0,RGB(0,0,254));
		m_ImageDrawSiceSecond.TransDrawImage(&BufferDC,30,0,m_ImageDrawSiceSecond.GetWidth()/9,m_ImageDrawSiceSecond.GetHeight(),
			m_bSicePos*m_ImageDrawSiceSecond.GetWidth()/9,0,RGB(0,0,254));

	}
	else if(m_bSicePos<18)
	{
		m_ImageDrawSiceUserFirst.TransDrawImage(&BufferDC,30,0,m_ImageDrawSiceUserFirst.GetWidth()/9,m_ImageDrawSiceUserFirst.GetHeight(),
			m_bSicePos%9*m_ImageDrawSiceUserFirst.GetWidth()/9,0,RGB(0,0,254));
		m_ImageDrawSiceUserSecond.TransDrawImage(&BufferDC,30,0,m_ImageDrawSiceUserSecond.GetWidth()/9,m_ImageDrawSiceUserSecond.GetHeight(),
			m_bSicePos%9*m_ImageDrawSiceUserSecond.GetWidth()/9,0,RGB(0,0,254));

	}
	else
	{
		m_ImageKingSiceFirst.TransDrawImage(&BufferDC,30,0,m_ImageKingSiceFirst.GetWidth()/9,m_ImageKingSiceFirst.GetHeight(),
			m_bSicePos%9*m_ImageKingSiceFirst.GetWidth()/9,0,RGB(0,0,254));
		m_ImageKingSiceSecond.TransDrawImage(&BufferDC,30,0,m_ImageKingSiceSecond.GetWidth()/9,m_ImageKingSiceSecond.GetHeight(),
			m_bSicePos%9*m_ImageKingSiceSecond.GetWidth()/9,0,RGB(0,0,254));
	}
	//绘画界面
	
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);
	
	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//建立消息
int CDrawSiceWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	//CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOREPOSITION);
	return 0;
}
//定时器消息
void CDrawSiceWnd::OnTimer(UINT nIDEvent)
{
	if(IDI_SICE_POS==nIDEvent)
	{

		if(m_bSicePos>=27)
		{
			KillTimer(IDI_SICE_POS);
			Sleep(500);
			//AfxGetMainWnd()->PostMessage(IDM_GAME_START,NULL,NULL);
			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
			if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_GAME_START,NULL,NULL);
		}
		else if(m_bSicePos>=18)
		{
			if(m_bSicePos==18)
			{
				Sleep(500);
				//AfxGetMainWnd()->PostMessage(IDM_SICE_TWO,NULL,NULL);
				CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
				if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_SICE_TWO,NULL,NULL);
			}
			m_bSicePos++;
		}
		else
		{
			if(m_bSicePos==9)
			{
				Sleep(500);
				//AfxGetMainWnd()->PostMessage(IDM_SICE_TWO,NULL,NULL);
				CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
				if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_SICE_TWO,NULL,NULL);
			}
			m_bSicePos++;


		}

	}
	Invalidate();

	__super::OnTimer(nIDEvent);
}
void CDrawSiceWnd::SetSiceInfo(CDC *dc,BYTE bMoveTime,BYTE SiceFirst,BYTE SiceSecond,BYTE SiceUserFirst,BYTE SiceUserSecond,BYTE KingSiceFirst,BYTE KingSiceSecond)
{
	HINSTANCE hInstance=AfxGetInstanceHandle();
	ParentDC=dc;
	if(m_ImageBack.IsNull())
	m_ImageBack.LoadFromResource(hInstance,IDB_SICE_BACK);
	m_ImageDrawSiceFirst.LoadFromResource(hInstance,IDB_DRAW_SICE1+SiceFirst-1);
	m_ImageDrawSiceSecond.LoadFromResource(hInstance,IDB_DRAW_SICE7+SiceSecond-1);
	m_ImageDrawSiceUserFirst.LoadFromResource(hInstance,IDB_DRAW_SICE1+SiceUserFirst-1);
	m_ImageDrawSiceUserSecond.LoadFromResource(hInstance,IDB_DRAW_SICE7+SiceUserSecond-1);
	m_ImageKingSiceFirst.LoadFromResource(hInstance,IDB_DRAW_SICE1+KingSiceFirst-1);
	m_ImageKingSiceSecond.LoadFromResource(hInstance,IDB_DRAW_SICE7+KingSiceSecond-1);

	m_bSicePos=0;
	SetTimer(IDI_SICE_POS,bMoveTime,NULL);
	

}
//基准位置
void CDrawSiceWnd::SetBenchmarkPos(int nXPos, int nYPos)
{
	m_nXPos=nXPos;
	m_nYPos=nYPos;
}

//////////////////////////////////////////////////////////////////////////
