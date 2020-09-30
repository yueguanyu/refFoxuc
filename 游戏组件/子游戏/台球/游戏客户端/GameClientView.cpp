#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"

//文本框ID　
#define IDC_USER_INFO_1					1500								//玩家信息1500--1504
#define IDC_USER_SCORE_1     			1505								//银子
#define IDC_USER_COMO_1 				1509								//连击1509--1510
#define IDC_BASE_SCORE                  1511                                //底分
#define IDC_RESULT_1					1515								//成绩1513--1517
#define IDC_BALL_NUM                    1520                                //球号
#define IDC_RESUTL_TEXT                 1521                                //结算框上的文本输出1521--1533
#define IDC_SYSTEM_INFO                 1535                                //1535--1547
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define IDC_FORCE                       1554                                //力量


//-----------------------------------------------------------------------------------//

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MOVE_END,OnMovieEnd)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////

#define MAX_DISTANCE 30
CHgeFont* g_pGDIFont;
CHgeFont* g_pGDIFontSmall;
CHgeFont * g_pGDIFontStytem;

// 全局变量(HGE)
CGameClientView* g_GameFrameView = NULL;
HGE* CGameClientView::m_pHge = NULL;
CHgeButtonMgr* CGameClientView::m_pBtMgr = NULL; 

//构造函数
CGameClientView::CGameClientView()

{ 
	m_ClubAnge = 0;
	m_offsetx = 0;
	m_offsety = 0;	
	m_hitForce = 100;
	m_ptMousePos.x = 0;
	m_ptMousePos.y = 0;
	m_ptHitMark.x = 0;
	m_ptHitMark.y = 0;
	m_offsetDistance = 0;
	m_comobNum[0] = 0;
	m_comobNum[1] = 0;
	m_baseBallState = waitSet;
	m_bFirstInBall = true;
	m_bHit = false;
	m_userBallType[0] = 200;
	m_userBallType[1] = 200;
	m_ptInBallDesk.x = m_offsetx + BAR_WIDTH + 10;
	m_ptInBallDesk.y = m_offsety + BAR_WIDTH + 50;
	m_roundCount = 0;
	m_bSingleMode = true;
	m_bMoveBallClub = false;
	m_angle = 0;
	
	m_nBaseScore = 0;
	m_pSpriteBack = NULL;
	m_pParent = NULL;

	m_pHge = NULL;
	m_pBtMgr = NULL;
	m_pHgeSound = NULL;
	g_GameFrameView = this;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}


//接口查询
void *  CGameClientView::QueryInterface( REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITQView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITQView,Guid,dwQueryVer);
	return CGameFrameViewGDI::QueryInterface(Guid,dwQueryVer);
}


// 增加按钮
void  CGameClientView::AddButton(BYTE cbID,const  char *filename, DWORD size/* = 0*/, bool bMipmap/* = false*/)
{
	if (m_pBtMgr)
	{	
		// 按钮存在
		if (m_pBtMgr->IsHaveButton(cbID)) return;
		// 代码错误
		if (NULL == filename) return;

		m_pBtMgr->AddButton(cbID, filename, size, bMipmap);
	}
}

// 删除按钮
void  CGameClientView::DelButton(BYTE cbID, bool bAll/* = false*/)
{
	if (m_pBtMgr)
	{	
		// 删除全部
		if (bAll) 
		{
			m_pBtMgr->DelButton();
			return;
		}

		// 按钮不存在
		if (!m_pBtMgr->IsHaveButton(cbID)) return;

		m_pBtMgr->DelButton(cbID);
	}
}

// 设置按钮
void  CGameClientView::SetButtonPos(BYTE cbID, CPoint pt)
{
	if (m_pBtMgr)
	{	
		// 按钮不存在
		if (!m_pBtMgr->IsHaveButton(cbID)) return;

		m_pBtMgr->SetButtonPt(cbID, pt);
	}
}

// 设置显示
void  CGameClientView::SetButtonShow(BYTE cbID, WORD wType)
{
	if (m_pBtMgr)
	{	
		// 按钮不存在
		if (!m_pBtMgr->IsHaveButton(cbID)) return;

		if (wType != 0)
			m_pBtMgr->SetButtonShow(cbID, wType);
	}
}

// 获取大小
void  CGameClientView::GetButtonSize(BYTE cbID, CSize& size)
{
	size.SetSize(0, 0);

	if (m_pBtMgr)
	{
		// 按钮不存在
		if (!m_pBtMgr->IsHaveButton(cbID)) return;

		m_pBtMgr->GetButtonSize(cbID, size);
	}
}

// 增加按钮
void  CGameClientView::AddSound(WORD wID, const char* fileName)
{
	if (NULL != m_pHgeSound)
	{
		// 声音存在
		if (m_pHgeSound->IsHaveSound(wID)) return;
		// 代码错误
		if (NULL == fileName) return;

		m_pHgeSound->AddSound(wID, fileName);
	}
}

// 删除声音
void  CGameClientView::DelSound(WORD wID, bool bAll/* = false*/)
{
	if (NULL != m_pHgeSound)
	{	
		// 删除全部
		if (bAll) 
		{
			m_pHgeSound->DelSound();
			return;
		}

		// 声音不存在
		if (!m_pHgeSound->IsHaveSound(wID)) return;

		m_pHgeSound->DelSound(wID);
	}
}


// 播放声音
void  CGameClientView::PlayGameSound(WORD wID, bool bLoop/* = false*/)
{
	if (NULL != m_pHgeSound)
	{	
		// 声音不存在
		if (!m_pHgeSound->IsHaveSound(wID)) return;

		m_pHgeSound->PlayGameSound(wID, bLoop);
	}
}

// 初始化HGE
void CGameClientView::InitHge(HWND hWnd)
{
	if (NULL != m_pHge || NULL == hWnd) return;

	m_pHge = hgeCreate(HGE_VERSION);

	m_pHge->System_SetState(HGE_FRAMEFUNC, FrameView);
	m_pHge->System_SetState(HGE_RENDERFUNC, RenderView);
	//m_pHge->System_SetState(HGE_SCREENWIDTH, 800);
	//m_pHge->System_SetState(HGE_SCREENHEIGHT, 600);
	m_pHge->System_SetState(HGE_SCREENBPP, 32);
	m_pHge->System_SetState(HGE_HWNDPARENT, hWnd);
	//	m_pHge->System_SetState(HGE_WINDOWED, false); //默认为false
	m_pHge->System_SetState(HGE_USESOUND, true);
	m_pHge->System_SetState(HGE_FPS, 100);
	m_pHge->System_SetState(HGE_HIDEMOUSE, false);
	m_pHge->System_SetState(HGE_SHOWSPLASH, false);

	if(m_pHge->System_Initiate())
	{
		m_pBtMgr = new CHgeButtonMgr(this);
		m_pHgeSound = new CHgeSound(this);

		OnInitHge();

		m_pHge->System_Start();

		SetTimer(1, 20, NULL);
	}
}

// 运行HGE
void CGameClientView::RunHge(float fDelTime/* = 0*/)
{
	if (m_pHge) m_pHge->System_Start();
}



// 绘制
bool CGameClientView::RenderView(void)
{
	if (g_GameFrameView)
	{	
		bool bRe = false;
		if (m_pHge)
		{
			m_pHge->Gfx_BeginScene();
			m_pHge->Gfx_Clear(0xFFFFFFFF);

			bRe = g_GameFrameView->RenderViewEx();
			if (m_pBtMgr) m_pBtMgr->DrawButton();

			m_pHge->Gfx_EndScene();
		}
		return bRe;
	}

	return false;
}

// 窗口控制
bool CGameClientView::FrameView(void)
{
	if (g_GameFrameView)
	{		
		bool bRe = g_GameFrameView->FrameViewEx();
		if (m_pBtMgr)m_pBtMgr->UpdateButton();
		return bRe;
	}

	return false;
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	InitHge(AfxGetMainWnd()->m_hWnd);

	PlayGameSound(IDS_BACK, true);
	return 0;
}


void CGameClientView::OnDestroy( )
{ 
	KillTimer(1);

	if (m_pHge) m_pHge->System_Shutdown();
}

void CGameClientView::OnTimer(UINT_PTR nIDEvent)
{ 
	if (1 == nIDEvent && m_pHge)
	{
		m_pHge->System_Start();
	}

	__super::OnTimer(nIDEvent);
}

