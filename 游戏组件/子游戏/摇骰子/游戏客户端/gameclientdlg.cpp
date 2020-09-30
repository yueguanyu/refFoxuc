#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
        ON_WM_TIMER()
		ON_MESSAGE(IDM_START,OnStart)
		ON_MESSAGE(IDM_SETCHIP,OnSetChip)
		ON_MESSAGE(IDM_THROW_DICE, OnThrowDice)
		ON_MESSAGE(IDM_LOOK_DICE, OnLookDice)
		ON_MESSAGE(IDM_SCROLL, OnScroll)
		ON_MESSAGE(IDM_DICE_NUM, OnDiceNum)
		ON_MESSAGE(IDM_YELL_DICE, OnYellOk)
		ON_MESSAGE(IDM_OPEN_DICE, OnOpenDice)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
const int				g_nGameTime = 90;
bool					g_bNewStart = true;	
#endif
/////////////////////////////////////////////////////////////////////////////////
//宏定义

//结束原因
#define GER_NORMAL						0x00							//常规结束
#define GER_DISMISS						0x01							//游戏解散
#define GER_USER_LEFT					0x02							//用户离开

//游戏定时器
#define IDI_START_GAME					200								//开始定时器
#define IDI_SET_CHIP					201								//下注定时器
#define IDI_THROW_DICE					202								//摇骰定时器
#define IDI_YELL_DICE					203								//喊话定时器
#define IDI_LOOK_DICE					204								//看骰定时器

#define IDI_THROW_ANIMAL_0				205								//摇骰动画定时器
#define IDI_THROW_ANIMAL_1				206								//摇骰动画定时器
#define IDI_OPEN_ANIMAL					207								//开骰场景定时器

