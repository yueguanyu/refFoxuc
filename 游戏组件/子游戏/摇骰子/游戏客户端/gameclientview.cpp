#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 
#define	IDC_START							100					//开始按钮
#define IDC_SET_CHIP_0						101					//下注按钮
#define IDC_SET_CHIP_1						102					//下注按钮
#define IDC_SET_CHIP_2						103					//下注按钮
#define IDC_SET_CHIP_3						104					//下注按钮
#define IDC_THROW_DICE						105					//摇骰按钮
#define IDC_LOOK_DICE						106					//看骰按钮
#define IDC_YELL_OK							107					//喊话按钮
#define IDC_SCROLL_UP						108					//上翻按钮
#define IDC_SCROLL_DOWN						109					//下翻按钮
#define IDC_DICE_NUM_1						110					//骰点按钮
#define IDC_DICE_NUM_2						111					//骰点按钮
#define IDC_DICE_NUM_3						112					//骰点按钮
#define IDC_DICE_NUM_4						113					//骰点按钮
#define IDC_DICE_NUM_5						114					//骰点按钮
#define IDC_DICE_NUM_6						115					//骰点按钮
#define IDC_OPEN_DICE						116					//开骰按钮
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SET_CHIP_0, IDC_SET_CHIP_3, OnSetChip)
	ON_BN_CLICKED(IDC_THROW_DICE, OnThrowDice)
	ON_BN_CLICKED(IDC_LOOK_DICE,  OnLookDice)
	ON_BN_CLICKED(IDC_SCROLL_UP,  OnScrollUp)
	ON_BN_CLICKED(IDC_SCROLL_DOWN,  OnScrollDown)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DICE_NUM_1, IDC_DICE_NUM_6, OnDiceNum)
	ON_BN_CLICKED(IDC_YELL_OK,  OnYellOk)
	ON_BN_CLICKED(IDC_OPEN_DICE,  OnOpenDice)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	
	//游戏变量
	m_bDiceCount = 0;
	m_bNowDicePoint = 0;
	m_lChip = 0;
	ZeroMemory(m_bShowDiceCup, sizeof(m_bShowDiceCup));
	ZeroMemory(m_nAnimalTime, sizeof(m_nAnimalTime));
	ZeroMemory(m_bShowDice, sizeof(m_bShowDice));
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(&m_MaxYell, sizeof(tagDiceYell));
	m_SceneOpenDice.Init();

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

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;



	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();	

	m_ImageBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageCenter.LoadFromResource(hInstance,IDB_VIEW_CENTER);
	m_ImageSezi.LoadFromResource(hInstance,IDB_SEZI);
	m_ImageThrowDiceAnimal[0].LoadFromResource(hInstance,IDB_ANI_THROWDICE_TOP);
	m_ImageThrowDiceAnimal[1].LoadFromResource(hInstance,IDB_ANI_THROWDICE_BOTTOM);
	m_ImageOpenDiceAnimal[0].LoadFromResource(hInstance,IDB_ANI_OPENDICE_TOP);
	m_ImageOpenDiceAnimal[1].LoadFromResource(hInstance,IDB_ANI_OPENDICE_BOTTOM);

	m_PngImageUserFrame.LoadImage(hInstance, TEXT("KUANG"));
	m_PngImageYellFrame.LoadImage(hInstance, TEXT("KUANG2"));
	m_PngImageJetton.LoadImage(hInstance, TEXT("JETTON"));
	m_PngImageWord.LoadImage(hInstance, TEXT("WORD"));
	m_PngImageJettonSmall.LoadImage(hInstance, TEXT("JETTON2"));
	m_PngImageNum1.LoadImage(hInstance, TEXT("NUM1"));
	m_PngImageNum2.LoadImage(hInstance, TEXT("NUM2"));
	m_PngImageDiceCup.LoadImage(hInstance, TEXT("DICECUP"));
	m_PngImageWordSucc.LoadImage(hInstance, TEXT("WORDSUCC"));
	m_PngImageWordFail.LoadImage(hInstance, TEXT("WORDFAIL"));
	m_PngImageSeziSmall.LoadImage(hInstance, TEXT("DICESMALL"));

	/*if(!m_PngOpenDice0.LoadImage(hInstance, TEXT("AniTDtop")))
	{
		AfxMessageBox(TEXT("资源文件装载失败"));

	};
	if(!m_PngOpenDice1.LoadImage(hInstance, TEXT("AniTDBottom")))
	{
		AfxMessageBox(TEXT("资源文件装载失败"));

	};*/


	TCHAR szBuffer[128]=TEXT("");
	for (int i = 0;i<29;i++)
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("ANITDTOP_%d"),i+1);
		m_PngOpenDiceTop[i].LoadImage(hInstance,szBuffer);

		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("ANITDBOTTOM-%d"),i+1);
		m_PngOpenDiceBottom[i].LoadImage(hInstance,szBuffer);
	}

	//创建控件
	CRect CreateRect(0,0,0,0);
	m_ScoreView.Create(IDD_GAME_SCORE,this);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btSetChip[0].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SET_CHIP_0);
	m_btSetChip[1].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SET_CHIP_1);
	m_btSetChip[2].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SET_CHIP_2);
	m_btSetChip[3].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SET_CHIP_3);
	m_btThrowDice.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_THROW_DICE);						
	m_btLookDice.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_LOOK_DICE);						
	m_btYellOK.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_YELL_OK);	
	m_btDiceScroll[0].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SCROLL_UP);
	m_btDiceScroll[1].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SCROLL_DOWN);
	for (int i = 0; i < 6; i++)
		m_btDiceNum[i].Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_DICE_NUM_1+i);
	m_btOpenDice.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OPEN_DICE);						

	//设置按钮
	//HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_START,hInstance,false,false);
	m_btSetChip[0].SetButtonImage(IDB_BT_CHIP_1,hInstance, false,false);
	m_btSetChip[1].SetButtonImage(IDB_BT_CHIP_2,hInstance, false,false);
	m_btSetChip[2].SetButtonImage(IDB_BT_CHIP_3,hInstance,false,false);
	m_btSetChip[3].SetButtonImage(IDB_BT_CHIP_4,hInstance, false,false);
	m_btThrowDice.SetButtonImage(IDB_BT_THROW_DICE,hInstance,false,false);
	m_btLookDice.SetButtonImage(IDB_BT_LOOK_DICE,hInstance,false,false);
	m_btYellOK.SetButtonImage(IDB_BT_YELL_OK,hInstance,false,false);
	m_btDiceScroll[0].SetButtonImage(IDB_BT_SCROLL_UP,hInstance, false,false);
	m_btDiceScroll[1].SetButtonImage(IDB_BT_SCROLL_DOWN, hInstance,false,false);
	for (int i = 0; i < 6; i++)
		m_btDiceNum[i].SetButtonImage(IDB_BT_DICE_NUM_1+i,hInstance,false,false);
	m_btOpenDice.SetButtonImage(IDB_BT_OPEN_DICE,hInstance,false,false);

	return 0;
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//游戏变量
	m_bDiceCount = 0;
	m_bNowDicePoint = 0;
	m_lChip = 0;
	ZeroMemory(m_bShowDiceCup, sizeof(m_bShowDiceCup));
	ZeroMemory(m_nAnimalTime, sizeof(m_nAnimalTime));
	ZeroMemory(m_bShowDice, sizeof(m_bShowDice));
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(&m_MaxYell, sizeof(tagDiceYell));
	m_SceneOpenDice.Init();

	return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//积分视图
	CRect rcControl;
	m_ScoreView.ShowWindow(SW_HIDE);
	m_ScoreView.GetWindowRect(&rcControl);

	//玩家信息
	m_ptAvatar[0].x=nWidth-67-m_PngImageUserFrame.GetWidth();
	m_ptAvatar[0].y=18;
	m_ptNickName[0].x=nWidth-67-m_PngImageUserFrame.GetWidth()+m_nXFace+42+22;
	m_ptNickName[0].y=30;
	m_ptClock[0].x=nWidth/2-m_nXFace/2-m_nXTimer*2+223;
	m_ptClock[0].y=28;
	m_ptReady[0].x=nWidth/2-m_nXTimer-m_nXFace/2+122;
	m_ptReady[0].y=m_nYTimer/2-4;
	m_ptDiceCup[0].x=nWidth/2-m_PngImageDiceCup.GetWidth()/2;
	m_ptDiceCup[0].y=nHeight/2-278;
	m_ptChip[0].x=nWidth/2+74;
	m_ptChip[0].y=nHeight/2-205;
	m_ptHandDice[0].x=nWidth/2 - long(34 * 2.5) - long(4 * 2.5);
	m_ptHandDice[0].y=nHeight/2-213;
	m_ptWordOpen[0].x=nWidth/2-42;
	m_ptWordOpen[0].y=nHeight/2-65-30;


	m_ptAvatar[1].x=88;
	m_ptAvatar[1].y=nHeight-m_PngImageUserFrame.GetHeight()+15;
	m_ptNickName[1].x=88+m_nXFace+42+22;
	m_ptNickName[1].y=nHeight-m_PngImageUserFrame.GetHeight()+27;
	m_ptClock[1].x=nWidth/2-m_nXFace/2-m_nXTimer*2+30;
	m_ptClock[1].y=nHeight-m_nYTimer+20;
	m_ptReady[1].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[1].y=nHeight-m_nYTimer/2-5;
	m_ptDiceCup[1].x=nWidth/2-m_PngImageDiceCup.GetWidth()/2;
	m_ptDiceCup[1].y=nHeight/2+165;
	m_ptChip[1].x=nWidth/2+-203;
	m_ptChip[1].y=nHeight/2+88;
	m_ptHandDice[1].x=nWidth/2 - long(34 * 2.5) - long(4 * 2.5);
	m_ptHandDice[1].y=nHeight/2+230;
	m_ptWordOpen[1].x=nWidth/2-42;
	m_ptWordOpen[1].y=nHeight/2+30;

	m_ptYellPanel.x=nWidth/2+22;
	m_ptYellPanel.y=nHeight/2+79;
	m_ptMaxYell.x=nWidth/2;
	m_ptMaxYell.y=nHeight/2;

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(20);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp, m_btStart, NULL, (nWidth - rcButton.Width()) / 2, ( nHeight - rcButton.Height() ) / 2 + 120, 0, 0, uFlags);
	
	m_btSetChip[0].GetWindowRect(&rcButton);
	DeferWindowPos(hDwp, m_btSetChip[0], NULL, (nWidth - rcButton.Width() * 4 - 3 * 5 ) / 2 , ( nHeight - rcButton.Height() ) / 2 + 120 - rcButton.Height() - 5, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btSetChip[1], NULL, (nWidth - rcButton.Width() * 4 - 3 * 5 ) / 2 + rcButton.Width() + 5  , ( nHeight - rcButton.Height() ) / 2 + 120 - rcButton.Height() - 5, 0, 0, uFlags);
	
	
	DeferWindowPos(hDwp, m_btSetChip[2], NULL, (nWidth - rcButton.Width() * 4 - 3 * 5 ) / 2 + rcButton.Width() * 2 + 10 , ( nHeight - rcButton.Height() ) / 2 + 120 - rcButton.Height() - 5, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btSetChip[3], NULL, (nWidth - rcButton.Width() * 4 - 3 * 5 ) / 2 + rcButton.Width() * 3 + 15 , ( nHeight - rcButton.Height() ) / 2 + 120 - rcButton.Height() - 5, 0, 0, uFlags);

	m_btOpenDice.GetWindowRect(&rcButton);	
	DeferWindowPos(hDwp, m_btThrowDice, NULL, nWidth/2 + 58, (nHeight - 40)/2 + 197, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btLookDice, NULL, nWidth/2 + 78, (nHeight - 40)/2 + 297, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btYellOK, NULL, nWidth - rcButton.Width() - 15, nHeight - (rcButton.Height()+5)*2 , 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btOpenDice, NULL, nWidth - rcButton.Width() - 15, nHeight - (rcButton.Height()+5), 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btDiceScroll[0], NULL, m_ptYellPanel.x+27, m_ptYellPanel.y-26, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btDiceScroll[1], NULL, m_ptYellPanel.x+27, m_ptYellPanel.y+77, 0, 0, uFlags);
	for (int i = 0; i < 6; i++)
		DeferWindowPos(hDwp, m_btDiceNum[i], NULL, m_ptYellPanel.x+65+43*i, m_ptYellPanel.y+16, 0, 0, uFlags);

	EndDeferWindowPos(hDwp);

	//动画位置
	//SetFlowerControlInfo(0,nWidth/2,m_nYFace+BIG_FACE_HEIGHT);
	//SetFlowerControlInfo(1,nWidth/2,nHeight-m_nYFace-BIG_FACE_HEIGHT);
	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageCenter,DRAW_MODE_CENTENT);

	//摇骰动画
	if (m_nAnimalTime[0]>0)
		DrawThrowDice(pDC, 0, m_nAnimalTime[0], nWidth, nHeight);
	if (m_nAnimalTime[1]>0)
		DrawThrowDice(pDC, 1, m_nAnimalTime[1], nWidth, nHeight);


	/*int nImageWidth = m_PngOpenDice0.GetWidth()/29;
	int nImageHeight = m_PngOpenDice0.GetHeight();
	int nFrame=10;
	{
		m_PngOpenDice0.DrawImage(pDC, nWidth/2, nHeight/2, nImageWidth, nImageHeight, nImageWidth*nFrame, 0, nImageWidth, nImageHeight);
	}
	
	{
		m_PngOpenDice1.DrawImage(pDC, nWidth/2, nHeight/2, nImageWidth, nImageHeight, nImageWidth*nFrame, 0, nImageWidth, nImageHeight);
	}*/

	//开骰场景
	DrawOpenDice(pDC, nWidth, nHeight);

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=0;
		IClientUserItem * pClientUserItem=GetClientUserItem(i);

		//绘画用户
		if (pClientUserItem!=NULL)
		{
			wUserTimer=GetUserClock(i);
			
			//用户底框
			m_PngImageUserFrame.DrawImage(pDC, m_ptAvatar[i].x-15, m_ptAvatar[i].y-15);

			//显示名字
			pDC->SetTextAlign(TA_CENTER|TA_TOP);
			pDC->SetTextColor(RGB(240, 240, 240));
			//pDC->TextOut(m_ptNickName[i].x, m_ptNickName[i].y, pClientUserItem->GetNickName(), lstrlen(pClientUserItem->GetNickName())) ;

			CDFontEx::DrawText(this,pDC,  12, 400, pClientUserItem->GetNickName(), m_ptNickName[i].x, m_ptNickName[i].y, RGB(240, 240, 240), TA_CENTER|TA_TOP);

			//举手
			if (pClientUserItem->GetUserStatus() == US_READY)
			{
				DrawUserReady(pDC, m_ptReady[i].x, m_ptReady[i].y);
			}
			//其他信息
			if (wUserTimer!=0) DrawUserClock(pDC, m_ptClock[i].x, m_ptClock[i].y, wUserTimer);

			DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pClientUserItem);

			//小筹码图
			if (i == 0)
				m_PngImageJettonSmall.DrawImage(pDC, m_ptAvatar[i].x+6, m_ptAvatar[i].y+45, 21, 21, 51, 0, 51, 51);
			else
				m_PngImageJettonSmall.DrawImage(pDC, m_ptAvatar[i].x+6, m_ptAvatar[i].y+45, 21, 21, 0, 0, 51, 51);

			//显示金币
			LONGLONG lMeScore = pClientUserItem->GetUserScore();
			CString strScore;

			strScore.Format(TEXT("￥%I64d"), lMeScore);
			//pDC->TextOut(m_ptNickName[i].x, m_ptNickName[i].y  + 14, strScore);

			CDFontEx::DrawText(this,pDC,  12, 400, strScore, m_ptNickName[i].x, m_ptNickName[i].y+14, RGB(240, 240, 240), TA_CENTER|TA_TOP);

		}
	}

	//最大喊话
	DrawMaxYell(pDC, nWidth, nHeight);
	//绘画骰盅
	DrawDiceCup(pDC);
	//绘画下注
	DrawChip(pDC);
	//手中骰子
	DrawSezi(pDC);
	//喊话面板
	DrawYellPanel(pDC);

	return;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength = lstrlen(pszString);
	int nXExcursion[8] = {1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8] = {-1,0,1,1,1,0,-1,-1};

	pDC->SetTextAlign(TA_CENTER | TA_BASELINE);

	//绘画边框
	//pDC->SetTextColor(crFrame);
	for (int i = 0; i < CountArray(nXExcursion); i++)
	{
		//pDC->TextOut(nXPos+nXExcursion[i], nYPos+nYExcursion[i], pszString, nStringLength);

		TextOut(pDC,nXPos+nXExcursion[i], nYPos+nYExcursion[i], pszString);

		//CDFontEx::DrawText(this,pDC,  nStringLength, 12, pszString, nXPos+nXExcursion[i], nYPos+nYExcursion[i], crFrame, TA_CENTER|TA_TOP);
	}

	//绘画字体
	//pDC->SetTextColor(crText);
	//pDC->TextOut(nXPos, nYPos, pszString, nStringLength);
	//CDFontEx::DrawText(this,pDC,  nStringLength, 12, pszString, nXPos, nYPos, crText, TA_CENTER|TA_TOP);

	TextOut(pDC,nXPos, nYPos, pszString);

	return;
}

