#include "StdAfx.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识

//控制按钮
#define IDC_START					100									//开始按钮
#define IDC_REGRET					101									//悔棋按钮
#define IDC_PEACE					102									//求和按钮
#define IDC_GIVEUP					103									//认输按钮
#define IDC_PRESERVE				104									//保存按钮
#define IDC_MANUAL					105									//棋谱按钮

//棋谱按钮
#define IDC_MANUAL_BT_HEAD			200									//棋谱按钮
#define IDC_MANUAL_BT_LAST			201									//棋谱按钮
#define IDC_MANUAL_BT_OPEN			202									//棋谱按钮
#define IDC_MANUAL_BT_NEXT			203									//棋谱按钮
#define IDC_MANUAL_BT_TAIL			204									//棋谱按钮

//控件标识
#define IDC_MANUAL_LIST				300									//棋谱标识

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)

	//系统消息
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//控制按钮
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_PEACE, OnBnClickedPeace)
	ON_BN_CLICKED(IDC_REGRET, OnBnClickedRegret)
	ON_BN_CLICKED(IDC_GIVEUP, OnBnClickedGiveUp)
	ON_BN_CLICKED(IDC_MANUAL, OnBnClickedManual)
	ON_BN_CLICKED(IDC_PRESERVE, OnBnClickedPreserve)

	//棋谱按钮
	ON_BN_CLICKED(IDC_MANUAL_BT_HEAD, OnBnClickedHead)
	ON_BN_CLICKED(IDC_MANUAL_BT_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_MANUAL_BT_OPEN, OnBnClickedOpen)
	ON_BN_CLICKED(IDC_MANUAL_BT_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_MANUAL_BT_TAIL, OnBnClickedTail)

	//控件消息
	ON_LBN_SELCHANGE(IDC_MANUAL_LIST, OnLbnSelchangeManualList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//游戏变量
	m_nFrameWidth = 0;
	m_nFrameHeight = 0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(&m_RuleStatusInfo,sizeof(m_RuleStatusInfo));
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//设置棋盘
	m_ChessBorad.SetChessBoradMode((GetSystemMetrics(SM_CXSCREEN)<=800));

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
void CGameClientView::ResetGameView()
{
	//__super::ResetGameView();

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(&m_RuleStatusInfo,sizeof(m_RuleStatusInfo));
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//控制按钮
	m_btStart.EnableWindow(FALSE);
	m_btPeace.EnableWindow(FALSE);
	m_btRegret.EnableWindow(FALSE);
	m_btGiveUp.EnableWindow(FALSE);
	m_btPreserve.EnableWindow(FALSE);

	//棋谱按钮
	m_btManualHead.EnableWindow(FALSE);
	m_btManualLast.EnableWindow(FALSE);
	m_btManualNext.EnableWindow(FALSE);
	m_btManualTail.EnableWindow(FALSE);

	//游戏控件
	m_ManualList.ResetContent();
	m_ScoreControl.RestorationData();

	//关闭窗口
	if (m_DlgRuleOption.m_hWnd!=NULL) m_DlgRuleOption.DestroyWindow();
	if (m_UserRequestPeace.m_hWnd!=NULL) m_UserRequestPeace.DestroyWindow();
	if (m_UserRequestRegret.m_hWnd!=NULL) m_UserRequestRegret.DestroyWindow();
	if (m_UserHint.m_hWnd!=NULL)m_UserHint.DestroyWindow();

	//设置棋盘
	m_ChessBorad.SetPositively(false);
	m_ChessBorad.SetManualView(false);
	m_ChessBorad.SetWaitGameRule(false);
	m_ChessBorad.SelectChessItem(INVALID_POS,INVALID_POS);
	m_ChessBorad.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);

	return ;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	if(m_nFrameWidth<400) m_nFrameWidth = nWidth;
	if(m_nFrameHeight<200) m_nFrameHeight = nHeight;


	INT nXFace=48;
	INT nYFace=48;
	INT nXTimer=65;
	INT nYTimer=69;
	INT nXBorder=15;

	//调整变量.
	m_EncircleInfoViewBack.nBBorder = 13;
	m_EncircleInfoViewBack.nLBorder = 176;
	m_EncircleInfoViewBack.nRBorder = 46;
	m_EncircleInfoViewBack.nTBorder = 14;
	INT nXBorderArea=m_EncircleInfoViewBack.nLBorder+m_EncircleInfoViewBack.nRBorder;
	INT nYBorderArea=m_EncircleInfoViewBack.nTBorder+m_EncircleInfoViewBack.nBBorder;
	bool bChessManual=((nWidth-m_EncircleInfoViewBack.nLBorder-m_EncircleInfoViewBack.nRBorder)>(m_SizeChessBorad.cx+m_nManualWidth));

	//棋盘位置
	INT nXChessBorad=nWidth/4/*-m_SizeChessBorad.cx-25*/;
	if(nWidth>m_nFrameWidth)nXChessBorad = m_nFrameWidth-m_SizeChessBorad.cx-25;
	INT nYChessBorad=nHeight/2-332;
	m_ChessBorad.SetBenchmarkPos(nXChessBorad,nYChessBorad);

	//控制位置
	m_ptControlView.y=nHeight-m_EncircleInfoViewBack.nBBorder-m_SizeControlView.cy;
	m_ptControlView.x=nXChessBorad;

	//用户位置
	m_ptUserInfoView.x=-5+nXBorder;
	m_ptUserInfoView.y=m_EncircleInfoViewBack.nTBorder+nYChessBorad+(m_SizeChessBorad.cy-m_SizeUserInfoView.cy)/2;

	//棋谱位置
	if ((bChessManual==true)&&(nWidth>m_nFrameWidth))
	{
		m_rcChessManual.top=m_EncircleInfoViewBack.nTBorder+10;
		m_rcChessManual.bottom=nHeight-m_EncircleInfoViewBack.nBBorder+10;
		m_rcChessManual.left=nWidth-nWidth/2+m_nFrameWidth/2-m_nManualWidth/2-m_EncircleInfoViewBack.nRBorder+35;
		m_rcChessManual.right=nWidth-nWidth/2+m_nFrameWidth/2+m_nManualWidth/2-m_EncircleInfoViewBack.nRBorder+35;
	}
	else m_rcChessManual.SetRect(0,0,0,0);

	//头像位置
	m_ptAvatar[0].x = m_ptUserInfoView.x+24;
	m_ptAvatar[0].y = m_ptUserInfoView.y+22;
	m_ptAvatar[1].x =	m_ptUserInfoView.x+24;
	m_ptAvatar[1].y = m_ptUserInfoView.y+312;

	//帐号位置
	m_ptNickName[0].x = m_ptUserInfoView.x+70;
	m_ptNickName[0].y = m_ptUserInfoView.y+86;
	m_ptNickName[1].x =	m_ptUserInfoView.x+70;
	m_ptNickName[1].y = m_ptUserInfoView.y+377;

	//准备位置
	m_ptReady[0].x = 220;
	m_ptReady[0].y = nHeight/2-50;
	m_ptReady[1].x = nWidth/2;
	m_ptReady[1].y = nHeight-220;

	//控制按钮
	CRect rcButton;
	m_btStart.GetWindowRect(&rcButton);

	//棋谱按钮
	CRect rcManualOpen;
	CRect rcManualControl;
	m_btManualOpen.GetWindowRect(&rcManualOpen);
	m_btManualHead.GetWindowRect(&rcManualControl);

	//位置计算
	INT nXControlBorad=30;
	INT nYControlExcursion=12;
	INT nXButton=m_ptControlView.x+nXControlBorad;
	INT nYButton=m_ptControlView.y+nYControlExcursion;
	INT nXButtonSpace=(m_SizeControlView.cx-rcButton.Width()*6-2*nXControlBorad)/7;

	//位置计算
	INT nYManualPos=m_rcChessManual.bottom-65;
	INT nXManualPos=m_rcChessManual.left+m_rcChessManual.Width()/2-rcManualOpen.Width()/2-rcManualControl.Width()*2;

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(42);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//控制按钮
	DeferWindowPos(hDwp,m_btStart,NULL,nXButton+nXButtonSpace,nYButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btRegret,NULL,nXButton+rcButton.Width()+nXButtonSpace*2,nYButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPeace,NULL,nXButton+rcButton.Width()*2+nXButtonSpace*3,nYButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nXButton+rcButton.Width()*3+nXButtonSpace*4,nYButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPreserve,NULL,nXButton+rcButton.Width()*4+nXButtonSpace*5,nYButton,0,0,uFlags);
	DeferWindowPos(hDwp,m_btManual,NULL,nXButton+rcButton.Width()*5+nXButtonSpace*6,nYButton,0,0,uFlags);

	//棋谱按钮
	DeferWindowPos(hDwp,m_btManualHead,NULL,nXManualPos,nYManualPos-rcManualControl.Height()/2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btManualLast,NULL,nXManualPos+rcManualControl.Width(),nYManualPos-rcManualControl.Height()/2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btManualOpen,NULL,nXManualPos+rcManualControl.Width()*2,nYManualPos-rcManualOpen.Height()/2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btManualNext,NULL,nXManualPos+rcManualControl.Width()*2+rcManualOpen.Width(),nYManualPos-rcManualControl.Height()/2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btManualTail,NULL,nXManualPos+rcManualControl.Width()*3+rcManualOpen.Width(),nYManualPos-rcManualControl.Height()/2,0,0,uFlags);

	//移动棋谱
	DeferWindowPos(hDwp,m_ManualList,NULL,m_rcChessManual.left+24,m_rcChessManual.top+93,m_rcChessManual.Width()-45,
		m_rcChessManual.Height()-188,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);

	//移动成绩
	CRect rcScoreControl;
	m_ScoreControl.GetWindowRect(&rcScoreControl);
	DeferWindowPos(hDwp,m_ScoreControl,NULL,m_EncircleInfoViewBack.nLBorder+nXChessBorad+(m_SizeChessBorad.cx-rcScoreControl.Width())/2-150,(nHeight-rcScoreControl.Height())*2/5,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//动画控件	
	/*SetFlowerControlInfo(1,m_ptAvatar[1].x,m_ptAvatar[1].y
		,BIG_FACE_WIDTH, BIG_FACE_HEIGHT, 30);
	SetFlowerControlInfo(0,m_ptAvatar[0].x,m_ptAvatar[0].y
		,BIG_FACE_WIDTH, BIG_FACE_HEIGHT, 30);*/

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	CRect rcViewBack(0,0,nWidth,nHeight);
	//DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	

	CDC* pDCBuff = pDC;

	//绘画背景
	for ( int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth() )
	{
		for ( int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight() )
		{
			m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
		}
	}
	//m_ImageViewBack.BitBlt( pDC->GetSafeHdc(), nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - m_ImageViewBack.GetHeight()/2 );



	m_ImageUserInfoView.BitBlt(pDC->m_hDC,m_ptUserInfoView.x,m_ptUserInfoView.y);


	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		
		IClientUserItem * pClientUserItem=GetClientUserItem(i);

		//tagUserInfo * pUserData=GetClientUserItem(i)->GetUserInfo();

		if (pClientUserItem==NULL) continue;

		//用户时间
		WORD wTimes = GetUserClock(i);

		//输出位置
		CRect rcAccounts(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+100,m_ptNickName[i].y+13);
		CRect rcLevelDescribe(m_ptNickName[i].x,m_ptNickName[i].y+20,m_ptNickName[i].x+100,m_ptNickName[i].y+13+20);

		//用户等级
		TCHAR pszLevelDescribe[LEN_ACCOUNTS] = TEXT("");
		GetLevelDescribe(pClientUserItem->GetUserScore(),pszLevelDescribe);

		//绘画用户
		pDC->SetTextColor(RGB(20,20,20));
		DrawUserAvatar(pDCBuff,m_ptAvatar[i].x,m_ptAvatar[i].y,pClientUserItem);
		if(wTimes>0)
			DrawUserClock(pDCBuff,m_ptAvatar[i].x+50,m_ptAvatar[i].y+28,wTimes);
		//pDC->DrawText(pClientUserItem->GetNickName(),lstrlen(pClientUserItem->GetNickName()),rcAccounts,DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
		//pDC->DrawText(pszLevelDescribe,lstrlen(pszLevelDescribe),rcLevelDescribe,DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);


		CDFontEx::DrawText(this,pDC, 12, 400, pClientUserItem->GetNickName(), rcAccounts, RGB(20,20,20), DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
		CDFontEx::DrawText(this,pDC,  12, 400, pszLevelDescribe, rcLevelDescribe, RGB(20,20,20), DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);

		//绘画标志
		if ((m_wBankerUser!=INVALID_CHAIR)&&(pClientUserItem->GetUserStatus()!=US_SIT))
		{
			//获取大小
			INT nImageWidht=m_ImageUserInfoFlag.GetWidth()/4;
			INT nImageHeight=m_ImageUserInfoFlag.GetHeight();

			//用户标志
			INT nXImagePos=(m_wBankerUser==i)?0:nImageWidht;
			if (m_wCurrentUser==i) nXImagePos+=2*nImageWidht;
			m_ImageUserInfoFlag.BitBlt(pDC->m_hDC,m_ptUserInfoView.x+77,m_ptUserInfoView.y+8+i*291,nImageWidht,nImageHeight,nXImagePos,0);
		
		}

		//游戏时间
		DrawUserClock(pDCBuff,m_ptNickName[i].x,m_ptNickName[i].y+42,m_UserStatusInfo[i].wDrawTimeCount);
		DrawUserClock(pDCBuff,m_ptNickName[i].x,m_ptNickName[i].y+62,m_UserStatusInfo[i].wStepTimeCount);
		DrawUserClock(pDCBuff,m_ptNickName[i].x,m_ptNickName[i].y+82,m_UserStatusInfo[i].wSecondTimeCount);
	}

	//规则信息
	INT nXPos=m_ptUserInfoView.x+70;
	INT nYPos=m_ptUserInfoView.y+218;
	DrawUserClock(pDCBuff,nXPos,nYPos,m_RuleStatusInfo.wDrawTimeCount);
	DrawUserClock(pDCBuff,nXPos,nYPos+20,m_RuleStatusInfo.wStepTimeCount);
	DrawUserClock(pDCBuff,nXPos,nYPos+40,m_RuleStatusInfo.wSecondTimeCount);

	//绘画棋盘
	m_ChessBorad.DrawChessBorad(pDCBuff);

	//控制底图
	
	m_ImageControlBack.BitBlt(pDC->m_hDC,m_ptControlView.x,m_ptControlView.y);

	
	//绘画棋谱
	if (m_rcChessManual.Width()>0)
	{
		//棋谱头尾
		m_ImageChessManualT.BitBlt(pDC->m_hDC,m_rcChessManual.left,m_rcChessManual.top);
		m_ImageChessManualB.BitBlt(pDC->m_hDC,m_rcChessManual.left,m_rcChessManual.bottom-m_ImageChessManualB.GetHeight());

		
		//棋谱底部
		INT nYStartPos=m_rcChessManual.top+m_ImageChessManualT.GetHeight();
		INT nYConcludePos=m_rcChessManual.bottom-m_ImageChessManualB.GetHeight();
		for (INT nYPos=nYStartPos;nYPos<nYConcludePos;nYPos+=m_ImageChessManualM.GetHeight())
		{
			INT nDrawHeight=__min(m_ImageChessManualM.GetHeight(),nYConcludePos-nYPos);
			m_ImageChessManualM.BitBlt(pDC->m_hDC,m_rcChessManual.left,nYPos,m_nManualWidth,nDrawHeight,0,0);
			
		}
	}

	return;
}

//庄家用户
bool CGameClientView::SetBankerUser(WORD wBankerUser)
{
	if (m_wBankerUser!=wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		RefreshGameView();
	}

	return true;
}

//当前用户
bool CGameClientView::SetCurrentUser(WORD wCurrentUser)
{
	if (m_wCurrentUser!=wCurrentUser)
	{
		//设置变量
		m_wCurrentUser=wCurrentUser;

		//更新界面
		RefreshGameView();
	}

	return true;
}

//设置规则
bool CGameClientView::SetGameRuleInfo(const tagGameRuleInfo & GameRuleInfo)
{
	//设置变量
	m_RuleStatusInfo.wDrawTimeCount=GameRuleInfo.wRuleDrawTime;
	m_RuleStatusInfo.wStepTimeCount=GameRuleInfo.wRuleStepTime;
	m_RuleStatusInfo.wSecondTimeCount=GameRuleInfo.wRuleSecondTime;

	//构造位置
	CRect rcUserInfoView;
	rcUserInfoView.top=m_ptUserInfoView.y;
	rcUserInfoView.left=m_ptUserInfoView.x;
	rcUserInfoView.right=rcUserInfoView.left+m_SizeUserInfoView.cx;
	rcUserInfoView.bottom=rcUserInfoView.top+m_SizeUserInfoView.cy;

	//更新界面
	RefreshGameView(&rcUserInfoView);

	return true;
}

//用户状态
bool CGameClientView::SetUserStatusInfo(WORD wChairID, const tagStatusInfo & UserStatusInfo)
{
	//设置变量
	m_UserStatusInfo[wChairID]=UserStatusInfo;

	//构造位置
	CRect rcUserInfoView;
	rcUserInfoView.top=m_ptUserInfoView.y;
	rcUserInfoView.left=m_ptUserInfoView.x;
	rcUserInfoView.right=rcUserInfoView.left+m_SizeUserInfoView.cx;
	rcUserInfoView.bottom=rcUserInfoView.top+m_SizeUserInfoView.cy;

	//更新界面
	RefreshGameView(&rcUserInfoView);

	return true;
}

//选择棋子
bool CGameClientView::SelectChessItem(BYTE cbXPos, BYTE cbYPos)
{
	//设置棋盘
	m_ChessBorad.SelectChessItem(cbXPos,cbYPos);

	//构造位置
	CPoint BoardPoint;
	m_ChessBorad.GetBenchmarkPos(BoardPoint);

	//构造位置
	CRect rcBorad;
	rcBorad.SetRect(BoardPoint.x,BoardPoint.y,BoardPoint.x+m_SizeChessBorad.cx,BoardPoint.y+m_SizeChessBorad.cy);

	//更新界面
	RefreshGameView(&rcBorad);

	return true;
}

//走棋路线
bool CGameClientView::SetMoveChessTrace(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal)
{
	//设置棋盘
	m_ChessBorad.SetMoveChessTrace(cbXPosStart,cbYPosStart,cbXPosTerminal,cbYPosTerminal);

	//构造位置
	CPoint BoardPoint;
	m_ChessBorad.GetBenchmarkPos(BoardPoint);

	//构造位置
	CRect rcBorad;
	rcBorad.SetRect(BoardPoint.x,BoardPoint.y,BoardPoint.x+m_SizeChessBorad.cx,BoardPoint.y+m_SizeChessBorad.cy);

	//更新界面
	RefreshGameView(&rcBorad);

	return true;
}

//用户时钟
VOID CGameClientView::DrawUserClock(CDC * pDC, INT nXPos, INT nYPos, WORD wClockCount)
{
	//加载资源
	//CImageHandle HandleClockNumber(&m_ImageClockNumber);
	INT nImageWidth=m_ImageClockNumber.GetWidth()/11;
	INT nImageHeight=m_ImageClockNumber.GetHeight();

	//绘画时针
	WORD wHour=wClockCount/3600L;
	m_ImageClockNumber.TransDrawImage(pDC,nXPos,nYPos,nImageWidth,nImageHeight,wHour/10*nImageWidth,0,RGB(255,0,255));
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth,nYPos,nImageWidth,nImageHeight,wHour%10*nImageWidth,0,RGB(255,0,255));

	//绘画冒号
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*2,nYPos,nImageWidth,nImageHeight,10*nImageWidth,0,RGB(255,0,255));

	//绘画分针
	WORD wMinute=(wClockCount-wHour*3600)/60;
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*3,nYPos,nImageWidth,nImageHeight,wMinute/10*nImageWidth,0,RGB(255,0,255));
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*4,nYPos,nImageWidth,nImageHeight,wMinute%10*nImageWidth,0,RGB(255,0,255));

	//绘画冒号
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*5,nYPos,nImageWidth,nImageHeight,10*nImageWidth,0,RGB(255,0,255));

	//绘画秒针
	WORD wSecond=wClockCount%60L;
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*6,nYPos,nImageWidth,nImageHeight,wSecond/10*nImageWidth,0,RGB(255,0,255));
	m_ImageClockNumber.TransDrawImage(pDC,nXPos+nImageWidth*7,nYPos,nImageWidth,nImageHeight,wSecond%10*nImageWidth,0,RGB(255,0,255));

	return;
}

