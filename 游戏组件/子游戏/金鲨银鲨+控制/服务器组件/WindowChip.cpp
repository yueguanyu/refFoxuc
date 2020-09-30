#include "StdAfx.h"
#include ".\windowchip.h"

//////////////////////////////////////////////////////////////////////////
//按钮
#define IDC_CHIP_100							1101
#define IDC_CHIP_1000							1102
#define IDC_CHIP_10000							1103
#define IDC_CHIP_MAX							1104
#define IDC_CHIP_EMPTY							1105
#define IDC_CHIP_OK								1106
#define IDC_CHIP_CLOSE							1107
#define IDC_CHIP_CLOSE_X						1108

//////////////////////////////////////////////////////////////////////////
CWindowChip::CWindowChip(void)
{
	m_lCellScore = 1;
	m_lPlayOriginalScore = 0;
	m_lPlayScore = 0;
	m_lPlayChip = 0;
}

CWindowChip::~CWindowChip(void)
{
}

//动画消息
VOID CWindowChip::OnWindowMovie()
{

}

//创建消息
VOID CWindowChip::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	//载入背景
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadImage(pD3DDevice, hResInstance, TEXT("CHIP_BACK"), TEXT("CHIP"));

	CRect rcCreate(0,0,0,0);
	m_btChip100.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_100,GetVirtualEngine(),this);			
	m_btChip1000.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_1000,GetVirtualEngine(),this);		
	m_btChip10000.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_10000,GetVirtualEngine(),this);		
	m_btChipMax.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_MAX,GetVirtualEngine(),this);		
	m_btChipEmpty.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_EMPTY,GetVirtualEngine(),this);	

	m_btOK.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CHIP_OK,GetVirtualEngine(),this);			
	m_btClose.ActiveWindow(rcCreate,WS_VISIBLE,IDC_CHIP_CLOSE,GetVirtualEngine(),this);		
	m_btCloseX.ActiveWindow(rcCreate,WS_VISIBLE,IDC_CHIP_CLOSE_X,GetVirtualEngine(),this);	

	m_btChip100.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_100"),TEXT("CHIP"),hResInstance);
	m_btChip1000.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_1000"),TEXT("CHIP"),hResInstance);
	m_btChip10000.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_10000"),TEXT("CHIP"),hResInstance);
	m_btChipMax.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_MAX"),TEXT("CHIP"),hResInstance);
	m_btChipEmpty.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_EMPTY"),TEXT("CHIP"),hResInstance);

	m_btOK.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_OK"),TEXT("CHIP"),hResInstance);
	m_btClose.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_CLOSE"),TEXT("CHIP"),hResInstance);
	m_btCloseX.SetButtonImage(pD3DDevice, TEXT("BT_CHIP_CLOSE_X"),TEXT("CHIP"),hResInstance);

	//创建字体
	m_D3DFontText.CreateFont( 100, TEXT("宋体"), 0 );

	LOGFONT LogFont;
	ZeroMemory(&LogFont, sizeof(LogFont));
	LogFont.lfHeight = 14;
	LogFont.lfWeight = 800;
	_sntprintf(LogFont.lfFaceName, CountArray(LogFont.lfFaceName), TEXT("宋体"));
	m_D3DFontBold.CreateFont( LogFont, 0 );

	//获取大小
	CSize SizeGameOver;
	SizeGameOver.SetSize(m_ImageBack.GetWidth(), m_ImageBack.GetHeight());

	//移动窗口
	SetWindowPos(0,0,SizeGameOver.cx,SizeGameOver.cy,0);

	//设置按钮位置
	CVirtualButton* pVirtualButton[] = { &m_btChip100, &m_btChip1000, &m_btChip10000, &m_btChipMax, &m_btChipEmpty };
	int nOffSet = 0;
	for ( int i = 0; i < CountArray(pVirtualButton); ++i )
	{
		pVirtualButton[i]->SetWindowPos( 20 + nOffSet, 100, 0, 0, SWP_NOSIZE );

		CRect rcButton;
		pVirtualButton[i]->GetClientRect(rcButton);
		nOffSet += rcButton.Width() + 5;
	}

	m_btOK.SetWindowPos( SizeGameOver.cx/2 - 100, 210, 0, 0, SWP_NOSIZE);
	m_btClose.SetWindowPos( SizeGameOver.cx/2 + 19, 210, 0, 0, SWP_NOSIZE);
	m_btCloseX.SetWindowPos( SizeGameOver.cx - 30, 15, 0, 0, SWP_NOSIZE);
}

