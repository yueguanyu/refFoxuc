#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "DialogMessage.h"
#include ".\gameclientdlg.h"

//////////////////////////////////////////////////////////////////////////
//定时标识
#define IDI_FREE					100									//休息时间
#define IDI_BET_START				101									//开始下注
#define IDI_BET_END					102									//下注结束
#define IDI_HORSES_START			103									//跑马开始

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_PLAYER_BET,OnPlayerBet)
	ON_MESSAGE(IDM_ADMIN_COMMDN, OnAdminControl)	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{

	//区域倍数
	for ( int i = 0; i < CountArray(m_nMultiple); ++i)
		m_nMultiple[i] = 1;

	//游戏分数
	memset(m_lPlayerBet, 0, sizeof(m_lPlayerBet));
	memset(m_lPlayerBetAll, 0, sizeof(m_lPlayerBetAll));

	//限制变量
	m_lAreaLimitScore = 0l;
	m_lUserLimitScore = 0l;

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
	//百人跑马
	SetWindowText(TEXT("百人跑马  --  Ver：6.6.1.0"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	return true;
}

//声音控制
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	if(bAllowSound)
		PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));
	else
		StopSound();

	return true;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	switch(nTimerID)
	{
	case IDI_FREE:			//游戏空闲
		{
			if( nElapse == 0 && wChairID == GetMeChairID() )
			{
				m_GameClientView.m_DlgPlayBet.SetCanBet(false);
				return false;
			}
		}
		return true;
	case IDI_BET_START:		//下注开始
		{
			if( nElapse == 0 && wChairID == GetMeChairID() )
			{
				m_GameClientView.m_DlgPlayBet.SetCanBet(false);
				return false;
			}
		}
		return true;
	case IDI_BET_END:		//下注结束
		{
			if( nElapse == 0 && wChairID == GetMeChairID() )
			{
				m_GameClientView.m_DlgPlayBet.SetCanBet(false);
				return false;
			}
		}
		return true;
	case IDI_HORSES_START:	//跑马开始
		{
			if( nElapse == 0 && wChairID == GetMeChairID() )
			{
				m_GameClientView.m_DlgPlayBet.SetCanBet(false);
				return false;
			}
		}
		return true;
	}
	return true;
}

//旁观状态
bool CGameClientEngine:: OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}


