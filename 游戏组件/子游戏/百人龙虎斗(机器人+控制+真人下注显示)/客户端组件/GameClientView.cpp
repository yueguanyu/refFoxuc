#include "StdAfx.h"
#include "Resource.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER			100									//闪动标识
#define IDI_SHOW_CHANGE_BANKER		101									//轮换庄家
#define IDI_DISPATCH_CARD			102									//发牌标识
#define IDI_BOMB_EFFECT				200									//爆炸标识


//按钮标识
#define IDC_JETTON_BUTTON_100		200									//按钮标识
#define IDC_JETTON_BUTTON_1000		201									//按钮标识
#define IDC_JETTON_BUTTON_10000		202									//按钮标识
#define IDC_JETTON_BUTTON_100000	203									//按钮标识
#define IDC_JETTON_BUTTON_1000000	204									//按钮标识
#define IDC_JETTON_BUTTON_5000000	205									//按钮标识
#define IDC_APPY_BANKER				206									//按钮标识
#define IDC_CANCEL_BANKER			207									//按钮标识
#define IDC_SCORE_MOVE_L			209									//按钮标识
#define IDC_SCORE_MOVE_R			210									//按钮标识
#define IDC_VIEW_CHART				211									//按钮标识
#define IDC_ADMIN					212									//按钮标识


//爆炸数目
#define BOMB_EFFECT_COUNT			8									//爆炸数目

void ViewMakeString(CString &strNum,LONGLONG lNumber)
{
	CString strTempNum;
	strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

	int nLength = strTempNum.GetLength();
	for (int i = 0; i < int((nLength-1)/3); i++)
		strTempNum.Insert(nLength - 3*(i+1), _T(","));
	strNum.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
	return;
}


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//全体下注
	ZeroMemory(m_lAreaInAllScore,sizeof(m_lAreaInAllScore));

	//下注信息
	ZeroMemory(m_lMeInAllScore,sizeof(m_lMeInAllScore));

	//发牌变量
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;

	//当局成绩
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//动画变量
	ZeroMemory(m_bBombEffect,sizeof(m_bBombEffect));
	ZeroMemory(m_cbBombFrameIndex,sizeof(m_cbBombFrameIndex));

	//状态信息
	m_lCurrentJetton=0L;
	m_cbWinnerSide=0xFF;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bFlashResult = false;
	m_blCanStore=false;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//历史成绩
	m_lMeStatisticScore=0;

	//控件变量
	m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	
	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageWinFlags.LoadFromResource(hInstance,IDB_WIN_FLAGS);
	m_ImageJettonView.LoadFromResource(hInstance,IDB_JETTOM_VIEW);
	m_ImageScoreNumber.LoadFromResource(hInstance,IDB_SCORE_NUMBER);
	m_ImageMeScoreNumber.LoadFromResource(hInstance,IDB_ME_SCORE_NUMBER);


	m_ImageFrame[AREA_LONG].LoadFromResource(hInstance, IDB_K_LONG );
	m_ImageFrame[AREA_PING].LoadFromResource( hInstance,IDB_K_HE );
	m_ImageFrame[AREA_HU].LoadFromResource( hInstance,IDB_K_HU );
	m_ImageFrame[AREA_2_13].LoadFromResource(hInstance, IDB_K_2_13 );
	m_ImageFrame[AREA_14].LoadFromResource(hInstance, IDB_K_14 );
	m_ImageFrame[AREA_15_26].LoadFromResource(hInstance, IDB_K_15_26 );
	m_ImageFrame[AREA_2_6].LoadFromResource( hInstance,IDB_K_2_6 );
	m_ImageFrame[AREA_7_11].LoadFromResource(hInstance, IDB_K_7_11 );
	m_ImageFrame[AREA_12_16].LoadFromResource(hInstance, IDB_K_12_16 );
	m_ImageFrame[AREA_17_21].LoadFromResource(hInstance, IDB_K_17_21 );
	m_ImageFrame[AREA_22_26].LoadFromResource( hInstance,IDB_K_22_26 );


	m_ImageGameEnd.LoadFromResource( hInstance,IDB_GAME_END );
	m_ImageGameEndFrame.LoadFromResource( hInstance,IDB_GAME_END_FRAME );
	m_ImageGamePoint.LoadFromResource( hInstance,IDB_GAME_POINT );

	m_ImageMeBanker.LoadFromResource(hInstance, IDB_ME_BANKER );
	m_ImageChangeBanker.LoadFromResource( hInstance,IDB_CHANGE_BANKER );
	m_ImageNoBanker.LoadFromResource( hInstance,IDB_NO_BANKER );	

	m_ImageTimeFlag.LoadFromResource(hInstance,IDB_TIME_FLAG);

	m_ImageBombEffect.LoadImage(hInstance,TEXT("FIRE_EFFECT"));

	m_szViewBack.SetSize( m_ImageViewBack.GetWidth(), m_ImageViewBack.GetHeight() );

	m_pClientControlDlg = NULL;
	m_hInst = NULL;

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}


//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	AfxSetResourceHandle(AfxGetInstanceHandle());
	//创建控件
	CRect rcCreate(0,0,0,0);

	m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
	m_ApplyUser.Create( IDD_DLG_GAME_RECORD	, this);
	m_GameRecord.ShowWindow(SW_HIDE);

	//下注按钮
	m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
	m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
	m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
	m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
	m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
	m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);
	m_btViewChart.Create(NULL,WS_CHILD,rcCreate,this,IDC_VIEW_CHART);

	//申请按钮
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);



	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false,false);
	m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false,false);
	m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false,false);
	m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false,false);
	m_btJetton1000000.SetButtonImage(IDB_BT_JETTON_1000000,hResInstance,false,false);
	m_btJetton5000000.SetButtonImage(IDB_BT_JETTON_5000000,hResInstance,false,false);

	m_btViewChart.SetButtonImage(IDB_BT_CHART,hResInstance,false,false);	

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

	//扑克控件
	m_CardControl[INDEX_LONG].SetDisplayFlag(true);
	m_CardControl[INDEX_HU].SetDisplayFlag(true);

#ifdef __BANKER___
	m_btBankerStorage.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_STORAGE);
	m_btBankerDraw.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_DRAW);

	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);
#endif

	//路单控件
	if (m_DlgViewChart.m_hWnd==NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);


	//控制按钮
	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	//控制
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("LongHuDouClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}
	return 0;
}


