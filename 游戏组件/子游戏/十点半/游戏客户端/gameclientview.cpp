#include "StdAfx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include "DlgInfomation.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 

//控制按钮
#define IDC_START						100								//开始按钮
#define IDC_GIVECARD					102								//跟注
#define IDC_STOPCARD					106								//放弃
#define IDC_ADDSCORE					107								//加注
#define IDC_ONESCORE					108								//一倍
#define IDC_TWOSCORE					109								//两倍
#define IDC_THREESCORE					110								//三倍
#define IDC_FOURSCORE					111								//四倍

#define IDI_MOVE_JETTON					100								//移动筹码定时器
#define IDI_MOVE_NUMBER					101								//滚动数字
#define IDI_SEND_CARD					102								//发牌标识
#define IDI_HIDE_CARD					103								//隐藏扑克
#define IDI_USER_ACTION					104								//动作标识
#define IDI_ACTION_ADD_JETTON			105								//筹码添加信息

#define TIME_MOVE_JETTON				20								//移动筹码时间
#define TIME_MOVE_NUMBER				50								//滚动数字时间
#define TIME_ACTION_ADD_JETTON			2000							//筹码添加信息时间

//发牌定义
#define SEND_STEP_COUNT					5								//步数
#define SPEED_SEND_CARD					10								//发牌速度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	//系统消息
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()

	//按钮消息
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_GIVECARD,OnBnClickedGiveCard)
	ON_BN_CLICKED(IDC_STOPCARD,OnBnClickedStopCard)
	ON_BN_CLICKED(IDC_ADDSCORE,OnBnClickedAddScore)
	ON_BN_CLICKED(IDC_ONESCORE,OnBnClickedOneScore)
	ON_BN_CLICKED(IDC_TWOSCORE,OnBnClickedTwoScore)
	ON_BN_CLICKED(IDC_THREESCORE,OnBnClickedThreeScore)
	ON_BN_CLICKED(IDC_FOURSCORE,OnBnClickedFourScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//状态变量
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserAddScore,sizeof(m_lUserAddScore));
	m_lCellScore = 0L;
	m_lJettonScore = 0L;
	m_wBankerUser = INVALID_CHAIR;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
		m_fUserCardScore[i] = -1;
	m_bWaitUserScore = false;
	m_bShowAddJetton = true;

	//动画变量
	m_SendCardPos.SetPoint(0,0);
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = SEND_STEP_COUNT;
	m_nXStep = 0;
	m_nYStep = 0;

	//动作变量
	m_wActionUser=INVALID_CHAIR;
	m_cbUserAction=0;

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageGameCard.LoadFromResource(hResInstance,IDB_GAME_CARD);
	m_ImageViewFill.LoadFromResource(hResInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadFromResource(hResInstance,IDB_VIEW_BACK);
	m_PngNumber.LoadImage(hResInstance,TEXT("CARD_SCORE_NUM"));
	m_PngScoreNum.LoadImage(hResInstance,TEXT("SCORE_NUM"));
	m_PngActionBack.LoadImage(hResInstance,TEXT("ACTION_BACK"));
	m_PngActionFont.LoadImage(hResInstance,TEXT("ACTION_FONT"));
	m_PngActionScoreBack.LoadImage( hResInstance,TEXT("ACTION_SCORE_BACK") );
	m_PngActionScoreNum.LoadImage( hResInstance,TEXT("ACTION_SCORE_NUM") );
	m_PngBkTopLeft.LoadImage( hResInstance,TEXT("BK_TOPLEFT") );
	m_PngBkTopRight.LoadImage( hResInstance,TEXT("BK_TOPRIGHT") );
	m_PngBkBottomLeft.LoadImage( hResInstance,TEXT("BK_BOTTOMLEFT") );
	m_PngBkBottomRight.LoadImage( hResInstance,TEXT("BK_BOTTOMRIGHT") );
	m_ImageTitle.LoadFromResource(hResInstance,IDB_TITLE);
	m_PngCardScore.LoadImage(hResInstance,TEXT("CARD_SCORE"));
	m_PngBanker.LoadImage(hResInstance,TEXT("BANKER_FLAG"));
	m_PngWaitScore.LoadImage(hResInstance,TEXT("WAIT_USER_SCORE"));

	//获取大小
	m_SizeGameCard.SetSize(m_ImageGameCard.GetWidth()/13,m_ImageGameCard.GetHeight()/5);

	m_nXFace=48;
	m_nYFace=48;
	m_nXTimer=65;
	m_nYTimer=69;

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
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

//重置界面
VOID CGameClientView::ResetGameView()
{
	//状态变量
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserAddScore,sizeof(m_lUserAddScore));
	m_lCellScore = 0L;
	m_lJettonScore = 0L;
	m_wBankerUser = INVALID_CHAIR;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
		m_fUserCardScore[i] = -1;
	m_bWaitUserScore = false;
	m_bShowAddJetton = true;

	//动画变量
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = 0;
	m_nXStep = 0;
	m_nYStep = 0;
	m_SendCardItemArray.RemoveAll();

	//动作变量
	m_wActionUser=INVALID_CHAIR;
	m_cbUserAction=0;

	//控制按钮
	m_btStart.ShowWindow(SW_HIDE);
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//准备位置
	m_ptReady[0].x = nWidth/2;
	m_ptReady[0].y = 200;
	m_ptReady[1].x = 220;
	m_ptReady[1].y = nHeight/2-4;
	m_ptReady[2].x = nWidth/2;
	m_ptReady[2].y = nHeight-200;
	m_ptReady[3].x = nWidth-200;
	m_ptReady[3].y = nHeight/2-4;

	//头像位置
	m_ptAvatar[0].x = nWidth/2-m_nXFace/2;
	m_ptAvatar[0].y = 5;
	m_ptAvatar[1].x = 5;
	m_ptAvatar[1].y = nHeight/2-85;
	m_ptAvatar[2].x = nWidth/2-m_nXFace/2;
	m_ptAvatar[2].y = nHeight-5-m_nYFace;
	m_ptAvatar[3].x = nWidth-5-m_nXFace;
	m_ptAvatar[3].y = nHeight/2-85;

	//时间位置
	m_ptClock[0].x = nWidth/2-m_nXFace-55;
	m_ptClock[0].y = 30;
	m_ptClock[1].x = 30;
	m_ptClock[1].y = nHeight/2-145;
	m_ptClock[2].x = nWidth/2-m_nXFace-100;
	m_ptClock[2].y = nHeight-30;
	m_ptClock[3].x = nWidth-30;
	m_ptClock[3].y = nHeight/2-145;

	m_ptBanker[0].SetPoint( nWidth/2-m_nXFace-25,10 );
	m_ptBanker[1].SetPoint( 15,nHeight/2-115 );
	m_ptBanker[2].SetPoint( nWidth/2-m_nXFace-30,nHeight-30 );
	m_ptBanker[3].SetPoint( nWidth-35,nHeight/2-115 );

	//得分数字位置
	m_ptJettons[0].SetPoint(nWidth/2+m_nXFace/2+30,35);
	m_ptJettons[1].SetPoint(75,nHeight/2-85+15);
	m_ptJettons[2].SetPoint(nWidth/2-m_nXFace/2+65,nHeight-5-m_nYFace-5);
	m_ptJettons[3].SetPoint(nWidth-5-m_nXFace-40,nHeight/2-85+10);

	//动作位置
	m_ptUserAction[0].SetPoint(nWidth/2,180);
	m_ptUserAction[1].SetPoint(+50,nHeight/2-120);
	m_ptUserAction[2].SetPoint(nWidth/2,nHeight-180);
	m_ptUserAction[3].SetPoint(nWidth-50,nHeight/2-120);

	//扑克控件
	m_CardControl[0].SetBenchmarkPos(nWidth/2,m_nYFace+10,enXCenter,enYTop);
	m_CardControl[1].SetBenchmarkPos(5,nHeight/2+26,enXLeft,enYCenter);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-10,enXCenter,enYBottom);
	m_CardControl[3].SetBenchmarkPos(nWidth-5,nHeight/2+26,enXRight,enYCenter);

	//筹码控件
	m_JettonControl.SetBenchmarkPos(nWidth/2,nHeight/2-30);

	//数字控件位置
	m_NumberControl[0].SetBencbmarkPos(nWidth/2,170,enXCenter);
	m_NumberControl[1].SetBencbmarkPos(155,nHeight/2-80,enXLeft);
	m_NumberControl[2].SetBencbmarkPos(nWidth/2,nHeight-195,enXCenter);
	m_NumberControl[3].SetBencbmarkPos(nWidth-155,nHeight/2-80,enXRight);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	
	CRect rcButton;
	m_btStart.GetWindowRect(&rcButton);

	//开始按钮
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-rcButton.Width()/2,nHeight-250,0,0,uFlags);

	m_btOneScore.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-rcButton.Width()*2-15,nHeight-200,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-rcButton.Width()-5,nHeight-200,0,0,uFlags);
	DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2+5,nHeight-200,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFourScore,NULL,nWidth/2+rcButton.Width()+15,nHeight-200,0,0,uFlags);

	m_btGiveCard.GetWindowRect(&rcButton);
	//要牌按钮
	DeferWindowPos(hDwp,m_btGiveCard,NULL,nWidth/2+170,nHeight-150,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStopCard,NULL,nWidth/2+170,nHeight-150+rcButton.Height()+5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddScore,NULL,nWidth/2+170,nHeight-150+rcButton.Height()*2+10,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//发牌起始位置
	m_SendCardPos.SetPoint(nWidth/2-48,nHeight/2-48);

	//SetFlowerControlInfo( 0,m_ptAvatar[0].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[0].y+m_nYFace );
	//SetFlowerControlInfo( 1,m_ptAvatar[1].x+m_nXFace,m_ptAvatar[1].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );
	//SetFlowerControlInfo( 2,m_ptAvatar[2].x+m_nXFace/2-BIG_FACE_WIDTH/2,m_ptAvatar[2].y-BIG_FACE_HEIGHT );
	//SetFlowerControlInfo( 3,m_ptAvatar[3].x-BIG_FACE_WIDTH,m_ptAvatar[3].y+m_nYFace/2-BIG_FACE_HEIGHT/2 );

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);
	m_ImageTitle.BitBlt(pDC->m_hDC,80,2);
	//四角
	m_PngBkTopLeft.DrawImage( pDC,0,0 );
	m_PngBkTopRight.DrawImage( pDC,nWidth-m_PngBkTopRight.GetWidth(),0 );
	m_PngBkBottomLeft.DrawImage( pDC,0,nHeight-m_PngBkBottomLeft.GetHeight() );
	m_PngBkBottomRight.DrawImage( pDC,nWidth-m_PngBkBottomRight.GetWidth(),nHeight-m_PngBkBottomRight.GetHeight() );

	//单元注数
	if( m_lCellScore > 0 )
		DrawNumberString( pDC,m_lCellScore,195,23,m_PngScoreNum,false );

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pClientUserItem=GetClientUserItem(i);
		//test
		//tagUserData *pClientUserItem = new tagUserData;
		//lstrcpy( pClientUserItem->GetNickName(),TEXT("SSSSSSSSkk") );
		//end test
		if (pClientUserItem==NULL) continue;

		//绘画属性
		WORD wTime = GetUserClock(i);
		//test
		//wTime = 30;
		//end test
		if( wTime > 0 )
			DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wTime);

		//用户名字
		pDC->SetTextAlign((i==3||i==4)?TA_RIGHT:TA_LEFT);
		DrawTextString(pDC,pClientUserItem->GetNickName(),RGB(255,255,255),RGB(0,0,0),m_ptAvatar[i].x+((i==3||i==4)?-5:50),m_ptAvatar[i].y+2);

		//用户头像
		DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pClientUserItem);

		//用户积分
		TCHAR szScore[64];
		LONGLONG lScore = m_lUserAddScore[i]==0?pClientUserItem->GetUserScore():(pClientUserItem->GetUserScore()-m_lUserAddScore[i]);
		_sntprintf(szScore,CountArray(szScore),TEXT("%I64d"),lScore);
		DrawTextString(pDC,szScore,RGB(255,255,255),RGB(0,0,0),m_ptAvatar[i].x+((i==3||i==4)?-5:50),m_ptAvatar[i].y+12*1+10);

		//庄家
		//test
		//m_wBankerUser = i;
		//end test
		if( m_wBankerUser == i )
		{
			m_PngBanker.DrawImage( pDC,m_ptBanker[i].x,m_ptBanker[i].y );
		}
		
		//test
		//delete pClientUserItem;
		//end test
	}

	//等待玩家下注
	if( m_bWaitUserScore )
	{
		m_PngWaitScore.DrawImage( pDC,(nWidth-m_PngWaitScore.GetWidth())/2,(nHeight-m_PngWaitScore.GetHeight())/2 );
	}

	//用户扑克			
	for (BYTE i=0;i<GAME_PLAYER;i++) 
	{
		m_CardControl[i].DrawCardControl(pDC);

		//用户准备
		IClientUserItem * pClientUserItem=GetClientUserItem(i);
		if (pClientUserItem==NULL) continue;
		//同意标志
		BYTE cbUserStatus=pClientUserItem->GetUserStatus();
		//test
		//cbUserStatus = US_READY;
		//end test
		if (cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
	}
	
	//绘画筹码
	m_JettonControl.DrawJettonControl(pDC);

	LONGLONG lJettonScore = (LONGLONG)m_JettonControl.GetScore();
	//test
	//lJettonScore = 1234L;
	//end test
	//绘画筹码总数
	if( lJettonScore > 0 )
	{
		DrawNumberString( pDC,lJettonScore,195,50,m_PngScoreNum,false );
	}

	//筹码添加信息
	if( m_bShowAddJetton && m_lJettonScore < lJettonScore )
	{
		m_PngActionScoreBack.DrawImage( pDC,nWidth/2-m_PngActionScoreBack.GetWidth()/2,
			nHeight/2-m_PngActionScoreNum.GetHeight()/2-90 );
		DrawNumberString( pDC,lJettonScore-m_lJettonScore,nWidth/2,nHeight/2-75,m_PngActionScoreNum,true );
	}

	//得分数字滚动动画
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_NumberControl[i].DrawNumberControl(pDC);
	}

	//牌面点数
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_fUserCardScore[i] != -1 )
		{
			INT nXPos,nYPos;
			if( i <= 2 )
			{
				CPoint pt = m_CardControl[i].GetTailPos();
				nXPos = pt.x + 64;
				nYPos = pt.y;
			}
			else
			{
				CPoint pt = m_CardControl[i].GetHeadPos();
				nXPos = pt.x-8;
				nYPos = pt.y;
			}
			
			DrawCardScore( pDC,nXPos,nYPos,m_fUserCardScore[i],i<=2?true:false );
		}
	}

	//绘画发牌扑克
	if (m_SendCardItemArray.GetCount()>0)
	{
		//变量定义
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//获取大小
		INT nItemWidth=m_SizeGameCard.cx;
		INT nItemHeight=m_SizeGameCard.cy;

		//绘画扑克
		m_ImageGameCard.TransDrawImage(pDC,m_SendCardCurPos.x,m_SendCardCurPos.y,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4,RGB(255,0,255));
	}

	//用户动作
	if (m_wActionUser!=INVALID_CHAIR)
	{
		//计算位置
		INT nXPos=m_ptUserAction[m_wActionUser].x-m_PngActionBack.GetWidth()/8;
		INT nYPos=m_ptUserAction[m_wActionUser].y-m_PngActionBack.GetHeight()/2;

		//绘画背景
		INT nImageIndex = m_wActionUser;
		m_PngActionBack.DrawImage(pDC,nXPos,nYPos,m_PngActionBack.GetWidth()/4,m_PngActionBack.GetHeight(),
			nImageIndex*m_PngActionBack.GetWidth()/4,0);

		//绘画动作
		m_PngActionFont.DrawImage(pDC,nXPos+10,nYPos+25,m_PngActionFont.GetWidth()/3,m_PngActionFont.GetHeight(),
			(m_cbUserAction-1)*m_PngActionFont.GetWidth()/3,0);
	}

	return;
}