//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_S_BET_START:			//开始下注
		return OnSubBetStart(pData, wDataSize);

	case SUB_S_BET_END:				//下注结束
		return OnSubBetEnd(pData, wDataSize);

	case SUB_S_HORSES_START:		//跑马开始
		return OnSubHorsesStart(pData, wDataSize);

	case SUB_S_HORSES_END:			//跑马结束
		return OnSubHorsesEnd(pData, wDataSize);

	case SUB_S_PLAYER_BET:			//用户下注
		return OnSubPlayerBet(pData, wDataSize);

	case SUB_S_PLAYER_BET_FAIL:		//下注失败
		return OnSubPlayerBetFail(pData, wDataSize);

	case SUB_S_NAMED_HORSES:		//马屁冠名
		return true;

	case SUB_S_MANDATOY_END:		//强制结束
		return OnSubMandatoryEnd(pData, wDataSize);

	case SUB_S_ADMIN_COMMDN:		//设置
		return OnSubAdminControl(pData,wDataSize);

	}
	//错误断言
	ASSERT(FALSE);
	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStation, bool bLookonUser, VOID * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneFree));
			if (wDataSize!=sizeof(CMD_S_SceneFree)) return false;

			//消息处理
			CMD_S_SceneFree * pSceneFree = (CMD_S_SceneFree *)pBuffer;

			const tagServerAttribute * pServerAttribute = (m_pIClientKernel!=NULL)?m_pIClientKernel->GetServerAttribute():NULL;
			if ( pServerAttribute != NULL )
				m_GameClientView.SetKindID(pServerAttribute->wKindID);


			//设置状态
			SetGameStatus(GS_FREE);
			m_GameClientView.SetGameStatus(GS_FREE);
			m_GameClientView.m_DlgPlayBet.SetCanBet(false);

			//设置位置
			WORD wMeChairID = GetMeChairID();
			m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));

			//设置时间
			SetGameClock(GetMeChairID(), IDI_FREE, pSceneFree->nTimeLeave);

			//设置场次
			m_GameClientView.SetStreak(pSceneFree->nStreak);

			//设置倍数
			memcpy(m_nMultiple, pSceneFree->nMultiple, sizeof(m_nMultiple));
			m_GameClientView.m_DlgPlayBet.SetMultiple(m_nMultiple);

			//设置名字
			for (int i = 0; i < HORSES_ALL; ++i)
				m_GameClientView.SetHorsesName(i, pSceneFree->szHorsesName[i]);

			//历史记录
			m_GameClientView.m_DlgRecord.m_GameRecords.RemoveAll();
			for ( int i = 0; i < MAX_SCORE_HISTORY; ++i )
			{
				if ( pSceneFree->GameRecords[i].nStreak != 0)
				{
					m_GameClientView.m_DlgRecord.m_GameRecords.Add(pSceneFree->GameRecords[i]);
				}
			}

			//全天赢的场次
			m_GameClientView.m_DlgStatistics.SetWinCount(pSceneFree->nWinCount);

			//限制变量
			m_lAreaLimitScore = pSceneFree->lAreaLimitScore;						//区域总限制
			m_lUserLimitScore = pSceneFree->lUserLimitScore;						//个人区域限制

			//更新马匹
			m_GameClientView.NweHorses();

			//游戏分数
			memset(m_lPlayerBet, 0, sizeof(m_lPlayerBet));
			memset(m_lPlayerBetAll, 0, sizeof(m_lPlayerBetAll));
			
			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.GetSafeHwnd())
				m_GameClientView.m_btOpenControl.ShowWindow(SW_SHOW);

			//更新控制
			UpdateControls();
			return true;
		}
	case GS_BET:	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneBet));
			if (wDataSize!=sizeof(CMD_S_SceneBet)) return false;

			//消息处理
			CMD_S_SceneBet * pSceneBet = (CMD_S_SceneBet *)pBuffer;

			//设置ID
			const tagServerAttribute * pServerAttribute = (m_pIClientKernel!=NULL)?m_pIClientKernel->GetServerAttribute():NULL;
			if ( pServerAttribute != NULL )
				m_GameClientView.SetKindID(pServerAttribute->wKindID);

			
			//设置状态
			SetGameStatus(GS_BET);
			m_GameClientView.SetGameStatus(GS_BET);
			m_GameClientView.m_DlgPlayBet.SetCanBet(true);

			//设置位置
			WORD wMeChairID = GetMeChairID();
			WORD wMeViewChairID = SwitchViewChairID(wMeChairID);
			m_GameClientView.SetMeChairID(wMeViewChairID);

			//设置时间
			SetGameClock(GetMeChairID(), IDI_BET_START, pSceneBet->nTimeLeave);

			//设置场次
			m_GameClientView.SetStreak(pSceneBet->nStreak);

			//设置倍数
			memcpy(m_nMultiple, pSceneBet->nMultiple, sizeof(m_nMultiple));
			m_GameClientView.m_DlgPlayBet.SetMultiple(m_nMultiple);

			//设置名字
			for (int i = 0; i < HORSES_ALL; ++i)
				m_GameClientView.SetHorsesName(i, pSceneBet->szHorsesName[i]);

			//历史记录
			m_GameClientView.m_DlgRecord.m_GameRecords.RemoveAll();
			for ( int i = 0; i < MAX_SCORE_HISTORY; ++i )
			{
				if ( pSceneBet->GameRecords[i].nStreak != 0)
				{
					m_GameClientView.m_DlgRecord.m_GameRecords.Add(pSceneBet->GameRecords[i]);
				}
			}

			//全天赢的场次
			m_GameClientView.m_DlgStatistics.SetWinCount(pSceneBet->nWinCount);

			//设置下注人数
			m_GameClientView.SetBetPlayerCount(pSceneBet->nBetPlayerCount);

			//个人最大下注
			pSceneBet->lUserMaxScore;

			//本人下注
			memcpy(m_lPlayerBet, pSceneBet->lPlayerBet, sizeof(m_lPlayerBet));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetPlayerBet(wMeViewChairID, i, m_lPlayerBet[i]);

			//所有下注
			memcpy(m_lPlayerBetAll, pSceneBet->lPlayerBetAll, sizeof(m_lPlayerBetAll));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetAllBet(i, m_lPlayerBetAll[i]);

			//限制变量
			m_lAreaLimitScore = pSceneBet->lAreaLimitScore;						//区域总限制
			m_lUserLimitScore = pSceneBet->lUserLimitScore;						//个人区域限制

			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.GetSafeHwnd())
				m_GameClientView.m_btOpenControl.ShowWindow(SW_SHOW);

			//更新控制
			UpdateControls();
			return true;
		}
	case GS_BET_END:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneBetEnd));
			if (wDataSize!=sizeof(CMD_S_SceneBetEnd)) return false;

			//消息处理
			CMD_S_SceneBetEnd * pSceneBetEnd = (CMD_S_SceneBetEnd *)pBuffer;

			const tagServerAttribute * pServerAttribute = (m_pIClientKernel!=NULL)?m_pIClientKernel->GetServerAttribute():NULL;
			if ( pServerAttribute != NULL )
				m_GameClientView.SetKindID(pServerAttribute->wKindID);


			//设置状态
			SetGameStatus(GS_BET_END);
			m_GameClientView.SetGameStatus(GS_BET_END);
			m_GameClientView.m_DlgPlayBet.SetCanBet(false);

			//设置位置
			WORD wMeChairID = GetMeChairID();
			WORD wMeViewChairID = SwitchViewChairID(wMeChairID);
			m_GameClientView.SetMeChairID(wMeViewChairID);

			//设置时间
			SetGameClock(GetMeChairID(), IDI_BET_START, pSceneBetEnd->nTimeLeave);

			//设置场次
			m_GameClientView.SetStreak(pSceneBetEnd->nStreak);

			//设置倍数
			memcpy(m_nMultiple, pSceneBetEnd->nMultiple, sizeof(m_nMultiple));
			m_GameClientView.m_DlgPlayBet.SetMultiple(m_nMultiple);

			//设置名字
			for (int i = 0; i < HORSES_ALL; ++i)
				m_GameClientView.SetHorsesName(i, pSceneBetEnd->szHorsesName[i]);

			//历史记录
			m_GameClientView.m_DlgRecord.m_GameRecords.RemoveAll();
			for ( int i = 0; i < MAX_SCORE_HISTORY; ++i )
			{
				if ( pSceneBetEnd->GameRecords[i].nStreak != 0)
				{
					m_GameClientView.m_DlgRecord.m_GameRecords.Add(pSceneBetEnd->GameRecords[i]);
				}
			}

			//全天赢的场次
			m_GameClientView.m_DlgStatistics.SetWinCount(pSceneBetEnd->nWinCount);

			//设置下注人数
			m_GameClientView.SetBetPlayerCount(pSceneBetEnd->nBetPlayerCount);

			//本人下注
			memcpy(m_lPlayerBet, pSceneBetEnd->lPlayerBet, sizeof(m_lPlayerBet));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetPlayerBet(wMeViewChairID, i, m_lPlayerBet[i]);

			//所有下注
			memcpy(m_lPlayerBetAll, pSceneBetEnd->lPlayerBetAll, sizeof(m_lPlayerBetAll));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetAllBet(i, m_lPlayerBetAll[i]);

			//限制变量
			m_lAreaLimitScore = pSceneBetEnd->lAreaLimitScore;						//区域总限制
			m_lUserLimitScore = pSceneBetEnd->lUserLimitScore;						//个人区域限制

			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.GetSafeHwnd())
				m_GameClientView.m_btOpenControl.ShowWindow(SW_SHOW);

			//更新控制
			UpdateControls();
			return true;
		}
	case GS_HORSES:			
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneHorses));
			if (wDataSize!=sizeof(CMD_S_SceneHorses)) return false;

			//消息处理
			CMD_S_SceneHorses * pSceneHorses = (CMD_S_SceneHorses *)pBuffer;

			const tagServerAttribute * pServerAttribute = (m_pIClientKernel!=NULL)?m_pIClientKernel->GetServerAttribute():NULL;
			if ( pServerAttribute != NULL )
				m_GameClientView.SetKindID(pServerAttribute->wKindID);

			//设置状态
			SetGameStatus(GS_HORSES);
			m_GameClientView.SetGameStatus(GS_HORSES);
			m_GameClientView.m_DlgPlayBet.SetCanBet(false);

			//设置位置
			WORD wMeChairID = GetMeChairID();
			WORD wMeViewChairID = SwitchViewChairID(wMeChairID);
			m_GameClientView.SetMeChairID(wMeViewChairID);

			//设置时间
			SetGameClock(GetMeChairID(), IDI_HORSES_START, pSceneHorses->nTimeLeave);

			//设置场次
			m_GameClientView.SetStreak(pSceneHorses->nStreak);

			//设置倍数
			memcpy(m_nMultiple, pSceneHorses->nMultiple, sizeof(m_nMultiple));
			m_GameClientView.m_DlgPlayBet.SetMultiple(m_nMultiple);

			//设置名字
			for (int i = 0; i < HORSES_ALL; ++i)
				m_GameClientView.SetHorsesName(i, pSceneHorses->szHorsesName[i]);

			//历史记录
			m_GameClientView.m_DlgRecord.m_GameRecords.RemoveAll();
			for ( int i = 0; i < MAX_SCORE_HISTORY; ++i )
			{
				if ( pSceneHorses->GameRecords[i].nStreak != 0)
				{
					m_GameClientView.m_DlgRecord.m_GameRecords.Add(pSceneHorses->GameRecords[i]);
				}
			}

			//全天赢的场次
			m_GameClientView.m_DlgStatistics.SetWinCount(pSceneHorses->nWinCount);

			//设置下注人数
			m_GameClientView.SetBetPlayerCount(pSceneHorses->nBetPlayerCount);

			//本人下注
			memcpy(m_lPlayerBet, pSceneHorses->lPlayerBet, sizeof(m_lPlayerBet));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetPlayerBet(wMeViewChairID, i, m_lPlayerBet[i]);

			//所有下注
			memcpy(m_lPlayerBetAll, pSceneHorses->lPlayerBetAll, sizeof(m_lPlayerBetAll));
			for ( int i = 0; i < AREA_ALL; ++i)
				m_GameClientView.SetAllBet(i, m_lPlayerBetAll[i]);

			//设置玩家输赢
			m_GameClientView.SetPlayerWinning(pSceneHorses->lPlayerWinning);

			//限制变量
			m_lAreaLimitScore = pSceneHorses->lAreaLimitScore;						//区域总限制
			m_lUserLimitScore = pSceneHorses->lUserLimitScore;						//个人区域限制

			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.GetSafeHwnd())
				m_GameClientView.m_btOpenControl.ShowWindow(SW_SHOW);

			//更新控制
			UpdateControls();
			return true;
		}
	}
	return false;
}


