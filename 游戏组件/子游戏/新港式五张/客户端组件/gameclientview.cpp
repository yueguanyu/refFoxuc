#include "StdAfx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//动画标识
#define IDI_HIDE_CARD					100								//隐藏扑克
#define IDI_SEND_CARD					101								//发牌标识
#define IDI_USER_ACTION					102								//动作标识
#define IDI_MOVE_CHIP					103								//筹码移动

//动画数目
#define SEND_CARD_COUNT					5								//发牌数目

//位置定义
#define CARD_EXCURSION_X				283								//扑克偏移
#define CARD_EXCURSION_Y				177								//扑克偏移

//发牌定义
#define SEND_STEP_COUNT					8								//步数
#define SPEED_SEND_CARD					10								//发牌速度

//筹码定义
#define CHIP_COUNT						14								//筹码数量
#define PILING_HEIGHT					6								//堆积高度


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)

	//系统消息
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()

	//按钮消息
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_CONTROL_FOLLOW, OnBnClickedFollow)
	ON_BN_CLICKED(IDC_CONTROL_GIVEUP, OnBnClickedGiveUp)
	ON_BN_CLICKED(IDC_CONTROL_SHOWHAND, OnBnClickedShowHand)

	ON_BN_CLICKED(IDC_ADD_SCORE, OnBnClickedAddScore)
	ON_BN_CLICKED(IDC_ADD_SCORE_ONE, OnBnClickedAddScoreOne)
	ON_BN_CLICKED(IDC_ADD_SCORE_TWO, OnBnClickedAddScoreTwo)
	ON_BN_CLICKED(IDC_ADD_SCORE_THREE, OnBnClickedAddScoreThree)
	ON_BN_CLICKED(IDC_ADD_SCORE_FOUR, OnBnClickedAddScoreFour)

	ON_BN_CLICKED(IDC_CONTROL_ADD, OnBnClickedADD)
	ON_BN_CLICKED(IDC_CONTROL_SUB, OnBnClickedSUB)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//状态变量
	m_bFirstChange = TRUE;
	m_lCellScore=0L;
	m_lDrawMaxScore=0L;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserAddScore,sizeof(m_lUserAddScore));
	ZeroMemory(m_lUserAddAllScore,sizeof(m_lUserAddAllScore));
	ZeroMemory(m_lUserSaveScore,sizeof(m_lUserSaveScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lOverScore,sizeof(m_lOverScore));
	ZeroMemory(m_lOverSaveScore,sizeof(m_lOverSaveScore));
	

	//动画变量
	m_SendCardPos.SetPoint(0,0);
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = SEND_STEP_COUNT;
	m_nXStep = 0;
	m_nYStep = 0;

	//动作变量
	m_cbUserAction=0;
	m_wActionUser=INVALID_CHAIR;

	//状态变量
	m_bShowScore=false;
	ZeroMemory(m_pHistoryScore,sizeof(m_pHistoryScore));

	//扑克筹码
	ZeroMemory(m_lPalyBetChip,sizeof(m_lPalyBetChip));
	m_lALLBetChip = 0;

	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		m_CardControl[i].Initialize( i != 2 );
	}

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageReady.LoadImage(AfxGetInstanceHandle(), TEXT("READY"));

	m_ImageGameCard.LoadImage(AfxGetInstanceHandle(), TEXT("GAME_CARD"));
	m_ImageViewFill.LoadFromResource(hResInstance,IDB_VIEW_FILL);
	m_ImageViewBack.LoadImage(hResInstance,TEXT("VIEW_BACK"));
	m_ImgaeScoreTotalNumber.LoadImage(hResInstance,TEXT("SCORE_NUMBER"));
	m_ImageActionBack.LoadImage(hResInstance,TEXT("ACTION_BACK"));
	m_ImageActionFont.LoadImage(hResInstance,TEXT("ACTION_FONT"));
	m_ImageShowHand.LoadImage(hResInstance,TEXT("SHOW_HAND"));

	m_ImageUserInfoH.LoadImage(hResInstance,TEXT("USER_INFO_H"));
	m_ImageUserInfoV.LoadImage(hResInstance,TEXT("USER_INFO_V"));
	m_ImageUserPalyH.LoadImage(hResInstance,TEXT("USERPLAYING_INFO_H"));
	m_ImageUserPalyV.LoadImage(hResInstance,TEXT("USERPLAYING_INFO_V"));

	m_ImageChip.LoadImage(hResInstance,TEXT("JETTON_VIEW"));

	m_ImageCellScore.LoadImage(hResInstance,TEXT("CELL_SCORE"));
	m_ImgaeScoreTotal.LoadImage(hResInstance,TEXT("SCORE_TOTAL"));

	m_ImageCellNumber.LoadImage(hResInstance,TEXT("CELL_NUMBER"));

	m_ImgaeScoreBackR.LoadImage(hResInstance,TEXT("GAME_SCORE_R"));
	m_ImgaeScoreBackL.LoadImage(hResInstance,TEXT("GAME_SCORE_L"));
	m_ImgaeScoreBackM.LoadImage(hResInstance,TEXT("GAME_SCORE_M"));

	m_ImgaeScoreSum.LoadImage(hResInstance,TEXT("SUM_IMSUM"));
	m_ImgaeScoreWin.LoadImage(hResInstance,TEXT("WIN_NUMBER"));
	m_ImgaeScoreLose.LoadImage(hResInstance,TEXT("LOSE_NUMBER"));

	m_ImageTime.LoadImage(hResInstance,TEXT("TIME_NUMBER"));

	m_ImageCradType.LoadImage(hResInstance,TEXT("CARD_TYPE"));

	//获取大小
	m_SizeGameCard.SetSize(m_ImageGameCard.GetWidth()/13,m_ImageGameCard.GetHeight()/5);

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
	//删除筹码
	for ( int i = 0 ; i < m_ArrayPlayChip.GetCount(); ++i )
		SafeDelete(m_ArrayPlayChip[i]);
	m_ArrayPlayChip.RemoveAll();

	for ( int i = 0 ; i < m_ArrayBetChip.GetCount(); ++i )
		SafeDelete(m_ArrayBetChip[i]);
	m_ArrayBetChip.RemoveAll();

	for ( int i = 0 ; i < m_ArrayOverChip.GetCount(); ++i )
		SafeDelete(m_ArrayOverChip[i]);
	m_ArrayOverChip.RemoveAll();
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN 
		&& m_PlayOperate.IsShow() && m_PlayOperate.IsAddShow() )
	{
		//发送消息
		SendEngineMessage(IDM_FOLLOW,(WPARAM)m_PlayOperate.GetBetCount(),0);
	}

	if ( m_PlayOperate.PreTranslateMessage(pMsg) )
	{
		CRect rect = m_PlayOperate.GetOperateRect();
		InvalidGameView(rect.left, rect.top, rect.Width(), rect.Height());
		return TRUE;
	}

	bool bUpdateGameView = false;
	if ( pMsg->message == WM_CHAR )
	{
		bUpdateGameView = true;

	}

	BOOL bReturn = __super::PreTranslateMessage(pMsg);

	if( bUpdateGameView )
	{
		CRect rect = m_PlayOperate.GetOperateRect();
		InvalidGameView(rect.left, rect.top, rect.Width(), rect.Height());
	}

	return bReturn;
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	__super::ResetGameView();

	//删除时间
	KillTimer(IDI_HIDE_CARD);
	KillTimer(IDI_SEND_CARD);
	KillTimer(IDI_USER_ACTION);

	//状态变量
	m_lCellScore=0L;
	m_lDrawMaxScore=0L;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserAddScore,sizeof(m_lUserAddScore));
	ZeroMemory(m_lUserAddAllScore,sizeof(m_lUserAddAllScore));
	ZeroMemory(m_lUserSaveScore,sizeof(m_lUserSaveScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lOverScore,sizeof(m_lOverScore));
	ZeroMemory(m_lOverSaveScore,sizeof(m_lOverSaveScore));

	//动画变量
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = 0;
	m_nXStep = 0;
	m_nYStep = 0;
	m_SendCardItemArray.RemoveAll();

	//动作变量
	m_cbUserAction=0;
	m_wActionUser=INVALID_CHAIR;

	//状态变量
	m_bShowScore=false;
	ZeroMemory(m_pHistoryScore,sizeof(m_pHistoryScore));

	//控制按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_PlayOperate.ShowOperate(false);

	//禁止按钮
	m_PlayOperate.m_btFollow.EnableWindow(FALSE);
	m_PlayOperate.m_btAddScore.EnableWindow(FALSE);
	m_PlayOperate.m_btShowHand.EnableWindow(FALSE);

	//扑克筹码
	ZeroMemory(m_lPalyBetChip,sizeof(m_lPalyBetChip));
	m_lALLBetChip = 0;

	//界面控件
	for (WORD i=0;i<GAME_PLAYER;i++)
		m_CardControl[i].SetCardData(NULL,0);

	//用户扑克
	m_CardControl[MYSELF_VIEW_ID].SetPositively(false);
	m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(false);

	//删除筹码
	for ( int i = 0 ; i < m_ArrayPlayChip.GetCount(); ++i )
		SafeDelete(m_ArrayPlayChip[i]);
	m_ArrayPlayChip.RemoveAll();

	for ( int i = 0 ; i < m_ArrayBetChip.GetCount(); ++i )
		SafeDelete(m_ArrayBetChip[i]);
	m_ArrayBetChip.RemoveAll();

	for ( int i = 0 ; i < m_ArrayOverChip.GetCount(); ++i )
		SafeDelete(m_ArrayOverChip[i]);
	m_ArrayOverChip.RemoveAll();

	return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//准备位置
	m_ptReady[0].SetPoint(nWidth/2 - 220, nHeight/2 - 140);
	m_ptReady[1].SetPoint(nWidth/2 - 235, nHeight/2 + 30);
	m_ptReady[2].SetPoint(nWidth/2 - 1,   nHeight/2 + 190 + 18);
	m_ptReady[3].SetPoint(nWidth/2 + 235, nHeight/2 + 30);
	m_ptReady[4].SetPoint(nWidth/2 + 220, nHeight/2 - 140);

	// 头像
	m_ptUserInfo[0].SetPoint(nWidth/2 - 220, nHeight/2 - 250);
	m_ptUserInfo[1].SetPoint(nWidth/2 - 335, nHeight/2 + 30);
	m_ptUserInfo[2].SetPoint(nWidth/2,       nHeight/2 + 240 + 18);
	m_ptUserInfo[3].SetPoint(nWidth/2 + 335, nHeight/2 + 30);
	m_ptUserInfo[4].SetPoint(nWidth/2 + 220, nHeight/2 - 250);

	//时间位置
	m_ptClock[0].SetPoint(nWidth/2 - 325, nHeight/2 - 252);
	m_ptClock[1].SetPoint(nWidth/2 - 345, nHeight/2 - 67);
	m_ptClock[2].SetPoint(nWidth/2 - 105, nHeight/2 + 235 + 18);
	m_ptClock[3].SetPoint(nWidth/2 + 345, nHeight/2 - 67);
	m_ptClock[4].SetPoint(nWidth/2 + 325, nHeight/2 - 252);

	//动作位置
	m_ptUserAction[0].SetPoint(nWidth/2 - 90, nHeight/2 - 245);
	m_ptUserAction[1].SetPoint(nWidth/2 - 335, nHeight/2 - 55);
	m_ptUserAction[2].SetPoint(nWidth/2 - 130, nHeight/2 + 190 + 18);
	m_ptUserAction[3].SetPoint(nWidth/2 + 335, nHeight/2 - 55);
	m_ptUserAction[4].SetPoint(nWidth/2 + 90, nHeight/2 - 245);


	//扑克控件
	m_CardControl[0].SetBenchmarkPos(nWidth/2 - 220, nHeight/2 - 200,	enXCenter,enYTop);
	m_CardControl[1].SetBenchmarkPos(nWidth/2 - 293, nHeight/2 + 30,	enXLeft,enYCenter);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,		 nHeight/2 + 193 + 18,	enXCenter,enYBottom);
	m_CardControl[3].SetBenchmarkPos(nWidth/2 + 293, nHeight/2 + 30,	enXRight,enYCenter);
	m_CardControl[4].SetBenchmarkPos(nWidth/2 + 220, nHeight/2 - 200,	enXCenter,enYTop);

	//玩家筹码
	m_ptPalyBetChip[0].SetPoint(nWidth/2 - 122, nHeight/2 - 125);
	m_ptPalyBetChip[1].SetPoint(nWidth/2 - 120,	nHeight/2 + 30);
	m_ptPalyBetChip[2].SetPoint(nWidth/2,		nHeight/2 + 45 + 18);
	m_ptPalyBetChip[3].SetPoint(nWidth/2 + 120,	nHeight/2 + 30);
	m_ptPalyBetChip[4].SetPoint(nWidth/2 + 122, nHeight/2 - 125);

	// 结束积分
	m_ptOverScore[0].SetPoint(nWidth/2 - 220, nHeight/2 - 75);
	m_ptOverScore[1].SetPoint(nWidth/2 - 205, nHeight/2 + 102);
	m_ptOverScore[2].SetPoint(nWidth/2,       nHeight/2 + 290 + 18);
	m_ptOverScore[3].SetPoint(nWidth/2 + 205, nHeight/2 + 102);
	m_ptOverScore[4].SetPoint(nWidth/2 + 220, nHeight/2 - 75);

	//发牌起始位置
	m_SendCardPos.SetPoint(nWidth / 2 + 20,nHeight / 2 - 200);
	
	//开始按钮
	m_btStart.SetWindowPos( NULL,nWidth/2 + 88,nHeight/2 + 150 + 22,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

	//控制窗口
	m_PlayOperate.SetOperatePos( nWidth/2 + 100, nHeight/2+120 + 18, PYOE_LEFT|PYOE_TOP);

	// 桌面筹码
	m_rectBetChip.SetRect( nWidth / 2 - 75,nHeight / 2 - 140, nWidth / 2 + 75,nHeight / 2 - 10 + 18);

	// 变换筹码位置
	for ( int i = 0 ; i < m_ArrayBetChip.GetCount(); ++i )
	{
		CPoint ptOffset( (nWidth - m_sizeWin.cx)/2,(nHeight - m_sizeWin.cy)/2 ); 
		m_ArrayBetChip[i]->ptBeginPos += ptOffset;
		m_ArrayBetChip[i]->ptEndPos += ptOffset;
		m_ArrayBetChip[i]->ptCurrentPos += ptOffset;
	}
	
	// 窗口大小
	m_sizeWin.SetSize(nWidth, nHeight);

	if( m_bFirstChange && nWidth != 0 && nWidth != 754 && nHeight != 0 )
	{
		m_bFirstChange = FALSE;
		if ( nWidth < (int)m_ImageViewBack.GetWidth() )
		{
		}
	}
	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	m_ImageViewBack.DrawImage( pDC, nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - m_ImageViewBack.GetHeight()/2);

	// 信息显示
	m_ImageCellScore.DrawImage( pDC, nWidth/2 - 360, 0 );
	m_ImgaeScoreTotal.DrawImage( pDC, nWidth/2 - 123, 0 );

	//信息变量
	if( m_lCellScore != 0 )
		DrawNumber(pDC, &m_ImageCellNumber, TEXT("0123456789"), m_lCellScore,   nWidth/2 - 292, 7);
	if( m_lDrawMaxScore != 0 )
		DrawNumber(pDC, &m_ImageCellNumber, TEXT("0123456789"), m_lDrawMaxScore,nWidth/2 - 292, 37);

	//计算总注
	LONGLONG lTableScore = m_lALLBetChip;
	for( INT i = 0; i < GAME_PLAYER; i++ )
		lTableScore += m_lPalyBetChip[i];

	//总注信息
	if ( lTableScore > 0L )
	{
		DrawNumber(pDC, &m_ImgaeScoreTotalNumber, TEXT("0123456789"), lTableScore, nWidth/2 - 23, 32);
	}

	//用户扑克
	for (BYTE i=0;i<GAME_PLAYER;i++) 
	{
		m_CardControl[i].DrawCardControl(pDC);
	}

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		//IClientUserItem * pIClientUserItem=GetClientUserItem(2);
		if (pIClientUserItem == NULL) continue;

		//绘画时间
		WORD wTime = GetUserClock(i);
		if ( wTime > 0 && wTime <= 99 )
		{
			TCHAR szTime[128] = {0};
			if( wTime > 9)
				_sntprintf(szTime,CountArray(szTime),TEXT("%d"), wTime);
			else
				_sntprintf(szTime,CountArray(szTime),TEXT("0%d"), wTime);

			DrawNumber(pDC , &m_ImageTime, TEXT("0123456789"), szTime, m_ptClock[i].x, m_ptClock[i].y, DT_CENTER );
		}

		// 绘画玩家信息
		CPoint ptShowPos;
		if ( i == 1 || i == 3 )
		{
			ptShowPos.SetPoint(m_ptUserInfo[i].x - m_ImageUserInfoV.GetWidth()/2, m_ptUserInfo[i].y - m_ImageUserInfoV.GetHeight()/2);
			
			// 背景
			m_ImageUserInfoV.DrawImage( pDC, ptShowPos.x, ptShowPos.y);

			// 外框
			if ( pIClientUserItem->GetUserStatus() == US_PLAYING && GetUserClock(i) )
			{
				m_ImageUserPalyV.DrawImage( pDC, m_ptUserInfo[i].x - m_ImageUserPalyV.GetWidth()/2, m_ptUserInfo[i].y - m_ImageUserPalyV.GetHeight()/2);
			}
			
			// 头像
			m_ptAvatar[i].SetPoint(ptShowPos.x + 13, ptShowPos.y + 8);
			DrawUserAvatar(pDC, m_ptAvatar[i].x, m_ptAvatar[i].y, pIClientUserItem);

			// 名字
			CRect rectInfo( ptShowPos.x + 8, ptShowPos.y + 65, ptShowPos.x + 67, ptShowPos.y + 77);
			DrawTextStringEx(pDC,pIClientUserItem->GetNickName(),RGB(255,255,255),RGB(0,0,0), rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
		
			// 用户等级
			rectInfo.OffsetRect( 0, 16 );
			DrawTextStringEx(pDC,GetLevelDescribe(pIClientUserItem),RGB(153,255,0),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);

			//用户积分
			rectInfo.OffsetRect( 0, 16 );
			DrawTextStringEx(pDC,AddComma(m_lUserScore[i]),RGB(255,255,0),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
		
			// 用户下注
			if( m_lTableScore[i] > 0L )
			{
				UINT oldAlign = pDC->SetTextAlign(TA_CENTER);
				TCHAR szBuffer[128];
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("已下注：%s"),AddComma(m_lTableScore[i]));

				if ( i == 1 )
				{
					pDC->SetTextAlign(TA_LEFT);
					DrawTextStringEx(pDC, szBuffer, RGB(200,249,248), RGB(0,0,0), m_ptUserInfo[i].x - 37, m_ptUserInfo[i].y + m_ImageUserInfoV.GetHeight()/2 + 2);
				}
				else
				{
					pDC->SetTextAlign(TA_RIGHT);
					DrawTextStringEx(pDC, szBuffer, RGB(200,249,248), RGB(0,0,0), m_ptUserInfo[i].x + 37, m_ptUserInfo[i].y + m_ImageUserInfoV.GetHeight()/2 + 2);
				}
				pDC->SetTextAlign(oldAlign);
			}
		}
		else
		{
			ptShowPos.SetPoint(m_ptUserInfo[i].x - m_ImageUserInfoH.GetWidth()/2, m_ptUserInfo[i].y - m_ImageUserInfoH.GetHeight()/2);
			
			// 背景
			m_ImageUserInfoH.DrawImage( pDC, ptShowPos.x, ptShowPos.y);

			// 外框
			if ( pIClientUserItem->GetUserStatus() == US_PLAYING && GetUserClock(i) )
			{
				m_ImageUserPalyH.DrawImage( pDC, m_ptUserInfo[i].x - m_ImageUserPalyH.GetWidth()/2, m_ptUserInfo[i].y - m_ImageUserPalyH.GetHeight()/2);
			}

			// 头像
			m_ptAvatar[i].SetPoint(ptShowPos.x + 9, ptShowPos.y + 9);
			DrawUserAvatar(pDC, m_ptAvatar[i].x, m_ptAvatar[i].y, pIClientUserItem);

			// 名字
			CRect rectInfo( ptShowPos.x + 64, ptShowPos.y + 11, ptShowPos.x + 162, ptShowPos.y + 23);
			DrawTextStringEx(pDC,pIClientUserItem->GetNickName(),RGB(255,255,255),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
		
			// 用户等级
			rectInfo.OffsetRect( 0, 16 );
			DrawTextStringEx(pDC,GetLevelDescribe(pIClientUserItem),RGB(153,255,0),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);

			//用户积分
			rectInfo.OffsetRect( 0, 16 );
			DrawTextStringEx(pDC,AddComma(m_lUserScore[i]),RGB(255,255,0),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);

			// 用户下注
			if( m_lTableScore[i] > 0L )
			{
				UINT oldAlign = pDC->SetTextAlign(TA_CENTER);
				TCHAR szBuffer[128];
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("已下注：%s"),AddComma(m_lTableScore[i]));

				if ( i == 2 )
					DrawTextStringEx(pDC, szBuffer, RGB(200,249,248), RGB(0,0,0), m_ptUserInfo[i].x, m_ptUserInfo[i].y - m_ImageUserInfoH.GetHeight()/2 - 12);
				else
					DrawTextStringEx(pDC, szBuffer, RGB(200,249,248), RGB(0,0,0), m_ptUserInfo[i].x, m_ptUserInfo[i].y + m_ImageUserInfoH.GetHeight()/2 + 2);

				pDC->SetTextAlign(oldAlign);
			}
		}
	}

	// 移动筹码
	for ( int i = 0 ; i < m_ArrayBetChip.GetCount(); ++i )
	{
		// 获取属性
		INT nChipHeight = m_ImageChip.GetHeight();
		INT nChipWidth  = m_ImageChip.GetWidth() / CHIP_COUNT;

		// 绘画筹码
		m_ImageChip.DrawImage( pDC, (INT)m_ArrayBetChip[i]->ptCurrentPos.x - nChipWidth/2, (INT)m_ArrayBetChip[i]->ptCurrentPos.y - nChipHeight / 2,
			nChipWidth, nChipHeight, 
			nChipWidth * m_ArrayBetChip[i]->wChipIndex, 0 );
	}

	// 回收筹码
	for ( int i = 0 ; i < m_ArrayOverChip.GetCount(); ++i )
	{
		// 获取属性
		INT nChipHeight = m_ImageChip.GetHeight();
		INT nChipWidth  = m_ImageChip.GetWidth() / CHIP_COUNT;

		// 绘画筹码
		if ( !m_ArrayOverChip[i]->bShow )
			continue;

		m_ImageChip.DrawImage( pDC, (INT)m_ArrayOverChip[i]->ptCurrentPos.x - nChipWidth/2, (INT)m_ArrayOverChip[i]->ptCurrentPos.y - nChipHeight / 2,
			nChipWidth, nChipHeight, 
			nChipWidth * m_ArrayOverChip[i]->wChipIndex, 0 );
	}

	//用户当前筹码
	int nChipCount[GAME_PLAYER] = {0};
	for ( int i = 0 ; i < m_ArrayPlayChip.GetCount(); ++i )
	{
		// 获取属性
		INT nChipHeight = m_ImageChip.GetHeight();
		INT nChipWidth  = m_ImageChip.GetWidth() / CHIP_COUNT;

		if( m_ArrayPlayChip[i]->wChipChairID < GAME_PLAYER && nChipCount[m_ArrayPlayChip[i]->wChipChairID] <= PILING_HEIGHT )
			nChipCount[m_ArrayPlayChip[i]->wChipChairID]++;

		if( nChipCount[m_ArrayPlayChip[i]->wChipChairID] >= PILING_HEIGHT )
			continue;

		// 绘画筹码
		m_ImageChip.DrawImage( pDC, (INT)m_ArrayPlayChip[i]->ptCurrentPos.x - nChipWidth/2, (INT)m_ArrayPlayChip[i]->ptCurrentPos.y - nChipHeight / 2,
			nChipWidth, nChipHeight, 
			nChipWidth * m_ArrayPlayChip[i]->wChipIndex, 0 );
	}

	//用户当前停止筹码
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem == NULL) continue;

		LONGLONG lPalyBetChip = m_lPalyBetChip[i];
		LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };
		UINT	 nChipCount    = 0;
		while ( lPalyBetChip > 0  && nChipCount < PILING_HEIGHT )
		{
			for( int j = CountArray(lScoreArray) - 1; j >=0; j-- )
			{
				if ( lPalyBetChip >= lScoreArray[j] )
				{
					// 获取属性
					INT nChipHeight = m_ImageChip.GetHeight();
					INT nChipWidth  = m_ImageChip.GetWidth() / CHIP_COUNT;
					INT nXPos = m_ptPalyBetChip[i].x - nChipWidth/2;
					INT nYPos = m_ptPalyBetChip[i].y - nChipHeight/2 - nChipCount * 5;
					m_ImageChip.DrawImage(pDC, nXPos, nYPos, nChipWidth, nChipHeight, nChipWidth * j, 0);
					lPalyBetChip -= lScoreArray[j];
					nChipCount++;
					break;
				}
			}
		}


		//筹码数字
		if( m_lPalyBetChip[i] > 0L )
		{
			UINT oldAlign = pDC->SetTextAlign(TA_CENTER);
			DrawTextStringEx(pDC, AddComma(m_lPalyBetChip[i]), RGB(200,249,248), RGB(0,0,0), m_ptPalyBetChip[i].x,m_ptPalyBetChip[i].y + 15);
			pDC->SetTextAlign(oldAlign);
		}
	}

	//绘画发牌扑克
	if ( m_SendCardItemArray.GetCount() > 0 )
	{
		//变量定义
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//获取大小
		int nItemWidth=m_SizeGameCard.cx;
		int nItemHeight=m_SizeGameCard.cy;

		//绘画扑克
		m_ImageGameCard.DrawImage(pDC,m_SendCardCurPos.x,m_SendCardCurPos.y,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4);
	}

	//用户动作
	if ( m_wActionUser != INVALID_CHAIR && m_lOverScore[m_wActionUser] == 0 )
	{
		if ( m_cbUserAction != AC_SHOW_HAND )
		{
			//计算位置
			INT nXPos=m_ptUserAction[m_wActionUser].x-m_ImageActionBack.GetWidth()/10;
			INT nYPos=m_ptUserAction[m_wActionUser].y-m_ImageActionBack.GetHeight()/2;

			//绘画背景
			m_ImageActionBack.DrawImage(pDC,nXPos ,nYPos, 
				m_ImageActionBack.GetWidth() / 5,	m_ImageActionBack.GetHeight(),
				m_wActionUser * m_ImageActionBack.GetWidth() / 5, 0);

			//绘画动作
			m_ImageActionFont.DrawImage(pDC,nXPos + 12, nYPos + 7,
				m_ImageActionFont.GetWidth() / 5,	m_ImageActionFont.GetHeight(),
				m_cbUserAction * m_ImageActionFont.GetWidth() / 5, 0);
		}
		else
		{
			//梭哈动作
			m_ImageShowHand.DrawImage(pDC,(nWidth-m_ImageShowHand.GetWidth())/2,(nHeight-m_ImageShowHand.GetHeight())/2);
		}
	}
	
	// 结束信息
	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		DrawOverScore(pDC, m_lOverScore[i], m_ptOverScore[i].x, m_ptOverScore[i].y);

		//获取用户
		IClientUserItem * pIClientUserItem = GetClientUserItem(i);

		if ( m_lOverScore[i] != 0 && m_CardControl[i].GetCardCount() == MAX_CARD_COUNT && m_CardControl[i].GetDisplayHead()
			&& ( pIClientUserItem == NULL || pIClientUserItem->GetUserStatus() != US_READY))
		{
			BYTE cbCrad[MAX_CARD_COUNT] = {0};
			m_CardControl[i].GetCardData( cbCrad, MAX_CARD_COUNT );

			//计算位置
			CPoint ptCenterPoint;
			m_CardControl[i].GetCenterPoint(ptCenterPoint);
			INT nXPos = ptCenterPoint.x - m_ImageCradType.GetWidth()/18;
			INT nYPos = ptCenterPoint.y - m_ImageCradType.GetHeight()/2;

			//绘画动作
			m_GameLogic.SortCardList( cbCrad, MAX_CARD_COUNT);
			m_ImageCradType.DrawImage(pDC,nXPos, nYPos ,
				m_ImageCradType.GetWidth() / 9,	m_ImageCradType.GetHeight(),
				(m_GameLogic.GetCardGenre( cbCrad, (BYTE)m_CardControl[i].GetCardCount() ) - 1) * m_ImageCradType.GetWidth() / 9, 0);
		}
	}
	
	//准备标志
	for( int i = 0; i < GAME_PLAYER; ++i )
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem == NULL) continue;

		if (pIClientUserItem->GetUserStatus() == US_READY)
			m_ImageReady.DrawImage(pDC, m_ptReady[i].x - m_ImageReady.GetWidth()/2, m_ptReady[i].y - m_ImageReady.GetHeight()/2);
	}

	// 操作界面
	m_PlayOperate.DrawPlayOperate(pDC,nWidth,nHeight);

	return;
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

	//删除处理
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	//更新界面
	InvalidGameView(0,0,0,0);

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

		//
		m_nStepCount = SEND_STEP_COUNT;
		m_SendCardCurPos = m_SendCardPos;
		m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
		m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

		InvalidGameView(0,0,0,0);

		//播放声音
		CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
		pGlobalUnits->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//游戏底注
