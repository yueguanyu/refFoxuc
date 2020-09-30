#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//宏定义
#define ACCOUNTS_HEIGHT 32
//爆炸标识
#define IDI_BOMB_EFFECT					101								//爆炸标识

//爆炸数目
#define BOMB_EFFECT_COUNT				6								//爆炸数目

//////////////////////////////////////////////////////////////////////////
//按钮标识 

//控制按钮
#define IDC_START						100								//开始按钮
#define IDC_TRUSTEE						101								//托管控制
#define IDC_LAST_TURN					102								//上轮扑克
#define IDC_SORT_CARD_ORDER				103								//扑克排序
#define IDC_SORT_CARD_COLOR				104								//扑克排序
#define IDC_SORT_CARD_COUNT				105								//扑克排序
#define IDC_SORT_CARD					106								//扑克排序

//游戏按钮
#define IDC_OUT_CARD					120								//出牌按钮
#define IDC_PASS_CARD					121								//PASS按钮
#define IDC_OUT_PROMPT	                122                             //提示按钮
#define IDC_TRUSTEE_CONTROL				203								//托管控制

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	//系统消息
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//游戏变量
	m_lCellScore=0L;
	m_cbMainValue=0;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	m_bShowFirstCard=true;
	m_nLButtonUpCount=0;

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//用户状态
	ZeroMemory(m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(m_bUserContinue,sizeof(m_bUserContinue));

	//游戏状态
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_wHoverCardItem=INVALID_ITEM;
	m_wMouseDownItem=INVALID_ITEM;

	//状态变量
	m_bShowScore=false;
	m_bLastTurnCard=false;

}