//更新控件
void CGameClientEngine::UpdateControls()
{

}

//加注消息
LRESULT CGameClientEngine::OnPlayerBet( WPARAM wParam, LPARAM lParam )
{
	//定义变量
	CMD_C_PlayerBet* pPlayerBet = (CMD_C_PlayerBet*)lParam;

	//验证下注
	for ( int i = 0 ; i < AREA_ALL ; ++i)
	{
		if ( pPlayerBet->lBetScore[i] > GetMeMaxBet(i) )
		{
			//下注失败消息
			CDialogMessage Message;
			Message.SetMessage(TEXT("下注金额超过系统限制金额，请重新下注！"));
			Message.DoModal();
			return 0;
		}
	}
	
	//发送消息
	SendSocketData( SUB_C_PLAYER_BET, pPlayerBet, sizeof(CMD_C_PlayerBet));

	//设置下注
	WORD wMeChairId = GetMeChairID();
	WORD wViewChairID = SwitchViewChairID(wMeChairId);
	for( int i = 0 ; i < AREA_ALL ; ++i )
	{
		m_GameClientView.SetPlayerBet(wViewChairID, i, pPlayerBet->lBetScore[i]);
		m_lPlayerBet[i] += pPlayerBet->lBetScore[i];
		m_lPlayerBetAll[i] += pPlayerBet->lBetScore[i];
	}
	return 0;
}
//控制
LRESULT CGameClientEngine::OnAdminControl( WPARAM wParam, LPARAM lParam )
{
	if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.GetSafeHwnd())
		SendSocketData(SUB_C_ADMIN_COMMDN,(CMD_C_ControlApplication*)wParam,sizeof(CMD_C_ControlApplication));

	return true;
}


