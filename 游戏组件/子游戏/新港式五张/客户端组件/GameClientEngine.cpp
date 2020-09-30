#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//游戏时间
#define IDI_ADD_SCORE				200									//加注定时器
#define IDI_START_GAME				201									//开始定时器

//游戏时间
#define TIME_ADD_SCORE				30									//加注定时器
#define TIME_START_GAME				15									//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnMessageStart)
	ON_MESSAGE(IDM_FOLLOW,OnMessageFollow)
	ON_MESSAGE(IDM_GIVE_UP,OnMessageGiveUp)
	ON_MESSAGE(IDM_ADD_SCORE,OnMessageAddScore)
	ON_MESSAGE(IDM_SHOW_HAND,OnMessageShowHand)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnMessageSendCardFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//加注信息
	m_lDrawMaxScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	m_lServiceCharge = 0l;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//辅助变量
	ZeroMemory(&m_GameEndPacket,sizeof(m_GameEndPacket));

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	if (m_pIClientKernel!=NULL) m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	return true;
}

//重置引擎
bool CGameClientEngine::OnResetGameEngine()
{
	//加注信息
	m_lDrawMaxScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//辅助变量
	ZeroMemory(&m_GameEndPacket,sizeof(m_GameEndPacket));

	return true;
}


//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
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
	case IDI_START_GAME:	//游戏开始
		{
			//关闭游戏
			if ((nElapse==0)&&(IsLookonMode()==false)&&(wChairID==GetMeChairID()))
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_ADD_SCORE:		//用户加注
		{
			//自动处理
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnMessageGiveUp(0,0);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}

	return false;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:	//用户加注
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发送扑克
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			return OnSubGameEnd(pData,wDataSize);
		}
#ifdef CHEAT_VERSION
	case SUB_S_GET_WINNER:
		{
			//消息处理
			return OnSubGetWinner( pData,wDataSize );
		}
#endif
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置界面
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);

			//玩家设置
			IClientUserItem * pIClientUserItem=GetMeUserItem();
			if ((IsLookonMode()==false)&&(pIClientUserItem->GetUserStatus()!=US_READY))
			{
				PerformAutoStart();
				m_GameClientView.m_btStart.ShowWindow(TRUE);
			}

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusFree->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusFree->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//变量定义
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//下注信息
			m_lCellScore = pStatusPlay->lCellScore;
			m_lServiceCharge = pStatusPlay->lServiceCharge;
			m_lDrawMaxScore=pStatusPlay->lDrawMaxScore;
			m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
			m_lTurnLessScore=pStatusPlay->lTurnLessScore;
			CopyMemory(m_lUserScore,pStatusPlay->lUserScore,sizeof(m_lUserScore));
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));

			//状态信息
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bShowHand=(pStatusPlay->cbShowHand==TRUE)?true:false;
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));

			//帐号名字
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				IClientUserItem * pIClientUserItem=GetTableUserItem(i);
				if (pIClientUserItem!=NULL) lstrcpyn(m_szAccounts[i],(pIClientUserItem->GetNickName()),CountArray(m_szAccounts[i]));
			}

			//旁观控制
			if ((IsLookonMode()==false)||(IsAllowLookon()==true))
			{
				m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(true);
			}

			//设置界面
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetDrawMaxScore(pStatusPlay->lDrawMaxScore);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				WORD wViewChairID=SwitchViewChairID(i);
				IClientUserItem * pIClientUserItem=GetTableUserItem(i);

				//设置积分
				if (pIClientUserItem!=NULL)
				{
					if (m_cbPlayStatus[i]==TRUE)
					{
						LONGLONG lUserScore=pIClientUserItem->GetUserScore();
						m_GameClientView.SetUserScore(wViewChairID,lUserScore - m_lUserScore[i] - m_lTableScore[i] - m_lServiceCharge);
					}
					else
					{
						m_GameClientView.SetUserScore(wViewChairID,pIClientUserItem->GetUserScore());
					}
				}

				//设置扑克
				if (m_lTableScore[i]!=0L)
				{
					if (m_cbPlayStatus[i]==FALSE)
					{
						BYTE cbCardData[MAX_COUNT];
						ZeroMemory(cbCardData,sizeof(cbCardData));
						m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,pStatusPlay->cbCardCount[i]);
					}
					else
					{
						BYTE cbCardCount=pStatusPlay->cbCardCount[i];
						m_GameClientView.m_CardControl[wViewChairID].SetCardData(pStatusPlay->cbHandCardData[i],cbCardCount);
					}
				}

				//筹码设置
				m_GameClientView.m_lPalyBetChip[wViewChairID] = m_lUserScore[i];
				m_GameClientView.m_lALLBetChip += m_lTableScore[i];
				m_GameClientView.SetUserTableScore(wViewChairID,m_lUserScore[i]+m_lTableScore[i]);
			}

			m_GameClientView.AddDeskChip( m_GameClientView.m_lALLBetChip, GAME_PLAYER );

			//当前用户
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID())) UpdateScoreControl();

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusPlay->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusPlay->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			//设置时间
			SetGameClock(m_wCurrentUser,IDI_ADD_SCORE,TIME_ADD_SCORE);

