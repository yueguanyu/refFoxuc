#include "StdAfx.h"
#include ".\constantlybutton.h"

CConstantlyButton::CConstantlyButton(void)
{
	m_bSend = FALSE;
}

CConstantlyButton::~CConstantlyButton(void)
{
}

//动画消息
VOID CConstantlyButton::OnWindowMovie()
{
	if ( m_bMouseMove && m_bMouseDown )
	{
		if ( m_WaitingTime.GetLapseCount(500) > 0 )
		{
			m_bSend = TRUE;
		}
		if ( m_bSend && m_SendTime.GetLapseCount(30) > 0 )
		{
			//按钮事件
			if (m_bEnable==true)
			{
				if (m_pParentWindow==NULL)
				{
					//获取设备
					ASSERT(m_pVirtualEngine!=NULL);
					CD3DDevice * pD3DDevice=m_pVirtualEngine->GetD3DDevice();

					//获取窗口
					ASSERT(pD3DDevice!=NULL);
					HWND hWndDevice=pD3DDevice->GetWndDevice();

					//发送消息
					ASSERT(hWndDevice!=NULL);
					if (hWndDevice!=NULL) SendMessage(hWndDevice,WM_COMMAND,m_uWindowID,(LPARAM)hWndDevice);
				}
				else
				{
					ASSERT(FALSE);
				}
			}
		}
	}
	else
	{
		m_bSend = FALSE;
	}
	CVirtualButton::OnWindowMovie();
}


//鼠标事件
VOID CConstantlyButton::OnEventMouse( UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos )
{
	switch (uMessage)
	{
	case WM_LBUTTONDOWN:	//按下消息
	case WM_LBUTTONDBLCLK:	//双击消息
		{
			m_WaitingTime.Initialization();
			m_SendTime.Initialization();
			break;
		}
	}

	CVirtualButton::OnEventMouse( uMessage, nFlags, nXMousePos, nYMousePos );
}

