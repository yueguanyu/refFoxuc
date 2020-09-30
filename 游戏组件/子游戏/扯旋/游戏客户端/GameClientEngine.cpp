#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include ".\GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_NULLITY					200									//无效定时器
#define IDI_START_GAME				201									//开始定时器
#define IDI_USER_ADD_SCORE			202									//下注定时器

//定时器标识
#define IDI_TIME_USER_INVEST		0									//下本定时器
#define IDI_TIME_OPEN_CARD			1									//开牌定时器
#define IDI_DELAY_SHOW_BUTTON		2									//延时定时器
#define IDI_DELAY_EXIT_WIN			3									//延时定时器

//时间标识
#ifdef _DEBUG
#define TIME_USER_INVEST			99									//下本定时器
#define TIME_USER_START_GAME		99									//开始定时器
#define TIME_USER_ADD_SCORE			99									//加注定时器
#define TIME_USER_OPEN_CARD			99									//摊牌定时器

#else
#define TIME_USER_INVEST			30									//下本定时器
#define TIME_USER_START_GAME		30									//开始定时器
#define TIME_USER_ADD_SCORE			30									//加注定时器
#define TIME_USER_OPEN_CARD			30									//摊牌定时器

#endif

#define TIME_DELAY_SHOW_BUTTON		1/10								//延时定时器
#define TIME_DELAY_EXIT_WIN			1									//延时定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)

	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_CLEAR_SCORE,OnClearScore)
	ON_MESSAGE(IDM_OPEN_CARD,OnOpenCard)
	ON_MESSAGE(IDM_DECREASE,OnDecreaseJetton)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
	ON_MESSAGE(IDM_OPEN_CARD_FINISH,OnOpenCardFinish)
	ON_WM_TIMER()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine() 
{
	//游戏变量
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_wViewChairID[i]=INVALID_CHAIR;
	}		
	m_wBankerUser=INVALID_CHAIR;							
	m_wCurrentUser=INVALID_CHAIR;				
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserGetScore,sizeof(m_lUserGetScore));
	ZeroMemory(m_lUserTax,sizeof(m_lUserTax));

	//下注变量
	m_lCellScore=0;
	m_lTurnMinScore=0;
	m_lTurnMaxScore=0;	
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	ASSERT(m_pGlobalUnits!=NULL);

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
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_wViewChairID[i]=INVALID_CHAIR;
	}		

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserGetScore,sizeof(m_lUserGetScore));
	ZeroMemory(m_lUserTax,sizeof(m_lUserTax));

	//下注变量
	m_lCellScore=0;
	m_lTurnMinScore=0;
	m_lTurnMaxScore=0;	
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	//删除时间
	KillTimer( IDI_DELAY_SHOW_BUTTON	);
	KillTimer( IDI_DELAY_EXIT_WIN	);

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();

	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=m_pGlobalUnits->IsAllowGameSound();
	GameOption.m_bAllowLookon=m_pGlobalUnits->m_bAllowFriendLookon;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置配置
		//EnableSound(GameOption.m_bEnableSound);
		//AllowUserLookon(0,GameOption.m_bAllowLookon);

		m_pGlobalUnits->m_bAllowSound = GameOption.m_bEnableSound;
		m_pGlobalUnits->m_bAllowFriendLookon =GameOption.m_bAllowLookon ;
	}

	return;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//中止判断
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
				return false;
			}

			//警告通知
			if (nElapse<=5) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_NULLITY:			//过滤时器
		{
			//中止判断
			if (nElapse==0)
			{
				KillGameClock(nTimerID);
				return false;
			}

			//警告通知
			if (nElapse<=5) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_USER_ADD_SCORE:	//下注时间
		{
			//中止判断
			if (nElapse==0)
			{
				KillGameClock(nTimerID);

				if(!IsLookonMode() && m_wCurrentUser==GetMeChairID())
				{
					OnAddScore(IDC_GIVE_UP,0);
				}

				return false;
			}

			//警告通知
			if (nElapse<=5 && !IsLookonMode() && m_wCurrentUser==GetMeChairID()) 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//控件控制
	//if(bLookonUser)
	{
		if(IsAllowLookon() )
			m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
		else m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(false);
	}
	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			//消息处理
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_USER_INVEST:	//用户下本
		{
			//消息处理
			return OnSubUserInvest(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			//消息处理
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			//消息处理
			return OnSubUserGiveUp(pBuffer,wDataSize);
		}
	case SUB_S_ADD_SCORE:	//用户下注
		{
			//消息处理
			return OnSubAddScore(pBuffer,wDataSize);
		}
	case SUB_S_OPEN_START:	//开始分牌
		{
			//消息处理
			return OnSubStartOpen(pBuffer,wDataSize);
		}
	case SUB_S_OPEN_CARD:	//用户摊牌
		{
			//消息处理
			return OnSubOpenCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//消息处理
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_TK_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置控件
			if (!IsLookonMode())
			{
				//开始按钮
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_USER_START_GAME);

				//设置单元
				m_lCellScore = pStatusFree->lCellScore;
				m_GameClientView.SetCellScore(pStatusFree->lCellScore);
				m_GameClientView.m_JetonControl.SetCellJetton(pStatusFree->lCellScore);
			}

			return true;
		}
	case GS_TK_INVEST:	//下本状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusInvest)) return false;
			CMD_S_StatusInvest * pStatusInvest=(CMD_S_StatusInvest *)pBuffer;

			//设置变量
			m_wBankerUser=pStatusInvest->wBankerUser;
			m_lCellScore = pStatusInvest->lCellScore;
			CopyMemory(m_lUserScore,pStatusInvest->lUserScore,sizeof(m_lUserScore));
			CopyMemory(m_cbPlayStatus,pStatusInvest->cbPlayStatus,sizeof(m_cbPlayStatus));
			m_GameClientView.m_JetonControl.SetCellJetton(m_lCellScore);

			//设置界面
			m_GameClientView.SetWaitInvest(true);
			m_GameClientView.SetCellScore(m_lCellScore);
			m_GameClientView.SetBankerUser(GetViewChairID(m_wBankerUser));
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//操作背景
				if(!pStatusInvest->bInvestFinish[i])
				{
					m_GameClientView.SetUserOperate(GetViewChairID(i),true);
				}

				//用户筹码
				m_GameClientView.SetUserJetton(GetViewChairID(i),m_lUserScore[i]);

				//获取用户
				IClientUserItem* pUserData=GetTableUserItem(i);

				if (pUserData==NULL) continue;

				//用户名字
				lstrcpyn(m_szAccounts[i],pUserData->GetNickName(),CountArray(m_szAccounts[i]));
			}

			//设置控件
			WORD wMeChairID=GetMeChairID();
			if(!pStatusInvest->bInvestFinish[wMeChairID] && !IsLookonMode())
			{
				//用户控件
				UpdateInvestControl();

				//自动处理
				if(m_GameClientView.m_btClearScore.IsWindowVisible()==FALSE)
				{
					OnAddScore(IDC_INVEST,0);

					if(m_pIStringMessage!=NULL)
						m_pIStringMessage->InsertCustomString(TEXT("\n余簸低于单元数,不能再加."),RGB(255,0,0),RGB(255,255,255));

					//InsertGeneralString(TEXT("\n余簸低于单元数,不能再加."),RGB(255,0,0),true);
				}
				//设置定时
				else SetTimer(IDI_TIME_USER_INVEST,TIME_USER_INVEST*1000,NULL);
			}

			//辅助定时
			SetGameClock(wMeChairID,IDI_NULLITY,TIME_USER_INVEST);

			return true;
		}
	case GS_TK_SCORE:	//下注状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pBuffer;

			//设置变量
			m_wBankerUser=pStatusScore->wBankerUser;
			m_wCurrentUser=pStatusScore->wCurrentUser;
			m_lTurnMaxScore=pStatusScore->lTurnMaxScore;
			m_lTurnMinScore=pStatusScore->lTurnMinScore;	
			m_lCellScore=pStatusScore->lCellScore;
			CopyMemory(m_lUserScore,pStatusScore->lUserScore,sizeof(m_lUserScore));
			CopyMemory(m_lTotalScore,pStatusScore->lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(m_cbPlayStatus,pStatusScore->cbPlayStatus,sizeof(m_cbPlayStatus));
			m_GameClientView.m_JetonControl.SetCellJetton(m_lCellScore);

			//明牌数目
			BYTE cbSpeCount=2,cbMingCount=cbSpeCount;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				for(WORD j=0;j<cbSpeCount;j++)
				{
					if(pStatusScore->cbMingCard[i][j]!=0)cbMingCount++;
					else break;
				}
				break;
			}

			//手上扑克
			WORD wMeChairID=GetMeChairID();
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//用户扑克
				CopyMemory(&m_cbHandCardData[i][cbSpeCount],pStatusScore->cbMingCard[i],sizeof(BYTE)*cbMingCount);
			}
			CopyMemory(m_cbHandCardData[wMeChairID],pStatusScore->cbHandCard,sizeof(BYTE)*cbSpeCount);

			//设置界面
			m_GameClientView.SetCellScore(m_lCellScore);
			m_GameClientView.SetBankerUser(GetViewChairID(m_wBankerUser));
			m_GameClientView.SetUserOperate(GetViewChairID(m_wCurrentUser),true);
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//下注筹码
				m_GameClientView.SetUserTableScore(GetViewChairID(i),m_lTotalScore[i]);

				//用户筹码
				m_GameClientView.SetUserJetton(GetViewChairID(i),m_lUserScore[i]-m_lTotalScore[i]);

				//用户扑克
				m_GameClientView.m_CardControl[GetViewChairID(i)].SetCardData(m_cbHandCardData[i],cbMingCount);
			}

			//用户名字
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;

				//获取用户
				IClientUserItem * pUserData=GetTableUserItem(i);
				if (pUserData!=NULL) lstrcpyn(m_szAccounts[i],pUserData->GetNickName(),CountArray(m_szAccounts[i]));
				else lstrcpyn(m_szAccounts[i],TEXT("用户已离开"),CountArray(m_szAccounts[i]));
			}

			//响应控件
			if(!IsLookonMode())m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetPositively(true);

			//用户控件
			if(m_wCurrentUser==wMeChairID && !IsLookonMode())
			{
				//更新控件
				UpdateScoreControl();
			}

			//设置时间
			SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

			return true;
		}
	case GS_TK_OPEN_CARD:	//开牌状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusOpenCard)) return false;
			CMD_S_StatusOpenCard * pStatusOpenCard=(CMD_S_StatusOpenCard *)pBuffer;

			//设置变量
			WORD wMeChairID=GetMeChairID();
			m_wBankerUser=pStatusOpenCard->wBankerUser;
			m_lCellScore = pStatusOpenCard->lCellScore;
			CopyMemory(m_lUserScore,pStatusOpenCard->lUserScore,sizeof(m_lUserScore));
			CopyMemory(m_lTotalScore,pStatusOpenCard->lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(m_cbPlayStatus,pStatusOpenCard->cbPlayStatus,sizeof(m_cbPlayStatus));
			m_GameClientView.m_JetonControl.SetCellJetton(m_lCellScore);

			//明牌数据
			BYTE cbSpeCount=2;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//用户扑克
				CopyMemory(&m_cbHandCardData[i][cbSpeCount],pStatusOpenCard->cbMingCard[i],sizeof(BYTE)*cbSpeCount);
			}
			CopyMemory(m_cbHandCardData[wMeChairID],pStatusOpenCard->cbHandCard,sizeof(BYTE)*cbSpeCount);

			//用户名字
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;

				//获取用户
				IClientUserItem * pUserData=GetTableUserItem(i);
				if (pUserData!=NULL) lstrcpyn(m_szAccounts[i],pUserData->GetNickName(),CountArray(m_szAccounts[i]));
				else lstrcpyn(m_szAccounts[i],TEXT("用户已离开"),CountArray(m_szAccounts[i]));
			}

			//设置界面
			m_GameClientView.SetWaitOpenCard(true);
			m_GameClientView.SetCellScore(m_lCellScore);
			m_GameClientView.SetBankerUser(GetViewChairID(m_wBankerUser));
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//操作背景
				if(pStatusOpenCard->cbOpenFinish[i]==FALSE)
				{
					m_GameClientView.SetUserOperate(GetViewChairID(i),true);
					m_GameClientView.m_CardControl[GetViewChairID(i)].SetCardData(m_cbHandCardData[i],MAX_COUNT);
				}
				else 
				{
					BYTE cbCardData[MAX_COUNT];
					ZeroMemory(cbCardData,sizeof(cbCardData));
					m_GameClientView.m_CardControl[GetViewChairID(i)].SetCardData(cbCardData,MAX_COUNT);
				}

				//用户筹码
				m_GameClientView.SetUserJetton(GetViewChairID(i),m_lUserScore[i]-m_lTotalScore[i]);
			}

			//总筹码数
			LONGLONG lAllScore=0;
			for(WORD i=0;i<GAME_PLAYER;i++)if(m_lTotalScore[i]>0)lAllScore+=m_lTotalScore[i];
			m_GameClientView.SetCenterScore(lAllScore);

			//下注信息
			TCHAR szBuffer[512]=TEXT("");
			myprintf(szBuffer,CountArray(szBuffer),TEXT("\n下注信息:"));
			//InsertGeneralString(szBuffer,RGB(0,0,0),true);
			
			if(m_pIStringMessage!=NULL)
				m_pIStringMessage->InsertCustomString(TEXT("\n下注信息:"),RGB(0,0,0),RGB(255,255,255));

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;

				//成绩输出
				myprintf(szBuffer,CountArray(szBuffer),TEXT("[%s]总下注:%I64d"),
					m_szAccounts[i],m_lTotalScore[i]);
				//InsertGeneralString(szBuffer,RGB(0,0,0),true);

				if(m_pIStringMessage!=NULL)
					m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,0,0),RGB(255,255,255));
			}

			//界面处理
			SetGameClock(GetMeChairID(),IDI_NULLITY,TIME_USER_OPEN_CARD);

			//显示控件
			if(pStatusOpenCard->cbOpenFinish[wMeChairID]==FALSE && !IsLookonMode())
			{
				//设置界面
				m_GameClientView.SetHitPositively(true);
				m_GameClientView.SetHitCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
				m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetCardData(NULL,0);

				//分牌按钮
				m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOpenCard.EnableWindow(FALSE);

				//设置时间
				SetTimer(IDI_TIME_OPEN_CARD,TIME_USER_OPEN_CARD*1000,NULL);
			}
			else if(!IsLookonMode())
			{
				//设置扑克
				BYTE cbTempData[MAX_COUNT];
				CopyMemory(&cbTempData[cbSpeCount],m_cbHandCardData[wMeChairID],cbSpeCount);
				CopyMemory(cbTempData,&m_cbHandCardData[wMeChairID][cbSpeCount],cbSpeCount);
				m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetCardData(cbTempData,MAX_COUNT);
				m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetDisplayHead(true);
			}

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//清理旁观
	if (IsLookonMode())OnStart(0,0);

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pUserData=GetTableUserItem(i);
		if (pUserData==NULL) continue;

		//游戏信息
		m_cbPlayStatus[i]=TRUE;

		//用户名字
		lstrcpyn(m_szAccounts[i],pUserData->GetNickName(),CountArray(m_szAccounts[i]));
	}
	CopyMemory(m_lUserScore,pGameStart->lUserScore,sizeof(m_lUserScore));

	//用户筹码
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=TRUE)continue;
		m_GameClientView.SetUserJetton(GetViewChairID(i),m_lUserScore[i]);

		m_GameClientView.SetUserOperate(GetViewChairID(i),true);
	}

	//设置庄家
	m_wBankerUser=pGameStart->wBankerUser;
	m_GameClientView.SetBankerUser(GetViewChairID(m_wBankerUser));

	//等待标志
	m_GameClientView.SetWaitInvest(true);

	//控件处理
	WORD wMeChairID = GetMeChairID();
	if(!IsLookonMode())
	{
		//用户控件
		UpdateInvestControl();

		//自动处理
		if(m_GameClientView.m_btClearScore.IsWindowVisible()==FALSE)
		{
			OnAddScore(IDC_INVEST,0);
			//InsertGeneralString(TEXT("\n余簸低于单元数,不能再加."),RGB(255,0,0),true);

			if(m_pIStringMessage!=NULL)
				m_pIStringMessage->InsertCustomString(TEXT("\n余簸低于单元数,不能再加."),RGB(255,0,0),RGB(255,255,255));
		}
		//设置定时
		else SetTimer(IDI_TIME_USER_INVEST,TIME_USER_INVEST*1000,NULL);
	}

	//辅助定时
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	SetGameClock(wMeChairID,IDI_NULLITY,TIME_USER_INVEST);

	return true;
}

