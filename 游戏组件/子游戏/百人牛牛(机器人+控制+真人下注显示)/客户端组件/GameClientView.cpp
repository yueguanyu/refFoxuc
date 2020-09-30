#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "Math.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER			100									//闪动标识
#define IDI_SHOW_CHANGE_BANKER		101									//轮换庄家
#define IDI_DISPATCH_CARD			102									//发牌标识
#define IDI_SHOWDISPATCH_CARD_TIP	103									//发牌提示
#define IDI_OPENCARD				104									//发牌提示
#define IDI_MOVECARD_END			105									//移動牌結束
#define IDI_POSTCARD				106									//发牌提示

//按钮标识
#define IDC_JETTON_BUTTON_100		260									//按钮标识
#define IDC_JETTON_BUTTON_1000		261									//按钮标识
#define IDC_JETTON_BUTTON_10000		262									//按钮标识
#define IDC_JETTON_BUTTON_100000	263									//按钮标识
#define IDC_JETTON_BUTTON_1000000	264									//按钮标识
#define IDC_JETTON_BUTTON_5000000	265									//按钮标识
#define IDC_APPY_BANKER				266									//按钮标识
#define IDC_CANCEL_BANKER			267									//按钮标识
#define IDC_SCORE_MOVE_L			268									//按钮标识
#define IDC_SCORE_MOVE_R			269									//按钮标识
#define IDC_VIEW_CHART				270									//按钮标识
#define IDC_JETTON_BUTTON_50000	    271									//按钮标识
#define IDC_JETTON_BUTTON_500000	272									//按钮标识
#define IDC_AUTO_OPEN_CARD			273									//按钮标识
#define IDC_OPEN_CARD				274									//按钮标识
#define IDC_BANK					275									//按钮标识
#define IDC_CONTINUE_CARD			276									//按钮标识
#define IDC_UP						277									//按钮标识
#define IDC_DOWN					278									//按钮标识

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_VIEWLBTUP,OnViLBtUp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
		//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

	//庄家信息
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;
	m_blCanStore=false;

	//当局成绩
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//状态信息
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinTianMen=false;
	m_bWinHuangMen=false;
	m_bWinXuanMen=false;
	m_bFlashResult=false;
	m_blMoveFinish = false;
	m_blAutoOpenCard = true;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

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

//重置界面
VOID CGameClientView::ResetGameView()
{
	//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
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
	m_blCanStore=false;

	//状态信息
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinTianMen=false;
	m_bWinHuangMen=false;
	m_bWinXuanMen=false;
	m_bFlashResult=false;
	m_bShowGameResult=false;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

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

	//清空列表
	m_ApplyUser.ClearAll();

	//清除桌面
	CleanUserJetton();

	//设置按钮
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
//位置信息
	m_nWinFlagsExcursionX = nWidth/2-260-41;	
	m_nWinFlagsExcursionY = nHeight/2 + 246-21;

	CSize Size;
	m_CardControl[0].GetControlSize(Size,3);
	Size.cy = Size.cy/2;

	int iWidth = m_ImageViewBack.GetWidth();

	int LifeWidth = nWidth/2-iWidth/2;
	int TopHeight = nHeight/2-m_ImageViewBack.GetHeight()/2;

	//区域位置
	int nCenterX=nWidth/2, nCenterY=nHeight/2, nBorderWidth=4;
	m_rcTianMen.left=LifeWidth+33+22;
	m_rcTianMen.top=TopHeight+257+7;
	m_rcTianMen.right=m_rcTianMen.left+167;
	m_rcTianMen.bottom=m_rcTianMen.top+138;

	m_rcDimen.left=LifeWidth+33+25+172;
	m_rcDimen.top=TopHeight+257+7;
	m_rcDimen.right=m_rcDimen.left+167;
	m_rcDimen.bottom=m_rcDimen.top+138;

	m_rcXuanMen.left=LifeWidth+33+25+177+170;
	m_rcXuanMen.top=TopHeight+257+7;
	m_rcXuanMen.right=m_rcXuanMen.left+167;
	m_rcXuanMen.bottom=m_rcXuanMen.top+138;


	m_rcHuangMen.left=LifeWidth+33+25+175+175+173;
	m_rcHuangMen.top=TopHeight+257+7;
	m_rcHuangMen.right=m_rcHuangMen.left+167;
	m_rcHuangMen.bottom=m_rcHuangMen.top+138;

	
	if ( m_pClientControlDlg )
	{
		CRect m_WinRect(m_rcTianMen);
		m_WinRect.top += m_WinRect.Height() / 2;
		m_SkinListCtrl[0].MoveWindow(m_WinRect);
		m_SkinListCtrl[0].SetColumnWidth( 0, m_WinRect.Width()/2 );
		m_SkinListCtrl[0].SetColumnWidth( 1, m_WinRect.Width()/2 );

		m_WinRect=m_rcDimen;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_SkinListCtrl[1].MoveWindow(m_WinRect);
		m_SkinListCtrl[1].SetColumnWidth( 0, m_WinRect.Width()/2 );
		m_SkinListCtrl[1].SetColumnWidth( 1, m_WinRect.Width()/2 );

		m_WinRect=m_rcXuanMen;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_SkinListCtrl[2].MoveWindow(m_WinRect);
		m_SkinListCtrl[2].SetColumnWidth( 0, m_WinRect.Width()/2 );
		m_SkinListCtrl[2].SetColumnWidth( 1, m_WinRect.Width()/2 );

		m_WinRect=m_rcHuangMen;
		m_WinRect.top += m_WinRect.Height() / 2;
		m_SkinListCtrl[3].MoveWindow(m_WinRect);
		m_SkinListCtrl[3].SetColumnWidth( 0, m_WinRect.Width()/2 );
		m_SkinListCtrl[3].SetColumnWidth( 1, m_WinRect.Width()/2 );
		
	}

	//筹码数字
	int ExcursionY=10;
	m_PointJettonNumber[ID_TIAN_MEN-1].SetPoint((m_rcTianMen.right+m_rcTianMen.left)/2, (m_rcTianMen.bottom+m_rcTianMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_DI_MEN -1].SetPoint((m_rcDimen.right+m_rcDimen.left)/2, (m_rcDimen.bottom+m_rcDimen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_HUANG_MEN-1].SetPoint((m_rcHuangMen.right+m_rcHuangMen.left)/2, (m_rcHuangMen.bottom+m_rcHuangMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_XUAN_MEN-1].SetPoint((m_rcXuanMen.right+m_rcXuanMen.left)/2, (m_rcXuanMen.bottom+m_rcXuanMen.top)/2-ExcursionY);

	//筹码位置
	m_PointJetton[ID_TIAN_MEN-1].SetPoint(m_rcTianMen.left, m_rcTianMen.top);
	m_PointJetton[ID_DI_MEN-1].SetPoint(m_rcDimen.left, m_rcDimen.top);
	m_PointJetton[ID_HUANG_MEN -1].SetPoint(m_rcHuangMen.left, m_rcHuangMen.top);
	m_PointJetton[ID_XUAN_MEN-1].SetPoint(m_rcXuanMen.left, m_rcXuanMen.top);


	//扑克控件
	m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2-355+135),enXCenter,enYTop);
	m_CardTypePoint[0] =CPoint(nWidth/2+125,nHeight/2-355+135+62); 

	m_CardControl[1].SetBenchmarkPos(CPoint(LifeWidth+Size.cy+Size.cy/6,nHeight/2+60-20),enXCenter,enYTop);
	m_CardTypePoint[1] =CPoint(LifeWidth+Size.cy+Size.cy/6,nHeight/2+60+112); 

	m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2-Size.cy+Size.cy/4-10,nHeight/2+60-20),enXCenter,enYTop);
	m_CardTypePoint[2] =CPoint(nWidth/2-Size.cy+Size.cy/4,nHeight/2+60+112); 

	m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2+Size.cy-Size.cy/4-10,nHeight/2+60-20),enXCenter,enYTop);
	m_CardTypePoint[3] =CPoint(nWidth/2+Size.cy-Size.cy/4,nHeight/2+60+112); 

	m_CardControl[4].SetBenchmarkPos(CPoint(nWidth-LifeWidth-Size.cy-Size.cy/6-20,nHeight/2+60-20),enXCenter,enYTop);
	m_CardTypePoint[4] =CPoint(nWidth-LifeWidth-Size.cy-Size.cy/6,nHeight/2+60+112); 

	m_CardControl[5].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2-100),enXCenter,enYTop);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(33);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	m_ApplyUser.m_viewHandle = m_hWnd;

	//列表控件
	DeferWindowPos(hDwp,m_ApplyUser,NULL,LifeWidth+245+20,TopHeight+10+9,311,92,uFlags);
	DeferWindowPos(hDwp,m_btUp,NULL,LifeWidth+245+20+310,TopHeight+10+10,75/5,16,uFlags);
	DeferWindowPos(hDwp,m_btDown,NULL,LifeWidth+245+20+310,TopHeight+10+12+88-20,75/5,16,uFlags);

	
	m_btUp.ShowWindow(SW_SHOW);
	m_btUp.EnableWindow(false);
	m_btDown.ShowWindow(SW_SHOW);
	m_btDown.EnableWindow(false);

	m_MeInfoRect.top = TopHeight+10+12;
	m_MeInfoRect.left = LifeWidth+245+20+389;

	//筹码按钮
	CRect rcJetton;
	m_btJetton100.GetWindowRect(&rcJetton);
	int nYPos = nHeight/2+265-42;
	int nXPos = nWidth/2+80+25;
	int nSpace = 0;

	DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos + nSpace + rcJetton.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton50000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace * 0 + rcJetton.Width() * 0,nYPos+rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500000,NULL,nXPos + nSpace * 1 + rcJetton.Width() * 1,nYPos+rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos+rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace * 3 + rcJetton.Width() *3,nYPos+rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);

	//上庄按钮
	DeferWindowPos(hDwp,m_btApplyBanker,NULL,nWidth/2+277-80,nHeight/2-340+74,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth/2+277-80,nHeight/2-340+74,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btScoreMoveL,NULL,LifeWidth+58+23+4,TopHeight+594+9,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR,NULL,LifeWidth+58+28+403+2,TopHeight+594+9,0,0,uFlags|SWP_NOSIZE);

	//开牌按钮
	DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,LifeWidth+624+20,TopHeight+198,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btOpenCard,NULL,LifeWidth+624+20,TopHeight+198+30,0,0,uFlags|SWP_NOSIZE);

	//其他按钮
	DeferWindowPos(hDwp,m_btBank,NULL,nWidth/2+290,nHeight/2-340+74,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btContinueCard,NULL,LifeWidth+624+20,TopHeight+198-30,0,0,uFlags|SWP_NOSIZE);

	//银行按钮
