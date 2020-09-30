#include "StdAfx.h"
#include "resource.h"
#include ".\playoperate.h"

BEGIN_MESSAGE_MAP(CEditOperate, CEdit)
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CEditOperate::CEditOperate()
{
	m_lMaxScore = 0;
	m_lMinScore = 0;
	m_Brush.CreateSolidBrush(RGB(0,0,0));
}

CEditOperate::~CEditOperate()
{
	m_Brush.DeleteObject();
}


HBRUSH CEditOperate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	return m_Brush;
}

HBRUSH CEditOperate::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	return m_Brush;
}

void CEditOperate::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TCHAR ch = TCHAR(nChar);

	// 对当前输入字符进行判断
	if ( ch != '\b' && ch != '0' && ch != '1' && ch != '2' && ch != '3' && ch != '4' && ch != '5' && ch != '6' && ch != '7' && ch != '8' && ch != '9' )
		return;

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	CString str;
	GetWindowText(str); 

	if( !str.IsEmpty() )
	{
		while( str.GetAt(0) == '0' )
		{
			str.Delete(0);
		}

		LONGLONG lScore = m_pPlayOperate->DeleteComma(str);
		if( lScore < 0 )
		{
			SetWindowText(TEXT("0"));
		}
		else if ( lScore < m_lMinScore )
		{
			SetWindowText(m_pPlayOperate->AddComma(m_lMinScore));
		}
		else if ( lScore > m_lMaxScore )
		{
			SetWindowText(m_pPlayOperate->AddComma(m_lMaxScore));
		}
		else
		{
			SetWindowText(m_pPlayOperate->AddComma(lScore));
		}
	}
	m_pPlayOperate->SetDragScore();
	SetSel(LineLength(), LineLength(), TRUE); 
}

void CEditOperate::OnRButtonDown(UINT nFlags, CPoint point)
{
}


void CEditOperate::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == VK_DELETE )
		return;
	
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////////////
// 玩家操作
CPlayOperate::CPlayOperate(void)
{
	m_bShow = false;
	m_bMouseDown = false;
	m_lCellScore = 0;
	m_ptPos.SetPoint( 0, 0 );
	m_sizeBakc.SetSize( 0, 0 );
	m_fontScoreInput.CreateFont(12,0,0,0,800,0,0,0,0,0,0,0,0,TEXT("宋体"));
	m_edieScoreInput.m_pPlayOperate = this;
}

CPlayOperate::~CPlayOperate(void)
{
	m_fontScoreInput.DeleteObject();
}

// 初始化
void CPlayOperate::Initialization( CWnd* pParent )
{
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadImage( hResInstance, TEXT("USER_CONTROL"));
	m_ImageDrag.LoadImage( hResInstance, TEXT("ARROW_CONTROL"));
	
	m_sizeBakc.SetSize( m_ImageBack.GetWidth(), m_ImageBack.GetHeight() );
	m_sizeDrag.SetSize( m_ImageDrag.GetWidth(), m_ImageDrag.GetHeight() );

	CRect rcCreate(0,0,0,0);
	m_btOneScore.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_ADD_SCORE_ONE);
	m_btTwoScore.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_ADD_SCORE_TWO);
	m_btThreeScore.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_ADD_SCORE_THREE);
	m_btFourScore.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_ADD_SCORE_FOUR);
	m_btAddScore.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_ADD_SCORE);
	m_btFollow.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_CONTROL_FOLLOW);
	m_btGiveUp.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_CONTROL_GIVEUP);
	m_btShowHand.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_CONTROL_SHOWHAND);
	m_btADD.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_CONTROL_ADD);
	m_btSUB.Create(NULL,WS_CHILD,rcCreate,pParent,IDC_CONTROL_SUB);

	m_btOneScore.SetButtonImage(IDB_BT_ADD_SCORE,hResInstance,false,false);
	m_btTwoScore.SetButtonImage(IDB_BT_ADD_SCORE,hResInstance,false,false);
	m_btThreeScore.SetButtonImage(IDB_BT_ADD_SCORE,hResInstance,false,false);
	m_btFourScore.SetButtonImage(IDB_BT_ADD_SCORE,hResInstance,false,false);
	m_btAddScore.SetButtonImage(IDB_BT_CONTROL_CENTER,hResInstance,false,false);
	m_btFollow.SetButtonImage(IDB_BT_CONTROL_FOLLOW,hResInstance,false,false);
	m_btGiveUp.SetButtonImage(IDB_BT_CONTROL_GIVEUP,hResInstance,false,false);
	m_btShowHand.SetButtonImage(IDB_BT_CONTROL_SHOWHAND,hResInstance,false,false);
	m_btADD.SetButtonImage(IDB_BT_CONTROL_ADD,hResInstance,false,false);
	m_btSUB.SetButtonImage(IDB_BT_CONTROL_SUB,hResInstance,false,false);

	m_btOneScore.SetFont(&m_fontScoreInput);
	m_btTwoScore.SetFont(&m_fontScoreInput);
	m_btThreeScore.SetFont(&m_fontScoreInput);
	m_btFourScore.SetFont(&m_fontScoreInput);

	// 输入框
	m_edieScoreInput.Create( WS_CHILD|ES_AUTOHSCROLL, CRect(0,0,0,0), pParent, 2021 );
	m_edieScoreInput.SetFont(&m_fontScoreInput);
}