//用户下本
bool CGameClientEngine::OnSubUserInvest(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_UserInvest)) return false;
	CMD_S_UserInvest * pUserInvest=(CMD_S_UserInvest *)pBuffer;

	if(GetMeChairID()!=pUserInvest->wChairID || IsLookonMode())
	{
		//筹码数目
		WORD wChairID = pUserInvest->wChairID;
		m_lUserScore[wChairID] = pUserInvest->lScore;
		m_GameClientView.SetUserJetton(GetViewChairID(wChairID),m_lUserScore[wChairID]);

		m_GameClientView.SetUserOperate(GetViewChairID(wChairID),false);
		PlayGameSound(AfxGetInstanceHandle(),TEXT("INVEST"));
	}

	return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//当前用户
	m_wCurrentUser = pSendCard->wCurrentUser;
	WORD wMeChairID = GetMeChairID();

	//下单元注
	BYTE cbSpeCount = 2;
	if(pSendCard->cbCardCount==cbSpeCount || pSendCard->wCurrentUser==INVALID_CHAIR)
	{
		//删除定时器
		KillGameClock(IDI_NULLITY);
		KillTimer(IDI_TIME_USER_INVEST);

		//清理界面
		m_GameClientView.SetWaitInvest(false);

		//查找用户
		for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)break;

		//下单元注
		if(m_lTotalScore[i]==0)
		{
			//响应扑克
			if(!IsLookonMode())m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetPositively(true);

			//下单元注
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;
				m_lTableScore[i]=m_lCellScore;
				m_lTotalScore[i]=m_lCellScore;

				//动画筹码
				m_GameClientView.SetJettonMoveInfo(GetViewChairID(i),MOVE_USER_ADD,m_lCellScore);

				//用户筹码
				LONGLONG lScore = m_lUserScore[i]-m_lTotalScore[i];
				m_GameClientView.SetUserJetton(GetViewChairID(i),lScore);
			}

			//大小下注
			if(m_wCurrentUser<GAME_PLAYER)
			{
				m_lTurnMinScore=0;
				m_lTurnMaxScore=m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
			}
		}
	}

	//手上扑克
	CopyMemory(m_cbHandCardData[wMeChairID],pSendCard->cbHandCard,cbSpeCount);

	//明牌数据
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		CopyMemory(&m_cbHandCardData[i][cbSpeCount],pSendCard->cbMingCard[i],cbSpeCount);
	}

	//扑克数目
	BYTE cbCardCount=0;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)
		{
			cbCardCount=(BYTE)m_GameClientView.m_CardControl[GetViewChairID(i)].GetCardCount();
			break;
		}
	}

	//派发扑克
	for(WORD i=cbCardCount;i<pSendCard->cbCardCount;i++)
	{
		for (WORD j=m_wBankerUser;j<m_wBankerUser+GAME_PLAYER;j++)
		{
			WORD w=j%GAME_PLAYER;
			if (m_cbPlayStatus[w]!=TRUE)continue;

			WORD wViewChairID=GetViewChairID(w);
			m_GameClientView.DispatchUserCard(wViewChairID,m_cbHandCardData[w][i]);
		}
	}

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubUserGiveUp(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;
	WORD wViewID = GetViewChairID(pGiveUp->wGiveUpUser);
	m_GameClientView.m_CardControl[wViewID].SetDisableCardBack(true);
	m_GameClientView.SetUserOperate(wViewID,false);
 
	//环境设置
	if(pGiveUp->wGiveUpUser==m_wCurrentUser && pGiveUp->wGameStatus==GS_TK_SCORE)
	{
		m_GameClientView.SetUserAction(wViewID,IDC_GIVE_UP);
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));
	}

	return true;
}