bool CGameClientView::SetCellScore(LONG lCellScore)
{
	m_PlayOperate.SetCellScore(lCellScore);
	if (m_lCellScore!=lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	return true;
}

//最大下注
bool CGameClientView::SetDrawMaxScore(LONGLONG lDrawMaxScore)
{
	if (m_lDrawMaxScore!=lDrawMaxScore)
	{
		//设置变量
		m_lDrawMaxScore=lDrawMaxScore;

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	return true;
}

//设置积分
bool CGameClientView::SetUserScore(WORD wChairID, LONGLONG lUserScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置积分
	if (m_lUserScore[wChairID]!=lUserScore)
	{
		m_lUserScore[wChairID] = lUserScore;
		m_lUserSaveScore[wChairID] = lUserScore;

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	return true;
}

//设置积分
bool CGameClientView::SetUserSaveScore(WORD wChairID, LONGLONG lUserScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置积分
	if (m_lUserSaveScore[wChairID] != lUserScore)
	{
		m_lUserAddScore[wChairID] = 0;
		m_lUserAddAllScore[wChairID] = lUserScore - m_lUserScore[wChairID];
		m_lUserSaveScore[wChairID] = lUserScore;

		//更新界面
		InvalidGameView(0,0,0,0);
	}

	return true;
}

//设置结束积分
bool CGameClientView::SetOverScore(WORD wChairID, LONGLONG lUserScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置积分
	if (m_lOverScore[wChairID] != lUserScore)
	{
		m_lOverScore[wChairID] = 0;
		m_lOverSaveScore[wChairID] = lUserScore;
	}

	return true;
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
		InvalidGameView(0,0,0,0);

		//设置时间
		if (m_wActionUser!=INVALID_CHAIR) SetTimer(IDI_USER_ACTION,3000,NULL);
	}

	return true;
}

//设置积分
bool CGameClientView::SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置变量
	m_pHistoryScore[wChairID]=pHistoryScore;

	//更新界面
	InvalidGameView(0,0,0,0);

	return true;
}

// 绘画数字
void CGameClientView::DrawNumber( CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
	DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
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


// 绘画结束分数
void CGameClientView::DrawOverScore( CDC * pDC, LONGLONG lOutNum, INT nXPos, INT nYPos )
{
	if( lOutNum == 0 )
		return;

	CPngImage* ImageNumber = NULL;
	TCHAR szOutNum[128] = {0};
	TCHAR szImageNum[128] = { TEXT("0123456789") };
	_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);

	if ( lOutNum > 0 )
		ImageNumber = &m_ImgaeScoreWin;
	else
		ImageNumber = &m_ImgaeScoreLose;

	INT nNumberHeight = ImageNumber->GetHeight();
	INT nNumberWidth  = ImageNumber->GetWidth() / 10;

	nXPos -= ((INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth) + m_ImgaeScoreSum.GetWidth() / 4);
	nYPos -= ImageNumber->GetHeight() / 2;

	int nBackX = nXPos + 35;
	int nBackY = nYPos + ImageNumber->GetHeight() / 2 - m_ImgaeScoreBackL.GetHeight() / 2 - 1;
	int nBackWidth = (int)lstrlen(szOutNum) * nNumberWidth + m_ImgaeScoreSum.GetWidth() / 2 - 35;

	if ( lOutNum < 0 )
		nBackWidth -= nNumberWidth;

	m_ImgaeScoreBackL.DrawImage( pDC, nXPos - 15, nBackY);
	for( int i = 0; i < nBackWidth; i += m_ImgaeScoreBackM.GetWidth() )
	{
		m_ImgaeScoreBackM.DrawImage( pDC, nBackX, nBackY);
		nBackX += m_ImgaeScoreBackM.GetWidth();
	}
	m_ImgaeScoreBackR.DrawImage( pDC, nBackX, nBackY);


	if ( lOutNum > 0)
		m_ImgaeScoreSum.DrawImage( pDC, nXPos, nYPos, m_ImgaeScoreSum.GetWidth()/2, m_ImgaeScoreSum.GetHeight(), 0, 0);
	else
		m_ImgaeScoreSum.DrawImage( pDC, nXPos, nYPos, m_ImgaeScoreSum.GetWidth()/2, m_ImgaeScoreSum.GetHeight(), m_ImgaeScoreSum.GetWidth()/2, 0);

	nXPos += m_ImgaeScoreSum.GetWidth()/2;

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


//开始按钮
VOID CGameClientView::OnBnClickedStart()
{
	//发送消息
	SendEngineMessage(IDM_START,0,0);

	return;
}

//放弃按钮
VOID CGameClientView::OnBnClickedGiveUp()
{
	//发送消息
	SendEngineMessage(IDM_GIVE_UP,0,0);

	return;
}

//跟注按钮
VOID CGameClientView::OnBnClickedFollow()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,0,0);

	return;
}

