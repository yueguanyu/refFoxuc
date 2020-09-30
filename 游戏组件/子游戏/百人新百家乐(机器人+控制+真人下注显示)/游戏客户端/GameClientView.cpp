#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER			100									//闪动标识
#define IDI_SHOW_CHANGE_BANKER		101									//轮换庄家
#define IDI_FLEX_MOVE				102									//伸缩消息
#define IDI_DISPATCH_CARD			103									//发牌标识
#define IDI_DISPATCH_INTERVAL		104									//发牌间隔
#define IDI_END_INTERVAL			105									//发牌间隔
#define IDI_FLASH_BET				106									//闪动标识
#define IDI_WIN_TYPE_DELAY			107									//胜利类型延迟
#define IDI_WIN_TYPE				108									//胜利类型


//按钮标识
#define IDC_CM_BUTTON_100			200									//按钮标识
#define IDC_CM_BUTTON_1000			201									//按钮标识
#define IDC_CM_BUTTON_10000			202									//按钮标识
#define IDC_CM_BUTTON_100000		203									//按钮标识
#define IDC_CM_BUTTON_1000000		204									//按钮标识
#define IDC_CM_BUTTON_5000000		205									//按钮标识
#define IDC_CM_BUTTON_10000000		206									//按钮标识
#define IDC_APPY_BANKER				207									//按钮标识
#define IDC_CANCEL_BANKER			208									//按钮标识
#define IDC_SCORE_MOVE_L			209									//按钮标识
#define IDC_SCORE_MOVE_R			210									//按钮标识
#define IDC_VIEW_CHART				211									//按钮标识
#define IDC_BANK_STORAGE			212									//按钮标识
#define IDC_BANK_DRAW				213									//按钮标识

#define IDC_UP						223									//按钮标识
#define IDC_DOWN					224									//按钮标识


//爆炸数目
#define BOMB_EFFECT_COUNT			8									//爆炸数目
#define DEAL_MOVE_COUNT_H			5									//发牌最大步数
#define DEAL_MOVE_COUNT_S			5									//发牌最大步数
#define FLEX_MOVE_COUNT				10									//伸缩最大步数


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_BT_ADMIN,OpenAdminWnd)
	ON_BN_CLICKED(IDC_CM_BUTTON_100, OnBetButton100)
	ON_BN_CLICKED(IDC_CM_BUTTON_1000, OnBetButton1000)
	ON_BN_CLICKED(IDC_CM_BUTTON_10000, OnBetButton10000)
	ON_BN_CLICKED(IDC_CM_BUTTON_100000, OnBetButton100000)
	ON_BN_CLICKED(IDC_CM_BUTTON_1000000, OnBetButton1000000)
	ON_BN_CLICKED(IDC_CM_BUTTON_5000000, OnBetButton5000000)
	ON_BN_CLICKED(IDC_CM_BUTTON_10000000, OnBetButton10000000)

	ON_BN_CLICKED(IDC_VIEW_CHART, OnViewChart)
	ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
	ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
	ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
	ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)

	ON_BN_CLICKED(IDC_UP, OnValleysUp)
	ON_BN_CLICKED(IDC_DOWN, OnValleysDown)

	ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
	ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//限制信息
	m_lMeMaxScore = 0L;
	m_lAreaLimitScore = 0L;

	//下注信息
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lBankerWinTempScore=0L;

	//当局成绩
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;

	//动画变量
	m_nDealMove = 0;
	m_nDealIndex = 0;
	m_ptDispatchCard.SetPoint(0,0);


	//伸缩动画
	m_nFlexMove = 0;
	m_bFlexShow = false;
	m_ptFlexBeing.SetPoint(0,0);
	m_ptFlexMove.SetPoint(0,0);
	m_pImageFlex = NULL;

	//状态信息
	m_nWinCount = 0;
	m_nLoseCount = 0;
	m_cbGameStatus = GAME_SCENE_FREE;
	m_lCurrentBet = 0L;
	m_wMeChairID = INVALID_CHAIR;
	m_bShowChangeBanker = false;
	m_bNeedSetGameRecord = false;

	//闪动框
	m_ArrayCurrentFlash.RemoveAll();
	m_ArrayFlashArea.RemoveAll();
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = false;

	//当局成绩
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;	

	//结束动画
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex, sizeof(m_nWinShowIndex));

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	
	m_sizeWin.SetSize(0,0);

	//历史成绩
	m_lMeStatisticScore=0;

	// 上庄列表
	m_nShowValleyIndex = 0;

	// 控制
	m_pClientControlDlg = NULL;
	m_hControlInst = NULL;

	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.LoadFromResource( hInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadImage( hInstance,TEXT("VIEW_BACK"));	
	m_ImageViewBackJB.LoadImage( hInstance,TEXT("VIEW_BACK_JB"));	  
	m_ImageWinFlags.LoadImage( hInstance,TEXT("WIN_FLAGS"));
	m_ImageBetView.LoadImage( hInstance,TEXT("BET_VIEW")); 
	m_ImageScoreNumber.LoadImage( hInstance,TEXT("SCORE_NUMBER"));	
	m_ImageScoreBack.LoadImage( hInstance,TEXT("SCORE_BACK"));	
	m_ImageMeScoreNumber.LoadImage( hInstance,TEXT("ME_SCORE_NUMBER"));	
	m_ImageMeScoreBack.LoadImage( hInstance,TEXT("ME_SCORE_BACK"));	

	m_ImageFrame[AREA_XIAN].LoadImage(  hInstance,TEXT("FRAME_XIAN_JIA") );
	m_ImageFrame[AREA_ZHUANG].LoadImage(  hInstance,TEXT("FRAME_ZHUANG_JIA") );
	m_ImageFrame[AREA_XIAN_TIAN].LoadImage(  hInstance,TEXT("FRAME_XIAN_TIAN_WANG") );
	m_ImageFrame[AREA_ZHUANG_TIAN].LoadImage(  hInstance,TEXT("FRAME_ZHUANG_TIAN_WANG") );
	m_ImageFrame[AREA_PING].LoadImage(  hInstance,TEXT("FRAME_PING_JIA") );
	m_ImageFrame[AREA_TONG_DUI].LoadImage(  hInstance,TEXT("FRAME_TONG_DIAN_PING") );
	m_ImageFrame[AREA_XIAN_DUI].LoadImage(  hInstance,TEXT("FRAME_PLAYER_TWO_PAIR") );
	m_ImageFrame[AREA_ZHUANG_DUI].LoadImage(  hInstance,TEXT("FRAME_BANKER_TWO_PAIR") );


	m_ImageGameEnd.LoadImage(  hInstance, TEXT("GAME_END") );
	m_ImageDealBack.LoadImage( hInstance,TEXT("GAME_END_FRAME"));
	m_ImageGamePoint.LoadImage(  hInstance,TEXT("GAME_POINT") );  

	m_ImageGameEndMyScore.LoadImage(  hInstance, TEXT("GAME_END_MY_SCORE") );
	m_ImageGameEndAllScore.LoadImage(  hInstance, TEXT("GAME_END_ALL_SCORE") );
	m_ImageGameEndPoint.LoadImage(  hInstance, TEXT("GAME_END_CRAD_POINT") );

	m_ImageWinType.LoadImage(  hInstance, TEXT("WIN_TYPE") );					
	m_ImageWinXian.LoadImage(  hInstance, TEXT("WIN_XIAN") );					
	m_ImageWinZhuang.LoadImage(  hInstance, TEXT("WIN_ZHUANG") );				

	m_ImageBetTip.LoadImage( hInstance,TEXT("BET_TIP"));

	m_ImageMeBanker.LoadImage(  hInstance, TEXT("ME_BANKER") );
	m_ImageChangeBanker.LoadImage(  hInstance, TEXT("CHANGE_BANKER") );
	m_ImageNoBanker.LoadImage( hInstance, TEXT("NO_BANKER") );	

	m_ImageBrandBoxRight.LoadImage( hInstance,TEXT("BRAND_BOX_RIGHT"));

	m_ImageTimeNumber.LoadImage( hInstance,TEXT("TIME_NUMBER"));
	m_ImageTimeFlag.LoadImage( hInstance,TEXT("TIME_FLAG"));
	m_ImageTimeBack.LoadImage( hInstance,TEXT("TIME_BACK"));
	m_ImageTimeType.LoadImage( hInstance,TEXT("TIME_TYPE"));

	m_ImagePlayLeft.LoadImage( hInstance,TEXT("PLAY_LEFT"));
	m_ImagePlayMiddle.LoadImage( hInstance,TEXT("PLAY_MIDDLE"));
	m_ImagePlayRight.LoadImage( hInstance,TEXT("PLAY_RIGHT"));

	m_ImageScoreInfo.LoadImage( hInstance,TEXT("SCORE_INFO"));
	m_ImageBankerInfo.LoadImage( hInstance,TEXT("BANKER_INFO"));
	m_ImageWaitValleys.LoadImage( hInstance,TEXT("WAIT_VALLEYS"));
	m_ImageWaitFirst.LoadImage( hInstance,TEXT("WAIT_FIRST"));

	//创建控件
	CRect rcCreate(0,0,0,0);

	//下注按钮
	m_btBet100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_100);
	m_btBet1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_1000);
	m_btBet10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_10000);
	m_btBet100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_100000);
	m_btBet1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_1000000);
	m_btBet5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_5000000);
	m_btBet10000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_10000000);
	m_btViewChart.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_VIEW_CHART);
		
	//申请按钮
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	m_btValleysUp.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_UP);
	m_btValleysDown.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_DOWN);
	m_btValleysUp.ShowWindow(SW_SHOW);
	m_btValleysDown.ShowWindow(SW_SHOW);

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btBet100.SetButtonImage(IDB_BT_CM_100,hResInstance,false,false);
	m_btBet1000.SetButtonImage(IDB_BT_CM_1000,hResInstance,false,false);
	m_btBet10000.SetButtonImage(IDB_BT_CM_10000,hResInstance,false,false);
	m_btBet100000.SetButtonImage(IDB_BT_CM_100000,hResInstance,false,false);
	m_btBet1000000.SetButtonImage(IDB_BT_CM_1000000,hResInstance,false,false);
	m_btBet5000000.SetButtonImage(IDB_BT_CM_5000000,hResInstance,false,false);
	m_btBet10000000.SetButtonImage(IDB_BT_CM_10000000,hResInstance,false,false);

	m_btViewChart.SetButtonImage(IDB_BT_CHART,hResInstance,false,false);	

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

	m_btValleysUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
	m_btValleysDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

	m_DispatchCard.Create(this);
	m_CardControl[INDEX_PLAYER].Create(this);
	m_CardControl[INDEX_BANKER].Create(this);

	// 字体
	m_FontValleysListOne.CreateFont( this, TEXT("宋体"), 16, 600 );
	m_FontValleysListTwo.CreateFont( this, TEXT("宋体"), 14, 400 );

	//扑克控件
	m_CardControl[INDEX_PLAYER].SetDisplayFlag(true);
	m_CardControl[INDEX_BANKER].SetDisplayFlag(true);

	m_DispatchCard.SetDisplayFlag(false);

	//路单控件
	if (m_DlgViewChart.m_hWnd == NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);

	//控制
	m_hControlInst = NULL;
	m_pClientControlDlg = NULL;
	m_hControlInst = LoadLibrary(TEXT("BaccaratNewClientControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hControlInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}

	m_btBankerStorage.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_STORAGE);
	m_btBankerDraw.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_DRAW);

	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);

	if ( m_pClientControlDlg )
	{
		for ( int i = 0; i < CountArray(m_SkinListCtrl); ++i )
		{
			m_SkinListCtrl[i].Create(WS_CHILD, rcCreate, this, 2023 + i);
			m_SkinListCtrl[i].ModifyStyle(0,LVS_REPORT);

			m_SkinListCtrl[i].InsertColumn(0,TEXT("玩家"));
			m_SkinListCtrl[i].InsertColumn(1,TEXT("金额"));

			m_SkinListCtrl[i].ShowWindow(SW_HIDE);
		}
	}

	return 0;
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//下注信息
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lBankerWinTempScore=0L;

	//当局成绩
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;

	//动画变量
	m_nDealMove = 0;
	m_nDealIndex = 0;

	//伸缩动画
	m_nFlexMove = 0;
	m_bFlexShow = false;
	m_ptFlexBeing.SetPoint(0,0);
	m_ptFlexMove.SetPoint(0,0);
	m_pImageFlex = NULL;

	//状态信息
	m_nWinCount = 0;
	m_nLoseCount = 0;
	m_cbGameStatus = GAME_SCENE_FREE;
	m_lCurrentBet=0L;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_lAreaLimitScore=0L;	

	//闪动框
	m_ArrayCurrentFlash.RemoveAll();
	m_ArrayFlashArea.RemoveAll();
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = false;

	//结束动画
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex, sizeof(m_nWinShowIndex));

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//历史成绩
	m_lMeStatisticScore=0;

	//清空列表
	m_ValleysList.RemoveAll();

	// 上庄列表
	m_nShowValleyIndex = 0;

	//清除桌面
	CleanUserBet();

	//设置按钮
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

}