#ifdef __BANKER___
	m_btBankerDraw.GetWindowRect(&rcJetton);
	DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2+313,nHeight/2+228,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2+313,nHeight/2+228+53,0,0,uFlags|SWP_NOSIZE);
#endif
	
	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}
//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_UP:
			OnUp();
			break;
	case IDC_DOWN:
			OnDown();
			break;
	case IDC_OPEN_CARD:
			OnOpenCard();
			break;
	case IDC_BANK_DRAW:
			OnBankDraw();
			break;
	case IDC_BT_ADMIN:
			OpenAdminWnd();
			break;
	case IDC_APPY_BANKER:
			OnApplyBanker();
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
	case IDC_CANCEL_BANKER:
			OnCancelBanker();
			break;
	case IDC_AUTO_OPEN_CARD:
			OnAutoOpenCard();
			break;
	case IDC_JETTON_BUTTON_100:
		{
			//设置变量
			m_lCurrentJetton=100L;
			break;
		}
	case IDC_JETTON_BUTTON_1000:
		{
			//设置变量
			m_lCurrentJetton=1000L;
			break;
		}
	case IDC_JETTON_BUTTON_10000:
		{
			//设置变量
			m_lCurrentJetton=10000L;
			break;
		}
	case IDC_JETTON_BUTTON_50000:
		{
			//设置变量
			m_lCurrentJetton=50000L;
			break;
		}
	case IDC_JETTON_BUTTON_100000:
		{
			//设置变量
			m_lCurrentJetton=100000L;
			break;
		}
	case IDC_JETTON_BUTTON_500000:
		{
			//设置变量
			m_lCurrentJetton=500000L;
			break;
		}
	case IDC_JETTON_BUTTON_1000000:
		{
			//设置变量
			m_lCurrentJetton=1000000L;
			break;
		}
	case IDC_JETTON_BUTTON_5000000:
		{
			//设置变量
			m_lCurrentJetton=5000000L;
			break;
		}
	case IDC_BANK:
		{
			break;
		}
	case IDC_CONTINUE_CARD:
		{
			SendEngineMessage(IDM_CONTINUE_CARD,0,0);
			break;
		}

	}
	return __super::OnCommand(wParam, lParam);
}


