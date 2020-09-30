#include "StdAfx.h"
#include "Resource.h"
#include "..\游戏服务器\GameLogic.h"
#include "GameClientView.h"
#include "GameClient.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识

//游戏按钮
#define IDC_START						100								//开始按钮
#define IDC_OUT_CARD					101								//出牌按钮
#define IDC_OUT_PROMPT					102								//提示按钮
#define IDC_SEND_CONCEAL				103								//留底按钮

//功能按钮
#define IDC_CONCEAL_CARD				200								//查看底牌
#define IDC_REQUEST_LEAVE				201								//请求离开
#define IDC_LAST_TURN_CARD				202								//上轮扑克
#define IDC_TRUSTEE_CONTROL				203								//托管控制

#define BIG_FACE_WIDTH 32
#define BIG_FACE_HEIGHT 32
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_SEND_CONCEAL, OnSendConceal)
	ON_BN_CLICKED(IDC_CONCEAL_CARD, OnConcealCard)
	ON_BN_CLICKED(IDC_REQUEST_LEAVE, OnRuquestLeave)
	ON_BN_CLICKED(IDC_LAST_TURN_CARD, OnLastTurnCard)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL, OnStusteeControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() 
{
	//显示变量
	m_bWaitConceal=false;
	m_bLastTurnCard=false;

	//游戏变量
	m_wCardScore=0xFFFF;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_bStrustee,sizeof(m_bStrustee));

	//叫牌信息
	m_cbCallCount=0;
	m_cbCallColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_wCallCardUser=INVALID_CHAIR;
	m_cbScoreCardCount = 0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBanker.LoadFromResource(hInstance,IDB_BANKER);
	m_ImageBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageCenter.LoadFromResource(hInstance,IDB_VIEW_CENTER);
	m_ImageColorFlag.LoadFromResource(hInstance,IDB_COLOR_FLAG);
	m_ImageLastTurn.LoadFromResource(hInstance,IDB_LAST_TURN_TIP);
	m_ImageWaitConceal.LoadFromResource(hInstance,IDB_WAIT_CONCEAL);
	m_ImageStatusInfo.LoadFromResource(hInstance,IDB_STATUS_INFO);
	m_ImageScoreCard.LoadFromResource(hInstance,IDB_SCORE_CARD);
	m_PngStrustee.LoadImage(hInstance,TEXT("IDP_STRUSTEE"));
	m_nXFace=50;
	m_nYFace=50;

	m_nXBorder=0;
	m_nYBorder=0;

	m_nYTimer=48;
	m_nXTimer=48;


	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//AfxMessageBox(TEXT("CGameClientView::OnCreate begin"));


	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	//m_ImageBanker.LoadFromResource(hInstance,IDB_BANKER);
	//m_ImageBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	//m_ImageCenter.LoadFromResource(hInstance,IDB_VIEW_CENTER);
	//m_ImageColorFlag.LoadFromResource(hInstance,IDB_COLOR_FLAG);
	//m_ImageLastTurn.LoadFromResource(hInstance,IDB_LAST_TURN_TIP);
	//m_ImageWaitConceal.LoadFromResource(hInstance,IDB_WAIT_CONCEAL);
	//m_ImageStatusInfo.LoadFromResource(hInstance,IDB_STATUS_INFO);
	//m_ImageScoreCard.LoadFromResource(hInstance,IDB_SCORE_CARD);
	//m_PngStrustee.LoadImage(hInstance,TEXT("IDP_STRUSTEE"));


	//AfxMessageBox(TEXT("CGameClientView::OnCreate begin 1"));
	//创建控件
	CRect rcCreate(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD/*|WS_CLIPSIBLINGS|WS_CLIPCHILDREN*/,rcCreate,this,10);
	m_CallCardWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,11);
	m_ConcealCardView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,12);

	//AfxMessageBox(TEXT("CGameClientView::OnCreate begin 2"));
	//创建扑克
	for (WORD i=0;i<4;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].SetCardSpace(DEF_CARD_H_SPACE,DEF_CARD_V_SPACE,0);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,20+i);
	}
	m_HandCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,60);

	//设置扑克
	m_HandCardControl.SetSinkWindow(AfxGetMainWnd());

	//AfxMessageBox(TEXT("CGameClientView::OnCreate begin 3"));
	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_OUT_CARD);
	m_btOutPrompt.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_OUT_PROMPT);
	m_btSendConceal.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_SEND_CONCEAL);

	//功能按钮
	m_btConcealCard.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,IDC_CONCEAL_CARD);
	m_btRuquestLeave.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,IDC_REQUEST_LEAVE);
	m_btLastTurnCard.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,IDC_LAST_TURN_CARD);
	m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,IDC_TRUSTEE_CONTROL);

	//AfxMessageBox(TEXT("CGameClientView::OnCreate begin 4"));
	//设置按钮
	m_btStart.SetButtonImage(IDB_START,hInstance,false,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false,false);
	m_btOutPrompt.SetButtonImage(IDB_OUT_PROMPT,hInstance,false,false);
	m_btSendConceal.SetButtonImage(IDB_SEND_CONCEAL,hInstance,false,false);

	//功能按钮
	m_btConcealCard.SetButtonImage(IDB_CONCEAL_CARD,hInstance,false,false);
	m_btRuquestLeave.SetButtonImage(IDB_REQUEST_LEAVE,hInstance,false,false);
	m_btLastTurnCard.SetButtonImage(IDB_LAST_TURN_CARD,hInstance,false,false);
	m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,hInstance,false,false);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btConcealCard,TEXT("查看底牌"));
	m_ToolTipCtrl.AddTool(&m_btRuquestLeave,TEXT("请求离开"));
	m_ToolTipCtrl.AddTool(&m_btLastTurnCard,TEXT("显示上一轮出牌"));
	m_ToolTipCtrl.AddTool(&m_btStusteeControl,TEXT("开始（停止）托管"));

	//创建视频
