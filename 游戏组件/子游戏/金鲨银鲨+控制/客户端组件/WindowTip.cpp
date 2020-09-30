#include "StdAfx.h"
#include ".\windowtip.h"

CWindowTip::CWindowTip(void)
{
	m_ptBenchmark.SetPoint(0, 0);
	m_sizeStage.SetSize(0, 0);
	m_bKeepOut = FALSE;
	m_nKeepOutAlpha = 0;
	ZeroMemory(m_pImageAnimal, sizeof(m_pImageAnimal));
	ZeroMemory(m_bAnimalFlicker, sizeof(m_bAnimalFlicker));
	ZeroMemory(m_ptAnimalSite, sizeof(m_ptAnimalSite));
}

CWindowTip::~CWindowTip(void)
{
}

//动画消息
VOID CWindowTip::OnWindowMovie()
{
	if ( m_bKeepOut )
	{
		m_nKeepOutAlpha += 20;
		if ( m_nKeepOutAlpha > 255)
			m_nKeepOutAlpha = 255;
	}
	else
	{
		m_nKeepOutAlpha -= 20;
		if ( m_nKeepOutAlpha < 0)
			m_nKeepOutAlpha = 0;
	}
}

//创建消息
VOID CWindowTip::OnWindowCreate( CD3DDevice * pD3DDevice )
{
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageKeepOut.LoadImage(pD3DDevice, hResInstance, TEXT("KEEP_OUT_ON_BET"), TEXT("MAIN"));
	
}

//鼠标事件
VOID CWindowTip::OnEventMouse( UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos )
{

}

//按钮事件
VOID CWindowTip::OnEventButton( UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos )
{

}

//绘画窗口
VOID CWindowTip::OnEventDrawWindow( CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos )
{
	if( m_nKeepOutAlpha > 0 )
		m_ImageKeepOut.DrawImage(pD3DDevice, m_ptBenchmark.x, m_ptBenchmark.y + m_sizeStage.cy - m_ImageKeepOut.GetHeight() - 1, m_nKeepOutAlpha);

	if ( m_bKeepOut )
	{
		int nTime = GetTickCount();
		for ( int i = 0; i < ANIMAL_MAX; ++i )
		{
			if ( m_bAnimalFlicker[i] && m_pImageAnimal[i] )
			{
				BYTE nFlickerAlpha = 0;
				int nCount = nTime%500;
				if ( nCount < 250 )
				{
					nFlickerAlpha = nCount;
				}
				else
				{
					nFlickerAlpha = 500 - nCount;
				}	

				//绘画底图
				m_pImageAnimal[i]->DrawImage(pD3DDevice,m_ptAnimalSite[i].x,m_ptAnimalSite[i].y,m_pImageAnimal[i]->GetWidth()/5,m_pImageAnimal[i]->GetHeight(),0,0);

				//绘画过渡
				m_pImageAnimal[i]->DrawImage(pD3DDevice,m_ptAnimalSite[i].x,m_ptAnimalSite[i].y,m_pImageAnimal[i]->GetWidth()/5,m_pImageAnimal[i]->GetHeight(),m_pImageAnimal[i]->GetWidth()/5*3,0,nFlickerAlpha);
			}
		}
	}
}