//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	//DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	//DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);


	CDC* pDCBuff = pDC;

	//绘画背景
	for ( int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth() )
	{
		for ( int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight() )
		{
			m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
		}
	}
	m_ImageViewBack.BitBlt( pDC->GetSafeHdc(), nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - m_ImageViewBack.GetHeight()/2 );

	//获取状态
	BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

	//状态提示
	CFont static InfoFont;
	InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	CFont * pOldFont=pDC->SelectObject(&InfoFont);
	pDC->SetTextColor(RGB(255,234,0));
	
	pDC->SelectObject(pOldFont);
	InfoFont.DeleteObject();

	//时间提示
	int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	int nFlagIndex=0;
	if (cbGameStatus==GAME_SCENE_FREE) nFlagIndex=0;
	else if (cbGameStatus==GAME_SCENE_PLACE_JETTON) nFlagIndex=1;
	else if (cbGameStatus==GAME_SCENE_GAME_END) nFlagIndex=2;
	m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-348+49, nHeight/2+225-367, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nFlagIndex * nTimeFlagWidth, 0);

	//Dlg里面的SetGameTimer(GetMeChairID()）里面调用的SwitchViewChairID在桌子人数大于8时实际是ViewID转化为0了

		WORD wUserTimer=GetUserClock(m_wMeChairID);
		if (wUserTimer!=0) DrawUserClock(pDCBuff,nWidth/2-323+49,nHeight/2+197-372,wUserTimer);

	//胜利边框
	FlashJettonAreaFrame(nWidth,nHeight,pDCBuff);

	//筹码资源
	CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/9,m_ImageJettonView.GetHeight());

	//绘画筹码
	for (INT i=0;i<AREA_COUNT;i++)
	{
		//变量定义
		LONGLONG lScoreCount=0L;
		LONGLONG static lScoreJetton[JETTON_COUNT]={100L,1000L,10000L,50000L,100000L,500000L,1000000L,5000000L};
		int static nJettonViewIndex=0;

		//绘画筹码
		for (INT_PTR j=0;j<m_JettonInfoArray[i].GetCount();j++)
		{
			//获取信息
			tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

			//累计数字
			ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
			lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

			//图片索引
			if (1==pJettonInfo->cbJettonIndex || 2==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex+1;
			else if (0==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex;
			else nJettonViewIndex=pJettonInfo->cbJettonIndex+1;

			//绘画界面
			m_ImageJettonView.DrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[i].x,pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,nJettonViewIndex*SizeJettonItem.cx,0);
			/*m_ImageJettonView.TransDrawImage(pDCBuff,pJettonInfo->nXPos+m_PointJetton[i].x,
				pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,
				nJettonViewIndex*SizeJettonItem.cx,0,RGB(255,0,255));*/
		}

		//绘画数字
		if (lScoreCount>0L)	DrawNumberString(pDCBuff,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
	}

	//绘画庄家
	DrawBankerInfo(pDCBuff,nWidth,nHeight);

	//绘画用户
	DrawMeInfo(pDCBuff,nWidth,nHeight);

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

    //发牌提示
	if (enDispatchCardTip_NULL!=m_enDispatchCardTip)
	{
		if (m_ImageDispatchCardTip.IsNull()==false) m_ImageDispatchCardTip.Destroy();
		if (enDispatchCardTip_Dispatch==m_enDispatchCardTip)
			m_ImageDispatchCardTip.LoadFromResource(AfxGetInstanceHandle(),IDB_DISPATCH_CARD);
		else
			m_ImageDispatchCardTip.LoadFromResource(AfxGetInstanceHandle(),IDB_CONTINUE_CARD);
		m_ImageDispatchCardTip.BitBlt(pDC->GetSafeHdc(), (nWidth-m_ImageDispatchCardTip.GetWidth())/2, nHeight/2);
	}

	//我的下注
	DrawMeJettonNumber(pDCBuff);

	/*test*/
	if (0)
	{
		//绘画扑克
		for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].DrawCardControl(pDCBuff);
	}

	//胜利标志
	DrawWinFlags(pDCBuff);

	/*if(1)
	{
		m_bShowGameResult= 1;
		DrawType(pDC,0);
	}*/
	if(m_blMoveFinish)
	{
		//显示结果
		DrawType(pDCBuff,0);
		
	}

	//结束状态
	if (cbGameStatus==GAME_SCENE_GAME_END)
	{
		//绘画扑克
		for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].DrawCardControl(pDCBuff);

	}
	if(m_blMoveFinish)
	{

		ShowGameResult(pDCBuff, nWidth, nHeight);
	}

	return;
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	AfxSetResourceHandle(AfxGetInstanceHandle());
	

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageWinFlags.LoadFromResource(hInstance,IDB_WIN_FLAGS);
	m_ImageJettonView.LoadImage(hInstance,TEXT("JETTOM_VIEW"));
	m_ImageScoreNumber.LoadImage(hInstance,TEXT("SCORE_NUMBER"));
	m_ImageMeScoreNumber.LoadImage(hInstance,TEXT("ME_SCORE_NUMBER"));

	//边框资源
	m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_MEN_EX"));
	m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_MEN_EX"));
	//m_ImageFrameQiao.LoadFromResource(hInstance,IDB_FRAME_QIAO_EX);
	m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_MEN_EX"));
	m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_MEN_EX"));
	//m_ImageFrameJiaoR.LoadFromResource(hInstance,IDB_FRAME_JIAO_R_EX);
	for (int i = 0;i<5;i++)m_ImageCardType[i].LoadFromResource(hInstance,IDB_CARDTYPE);
	m_ImageGameEnd.LoadImage( hInstance,TEXT("GAME_END") );

	m_ImageMeBanker.LoadFromResource( hInstance,IDB_ME_BANKER );
	m_ImageChangeBanker.LoadFromResource( hInstance,IDB_CHANGE_BANKER );
	m_ImageNoBanker.LoadFromResource( hInstance,IDB_NO_BANKER );	

	m_ImageTimeFlag.LoadFromResource(hInstance,IDB_TIME_FLAG);
	
	//创建控件
	CRect rcCreate(0,0,0,0);
	m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
	m_ApplyUser.Create( IDD_DLG_GAME_RECORD	, this );
	m_GameRecord.ShowWindow(SW_HIDE);

	//下注按钮
	m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
	m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
	m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
	m_btJetton50000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_50000);
	m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);

	m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
	m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
	m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

	//申请按钮
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

	m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_AUTO_OPEN_CARD);
	m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_OPEN_CARD);

	m_btBank.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK);
	m_btContinueCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_CONTINUE_CARD);

	m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
	m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_DOWN);

	//	m_btBankerStorage.EnableWindow(TRUE);
	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false,false);
	m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false,false);
	m_btJetton50000.SetButtonImage(IDB_BT_JETTON_50000,hResInstance,false,false);
	m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false,false);
	m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false,false);
	m_btJetton500000.SetButtonImage(IDB_BT_JETTON_500000,hResInstance,false,false);
	m_btJetton1000000.SetButtonImage(IDB_BT_JETTON_1000000,hResInstance,false,false);
	m_btJetton5000000.SetButtonImage(IDB_BT_JETTON_5000000,hResInstance,false,false);

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);

	m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
	m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

	m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false,false);
	m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false,false);

	m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);
	m_btContinueCard.SetButtonImage(IDB_BT_CONTINUE_CARD,hResInstance,false,false);

//	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
//	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);

#ifdef __BANKER___	
	m_btBankerStorage.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_STORAGE);
	m_btBankerDraw.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK_DRAW);

	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);
