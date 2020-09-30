#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME				200									//开始定时器
#define IDI_USER_ADD_SCORE			201									//加注定时器

//时间标识
#define TIME_START_GAME				30									//开始定时器
#define TIME_USER_ADD_SCORE			30									//放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine,CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_FOLLOW,OnFollow)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_SHOW_HAND,OnShowHand)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//加注信息
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lShowHandScore = 0L;
	ZeroMemory( m_lUserScore,sizeof(m_lUserScore) );

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//设置图标
	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//加注信息
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lShowHandScore = 0L;
	ZeroMemory( m_lUserScore,sizeof(m_lUserScore) );

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return true;
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
			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			//获取位置
			WORD wViewChairID=SwitchViewChairID(wChairID);

			//中止判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wViewChairID==MYSELF_VIEW_ID)) OnGiveUp(1,1);
				return false;
			}
			return true;
		}
	}

	return false;
}


bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}
//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:	//用户下注
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubGameEnd(pData,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置控件
			if (IsLookonMode()==false&& GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameClock(GetMeChairID(),IDI_START_GAME,30);
			}

			return true;
		}
		
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//下注信息
			m_lCellScore=pStatusPlay->lCellScore;
			m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
			m_lTurnLessScore=pStatusPlay->lTurnLessScore;
			m_lShowHandScore = pStatusPlay->lShowHandScore;
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));

			//状态变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bShowHand=(pStatusPlay->bShowHand==TRUE)?true:false;
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));

			//帐号名字
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				IClientUserItem * pIClientUserItem=GetTableUserItem(i);
				if (pIClientUserItem!=NULL)
				{
					const tagUserInfo *pUserData=pIClientUserItem->GetUserInfo();
					m_lUserScore[i] = pUserData->lScore;
					lstrcpyn(m_szAccounts[i],pUserData->szNickName,CountArray(m_szAccounts[i]));
					m_GameClientView.SetUserScore( SwitchViewChairID(i),pUserData->lScore );
				}
			}

			//设置界面
			LONGLONG lTableScore=0L;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置位置
				WORD wViewChairID=SwitchViewChairID(i);

				//设置扑克
				if (m_cbPlayStatus[i]==TRUE) 
				{
					BYTE cbCardCount=pStatusPlay->cbCardCount[i];
					m_GameClientView.m_CardControl[wViewChairID].SetCardData(pStatusPlay->cbHandCardData[i],cbCardCount);
				}
				lTableScore += m_lTableScore[2*i+1];
				//筹码设置
				m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(m_lTableScore[2*i]);
				//设置下注
				m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[2*i]+m_lTableScore[2*i+1]);
			}
			m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(lTableScore);

			//
			m_GameClientView.SetCellScore(m_lCellScore);

			//玩家设置
			if (IsLookonMode()==false) 
			{
				//控制设置
				m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(true);
				if (m_wCurrentUser==GetMeChairID()) UpdateScoreControl();
			}

			//设置定时器
			SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
	SetGameStatus(GAME_STATUS_PLAY);

	//下注变量
	m_lCellScore=pGameStart->lCellScore;
	m_lTurnMaxScore=pGameStart->lTurnMaxScore;
	m_lTurnLessScore=pGameStart->lTurnLessScore;
	m_lShowHandScore = pGameStart->lShowHandScore;
	ZeroMemory( m_lUserScore,sizeof(m_lUserScore) );

	//状态变量
	m_wCurrentUser=pGameStart->wCurrentUser;

	//设置变量
	LONGLONG lTableScore=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem* pUserItem=GetTableUserItem(i);
		if (pUserItem==NULL) continue;

		//游戏信息
		m_cbPlayStatus[i]=TRUE;
		m_lUserScore[i] = pUserItem->GetUserScore();
		lTableScore+=m_lCellScore;
		m_lTableScore[2*i+1]=m_lCellScore;

		//用户名字
		lstrcpyn(m_szAccounts[i],pUserItem->GetNickName(),CountArray(m_szAccounts[i]));

		m_GameClientView.SetUserScore( SwitchViewChairID(i),pUserItem->GetUserScore());
	}

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);
	m_GameClientView.SetCellScore(m_lCellScore);

	//设置界面
	lTableScore = 0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		m_GameClientView.m_CardControl[wViewChairID].SetCardData(NULL,0); 
		m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(false);
		lTableScore += m_lTableScore[2*i+1];
		//设置筹码
		m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(m_lTableScore[2*i]);
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[i*2]+m_lTableScore[i*2+1]);
	}
	m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(lTableScore);

	//派发扑克
	for (BYTE cbIndex=0;cbIndex<2;cbIndex++)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (m_cbPlayStatus[i]==TRUE)
			{
				//变量定义
				WORD wViewChairID=SwitchViewChairID(i);
				BYTE cbCardData[2]={0,pGameStart->cbCardData[i]};

				//派发扑克
				cbCardData[0]=(GetMeChairID()==i)?pGameStart->cbObscureCard:0;
				m_GameClientView.DispatchUserCard(wViewChairID,cbCardData[cbIndex]);
			}
		}
	}

	//玩家设置
	if (IsLookonMode()==false) 
	{
		//控制设置
		m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(true);
	}

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//激活框架
	if (IsLookonMode()==false) ActiveGameFrame();

	return true;
}

