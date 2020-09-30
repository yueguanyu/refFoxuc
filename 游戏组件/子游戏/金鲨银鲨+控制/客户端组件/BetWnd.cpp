#include "StdAfx.h"
#include ".\betwnd.h"

CBetWnd::CBetWnd(void)
{
	ZeroMemory(m_lPlayAllBet, sizeof(m_lPlayAllBet));
}

CBetWnd::~CBetWnd(void)
{
}
BEGIN_MESSAGE_MAP(CBetWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()

//绘画
void CBetWnd::OnPaint()
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
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SetTextColor(RGB(250,250,250));

	CPoint ptSite[ANIMAL_MAX] = { CPoint(146, 49), CPoint(204, 49), CPoint(204,109), CPoint(204,170), 
								  CPoint(87,49), CPoint(29,49), CPoint(29,109), CPoint(29,170),
								  CPoint(146, 170), CPoint(87, 170), CPoint(87, 109), CPoint(146, 109) };
	m_ImageBack.DrawImage(&DCBuffer, 0, 0);

	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		TCHAR szInfo[MAX_PATH];
		_sntprintf(szInfo, CountArray(szInfo), TEXT("%I64d"),m_lPlayAllBet[i]);
		DrawNumber(&DCBuffer, &m_ImageNumber, TEXT("0123456789"), szInfo, ptSite[i].x, ptSite[i].y, DT_CENTER);
	}

	//DCBuffer.FillSolidRect( 0, 0, rcClient.Width(),rcClient.Height(), RGB(100,100,100));

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画
BOOL CBetWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//创建
int CBetWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadImage( hResInstance,  TEXT("BET_BACK") );
	m_ImageNumber.LoadImage( hResInstance,  TEXT("BET_NUMBER") );

	return 0;
}

// 绘画数字
void CBetWnd::DrawNumber(CDC * pDC, CPngImage* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
	// 加载资源
	INT nNumberHeight=pImageNumber->GetHeight();
	INT nNumberWidth=pImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				pImageNumber->DrawImage(pDC, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//设置所有玩家下注
void CBetWnd::SetPlayAllBet( LONGLONG lPlayAllBet[ANIMAL_MAX] )
{
	CopyMemory(m_lPlayAllBet, lPlayAllBet, sizeof(m_lPlayAllBet)); 

	if ( GetSafeHwnd() )
		Invalidate(FALSE);
}

//设置所有玩家下注
void CBetWnd::SetPlayAllBet( LONGLONG lPlayAllBet, int nAnimalIndex )
{
	m_lPlayAllBet[nAnimalIndex] = lPlayAllBet; 
	
	if ( GetSafeHwnd() )
		Invalidate(FALSE);
}