// 按钮消息
BOOL CPlayOperate::OnCommand( WPARAM wParam, LPARAM lParam )
{
	return FALSE;
}

// 绘画函数
void CPlayOperate::DrawPlayOperate( CDC * pDC, INT nWidth, INT nHeight )
{
	if ( !m_bShow )
		return;

	m_ImageBack.DrawImage( pDC, m_ptPos.x, m_ptPos.y );

	int nDragX = m_lDragSite;
	int nOffSet = m_sizeDrag.cx / 2;
	if ( nDragX < m_rectDrag.left + nOffSet )
		nDragX = m_rectDrag.left + nOffSet;
	else if ( nDragX > m_rectDrag.right - nOffSet )
		nDragX = m_rectDrag.right - nOffSet;

	m_ImageDrag.DrawImage( pDC, nDragX - nOffSet, m_rectDrag.top );
}

// 设置位置
void CPlayOperate::SetOperatePos( int nPosX, int nPosY, UINT nFlags )
{
	if ( PYOE_RIGHT&nFlags )
		m_ptPos.x = nPosX - m_sizeBakc.cx;
	else if ( PYOE_CENTER&nFlags )
		m_ptPos.x = nPosX - m_sizeBakc.cx/2;
	else
		m_ptPos.x = nPosX;


	if ( PYOE_BOTTOM&nFlags )
		m_ptPos.y = nPosY - m_sizeBakc.cy;
	else if ( PYOE_VCENTER&nFlags )
		m_ptPos.y = nPosY - m_sizeBakc.cy/2;
	else
		m_ptPos.y = nPosY;

	//按钮控件
	CRect rcButton;
	m_btOneScore.GetWindowRect(&rcButton);

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	DeferWindowPos( hDwp, m_btOneScore,  NULL, nPosX + 10, nPosY + 18, 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btTwoScore,  NULL, nPosX + 10, nPosY + 18 + rcButton.Height(), 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btThreeScore,NULL, nPosX + 10, nPosY + 18 + rcButton.Height() * 2, 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btFourScore, NULL, nPosX + 10, nPosY + 18 + rcButton.Height() * 3, 0, 0, uFlags );

	DeferWindowPos( hDwp, m_btFollow,   NULL, nPosX + 98, nPosY + 18 , 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btShowHand, NULL, nPosX + 98, nPosY + 55 , 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btGiveUp,   NULL, nPosX + 98, nPosY + 100 , 0, 0, uFlags );
	
	DeferWindowPos( hDwp, m_btAddScore, NULL, nPosX + 143, nPosY + 142 , 0, 0, uFlags );

	DeferWindowPos( hDwp, m_btADD, NULL, nPosX + 129, nPosY + 164 , 0, 0, uFlags );
	DeferWindowPos( hDwp, m_btSUB, NULL, nPosX + 8, nPosY + 164 , 0, 0, uFlags );

	//结束移动
	EndDeferWindowPos(hDwp);


	CRect rcButtonOver;
	m_btOneScore.GetWindowRect(&rcButtonOver);
	m_lDragSite += rcButtonOver.left - rcButton.left;

	m_edieScoreInput.MoveWindow( nPosX + 17, nPosY + 146, 118, 12);

	m_rectDrag.SetRect( nPosX + 23, nPosY + 164, nPosX + 127, nPosY + 183 );
}

// 设置信息
void CPlayOperate::SetCellScore( LONG lCellScore )
{
	m_lCellScore = lCellScore;

	CString str;
	str.Format(TEXT("%d"), lCellScore);
	m_btOneScore.SetWindowText(str);

	str.Format(TEXT("%d"), lCellScore * 2);
	m_btTwoScore.SetWindowText(str);

	str.Format(TEXT("%d"), lCellScore * 3);
	m_btThreeScore.SetWindowText(str);

	str.Format(TEXT("%d"), lCellScore * 4);
	m_btFourScore.SetWindowText(str);
}