//下注开始
bool CGameClientEngine::OnSubBetStart( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BetStart));
	if (wDataSize!=sizeof(CMD_S_BetStart)) return false;

	if (m_GameClientView.m_DlgControl.GetSafeHwnd())
	{
		m_GameClientView.m_DlgControl.ResetUserBet();
	}

	//消息处理
	CMD_S_BetStart * pBetStart = (CMD_S_BetStart *)pBuffer;

	//设置时间
	SetGameClock(GetMeChairID(), IDI_BET_START, pBetStart->nTimeLeave);
	m_GameClientView.SetBetEndTime(pBetStart->nTimeBetEnd);

	//设置状态
	SetGameStatus(GS_BET);
	m_GameClientView.SetGameStatus(GS_BET);
	m_GameClientView.m_DlgPlayBet.SetCanBet(true);

	//设置位置
	m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));
	return true;
}

//下注结束
bool CGameClientEngine::OnSubBetEnd( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BetEnd));
	if (wDataSize!=sizeof(CMD_S_BetEnd)) return false;

	//消息处理
	CMD_S_BetEnd * pBetEnd = (CMD_S_BetEnd *)pBuffer;

	//设置时间
	SetGameClock(GetMeChairID(), IDI_BET_END, pBetEnd->nTimeLeave);

	//设置状态
	SetGameStatus(GS_BET_END);
	m_GameClientView.SetGameStatus(GS_BET_END);
	m_GameClientView.m_DlgPlayBet.SetCanBet(false);
	return true;
}