//用户下注
bool CGameClientEngine::OnSubAddScore(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	//删除时间
	KillTimer(IDI_USER_ADD_SCORE);

	//辅助变量
	WORD wMeChairID = GetMeChairID();
	WORD wAddUser = pAddScore->wAddScoreUser;
	LONGLONG lAddScore = pAddScore->lAddScoreCount;
	WORD wViewAddUser = GetViewChairID(wAddUser);

	//清理界面
	m_GameClientView.SetUserOperate(wViewAddUser,false);

	//环境设置
	if((wAddUser!=wMeChairID || IsLookonMode()) && m_cbPlayStatus[wAddUser]!=FALSE)
	{
		//播放声音
		WORD wUserAction=0;
		if(lAddScore==0)
		{
			wUserAction = IDC_PASS;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
		}
		else if(lAddScore==m_lTurnMaxScore)
		{
			wUserAction = IDC_SHOW_HAND;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
		}
		else if(lAddScore>m_lTurnMinScore)
		{
			wUserAction = IDC_ADD_SCORE;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
		else if(lAddScore==m_lTurnMinScore)
		{
			wUserAction = IDC_FOLLOW;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
		}

		//提示操作
		if(wUserAction>0)m_GameClientView.SetUserAction(wViewAddUser,wUserAction);
	}

	//设置变量
	m_lTurnMaxScore =pAddScore->lTurnMaxScore;
	m_lTurnMinScore	= pAddScore->lTurnMinScore;
	m_wCurrentUser = pAddScore->wCurrentUser;

	//界面筹码
	if(wAddUser!=wMeChairID || IsLookonMode())
	{
		//设置筹码
		m_lTotalScore[wAddUser]+=lAddScore;
		m_lTableScore[wAddUser]+=lAddScore;

		//移动筹码
		m_GameClientView.SetJettonMoveInfo(wViewAddUser,MOVE_USER_ADD,lAddScore);

		//视图筹码
		LONGLONG lMeGoldCount = m_lUserScore[wAddUser]-m_lTotalScore[wAddUser];
		m_GameClientView.SetUserJetton(wViewAddUser,lMeGoldCount);
	}

	//当前用户
	if(m_wCurrentUser==wMeChairID && !IsLookonMode())
	{
		//更新控件
		UpdateScoreControl();
	}

	//设置时间
	if(m_wCurrentUser<GAME_PLAYER)
	{
		//设置界面
		m_GameClientView.SetUserOperate(GetViewChairID(m_wCurrentUser),true);

		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return true;
}

//开始分牌
bool CGameClientEngine::OnSubStartOpen(VOID * pBuffer, WORD wDataSize)
{
	//分牌状态
	SetGameStatus(GS_TK_OPEN_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	//判断动作
	if(!m_GameClientView.IsCardMoveing())OnSendCardFinish(0,0);

	return true;
}

//用户开牌
bool CGameClientEngine::OnSubOpenCard(VOID * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_Open_Card)) return false;
	CMD_S_Open_Card * pStatusOpenCard=(CMD_S_Open_Card *)pBuffer;

	//辅助变量
	WORD wMeChairID = GetMeChairID();
	WORD wOpenUser = pStatusOpenCard->wChairID;
	WORD wViewOpenUser = GetViewChairID(wOpenUser);

	//设置界面
	if(wMeChairID!=wOpenUser || IsLookonMode())
	{
		BYTE cbCardData[MAX_COUNT];
		ZeroMemory(cbCardData,sizeof(cbCardData));
		m_GameClientView.m_CardControl[wViewOpenUser].SetCardData(cbCardData,MAX_COUNT);

		m_GameClientView.SetUserOperate(wViewOpenUser,false);
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OPEN_CARD"));
	}

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(VOID * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//隐藏按钮
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);	
	m_GameClientView.m_btInvest.ShowWindow(SW_HIDE);	
	m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);	
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);		
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);	
	m_GameClientView.m_btPass.ShowWindow(SW_HIDE);		
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);	
	m_GameClientView.m_btClearScore.ShowWindow(SW_HIDE);
	for(WORD i=0;i<CONTROL_COUNT;i++)m_GameClientView.m_btDecrease[i].ShowWindow(SW_HIDE);
	m_GameClientView.m_JetonControl.SetUserJetton(0);
	m_GameClientView.m_JetonControl.SetAddJetton(0);

	//删除时间
	KillTimer(IDI_TIME_USER_INVEST);	
	KillTimer(IDI_TIME_OPEN_CARD);	
	KillGameClock(IDI_USER_ADD_SCORE);
	KillGameClock(IDI_NULLITY);

	//清理界面
	m_GameClientView.SetWaitInvest(false);
	m_GameClientView.SetWaitOpenCard(false);
	m_GameClientView.SetHitPositively(false);
	m_GameClientView.SetHitCardData(NULL,0);
	for(WORD i=0;i<CONTROL_COUNT;i++)
	{
		m_GameClientView.m_CardControl[i].SetPositively(false);
	}
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetOffLinkUser(i,false);
		m_GameClientView.SetUserOperate(i,false);
	}

	//设置变量
	CopyMemory(m_lUserTax,pGameEnd->lGameTax,sizeof(m_lUserTax));
	CopyMemory(m_lUserGetScore,pGameEnd->lGameScore,sizeof(m_lUserGetScore));
	CopyMemory(m_cbHandCardData,pGameEnd->cbCardData,sizeof(m_cbHandCardData));

	//结束原因
	switch(pGameEnd->cbEndReason)
	{
	case END_REASON_NORMAL:			//正常结束
		{
			//查找用户
			WORD wHeadMaxUser=INVALID_CHAIR;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				wHeadMaxUser = (i+m_wBankerUser)%GAME_PLAYER;
				if(m_cbPlayStatus[wHeadMaxUser]!=FALSE)break;
			}
			WORD wFirstOpen = wHeadMaxUser;

			//比较头牌
			BYTE cbSpeCount = 2;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				//当前用户
				WORD wCurUser = (i+m_wBankerUser)%GAME_PLAYER;
				if(m_cbPlayStatus[wCurUser]==FALSE || wCurUser==wHeadMaxUser)continue;

				//比较大小
				BYTE cbResult=m_GameLogic.CompareCard(m_cbHandCardData[wCurUser],m_cbHandCardData[wHeadMaxUser],cbSpeCount);
				if(cbResult==TRUE)wHeadMaxUser=wCurUser;
			}
			ASSERT(m_cbPlayStatus[wHeadMaxUser]==TRUE);

			//无效用户
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)continue;
				m_GameClientView.m_CardControl[GetViewChairID(i)].SetDisableCardBack(true);
			}

			//视图扑克
			BYTE cbTempData[GAME_PLAYER][MAX_COUNT];
			ZeroMemory(cbTempData,sizeof(cbTempData));
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]!=TRUE)continue;

				WORD wViewID = GetViewChairID(i);
				CopyMemory(cbTempData[wViewID],m_cbHandCardData[i],MAX_COUNT);

				//扑克牌型
				TCHAR szHeap[LEN_NICKNAME]=TEXT("");
				TCHAR sxTail[LEN_NICKNAME]=TEXT("");
				GetHeapTailType(i,szHeap,sxTail);
				m_GameClientView.SetCardType(wViewID,szHeap,sxTail);
			}

			//开牌动画
			WORD wViewOpen = GetViewChairID(wFirstOpen);
			WORD wViewHeadMaxUser = GetViewChairID(wHeadMaxUser);
			m_GameClientView.SetOpenCardInfo(cbTempData,wViewOpen,wViewHeadMaxUser);

			break;
		}
	case END_REASON_EXIT:			//强退结束
		{
			//筹码集中
			LONGLONG lScore=0;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				lScore += m_lTotalScore[i];
				m_GameClientView.SetUserTableScore(i,0);
			}
			m_GameClientView.SetCenterScore(lScore);

			SetTimer(IDI_DELAY_EXIT_WIN,500*TIME_DELAY_EXIT_WIN,NULL);

			break;
		}
	case END_REASON_PASS:			//让牌结束
		{
			//设置扑克
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;
				m_GameClientView.m_CardControl[GetViewChairID(i)].SetDisableCardBack(false);

				BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[GetViewChairID(i)].GetCardCount();
				m_GameClientView.m_CardControl[GetViewChairID(i)].SetCardData(m_cbHandCardData[i],cbCardCount);
			}
			m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetDisplayHead(true);

			//退回金币
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_CardControl[i].SetPositively(false);
				m_GameClientView.SetUserTableScore(i,0);

				if(m_lUserGetScore[i]==0 || GetTableUserItem(i)==NULL)continue;
				m_lUserGetScore[i]=0;
				m_GameClientView.SetJettonMoveInfo(GetViewChairID(i),MOVE_BACK_SCORE,m_lTotalScore[i]);
			}

			SetTimer(IDI_DELAY_SHOW_BUTTON,1000*TIME_DELAY_SHOW_BUTTON,NULL);

			break;
		}
	}


	return true;
}