//开始按钮
VOID CGameClientView::OnBnClickedStart()
{
	//发送消息
	SendEngineMessage(IDM_START,0,0);

	return;
}

//要牌
VOID CGameClientView::OnBnClickedGiveCard()
{
	//发送消息
	SendEngineMessage(IDM_GIVE_CARD,0,0);

	return;
}

//停牌
VOID CGameClientView::OnBnClickedStopCard()
{
	//发送消息
	SendEngineMessage(IDM_STOP_CARD,0,0);

	return;
}

//
VOID CGameClientView::OnBnClickedAddScore()
{
	//发送消息
	SendEngineMessage(IDM_ADD_SCORE,0,0);

	return;
}

//1倍底注
VOID CGameClientView::OnBnClickedOneScore()
{
	//发送消息
	SendEngineMessage(IDM_USER_SCORE,1,0);

	return;
}

//2倍底注
VOID CGameClientView::OnBnClickedTwoScore()
{
	//发送消息
	SendEngineMessage(IDM_USER_SCORE,2,0);

	return;
}

//3倍底注
VOID CGameClientView::OnBnClickedThreeScore()
{
	//发送消息
	SendEngineMessage(IDM_USER_SCORE,3,0);

	return;
}

//4倍底注
VOID CGameClientView::OnBnClickedFourScore()
{
	//发送消息
	SendEngineMessage(IDM_USER_SCORE,4,0);

	return;
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//变量定义
	CRect rcCreate(0,0,0,0);

	//控制按钮
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btGiveCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_GIVECARD);
	m_btStopCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_STOPCARD);
	m_btAddScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_ADDSCORE);
	m_btOneScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_ONESCORE);
	m_btTwoScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_TWOSCORE);
	m_btThreeScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_THREESCORE);
	m_btFourScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_FOURSCORE);

	//控制按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_START,hResInstance,false,false);
	m_btGiveCard.SetButtonImage(IDB_BT_GIVECARD,hResInstance,false,false);
	m_btStopCard.SetButtonImage(IDB_BT_STOPCARD,hResInstance,false,false);
	m_btAddScore.SetButtonImage(IDB_BT_ADDSCORE,hResInstance,false,false);
	m_btOneScore.SetButtonImage(IDB_BT_SCORE,hResInstance,false,false);
	m_btTwoScore.SetButtonImage(IDB_BT_SCORE,hResInstance,false,false);
	m_btThreeScore.SetButtonImage(IDB_BT_SCORE,hResInstance,false,false);
	m_btFourScore.SetButtonImage(IDB_BT_SCORE,hResInstance,false,false);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btStart,TEXT("开始游戏"));
	m_ToolTipCtrl.AddTool(&m_btGiveCard,TEXT("要牌"));
	m_ToolTipCtrl.AddTool(&m_btStopCard,TEXT("停牌"));
	m_ToolTipCtrl.AddTool(&m_btAddScore,TEXT("加倍"));

	//test
	//m_btStart.ShowWindow( SW_SHOW );
	//m_btGiveCard.ShowWindow( SW_SHOW );
	//m_btStopCard.ShowWindow( SW_SHOW );
	//m_btAddScore.ShowWindow( SW_SHOW );
	//m_btOneScore.ShowWindow( SW_SHOW );
	//m_btTwoScore.ShowWindow( SW_SHOW );
	//m_btThreeScore.ShowWindow( SW_SHOW );
	//m_btFourScore.ShowWindow( SW_SHOW );
	//BYTE byCard[] = {
	//	0x1b,0x0B,0x0C,0x0D,0x0d,0x1b,0x0B,0x01,0x4e
	//};
	//CGameLogic gm;
	//for( WORD i = 0; i < GAME_PLAYER; i++ )
	//{
	//	m_CardControl[i].SetCardData( byCard,9 );
	//	FLOAT fCardScore = gm.GetCardGenre(byCard,9);
	//	SetCardScore( i,fCardScore );
	//	//m_JettonControl[i]
	//}
	//m_lCellScore = 1L;
	//SetUserAction( 2,AC_ADD_SCORE );
	//end test

	return 0;
}