//摇骰动画
void CGameClientView::DrawThrowDice(CDC *pDC, WORD wViewChair, int nTime, int nX, int nY)
{
	
	if (nTime <= 0 || nTime > 29+24*2)		return;	//共29+24*2帧

	if (wViewChair<0||wViewChair>1) return;

	int nImageWidth = m_PngOpenDiceTop[0].GetWidth();
	int nImageHeight = m_PngOpenDiceTop[0].GetHeight();
	int nDesX = (nX-nImageWidth)/2;
	int nDesY = 0;
	int nFrame = 0;
	if (nTime > 0 && nTime <= 29)			nFrame = 29 - nTime;
	else if (nTime > 29 && nTime <= 53)		nFrame = nTime - 30;
	else if (nTime > 53 && nTime <= 77)		nFrame = nTime - 54;

	if (wViewChair == 0)
	{
		nDesX -= 7;
		nDesY = nY/2 - 349;
	}
	else
	{
		nDesX += 18;
		nDesY = nY/2 + 151;
	}
	if(nFrame<0||nFrame>29) 
	{
		return;
	}

	//m_ImageThrowDiceAnimal[wViewChair].TransDrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight-10, 
	//					nImageWidth*nFrame, 0, RGB(255,0,255));


	//int nImageWidth = m_PngOpenDice0.GetWidth()/29;
	//int nImageHeight = m_PngOpenDice0.GetHeight();

	if (wViewChair == 0)
	{
		//m_PngOpenDice0.DrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight, nImageWidth*nFrame, 0, nImageWidth, nImageHeight);

		m_PngOpenDiceTop[nFrame].DrawImage(pDC, nDesX, nDesY);
	}
	else
	{
		//m_PngOpenDice1.DrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight, nImageWidth*nFrame, 0, nImageWidth, nImageHeight);
		m_PngOpenDiceBottom[nFrame].DrawImage(pDC, nDesX, nDesY);
	}

	//m_PngOpenDice[wViewChair].DrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight-10, nImageWidth*nFrame, 0);
}