//析构函数
CGameClientView::~CGameClientView()
{
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//删除时间
	KillTimer(IDI_BOMB_EFFECT);

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//游戏变量
	m_lCellScore=0L;
	m_cbMainValue=0;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	m_bShowFirstCard=true;
	m_nLButtonUpCount=0;

	//用户状态
	ZeroMemory(m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(m_bUserContinue,sizeof(m_bUserContinue));

	//游戏状态
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_wHoverCardItem=INVALID_ITEM;
	m_wMouseDownItem=INVALID_ITEM;

	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//状态变量
	m_bShowScore=false;
	m_bLastTurnCard=false;

	//控制按钮
	m_btStart.ShowWindow(SW_HIDE);

	//禁止按钮
	m_btStustee.EnableWindow(FALSE);
	m_btLastTurn.EnableWindow(FALSE);
	m_btSort.EnableWindow(FALSE);
	m_btSortCardCount.EnableWindow(FALSE);
	m_btSortCardColor.EnableWindow(FALSE);
	m_btSortCardOrder.EnableWindow(FALSE);

	//游戏按钮
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	//m_btOutPrompt.ShowWindow(SW_HIDE);

	//还原按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btStustee.SetButtonImage(IDB_BT_START_TRUSTEE,hResInstance,false,false);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);
	}

	//扑克控件
	m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	return ;
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	int m_nXFace=50,m_nYFace=50;
	int m_nXTimer=50,m_nYTimer=50;
	int m_nYBorder=0,m_nXBorder=0;
#ifdef VIDEO_GAME
	//头像位置
	m_ptAvatar[0].x=(nWidth-m_nXFace)/2;
	m_ptAvatar[0].y=5;
	m_ptNickName[0].x=m_ptAvatar[0].x+m_nXFace+10;
	m_ptNickName[0].y=m_ptAvatar[0].y;
	m_ptClock[0].x=m_ptAvatar[0].x-m_nXTimer/2-5;
	m_ptClock[0].y=m_ptAvatar[0].y+m_nYTimer/2;
	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=200;
	m_ptAuto[0].x=nWidth/2+m_nXFace+m_nXTimer+35;
	m_ptAuto[0].y=m_nYBorder+5;
	//SetFlowerControlInfo( 0,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[0].y+m_nYFace );

	m_ptAvatar[2].x=(nWidth-m_nXFace)/2;
	m_ptAvatar[2].y=nHeight-m_nYFace-5;
	m_ptNickName[2].x=m_ptAvatar[2].x+m_nXFace+10;
	m_ptNickName[2].y=m_ptAvatar[2].y+m_nYFace/2+5;
	m_ptClock[2].x=m_ptAvatar[0].x-m_nXTimer/2-5;
	m_ptClock[2].y=m_ptAvatar[2].y+5;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nHeight-260;
	m_ptAuto[2].x=nWidth/2+m_nXFace+m_nXTimer+35;
	m_ptAuto[2].y=nHeight-m_nYBorder-m_nYFace-10;
	//SetFlowerControlInfo( 2,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[2].y-BIG_FACE_HEIGHT );

	m_ptAvatar[1].x=5;
	m_ptAvatar[1].y=(nHeight-m_nYFace)/2-50;
	m_ptNickName[1].x=5;
	m_ptNickName[1].y=m_ptAvatar[1].y+m_nYFace+10;
	m_ptClock[1].x=m_ptAvatar[1].x+m_nXTimer/2;
	m_ptClock[1].y=m_ptAvatar[1].y-m_nYTimer/2-5;
	m_ptReady[1].x=nWidth*3/10;
	m_ptReady[1].y=nHeight/2-50;
	m_ptAuto[1].x=m_nXBorder+5;
	m_ptAuto[1].y=nHeight/2-m_nYBorder-m_nYFace-m_nYTimer-83;
	//SetFlowerControlInfo( 1,m_ptAvatar[1].x+m_nXFace,m_ptAvatar[1].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	m_ptAvatar[3].x=nWidth-m_nXFace-5;
	m_ptAvatar[3].y=(nHeight-m_nYFace)/2-50;
	m_ptNickName[3].x=nWidth-5;
	m_ptNickName[3].y=m_ptAvatar[3].y+m_nYFace+10;
	m_ptClock[3].x=m_ptAvatar[3].x+m_nXTimer/2-15;
	m_ptClock[3].y=m_ptAvatar[1].y-m_nYTimer/2-5;
	m_ptReady[3].x=nWidth*7/10;
	m_ptReady[3].y=nHeight/2-50;
	m_ptAuto[3].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptAuto[3].y=nHeight/2-m_nYBorder-m_nYFace-m_nYTimer-83;
	//SetFlowerControlInfo( 3,m_ptAvatar[3].x-BIG_FACE_WIDTH,m_ptAvatar[3].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	//扑克位置
	m_ptHeap[0].SetPoint(m_ptNickName[0].x+5,m_ptNickName[0].y+18);
	m_ptHeap[1].SetPoint(m_ptNickName[1].x+5,m_ptNickName[1].y+18);
	m_ptHeap[2].SetPoint(m_ptNickName[2].x+5,m_ptNickName[2].y+18);
	m_ptHeap[3].SetPoint(m_ptNickName[3].x-80,m_ptNickName[3].y+18);

	//放弃位置
	m_ptPass[0].SetPoint(nWidth/2,180);
	m_ptPass[1].SetPoint(nWidth*3/10,nHeight/2-50);
	m_ptPass[2].SetPoint(nWidth/2,nHeight-260);
	m_ptPass[3].SetPoint(nWidth*7/10,nHeight/2-50);

	//用户扑克
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2,60,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(75+35,nHeight/2-50,enXLeft,enYCenter);
	m_HandCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-60,enXCenter,enYBottom);
	m_HandCardControl[3].SetBenchmarkPos(nWidth-75-35,nHeight/2-50,enXRight,enYCenter);

	//出牌扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,145,enXCenter,enYTop);
	m_UserCardControl[1].SetBenchmarkPos(nWidth*3/10,nHeight/2-50,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-300,enXCenter,enYCenter);
	m_UserCardControl[3].SetBenchmarkPos(nWidth*7/10,nHeight/2-50,enXCenter,enYCenter);

	m_5RecordCardControl.SetBenchmarkPos(5,75,enXLeft,enYTop);
	m_10RecordCardControl.SetBenchmarkPos(5,105,enXLeft,enYTop);
	m_KRecordCardControl.SetBenchmarkPos(5,135,enXLeft,enYTop);
	m_5RecordCardControl.SetDisplayItem(true);
	m_10RecordCardControl.SetDisplayItem(true);
	m_KRecordCardControl.SetDisplayItem(true);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//计算位置
	CSize CardSize=m_HandCardControl[MYSELF_VIEW_ID].GetCardSize();
	INT nYPosButton=nHeight-CardSize.cy-DEF_SHOOT_DISTANCE-63;

	//控制按钮
	CRect rcStart;
	m_btStart.GetWindowRect(&rcStart);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcStart.Width())/2,nYPosButton-rcStart.Height(),0,0,uFlags);

	//控制按钮
	CRect rcControl;
	m_btStustee.GetWindowRect(&rcControl);
	DeferWindowPos(hDwp,m_btStustee,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*1-5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurn,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*2-10,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardCount,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*3-15,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardColor,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*4-20,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSort,		  NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*5-25,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardOrder,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*6-30,0,0,uFlags);

	//游戏按钮
	CRect rcOutCard;
	m_btOutCard.GetWindowRect(&rcOutCard);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcOutCard.Width()*3/2-10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcOutCard.Width()/2+10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcOutCard.Width()/2,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth,nHeight,0,0,uFlags);

	//视频窗口
	/*CRect rcAVDlg;
	m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2-3,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,m_nXBorder+5,nHeight/2-3,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),5,0,0,uFlags);
	m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);*/

	//结束移动
	EndDeferWindowPos(hDwp);