#endif

	//扑克控件
	for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetDisplayFlag(true);

	//控制
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("OxBattleClientControl.dll"));
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

	
	if ( m_pClientControlDlg )
	{
		//AfxMessageBox(_T("玩家"));
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
void CGameClientView::SetMoveCardTimer()
{
	KillTimer(IDI_POSTCARD);
	m_OpenCardIndex = 1;
	m_bcShowCount = 0;
	m_PostStartIndex =  m_GameLogic.GetCardValue(m_bcfirstShowCard);
	if(m_PostStartIndex>1)
		m_PostStartIndex  = (m_PostStartIndex-1)%5;
	m_PostCardIndex=m_PostStartIndex;
	for (int i = 0;i<5;i++)
	{
		m_CardControl[i].m_blAdd = true;
		m_CardControl[i].m_blAuto = m_blAutoOpenCard;
		m_CardControl[i].m_blGameEnd = true;
		m_CardControl[i].m_blShowResult = false;
		m_CardControl[i].m_blShowLineResult = false;
	}
	SetTimer(IDI_POSTCARD,300,NULL);
	SetTimer(IDI_DISPATCH_CARD,25000,NULL);

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
	ASSERT(lCurrentJetton>=0L);
	m_lCurrentJetton=lCurrentJetton;

	return;
}

//历史记录
void CGameClientView::SetGameHistory(bool bWinTian, bool bWinDi, bool bWinXuan,bool bWinHuang)
{
	//设置数据
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinTianMen=bWinTian;
	GameRecord.bWinDiMen =bWinDi;
	GameRecord.bWinXuanMen=bWinXuan;
	GameRecord.bWinHuangMen=bWinHuang;

	//操作类型
	if (0==m_lUserJettonScore[ID_TIAN_MEN]) GameRecord.enOperateTianMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_TIAN_MEN] > 0 && true==bWinTian) GameRecord.enOperateTianMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_TIAN_MEN] > 0 && false==bWinTian) GameRecord.enOperateTianMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_DI_MEN]) GameRecord.enOperateDiMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_DI_MEN] > 0 && true==bWinDi) GameRecord.enOperateDiMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_DI_MEN] >0 && false==bWinDi) GameRecord.enOperateDiMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_XUAN_MEN]) GameRecord.enOperateXuanMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_XUAN_MEN] > 0 && true==bWinXuan) GameRecord.enOperateXuanMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_XUAN_MEN] >0 && false==bWinXuan) GameRecord.enOperateXuanMen=enOperateResult_Lost;


	if (0==m_lUserJettonScore[ID_HUANG_MEN]) GameRecord.enOperateHuangMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_HUANG_MEN] > 0 && true==bWinHuang) GameRecord.enOperateHuangMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_HUANG_MEN] >0 && false==bWinHuang) GameRecord.enOperateHuangMen=enOperateResult_Lost;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}

	int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( MAX_FALG_COUNT < nHistoryCount ) m_btScoreMoveR.EnableWindow(TRUE);

	//移到最新记录
	if ( MAX_FALG_COUNT < nHistoryCount )
	{
		m_nScoreHead = (m_nRecordLast - MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
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

	//下注信息
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//全体下注
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//所有玩家下注
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
	
	for ( int i = 0; i < CountArray(m_SkinListCtrl); ++i )
	{
		if ( m_SkinListCtrl[i].GetSafeHwnd() )
			m_SkinListCtrl[i].DeleteAllItems();
	}

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//效验参数
	ASSERT(cbViewIndex<=ID_HUANG_MEN);
	if (cbViewIndex>ID_HUANG_MEN) return;

	m_lUserJettonScore[cbViewIndex]=lJettonCount;

	//更新界面
	InvalidGameView(0,0,0,0);
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[5][5])
{
	if (cbTableCardArray!=NULL)
	{
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

		//开始发牌
		DispatchCard();
	}
	else
	{
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}

//设置筹码
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//效验参数
	ASSERT(cbViewIndex<=ID_HUANG_MEN);
	if (cbViewIndex>ID_HUANG_MEN) return;

	//变量定义
	bool bPlaceJetton=false;
	LONGLONG lScoreIndex[JETTON_COUNT]={100L,1000L,10000L,50000L,100000L,500000L,1000000L,5000000L};

	//边框宽度
	int nFrameWidth=0, nFrameHeight=0;
	int nBorderWidth=6;

	switch (cbViewIndex)
	{
	case ID_TIAN_MEN:
		{ 
			m_lAllJettonScore[ID_TIAN_MEN] += lScoreCount;
			nFrameWidth = m_rcTianMen.right-m_rcTianMen.left;
			nFrameHeight = m_rcTianMen.bottom-m_rcTianMen.top;
			break;
		}
	case ID_DI_MEN:
		{
			m_lAllJettonScore[ID_DI_MEN] += lScoreCount;
			nFrameWidth = m_rcDimen.right-m_rcDimen.left;
			nFrameHeight = m_rcDimen.bottom-m_rcDimen.top;
			break;
		}
	case ID_HUANG_MEN :
		{ 
			m_lAllJettonScore[ID_HUANG_MEN] += lScoreCount;
			nFrameWidth = m_rcHuangMen.right-m_rcHuangMen.left;
			nFrameHeight = m_rcHuangMen.bottom-m_rcHuangMen.top;
			break;
		}
	case ID_XUAN_MEN:
		{ 
			m_lAllJettonScore[ID_XUAN_MEN] += lScoreCount;
			nFrameWidth = m_rcXuanMen.right-m_rcXuanMen.left;
			nFrameHeight = m_rcXuanMen.bottom-m_rcXuanMen.top;
			break;
		}
	default:
		{
			ASSERT(FALSE);
			return;
		}
	}

	nFrameWidth += nBorderWidth;
	nFrameHeight += nBorderWidth;

	//增加判断
	bool bAddJetton=lScoreCount>0?true:false;
	lScoreCount= (lScoreCount>0?lScoreCount:-lScoreCount);

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
				int nJettonSize=46;
				JettonInfo.cbJettonIndex=cbScoreIndex;
				JettonInfo.nXPos=rand()%(nFrameWidth-nJettonSize-20)+6;
				JettonInfo.nYPos=rand()%(nFrameHeight-nJettonSize-23-14)+6;

				//插入数组
				bPlaceJetton=true;
				m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex-1].GetCount(); ++nIndex)
				{
					//移除判断
					tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex-1][nIndex];
					if (JettonInfo.cbJettonIndex==cbScoreIndex)
					{
						m_JettonInfoArray[cbViewIndex-1].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	//更新界面
	if (bPlaceJetton==true)InvalidGameView(0,0,0,0);

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
void CGameClientView::SetWinnerSide(bool bWinTian, bool bWinDi, bool bWinXuan,bool bWinHuang, bool bSet)
{	
	//设置时间
	if (true==bSet)
	{
		//设置定时器
		SetTimer(IDI_FLASH_WINNER,500,NULL);

		//全胜判断
		if (false==bWinTian && false==bWinDi && false==bWinXuan &&false ==bWinHuang )
		{
			//清除资源			
			m_ImageFrameTianMen.DestroyImage();
			m_ImageFrameDiMen.DestroyImage();
			m_ImageFrameHuangMen.DestroyImage();
			m_ImageFrameXuanMen.DestroyImage();			
			//重设资源
			HINSTANCE hInstance=AfxGetInstanceHandle();
			m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_EMN"));
			m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_EMN"));
			m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_EMN"));
			m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_EMN"));

		}
	}
	else 
	{
		//清楚定时器
		KillTimer(IDI_FLASH_WINNER);

		//全胜判断
		if (false==m_bWinXuanMen && false==m_bWinTianMen && false==m_bWinHuangMen)
		{
			//清除资源
			m_ImageFrameTianMen.DestroyImage();
			m_ImageFrameDiMen.DestroyImage();
			m_ImageFrameHuangMen.DestroyImage();
			m_ImageFrameXuanMen.DestroyImage();

			//重设资源
			HINSTANCE hInstance=AfxGetInstanceHandle();
			m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_MEN_EX"));
			m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_MEN_EX"));
			m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_MEN_EX"));
			m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_MEN_EX"));
		}
	}

	//设置变量
	m_bWinTianMen=bWinTian;
	m_bWinHuangMen=bWinHuang;
	m_bWinXuanMen=bWinXuan;
	m_bWinDiMen=bWinDi;
	m_bFlashResult=bSet;
	m_bShowGameResult=bSet;
	m_cbAreaFlash=0xFF;

	//更新界面
	InvalidGameView(0,0,0,0);

	return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
	if (m_rcTianMen.PtInRect(MousePoint))
	{
		return ID_TIAN_MEN;

	}else
	{
		if (m_rcDimen.PtInRect(MousePoint))
		{
			return ID_DI_MEN;

		}

	}
	if (m_rcHuangMen.PtInRect(MousePoint)) return ID_HUANG_MEN;	
	if (m_rcXuanMen.PtInRect(MousePoint)) return ID_XUAN_MEN ;	
	return 0xFF;
}
void CGameClientView::DrawType(CDC* pDC,WORD wChairID)
{
	if (false==m_bShowGameResult) return;

	for (int i = 0;i<5;i++)	
	{
		int iIndex = 0;
		//iIndex = CT_SPECIAL_NIUNIUXW-2;
		//m_lUserCardType[IDC_BANK]
		if(m_lUserCardType[i]!= CT_POINT)
		{
			iIndex = m_lUserCardType[i]-2;
			if(iIndex>=13)
			{
				if(iIndex == 13)
				{
					iIndex = 12;
					//break;
				}
				if(iIndex == 14)
				{
					iIndex = 11;
					//break;
				}
				if(iIndex == 15)
				{
					iIndex = 13;
					//	break;
				}
			}
			else
			{
				if(iIndex>10)
				{
					iIndex = 10;
				}
				if(iIndex<0)
				{
					iIndex = 0;
				}
			}
			//iIndex = 11;
		}
		
		//HandleImageCardType[i](&m_ImageCardType[i]);
		m_ImageCardType[i].TransDrawImage(pDC,m_CardTypePoint[i].x,m_CardTypePoint[i].y,m_ImageCardType[i].GetWidth()/14,
			m_ImageCardType[i].GetHeight(),m_ImageCardType[i].GetWidth()/14*iIndex,0,RGB(255,0,255));
	}
}

//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore)
{
	//加载资源
	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

	if ( bMeScore ) SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/10, m_ImageMeScoreNumber.GetHeight());

	//计算数目
	int lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	int nYDrawPos=nYPos-SizeScoreNumber.cy/2;
	int nXDrawPos=nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

	//绘画桌号
	for (int i=0;i<lNumberCount;i++)
	{
		//绘画号码
		int lCellNumber=(int)(lNumber%10);
		if ( bMeScore )
		{
			m_ImageMeScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0);
		}
		else
		{
			m_ImageScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0);
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
	CString strNumber, strTempNum;
	strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

	int nLength = strTempNum.GetLength();
	if (nLength <= 3)
		strNumber.Format(_T("%I64d"), lNumber);
	else
	{
		for (int i = 0; i < int((nLength-1)/3); i++)
			strTempNum.Insert(nLength - 3*(i+1), _T(","));
		strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
	}

	//输出数字	
	//pDC->TextOut(nXPos,nYPos,strNumber);
	CDFontEx::DrawText(this,pDC,12,400,strNumber,nXPos,nYPos,RGB(255,234,0),DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat)
{
	CString strNumber, strTempNum;
	strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

	int nLength = strTempNum.GetLength();
	if (nLength <= 3)
		strNumber.Format(_T("%I64d"), lNumber);
	else
	{
		for (int i = 0; i < int((nLength-1)/3); i++)
			strTempNum.Insert(nLength - 3*(i+1), _T(","));
		strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
	}

	//输出数字
	/*if (nFormat==-1) pDC->DrawText(strNumber,rcRect,DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
	else pDC->DrawText(strNumber,rcRect,nFormat);*/

	if (nFormat==-1) 
		CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,234,0), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
	else 
		CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,234,0), nFormat);
}
void CGameClientView::KillCardTime()
{
	KillTimer(IDI_FLASH_WINNER);
	KillTimer(IDI_POSTCARD);
	KillTimer(IDI_OPENCARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_SHOWDISPATCH_CARD_TIP);
	for (int i = 0;i<5;i++)
	{
	  m_CardControl[i].m_blShowResult = false;
	  m_CardControl[i].m_blShowLineResult = false;
	}
}
//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//闪动胜方
	if (nIDEvent==IDI_FLASH_WINNER)
	{
		//设置变量
		m_bFlashResult=!m_bFlashResult;

		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}
	if(IDI_POSTCARD ==nIDEvent)
	{

		if(m_bcShowCount<5)
		{
			if(m_CardControl[5].m_CardTempItemArray.GetCount()>0)
			{
				m_CardControl[5].m_blhideOneCard = false;
				m_CardControl[5].m_blGameEnd = true;
				m_CardControl[5].SetDisplayFlag(true);
				m_CardControl[5].ShowCardControl(true);
				m_CardControl[5].m_CardItemArray.SetSize(1);
				m_CardControl[5].m_CardItemArray[0]=m_CardControl[5].m_CardTempItemArray[0];
				m_CardControl[5].m_CardItemArray[0].cbCardData = m_bcfirstShowCard;
				m_CardControl[5].m_CardItemArray[0].bShoot = false;
			}
			
			if (m_bcShowCount == 2)
				m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		}else
		{
			m_CardControl[5].m_CardItemArray.SetSize(0);

			bool blKillTimer = true;
			for (int i = 0;i<5;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
				{
					blKillTimer = false;
					break;
				}
			}
			m_PostCardIndex = m_PostCardIndex%5;
			m_CardControl[m_PostCardIndex].OnCopyCard();
			m_PostCardIndex++;
			if(blKillTimer)
			{	
				KillTimer(IDI_POSTCARD);
				SetTimer(IDI_OPENCARD,25,NULL);
			}
			else
				m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

			InvalidGameView(0,0,0,0);
		}
		m_bcShowCount++;
	
	
	}
	if(IDI_OPENCARD==nIDEvent)
	{
		if(m_blAutoOpenCard == false)
		{
			while (1)
			{
				bool blCopy = false;
				for (int i = 0;i<5;i++)
				{
					if(m_CardControl[i].m_CardItemArray.GetCount()!=5)
					{
						blCopy = true;
						break;
					}
					if(m_CardControl[i].m_CardTempItemArray.GetCount()==0)
					{
						blCopy = false;
						break;
					}
				}
				if(false == blCopy)
				{
					break;
				}else
				{
					m_CardControl[i].OnCopyCard();
				}
			}

			for (int i = 0;i<5;i++)
			{
				m_CardControl[i].m_blGameEnd = false;
				m_CardControl[i].m_blhideOneCard = false;
			}
			//更新界面
			InvalidGameView(0,0,0,0);
			KillTimer(IDI_OPENCARD);

			SetTimer(IDI_DISPATCH_CARD,10000,NULL);

			return ;

		}
		{
			if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x<40&&m_CardControl[m_OpenCardIndex%5].m_blAdd)
			{
				m_CardControl[m_OpenCardIndex%5].m_blGameEnd = false;
				m_CardControl[m_OpenCardIndex%5].m_MovePoint.x+=2;
				m_CardControl[m_OpenCardIndex%5].m_blhideOneCard = false;
				//更新界面
				InvalidGameView(0,0,0,0);


			}else
			{

				m_CardControl[m_OpenCardIndex%5].m_blAdd = false;
				m_CardControl[m_OpenCardIndex%5].m_blGameEnd = false;
				if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x>10)
				m_CardControl[m_OpenCardIndex%5].m_MovePoint.x-=2;
				m_CardControl[m_OpenCardIndex%5].m_blhideOneCard = false;
				InvalidGameView(0,0,0,0);
				if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x<=10)
				{
					m_CardControl[m_OpenCardIndex%5].m_tMoveCard.m_blMoveCard = true;
					m_CardControl[m_OpenCardIndex%5].m_tMoveCard.m_blMoveFinish = true;
					m_CardControl[m_OpenCardIndex%5].m_MovePoint.x = 0;
					m_CardControl[m_OpenCardIndex%5].m_MovePoint.y = 0;
					m_CardControl[m_OpenCardIndex%5].m_blAdd = true;
					m_CardControl[m_OpenCardIndex%5].m_blGameEnd = true;

					if(m_lUserCardType[m_OpenCardIndex%5]==CT_POINT&&m_lUserCardType[m_OpenCardIndex%5]==CT_SPECIAL_BOMEBOME)
					{
						m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = true;
					}else
					{
						m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = false;

					}
					//BYTE bcTmp[5];
					//BYTE bcCardDataTmp[5];
					//m_CardControl[m_OpenCardIndex%5].GetCardData(bcCardDataTmp,5);
				/*	int iType = m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);
					
					BYTE bcResult[5];
					if(iType==CT_POINT)
					{
						m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = true;
						CopyMemory(bcResult,bcTmp,5);
					}else
					{
						CopyMemory(bcResult,bcTmp+3,2);
						CopyMemory(bcResult+2,bcTmp,3);

					}*/
					
					m_CardControl[m_OpenCardIndex%5].SetCardData(m_cbTableSortCardArray[m_OpenCardIndex%5],5,false);
					m_CardControl[m_OpenCardIndex%5].m_blShowResult = true;
					m_OpenCardIndex++;

					InvalidGameView(0,0,0,0);

					if(m_OpenCardIndex==6)
					{
						/*for (int i = 0;i<5;i++)
						{
							m_CardControl[i].m_blGameEnd = true;
							BYTE bcTmp[5];
							BYTE bcCardDataTmp[5];
							m_CardControl[i].GetCardData(bcCardDataTmp,5);
							m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);
							m_CardControl[i].SetCardData(bcTmp,5,false);
						}*/
						KillTimer(IDI_OPENCARD);
						InvalidGameView(0,0,0,0);

						//设置显示牌定时器
						SetTimer(IDI_DISPATCH_CARD,30,NULL);
					}

				}

			}
		}
	}
	//轮换庄家
	else if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )
	{
		ShowChangeBanker( false );

		return;
	}
	else if (nIDEvent==IDI_DISPATCH_CARD)
	{

		//完成发牌
		FinishDispatchCard(true);
		
	/*	while (1)
		{
			bool blCopy = false;
			for (int i = 0;i<5;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=5)
				{
					blCopy = true;
					break;
				}
				if(m_CardControl[i].m_CardTempItemArray.GetCount()==0)
				{
					blCopy = false;
					break;
				}
			}
			if(false == blCopy)
			{
				break;
			}else
			{
				m_CardControl[i].OnCopyCard();
			}
		}*/

		{
			for (int i = 0;i<5;i++)
			{
				m_CardControl[i].m_blGameEnd = true;
				m_CardControl[i].m_blhideOneCard = false;

				if(m_lUserCardType[i]==CT_POINT||m_lUserCardType[i]==CT_SPECIAL_BOMEBOME)
				{
					m_CardControl[i].m_blShowLineResult = true;
				}else
				{
					m_CardControl[i].m_blShowLineResult = false;

				}
			//	if (m_blAutoOpenCard==false)
				{
				//	BYTE bcTmp[5];
					//BYTE bcCardDataTmp[5];
				/*	m_CardControl[i].GetCardData(bcCardDataTmp,5);
					int iType = m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);
					BYTE bcResult[5];

					if(iType==CT_POINT)
					{
						m_CardControl[i].m_blShowLineResult = true;
						CopyMemory(bcResult,bcTmp,5);
					}else
					{
						CopyMemory(bcResult,bcTmp+3,2);
						CopyMemory(bcResult+2,bcTmp,3);
					}*/
					m_CardControl[i].SetCardData(m_cbTableSortCardArray[i],5,false);
					//m_CardControl[i].SetCardData(bcResult,5,false);
					m_CardControl[i].m_blShowResult = true;
				}
			}
			KillTimer(IDI_OPENCARD);
			InvalidGameView(0,0,0,0);

		}
		m_blMoveFinish = true;
		//更新界面
		InvalidGameView(0,0,0,0);

		return;
	}
	else if (IDI_SHOWDISPATCH_CARD_TIP==nIDEvent)
	{
		SetDispatchCardTip(enDispatchCardTip_NULL);

	}else if(nIDEvent==IDI_MOVECARD_END)
	{
		KillTimer(IDI_MOVECARD_END);
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{	

	for (int i = 0;i<5;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonDown(nFlags, Point);
		}
	}
		
	if (m_lCurrentJetton!=0L)
	{
		int iTimer = 1;
		//下注区域
		BYTE cbJettonArea=GetJettonArea(Point);
	
		//最大下注
		LONGLONG lMaxJettonScore=GetUserMaxJetton();

		if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore)
		{		
			return ;
		}
		//合法判断
		//if (lMaxJettonScore < m_lCurrentJetton*iTimer) return ;

		//发送消息
		if (cbJettonArea!=0xFF) SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,0);
	}
		InvalidGameView(0,0,0,0);

	__super::OnLButtonDown(nFlags,Point);
}
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	
	for (int i = 0;i<5;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonUp(nFlags, Point);
			InvalidGameView(0,0,0,0);
		}
		
	}
	__super::OnLButtonUp(nFlags,Point);
}
//接受其他控件传来的消息
LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
{
	CPoint *pPoint = (CPoint*)lParam;
	ScreenToClient(pPoint);
    OnLButtonUp(1,*pPoint);
	return 1;

}
//鼠标移动消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	for (int i = 0;i<5;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnMouseMove(nFlags, point);
		InvalidGameView(0,0,0,0);

		}

	}
	__super::OnMouseMove(nFlags,point);
}
//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置变量
	m_lCurrentJetton=0L;

	if (m_pGameClientDlg->GetGameStatus()!=GAME_SCENE_GAME_END && m_cbAreaFlash!=0xFF)
	{
		m_cbAreaFlash=0xFF;
		InvalidGameView(0,0,0,0);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_lCurrentJetton!=0L)
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

		//区域判断
		if (cbJettonArea==0xFF)
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		//最大下注
		LONGLONG lMaxJettonScore=GetUserMaxJetton();

		//合法判断
		int iTimer = 1;
		if ((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore || lMaxJettonScore < m_lCurrentJetton)
		{
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
		case 50000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_50000)));
				return TRUE;
			}
		case 100000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
				return TRUE;
			}
		case 500000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500000)));
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
		}
	}
	else
	{
		ClearAreaFlash();
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

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	//pDC->SetTextColor(crFrame);
	//for (int i=0;i<CountArray(nXExcursion);i++)
	//{
	//	pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	//}

	////绘画字体
	//pDC->SetTextColor(crText);
	//pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}
//庄家信息
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore) 
{
	//庄家椅子号
	WORD wBankerUser=INVALID_CHAIR;

	//查找椅子号
	if (0!=dwBankerUserID)
	{
		for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
		{
			IClientUserItem *pUserItem = GetClientUserItem(wChairID);
			if(pUserItem == NULL) continue;
			tagUserInfo  *pUserData=pUserItem->GetUserInfo();
			if (NULL!=pUserData && dwBankerUserID==pUserData->dwUserID)
			{
				wBankerUser=wChairID;
				break;
			}
		}
	}

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
	//非空判断
	//m_nRecordLast = 14;
	if (m_nRecordLast==m_nRecordFirst) return;

	//资源变量

	int nIndex = m_nScoreHead;
	COLORREF static clrOld ;

	clrOld = pDC->SetTextColor(RGB(255,234,0));
	int nDrawCount=0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < MAX_FALG_COUNT )
	{
		//胜利标识
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		bool  static bWinMen[4];
		bWinMen[0]=ClientGameRecord.bWinTianMen;
		bWinMen[1]=ClientGameRecord.bWinDiMen ;
		bWinMen[2]=ClientGameRecord.bWinXuanMen;
		bWinMen[3]=ClientGameRecord.bWinHuangMen;

		//操作结果
		enOperateResult static OperateResult[4];
		OperateResult[0]=ClientGameRecord.enOperateTianMen;
		OperateResult[1]=ClientGameRecord.enOperateDiMen;
		OperateResult[2]=ClientGameRecord.enOperateXuanMen;
		OperateResult[3]=ClientGameRecord.enOperateHuangMen;

		for (int i=0; i<4; ++i)
		{
			//位置变量
			int static nYPos=0,nXPos=0;
			nYPos=m_nWinFlagsExcursionY+i*27;
			nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 27;

			//胜利标识
			int static nFlagsIndex=0;
			if (false==bWinMen[i])
			{
				nFlagsIndex=2;
			}
			else
			{
				nFlagsIndex=0;
			}
			if (OperateResult[i] != enOperateResult_NULL)
			{			
				nFlagsIndex+=1;

				if(nFlagsIndex>=4)
				{
					nFlagsIndex=3;
				}
			}
			//绘画标识
			m_ImageWinFlags.TransDrawImage( pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/4 , 
				m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/4 * nFlagsIndex, 0, RGB(255, 0, 255));
		}

		//移动下标
		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}
	pDC->SetTextColor(clrOld);
}
//手工搓牌
afx_msg void CGameClientView::OnOpenCard()
{
	SendEngineMessage(IDM_OPEN_CARD,0,0);

}
//自动搓牌
afx_msg void CGameClientView::OnAutoOpenCard()
{
	SendEngineMessage(IDM_AUTO_OPEN_CARD,0,0);

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
	if ( nHistoryCount == MAX_FALG_COUNT ) return;

	m_nScoreHead = ( m_nScoreHead + 1 ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount-1 == MAX_FALG_COUNT ) m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);

	//更新界面
	InvalidGameView(0,0,0,0);
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
	//显示判断
	//if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END) return;
	if (false==m_bShowGameResult) return;

	int	nXPos = nWidth / 2 - 129;
	int	nYPos = nHeight / 2 - 148;

	CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
	DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
	m_ImageGameEnd.DrawImage(pDC, nXPos+2, nYPos+70);

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

	for(int l=lpRect->top; l<lpRect->bottom; l++)
	{
		for(int k=lpRect->left; k<lpRect->right; k++)
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
LONGLONG CGameClientView::GetUserMaxJetton()
{
	int iTimer = 10;
	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex]*iTimer;

	//庄家金币
	LONGLONG lBankerScore=2147483647;
	if (m_wBankerUser!=INVALID_CHAIR) lBankerScore=m_lBankerScore;
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lBankerScore-=m_lAllJettonScore[nAreaIndex]*iTimer;

	//区域限制
	LONGLONG lMeMaxScore;
	if((m_lMeMaxScore-lNowJetton)/iTimer>m_lAreaLimitScore)
	{
		lMeMaxScore= m_lAreaLimitScore*iTimer;

	}else
	{
		lMeMaxScore = m_lMeMaxScore-lNowJetton;
		lMeMaxScore = lMeMaxScore;
	}

	//庄家限制
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	lMeMaxScore /= iTimer; 

	//非零限制
	//ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
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
	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
	{
		if (m_lUserJettonScore[nAreaIndex] > 0 )
			DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x,m_PointJettonNumber[nAreaIndex-1].y+25, true);
	}
}