// 加载游戏资源
bool CGameClientView::LoadResource()
{	
//	MessageBox(0,0,0);

	g_pGDIFont = new CHgeFont(("宋体"), 14,true);
	g_pGDIFont->SetColor( ARGB(255,255,255,255) ); // 设置像素字体颜色
	
	g_pGDIFontSmall = new CHgeFont(("宋体"), 12);
	g_pGDIFontSmall->SetColor( ARGB(255,255,255,255) ); // 设置像素字体颜色
	g_pGDIFontStytem = new CHgeFont(("宋体"), 14);
	g_pGDIFontStytem->SetColor( ARGB(255,255,255,255) ); // 设置像素字体颜色
	
	m_pSpriteBack = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\di.png"));
	m_pSpriteClub = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\cue.png"));
	m_pSpriteClub->SetHotSpot(-16,8);//设置重心点
	m_pSpriteSightBall = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\25x25.png"));
	m_pSpriteSightBall->SetHotSpot(banjin,banjin);
	m_pSpriteSightBall->SetColor(ARGB(255,255,255,255));
	m_pSpriteHoleBall = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\38x38.png"));
	m_pSpriteHoleBall->SetHotSpot(19,19);
	m_pSpriteHoleBall->SetColor(ARGB(100,255,255,255));

	m_pTextForce = new CHgeText(IDC_FORCE,0,0,0,0,g_pGDIFont);
//	AddCtrl(m_pTextForce);

	m_pSpriteMovieSingle = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\lizi.png")); 
	CPoint pt(0,0);
	for(int i=0;i<NUMHOLE;i++)
		m_liziMovie[i].SetResource(m_pSpriteMovieSingle,20,pt,100);

	for(int i=0;i<72;i++)
	{
		int temp = i;
		if(i==66)
			continue;
		if(i>66)
			temp-=1;
		CString str;
		str.Format(TEXT("TaiQiu\\pic\\coin+\\coin+%d.png"),i+1);
		
		char   szANSIString   [MAX_PATH];   
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   str,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  

		m_pSpriteCoinObverse[temp] = m_HgeLoad.LoadHgeSprite(szANSIString); 
	}
	m_coinMovieObverse.SetResource(m_pSpriteCoinObverse,71,pt,100,this);
	m_coinMovieObverse.SetDisappearTime(1000);
	m_coinMovieObverse.SetPerExchangePic(2);
	for(int i=0;i<72;i++)
	{
		CString str;
		str.Format(TEXT("TaiQiu\\pic\\coin-\\coin-%d.png"),i+1);
		char   szANSIString   [MAX_PATH];   
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   str,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  
		m_pSpriteCoinReverse[i] = m_HgeLoad.LoadHgeSprite(szANSIString); 
	}
	//m_pSpriteCoinReverse = new CHgeSprite(_T("TaiQiu\\pic\\coin-.png"));
	m_coinMovieReverse.SetResource(m_pSpriteCoinReverse,72,pt,100,this);
	m_coinMovieReverse.SetDisappearTime(1000);
	m_coinMovieReverse.SetPerExchangePic(2);

	m_pSpriteReverse = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\2P.png")); 
	m_movieReverse.SetResource(m_pSpriteReverse,this);
	m_pSpriteObverse = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\1P.png")); 
	m_movieObverse.SetResource(m_pSpriteObverse,this);

	m_pSpriteInfo[0] = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\info_left.png")); 
	m_pSpriteInfo[1] = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\info_right.png")); 
	m_pSpriteCenterInfo = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\info_center.png")); 
	m_pSpriteHitMark =  m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\TEX_HitMarker.png")); 
	m_pSpriteHitMark->SetHotSpot(6,6);
	m_pSpriteHitBigBall = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\CueRegion.png")); 
	m_pSpriteHitForce_left = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\HitForce_left.png")); 
	m_pSpriteHitForce_right = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\HitForce_right.png")); 
	m_pHitAnge_right = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\HitAngle—right.png")); 
	m_pHitAngle_left = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\HitAngle—left.png")); 
	m_pSpriteLogo = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\800x600nobg.png")); 
	m_pSpriteHand = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\hand.png")); 
	m_pSpriteFailOpen = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\FailOpenning.png"));  
	m_pSpriteContinue = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\continue.png")); 
	m_pSpriteChange = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\PlayerChange.png")); 
	m_pSpriteSelect = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\ConfirmBallRange.png")); 
	m_pSpriteFailHit = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\PlayerChangeFoul.png")); 
	m_movieHitResult.SetResource(m_pSpriteFailHit,this);
	m_pSpriteWin = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\win.png")); 
	m_pSpriteLose = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\lose.png")); 
	m_pSpriteNum =  m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\num.png")); 
	m_pSpriteGameStart = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\gamestart.png"));
	m_movieGameStart.SetResource(m_pSpriteGameStart,this);
	m_movieGameStart.SetNumResource(m_pSpriteNum);

	m_pSpriteBallNumBk = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\ballNum.png"));
	m_pSpriteComob = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\combom.png"));
	m_movieComob.SetResource(m_pSpriteComob,this);
	m_movieComob.SetNumResource(m_pSpriteNum);
	m_pSpriteTimeNum = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\timer.png"));
	m_pSpriteSelect_hua = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\ConfirmBallRange_hua.png"));
	//Clock
	for(int i=0;i<5;i++)
	{
		CString str;
		str.Format(_T("TaiQiu\\pic\\clock_%d.png"),i);

		char   szANSIString   [MAX_PATH];   
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   str,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  



		m_pSpriteTimeBar[i] = m_HgeLoad.LoadHgeSprite(szANSIString);
	}
	m_pSpriteDirection[0] = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\flash0.png"));
	m_pSpriteDirection[1] =  m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\flash1.png"));
	m_directionMovie.SetResource(m_pSpriteDirection,2,pt,100,NULL);

	m_pSpriteResult = m_HgeLoad.LoadHgeSprite(("TaiQiu\\pic\\结算.png"));

	////文本
	for( int i = 0; i < GAME_PLAYER; i++ )
	{
		m_pTextUserInfo[i] = new CHgeText( IDC_USER_INFO_1+i, 0, 0, 0, 0, g_pGDIFont );		
//		AddCtrl( m_pTextUserInfo[i] );		

		m_pTextUserScore[i] = new CHgeText( IDC_USER_SCORE_1+i, 0, 0, 0, 0, g_pGDIFont );		
//		AddCtrl( m_pTextUserScore[i] );		

		m_pTextUserCombom[i] = new CHgeText( IDC_USER_COMO_1+i, 0, 0, 0, 0, g_pGDIFont );		
//		AddCtrl( m_pTextUserCombom[i] );		

		m_pTextUserBaseScore[i] = new CHgeText( IDC_BASE_SCORE+i, 0, 0, 0, 0, g_pGDIFont );		
//		AddCtrl( m_pTextUserBaseScore[i] );		
	}
	m_pTextBallNum = new CHgeText( IDC_BALL_NUM, 0, 0, 0, 0, g_pGDIFontSmall );		
//	AddCtrl( m_pTextBallNum );		
	
	for(int i=0;i<12;i++)
	{
		m_pTextEndResult[i] = new CHgeText(IDC_RESUTL_TEXT+i,0,0,0,0,g_pGDIFontStytem);
//		AddCtrl( m_pTextEndResult[i] );
		m_pTextSystemInfo[i] = new CHgeText(IDC_SYSTEM_INFO+i,0,0,0,0,g_pGDIFontStytem); 
	}
  
	////按钮
	AddButton(IDC_START, ("TaiQiu\\pic\\buttons\\start.png"));
	SetButtonShow(IDC_START, TY_BTHIDE);

	AddButton(IDC_REBEGIN, ("TaiQiu\\pic\\buttons\\ck.png"));
	SetButtonShow(IDC_REBEGIN, TY_BTSHOW|TY_BTUNENABLE);

	AddButton(IDC_RESTART, ("TaiQiu\\pic\\buttons\\restart.png"));
	SetButtonShow(IDC_RESTART, TY_BTHIDE);

	AddButton(IDC_SOUND, ("TaiQiu\\pic\\buttons\\sound.png"));
	SetButtonShow(IDC_SOUND, TY_BTHIDE);

	AddButton(IDC_GIVEPU, ("TaiQiu\\pic\\buttons\\rs.png"));
	SetButtonShow(IDC_GIVEPU, TY_BTHIDE|TY_BTUNENABLE);

	// 声音
	AddSound(IDS_BACK, ("TaiQiu\\sound\\common\\bkmusic\\Background.mp3"));
	AddSound(IDS_CLOCK, ("TaiQiu\\sound\\common\\clock.wav"));
	AddSound(IDS_ENTER, ("TaiQiu\\sound\\common\\enter.mp3"));
	AddSound(IDS_COMBO, ("TaiQiu\\sound\\common\\Combo.wav"));
	AddSound(IDS_BEG, ("TaiQiu\\sound\\common\\game_beg.wav"));
	AddSound(IDS_WIN, ("TaiQiu\\sound\\common\\win.wav"));
	AddSound(IDS_FAILURE, ("TaiQiu\\sound\\common\\failure.wav"));
	AddSound(IDS_HIT, ("TaiQiu\\sound\\common\\Hit.wav"));
	AddSound(IDS_COIN, ("TaiQiu\\SOUND\\COMMON\\coin.wav"));
	AddSound(IDS_GOOD, ("TaiQiu\\sound\\common\\goodshot.wav"));
	AddSound(IDS_COLL, ("TaiQiu\\sound\\common\\Collide.wav"));
	AddSound(IDS_HOLE, ("TaiQiu\\sound\\common\\hole.wav"));

	return true;
}

// HGE初始化后
void CGameClientView::OnInitHge(void)
{
	// 设置球HGE
	for(int i=0;i<BALL_NUM;i++)
	{ 
		m_balls[i].SetView((ITQView*)this);
		m_visibleBalls[i].SetView((ITQView*)this);
	}
	InitVisibleBall(true,false);
	m_IsGameEnd = false;
	m_physics.Init(this);
	ResetBall();
	m_baseBallState = waitSet;
	CString str = _T("欢迎来到美式8球房间,祝您游戏愉快!");	 
	m_systemInfoList.AddTail(str);
	str = _T("您现在处于练习模式,点击开始按钮, 等待开始!");
	m_systemInfoList.AddTail(str);
	
	// 装载资源
	LoadResource();
}