#ifdef VIDEO_GAME
	for (WORD i=0;i<4;i++)
	{
		//创建视频
		m_DlgVedioService[i].Create(NULL,NULL,WS_CLIPSIBLINGS|WS_CHILD|WS_VISIBLE,rcCreate,this,70+i);
		m_DlgVedioService[i].InitVideoService(i==2,true);

		//设置视频
		g_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
	}
#endif

	
	//AfxMessageBox(TEXT("CGameClientView::OnCreate"));
	return 0;
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//重置界面
void CGameClientView::ResetGameView()
{
	//显示变量
	m_bWaitConceal=false;
	m_bLastTurnCard=false;

	//游戏变量
	m_wCardScore=0xFFFF;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_bStrustee,sizeof(m_bStrustee));

	//叫牌信息
	m_cbCallCount=0;
	m_cbCallColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_wCallCardUser=INVALID_CHAIR;
	m_cbScoreCardCount = 0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//隐藏控件
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btOutPrompt.ShowWindow(SW_HIDE);
	m_btSendConceal.ShowWindow(SW_HIDE);

	//禁用按钮
	m_btConcealCard.EnableWindow(FALSE);
	m_btRuquestLeave.EnableWindow(FALSE);
	m_btLastTurnCard.EnableWindow(FALSE);
	m_btStusteeControl.EnableWindow(FALSE);
	m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,AfxGetInstanceHandle(),false,false);

	//设置扑克
	m_HandCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayFlag(false);
	for (WORD i=0;i<CountArray(m_UserCardControl);i++) m_UserCardControl[i].SetCardData(NULL,0);

	//控件组件
	m_ScoreView.ShowWindow(SW_HIDE);
	m_CallCardWnd.ShowWindow(SW_HIDE);
	m_ConcealCardView.ShowWindow(SW_HIDE);

	return;
}