#define IDT_START_TIME					30								//开始时间
#define IDT_THROW_SPACE					50								//摇骰时间间隔
////////////////////////////////////////////////////////////////////////////////////
//构造函数
CGameClientEngine::CGameClientEngine() 
{
	//游戏变量
	m_bSetChipTime = 30;
	m_bThrowDiceTime = 5; 
	m_bYellDiceTime = 30;
	m_bOpenInterval = 2;

	m_wTurnCount = 0;
	m_lChip = 0;
	m_lMaxChip = 0;
	m_nTimer = IDI_LOOK_DICE;
	m_wCurUser = INVALID_CHAIR;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//设置标题
	SetWindowText(TEXT("摇骰子--  Ver：6.0.1.0"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_wTurnCount = 0;
	m_lChip = 0;
	m_lMaxChip = 0;
	m_wCurUser = INVALID_CHAIR;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));
	
	m_GameClientView.m_bDiceCount = 0;
	m_GameClientView.m_bNowDicePoint = 0;
	m_GameClientView.m_lChip = 0;
	ZeroMemory(m_GameClientView.m_bShowDiceCup, sizeof(m_GameClientView.m_bShowDiceCup));
	ZeroMemory(m_GameClientView.m_nAnimalTime, sizeof(m_GameClientView.m_nAnimalTime));
	ZeroMemory(&m_GameClientView.m_MaxYell, sizeof(tagDiceYell));
	ZeroMemory(m_GameClientView.m_bShowDiceCup, sizeof(m_GameClientView.m_bShowDiceCup));
	m_GameClientView.m_SceneOpenDice.Init();
	BYTE bHandDice[5] = {0};
	m_GameClientView.SetHandDice(0, false, bHandDice);
	m_GameClientView.SetHandDice(1, false, bHandDice);

	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{	
	////构造数据
	//if(false==IsLookonMode())
	//{
	//	CGameOption GameOption;
	//	GameOption.m_bEnableSound=IsEnableSound();

	//	//配置数据
	//	if (GameOption.DoModal()==IDOK)
	//	{
	//		//设置变量
	//		EnableSound(GameOption.m_bEnableSound);
	//	}
	//}

	return;
}
//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_SET_CHIP:			//开始下注
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false) && (wChairID==GetMeChairID())) 
					OnSetChip((WPARAM)1, 0);

				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_THROW_DICE:		//开始摇骰
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false) && (wChairID==GetMeChairID())) 
					OnThrowDice(0, 0);

				return false;
			}
			return true;
		}
	case IDI_YELL_DICE:			//开始喊话
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false) && (wChairID==GetMeChairID())) 
				{
					if (m_GameClientView.m_MaxYell.bDicePoint == 0 || m_GameClientView.m_MaxYell.bDiceCount == 0)
					{
						SetYellPanel(2);
						OnDiceNum(1, 0);
						OnYellOk(0, 0);
					}
					else
						OnOpenDice(0, 0);
				}
	
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{	
	switch (wSubCmdID)
	{
	case SUB_S_START_CHIP:
		{
			return OnSubStartChip(pData, wDataSize);
		}
	case SUB_S_CHIP_RESULT:
		{
			return OnSubChipResult(pData, wDataSize);
		}
	case SUB_S_SEND_DICE:
		{
			return OnSubSendDice(pData, wDataSize);
		}
	case SUB_S_THROW_DICE:
		{
			return OnSubThrowDice(pData, wDataSize);
		}
	case SUB_S_YELL_DICE:
		{
			return OnSubYellDice(pData, wDataSize);
		}
	case SUB_S_YELL_RESULT:
		{
			return OnSubYellResult(pData, wDataSize);
		}
	case SUB_S_OPEN_DICE:
		{
			return OnSubOpenDice(pData, wDataSize);
		}
	case SUB_S_GAME_END:
		{
			return OnSubGameEnd(pData, wDataSize);
		}
	}
	
	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			
			//设置控件
			if ( IsLookonMode() == false )
			{		

				if(GetMeUserItem()->GetUserStatus()!=US_READY)
				{
					m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
					m_GameClientView.m_btStart.EnableWindow(TRUE);
					m_GameClientView.m_btStart.SetFocus();
					SetGameClock(GetMeChairID(),IDI_START_GAME,IDT_START_TIME);
				}
				
			}

			return true;
		}
	case GS_WK_SETCHIP:		//下注状态
		{
			//验证数据
			ASSERT(sizeof(CMD_S_StatusSetChip) == wDataSize);
			if (sizeof( CMD_S_StatusSetChip) != wDataSize ) return false;

			//界面设置
			m_GameClientView.m_bShowDiceCup[0] = true;
			m_GameClientView.m_bShowDiceCup[1] = true;

			CMD_S_StatusSetChip  *pStatusSetChip = (CMD_S_StatusSetChip *) pData;
			
			//模拟消息包
			CMD_S_Chip_Start chipStart = {0};
			chipStart.wUserID = pStatusSetChip->wChipUser;
			chipStart.lMaxChip = pStatusSetChip->lMaxChip;

			OnSubStartChip((void*)&chipStart, sizeof(chipStart));


			IClientUserItem* pUserData=GetTableUserItem(GetMeChairID());

			
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
			

			return true;
		}
	case GS_WK_THROWDICE:	//摇骰状态
		{
			//验证数据
			ASSERT(sizeof(CMD_S_StatusThrow) == wDataSize);
			if (sizeof(CMD_S_StatusThrow) != wDataSize) return false;

	
			CMD_S_StatusThrow  *pStatusThrowDice = (CMD_S_StatusThrow *) pData;

			//设置变量
			m_lChip = pStatusThrowDice->lChip;
			m_GameClientView.m_lChip = pStatusThrowDice->lChip;

			//界面设置
			m_GameClientView.m_bShowDiceCup[0] = true;
			m_GameClientView.m_bShowDiceCup[1] = true;
				
			//模拟消息包
			if (!pStatusThrowDice->bHaveThrow[GetMeChairID()])
			{
				CMD_S_SendDice sendDice = {0};
				sendDice.wCurUser = GetMeChairID();
				memcpy(sendDice.bDiceData, pStatusThrowDice->bDiceData, sizeof(pStatusThrowDice->bDiceData));

				OnSubSendDice((void*)&sendDice, sizeof(sendDice));
			}
			else
			{
				WORD wMyViewChairID = SwitchViewChairID(GetMeChairID());
				memcpy(m_bDiceData[GetMeChairID()], pStatusThrowDice->bDiceData, sizeof(pStatusThrowDice->bDiceData));
				m_GameClientView.SetHandDice(wMyViewChairID, false, pStatusThrowDice->bDiceData);

				if (!IsLookonMode())
				{
					m_GameClientView.m_btLookDice.EnableWindow(TRUE);
					m_GameClientView.m_btLookDice.ShowWindow(SW_SHOW);
				}
			}

			IClientUserItem* pUserData=GetTableUserItem(GetMeChairID());

			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//验证数据
			ASSERT(sizeof(CMD_S_StatusPlay) == wDataSize);
			if (sizeof(CMD_S_StatusPlay) != wDataSize) return false;

			
			CMD_S_StatusPlay  *pStatusPlay = (CMD_S_StatusPlay *) pData;

			//设置变量
			WORD wMyChairID = SwitchViewChairID(GetMeChairID());
			m_lChip = pStatusPlay->lChip;
			m_GameClientView.m_lChip = pStatusPlay->lChip;
			memcpy(m_bDiceData[GetMeChairID()], pStatusPlay->bDiceData, sizeof(pStatusPlay->bDiceData));
			memcpy(m_UserYell, pStatusPlay->userYell, sizeof(m_UserYell));
			m_GameClientView.SetHandDice(wMyChairID, false, pStatusPlay->bDiceData);

			//界面设置
			m_GameClientView.m_bShowDiceCup[0] = true;
			m_GameClientView.m_bShowDiceCup[1] = true;
				
			//模拟消息包
			CMD_S_YellDice_Result yellResult = {0};
			yellResult.wYellUser = (pStatusPlay->wCurUser+1)%2;
			memcpy(&yellResult.curYell, &pStatusPlay->userYell[yellResult.wYellUser], sizeof(yellResult.curYell));

			OnSubYellResult((void*)&yellResult, sizeof(yellResult));

			CMD_S_YellDice yellDice = {0};
			yellDice.wCurUser = pStatusPlay->wCurUser;
			yellDice.wGameTurn = pStatusPlay->wGameTurn;
			memcpy(&yellDice.maxYell, &pStatusPlay->userYell[(pStatusPlay->wCurUser+1)%2], sizeof(yellDice.maxYell));

			OnSubYellDice((void*)&yellDice, sizeof(yellDice));

			IClientUserItem* pUserData=GetTableUserItem(GetMeChairID());

			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			return true;
		}
	case GS_WK_OPENDICE:	//开骰状态
		{
			//验证数据
			ASSERT(sizeof(CMD_S_StatusOpen) == wDataSize);
			if (sizeof(CMD_S_StatusOpen) != wDataSize) return false;

			

			CMD_S_StatusOpen  *pStatusOpen = (CMD_S_StatusOpen *) pData;

			//设置变量
			WORD wMyChairID = SwitchViewChairID(GetMeChairID());
			m_lChip = pStatusOpen->lChip;
			m_GameClientView.m_lChip = pStatusOpen->lChip;
			memcpy(&m_GameClientView.m_MaxYell, &pStatusOpen->curYell, sizeof(pStatusOpen->curYell));

			//界面设置
			m_GameClientView.m_bShowDiceCup[0] = true;
			m_GameClientView.m_bShowDiceCup[1] = true;

			//模拟消息包
			CMD_S_OpenDice openDice = {0};
			openDice.bDicePoint = pStatusOpen->curYell.bDicePoint;
			openDice.bOpenRight = pStatusOpen->bOpenRight;
			openDice.nDiceOpenCount = pStatusOpen->curYell.bDiceCount;
			openDice.nDiceRealCount = pStatusOpen->nDiceRealCount;
			openDice.wOpenUser = pStatusOpen->wOpenUser;
			memcpy(openDice.bDiceData, pStatusOpen->bDiceData, sizeof(pStatusOpen->bDiceData));
			
			OnSubOpenDice((void *)&openDice, sizeof(openDice));


			IClientUserItem* pUserData=GetTableUserItem(GetMeChairID());

			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			return true;
		}
	}	

	return true;
}