//绘画骰子
void CGameClientView::DrawSezi(CDC *pDC)
{
	int nImageWidth = m_ImageSezi.GetWidth();
	int nImageHeight = m_ImageSezi.GetHeight();
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (m_bShowDice[i])
		{
			//画背景
			CDC tmpDC;

			CBitmap bitmap;
			bitmap.LoadBitmap(IDB_SEZI_BACK);
			tmpDC.CreateCompatibleDC(pDC);
			tmpDC.SelectObject(&bitmap);

			BLENDFUNCTION   bf = {};
			bf.AlphaFormat=0;   
			bf.BlendFlags=0;   
			bf.BlendOp=AC_SRC_OVER;   
			bf.SourceConstantAlpha=122;
			AlphaBlend(pDC->GetSafeHdc(), m_ptHandDice[i].x-7, m_ptHandDice[i].y-6, 200, 50, tmpDC.GetSafeHdc(), 0, 0, 200, 50, bf);

			//画骰子
			for (int j = 0; j < 5; j++)
			{
				//m_bDiceData[i][j] = 5;
				if (m_bDiceData[i][j] != 0)
					m_ImageSezi.TransDrawImage(pDC, m_ptHandDice[i].x+j*38, m_ptHandDice[i].y, nImageWidth/6, nImageHeight, 
						nImageWidth/6*(m_bDiceData[i][j]-1), 0, RGB(255,0,255));
			}
		}
	}	
}