//用户等级
TCHAR *CGameClientView::GetLevelDescribe(LONGLONG lUserScore,TCHAR tcUserInfo[32])
{
	WORD wSize = sizeof(tcUserInfo)*LEN_ACCOUNTS;
	if(lUserScore<30) _sntprintf(tcUserInfo,wSize,TEXT("三级棋童"));
	else if(lUserScore<80) _sntprintf(tcUserInfo,wSize,TEXT("二级棋童"));
	else if(lUserScore<160) _sntprintf(tcUserInfo,wSize,TEXT("一级棋童"));
	else if(lUserScore<280) _sntprintf(tcUserInfo,wSize,TEXT("九级棋士"));
	else if(lUserScore<450) _sntprintf(tcUserInfo,wSize,TEXT("八级棋士"));
	else if(lUserScore<680) _sntprintf(tcUserInfo,wSize,TEXT("七级棋士"));
	else if(lUserScore<980) _sntprintf(tcUserInfo,wSize,TEXT("六级棋士"));
	else if(lUserScore<1360) _sntprintf(tcUserInfo,wSize,TEXT("五级棋士"));
	else if(lUserScore<1830) _sntprintf(tcUserInfo,wSize,TEXT("四级棋士"));
	else if(lUserScore<2400) _sntprintf(tcUserInfo,wSize,TEXT("三级棋士"));
	else if(lUserScore<3880) _sntprintf(tcUserInfo,wSize,TEXT("二级棋士"));
	else if(lUserScore<4810) _sntprintf(tcUserInfo,wSize,TEXT("一级棋士"));
	else if(lUserScore<5880) _sntprintf(tcUserInfo,wSize,TEXT("三级棋手"));
	else if(lUserScore<7100) _sntprintf(tcUserInfo,wSize,TEXT("二级棋手"));
	else if(lUserScore<8480) _sntprintf(tcUserInfo,wSize,TEXT("一级棋手"));
	else if(lUserScore<10030) _sntprintf(tcUserInfo,wSize,TEXT("三级大师"));
	else if(lUserScore<11760) _sntprintf(tcUserInfo,wSize,TEXT("二级大师"));
	else if(lUserScore<13680) _sntprintf(tcUserInfo,wSize,TEXT("一级大师"));
	else if(lUserScore<15800) _sntprintf(tcUserInfo,wSize,TEXT("国家大师"));
	else _sntprintf(tcUserInfo,wSize,TEXT("特级大师"));

	//LevelItem1=三级棋童
	//LevelItem2=二级棋童,30
	//LevelItem3=一级棋童, 80
	//LevelItem4=九级棋士, 160
	//LevelItem5=八级棋士, 280
	//LevelItem6=七级棋士, 450
	//LevelItem7=六级棋士, 680
	//LevelItem8=五级棋士, 980
	//LevelItem9=四级棋士, 1360
	//LevelItem10=三级棋士,1830
	//LevelItem11=二级棋士,2400
	//LevelItem12=一级棋士,3880
	//LevelItem13=三级棋手,4810
	//LevelItem14=二级棋手,5880
	//LevelItem15=一级棋手,7100
	//LevelItem16=三级大师,8480
	//LevelItem17=二级大师,10030
	//LevelItem18=一级大师,11760
	//LevelItem19=国家大师,13680
	//LevelItem20=特级大师,15800

	return tcUserInfo;
}