//定时器
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == IDI_LOOK_DICE)												//看骰计时
	{
		KillTimer(IDI_LOOK_DICE);
		WORD wMyChair = SwitchViewChairID(GetMeChairID());
		m_GameClientView.SetHandDice(wMyChair, false, NULL);
		
		//播放声音
		PlayGameSound(AfxGetInstanceHandle(), TEXT("OUT_CARD"));
	}
	else if (nIDEvent == IDI_THROW_ANIMAL_1 || nIDEvent == IDI_THROW_ANIMAL_0)	//摇骰动画
	{
		//计算玩家
		WORD wChairID = SwitchViewChairID(nIDEvent - IDI_THROW_ANIMAL_0);
		int nTime = m_GameClientView.m_nAnimalTime[wChairID];
		//MyDebug(_T("%d %d %d"), wChairID, nTime, GetTickCount());
		if (nTime <= 1 || nTime >= 30+24*2)
		{
			KillTimer(nIDEvent);
			m_GameClientView.OnThrowAnimal(wChairID, enTAEnd);
			if (!IsLookonMode())
			{
				if (GetMeChairID() == nIDEvent - IDI_THROW_ANIMAL_0)
				{
					m_GameClientView.m_btLookDice.EnableWindow(TRUE);
					m_GameClientView.m_btLookDice.ShowWindow(SW_SHOW);
				}
				//发送消息
				if (nIDEvent - IDI_THROW_ANIMAL_0 == GetMeChairID() && nTime == 1)
					SendSocketData(SUB_C_THROWFINISH);
			}
		}
		else
		{
			if (nTime % 2 == 1)
				m_GameClientView.OnThrowAnimal(wChairID, enTAPlay);

			//播放声音
			if (nTime == 73 || nTime == 53 || nTime == 23)
				PlayGameSound(AfxGetInstanceHandle(), TEXT("DRAW_SICE"));
		}
	}
	else if (nIDEvent == IDI_OPEN_ANIMAL)
	{
		//定义变量
		int nTime = m_GameClientView.m_SceneOpenDice.bODTime;
		//MyDebug(_T("Animal Open %d"), nTime);
		if (nTime > 1 && nTime <= 55)		//进行中
		{
			m_GameClientView.m_SceneOpenDice.bODTime--;

			if (nTime == 41)
			{
				m_GameClientView.SetHandDice(0, true, NULL);
				m_GameClientView.SetHandDice(1, true, NULL);
			}

			if ( (nTime > 40 && nTime <= 55) || (nTime >= 2 && nTime <= 21) )
				m_GameClientView.RefreshGameView();

			//播放声音
			if (nTime == 2 || nTime == 21 || nTime == 41)
				PlayGameSound(AfxGetInstanceHandle(), TEXT("OUT_CARD"));
		}
		else if (nTime == 1)				//结束
		{
			KillTimer(nIDEvent);

			if (!IsLookonMode())
				SendSocketData(SUB_C_COMPLETEOPEN);
		}
		else								//异常
		{
			KillTimer(nIDEvent);

			m_GameClientView.m_SceneOpenDice.Init();
			//MyMsgBox(_T("IDI_OPEN_ANIMAL Error!"));
		}
	}

	__super::OnTimer(nIDEvent) ;
}