//#ifdef CHEAT_VERSION
			////获取信息
			//if( CUserRight::CanCheat(m_pIClientKernel->GetUserRight()) )
			//{
			//	//发送消息
			//	SendSocketData(SUB_C_GET_WINNER);
			//}
//#endif

			return true;
		}
	}

	return false;
}

//用户进入
VOID CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//设置成绩
	if (bLookonUser==false)
	{
		//历史成绩
		m_HistoryScore.OnEventUserEnter(pIClientUserItem->GetChairID());

		//设置积分
		WORD wViewChairID=SwitchViewChairID(pIClientUserItem->GetChairID());
		m_GameClientView.SetUserScore(wViewChairID,pIClientUserItem->GetUserScore());

		//更新界面
		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	return;
}

//用户离开
VOID CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//自动开始
	if (bLookonUser==false) PerformAutoStart();

	//设置成绩
	if (bLookonUser==false)
	{
		//设置变量
		m_HistoryScore.OnEventUserLeave(pIClientUserItem->GetChairID());

		//更新界面
		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	return;
}

//用户积分
VOID CGameClientEngine::OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//设置积分
	if (bLookonUser==false)
	{
		WORD wViewChairID=SwitchViewChairID(pIClientUserItem->GetChairID());
		m_GameClientView.SetUserSaveScore(wViewChairID,pIClientUserItem->GetUserScore());
	}

	return;
}

//用户状态
VOID CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//变量定义
	IClientUserItem * pIMySelfUserItem=GetMeUserItem();
	BYTE cbUserStatus=pIClientUserItem->GetUserStatus();

	//自动开始
	if ((pIClientUserItem!=pIMySelfUserItem)&&(cbUserStatus==US_READY)) PerformAutoStart();

	//停止开始
	if ((pIClientUserItem==pIMySelfUserItem)&&(cbUserStatus==US_READY)) KillGameClock(IDI_START_GAME);

	return;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	SetGameStatus(GAME_SCENE_PLAY);

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=pGameStart->wCurrentUser;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//设置变量
	m_lServiceCharge=pGameStart->lServiceCharge;
	m_lDrawMaxScore=pGameStart->lDrawMaxScore;
	m_lTurnMaxScore=pGameStart->lTurnMaxScore;
	m_lTurnLessScore=pGameStart->lTurnLessScore;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//变量定义
	BYTE cbCardData[2][GAME_PLAYER];
	WORD wStartChairID=INVALID_CHAIR;
	ZeroMemory(cbCardData,sizeof(cbCardData));

	//删除筹码
	for ( int i = 0 ; i < m_GameClientView.m_ArrayPlayChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayPlayChip[i]);
	m_GameClientView.m_ArrayPlayChip.RemoveAll();

	for ( int i = 0 ; i < m_GameClientView.m_ArrayBetChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayBetChip[i]);
	m_GameClientView.m_ArrayBetChip.RemoveAll();

	for ( int i = 0 ; i < m_GameClientView.m_ArrayOverChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayOverChip[i]);
	m_GameClientView.m_ArrayOverChip.RemoveAll();

	//设置界面
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetDrawMaxScore(m_lDrawMaxScore);
	m_GameClientView.SetCellScore(pGameStart->lCellScore);
	m_lCellScore = pGameStart->lCellScore;

	//设置界面
	for (WORD i = 0;i < GAME_PLAYER; i++)
	{
		m_GameClientView.SetOverScore(i, 0);
	}

	//设置界面
	m_GameClientView.m_lALLBetChip = 0l;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);

		//设置扑克
		if (pGameStart->cbCardData[i]!=0x00)
		{
			cbCardData[1][wViewChairID]=pGameStart->cbCardData[i];
			cbCardData[0][wViewChairID]=(wViewChairID==MYSELF_VIEW_ID)?pGameStart->cbObscureCard:0xFF;
		}

		//玩家设置
		if (pGameStart->cbCardData[i]!=0x00)
		{
			//发牌用户
			if (wStartChairID==INVALID_CHAIR) wStartChairID=wViewChairID;

			//状态变量
			m_cbPlayStatus[i]=TRUE;
			m_lTableScore[i]=pGameStart->lCellScore;
			lstrcpyn(m_szAccounts[i],(pIClientUserItem->GetNickName()),CountArray(m_szAccounts[i]));

			//设置筹码
			m_GameClientView.m_lPalyBetChip[wViewChairID] = 0l;
			m_GameClientView.m_lALLBetChip += pGameStart->lCellScore;
			m_GameClientView.SetUserTableScore(wViewChairID,pGameStart->lCellScore);

			//设置桌面筹码
			m_GameClientView.AddPlayChip( pGameStart->lCellScore, wViewChairID );

			//设置积分
			ASSERT(pIClientUserItem!=NULL);
			LONGLONG lUserScore=pIClientUserItem->GetUserScore();
			m_GameClientView.SetUserScore(wViewChairID,lUserScore - m_lTableScore[i] - m_lUserScore[1] - m_lServiceCharge);
		}
		else
		{
			//设置积分
			m_GameClientView.SetUserScore(wViewChairID,0L);

			//设置筹码
			m_GameClientView.m_lPalyBetChip[wViewChairID] = 0l;
		}

		//设置扑克
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetDisplayHead(false);
	}

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
		ActiveGameFrame();
		m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(true);
	}

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

