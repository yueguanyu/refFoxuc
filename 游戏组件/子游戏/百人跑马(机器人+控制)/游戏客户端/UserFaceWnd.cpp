#include "StdAfx.h"
#include ".\userfacewnd.h"

CUserFaceWnd::CUserFaceWnd(void)
{
	m_pFrameView = NULL;
	m_pUserData = NULL;
}

CUserFaceWnd::~CUserFaceWnd(void)
{
	m_pFrameView = NULL;
	m_pUserData = NULL;
}

BEGIN_MESSAGE_MAP(CUserFaceWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CUserFaceWnd::OnPaint()
{
	CPaintDC dc(this); 

	////获取位置
	//CRect rcClient;
	//GetClientRect(&rcClient);

	////创建缓冲
	//CDC DCBuffer;
	//CBitmap ImageBuffer;
	//DCBuffer.CreateCompatibleDC(&dc);
	//ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	//CBitmap* oldBitmap = DCBuffer.SelectObject(&ImageBuffer);

	//DCBuffer.FillSolidRect(0, 0, rcClient.Width(), rcClient.Height(), RGB(255, 255, 255));
	//if ( m_pFrameView != NULL && m_pUserData != NULL )
	//{
	//	m_pFrameView->DrawUserAvatar(&DCBuffer, m_pUserData, 2, 1, m_pUserData->cbUserStatus == US_OFFLINE );
	//}

	////绘画界面
	//dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	////清理资源
	//DCBuffer.SelectObject(oldBitmap);
	//DCBuffer.DeleteDC();
	//ImageBuffer.DeleteObject();
}

BOOL CUserFaceWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//设置绘画指针
void CUserFaceWnd::SetFrameView( CGameFrameViewD3D* pFrameView )
{
	m_pFrameView = pFrameView;
}

//设置头像指针
void CUserFaceWnd::SetUserData(const IClientUserItem* pUserData )
{
	if( pUserData != m_pUserData )
	{
		m_pUserData = pUserData;
		if(GetSafeHwnd())
		{
			Invalidate(FALSE);
		}
	}
}