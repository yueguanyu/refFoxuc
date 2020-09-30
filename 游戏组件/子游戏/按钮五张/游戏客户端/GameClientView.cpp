#include "StdAfx.h"
#include "Resource.h"
#include "GameClientEngine.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//按钮标识
#define IDC_START						100								//开始按钮
#define IDC_FOLLOW						103								//跟注按钮
#define IDC_GIVE_UP						104								//放弃按钮
#define IDC_SHOW_HAND					105								//梭哈按钮
#define IDC_ADD_TIMES1					106								//查分按钮
#define IDC_ADD_TIMES2					107								//查分按钮
#define IDC_ADD_TIMES3					108								//查分按钮

//加注按钮
#define IDC_ADD_SCORE					113								//加注按钮

//定时器标识
#define IDI_SEND_CARD					100								//发牌定时器
#define IDI_SHOW_CARD					101								//
#define IDI_USER_SHOW_HAND				102								//

#define SPEED_MOVE_CARD					30								//

#define TIME_USER_SHOW_HAND				3000							//

//发牌定义
#define SEND_STEP_COUNT					6								//步数
#define SPEED_SEND_CARD					30								//发牌速度

#define SHOW_CARD_ANIMATE												//设置看牌动画,如果要取消动画,则注释掉

//////////////////////////////////////////////////////////////////////////
																			  
BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)							  
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//动画变量
	m_SendCardPos.SetPoint(0,0);
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = SEND_STEP_COUNT;
	m_nXStep = 0;
	m_nYStep = 0;

	//数据变量
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lCellScore = 0L;
	m_bUserShowHand = false;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageCard.LoadFromResource(hInstance,IDB_CARD);
	m_ImageViewBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
	m_PngNumber.LoadImage(hInstance,TEXT("NUMBER"));
	m_PngShowHand.LoadImage( hInstance,TEXT("SHOW_HAND") );


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

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_START);
	m_btFollow.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_FOLLOW);
	m_btGiveUp.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_GIVE_UP);
	m_btShowHand.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_SHOW_HAND);
	m_btAddTimes1.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_ADD_TIMES1);
	m_btAddTimes2.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_ADD_TIMES2);
	m_btAddTimes3.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_ADD_TIMES3);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_START,hInstance,false,false);
	m_btFollow.SetButtonImage(IDB_BT_FOLLOW,hInstance,false,false);
	m_btGiveUp.SetButtonImage(IDB_BT_FANGQI,hInstance,false,false);
	m_btShowHand.SetButtonImage(IDB_BT_SOHA,hInstance,false,false);
	m_btAddTimes1.SetButtonImage(IDB_BT_ADD_SCORE,hInstance,false,false);
	m_btAddTimes2.SetButtonImage(IDB_BT_ADD_SCORE,hInstance,false,false);
	m_btAddTimes3.SetButtonImage(IDB_BT_ADD_SCORE,hInstance,false,false);

	//
	m_PlayerJeton[GAME_PLAYER].SetDrawMode(enDrawMode_Rand);

	//test
	/*BYTE byCard[] = 
	{
		0x01,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,							//方块 A - K
		0x11,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,							//梅花 A - K
		0x21,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,							//红桃 A - K
		0x31,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,
	};
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_CardControl[i].SetCardData(byCard,5);
		m_PlayerJeton[i].SetScore(123456L);
		m_lTableScore[i] = 10000L;
	}
	m_PlayerJeton[GAME_PLAYER].SetScore(123456L);
	//m_btStart.ShowWindow(SW_SHOW);
	m_btFollow.ShowWindow(SW_SHOW);
	m_btGiveUp.ShowWindow(SW_SHOW);
	m_btShowHand.ShowWindow(SW_SHOW);
	for( i = 0; i < GAME_PLAYER; i++ )
	{
		m_ScoreView.SetGameScore(i,TEXT("S"),(i%2!=0)?1L:-1L);
		m_ScoreView.SetGameTax(i,i%2==0?-1:1);
	}
	m_ScoreView.SetGameScore(2,TEXT("sssssssssssS"),0);
	m_ScoreView.ShowWindow(SW_SHOW);
	m_lCellScore = 93L;*/
	//end test

	return 0;
}