#ifdef CHEAT_VERSION
	//获取信息
	if( CUserRight::CanCheat(m_pIClientKernel->GetUserRight()) )
	{
		//发送消息
		SendSocketData(SUB_C_GET_WINNER);
	}
#endif

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubGiveUp(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GiveUp));
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;

	//变量定义
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pData;

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	WORD wViewChairID=SwitchViewChairID(wGiveUpUser);

	//完成发牌
	m_GameClientView.ConcludeDispatch();

	//放弃处理
	if ((IsLookonMode()==true)||(wViewChairID!=MYSELF_VIEW_ID))
	{
		//设置动作
		m_GameClientView.SetUserAction(wViewChairID,AC_GIVE_UP);

		//变量定义
		BYTE cbCardData[MAX_COUNT];
		WORD wCardCount=m_GameClientView.m_CardControl[wViewChairID].GetCardCount();

		//设置扑克
		ZeroMemory(cbCardData,sizeof(cbCardData));
		m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(false);
		m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,wCardCount);

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

		//更新界面
		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	//下注变量
	m_lDrawMaxScore=pGiveUp->lDrawMaxScore;
	m_lTurnMaxScore=pGiveUp->lTrunMaxScore;

	//状态变量
	m_cbPlayStatus[wGiveUpUser]=FALSE;

	//设置界面
	m_GameClientView.SetDrawMaxScore(m_lDrawMaxScore);

	//设置用户
	m_wCurrentUser=pGiveUp->wCurrentUser;

	//加注设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID())) 
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	if (GetClockChairID()!=m_wCurrentUser) SetGameClock(m_wCurrentUser,IDI_ADD_SCORE,TIME_ADD_SCORE);

	return true;
}