#else
	//头像位置
	m_ptAvatar[0].x=(nWidth-m_nXFace)/2;
	m_ptAvatar[0].y=5;
	m_ptNickName[0].x=m_ptAvatar[0].x+m_nXFace+10;
	m_ptNickName[0].y=m_ptAvatar[0].y;
	m_ptClock[0].x=m_ptAvatar[0].x-m_nXTimer/2-5;
	m_ptClock[0].y=m_ptAvatar[0].y+m_nYTimer/2;
	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=200;
	m_ptAuto[0].x=nWidth/2+m_nXFace+m_nXTimer+35;
	m_ptAuto[0].y=m_nYBorder+5;
	//SetFlowerControlInfo( 0,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[0].y+m_nYFace );

	m_ptAvatar[2].x=(nWidth-m_nXFace)/2;
	m_ptAvatar[2].y=nHeight-m_nYFace-5;
	m_ptNickName[2].x=m_ptAvatar[2].x+m_nXFace+10;
	m_ptNickName[2].y=m_ptAvatar[2].y+m_nYFace/2+5;
	m_ptClock[2].x=m_ptAvatar[0].x-m_nXTimer/2-15;
	m_ptClock[2].y=m_ptAvatar[2].y+15;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nHeight-260;
	m_ptAuto[2].x=m_ptAvatar[2].x-105;
	m_ptAuto[2].y=m_ptAvatar[2].y;
	//SetFlowerControlInfo( 2,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[2].y-BIG_FACE_HEIGHT );

	m_ptAvatar[1].x=5;
	m_ptAvatar[1].y=(nHeight-m_nYFace)/2-50;
	m_ptNickName[1].x=5;
	m_ptNickName[1].y=m_ptAvatar[1].y+m_nYFace+10;
	m_ptClock[1].x=m_ptAvatar[1].x+m_nXTimer/2;
	m_ptClock[1].y=m_ptAvatar[1].y-m_nYTimer/2-15;
	m_ptReady[1].x=nWidth*3/10;
	m_ptReady[1].y=nHeight/2-50;
	m_ptAuto[1].x=m_ptAvatar[1].x;
	m_ptAuto[1].y=m_ptAvatar[1].y+115;
	//SetFlowerControlInfo( 1,m_ptAvatar[1].x+m_nXFace,m_ptAvatar[1].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	m_ptAvatar[3].x=nWidth-m_nXFace-5;
	m_ptAvatar[3].y=(nHeight-m_nYFace)/2-50;
	m_ptNickName[3].x=nWidth-5;
	m_ptNickName[3].y=m_ptAvatar[3].y+m_nYFace+10;
	m_ptClock[3].x=m_ptAvatar[3].x+m_nXTimer/2-15;
	m_ptClock[3].y=m_ptAvatar[1].y-m_nYTimer/2-15;
	m_ptReady[3].x=nWidth*7/10;
	m_ptReady[3].y=nHeight/2-50;
	m_ptAuto[3].x=m_ptAvatar[3].x;
	m_ptAuto[3].y=m_ptAvatar[3].y+115;
	//SetFlowerControlInfo( 3,m_ptAvatar[3].x-BIG_FACE_WIDTH,m_ptAvatar[3].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	//扑克位置
	m_ptHeap[0].SetPoint(m_ptNickName[0].x+5,m_ptNickName[0].y+18);
	m_ptHeap[1].SetPoint(m_ptNickName[1].x+5,m_ptNickName[1].y+18);
	m_ptHeap[2].SetPoint(m_ptNickName[2].x+5,m_ptNickName[2].y+18);
	m_ptHeap[3].SetPoint(m_ptNickName[3].x-80,m_ptNickName[3].y+18);

	//放弃位置
	m_ptPass[0].SetPoint(nWidth/2,180);
	m_ptPass[1].SetPoint(nWidth*3/10,nHeight/2-50);
	m_ptPass[2].SetPoint(nWidth/2,nHeight-260);
	m_ptPass[3].SetPoint(nWidth*7/10,nHeight/2-50);

	//用户扑克
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2,60,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(75,nHeight/2-50,enXLeft,enYCenter);
	m_HandCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-60,enXCenter,enYBottom);
	m_HandCardControl[3].SetBenchmarkPos(nWidth-75,nHeight/2-50,enXRight,enYCenter);

	//出牌扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,145,enXCenter,enYTop);
	m_UserCardControl[1].SetBenchmarkPos(nWidth*3/10,nHeight/2-50,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-300,enXCenter,enYCenter);
	m_UserCardControl[3].SetBenchmarkPos(nWidth*7/10,nHeight/2-50,enXCenter,enYCenter);

	m_5RecordCardControl.SetBenchmarkPos(5,75,enXLeft,enYTop);
	m_10RecordCardControl.SetBenchmarkPos(5,105,enXLeft,enYTop);
	m_KRecordCardControl.SetBenchmarkPos(5,135,enXLeft,enYTop);
	m_5RecordCardControl.SetDisplayItem(true);
	m_10RecordCardControl.SetDisplayItem(true);
	m_KRecordCardControl.SetDisplayItem(true);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//计算位置
	CSize CardSize=m_HandCardControl[MYSELF_VIEW_ID].GetCardSize();
	INT nYPosButton=nHeight-CardSize.cy-DEF_SHOOT_DISTANCE-63;

	//控制按钮
	CRect rcStart;
	m_btStart.GetWindowRect(&rcStart);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcStart.Width())/2,nYPosButton-rcStart.Height(),0,0,uFlags);

	//控制按钮
	CRect rcControl;
	m_btStustee.GetWindowRect(&rcControl);
	DeferWindowPos(hDwp,m_btStustee,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*1-5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurn,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*2-10,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardCount,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*3-15,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardColor,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*4-20,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSort,		  NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*5-25,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardOrder,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*6-30,0,0,uFlags);

	//游戏按钮
	CRect rcOutCard;
	m_btOutCard.GetWindowRect(&rcOutCard);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcOutCard.Width()*3/2-10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcOutCard.Width()/2+10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcOutCard.Width()/2,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth,nHeight,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