//开始下注
bool CGameClientEngine::OnSubStartChip(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_Chip_Start));
	if (wDataSize != sizeof(CMD_S_Chip_Start)) return false;

	CMD_S_Chip_Start *pChipStart = (CMD_S_Chip_Start*)pBuffer;

	SetGameStatus(GS_WK_SETCHIP);

	//下注时间
	SetGameClock(pChipStart->wUserID, IDI_SET_CHIP, m_bSetChipTime);

	//设置变量
	m_lMaxChip = pChipStart->lMaxChip;

	//设置按钮
	if (GetMeChairID() == pChipStart->wUserID && !IsLookonMode())
	{
		for (int i = 0; i < 4; i++)
		{
			CString strChip;
			strChip.Format(TEXT("%I64d"), (LONGLONG)(m_lMaxChip/4*(i+1)));
			m_GameClientView.m_btSetChip[i].SetWindowText(strChip);
		}
		m_GameClientView.UpdateButton(enUBChip);
	}

	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//下注结果
bool CGameClientEngine::OnSubChipResult(const void * pBuffer, WORD wDataSize)
{
	//针对旁观
	if (IsLookonMode())
	{
		m_wTurnCount = 0;
		m_lChip = 0;
		m_lMaxChip = 0;
		m_wCurUser = INVALID_CHAIR;
		ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
		ZeroMemory(m_UserYell, sizeof(m_UserYell));
		
		m_GameClientView.m_bDiceCount = 0;
		m_GameClientView.m_bNowDicePoint = 0;
		m_GameClientView.m_lChip = 0;
		ZeroMemory(m_GameClientView.m_bShowDiceCup, sizeof(m_GameClientView.m_bShowDiceCup));
		ZeroMemory(m_GameClientView.m_nAnimalTime, sizeof(m_GameClientView.m_nAnimalTime));
		ZeroMemory(&m_GameClientView.m_MaxYell, sizeof(tagDiceYell));
		m_GameClientView.m_SceneOpenDice.Init();
		BYTE bHandDice[5] = {0};
		m_GameClientView.SetHandDice(0, false, bHandDice);
		m_GameClientView.SetHandDice(1, false, bHandDice);

		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.UpdateButton(enUBClickBegin);
	}

	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_Chip_Result));
	if (wDataSize != sizeof(CMD_S_Chip_Result)) return false;

	CMD_S_Chip_Result *pChipStart = (CMD_S_Chip_Result*)pBuffer;

	//设置变量
	m_lChip = pChipStart->lChip;
	m_GameClientView.m_lChip = pChipStart->lChip;

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(), _T("OUT_CARD"));

	//MyDebug(_T("%d"), m_lChip);

	return true;
}