//加注按钮
VOID CGameClientView::OnBnClickedAddScore()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,(WPARAM)m_PlayOperate.GetBetCount(),0);

	return;
}

//加注按钮
VOID CGameClientView::OnBnClickedAddScoreOne()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,m_lCellScore,0);
}

//加注按钮
VOID CGameClientView::OnBnClickedAddScoreTwo()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,m_lCellScore*2,0);
}

//加注按钮
VOID CGameClientView::OnBnClickedAddScoreThree()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,m_lCellScore*3,0);
}

//加注按钮
VOID CGameClientView::OnBnClickedAddScoreFour()
{
	//发送消息
	SendEngineMessage(IDM_FOLLOW,m_lCellScore*4,0);
}

//梭哈按钮
VOID CGameClientView::OnBnClickedShowHand()
{
	//发送消息
	SendEngineMessage(IDM_SHOW_HAND,0,0);

	return;
}

//积分按钮
VOID CGameClientView::OnBnClickedADD()
{
	m_PlayOperate.ScoreADD();

	CRect rect = m_PlayOperate.GetOperateRect();
	InvalidGameView(rect.left, rect.top, rect.Width(), rect.Height());
}

//积分按钮
VOID CGameClientView::OnBnClickedSUB()
{
	m_PlayOperate.ScoreSUB();

	CRect rect = m_PlayOperate.GetOperateRect();
	InvalidGameView(rect.left, rect.top, rect.Width(), rect.Height());
}