//调整控件
void CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
#ifdef VIDEO_GAME
	//变量定义
	int nViewHeight=nHeight-m_nYBorder;

	//设置坐标
	m_ptAvatar[3].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptAvatar[3].y=nViewHeight/2-m_nYFace-25;
	m_ptNickName[3].x=nWidth-m_nXBorder-5;
	m_ptNickName[3].y=m_ptAvatar[3].y+m_nYFace+5;
	m_ptClock[3].x=nWidth-m_nXBorder-m_nXTimer/2-5;
	m_ptClock[3].y=m_ptNickName[3].y+45;
	m_ptReady[3].x=nWidth-m_nXBorder-m_nXFace-55;
	m_ptReady[3].y=m_ptAvatar[3].y + m_nYFace/2 + 15;
	m_PointBanker[3].x=nWidth-m_nXBorder-43;
	m_PointBanker[3].y=m_ptAvatar[3].y-50;
	m_ptStrustee[3].x = m_ptAvatar[3].x-45;
	m_ptStrustee[3].y = m_ptAvatar[3].y-50;
	//SetFlowerControlInfo( 3,m_ptAvatar[3].x-BIG_FACE_WIDTH,m_ptAvatar[3].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );


	m_ptAvatar[1].x=m_nXBorder+5;
	m_ptAvatar[1].y=nViewHeight/2-m_nYFace-25;
	m_ptNickName[1].x=m_nXBorder+5;
	m_ptNickName[1].y=m_ptAvatar[1].y+m_nYFace+5;
	m_ptClock[1].x=m_nXBorder+m_nXTimer/2+5;
	m_ptClock[1].y=m_ptNickName[1].y+45;
	m_ptReady[1].x=m_ptAvatar[1].x + m_nXFace + 55;
	m_ptReady[1].y=m_ptAvatar[1].y+m_nYFace/2+15;
	m_PointBanker[1].x=m_nXBorder+5;
	m_PointBanker[1].y=m_ptAvatar[1].y-50;
	m_ptStrustee[1].x = m_ptAvatar[1].x+m_nXFace+15;
	m_ptStrustee[1].y = m_ptAvatar[1].y-50;
	//SetFlowerControlInfo( 1,m_ptAvatar[1].x+m_nXFace,m_ptAvatar[1].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	m_ptAvatar[0].x=nWidth/2-m_nXFace/2;
	m_ptAvatar[0].y=m_nYBorder+5;
	m_ptNickName[0].x=nWidth/2+5+m_nXFace/2-50;
	m_ptNickName[0].y=m_ptAvatar[0].y+36+15;
	m_ptClock[0].x=m_ptNickName[0].x+90;
	m_ptClock[0].y=m_nYBorder+m_nYTimer/2+5;
	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=m_nYBorder+m_nYFace+55;
	m_PointBanker[0].x=nWidth/2-m_nXFace/2-80;
	m_PointBanker[0].y=m_nYBorder+5;
	m_ptStrustee[0].x = m_PointBanker[0].x+45;
	m_ptStrustee[0].y = m_ptNickName[0].y;
	//SetFlowerControlInfo( 0,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[0].y+m_nYFace );

	m_ptAvatar[2].x=nWidth/2-m_nXFace/2;
	m_ptAvatar[2].y=nViewHeight-m_nYFace-15;
	m_ptNickName[2].x=nWidth/2+5+m_nXFace/2-50;
	m_ptNickName[2].y=m_ptAvatar[2].y+36+15;
	m_ptClock[2].x=m_ptNickName[2].x+90;
	m_ptClock[2].y=nViewHeight-m_nYTimer/2;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nViewHeight-m_nYBorder-250;
	m_PointBanker[2].x=nWidth/2-m_nXFace/2-80;
	m_PointBanker[2].y=nViewHeight-42;
	m_ptStrustee[2].x = m_PointBanker[2].x+45;
	m_ptStrustee[2].y = m_ptNickName[2].y-3;
	//SetFlowerControlInfo( 2,m_ptAvatar[2].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[2].y-BIG_FACE_HEIGHT );

	m_HandCardControl.SetMaxWidth(nWidth-10);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nViewHeight-m_nYFace-m_nYBorder-20,enXCenter,enYBottom);

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//计算位置
	INT nYPosButton=nViewHeight-m_nYFace-175-52;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nYPosButton+65,0,0,uFlags);

	//留底按钮
	m_btSendConceal.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btSendConceal,NULL,(nWidth-rcButton.Width())/2,nYPosButton,0,0,uFlags);

	//出牌按钮
	m_btOutCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2+15,nYPosButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()-15,nYPosButton,0,0,uFlags);

	//功能按钮
	m_btConcealCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,m_nXBorder+5,nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btRuquestLeave,NULL,m_nXBorder+5+(rcButton.Width()+5),nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btConcealCard,NULL,m_nXBorder+5+(rcButton.Width()+5)*2,nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurnCard,NULL,m_nXBorder+5+(rcButton.Width()+5)*3,nViewHeight-5-rcButton.Height(),0,0,uFlags);

	//视频窗口
	CRect rcAVDlg;
	m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2-3,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,m_nXBorder+5,nHeight/2-3,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),5,0,0,uFlags);
	m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);

	//移动控件
	EndDeferWindowPos(hDwp);

	//移动扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+78,enXCenter,enYTop);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nYPosButton-5,enXCenter,enYBottom);
	m_UserCardControl[3].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-110,nViewHeight/2-71,enXRight,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+110,nViewHeight/2-71,enXLeft,enYCenter);

	//叫牌窗口
	CRect rcCallCard;
	m_CallCardWnd.GetWindowRect(&rcCallCard);
	m_CallCardWnd.SetWindowPos(NULL,nWidth/2-5,nYPosButton,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//积分视图
	CRect rcScore;
	m_ScoreView.GetWindowRect(&rcScore);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcScore.Width())/2,nViewHeight/2-200,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//底牌窗口
	CRect rcConceal;
	m_ConcealCardView.GetWindowRect(&rcConceal);
	m_ConcealCardView.SetWindowPos(NULL,(nWidth-rcConceal.Width())/2,nViewHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);

