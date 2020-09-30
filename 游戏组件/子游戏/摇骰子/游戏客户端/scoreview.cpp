#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////

#define IDC_ENSURE						200					//确认按钮

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_ENSURE, OnEnsureButton)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.LoadFromResource(AfxGetInstanceHandle(),IDB_GAME_END);
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

	//创建按钮
	CRect CreateRect(0,0,0,0);
	m_btEnsure.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ENSURE);
	m_btEnsure.SetButtonImage(IDB_BT_OK, AfxGetInstanceHandle(), false,false);
	m_btEnsure.MoveWindow(145, 208, 97, 35);
	m_btEnsure.EnableWindow(TRUE);
	m_btEnsure.ShowWindow(SW_SHOW);

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//创建资源管理器
	m_pSkinResourceManager= (CSkinResourceManager *)CSkinResourceManager::GetInstance();
	ASSERT(m_pSkinResourceManager!=NULL);

	return TRUE;
}

//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,0,0));
	dc.SelectObject(m_pSkinResourceManager->GetDefaultFont());

	//绘画背景
	m_ImageBack.BitBlt(dc,0,0);

	//显示分数
	CRect rcDraw;
	CString strScore ;
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		//if(_tcscpy(m_szUserName[i],_T("")))
		{
			dc.SetTextColor( RGB(215, 198, 108) );

			//用户名字
			rcDraw.left=37;
			rcDraw.right=197;
			rcDraw.top=i*40+87;
			rcDraw.bottom=rcDraw.top+20;
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);

			//字体颜色
			if ( m_lGameScore[i] > 0 )
				dc.SetTextColor( RGB(215, 198, 108) ) ;
			else 
				dc.SetTextColor( RGB( 250, 48, 0 ) ) ;

			//用户积分
			rcDraw.left=213;
			rcDraw.right=351;
			strScore.Format(_T("%I64d") , m_lGameScore[i]) ;
			dc.DrawText(strScore,strScore.GetLength(),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);
		}
	}

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
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(LONGLONG lGameTax)
{
	//设置变量
	m_lGameTax=lGameTax;
	Invalidate(FALSE);

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
		Invalidate(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

void CScoreView::OnKillFocus(CWnd* pNewWnd)
{
	//隐藏窗体
	ShowWindow(SW_HIDE) ;

	CDialog::OnKillFocus(pNewWnd);
}