//获取牌型
void CGameClientEngine::GetHeapTailType(WORD wChairID,TCHAR szHeapString[],TCHAR szTailString[])
{
	//获取牌型
	BYTE cbHeapType=m_GameLogic.GetCardType(m_cbHandCardData[wChairID],2);
	BYTE cbTailType=m_GameLogic.GetCardType(&m_cbHandCardData[wChairID][2],2);

	//判断牌型
	for(BYTE i=0;i<2;i++)
	{
		//代理变量
		BYTE cbTemp = ((i==0)?(cbHeapType):(cbTailType));
		TCHAR *pszString = ((i==0)?(szHeapString):(szTailString));
		BYTE cbTempCardData[MAX_COUNT];
		if(i==1)CopyMemory(cbTempCardData,&m_cbHandCardData[wChairID][2],sizeof(BYTE)*2);
		else CopyMemory(cbTempCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*2);

		//判断牌型
		switch(cbTemp)
		{
		case D2H_VALUE0:
			{
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("0点"));
				break;
			}
		case D2H_D2H:
			{
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("丁二皇"));
				break;
			}
		case D2H_TH:
			{
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("天皇"));
				break;
			}
		case D2H_TG:
			{
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("天杠"));
				break;
			}
		case D2H_DG:
			{
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("地杠"));
				break;
			}
		case D2H_DOBLUE_Q:
		case D2H_DOBLUE_2:
		case D2H_DOBLUE_8:
		case D2H_DOBLUE_4:
		case D2H_DOBLUE_46A:
		case D2H_DOBLUE_67AJ:
		case D2H_DOBLUE_9875:
			{
				TCHAR tChInfo[LEN_NICKNAME]=TEXT("");
				GetCardType(cbTempCardData,1,tChInfo);
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("%s对"),tChInfo);
				break;
			}
		default:
			{
				TCHAR tChInfo[LEN_NICKNAME]=TEXT("");
				GetCardType(cbTempCardData,2,tChInfo);
				_sntprintf(pszString,sizeof(szHeapString[0])*LEN_NICKNAME,TEXT("%s%d"),tChInfo,cbTemp);
			}
		}
	}

	return ;
}