//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_START:				//开始按钮
		{
			//发送消息
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_FOLLOW:			//跟注按钮
		{
			//发送消息
			SendEngineMessage(IDM_FOLLOW,0,0);
			return TRUE;
		}
	case IDC_GIVE_UP:			//放弃按钮
		{
			//发送消息
			SendEngineMessage(IDM_GIVE_UP,0,0);
			return TRUE;
		}
	case IDC_SHOW_HAND:			//梭哈按钮
		{
			//发送消息
			SendEngineMessage(IDM_SHOW_HAND,0,0);
			return TRUE;
		}
	case IDC_ADD_TIMES1:			//加注按钮
	case IDC_ADD_TIMES2:
	case IDC_ADD_TIMES3:
		{
			SendEngineMessage(IDM_ADD_SCORE,0,LOWORD(wParam)-IDC_ADD_TIMES1+1);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//动画变量
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = 0;
	m_nXStep = 0;
	m_nYStep = 0;
	m_SendCardItemArray.RemoveAll();

	//数据变量
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lCellScore = 0L;
	m_bUserShowHand = false;

	//设置筹码
	for (WORD i=0;i<GAME_PLAYER+1;i++) 
	{
		m_PlayerJeton[i].SetScore(0L);
	}

	//删除定时器
	KillTimer(IDI_SEND_CARD);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btFollow.ShowWindow(SW_HIDE);
	m_btShowHand.ShowWindow(SW_HIDE);

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);
	for (WORD i=0;i<GAME_PLAYER;i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
VOID CGameClientView::RectifyControl(int nWidth, int nHeight)
{
	//调整坐标
	m_ptAvatar[0].x=nWidth/2+330-40;
	m_ptAvatar[0].y=nHeight/2-232;
	m_ptNickName[0].x=m_ptAvatar[0].x-5;
	m_ptNickName[0].y=m_ptAvatar[0].y+12;
	m_ptClock[0].x=m_ptNickName[0].x - 100;
	m_ptClock[0].y=m_ptNickName[0].y + 10;
	m_ptReady[0].x=m_ptNickName[0].x - 120;
	m_ptReady[0].y=m_ptNickName[0].y + 12;
	m_ptTableScore[0].x = nWidth/2+158;
	m_ptTableScore[0].y = nHeight/2-200;

	//调整坐标
	m_ptAvatar[1].x=nWidth/2+330-40;
	m_ptAvatar[1].y=nHeight/2-40;
	m_ptNickName[1].x=m_ptAvatar[1].x-5;
	m_ptNickName[1].y=m_ptAvatar[1].y+12;
	m_ptClock[1].x=m_ptNickName[1].x - 100;
	m_ptClock[1].y=m_ptNickName[1].y ;
	m_ptReady[1].x=m_ptNickName[1].x - 120;
	m_ptReady[1].y=m_ptNickName[1].y + 10;
	m_ptTableScore[1].x = nWidth/2+145;
	m_ptTableScore[1].y = nHeight/2+25;

	//调整坐标
	m_ptAvatar[2].x=nWidth/2-20;
	m_ptAvatar[2].y=nHeight/2+215;
	m_ptNickName[2].x=m_ptAvatar[2].x+50;
	m_ptNickName[2].y=m_ptAvatar[2].y+12;
	m_ptClock[2].x=m_ptAvatar[2].x-40;
	m_ptClock[2].y=m_ptAvatar[2].y+40/2;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nHeight/2+53;
	m_ptTableScore[2].x = nWidth/2;
	m_ptTableScore[2].y = nHeight/2+75;

	//调整坐标
	m_ptAvatar[3].x=nWidth/2-340;
	m_ptAvatar[3].y=nHeight/2-40;
	m_ptNickName[3].x=m_ptAvatar[3].x+50;
	m_ptNickName[3].y=m_ptAvatar[3].y+12;
	m_ptClock[3].x=m_ptNickName[3].x + 100;
	m_ptClock[3].y=m_ptNickName[3].y ;
	m_ptReady[3].x=m_ptNickName[3].x + 120;
	m_ptReady[3].y=m_ptNickName[3].y + 10;
	m_ptTableScore[3].x = nWidth/2-153;
	m_ptTableScore[3].y = nHeight/2+25;

	//调整坐标
	m_ptAvatar[4].x=nWidth/2-340;
	m_ptAvatar[4].y=nHeight/2-232;
	m_ptNickName[4].x=m_ptAvatar[4].x+50;
	m_ptNickName[4].y=m_ptAvatar[4].y+12;
	m_ptClock[4].x=m_ptNickName[4].x + 100;
	m_ptClock[4].y=m_ptNickName[4].y + 10;
	m_ptReady[4].x=m_ptNickName[4].x + 120;
	m_ptReady[4].y=m_ptNickName[4].y + 12;
	m_ptTableScore[4].x = nWidth/2-158;
	m_ptTableScore[4].y = nHeight/2-200;

	//用户扑克
	m_CardControl[0].SetBenchmarkPos(nWidth/2+258,nHeight/2-183,enXCenter,enYTop);
	m_CardControl[1].SetBenchmarkPos(nWidth/2+225,nHeight/2+10,enXCenter,enYTop);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,nHeight/2+200,enXCenter,enYBottom);
	m_CardControl[3].SetBenchmarkPos(nWidth/2-233,nHeight/2+10,enXCenter,enYTop);
	m_CardControl[4].SetBenchmarkPos(nWidth/2-268,nHeight/2-183,enXCenter,enYTop);

	//下注框位置
	INT nXControl = nWidth/2+150;
	INT nYControl = nHeight/2+150;

	//按钮控件
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	
	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2+58,nHeight/2+264,0,0,uFlags);

	//控制按钮
	m_btFollow.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btFollow,NULL,nXControl,nYControl,0,0,uFlags);
	DeferWindowPos(hDwp,m_btShowHand,NULL,nXControl+rcButton.Width()+1,nYControl,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes3,NULL,nXControl,nYControl+rcButton.Height()+1,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes2,NULL,nXControl+rcButton.Width()+1,nYControl+rcButton.Height()+1,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes1,NULL,nXControl,nYControl+rcButton.Height()*2+1,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nXControl+rcButton.Width()+1,nYControl+rcButton.Height()*2+1,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//玩家筹码
	m_PlayerJeton[0].SetBenchmarkPos(nWidth/2+138,nHeight/2-140);
	m_PlayerJeton[1].SetBenchmarkPos(nWidth/2+125,nHeight/2-20);
	m_PlayerJeton[2].SetBenchmarkPos(nWidth/2,nHeight/2+45);
	m_PlayerJeton[3].SetBenchmarkPos(nWidth/2-135,nHeight/2-20);
	m_PlayerJeton[4].SetBenchmarkPos(nWidth/2-138,nHeight/2-140);
	//桌面筹码
	m_PlayerJeton[GAME_PLAYER].SetBenchmarkPos(nWidth/2-70,nHeight/2-130);

	//发牌起始位置
	m_SendCardPos.SetPoint(nWidth/2+150,nHeight/2-340);

	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,nHeight/2-240,0,0,SWP_NOZORDER|SWP_NOSIZE);
	
	return;
}