//重置界面
VOID CGameClientView::ResetGameView()
{
	//全体下注
	ZeroMemory(m_lAreaInAllScore,sizeof(m_lAreaInAllScore));

	//下注信息
	ZeroMemory(m_lMeInAllScore,sizeof(m_lMeInAllScore));

	//发牌变量
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;

	//当局成绩
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//动画变量
	ZeroMemory(m_bBombEffect,sizeof(m_bBombEffect));
	ZeroMemory(m_cbBombFrameIndex,sizeof(m_cbBombFrameIndex));

	//状态信息
	m_lCurrentJetton=0L;
	m_cbWinnerSide=0xFF;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_blCanStore=false;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//历史成绩
	m_lMeStatisticScore=0;

	//发牌变量
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));

	//清空列表
	m_ApplyUser.ClearAll();

	//清除桌面
	CleanUserJetton();

	//动画变量
	ZeroMemory(m_bBombEffect,sizeof(m_bBombEffect));
	ZeroMemory(m_cbBombFrameIndex,sizeof(m_cbBombFrameIndex));

	//设置按钮
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

	return;
}
//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_JETTON_BUTTON_100:
		m_lCurrentJetton=100L;
		break;
	case IDC_JETTON_BUTTON_1000:
		m_lCurrentJetton=1000L;
		break;
	case IDC_JETTON_BUTTON_10000:
		m_lCurrentJetton=10000L;
		break;
	case IDC_JETTON_BUTTON_100000:
		m_lCurrentJetton=100000L;
		break;
	case IDC_JETTON_BUTTON_1000000:
		m_lCurrentJetton=1000000L;
		break;
	case IDC_JETTON_BUTTON_5000000:
		m_lCurrentJetton=5000000L;
		break;
	case IDC_VIEW_CHART:
			OnViewChart();
		break;
	case IDC_APPY_BANKER:
		SendEngineMessage(IDM_APPLY_BANKER,1,0);		
		break;
	case IDC_CANCEL_BANKER:
		SendEngineMessage(IDM_APPLY_BANKER,0,0);
		break;
	case IDC_SCORE_MOVE_L:
		OnScoreMoveL();
		break;
	case IDC_SCORE_MOVE_R:
		OnScoreMoveR();
		break;
	case IDC_BANK_STORAGE:
		OnBankStorage();
		break;
	case IDC_BANK_DRAW:
		OnBankDraw();
		break;
	case IDC_ADMIN:
		OpenAdminWnd();
		break;
	}
	return __super::OnCommand(wParam, lParam);
}
//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//位置信息
	m_nWinFlagsExcursionX = nWidth/2-148;	
	m_nWinFlagsExcursionY = nHeight/2 + 251;
	m_nWinPointsExcursionX = nWidth/2-353;
	m_nWinPointsExcursionY = nHeight/2 - 267;	

	//区域位置
	int nCenterX = nWidth/2, nCenterY = nHeight/2;

	//位置信息
	m_ptOffset.x = (nWidth - m_szViewBack.cx)/2;
	m_ptOffset.y = (nHeight - m_szViewBack.cy)/2;

	//区域位置
	m_rcBetAreaBox[AREA_LONG].left = m_ptOffset.x + 93;
	m_rcBetAreaBox[AREA_LONG].top = m_ptOffset.y + 193;
	m_rcBetAreaBox[AREA_LONG].right = m_rcBetAreaBox[AREA_LONG].left + 186;
	m_rcBetAreaBox[AREA_LONG].bottom = m_rcBetAreaBox[AREA_LONG].top + 125;

	m_rcBetAreaBox[AREA_PING].left = m_ptOffset.x + 276;
	m_rcBetAreaBox[AREA_PING].top = m_ptOffset.y + 193;
	m_rcBetAreaBox[AREA_PING].right = m_rcBetAreaBox[AREA_PING].left + 267;
	m_rcBetAreaBox[AREA_PING].bottom = m_rcBetAreaBox[AREA_PING].top + 125;

	m_rcBetAreaBox[AREA_HU].left = m_ptOffset.x + 540;
	m_rcBetAreaBox[AREA_HU].top = m_ptOffset.y + 193;
	m_rcBetAreaBox[AREA_HU].right = m_rcBetAreaBox[AREA_HU].left + 185;
	m_rcBetAreaBox[AREA_HU].bottom = m_rcBetAreaBox[AREA_HU].top + 125;

	m_rcBetAreaBox[AREA_2_13].left = m_ptOffset.x + 93;
	m_rcBetAreaBox[AREA_2_13].top = m_ptOffset.y + 315;
	m_rcBetAreaBox[AREA_2_13].right = m_rcBetAreaBox[AREA_2_13].left + 186;
	m_rcBetAreaBox[AREA_2_13].bottom = m_rcBetAreaBox[AREA_2_13].top + 86;

	m_rcBetAreaBox[AREA_14].left = m_ptOffset.x + 276;
	m_rcBetAreaBox[AREA_14].top = m_ptOffset.y + 315;
	m_rcBetAreaBox[AREA_14].right = m_rcBetAreaBox[AREA_14].left+ 267;
	m_rcBetAreaBox[AREA_14].bottom = m_rcBetAreaBox[AREA_14].top + 86;

	m_rcBetAreaBox[AREA_15_26].left = m_ptOffset.x + 540;
	m_rcBetAreaBox[AREA_15_26].top = m_ptOffset.y + 315;
	m_rcBetAreaBox[AREA_15_26].right = m_rcBetAreaBox[AREA_15_26].left + 185;
	m_rcBetAreaBox[AREA_15_26].bottom = m_rcBetAreaBox[AREA_15_26].top + 86;

	m_rcBetAreaBox[AREA_2_6].left = m_ptOffset.x + 93;
	m_rcBetAreaBox[AREA_2_6].top = m_ptOffset.y + 398;
	m_rcBetAreaBox[AREA_2_6].right = m_rcBetAreaBox[AREA_2_6].left + 130;
	m_rcBetAreaBox[AREA_2_6].bottom = m_rcBetAreaBox[AREA_2_6].top + 86;

	m_rcBetAreaBox[AREA_7_11].left = m_ptOffset.x + 220;
	m_rcBetAreaBox[AREA_7_11].top = m_ptOffset.y + 398;
	m_rcBetAreaBox[AREA_7_11].right = m_rcBetAreaBox[AREA_7_11].left + 131;
	m_rcBetAreaBox[AREA_7_11].bottom = m_rcBetAreaBox[AREA_7_11].top + 86;

	m_rcBetAreaBox[AREA_12_16].left = m_ptOffset.x + 348;
	m_rcBetAreaBox[AREA_12_16].top = m_ptOffset.y + 398;
	m_rcBetAreaBox[AREA_12_16].right = m_rcBetAreaBox[AREA_12_16].left + 131;
	m_rcBetAreaBox[AREA_12_16].bottom = m_rcBetAreaBox[AREA_12_16].top + 86;

	m_rcBetAreaBox[AREA_17_21].left = m_ptOffset.x + 476;
	m_rcBetAreaBox[AREA_17_21].top = m_ptOffset.y + 398;
	m_rcBetAreaBox[AREA_17_21].right = m_rcBetAreaBox[AREA_17_21].left + 129;
	m_rcBetAreaBox[AREA_17_21].bottom = m_rcBetAreaBox[AREA_17_21].top + 86;

	m_rcBetAreaBox[AREA_22_26].left = m_ptOffset.x + 602;
	m_rcBetAreaBox[AREA_22_26].top = m_ptOffset.y + 398;
	m_rcBetAreaBox[AREA_22_26].right = m_rcBetAreaBox[AREA_22_26].left + 123;
	m_rcBetAreaBox[AREA_22_26].bottom = m_rcBetAreaBox[AREA_22_26].top + 86;

	for(int i = 0 ; i < AREA_ALL; ++i)
	{
		m_rcBetArea[i].top = m_rcBetAreaBox[i].top + 3;
		m_rcBetArea[i].bottom = m_rcBetAreaBox[i].bottom - 3;
		m_rcBetArea[i].left = m_rcBetAreaBox[i].left + 3;
		m_rcBetArea[i].right = m_rcBetAreaBox[i].right - 3;
	}



	int ExcursionY=10;
	for ( int i = 0; i < AREA_ALL; ++i )
	{
		//筹码数字
		m_PointJettonNumber[i].SetPoint((m_rcBetArea[i].right+m_rcBetArea[i].left)/2, (m_rcBetArea[i].bottom+m_rcBetArea[i].top)/2-ExcursionY);
		//筹码位置
		m_PointJetton[i].SetPoint(m_rcBetArea[i].left, m_rcBetArea[i].top);
	}


	//扑克控件
	m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2-25+17,nHeight/2-190),enXRight,enYTop);
	m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2+25-15,nHeight/2-190),enXLeft,enYTop);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//列表控件
	DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 + 118,nHeight/2-314,256,80,uFlags);

	//筹码按钮
	CRect rcJetton;
	m_btJetton100.GetWindowRect(&rcJetton);
	int nYPos = nHeight/2+147;
	int nXPos = nWidth/2-130+8;
	int nSpace = 5;

	DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos + nSpace + rcJetton.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos + nSpace * 4 + rcJetton.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace * 5 + rcJetton.Width() * 5,nYPos,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btViewChart,NULL,nWidth/2-377,nHeight/2+180,0,0,uFlags|SWP_NOSIZE);

	//上庄按钮
	DeferWindowPos(hDwp,m_btApplyBanker,NULL,nWidth/2+280,nHeight/2-352,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth/2+280,nHeight/2-352,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btScoreMoveL,NULL,nWidth/2-203,nHeight/2+272,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR,NULL,nWidth/2+302,nHeight/2+270,0,0,uFlags|SWP_NOSIZE);