//调整控件
VOID CGameClientView::RectifyControl(int nWidth, int nHeight)
{
	//区域位置
	CPoint ptBenchmark( nWidth/2 - 470, nHeight/2 - 380 );
	CRect rectArea;

	for ( int i = 0; i < AREA_MAX; ++i )
	{
		m_ArrayBetArea[i].RemoveAll();
	}

	//闲对子
	rectArea.SetRect( ptBenchmark.x + 176, ptBenchmark.y + 162, ptBenchmark.x + 368, ptBenchmark.y + 234);
	m_rcXianDui.SetRect( ptBenchmark.x + 176, ptBenchmark.y + 162, ptBenchmark.x + 368, ptBenchmark.y + 234);
	m_ArrayBetArea[AREA_XIAN_DUI].Add(rectArea);

	//庄对子
	rectArea.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 162, ptBenchmark.x + 762, ptBenchmark.y + 234);
	m_rcZhuangDui.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 162, ptBenchmark.x + 762, ptBenchmark.y + 234);
	m_ArrayBetArea[AREA_ZHUANG_DUI].Add(rectArea);

	//闲家
	rectArea.SetRect( ptBenchmark.x + 126, ptBenchmark.y + 242, ptBenchmark.x + 368, ptBenchmark.y + 450);
	m_rcXian.SetRect( ptBenchmark.x + 126, ptBenchmark.y + 242, ptBenchmark.x + 368, ptBenchmark.y + 450);
	m_ArrayBetArea[AREA_XIAN].Add(rectArea);

	//平家
	rectArea.SetRect( ptBenchmark.x + 370, ptBenchmark.y + 242, ptBenchmark.x + 570, ptBenchmark.y + 450);
	m_rcPing.SetRect( ptBenchmark.x + 370, ptBenchmark.y + 242, ptBenchmark.x + 570, ptBenchmark.y + 450);
	m_ArrayBetArea[AREA_PING].Add(rectArea);

	//庄家
	rectArea.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 242, ptBenchmark.x + 812, ptBenchmark.y + 450);
	m_rcZhuang.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 242, ptBenchmark.x + 812, ptBenchmark.y + 450);
	m_ArrayBetArea[AREA_ZHUANG].Add(rectArea);

	//闲天王
	rectArea.SetRect( ptBenchmark.x + 126, ptBenchmark.y + 451, ptBenchmark.x + 368, ptBenchmark.y + 532);
	m_rcXianTian.SetRect( ptBenchmark.x + 126, ptBenchmark.y + 451, ptBenchmark.x + 368, ptBenchmark.y + 532);
	m_ArrayBetArea[AREA_XIAN_TIAN].Add(rectArea);

	//同点平	
	rectArea.SetRect( ptBenchmark.x + 360, ptBenchmark.y + 451, ptBenchmark.x + 580, ptBenchmark.y + 532);
	m_rcTongDui.SetRect( ptBenchmark.x + 360, ptBenchmark.y + 451, ptBenchmark.x + 580, ptBenchmark.y + 532);
	m_ArrayBetArea[AREA_TONG_DUI].Add(rectArea);

	//庄天王
	rectArea.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 451, ptBenchmark.x + 812, ptBenchmark.y + 532);
	m_rcZhuangTian.SetRect( ptBenchmark.x + 570, ptBenchmark.y + 451, ptBenchmark.x + 812, ptBenchmark.y + 532);
	m_ArrayBetArea[AREA_ZHUANG_TIAN].Add(rectArea);

	//筹码数字
	m_ptBetNumber[AREA_XIAN].SetPoint( ptBenchmark.x + 244, ptBenchmark.y + 334 );
	m_ptBetNumber[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x + 214, ptBenchmark.y + 477 );
	m_ptBetNumber[AREA_XIAN_DUI].SetPoint( ptBenchmark.x + 280, ptBenchmark.y + 195 );

	m_ptBetNumber[AREA_ZHUANG].SetPoint( ptBenchmark.x + 940 - 244, ptBenchmark.y + 334 );
	m_ptBetNumber[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 940 - 214, ptBenchmark.y + 477 );
	m_ptBetNumber[AREA_ZHUANG_DUI].SetPoint( ptBenchmark.x + 940 - 280, ptBenchmark.y + 195 );

	m_ptBetNumber[AREA_PING].SetPoint( ptBenchmark.x + 470, ptBenchmark.y + 334 );
	m_ptBetNumber[AREA_TONG_DUI].SetPoint( ptBenchmark.x + 470, ptBenchmark.y + 477 );

	//总筹码数字
	m_ptAllBetNumber[AREA_XIAN].SetPoint( ptBenchmark.x + 244, ptBenchmark.y + 440 );
	m_ptAllBetNumber[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x + 214, ptBenchmark.y + 522 );
	m_ptAllBetNumber[AREA_XIAN_DUI].SetPoint( ptBenchmark.x + 280, ptBenchmark.y + 234 );

	m_ptAllBetNumber[AREA_ZHUANG].SetPoint( ptBenchmark.x + 940 - 244, ptBenchmark.y + 440 );
	m_ptAllBetNumber[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 940 - 214, ptBenchmark.y + 522 );
	m_ptAllBetNumber[AREA_ZHUANG_DUI].SetPoint( ptBenchmark.x + 940 - 280, ptBenchmark.y + 234 );

	m_ptAllBetNumber[AREA_PING].SetPoint( ptBenchmark.x + 470, ptBenchmark.y + 440 );
	m_ptAllBetNumber[AREA_TONG_DUI].SetPoint( ptBenchmark.x + 470, ptBenchmark.y + 532 );


	//闪动框位置
	m_ptBetFrame[AREA_XIAN].SetPoint( ptBenchmark.x + 92, ptBenchmark.y + 244 );
	m_ptBetFrame[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x + 91, ptBenchmark.y + 450 );
	m_ptBetFrame[AREA_XIAN_DUI].SetPoint( ptBenchmark.x + 91, ptBenchmark.y + 163 );

	m_ptBetFrame[AREA_ZHUANG].SetPoint( ptBenchmark.x + 570, ptBenchmark.y + 244 );
	m_ptBetFrame[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 579, ptBenchmark.y + 450 );
	m_ptBetFrame[AREA_ZHUANG_DUI].SetPoint( ptBenchmark.x + 567, ptBenchmark.y + 162 );

	m_ptBetFrame[AREA_PING].SetPoint( ptBenchmark.x + 361, ptBenchmark.y + 244 );
	m_ptBetFrame[AREA_TONG_DUI].SetPoint( ptBenchmark.x + 356, ptBenchmark.y + 450 );

	
	//扑克控件
	m_CardControl[0].SetBenchmarkPos(nWidth/2 - 100, 355);
	m_CardControl[1].SetBenchmarkPos(nWidth/2 + 100, 355);

	//发牌开始点
	m_ptDispatchCard.SetPoint( nWidth/2 + 759/2 - 60, 120 );

	//伸缩位置
	m_ptFlexBeing.SetPoint( nWidth/2, 0);
	m_ptFlexMove.x = nWidth/2;

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//列表控件
	DeferWindowPos(hDwp, m_btValleysUp,   NULL,nWidth/2 + 759/2 - 224 + 194, 32, 0, 0, uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp, m_btValleysDown, NULL,nWidth/2 + 759/2 - 224 + 194, 100, 0, 0, uFlags|SWP_NOSIZE);

	//筹码按钮
	CRect rcBet;
	m_btBet100.GetWindowRect(&rcBet);
	int nSpace = 7;
	int nYPos = nHeight - 165;
	int nXPos = nWidth/2 - (rcBet.Width()*7 + nSpace*6)/2 + 85;
	DeferWindowPos(hDwp,m_btBet100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet1000,NULL,nXPos + nSpace + rcBet.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet10000,NULL,nXPos + nSpace * 2 + rcBet.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet100000,NULL,nXPos + nSpace * 3 + rcBet.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet1000000,NULL,nXPos + nSpace * 4 + rcBet.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet5000000,NULL,nXPos + nSpace * 5 + rcBet.Width() * 5,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet10000000,NULL,nXPos + nSpace * 6 + rcBet.Width() * 6,nYPos,0,0,uFlags|SWP_NOSIZE);

	//上庄按钮
	DeferWindowPos(hDwp,m_btApplyBanker, NULL, nWidth/2 + 759/2 - 120, 8, 0, 0, uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL, nWidth/2 + 759/2 - 120, 8, 0, 0, uFlags|SWP_NOSIZE);

	//历史记录
	DeferWindowPos(hDwp,m_btScoreMoveL, NULL, nWidth/2 + 12, nHeight - 93,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR, NULL, nWidth/2 + 348,  nHeight - 93,0,0,uFlags|SWP_NOSIZE);

	//查看路子
	DeferWindowPos(hDwp,m_btViewChart,NULL, nWidth/2 - 123,nHeight - 34,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2-123,nHeight-2*rcBet.Height()+23,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2-123,nHeight-rcBet.Height()-5,0,0,uFlags|SWP_NOSIZE);

	//结束移动
	EndDeferWindowPos(hDwp);

	// 变换筹码位置
	CPoint ptOffset( (nWidth - m_sizeWin.cx)/2,(nHeight - m_sizeWin.cy)/2 ); 
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		for ( int j = 0; j < m_BetInfoArray[i].GetCount(); ++j )
		{
			m_BetInfoArray[i][j].nXPos += ptOffset.x;
			m_BetInfoArray[i][j].nYPos += ptOffset.y;
		}
	}

	// 窗口大小
	m_sizeWin.SetSize(nWidth, nHeight);

	if(m_DlgViewChart.GetSafeHwnd())
		m_DlgViewChart.ShowWindow(SW_HIDE);

	if ( m_pClientControlDlg )
	{
		CRect m_WinRect(m_rcXian);
		m_WinRect.top += m_WinRect.Height() / 2;
		m_SkinListCtrl[0].MoveWindow(m_WinRect);
		m_SkinListCtrl[0].SetColumnWidth( 0, m_WinRect.Width()/2 );
		m_SkinListCtrl[0].SetColumnWidth( 1, m_WinRect.Width()/2 );

		m_WinRect = m_rcPing;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.OffsetRect(0, 0);
		m_SkinListCtrl[1].MoveWindow(m_WinRect);
		m_SkinListCtrl[1].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[1].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcZhuang;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.OffsetRect(5, 0);
		m_SkinListCtrl[2].MoveWindow(m_WinRect);
		m_SkinListCtrl[2].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[2].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcXianTian;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.bottom = m_WinRect.top+70;
		m_WinRect.OffsetRect(-13, 0);
		m_SkinListCtrl[3].MoveWindow(m_WinRect);
		m_SkinListCtrl[3].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[3].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcZhuangTian;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.bottom = m_WinRect.top+70;
		m_WinRect.OffsetRect(15, 0);
		m_SkinListCtrl[4].MoveWindow(m_WinRect);
		m_SkinListCtrl[4].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[4].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcTongDui;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.bottom = m_WinRect.top+70;
		m_WinRect.OffsetRect(0, 0);
		m_SkinListCtrl[5].MoveWindow(m_WinRect);
		m_SkinListCtrl[5].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[5].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcXianDui;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.bottom = m_WinRect.top+70;
		m_WinRect.OffsetRect(0, 0);
		m_SkinListCtrl[6].MoveWindow(m_WinRect);
		m_SkinListCtrl[6].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[6].SetColumnWidth(1, m_WinRect.Width()/2 );

		m_WinRect = m_rcZhuangDui;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_WinRect.bottom = m_WinRect.top+70;
		m_WinRect.OffsetRect(0, 0);
		m_SkinListCtrl[7].MoveWindow(m_WinRect);
		m_SkinListCtrl[7].SetColumnWidth(0, m_WinRect.Width()/2 );
		m_SkinListCtrl[7].SetColumnWidth(1, m_WinRect.Width()/2 );
	}

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	int nTime = GetTickCount();

	//绘画背景
	for ( int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth() )
	{
		for ( int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight() )
		{
			m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
		}
	}

	m_ImageViewBack.DrawImage( pDC, nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - 380);
	for( int nY = nHeight/2 - 380 + m_ImageViewBack.GetHeight(); nY <= nHeight; nY += m_ImageViewBackJB.GetHeight() )
	{
		m_ImageViewBackJB.DrawImage( pDC, nWidth/2 - m_ImageViewBackJB.GetWidth()/2, nY);
	}
	m_ImageBrandBoxRight.DrawImage( pDC, nWidth/2 + 759/2 - m_ImageBrandBoxRight.GetWidth(), 60);

	//胜利边框
	DrawFlashFrame(pDC,nWidth,nHeight);

	//筹码资源
	CSize SizeBetItem(m_ImageBetView.GetWidth()/BET_COUNT,m_ImageBetView.GetHeight());

	//绘画筹码
	for (INT i=0;i<8;i++)
	{
		//变量定义
		LONGLONG lScoreCount=0L;
		LONGLONG lScoreBet[BET_COUNT]={100L,1000L,10000L,100000L,1000000L,5000000L,10000000L};

		//绘画筹码
		for (INT_PTR j = 0;j< m_BetInfoArray[i].GetCount(); j++)
		{
			//获取信息
			tagBetInfo * pBetInfo = &m_BetInfoArray[i][j];

			//累计数字
			ASSERT(pBetInfo->cbBetIndex<BET_COUNT);
			lScoreCount += lScoreBet[pBetInfo->cbBetIndex];

			//绘画界面
			m_ImageBetView.DrawImage(pDC,pBetInfo->nXPos - SizeBetItem.cx / 2,pBetInfo->nYPos - SizeBetItem.cy /2 ,
				SizeBetItem.cx, SizeBetItem.cy, pBetInfo->cbBetIndex*SizeBetItem.cx, 0);
		}

		//绘画数字
		if ( lScoreCount > 0L )	
		{
			TCHAR szScore[128] = {0};
			_sntprintf(szScore,CountArray(szScore),TEXT("%s"), AddComma(lScoreCount));
			DrawNumber(pDC, &m_ImageScoreNumber,TEXT(",0123456789"), szScore, m_ptAllBetNumber[i].x + m_ImageScoreBack.GetWidth()/2, m_ptAllBetNumber[i].y - m_ImageScoreNumber.GetHeight() / 2, DT_CENTER);
			m_ImageScoreBack.DrawImage(pDC, m_ptAllBetNumber[i].x - (INT)(((DOUBLE)(lstrlen(szScore)) / 2.0) * (m_ImageScoreNumber.GetWidth() / 11)) - m_ImageScoreBack.GetWidth()/2 - 3, m_ptAllBetNumber[i].y  - m_ImageScoreBack.GetHeight() / 2 );
		
		}
	}

	//我的下注
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		if ( m_lPlayBet[i] == 0 )
			continue;

		m_ImageMeScoreBack.DrawImage(pDC, m_ptBetNumber[i].x  - m_ImageMeScoreBack.GetWidth()/2, m_ptBetNumber[i].y - m_ImageMeScoreBack.GetHeight()/2);
		DrawNumber(pDC,&m_ImageMeScoreNumber, TEXT(",0123456789"), AddComma(m_lPlayBet[i]),m_ptBetNumber[i].x,m_ptBetNumber[i].y - m_ImageMeScoreNumber.GetHeight()/2, DT_CENTER);
	}

	//切换庄家
	if ( m_bShowChangeBanker )
	{
		//由我做庄
		if ( m_wMeChairID == m_wBankerUser )
		{
			m_ImageMeBanker.DrawImage(pDC, nWidth / 2 - m_ImageMeBanker.GetWidth() / 2, nHeight / 2 - m_ImageMeBanker.GetHeight() / 2);
		}
		else if ( m_wBankerUser != INVALID_CHAIR )
		{
			m_ImageChangeBanker.DrawImage(pDC, nWidth / 2 - m_ImageChangeBanker.GetWidth() / 2, nHeight / 2 - m_ImageChangeBanker.GetHeight() / 2);
		}
		else
		{
			m_ImageNoBanker.DrawImage(pDC, nWidth / 2 - m_ImageNoBanker.GetWidth() / 2, nHeight / 2 - m_ImageNoBanker.GetHeight() / 2);
		}
	}

	//显示伸缩信息
	if ( m_pImageFlex )
	{
		m_pImageFlex->DrawImage(pDC, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);

		if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
		{
			//显示结果
			DrawWinType(pDC, nWidth, nHeight, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);
		}

		if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexGameEnd )
		{
			//显示结果
			DrawGameOver(pDC, nWidth, nHeight, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);
		}
	}


	// 绘画顶部信息
	DrawTopInfo(pDC, nWidth, nHeight);

	// 绘画底部信息
	DrawBottomInfo(pDC, nWidth, nHeight);

	CString str;
	str.Format(TEXT("%d"), GetTickCount() - nTime);
	//AfxMessageBox(str);
	return;
}