//是否发牌
bool CGameClientView::IsDispatchStatus()
{
	return m_SendCardItemArray.GetCount()>0;
}

//停止发牌
VOID CGameClientView::ConcludeDispatch()
{
	//完成判断
	if (m_SendCardItemArray.GetCount()==0) return;

	//完成动画
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//获取数据
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//设置扑克
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	//发送消息
	SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

	//删除处理
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	//更新界面
	RefreshGameView();

	return;
}

//发送扑克
VOID CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	tagSendCardItem SendCardItem;
	SendCardItem.wChairID=wChairID;
	SendCardItem.cbCardData=cbCardData;
	m_SendCardItemArray.Add(SendCardItem);

	//启动动画
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//设置定时器
		SetTimer(IDI_SEND_CARD,SPEED_SEND_CARD,NULL);

		//设置步数,步长
		m_nStepCount = SEND_STEP_COUNT;
		m_SendCardCurPos = m_SendCardPos;
		m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
		m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

		RefreshGameView();
	}

	return;
}

//设置积分
bool CGameClientView::SetScoreInfo(WORD wChairID, LONGLONG lUserScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置积分
	if (m_lUserScore[wChairID]!=lUserScore)
	{
		m_lUserScore[wChairID] = lUserScore;

		//更新界面
		RefreshGameView();
	}

	return true;
}