// 帧逻辑函数
bool CGameClientView::FrameViewEx()
{ 
	float dt = m_pHge->Timer_GetDelta();

	for (int i=0;i<11;i++)
	{
		for(int j=0;j<BALL_NUM;j++)
		{
			m_balls[j].Move();			
		}
		m_physics.Allcollision(&m_balls[0]);	
	}
	//m_physics.Allcollision(&m_balls[0]);
	for(int i=0;i<NUMHOLE;i++)	
		m_liziMovie[i].OnCalcFrame(dt);
	m_coinMovieReverse.OnCalcFrame(dt);
	m_coinMovieObverse.OnCalcFrame(dt);
	m_movieObverse.CalcPosition(dt);
	m_movieReverse.CalcPosition(dt);
	m_movieHitResult.CalcPosition(dt);
	m_movieGameStart.CalcPosition(dt);
	m_movieComob.CalcPosition(dt);
	m_directionMovie.OnCalcFrame(dt);
	if(m_offsetDistance<MAX_DISTANCE&&m_offsetDistance!=0&&m_baseBallState==startHit)
	{
		m_offsetDistance += 2;
		if(m_bHit==false&&m_offsetDistance>=MAX_DISTANCE&&m_pParent->m_wCurrentUser==m_pParent->GetMeChairID())
		{
				//发送消息
			CMD_S_HitBall  hitBall;
			hitBall.fHitForce = m_hitForce;
			hitBall.fHitPosx = m_ptCenterHitMark.x - m_ptHitMark.x;
			hitBall.fHitPosy = m_ptCenterHitMark.y - m_ptHitMark.y;
			hitBall.fMousePosx = m_ptInBallDesk.x-m_offsetx;
			hitBall.fMousePosy = m_ptInBallDesk.y-m_offsety;
			hitBall.fbaseBallPosx = m_balls[0].Ball_Pos.X;
			hitBall.fbaseBallPosy = m_balls[0].Ball_Pos.Y;
			hitBall.fAngle = m_angle;

			HitBall(m_ptCenterHitMark.x - m_ptHitMark.x,m_ptCenterHitMark.y - m_ptHitMark.y,m_ptInBallDesk.x,m_ptInBallDesk.y,m_hitForce);
			m_ptHitMark = m_ptCenterHitMark;
			m_pParent->KillGameClock(IDI_HIT_BALL);
			//TRACE("m_pParent->KillGameClock(IDI_HIT_BALL);");
			if(m_pParent&&m_bSingleMode==false&&!m_pParent->IsLookonMode())
				m_pParent->SendSocketData(SUB_S_HITBALL,&hitBall,sizeof(hitBall));
			if(m_bSingleMode)
			{
				SetButtonShow(IDC_REBEGIN, TY_BTSHOW|TY_BTENABLE);
				SetButtonShow(IDC_GIVEPU, TY_BTHIDE|TY_BTUNENABLE);
			}
			m_bHit = true;
			TRACE("HIT_BALL:m_bHit=true");
			m_angle = 0;
			while(::ShowCursor(true)<0);
		}
	}
	static DWORD time = 0;	
	if(m_baseBallState==afterHit)
	{
		time += dt*1000;
		if(time>1000)
		{//1s检测一次
			time = 0;
			TRACE("m_bHit=%d",m_bHit);
			if(BallStateCheck(dt)&&m_bHit)
			{
				m_bHit = false;
				//SetTimer(IDI_SUSPEND_SEND,500,NULL);
				/*if(m_pParent->IsLookonMode()==false&&m_pParent->GetMeChairID()==m_pParent->m_wCurrentUser)
				{
					CRect rc ;
					rc.left = m_offsetx + BAR_WIDTH;
					rc.top = m_offsety + BAR_WIDTH-1;
					rc.right = rc.left + m_pSpriteLogo->GetWidth() - BAR_WIDTH * 2;
					rc.bottom = rc.top + m_pSpriteLogo->GetHeight() - BAR_WIDTH * 2;
					if(rc.PtInRect(m_ptMousePos))
						while(ShowCursor(false)>=0);
				}*/
			 
				if(/*m_pParent->m_wCurrentUser==m_pParent->GetMeChairID()&&*/!this->m_bSingleMode&&!m_pParent->IsLookonMode())
				{//球停止运动发送消息
					//AfxMessageBox("SendHitResult");
					int holeNum = m_physics.m_nOverNum;
					BYTE firstBall = m_physics.m_wFistHitBall;
					CMD_S_HitResult  hitResult;
					hitResult.inHoleNum = holeNum;
					hitResult.firstBallNum = firstBall;
					hitResult.bHitWithWall = m_physics.m_bHitWithWall;
					memcpy(hitResult.ballInHole,m_physics.m_wOverBall,16*sizeof(BYTE));
					for(int i=0;i<BALL_NUM;i++)
					{
						hitResult.ptBall[i].x = m_balls[i].Ball_Pos.X;
						hitResult.ptBall[i].y = m_balls[i].Ball_Pos.Y;
					}
					TRACE("SendResult:%d",m_pParent->GetMeChairID());
					if(m_pParent)
						m_pParent->SendSocketData(SUB_S_HIT_RESULT,&hitResult,sizeof(hitResult));
				}						
			}
		}
	}
	//移动球杆
	static int moveBallTime = 0;
	if(m_bMoveBallClub&&(m_pParent->m_wCurrentUser!=m_pParent->GetMeChairID()||m_pParent->IsLookonMode())&&!m_bSingleMode&&(m_ptInBallDesk.x!=m_ptBallClub.x||m_ptInBallDesk.y!=m_ptBallClub.y))
	{
		//1s之内完成
		moveBallTime += 1000*dt;
		if(moveBallTime>1000)
		{
			m_ptInBallDesk.x = m_ptBallClub.x;
			m_ptInBallDesk.y = m_ptBallClub.y;
			m_bMoveBallClub = false;
			TRACE("End:%d,%d",m_ptInBallDesk.x,m_ptInBallDesk.y);
		}
		else
		{
			float rate = (m_ptBallClub.x-m_ptInBallDesk.x)/1000;
			m_ptInBallDesk.x += rate *  moveBallTime;
			rate = (m_ptBallClub.y -m_ptInBallDesk.y)/1000;
			m_ptInBallDesk.y += rate * moveBallTime;
		}
	}
	else
	{
		moveBallTime = 0;
		m_bMoveBallClub = false;
	}

	static DWORD tickCount = GetTickCount();
	//设置球杆
	if(!m_pParent|| m_pParent->m_wCurrentUser!=m_pParent->GetMeChairID())
		return false;
	if(m_baseBallState==startHit||m_IsGameEnd)
		return false;
	if(m_baseBallState==waitHit&&m_pParent&&m_pParent->GetGameStatus()==GS_WK_WAIT_HITBALL)
	{
		if(GetTickCount()-tickCount>1000&&m_bSingleMode==false)
		{
			tickCount = GetTickCount();
			CMD_S_BALLCLUB  ballClub;
			ballClub.m_hitForce = m_hitForce;
			ballClub.ptMousePos.x = m_ptInBallDesk.x - m_offsetx;
			ballClub.ptMousePos.y = m_ptInBallDesk.y - m_offsety;
			ballClub.ptBaseBallPos.x = m_balls[0].Ball_Pos.X;
			ballClub.ptBaseBallPos.y = m_balls[0].Ball_Pos.Y;
			m_pParent->SendSocketData(SUB_S_CLUBPOS,&ballClub,sizeof(ballClub));
			TRACE("Send:%f,%f",ballClub.ptMousePos.x,ballClub.ptMousePos.y);
		}
	}

	return false;
}
//打击球后的检测
bool CGameClientView::BallStateCheck(float dt)
{
	//球打击后才需要检测
	for(int i=0;i<BALL_NUM;i++)
	{
		if(m_balls[i].BALL_bEnable==false)
			continue;
		if(m_balls[i].BALL_State!=4)
			return false;
		if(m_balls[i].BALL_V.X!=0||m_balls[i].BALL_V.Y!=0||m_balls[i].BALL_V.Z!=0)
			return false;
		if(m_balls[i].BALL_Vang.X!=0||m_balls[i].BALL_Vang.Y!=0||m_balls[i].BALL_Vang.Z!=0)
			return false;
	}
	m_baseBallState = allState;//全部禁止
	if(m_bSingleMode)
	{
		if(m_balls[0].BALL_bEnable)
			m_baseBallState = waitHit;
		else
			m_baseBallState = waitSet;
	}
	//所有的球都禁止了。
	//设置进洞的球	
	return true;
}
	// 击球
void CGameClientView::HitBall(float fHitPx,float fHitPy,float mousex,float mousey,float fHitForce)
{
	m_bHit = true;
	m_baseBallState = afterHit;
	m_offsetDistance = 0;
	m_physics.InitOneOver();
	Vector3 VTMP , Bpos;
	Bpos = m_balls[0].Ball_Pos;
	//Bpos.X += m_offsetx;
	//Bpos.Y += m_offsety;	 
	mousex -= m_offsetx;
	mousey -= m_offsety;
	VTMP = NormaliseVector((Vector3(mousex, mousey)-Bpos));	 
	Vector3 hitp ; //击打点		
	VTMP.Z = PI*(m_angle/180);
	//TRACE("%f",VTMP.Z);  
	hitp = RotateZ(VTMP, PI_DIV2)* (-banjin * fHitPx/60) ;
	hitp += Bpos;
	hitp = Vector3(hitp.X, hitp.Y, -banjin * fHitPy/60);
 	//float fHitForce = m_hitForce;//((CPanelForce *)gui->GetCtrl(16))->m_fHitForce;
	m_balls[0].ApplyForce(VTMP * fHitForce / 17.f,hitp) ;
	m_hitForce = 100;

	PlayGameSound(IDS_HIT);	
}
// 控件事件通知
void CGameClientView::OnEventButton(WORD wButtonID, BYTE cbButtonType)
{
	if(m_pParent->IsLookonMode())
		return ;
	if( wButtonID == IDC_START ) 
		OnStart();		 
	else if(wButtonID==IDC_RESTART)
	{
		OnStart();
	}
	else if( wButtonID == IDC_SOUND )
		OnSound();
	else if (wButtonID == IDC_GIVEPU )
	{
		if(m_pParent->IsLookonMode())
			return ;
		m_pParent->SendSocketData(SUB_S_GIVEUP);
	}
	else if(wButtonID==IDC_REBEGIN)
	{
		SetButtonShow(IDC_REBEGIN, TY_BTUNENABLE);
		ResetGameView();
	}
}

//重置界面
void CGameClientView::ResetGameView()
{

	m_comobNum[0] = 0;
	m_comobNum[1] = 0;
	ResetBall();
	m_baseBallState = waitSet;
	m_bFirstInBall = true;
	
	m_userBallType[0] = 200;
	m_userBallType[1] = 200;
	m_bSingleMode = true;
	m_IsGameEnd = false;
	this->m_bHit = false;
	InitVisibleBall(0,false);
	m_bMoveBallClub = false;
	SetButtonShow(IDC_RESTART, TY_BTHIDE);
	return;
}

void CGameClientView::ResetUI()
{
	ResetGameView();
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	if(m_pParent == NULL || m_pParent->m_bInited == false)return;
	int logoWidth = m_pSpriteLogo->GetWidth();
	int logoHeight = m_pSpriteLogo->GetHeight();
	m_lWidth = nWidth;
	m_lHeight = nHeight;
	m_offsetx = (m_lWidth-logoWidth)/2;
	m_offsety = (m_lHeight-logoHeight-230)/2;
	if(m_offsetx<=0)
		m_offsetx = 0;
	if(m_offsety<=0)
		m_offsety = 0;
	for(int i=0;i<BALL_NUM;i++)
	{
		m_balls[i].SetOffSet(m_offsetx,m_offsety);
		m_visibleBalls[i].SetOffSet(m_offsetx,m_offsety);
	}
	this->m_physics.SetOffset(m_offsetx,m_offsety);
	int x = m_offsetx + 13;
	int y = m_offsety + m_pSpriteLogo->GetHeight() - 5;	 
	x += m_pSpriteInfo[0]->GetWidth(); 
	x += m_pSpriteCenterInfo->GetWidth();	 
	x += m_pSpriteInfo[1]->GetWidth();
	x -= 53;
	y += 51;
	m_ptCenterHitMark.x = x;
	m_ptCenterHitMark.y = y;
	m_ptHitMark = m_ptCenterHitMark;

	x = m_offsetx + 13 + 82;
	y = m_offsety + m_pSpriteLogo->GetHeight()  + 62;
	BYTE me = 0;
	BYTE other =  1;
	if(m_pParent&&m_pParent->GetMeChairID()==1)
	{
		me = 0;
		other = 1;
	}
	else
	{
		me = 1;
		other = 0;
	}
	//CString str;
	//str.Format("%d",m_pParent->GetMeChairID());
	//AfxMessageBox(str);
	m_ptReady[me].x = m_offsetx + 38;
	m_ptReady[me].y = m_offsety + 450;
	m_ptReady[other].x = m_offsetx + 510;
	m_ptReady[other].y = m_offsety + 450;
	//左边座位号为0；
	m_pTextUserInfo[me]->SetPos(x,y);
	m_pTextUserScore[me]->SetPos(x,y+23);
	m_pTextUserCombom[me]->SetPos(x,y+46);
	m_pTextUserBaseScore[me]->SetPos(x,y+69);

	//右边座位号为1
	x = m_offsetx + 13 + 557;
	y = m_offsety +  m_pSpriteLogo->GetHeight()  + 62;
	m_pTextUserInfo[other]->SetPos(x,y);
	m_pTextUserScore[other]->SetPos(x,y+23);
	m_pTextUserCombom[other]->SetPos(x,y+46);
	m_pTextUserBaseScore[other]->SetPos(x,y+69);

	x = m_offsetx + 42;
	y = m_offsety + m_pSpriteLogo->GetHeight()  + 70;
	m_ptAvatar[me].x = x;
	m_ptAvatar[me].y = y;
	x = m_offsetx + 519;
	y = m_offsety + m_pSpriteLogo->GetHeight()  + 70;
	m_ptAvatar[other].x = x;
	m_ptAvatar[other].y = y;
	RectifyButton();

}

	//设置按钮位置