//跑马开始
bool CGameClientEngine::OnSubHorsesStart( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_HorsesStart));
	if (wDataSize!=sizeof(CMD_S_HorsesStart)) return false;

	//消息处理
	CMD_S_HorsesStart * pHorsesStart = (CMD_S_HorsesStart *)pBuffer;

	//设置时间
	SetGameClock(GetMeChairID(), IDI_HORSES_START, pHorsesStart->nTimeLeave);

	//设置状态
	SetGameStatus(GS_HORSES);
	m_GameClientView.SetGameStatus(GS_HORSES);
	m_GameClientView.m_DlgPlayBet.SetCanBet(false);

	//设置名次
	m_GameClientView.SetRanking(pHorsesStart->cbHorsesRanking);

	//设置输赢
	m_GameClientView.SetPlayerWinning(pHorsesStart->lPlayerWinning + pHorsesStart->lPlayerReturnBet);

	//开始动画
	m_GameClientView.HorsesStart(pHorsesStart->nHorsesSpeed);
	return true;
}

//跑马结束
bool CGameClientEngine::OnSubHorsesEnd( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_HorsesEnd));
	if (wDataSize!=sizeof(CMD_S_HorsesEnd)) return false;

	//消息处理
	CMD_S_HorsesEnd * pHorsesEnd = (CMD_S_HorsesEnd *)pBuffer;

	//设置时间
	SetGameClock(GetMeChairID(), IDI_FREE, pHorsesEnd->nTimeLeave);

	//保存记录
	m_GameClientView.m_DlgRecord.m_GameRecords.Add(pHorsesEnd->RecordRecord);
	if ( m_GameClientView.m_DlgRecord.m_GameRecords.GetCount() > MAX_SCORE_HISTORY )
	{
		m_GameClientView.m_DlgRecord.m_GameRecords.RemoveAt(0);
	}
	if ( m_GameClientView.m_DlgRecord.GetSafeHwnd() && m_GameClientView.m_DlgRecord.IsWindowVisible())
	{
		m_GameClientView.m_DlgRecord.Invalidate(FALSE);
	}
	

	//保存下注记录
	for ( int i = 0 ; i < AREA_ALL; ++i )
	{
		if ( pHorsesEnd->lPlayerBet[i] > 0 )
		{
			BetRecordInfo Info;
			Info.nStreak = pHorsesEnd->RecordRecord.nStreak;
			Info.nHours = pHorsesEnd->RecordRecord.nHours;
			Info.nMinutes = pHorsesEnd->RecordRecord.nMinutes;
			Info.nRanking = i;
			Info.lBet = pHorsesEnd->lPlayerBet[i];
			Info.lWin = pHorsesEnd->lPlayerWinning[i];
			m_GameClientView.m_DlgBetRecord.AddInfo(&Info);
		}
	}

	//设置场次
	m_GameClientView.SetStreak(pHorsesEnd->RecordRecord.nStreak);

	//全天赢的场次
	m_GameClientView.m_DlgStatistics.SetWinCount(pHorsesEnd->nWinCount);

	//保存倍数
	memcpy(m_nMultiple, pHorsesEnd->nMultiple, sizeof(m_nMultiple));
	m_GameClientView.m_DlgPlayBet.SetMultiple(m_nMultiple);

	//设置状态
	SetGameStatus(GS_FREE);
	m_GameClientView.SetGameStatus(GS_FREE);
	m_GameClientView.m_DlgPlayBet.SetCanBet(false);

	//更新马匹
	m_GameClientView.NweHorses();

	//游戏分数
	memset(m_lPlayerBet, 0, sizeof(m_lPlayerBet));
	memset(m_lPlayerBetAll, 0, sizeof(m_lPlayerBetAll));

	return true;
}