//时间消息
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
	//消息处理
	switch (nIDEvent)
	{
	case IDI_HIDE_CARD:		//隐藏扑克
		{
			//删除时间
			KillTimer(IDI_HIDE_CARD);

			//隐藏判断
			IClientUserItem * pIClientUserItem=GetClientUserItem(MYSELF_VIEW_ID);
			if( pIClientUserItem == NULL )
				return;

			if ( m_lOverScore[MYSELF_VIEW_ID] == 0 && m_CardControl[MYSELF_VIEW_ID].GetDisplayHead() && m_CardControl[MYSELF_VIEW_ID].GetPositively() && pIClientUserItem->GetUserStatus() == US_PLAYING )
			{
				//设置控件
				m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(false);

				//更新界面
				InvalidGameView(0,0,0,0);
			}

			return;
		}
	case IDI_SEND_CARD:		//发牌动画
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
					//
					wChairID = m_SendCardItemArray[0].wChairID;
					m_nStepCount = SEND_STEP_COUNT;
					m_SendCardCurPos = m_SendCardPos;
					m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
					m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

					//播放声音
					CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
					pGlobalUnits->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

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
	case IDI_USER_ACTION:	//用户动作
		{
			//设置变量
			m_cbUserAction=0;
			m_wActionUser=INVALID_CHAIR;

			//更新界面
			InvalidGameView(0,0,0,0);

			return;
		}
	case IDI_MOVE_CHIP:
		{
			BOOL bUpdateGameView = FALSE;
			for ( int i = 0 ; i < m_ArrayBetChip.GetCount(); ++i )
			{
				if( m_ArrayBetChip[i]->Move() )
				{
					bUpdateGameView = TRUE;
				}
			}

			int nIndex = 0;
			while ( nIndex < m_ArrayPlayChip.GetCount())
			{
				if( m_ArrayPlayChip[nIndex]->Move() )
				{
					bUpdateGameView = TRUE;
					nIndex++;
				}
				else
				{
					bUpdateGameView = TRUE;

					// 变量定义
					LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };

					// 添加分数
					m_lPalyBetChip[m_ArrayPlayChip[nIndex]->wChipChairID] += lScoreArray[m_ArrayPlayChip[nIndex]->wChipIndex];

					// 删除筹码
					SafeDelete(m_ArrayPlayChip[nIndex]);
					m_ArrayPlayChip.RemoveAt(nIndex);

				}
			}

			// 添加回收
			if ( m_ArrayOverChip.GetCount() )
			{
				// 回收位置
				WORD   wChairID = m_ArrayOverChip[0]->wChipChairID;
				CPoint ptBenchmarkPos = m_ptAvatar[wChairID];
				ptBenchmarkPos.Offset( 25, 25 );

				// 添加回收
				if ( m_ArrayBetChip.GetCount() )
				{
					INT_PTR nMaxIndex = m_ArrayBetChip.GetCount()/5;
					if ( nMaxIndex <= 0 )
						nMaxIndex = 1;

					while ( nMaxIndex > 0 )
					{
						stuMoveChip* pMoveChip = new stuMoveChip(m_ArrayBetChip[0]->wChipIndex, wChairID, 10, m_ArrayBetChip[0]->ptCurrentPos, ptBenchmarkPos, FALSE);
						m_ArrayOverChip.Add(pMoveChip);
						SafeDelete(m_ArrayBetChip[0]);
						m_ArrayBetChip.RemoveAt(0);
						nMaxIndex--;
					}

				}

				// 添加回收
				if ( m_ArrayPlayChip.GetCount() )
				{
					INT_PTR nMaxIndex = m_ArrayPlayChip.GetCount()/5;
					if ( nMaxIndex <= 0 )
						nMaxIndex = 1;

					while ( nMaxIndex > 0 )
					{
						stuMoveChip* pMoveChip = new stuMoveChip(m_ArrayPlayChip[0]->wChipIndex, wChairID, 10, m_ArrayPlayChip[0]->ptCurrentPos, ptBenchmarkPos, FALSE);
						m_ArrayOverChip.Add(pMoveChip);
						SafeDelete(m_ArrayPlayChip[0]);
						m_ArrayPlayChip.RemoveAt(0);
						nMaxIndex--;
					}
				}
				

				// 添加回收
				LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };
			
				for ( int nSite = 0; nSite < GAME_PLAYER; ++nSite )
				{
					int nChipCount = 0;
					while ( m_lPalyBetChip[nSite] > 0 )
					{
						for( int j = CountArray(lScoreArray) - 1; j >=0; j-- )
						{
							if ( m_lPalyBetChip[nSite] >= lScoreArray[j] )
							{	
								CPoint ptBegin(m_ptPalyBetChip[nSite]);
								CPoint ptEnd(ptBenchmarkPos);
								ptEnd.y -= nChipCount * 5;
								ptBegin.y -= nChipCount * 5;
								stuMoveChip* pMoveChip = new stuMoveChip(j, wChairID, 10, ptBegin, ptEnd, FALSE);
								if ( nChipCount >= PILING_HEIGHT )
									pMoveChip->SetShow(FALSE);
								
								m_ArrayOverChip.Add(pMoveChip);
								m_lPalyBetChip[nSite] -= lScoreArray[j];
								nChipCount++;
								break;
							}
						}
					}
				}
			}

			// 移动回收动画
			nIndex = 0;
			while ( nIndex < m_ArrayOverChip.GetCount())
			{
				if( m_ArrayOverChip[nIndex]->Move() )
				{
					bUpdateGameView = TRUE;
					nIndex++;
				}
				else
				{
					// 变量定义
					WORD wChipChairID = m_ArrayOverChip[nIndex]->wChipChairID;
					LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };

					// 添加分数
					m_lUserScore[wChipChairID] += lScoreArray[m_ArrayOverChip[nIndex]->wChipIndex];
					m_lUserAddScore[wChipChairID] += lScoreArray[m_ArrayOverChip[nIndex]->wChipIndex];

					if ( m_lUserAddScore[wChipChairID] > m_lUserAddAllScore[wChipChairID] )
						m_lUserAddScore[wChipChairID] = m_lUserAddAllScore[wChipChairID];


					for ( int i = 0; i < GAME_PLAYER; ++i )
					{
						if ( m_lUserAddAllScore[i] == 0 || m_lOverSaveScore[i] == 0 )
							continue;

						m_lOverScore[i] = (LONGLONG)( m_lOverSaveScore[i] * m_lUserAddScore[wChipChairID] / m_lUserAddAllScore[wChipChairID]);
					}

					// 删除筹码
					SafeDelete(m_ArrayOverChip[nIndex]);
					m_ArrayOverChip.RemoveAt(nIndex);

					bUpdateGameView = TRUE;

					if ( m_ArrayOverChip.GetCount() == 0 )
					{
						memcpy(m_lUserScore, m_lUserSaveScore, sizeof(m_lUserScore));
						memcpy(m_lOverScore, m_lOverSaveScore, sizeof(m_lOverScore));
					}
				}
			}


			//更新界面
			if ( bUpdateGameView )
			{
				InvalidGameView( 0,0,0,0 );
			}
			
			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_PlayOperate.Initialization(this);

	//资源变量
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//控制按钮
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);

	//控制按钮
	m_btStart.SetButtonImage(IDB_BT_START,hResInstance,false,false);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btStart,TEXT("开始游戏"));
	m_ToolTipCtrl.AddTool(&m_PlayOperate.m_btFollow,TEXT("跟注"));
	m_ToolTipCtrl.AddTool(&m_PlayOperate.m_btGiveUp,TEXT("放弃"));
	m_ToolTipCtrl.AddTool(&m_PlayOperate.m_btAddScore,TEXT("加注"));
	m_ToolTipCtrl.AddTool(&m_PlayOperate.m_btShowHand,TEXT("梭哈"));

	m_DFontEx.CreateFont(this, TEXT("宋体"), 12, 400 );

	// 开启筹码移动
	SetTimer(IDI_MOVE_CHIP, 30, NULL);

	return 0;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//for ( int i = 0 ; i < GAME_PLAYER; ++i )
	//{
	//	BYTE cbCrad[5] = { 0x09,0x19,0x29,0x0C,0x0A };
	//	m_CardControl[i].SetCardData( cbCrad, 5);
	//}
	//
	//InvalidGameView(0,0,0,0);

	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克点击
	if (m_CardControl[MYSELF_VIEW_ID].OnEventLeftMouseDown(MousePoint)==true)
	{
		//更新界面
		InvalidGameView(0,0,0,0);

		//设置时间
		if (m_CardControl[MYSELF_VIEW_ID].GetDisplayHead()==true)
		{
			SetTimer(IDI_HIDE_CARD,2000,NULL);
		}
		else KillTimer(IDI_HIDE_CARD);
	}

	return;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_CardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true) return TRUE;

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//绘画字符
bool CGameClientView::DrawTextStringEx(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, INT nDrawFormat)
{
	//变量定义
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//保存格式
	UINT nTextAlign=pDC->SetTextAlign(TA_TOP|TA_LEFT);

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
		m_DFontEx.DrawText(pDC,pszString,&rcFrame,crFrame,nDrawFormat);
	}

	//绘画字符
	m_DFontEx.DrawText(pDC,pszString,&rcRect,crText,nDrawFormat);

	//还原格式
	pDC->SetTextAlign(nTextAlign);

	return true;
}