//骰子数值
bool CGameClientEngine::OnSubSendDice(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_SendDice));
	if (wDataSize != sizeof(CMD_S_SendDice)) return false;

	CMD_S_SendDice *pSendDice = (CMD_S_SendDice*)pBuffer;

	SetGameStatus(GS_WK_THROWDICE);

	//摇骰时间
	SetGameClock(GetMeChairID(), IDI_THROW_DICE, m_bThrowDiceTime);

	//设置变量
	WORD wMyViewChairID = SwitchViewChairID(GetMeChairID());
	memcpy(m_bDiceData[GetMeChairID()], pSendDice->bDiceData, sizeof(pSendDice->bDiceData));
	m_GameClientView.SetHandDice(wMyViewChairID, false, pSendDice->bDiceData);
	m_GameClientView.m_bShowDiceCup[0] = true;	
	m_GameClientView.m_bShowDiceCup[1] = true;

	//设置按钮
	if (!IsLookonMode())
	{
		m_GameClientView.m_btThrowDice.EnableWindow(TRUE);
		m_GameClientView.m_btThrowDice.ShowWindow(SW_SHOW);
	}

	m_GameClientView.RefreshGameView();

	//MyDebug(_T("%d %d %d %d %d"), pSendDice->bDiceData[0], pSendDice->bDiceData[1], pSendDice->bDiceData[2], 
	//		pSendDice->bDiceData[3], pSendDice->bDiceData[4]);

	return true;
}

//玩家摇骰
bool CGameClientEngine::OnSubThrowDice(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_ThrowDice));
	if (wDataSize != sizeof(CMD_S_ThrowDice)) return false;

	CMD_S_ThrowDice *pThrowDice = (CMD_S_ThrowDice*)pBuffer;

	//自己退出
	if (pThrowDice->wThrowUser == GetMeChairID() && !IsLookonMode())	return true;

	WORD wChairID = SwitchViewChairID(pThrowDice->wThrowUser);

	//启动动画
	m_nTimer = SetTimer(IDI_THROW_ANIMAL_0+pThrowDice->wThrowUser, IDT_THROW_SPACE, NULL);
	m_GameClientView.OnThrowAnimal(wChairID, enTABegin);

	//MyDebug(_T("Throw(%d  %d)"), GetMeChairID(), pThrowDice->wThrowUser);

	return true;
}

//玩家喊话
bool CGameClientEngine::OnSubYellDice(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_YellDice));
	if (wDataSize != sizeof(CMD_S_YellDice)) return false;

	CMD_S_YellDice *pYellDice = (CMD_S_YellDice*)pBuffer;

	SetGameStatus(GS_WK_PLAYING);

	//喊话时间
	KillGameClock(IDI_YELL_DICE);
	SetGameClock(pYellDice->wCurUser, IDI_YELL_DICE, m_bYellDiceTime);

	//设置变量
	m_wCurUser = pYellDice->wCurUser;
	m_wTurnCount = pYellDice->wGameTurn;

	//设置按钮
	if (GetMeChairID() == pYellDice->wCurUser && !IsLookonMode())
	{
		m_GameClientView.UpdateButton(pYellDice->wGameTurn==0?enUBYellFirst:enUBYell);
		SetYellPanel(pYellDice->wGameTurn==0?2:pYellDice->maxYell.bDiceCount);
	}

	m_GameClientView.RefreshGameView();

	//MyDebug(_T("Yell(%d  %d  turn=%d yell=[%d %d])"), GetMeChairID(), pYellDice->wCurUser, pYellDice->wGameTurn,
	//	pYellDice->maxYell.bDiceCount, pYellDice->maxYell.bDicePoint);

	return true;
}