//设置动作
bool CGameClientView::SetUserAction(WORD wActionUser, BYTE cbUserAction)
{
	//设置动作
	if ((m_wActionUser!=wActionUser)||(m_cbUserAction!=cbUserAction))
	{
		//设置变量
		m_wActionUser=wActionUser;
		m_cbUserAction=cbUserAction;

		//更新界面
		RefreshGameView();

		//设置时间
		if (m_wActionUser!=INVALID_CHAIR) SetTimer(IDI_USER_ACTION,3000,NULL);
	}

	return true;
}

//设置牌面分
VOID CGameClientView::SetCardScore( WORD wChairId, FLOAT fCardScore )
{
	if( wChairId == INVALID_CHAIR )
	{
		for( WORD i = 0; i < GAME_PLAYER; i++ )
			m_fUserCardScore[i] = -1;
		RefreshGameView();
	}
	else if( m_fUserCardScore[wChairId] != fCardScore )
	{
		m_fUserCardScore[wChairId] = fCardScore;
		RefreshGameView();
	}
}

//设置单元分
VOID CGameClientView::SetCellScore( LONGLONG lScore )
{
	m_lCellScore = lScore;
	RefreshGameView();
}

//加筹码
VOID CGameClientView::OnUserAddJettons( WORD wChairId, LONGLONG lScore )
{
	if( wChairId == INVALID_CHAIR ) 
	{
		ZeroMemory(m_lUserAddScore,sizeof(m_lUserAddScore));
		//筹码添加分
		m_lJettonScore = 0L;
		return;
	}
	m_lUserAddScore[wChairId] += lScore;
	m_JettonControl.AddScore(lScore,m_ptJettons[wChairId]);
}