//获取牌型
void CGameClientEngine::GetCardType(BYTE cbCardData[],BYTE cbCardCount,TCHAR szResult[])
{
	BYTE cbValue=m_GameLogic.GetSpeCardValue(cbCardData,cbCardCount);

	//判断牌型
	switch(cbValue)
	{
	case 9:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("天"));
			break;
		}
	case 8:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("地"));
			break;
		}
	case 7:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("仁"));
			break;
		}
	case 6:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("和"));
			break;
		}
	case 5:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("中"));
			break;
		}
	case 4:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("下"));
			break;
		}
	case 3:
		{
			_sntprintf(szResult,sizeof(szResult[0])*LEN_NICKNAME,TEXT("花"));
			break;
		}
	}

	return ;
}

//获取名字
void CGameClientEngine::GetViewUserName(WORD wViewChairID,TCHAR szNametring[])
{
	//查找用户
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_wViewChairID[i]==wViewChairID)break;
	}
	ASSERT(i<GAME_PLAYER);
	if(i>=GAME_PLAYER)return;

	_sntprintf(szNametring,sizeof(m_szAccounts[i]),TEXT("%s"),m_szAccounts[i]);

	return;
}

//视窗位置
WORD CGameClientEngine::GetViewChairID(WORD wChairID)
{
	if(m_wViewChairID[wChairID]==INVALID_CHAIR)
	{
		m_wViewChairID[wChairID]=SwitchViewChairID(wChairID);
	}

	return m_wViewChairID[wChairID];
}