//喊话结果
bool CGameClientEngine::OnSubYellResult(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_YellDice_Result));
	if (wDataSize != sizeof(CMD_S_YellDice_Result)) return false;

	CMD_S_YellDice_Result *pYellDiceResult = (CMD_S_YellDice_Result*)pBuffer;

	//设置变量
	memcpy(&m_UserYell[pYellDiceResult->wYellUser], &pYellDiceResult->curYell, sizeof(tagDiceYell));
	memcpy(&m_GameClientView.m_MaxYell, &pYellDiceResult->curYell, sizeof(tagDiceYell));

	//播放声音
	IClientUserItem * pClientUserItem=GetTableUserItem(pYellDiceResult->wYellUser);
	if (pClientUserItem == NULL)
		PlaySoundBySex(pYellDiceResult->curYell.bDiceCount, pYellDiceResult->curYell.bDicePoint);
	else
		PlaySoundBySex(pYellDiceResult->curYell.bDiceCount, pYellDiceResult->curYell.bDicePoint, (pClientUserItem->GetGender()==1));

	m_GameClientView.RefreshGameView();

	return true;
}

//玩家开骰
bool CGameClientEngine::OnSubOpenDice(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_OpenDice));
	if (wDataSize != sizeof(CMD_S_OpenDice)) return false;

	CMD_S_OpenDice *pYellDiceResult = (CMD_S_OpenDice*)pBuffer;

	SetGameStatus(GS_WK_OPENDICE);

	//设置变量
	m_GameClientView.m_SceneOpenDice.wOpenUser = SwitchViewChairID(pYellDiceResult->wOpenUser);
	m_GameClientView.m_SceneOpenDice.bRealDiceCount = pYellDiceResult->nDiceRealCount;
	m_GameClientView.m_SceneOpenDice.bDicePoint = pYellDiceResult->bDicePoint;
	m_GameClientView.m_SceneOpenDice.bOpenRight = pYellDiceResult->bOpenRight;
	m_GameClientView.m_SceneOpenDice.bODTime = 55;	
	//共3个场景 3 开盖动画(100ms*15) 2 显示实际共多少个(喊开消失2000ms) 1 显示是否开对(2000ms) 0 则不显示	bODTime单位100ms

	//启动计时
	KillGameClock(IDI_YELL_DICE);
	KillTimer(IDI_LOOK_DICE);
	m_nTimer = SetTimer(IDI_OPEN_ANIMAL, 100, NULL);

	//设置按钮
	if (!IsLookonMode())
	{
		m_GameClientView.UpdateButton(enUBOpen);
	}

	//设置界面
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		WORD wChairID = SwitchViewChairID(i);
		m_GameClientView.SetHandDice(wChairID, false, pYellDiceResult->bDiceData[i]);
		m_GameClientView.m_bShowDiceCup[i] = false;
	}
	m_GameClientView.RefreshGameView();

	//播放声音
	IClientUserItem * pClientUserItem=GetTableUserItem(pYellDiceResult->wOpenUser);
	if (pClientUserItem == NULL || pClientUserItem->GetGender() == 1)
		PlayGameSound(AfxGetInstanceHandle(), _T("GAME_OPEN_B"));
	else
		PlayGameSound(AfxGetInstanceHandle(), _T("GAME_OPEN_G"));

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;

	CMD_S_GameEnd *pGameEnd = (CMD_S_GameEnd*)pBuffer;

	//游戏状态
	SetGameStatus(GS_WK_FREE);

	//删除定时器
	KillGameClock(IDI_OPEN_ANIMAL);
	KillGameClock(IDI_SET_CHIP);
	KillGameClock(IDI_THROW_DICE);
	KillGameClock(IDI_YELL_DICE);
	KillGameClock(IDI_LOOK_DICE);
	KillTimer(IDI_THROW_ANIMAL_0);
	KillTimer(IDI_THROW_ANIMAL_1);
	KillTimer(IDI_OPEN_ANIMAL);

	//InsertShtickString( TEXT( "" ), RGB( 255, 0, 0 ) , true ) ;
	//InsertShtickString( TEXT( "" ), RGB( 255, 0, 0 ) , true ) ;
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString( TEXT( "本局得分：\n" )) ;

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if(NULL!=pClientUserItem)
		{
            m_GameClientView.m_ScoreView.SetGameScore(i,pClientUserItem->GetNickName(),pGameEnd->lGameScore[i]);
			CString strScoreRes ;
			strScoreRes.Format( TEXT( "%s：%I64d\n" ) , pClientUserItem->GetNickName(),pGameEnd->lGameScore[i] ) ;
			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertNormalString( strScoreRes) ;
		}
		else
			m_GameClientView.m_ScoreView.SetGameScore(i,TEXT(""),0);
	}
	CString strTax;
	strTax.Format(_T("服务费：%I64d"), pGameEnd->lGameTax);
	//InsertShtickString( strTax, RGB( 255, 0, 0 ) , true );

	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertNormalString( strTax) ;

	m_GameClientView.m_ScoreView.CenterWindow(&m_GameClientView) ;
    m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//播放声音
	if(pGameEnd->lGameScore[GetMeChairID()]>0)	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else										PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置按钮
		m_GameClientView.UpdateButton(enUBEnd);

		SetGameClock(GetMeChairID(),IDI_START_GAME,IDT_START_TIME);
	}
	m_GameClientView.m_bDiceCount = 0;
	m_GameClientView.m_bNowDicePoint = 0;

	return true;
}