void CGameClientView::RectifyButton()
{	
//	//移动按钮
	CRect rcButton;
  
	int x = 0;
	int y = 0;
	x = m_offsetx + 435;
	y = m_offsety + 600;
	SetButtonPos(IDC_SOUND, CPoint(x, y));
	x = x - 192;
	SetButtonPos(IDC_GIVEPU, CPoint(x, y));
	SetButtonPos(IDC_REBEGIN, CPoint(x, y));	
	SetButtonPos(IDC_START, CPoint(711+m_offsetx, 545+m_offsety));
}

//绘画界面
bool CGameClientView::RenderViewEx(void)
{
	CRect rect;
	GetClientRect(&rect);
	////画背景

	DrawBack(&rect); 
	m_pTextBallNum->printfEx("");
	for(int i=1;i<BALL_NUM;i++)
	{
		m_balls[i].Render(); 		 
	}
	if(m_baseBallState!=waitSet&&m_baseBallState!=nothing)
		m_balls[0].Render();
	int x = m_ptInBallDesk.x-15;
	int y = m_ptInBallDesk.y-15;
	if(m_baseBallState==waitSet)
	{// 画手拿球的状态		
		//是否与某一个球重合
		bool canSet = true;
		for(int i=1;i<BALL_NUM;i++)
		{
			if(m_balls[i].BALL_bEnable==false)
				continue;
			Vector3 temp( m_ptInBallDesk.x-m_offsetx,m_ptInBallDesk.y-m_offsety,0);
			float a = (temp-m_balls[i].Ball_Pos).mag();
			if(a<banjin+10)
			{
				canSet = false;
				break;
			}
		}
	 
		if(canSet&&x>=m_offsetx+42&&x<m_offsetx+200&&y>m_offsety+38&&y<m_offsety+373)
		{//球的状态合理
			
			m_pSpriteHand->SetTextureRect(m_pSpriteHand->GetWidth()/2,0,m_pSpriteHand->GetWidth()/2,m_pSpriteHand->GetHeight());			
			m_pSpriteHand->Render(x,y);
		}
		else
		{
			m_pSpriteHand->SetTextureRect(0,0,m_pSpriteHand->GetWidth()/2,m_pSpriteHand->GetHeight());
			 
			m_pSpriteHand->Render(x, y);
		}
	}
	DrawBallClub();

	//int i;
	//for (i=0;i<NUMWALL;i++)
	//{
	//	physics::cline &tWALL = this->m_physics.WALL[i];
	//	m_balls[0].m_GraphHelper->Gfx_RenderLine(tWALL.starpnt.X, tWALL.starpnt.Y,tWALL.endpnt.X,  tWALL.endpnt.Y);
	//	
	//}
	//for (i=0;i<NUMPING;i++)
	//{
	//	int x1 = m_physics.Ping[i].pnt.X;
	//	int y1 = m_physics.Ping[i].pnt.Y;
	//	int R = m_physics.Ping[i].R;
	// 
	//	//Ellipse	(hdc,x1+R,BACK_HEIGHT-(y1-R),x1-R,BACK_HEIGHT-(y1+R));
	//	m_pSpriteSightBall->Render( x1,(y1));
	//	 
	//}

	//
	//for (i=0;i<NUMHOLE;i++)
	//{
	//	 int x1 = m_physics.Hole[i].pnt.X;
	//	 int y1 = m_physics.Hole[i].pnt.Y;
	//	 int R = m_physics.Hole[i].R;
	//	// HPEN hPen;
	//	// hPen= (HPEN)SelectObject(hdc,   GetStockObject(HOLLOW_BRUSH));
	//	// Ellipse	(hdc,x1+R,BACK_HEIGHT-(y1-R),x1-R,BACK_HEIGHT-(y1+R));
	//	m_pSpriteHoleBall->Render( x1,(y1));
	//}
	x = m_offsetx + 13;
	y = m_offsety + m_pSpriteLogo->GetHeight() - 5;
	m_pSpriteInfo[0]->Render(x,y);
	x += m_pSpriteInfo[0]->GetWidth();
	m_pSpriteCenterInfo->Render(x,y);
	x += m_pSpriteCenterInfo->GetWidth();
	m_pSpriteInfo[1]->Render(x,y);	

	m_pSpriteHitMark->Render(m_ptHitMark.x,m_ptHitMark.y);
	float w = m_pSpriteHitForce_left->GetWidth()/15;
	int h  = m_pSpriteHitForce_left->GetHeight();
	float offset = 100.0/30.0;
	if(m_hitForce<offset)
		m_hitForce = MIN_FORCE;
	float bx = int(m_hitForce/offset-1) ;
	x = m_offsetx + 13 + m_pSpriteInfo[0]->GetWidth()+m_pSpriteCenterInfo->GetWidth()+m_pSpriteInfo[1]->GetWidth();
	x = x - w;
	y =  m_offsety + m_pSpriteLogo->GetHeight() - 4;
	if(bx<15)
	{
		bx = bx * w;
		m_pSpriteHitForce_left->SetTextureRect(bx,0,w,h);
		m_pSpriteHitForce_left->Render(x,y);
	}
	else
	{
		bx = (bx-15)*w;
		m_pSpriteHitForce_right->SetTextureRect(bx,0,w,h);
		m_pSpriteHitForce_right->Render(x,y);
	}

	if(m_angle>0)
	{
		w = m_pHitAngle_left->GetWidth()/22;
		h = m_pHitAngle_left->GetHeight();
		offset = 90/44;
		bx = int(m_angle/offset-1);
		x = 8+ m_offsetx  + m_pSpriteInfo[0]->GetWidth()+m_pSpriteCenterInfo->GetWidth()+m_pSpriteInfo[1]->GetWidth();
		x = x - w;
		y =  m_offsety + m_pSpriteLogo->GetHeight() - 5;
		if(bx<22)
		{
			bx = bx * w+0.5;
			m_pHitAngle_left->SetTextureRect(bx,0,w,h);
			m_pHitAngle_left->Render(x,y);
		}
		else
		{
			bx = (bx-22)*w+0.2;
			m_pHitAnge_right->SetTextureRect(bx,0,w,h);
			m_pHitAnge_right->Render(x,y);
		}

	}
	//m_pTextForce->SetPos(m_ptMousePos.x,m_ptMousePos.y);
	//m_pTextForce->printf("%f",m_hitForce);
	for(int i=0;i<NUMHOLE;i++)
		m_liziMovie[i].DrawMovice();	
	m_coinMovieObverse.DrawMovice();
	m_coinMovieReverse.DrawMovice();
	m_movieObverse.DrawMovie();
	m_movieReverse.DrawMovie();
	m_movieHitResult.DrawMovie();
	m_movieGameStart.DrawMovie();
	m_movieComob.DrawMovie();
	m_directionMovie.DrawMovice();

	DrawUser(&rect);
	DrawResultFrame(&rect);
	for(int i=1;i<BALL_NUM;i++)
		m_visibleBalls[i].RenderEx();

	for(int i=1;i<BALL_NUM;i++)
	{	 
		//if(m_baseBallState==waitHit&&m_balls[i].BALL_State==4&&m_balls[i].BALL_bEnable&&m_balls[i].MouseMoveIn(m_ptMousePos.x,m_ptMousePos.y))
		if(m_balls[i].BALL_bEnable&&m_balls[i].MouseMoveIn(m_ptMousePos.x,m_ptMousePos.y))
		{
			m_pSpriteBallNumBk->Render(m_balls[i].Ball_Pos.X-26+m_offsetx,m_balls[i].Ball_Pos.Y-40+m_offsety);
			CString str;
			str.Format(TEXT("%d号球"),m_balls[i].BallNo);
			m_pTextBallNum->SetPos(m_balls[i].Ball_Pos.X-26+4+m_offsetx,m_balls[i].Ball_Pos.Y-40+13+m_offsety);

			char   szANSIString   [MAX_PATH];   
			WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   str,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  
			m_pTextBallNum->printfEx(szANSIString);
		}
	}

	POSITION pos = m_systemInfoList.GetHeadPosition();
	int flag=0;
	x = m_offsetx + 240;
	y = m_offsety + 500;
	int maxNum = 0;
	while(pos)
	{//最多6行
		CString str = m_systemInfoList.GetNext(pos);
		int n = SplitString(str,18);
		maxNum += n;
		if(maxNum>6)
		{
			m_systemInfoList.RemoveHead();
			break;
		}
		m_pTextSystemInfo[flag]->SetMode(HGETEXT_LEFT|HGETEXT_CENTER);
		m_pTextSystemInfo[flag]->SetPos(x,y);

		char   szANSIString   [MAX_PATH];   
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   str,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  

		m_pTextSystemInfo[flag]->printfEx(szANSIString);		
		y+= n*18;
		flag++;
		
	}
	//画箭头
    if(m_bSingleMode==false)
	{
		CPoint pt;
		if(m_pParent->SwitchViewChairID(m_pParent->m_wCurrentUser)==0)
		{// 右边
			pt.x = m_offsetx + 514;
			pt.y = m_offsety + 462;
		}
		else
		{//左边
			pt.x = m_offsetx + 40;
			pt.y = m_offsety + 462;
		}
		if(m_baseBallState!=allState&&m_baseBallState!=nothing)
		{
			//this->m_directionMovie.Start(pt,500,true);
			static DWORD tickCount = GetTickCount();
			if(GetTickCount()-tickCount>800)
			{
				m_pSpriteDirection[0]->Render(pt.x,pt.y);
				if(GetTickCount()-tickCount>1600)
					tickCount = GetTickCount();
			}
			else
			{
				m_pSpriteDirection[1]->Render(pt.x,pt.y);
			}
		}
	}
	DrawTableWaitTime();

	return false;
}


	//画球杆