#endif

	return;
}
//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{

	switch (LOWORD(wParam))
	{
	case  IDC_START:
		{
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case  IDC_TRUSTEE:
		{
			SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);
			return TRUE;
		}
	case  IDC_LAST_TURN:
		{
			SendEngineMessage(IDM_LAST_TURN_CARD,0,0);
			return TRUE;
		}
	case  IDC_SORT_CARD:
		{
			SendEngineMessage(IDM_SORT_HAND_CARD,ST_ORDER,ST_ORDER);
			return TRUE;
		}
	case  IDC_SORT_CARD_COLOR:
		{
			SendEngineMessage(IDM_SORT_HAND_CARD,ST_TONGHUA,ST_TONGHUA);
			return TRUE;
		}
	case  IDC_SORT_CARD_COUNT:
		{
			SendEngineMessage(IDM_SORT_HAND_CARD,ST_510K,ST_510K);
			return TRUE;
		}
	case  IDC_SORT_CARD_ORDER:
		{
			SendEngineMessage(IDM_SORT_HAND_CARD,ST_ORDER,ST_ORDER);
			return TRUE;
		}
	case  IDC_OUT_CARD:
		{
			SendEngineMessage(IDM_OUT_CARD,0,0);
			return TRUE;
		}
	case  IDC_PASS_CARD:
		{
			SendEngineMessage(IDM_PASS_CARD,0,0);
			return TRUE;
		}
	case  IDC_OUT_PROMPT:
		{
			SendEngineMessage(IDM_OUT_PROMPT,0,0);
			return TRUE;
		}
	}
	return __super::OnCommand(wParam, lParam);
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);

	//状态信息
	m_ImageStatusInfo.BitBlt(pDC->m_hDC,5,5,SRCCOPY);

	pDC->SetTextColor(RGB(240,240,240));

	//构造字符
	TCHAR szBuffer[32]=TEXT("");

	_sntprintf(szBuffer,CountArray(szBuffer),TEXT(" %I64d 分"),m_PlayerScore[0]+m_PlayerScore[2]);
	CDFontEx::DrawText(this, pDC, 14, 400,szBuffer,100,23, RGB(240,240,240), DT_LEFT);

	_sntprintf(szBuffer,CountArray(szBuffer),TEXT(" %I64d 分"),m_PlayerScore[1]+m_PlayerScore[3]);
	CDFontEx::DrawText(this, pDC, 14, 400,szBuffer,100,46, RGB(240,240,240), DT_LEFT);


	//本轮分数信息
	if(m_TurnScore>0)
	{
		m_ImageTurnScore.TransDrawImage(pDC,nWidth/2-m_ImageTurnScore.GetWidth()/2,nHeight/2-m_ImageTurnScore.GetHeight()/2,RGB(255,0,255));
		DrawNumberString(pDC,m_TurnScore,nWidth/2+m_ImageTurnScore.GetWidth()/2+30,nHeight/2);
	}

	m_5RecordCardControl.DrawCardControl(pDC);
	m_10RecordCardControl.DrawCardControl(pDC);
	m_KRecordCardControl.DrawCardControl(pDC);

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem *pUserItem=GetClientUserItem(i);
		const tagUserInfo * pUserData=(pUserItem==NULL?NULL:pUserItem->GetUserInfo());
		WORD wUserTimer=GetUserClock(i);
		if (pUserData==NULL) continue;

		//绘画属性
		DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pUserItem);

		if (wUserTimer!=0&&wUserTimer<=30) DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);

		//绘画用户
		CDFontEx::DrawText(this, pDC, 14, 400,pUserData->szNickName,m_ptNickName[i].x,m_ptNickName[i].y, RGB(240,240,240),i==3?DT_RIGHT:DT_LEFT);
		//得分信息
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("得分： %I64d 分"),m_PlayerScore[i]);
		if(i!=2)
		{
			CDFontEx::DrawText(this, pDC, 14, 400,szBuffer,m_ptHeap[i].x,m_ptHeap[i].y+18, RGB(240,240,240),DT_LEFT);
	
		}
		else{
			CDFontEx::DrawText(this, pDC, 14, 400,szBuffer,5,nHeight-36, RGB(240,240,240),i==3?DT_RIGHT:DT_LEFT);

		}
		//托管标志
		if(m_bAutoPlayer[i]==true)
		{
			ImageScore.DrawImage(pDC,m_ptAuto[i].x,m_ptAuto[i].y);
		}
	}


	//用户扑克
	bool bHaveCard=false,bHaveNoCard=false;
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_HandCardControl[i].GetCardCount()==0) bHaveNoCard=true;
		if(m_HandCardControl[i].GetCardCount()>0)  bHaveCard=true;
	}
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		//用户扑克
		m_UserCardControl[i].DrawCardControl(pDC);

		//玩家扑克
		if (i==MYSELF_VIEW_ID)
		{
			//绘画扑克
			m_HandCardControl[i].DrawCardControl(pDC);

			//绘画数字
			if (m_HandCardControl[i].GetCardCount()>0)
			{
				//构造字符
				TCHAR szCardCount[32]=TEXT("");
				_sntprintf(szCardCount,CountArray(szCardCount),TEXT("牌数：剩余 %d 张"),m_HandCardControl[i].GetCardCount());

				//输出字符
				CDFontEx::DrawText(this, pDC, 14, 400,szCardCount,5,nHeight-18, RGB(240,240,240),DT_LEFT);
			}
		}

		//用户扑克
		if ((i!=MYSELF_VIEW_ID)&&(m_HandCardControl[i].GetCardCount()>0))
		{
			if (m_HandCardControl[i].GetDisplayItem()==false)
			{
				//绘画数字
				WORD wCardCount=m_HandCardControl[i].GetCardCount();

				//构造字符
				TCHAR szCardCount[32]=TEXT("");
				_sntprintf(szCardCount,CountArray(szCardCount),TEXT("剩余        张"));


				if(bHaveCard&&bHaveNoCard) 
					CDFontEx::DrawText(this, pDC, 14, 400,szCardCount,m_ptHeap[i].x-5,m_ptHeap[i].y, RGB(240,240,240),DT_LEFT);

				if(bHaveCard&&bHaveNoCard) DrawNumberString(pDC,wCardCount,m_ptHeap[i].x+42,m_ptHeap[i].y+5);
			}
			else
			{
				//绘画扑克
				m_HandCardControl[i].DrawCardControl(pDC);
			}
		}
	}

	//用户扑克
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].DrawCardControl(pDC);
	}

	//加载资源

	CSize SizeWinOrder(ImageWinOrder.GetWidth()/4,ImageWinOrder.GetHeight());

	//绘画名次
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_wWinOrder[i]!=0)
		{
			//获取位置
			CPoint CenterPoint;
			m_HandCardControl[i].GetCenterPoint(CenterPoint);

			//绘画信息
			ImageWinOrder.DrawImage(pDC,CenterPoint.x-SizeWinOrder.cx/2,CenterPoint.y-SizeWinOrder.cy/2,SizeWinOrder.cx,SizeWinOrder.cy,
				(m_wWinOrder[i]-1)*SizeWinOrder.cx,0,SizeWinOrder.cx,SizeWinOrder.cy);
		}
	}

	//用户标志
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem *pUserItem=GetClientUserItem(i);
		const tagUserInfo * pUserData=(pUserItem==NULL?NULL:pUserItem->GetUserInfo());
		if (pUserData==NULL) continue;

		//同意标志
		BYTE cbUserStatus=pUserData->cbUserStatus;
		if ((cbUserStatus==US_READY)||(m_bUserContinue[i]==true)) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
	}

	//加载资源
	CSize SizeUserPass(m_ImageUserPass.GetWidth(),m_ImageUserPass.GetHeight());

	//绘画状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			INT nXDrawPos=m_ptPass[i].x-SizeUserPass.cx/2;
			INT nYDrawPos=m_ptPass[i].y-SizeUserPass.cy/2;
			m_ImageUserPass.TransDrawImage(pDC,nXDrawPos,nYDrawPos,RGB(255,0,255));
		}
	}

	//上轮标志
	if (m_bLastTurnCard==true)
	{

		//绘画标志
		INT nImageWidth=m_ImageLastTurnTip.GetWidth();
		INT nImageHegith=m_ImageLastTurnTip.GetHeight();
		m_ImageLastTurnTip.TransDrawImage(pDC,(nWidth-nImageWidth)/2,(nHeight-nImageHegith)/2-50,RGB(255,0,255));
	}

	//绘画爆炸
	if (m_bBombEffect==true)
	{

		//绘画效果
		INT nImageHeight=m_ImageBombEffect.GetHeight();
		INT nImageWidth=m_ImageBombEffect.GetWidth()/BOMB_EFFECT_COUNT;
		m_ImageBombEffect.TransDrawImage(pDC,(nWidth-nImageWidth)/2,0,nImageWidth,nImageHeight,
			nImageWidth*(m_cbBombFrameIndex%BOMB_EFFECT_COUNT),0,RGB(255,0,255));
	}

	//积分视图
	//tagScoreInfoC fdde;
	//ZeroMemory(&fdde,sizeof(fdde));
	//m_ScoreControl.SetScoreInfo(fdde);
	m_ScoreControl.DrawScoreView(pDC,nWidth/2,nHeight*2/5);

	return;
}