//用户加注
bool CGameClientEngine::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=SwitchViewChairID(wAddScoreUser);

	//梭哈判断
	m_bShowHand = pAddScore->bShowHand;
	if( m_bShowHand && m_cbPlayStatus[wAddScoreUser] ) m_GameClientView.SetUserShowHand( true );

	//加注处理
	if ((IsLookonMode()==true)||(pAddScore->wAddScoreUser!=wMeChairID))
	{
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			//加注界面
			m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(pAddScore->lAddScoreCount);

			//播放声音
			//获取积分
			LONGLONG lTableScore=m_lTableScore[wAddScoreUser*2+1];
			LONGLONG lTurnAddScore=m_lTableScore[wAddScoreUser*2];

			//播放声音
			if ((pAddScore->lAddScoreCount-lTurnAddScore)==0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
			else if (m_bShowHand) PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
			else if ((pAddScore->lAddScoreCount+lTableScore)==m_lTurnLessScore) PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
	}

	//设置变量
	m_wCurrentUser=pAddScore->wCurrentUser;
	m_lTurnLessScore=pAddScore->lTurnLessScore;
	if( pAddScore->lAddScoreCount > 0L )
		m_lTableScore[wAddScoreUser*2]=pAddScore->lAddScoreCount;

	//设置筹码
	m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[wAddScoreUser*2+1]+m_lTableScore[wAddScoreUser*2]);
	m_GameClientView.InvalidGameView(0,0,0,0);

	//控制界面
	if ((IsLookonMode()==false)&&(pAddScore->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	if (m_wCurrentUser==INVALID_CHAIR) KillGameClock(IDI_USER_ADD_SCORE);
	else SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	BYTE cbCardData[MAX_COUNT]={0,0,0,0,0};
	WORD wViewChairID=SwitchViewChairID(wGiveUpUser);
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardCount();

	//设置扑克
	m_GameClientView.m_CardControl[wViewChairID].SetPositively(false);
	m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(false);
	m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);

	//状态设置
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID())) 
	{
		SetGameStatus(GAME_STATUS_FREE);
	}

	//更新数据
	if( m_lTurnMaxScore != pGiveUp->lTurnMaxScore )
	{
		//设置数据
		m_lTurnMaxScore = pGiveUp->lTurnMaxScore;

		if( !IsLookonMode() && m_wCurrentUser==GetMeChairID() && wGiveUpUser != m_wCurrentUser )
			UpdateScoreControl();
	}

	//环境设置
	if (wGiveUpUser==GetClockChairID()) KillGameClock(IDI_USER_ADD_SCORE);
	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_lTurnMaxScore=pSendCard->lTurnMaxScore;
	m_lTurnLessScore=pSendCard->lTurnLessScore;

	//设置筹码
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(0L);
		m_GameClientView.m_PlayerJeton[GAME_PLAYER].AddScore(m_lTableScore[i*2]);
	}

	//累计金币
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lTableScore[i*2+1]+=m_lTableScore[i*2];
		m_lTableScore[i*2]=0L;
	}

	//派发扑克,从上次最大玩家开始发起
	WORD wLastMostUser = pSendCard->wLastMostUser;
	ASSERT( wLastMostUser != INVALID_CHAIR );
	for (BYTE i=0;i<pSendCard->cbSendCardCount;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			WORD wChairId = (wLastMostUser+j)%GAME_PLAYER;
			if (m_cbPlayStatus[wChairId]==TRUE)
			{
				WORD wViewChairID=SwitchViewChairID(wChairId);
				m_GameClientView.DispatchUserCard(wViewChairID,pSendCard->cbCardData[wChairId][i]);
			}
		}
	}

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//状态设置
	HideScoreControl();
	SetGameStatus(GAME_STATUS_FREE);
	KillGameClock(IDI_USER_ADD_SCORE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//正常结束
	m_GameClientView.m_ScoreView.ResetScore();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameScore[i]!=0L)
		{
			m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
			//扣税
			m_GameClientView.m_ScoreView.SetGameTax(i,pGameEnd->lGameTax[i]);
			if (pGameEnd->cbCardData[i]!=0)
			{
				//获取扑克
				BYTE cbCardData[5];
				WORD wViewChairID=SwitchViewChairID(i);
				BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardData(cbCardData,CountArray(cbCardData));

				//设置扑克
				cbCardData[0]=pGameEnd->cbCardData[i];
				m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(true);
				m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
			}
		}
	}
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置积分
	TCHAR szBuffer[128] = TEXT("");
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString(TEXT("\n本局结束,成绩统计:\n"));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( pGameEnd->lGameScore[i] != 0L && m_szAccounts[i][0]!=0 )
		{
			//在聊天框显示成绩
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s: %+I64d分\n"),m_szAccounts[i],pGameEnd->lGameScore[i]);
			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertNormalString(szBuffer);
		}
	}

	//加注信息
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//开始按钮
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	return true;
}