#else
	//变量定义
	int nViewHeight=nHeight-m_nYBorder;

	//设置坐标
	m_ptAvatar[3].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptAvatar[3].y=nViewHeight/2-m_nYFace-25;
	m_ptNickName[3].x=nWidth-m_nXBorder-13;
	m_ptNickName[3].y=m_ptAvatar[3].y+m_nYFace+5;
	m_ptClock[3].x=nWidth-m_nXBorder-m_nXTimer/2-5;
	m_ptClock[3].y=m_ptNickName[3].y+45;
	m_ptReady[3].x=nWidth-m_nXBorder-m_nXFace-55;
	m_ptReady[3].y=m_ptAvatar[3].y + m_nYFace/2 + 15;
	m_PointBanker[3].x=nWidth-m_nXBorder-43;
	m_PointBanker[3].y=m_ptAvatar[3].y-50;
	m_ptStrustee[3].x = m_ptAvatar[3].x-45;
	m_ptStrustee[3].y = m_ptAvatar[3].y-50;
	//SetFlowerControlInfo( 3,m_ptAvatar[3].x-BIG_FACE_WIDTH,m_ptAvatar[3].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	
	m_ptAvatar[1].x=m_nXBorder+5;
	m_ptAvatar[1].y=nViewHeight/2-m_nYFace-25;
	m_ptNickName[1].x=m_nXBorder+2;
	m_ptNickName[1].y=m_ptAvatar[1].y+m_nYFace+5;
	m_ptClock[1].x=m_nXBorder+m_nXTimer/2+5;
	m_ptClock[1].y=m_ptNickName[1].y+45;
	m_ptReady[1].x=m_ptAvatar[1].x + m_nXFace + 55;
	m_ptReady[1].y=m_ptAvatar[1].y+m_nYFace/2+15;
	m_PointBanker[1].x=m_nXBorder+5;
	m_PointBanker[1].y=m_ptAvatar[1].y-50;
	m_ptStrustee[1].x = m_ptAvatar[1].x+m_nXFace+15;
	m_ptStrustee[1].y = m_ptAvatar[1].y-50;
	//SetFlowerControlInfo( 1,m_ptAvatar[1].x+m_nXFace,m_ptAvatar[1].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	m_ptAvatar[0].x=nWidth/2-m_nXFace/2;
	m_ptAvatar[0].y=m_nYBorder+5;
	m_ptNickName[0].x=nWidth/2+5+m_nXFace/2-58;
	m_ptNickName[0].y=m_ptAvatar[0].y+36+15;
	m_ptClock[0].x=m_ptNickName[0].x+90;
	m_ptClock[0].y=m_nYBorder+m_nYTimer/2+5;
	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=m_nYBorder+m_nYFace+55;
	m_PointBanker[0].x=nWidth/2-m_nXFace/2-80;
	m_PointBanker[0].y=m_nYBorder+5;
	m_ptStrustee[0].x = m_PointBanker[0].x+45;
	m_ptStrustee[0].y = m_ptNickName[0].y;
	//SetFlowerControlInfo( 0,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[0].y+m_nYFace );

	m_ptAvatar[2].x=nWidth/2-m_nXFace/2;
	m_ptAvatar[2].y=nViewHeight-m_nYFace-15;
	m_ptNickName[2].x=nWidth/2+5+m_nXFace/2-58;
	m_ptNickName[2].y=m_ptAvatar[2].y+36+15;
	m_ptClock[2].x=m_ptNickName[2].x+90;
	m_ptClock[2].y=nViewHeight-m_nYTimer/2-20;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nViewHeight-m_nYBorder-250;
	m_PointBanker[2].x=nWidth/2-m_nXFace/2-80;
	m_PointBanker[2].y=nViewHeight-42;
	m_ptStrustee[2].x = m_PointBanker[2].x+45;
	m_ptStrustee[2].y = m_ptNickName[2].y-3;
	//SetFlowerControlInfo( 2,m_ptAvatar[2].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[2].y-BIG_FACE_HEIGHT );

	//手上扑克
	m_HandCardControl.SetMaxWidth(nWidth-10);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nViewHeight-m_nYFace-m_nYBorder-20,enXCenter,enYBottom);

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//计算位置
	INT nYPosButton=nViewHeight-m_nYFace-175-52;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nYPosButton+65,0,0,uFlags);

	//留底按钮
	m_btSendConceal.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btSendConceal,NULL,(nWidth-rcButton.Width())/2,nYPosButton,0,0,uFlags);

	//出牌按钮
	m_btOutCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2+15,nYPosButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()-15,nYPosButton,0,0,uFlags);

	//功能按钮
	m_btConcealCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btRuquestLeave,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*2,nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btConcealCard,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*3,nViewHeight-5-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurnCard,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*4,nViewHeight-5-rcButton.Height(),0,0,uFlags);

	//移动控件
	EndDeferWindowPos(hDwp);

	//用户扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+78,enXCenter,enYTop);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nYPosButton-5,enXCenter,enYBottom);
	m_UserCardControl[3].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-110,nViewHeight/2-71,enXRight,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+110,nViewHeight/2-71,enXLeft,enYCenter);


	//叫牌窗口
	CRect rcCallCard;
	m_CallCardWnd.GetWindowRect(&rcCallCard);
	m_CallCardWnd.SetWindowPos(NULL,nWidth/2+20,nYPosButton,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//积分视图
	CRect rcScore;
	m_ScoreView.GetWindowRect(&rcScore);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcScore.Width())/2,nViewHeight/2-200,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//底牌窗口
	CRect rcConceal;
	m_ConcealCardView.GetWindowRect(&rcConceal);
	m_ConcealCardView.SetWindowPos(NULL,(nWidth-rcConceal.Width())/2,nViewHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);