//移除筹码
VOID CGameClientView::OnUserRemoveJettons( WORD wChairId, LONGLONG lScore )
{
	if( wChairId == INVALID_CHAIR ) return;
	if( lScore != 0 )
	{
		m_JettonControl.RemoveScore(lScore,m_ptJettons[wChairId]);
	}
	else 
	{
		m_JettonControl.RemoveAllScore( m_ptJettons[wChairId] );
	}
}

//开始筹码动画
VOID CGameClientView::BeginMoveJettons()
{
	if( m_JettonControl.BeginMoveJettons() )
	{
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );
		RefreshGameView();
	}
}

//停止筹码动画
VOID CGameClientView::StopMoveJettons()
{
	if( m_JettonControl.FinishMoveJettons() )
	{
		KillTimer( IDI_MOVE_JETTON );
		//发送消息
		SendEngineMessage(IDM_MOVE_JET_FINISH,0,0);
		RefreshGameView();
	}
}

//开始数字滚动
VOID CGameClientView::BeginMoveNumber()
{
	BOOL bSuccess = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].BeginScrollNumber() )
			bSuccess = TRUE;
	}
	if( bSuccess )
	{
		SetTimer( IDI_MOVE_NUMBER,TIME_MOVE_NUMBER,NULL );
		RefreshGameView();
	}
}