//主牌数值
bool CGameClientView::SetMainValue(BYTE cbMainValue)
{
	//设置变量
	m_cbMainValue=cbMainValue;

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//单元积分
bool CGameClientView::SetCellScore(LONGLONG lCellScore)
{
	//设置变量
	m_lCellScore=lCellScore;

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置爆炸
bool CGameClientView::SetBombEffect(bool bBombEffect)
{
	if (bBombEffect==true)
	{
		//设置变量
		m_bBombEffect=true;
		m_cbBombFrameIndex=0;

		//启动时间
		SetTimer(IDI_BOMB_EFFECT,100,NULL);
	}
	else
	{
		//停止动画
		if (m_bBombEffect==true)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT);

			//设置变量
			m_bBombEffect=false;
			m_cbBombFrameIndex=0;

			//更新界面
			InvalidGameView(0,0,0,0);
		}
	}

	return true;
}

//设置上轮
VOID CGameClientView::SetLastTurnCard(bool bLastTurnCard)
{
	//设置变量
	if (bLastTurnCard!=m_bLastTurnCard)
	{
		//设置变量
		m_bLastTurnCard=bLastTurnCard;

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	return;
}

//设置放弃
bool CGameClientView::SetUserPass(WORD wChairID, bool bUserPass)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bUserPass[i]=bUserPass;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置继续
bool CGameClientView::SetUserContinue(WORD wChairID, bool bContinue)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bUserContinue[i]=bContinue;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserContinue[wChairID]=bContinue;
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置名次
bool CGameClientView::SetUserWinOrder(WORD wChairID, WORD wWinOrder)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_wWinOrder[i]=wWinOrder;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_wWinOrder[wChairID]=wWinOrder;
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置信息
bool CGameClientView::SetUserPersist(WORD wChairID, WORD wPersistInfo[2])
{
	//设置变量
	if (wChairID<GAME_PLAYER)
	{
		m_wPersistInfo[wChairID][0]=wPersistInfo[0];
		m_wPersistInfo[wChairID][1]=wPersistInfo[1];
	}
	else
	{
		ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置等级
bool CGameClientView::SetUserValueOrder(BYTE cbMySelfOrder, BYTE cbEnemyOrder)
{
	//设置变量
	m_cbValueOrder[0]=cbMySelfOrder;
	m_cbValueOrder[1]=cbEnemyOrder;

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

//设置游戏分数
bool CGameClientView::SetScoreInfo(LONGLONG TurnScore,LONGLONG PlayerScore[4])
{
	m_TurnScore=TurnScore;
	CopyMemory(m_PlayerScore,PlayerScore,sizeof(m_PlayerScore));
	InvalidGameView(0,0,0,0);
	return true;
}

//绘画数字
VOID CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
	INT nNumberHeight=m_ImageNumber.GetHeight();
	INT nNumberWidth=m_ImageNumber.GetWidth()/10;

	//计算数目
	LONG lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos+lNumberCount*nNumberWidth/2-nNumberWidth;

	//绘画桌号
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONG lCellNumber = static_cast<LONG>(lNumber%10);
		m_ImageNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0,RGB(255,0,255));

		//设置变量
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//时间消息
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
	//爆炸动画
	if (nIDEvent==IDI_BOMB_EFFECT)
	{
		//停止判断
		if (m_bBombEffect==false)
		{
			KillTimer(IDI_BOMB_EFFECT);
			return;
		}

		//设置变量
		if ((m_cbBombFrameIndex+1)>=BOMB_EFFECT_COUNT)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT);

			//设置变量
			m_bBombEffect=false;
			m_cbBombFrameIndex=0;
		}
		else m_cbBombFrameIndex++;

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//加载资源

	m_ImageNumber.LoadFromResource(this,hResInstance,IDB_NUMBER);
	m_ImageViewFill.LoadFromResource(this,hResInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadFromResource(this,hResInstance,IDB_VIEW_BACK);
	m_ImageUserPass.LoadFromResource(this,hResInstance,IDB_USER_PASS);
	m_ImageStatusInfo.LoadFromResource(this,hResInstance,IDB_STATUS_INFO);
	m_ImageBombEffect.LoadFromResource(this,hResInstance,IDB_BOMB_EFFECT);
	m_ImageValueOrder.LoadFromResource(this,hResInstance,IDB_VALUE_ORDER);
	m_ImageLastTurnTip.LoadFromResource(this,hResInstance,IDB_LAST_TURN_TIP);
	m_ImageTurnScore.LoadFromResource(this,hResInstance,IDB_TURN_SCORE);
	ImageScore.LoadImage(this,hResInstance,TEXT("TRUSTEE"));
	ImageWinOrder.LoadImage(this,hResInstance,TEXT("WIN_ORDER"));
	//控制按钮
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btStustee.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE);
	m_btLastTurn.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_LAST_TURN);
	m_btSort.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD);
	m_btSortCardColor.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_COLOR);
	m_btSortCardCount.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_COUNT);
	m_btSortCardOrder.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_ORDER);
	//m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);
	//m_btStusteeControl.EnableWindow(FALSE);

	//游戏按钮
	m_btOutCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_OUT_CARD);
	m_btPassCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_PASS_CARD);
	//m_btOutPrompt.Create(NULL,WS_CHILD,rcCreate,this,IDC_OUT_PROMPT);

	//控制按钮
	m_btStart.SetButtonImage(IDB_BT_START,hResInstance,false,false);
	m_btLastTurn.SetButtonImage(IDB_BT_LAST_TURN,hResInstance,false,false);
	m_btStustee.SetButtonImage(IDB_BT_START_TRUSTEE,hResInstance,false,false);
	m_btSort.SetButtonImage(IDB_BT_SORT,hResInstance,false,false);
	m_btSortCardColor.SetButtonImage(IDB_BT_SORT_CARD_COLOR,hResInstance,false,false);
	m_btSortCardCount.SetButtonImage(IDB_BT_SORT_CARD_COUNT,hResInstance,false,false);
	m_btSortCardOrder.SetButtonImage(IDB_BT_SORT_CARD_ORDER,hResInstance,false,false);

	//m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,hInstance,false);

	//游戏按钮
	m_btOutCard.SetButtonImage(IDB_BT_OUT_CARD,hResInstance,false,false);
	m_btPassCard.SetButtonImage(IDB_BT_PASS_CARD,hResInstance,false,false);
	//m_btOutPrompt.SetButtonImage(IDB_BT_OUT_PROMPT,hResInstance,false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
	
		//用户扑克
		if (i==MYSELF_VIEW_ID)
			m_HandCardControl[i].OnCreateRes(this);
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
			
		}

		//玩家扑克
		if (i==0)
		{
			m_HandCardControl[i].SetCardMode(this,true);
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE_SMALL,DEF_Y_DISTANCE_SMALL,0);
		}

		//玩家扑克
		if ((i==1)||(i==3))
		{
			m_HandCardControl[i].SetCardMode(this,true);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE_SMALL,DEF_Y_DISTANCE_SMALL,0);
		}

		//出牌扑克
		m_UserCardControl[i].OnCreateRes(this);
		m_UserCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetCardDistance(DEF_X_DISTANCE,0,0);
	}

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btStart,TEXT("开始游戏"));

	m_ScoreControl.OnCreateRes(this);
	
	m_5RecordCardControl.OnCreateRes(this);
	m_10RecordCardControl.OnCreateRes(this);
	m_KRecordCardControl.OnCreateRes(this);
