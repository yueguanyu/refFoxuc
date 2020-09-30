#include "Stdafx.h"
#include "GameScore.h"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_CLOSE_SCORE				100									//关闭成绩

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameScore, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameScore::CGameScore() : CDialog(IDD_GAME_SCORE)
{
	//初始化变量
	ZeroMemory( &m_ScoreInfo,sizeof(m_ScoreInfo) );
	m_cbWeaveCount = 0;
	m_ChiHuRight.SetEmpty();

	HINSTANCE hInst = AfxGetInstanceHandle();
	m_ImageBack.LoadFromResource(hInst ,IDB_GAME_SCORE);
	m_ImageFangPao.LoadImage( hInst,TEXT("FANG_PAO") );
	m_ImageZiMo.LoadImage( hInst,TEXT("ZI_MO") );
}

//析构函数
CGameScore::~CGameScore()
{
}

//控件绑定
void CGameScore::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btCloseScore);
}

//初始化函数
BOOL CGameScore::OnInitDialog()
{
	__super::OnInitDialog();

	//删除窗口标题和获取属性
	ModifyStyle(WS_CAPTION,0,0);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	

	//创建区域
	CRgn RgnControl;
	m_ImageBack.CreateImageRegion(RgnControl,RGB(255,0,255));

	//设置窗口
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//移动窗口
		SetWindowRgn(RgnControl,TRUE);
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);
	}

	m_btCloseScore.SetButtonImage(IDB_BT_SCORE_CLOSE,AfxGetInstanceHandle(),false,false);

	//调整按钮
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcBtClose;
	m_btCloseScore.GetWindowRect(&rcBtClose);
	m_btCloseScore.SetWindowPos(NULL,(rcClient.Width()-rcBtClose.Width())/2,rcClient.Height()-rcBtClose.Height()-10,0,0,SWP_NOSIZE|SWP_NOZORDER);

	//创建字体
	//LOGFONT lg;
	//ZeroMemory( &lg,sizeof(lg) );
	//lg.lfCharSet = GB2312_CHARSET;
	//_tcscpy( lg.lfFaceName,"黑体" );
	//lg.lfHeight = -20;
	//if( !m_InfoFont.CreateFontIndirect( &lg ) )
	//	m_InfoFont.Attach(CSkinResourceManager::GetDefaultFont());

	//设置透明
	ModifyStyleEx(0,0x00080000);
	HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL")); 
	if(hInst) 
	{ 
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes（）函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),0,220,2); 
		FreeLibrary(hInst); 
	}

	return TRUE;
}

//复位数据
void CGameScore::RestorationData()
{
	//设置变量
	m_cbWeaveCount=0;
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	//隐藏窗口
	if (m_hWnd!=NULL) ShowWindow(SW_HIDE);

	return;
}