//停止数字滚动
VOID CGameClientView::StopMoveNumber()
{
	BOOL bMoving = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].FinishScrollNumber() )
			bMoving = TRUE;
	}
	if( bMoving )
	{
		KillTimer( IDI_MOVE_NUMBER );
		RefreshGameView();
	}
}

//设置结束信息
VOID CGameClientView::SetGameEndInfo( WORD wWinner )
{
	if( wWinner == INVALID_CHAIR ) return ;

	m_JettonControl.RemoveAllScore( m_ptJettons[wWinner] );
	if( m_JettonControl.BeginMoveJettons() )
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );
	RefreshGameView();
}

//定时器
void CGameClientView::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case IDI_HIDE_CARD:		//隐藏扑克
		{
			//删除时间
			KillTimer(IDI_HIDE_CARD);

			//隐藏判断
			if (m_CardControl[MYSELF_VIEW_ID].GetDisplayHead()==true)
			{
				//设置控件
				m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(false);

				//更新界面
				RefreshGameView();
			}

			return;
		}
	case IDI_SEND_CARD:		//发牌动画
		{
			//绘画区域
			CRect rcDraw( m_SendCardCurPos.x,m_SendCardCurPos.y,
				m_SendCardCurPos.x+m_SizeGameCard.cx,m_SendCardCurPos.y+m_SizeGameCard.cy );

			//设置位置
			m_SendCardCurPos.x += m_nXStep;
			m_SendCardCurPos.y += m_nYStep;

			rcDraw |= CRect( m_SendCardCurPos.x,m_SendCardCurPos.y,
				m_SendCardCurPos.x+m_SizeGameCard.cx,m_SendCardCurPos.y+m_SizeGameCard.cy );

			//更新界面
			RefreshGameView(&rcDraw);

			//停止判断
			if ( --m_nStepCount == 0 )
			{
				//获取扑克
				BYTE cbCardData[MAX_COUNT];
				WORD wChairID=m_SendCardItemArray[0].wChairID;
				BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

				//设置扑克
				if (cbCardCount<MAX_COUNT)
				{
					cbCardData[cbCardCount++]=m_SendCardItemArray[0].cbCardData;
					m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
				}

				//删除动画
				m_SendCardItemArray.RemoveAt(0);

				RefreshGameView();

				//继续动画
				if (m_SendCardItemArray.GetCount()>0)
				{
					//获取位置
					wChairID = m_SendCardItemArray[0].wChairID;
					m_nStepCount = SEND_STEP_COUNT;
					m_SendCardCurPos = m_SendCardPos;
					m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
					m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

					//播放声音
					CGameClientEngine * pGameDlg=(CGameClientEngine *)AfxGetMainWnd();
					pGameDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

					return;
				}
				else 
				{
					//完成处理
					KillTimer(IDI_SEND_CARD);
					//发送消息
					SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

					return;
				}
			}

			return;
		}
	case IDI_MOVE_JETTON:				//筹码动画
		{
			if( !m_JettonControl.PlayMoveJettons() )
			{
				KillTimer(IDI_MOVE_JETTON);

				//发送消息
				SendEngineMessage(IDM_MOVE_JET_FINISH,0,0);

				//显示筹码添加信息
				if( m_lJettonScore < m_JettonControl.GetScore() )
				{
					SetTimer( IDI_ACTION_ADD_JETTON,TIME_ACTION_ADD_JETTON,NULL );
				}

				//播放声音
				CGameClientEngine * pGameDlg=(CGameClientEngine *)AfxGetMainWnd();
				pGameDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

				RefreshGameView();

				return;
			}
			CRect rcDraw;
			//获取更新区域
			m_JettonControl.GetDrawRect(rcDraw);
			RefreshGameView(&rcDraw);
			return;
		}
	case IDI_ACTION_ADD_JETTON:
		{
			KillTimer( IDI_ACTION_ADD_JETTON );
			m_lJettonScore = (LONGLONG)m_JettonControl.GetScore();
			RefreshGameView();
			return;
		}
	case IDI_MOVE_NUMBER:				//数字滚动
		{
			BOOL bMoving = FALSE;
			for( INT i = 0; i < GAME_PLAYER; i++ )
			{
				if( m_NumberControl[i].PlayScrollNumber() )
				{
					bMoving = TRUE;
					//获取更新区域
					CRect rc;
					m_NumberControl[i].GetDrawRect(rc);
					RefreshGameView(&rc);
				}
			}
			if( !bMoving )
				KillTimer(IDI_MOVE_NUMBER);
			return ;
		}
	case IDI_USER_ACTION:	//用户动作
		{
			KillTimer( IDI_USER_ACTION );

			//设置变量
			m_cbUserAction=0;
			m_wActionUser=INVALID_CHAIR;

			//更新界面
			RefreshGameView();

			return;
		}
	}
	__super::OnTimer(nIDEvent);
}