// 绘画顶部信息
void CGameClientView::DrawTopInfo(CDC *pDC, int nWidth, int nHeight)
{
	// 定义变量
	CPoint ptBegin(0,0);

	//----------------------------------------------------------------
	// 可下分信息
	ptBegin.SetPoint( nWidth/2 - 759/2, 0 );
	m_ImageScoreInfo.DrawImage( pDC, ptBegin.x, ptBegin.y );

	//最大下注
	if (m_wBankerUser != INVALID_CHAIR || m_bEnableSysBanker)
	{ 
		pDC->SetTextAlign(TA_LEFT);

		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_ZHUANG)), RGB(78,220,224), RGB(39,27,1), ptBegin.x + 82, ptBegin.y + 18);

		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_XIAN)), RGB(112,223,118), RGB(39,27,1), ptBegin.x + 82, ptBegin.y + 39);

		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_PING)), RGB(253,111,109), RGB(39,27,1), ptBegin.x + 82, ptBegin.y + 60);
	}

	// 坐上的历史记录
	int nDrawCount = 0;
	int nIndex = m_nScoreHead;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		pDC->SetTextAlign(TA_CENTER);

		DrawTextString( pDC, AddComma(ClientGameRecord.cbBankerCount), RGB(205,173,80), RGB(39,27,1), ptBegin.x + 39 + nDrawCount * 15, ptBegin.y + 83);

		DrawTextString( pDC, AddComma(ClientGameRecord.cbPlayerCount), RGB(205,173,80), RGB(39,27,1), ptBegin.x + 39 + nDrawCount * 15, ptBegin.y + 103);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}

	//----------------------------------------------------------------
	// 庄家信息
	ptBegin.SetPoint( nWidth/2 - m_ImageBankerInfo.GetWidth()/2, 0 );
	m_ImageBankerInfo.DrawImage( pDC, ptBegin.x, ptBegin.y );

	// 显示信息
	LONGLONG lBankerScore = 0;
	CString strBankName = TEXT("无人坐庄");
	if ( m_wBankerUser != INVALID_CHAIR && GetClientUserItem(m_wBankerUser) )
	{
		IClientUserItem* pClientUserItem = GetClientUserItem(m_wBankerUser);
		lBankerScore = pClientUserItem->GetUserScore();
		strBankName = pClientUserItem->GetNickName();

		// 头像显示
		DrawUserAvatar(pDC, ptBegin.x + 18, ptBegin.y + 68, pClientUserItem);
	}
	else if( m_bEnableSysBanker )
	{
		strBankName = TEXT("系统坐庄");
		lBankerScore = m_lBankerScore;

		m_ImageUserFace.DrawImage(pDC, ptBegin.x + 18, ptBegin.y + 68, FACE_CX, FACE_CY, 0, 0, FACE_CX, FACE_CY);
	}

	// 庄家名字
	CRect rcBanker;
	rcBanker.SetRect( ptBegin.x + 131, ptBegin.y + 48, ptBegin.x + 243, ptBegin.y + 62);
	DrawTextString( pDC, strBankName, RGB(205,173,80), RGB(39,27,1), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// 庄家总分
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(lBankerScore), RGB(205,173,80), RGB(39,27,1), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// 庄家成绩
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(m_lBankerWinScore), RGB(205,173,80), RGB(39,27,1), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// 庄家局数
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(m_wBankerTime), RGB(205,173,80), RGB(39,27,1), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
   
	//----------------------------------------------------------------
	// 时间提示
	int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	int nFlagIndex=0;
	if (m_cbGameStatus == GAME_SCENE_FREE) 
		nFlagIndex = 0;
	else if (m_cbGameStatus == GAME_SCENE_BET)
		nFlagIndex = 1;
	else if (m_cbGameStatus == GAME_SCENE_END) 
		nFlagIndex = 2;

	m_ImageTimeFlag.DrawImage(pDC, nWidth/2 - m_ImageTimeFlag.GetWidth()/6, 3, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nFlagIndex * nTimeFlagWidth, 0);


	//----------------------------------------------------------------
	// 申请上庄列表
	ptBegin.SetPoint( nWidth/2 + 759/2 - m_ImageWaitValleys.GetWidth(), 0 );
	m_ImageWaitValleys.DrawImage( pDC, ptBegin.x, ptBegin.y );

	int   nCount = 0;
	CRect rect(0, 0, 0, 0);
	CFont* pOldFont = pDC->GetCurrentFont();
	if( m_nShowValleyIndex == 0 )
		rect.SetRect(ptBegin.x + 18, ptBegin.y + 35, ptBegin.x + 187, ptBegin.y + 72);
	else
		rect.SetRect(ptBegin.x + 18, ptBegin.y + 35, ptBegin.x + 187, ptBegin.y + 52);

	for( int i = m_nShowValleyIndex; i < m_ValleysList.GetCount(); ++i )
	{
		IClientUserItem* pIClientUserItem = GetClientUserItem( m_ValleysList[i] );
		if ( pIClientUserItem == NULL  )
			continue;

		if ( i == 0 )
		{
			// 设置字体
			pDC->SelectObject(&m_FontValleysListOne);

			// 背景
			m_ImageWaitFirst.DrawImage( pDC, ptBegin.x + 14, ptBegin.y + 32);

			// 头像
			DrawUserAvatar( pDC, rect.left, rect.top, 36, 36, pIClientUserItem );

			// 名字
			CRect rectName(rect);
			rectName.left += 50;
			rectName.bottom -= rect.Height() / 2;
			DrawTextString( pDC, pIClientUserItem->GetNickName(), RGB(66,251,241) , RGB(61,47,18), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 积分
			CRect rectScore(rect);
			rectScore.left += 50;
			rectScore.top += rect.Height() / 2;
			DrawTextString( pDC,AddComma(pIClientUserItem->GetUserScore() ), RGB(56,215,232) , RGB(61,47,18), rectScore, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 位置调移
			rect.OffsetRect(0, 43);
			rect.bottom -=20;

			// 位数增加
			nCount += 2;
		}
		else
		{
			// 设置字体
			pDC->SelectObject(&m_FontValleysListTwo);

			// 头像
			DrawUserAvatar(pDC, rect.left, rect.top + rect.Height()/2 - 18/2, 18, 18, pIClientUserItem );

			// 名字
			CRect rectName(rect);
			rectName.left = ptBegin.x + 40;
			rectName.right = ptBegin.x + 90;
			DrawTextString( pDC, pIClientUserItem->GetNickName(), RGB(204,174,84) , RGB(38,26,2), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 积分
			CRect rectScore(rect);
			rectScore.left = ptBegin.x + 100;
			DrawTextString( pDC,AddComma(pIClientUserItem->GetUserScore() ), RGB(204,174,84) , RGB(38,26,2), rectScore, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 位置调移
			rect.OffsetRect(0, 21);

			// 位数增加
			nCount++;
		}

		if ( nCount == 4 )
		{
			break;
		}
	}
	pDC->SelectObject(pOldFont);
}

// 绘画底部信息
void CGameClientView::DrawBottomInfo(CDC *pDC, int nWidth, int nHeight)
{
	// 绘画位置
	int nDrawWidth = m_ImagePlayLeft.GetWidth() + m_ImagePlayMiddle.GetWidth()*3 + m_ImagePlayRight.GetWidth();
	CRect rectDraw( nWidth/2 - nDrawWidth/2, nHeight - m_ImagePlayLeft.GetHeight(), nWidth/2 + nDrawWidth/2, nHeight);

	// 底部背景
	CPoint ptBegin( rectDraw.left, rectDraw.top );
	m_ImagePlayLeft.DrawImage( pDC, ptBegin.x, ptBegin.y );
	ptBegin.x += m_ImagePlayLeft.GetWidth();
	for( int n = 0; n < 3; n++ )
	{
		m_ImagePlayMiddle.DrawImage( pDC, ptBegin.x, ptBegin.y );
		ptBegin.x += m_ImagePlayMiddle.GetWidth();
	}
	m_ImagePlayRight.DrawImage( pDC, ptBegin.x, ptBegin.y );

	//--------------------------------------------------------------------
	// 时间信息
	ptBegin.SetPoint( rectDraw.left, rectDraw.top - 30 );
	if ( m_wMeChairID != INVALID_CHAIR )
	{
		WORD wUserTimer = GetUserClock(m_wMeChairID);
		if ( wUserTimer != 0 ) 
		{
			DrawTime(pDC, wUserTimer, ptBegin.x, ptBegin.y );

			int nTimeFlagWidth = m_ImageTimeType.GetWidth()/3;
			int nFlagIndex=0;
			if (m_cbGameStatus == GAME_SCENE_FREE) 
				nFlagIndex = 1;
			else if (m_cbGameStatus == GAME_SCENE_BET)
				nFlagIndex = 2;
			else if (m_cbGameStatus == GAME_SCENE_END) 
				nFlagIndex = 0;

			ptBegin.SetPoint( rectDraw.left + 60, rectDraw.top - 18 );
			m_ImageTimeType.DrawImage(pDC, ptBegin.x, ptBegin.y - m_ImageTimeType.GetHeight()/2, nTimeFlagWidth, m_ImageTimeType.GetHeight(),
				nFlagIndex * nTimeFlagWidth, 0);

		}
	}

	//--------------------------------------------------------------------
	// 个人信息
	ptBegin.SetPoint( rectDraw.left, rectDraw.top );
	if (m_wMeChairID != INVALID_CHAIR)
	{
		//获取玩家
		IClientUserItem* pMeUserItem = GetClientUserItem(m_wMeChairID);
		if ( pMeUserItem != NULL )
		{
			// 头像
			DrawUserAvatar( pDC, ptBegin.x + 17, nHeight - 76, pMeUserItem );

			// 名字
			CRect rect( ptBegin.x + 130, nHeight - 93, ptBegin.x + 247, nHeight - 75);
			DrawTextString(pDC, &m_FontValleysListTwo, pMeUserItem->GetNickName(), RGB(205,174,83), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 积分
			rect.OffsetRect(0, 21);
			LONGLONG lMeBet = 0;
			for( int i = 0; i < AREA_MAX; ++i )
				lMeBet += m_lPlayBet[i];

			DrawTextString(pDC, &m_FontValleysListTwo, AddComma( pMeUserItem->GetUserScore() - lMeBet ), RGB(255,225,45), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// 成绩
			rect.OffsetRect(0, 21);
			DrawTextString(pDC, &m_FontValleysListTwo, AddComma(m_lMeStatisticScore), RGB(83,204,101), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		
			// 命中率
			CString strHit;
			FLOAT fHit = 0.f;
			rect.OffsetRect(0, 21);
			if ( m_nWinCount + m_nLoseCount > 0 )
				fHit = (FLOAT)m_nWinCount / ((FLOAT)m_nWinCount + (FLOAT)m_nLoseCount);

			if ( fHit < 0.001f )
				strHit = TEXT("无战果");
			else
				strHit.Format(TEXT("%.2f%%"), fHit*100);
			
			DrawTextString(pDC, &m_FontValleysListTwo, strHit, RGB(63,156,200), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
	}

	//--------------------------------------------------------------------
	// 右下简版路子

	int nIndex = m_nScoreHead;
	int nDrawCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{

		int nYPos = 0;
		if ( m_GameRecordArrary[nIndex].cbBankerCount > m_GameRecordArrary[nIndex].cbPlayerCount ) nYPos = rectDraw.bottom - 80;
		else if (m_GameRecordArrary[nIndex].cbBankerCount < m_GameRecordArrary[nIndex].cbPlayerCount ) nYPos = rectDraw.bottom - 53;
		else nYPos = rectDraw.bottom - 26;
		int nXPos = rectDraw.right - 330 + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 23;

		int nFlagsIndex = 2;
		if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_NULL ) nFlagsIndex = 2;
		else if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_Win) nFlagsIndex = 0;
		else if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_Lost) nFlagsIndex = 1;

		m_ImageWinFlags.DrawImage( pDC, nXPos - m_ImageWinFlags.GetWidth()/6, nYPos - m_ImageWinFlags.GetHeight()/2, m_ImageWinFlags.GetWidth()/3 , 
			m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/3 * nFlagsIndex, 0 );

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}

	//统计个数
	nIndex = m_nRecordFirst;
	int nPlayerCount = 0, nBankerCount = 0, nTieCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast != m_nRecordFirst ) )
	{
		if ( m_GameRecordArrary[nIndex].cbBankerCount < m_GameRecordArrary[nIndex].cbPlayerCount ) nPlayerCount++;
		else if ( m_GameRecordArrary[nIndex].cbBankerCount == m_GameRecordArrary[nIndex].cbPlayerCount ) nTieCount++;
		else nBankerCount++;

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}

	CRect rect(rectDraw.right - 61, rectDraw.bottom - 92, rectDraw.right - 38, rectDraw.bottom - 67);
	DrawTextString( pDC, AddComma(nBankerCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	rect.OffsetRect(0, 26);
	rect.bottom += 1;
	DrawTextString( pDC, AddComma(nPlayerCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	rect.OffsetRect(0, 28);
	DrawTextString( pDC, AddComma(nTieCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}

//显示输赢
void CGameClientView::DrawWinType(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY )
{
	// 显示输赢
	int nTypeWidth = m_ImageWinType.GetWidth()/5;
	int nTypeHeight = m_ImageWinType.GetHeight();


	//平
	if( m_nWinShowIndex[AREA_PING] > 0 )
	{
		BYTE cbAlpha = (BYTE)((DOUBLE)m_nWinShowIndex[AREA_PING] / 6.0 * 255.0);
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/2 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, 0, 0, nTypeWidth, nTypeHeight, cbAlpha);
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 3, 0, nTypeWidth, nTypeHeight, 255 - cbAlpha );
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 1, 0, nTypeWidth, nTypeHeight, 255 - cbAlpha );
	}
	else
	{

		// 庄
		if( m_nWinShowIndex[AREA_ZHUANG] == 0 )
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 3, 0, nTypeWidth, nTypeHeight );
		}
		else if( m_nWinShowIndex[AREA_ZHUANG] < 6 )
		{
			int nZhuangWidth = m_ImageWinZhuang.GetWidth()/6;
			int nZhuangHeight = m_ImageWinZhuang.GetHeight();
			m_ImageWinZhuang.DrawImage(pDC, nBeginX + 390/4*3 - nZhuangWidth/2, nBeginY + 70, nZhuangWidth, nZhuangHeight, nZhuangWidth * m_nWinShowIndex[AREA_ZHUANG], 0, nZhuangWidth, nZhuangHeight );
		}
		else
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 4, 0, nTypeWidth, nTypeHeight );;
		}

		// 闲
		if( m_nWinShowIndex[AREA_XIAN] == 0 )
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 1, 0, nTypeWidth, nTypeHeight );
		}
		else if( m_nWinShowIndex[AREA_XIAN] < 6 )
		{
			int nXianWidth = m_ImageWinXian.GetWidth()/6;
			int nXianHeight = m_ImageWinXian.GetHeight();
			m_ImageWinXian.DrawImage(pDC, nBeginX + 390/4 - nXianWidth/2, nBeginY + 70, nXianWidth, nXianHeight, nXianWidth * m_nWinShowIndex[AREA_XIAN], 0, nXianWidth, nXianHeight );
		}
		else
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 2, 0, nTypeWidth, nTypeHeight );
		}
	}

	//计算点数
	BYTE cbCardData[2][5] = {0};
	WORD cbCardCount[2] = {m_CardControl[INDEX_PLAYER].GetCardCount(), m_CardControl[INDEX_BANKER].GetCardCount() };
	int  nPointWidth = m_ImageGamePoint.GetWidth() / 10;

	// 闲家点数
	if ( cbCardCount[INDEX_PLAYER] > 0 )
	{
		// 计算点数
		m_CardControl[INDEX_PLAYER].GetCardData(cbCardData[INDEX_PLAYER], 5);
		BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(cbCardData[INDEX_PLAYER], (BYTE)cbCardCount[INDEX_PLAYER]);

		//绘画点数
		m_ImageGamePoint.DrawImage(pDC, nWidth/2 - 100 - nPointWidth/2, 170, nPointWidth, m_ImageGamePoint.GetHeight(), cbPlayerPoint * nPointWidth, 0);
	}

	// 庄家点数
	if ( cbCardCount[INDEX_BANKER] > 0 )
	{
		// 计算点数
		m_CardControl[INDEX_BANKER].GetCardData(cbCardData[INDEX_BANKER], 5);
		BYTE cbBankerPoint = m_GameLogic.GetCardListPip(cbCardData[INDEX_BANKER], (BYTE)cbCardCount[INDEX_BANKER]);

		//绘画点数
		m_ImageGamePoint.DrawImage(pDC, nWidth/2 + 100 - nPointWidth/2 , 170, nPointWidth, m_ImageGamePoint.GetHeight(), cbBankerPoint * nPointWidth, 0);
	}

	//绘画扑克
	m_CardControl[INDEX_PLAYER].DrawCardControl(pDC);
	m_CardControl[INDEX_BANKER].DrawCardControl(pDC);

	//发牌扑克
	m_DispatchCard.DrawCardControl(pDC);	


	// 发牌信息
	if( m_CardControl[INDEX_PLAYER].GetCardCount() || m_CardControl[INDEX_BANKER].GetCardCount() )
	{
		// 设置字体
		UINT   pOldAlign = pDC->SetTextAlign(TA_CENTER);

		// 发牌提示
		int nBegin = 0;
		int nStrCount = 0;
		TCHAR szString[512] = {0};
		_sntprintf(szString, sizeof(szString), TEXT("%s"), m_strDispatchCardTips);
		for(int i = 0; i < lstrlen(szString); ++i  )
		{
			if ( szString[i] == '\n' || i == lstrlen(szString) - 1 )
			{	
				TCHAR szTemp[128];
				memcpy(szTemp, &szString[nBegin], (i - nBegin + 1) * sizeof(TCHAR));

				if ( i == lstrlen(szString) - 1 )
					szTemp[(i - nBegin) + 1] = '\0';
				else
					szTemp[(i - nBegin)] = '\0';

				nBegin = i;
				DrawTextString( pDC, &m_FontValleysListTwo, szTemp, RGB(204,174,84) , RGB(38,26,2),  nWidth/2, 443 + nStrCount * 18 );
				nStrCount++;
			}
		}

		// 还原字体
		pDC->SetTextAlign(pOldAlign);
	}
}