void  CGameClientView::DrawBallClub()
{
	//if(!m_pParent||m_pParent->m_wCurrentUser!=m_pParent->GetMeChairID())
	//	return ;
	if(this->m_IsGameEnd)
		return ;
	if((m_baseBallState==waitHit)&&m_balls[0].BALL_bEnable&&m_balls[0].BALL_State==4)
	{			
		//画线
		static int nMoveLine=0;
		float bx = m_balls[0].Ball_Pos.X;
		float by = m_balls[0].Ball_Pos.Y;
		by += m_offsety;
		bx += m_offsetx;
		static float mousex = 0,mousey = 0;
		//m_balls[0].m_GraphHelper->Input_GetMousePos(&mousex,&mousey);
		CRect rc ;
		rc.left = m_offsetx + BAR_WIDTH;
		rc.top = m_offsety + BAR_WIDTH;
		rc.right = rc.left + m_pSpriteLogo->GetWidth() - BAR_WIDTH * 2;
		rc.bottom = rc.top + m_pSpriteLogo->GetHeight() - BAR_WIDTH * 2;
		if(rc.PtInRect(m_ptInBallDesk))
		{
			mousex = m_ptInBallDesk.x;
			mousey = m_ptInBallDesk.y;
		}
		//角度
		m_ClubAnge = User_Atn((FLOAT)(bx-(mousex)),(FLOAT)(by-(mousey)));//
		//球杆
		int length  = GetBallClubLength();
		//m_pSpriteClockNum->SetTextureRect( nNum1*w, 0, w, 40/*m_pSpriteClockNum->GetHeight()*/ );
		m_pSpriteClub->SetTextureRect(0,0,length,m_pSpriteClub->GetHeight());

		m_pSpriteClub->SetHotSpot(-16 - m_offsetDistance,8);//设置重心点

		m_pSpriteClub->RenderEx(m_balls[0].Ball_Pos.X+m_offsetx,m_balls[0].Ball_Pos.Y+m_offsety,m_ClubAnge);
		//线
		if(m_pParent->m_wCurrentUser==m_pParent->GetMeChairID()&&(!m_pParent->IsLookonMode()||m_bSingleMode))
		{
			float nLength=sqrt((float)((bx-mousex)*(bx-mousex)+(by-mousey)*(by-mousey)));
			float sinValue=(mousey-by)/nLength;
			float cosValue=(mousex-bx)/nLength;	
			for(int i=0;i+10<nLength;i+=10)
			{
				//上下
				m_balls[0].GetHge()->Gfx_RenderLine(bx+(i+nMoveLine)*cosValue-1,by+(i+nMoveLine)*sinValue
					,bx+(i+nMoveLine)*cosValue+1,by+(i+nMoveLine)*sinValue);
				m_balls[0].GetHge()->Gfx_RenderLine(bx+(i+nMoveLine)*cosValue,by+(i+nMoveLine)*sinValue-1
					,bx+(i+nMoveLine)*cosValue,by+(i+nMoveLine)*sinValue+1);			
			}
			nMoveLine++;
			if(nMoveLine>10)
				nMoveLine=0;	
			m_pSpriteSightBall->Render(mousex,mousey);
		}
	

		static float tempForce = m_hitForce;
		SHORT state = GetKeyState(VK_LBUTTON);
		if(HIWORD(state)&&rc.PtInRect(m_ptMousePos)&&m_pParent->m_wCurrentUser==m_pParent->GetMeChairID()&&m_pParent->IsLookonMode()==false)
		{//鼠标左键被按下了。。
			if(m_hitForce>tempForce)
				m_hitForce += 1;
			else
				m_hitForce -= 1;
			if(m_hitForce<MIN_FORCE)
			{
				m_hitForce = MIN_FORCE;
				tempForce = m_hitForce - 1;
			}
			if(m_hitForce>=100)
			{
				m_hitForce = 100;
				tempForce = 100;
			}

			float w = m_pSpriteHitForce_left->GetWidth()/15;
			int h  = m_pSpriteHitForce_left->GetHeight();
			float offset = 100.0/30.0;
			if(m_hitForce<offset)
				m_hitForce = MIN_FORCE;
			float bx = int(m_hitForce/offset-1) ;
			int x = m_balls[0].Ball_Pos.X-50+m_offsetx;
			int y =  m_balls[0].Ball_Pos.Y-50+m_offsety;
			if(bx<15)
			{
				bx = bx * w;
				m_pSpriteHitForce_left->SetTextureRect(bx,0,w,h);
				m_pSpriteHitForce_left->Render(x,y);
			}
			else
			{
				bx = (bx-15)*w;
				m_pSpriteHitForce_right->SetTextureRect(bx,0,w,h);
				m_pSpriteHitForce_right->Render(x,y);
			}		 
		}
		else
		{ 
			//m_hitForce = 100;
			tempForce = 100;
		}
	}	
}

//画背景
void CGameClientView::DrawBack( CRect *pRect )
{
	ASSERT( pRect != NULL );
	ASSERT( m_pSpriteBack != NULL );

	int nW = m_pSpriteBack->GetWidth();
	int nH = m_pSpriteBack->GetHeight();
	//画底图
	for( int i = 0; i < pRect->Width(); i += m_pSpriteBack->GetWidth())
	{
		for( int j = 0; j < pRect->Height(); j += m_pSpriteBack->GetHeight())
		{
			m_pSpriteBack->Render( i, j );
		}
	}


	////画LOGO
	//m_pSpriteLogo->SetColor( ARGB(150, 255, 255, 255) );
	//m_pSpriteLogo->Render( 0/*(pRect->Width() - m_pSpriteLogo->GetWidth())/2*/, 
	//	(pRect->Height()-80 - m_pSpriteLogo->GetHeight())/2 ); 
	m_pSpriteLogo->Render(m_offsetx,m_offsety);
}

//画玩家
void CGameClientView::DrawUser( CRect *pRect )
{
	
	////绘画用户
	char szBuffer[1024];
	char   szANSIString   [MAX_PATH];   
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer = GetUserClock(i);
		tagUserInfo * pUserData=GetClientUserItem(i) ==NULL ? NULL:GetClientUserItem(i)->GetUserInfo();
		//绘画用户
		if (pUserData!=NULL)
		{	
			////用户信息
			//			DrawAvatar(pUserData->wFaceID,m_ptAvatar[i].x,m_ptAvatar[i].y,pUserData->cbUserStatus==US_OFFLINE);
			WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   pUserData->szNickName,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  
			_snprintf(szBuffer,1024,("昵称:%s"),szANSIString);
			int len = strlen(szBuffer);
			int needLen = 15;
			if(len<needLen)
			{
				szBuffer[needLen] = '\0';
			}
			else
			{
				int i = 0;
				for(;i<needLen-3;i++)
				{	
					if(szBuffer[i]&0XF000)
						i++;
				}			
				for( ;i<needLen;i++)
				{

					szBuffer[i] = '.';
				}
				szBuffer[needLen] = '\0';
			}	

			m_pTextUserInfo[i]->printfEx( ("%s"), szBuffer);	
			ZeroMemory(szBuffer,sizeof(szBuffer));		
			if(m_pParent&&m_pParent->m_serverType&GAME_GENRE_SCORE)
				_snprintf(szBuffer,1024,("积分:%I64d"),pUserData->lScore);
			else
				_snprintf(szBuffer,1024,("银子:%I64d"),pUserData->lScore);			


			m_pTextUserScore[i]->printfEx( ("%s"), szBuffer);	
			ZeroMemory(szBuffer,sizeof(szBuffer));		
			_snprintf(szBuffer,1024,("连击:%d"),m_comobNum[i]);			

			//char   szANSIString   [MAX_PATH];   
			m_pTextUserCombom[i]->printfEx( ("%s"), szBuffer);	
			if(m_pParent&&m_pParent->m_serverType&GAME_GENRE_SCORE)
			{
				int totalCount = pUserData->dwWinCount+pUserData->dwLostCount+pUserData->dwDrawCount;
				float winRate = 0.0;
				if(totalCount>0)
					winRate = float(pUserData->dwWinCount)/totalCount;
				else winRate = 0.0;
				winRate*=100;
				CString str;
				str.Format(TEXT("胜率:%3.2f"),winRate);	
				str += TEXT("\%");

				m_pTextUserBaseScore[i]->printfEx( ("%s%%"), szBuffer);
			}
			else
			{
				_snprintf(szBuffer,1024,("底分:%I64d"),m_nBaseScore);	

				char   szANSIString   [MAX_PATH];   
				m_pTextUserBaseScore[i]->printfEx( ("%s"), szBuffer);
			}

			//其他信息
			if (wUserTimer!=0) 
				DrawTime( wUserTimer,m_ptTimer[i] );
			//if (pUserData->cbUserStatus==US_READY) 
			//DrawUserReady(m_ptReady[i].x,m_ptReady[i].y);
		}		
		else
		{
			m_pTextUserInfo[i]->printfEx((""));
			m_pTextUserScore[i]->printfEx((""));
			m_pTextUserCombom[i]->printfEx((""));
			m_pTextUserBaseScore[i]->printfEx((""));
		}	
	}	 


	static int waitTime = 0;
	static int tickCount = GetTickCount();
	static int lastWaitTime = 0;
	tagUserInfo * pUserData0=GetClientUserItem(0) ==NULL ? NULL:GetClientUserItem(0)->GetUserInfo();
	tagUserInfo * pUserData1=GetClientUserItem(1) ==NULL ? NULL:GetClientUserItem(1)->GetUserInfo();
	if(pUserData0&&pUserData0->cbUserStatus==US_SIT&&pUserData1&&pUserData1->cbUserStatus==US_READY||
		pUserData1&&pUserData1->cbUserStatus==US_SIT&&pUserData0&&pUserData0->cbUserStatus==US_READY)
	{//两个人都坐下，一个人点开始了
		if(waitTime==0&&lastWaitTime==0)
		{
			tickCount = GetTickCount();
			waitTime = 10;
			lastWaitTime = 10;
		}
	}
	else
	{
		waitTime = 0;
		lastWaitTime = waitTime;
	}
	if(waitTime>0&&GetTickCount()-tickCount>=1000)
	{
		tickCount = GetTickCount();
		lastWaitTime = waitTime;
		waitTime--;
		if(waitTime==0)
		{
			OnStart();
			waitTime = 0;
		}
		else
		{
			CString str;
			str.Format(_T("游戏将在%ds后自动开始..."),waitTime);
			m_systemInfoList.AddTail(str);
		}
	}


}
 	//画时间