//左键按下
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克点击
	if (m_CardControl[MYSELF_VIEW_ID].OnEventLeftMouseDown(MousePoint)==true)
	{
		//更新界面
		RefreshGameView();

		//设置时间
		if (m_CardControl[MYSELF_VIEW_ID].GetDisplayHead()==true)
		{
			SetTimer(IDI_HIDE_CARD,2000,NULL);
		}
		else KillTimer(IDI_HIDE_CARD);
	}

	//test
	//static int n = 0;
	//if( ++n <= 1 )
	//{
	//	for( WORD i = 0; i < GAME_PLAYER; i++ )
	//	{
	//		if( i == 0 ) continue;
	//		OnUserAddJettons( i,123456L );
	//	}
	//	BeginMoveJettons();	
	//}
	//else 
	//{
	//	OnUserAddJettons( 0,123456L*3 );
	//	for( WORD i = 0; i < GAME_PLAYER; i++ )
	//	{
	//		if( i == 0 ) continue;
	//		OnUserRemoveJettons( i,123456L*2 );
	//	}
	//	BeginMoveJettons();
	//}

	//CDlgInfomation dlg;
	//if( dlg.DoModal() == IDOK )
	//	SendEngineMessage( WM_CLOSE );
	//for( WORD i = 0; i < GAME_PLAYER; i++ )
	//{
	//	m_NumberControl[i].SetScore( 1234567L );
	//}
	//BeginMoveNumber();
	//end test

	return;
}