//更新控制
void CGameClientEngine::UpdateScoreControl()
{
	ActiveGameFrame();
	WORD wMeChairID = GetMeChairID();

	//可加用户
	WORD wShowHandCount=0,wUserCount=0;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		wUserCount++;
		if(m_lUserScore[i]==m_lTotalScore[i])wShowHandCount++;
	}
	if(wUserCount<=wShowHandCount+1 && m_lUserScore[wMeChairID]>m_lTurnMaxScore)wShowHandCount=TRUE;
	else wShowHandCount=0;

	//筹码控件
	if(m_lTurnMaxScore-m_lTurnMinScore-m_lCellScore>0 && wShowHandCount==0)
	{
		//清理按钮
		m_GameClientView.m_btClearScore.ShowWindow(SW_SHOW);

		//减注按钮
		for(WORD i=0;i<CONTROL_COUNT;i++)
		{
			m_GameClientView.m_btDecrease[i].ShowWindow(SW_SHOW);
		}
 
		//用户筹码
		m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,0);
		m_GameClientView.m_JetonControl.SetUserJetton(m_lUserScore[wMeChairID]-m_lTotalScore[wMeChairID]-m_lTurnMinScore);
		m_GameClientView.m_JetonControl.SetMinJetton(m_lTurnMinScore);
		m_GameClientView.m_JetonControl.SetUserHaveCount(0);
	}

	//让牌按钮
	BYTE cbEnable=((m_lTurnMinScore>0L)?FALSE:TRUE);
	m_GameClientView.m_btPass.ShowWindow(SW_SHOW);
	m_GameClientView.m_btPass.EnableWindow(cbEnable);

	//跟注按钮
	cbEnable=((m_lTurnMinScore==m_lTurnMaxScore||m_lTurnMinScore==0L)?FALSE:TRUE);
	m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
	m_GameClientView.m_btFollow.EnableWindow(cbEnable);

	//加注判断
	//cbEnable=((m_lTurnMaxScore-m_lCellScore>=m_lTurnMinScore)?TRUE:FALSE);
	m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);

	//放弃按钮
	m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	m_GameClientView.m_btGiveUp.EnableWindow(TRUE);

	//梭哈按钮
	m_GameClientView.m_btShowHand.ShowWindow(SW_SHOW);
	m_GameClientView.m_btShowHand.EnableWindow(TRUE);

	return;
}

//更新控制
void CGameClientEngine::UpdateInvestControl()
{
	//自己筹码
	WORD wMeChairID=GetMeChairID();
	LONGLONG lScore = GetTableUserItem(wMeChairID)->GetUserScore();

	//剩余筹码
	lScore-=m_lUserScore[wMeChairID];

	//显示判断
	if(lScore>0)
	{
		//最少注数
		if(m_lUserScore[wMeChairID]<m_lCellScore)
		{
			lScore-=m_lCellScore;
			if(lScore>0)m_GameClientView.m_JetonControl.SetMinJetton(m_lCellScore);
		}

		//显示控件
		if(lScore-m_lCellScore>0)
		{
			//清理按钮
			m_GameClientView.m_btClearScore.ShowWindow(SW_SHOW);

			//减注按钮
			for(WORD i=0;i<CONTROL_COUNT;i++)
			{
				m_GameClientView.m_btDecrease[i].ShowWindow(SW_SHOW);
			}

			//用户筹码
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,0);
			m_GameClientView.m_JetonControl.SetUserJetton(lScore);
			m_GameClientView.m_JetonControl.SetUserHaveCount(m_lUserScore[wMeChairID]);

			//下本按钮
			m_GameClientView.m_btInvest.ShowWindow(SW_SHOW);

			ActiveGameFrame();
		}
	}

	return;
}

//开始按钮
LRESULT	CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	////派发扑克
	//for(WORD i=0;i<MAX_COUNT;i++)
	//{
	//	for (WORD j=0;j<GAME_PLAYER;j++)
	//	{
	//		m_GameClientView.m_CardControl[j].SetCardData(NULL,0);
	//		m_GameClientView.m_CardControl[j].SetDisplayHead(true);
	//		WORD w=j%GAME_PLAYER;

	//		m_GameClientView.DispatchUserCard(w,m_cbHandCardData[w][i]);
	//	}
	//}
	////	//筹码集中
	////	for(WORD i=0;i<GAME_PLAYER;i++)
	////	{
	////		m_GameClientView.SetUserTableScore(i,55);
	////		LONGLONG lScore = m_GameClientView.m_AddGoldView[i].GetGold();
	////		if(lScore==0)continue;
	////		m_GameClientView.SetUserTableScore(i,0);
	////		m_GameClientView.SetJettonMoveInfo(i,MOVE_POOL_ALLSCORE,lScore);
	////	}
	//return 0;

	//移动筹码
	//m_GameClientView.SetJettonMoveInfo(MY_VIEW_CHAIR_ID,MOVE_USER_GETSCORE,10000);

	//BYTE cbCardListData[32]=
	//{
	//	0x02,0x22,0x23,0x04,0x14,0x24,0x34,
	//	0x25,0x35,0x06,0x16,0x26,0x36,
	//	0x07,0x17,0x27,0x37,0x08,0x18,0x28,0x38,
	//	0x29,0x39,0x0A,0x1A,0x2A,0x3A,0x1B,0x3B,0x0C,0x2C,
	//	0x4E,
	//};

	//CopyMemory(m_cbHandCardData,cbCardListData,sizeof(m_cbHandCardData));

	//TCHAR tHeap[32],tTail[32];

	//for(WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	m_GameClientView.SetUserOperate(i,false);
	//	GetHeapTailType(i,tHeap,tTail);
	//	m_GameClientView.SetCardType(i,tHeap,tTail);
	//}
	//m_GameClientView.SetOpenCardInfo(m_cbHandCardData,0,0);
	//return 0;
#endif 

	//清理变量
	m_wBankerUser=INVALID_CHAIR;							
	m_wCurrentUser=INVALID_CHAIR;				
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserGetScore,sizeof(m_lUserGetScore));
	ZeroMemory(m_lUserTax,sizeof(m_lUserTax));

	//清理界面
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetUserOperate(i,false);
		m_GameClientView.SetBankerUser(INVALID_CHAIR);
		m_GameClientView.m_CardControl[i].SetDisableCardBack(false);
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.SetCardType(INVALID_CHAIR,NULL,NULL);
	}
	m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetPositively(false);
	m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetDisplayHead(false);

	//下注变量
	m_lTurnMinScore=0;
	m_lTurnMaxScore=0;	
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	//删除时间
	KillTimer(IDI_DELAY_SHOW_BUTTON);
	KillTimer(IDI_DELAY_EXIT_WIN);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//发送消息
	if(!IsLookonMode())
	{
		m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		KillGameClock(IDI_START_GAME);
		SendUserReady(NULL,0);
	}

	return 0;
}