//开始按钮
VOID CGameClientView::OnBnClickedStart()
{
	//发送消息
	SendEngineMessage(IDM_START,0,0);

	return;
}

//求和按钮
VOID CGameClientView::OnBnClickedPeace()
{
	//发送消息
	SendEngineMessage(IDM_PEACE,0,0);

	return;
}

//悔棋按钮
VOID CGameClientView::OnBnClickedRegret()
{
	//发送消息
	SendEngineMessage(IDM_REGRET,0,0);

	return;
}

//认输按钮
VOID CGameClientView::OnBnClickedGiveUp()
{
	//发送消息
	SendEngineMessage(IDM_GIVEUP,0,0);

	return;
}

//打谱按钮
VOID CGameClientView::OnBnClickedManual()
{
	//发送消息
	SendEngineMessage(IDM_MANUAL,0,0);

	return;
}

//保存按钮
VOID CGameClientView::OnBnClickedPreserve()
{
	//发送消息
	SendEngineMessage(IDM_PRESERVE,0,0);

	return;
}

//棋谱按钮
VOID CGameClientView::OnBnClickedHead()
{
	//发送消息
	WORD wStepCount=0;
	SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);

	return;
}

//棋谱按钮
VOID CGameClientView::OnBnClickedLast()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//发送消息
	if (nCurrentSel!=LB_ERR)
	{
		WORD wStepCount=nCurrentSel;
		SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);
	}

	return;
}