void CGameClientView::DrawTime( int _nTime, CPoint point )
{
	
	if(m_pParent ==NULL )
		return;
	if(_nTime>100)
		return ;
	static int newTime = _nTime;
	float nTime = _nTime;
	static DWORD tickCount = GetTickCount();
	if(newTime==_nTime&&(GetTickCount() - tickCount)<1000*30)
	{
		nTime -= (GetTickCount() - tickCount)/float(1000.0);
	}
	else
	{		
		newTime = _nTime;
		tickCount = GetTickCount();
	}
	//底
	int x = m_offsetx + 290;
	int y = m_offsety + 601;
//	m_pSpriteTimeBar[0]->Render(x,y);
	int totalTime = 0;
	if(m_pParent->GetGameStatus()==GAME_STATUS_FREE)
		totalTime = TIME_GAME_START;
	else if(m_pParent->GetGameStatus()==GS_WK_SET_SETBASEBALL)
		totalTime = TIME_SET_BASEBALL;
	else if(m_pParent->GetGameStatus()==GS_WK_WAIT_HITBALL)
		totalTime = TIME_HIT_BALL;
	totalTime = totalTime/1000;
	float rate = nTime/float(totalTime);
	int index = 1;
	if(rate>0.75)
		index = 1;
	else if(rate>0.5)
		index = 2;
	else if(rate>0.25)
		index = 3;
	else index = 4;
	 
	m_pSpriteTimeBar[index]->SetTextureRect(0,0,(rate)*m_pSpriteTimeBar[index]->GetWidth(),m_pSpriteTimeBar[index]->GetHeight());
	m_pSpriteTimeBar[index]->Render(x,y);
	//画数字
	x += 70;
	y += 0;
	int nNum1 = _nTime/10;
	int nNum2 = _nTime%10;
	int w = m_pSpriteTimeNum->GetWidth()/10;
	m_pSpriteTimeNum->SetTextureRect( (nNum1+9)%10*w, 0, w,  m_pSpriteTimeNum->GetHeight());
	m_pSpriteTimeNum->Render( x, y);
	m_pSpriteTimeNum->SetTextureRect( (nNum2+9)%10*w, 0, w, m_pSpriteTimeNum->GetHeight() );
	m_pSpriteTimeNum->Render( x+w,y );

}
void CGameClientView::DrawTableWaitTime()
{
	 tagUserInfo * pUserData=GetClientUserItem(1) ==NULL ? NULL:GetClientUserItem(1)->GetUserInfo();
	if(pUserData==NULL||pUserData->cbUserStatus!=US_SIT)
		return ;
	WORD wUserTimer = GetUserClock(pUserData->wChairID);
	if(wUserTimer&&wUserTimer<100)
		DrawTime(wUserTimer,m_ptTimer[2]);
}
 
//画结算框
void CGameClientView::DrawResultFrame( CRect *pRect )
{
	for(int i=0;i<12;i++)
	{
		m_pTextEndResult[i]->printfEx("");
	}
	if(!this->m_IsGameEnd)
		return ;
	int x = m_offsetx + m_pSpriteLogo->GetWidth()/2 - m_pSpriteWin->GetWidth()/2;
	int y = m_offsety + m_pSpriteLogo->GetHeight()/2 - m_pSpriteLose->GetHeight()/2;
	CSize size;
	GetButtonSize(IDC_RESTART, size);
	SetButtonPos(IDC_RESTART, CPoint(x+(m_pSpriteWin->GetWidth()-size.cx)/2,y+125));
	if(m_pParent->IsLookonMode())
	{
		SetButtonShow(IDC_RESTART, TY_BTHIDE|TY_BTUNENABLE);
	}
	else
	{
		SetButtonShow(IDC_RESTART, TY_BTSHOW|TY_BTENABLE);
	}
	if(m_endScore[1]>0)
	{
		m_pSpriteWin->Render(x,y);
	}
	else
	{
		m_pSpriteLose->Render(x,y);
	}
	TCHAR szBuffer[1024];
	ZeroMemory(szBuffer,sizeof(szBuffer));
	int py = y + 86;
	int textNum = 0;
	for(int i=0;i<GAME_PLAYER;i++)
	{	
		int px = x + 16;
		BYTE player  = i;
		 tagUserInfo * pUserData=GetClientUserItem(player) ==NULL ? NULL:GetClientUserItem(player)->GetUserInfo();
		if(pUserData==NULL)
			continue;
		_sntprintf(szBuffer,1024,TEXT("%s"),pUserData->szNickName);
		/*int len = strlen(szBuffer);
		int needLen = 20;
		if(len<needLen)
		{
			szBuffer[needLen] = '\0';
		}
		else
		{
			int i = 0;
			for(;i<needLen-3;i++)
			{	
				if(szBuffer[i]&0XF000)
					i++;
			}			 		
			for( ;i<needLen;i++)
			{
				
				szBuffer[i] = '.';
			}
			szBuffer[needLen] = '\0';
		}	*/
		this->FillStringWithSpace(szBuffer,15);

		m_pTextEndResult[textNum]->SetPos(px,py);

		char   szANSIString   [MAX_PATH];   
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   szBuffer,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   );  

		m_pTextEndResult[textNum++]->printfEx(szANSIString);
		ZeroMemory(szBuffer,sizeof(szBuffer));
		//连击
		_sntprintf(szBuffer,1024,_T("%d"),this->m_comobNum[i]);
		px = x + 155;
		m_pTextEndResult[textNum]->SetPos(px,py);
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   szBuffer,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   ); 
		m_pTextEndResult[textNum++]->printfEx(szANSIString);
		//胜负
		if(m_endScore[i]>0)
		{
			_sntprintf(szBuffer,1024,TEXT("%s"),TEXT("胜利"));
		}
		else
		{
			_sntprintf(szBuffer,1024,TEXT("%s"),TEXT("失败"));
		}
		px = x + 224;
		m_pTextEndResult[textNum]->SetPos(px,py);


		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   szBuffer,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   ); 
		m_pTextEndResult[textNum++]->printfEx(szANSIString);
		//银子
		if(m_endScore[i]>0)
			_sntprintf(szBuffer,1024,TEXT("+%I64d"),m_endScore[i]);
		else
			_sntprintf(szBuffer,1024,TEXT("%I64d"),m_endScore[i]);
		px = x + 310;
		m_pTextEndResult[textNum]->SetPos(px,py);
		WideCharToMultiByte   (   CP_ACP,   WC_COMPOSITECHECK,   szBuffer,   -1,   szANSIString,   sizeof(szANSIString),   NULL,   NULL   ); 
		m_pTextEndResult[textNum++]->printfEx(szANSIString);
		py += 22;
	}	
}

//画提示信息
void CGameClientView::DrawFront( CRect *pRect )
{		
}

//画游戏信息
void CGameClientView::DrawGameInfo( CRect *pRect )
{  
} 

//基础分数
void CGameClientView::SetBaseScore(__int64 nBaseScore)
{
	//设置变量
	m_nBaseScore=nBaseScore;
	return;
}

	//语音
void CGameClientView::OnSound()
{
}
void CGameClientView::OnStart()
{
	if(!m_pParent||m_pParent->IsLookonMode())
		return ;
	ResetGameView();	
	if(m_pParent)
		m_pParent->SendMessage(IDM_START,0,0);

	SetButtonShow(IDC_START, TY_BTHIDE|TY_BTUNENABLE);
} 

void CGameClientView::SetGameEnd()
{
}

//////////////////////////////////////////////////////////////////////////
// 左键按下
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{	
	__super::OnLButtonDown(nFlags, point);

	if (m_pBtMgr) m_pBtMgr->SetMLDown(point);

	float x = point.x,
		  y = point.y;

	//m_liziMovie.Start(x,y,25,false);

	//if(m_pParent->IsLookonMode())
	//	return true;
	//调整球的打击点
	float xLength = x - m_ptCenterHitMark.x;
	float yLength = y - m_ptCenterHitMark.y;
	float length = sqrt(xLength*xLength + yLength*yLength);
	//如果亮点之间的距离，小于球的半径，那么这个点落在球内

	if(length<31)
	{
		m_ptHitMark.x = x;
		m_ptHitMark.y = y;
	} 

	//47--31
	//如果在角度的那个区域
	if(length>32&&length<47&&x>m_ptCenterHitMark.x &&y<m_ptCenterHitMark.y )
	{//45
		int length = x - m_ptCenterHitMark.x;
		if(length>45)
			length = 45;
		float rate = float(length)/45.0;
		m_angle = (1-rate)*90;
		if(m_angle>90)
			m_angle = 90;
	}
}

// 左键弹起
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{	
	__super::OnLButtonUp(nFlags, point);

	if (m_pBtMgr) m_pBtMgr->SetMLUp(point);

	float x = point.x,
		  y = point.y;

	if(!m_pParent||m_pParent->m_wCurrentUser!=m_pParent->GetMeChairID()||m_IsGameEnd||(m_pParent->IsLookonMode()&&!this->m_bSingleMode))
		return;

	CRect rc ;
	rc.left = m_offsetx + BAR_WIDTH;
	rc.top = m_offsety + BAR_WIDTH;
	rc.right = rc.left + m_pSpriteLogo->GetWidth() - BAR_WIDTH * 2;
	rc.bottom = rc.top + m_pSpriteLogo->GetHeight() - BAR_WIDTH * 2;
	if(!rc.PtInRect(CPoint(x,y)))
	{
		return;
	}
	if(m_baseBallState==waitSet)
	{
		bool canSet = true;
		for(int i=1;i<BALL_NUM;i++)
		{
			if(m_balls[i].BALL_bEnable==false)
				continue;
			Vector3 temp( m_ptInBallDesk.x-m_offsetx,m_ptInBallDesk.y-m_offsety,0);
			float a = (temp-m_balls[i].Ball_Pos).mag();
			if(a<banjin+10)
			{
				canSet = false;
				break;
			}
		}

		int x = m_ptInBallDesk.x-15;
		int y = m_ptInBallDesk.y-15;
		if(canSet&&x>=m_offsetx+42&&x<m_offsetx+200&&y>m_offsety+38&&y<m_offsety+373)
		{
			m_balls[0].Ball_Pos.X = m_ptInBallDesk.x-m_offsetx;
			m_balls[0].Ball_Pos.Y = m_ptInBallDesk.y-m_offsety;
			m_balls[0].BALL_bEnable = true;
			//如果在洞里面，要放在外面
			float a = (m_physics.Hole[0].pnt - m_balls[0].Ball_Pos).mag();
			if(a<20)
			{
				m_balls[0].Ball_Pos.X += 20;
				m_balls[0].Ball_Pos.Y += 10;
			}
			a = (m_physics.Hole[5].pnt - m_balls[0].Ball_Pos).mag();
			if(a<20)
			{
				m_balls[0].Ball_Pos.X += 20;
				m_balls[0].Ball_Pos.Y -= 10;
			}			
			m_baseBallState = waitHit;
			if(m_bSingleMode==false)
			{
				CMD_S_END_SET_BASEBALL  setBaseBall;
				setBaseBall.ballPosx = m_balls[0].Ball_Pos.X;
				setBaseBall.ballPosy = m_balls[0].Ball_Pos.Y;
				m_pParent->SendSocketData(SUB_S_END_SETBASEBALL,&setBaseBall,sizeof(setBaseBall));
			}
			m_pParent->KillGameClock(IDI_SET_BASEBALL);
			//TRACE("LButtonUp:SendSocketData SUB_S_ENDSETBASEBALL");
		}
		return;
	}
	if(m_baseBallState==waitHit)
	{
		m_baseBallState = startHit;
		m_offsetDistance = 1;	
	}
}