//加注按钮
LRESULT	CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	BYTE cbMsg = BYTE(wParam);
	WORD wMeChairID = GetMeChairID();
	LONGLONG lAddScore = m_GameClientView.m_JetonControl.GetAddJetton();

	//隐藏按钮
	m_GameClientView.m_btClearScore.ShowWindow(SW_HIDE);
	for(WORD i=0;i<CONTROL_COUNT;i++)m_GameClientView.m_btDecrease[i].ShowWindow(SW_HIDE);
	if(IDC_INVEST==cbMsg)
	{
		m_GameClientView.m_btInvest.ShowWindow(SW_HIDE);

		//删除时间
		KillTimer(IDI_TIME_USER_INVEST);	
	}
	else
	{
		m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);
		m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
		m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPass.ShowWindow(SW_HIDE);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);

		//删除时间
		KillGameClock(IDI_USER_ADD_SCORE);
	}

	//界面清理
	m_GameClientView.m_JetonControl.SetAddJetton(0);
	m_GameClientView.m_JetonControl.SetUserJetton(0);
	m_GameClientView.m_JetonControl.SetMinJetton(0);
	m_GameClientView.SetUserOperate(MY_VIEW_CHAIR_ID,false);

	//按钮消息
	switch(cbMsg)
	{
	case IDC_INVEST:			//下本消息
		{
			//发送数据
			CMD_C_UserInvest UserInvest;
			UserInvest.lInvestGold = lAddScore;
			SendSocketData(SUB_C_USER_INVEST,&UserInvest,sizeof(UserInvest));

			//视图筹码
			m_lUserScore[wMeChairID]+=UserInvest.lInvestGold;
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,m_lUserScore[wMeChairID]);

			break;
		}
	case IDC_PASS:				//不加消息
		{
			//发送数据
			CMD_C_AddScore AddScore;
			AddScore.lScore=0;
			SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//视图筹码
			LONGLONG lMeGoldCount = m_lUserScore[wMeChairID]-m_lTotalScore[wMeChairID];
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,lMeGoldCount);

			break;
		}
	case IDC_ADD_SCORE:			//加注消息
		{
			//发送数据
			CMD_C_AddScore AddScore;
			AddScore.lScore=lAddScore;
			SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//设置筹码
			m_lTotalScore[wMeChairID]+=AddScore.lScore;
			m_lTableScore[wMeChairID]+=AddScore.lScore;

			//视图筹码
			LONGLONG lMeGoldCount = m_lUserScore[wMeChairID]-m_lTotalScore[wMeChairID];
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,lMeGoldCount);

			//移动筹码
			m_GameClientView.SetJettonMoveInfo(MY_VIEW_CHAIR_ID,MOVE_USER_ADD,AddScore.lScore);

			break;
		}
	case IDC_FOLLOW:			//跟注消息
		{
			//发送数据
			CMD_C_AddScore AddScore;
			AddScore.lScore=m_lTurnMinScore;
			SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//设置筹码
			m_lTotalScore[wMeChairID]+=AddScore.lScore;
			m_lTableScore[wMeChairID]+=AddScore.lScore;

			//视图筹码
			LONGLONG lMeGoldCount = m_lUserScore[wMeChairID]-m_lTotalScore[wMeChairID];
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,lMeGoldCount);

			//移动筹码
			m_GameClientView.SetJettonMoveInfo(MY_VIEW_CHAIR_ID,MOVE_USER_ADD,AddScore.lScore);

			break;
		}
	case IDC_SHOW_HAND:			//全压消息
		{
			//发送数据
			CMD_C_AddScore AddScore;
			AddScore.lScore=m_lTurnMaxScore;
			SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//设置筹码
			m_lTotalScore[wMeChairID]+=AddScore.lScore;
			m_lTableScore[wMeChairID]+=AddScore.lScore;

			//视图筹码
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,0);

			//移动筹码
			m_GameClientView.SetJettonMoveInfo(MY_VIEW_CHAIR_ID,MOVE_USER_ADD,AddScore.lScore);

			break;
		}
	case IDC_GIVE_UP:			//放弃消息
		{
			//发送数据
			SendSocketData(SUB_C_GIVE_UP);

			//视图筹码
			LONGLONG lMeGoldCount = m_lUserScore[wMeChairID]-m_lTotalScore[wMeChairID];
			m_GameClientView.SetUserJetton(MY_VIEW_CHAIR_ID,lMeGoldCount);

			//扑克无效
			m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetDisableCardBack(true);

			break;
		}
	}

	//播放声音
	switch(cbMsg)
	{
	case IDC_INVEST:			//下本消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("INVEST"));
			break;
		}
	case IDC_PASS:				//不加消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
			break;
		}
	case IDC_ADD_SCORE:			//加注消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
			break;
		}
	case IDC_FOLLOW:			//跟注消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
			break;
		}
	case IDC_SHOW_HAND:			//全压消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
			break;
		}
	case IDC_GIVE_UP:			//放弃消息
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));
			break;
		}
	}

	//提示操作
	m_GameClientView.SetUserAction(MY_VIEW_CHAIR_ID,cbMsg);

	return 0;
}

//清理按钮
LRESULT	CGameClientEngine::OnClearScore(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_JetonControl.ClearAddJetton();

	PlayGameSound(AfxGetInstanceHandle(),TEXT("CLEAR"));

	//更新界面
	m_GameClientView.RefreshGameView();

	return 0;
}

//减注按钮
LRESULT	CGameClientEngine::OnDecreaseJetton(WPARAM wParam, LPARAM lParam)
{
	if(m_GameClientView.m_JetonControl.DecreaseJetton(WORD(wParam)-1))
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_JETTON"));

		//更新界面
		m_GameClientView.RefreshGameView();
	}

	return 0;
}