#endif		//VIDEO_GAME

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,DRAW_MODE_SPREAD);
	//DrawViewImage(pDC,m_ImageCenter,enMode_Centent);
	//CImageHandle HandleCenter(&m_ImageCenter);
	m_ImageCenter.BitBlt(pDC->m_hDC,nWidth/2-m_ImageCenter.GetWidth()/2,
		nHeight/2-m_ImageCenter.GetHeight()/2-40);

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
//#ifndef	_DEBUG
//		WORD wUserTimer=GetUserClock(i);
//		
//		IClientUserItem * pUserData=GetClientUserItem(i);
//#else
//		
//#endif

		WORD wUserTimer=GetUserClock(i);

		IClientUserItem * pUserData=GetClientUserItem(i);
		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign(i==3?TA_RIGHT:TA_LEFT);
			//DrawTextString(pDC,pUserData->GetNickName(),RGB(255,255,255),RGB(0,0,255),m_ptNickName[i].x,m_ptNickName[i].y);


			CRect static StrRect;
			StrRect.left = m_ptNickName[i].x;
			StrRect.top = m_ptNickName[i].y;
			StrRect.right = StrRect.left+85;
			StrRect.bottom = StrRect.top+20;
			DrawTextString(pDC,pUserData->GetNickName(), RGB(255,255,255),RGB(0,0,255),StrRect);

			//其他信息
			if (wUserTimer!=0) DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
			if (pUserData->GetUserStatus()==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pUserData);
			//托管标志
			if( m_bStrustee[i] )
				m_PngStrustee.DrawImage(pDC,m_ptStrustee[i].x,m_ptStrustee[i].y);
		}

	}
	
	//得分扑克
	//CImageHandle HandleScoreBack(&m_ImageStatusInfo);
	int nXPos = m_nXBorder+3;
	int nYPos = m_nYBorder + 3;
	m_ImageStatusInfo.BitBlt(pDC->m_hDC,nXPos,nYPos);
	if (m_cbScoreCardCount>0)
	{
		//加载资源
		//CImageHandle HandleScoreCard(&m_ImageScoreCard);
		CSize SizeScoreCard(m_ImageScoreCard.GetWidth()/3,m_ImageScoreCard.GetHeight()/4);

		//绘画扑克
		for (WORD i=0;i<m_cbScoreCardCount;i++)
		{
			//获取位置
			INT nXImagePos=0;
			INT nYImagePos=0;

			//获取位置
			switch (m_cbScoreCardData[i]&0x0F)
			{
			case 5:
				{
					nXImagePos=0;
					nYImagePos=((m_cbScoreCardData[i]&0xF0)>>4)*SizeScoreCard.cy;
					break;
				}
			case 10:
				{
					nXImagePos=SizeScoreCard.cx;
					nYImagePos=((m_cbScoreCardData[i]&0xF0)>>4)*SizeScoreCard.cy;
					break;
				}
			case 13:
				{
					nXImagePos=SizeScoreCard.cx*2;
					nYImagePos=((m_cbScoreCardData[i]&0xF0)>>4)*SizeScoreCard.cy;
					break;
				}
			}

			//绘画扑克
			INT nXPos1=nXPos+14*(i%16)+3;
			INT nYPos1=nYPos+25*(i/16)+3;
			m_ImageScoreCard.TransDrawImage(pDC,nXPos1,nYPos1,SizeScoreCard.cx,SizeScoreCard.cy,nXImagePos,nYImagePos,RGB(17,66,87));
		}
	}

	nXPos += 77;
	nYPos += 106;
	CRect rcDraw(nXPos,nYPos,nXPos+163,nYPos+17);
	//主牌信息
	if ((m_cbValueOrder[0]!=VALUE_ERROR)&&(m_cbValueOrder[1]!=VALUE_ERROR))
	{
		//变量定义
		LPCTSTR szValue[]=
		{
			TEXT("A"),TEXT("2"),TEXT("3"),TEXT("4"),TEXT("5"),
			TEXT("6"),TEXT("7"),TEXT("8"),TEXT("9"),TEXT("10"),
			TEXT("J"),TEXT("Q"),TEXT("K")
		};

		//构造信息
		TCHAR szValueOrder[128]=TEXT("");
		myprintf(szValueOrder,CountArray(szValueOrder),TEXT("%s  [ 本方打 %s，对方打 %s ]"),szValue[m_cbMainValue-1],
			szValue[m_cbValueOrder[0]-1],szValue[m_cbValueOrder[1]-1]);

		//绘画信息
		pDC->SetTextAlign(TA_LEFT);
		DrawTextString(pDC,szValueOrder,RGB(255,255,255),RGB(0,0,0),rcDraw);
	}

	rcDraw.OffsetRect(7,22);
	rcDraw.right = rcDraw.left+78;
	//叫牌用户
	if (m_wCallCardUser!=INVALID_CHAIR)
	{
		//获取用户
#ifndef	TEST
		IClientUserItem *pUserData = GetClientUserItem(m_wCallCardUser);
#else
		IClientUserItem *pUserData = new IClientUserItem;
		myprintf(pUserData->GetNickName(),CountArray(pUserData->GetNickName()),TEXT("%s"),TEXT("S"));
#endif

		//绘画用户
		if (pUserData!=NULL)
		{
			pDC->SetTextAlign(TA_LEFT);
			DrawTextString(pDC,pUserData->GetNickName(),RGB(255,255,255),RGB(0,0,0),rcDraw);

			//叫牌花色
			if (m_cbCallCount>0)
			{
				//加载资源
				CSize SizeCallColor;
				//CImageHandle HandleCallColor(&m_ImageColorFlag);
				SizeCallColor.SetSize(m_ImageColorFlag.GetWidth()/5,m_ImageColorFlag.GetHeight());

				//叫牌花色
				for (BYTE i=0;i<m_cbCallCount;i++)
				{
					INT nXPos1=rcDraw.right+52+i*SizeCallColor.cx-(m_cbCallCount*SizeCallColor.cx)/2;
					m_ImageColorFlag.TransDrawImage(pDC,nXPos1,rcDraw.top,SizeCallColor.cx,SizeCallColor.cy,
						SizeCallColor.cx*((m_cbCallColor&0xF0)>>4),0,RGB(255,0,255));
				}
			}
		}
#ifdef	TEST
		delete pUserData;
#endif
	}

	rcDraw.OffsetRect(0,22);
	//当前得分
	if (m_wCardScore!=0xFFFF)
	{
		//构造信息
		TCHAR szCardScore[16]=TEXT("");
		myprintf(szCardScore,CountArray(szCardScore),TEXT("%d"),m_wCardScore);

		//绘画信息
		pDC->SetTextAlign(TA_LEFT);
		DrawTextString(pDC,szCardScore,RGB(255,255,255),RGB(0,0,0),&rcDraw);
	}

	//庄家信息
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		//庄家标志
		//CImageHandle ImageHandleBanker(&m_ImageBanker);
		m_ImageBanker.TransDrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y,m_ImageBanker.GetWidth(),m_ImageBanker.GetHeight()
			,0,0,RGB(255,0,255));
	}

	//底牌标志
	if (m_bWaitConceal==true)
	{
		//加载资源
		//CImageHandle HandleWaitConceal(&m_ImageWaitConceal);

		//绘画标志
		int nImageWidth=m_ImageWaitConceal.GetWidth();
		int nImageHegith=m_ImageWaitConceal.GetHeight();
		m_ImageWaitConceal.TransDrawImage(pDC,(nWidth-nImageWidth)/2,(nHeight-nImageHegith)/2-50,RGB(255,0,255));
	}

	//上轮标志
	if (m_bLastTurnCard==true)
	{
		//加载资源
		//CImageHandle HandleLastTurn(&m_ImageLastTurn);

		//绘画标志
		int nImageWidth=m_ImageLastTurn.GetWidth();
		int nImageHegith=m_ImageLastTurn.GetHeight();
		m_ImageLastTurn.TransDrawImage(pDC,(nWidth-nImageWidth)/2,(nHeight-nImageHegith)/2-65,RGB(255,0,255));
	}

	return;
}