//显示结果
void CGameClientView::DrawGameOver(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY )
{
	// 点数
	int  nPointWidth = m_ImageGameEndPoint.GetWidth() / 10;
	BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER], m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER], m_cbCardCount[INDEX_BANKER]);

	//绘画点数
	m_ImageGameEndPoint.DrawImage(pDC, nBeginX + 80 , nBeginY + 74, nPointWidth, m_ImageGamePoint.GetHeight(), cbPlayerPoint * nPointWidth, 0);
	m_ImageGameEndPoint.DrawImage(pDC, nBeginX + 321 , nBeginY + 74, nPointWidth, m_ImageGamePoint.GetHeight(), cbBankerPoint * nPointWidth, 0);

	// 分数
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN_DUI], true), nBeginX + 87 , nBeginY + 125, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_ZHUANG_DUI], true), nBeginX + 334 , nBeginY + 125, DT_CENTER);

	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN], true), nBeginX + 87 , nBeginY + 174, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_PING], true), nBeginX + 208 , nBeginY + 174, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_ZHUANG], true), nBeginX + 334 , nBeginY + 174, DT_CENTER);

	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN_TIAN], true), nBeginX + 87 , nBeginY + 223, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_TONG_DUI], true), nBeginX + 208 , nBeginY + 223, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_ZHUANG_TIAN], true), nBeginX + 334 , nBeginY + 223, DT_CENTER);

	LONGLONG lPlayAllScore = 0l;
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		lPlayAllScore += m_lPlayScore[i];
	}
	DrawNumber(pDC, &m_ImageGameEndAllScore, TEXT("+-,0123456789"), AddComma(lPlayAllScore, true), nBeginX + 97 , nBeginY + 259, DT_LEFT);
}