//开始发牌
void CGameClientView::DispatchCard()
{
	//设置界面
	for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].m_CardItemArray.SetSize(0);
	//设置标识
	m_bNeedSetGameRecord=true;
}

//结束发牌
void CGameClientView::FinishDispatchCard(bool bNotScene)
{
	//完成判断
	if (m_bNeedSetGameRecord==false) return;

	//设置标识
	m_bNeedSetGameRecord=false;

	//删除定时器
	KillTimer(IDI_DISPATCH_CARD);

	////设置扑克
	//for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetCardData(m_cbTableCardArray[i],2);

	//推断赢家
	bool bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang;

	BYTE TianMultiple,diMultiple,TianXuanltiple,HuangMultiple;

	DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang,TianMultiple,diMultiple,TianXuanltiple,HuangMultiple);

	//保存记录
	if (bNotScene)
		SetGameHistory(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang);

	//累计积分
	m_lMeStatisticScore+=m_lMeCurGameScore;
	m_lBankerWinScore=m_lTmpBankerWinScore;

	//设置赢家
	SetWinnerSide(bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang, true);

	//播放声音
	if (m_lMeCurGameScore>0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
	else if (m_lMeCurGameScore<0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
	else m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
}

//终止动画
void CGameClientView::StopAnimal()
{
	//定时器
	KillTimer(IDI_POSTCARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_FLASH_WINNER);
	KillTimer(IDI_OPENCARD);

	FinishDispatchCard(true);

	for (int i = 0;i<4;i++)
	{
		m_CardControl[i].m_blGameEnd = true;
		m_CardControl[i].m_blhideOneCard = false;
	}
	m_blMoveFinish = true;
	m_bFlashResult = false;
}

//胜利边框
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
	//合法判断
	if (m_cbAreaFlash==0xFF && false==m_bFlashResult) return;

	//下注判断
	if (false==m_bFlashResult)
	{
		if (m_pGameClientDlg->GetGameStatus()==GAME_SCENE_PLACE_JETTON)
		{
			switch (m_cbAreaFlash)
			{
			case ID_TIAN_MEN:	
				{
					//绘画图片

					m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
					break;
				}
			case ID_DI_MEN:
				{
					//绘画图片

					m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);
					break;
				}
			case ID_XUAN_MEN:
				{	
					//绘画图片

					m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);
					break;
				}
			case ID_HUANG_MEN:
				{
					//绘画图片
	
					m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
					break;
				}
			}
		}
		
	}
	else
	{
		//庄全胜判断
		if (false==m_bWinXuanMen && false==m_bWinTianMen && false==m_bWinHuangMen&&m_bWinDiMen == false)
		{
			//加载资源


			//绘画图片
			m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
			m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);	
			m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
			m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);
		}
		else
		{
			//门判断
			if (true==m_bWinXuanMen)
			{

				m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);
			}
			if (true==m_bWinTianMen)
			{

				m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
			}
			if (true==m_bWinHuangMen)
			{

				m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
			}
			if (true==m_bWinDiMen)
			{

				m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);
			}
		}
	}
}