//隐藏控制
void CGameClientEngine::HideScoreControl()
{
	//控制按钮
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddTimes1.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddTimes2.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddTimes3.ShowWindow(SW_HIDE);
	return;
}

//更新控制
void CGameClientEngine::UpdateScoreControl()
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardCount();
	LONGLONG lDrawAddScore=m_lTableScore[wMeChairID*2]+m_lTableScore[wMeChairID*2+1];

	//下注按钮
	m_GameClientView.m_btAddTimes1.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAddTimes2.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAddTimes3.ShowWindow(SW_SHOW);
	LONGLONG lLeaveScore=__max(m_lTurnMaxScore-m_lTurnLessScore,0L);
	m_GameClientView.m_btAddTimes1.EnableWindow(lLeaveScore>=m_lCellScore?TRUE:FALSE);
	m_GameClientView.m_btAddTimes2.EnableWindow(lLeaveScore>=m_lCellScore*2?TRUE:FALSE);
	m_GameClientView.m_btAddTimes3.EnableWindow(lLeaveScore>=m_lCellScore*3?TRUE:FALSE);

	//显示按钮
	LONGLONG lUserScore = m_lUserScore[wMeChairID];
	m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
	if( !m_bShowHand && lUserScore > m_lTurnLessScore )
		m_GameClientView.m_btFollow.EnableWindow();
	else
		m_GameClientView.m_btFollow.EnableWindow(FALSE);

	m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	m_GameClientView.m_btShowHand.ShowWindow(SW_SHOW);

	//控制按钮
	lLeaveScore = m_lTurnMaxScore-m_lTableScore[wMeChairID*2+1]-m_lTableScore[wMeChairID*2];
	m_GameClientView.m_btShowHand.EnableWindow((cbCardCount>=3&&(m_bShowHand||lLeaveScore>0L))?TRUE:FALSE);

	return;
}

//开始按钮
LRESULT	CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_START_GAME);

	//设置控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_PlayerJeton[i].SetScore(0L);
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
	}
	m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(0L);

	//设置界面
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);
	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//放弃按钮
LRESULT CGameClientEngine::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//放弃设置
	HideScoreControl();

	//界面设置
	KillGameClock(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//发送消息
	SendSocketData(SUB_C_GIVE_UP,0,0);

	return 0;
}

//跟注按钮
LRESULT CGameClientEngine::OnFollow(WPARAM wParam, LPARAM lParam)
{
	//获取筹码
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=m_lTurnLessScore-m_lTableScore[wMeChairID*2+1];

	//加注设置
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[MYSELF_VIEW_ID].SetScore(lCurrentScore);

	//界面设置
	KillGameClock(IDI_USER_ADD_SCORE);

	PlayGameSound(AfxGetInstanceHandle(),0L==lCurrentScore?TEXT("NO_ADD"):TEXT("FOLLOW"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//加注按钮 
LRESULT	CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	//获取筹码
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=m_lTurnLessScore-m_lTableScore[wMeChairID*2+1]+m_lCellScore*(lParam);

	//加注设置
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[MYSELF_VIEW_ID].SetScore(lCurrentScore);

	//界面设置
	KillGameClock(IDI_USER_ADD_SCORE);
	m_GameClientView.InvalidGameView(0,0,0,0);

	LONGLONG lShowHandScore = __min(m_lUserScore[wMeChairID],m_lShowHandScore);
	if( lCurrentScore + m_lTableScore[wMeChairID*2+1] == lShowHandScore )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//梭哈按钮
LRESULT CGameClientEngine::OnShowHand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	const tagUserInfo* pUserData=NULL;

	//获取用户
	if (wMeChairID!=INVALID_CHAIR)
	{
		pUserData=GetMeUserItem()->GetUserInfo();
		if (pUserData==NULL) return 0;
	}

	//验证
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardCount();
	if( cbCardCount < 3 || m_wCurrentUser!=wMeChairID ) return 0;

	//金币统计
	LONGLONG lShowHandScore = __min(m_lUserScore[wMeChairID],m_lShowHandScore);
	LONGLONG lCurrentScore=lShowHandScore-m_lTableScore[wMeChairID*2+1];

	//加注设置
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[MYSELF_VIEW_ID].SetScore(lCurrentScore);

	//界面设置
	KillGameClock(TIME_USER_ADD_SCORE);
	m_GameClientView.InvalidGameView(0,0,0,0);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//加注状态
	if ((IsLookonMode()==false)&&(GetMeChairID()==m_wCurrentUser))
	{
		//界面设置
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