//设置筹码
void CGameClientView::SetCurrentBet(LONGLONG lCurrentBet)
{
	//设置变量
	ASSERT(lCurrentBet>=0L);
	m_lCurrentBet = lCurrentBet;

	if (m_lCurrentBet == 0L && m_cbGameStatus != GAME_SCENE_END )
	{
		for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
		}
		InvalidGameView(0,0,0,0);
	}
	return;
}

//历史记录
void CGameClientView::SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount, BYTE cbKingWinner, bool bPlayerTwoPair, bool bBankerTwoPair)
{
	//设置数据
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.enOperateFlags = OperateResult;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.cbBankerCount = cbBankerCount;
	GameRecord.bPlayerTwoPair=bPlayerTwoPair;
	GameRecord.bBankerTwoPair=bBankerTwoPair;
	GameRecord.cbKingWinner=cbKingWinner;

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
void CGameClientView::CleanUserBet()
{
	//清理数组
	for (BYTE i = 0; i < CountArray(m_BetInfoArray); i++)
		m_BetInfoArray[i].RemoveAll();

	//下注信息
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

	//删除下注信息
	m_strDispatchCardTips = TEXT("");

	//删除定时器
	KillTimer(IDI_FLASH_WINNER);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_DISPATCH_INTERVAL);
	KillTimer(IDI_FLASH_BET);
	KillTimer(IDI_WIN_TYPE);
	KillTimer(IDI_WIN_TYPE_DELAY);
	

	//扑克信息
	ZeroMemory(m_cbCardCount, sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	m_DispatchCard.SetCardData(NULL, 0);
	m_CardControl[0].SetCardData(NULL, 0);
	m_CardControl[1].SetCardData(NULL, 0);

	for ( int i = 0; i < CountArray(m_SkinListCtrl); ++i )
	{
		if ( m_SkinListCtrl[i].GetSafeHwnd() )
			m_SkinListCtrl[i].DeleteAllItems();
	}
	
	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//设置状态
VOID CGameClientView::SetGameStatus( BYTE cbGameStatus )
{
	m_cbGameStatus = cbGameStatus;

	if ( m_cbGameStatus == GAME_SCENE_BET )
	{
		FlexAnimation(enFlexBetTip, true);

		SetTimer(IDI_FLASH_BET, 30, NULL);
	}
	else if ( m_cbGameStatus == GAME_SCENE_END )
	{
		FlexAnimation(enFlexBetTip, false);

		KillTimer(IDI_FLASH_BET);
	}
	else if ( m_cbGameStatus == GAME_SCENE_FREE )
	{
		//设置界面
		m_CardControl[INDEX_PLAYER].SetCardData(NULL, 0);
		m_CardControl[INDEX_BANKER].SetCardData(NULL, 0);
		FlexAnimation(m_enFlexMode, false);

		KillTimer(IDI_FLASH_BET);
	}
}

//我的位置
void CGameClientView::SetMeChairID(WORD wMeChairID)
{
	m_wMeChairID = wMeChairID;
}

//设置最大下注
void CGameClientView::SetPlayBetScore(LONGLONG lPlayBetScore)
{
	m_lMeMaxScore = lPlayBetScore;
}

//区域限制
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
	m_lAreaLimitScore = lAreaLimitScore;
}

//庄家信息
void CGameClientView::SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore, LONGLONG lBankerWinScore, WORD wBankerTime)
{
	m_wBankerUser = wBankerUser;
	m_lBankerScore = lBankerScore;
	m_lBankerWinScore = lBankerWinScore;
	m_wBankerTime = wBankerTime;
}

//庄家信息
void CGameClientView::SetBankerInfo( WORD wBankerUser, LONGLONG lBankerScore )
{
	m_wBankerUser = wBankerUser;
	m_lBankerScore = lBankerScore;
}

//庄家信息
void CGameClientView::SetBankerOverInfo( LONGLONG lBankerWinScore, WORD wBankerTime )
{
	m_lBankerWinTempScore = lBankerWinScore;
	m_wBankerTime = wBankerTime;
}

//设置系统是否坐庄
void CGameClientView::SetEnableSysBanker(bool bEnableSysBanker)
{
	m_bEnableSysBanker = bEnableSysBanker;
}

//个人下注
void CGameClientView::SetPlayBet(BYTE cbViewIndex, LONGLONG lBetCount)
{
	m_lPlayBet[cbViewIndex] = lBetCount;
}

//全部下注
void CGameClientView::SetAllBet(BYTE cbViewIndex, LONGLONG lBetCount)
{
	m_lAllBet[cbViewIndex] = lBetCount;
}