//推断赢家
void CGameClientView::DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan,bool &bWinHuan,BYTE &TianMultiple,BYTE &diMultiple,BYTE &TianXuanltiple,BYTE &HuangMultiple )
{
	//大小比较
	bWinTian=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[SHUN_MEN_INDEX],5,TianMultiple)==1?true:false;
	bWinDi=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DUI_MEN_INDEX],5,diMultiple)==1?true:false;
	bWinXuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DAO_MEN_INDEX],5,TianXuanltiple)==1?true:false;
	bWinHuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[HUAN_MEN_INDEX],5,HuangMultiple)==1?true:false;
}

////控件命令
//BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	//获取ID
//	WORD wControlID=LOWORD(wParam);
//
//	//控件判断
//	switch (wControlID)
//	{
//	case IDC_JETTON_BUTTON_100:
//		{
//			//设置变量
//			m_lCurrentJetton=100L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_1000:
//		{
//			//设置变量
//			m_lCurrentJetton=1000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_10000:
//		{
//			//设置变量
//			m_lCurrentJetton=10000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_50000:
//		{
//			//设置变量
//			m_lCurrentJetton=50000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_100000:
//		{
//			//设置变量
//			m_lCurrentJetton=100000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_500000:
//		{
//			//设置变量
//			m_lCurrentJetton=500000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_1000000:
//		{
//			//设置变量
//			m_lCurrentJetton=1000000L;
//			break;
//		}
//	case IDC_JETTON_BUTTON_5000000:
//		{
//			//设置变量
//			m_lCurrentJetton=5000000L;
//			break;
//		}
//	case IDC_AUTO_OPEN_CARD:
//		{
//			break;
//		}
//	case IDC_OPEN_CARD:
//		{
//			break;
//		}
//	case IDC_BANK:
//		{
//			break;
//		}
//	case IDC_CONTINUE_CARD:
//		{
//			SendEngineMessage(IDM_CONTINUE_CARD,0,0);
//			break;
//		}
//	}
//
//	return CGameFrameView::OnCommand(wParam, lParam);
//}