//用户加注
bool CGameClientEngine::OnSubAddScore(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;

	//变量定义
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pData;

	//变量定义
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=SwitchViewChairID(wAddScoreUser);

	//完成发牌
	m_GameClientView.ConcludeDispatch();

	//加注处理
	if ((IsLookonMode()==true)||(wViewChairID!=MYSELF_VIEW_ID))
	{
		//变量定义
		LONGLONG lCurrentScore=pAddScore->lUserScoreCount;
		LONGLONG lDrawAddScore=m_lTableScore[wAddScoreUser];
		IClientUserItem * pIClientUserItem=GetTableUserItem(wAddScoreUser);

		//筹码设置
		m_lUserScore[wAddScoreUser]=lCurrentScore;
		m_GameClientView.AddPlayBetChip(m_lUserScore[wAddScoreUser], wViewChairID);
		m_GameClientView.SetUserTableScore(wViewChairID,m_lUserScore[wAddScoreUser]+m_lTableScore[wAddScoreUser]);

		//设置积分
		if (pIClientUserItem!=NULL)
		{
			LONGLONG lUserDrawScore=m_lUserScore[wAddScoreUser] + m_lTableScore[wAddScoreUser] + m_lServiceCharge;
			m_GameClientView.SetUserScore(wViewChairID,pIClientUserItem->GetUserScore()-lUserDrawScore);
		}

		//梭哈判断
		if ((lCurrentScore+lDrawAddScore)==m_lDrawMaxScore)
		{
			//设置动作
			m_bShowHand=true;
			m_GameClientView.SetUserAction(wViewChairID,AC_SHOW_HAND);

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
		}
		else
		{
			if (lCurrentScore==0L)
			{
				//不加动作
				PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
				m_GameClientView.SetUserAction(wViewChairID,AC_NO_ADD);
			}
			else if ((lCurrentScore+lDrawAddScore)==m_lTurnLessScore)
			{
				//跟注动作
				PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
				m_GameClientView.SetUserAction(wViewChairID,AC_FOLLOW);
			}
			else
			{
				//加注动作
				PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
				m_GameClientView.SetUserAction(wViewChairID,AC_ADD_SCORE);
			}
		}

		//更新界面
		m_GameClientView.InvalidGameView(0,0,0,0);
	}

	//设置变量
	m_wCurrentUser=pAddScore->wCurrentUser;
	m_lTurnLessScore=pAddScore->lTurnLessScore;

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	SetGameClock(m_wCurrentUser,IDI_ADD_SCORE,TIME_ADD_SCORE);

	return true;
}

//发送扑克
bool CGameClientEngine::OnSubSendCard(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pData;

	//设置变量
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_lTurnMaxScore=pSendCard->lTurnMaxScore;

	//汇集金币
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//桌面筹码
		m_lTableScore[i]+=m_lUserScore[i];
		m_GameClientView.m_lALLBetChip += m_lUserScore[i];
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[i]);

		// 桌面筹码
		m_GameClientView.AddDeskChip(m_lUserScore[i], wViewChairID);

		//用户筹码
		m_lUserScore[i]=0L;
		m_GameClientView.AddPlayBetChip(m_lUserScore[i], wViewChairID);
	}

	//派发扑克
	for (BYTE i=0;i<pSendCard->cbSendCardCount;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			WORD wChairId = (pSendCard->wStartChairID+j)%GAME_PLAYER;
			if (m_cbPlayStatus[wChairId]==TRUE)
			{
				WORD wViewChairID=SwitchViewChairID(wChairId);
				m_GameClientView.DispatchUserCard(wViewChairID,pSendCard->cbCardData[i][wChairId]);
			}
		}
	}

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//变量定义
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//结束设置
	KillGameClock(IDI_ADD_SCORE);
	SetGameStatus(GAME_SCENE_FREE);

	//界面设置
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);

	//保存数据
	CopyMemory(&m_GameEndPacket,pGameEnd,sizeof(CMD_S_GameEnd));

	//结束游戏
	PerformGameConclude();

	return true;
}

//自动开始
bool CGameClientEngine::PerformAutoStart()
{
	//变量定义
	WORD wTableUserCount=0,wReadyUserCount=0;
	IClientUserItem * pIMySelfUserItem=GetMeUserItem();

	//状态判断
	if ((IsLookonMode()==true)||(m_pIClientKernel->GetClockID()==IDI_START_GAME)) return false;
	if ((pIMySelfUserItem==NULL)||(pIMySelfUserItem->GetUserStatus()==US_READY)) return false;

	//防作弊模式
	if ( CServerRule::IsAllowAvertCheatMode(m_pIClientKernel->GetServerAttribute()->dwServerRule) )
	{
		//开始设置
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,TIME_START_GAME);
		return true;
	}

	//统计用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetTableUserItem(i);

		//数目统计
		if (pIClientUserItem!=NULL) 
		{
			wTableUserCount++;
			if (pIClientUserItem->GetUserStatus()==US_READY) wReadyUserCount++;
		}
	}

	//开始判断
	if ((wTableUserCount>=2)&&((wReadyUserCount+1)==wTableUserCount))
	{
		//开始设置
		WORD wMeChairID=GetMeChairID();
		SetGameClock(wMeChairID,IDI_START_GAME,TIME_START_GAME);

		return true;
	}

	return false;
}