//设置得分
void CGameClientView::SetCardScore(WORD wCardScore)
{
	//设置用户
	if (wCardScore!=m_wCardScore)
	{
		//设置变量
		m_wCardScore=wCardScore;

		//更新界面
		RefreshGameView();
	}

	return;
}

//庄家用户
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	//设置用户
	if (wBankerUser!=m_wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		RefreshGameView();
	}

	return;
}

//设置提示
void CGameClientView::SetWaitConceal(bool bWaitConceal)
{
	//设置变量
	if (bWaitConceal!=m_bWaitConceal)
	{
		//设置变量
		m_bWaitConceal=bWaitConceal;

		//更新界面
		RefreshGameView();
	}

	return;
}

//设置上轮
void CGameClientView::SetLastTurnCard(bool bLastTurnCard)
{
	//设置变量
	if (bLastTurnCard!=m_bLastTurnCard)
	{
		//设置变量
		m_bLastTurnCard=bLastTurnCard;

		//更新界面
		RefreshGameView();
	}

	return;
}

//设置主牌
void CGameClientView::SetValueOrder(BYTE cbMainValue, BYTE cbValueMySelf, BYTE cbValueOther)
{
	//设置变量
	m_cbMainValue=cbMainValue;
	m_cbValueOrder[0]=cbValueMySelf;
	m_cbValueOrder[1]=cbValueOther;

	//更新界面
	RefreshGameView();

	return;
}