#ifdef __BANKER___
	m_btBankerDraw.GetWindowRect(&rcJetton);
	DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2-370,nHeight/2+192,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2-310,nHeight/2+192,0,0,uFlags|SWP_NOSIZE);
#endif
	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}


//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);
	
	//获取状态
	BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

	//状态提示
	CFont static InfoFont;
	InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	CFont * pOldFont=pDC->SelectObject(&InfoFont);
	pDC->SetTextColor(RGB(255,234,0));
	CRect rcDispatchCardTips(m_nWinPointsExcursionX, m_nWinPointsExcursionY+50, m_nWinPointsExcursionX + 220, m_nWinPointsExcursionY+115);

	if (m_pGameClientDlg->GetGameStatus()==GAME_SCENE_PLACE_JETTON)
		pDC->DrawText( TEXT("游戏开始了，请您下分"), rcDispatchCardTips, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_WORDBREAK );
	//else if (m_pGameClientDlg->GetGameStatus()==GS_GAME_END)
	//	pDC->DrawText( m_strDispatchCardTips, rcDispatchCardTips, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_WORDBREAK );

	pDC->SelectObject(pOldFont);
	InfoFont.DeleteObject();

	//时间提示

	int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	int nFlagIndex=0;
	if (cbGameStatus==GAME_STATUS_FREE) 
		nFlagIndex=0;
	else if (cbGameStatus==GAME_SCENE_PLACE_JETTON) 
		nFlagIndex=1;
	else if (cbGameStatus==GAME_SCENE_GAME_END)
		nFlagIndex=2;

	m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-230 + 25, nHeight/2+195, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nFlagIndex * nTimeFlagWidth, 0);

	//最大下注
	pDC->SetTextColor(RGB(255,234,0));
	LONGLONG lLimitScore=GetMaxPlayerScore(AREA_LONG);
	if ( !m_bEnableSysBanker && m_wBankerUser == INVALID_CHAIR )
		lLimitScore = 0l;
	DrawNumberStringWithSpace(pDC,lLimitScore,nWidth/2-300, nHeight/2 - 347);

	lLimitScore=GetMaxPlayerScore(AREA_HU);
	if ( !m_bEnableSysBanker && m_wBankerUser == INVALID_CHAIR )
		lLimitScore = 0l;
	DrawNumberStringWithSpace(pDC,lLimitScore,nWidth/2-300, nHeight/2 - 322);

	lLimitScore=GetMaxPlayerScore(AREA_PING);
	if ( !m_bEnableSysBanker && m_wBankerUser == INVALID_CHAIR )
		lLimitScore = 0l;
	DrawNumberStringWithSpace(pDC,lLimitScore,nWidth/2-300, nHeight/2 - 299);

	//胜利边框
	FlashJettonAreaFrame(nWidth,nHeight,pDC);

	//筹码资源
	CSize SizeJettonItem(m_ImageJettonView.GetWidth()/JETTON_COUNT,m_ImageJettonView.GetHeight());

	//绘画筹码
	for (INT i=0;i<AREA_ALL;i++)
	{
		//变量定义
		LONGLONG lScoreCount=0L;
		LONGLONG lScoreJetton[JETTON_COUNT]={100L,1000L,10000L,100000L,1000000L,5000000L};

		//绘画筹码
		for (INT_PTR j=0;j<m_JettonInfoArray[i].GetCount();j++)
		{
			//获取信息
			tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

			//累计数字
			ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
			lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

			//绘画界面
			m_ImageJettonView.TransDrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[i].x,
				pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,
				pJettonInfo->cbJettonIndex*SizeJettonItem.cx,0,RGB(255,0,255) );
		}

		//绘画数字
		if (lScoreCount>0L)	DrawNumberString(pDC,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
	}

	//庄家信息																											
	pDC->SetTextColor(RGB(255,234,0));
	
	//获取玩家
	IClientUserItem *pUserItem =NULL;
	tagUserInfo  *pUserData = NULL;
	if(m_wBankerUser != INVALID_CHAIR)
		pUserItem= GetClientUserItem(m_wBankerUser);
	if(pUserItem != NULL)
	{
		pUserData = pUserItem->GetUserInfo();
	}
	
	//位置信息
	CRect static StrRect;
	StrRect.left = nWidth/2-85;
	StrRect.top = nHeight/2 - 340;
	StrRect.right = StrRect.left + 100;
	StrRect.bottom = StrRect.top + 15;
	

	//庄家名字
	pDC->DrawText(pUserData == NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->szNickName, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

	//庄家总分
	StrRect.left = nWidth/2-85;
	StrRect.top = nHeight/2 - 316;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,pUserData==NULL?0:pUserData->lScore, StrRect);

	//庄家局数
	StrRect.left = nWidth/2-85;
	StrRect.top = nHeight/2 - 266;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_wBankerTime,StrRect);

	//庄家成绩
	StrRect.left = nWidth/2-85;
	StrRect.top = nHeight/2 - 291;
	StrRect.right = StrRect.left + 190;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);

	//绘画用户
	if (m_wMeChairID!=INVALID_CHAIR)
	{
		IClientUserItem *pUserItem =GetClientUserItem(m_wMeChairID);

		tagUserInfo *pMeUserData = NULL;
		if(pUserItem!=NULL)
			pMeUserData = pUserItem->GetUserInfo();
		if ( pMeUserData != NULL )
		{
			//游戏信息
			pDC->SetTextColor(RGB(255,255,255));


			LONGLONG lMeJetton = 0;
			for (int nAreaIndex = 0; nAreaIndex < AREA_ALL; ++nAreaIndex ) 
				lMeJetton += m_lMeInAllScore[nAreaIndex];

			CRect rcAccount(CPoint(nWidth/2-297,nHeight/2+268),CPoint(nWidth/2-297+73,nHeight/2+272+15));
			CRect rcGameScore(CPoint(nWidth/2-297,nHeight/2+294),CPoint(nWidth/2-297+73,nHeight/2+294+10));
			CRect rcResultScore(CPoint(nWidth/2-297,nHeight/2+294+23),CPoint(nWidth/2-297+73,nHeight/2+294+10+23));

			CString strM;
			ViewMakeString(strM, pMeUserData->lScore-lMeJetton);
			pDC->DrawText(strM,lstrlen(strM),rcGameScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);

			ViewMakeString(strM, m_lMeStatisticScore);
			pDC->DrawText(strM,lstrlen(strM),rcResultScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);

			//DrawNumberStringWithSpace(pDC,pMeUserData->lScore-lMeJetton,rcGameScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
			//DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcResultScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
			pDC->DrawText(pMeUserData->szNickName,lstrlen(pMeUserData->szNickName),rcAccount,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
		}
	}

	//切换庄家
	if ( m_bShowChangeBanker )
	{
		int	nXPos = nWidth / 2 - 130;
		int	nYPos = nHeight / 2 - 160;

		//由我做庄
		if ( m_wMeChairID == m_wBankerUser )
		{
			m_ImageMeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else if ( m_wBankerUser != INVALID_CHAIR )
		{
			m_ImageChangeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else
		{
			m_ImageNoBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
	}

	//我的下注
	DrawMeJettonNumber(pDC);

	//绘画时间
	if (m_wMeChairID!=INVALID_CHAIR)
	{
		WORD wUserTimer=GetUserClock(m_wMeChairID);
		if (wUserTimer!=0) 
			DrawUserClock(pDC,nWidth/2-255 + 25,nHeight/2-260+435,wUserTimer);
	}

	//结束状态
	if (cbGameStatus==GAME_SCENE_GAME_END)
	{
		int	nXPos = nWidth / 2 - 129;
		int	nYPos = nHeight / 2 - 208;

		CRect rcAlpha(nXPos, nYPos, nXPos+ m_ImageGameEndFrame.GetWidth(), nYPos+m_ImageGameEndFrame.GetHeight());
		DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);

		//绘画边框
		m_ImageGameEndFrame.TransDrawImage(pDC, nXPos, nYPos, RGB(255,0,255));

		//绘画扑克
		m_CardControl[INDEX_LONG].DrawCardControl(pDC);
		m_CardControl[INDEX_HU].DrawCardControl(pDC);

		//发牌扑克
		if (m_ptDispatchCard.x!=0 && m_ptDispatchCard.y!=0) 
		{
			m_DispatchCard.SetBenchmarkPos(m_ptDispatchCard.x,m_ptDispatchCard.y,enXCenter,enYCenter);
			m_DispatchCard.DrawCardControl(pDC);
		}		
	}

	//胜利标志
	DrawWinFlags(pDC);

	//显示结果
	ShowGameResult(pDC, nWidth, nHeight);	

	//扑克点数
	if (m_cbSendCount[INDEX_LONG]+m_cbSendCount[INDEX_HU]!=0)
	{
		//变量定义
		BYTE cbPlayerPoint, cbBankerPoint;
		int	nXPos = nWidth / 2 - 129;
		int	nYPos = nHeight / 2 - 208;		

		//绘画点数
		int nPointWidth = m_ImageGamePoint.GetWidth() / 14;
		if (m_cbSendCount[INDEX_LONG]!=0)
		{
			//计算点数
			cbPlayerPoint = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_LONG][0]);
			m_ImageGamePoint.TransDrawImage(pDC, nXPos + 28, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
				cbPlayerPoint * nPointWidth, 0, RGB(255, 0, 255));
		}
		if (m_cbSendCount[INDEX_HU]!=0)
		{
			//计算点数
			cbBankerPoint = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_HU][0]);
			m_ImageGamePoint.TransDrawImage(pDC, nXPos + 28 + 136, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
				cbBankerPoint * nPointWidth, 0, RGB(255, 0, 255));
		}
	}

	//爆炸效果
	DrawBombEffect(pDC);

	return;
}