//绘画骰盅
void CGameClientView::DrawDiceCup(CDC * pDC)
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (m_bShowDiceCup[i])
		{
			m_PngImageDiceCup.DrawImage(pDC, m_ptDiceCup[i].x, m_ptDiceCup[i].y);
		}
	}
}

//绘画下注
void CGameClientView::DrawChip(CDC *pDC)
{	
	int nWidth = m_PngImageJetton.GetWidth();
	int nHeight = m_PngImageJetton.GetHeight();
	if (m_lChip > 0)
	{
		//酒瓶
		m_PngImageJetton.DrawImage(pDC, m_ptChip[0].x, m_ptChip[0].y);
		m_PngImageJetton.DrawImage(pDC, m_ptChip[1].x, m_ptChip[1].y);

		//下注
		CString strChip;
		strChip.Format(_T("%I64d"), m_lChip);
		CFont InfoFont;

		InfoFont.CreateFont(16,0,0,0,100,0,0,0,134,3,2,1,2,TEXT("宋体"));
		pDC->SetTextColor(RGB(255,255,255));
		CFont * pOldFont=pDC->SelectObject(&InfoFont);
		pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

		//pDC->TextOut(m_ptChip[0].x+nWidth/2, m_ptChip[0].y+nHeight+18, strChip);
		//pDC->TextOut(m_ptChip[1].x+nWidth/2, m_ptChip[1].y+nHeight+18, strChip);

		
		TextOut(pDC,m_ptChip[0].x+nWidth/2, m_ptChip[0].y+nHeight+18, strChip);
		TextOut(pDC,m_ptChip[1].x+nWidth/2, m_ptChip[1].y+nHeight+18, strChip);

		//CDFontEx::DrawText(this,pDC,  12, 400, strChip, m_ptChip[0].x+nWidth/2,m_ptChip[0].y+nHeight+18, RGB(255,255,255), TA_CENTER|TA_BOTTOM);
		//CDFontEx::DrawText(this,pDC,  12, 400, strChip, m_ptChip[1].x+nWidth/2,m_ptChip[0].y+nHeight+18, RGB(255,255,255), TA_CENTER|TA_BOTTOM);

		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}
}