// 鼠标移动
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if (m_pBtMgr) m_pBtMgr->SetMMove(point);

	float x = point.x,
		y = point.y;

	if(m_pParent == NULL || m_pParent->IsLookonMode()&&!this->m_bSingleMode)
		return;
	CRect rc ;
	rc.left = m_offsetx + BAR_WIDTH;
	rc.top = m_offsety + BAR_WIDTH-1;
	rc.right = rc.left + m_pSpriteLogo->GetWidth() - BAR_WIDTH * 2;
	rc.bottom = rc.top + m_pSpriteLogo->GetHeight() - BAR_WIDTH * 2;
	//static CPoint ptMousePos = CPoint(x,y);
	static DWORD tickCount = GetTickCount();
	/*if(rc.PtInRect(CPoint((int)x,int(y)))&&!rc.PtInRect(m_ptMousePos))
	{		
		if((m_baseBallState==waitHit||m_baseBallState==waitSet)&&m_balls[0].BALL_bEnable&&m_balls[0].BALL_State==4)			
		{ 
			if(m_pParent->IsLookonMode()==false&&m_pParent->GetMeChairID()==m_pParent->m_wCurrentUser)
				while(ShowCursor(false)>=0);				 
		}
		else
			while(::ShowCursor(true)<0);
		 
	}
	else if(!rc.PtInRect(CPoint(int(x),int(y)))&&rc.PtInRect(m_ptMousePos))
	{
		 
		while(::ShowCursor(true)<0);
	}*/
	if(rc.PtInRect(CPoint(x,y))&&m_pParent->m_wCurrentUser==m_pParent->GetMeChairID()&&!m_pParent->IsLookonMode()&&(m_baseBallState==waitHit||m_baseBallState==waitSet))
	{
		while(ShowCursor(false)>=0);
	}
	else
	{
		while(ShowCursor(true)<0);
	}
 
	if(m_IsGameEnd)
	{
		while(::ShowCursor(true)<0);
	}
	m_ptMousePos.x = x;
	m_ptMousePos.y = y;	
	if(!m_pParent||m_pParent->m_wCurrentUser!=m_pParent->GetMeChairID())
		return;
	if(m_baseBallState==startHit||m_IsGameEnd)
		return;


	if(rc.PtInRect(CPoint((int)x,int(y))))
	{	
		m_ptInBallDesk.x = x;
		m_ptInBallDesk.y = y;
		if(m_baseBallState==waitSet&&m_pParent&&m_pParent->GetGameStatus()==GS_WK_SET_SETBASEBALL)
		{
			if(m_pParent->m_wCurrentUser==m_pParent->GetMeChairID()&&m_pParent->IsLookonMode()==false)
			{				
				if( GetTickCount()-tickCount>1000&&m_bSingleMode==false)
				{
					tickCount = GetTickCount();
					CMD_S_SET_BASEBALL  setBaseBall;
					setBaseBall.mousex = x-m_offsetx;
					setBaseBall.mousey = y-m_offsety;
					m_pParent->SendSocketData(SUB_S_SETBASEBALL,&setBaseBall,sizeof(setBaseBall));
				}
			}
		}		
		/*else
		{*/
			m_ptInBallDesk.x = x;
			m_ptInBallDesk.y = y;
		//}
	}	

	SHORT state = GetKeyState(VK_LBUTTON);
	rc.left = m_ptHitMark.x - 6;
	rc.top = m_ptHitMark.y - 6;
	rc.bottom = m_ptHitMark.y + 6;
	rc.right = m_ptHitMark.x + 6;
	float xLength = x - m_ptCenterHitMark.x;
	float yLength = y - m_ptCenterHitMark.y;
	float length = sqrt(xLength*xLength + yLength*yLength);
	//如果亮点之间的距离，小于球的半径，那么这个点落在球内
	if(length<31&&HIWORD(state)&&rc.PtInRect(m_ptMousePos))
	{
		m_ptHitMark = m_ptMousePos;
	}	
	//47--31
	//如果在角度的那个区域
	if(length>32&&length<47&&x>m_ptCenterHitMark.x &&y<m_ptCenterHitMark.y&&HIWORD(state))
	{//45
		int length = x - m_ptCenterHitMark.x;
		if(length>45)
			length = 45;
		float rate = float(length)/45.0;
		m_angle = (1-rate)*90;
		if(m_angle>90)
			m_angle = 90;
	}
} 


// 大小
void CGameClientView::OnSize(UINT nType, int cx, int cy)
{	
	__super::OnSize(nType, cx, cy);
	
	if (cx > 0x71 &&
		cy > 0x71 &&
		m_pHge)
	{
		int y = (m_pParent != NULL) ? /*m_pParent->GetYExcursionPos()*/0 : 29;	// 默认值根据需要改
		::MoveWindow(m_pHge->System_GetState(HGE_HWND), 9, 29, cx, cy, false);
		//m_pHge->_Resize(cx, cy);	
	}
}

 
//往字符串后加空格
void CGameClientView::FillStringWithSpace( TCHAR str[], int nLen )
{
	ASSERT( nLen < 128 );
	if( nLen > 128 ) 
		return;

	int pos1 = 0;
	if( _tcsclen(str) < nLen )
		pos1 = (nLen - _tcsclen(str))/2;
	TCHAR szBuffer[128];
	for( int i = 0, j = 0; i < nLen; i++ )
	{
		if( i < pos1 )
			szBuffer[i] = ' ';
		else if( i >= _tcsclen(str) + pos1 )
			szBuffer[i] = ' ';
		else
			szBuffer[i] = str[j++];
	}
	_sntprintf( str, nLen-1, TEXT("%s"), szBuffer );
	str[nLen-1] = '\0';	
}
//---------------------------------------------------------------------------------------------//
//动画结束的消息通知
LRESULT CGameClientView::OnMovieEnd(WPARAM wParam,LPARAM lParam)
{
	if(this->m_IsGameEnd)
		return 0;
	CPoint ptStart ;
	CPoint ptEnd;
	ptStart.x = m_offsetx + (m_pSpriteLogo->GetWidth()-m_pSpriteObverse->GetWidth())/2;
	ptStart.y = m_offsety + (m_pSpriteLogo->GetHeight()-m_pSpriteObverse->GetHeight())/2;
	ptEnd = ptStart;
	ptEnd.y -= 150;
	if(wParam==DWORD(&m_coinMovieObverse))
	{//显示正面开球
		m_movieObverse.StartMovie(ptStart,ptEnd,2000,500);
		m_baseBallState = waitSet;
		m_pParent->SetGameClock(m_pParent->m_wCurrentUser,IDI_SET_BASEBALL,TIME_SET_BASEBALL/1000);
		TCHAR szBuffer[1024];
		 tagUserInfo * pUserData=GetClientUserItem(m_pParent->SwitchViewChairID(m_pParent->m_wCurrentUser)) ==NULL ? NULL:GetClientUserItem(m_pParent->SwitchViewChairID(m_pParent->m_wCurrentUser))->GetUserInfo();	
		_sntprintf(szBuffer,1024,TEXT("%s "),pUserData->szNickName);
		CString str;
		str.Format(TEXT("%s放置母球"),szBuffer);
		m_systemInfoList.AddTail(str);
		//发送消息
	}
	if(wParam==DWORD(&m_coinMovieReverse))
	{//显示反面开球
		m_movieReverse.StartMovie(ptStart,ptEnd,2000,500);
		m_baseBallState = waitSet;
		m_pParent->SetGameClock(m_pParent->m_wCurrentUser,IDI_SET_BASEBALL,TIME_SET_BASEBALL/1000);
		TCHAR szBuffer[1024];
		tagUserInfo * pUserData=GetClientUserItem(m_pParent->SwitchViewChairID(m_pParent->m_wCurrentUser))==NULL?NULL:GetClientUserItem(m_pParent->SwitchViewChairID(m_pParent->m_wCurrentUser))->GetUserInfo();	
		_sntprintf(szBuffer,1024,TEXT("%s "),pUserData->szNickName);
		CString str;
		str.Format(TEXT("%s放置母球"),szBuffer);
		m_systemInfoList.AddTail(str);
		//发送消息
	}
	if(wParam==DWORD(&m_movieGameStart))
	{		
		int x = m_offsetx + (m_pSpriteLogo->GetWidth() - m_pSpriteCoinObverse[0]->GetWidth()/72)/2;
		int y = m_offsety + (m_pSpriteLogo->GetHeight() - m_pSpriteCoinObverse[0]->GetHeight())/2;	
		if(m_pParent->m_wCurrentUser==0)
		{//1p开球,正面
			m_coinMovieObverse.Start(x,y,25,false);
			m_coinMovieObverse.SetDisappearTime(1000);
		}
		else
		{//2p开球
			m_coinMovieReverse.Start(x,y,25,false);
			m_coinMovieReverse.SetDisappearTime(1000);
		}
		m_baseBallState = nothing;	
		PlayGameSound(IDS_COIN);	
	}
	return 1;
}

 
 
 
 
void CGameClientView::ResetBall()
{
	
	 float yups =BALLDiameter*0.9160256f;  //*3BALLDIAMETER* _/-3/2
	m_balls[0].Ball_Pos=Vector3(222,244) ;
	m_balls[0].Clear();

	FLOAT bp8[][3]={{0,0,0}
	,{0,0,1},
	{yups,banjin,4},{yups,-banjin,9},
	{yups*2,banjin*2,7},{yups*2,0,8},{yups*2,-banjin*2,3}
	,{yups*3,banjin*3,2},{yups*3,banjin,6},{yups*3,-banjin,5},{yups*3,-banjin*3,10}
	,{yups*4,banjin*4,15},{yups*4,banjin*2,13},{yups*4,0,11},{yups*4,-banjin*2,14},{yups*4,-banjin*4,12}};

	m_balls[0].Init(0,Vector3(222,244,0));
	for(int i=1;i<BALL_NUM;i++)
		m_balls[(int)bp8[i][2]].Init((int)bp8[i][2],Vector3(bp8[i][0]+577,bp8[i][1]+222,0));	
	srand( GetTickCount());
	for (int i=1;i<BALL_NUM;i++)
	{
		m_balls[i].Clear();
		Vector3  ran=NormaliseVector(Vector3(ranged_random(.1f,2),ranged_random(.1f,2),ranged_random(.1f,2)));
		m_balls[i].BALL_Orientation= rotation_matrix (ranged_random(1,800), ran);	// 设置每个球的随机方位
		m_balls[i].SetOffSet(m_offsetx,m_offsety);		
	}
}