//设置信息
void CGameClientView::SetMeMaxScore(LONGLONG lMeMaxScore)
{
	if (m_lMeMaxScore!=lMeMaxScore)
	{
		//设置变量
		m_lMeMaxScore=lMeMaxScore;
	}

	return;
}

//最大下注
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
	if ( m_lAreaLimitScore!= lAreaLimitScore )
	{
		//设置变量
		m_lAreaLimitScore=lAreaLimitScore;
	}
}

//设置筹码
void CGameClientView::SetCurrentJetton(LONGLONG lCurrentJetton)
{
	//设置变量
	ASSERT(lCurrentJetton >= 0L);
	m_lCurrentJetton = lCurrentJetton;

	if ( m_lCurrentJetton == 0l )
	{
		m_cbAreaFlash = 0xFF;
	}

	return;
}

//历史记录
void CGameClientView::SetGameHistory(enOperateResult OperateResult, BYTE cbResult, BYTE cbLong, BYTE cbHu)
{
	//设置数据
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.enOperateFlags = OperateResult;
	GameRecord.cbResult = cbResult;
	GameRecord.cbLong = cbLong;
	GameRecord.cbHu = cbHu;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}

	int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( 12 < nHistoryCount ) m_btScoreMoveR.EnableWindow(TRUE);

	//移到最新记录
	if ( 12 < nHistoryCount )
	{
		m_nScoreHead = (m_nRecordLast - 12 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
	}

	//更新路子
	if (m_DlgViewChart.m_hWnd!=NULL)
	{
		m_DlgViewChart.SetGameRecord(GameRecord);
	}

	return;
}

//清理筹码
void CGameClientView::CleanUserJetton()
{
	//清理数组
	for (BYTE i=0;i<CountArray(m_JettonInfoArray);i++)
	{
		m_JettonInfoArray[i].RemoveAll();
	}

	//全体下注
	ZeroMemory(m_lAreaInAllScore,sizeof(m_lAreaInAllScore));

	//下注信息
	ZeroMemory(m_lMeInAllScore,sizeof(m_lMeInAllScore));
	m_strDispatchCardTips=TEXT("");

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//效验参数
	ASSERT(cbViewIndex<AREA_ALL);
	if (cbViewIndex>=AREA_ALL) return;

	m_lMeInAllScore[cbViewIndex] = lJettonCount;

	//更新界面
	InvalidGameView(0,0,0,0);
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbCardCount[2], BYTE cbTableCardArray[2][3])
{
	if (cbCardCount!=NULL)
	{
		m_cbCardCount[0]=cbCardCount[0];
		m_cbCardCount[1]=cbCardCount[1];
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

		//开始发牌
		DispatchCard();
	}
	else
	{
		ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}

//设置筹码
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//效验参数
	ASSERT(cbViewIndex<AREA_ALL);
	if (cbViewIndex>=AREA_ALL) return;

	//设置炸弹
	if (lScoreCount==5000000L) SetBombEffect(true,cbViewIndex);

	//变量定义
	bool bPlaceJetton=false;
	LONGLONG lScoreIndex[JETTON_COUNT]={100L,1000L,10000L,100000L,1000000L,5000000L};

	//边框宽度
	int nFrameWidth=0, nFrameHeight=0;
	int nBorderWidth=6;

	m_lAreaInAllScore[cbViewIndex] += lScoreCount;

	nFrameWidth = m_rcBetArea[cbViewIndex].right-m_rcBetArea[cbViewIndex].left;
	nFrameHeight = m_rcBetArea[cbViewIndex].bottom-m_rcBetArea[cbViewIndex].top;

	nFrameWidth += nBorderWidth;
	nFrameHeight += nBorderWidth;

	//增加判断
	bool bAddJetton=lScoreCount>0?true:false;

	if ( lScoreCount < 0 )
	{
		lScoreCount = -lScoreCount;
	}
	//增加筹码
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//计算数目
		BYTE cbScoreIndex=JETTON_COUNT-i-1;
		LONGLONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount==0L) continue;

		//加入筹码
		for (LONGLONG j=0;j<lCellCount;j++)
		{
			if (true==bAddJetton)
			{
				//构造变量
				tagJettonInfo JettonInfo;
				int nJettonSize=68;
				JettonInfo.cbJettonIndex=cbScoreIndex;
				JettonInfo.nXPos=rand()%(nFrameWidth-nJettonSize);
				JettonInfo.nYPos=rand()%(nFrameHeight-nJettonSize);

				//插入数组
				bPlaceJetton=true;
				m_JettonInfoArray[cbViewIndex].Add(JettonInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex].GetCount(); ++nIndex)
				{
					//移除判断
					tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex][nIndex];
					if (JettonInfo.cbJettonIndex==cbScoreIndex)
					{
						m_JettonInfoArray[cbViewIndex].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	//更新界面
	if (bPlaceJetton==true) 	InvalidGameView(0,0,0,0);

	return;
}

//当局成绩
void CGameClientView::SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue)
{
	m_lMeCurGameScore=lMeCurGameScore;			
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;			
	m_lBankerCurGameScore=lBankerCurGameScore;			
	m_lGameRevenue=lGameRevenue;					
}