//添加筹码
void CGameClientView::AddChip(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//效验参数
	ASSERT(cbViewIndex < AREA_MAX);
	if (cbViewIndex >= AREA_MAX) return;

	//变量定义
	LONGLONG lScoreIndex[BET_COUNT]={100L,1000L,10000L,100000L,1000000L,5000000L,10000000L};

	//增加判断
	bool bAddBet = (lScoreCount > 0L)?true:false;
	if(lScoreCount < 0L) lScoreCount = (lScoreCount*-1L);

	//增加筹码
	for (BYTE i = 0;i < CountArray(lScoreIndex); i++)
	{
		//计算数目
		BYTE cbScoreIndex=BET_COUNT-i-1;
		LONGLONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount == 0L) continue;

		//加入筹码
		for (LONGLONG j = 0;j < lCellCount;j++)
		{
			if (true == bAddBet)
			{
				// 构造变量
				tagBetInfo BetInfo;
				INT_PTR nAreaIndex = rand()%m_ArrayBetArea[cbViewIndex].GetCount();
				CRect rect(m_ArrayBetArea[cbViewIndex][nAreaIndex]);
				BetInfo.cbBetIndex = cbScoreIndex;

				//筹码资源
				CSize SizeBetItem(m_ImageBetView.GetWidth()/BET_COUNT,m_ImageBetView.GetHeight());
				SizeBetItem += CSize(10,10);

				BetInfo.nXPos = (rand()%(rect.Width() - SizeBetItem.cx)) + rect.left + SizeBetItem.cx/2; 
				BetInfo.nYPos = (rand()%(rect.Height() - SizeBetItem.cy)) + rect.top + SizeBetItem.cy/2;

				//插入数组
				m_BetInfoArray[cbViewIndex].Add(BetInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_BetInfoArray[cbViewIndex].GetCount(); ++nIndex)
				{
					//移除判断
					tagBetInfo &BetInfo=m_BetInfoArray[cbViewIndex][nIndex];
					if (BetInfo.cbBetIndex == cbScoreIndex)
					{
						m_BetInfoArray[cbViewIndex].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//减少数目
		lScoreCount -= lCellCount*lScoreIndex[cbScoreIndex];
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}


//设置扑克
void CGameClientView::SetCardInfo(BYTE cbCardCount[2], BYTE cbTableCardArray[2][3])
{
	if (cbCardCount != NULL)
	{
		CopyMemory(m_cbCardCount,cbCardCount,sizeof(m_cbCardCount));
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));
	}
	else
	{
		ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}


//当局成绩
void CGameClientView::SetCurGameScore(LONGLONG lPlayScore[AREA_MAX], LONGLONG lPlayAllScore)
{
	memcpy(m_lPlayScore, lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = lPlayAllScore;

	for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		if ( lPlayScore[i] > 0 )
		{
			m_nWinCount++;
		}
		else if ( lPlayScore[i] < 0)
		{
			m_nLoseCount++;
		}
	}
}

//获取区域
BYTE CGameClientView::GetBetArea(CPoint MousePoint)
{
	for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		for ( int n = 0; n < m_ArrayBetArea[i].GetCount(); ++n )
		{
			if ( m_ArrayBetArea[i][n].PtInRect(MousePoint))
			{
				return i;
			}
		}
	}

	return 0xFF;
}

//筹码按钮
void CGameClientView::OnBetButton100()
{
	//设置变量
	m_lCurrentBet=100L;

	return;
}

//筹码按钮
void CGameClientView::OnBetButton1000()
{
	//设置变量
	m_lCurrentBet=1000L;

	return;
}

//筹码按钮
void CGameClientView::OnBetButton10000()
{
	//设置变量
	m_lCurrentBet=10000L;

	return;
}

//筹码按钮
void CGameClientView::OnBetButton100000()
{
	//设置变量
	m_lCurrentBet=100000L;

	return;
}

//筹码按钮
void CGameClientView::OnBetButton1000000()
{
	//设置变量
	m_lCurrentBet=1000000L;

	return;
}
//筹码按钮
void CGameClientView::OnBetButton5000000()
{
	//设置变量
	m_lCurrentBet=5000000L;

	return;
}

//筹码按钮
void CGameClientView::OnBetButton10000000()
{
	//设置变量
	m_lCurrentBet=10000000L;

	return;
}
//查看路子
void CGameClientView::OnViewChart()
{
	if (m_DlgViewChart.m_hWnd == NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);

	//显示判断
	if (!m_DlgViewChart.IsWindowVisible())
	{
		m_DlgViewChart.CenterWindow(this);
		m_DlgViewChart.ShowWindow(SW_SHOW);
	}
	else
	{
		m_DlgViewChart.ShowWindow(SW_HIDE);
	}
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	
	if ( nIDEvent == IDI_FLASH_WINNER )				//闪动胜方
	{
		//设置变量
		if ( m_bFlashAreaAlpha )
		{
			m_nFlashAreaAlpha += 10;
			if ( m_nFlashAreaAlpha > 240)
			{
				m_bFlashAreaAlpha = false;
			}
		}
		else
		{
			m_nFlashAreaAlpha -= 10;
			if ( m_nFlashAreaAlpha < 10)
			{
				m_bFlashAreaAlpha = true;
			}
		}
		

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}
	else if ( nIDEvent == IDI_FLASH_BET )	//选取闪动
	{
		int nIndex = 0;
		while( nIndex < m_ArrayCurrentFlash.GetCount() )
		{
			if( m_ArrayCurrentFlash[nIndex].bFlashAreaAlpha )
			{
				m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha += 30;
				if ( m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha > 255 )
				{
					m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha = 255;
				}
				nIndex++;
			}
			else
			{
				m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha -= 30;
				if ( m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha < 10 )
				{
					m_ArrayCurrentFlash.RemoveAt(nIndex);
					continue;
				}
				nIndex++;
			}
		}

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}
	else if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )	//轮换庄家
	{
		ShowChangeBanker( false );
		return;
	}
	else if ( nIDEvent == IDI_FLEX_MOVE )		//伸缩移动
	{
		if ( m_pImageFlex == NULL )
			return;
		
		//位置改变
		m_ptFlexMove.y = (m_pImageFlex->GetHeight() + 137) * m_nFlexMove / FLEX_MOVE_COUNT + m_ptFlexBeing.y - m_pImageFlex->GetHeight();

		if ( m_bFlexShow )
		{
			m_nFlexMove++;
			if( m_nFlexMove > FLEX_MOVE_COUNT )
			{
				m_nFlexMove = FLEX_MOVE_COUNT;
				KillTimer(IDI_FLEX_MOVE);

				if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
				{
					//开始发牌
					DispatchCard();
				}
			}
		}
		else
		{
			m_nFlexMove--;
			if ( m_nFlexMove < 0 )
			{
				m_nFlexMove = 0;
				KillTimer(IDI_FLEX_MOVE);

				if ( m_cbGameStatus == GAME_SCENE_FREE && m_enFlexMode == enFlexDealCrad )
				{
					FlexAnimation(enFlexNULL, false);
				}
				else if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexBetTip )
				{
					//显示发牌
					m_ptFlexMove.y = m_ptFlexBeing.y - m_ImageDealBack.GetHeight();
					FlexAnimation(enFlexDealCrad, true);
				}
				else if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
				{
					//显示结算
					m_ptFlexMove.y = m_ptFlexBeing.y - m_ImageGameEnd.GetHeight();
					FlexAnimation(enFlexGameEnd, true);
				}
			}
		}
		
		//更新界面
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_DISPATCH_CARD )		// 发牌动画
	{
		CPoint ptMoveCrad;
		CPoint ptObjectiveCrad = m_CardControl[m_nDealIndex].GetBenchmarkPos();

		//播放声音
		if( m_nDealMove == 0 && !CD2DEngine::GetD2DEngine())
		{
			SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("DISPATCH_CARD"),0);
		}

		if( m_nDealMove <= DEAL_MOVE_COUNT_S )
		{
			ptMoveCrad.y = ( ptObjectiveCrad.y - m_ptDispatchCard.y ) * m_nDealMove / DEAL_MOVE_COUNT_S + m_ptDispatchCard.y;
			ptMoveCrad.x = m_ptDispatchCard.x;
		}
		else if( m_nDealMove - DEAL_MOVE_COUNT_S <= DEAL_MOVE_COUNT_H )
		{
			ptMoveCrad.y = ptObjectiveCrad.y;
			ptMoveCrad.x = ( ptObjectiveCrad.x - m_ptDispatchCard.x ) * (m_nDealMove - DEAL_MOVE_COUNT_S) / DEAL_MOVE_COUNT_H + m_ptDispatchCard.x;
		}
	
		m_DispatchCard.SetBenchmarkPos(ptMoveCrad.x,ptMoveCrad.y);
   		m_DispatchCard.ShowCardControl(true);

		m_nDealMove++;
		if ( m_nDealMove > DEAL_MOVE_COUNT_H + DEAL_MOVE_COUNT_S )
		{
			m_nDealMove = 0;
			if ( m_CardControl[m_nDealIndex].GetCardCount() < m_cbCardCount[m_nDealIndex] )
			{
				m_CardControl[m_nDealIndex].SetCardData( m_cbTableCardArray[m_nDealIndex], m_CardControl[m_nDealIndex].GetCardCount() + 1);
			}
			int nNextCrad = (m_nDealIndex + 1)%2;
			if ( m_CardControl[nNextCrad].GetCardCount() < m_cbCardCount[nNextCrad] )
			{
				m_nDealIndex = nNextCrad;
			}

			if ( m_CardControl[INDEX_PLAYER].GetCardCount() == m_cbCardCount[INDEX_PLAYER]
				&& m_CardControl[INDEX_BANKER].GetCardCount() == m_cbCardCount[INDEX_BANKER])
			{
				m_DispatchCard.SetCardData(NULL, 0);

				//发牌提示
				SetDispatchCardTips();

				//完成发牌
				FinishDispatchCard();
			}
			else 
			{
				//屏蔽发送牌
				m_DispatchCard.ShowCardControl(false);

				//发牌提示
				SetDispatchCardTips();

				//删除定时
				KillTimer(IDI_DISPATCH_CARD);

				//开启间隔定时
				if( m_CardControl[m_nDealIndex].GetCardCount() >= 2 )
					SetTimer(IDI_DISPATCH_INTERVAL, 1000, NULL);
				else
					SetTimer(IDI_DISPATCH_INTERVAL, 300, NULL);
			}
		}

		//更新界面
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_DISPATCH_INTERVAL )
	{
		//删除定时
		KillTimer(IDI_DISPATCH_INTERVAL);

		//开启发牌
		SetTimer(IDI_DISPATCH_CARD, 30, NULL);

		//更新界面
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_END_INTERVAL )
	{
		//删除定时
		KillTimer(IDI_END_INTERVAL);

		BYTE cbCrad[1] = {0};
		m_CardControl[INDEX_PLAYER].SetCardData(cbCrad, 0);
		m_CardControl[INDEX_BANKER].SetCardData(cbCrad, 0);

		//开启结束
		FlexAnimation(enFlexDealCrad, false);

		//更新界面
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_WIN_TYPE_DELAY)
	{
		//删除定时
		KillTimer(IDI_WIN_TYPE_DELAY);

		//开启动画
		SetTimer(IDI_WIN_TYPE, 40, NULL);

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}
	// 输赢翻动画
	else if ( nIDEvent == IDI_WIN_TYPE )
	{
		if ( m_nWinShowArea >= CountArray(m_nWinShowIndex) || m_nWinShowArea < 0 )
		{
			KillTimer(IDI_WIN_TYPE);
			return;
		}

		m_nWinShowIndex[m_nWinShowArea]++;

		if ( m_nWinShowIndex[m_nWinShowArea] == 6)
		{
			m_nWinShowArea = INT_MAX;
			KillTimer(IDI_WIN_TYPE);

			//如果有输赢 则开启输赢界面
			for ( int i = 0 ; i < AREA_MAX; ++i )
			{
				if ( m_lPlayScore[i] != 0 )
				{
					SetTimer(IDI_END_INTERVAL, 3000, NULL);
					break;
				}
			}
			return;
		}

		//更新界面
		InvalidGameView(0,0,0,0);
		return;
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	//BYTE cbCrad[3] = {0x03,0x03,0x03};
	//m_CardControl[INDEX_PLAYER].SetCardData(cbCrad, 3);

	//m_CardControl[1].SetCardData(cbCrad, 1);

	////更新界面
	InvalidGameView(0,0,0,0);
	if ( m_lCurrentBet != 0L )
	{
		LONGLONG lMaxPlayerScore = 0;
		BYTE cbBetArea = GetBetArea(Point);
		lMaxPlayerScore = GetMaxPlayerScore(cbBetArea);

		if ( lMaxPlayerScore < m_lCurrentBet )
			return ;

		//发送消息
		if ( cbBetArea != 0xFF ) 
			SendEngineMessage(IDM_PALY_BET, cbBetArea, 0);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置变量
	m_lCurrentBet = 0L;
	for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
	{
		m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
	}

	InvalidGameView(0,0,0,0);

	__super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if ( m_lCurrentBet != 0L && m_cbGameStatus == GAME_SCENE_BET )
	{
		//获取区域
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbBetArea = GetBetArea(MousePoint);

		//设置变量
		bool bBid = false;
		for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			if ( m_ArrayCurrentFlash[i].cbFlashArea == cbBetArea )
			{
				bBid = true;
				m_ArrayCurrentFlash[i].bFlashAreaAlpha = true;
			}
			else
			{
				m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
			}
		}

		if ( !bBid && cbBetArea < AREA_MAX )
		{
			tagFlashInfo FlashInfo;
			FlashInfo.cbFlashArea = cbBetArea;
			FlashInfo.bFlashAreaAlpha = true;
			FlashInfo.nFlashAreaAlpha = 10;
			m_ArrayCurrentFlash.Add(FlashInfo);
		}

		//区域判断
		if (cbBetArea == 0xFF)
		{
			SetCursor( LoadCursor(NULL, IDC_ARROW) );
			return TRUE;
		}

		//大小判断
		LONGLONG lMaxPlayerScore = GetMaxPlayerScore(cbBetArea);

		if ( lMaxPlayerScore < m_lCurrentBet )
		{
			SetCursor( LoadCursor(NULL, IDC_NO) );
			return TRUE;
		}

		//设置光标
		switch (m_lCurrentBet)
		{
		case 100:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)) );
				return TRUE;
			}
		case 1000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)) );
				return TRUE;
			}
		case 10000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)) );
				return TRUE;
			}
		case 100000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)) );
				return TRUE;
			}
		case 1000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)) );
				return TRUE;
			}
		case 5000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)) );
				return TRUE;
			}	
		case 10000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000000)) );
				return TRUE;
			}
		}
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

//上庄按钮
void CGameClientView::OnApplyBanker()
{
	SendEngineMessage(IDM_APPLY_BANKER,1,0);
}

//下庄按钮
void CGameClientView::OnCancelBanker()
{
	SendEngineMessage(IDM_APPLY_BANKER,0,0);
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

//银行存款
void CGameClientView::OnBankStorage()
{
	//获取接口
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerStorage.GetWindowRect(&btRect);
		ShowInsureSave(pIClientKernel,CPoint(btRect.right,btRect.top));
	}
}

//银行取款
void CGameClientView::OnBankDraw()
{
	//获取接口
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerDraw.GetWindowRect(&btRect);
		ShowInsureGet(pIClientKernel,CPoint(btRect.right,btRect.top));
	}
}