//绘画喊话 (用来显示几个几)
void CGameClientView::DrawYell(CDC *pDC, BYTE bDiceCount, BYTE bDicePoint, int nX, int nY)
{
		//个数
		DrawPicNum(pDC, &m_PngImageNum2, bDiceCount, nX-53, nY);

		//字
		int nImgWordWidth = m_PngImageWord.GetWidth();
		int nImgWordHeight = m_PngImageWord.GetHeight();

		m_PngImageWord.DrawImage(pDC, nX-nImgWordWidth/4, nY-nImgWordHeight/2, nImgWordWidth/2, nImgWordHeight,
					nImgWordWidth/2, 0);
		
		//骰子
		if (bDicePoint >= 1 && bDicePoint <= 6)
		{
			int nImageWidth = m_ImageSezi.GetWidth();
			int nImageHeight = m_ImageSezi.GetHeight();
			m_ImageSezi.TransDrawImage(pDC, nX+nImgWordWidth/4+15, nY-nImageHeight/2, nImageWidth/6, nImageHeight, 
					nImageWidth/6*(bDicePoint-1), 0, RGB(255,0,255));
		}
}

//最大喊话
void CGameClientView::DrawMaxYell(CDC *pDC, int nWidth, int nHeight)
{
	/*m_MaxYell.bDiceCount = 3;
	m_MaxYell.bDicePoint = 5;*/
	//最大喊话
	if (m_MaxYell.bDiceCount != 0 && m_MaxYell.bDicePoint != 0)
	{
		DrawYell(pDC, m_MaxYell.bDiceCount, m_MaxYell.bDicePoint, m_ptMaxYell.x, m_ptMaxYell.y);
	}
}