//设置胜方
void CGameClientView::SetWinnerSide(BYTE cbWinnerSide,bool blSet)
{
	//设置变量
	m_cbWinnerSide=cbWinnerSide;
	m_cbAreaFlash=cbWinnerSide;
	m_bFlashResult=blSet;

	//设置时间
	if (cbWinnerSide!=0xFF)
	{
		SetTimer(IDI_FLASH_WINNER,500,NULL);
	}
	else KillTimer(IDI_FLASH_WINNER);

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
	for ( int i = 0 ; i < AREA_ALL ; ++i)
	{
		if (m_rcBetArea[i].PtInRect(MousePoint)) 
			return i;
	}
	return 0xFF;
}

//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore)
{
	//加载资源
	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

	if ( bMeScore ) SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/10, m_ImageMeScoreNumber.GetHeight());

	//计算数目
	INT lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
	INT nXDrawPos=nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

	//绘画桌号
	for (LONGLONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		INT lCellNumber=(INT)(lNumber%10);
		if ( bMeScore )
		{
			m_ImageMeScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}
		else
		{
			m_ImageScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}

		//设置变量
		lNumber/=10;
		nXDrawPos-=SizeScoreNumber.cx;
	};

	return;
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
	CString strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
	if (lNumber==0) strNumber=TEXT("0");
	int nNumberCount=0;
	LONGLONG lTmpNumber=lNumber;
	if (lNumber<0) lNumber=-lNumber;
	while (lNumber>0)
	{
		strTmpNumber1.Format(TEXT("%ld"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if (nNumberCount==4)
		{
			strTmpNumber2 += (TEXT(" ") +strNumber);
			strNumber=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		lNumber/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		strTmpNumber2 += (TEXT(" ") +strNumber);
		strNumber=strTmpNumber2;
	}

	if (lTmpNumber<0) strNumber=TEXT("-")+strNumber;

	//输出数字
	TextOut(pDC,nXPos,nYPos,strNumber,lstrlen(strNumber));
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat)
{
	CString strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
	if (lNumber==0) strNumber=TEXT("0");
	int nNumberCount=0;
	LONGLONG lTmpNumber=lNumber;
	if (lNumber<0) lNumber=-lNumber;
	while (lNumber>0)
	{
		strTmpNumber1.Format(TEXT("%ld"),lNumber%10);
		nNumberCount++;
		strTmpNumber2 = strTmpNumber1+strTmpNumber2;

		if (nNumberCount==4)
		{
			strTmpNumber2 += (TEXT(" ") +strNumber);
			strNumber=strTmpNumber2;
			nNumberCount=0;
			strTmpNumber2=TEXT("");
		}
		lNumber/=10;
	}

	if (strTmpNumber2.IsEmpty()==FALSE)
	{
		strTmpNumber2 += (TEXT(" ") +strNumber);
		strNumber=strTmpNumber2;
	}

	if (lTmpNumber<0) strNumber=TEXT("-")+strNumber;

	//输出数字
	if (nFormat==-1) pDC->DrawText(strNumber,rcRect,DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
	else pDC->DrawText(strNumber,rcRect,nFormat);
}
//查看路子
void CGameClientView::OnViewChart()
{
	if (m_DlgViewChart.m_hWnd==NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);

	//显示判断
	if (!m_DlgViewChart.IsWindowVisible())
	{
		m_DlgViewChart.ShowWindow(SW_SHOW);
		m_DlgViewChart.CenterWindow();
	}
	else
	{
		m_DlgViewChart.ShowWindow(SW_HIDE);
	}
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//闪动胜方
	if (nIDEvent==IDI_FLASH_WINNER)
	{
		//设置变量
		if (m_cbAreaFlash!=m_cbWinnerSide)
		{
			m_cbAreaFlash=m_cbWinnerSide;
		}
		else 
		{
			m_cbAreaFlash=0xFF;
		}
		//设置变量
		m_bFlashResult=!m_bFlashResult;

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}

	//轮换庄家
	if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )
	{
		ShowChangeBanker( false );

		return;
	}

	else if (nIDEvent==IDI_DISPATCH_CARD)
	{
		bool bPlayerCard=false;
		if ((m_cbSendCount[0]+m_cbSendCount[1])<4)
		{
			//定义索引
			WORD wIndex=(m_cbSendCount[0]+m_cbSendCount[1])%2;
			if (wIndex==0) bPlayerCard=true;
		}
		else if (m_cbSendCount[INDEX_LONG]<m_cbCardCount[INDEX_LONG]) bPlayerCard=true;

		//发牌判断
		CRect rcView;
		GetClientRect(&rcView);
		int nEndPosX = 0;
		if (bPlayerCard) nEndPosX=rcView.Width()/2;
		else nEndPosX=rcView.Width()/2+110;
		int nEndPosY = rcView.Height()/2-185;
		if ( (m_ptDispatchCard.x > nEndPosX || m_ptDispatchCard.y < nEndPosY) &&
			m_ptDispatchCard.x != 0 && m_ptDispatchCard.y != 0 )
		{
			//修改位置
			if (m_ptDispatchCard.y < nEndPosY) m_ptDispatchCard.y += 73; 
			else m_ptDispatchCard.x -= 115;

			//更新界面
			InvalidGameView(0,0,0,0);

			return;
		}

		//停顿扑克
		static BYTE cbStopCount=0;
		if (cbStopCount<6)
		{			
			cbStopCount++;
			return;
		}
		else 
		{
			cbStopCount=0;			
		}

		if ( m_cbSendCount[INDEX_LONG]+m_cbSendCount[INDEX_HU]<m_cbCardCount[INDEX_LONG] + m_cbCardCount[INDEX_HU]-1 ) 
		{
			m_ptDispatchCard.x = rcView.Width()/2+220;
			m_ptDispatchCard.y = rcView.Height()/2-200;	
		}
		else
		{
			m_DispatchCard.SetCardData(NULL,0);
		}
	
		//播放声音
		m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));

		//发送扑克
		if ((m_cbSendCount[0]+m_cbSendCount[1])<2)
		{
			//定义索引
			WORD wIndex=(m_cbSendCount[0]+m_cbSendCount[1])%2;

			//设置扑克
			m_cbSendCount[wIndex]++;
			m_CardControl[wIndex].SetCardData(m_cbTableCardArray[wIndex],m_cbSendCount[wIndex]);

			//发牌提示
			SetDispatchCardTips();

			//更新界面
			InvalidGameView(0,0,0,0);

			return;
		}

		m_ptDispatchCard.x = 0;
		m_ptDispatchCard.y = 0;

		//完成发牌
		FinishDispatchCard();

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}

	//爆炸动画
	if (nIDEvent<=IDI_BOMB_EFFECT+AREA_22_26 && IDI_BOMB_EFFECT+AREA_LONG<=nIDEvent)
	{
		WORD wIndex=nIDEvent-IDI_BOMB_EFFECT;
		//停止判断
		if (m_bBombEffect[wIndex]==false)
		{
			KillTimer(nIDEvent);
			return;
		}

		//设置变量
		if ((m_cbBombFrameIndex[wIndex]+1)>=BOMB_EFFECT_COUNT)
		{
			//删除时间
			KillTimer(nIDEvent);

			//设置变量
			m_bBombEffect[wIndex]=false;
			m_cbBombFrameIndex[wIndex]=0;
		}
		else m_cbBombFrameIndex[wIndex]++;

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if (m_lCurrentJetton != 0L)
	{
		BYTE cbJettonArea = GetJettonArea(Point);

		LONGLONG lMaxPlayerScore = GetMaxPlayerScore(cbJettonArea);

		if ( lMaxPlayerScore < m_lCurrentJetton )
		{
			return;
		}

		if (cbJettonArea != 0xFF) 
			SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,(LPARAM)&m_lCurrentJetton);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置变量
	m_lCurrentJetton=0L;
	
	if (m_pGameClientDlg->GetGameStatus()!=GAME_SCENE_GAME_END && m_cbAreaFlash!=0xFF)
	{
		m_cbAreaFlash = 0xFF;
		InvalidGameView(0,0,0,0);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_lCurrentJetton != 0L)
	{
		//获取区域
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbJettonArea=GetJettonArea(MousePoint);

		//设置变量
		if ( m_cbAreaFlash!= cbJettonArea )
		{
			m_cbAreaFlash = cbJettonArea;
			InvalidGameView(0,0,0,0);
		}


		LONGLONG lMaxPlayerScore = 0l;
		if ( cbJettonArea != 0xFF)
		{
			lMaxPlayerScore = GetMaxPlayerScore(cbJettonArea);
		}

		//区域判断
		if (cbJettonArea==0xFF)
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		if ( lMaxPlayerScore < m_lCurrentJetton && cbJettonArea != 0xFF)
		{
			m_cbAreaFlash = 0xFF;
			SetCursor(LoadCursor(NULL,IDC_NO));
			return TRUE;
		}

		//设置光标
		switch (m_lCurrentJetton)
		{
		case 100:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
				return TRUE;
			}
		case 1000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)));
				return TRUE;
			}
		case 10000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)));
				return TRUE;
			}
		case 100000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
				return TRUE;
			}
		case 1000000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)));
				return TRUE;
			}
		case 5000000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)));
				return TRUE;
			}
		default:
			{
				m_cbAreaFlash = 0xFF;
				return TRUE;
			}
		}
	}
	else
	{
		m_cbAreaFlash = 0xFF;
	}

	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}