//棋谱按钮
VOID CGameClientView::OnBnClickedOpen()
{
	//提示信息
	//CInformation Information(this);
	//Information.ShowMessageBox(TEXT("提示"),TEXT("这里是打开“多功能棋谱管理器”的按钮，我们将在未来的升级版本中提供此功能！"));

	return;
}

//棋谱按钮
VOID CGameClientView::OnBnClickedNext()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//发送消息
	WORD wStepCount=(nCurrentSel==LB_ERR)?1:(nCurrentSel+2);
	SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);

	return;
}

//棋谱按钮
VOID CGameClientView::OnBnClickedTail()
{
	//发送消息
	WORD wStepCount=m_ManualList.GetCount();
	SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);

	return;
}

//选择改变
VOID CGameClientView::OnLbnSelchangeManualList()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//发送消息
	WORD wStepCount=nCurrentSel+1;
	SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);

	return;
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	AfxSetResourceHandle(AfxGetInstanceHandle());

	//框架资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//加载资源
	m_ImageViewFill.LoadFromResource(this,hResInstance,IDB_VIEW_FILL);
	m_ImageClockNumber.LoadFromResource(this,hResInstance,IDB_CLOCK_NUMBER);
	m_ImageControlBack.LoadFromResource(this,hResInstance,IDB_CONTROL_BACK);
	m_ImageUserInfoFlag.LoadFromResource(this,hResInstance,IDB_USER_INFO_FLAG);
	m_ImageUserInfoView.LoadFromResource(this,hResInstance,IDB_USER_INFO_VIEW);
	m_ImageChessManualT.LoadFromResource(this,hResInstance,IDB_CHESS_MANUAL_T);
	m_ImageChessManualM.LoadFromResource(this,hResInstance,IDB_CHESS_MANUAL_M);
	m_ImageChessManualB.LoadFromResource(this,hResInstance,IDB_CHESS_MANUAL_B);

	//获取大小
	m_nManualWidth=m_ImageChessManualM.GetWidth();
	m_ChessBorad.GetControlSize(m_SizeChessBorad);
	m_SizeControlView.SetSize(m_ImageControlBack.GetWidth(),m_ImageControlBack.GetHeight());
	m_SizeUserInfoView.SetSize(m_ImageUserInfoView.GetWidth(),m_ImageUserInfoView.GetHeight());

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_ScoreControl.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rcCreate,this,100);
	m_ManualList.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY,rcCreate,this,IDC_MANUAL_LIST);

	//设置控件
	m_UserRequestPeace.m_cbRequestType=REQUEST_PEACE;
	m_UserRequestRegret.m_cbRequestType=REQUEST_REGRET;
	
	
	CSkinResourceManager * pSkinResourceManager = (CSkinResourceManager *)CSkinResourceManager::GetInstance();
	if(pSkinResourceManager!=NULL)
		m_ManualList.SetFont((CFont *)(&pSkinResourceManager->GetDefaultFont()));

	//控制按钮
	m_btManual.Create(NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,IDC_MANUAL);
	m_btStart.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_CLIPSIBLINGS,rcCreate,this,IDC_START);
	m_btPeace.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_CLIPSIBLINGS,rcCreate,this,IDC_PEACE);
	m_btRegret.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_CLIPSIBLINGS,rcCreate,this,IDC_REGRET);
	m_btGiveUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_CLIPSIBLINGS,rcCreate,this,IDC_GIVEUP);
	m_btPreserve.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_CLIPSIBLINGS,rcCreate,this,IDC_PRESERVE);

	//棋谱按钮
	m_btManualOpen.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MANUAL_BT_OPEN);
	m_btManualHead.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_MANUAL_BT_HEAD);
	m_btManualLast.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_MANUAL_BT_LAST);
	m_btManualNext.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_MANUAL_BT_NEXT);
	m_btManualTail.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_MANUAL_BT_TAIL);

	//控制按钮
	//HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_START,hResInstance,false,false);
	m_btPeace.SetButtonImage(IDB_BT_PEACE,hResInstance,false,false);
	m_btManual.SetButtonImage(IDB_BT_MANUAL,hResInstance,false,false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVEUP,hResInstance,false,false);
	m_btRegret.SetButtonImage(IDB_BT_REGRET,hResInstance,false,false);
	m_btPreserve.SetButtonImage(IDB_BT_PRESERVE,hResInstance,false,false);

	//棋谱按钮
	m_btManualHead.SetButtonImage(IDB_MANUAL_BT_HEAD,hResInstance,false,false);
	m_btManualNext.SetButtonImage(IDB_MANUAL_BT_NEXT,hResInstance,false,false);
	m_btManualTail.SetButtonImage(IDB_MANUAL_BT_TAIL,hResInstance,false,false);
	m_btManualOpen.SetButtonImage(IDB_MANUAL_BT_OPEN,hResInstance,false,false);
	m_btManualLast.SetButtonImage(IDB_MANUAL_BT_LAST,hResInstance,false,false);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);

	//控制提示
	m_ToolTipCtrl.AddTool(&m_btStart,TEXT("开始"));
	m_ToolTipCtrl.AddTool(&m_btPeace,TEXT("求和"));
	m_ToolTipCtrl.AddTool(&m_btManual,TEXT("棋谱"));
	m_ToolTipCtrl.AddTool(&m_btGiveUp,TEXT("认输"));
	m_ToolTipCtrl.AddTool(&m_btRegret,TEXT("悔棋"));
	m_ToolTipCtrl.AddTool(&m_btPreserve,TEXT("保存"));

	//棋谱提示
	m_ToolTipCtrl.AddTool(&m_btManualHead,TEXT("最前"));
	m_ToolTipCtrl.AddTool(&m_btManualLast,TEXT("前一步"));
	m_ToolTipCtrl.AddTool(&m_btManualOpen,TEXT("打开棋谱"));
	m_ToolTipCtrl.AddTool(&m_btManualNext,TEXT("下一步"));
	m_ToolTipCtrl.AddTool(&m_btManualTail,TEXT("最后"));

	//创建窗口
	//CDlgUserRequest * pDlgUserHint=&m_UserRequestRegret;
	//if (pDlgUserHint->m_hWnd==NULL) pDlgUserHint->Create(IDD_USER_REQUEST,this);

	////显示窗口
	//pDlgUserHint->ShowWindow(SW_SHOW);
	//pDlgUserHint->SetForegroundWindow();

	////创建窗口
	//CDlgHint * pDlgUserHint=&m_UserHint;
	//if (pDlgUserHint->m_hWnd==NULL) pDlgUserHint->Create(IDD_USER_HINT,this);

	//////显示窗口
	//pDlgUserHint->ShowWindow(SW_SHOW);
	//pDlgUserHint->SetForegroundWindow();

	//m_ScoreControl.ShowWindow(5);
	return 0;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//点击处理
	m_ChessBorad.OnEventLMouseDown(Point);

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//发送消息
	WORD wStepCount=m_ManualList.GetCount();
	SendEngineMessage(IDM_ORIENTATION_MANUAL,wStepCount,0);

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
void CGameClientView::RefreshGameView(CRect rect)
{
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}

//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//switch (LOWORD(wParam))
	{

	}

	return __super::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