//隐藏控制
VOID CGameClientEngine::HideScoreControl()
{
	//隐藏控件
	m_GameClientView.m_PlayOperate.ShowOperate(false);

	//禁用控件
	m_GameClientView.m_PlayOperate.m_btFollow.EnableWindow(FALSE);
	m_GameClientView.m_PlayOperate.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_PlayOperate.m_btShowHand.EnableWindow(FALSE);

	m_GameClientView.InvalidGameView(0,0,0,0);

	return;
}

//更新控制
VOID CGameClientEngine::UpdateScoreControl()
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	LONGLONG lDrawUserScore=m_lTableScore[wMeChairID]+m_lUserScore[wMeChairID];

	//当前用户
	if ((m_wCurrentUser==wMeChairID)&&(m_cbPlayStatus[wMeChairID]==TRUE))
	{
		// 最小下注
		LONGLONG lMinBet = (m_lTurnLessScore==lDrawUserScore)?m_lCellScore:0;

		//显示按钮
		m_GameClientView.m_PlayOperate.ShowOperate(true);
		m_GameClientView.m_PlayOperate.SetBetCount( m_lTurnMaxScore - m_lTurnLessScore, lMinBet );

		m_GameClientView.m_PlayOperate.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_PlayOperate.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_PlayOperate.m_btAddScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_PlayOperate.m_btShowHand.ShowWindow(SW_SHOW);

		//标志变量
		bool bEnableShowHand=(m_lDrawMaxScore==m_lTurnMaxScore);
		UINT uBtImageID=(m_lTurnLessScore==lDrawUserScore)?IDB_BT_CONTROL_NOADD:IDB_BT_CONTROL_FOLLOW;

		//控制按钮
		m_GameClientView.m_PlayOperate.m_btFollow.EnableWindow((m_lTurnMaxScore>=m_lTurnLessScore)?TRUE:FALSE);
		m_GameClientView.m_PlayOperate.m_btShowHand.EnableWindow((bEnableShowHand==true)?TRUE:FALSE);
		m_GameClientView.m_PlayOperate.m_btAddScore.EnableWindow((m_lTurnMaxScore>=m_lTurnLessScore)?TRUE:FALSE);
		m_GameClientView.m_PlayOperate.m_btFollow.SetButtonImage(uBtImageID,AfxGetInstanceHandle(),false,false);
	}
	else
	{
		//隐藏控件
		m_GameClientView.m_PlayOperate.ShowOperate(false);

		//禁用控件
		m_GameClientView.m_PlayOperate.m_btFollow.EnableWindow(FALSE);
		m_GameClientView.m_PlayOperate.m_btAddScore.EnableWindow(FALSE);
		m_GameClientView.m_PlayOperate.m_btShowHand.EnableWindow(FALSE);
	}

	m_GameClientView.InvalidGameView(0,0,0,0);

	return;
}

//执行结束
VOID CGameClientEngine::PerformGameConclude()
{
	//隐藏控件
	HideScoreControl();

	//用户成绩
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//空位用户
		if (m_GameEndPacket.lGameScore[i]==0L) continue;

		//设置成绩
		m_HistoryScore.OnEventUserScore(i,m_GameEndPacket.lGameScore[i]);
	}

	//成绩界面
	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		m_GameClientView.SetOverScore(SwitchViewChairID(i), m_GameEndPacket.lGameScore[i]);
	}

	// 回收动画
	LONGLONG lMaxScore = 0l;
	WORD     wWinSite = INVALID_CHAIR;
	for ( int i = 0; i < GAME_PLAYER; ++i )
	{
		if (  m_GameEndPacket.lGameScore[i] > lMaxScore )
		{
			lMaxScore = m_GameEndPacket.lGameScore[i];
			wWinSite = SwitchViewChairID(i);
		}
	}

	if( wWinSite != INVALID_CHAIR )
	{
		m_GameClientView.ChioRecovery(wWinSite);
	}
	

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (m_GameEndPacket.lGameScore[i]!=0L)
		{
			//过虑判断
			if (m_GameEndPacket.cbCardData[i]==0x00) continue;

			//获取扑克
			BYTE cbCardData[MAX_CHAIR];
			WORD wViewChairID=SwitchViewChairID(i);
			WORD wCardCount=m_GameClientView.m_CardControl[wViewChairID].GetCardData(cbCardData,CountArray(cbCardData));

			//设置扑克
			cbCardData[0]=m_GameEndPacket.cbCardData[i];
			m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(true);
			m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,wCardCount);
		}
	}

	//设置控件
	m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(false);

	//播放声音
	LONGLONG lMeScore=m_GameEndPacket.lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//开始设置
	if ((IsLookonMode()==false)&&(GetMeChairID()!=INVALID_CHAIR))
	{
		PerformAutoStart();
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	}

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return;
}