//动画判断
bool CGameClientView::IsDispatchCard()
{
	return m_SendCardItemArray.GetCount()>0;
}

//完成发牌
void CGameClientView::FinishDispatchCard()
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

	//删除处理
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	AfxGetMainWnd()->SendMessage(IDM_SEND_CARD_FINISH,0,0);

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//发送扑克
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
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

		//
		m_nStepCount = SEND_STEP_COUNT;
		m_SendCardCurPos = m_SendCardPos;
		m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
		m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

		//
		InvalidGameView(0,0,0,0);

		//播放声音
		CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
		pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//设置下注
void CGameClientView::SetUserTableScore(WORD wChairID, LONGLONG lTableScore)
{
	//设置数据
	if (wChairID!=INVALID_CHAIR) m_lTableScore[wChairID]=lTableScore;
	else ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//发送处理
	if (nIDEvent==IDI_SEND_CARD)
	{
		//设置位置
		m_SendCardCurPos.x += m_nXStep;
		m_SendCardCurPos.y += m_nYStep;
		//更新界面
		InvalidGameView(0,0,0,0);
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
			//继续动画
			if (m_SendCardItemArray.GetCount()>0)
			{
				wChairID = m_SendCardItemArray[0].wChairID;
				m_nStepCount = SEND_STEP_COUNT;
				m_SendCardCurPos = m_SendCardPos;
				m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
				m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;
				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
				return;
			}
			else 
			{	//完成处理
				KillTimer(IDI_SEND_CARD);
				PostEngineMessage(IDM_SEND_CARD_FINISH,0,0);
				return;
			}
		}
		return;
	}
	else if( nIDEvent == IDI_USER_SHOW_HAND )
	{
		KillTimer( IDI_USER_SHOW_HAND );
		m_bUserShowHand = false;
		InvalidGameView(0,0,0,0);
		return;
	}