//喊话面板
void CGameClientView::DrawYellPanel(CDC *pDC)
{
	/*m_bDiceCount = 8;
	m_bNowDicePoint = 6;*/
	if (m_bDiceCount >=2 && m_bDiceCount <= 12)
	{
		//背景
		int nWidth = m_PngImageYellFrame.GetWidth();
		int nHeight = m_PngImageYellFrame.GetHeight();
		m_PngImageYellFrame.DrawImage(pDC, m_ptYellPanel.x, m_ptYellPanel.y);

		//个数
		DrawPicNum(pDC, &m_PngImageNum1, m_bDiceCount, m_ptYellPanel.x+42, m_ptYellPanel.y+nHeight/2);

		//结果显示
		DrawYell(pDC, m_bDiceCount, m_bNowDicePoint,  m_ptYellPanel.x+135, m_ptYellPanel.y+124);
	}
}

//开骰场景
void CGameClientView::DrawOpenDice(CDC *pDC, int nX, int nY)
{
	/*m_SceneOpenDice.bODTime = 40;
	m_SceneOpenDice.bRealDiceCount = 5;
	m_SceneOpenDice.bDicePoint = 6;
	m_SceneOpenDice.bOpenRight = true;
	m_SceneOpenDice.wOpenUser = 0;*/
	if (m_SceneOpenDice.bODTime <= 0 || m_SceneOpenDice.bODTime > 55)		return ;

	//开骰动画
	int nImageWidth = m_ImageOpenDiceAnimal[0].GetWidth()/15;
	int nImageHeight = m_ImageOpenDiceAnimal[0].GetHeight();
	int nDesX = (nX-nImageWidth)/2;
	int nDesY = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (i == 0)
		{
			nDesX -= 7;
			nDesY = nY/2 - 352;
		}
		else
		{
			nDesX += 18;
			nDesY = nY/2 + 158;
		}
		if (m_SceneOpenDice.bODTime >= 41 && m_SceneOpenDice.bODTime <= 55)		
			m_ImageOpenDiceAnimal[i].TransDrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight-10, 
				nImageWidth*(m_SceneOpenDice.bODTime-41), 0, RGB(255,0,255));
		else
			m_ImageOpenDiceAnimal[i].TransDrawImage(pDC, nDesX, nDesY, nImageWidth, nImageHeight-10, 
				0, 0, RGB(255,0,255));
	}

	//显示骰子
	if (m_SceneOpenDice.bODTime >= 1 && m_SceneOpenDice.bODTime <= 46)
	{
		int nWidth = m_PngImageSeziSmall.GetWidth() / 6;
		int nHeight = m_PngImageSeziSmall.GetHeight();
		int nDesX = (nX-nWidth)/2;
		int nDesY = 0;
		int nPosX[5] = {1, -4, -2, 3, 5};
		int nPosY[5] = {1, 4, 9, 9, 4};
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i == 0)
				nDesY = nY/2 - 352 + 100;
			else
				nDesY = nY/2 + 158 + 32;
			
			for (int j = 0; j < 5; j++)
				m_PngImageSeziSmall.DrawImage(pDC, nDesX+nPosX[j]*5, nDesY+nPosY[j]*5, nWidth, nHeight, nWidth*(m_bDiceData[i][j]-1), 0);		
		}
	}
	

	if (m_SceneOpenDice.bODTime <= 40 && m_SceneOpenDice.bODTime >= 1)	//实际数目
	{
		int nImageWidth = m_PngImageWord.GetWidth();
		int nImageHeight = m_PngImageWord.GetHeight();

		//"共"字
		m_PngImageWord.DrawImage(pDC, m_ptMaxYell.x-nImageWidth/4-72, m_ptMaxYell.y-nImageHeight/2+50, nImageWidth/2, nImageHeight, 0, 0);

		//实际数目
		DrawYell(pDC, m_SceneOpenDice.bRealDiceCount, m_SceneOpenDice.bDicePoint, m_ptMaxYell.x+nImageWidth/4+8, m_ptMaxYell.y+50);

		if (m_SceneOpenDice.bODTime >= 1+1 && m_SceneOpenDice.bODTime <= 20)//是否开中 (此图最后淡出后消失不显示所以为1+1)
		{
			int nFrame = 6 - int((m_SceneOpenDice.bODTime-2) / 2);
			if (nFrame < 0)		nFrame = 0;
			int nWordWidth = m_PngImageWordSucc.GetWidth() / 7;
			int nWordHeight = m_PngImageWordSucc.GetHeight();

			CPngImage *pImage = NULL;
			if (m_SceneOpenDice.bOpenRight)	
				pImage = &m_PngImageWordSucc;
			else
				pImage = &m_PngImageWordFail;
			
			pImage->DrawImage(pDC, m_ptMaxYell.x-nWordWidth/2, m_ptMaxYell.y-nWordHeight-35, nWordWidth, nWordHeight,
				nWordWidth*nFrame, 0, nWordWidth, nWordHeight);		
		}
	}
}