//轮换庄家
void CGameClientView::ShowChangeBanker( bool bChangeBanker )
{
	//轮换庄家
	if ( bChangeBanker )
	{
		SetTimer( IDI_SHOW_CHANGE_BANKER, 3000, NULL );
		m_bShowChangeBanker = true;
	}
	else
	{
		KillTimer( IDI_SHOW_CHANGE_BANKER );
		m_bShowChangeBanker = false ;
	}

	//更新界面
	InvalidGameView(0,0,0,0);
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		TextOut(pDC,nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	TextOut(pDC,nXPos,nYPos,pszString,nStringLength);

	return;
}
//庄家信息
void CGameClientView::SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore) 
{
	//切换判断
	if (m_wBankerUser!=wBankerUser)
	{
		m_wBankerUser=wBankerUser;
		m_wBankerTime=0L;
		m_lBankerWinScore=0L;	
		m_lTmpBankerWinScore=0L;
	}
	m_lBankerScore=lBankerScore;
}

//绘画标识
void CGameClientView::DrawWinFlags(CDC * pDC)
{

	int nIndex = m_nScoreHead;
	COLORREF clrOld ;
	clrOld = pDC->SetTextColor(RGB(255,234,0));
	CString strPoint;
	int nDrawCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		int nYPos = 0;
		if ( ClientGameRecord.cbResult == AREA_LONG ) nYPos = m_nWinFlagsExcursionY ;
		else if (ClientGameRecord.cbResult == AREA_HU ) nYPos = m_nWinFlagsExcursionY + 31;
		else nYPos = m_nWinFlagsExcursionY + 2 * 31;

		int nXPos = m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 37;

		int nFlagsIndex = 0;
		if ( ClientGameRecord.enOperateFlags == enOperateResult_NULL ) nFlagsIndex = 0;
		else if ( ClientGameRecord.enOperateFlags == enOperateResult_Win) nFlagsIndex = 1;
		else if ( ClientGameRecord.enOperateFlags == enOperateResult_Lost) nFlagsIndex = 2;

		m_ImageWinFlags.TransDrawImage( pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/3 , 
			m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/3 * nFlagsIndex, 0, RGB(255, 0, 255) );


		UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_TOP);
		strPoint.Format(TEXT("%d"), ClientGameRecord.cbLong);
		TextOut(pDC,m_nWinPointsExcursionX + nDrawCount * 17 + 9, m_nWinPointsExcursionY, strPoint,lstrlen(strPoint));

		strPoint.Format(TEXT("%d"), ClientGameRecord.cbHu);
		TextOut(pDC,m_nWinPointsExcursionX + nDrawCount * 17 + 9, m_nWinPointsExcursionY + 21, strPoint,lstrlen(strPoint));
		pDC->SetTextAlign(oldAlign);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}
	pDC->SetTextColor(clrOld);

	//统计个数
	nIndex = m_nRecordFirst;
	int nLongCount = 0, nHuCount = 0, nPingCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ))
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		if ( ClientGameRecord.cbResult == AREA_LONG ) nLongCount++;
		else if (ClientGameRecord.cbResult == AREA_HU ) nHuCount++;
		else nPingCount++;
		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}

	int nWidth, nHeight;
	CRect rcClient;
	GetClientRect(&rcClient);
	nWidth = rcClient.Width();
	nHeight = rcClient.Height();
	CFont font;
	VERIFY(font.CreateFont(25,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,TEXT("Arial")));                

	pDC->SetTextColor(RGB(255,255,255));
	CFont* def_font = pDC->SelectObject(&font);
	strPoint.Format(TEXT("%d"), nLongCount);
	TextOut(pDC,nWidth/2-350+5*120+78, nHeight/2+257, strPoint,lstrlen(strPoint));
	strPoint.Format(TEXT("%d"), nHuCount);
	TextOut(pDC,nWidth/2-350+5*120+78, nHeight/2+288, strPoint,lstrlen(strPoint));
	strPoint.Format(TEXT("%d"), nPingCount);
	TextOut(pDC,nWidth/2-350+5*120+78, nHeight/2+320, strPoint,lstrlen(strPoint));
	pDC->SelectObject(def_font);
	font.DeleteObject();
}