//开始消息
LRESULT CGameClientEngine::OnMessageStart(WPARAM wParam, LPARAM lParam)
{
	//加注信息
	m_lDrawMaxScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//删除时间
	KillGameClock(IDI_START_GAME);

	//完成发牌
	m_GameClientView.ConcludeDispatch();

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_lPalyBetChip[i] = 0l;
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.SetOverScore(i, 0);
	}
	m_GameClientView.m_lALLBetChip = 0l;

	//删除筹码
	for ( int i = 0 ; i < m_GameClientView.m_ArrayPlayChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayPlayChip[i]);
	m_GameClientView.m_ArrayPlayChip.RemoveAll();

	for ( int i = 0 ; i < m_GameClientView.m_ArrayBetChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayBetChip[i]);
	m_GameClientView.m_ArrayBetChip.RemoveAll();

	for ( int i = 0 ; i < m_GameClientView.m_ArrayOverChip.GetCount(); ++i )
		SafeDelete(m_GameClientView.m_ArrayOverChip[i]);
	m_GameClientView.m_ArrayOverChip.RemoveAll();

	//界面设置
	m_GameClientView.SetDrawMaxScore(0L);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);

	//设置控件
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	//发送消息
	m_pIClientKernel->SendUserReady(NULL,0);

	return 0;
}

//跟注消息
LRESULT CGameClientEngine::OnMessageFollow(WPARAM wParam, LPARAM lParam)
{
	//隐藏控件
	HideScoreControl();

	//删除时间
	KillGameClock(IDI_ADD_SCORE);


	//设置控件
	WORD wMeChairID=GetMeChairID();
	LONGLONG lScoreLess = m_lTurnLessScore - ( m_lTableScore[wMeChairID] + m_lUserScore[wMeChairID] );
	LONGLONG lXXParam = wParam + (LONGLONG)lScoreLess;

	//金币计算
	LONGLONG lDrawAddScore=m_lTableScore[wMeChairID]+m_lUserScore[wMeChairID];
	LONGLONG lCurrentAddScore=__max(m_lTurnLessScore-lDrawAddScore,lXXParam);

	//筹码设置
	m_lUserScore[wMeChairID]+=lCurrentAddScore;
	m_GameClientView.AddPlayBetChip(m_lUserScore[wMeChairID], MYSELF_VIEW_ID);
	m_GameClientView.SetUserTableScore(MYSELF_VIEW_ID,m_lUserScore[wMeChairID]+m_lTableScore[wMeChairID]);

	//设置积分
	IClientUserItem * pIClientUserItem=GetMeUserItem();
	LONGLONG lUserItemScore=pIClientUserItem->GetUserScore();
	m_GameClientView.SetUserScore(MYSELF_VIEW_ID,lUserItemScore-m_lUserScore[wMeChairID]-m_lTableScore[wMeChairID] - m_lServiceCharge);

	//梭哈判断
	if ((m_bShowHand==false)&&((lCurrentAddScore+lDrawAddScore)==m_lDrawMaxScore))
	{
		//设置动作
		m_bShowHand=true;
		m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_SHOW_HAND);

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
	}
	else
	{
		if (lCurrentAddScore==0L)
		{
			//不加动作
			PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
			m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_NO_ADD);
		}
		else if ((lCurrentAddScore+lDrawAddScore)==m_lTurnLessScore)
		{
			//跟注动作
			PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
			m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_FOLLOW);
		}
		else
		{
			//加注动作
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
			m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_ADD_SCORE);
		}
	}

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentAddScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return 0;
}