// 申请列表上
void CGameClientView::OnValleysUp()
{
	if ( m_nShowValleyIndex > 0 )
		m_nShowValleyIndex--;

	//更新界面
	InvalidGameView(0,0,0,0);
}

// 申请列表下
void CGameClientView::OnValleysDown()
{
	if( m_nShowValleyIndex < m_ValleysList.GetCount() - 1 )
		m_nShowValleyIndex++;

	//更新界面
	InvalidGameView(0,0,0,0);
}

//管理员控制
void CGameClientView::OpenAdminWnd()
{
	if( m_pClientControlDlg != NULL )
	{
		if( !m_pClientControlDlg->IsWindowVisible() ) 
		{
			m_pClientControlDlg->ShowWindow(SW_SHOW);
			m_pClientControlDlg->UpdateControl();
		}
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
}


//最大下注
LONGLONG CGameClientView::GetMaxPlayerScore( BYTE cbBetArea )
{	
	if ( m_wMeChairID == INVALID_CHAIR)
		return 0L;

	IClientUserItem* pMeUserItem = GetClientUserItem(m_wMeChairID);
	if ( pMeUserItem == NULL || cbBetArea >= AREA_MAX )
		return 0L;

	if( cbBetArea >= AREA_MAX )
		return 0L;

	//已下注额
	LONGLONG lNowBet = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
		lNowBet += m_lPlayBet[nAreaIndex];

	//庄家金币
	LONGLONG lBankerScore = -1;

	//区域倍率
	BYTE cbMultiple[AREA_MAX] = {MULTIPLE_XIAN, MULTIPLE_PING, MULTIPLE_ZHUANG, 
		MULTIPLE_XIAN_TIAN, MULTIPLE_ZHUANG_TIAN, MULTIPLE_TONG_DIAN, 
		MULTIPLE_XIAN_PING, MULTIPLE_ZHUANG_PING};

	//区域输赢
	BYTE cbArae[4][4] = {	{ AREA_XIAN_DUI,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_ZHUANG_DUI,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_XIAN,		AREA_PING,		AREA_ZHUANG,		AREA_MAX },  
							{ AREA_XIAN_TIAN,	AREA_TONG_DUI,	AREA_ZHUANG_TIAN,	255 }};
	//筹码设定
	for ( int nTopL = 0; nTopL < 4; ++nTopL )
	{
		if( cbArae[0][nTopL] == AREA_MAX )
			continue;

		for ( int nTopR = 0; nTopR < 4; ++nTopR )
		{
			if( cbArae[1][nTopR] == AREA_MAX )
				continue;

			for ( int nCentral = 0; nCentral < 4; ++nCentral )
			{
				if( cbArae[2][nCentral] == AREA_MAX )
					continue;

				for ( int nBottom = 0; nBottom < 4; ++nBottom )
				{
					if( cbArae[3][nBottom] == AREA_MAX )
						continue;

					BYTE cbWinArea[AREA_MAX] = {FALSE};

					//指定获胜区域
					if ( cbArae[0][nTopL] != 255 && cbArae[0][nTopL] != AREA_MAX )
						cbWinArea[cbArae[0][nTopL]] = TRUE;

					if ( cbArae[1][nTopR] != 255 && cbArae[1][nTopR] != AREA_MAX )
						cbWinArea[cbArae[1][nTopR]] = TRUE;

					if ( cbArae[2][nCentral] != 255 && cbArae[2][nCentral] != AREA_MAX )
						cbWinArea[cbArae[2][nCentral]] = TRUE;

					if ( cbArae[3][nBottom] != 255 && cbArae[3][nBottom] != AREA_MAX )
						cbWinArea[cbArae[3][nBottom]] = TRUE;

					//选择区域为玩家胜利，同等级的其他的区域为玩家输。以得出最大下注值
					for ( int i = 0; i < 4; i++ )
					{
						for ( int j = 0; j < 4; j++ )
						{
							if ( cbArae[i][j] == cbBetArea )
							{
								for ( int n = 0; n < 4; ++n )
								{
									if ( cbArae[i][n] != 255 && cbArae[i][n] != AREA_MAX )
									{
										cbWinArea[cbArae[i][n]] = FALSE;
									}
								}
								cbWinArea[cbArae[i][j]] = TRUE;
							}
						}
					}

					LONGLONG lScore = m_lBankerScore;
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
					{
						if ( cbWinArea[nAreaIndex] == TRUE )
						{
							lScore -= m_lAllBet[nAreaIndex]*(cbMultiple[nAreaIndex] - 1);
						}
						else if ( cbWinArea[AREA_PING] == TRUE && ( nAreaIndex == AREA_XIAN || nAreaIndex == AREA_ZHUANG ) )
						{

						}
						else
						{
							lScore += m_lAllBet[nAreaIndex];
						}
					}
					if ( lBankerScore == -1 )
						lBankerScore = lScore;
					else
						lBankerScore = min(lBankerScore, lScore);
				}
			}
		}
	}

	//最大下注
	LONGLONG lMaxBet = 0L;

	//最大下注
	lMaxBet = min( m_lMeMaxScore - lNowBet, m_lAreaLimitScore - m_lPlayBet[cbBetArea]);

	lMaxBet = min( m_lMeMaxScore - lNowBet, m_lAreaLimitScore - m_lAllBet[cbBetArea]);

	lMaxBet = min( lMaxBet, lBankerScore / (cbMultiple[cbBetArea] - 1));

	//非零限制
	ASSERT(lMaxBet >= 0);
	lMaxBet = max(lMaxBet, 0);

	return lMaxBet;
}

//开始发牌
void CGameClientView::DispatchCard()
{
	//发牌扑克
	BYTE cbCardData = 0;
	m_nDealMove = 0;
	m_nDealIndex = 0;
	m_DispatchCard.SetCardData(&cbCardData,1);
	m_DispatchCard.SetBenchmarkPos(m_ptDispatchCard.x,m_ptDispatchCard.y);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD, 30, NULL);

	//输赢动画
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex,sizeof(m_nWinShowIndex));

	//设置标识
	m_bNeedSetGameRecord = true;
}

//结束发牌
void CGameClientView::FinishDispatchCard(bool bScene)
{
	//完成判断
	if (m_bNeedSetGameRecord == false) return;

	//设置标识
	m_bNeedSetGameRecord = false;

	//删除定时器
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_FLEX_MOVE);

	//背景信息
	FlexAnimation(enFlexDealCrad, true, false);

	//设置扑克
	m_CardControl[INDEX_PLAYER].SetCardData(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	m_CardControl[INDEX_BANKER].SetCardData(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	m_DispatchCard.SetCardData(NULL,0);

	//闪动
	FlashAnimation(true);

	//设置记录
	//扑克点数
	BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//操作类型
	enOperateResult OperateResult = enOperateResult_NULL;
	if (0 < m_lPlayAllScore) OperateResult = enOperateResult_Win;
	else if (m_lPlayAllScore < 0) OperateResult = enOperateResult_Lost;
	else OperateResult = enOperateResult_NULL;

	//推断赢家
	BYTE cbWinner,cbKingWinner;
	bool bPlayerTwoPair,bBankerTwoPair;
	DeduceWinner(cbWinner, cbKingWinner, bPlayerTwoPair, bBankerTwoPair);

	//保存记录
	if(!bScene)
		SetGameHistory(OperateResult, cbPlayerPoint, cbBankerPoint, cbKingWinner, bPlayerTwoPair,bBankerTwoPair);

	//累计积分
	m_lMeStatisticScore += m_lPlayAllScore;
	m_lBankerWinScore = m_lBankerWinTempScore;

	//播放声音
	if ( m_lPlayAllScore > 0 ) 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_WIN"),0);
	else if ( m_lPlayAllScore < 0 ) 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_LOST"),0);
	else 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_DRAW"),0);

	//开启输赢动画
	if( cbPlayerPoint < cbBankerPoint )
		m_nWinShowArea = AREA_ZHUANG;
	else if( cbPlayerPoint > cbBankerPoint )
		m_nWinShowArea = AREA_XIAN;
	else
		m_nWinShowArea = AREA_PING;

	SetTimer(IDI_WIN_TYPE_DELAY, 1100, NULL);
}

//胜利边框
void CGameClientView::DrawFlashFrame(CDC *pDC, int nWidth, int nHeight)
{
	if ( m_cbGameStatus == GAME_SCENE_BET )
	{
		for ( int i = 0; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			m_ImageFrame[m_ArrayCurrentFlash[i].cbFlashArea].AlphaDrawImage(pDC, m_ptBetFrame[m_ArrayCurrentFlash[i].cbFlashArea].x, m_ptBetFrame[m_ArrayCurrentFlash[i].cbFlashArea].y, (BYTE)m_ArrayCurrentFlash[i].nFlashAreaAlpha);
		}
	}
	else if ( m_cbGameStatus == GAME_SCENE_END && m_nFlashAreaAlpha > 10 )
	{
		for ( int i = 0; i < m_ArrayFlashArea.GetCount(); ++i )
		{
			m_ImageFrame[m_ArrayFlashArea[i]].AlphaDrawImage(pDC, m_ptBetFrame[m_ArrayFlashArea[i]].x, m_ptBetFrame[m_ArrayFlashArea[i]].y, (BYTE)m_nFlashAreaAlpha);
		}
	}
}

//推断赢家
void CGameClientView::DeduceWinner( BYTE* pWinArea )
{
	//计算牌点
	BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//胜利区域--------------------------
	//平
	if( cbPlayerCount == cbBankerCount )
	{
		pWinArea[AREA_PING] = TRUE;

		// 同平点
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}

			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] )
			{
				pWinArea[AREA_TONG_DUI] = TRUE;
			}
		}
	}
	// 庄
	else if ( cbPlayerCount < cbBankerCount)  
	{
		pWinArea[AREA_ZHUANG] = TRUE;

		//天王判断
		if ( cbBankerCount == 8 || cbBankerCount == 9 )
		{
			pWinArea[AREA_ZHUANG_TIAN] = TRUE;
		}
	}
	// 闲
	else 
	{
		pWinArea[AREA_XIAN] = TRUE;

		//天王判断
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 )
		{
			pWinArea[AREA_XIAN_TIAN] = TRUE;
		}
	}


	//对子判断
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][1]))
	{
		pWinArea[AREA_XIAN_DUI] = TRUE;
	}
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][1]))
	{
		pWinArea[AREA_ZHUANG_DUI] = TRUE;
	}
}

//推断赢家
void CGameClientView::DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner, bool &bPlayerTwoPair, bool &bBankerTwoPair)
{
	cbWinner = 0;
	cbKingWinner = 0;

	//计算牌点
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//胜利玩家
	if (cbPlayerCount == cbBankerCount)
	{
		cbWinner=AREA_PING;

		//同点平判断
		bool bAllPointSame = false;
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}
			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] ) bAllPointSame = true;
		}
		if ( bAllPointSame ) cbKingWinner = AREA_TONG_DUI;
	}
	else if (cbPlayerCount<cbBankerCount) 
	{
		cbWinner=AREA_ZHUANG;

		//天王判断
		if ( cbBankerCount == 8 || cbBankerCount == 9 ) cbKingWinner = AREA_ZHUANG_TIAN;
	}
	else 
	{
		cbWinner=AREA_XIAN;

		//天王判断
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 ) cbKingWinner = AREA_XIAN_TIAN;
	}

	//对子判断
	bPlayerTwoPair=false;
	bBankerTwoPair=false;
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][1]))
		bPlayerTwoPair=true;
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][1]))
		bBankerTwoPair=true;
}