//	//创建视频
//#ifdef VIDEO_GAME
//	for (WORD i=0;i<4;i++)
//	{
//		//创建视频
//		m_DlgVedioService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,70+i);
//		m_DlgVedioService[i].InitVideoService(i==2,true);
//
//		//设置视频
//		g_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
//	}
//#endif

	return 0;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_HandCardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true) return TRUE;

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}
//鼠标消息
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//变量定义
	WORD wHoverCardItem=INVALID_ITEM;

	//移动判断
	if (m_wHoverCardItem!=INVALID_ITEM)
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//扑克大小
		CSize ControlSize;
		m_HandCardControl[MYSELF_VIEW_ID].GetControlSize(ControlSize);

		//扑克位置
		CPoint OriginPoint;
		m_HandCardControl[MYSELF_VIEW_ID].GetOriginPoint(OriginPoint);

		//横行调整
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//获取索引
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		wHoverCardItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(MousePoint);

		//移动变量
		if (wHoverCardItem!=m_wHoverCardItem) m_bMoveMouse=true;
	}

	//交换扑克
	if ((m_bSwitchCard==true)&&(wHoverCardItem!=m_wHoverCardItem))
	{
		//设置扑克
		m_HandCardControl[MYSELF_VIEW_ID].MoveCardItem(wHoverCardItem);

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	//选择扑克
	if ((m_bSelectCard==true)&&(wHoverCardItem!=m_wHoverCardItem))
	{
		//设置扑克
		if (wHoverCardItem>m_wMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(m_wMouseDownItem,wHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(wHoverCardItem,m_wMouseDownItem);
		}

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	//设置变量
	m_wHoverCardItem=wHoverCardItem;

	return;
}

//鼠标消息
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//状态判断
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//区域判断
		if (m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point)==NULL)
		{
			//获取扑克
			BYTE cbCardData[MAX_COUNT];
			WORD wCardCount=m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

			//更新界面
			if (wCardCount>=0)
			{
				//设置扑克
				m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

				//发送消息
				SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

				//更新界面
				InvalidGameView(0,0,0,0);
			}
		}
		else
		{
			//出牌消息
			if (m_btOutCard.IsWindowVisible()&&m_btOutCard.IsWindowEnabled())
			{
				SendEngineMessage(IDM_OUT_CARD,0,0);
			}

		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	m_nLButtonUpCount++;
	if ( m_nLButtonUpCount == 2 )
	{
		return __super::OnLButtonUp(nFlags, Point);
	}

	//默认处理
	if (m_bMoveMouse==false)
	{
		//获取扑克
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_wMouseDownItem) pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

		//设置扑克
		if (pCardItem!=NULL)
		{
			//设置扑克
			pCardItem->bShoot=!pCardItem->bShoot;

			//发送消息
			SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

			//更新界面
			InvalidGameView(0,0,0,0);
		}
	}
	//选择处理
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		if (m_wHoverCardItem>m_wMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(m_wMouseDownItem,m_wHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(m_wHoverCardItem,m_wMouseDownItem);
		}

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	//交换处理
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);
		SendEngineMessage(IDM_SORT_HAND_CARD,ST_CUSTOM,ST_CUSTOM);

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	//释放鼠标
	ReleaseCapture();

	//设置变量
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//设置索引
	m_wMouseDownItem=INVALID_ITEM;
	m_wHoverCardItem=INVALID_ITEM;

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);
	m_nLButtonUpCount=0;

	//状态判断
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//获取扑克
		m_wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);

		//设置扑克
		if (m_wMouseDownItem!=INVALID_ITEM)
		{
			//获取扑克
			tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_wMouseDownItem);

			//设置变量
			m_bMoveMouse=false;
			m_wHoverCardItem=m_wMouseDownItem;

			//操作变量
			if (pCardItem->bShoot==false) 
			{
				m_bSelectCard=true;
			}
			else
			{
				m_bSwitchCard=true;
			}

			//设置鼠标
			SetCapture();
		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//设置扑克
	if (m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point)==NULL)
	{
		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wCardCount=m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

		//更新界面
		if (wCardCount>=0)
		{
			//禁止按钮
			m_btOutCard.EnableWindow(FALSE);

			//设置扑克
			m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

			//更新界面
			InvalidGameView(0,0,0,0);
		}
	}
	else
	{
		//状态判断
		if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true) m_wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);

		//获取扑克
		tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_wMouseDownItem);
		BYTE bSelectCard=pCardItem->cbCardData;
		pCardItem->bShoot=false;

		//查找附近牌
		WORD wStartIndex=0;
		WORD wEndIndex=0;
		for(int i=m_wMouseDownItem;i>=0;i--)
		{
			if((m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i)->cbCardData&0x0F)==(bSelectCard&0x0F)) wStartIndex=i;
			else	break;
		}
		for(int i=m_wMouseDownItem;i<m_HandCardControl[MYSELF_VIEW_ID].GetCardCount();i++)
		{
			if((m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i)->cbCardData&0x0F)==(bSelectCard&0x0F))		wEndIndex=i;
			else	break;
		}

		//弹起增加牌
		m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(wStartIndex,wEndIndex);

		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		InvalidGameView(0,0,0,0);
	}
	return;
}
void CGameClientView::SetAutoUser(bool bAutoUser[4])
{
	CopyMemory(m_bAutoPlayer,bAutoUser,sizeof(m_bAutoPlayer));

	//更新界面
	InvalidGameView(0,0,0,0);
	return;
}
//////////////////////////////////////////////////////////////////////////
//牌记录
void CGameClientView::Set510KRecord(BYTE bRecord[3][8])
{
	BYTE i=0;
	for(i=0;i<8;i++) if(bRecord[0][i]==0) break;
	m_5RecordCardControl.SetCardData(bRecord[0],i);
	for(i=0;i<8;i++) if(bRecord[1][i]==0) break;
	m_10RecordCardControl.SetCardData(bRecord[1],i);
	for(i=0;i<8;i++) if(bRecord[2][i]==0) break;
	m_KRecordCardControl.SetCardData(bRecord[2],i);

	//更新界面
	InvalidGameView(0,0,0,0);
	return;
}

//////////////////////////////////////////////////////////////////////////////////