// 绘画字符
bool CGameClientView::DrawTextStringEx( CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, INT nXPos, INT nYPos )
{
	//变量定义
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//保存格式
	UINT nTextAlign=pDC->GetTextAlign();
	INT nDrawFormat=0;

	if ( nTextAlign&TA_CENTER )
		nDrawFormat |= DT_CENTER;
	else if( nTextAlign&TA_RIGHT )
		nDrawFormat |= DT_RIGHT;
	else
		nDrawFormat |= DT_LEFT;

	if( nTextAlign&TA_BOTTOM )
		nDrawFormat |= DT_BOTTOM;
	else 
		nDrawFormat |= DT_TOP;

	//绘画边框
	for (INT i=0;i<CountArray(nXExcursion);i++)
	{
		//计算位置
		int nYPosT=nYPos+nYExcursion[i];
		int nXPosT=nXPos+nXExcursion[i];

		//绘画字符
		m_DFontEx.DrawText(pDC,pszString,nXPosT,nYPosT,crFrame,nDrawFormat);
	}

	//绘画字符
	m_DFontEx.DrawText(pDC,pszString,nXPos,nYPos,crText,nDrawFormat);

	return true;
}

// 添加桌面下注
void CGameClientView::AddDeskChip( LONGLONG lScore, WORD wChairID )
{

	// 变量定义
	LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };
	UINT	 nChipCount    = 0;

	// 删除下注
	if( wChairID <= GAME_PLAYER )
	{
		int nIndex = 0;
		while( nIndex < m_ArrayPlayChip.GetCount() )
		{	
			if( m_ArrayPlayChip[nIndex]->wChipChairID == wChairID )
			{
				WORD	wChipIndex; 
				UINT	nAllTime; 
				DPOINT	ptBeginPos; 
				DPOINT	ptEndPos;

				wChipIndex = m_ArrayPlayChip[nIndex]->wChipIndex;
				nAllTime = 8;
				CPoint ptBenchmarkPos = m_ptPalyBetChip[wChairID];
				ptBeginPos.SetPoint( m_ArrayPlayChip[nIndex]->ptCurrentPos.x, m_ArrayPlayChip[nIndex]->ptCurrentPos.y);
				ptEndPos.SetPoint( m_rectBetChip.left + rand()%m_rectBetChip.Width(), m_rectBetChip.top + rand()%m_rectBetChip.Height());
				stuMoveChip* pMoveChip = new stuMoveChip(wChipIndex, wChairID, nAllTime, ptBeginPos, ptEndPos, TRUE);
				m_ArrayBetChip.Add(pMoveChip);
				lScore -= lScoreArray[m_ArrayPlayChip[nIndex]->wChipIndex];
				SafeDelete(m_ArrayPlayChip[nIndex]);
				m_ArrayPlayChip.RemoveAt(nIndex);
				continue;
			}
			nIndex++;
		}
	}

	// 获取筹码
	nChipCount    = 0;
	while ( lScore > 0 )
	{
		for( int i = CountArray(lScoreArray) - 1; i >=0; i-- )
		{
			if ( lScore >= lScoreArray[i] )
			{
				WORD	wChipIndex; 
				UINT	nAllTime; 
				DPOINT	ptBeginPos; 
				DPOINT	ptEndPos;

				if ( wChairID >= GAME_PLAYER )
				{
					wChipIndex = i;
					nAllTime = 0;
					ptBeginPos.SetPoint( m_rectBetChip.left + rand()%m_rectBetChip.Width(), m_rectBetChip.top + rand()%m_rectBetChip.Height());
					ptEndPos = ptBeginPos;
				}
				else
				{
					wChipIndex = i;
					nAllTime = 8;
					CPoint ptBenchmarkPos = m_ptPalyBetChip[wChairID];
					ptBeginPos.SetPoint( ptBenchmarkPos.x, ptBenchmarkPos.y - nChipCount * 5);
					ptEndPos.SetPoint( m_rectBetChip.left + rand()%m_rectBetChip.Width(), m_rectBetChip.top + rand()%m_rectBetChip.Height());
				}
				stuMoveChip* pMoveChip = new stuMoveChip(wChipIndex, wChairID, nAllTime, ptBeginPos, ptEndPos, TRUE);
				m_ArrayBetChip.Add(pMoveChip);
				lScore -= lScoreArray[i];
				if ( nChipCount < PILING_HEIGHT )
					nChipCount++;

				break;
			}
		}
	}
}