//设置喊话
void CGameClientEngine::SetYellPanel(BYTE bDiceCount)
{
	//特殊数字
	if (bDiceCount == 255)
	{
		m_GameClientView.m_bDiceCount = 0;
		m_GameClientView.m_bNowDicePoint = 0;
		for (int i = 0; i < 6; i++)
		{
			m_GameClientView.m_btDiceNum[i].ShowWindow(SW_HIDE);
			m_GameClientView.m_btDiceNum[i].EnableWindow(FALSE);
		}
		m_GameClientView.RefreshGameView();

		return;
	}

	//数据验证
	if (bDiceCount < 2 || bDiceCount > 12 )	return;
	
	m_GameClientView.m_bDiceCount = bDiceCount;
	m_GameClientView.m_bNowDicePoint = 0;

	//可喊范围
	BYTE bMax = 0, bMin = 0;
	LRESULT ret = m_GameLogic.CalcDiceRange(&m_UserYell[(m_wCurUser+1)%2], bDiceCount, (m_wTurnCount==0), &bMin, &bMax);
	if (ret == 0)
	{
		for (int i = 0; i < 6; i++)
		{
			m_GameClientView.m_btDiceNum[i].ShowWindow(SW_SHOW);
			if (i >= bMin-1 && i <= bMax-1)
				m_GameClientView.m_btDiceNum[i].EnableWindow(TRUE);
			else
				m_GameClientView.m_btDiceNum[i].EnableWindow(FALSE);
		}
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			m_GameClientView.m_btDiceNum[i].ShowWindow(SW_SHOW);
			m_GameClientView.m_btDiceNum[i].EnableWindow(FALSE);
		}
	}

	m_GameClientView.RefreshGameView();
}

//播放声音
void CGameClientEngine::PlaySoundBySex(BYTE bDiceCount, BYTE bDicePoint, bool bBoy/* = true*/)
{
	//数据验证
	if (bDiceCount < 2 || bDiceCount > 12 || bDicePoint < 1 || bDicePoint > 6)
		return;

	//播放声音
	CString strSoundName;
	strSoundName.Format(_T("YELL_%d%d_%s"), bDiceCount, bDicePoint, bBoy?_T("B"):_T("G"));
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//设置界面
	KillGameClock(IDI_START_GAME);

	m_wTurnCount = 0;
	m_lChip = 0;
	m_lMaxChip = 0;
	m_wCurUser = INVALID_CHAIR;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));
	
	m_GameClientView.m_bDiceCount = 0;
	m_GameClientView.m_bNowDicePoint = 0;
	m_GameClientView.m_lChip = 0;
	ZeroMemory(m_GameClientView.m_bShowDiceCup, sizeof(m_GameClientView.m_bShowDiceCup));
	ZeroMemory(m_GameClientView.m_nAnimalTime, sizeof(m_GameClientView.m_nAnimalTime));
	ZeroMemory(&m_GameClientView.m_MaxYell, sizeof(tagDiceYell));
	m_GameClientView.m_SceneOpenDice.Init();
	BYTE bHandDice[5] = {0};
	m_GameClientView.SetHandDice(0, false, bHandDice);
	m_GameClientView.SetHandDice(1, false, bHandDice);

	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置按钮
	if (!IsLookonMode())
	{
		m_GameClientView.UpdateButton(enUBClickBegin);
	}

	//发送消息
	SendUserReady(NULL,0);

#ifdef _DEBUG
	g_bNewStart = true ;
#endif

	//SetTimer(IDI_THROW_ANIMAL_0, IDT_THROW_SPACE, NULL);
	//SetTimer(IDI_THROW_ANIMAL_1, IDT_THROW_SPACE, NULL);
	//m_GameClientView.OnThrowAnimal(SwitchViewChairID(0), enTABegin);
	//m_GameClientView.OnThrowAnimal(SwitchViewChairID(1), enTABegin);

	return 0;
}