//鼠标事件
VOID CWindowChip::OnEventMouse( UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos )
{

}

//按钮事件
VOID CWindowChip::OnEventButton( UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos )
{
	//按钮
	switch( uButtonID )
	{	
	case IDC_CHIP_100:
		{
			m_lPlayChip += 100;
			m_lPlayScore -= m_lCellScore*100;

			//按钮激活
			ButtonEnable();
		}
		break;
	case IDC_CHIP_1000:
		{
			m_lPlayChip += 1000;
			m_lPlayScore -= m_lCellScore*1000;

			//按钮激活
			ButtonEnable();
		}
		break;
	case IDC_CHIP_10000:
		{
			m_lPlayChip += 10000;
			m_lPlayScore -= m_lCellScore*10000;

			//按钮激活
			ButtonEnable();
		}
		break;
	case IDC_CHIP_MAX:
		{
			m_lPlayChip += m_lPlayScore/m_lCellScore;
			m_lPlayScore -= m_lCellScore*(m_lPlayScore/m_lCellScore);

			//按钮激活
			ButtonEnable();
		}
		break;
	case IDC_CHIP_EMPTY:
		{
			m_lPlayScore = m_lPlayOriginalScore;
			m_lPlayChip = 0;

			//按钮激活
			ButtonEnable();
		}	
		break;
	case IDC_CHIP_OK:
		{
			CGameFrameEngine::GetInstance()->SendMessage(IDM_EXCHANGE_CHIP, (WPARAM)&m_lPlayChip, 0);
			ShowWindow(false);
		}
		break;
	case IDC_CHIP_CLOSE:
	case IDC_CHIP_CLOSE_X:
		{
			ShowWindow(false);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
		}
		break;
	}
}

//绘画窗口
VOID CWindowChip::OnEventDrawWindow( CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos )
{
	m_ImageBack.DrawImage(pD3DDevice, nXOriginPos, nYOriginPos);

	CString str;
	str.Format(TEXT("兑换比例：1筹码 = %d金币"), m_lCellScore);
	DrawTextString(pD3DDevice, &m_D3DFontText, str, 
		CRect(nXOriginPos + 50, nYOriginPos + 155, nXOriginPos + 427, nYOriginPos + 170), DT_CENTER, D3DCOLOR_XRGB(0,0,0), D3DCOLOR_XRGB(255,255,255));

	LONGLONG lDifference = m_lPlayOriginalScore - m_lPlayScore;
	str.Format(TEXT("当前兑换：%I64d筹码 = %I64d金币"), m_lPlayChip, lDifference);
	DrawTextString(pD3DDevice, &m_D3DFontBold, str, 
		CRect(nXOriginPos + 50, nYOriginPos + 175, nXOriginPos + 427, nYOriginPos + 205), DT_CENTER, D3DCOLOR_XRGB(0,0,0), D3DCOLOR_XRGB(255,255,255));

}

//设置信息
VOID CWindowChip::SetChipInfo( LONG lCellScore, LONGLONG lPlayScore, LONGLONG lPlayChip)
{
	m_lCellScore = lCellScore;
	m_lPlayScore = lPlayScore;
	m_lPlayOriginalScore = lPlayScore;
	m_lPlayChip = lPlayChip;
}

//显示窗口
VOID CWindowChip::ShowWindow( bool bVisible )
{
	if ( bVisible )
	{
		//按钮激活
		ButtonEnable();
	}
	CVirtualWindow::ShowWindow( bVisible  );
}

//按钮激活
VOID CWindowChip::ButtonEnable()
{
	m_btChip100.EnableWindow( m_lPlayScore/m_lCellScore >= 100 );
	m_btChip1000.EnableWindow( m_lPlayScore/m_lCellScore >= 1000 );	
	m_btChip10000.EnableWindow( m_lPlayScore/m_lCellScore >= 10000 );
	m_btChipMax.EnableWindow(  m_lPlayScore/m_lCellScore >= 1 );
	m_btChipEmpty.EnableWindow( true );

	m_btOK.EnableWindow( m_lPlayChip > 0 );
}

//绘画字符
VOID CWindowChip::DrawTextString( CD3DDevice* pD3DDevice, CD3DFont * pD3DFont, LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame )
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
	pD3DFont->DrawText(pD3DDevice, pszString,&rcDraw,nFormat,crText);

	return;
}