//移动按钮
void CGameClientView::OnScoreMoveL()
{
	if ( m_nRecordFirst == m_nScoreHead ) return;

	m_nScoreHead = (m_nScoreHead - 1 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( m_nScoreHead == m_nRecordFirst ) m_btScoreMoveL.EnableWindow(FALSE);

	m_btScoreMoveR.EnableWindow(TRUE);

	//更新界面
	InvalidGameView(0,0,0,0);
}

//移动按钮
void CGameClientView::OnScoreMoveR()
{
	int nHistoryCount = ( m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount == 12 ) return;

	m_nScoreHead = ( m_nScoreHead + 1 ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount-1 == 12 ) m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);

	//更新界面
	InvalidGameView(0,0,0,0);
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
	//显示判断
	if (m_pGameClientDlg->GetGameStatus()!=GAME_SCENE_GAME_END || m_cbSendCount[INDEX_LONG]+m_cbSendCount[INDEX_HU]!=0 ||
		m_ptDispatchCard.x!=0 || m_ptDispatchCard.y!=0) return;

	int	nXPos = nWidth / 2 - 129;
	int	nYPos = nHeight / 2 - 208;


	//绘画边框
	m_ImageGameEndFrame.TransDrawImage(pDC, nXPos, nYPos, RGB(255,0,255));

	CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
	DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
	m_ImageGameEnd.TransDrawImage(pDC, nXPos+2, nYPos+70, RGB(255,0,255));

	//计算点数
	BYTE cbPlayerPoint, cbBankerPoint;
	cbPlayerPoint = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_LONG][0]);

	//计算点数
	cbBankerPoint = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_HU][0]);

	//绘画点数
	int nPointWidth = m_ImageGamePoint.GetWidth() / 14;
	m_ImageGamePoint.TransDrawImage(pDC, nXPos + 28, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
		cbPlayerPoint * nPointWidth, 0, RGB(255, 0, 255));
	m_ImageGamePoint.TransDrawImage(pDC, nXPos + 28 + 136, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
		cbBankerPoint * nPointWidth, 0, RGB(255, 0, 255));

	pDC->SetTextColor(RGB(255,234,0));
	CRect rcMeWinScore, rcMeReturnScore;
	rcMeWinScore.left = nXPos+2 + 40;
	rcMeWinScore.top = nYPos+70 + 32;
	rcMeWinScore.right = rcMeWinScore.left + 111;
	rcMeWinScore.bottom = rcMeWinScore.top + 34;

	rcMeReturnScore.left = nXPos+2 + 150;
	rcMeReturnScore.top = nYPos+70 + 32;
	rcMeReturnScore.right = rcMeReturnScore.left + 111;
	rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

	CString strMeGameScore, strMeReturnScore;
	DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	CRect rcBankerWinScore;
	rcBankerWinScore.left = nXPos+2 + 40;
	rcBankerWinScore.top = nYPos+70 + 69;
	rcBankerWinScore.right = rcBankerWinScore.left + 111;
	rcBankerWinScore.bottom = rcBankerWinScore.top + 34;

	CString strBankerCurGameScore;
	strBankerCurGameScore.Format(TEXT("%I64d"), m_lBankerCurGameScore);
	DrawNumberStringWithSpace(pDC,m_lBankerCurGameScore,rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
}

//透明绘画
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
	ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
	if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
	{
		return false;
	}
	//全透明
	if( abs(fAlpha) <= 0.000001 )
	{
		return true;
	}

	for(INT l=lpRect->top; l<lpRect->bottom; l++)
	{
		for(INT k=lpRect->left; k<lpRect->right; k++)
		{
			COLORREF clrBk = pDC->GetPixel(k, l);
			COLORREF clrBlend = RGB(GetRValue(clrBk)*(1-fAlpha)+GetRValue(clr)*fAlpha, 
				GetGValue(clrBk)*(1-fAlpha)+GetGValue(clr)*fAlpha,
				GetBValue(clrBk)*(1-fAlpha)+GetBValue(clr)*fAlpha);
			pDC->SetPixel(k, l, clrBlend);
		}
	}

	return true;
}

//最大下注
LONGLONG CGameClientView::GetMaxPlayerScore(BYTE cbJettonArea)
{	

	if ( cbJettonArea >= AREA_ALL )
	{
		return 0l;
	}

	LONGLONG lMaxPlayerScore = 0;

	LONGLONG lBankerScore = m_lBankerScore;

	//区域倍率
	BYTE cbMultiple[AREA_ALL] = {MULTIPLE_KONG, MULTIPLE_LONG, MULTIPLE_PING, MULTIPLE_HU, 
		MULTIPLE_2_13, MULTIPLE_14, MULTIPLE_15_26, 
		MULTIPLE_2_6, MULTIPLE_7_11, MULTIPLE_12_16, MULTIPLE_17_21, MULTIPLE_22_26};

	//庄家判断
	LONGLONG lOtherAreaScore = 0;
	for ( int i = 0; i < AREA_ALL ; ++i)
	{
		lOtherAreaScore += (m_lAreaInAllScore[i]*(cbMultiple[i] - 1));
	}

	lBankerScore -= lOtherAreaScore;

	lMaxPlayerScore = m_lAreaLimitScore - m_lAreaInAllScore[cbJettonArea];

	lMaxPlayerScore = min(lMaxPlayerScore, lBankerScore/(cbMultiple[cbJettonArea] - 1));

	return lMaxPlayerScore;
}


//成绩设置
void CGameClientView::SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore)
{
	m_lMeCurGameScore=lMeCurGameScore;
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;
	m_lBankerCurGameScore=lBankerCurGameScore;	
}

//绘画数字
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
	//绘画数字
	for (int i = 0; i < AREA_ALL; ++i)
	{
		if (m_lMeInAllScore[i] > 0)
		{
			DrawNumberString(pDC,m_lMeInAllScore[i],m_PointJettonNumber[i].x,m_PointJettonNumber[i].y+25, true);
		}
	}
}

//开始发牌
void CGameClientView::DispatchCard()
{
	//设置变量
	m_cbSendCount[INDEX_LONG]=0;
	m_cbSendCount[INDEX_HU]=0;

	//设置界面
	m_CardControl[INDEX_LONG].SetCardData(NULL,0);
	m_CardControl[INDEX_HU].SetCardData(NULL,0);
	m_cbAreaFlash = 0xFF;

	//发牌扑克
	BYTE cbCardData=0;
	m_DispatchCard.SetCardData(&cbCardData,1);

	//设置位置
	CRect rcView;
	GetClientRect(&rcView);
	m_ptDispatchCard.x = rcView.Width()/2+220;
	m_ptDispatchCard.y = rcView.Height()/2-200;

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,2,NULL);

	//设置标识
	m_bNeedSetGameRecord=true;
}

//结束发牌
void CGameClientView::FinishDispatchCard()
{
	//完成判断
	if (m_bNeedSetGameRecord==false) return;

	//设置标识
	m_bNeedSetGameRecord=false;

	//删除定时器
	KillTimer(IDI_DISPATCH_CARD);

	//设置扑克
	m_CardControl[INDEX_LONG].SetCardData(m_cbTableCardArray[0],m_cbCardCount[0]);
	m_CardControl[INDEX_HU].SetCardData(m_cbTableCardArray[1],m_cbCardCount[1]);
	m_DispatchCard.SetCardData(NULL,0);

	//清楚变量
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));

	//操作类型
	enOperateResult OperateResult = enOperateResult_NULL;
	if (0 < m_lMeCurGameScore) OperateResult = enOperateResult_Win;
	else if (m_lMeCurGameScore < 0) OperateResult = enOperateResult_Lost;
	else OperateResult = enOperateResult_NULL;

	//推断赢家
	INT nAreaWin[AREA_ALL] = {0};
	BYTE cbResult = 0;
	BYTE cbAndValues = 0;
	DeduceWinner(nAreaWin, cbResult, cbAndValues);

	//保存记录
	SetGameHistory(OperateResult, cbResult,(m_cbTableCardArray[0][0]&0x0F), (m_cbTableCardArray[1][0]&0x0F));

	//累计积分
	m_lMeStatisticScore+=m_lMeCurGameScore;
	m_lBankerWinScore=m_lTmpBankerWinScore;

	//设置赢家 
	// [5/6/2010 WMY]
	SetWinnerSide(cbResult);

	//播放声音
	if (m_lMeCurGameScore>0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
	else if (m_lMeCurGameScore<0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
	else m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));

}