// 设置最大数
void CPlayOperate::SetBetCount(  LONGLONG lMaxBetCount, LONGLONG lMinBetCount  )
{
	m_edieScoreInput.m_lMaxScore = lMaxBetCount;
	m_edieScoreInput.m_lMinScore = lMinBetCount;

	if ( m_edieScoreInput.m_lMaxScore < 0 )
		m_edieScoreInput.m_lMaxScore = 0;

	if ( m_edieScoreInput.m_lMinScore < 0 )
		m_edieScoreInput.m_lMinScore = 0;

	if ( m_edieScoreInput.m_lMinScore > m_edieScoreInput.m_lMaxScore )
		m_edieScoreInput.m_lMinScore = m_edieScoreInput.m_lMaxScore;

	if ( m_lCellScore > lMaxBetCount )
	{
		m_btOneScore.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		str.Format(TEXT("%d"), m_lCellScore);
		m_btOneScore.SetWindowText(str);
		m_btOneScore.EnableWindow(TRUE);
	}
	

	if ( m_lCellScore * 2 > lMaxBetCount )
	{
		m_btTwoScore.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		str.Format(TEXT("%d"), m_lCellScore * 2);
		m_btTwoScore.SetWindowText(str);
		m_btTwoScore.EnableWindow(TRUE);
	}

	if ( m_lCellScore * 3 > lMaxBetCount )
	{
		m_btThreeScore.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		str.Format(TEXT("%d"), m_lCellScore * 3);
		m_btThreeScore.SetWindowText(str);
		m_btThreeScore.EnableWindow(TRUE);
	}

	if ( m_lCellScore * 4 > lMaxBetCount )
	{
		m_btFourScore.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		str.Format(TEXT("%d"), m_lCellScore * 4);
		m_btFourScore.SetWindowText(str);
		m_btFourScore.EnableWindow(TRUE);
	}

	if ( m_edieScoreInput.m_lMinScore != 0 )
	{
		m_edieScoreInput.SetWindowText(AddComma(m_edieScoreInput.m_lMinScore));
		m_edieScoreInput.SetSel(m_edieScoreInput.LineLength(), m_edieScoreInput.LineLength(), TRUE); 
	}
}

// 获取下载数量
LONGLONG CPlayOperate::GetBetCount()
{
	CString str;
	m_edieScoreInput.GetWindowText(str);
	LONGLONG lScore = DeleteComma(str);

	if ( lScore < m_edieScoreInput.m_lMinScore )
	{
		lScore = m_edieScoreInput.m_lMinScore;
	}	
	return lScore;
}

// 获取位置
CRect CPlayOperate::GetOperateRect()
{
	return CRect( m_ptPos.x, m_ptPos.y, m_ptPos.x + m_sizeBakc.cx, m_ptPos.y + m_sizeBakc.cy);
}


// 显示函数
void CPlayOperate::ShowOperate( bool bShow )
{
	m_edieScoreInput.SetWindowText(TEXT(""));
	m_bShow = bShow;

	if ( !m_bShow )
	{
		m_btOneScore.ShowWindow(SW_HIDE);
		m_btTwoScore.ShowWindow(SW_HIDE);
		m_btThreeScore.ShowWindow(SW_HIDE);
		m_btFourScore.ShowWindow(SW_HIDE);
		m_btAddScore.ShowWindow(SW_HIDE);
		m_btFollow.ShowWindow(SW_HIDE);
		m_btGiveUp.ShowWindow(SW_HIDE);
		m_btShowHand.ShowWindow(SW_HIDE);
		m_edieScoreInput.ShowWindow(SW_HIDE);
		m_btADD.ShowWindow(SW_HIDE);
		m_btSUB.ShowWindow(SW_HIDE);
	}
	else
	{
		m_btADD.ShowWindow(SW_SHOW);
		m_btSUB.ShowWindow(SW_SHOW);
		m_btOneScore.ShowWindow(SW_SHOW);
		m_btTwoScore.ShowWindow(SW_SHOW);
		m_btThreeScore.ShowWindow(SW_SHOW);
		m_btFourScore.ShowWindow(SW_SHOW);
		m_edieScoreInput.ShowWindow(SW_SHOW);
		m_edieScoreInput.SetFocus();
		m_lDragSite = m_rectDrag.left;
	}
}