#ifdef SHOW_CARD_ANIMATE
	else if( nIDEvent == IDI_SHOW_CARD )
	{
		if( !m_CardControl[MYSELF_VIEW_ID].PlayMoveCard() )
		{
			KillTimer( IDI_SHOW_CARD );
		}
		InvalidGameView(0,0,0,0);
		return;
	}
#endif

	__super::OnTimer(nIDEvent);
}

//设置单元注
void CGameClientView::SetCellScore( LONGLONG lCellScore )
{
	if( m_lCellScore == lCellScore ) return;
	m_lCellScore = lCellScore;
	//设置下注按钮标题
	TCHAR szNum[16];
	_i64tot(m_lCellScore,szNum,10);
    m_btAddTimes1.SetWindowText(szNum);
	
	_i64tot(m_lCellScore*2,szNum,10);
	m_btAddTimes2.SetWindowText(szNum);
	
	_i64tot(m_lCellScore*3,szNum,10);
	m_btAddTimes3.SetWindowText(szNum);
	InvalidGameView(0,0,0,0);
}

//
void CGameClientView::SetUserShowHand( bool bShowHand )
{
	if( m_bUserShowHand != bShowHand )
	{
		m_bUserShowHand = bShowHand;
		
		//
		if( m_bUserShowHand )
			SetTimer( IDI_USER_SHOW_HAND,TIME_USER_SHOW_HAND,NULL );

		InvalidateRect( NULL );
	}
}

VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);

	IClientUserItem*pClientUserItem=NULL;
	tagUserInfo* pUserData=NULL;



	//绘画用户
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserClock(i);
		pClientUserItem=GetClientUserItem(i);
		pUserData=(pClientUserItem!=NULL)?pClientUserItem->GetUserInfo():NULL;

		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign(((i==0)||(i==1))?TA_RIGHT:TA_LEFT);
			DrawTextString(pDC,pUserData->szNickName,RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x,m_ptNickName[i].y);

			//用户金币
			LONGLONG lLeaveScore=(pUserData->cbUserStatus==US_PLAYING)?m_lUserScore[i]-m_lTableScore[i]:pUserData->lScore;
			_i64tot(lLeaveScore,szBuffer,10);
			DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x,m_ptNickName[i].y+16);
			//当前下注
			if( m_lTableScore[i] > 0L )
			{
				if( i == MYSELF_VIEW_ID ) pDC->SetTextAlign(TA_RIGHT);
				_i64tot(m_lTableScore[i],szBuffer,10);
				DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptTableScore[i].x,m_ptTableScore[i].y);
			}

			//其他信息
			
			//定时器不显示
			if (wUserTimer!=0) DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
			//用户开始不显示
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			//调用出现异常，跟进到该组件内部发现指针莫名被清零
			DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pClientUserItem);
		}

		//玩家筹码
		m_PlayerJeton[i].DrawJettonControl(pDC);
		//筹码数字
		LONGLONG lJetonScore = m_PlayerJeton[i].GetScore();
		if( lJetonScore > 0L )
		{
			CPoint pt = m_PlayerJeton[i].GetBenchmarkPos();
			CRect rcDraw(pt.x-55,pt.y+15,pt.x+55,pt.y+27);
			_i64tot(lJetonScore,szBuffer,10);
			pDC->SetTextAlign(TA_LEFT);
			DrawTextString(pDC,szBuffer,RGB(255,255,85),RGB(0,0,0),&rcDraw);
		}
	}
	m_PlayerJeton[GAME_PLAYER].DrawJettonControl(pDC);

	for( WORD i = 0; i < GAME_PLAYER; i++ )
		m_CardControl[i].DrawCardControl(pDC);

	INT nNumberWidth = m_PngNumber.GetWidth()/10;
	INT nNumberHeight = m_PngNumber.GetHeight();

	//底注信息
	if( m_lCellScore > 0L )
	{
		INT nXPos;
		INT nYPos = nHeight/2-325;
		int nCellCount = 0;
		BYTE byCell[10];
		LONGLONG lCellScore = m_lCellScore;
		while( lCellScore > 0L )
		{
			byCell[nCellCount++] = (BYTE)(lCellScore-lCellScore/10*10);
			lCellScore /= 10;
			if( nCellCount == 10 ) break;
		}
		nXPos = nWidth/2-200-nCellCount*(nNumberWidth-3)/2;
		for( int j = nCellCount-1; j >= 0; j-- )
		{
			m_PngNumber.DrawImage(pDC,nXPos,nYPos,nNumberWidth,nNumberHeight,byCell[j]*nNumberWidth,0);
			nXPos += (nNumberWidth-3);
		}
	}
	//计算总注
	LONGLONG lTableScore=0L;
	//注:包括正在下注在内
	for( i = 0; i < GAME_PLAYER; i++ )
		lTableScore += m_PlayerJeton[i].GetScore();
	lTableScore+=m_PlayerJeton[GAME_PLAYER].GetScore();
	//test
	//lTableScore = 1234567890L;
	//总注信息
	if (lTableScore>0L)
	{
		INT nXPos = nWidth/2+13;
		INT nYPos = nHeight/2-297;
		int nCellCount = 0;
		BYTE byCell[10];
		while( lTableScore > 0L )
		{
			byCell[nCellCount++] = (BYTE)(lTableScore-lTableScore/10*10);
			lTableScore /= 10;
			if( nCellCount == 10 ) break;
		}
		for( int j = nCellCount-1; j >= 0; j-- )
		{
			m_PngNumber.DrawImage(pDC,nXPos,nYPos,nNumberWidth,nNumberHeight,byCell[j]*nNumberWidth,0);
			nXPos += (nNumberWidth-3);
		}
	}

	//用户梭哈
	if( m_bUserShowHand )
	{
		m_PngShowHand.DrawImage( pDC,(nWidth-m_PngShowHand.GetWidth())/2,(nHeight-m_PngShowHand.GetHeight())/2 );
	}

	//绘画扑克
	if (m_SendCardItemArray.GetCount()>0)
	{
		//变量定义
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//获取大小
		int nItemWidth=m_ImageCard.GetWidth()/13;
		int nItemHeight=m_ImageCard.GetHeight()/5;

		//绘画扑克
		m_ImageCard.BlendDrawImage(pDC,m_SendCardCurPos.x,m_SendCardCurPos.y,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4,RGB(255,0,255),255);
	}
}
//
void CGameClientView::SetUserScore( WORD wChairId, LONGLONG lScore )
{
	if( wChairId == INVALID_CHAIR )
		ZeroMemory( m_lUserScore,sizeof(m_lUserScore) );
	else m_lUserScore[wChairId] = lScore;
	InvalidateRect( NULL );
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_CardControl[2].OnEventSetCursor(MousePoint)==true) return TRUE;

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克点击
	if (m_CardControl[MYSELF_VIEW_ID].OnEventLeftMouseDown(MousePoint)==true)
	{
#ifdef	SHOW_CARD_ANIMATE
		if( m_CardControl[MYSELF_VIEW_ID].BeginMoveCard() )
		{
			SetTimer( IDI_SHOW_CARD,SPEED_MOVE_CARD,NULL );
			//更新界面
			InvalidGameView(0,0,0,0);
		}
#else
		m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(true);
		InvalidGameView(0,0,0,0);
#endif
	}

	return;
}

//
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克点击
	if (m_CardControl[MYSELF_VIEW_ID].OnEventLeftMouseUp(MousePoint)==true)
	{
#ifndef	SHOW_CARD_ANIMATE
		m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(false);
		InvalidGameView(0,0,0,0);
		PostMessage( WM_SETCURSOR );
#endif
	}
}

//////////////////////////////////////////////////////////////////////////