//发牌提示
void CGameClientView::SetDispatchCardTips()
{
	if (m_CardControl[INDEX_PLAYER].GetCardCount() + m_CardControl[INDEX_BANKER].GetCardCount() < 4) 
		return;

	//if ( m_CardControl[INDEX_PLAYER].GetCardCount() == m_cbCardCount[INDEX_PLAYER] && m_CardControl[INDEX_BANKER].GetCardCount() == m_cbCardCount[INDEX_BANKER])
	//{
	//	//计算点数
	//	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	//	BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);

	//	CString strTips;
	//	if ( cbPlayerCount > cbBankerCount)
	//	{
	//		strTips.Format(TEXT("闲%d点，庄%d点，闲赢。\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	else if ( cbPlayerCount < cbBankerCount)
	//	{
	//		strTips.Format(TEXT("闲%d点，庄%d点，庄赢。\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	if ( cbPlayerCount == cbBankerCount)
	//	{
	//		strTips.Format(TEXT("闲%d点，庄%d点，平。\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	return;
	//}


	//计算点数
	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],2);
	BYTE cbPlayerTwoCardCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],2);

	//闲家补牌	
	if( cbPlayerTwoCardCount <= 5 && cbBankerCount < 8 
		&& m_CardControl[INDEX_PLAYER].GetCardCount() + m_CardControl[INDEX_BANKER].GetCardCount() == 4)
	{		
		CString strTips;
		strTips.Format(TEXT("闲%d点，庄%d点，闲继续拿牌\n"), cbPlayerTwoCardCount, cbBankerCount);
		m_strDispatchCardTips = strTips;
		return;
	}

	if ( m_CardControl[INDEX_BANKER].GetCardCount() == 3 ) return;

	BYTE cbPlayerThirdCardValue = 0 ;	//第三张牌点数

	//计算点数
	cbPlayerThirdCardValue = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_PLAYER][2]);

	//庄家补牌
	CString strTips;
	if( cbPlayerTwoCardCount < 8 && cbBankerCount < 8 )
	{
		switch(cbBankerCount)
		{
		case 0:
		case 1:
		case 2:
			{
				strTips.Format(TEXT("闲前两张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerTwoCardCount, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 3:
			if(m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 8)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if (m_cbCardCount[INDEX_PLAYER] == 2) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}			
			break;
		case 4:
			if(m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 1 && cbPlayerThirdCardValue != 8 && cbPlayerThirdCardValue != 9 && cbPlayerThirdCardValue != 0)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if ( m_cbCardCount[INDEX_PLAYER] == 2) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 5:
			if( m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 1 && cbPlayerThirdCardValue != 2 && cbPlayerThirdCardValue != 3  && cbPlayerThirdCardValue != 8 && cbPlayerThirdCardValue != 9 &&  cbPlayerThirdCardValue != 0)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if ( m_cbCardCount[INDEX_PLAYER] == 2 ) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 6:
			if( m_cbCardCount[INDEX_PLAYER] == 3 && ( cbPlayerThirdCardValue == 6 || cbPlayerThirdCardValue == 7 ))
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
			//不须补牌
		case 7:
		case 8:
		case 9:
			break;
		default:
			break;
		}
	}

	return ;
}

// 添加逗号
CString CGameClientView::AddComma( LONGLONG lScore , bool bPlus /*= false*/)
{
	CString strScore;
	CString strReturn;
	LONGLONG lNumber = lScore;
	if ( lScore < 0 )
		lNumber = -lNumber;

	strScore.Format(TEXT("%I64d"), lNumber);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%3) == 0 && nStrCount != 0 )
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

	if ( lScore < 0 )
		strReturn.Insert(0, '-');

	if ( bPlus && lScore > 0 )
		strReturn.Insert(0, '+');

	return strReturn;
}

// 删除逗号
LONGLONG CGameClientView::DeleteComma( CString strScore )
{
	LONGLONG lScore = 0l;
	strScore.Remove(',');
	if ( !strScore.IsEmpty() )
		lScore = _ttoi64(strScore);

	return lScore;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};


	//保存设置
	UINT uAlign = pDC->GetTextAlign();
	UINT nDrawFormat = 0;
	if ( uAlign&TA_CENTER )
		nDrawFormat |= DT_CENTER;
	else if( uAlign&TA_RIGHT )
		nDrawFormat |= DT_RIGHT;
	else
		nDrawFormat |= DT_LEFT;

	if( uAlign&TA_BOTTOM )
		nDrawFormat |= DT_BOTTOM;
	else 
		nDrawFormat |= DT_TOP;

	//绘画边框
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//绘画字符
		CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos+nXExcursion[i],nYPos+nYExcursion[i], crFrame, nDrawFormat);
	}
	//绘画字符
	CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos,nYPos, crText, nDrawFormat);

}

void CGameClientView::DrawTextString( CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat )
{
	//变量定义
	INT nStringLength=lstrlen(pszString);
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	for (INT i=0;i<CountArray(nXExcursion);i++)
	{
		//计算位置
		CRect rcFrame;
		rcFrame.top=rcRect.top+nYExcursion[i];
		rcFrame.left=rcRect.left+nXExcursion[i];
		rcFrame.right=rcRect.right+nXExcursion[i];
		rcFrame.bottom=rcRect.bottom+nYExcursion[i];

		//绘画字符
		CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcFrame, crFrame, nDrawFormat);
	}

	//绘画字符
	CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcRect, crText, nDrawFormat);
}

//绘画字符
void CGameClientView::DrawTextString( CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos )
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};


	//保存设置
	UINT uAlign = pDC->GetTextAlign();
	UINT nDrawFormat = 0;
	if ( uAlign&TA_CENTER )
		nDrawFormat |= DT_CENTER;
	else if( uAlign&TA_RIGHT )
		nDrawFormat |= DT_RIGHT;
	else
		nDrawFormat |= DT_LEFT;

	if( uAlign&TA_BOTTOM )
		nDrawFormat |= DT_BOTTOM;
	else 
		nDrawFormat |= DT_TOP;

	//绘画边框
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//绘画字符
		pFont->DrawText(pDC, pszString, nXPos+nXExcursion[i],nYPos+nYExcursion[i], crFrame, nDrawFormat);
	}

	//绘画字符
	pFont->DrawText( pDC, pszString, nXPos,nYPos, crText, nDrawFormat);
}

//绘画字符
void CGameClientView::DrawTextString( CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat )
{
	//变量定义
	INT nStringLength=lstrlen(pszString);
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	for (INT i=0;i<CountArray(nXExcursion);i++)
	{
		//计算位置
		CRect rcFrame;
		rcFrame.top=rcRect.top+nYExcursion[i];
		rcFrame.left=rcRect.left+nXExcursion[i];
		rcFrame.right=rcRect.right+nXExcursion[i];
		rcFrame.bottom=rcRect.bottom+nYExcursion[i];

		//绘画字符
		pFont->DrawText( pDC, pszString, rcFrame, crFrame, nDrawFormat);
	}

	//绘画字符
	pFont->DrawText( pDC, pszString, rcRect, crText, nDrawFormat);
}

// 绘画数字
void CGameClientView::DrawNumber( CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
	DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// 绘画数字
void CGameClientView::DrawNumber( CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pDC, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC , CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
	// 加载资源
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pDC, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}


// 绘画时钟
void CGameClientView::DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos)
{
	//背景数字
	m_ImageTimeBack.DrawImage( pDC, nXPos, nYPos - m_ImageTimeBack.GetHeight()/2 );

	TCHAR szOutNum[128] = {0};
	if ( wUserTime > 99 )
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("99"));
	else if ( wUserTime < 10 )
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("0%d"), wUserTime);
	else
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%d"), wUserTime);

	//背景数字
	DrawNumber( pDC, &m_ImageTimeNumber, TEXT("0123456789"), szOutNum, nXPos + m_ImageTimeBack.GetWidth()/2, nYPos - 6, DT_CENTER);
}

//伸缩动画
void CGameClientView::FlexAnimation( enFlexMode nFlexMode, bool bShow , bool bMove /*= true*/)
{
	//赋值类型
	m_enFlexMode = nFlexMode;

	// 指针
	if ( nFlexMode == enFlexNULL )
	{
		m_pImageFlex = NULL;
		KillTimer(IDI_FLEX_MOVE);
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nFlexMode == enFlexBetTip )
	{
		m_pImageFlex = &m_ImageBetTip;
	}
	else if ( nFlexMode == enFlexDealCrad )
	{
		m_pImageFlex = &m_ImageDealBack;
	}
	else if ( nFlexMode == enFlexGameEnd )
	{
		m_pImageFlex = &m_ImageGameEnd;
	}

	//  无动画 即刻显示
	if ( !bMove )
	{
		if ( !bShow )
		{
			m_pImageFlex = NULL;
			m_bFlexShow = false;
			m_nFlexMove = 0;
		}
		else
		{
			m_bFlexShow = true;
			m_nFlexMove = FLEX_MOVE_COUNT;
			m_ptFlexMove = m_ptFlexBeing;
			m_ptFlexMove.y += 137;
		}
		return;
	}

	// 状态一样不显示
	if( m_bFlexShow == bShow)
		return;

	// 开启动画
	m_bFlexShow = bShow;
	SetTimer( IDI_FLEX_MOVE, 30, NULL );
	return;
}

//闪烁动画
void CGameClientView::FlashAnimation( bool bBegin )
{
	if ( !bBegin )
	{
		m_ArrayFlashArea.RemoveAll();
		m_nFlashAreaAlpha = 0;
		m_bFlashAreaAlpha = false;
		KillTimer(IDI_FLASH_WINNER);
		return;
	}

	//推断玩家
	BYTE cbWinArea[AREA_MAX] = {FALSE};
	DeduceWinner(cbWinArea);

	for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		if ( cbWinArea[i] == TRUE )
		{
			m_ArrayFlashArea.Add(i);
		}
	}
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = TRUE;
	SetTimer( IDI_FLASH_WINNER, 30, NULL );
}

//控制下注检测
void CGameClientView::ControlBetDetection( BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair )
{
	if (m_pClientControlDlg == NULL || wChair >= GAME_PLAYER )
		return;

	//获取金币
	m_lAllPlayBet[wChair][cbViewIndex] += lScoreCount;

	//玩家信息
	IClientUserItem* pUserData = GetClientUserItem(wChair);
	int nListIndex = 0;
	if ( cbViewIndex == AREA_XIAN )
		nListIndex = 0;
	else if ( cbViewIndex == AREA_PING )
		nListIndex = 1;
	else if ( cbViewIndex == AREA_ZHUANG )
		nListIndex = 2;
	else if ( cbViewIndex == AREA_XIAN_TIAN )
		nListIndex = 3;
	else if ( cbViewIndex == AREA_ZHUANG_TIAN )
		nListIndex = 4;
	else if ( cbViewIndex == AREA_TONG_DUI )
		nListIndex = 5;
	else if ( cbViewIndex == AREA_XIAN_DUI )
		nListIndex = 6;
	else if ( cbViewIndex == AREA_ZHUANG_DUI )
		nListIndex = 7;
	else
		ASSERT(FALSE);

	//构造变量
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)pUserData->GetNickName();

	//查找子项
	int nItem = m_SkinListCtrl[nListIndex].FindItem( &lvFindInfo );
	if ( nItem == -1 )
	{
		nItem = m_SkinListCtrl[nListIndex].InsertItem(m_SkinListCtrl[nListIndex].GetItemCount(), pUserData->GetNickName());//插入行
	}


	CString strScore;
	strScore.Format(TEXT("%I64d"), m_lAllPlayBet[wChair][cbViewIndex]);
	m_SkinListCtrl[nListIndex].SetItemText(nItem, 1, strScore);		//设置数据
}



//////////////////////////////////////////////////////////////////////////