//玩家下注
bool CGameClientEngine::OnSubPlayerBet(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayerBet));
	if (wDataSize!=sizeof(CMD_S_PlayerBet)) return false;

	//消息处理
	CMD_S_PlayerBet * pPlayerBet = (CMD_S_PlayerBet *)pBuffer;

	if (pPlayerBet->bIsAndroid==false )
	{
		m_GameClientView.m_DlgControl.SetUserBetScore(pPlayerBet->lBetScore);
	}
	//设置下注人数
	m_GameClientView.SetBetPlayerCount(pPlayerBet->nBetPlayerCount);

	//添加注数
	if ( pPlayerBet->wChairID != GetMeChairID() )
	{
		WORD wViewChairID = SwitchViewChairID(pPlayerBet->wChairID);

		for( int i = 0 ; i < AREA_ALL ; ++i )
		{
			m_GameClientView.SetPlayerBet(wViewChairID, i, pPlayerBet->lBetScore[i]);
			m_lPlayerBetAll[i] += pPlayerBet->lBetScore[i];
		}
	}
	return true;
}

//下注失败
bool CGameClientEngine::OnSubPlayerBetFail( const void * pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayerBetFail));
	if (wDataSize!=sizeof(CMD_S_PlayerBetFail)) return false;

	//消息处理
	CMD_S_PlayerBetFail * pPlayerBetFail = (CMD_S_PlayerBetFail *)pBuffer;

	//自己下注删除
	if (pPlayerBetFail->wChairID == GetMeChairID())
	{
		for( int i = 0 ; i < AREA_ALL ; ++i )
		{
			m_lPlayerBet[i] -= pPlayerBetFail->lBetScore[i];
			m_lPlayerBetAll[i] -= pPlayerBetFail->lBetScore[i];
		}
	}

	//下注失败
	WORD wViewChairID = SwitchViewChairID(pPlayerBetFail->wChairID);
	for( int i = 0 ; i < AREA_ALL ; ++i )
	{
		m_GameClientView.SetPlayerBet(wViewChairID, i, -pPlayerBetFail->lBetScore[i]);
	}

	//提示失败消息
	if ( pPlayerBetFail->wChairID == GetMeChairID() )
	{
		CDialogMessage Message;

		if ( pPlayerBetFail->cbFailType == FAIL_TYPE_TIME_OVER )
		{
			Message.SetMessage(TEXT("请在下注时间进行下注！"));	
		}
		else
		{
			Message.SetMessage(TEXT("下注金额超过系统限制金额，请重新下注！"));
		}
		Message.DoModal();
	}
	return true;
}

//强制结束
bool CGameClientEngine::OnSubMandatoryEnd( const void * pBuffer, WORD wDataSize )
{
	if ( wDataSize != 0 )
		return false;

	//跑马结束
	m_GameClientView.HorsesEnd();
	return true;
}

//控制
bool CGameClientEngine::OnSubAdminControl( const void * pBuffer, WORD wDataSize )
{
	ASSERT(wDataSize==sizeof(CMD_S_ControlReturns));
	if(wDataSize!=sizeof(CMD_S_ControlReturns)) return false;

	if( NULL != m_GameClientView.m_DlgControl.m_hWnd )
	{
		CMD_S_ControlReturns* pResult = (CMD_S_ControlReturns*)pBuffer;
		m_GameClientView.m_DlgControl.UpdateControl(pResult);
	}
	return true;
}

//自己最大下分
LONGLONG CGameClientEngine::GetMeMaxBet( BYTE cbArea /*= AREA_ALL */ )
{
	//找出玩家
	IClientUserItem * pUserData = GetTableUserItem(GetMeChairID());
	if ( pUserData == NULL || cbArea > AREA_ALL ) 
		return 0l;

	//玩家分数
	LONGLONG lUserScore = pUserData->GetUserScore();

	//减去自己已下注
	for(int i = 0; i < AREA_ALL; ++i)
	{
		lUserScore -= m_lPlayerBet[i];
	}

	//如果是区域下注. 还要判断区域限制
	if ( cbArea < AREA_ALL )
	{
		//个人区域限制
		LONGLONG lUserLimit = m_lUserLimitScore - m_lPlayerBet[cbArea];
		lUserScore = __min( lUserScore, lUserLimit);

		//区域总限制
		LONGLONG lAreaLimit = m_lAreaLimitScore - m_lPlayerBetAll[cbArea];
		lUserScore = __min( lUserScore, lAreaLimit);
	}

	//异常错误限制
	ASSERT( lUserScore >= 0l );
	if( lUserScore < 0 )
		lUserScore = 0;

	return lUserScore;
}