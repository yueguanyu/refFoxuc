#include "StdAfx.h"
#include ".\windowover.h"

CWindowOver::CWindowOver(void)
{
	m_lPlayScore = 0;
}

CWindowOver::~CWindowOver(void)
{
}

//动画消息
VOID CWindowOver::OnWindowMovie()
{

}

//创建消息
VOID CWindowOver::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	//载入背景
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("OVER_BACK"), TEXT("OVER"));
	m_ImageNumber.LoadImage(pD3DDevice, hResInstance, TEXT("OVER_NUMBER"), TEXT("OVER"));

	//获取大小
	CSize SizeGameOver;
	SizeGameOver.SetSize(m_ImageBack.GetWidth(), m_ImageBack.GetHeight());

	//移动窗口
	SetWindowPos(0,0,SizeGameOver.cx,SizeGameOver.cy,0);
}

//鼠标事件
VOID CWindowOver::OnEventMouse( UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos )
{
}

//按钮事件
VOID CWindowOver::OnEventButton( UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos )
{
	
}

//绘画窗口
VOID CWindowOver::OnEventDrawWindow( CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos )
{
	m_ImageBack.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos);

	TCHAR szNumber[128];
	_sntprintf(szNumber, CountArray(szNumber), TEXT("%I64d"), m_lPlayScore);
	DrawNumber(pD3DDevice, &m_ImageNumber, TEXT("-0123456789"), szNumber, nXOriginPos + 179, nYOriginPos + 76, DT_CENTER);
}


// 绘画数字
void CWindowOver::DrawNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/, BYTE cbAlpha /*= 255*/)
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
				pImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight, cbAlpha);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}


//绘画字符
VOID CWindowOver::DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont, LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame)
{
	//变量定义
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//计算位置
		CRect rcFrame;
		rcFrame.top=rcDraw.top+nYExcursion[i];
		rcFrame.left=rcDraw.left+nXExcursion[i];
		rcFrame.right=rcDraw.right+nXExcursion[i];
		rcFrame.bottom=rcDraw.bottom+nYExcursion[i];

		//绘画字符
		pD3DFont->DrawText(pD3DDevice, pszString,&rcFrame,nFormat,crFrame);
	}

	//绘画字符
	pD3DFont->DrawText(pD3DDevice, pszString,&rcDraw, nFormat, crText);

	return;
}