//设置积分
void CGameScore::SetScoreInfo(const tagScoreInfo & ScoreInfo, const tagWeaveInfo & WeaveInfo, const CChiHuRight &ChiHuRight)
{
	//设置变量
	m_ScoreInfo=ScoreInfo;
	m_cbWeaveCount=WeaveInfo.cbWeaveCount;
	m_ChiHuRight = ChiHuRight;

	//组合变量
	for (BYTE i=0;i<m_cbWeaveCount;i++)
	{
		bool bPublicWeave=(WeaveInfo.cbPublicWeave[i]==TRUE);
		m_WeaveCard[i].SetCardData(WeaveInfo.cbCardData[i],WeaveInfo.cbCardCount[i]);
		m_WeaveCard[i].SetDisplayItem(true);
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return;
}

//关闭按钮
void CGameScore::OnOK()
{
	//隐藏窗口
	RestorationData();

	__super::OnOK();
}

//重画函数
void CGameScore::OnPaint()
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
	//DCBuffer.SelectObject(m_InfoFont);
	DCBuffer.SetTextColor(RGB(250,250,250));

	//绘画背景

	if (m_ImageBack.IsNull()==false) m_ImageBack.BitBlt(DCBuffer,0,0);

	//位置变量
	int nCardSpace=5;
	int nItemWidth=g_CardResource.m_ImageTableBottom.GetViewWidth();
	int nTotalWidth=m_cbWeaveCount*(nItemWidth*3+nCardSpace)+nItemWidth*m_ScoreInfo.cbCardCount+nCardSpace;

	//计算位置
	int nYCardPos=140-g_CardResource.m_ImageTableBottom.GetViewHeight();
	int nXCardPos=(rcClient.Width()-nTotalWidth)/2;

	//绘画组合
	for (BYTE i=0;i<m_cbWeaveCount;i++)
	{
		//绘画扑克
		m_WeaveCard[i].DrawCardControl(&DCBuffer,nXCardPos,nYCardPos,false);

		//设置位置
		nXCardPos+=(nCardSpace+nItemWidth*3);
	}

	//绘画扑克
	for (BYTE i=0;i<m_ScoreInfo.cbCardCount;i++)
	{
		//绘画扑克
		g_CardResource.m_ImageTableBottom.DrawCardItem(&DCBuffer,m_ScoreInfo.cbCardData[i],nXCardPos,nYCardPos,false);

		//设置位置
		nXCardPos+=nItemWidth;
		if ((i+2)==m_ScoreInfo.cbCardCount) nXCardPos+=nCardSpace;
	}

	TCHAR szBuffer[32] = TEXT("");
	//胡牌信息
	if( m_ScoreInfo.wProvideUser != INVALID_CHAIR )
	{
		CRect rcDraw;
		//放炮
		if( m_ScoreInfo.wProvideUser != m_ScoreInfo.wChiHuUser )
		{
			rcDraw.SetRect(15,148,190,175);
			DCBuffer.DrawText( m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser],lstrlen(m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser]),
				&rcDraw,DT_SINGLELINE|DT_END_ELLIPSIS|DT_RIGHT|DT_BOTTOM );
			m_ImageFangPao.DrawImage( &DCBuffer,205,150 );
			rcDraw.SetRect(295,148,482,175);
			DCBuffer.DrawText( m_ScoreInfo.szUserName[m_ScoreInfo.wChiHuUser],lstrlen(m_ScoreInfo.szUserName[m_ScoreInfo.wChiHuUser]),
				&rcDraw,DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT|DT_BOTTOM );
		}
		//自摸
		else
		{
			rcDraw.SetRect(15,148,190,175);
			DCBuffer.DrawText( m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser],lstrlen(m_ScoreInfo.szUserName[m_ScoreInfo.wProvideUser]),
				&rcDraw,DT_SINGLELINE|DT_END_ELLIPSIS|DT_RIGHT|DT_BOTTOM );
			m_ImageZiMo.DrawImage( &DCBuffer,215,150 );
		}
	}

	//列表信息
	CString szUserInfo;
	CSkinButton btDraw;
	DCBuffer.SetTextAlign(TA_LEFT);
	INT nY=200,nCard=190,nMagic=270,nGang=345,nScore=415;
	DCBuffer.TextOut(66,nY,TEXT("玩家"));
	//btDraw.SetCapture(&DCBuffer,TEXT("玩家"),RGB(0,255,255),RGB(0,0,0),66,nY);
	//btDraw.DrawTextString(&DCBuffer,TEXT("明杠"),RGB(0,255,255),RGB(0,0,0),nCard,nY);
	//btDraw.DrawTextString(&DCBuffer,TEXT("暗杠"),RGB(0,255,255),RGB(0,0,0),nMagic,nY);
	//btDraw.DrawTextString(&DCBuffer,TEXT("胡牌"),RGB(0,255,255),RGB(0,0,0),nGang,nY);
	//btDraw.DrawTextString(&DCBuffer,TEXT("合计"),RGB(0,255,255),RGB(0,0,0),nScore,nY);
	DCBuffer.TextOut(nCard,nY,TEXT("明杠"));
	DCBuffer.TextOut(nMagic,nY,TEXT("暗杠"));
	DCBuffer.TextOut(nGang,nY,TEXT("胡牌"));
	DCBuffer.TextOut(nScore,nY,TEXT("合计"));
	//胡牌得分
	LONGLONG lScore[GAME_PLAYER]={};
	for( WORD j = 0; j < GAME_PLAYER; j++ )
	{
		if(j==m_ScoreInfo.wChiHuUser)
		{
			lScore[j]+=((m_ScoreInfo.wProvideUser != m_ScoreInfo.wChiHuUser)?1:2);
			lScore[(j+1)%GAME_PLAYER]-=((m_ScoreInfo.wProvideUser != m_ScoreInfo.wChiHuUser)?1:2);
		}
		if(j==m_ScoreInfo.wExitUser)
		{
			lScore[j]-=3;
			lScore[(j+1)%GAME_PLAYER]+=3;
		}
	}

	//强退用户
	if(INVALID_CHAIR!=m_ScoreInfo.wExitUser)
	{
		DCBuffer.SetTextAlign(TA_CENTER);
		szUserInfo.Format(TEXT("[%s]强退，扣除%I64d分"),m_ScoreInfo.szUserName[m_ScoreInfo.wExitUser],3*m_ScoreInfo.lCellScore);
		//btDraw.DrawTextString(&DCBuffer,szUserInfo,RGB(255,255,255),RGB(0,0,0),250,110);
			DCBuffer.TextOut(250,110,szUserInfo);
	}

	//得分信息

	for( WORD j = 0; j < GAME_PLAYER; j++ )
	{
		WORD i=(j+m_ScoreInfo.wMeChairID)%GAME_PLAYER;
		INT nY1=nY+32+j*30;
		INT nX = 80;
		INT nAdd=18;
		DCBuffer.SetTextColor(((i==m_ScoreInfo.wMeChairID)? RGB(252,255,0):RGB(252,255,0) ));
				DCBuffer.SetTextAlign(TA_LEFT);
		//玩家
		szUserInfo=m_ScoreInfo.szUserName[i];
		//btDraw.DrawTextString(&DCBuffer,szUserInfo,RGB(255,255,255),RGB(0,0,0),nX,nY1);
		CRect rc(20,nY1,160,nY1+16);
		DCBuffer.DrawText(szUserInfo,&rc,DT_LEFT|DT_END_ELLIPSIS);
			DCBuffer.SetTextAlign(TA_CENTER);
			//DCBuffer.TextOut(nX,nY1,szUserInfo);
		//明杠
		nX = nCard+nAdd;
		_sntprintf( szBuffer,CountArray(szBuffer),TEXT("%I64d"),m_ScoreInfo.lMingGang[i]*m_ScoreInfo.lCellScore );
		//btDraw.DrawTextString(&DCBuffer,szBuffer,RGB(255,255,255),RGB(0,0,0),nX,nY1);
			DCBuffer.TextOut(nX,nY1,szBuffer);
		//暗杠
		nX = nMagic+nAdd;
		_sntprintf( szBuffer,CountArray(szBuffer),TEXT("%I64d"),m_ScoreInfo.lLangGang[i]*m_ScoreInfo.lCellScore );
		//btDraw.DrawTextString(&DCBuffer,szBuffer,RGB(255,255,255),RGB(0,0,0),nX,nY1);
			DCBuffer.TextOut(nX,nY1,szBuffer);
		//胡牌
		nX = nGang+nAdd;
		_sntprintf( szBuffer,CountArray(szBuffer),TEXT("%I64d"),lScore[i]*m_ScoreInfo.lCellScore );
		//btDraw.DrawTextString(&DCBuffer,szBuffer,RGB(255,255,255),RGB(0,0,0),nX,nY1);
			DCBuffer.TextOut(nX,nY1,szBuffer);
		//分数
		nX = nScore+nAdd;
		_sntprintf( szBuffer,CountArray(szBuffer),TEXT("%I64d分"),m_ScoreInfo.lGameScore[i] );
		//btDraw.DrawTextString(&DCBuffer,szBuffer,RGB(255,255,255),RGB(0,0,0),nX,nY1);
			DCBuffer.TextOut(nX,nY1,szBuffer);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//鼠标消息
void CGameScore::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//背景函数
BOOL CGameScore::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