//设置亮主
void CGameClientView::SetCallCardInfo(WORD wCallCardUser, BYTE cbCallColor, BYTE cbCallCount)
{
	//设置变量
	m_cbCallColor=cbCallColor;
	m_cbCallCount=cbCallCount;
	m_wCallCardUser=wCallCardUser;

	//更新界面
	RefreshGameView();

	return;
}

//艺术字体
//void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
//{
//	//变量定义
//	int nStringLength=lstrlen(pszString);
//	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
//	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};
//
//	//绘画边框
//	pDC->SetTextColor(crFrame);
//	for (int i=0;i<CountArray(nXExcursion);i++)
//	{
//		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
//
//		TextOut();
//	}
//
//	//绘画字体
//	pDC->SetTextColor(crText);
//	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);
//
//	return;
//}

//艺术字体
//void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect)
//{
//	//变量定义
//	int nStringLength=lstrlen(pszString);
//	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
//	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};
//
//	//绘画边框
//	pDC->SetTextColor(crFrame);
//	CRect rcDraw;
//	for (int i=0;i<CountArray(nXExcursion);i++)
//	{
//		rcDraw.CopyRect(lpRect);
//		rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
//		pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
//	}
//
//	//绘画字体
//	rcDraw.CopyRect(lpRect);
//	pDC->SetTextColor(crText);
//	pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
//
//	return;
//}
//