// 添加玩家下注
void CGameClientView::AddPlayChip( LONGLONG lScore, WORD wChairID )
{
	if( wChairID >= GAME_PLAYER )
		return;

	// 变量定义
	LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };

	// 获取筹码
	while ( lScore > 0 )
	{
		for( int i = CountArray(lScoreArray) - 1; i >=0; i-- )
		{
			if ( lScore >= lScoreArray[i] )
			{
				WORD	wChipIndex; 
				UINT	nAllTime; 
				DPOINT	ptBeginPos; 
				DPOINT	ptEndPos;

				wChipIndex = i;
				nAllTime = 10;
				CPoint ptBenchmarkPos = m_ptAvatar[wChairID];
				ptBenchmarkPos.Offset( 25, 25 );
				ptBeginPos.SetPoint( ptBenchmarkPos.x, ptBenchmarkPos.y );
				ptEndPos.SetPoint( m_rectBetChip.left + rand()%m_rectBetChip.Width(), m_rectBetChip.top + rand()%m_rectBetChip.Height());

				stuMoveChip* pMoveChip = new stuMoveChip(wChipIndex, wChairID, nAllTime, ptBeginPos, ptEndPos, TRUE);
				m_ArrayBetChip.Add(pMoveChip);
				lScore -= lScoreArray[i];
				
				break;
			}
		}
	}
	
}