//绘画数字	(居中绘画)
void CGameClientView::DrawPicNum(CDC *pDC, CPngImage *pImage, BYTE bNum, int nX, int nY)
{
	int nWidth = pImage->GetWidth() / 10;
	int nHeight = pImage->GetHeight();
	if (bNum >= 0 && bNum <= 9)
	{
		pImage->DrawImage(pDC, nX-nWidth/2, nY-nHeight/2, nWidth, nHeight, nWidth*bNum, 0);
	}
	else if (bNum >= 10 && bNum <= 99)
	{
		int nUnits = bNum % 10;
		int nTen = int(bNum / 10);
		pImage->DrawImage(pDC, nX-nWidth-2, nY-nHeight/2, nWidth, nHeight, nWidth*nTen, 0);
		pImage->DrawImage(pDC, nX-7, nY-nHeight/2, nWidth, nHeight, nWidth*nUnits, 0);
	}
}

//更新视图
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}

//更新按钮
void CGameClientView::UpdateButton(enUpdateButton nType)
{
	//按钮状态结构体
	tagButtonState bState = {0};

	//设置状态
	switch (nType)
	{			 //开始 下注 摇骰 看骰 喊话 翻页 骰点 开骰		
	case enUBClickBegin:
		bState.Set(0,   0,	  0,   0,   0,   0,   0,   0);
		break;
	case enUBChip:
		bState.Set(0,   3,	  0,   0,   0,   0,   0,   0);
		break;
	case enUBYellFirst:
		bState.Set(0,   0,	  0,   3,   2,   3,   0,   0);
		break;
	case enUBYell:
		bState.Set(0,   0,	  0,   3,   2,   3,   0,   3);
		break;
	case enUBOpen:	
		bState.Set(0,   0,	  0,   0,   0,   0,   0,   0);
		break;
	case enUBEnd:
		bState.Set(3,   0,	  0,   0,   0,   0,   0,   0);
		break;
	case enTest:
		bState.Set(3,   3,	  3,   3,   3,   3,   3,   3);
		break;
	default:
		CString strDebug;
		strDebug.Format(_T("CGameClientView::UpdateButton nType = %d"), nType);
		OutputDebugString(strDebug);
		return;
	}

	//更新按钮
	m_btStart.ShowWindow(bState.bStart/2==1?SW_SHOW:SW_HIDE);
	m_btStart.EnableWindow(bState.bStart%2==1);
	for (int i = 0; i < 4; i++)
	{
		m_btSetChip[i].ShowWindow(bState.bSetChip/2==1?SW_SHOW:SW_HIDE);
		m_btSetChip[i].EnableWindow(bState.bSetChip%2==1);
	}
	m_btThrowDice.ShowWindow(bState.bThrowDice/2==1?SW_SHOW:SW_HIDE);
	m_btThrowDice.EnableWindow(bState.bThrowDice%2==1);
	m_btLookDice.ShowWindow(bState.bLookDice/2==1?SW_SHOW:SW_HIDE);
	m_btLookDice.EnableWindow(bState.bLookDice%2==1);
	m_btYellOK.ShowWindow(bState.bYellOK/2==1?SW_SHOW:SW_HIDE);
	m_btYellOK.EnableWindow(bState.bYellOK%2==1);
	for (int i = 0; i < 2; i++)
	{
		m_btDiceScroll[i].ShowWindow(bState.bDiceScroll/2==1?SW_SHOW:SW_HIDE);
		m_btDiceScroll[i].EnableWindow(bState.bDiceScroll%2==1);
	}
	for (int i = 0; i < 6; i++)
	{
		m_btDiceNum[i].ShowWindow(bState.bDiceNum/2==1?SW_SHOW:SW_HIDE);
		m_btDiceNum[i].EnableWindow(bState.bDiceNum%2==1);
	}
	m_btOpenDice.ShowWindow(bState.bOpenDice/2==1?SW_SHOW:SW_HIDE);
	m_btOpenDice.EnableWindow(bState.bOpenDice%2==1);
}