//开始按钮
void CGameClientView::OnStart()
{
	SendEngineMessage(IDM_START,0,0);
	return;
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	SendEngineMessage(IDM_OUT_CARD,1,1);
	return;
}

//提示按钮
void CGameClientView::OnOutPrompt()
{
	SendEngineMessage(IDM_OUT_PROMPT,0,0);
	return;
}

//留底按钮
void CGameClientView::OnSendConceal()
{
	SendEngineMessage(IDM_SEND_CONCEAL,0,0);
	return;
}

//查看底牌
void CGameClientView::OnConcealCard()
{
	SendEngineMessage(IDM_CONCEAL_CARD,0,0);
	return;
}

//请求离开
void CGameClientView::OnRuquestLeave()
{
	SendEngineMessage(IDM_REQUEST_LEAVE,0,0);
	return;
}

//上轮扑克
void CGameClientView::OnLastTurnCard()
{
	SendEngineMessage(IDM_LAST_TURN_CARD,0,0);
	return;
}

//拖管控制
void CGameClientView::OnStusteeControl()
{
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}

//鼠标消息
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_btSendConceal.EnableWindow(FALSE);
	m_HandCardControl.ShootAllCard(false);

	return;
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_btSendConceal.EnableWindow(FALSE);
	m_HandCardControl.ShootAllCard(false);

#ifdef	TEST
	BYTE cbCard[MAX_COUNT] = {
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	
	0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D
	};
	DWORD dwCount = m_HandCardControl.GetCardCount();
	m_HandCardControl.SetCardData(cbCard,dwCount/2);
	RefreshGameView();
#endif

	return;
}

//设置得分牌
void CGameClientView::SetScoreData( const BYTE cbScoreCard[],BYTE cbCardCount )
{
	if( cbCardCount == 0 || cbScoreCard == NULL )
	{
		m_cbScoreCardCount = 0;
		ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));
	}else
	{
		m_cbScoreCardCount = cbCardCount;
		CopyMemory(m_cbScoreCardData,cbScoreCard,sizeof(BYTE)*cbCardCount);
	}
	RefreshGameView();
}

//设置托管标志
void CGameClientView::SetStrustee( WORD wChairId,bool bStrustee )
{
	if( INVALID_CHAIR == wChairId )
	{
		memset(m_bStrustee,bStrustee,sizeof(m_bStrustee));
	}else
	{
		m_bStrustee[wChairId] = bStrustee;
	}
	RefreshGameView();
	return;
}

//更新视图
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}
//更新视图
void CGameClientView::RefreshGameView(CRect &rect)
{
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}

//////////////////////////////////////////////////////////////////////////