//我的位置
void CGameClientView::SetMeChairID(WORD dwMeUserID)
{
	m_wMeChairID=dwMeUserID;
}

//设置提示
void CGameClientView::SetDispatchCardTip(enDispatchCardTip DispatchCardTip)
{
	//设置变量
	m_enDispatchCardTip=DispatchCardTip;

	//设置定时器
	if (enDispatchCardTip_NULL!=DispatchCardTip) SetTimer(IDI_SHOWDISPATCH_CARD_TIP,2*1000,NULL);		
	else KillTimer(IDI_SHOWDISPATCH_CARD_TIP);

	//更新界面
	InvalidGameView(0,0,0,0);
}

//绘画庄家
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
	//庄家信息																											
	pDC->SetTextColor(RGB(255,234,0));

	IClientUserItem *pUserItem =NULL;
	if(m_wBankerUser != INVALID_CHAIR)
		pUserItem = GetClientUserItem(m_wBankerUser);
	//获取玩家
	tagUserInfo const *pUserData = pUserItem==NULL ? NULL : pUserItem->GetUserInfo();

	//位置信息
	CRect static StrRect;
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 328;
	StrRect.right = StrRect.left + 104;
	StrRect.bottom = StrRect.top + 15;

	//庄家名字
	//pDC->DrawText(pUserData==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->szNickName, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

	CDFontEx::DrawText(this,pDC,  12, 400, pUserData==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->szNickName, StrRect, RGB(255,234,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE);

	//庄家总分
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 306;
	StrRect.right = StrRect.left + 104;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,pUserData==NULL?0:pUserData->lScore, StrRect);

	//庄家成绩
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 255;
	StrRect.right = StrRect.left + 104;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);

	//庄家局数
	StrRect.left = nWidth/2-293;
	StrRect.top = nHeight/2 - 280;
	StrRect.right = StrRect.left + 104;
	StrRect.bottom = StrRect.top + 15;
	DrawNumberStringWithSpace(pDC,m_wBankerTime,StrRect);
}