//设置骰子
void CGameClientView::SetHandDice(WORD wChairID, bool bShow, BYTE bDiceData[])
{
	m_bShowDice[wChairID] = bShow;
	if (bDiceData != NULL)
		memcpy(m_bDiceData[wChairID], bDiceData, sizeof(BYTE)*5);

	RefreshGameView();
}

//摇骰动画
void CGameClientView::OnThrowAnimal(WORD wChairID, enAnimalAction enAction)
{
	
	if (enAction == enTABegin)
	{
		m_bShowDiceCup[wChairID] = false;
		m_nAnimalTime[wChairID] = 29+24*2;
	}
	else if (enAction == enTAPlay)
	{
		m_bShowDiceCup[wChairID] = false;
		m_nAnimalTime[wChairID] -= 2;
		if (m_nAnimalTime[wChairID] < 1)	m_nAnimalTime[wChairID] = 1;
	}
	else if (enAction == enTAEnd)
	{
		m_bShowDiceCup[wChairID] = true;
		m_nAnimalTime[wChairID] = 0;
	}

	RefreshGameView();
	/*static int a = 0;
	int b = GetTickCount() - a;
	if (b >= 50)
	{
		RefreshGameView();
		a = GetTickCount();
	}*/
}

//开始按钮
void CGameClientView::OnStart()
{
	SendEngineMessage(IDM_START,0,0);

	//更新界面
	RefreshGameView();

	return;
}

//设置压注
void CGameClientView::OnSetChip(UINT nCtrlID) 
{
	UINT nCurPort = nCtrlID - IDC_SET_CHIP_0 + 1;
	SendEngineMessage(IDM_SETCHIP, (WPARAM)nCurPort, 0) ;
	return;
}

//摇骰按钮
void CGameClientView::OnThrowDice()
{
	SendEngineMessage(IDM_THROW_DICE, 0, 0) ;
	return;
}

//看骰按钮
void CGameClientView::OnLookDice()
{
	SendEngineMessage(IDM_LOOK_DICE, 0, 0);
	return;
}

//上翻按钮
void CGameClientView::OnScrollUp()
{
	SendEngineMessage(IDM_SCROLL, 0, 0);
	return;
}

//下翻按钮
void CGameClientView::OnScrollDown()
{
	SendEngineMessage(IDM_SCROLL, 1, 0);
	return;
}

//骰点按钮
void CGameClientView::OnDiceNum(UINT nCtrlID)
{
	UINT nCurPort = nCtrlID - IDC_DICE_NUM_1 + 1;
	SendEngineMessage(IDM_DICE_NUM, (WPARAM)nCurPort, 0);
	return;
}

//喊话按钮
void CGameClientView::OnYellOk()
{
	SendEngineMessage(IDM_YELL_DICE, 0, 0);
	return;
}

//开骰按钮
void CGameClientView::OnOpenDice()
{
	SendEngineMessage(IDM_OPEN_DICE, 0, 0);
	return;
}
//////////////////////////////////////////////////////////////////////////