// 添加玩家加注
void CGameClientView::AddPlayBetChip( LONGLONG lScore, WORD wChairID )
{
	if( wChairID >= GAME_PLAYER )
		return;

	if ( lScore == 0 )
	{
		m_lPalyBetChip[wChairID] = lScore;
		return;
	}

	lScore -= m_lPalyBetChip[wChairID];

	// 变量定义
	LONGLONG lScoreArray[] = { 1L, 5L, 10L, 50L, 100L, 500L, 1000L, 5000L, 10000L, 50000L, 100000L, 500000L, 1000000L, 5000000L };
	UINT	 nChipCount    = 0;

	// 获取筹码
	while ( lScore > 0 )
	{
		for( int i = CountArray(lScoreArray) - 1; i >=0; i-- )
		{
			if ( lScore >= lScoreArray[i] )
			{
				WORD	wChipIndex; 
				UINT	nAllTime; 
				DPOINT	ptBeginPos; 
				DPOINT	ptEndPos;

				wChipIndex = i;
				nAllTime = 15;
				CPoint ptBenchmarkPos = m_ptAvatar[wChairID];
				ptBenchmarkPos.Offset( 25, 25 );
				ptBeginPos.SetPoint( ptBenchmarkPos.x, ptBenchmarkPos.y - nChipCount * 5);
				ptEndPos.SetPoint( m_ptPalyBetChip[wChairID].x, m_ptPalyBetChip[wChairID].y - nChipCount * 5);

				stuMoveChip* pMoveChip = new stuMoveChip(wChipIndex, wChairID, nAllTime, ptBeginPos, ptEndPos, FALSE);
				m_ArrayPlayChip.Add(pMoveChip);
				lScore -= lScoreArray[i];

				if ( nChipCount < PILING_HEIGHT )
					nChipCount++;
				break;
			}
		}
	}
}

// 筹码回收
void CGameClientView::ChioRecovery( WORD wChairID )
{
	// 回收位置
	CPoint ptBenchmarkPos = m_ptAvatar[wChairID];
	ptBenchmarkPos.Offset( 25, 25 );

	// 添加回收
	if ( m_ArrayBetChip.GetCount() )
	{
		stuMoveChip* pMoveChip = new stuMoveChip(m_ArrayBetChip[0]->wChipIndex, wChairID, 10, m_ArrayBetChip[0]->ptCurrentPos, ptBenchmarkPos, FALSE);
		m_ArrayOverChip.Add(pMoveChip);
		SafeDelete(m_ArrayBetChip[0]);
		m_ArrayBetChip.RemoveAt(0);
	}
}

// 消息函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CGameFrameView::OnCommand(wParam, lParam);
}

// 添加逗号
CString CGameClientView::AddComma( LONGLONG lScore )
{
	CString strScore;
	CString strReturn;
	strScore.Format(TEXT("%I64d"), lScore);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%4) == 0 && nStrCount != 0 )
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
	return strReturn;
}
	
//////////////////////////////////////////////////////////////////////////