void CGameClientView::SetFirstShowCard(BYTE bcCard)
{
	m_bcfirstShowCard =  bcCard;
	m_CardControl[5].SetCardData(NULL,0);
	m_CardControl[5].SetCardData(&bcCard,1);
	m_CardControl[5].m_blGameEnd = true;
	m_CardControl[5].SetDisplayFlag(true);
	m_CardControl[5].ShowCardControl(true);
}

//绘画玩家
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return;

	const tagUserInfo *pMeUserData = GetClientUserItem(m_wMeChairID)->GetUserInfo();

	if ( pMeUserData != NULL )
	{
		//字体颜色
		pDC->SetTextColor(RGB(255,234,0));

		//总共下注
		LONGLONG lMeJetton=0L;
		for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lMeJetton += m_lUserJettonScore[nAreaIndex];

		//设置位置
		CRect static rcDraw;
		rcDraw.left=m_MeInfoRect.left;
		rcDraw.top=m_MeInfoRect.top;
		rcDraw.right=rcDraw.left+114;
		rcDraw.bottom=rcDraw.top+15;

		//绘画帐号
		//pDC->DrawText(pMeUserData->szNickName,lstrlen(pMeUserData->szNickName),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		CDFontEx::DrawText(this,pDC,  12, 400, pMeUserData->szNickName, rcDraw, RGB(255,234,0), DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//绘画积分
		rcDraw.top=m_MeInfoRect.top+21;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,pMeUserData->lScore-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//绘画成绩
		rcDraw.top=m_MeInfoRect.top+2*21;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	}
}

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
		ShowInsureSave(pIClientKernel,CPoint(btRect.left-200,btRect.top));
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
		ShowInsureGet(pIClientKernel,CPoint(btRect.left-200,btRect.top));
	}
#endif
}

void CGameClientView::OnUp()
{
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nPos = m_ApplyUser.m_AppyUserList.GetScrollPos(SB_VERT);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);

}

void CGameClientView::OnDown()
{
	tagSCROLLINFO scrollInfo = {};
	m_ApplyUser.m_AppyUserList.GetScrollInfo(SB_VERT, &scrollInfo);
	int nPos = scrollInfo.nPos - scrollInfo.nMin;
	if(m_ApplyUser.m_AppyUserList.GetItemCount()>MAX_APPLY_DISPLAY)
	{
		if(nPos>m_ApplyUser.m_AppyUserList.GetItemCount()-7)
		{
			return ;
		}
	}
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);
};

void CGameClientView::OpenAdminWnd()
{
	//如果有此权限
	if(m_pClientControlDlg != NULL )
	{
		if(!m_pClientControlDlg->IsWindowVisible()) 
		{
			m_pClientControlDlg->UpdateControl();
			m_pClientControlDlg->ShowWindow(SW_SHOW);
		}
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
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

	int nListIndex = cbViewIndex-1;
	//int nListIndex = 0;
	/*if ( cbViewIndex == AREA_XIAN )
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
		ASSERT(FALSE);*/

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
//////