//设置光标
BOOL CGameClientView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_CardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true) return TRUE;

	return __super::OnSetCursor(pWnd,nHitTest,message);
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
void CGameClientView::RefreshGameView(CRect rect)
{
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}
//绘画牌点
VOID CGameClientView::DrawCardScore( CDC * pDC, INT nXPos, INT nYPos, FLOAT fCardScore, bool bLeftAlign )
{
	int nWidthCardScore = m_PngCardScore.GetWidth()/11;
	int nHeightCardScore = m_PngCardScore.GetHeight();
	int nWidthCardNum = m_PngNumber.GetWidth()/10;
	int nHeightCardNum = m_PngNumber.GetHeight();

	int nXDraw = nXPos;
	int nYDraw = nYPos;

	//大天王、九小、八小、七小、六小、天王、人五小、五小
	if( fCardScore >= CT_WU_XIAO )
	{
		if( !bLeftAlign )
			nXDraw -= nWidthCardScore*((fCardScore==CT_REN_WU_XIAO||fCardScore==CT_DA_TIAN_WANG)?3:2)/3;
		m_PngCardScore.DrawImage( pDC,nXDraw,nYDraw,nWidthCardScore,nHeightCardScore,
			((BYTE)(CT_DA_TIAN_WANG-fCardScore))*nWidthCardScore,0,nWidthCardScore,nHeightCardScore );
	}
	//爆牌
	else if( fCardScore == CT_ERROR )
	{
		if( !bLeftAlign )
			nXDraw -= nWidthCardScore*2/3;
		m_PngCardScore.DrawImage( pDC,nXDraw,nYDraw,nWidthCardScore,nHeightCardScore,
			10*nWidthCardScore,0,nWidthCardScore,nHeightCardScore );
	}
	//数字
	else
	{
		if( !bLeftAlign )
		{
			if( fCardScore >= 10.0 )
				nXDraw -= nHeightCardNum*2;
			else nXDraw -= nHeightCardNum;

			nXDraw -= nWidthCardScore*((fCardScore-(LONGLONG)fCardScore)>0?2:1)/3;
		}

		//计算数目
		INT nNumberCount=0;
		LONGLONG lNumber = (LONGLONG)fCardScore;
		LONGLONG lNumberTemp=lNumber;
		do
		{
			nNumberCount++;
			lNumberTemp/=10;
		} while (lNumberTemp!=0);
		nXDraw += nWidthCardNum*(nNumberCount-1);
		//绘画数字
		lNumberTemp = lNumber;
		for (INT i=0;i<nNumberCount;i++)
		{
			//绘画号码
			INT lCellNumber=INT(lNumberTemp%10);
			m_PngNumber.DrawImage(pDC,nXDraw,nYDraw,nWidthCardNum,nHeightCardNum,lCellNumber*nWidthCardNum,0);

			//设置变量
			lNumberTemp/=10;
			nXDraw-=nWidthCardNum;
		}

		nXDraw += nWidthCardNum*(nNumberCount+1);
		if( (fCardScore-(LONGLONG)fCardScore) > 0 )
		{
			m_PngCardScore.DrawImage( pDC,nXDraw,nYDraw,nWidthCardScore,nHeightCardScore,
				8*nWidthCardScore,0,nWidthCardScore,nHeightCardScore );
		}
		else
		{
			m_PngCardScore.DrawImage( pDC,nXDraw,nYDraw,nWidthCardScore,nHeightCardScore,
				9*nWidthCardScore,0,nWidthCardScore,nHeightCardScore );
		}
	}
}

//绘画数字
VOID CGameClientView::DrawNumberString( CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, CPngImage &PngNumber, bool bHasSign )
{
	//加载资源
	INT nNumberHeight=PngNumber.GetHeight();
	INT nNumberWidth=PngNumber.GetWidth()/(10+(bHasSign?2:0));

	//计算数目
	INT nNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		nNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp!=0);

	//位置定义
	INT nYDrawPos=INT(nYPos-nNumberHeight/2);
	INT nXDrawPos=INT(nXPos+(nNumberCount+(bHasSign?1:0))*nNumberWidth/2-nNumberWidth);

	BYTE bySignCount = bHasSign?2:0;
	//绘画数字
	if( lNumber < 0L ) lNumberTemp = -lNumber;
	else lNumberTemp = lNumber;
	for (INT i=0;i<nNumberCount;i++)
	{
		//绘画号码
		INT lCellNumber=INT(lNumberTemp%10);
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,(lCellNumber+bySignCount)*nNumberWidth,0);

		//设置变量
		lNumberTemp/=10;
		nXDrawPos-=nNumberWidth;
	}
	//绘画正负号
	if( bHasSign )
	{
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,(lNumber>=0?0:1)*nNumberWidth,0);
	}

	return;
}


//艺术字体
bool CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, INT nXPos, INT nYPos)
{
	//变量定义
	INT nStringLength=lstrlen(pszString);
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//保存设置
	UINT nTextAlign=pDC->GetTextAlign();
	COLORREF rcTextColor=pDC->GetTextColor();

	//绘画边框
	for (INT i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->SetTextColor(crFrame);
		TextOut(pDC,nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	TextOut(pDC,nXPos,nYPos,pszString,nStringLength);

	//还原设置
	pDC->SetTextAlign(nTextAlign);
	pDC->SetTextColor(rcTextColor);

	return true;
}

//设置庄家
VOID CGameClientView::SetBankerUser( WORD wBankerUser )
{
	if( m_wBankerUser != wBankerUser )
	{
		m_wBankerUser = wBankerUser;
		RefreshGameView();
	}
}

//设置等待
VOID CGameClientView::SetWaitUserScore( bool bWaitUserScore )
{
	if( m_bWaitUserScore != bWaitUserScore )
	{
		m_bWaitUserScore = bWaitUserScore;
		RefreshGameView();
	}
}

//设置显示
VOID CGameClientView::ShowAddJettonInfo( bool bShow )
{
	if( m_bShowAddJetton != bShow )
	{
		m_bShowAddJetton = bShow;
		RefreshGameView();
	}
}

//////////////////////////////////////////////////////////////////////////