//分牌按钮
LRESULT	CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	WORD wMeChairID=GetMeChairID();
	BYTE cbCardData[MAX_COUNT];
	m_GameClientView.GetHitCardData(cbCardData,MAX_COUNT);

	//比较大小
	BYTE cbSpeCount = 2;
	BYTE cbValue=m_GameLogic.CompareCard(cbCardData,&cbCardData[cbSpeCount],cbSpeCount);
	if(cbValue==TRUE)
	{
		CopyMemory(&m_cbHandCardData[wMeChairID][cbSpeCount],cbCardData,cbSpeCount);
		CopyMemory(m_cbHandCardData[wMeChairID],&cbCardData[cbSpeCount],cbSpeCount);
	}
	else 
	{
		CopyMemory(m_cbHandCardData[wMeChairID],cbCardData,MAX_COUNT);
		m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetCardData(cbCardData,MAX_COUNT);
	}

	//发送信息
	CMD_C_OpenCard OpenCard;
	CopyMemory(OpenCard.cbCardData,m_cbHandCardData[wMeChairID],sizeof(OpenCard.cbCardData));
	SendSocketData(SUB_C_OPEN_CARD,&OpenCard,sizeof(OpenCard));

	//设置扑克
	BYTE cbTempData[MAX_COUNT];
	if(cbValue==TRUE || cbValue==FALSE)
	{
		CopyMemory(cbTempData,cbCardData,MAX_COUNT);
	}
	else 
	{
		CopyMemory(&cbTempData[cbSpeCount],cbCardData,cbSpeCount);
		CopyMemory(cbTempData,&cbCardData[cbSpeCount],cbSpeCount);
	}
	m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetCardData(cbTempData,MAX_COUNT);
	m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetDisplayHead(true);

	//设置界面
	m_GameClientView.SetUserOperate(MY_VIEW_CHAIR_ID,false);
	m_GameClientView.SetHitPositively(false);
	m_GameClientView.SetHitCardData(NULL,0);

	//分牌按钮
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//设置时间
	KillTimer(IDI_TIME_OPEN_CARD);

	PlayGameSound(AfxGetInstanceHandle(),TEXT("OPEN_CARD"));

	return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//分牌状态
	if(GetGameStatus()==GS_TK_OPEN_CARD)
	{
		//筹码集中
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			LONGLONG lScore = m_GameClientView.m_AddGoldView[i].GetGold();
			if(lScore==0)continue;
			m_GameClientView.SetUserTableScore(i,0);
			m_GameClientView.SetJettonMoveInfo(i,MOVE_POOL_ALLSCORE,lScore);
		}

		//下注信息
		TCHAR szBuffer[512]=TEXT("");
		myprintf(szBuffer,CountArray(szBuffer),TEXT("\n下注信息:"));
		//InsertGeneralString(szBuffer,RGB(0,0,0),true);
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertCustomString(TEXT("\n下注信息:"),RGB(255,0,0),RGB(255,255,255));
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if(m_lTotalScore[i]==0)continue;

			//成绩输出
			myprintf(szBuffer,CountArray(szBuffer),TEXT("[%s]总下注:%I64d"),
				m_szAccounts[i],m_lTotalScore[i]);
			//InsertGeneralString(szBuffer,RGB(0,0,0),true);

			if(m_pIStringMessage!=NULL)
				m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,0,0),RGB(255,255,255));
		}

		//操作背景
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			m_GameClientView.SetUserOperate(GetViewChairID(i),true);
		}

		//界面处理
		SetGameClock(GetMeChairID(),IDI_NULLITY,TIME_USER_OPEN_CARD);
		m_GameClientView.SetWaitOpenCard(true);

		//过滤用户
		WORD wMeChairID=GetMeChairID();
		if(m_cbPlayStatus[wMeChairID]!=TRUE || IsLookonMode())return 0;

		//设置界面
		m_GameClientView.SetHitPositively(true);
		m_GameClientView.SetHitCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
		m_GameClientView.m_CardControl[MY_VIEW_CHAIR_ID].SetCardData(NULL,0);

		//分牌按钮
		m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOpenCard.EnableWindow(FALSE);

		//设置时间
		SetTimer(IDI_TIME_OPEN_CARD,TIME_USER_OPEN_CARD*1000,NULL);

	}
	//继续下注
	else if(m_wCurrentUser<GAME_PLAYER)
	{
		//用户筹码
		WORD wUserViewID = GetViewChairID(m_wCurrentUser);

		//用户控件
		if(m_wCurrentUser==GetMeChairID() && !IsLookonMode())
		{
			//更新控件
			UpdateScoreControl();
		}

		//设置界面
		m_GameClientView.SetUserOperate(wUserViewID,true);

		//设置时间
		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return 0;
}

//分牌完成
LRESULT CGameClientEngine::OnOpenCardFinish(WPARAM wParam, LPARAM lParam)
{
	//用户得分
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		LONGLONG lScore = m_lUserGetScore[i]+m_lTotalScore[i];
		if(lScore<=0 || GetTableUserItem(i)==NULL)continue;
		m_GameClientView.SetJettonMoveInfo(GetViewChairID(i),MOVE_USER_GETSCORE,lScore);
	}
	m_GameClientView.SetCenterScore(0);

	SetTimer(IDI_DELAY_SHOW_BUTTON,1000*TIME_DELAY_SHOW_BUTTON,NULL);

	return 0;
}

//定时操作
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case IDI_TIME_USER_INVEST:			//用户下本
		{
			OnAddScore(IDC_INVEST,0);

			break;
		}
	case IDI_TIME_OPEN_CARD:			//用户开牌
		{
			OnOpenCard(0,0);

			break;
		}
	case IDI_DELAY_SHOW_BUTTON:			//分钱结束
		{
			if(!m_GameClientView.IsJettonMoveing())
			{
				//自由状态
				SetGameStatus(GS_TK_FREE);

				//删除时间
				KillTimer(nIDEvent);

				//下注人数
				for(WORD i=0;i<GAME_PLAYER;i++)if(m_lTotalScore[i]>0)break;

				//结束信息
				if(i<GAME_PLAYER)
				{
					//设置积分
					m_GameClientView.m_ScoreView.ResetScore();
					for(WORD i=0;i<GAME_PLAYER;i++)
					{
						if(m_lTotalScore[i]==0)continue;
						m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],m_lUserGetScore[i],m_lTotalScore[i]);
						m_GameClientView.m_ScoreView.SetGameTax(m_lUserTax[i],i);
					}
					m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

					//成绩显示
					TCHAR szBuffer[512]=TEXT("");
					myprintf(szBuffer,CountArray(szBuffer),TEXT("\n本局结束,成绩统计:"));
					//InsertGeneralString(szBuffer,RGB(0,0,255),true);

					if(m_pIStringMessage!=NULL)
						m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,0,255),RGB(255,255,255));

					for (WORD i=0;i<GAME_PLAYER;i++)
					{
						if(m_lTotalScore[i]==0)continue;

						//成绩输出
						myprintf(szBuffer,CountArray(szBuffer),
							TEXT("[%s]得分:%+I64d;"),/*下注:%I64d,	*/
							m_szAccounts[i],/*m_lTotalScore[i],*/m_lUserGetScore[i]);
						//InsertGeneralString(szBuffer,RGB(0,0,255),true);
						if(m_pIStringMessage!=NULL)
							m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,0,255),RGB(255,255,255));
					}
				}

				WORD wMeChairID = GetMeChairID();
				if(!IsLookonMode())
				{
					//开始按钮
					m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
					m_GameClientView.m_btStart.SetFocus();
					SetGameClock(wMeChairID,IDI_START_GAME,TIME_USER_START_GAME);
				}

				//播放声音
				if(m_lUserGetScore[wMeChairID]<0)
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
				else if(m_lUserGetScore[wMeChairID]>0)PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
				else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));
			}
			break;
		}
	case IDI_DELAY_EXIT_WIN:				//延时结束
		{
			//删除时间
			KillTimer(nIDEvent);

			OnOpenCardFinish(0,0);

			break;
		}
	}

	CGameFrameEngine::OnTimer(nIDEvent);
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////