//放弃消息		 
LRESULT CGameClientEngine::OnMessageGiveUp(WPARAM wParam, LPARAM lParam)
{
	//隐藏控件
	HideScoreControl();

	//界面设置
	KillGameClock(IDI_ADD_SCORE);

	//设置动作
	m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_GIVE_UP);

	//变量定义
	BYTE cbCardData[MAX_COUNT];
	WORD wCardCount=m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardCount();

	//设置扑克
	ZeroMemory(cbCardData,sizeof(cbCardData));
	m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetCardData(cbCardData,wCardCount);

	//设置控件
	m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively(false);
	m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetDisplayHead(false);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//发送消息
	SendSocketData(SUB_C_GIVE_UP);

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return 0;
}

//加注消息		 
LRESULT CGameClientEngine::OnMessageAddScore(WPARAM wParam, LPARAM lParam)
{
	//设置按钮
	m_GameClientView.m_PlayOperate.m_btFollow.EnableWindow(FALSE);
	m_GameClientView.m_PlayOperate.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_PlayOperate.m_btShowHand.EnableWindow(FALSE);

	return 0;
}

//梭哈消息
LRESULT CGameClientEngine::OnMessageShowHand(WPARAM wParam, LPARAM lParam)
{
	//隐藏控件
	HideScoreControl();

	//界面设置
	KillGameClock(IDI_ADD_SCORE);

	//设置动作
	m_bShowHand=true;
	m_GameClientView.SetUserAction(MYSELF_VIEW_ID,AC_SHOW_HAND);

	//金币计算
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentAddScore=m_lDrawMaxScore-m_lTableScore[wMeChairID]-m_lUserScore[wMeChairID];

	//筹码设置
	m_lUserScore[wMeChairID]+=lCurrentAddScore;
	m_GameClientView.AddPlayBetChip(m_lUserScore[wMeChairID], MYSELF_VIEW_ID);
	m_GameClientView.SetUserTableScore(MYSELF_VIEW_ID,m_lUserScore[wMeChairID]+m_lTableScore[wMeChairID]);

	//设置积分
	IClientUserItem * pIClientUserItem=GetMeUserItem();
	LONGLONG lUserItemScore=pIClientUserItem->GetUserScore();
	m_GameClientView.SetUserScore(MYSELF_VIEW_ID,lUserItemScore-m_lUserScore[wMeChairID]-m_lTableScore[wMeChairID]-m_lServiceCharge);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentAddScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnMessageSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//设置界面
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);

	//设置控制
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	SetGameClock(m_wCurrentUser,IDI_ADD_SCORE,TIME_ADD_SCORE);

	return 0;
}

//获取胜者
bool CGameClientEngine::OnSubGetWinner( VOID * pData, WORD wDataSize )
{
	return true;


	//变量定义
	CMD_S_GetWinner *pGetWinner = (CMD_S_GetWinner *)pData;

	//效验数据
	ASSERT( wDataSize >= sizeof(pGetWinner->wOrderCount) );
	ASSERT( wDataSize == sizeof(CMD_S_GetWinner)-sizeof(pGetWinner->wChairOrder)+
		sizeof(WORD)*pGetWinner->wOrderCount );
	if (wDataSize != sizeof(CMD_S_GetWinner)-sizeof(pGetWinner->wChairOrder)+
		sizeof(WORD)*pGetWinner->wOrderCount) 
		return false;

	CString strMessage = TEXT("本局玩家大小名次是：");

	//设置扑克
	for( WORD i = 0; i < pGetWinner->wOrderCount; i++ )
	{
		if( m_cbPlayStatus[pGetWinner->wChairOrder[i]] && m_szAccounts[pGetWinner->wChairOrder[i]][0] != 0 )
		{
			if( i > 0 ) strMessage += TEXT("、");

			strMessage += m_szAccounts[pGetWinner->wChairOrder[i]];
		}
	}

	if ( m_pIStringMessage )
		m_pIStringMessage->InsertSystemString(strMessage);

	return true;
}

//////////////////////////////////////////////////////////////////////////