//得到球杆的长度
int CGameClientView:: GetBallClubLength()
{
	 

	//x = m_offsetx;
	//x = m_offsetx +　m_pSpriteLogo->GetHeight();
	//y = m_offsety;
	//y = m_offsety + m_pSpriteLogo->GetHeight();

	//y = y1 + ((x-x2)/(x2-x1)) * (y2-y1);

	float width = m_pSpriteLogo->GetWidth() + m_offsetx - 28;
	float height = m_pSpriteLogo->GetHeight() + m_offsety;
	float clubLenght = m_pSpriteClub->GetWidth();
	float x1 = m_ptInBallDesk.x;
	float y1 = m_ptInBallDesk.y;
	float x2 = m_balls[0].Ball_Pos.X + m_offsetx;
	float y2 = m_balls[0].Ball_Pos.Y + m_offsety;
	int length = 0;
	if(abs(x1-x2)<5)
	{
		if(y1>y2)
		{
			length = abs(y2-(m_offsety));
		}
		else
		{
			length =  abs(height - y2);
		}
		return length<clubLenght?length:clubLenght;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}

	if(abs(y1-y2)<5)
	{
		if(x1>x2)
			length =  abs(x2-(m_offsetx+14));
		else 
			length = abs(width-x2);
		return length<clubLenght?length:clubLenght;;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}
	//左边
	float x = m_offsetx + 14;
	float y = y1 + ((x-x1)/(x2-x1)) * (y2-y1);
	if(x1>x2&&y>=m_offsety&&y<=height)
	{
		length = sqrt((x2-x)*(x2-x)+(y-y2)*(y-y2));
		return length<clubLenght?length:clubLenght;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}
	//下面
	y = height;
	x = ((y-y1)/(y2-y1))*(x2-x1) + x1;
	if(y2>y1&&x>m_offsetx&&x<width)
	{
		length = sqrt((x2-x)*(x2-x)+(y-y2)*(y-y2));
		return length<clubLenght?length:clubLenght;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}
	//右面;;
	//x = m_offsetx + m_pSpriteLogo->GetHeight();
	x = width;
	y = y1 + ((x-x1)/(x2-x1)) * (y2-y1);
	if(x1<x2&&y>=m_offsety&&y<=height)
	{
		length = sqrt((x2-x)*(x2-x)+(y-y2)*(y-y2));
		return length<clubLenght?length:clubLenght;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}
	//上面
	y = m_offsety ;
    x = ((y-y1)/(y2-y1))*(x2-x1) + x1;
	if(y2<y1&&x>m_offsetx&&x<width)
	{
		length = sqrt((x2-x)*(x2-x)+(y-y2)*(y-y2));
		return length<clubLenght?length:clubLenght;
		length = clubLenght - length ;
		return length>0?length:clubLenght;
	}
	ASSERT(0);
	return clubLenght;
}
//初始化在下面显示每个人没被打进去的球，leftLitterNum：左边是小球否
void CGameClientView::InitVisibleBall(bool leftLitterNum,bool visible)
{
 
	if(leftLitterNum)
	{
		for(int i=1;i<BALL_NUM;i++)
		{
			if(i<9)
				m_visibleBalls[i].Init(i,Vector3(36+(i-1)*28,610,0));	 	 
			else
				m_visibleBalls[i].Init(i,Vector3(300+(i-1)*28,610,0));	 
			m_visibleBalls[i].BALL_bEnable = visible;
		}
		//m_visibleBalls[0].Init(8,Vector3(301+15*26,610,0));	
		//m_visibleBalls[0].BALL_bEnable = false;
	}
	else
	{
		for(int i=15;i>0;i--)
		{
			if(i>8)
				m_visibleBalls[i].Init(i,Vector3(36+(15-i)*28,610,0));	 	 
			else if(i<8)
				m_visibleBalls[i].Init(i,Vector3(300+(15-i)*28,610,0));	
			m_visibleBalls[i].BALL_bEnable = visible;
		}
		//m_visibleBalls[0].Init(8,Vector3(301+15*26,610,0));	
		//m_visibleBalls[0].BALL_bEnable = false;
	}
	m_visibleBalls[8].BALL_bEnable = false;

	for (int i=1;i<BALL_NUM;i++)
	{	 
		Vector3  ran=NormaliseVector(Vector3(0,0,0));
		m_visibleBalls[i].BALL_Orientation= rotation_matrix (0, ran);	// 设置每个球的随机方位
		m_visibleBalls[i].SetOffSet(m_offsetx,m_offsety);		
	}
}
	//设置游戏开始
void CGameClientView::SetGameStart()
{
	ResetGameView();
	int x = m_offsetx + (m_pSpriteLogo->GetWidth()-m_pSpriteGameStart->GetWidth())/2;
	int y = m_offsety + (m_pSpriteLogo->GetHeight()-m_pSpriteGameStart->GetHeight())/2;
	CPoint ptStart(x,y);
	CPoint ptEnd(x,y-150);;
	m_movieGameStart.ShowNum(m_roundCount,CPoint(160,80));
	m_movieGameStart.StartMovie(ptStart,ptEnd,2000,500);
	m_bSingleMode = false;
	m_offsetDistance = 0;
	m_bHit = false;
	CString str;
	str.Format(TEXT("游戏开始"));
	m_systemInfoList.AddTail(str);
	m_baseBallState = nothing;
	if(m_pParent->IsLookonMode()==false)
	{
		SetButtonShow(IDC_GIVEPU, TY_BTSHOW|TY_BTENABLE);
		SetButtonShow(IDC_REBEGIN, TY_BTHIDE|TY_BTUNENABLE);
	}
}
	//设置进球
void CGameClientView::SetHitBallInHole(int ballNum)
{
	if(m_bSingleMode)
		return ;
	//检测是不是第一个进球
	bool firstInBall = m_bFirstInBall;
	if(m_bFirstInBall)
	{
		m_bFirstInBall = false;
		BYTE me = m_pParent->GetMeChairID();
		BYTE next = (m_pParent->GetMeChairID()+1)%2;
		if(ballNum>8)
		{//如果进的球是大球
			if(m_pParent->m_wCurrentUser==m_pParent->GetMeChairID())
			{//如果是我自己进球
					m_userBallType[me] = 1;
					m_userBallType[next] = 0;
				if(m_pParent->GetMeChairID()==1)
				{//如果我是2P
					InitVisibleBall(true,true);				
				}
				else
				{
					InitVisibleBall(false,true);

				}
			}
			else
			{
					m_userBallType[me] = 0;
					m_userBallType[next] = 1;
				if(m_pParent->GetMeChairID()==1)
				{//如果我是2P,我是小球
					InitVisibleBall(false,true);
				}
				else
				{
					InitVisibleBall(true,true);
				}
			}
		}
		else
		{//如果进的是小球			 
			if(m_pParent->m_wCurrentUser==m_pParent->GetMeChairID())
			{//如果是我自己进球
				m_userBallType[me] = 0;
				m_userBallType[next] = 1;
				if(m_pParent->GetMeChairID()==1)
				{//如果我是2P
					InitVisibleBall(false,true);
				}
				else
				{
					InitVisibleBall(true,true);
				}
			}
			else
			{
				m_userBallType[me] = 1;
				m_userBallType[next] = 0;
				if(m_pParent->GetMeChairID()==1)
				{//如果我是1P,我是 大球
					InitVisibleBall(true,true);
				}
				else
				{
					InitVisibleBall(false,true);
				}
			}
		}		
	}
	m_visibleBalls[ballNum].BALL_bEnable = false;
	bool bRightIn = false;
	BYTE firstHitBall = m_physics.m_wFistHitBall;

	if(ballNum>8&&m_userBallType[m_pParent->m_wCurrentUser]==1&&firstHitBall>8||ballNum<8&&m_userBallType[m_pParent->m_wCurrentUser]==0&&firstHitBall<8)
	{
		bRightIn = true;
	}
	if(ballNum==0)
	{
		bRightIn = false;
	}
	BYTE inholeNum = 0;
	if(m_userBallType[m_pParent->m_wCurrentUser]==1)
	{
		for(int i=9;i<16;i++)
			if(m_visibleBalls[i].BALL_bEnable==false)
				inholeNum ++;
	}
	else if(m_userBallType[m_pParent->m_wCurrentUser==0])
	{
		for(int i=1;i<8;i++)
		{
			if(m_visibleBalls[i].BALL_bEnable==false)
				inholeNum++;
		}
	}
	if(firstInBall)
	{
		bRightIn = true;
	}
	if(ballNum==8)
	{		
		if(inholeNum==7&&firstHitBall==8)
			bRightIn = true;
	}
	//如果之前有白球进去了
	if(m_balls[0].BALL_bEnable==false)
	{
		bRightIn = false;
	}
	//如果之前有黑8进去了
	if(m_balls[8].BALL_bEnable==false&&inholeNum!=7)
	{
		bRightIn = false;
	}
	if(bRightIn)
	{
		PlayGameSound(IDS_GOOD);
	}
}
//拆分字符串
int CGameClientView::SplitString(CString &str,int _splitLen)
{//每一行都是一个text
	if(_splitLen<=0)
		return 1;
	if(str.GetLength()<=_splitLen)
		return 1;

	//GetWidthByChar();

	int num = 0;
	int splitLen = _splitLen;
	//处理宽字符
	int i = 0;
	for(i=0;i<splitLen;i++)
	{
		if(str[i]&0XF000)
			i++;
	}
	//str[i] = '\0';	
	splitLen = i;

	CString strTemp = str.Left(splitLen);
	strTemp += "\n";
	num ++;
	CString strleft = str.Right(str.GetLength()-splitLen);
	while(strleft.GetLength()>_splitLen)
	{
		//处理宽字符
		for(int i=0;i<splitLen;i++)
		{
			if(str[i]&0XF000)
				i++;
		}

		splitLen = i;
		CString temp = strleft.Left(splitLen);
		temp += "\n";
		strTemp += temp;
		strleft = strleft.Right(strleft.GetLength()-splitLen);
		num ++;
	}
	if(strleft.IsEmpty()==false)
	{
		strleft += "\n";
		strTemp += strleft;
		str = strTemp;
		num ++;
	}
	return num;
}
//void 对方移动球杆
void CGameClientView::SetBallClubMove( PT pt)
{
	m_ptBallClub = pt;
	m_ptBallClub.x += m_offsetx;
	m_ptBallClub.y += m_offsety;
	m_bMoveBallClub = true;
}