BOOL CPlayOperate::PreTranslateMessage( MSG * pMsg )
{
	if ( pMsg->message == WM_LBUTTONDOWN )
	{
		CPoint ptMouse(LOWORD(pMsg->lParam) , HIWORD(pMsg->lParam));
		if( m_rectDrag.PtInRect(ptMouse) )
		{
			m_bMouseDown = true;
			m_lDragSite = ptMouse.x;
			SetEditScore();
			return TRUE;
		}
	}
	else if ( pMsg->message == WM_MOUSEMOVE )
	{
		CPoint ptMouse(LOWORD(pMsg->lParam) , HIWORD(pMsg->lParam));
		if ( m_bMouseDown && m_rectDrag.PtInRect(ptMouse) )
		{
			m_lDragSite = ptMouse.x;
			SetEditScore();
			return TRUE;
		}
	}
	else if ( pMsg->message == WM_LBUTTONUP || pMsg->message == WM_KILLFOCUS )
	{
		m_bMouseDown = false;
	}
	
	return FALSE;
}

// 设置积分
void CPlayOperate::SetEditScore()
{
	int nWidth = m_rectDrag.Width() - m_sizeDrag.cx;
	int nOffSet = m_sizeDrag.cx / 2;
	int nDragX = m_lDragSite;
	if ( nDragX < m_rectDrag.left + nOffSet )
		nDragX = m_rectDrag.left + nOffSet;
	else if ( nDragX > m_rectDrag.right - nOffSet )
		nDragX = m_rectDrag.right - nOffSet;

	nDragX -= m_rectDrag.left + nOffSet;

	LONGLONG lCurrentScore = 0;
	lCurrentScore = (LONGLONG)((m_edieScoreInput.m_lMaxScore - m_edieScoreInput.m_lMinScore) * ( (DOUBLE)nDragX/(DOUBLE)nWidth )) + m_edieScoreInput.m_lMinScore;
	m_edieScoreInput.SetWindowText(AddComma(lCurrentScore));
	m_edieScoreInput.SetSel(m_edieScoreInput.LineLength(), m_edieScoreInput.LineLength(), TRUE); 
}

// 设置积分
void CPlayOperate::SetDragScore()
{
	CString str;
	m_edieScoreInput.GetWindowText(str);
	LONGLONG lScore = DeleteComma(str);

	if ( lScore < m_edieScoreInput.m_lMinScore )
	{
		lScore = m_edieScoreInput.m_lMinScore;
		m_edieScoreInput.SetWindowText(AddComma(lScore));
		m_edieScoreInput.SetSel(m_edieScoreInput.LineLength(), m_edieScoreInput.LineLength(), TRUE); 
	}	

	lScore -= m_edieScoreInput.m_lMinScore;
	int nWidth = m_rectDrag.Width() - m_sizeDrag.cx;
	int nDragX = (int)( ((DOUBLE)lScore / (DOUBLE)(m_edieScoreInput.m_lMaxScore - m_edieScoreInput.m_lMinScore)) * nWidth);
	
	nDragX += m_rectDrag.left + m_sizeDrag.cx / 2;
	m_lDragSite = nDragX;
}

// 设置积分
void CPlayOperate::ScoreADD()
{
	CString str;
	m_edieScoreInput.GetWindowText(str);
	LONGLONG lScore = DeleteComma(str);

	if( lScore < m_edieScoreInput.m_lMaxScore )
		lScore++;
	
	m_edieScoreInput.SetWindowText(AddComma(lScore));
	m_edieScoreInput.SetSel(m_edieScoreInput.LineLength(), m_edieScoreInput.LineLength(), TRUE); 
	SetDragScore();
}


// 设置积分
void CPlayOperate::ScoreSUB()
{
	CString str;
	m_edieScoreInput.GetWindowText(str);
	LONGLONG lScore = DeleteComma(str);

	if( lScore > 0 )
		lScore--;
	
	m_edieScoreInput.SetWindowText(AddComma(lScore));
	m_edieScoreInput.SetSel(m_edieScoreInput.LineLength(), m_edieScoreInput.LineLength(), TRUE); 
	SetDragScore();
}

// 添加逗号
CString CPlayOperate::AddComma( LONGLONG lScore )
{
	CString strScore;
	CString strReturn;
	strScore.Format(TEXT("%I64d"), lScore);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%4) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}
	return strReturn;
}

// 添加逗号
CString CPlayOperate::AddComma( CString strScore )
{
	CString strReturn;
	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%4) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}
	return strReturn;
}

// 删除逗号
LONGLONG CPlayOperate::DeleteComma( CString strScore )
{
	LONGLONG lScore = 0l;
	strScore.Remove(',');
	if ( !strScore.IsEmpty() )
		lScore = _ttoi64(strScore);

	return lScore;
}