//压注消息
LRESULT CGameClientEngine::OnSetChip(WPARAM wParam, LPARAM lParam)
{
	//无用参数
	UNREFERENCED_PARAMETER(lParam);

	KillGameClock(IDI_SET_CHIP);

	//发送参数
	CMD_C_Chip  SetChip = {0};
	SetChip.lChip = (int)wParam * m_lMaxChip / 4;
	SetChip.wUserID = GetMeChairID();

	SendSocketData(SUB_C_CHIP, &SetChip, sizeof(SetChip));

	//设置按钮
	for (int i = 0; i < 4; i++)
	{
		m_GameClientView.m_btSetChip[i].ShowWindow(SW_HIDE);
		m_GameClientView.m_btSetChip[i].EnableWindow(FALSE);
	}

	return true ;
}

//摇骰消息
LRESULT CGameClientEngine::OnThrowDice(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	WORD wMyChairID = SwitchViewChairID(GetMeChairID());

	//设置界面
	KillGameClock(IDI_THROW_DICE);	
	m_nTimer = SetTimer(IDI_THROW_ANIMAL_0+GetMeChairID(), IDT_THROW_SPACE, NULL);
	m_GameClientView.OnThrowAnimal(wMyChairID, enTABegin);

	//发送消息
	SendSocketData(SUB_C_THROWDICE);

	//设置按钮
	m_GameClientView.m_btThrowDice.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThrowDice.EnableWindow(FALSE);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(), _T("OUT_CARD"));

	return true;
}

//看骰消息
LRESULT CGameClientEngine::OnLookDice(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	WORD wMyChair = SwitchViewChairID(GetMeChairID());

	//设置界面
	KillTimer(IDI_LOOK_DICE);
	m_nTimer = SetTimer(IDI_LOOK_DICE, 5000, NULL);

	m_GameClientView.SetHandDice(wMyChair, true, NULL);

	return true;
}

//骰数消息
LRESULT CGameClientEngine::OnScroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	
	//定义变量
	BYTE bCount = m_GameClientView.m_bDiceCount;

	if ( (wParam == 0 && bCount == 12) || (wParam == 1 && bCount == 2) )	return true;

	m_GameClientView.m_bDiceCount += (wParam==0?1:-1);
	m_GameClientView.m_bNowDicePoint = 0;

	//设置界面
	SetYellPanel(m_GameClientView.m_bDiceCount);
	m_GameClientView.m_btYellOK.EnableWindow(FALSE);

	return true;
}

//骰点消息
LRESULT CGameClientEngine::OnDiceNum(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	//定义变量
	m_GameClientView.m_bNowDicePoint = (BYTE)wParam;

	//设置界面
	m_GameClientView.m_btYellOK.EnableWindow(TRUE);

	m_GameClientView.RefreshGameView();

	return true;
}	

//喊话消息
LRESULT CGameClientEngine::OnYellOk(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//关闭计时
	KillGameClock(IDI_YELL_DICE);

	//发送消息
	CMD_C_YellDice yellDice = {0};
	yellDice.wUserID = GetMeChairID();
	yellDice.curYell.bDiceCount = m_GameClientView.m_bDiceCount;
	yellDice.curYell.bDicePoint = m_GameClientView.m_bNowDicePoint;
	SendSocketData(SUB_C_YELLDICE, &yellDice, sizeof(CMD_C_YellDice));

	//设置界面
	SetYellPanel(255);
	m_GameClientView.m_btYellOK.ShowWindow(SW_HIDE);
	m_GameClientView.m_btYellOK.EnableWindow(FALSE);
	m_GameClientView.m_btOpenDice.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOpenDice.EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[0].EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[0].ShowWindow(SW_HIDE);
	m_GameClientView.m_btDiceScroll[1].EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[1].ShowWindow(SW_HIDE);

	return true;
}

//开骰消息
LRESULT CGameClientEngine::OnOpenDice(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//关闭计时
	KillGameClock(IDI_YELL_DICE);

	//发送消息
	SendSocketData(SUB_C_OPENDICE);

	//设置界面
	SetYellPanel(255);
	m_GameClientView.m_btYellOK.ShowWindow(SW_HIDE);
	m_GameClientView.m_btYellOK.EnableWindow(FALSE);
	m_GameClientView.m_btOpenDice.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOpenDice.EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[0].EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[0].ShowWindow(SW_HIDE);
	m_GameClientView.m_btDiceScroll[1].EnableWindow(FALSE);
	m_GameClientView.m_btDiceScroll[1].ShowWindow(SW_HIDE);

	return true;
}