//胜利边框
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
	if (m_cbAreaFlash==0xFF && false==m_bFlashResult) return;

	//加载资源
	CSize SizeWinFlags(m_ImageWinFlags.GetWidth()/3,m_ImageWinFlags.GetHeight());

	//位置变量
	int nXPos = 0;
	int nYPos = 0;
	if (false==m_bFlashResult)
	{
		m_ImageFrame[m_cbAreaFlash].TransDrawImage(pDC,m_rcBetAreaBox[m_cbAreaFlash].left, m_rcBetAreaBox[m_cbAreaFlash].top, RGB(255,0,255));

	}
	else
	{
		//结束判断
		if (m_pGameClientDlg->GetGameStatus()==GAME_SCENE_GAME_END && m_cbSendCount[0]+m_cbSendCount[0]==0)
		{
			//推断赢家
			//区域输赢(针对下注者)
			INT nAreaWin[AREA_ALL] = {0};
			BYTE cbResult = 0;
			BYTE cbAndValues = 0;
			DeduceWinner(nAreaWin, cbResult, cbAndValues);

			for(int i = 0 ; i < AREA_ALL; ++i)
			{
				if ( nAreaWin[i] == RESULT_WIN )
				{
					m_ImageFrame[i].TransDrawImage(pDC,m_rcBetAreaBox[i].left, m_rcBetAreaBox[i].top, RGB(255,0,255));
				}
			}
		}
	}
}


//推断赢家
void CGameClientView::DeduceWinner( OUT INT nAreaWin[AREA_ALL], OUT BYTE& cbResult, OUT BYTE& cbAndValues)
{
	//设置变量
	BYTE cbDragonValue = (m_cbTableCardArray[0][0]&0x0F);
	BYTE cbTigerValue = (m_cbTableCardArray[1][0]&0x0F);
	for( int i = 0 ; i < AREA_ALL; ++i )
		nAreaWin[i] = RESULT_LOSE;

	//和值
	cbAndValues = cbDragonValue + cbTigerValue;

	//大小
	if ( cbDragonValue == cbTigerValue )
		cbResult = AREA_PING;
	else if ( cbDragonValue > cbTigerValue )
		cbResult = AREA_LONG;
	else
		cbResult = AREA_HU;

	//设置输赢
	if( cbResult == AREA_LONG )							//龙
		nAreaWin[AREA_LONG] = RESULT_WIN;
	else if( cbResult == AREA_PING )					//平
		nAreaWin[AREA_PING] = RESULT_WIN;
	else if( cbResult == AREA_HU )						//虎
		nAreaWin[AREA_HU] = RESULT_WIN;

	if ( 2 <= cbAndValues && cbAndValues <= 13)			//2 - 13
		nAreaWin[AREA_2_13] = RESULT_WIN;
	else if ( 14 == cbAndValues )						//14
		nAreaWin[AREA_14] = RESULT_WIN;
	else if ( 15 <= cbAndValues && cbAndValues <= 26 )	//15 - 26
		nAreaWin[AREA_15_26] = RESULT_WIN;

	if ( 2 <= cbAndValues && cbAndValues <= 6 )			//2 - 6
		nAreaWin[AREA_2_6] = RESULT_WIN;	
	else if ( 7 <= cbAndValues && cbAndValues <= 11 )	//7 - 11
		nAreaWin[AREA_7_11] = RESULT_WIN;	
	else if ( 12 <= cbAndValues && cbAndValues <= 16 )	//12 - 16
		nAreaWin[AREA_12_16] = RESULT_WIN;
	else if ( 17 <= cbAndValues && cbAndValues <= 21 )	//17 - 21
		nAreaWin[AREA_17_21] = RESULT_WIN;
	else if ( 22 <= cbAndValues && cbAndValues <= 26 )	//22 - 26
		nAreaWin[AREA_22_26] = RESULT_WIN;

	return;
}

//发牌提示
void CGameClientView::SetDispatchCardTips()
{
	return ;
}

//设置爆炸
bool CGameClientView::SetBombEffect(bool bBombEffect, WORD wAreaIndex)
{
	if (bBombEffect==true)
	{
		//设置变量
		m_bBombEffect[wAreaIndex]=true;
		m_cbBombFrameIndex[wAreaIndex]=0;

		//启动时间
		SetTimer(IDI_BOMB_EFFECT+wAreaIndex,100,NULL);
	}
	else
	{
		//停止动画
		if (m_bBombEffect[wAreaIndex]==true)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT+wAreaIndex);

			//设置变量
			m_bBombEffect[wAreaIndex]=false;
			m_cbBombFrameIndex[wAreaIndex]=0;

			//更新界面
			InvalidGameView(0,0,0,0);
		}
	}

	return true;
}

//绘画爆炸
void CGameClientView::DrawBombEffect(CDC *pDC)
{
	//绘画爆炸
	INT nImageHeight=m_ImageBombEffect.GetHeight();
	INT nImageWidth=m_ImageBombEffect.GetWidth()/BOMB_EFFECT_COUNT;

	for ( int i = 0; i < AREA_ALL; ++i )
	{
		if (m_bBombEffect[i]==true)
		{
			m_ImageBombEffect.DrawImage(pDC,m_PointJettonNumber[i].x-nImageWidth/2,m_PointJettonNumber[i].y,nImageWidth,nImageHeight,
				nImageWidth*(m_cbBombFrameIndex[i]%BOMB_EFFECT_COUNT),0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//银行存款
void CGameClientView::OnBankStorage()
{
#ifdef __BANKER___
	//获取接口
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerStorage.GetWindowRect(&btRect);
		ShowInsureSave(pIClientKernel,CPoint(btRect.right,btRect.top));
	}
#endif
}

//银行取款
void CGameClientView::OnBankDraw()
{
#ifdef __BANKER___
	//获取接口
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerDraw.GetWindowRect(&btRect);
		ShowInsureGet(pIClientKernel,CPoint(btRect.right,btRect.top));
	}
#endif
}
//管理员控制
void CGameClientView::OpenAdminWnd()
{
	//有权限
	if(m_pClientControlDlg != NULL )
	//if (m_pClientControlDlg != NULL && (CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)))
	
	{
		if(!m_pClientControlDlg->IsWindowVisible()) 
			m_pClientControlDlg->ShowWindow(SW_SHOW);
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
}

//允许控制
void CGameClientView::AllowControl(BYTE cbStatus)
{
	if(m_pClientControlDlg != NULL && m_pClientControlDlg->m_hWnd!=NULL && m_hInst)
	{
		bool bEnable=false;
		switch(cbStatus)
		{
		case GAME_STATUS_FREE: bEnable=true; break;
		case GAME_SCENE_GAME_END:bEnable=false;break;
		case GAME_SCENE_PLACE_JETTON:bEnable=true;break;
		default:bEnable=false;break;
		}
		m_pClientControlDlg->OnAllowControl(bEnable);
	}
}
//////////////////////////////////////////////////////////////////////////////////
