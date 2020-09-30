#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

//空闲时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					1									//空闲时间

//下注时间
#define IDI_BET						2									//下注时间
#ifdef _DEBUG
	#define TIME_BET				20									//下注时间
#else
	#define TIME_BET				30									//下注时间
#endif 


//结束时间
#define IDI_END						3									//结束时间
#define TIME_END					17									//结束时间


LONGLONG		CTableFrameSink::m_lStorageStart = 0L;
LONGLONG		CTableFrameSink::m_lStorageDeduct = 1L;

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//下注数
	m_lCellScore = 0;
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//玩家分数
	ZeroMemory(m_dwDrawStartTime, sizeof(m_dwDrawStartTime));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	ZeroMemory(m_lPlayOriginalScore, sizeof(m_lPlayOriginalScore));
	ZeroMemory(m_lPlayChip, sizeof(m_lPlayChip));

	//筹码
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//转盘路径
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//转盘倍数
	m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));
	
	//生成几率
	ZeroMemory(m_nGenerationChance, sizeof(m_nGenerationChance));
	m_nGenerationChance[ANIMAL_LION] = 20;
	m_nGenerationChance[ANIMAL_PANDA] = 20;
	m_nGenerationChance[ANIMAL_MONKEY] = 40;
	m_nGenerationChance[ANIMAL_RABBIT] = 40;
	m_nGenerationChance[ANIMAL_EAGLE] = 20;
	m_nGenerationChance[ANIMAL_PEACOCK] = 20;
	m_nGenerationChance[ANIMAL_PIGEON] = 40;
	m_nGenerationChance[ANIMAL_SWALLOW] = 40;
	m_nGenerationChance[ANIMAL_SLIVER_SHARK] = 5;
	m_nGenerationChance[ANIMAL_GOLD_SHARK] = 5;

	//状态变量
	m_dwBetTime=0L;

	//时间设置
	m_cbFreeTime = TIME_FREE;
	m_cbBetTime = TIME_BET;
	m_cbEndTime = TIME_END;

	//设置变量
	m_lAreaLimitScore = 0;
	m_lPlayLimitScore = 0;

	//服务控制
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("SharkBattleServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}
//释放对象
VOID  CTableFrameSink::Release()
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}

	delete this; 

}
//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//错误判断
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
		return false;
	}

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//设置文件名
	TCHAR szRoomName[32];
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(szConfigFileName,CountArray(szConfigFileName),TEXT("%s\\SharkBattleConfig.ini"),szPath);
	memcpy(szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	//变量定义
	TCHAR OutBuf[255] = {};

	//单位积分
	m_lCellScore = GetPrivateProfileInt(szRoomName, TEXT("CellScore"), 1, szConfigFileName);
	if ( m_lCellScore < 1 )
		m_lCellScore = 1;
	
	//区域限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("AreaLimitScore"), _T("0"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lAreaLimitScore);

	//玩家区域限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("PlayLimitScore"), _T("0"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lPlayLimitScore);

	//库存
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("StorageStart"), _T("0"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageStart);

	//衰减值
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("StorageDeduct"), _T("1"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageDeduct);

	//下注机器人数目
	m_nMaxChipRobot = GetPrivateProfileInt(m_szGameRoomName, TEXT("RobotBetCount"), 10, szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//最多个数
	m_nRobotListMaxCount = GetPrivateProfileInt(m_szGameRoomName, _T("RobotListMaxCount"), 5, szConfigFileName);	

	//机器人下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("RobotAreaLimit"), TEXT("50000000"), OutBuf, 255*sizeof(TCHAR), szConfigFileName);
	_snwscanf(OutBuf, wcslen(OutBuf), TEXT("%I64d"), &m_lRobotAreaLimit);


	//时间控制
	m_cbFreeTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("FreeTime"), TIME_FREE, szConfigFileName);

	m_cbBetTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("BetTime"), TIME_BET, szConfigFileName);

	m_cbEndTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("EndTime"), TIME_END, szConfigFileName);

	if (m_cbFreeTime <= 0 || m_cbFreeTime > 99)			
		m_cbFreeTime = TIME_FREE;
	if (m_cbBetTime <= 0 || m_cbBetTime > 99)		
		m_cbBetTime = TIME_BET;
	if (m_cbEndTime <= 0 || m_cbEndTime > 99)			
		m_cbEndTime = TIME_END;


	if( m_lStorageDeduct < 1 )
	{
		m_lStorageDeduct = 1;
	}
	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

	//筹码
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//转盘路径
	m_bTurnTwoTime = FALSE;
	ZeroMemory(m_nTurnTableTarget, sizeof(m_nTurnTableTarget));

	//转盘倍数
	m_nLastMultipleIndex = 0;
	ZeroMemory(m_nAnimalMultiple, sizeof(m_nAnimalMultiple));

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NETWORK_ERROR:		//网络中断
	case GER_USER_LEAVE:		//用户离开
		{
			//非结算状态， 返还下注金币
			if (m_pITableFrame->GetGameStatus() != GAME_SCENE_END)
			{
				//定义消息
				CMD_S_BetClear CMDBetClear;
				CMDBetClear.wChairID = wChairID;
				CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

				//清除下注
				BOOL bSendMessage = FALSE;
				for ( int i = 0; i < ANIMAL_MAX; ++i )
				{
					if ( m_lPlayBet[wChairID][i] > 0 )
					{
						bSendMessage = TRUE;
						m_lAllBet[i] -= m_lPlayBet[wChairID][i];
						m_lPlayBet[wChairID][i] = 0;
					}
				}

				//发送消息
				if ( bSendMessage )
				{
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
				}
			}

			//写分
			LONGLONG lScore = (m_lPlayScore[wChairID] - m_lPlayOriginalScore[wChairID]) + (m_lPlayChip[wChairID] * m_lCellScore);
			if ( lScore != 0 )
			{
				//防止负分
				if ( lScore < 0 && (-lScore) > pIServerUserItem->GetUserScore() )
				{
					lScore = -pIServerUserItem->GetUserScore();
				}

				//计算税收
				LONGLONG lRevenue = 0;
				if ( lScore > 0 )
				{
					lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lScore);
					lScore -= lRevenue;
				}

				//写分
				tagScoreInfo ScoreInfo;
				ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
				ScoreInfo.lScore = lScore;
				ScoreInfo.lRevenue = lRevenue;
				if ( lScore > 0 )
					ScoreInfo.cbType = SCORE_TYPE_WIN;
				else if ( lScore < 0 )
					ScoreInfo.cbType = SCORE_TYPE_LOSE;
				else
					ScoreInfo.cbType = SCORE_TYPE_DRAW;

				m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
				//m_pITableFrame->WriteUserScore(wChairID, ScoreInfo, INVALID_DWORD,  (DWORD)time(NULL) - m_dwDrawStartTime[wChairID]);//原来的
				//pIServerUserItem->WriteUserScore(ScoreInfo.lScore, 0, ScoreInfo.lRevenue, 0, ScoreInfo.cbType, );
			}

			return true;
		}
	}
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch( cbGameStatus )
	{
	case GAME_SCENE_FREE:
		{
			//变量定义
			CMD_S_StatusFree CMDStatusFree;
			ZeroMemory(&CMDStatusFree,sizeof(CMDStatusFree));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusFree.cbTimeLeave = (BYTE)(m_cbFreeTime - __min(dwPassTime, m_cbFreeTime));

			//变量信息
			CMDStatusFree.lCellScore = m_lCellScore;
			CMDStatusFree.lPlayScore = pIServerUserItem->GetUserScore();
			CMDStatusFree.lStorageStart = m_lStorageStart;

			CMDStatusFree.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusFree.lPlayLimitScore = m_lPlayLimitScore;

			//游戏记录
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusFree.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusFree.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusFree,sizeof(CMDStatusFree));
			return bReturn;
		}
		break;
	case GAME_SCENE_BET:
		{
			//变量定义
			CMD_S_StatusPlay CMDStatusPlay;
			ZeroMemory(&CMDStatusPlay,sizeof(CMDStatusPlay));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusPlay.cbTimeLeave = (BYTE)(m_cbBetTime - __min(dwPassTime, m_cbBetTime));

			//变量信息
			CMDStatusPlay.lCellScore = m_lCellScore;
			CMDStatusPlay.lPlayScore = m_lPlayScore[wChairID];
			CMDStatusPlay.lPlayChip = m_lPlayChip[wChairID];
			CMDStatusPlay.lStorageStart = m_lStorageStart;

			CMDStatusPlay.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusPlay.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory( CMDStatusPlay.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusPlay.nAnimalMultiple));
			CopyMemory( CMDStatusPlay.lAllBet, m_lAllBet, sizeof(CMDStatusPlay.lAllBet));
			CopyMemory( CMDStatusPlay.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusPlay.lPlayBet));

			//游戏记录
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusPlay.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusPlay.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusPlay,sizeof(CMDStatusPlay));
			return bReturn;
		}
		break;
	case GAME_SCENE_END:
		{
			//变量定义
			CMD_S_StatusEnd CMDStatusEnd;
			ZeroMemory(&CMDStatusEnd,sizeof(CMDStatusEnd));

			//时间信息
			DWORD dwPassTime = (DWORD)time(NULL)-m_dwBetTime;
			CMDStatusEnd.cbTimeLeave = (BYTE)(m_cbEndTime - __min(dwPassTime, m_cbEndTime));

			//变量信息
			CMDStatusEnd.lCellScore = m_lCellScore;
			CMDStatusEnd.lPlayScore = m_lPlayScore[wChairID];
			CMDStatusEnd.lPlayChip = m_lPlayChip[wChairID];
			CMDStatusEnd.lStorageStart = m_lStorageStart;

			CMDStatusEnd.lAreaLimitScore = m_lAreaLimitScore;
			CMDStatusEnd.lPlayLimitScore = m_lPlayLimitScore;

			CopyMemory( CMDStatusEnd.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDStatusEnd.nAnimalMultiple));
			CopyMemory( CMDStatusEnd.lAllBet, m_lAllBet, sizeof(CMDStatusEnd.lAllBet));
			CopyMemory( CMDStatusEnd.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDStatusEnd.lPlayBet));

			//游戏记录
			for ( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				CMDStatusEnd.nTurnTableRecord[i] = INT_MAX;
				if( i < m_ArrayTurnTableRecord.GetCount() )
					CMDStatusEnd.nTurnTableRecord[i] = m_ArrayTurnTableRecord[i];
			}

			//发送场景
			bool bReturn = m_pITableFrame->SendGameScene(pIServerUserItem,&CMDStatusEnd,sizeof(CMDStatusEnd));
			return bReturn;
		}
		break;
	}
	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_BET:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if ( m_pITableFrame->GetGameStatus() != GAME_SCENE_END )
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GAME_SCENE_END);

				//结束游戏
				int nPrizesMultiple = GameOver();

				//发送结束啊消息
				CMD_S_GameEnd CMDGameEnd;
				ZeroMemory(&CMDGameEnd, sizeof(CMDGameEnd));
				CMDGameEnd.cbTimeLeave = m_cbEndTime;
				CMDGameEnd.bTurnTwoTime = m_bTurnTwoTime;
				CMDGameEnd.nPrizesMultiple = nPrizesMultiple;
				CMDGameEnd.lPlayShowPrizes = (rand()%70000)/1000*1000;
				CopyMemory(CMDGameEnd.nTurnTableTarget, m_nTurnTableTarget, sizeof(CMDGameEnd.nTurnTableTarget));
				for ( int i = 0; i < GAME_PLAYER; ++i )
				{
					IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if ( pServerUserItem == NULL || !pServerUserItem->IsClientReady() )
						continue;

					CMDGameEnd.lPlayWin[0] = m_lPlayWinScore[i][0];
					CMDGameEnd.lPlayWin[1] = m_lPlayWinScore[i][1];
					CMDGameEnd.lPlayPrizes = m_lPlayWinPrizes[i];
					m_pITableFrame->SendUserItemData(pServerUserItem, SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));
				}
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END, &CMDGameEnd, sizeof(CMDGameEnd));

				//写分
				GameOverWriteScore();

				//设置时间
				m_dwBetTime = (DWORD)time(NULL);
				if ( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_GOLD_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 30)*1000, 1, NULL);	
				}
				else if( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]) == ANIMAL_SLIVER_SHARK )
				{
					m_pITableFrame->SetGameTimer(IDI_END, (m_cbEndTime + 17)*1000, 1, NULL);	
				}
				else
				{
					m_pITableFrame->SetGameTimer(IDI_END, m_cbEndTime*1000, 1, NULL);	
				}	
			}
			return true;
		}
	case IDI_END:				//结束游戏
		{
			//开始游戏
			//-------------------------------------------------------

			//设置时间
			m_dwBetTime = (DWORD)time(NULL);

			//显示库存
			static int	nCurrentHour = 0;							//显示库存
			CTime time = CTime::GetCurrentTime();					//获取当前时间. 
			if ( time.GetHour() != nCurrentHour )
			{
				TCHAR szServiceInfo[128] = {0};
				_sntprintf(szServiceInfo,sizeof(szServiceInfo), TEXT("当前库存量：%I64d"), m_lStorageStart);
				CTraceService::TraceString(szServiceInfo,TraceLevel_Info);
				nCurrentHour = time.GetHour();
			}

			//消息定义
			CMD_S_GameStart CMDGameStart;
			ZeroMemory(&CMDGameStart,sizeof(CMDGameStart));

			//设置变量
			CMDGameStart.cbTimeLeave = m_cbBetTime;
			CMDGameStart.lStorageStart = m_lStorageStart;

			//生产倍数
			int nAnimalMultiple[5][ANIMAL_MAX] = 
			{
				{ 12,  8,  8,  6, 12,  8,  8,  6, 24, 24,  2,  2},
				{ 24, 24, 12,  3, 24, 24, 12,  3, 24, 24,  2,  2},
				{ 24,  8,  6,  6, 24,  8,  6,  6, 24, 24,  2,  2},
				{ 24, 12,  8,  4, 24, 12,  8,  4, 24, 24,  2,  2},
				{ 12, 12,  6,  6, 12, 12,  6,  6, 24, 24,  2,  2},
			};

			int nLastMultipleIndex = m_nLastMultipleIndex;
			do 
			{
				m_nLastMultipleIndex = rand()%5;
			} while ( nLastMultipleIndex == m_nLastMultipleIndex );
			CopyMemory(m_nAnimalMultiple, nAnimalMultiple[m_nLastMultipleIndex], sizeof(m_nAnimalMultiple));
			CopyMemory(CMDGameStart.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDGameStart.nAnimalMultiple));

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));

			//开启定时器
			m_pITableFrame->SetGameTimer(IDI_BET, m_cbBetTime*1000, 1, NULL);

			//设置状态
			m_pITableFrame->SetGameStatus(GAME_SCENE_BET);

			return true;
		}
	}

	return false;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
	case SUB_C_EXCHANGE_CHIP:		//兑换筹码
		return OnSubChip( pData, wDataSize, pIServerUserItem );
	case SUB_C_PLAY_BET:			//用户下注
		return OnSubPlayBet( pData, wDataSize, pIServerUserItem );
	case SUB_C_BET_CLEAR:			//清除下注
		return OnSubBetClear( pData, wDataSize, pIServerUserItem );
	case SUB_C_AMDIN_COMMAND:
		{
			
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			//权限判断
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			
			if ( m_pServerContro == NULL)
				return true;

			
			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	if ( m_dwBetTime == 0 )
	{
		//开始游戏
		m_pITableFrame->StartGame();
	}

	//强制设置成游戏状态
	pIServerUserItem->SetUserStatus(US_PLAYING, pIServerUserItem->GetTableID(), wChairID);

	if ( m_dwBetTime == 0 )
	{
		//设置时间
		m_dwBetTime=(DWORD)time(NULL);

		//显示库存
		static int	nCurrentHour = 0;							//显示库存
		CTime time = CTime::GetCurrentTime();					//获取当前时间. 
		if ( time.GetHour() != nCurrentHour )
		{
			TCHAR szServiceInfo[128] = {0};
			_sntprintf(szServiceInfo,sizeof(szServiceInfo), TEXT("当前库存量：%I64d"), m_lStorageStart);
			CTraceService::TraceString(szServiceInfo,TraceLevel_Info);
			nCurrentHour = time.GetHour();
		}

		//消息定义
		CMD_S_GameStart CMDGameStart;
		ZeroMemory(&CMDGameStart,sizeof(CMDGameStart));

		//设置变量
		CMDGameStart.cbTimeLeave = m_cbBetTime;
		CMDGameStart.lStorageStart = m_lStorageStart;

		//生产倍数
		int nAnimalMultiple[5][ANIMAL_MAX] = 
		{
			{ 12,  8,  8,  6, 12,  8,  8,  6, 24, 24,  2,  2},
			{ 24, 24, 12,  3, 24, 24, 12,  3, 24, 24,  2,  2},
			{ 24,  8,  6,  6, 24,  8,  6,  6, 24, 24,  2,  2},
			{ 24, 12,  8,  4, 24, 12,  8,  4, 24, 24,  2,  2},
			{ 12, 12,  6,  6, 12, 12,  6,  6, 24, 24,  2,  2},
		};

		int nLastMultipleIndex = m_nLastMultipleIndex;
		do 
		{
			m_nLastMultipleIndex = rand()%5;
		} while ( nLastMultipleIndex == m_nLastMultipleIndex );
		CopyMemory(m_nAnimalMultiple, nAnimalMultiple[m_nLastMultipleIndex], sizeof(m_nAnimalMultiple));
		CopyMemory(CMDGameStart.nAnimalMultiple, m_nAnimalMultiple, sizeof(CMDGameStart.nAnimalMultiple));

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START, &CMDGameStart, sizeof(CMDGameStart));

		//设置时间
		m_pITableFrame->SetGameTimer(IDI_BET, m_cbBetTime*1000, 1, NULL);

		//设置状态
		m_pITableFrame->SetGameStatus(GAME_SCENE_BET);
	}

	//设置变量
	m_dwDrawStartTime[wChairID] = (DWORD)time(NULL);
	m_lPlayOriginalScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayScore[wChairID] = pIServerUserItem->GetUserScore();
	m_lPlayChip[wChairID] = 0;
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( bLookonUser )
		return true;

	//设置变量
	m_dwDrawStartTime[wChairID] = 0;
	m_lPlayOriginalScore[wChairID] = 0;
	m_lPlayScore[wChairID] = 0;
	m_lPlayChip[wChairID] = 0;
	m_lPlayWinChip[wChairID] = 0;
	m_lPlayWinPrizes[wChairID] = 0;
	ZeroMemory(m_lPlayWinScore[wChairID], sizeof(LONGLONG)*2);
	ZeroMemory(m_lPlayWinAnimal[wChairID], sizeof(LONGLONG)*ANIMAL_MAX);

	////检测是否有人
	//for ( WORD i = 0; i < GAME_PLAYER; ++i )
	//{
	//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
	//	if (pIServerUserItem == NULL) continue;

	//	return true;
	//}

	//结束游戏
	//m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

	return true;
}

//筹码兑换消息
bool CTableFrameSink::OnSubChip( VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem )
{
	//效验数据
	ASSERT( wDataSize == sizeof(CMD_C_Chip) );
	if ( wDataSize != sizeof(CMD_C_Chip) ) return false;

	//消息处理
	CMD_C_Chip * pChip = (CMD_C_Chip *)pData;
	WORD wChairID = pIServerUserItem->GetChairID();

	//效验信息
	if ( pChip->lChip * m_lCellScore > m_lPlayScore[wChairID] )
	{
		ASSERT(FALSE);
		return false;
	}

	//修改变量
	m_lPlayScore[wChairID] -= pChip->lChip * m_lCellScore;
	m_lPlayChip[wChairID] += pChip->lChip;

	return true;
}

//用户下注消息
bool CTableFrameSink::OnSubPlayBet(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT( wDataSize == sizeof(CMD_C_PlayBet) );
	if ( wDataSize != sizeof(CMD_C_PlayBet) ) return false;

	//消息处理
	CMD_C_PlayBet * pPlayBet = (CMD_C_PlayBet *)pData;
	WORD wChairID = pIServerUserItem->GetChairID();

	//下注异常
	if ( pPlayBet->nAnimalIndex < 0 || pPlayBet->nAnimalIndex >= ANIMAL_MAX || pPlayBet->lBetChip > m_lPlayChip[wChairID] || pPlayBet->lBetChip < 0 )
	{
		ASSERT(FALSE);
		return false;
	}

	//下注为0
	if( pPlayBet->lBetChip == 0 ) 
	{
		ASSERT(FALSE);
		return true;
	}

	//判断超额
	if ( (m_lAreaLimitScore != 0 && m_lAllBet[pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lAreaLimitScore)
		|| (m_lPlayLimitScore != 0 && m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] + pPlayBet->lBetChip > m_lPlayLimitScore)
		|| ( m_pITableFrame->GetGameStatus() != GAME_SCENE_BET ) )
	{
		//下注失败
		CMD_S_PlayBetFail CMDPlayBetFail;
		CMDPlayBetFail.wChairID = wChairID;
		CMDPlayBetFail.nAnimalIndex = pPlayBet->nAnimalIndex;
		CMDPlayBetFail.lBetChip = pPlayBet->lBetChip;
		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_PLAY_BET_FAIL, &CMDPlayBetFail, sizeof(CMDPlayBetFail));
		return true;
	}

	//下注成功
	m_lPlayChip[wChairID] -= pPlayBet->lBetChip;
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel>0)//星之光注释
		m_HPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	if (pIServerUserItem && pIServerUserItem->IsAndroidUser())
		m_APlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel==0)//星之光注释
		m_FPlayBet[pPlayBet->nAnimalIndex]+=pPlayBet->lBetChip;
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
	m_lPlayBet[wChairID][pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;
	m_lAllBet[pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;

	//发送消息
	CMD_S_PlayBet CMDPlayBet;
	CMDPlayBet.wChairID = wChairID;
	CMDPlayBet.nAnimalIndex = pPlayBet->nAnimalIndex;
	CMDPlayBet.lBetChip = pPlayBet->lBetChip;
	CMDPlayBet.mHBet=0;
	//if (pIServerUserItem && pIServerUserItem->GetUserInfo()->cbMemberLevel>0)  //星之光注释
		CMDPlayBet.mHBet=pPlayBet->lBetChip;//星之光注释
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));

	return true;
}

//清除下注消息
bool CTableFrameSink::OnSubBetClear(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT( wDataSize == NULL );
	if ( wDataSize != NULL ) return false;

	//消息处理
	WORD wChairID = pIServerUserItem->GetChairID();

	//定义消息
	CMD_S_BetClear CMDBetClear;
	CMDBetClear.wChairID = wChairID;
	CopyMemory(CMDBetClear.lPlayBet, m_lPlayBet[wChairID], sizeof(CMDBetClear.lPlayBet));

	//清除下注
	BOOL bSendMessage = FALSE;
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		if ( m_lPlayBet[wChairID][i] > 0 )
		{
			bSendMessage = TRUE;
			m_lPlayChip[wChairID] += m_lPlayBet[wChairID][i];
			m_lAllBet[i] -= m_lPlayBet[wChairID][i];
			m_lPlayBet[wChairID][i] = 0;
		}
	}
	
	//发送消息
	if ( bSendMessage )
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_CLEAR, &CMDBetClear, sizeof(CMDBetClear));
	}
	
	return true;
}

//生成转盘
bool CTableFrameSink::CreateTableTable()
 {
	//设置变量
	m_bTurnTwoTime = FALSE;

	//生产第一张图
	int nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
															m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
															m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
															m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4,
															m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_RABBIT]/3,
															m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_SWALLOW]/3,
															m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SLIVER_SHARK]/4, m_nGenerationChance[ANIMAL_PIGEON]/2,
															m_nGenerationChance[ANIMAL_PIGEON]/2, m_nGenerationChance[ANIMAL_GOLD_SHARK]/4, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

	
	////int nTemp[] = { 0, 4 };
	//static nWin = 0;
	//nWin = (nWin + 1)%CountArray(nTemp);
	//nWinTable = nTemp[nWin];
	//nWinTable = 4;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//m_nTurnTableTarget[0]//控制第一次现得动物
	m_nTurnTableTarget[0] = nWinTable;
	BYTE ProbilityRand=GetProbility();
	bool bControl = false;
	if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//在此处加入控制
	{
		if(m_pServerContro->ControlResult(&m_nTurnTableTarget[0]))
		{
			bControl=true;
			m_pServerContro->CompleteControl();
			nWinTable=m_nTurnTableTarget[0];
			if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK ) //星之光QQ1090075705 修正
			m_bTurnTwoTime = TRUE; //星之光QQ1090075705 修正
		}			
	}
	else if(rand()%100<ProbilityRand)
	{
		int minArea1=0;
		int minArea2=0;
		int minArea3=0;
		int minArea4=0;
		LONGLONG AreaBet[8];
		AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
		AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
		AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
		AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
		AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
		AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
		AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
		AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
		LONGLONG maxAreaBet=0;
		int maxArea=0;
		for	 (int i=0;i<8;i++)
		{
			if (maxAreaBet<=AreaBet[i])
			{
				maxAreaBet=AreaBet[i];
				maxArea=i;
			}
		}
		if (maxAreaBet!=0)
		{
			LONGLONG AreaBet1=AreaBet[maxArea];
			LONGLONG AreaBet2=AreaBet[maxArea];
			LONGLONG AreaBet3=AreaBet[maxArea];
			LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
		/*	switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
		}
		m_nTurnTableTarget[0] = nWinTable;
		//如果是鲨鱼，生产下一张图
		if ( CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_GOLD_SHARK || CGameLogic::GetInstance()->TurnTableAnimal(nWinTable) == ANIMAL_SLIVER_SHARK )
		{
			m_bTurnTwoTime = TRUE;


			//生产第二张图
			nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
				0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
				0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
				0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
				m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
				m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
				m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
				m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

			////nWinTable=3;
			//m_nTurnTableTarget[1] //控制当出现金鲨银鲨之后，第二次出现的动物
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			m_nTurnTableTarget[1] = nWinTable;
			int minArea1=0;
			int minArea2=0;
			int minArea3=0;
			int minArea4=0;
			LONGLONG AreaBet[8];
			AreaBet[0]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_LION]*m_nAnimalMultiple[ANIMAL_LION];
			AreaBet[1]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_PANDA]*m_nAnimalMultiple[ANIMAL_PANDA];
			AreaBet[2]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_MONKEY]*m_nAnimalMultiple[ANIMAL_MONKEY];
			AreaBet[3]=m_HPlayBet[ANIMAL_BEAST]*2+m_HPlayBet[ANIMAL_RABBIT]*m_nAnimalMultiple[ANIMAL_RABBIT];
			AreaBet[4]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_EAGLE]*m_nAnimalMultiple[ANIMAL_EAGLE];
			AreaBet[5]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PEACOCK]*m_nAnimalMultiple[ANIMAL_PEACOCK];
			AreaBet[6]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_PIGEON]*m_nAnimalMultiple[ANIMAL_PIGEON];
			AreaBet[7]=m_HPlayBet[ANIMAL_BIRD]*2+m_HPlayBet[ANIMAL_SWALLOW]*m_nAnimalMultiple[ANIMAL_SWALLOW];
			LONGLONG maxAreaBet=0;
			int maxArea=0;
			for	 (int i=0;i<8;i++)
			{
				if (maxAreaBet<=AreaBet[i])
				{
					maxAreaBet=AreaBet[i];
					maxArea=i;
				}
			}
			if (maxAreaBet!=0)
			{
				LONGLONG AreaBet1=AreaBet[maxArea];
				LONGLONG AreaBet2=AreaBet[maxArea];
				LONGLONG AreaBet3=AreaBet[maxArea];
				LONGLONG AreaBet4=AreaBet[maxArea];
			for	 (int i=0;i<8;i++)
			{
				if (AreaBet1>=AreaBet[i])
				{
					AreaBet1=AreaBet[i];
					minArea1=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet2>=AreaBet[i]&&i!=minArea1)
				{
					AreaBet2=AreaBet[i];
					minArea2=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet3>=AreaBet[i]&&i!=minArea1&&i!=minArea2)
				{
					AreaBet3=AreaBet[i];
					minArea3=i;
				}
			}
			for (int i=0;i<8;i++)
			{
				if (AreaBet4>=AreaBet[i]&&i!=minArea1&&i!=minArea2&&i!=minArea3)
				{
					AreaBet4=AreaBet[i];
					minArea4=i;
				}
			}
			/*switch(rand()%4)
			{
			case 0:
				nWinTable=minArea1;
				break;
			case 1:
				nWinTable=minArea2;
				break;
			case 2:
				nWinTable=minArea3;
				break;
			case 3:
				nWinTable=minArea4;
				break;
			}*/
			nWinTable=minArea1;
			switch(nWinTable)
			{
			case 0:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=5;
						break;
					case 1:
						nWinTable=6;
						break;
					case 2:
						nWinTable=7;
						break;
					}
				}
				break;
			case 1:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=9;
						break;
					case 1:
						nWinTable=10;
						break;
					}
				}
				break;
			case 2:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=12;
						break;
					case 1:
						nWinTable=13;
						break;
					}
				}
				break;
			case 3:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=15;
						break;
					case 1:
						nWinTable=16;
						break;
					case 2:
						nWinTable=17;
						break;
					}
				}
				break;
			case 4:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=1;
						break;
					case 1:
						nWinTable=2;
						break;
					case 2:
						nWinTable=3;
						break;
					}
				}
				break;
			case 5:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=26;
						break;
					case 1:
						nWinTable=27;
						break;
					}
				}
				break;
			case 6:
				{
					switch(rand()%2)
					{
					case 0:
						nWinTable=23;
						break;
					case 1:
						nWinTable=24;
						break;
					}
				}
				break;
			case 7:
				{
					switch(rand()%3)
					{
					case 0:
						nWinTable=19;
						break;
					case 1:
						nWinTable=20;
						break;
					case 2:
						nWinTable=21;
						break;
					}
				}
				break;
			}
			m_nTurnTableTarget[1] = nWinTable;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	else
	{
		//生产第二张图
		nWinTable = CGameLogic::GetInstance()->RandomArea(TURAN_TABLE_MAX, 
			0, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3, m_nGenerationChance[ANIMAL_EAGLE]/3,
			0, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3, m_nGenerationChance[ANIMAL_LION]/3,
			0, m_nGenerationChance[ANIMAL_PANDA]/2, m_nGenerationChance[ANIMAL_PANDA]/2, 0,
			m_nGenerationChance[ANIMAL_MONKEY]/2, m_nGenerationChance[ANIMAL_MONKEY]/2, 0, m_nGenerationChance[ANIMAL_RABBIT]/3,
			m_nGenerationChance[ANIMAL_RABBIT]/3, m_nGenerationChance[ANIMAL_RABBIT]/3, 0, m_nGenerationChance[ANIMAL_SWALLOW]/3,
			m_nGenerationChance[ANIMAL_SWALLOW]/3, m_nGenerationChance[ANIMAL_SWALLOW]/3, 0, m_nGenerationChance[ANIMAL_PIGEON]/2,
			m_nGenerationChance[ANIMAL_PIGEON]/2, 0, m_nGenerationChance[ANIMAL_PEACOCK]/2, m_nGenerationChance[ANIMAL_PEACOCK]/2);

		////nWinTable=3;
		//m_nTurnTableTarget[1] //控制当出现金鲨银鲨之后，第二次出现的动物
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_nTurnTableTarget[1] = nWinTable;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

//游戏结束计算
int CTableFrameSink::GameOver()
{
	//定义变量
	int nPrizesMultiple = 0;
	bool bSuccess = false;
	LONGLONG lSystemScore = 0l;

	LONGLONG lTempCalculateScore = 0l;
	LONGLONG lMaxSystemScore = LLONG_MIN;
	int nMaxTurnTableTarget[2] = {0};
	int nWhileCount = 0;

	//计算分数
	do 
	{
		//输赢分数
		ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
		ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
		ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
		ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

		//系统输赢
		lSystemScore = 0l;

		//派发扑克
		CreateTableTable();

		//试探性判断
		lSystemScore += CalculateScore( true, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]), nPrizesMultiple );
//CHAR sztmp1[32] ;
//_snprintf(sztmp1, sizeof(sztmp1), "1个：%ld " ,lSystemScore ) ;
//MessageBoxA(NULL,sztmp1,"标题1", MB_OK );

		if ( m_bTurnTwoTime)
			lSystemScore += CalculateScore( false, CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]), nPrizesMultiple );
//CHAR sztmp2[32] ;
//_snprintf(sztmp2, sizeof(sztmp2), "得分：%s,%ld,%ld " ,sztmp1,lSystemScore,m_lStorageStart ) ;
//MessageBoxA(NULL,sztmp2,"标题2", MB_OK );

		//系统分值计算
		if ((lSystemScore + m_lStorageStart) < 0l)
		{
			bSuccess =  true;

			nWhileCount++;
			if( lSystemScore > lMaxSystemScore )
			{
				lMaxSystemScore = lSystemScore;
				CopyMemory(nMaxTurnTableTarget, m_nTurnTableTarget, sizeof(nMaxTurnTableTarget));
			}

			if( nWhileCount >= 100000 && lMaxSystemScore != LLONG_MIN )
			{
				//强制结束
				m_lStorageStart += lMaxSystemScore;
				CopyMemory(m_nTurnTableTarget, nMaxTurnTableTarget, sizeof(m_nTurnTableTarget));
				break;
			}
		}
		else
		{
			m_lStorageStart += lSystemScore;
			
			//有人并且有人下注才衰减库存
			for( int i = 0; i < GAME_PLAYER; ++i )
			{
				if( m_pITableFrame->GetTableUserItem(i) != NULL && lSystemScore != 0 )
				{
					m_lStorageStart = m_lStorageStart - (m_lStorageStart * m_lStorageDeduct / 1000);
					break;
				}
			}

			bSuccess = true;
		}

	} while (!bSuccess);

	//添加记录
	m_ArrayTurnTableRecord.Add( m_nTurnTableTarget[0] );
	if( m_ArrayTurnTableRecord.GetCount() > RECORD_COUNT_MAX )
	{
		m_ArrayTurnTableRecord.RemoveAt(0);
	}
////////星之光QQ1090075705 测试用
/*
CHAR sztmp2[32] ;
_snprintf(sztmp2, sizeof(sztmp2), "第一个：%d 第二个：%d" ,CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[0]),CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableTarget[1]) ) ;
MessageBoxA(NULL,sztmp2,"标题", MB_OK );
*/
/////////////星之光QQ1090075705 测试用


return nPrizesMultiple;
}

//计算得分
LONGLONG CTableFrameSink::CalculateScore( bool bFirst, int nWinAnimal, int& nPrizesMultiple )
{

///星之光QQ1090075705 测试用
/*
CHAR sztmp3[32] ;
_snprintf(sztmp3, sizeof(sztmp3), "第%d次调用,开出：%d" ,bFirst,nWinAnimal ) ;
MessageBoxA(NULL,sztmp3,"标题", MB_OK );
*/
///星之光QQ1090075705 测试用

	//系统输赢
	LONGLONG lSystemScore = 0l;
	
	//彩金
	if ( bFirst && nWinAnimal == ANIMAL_GOLD_SHARK )
	{
		nPrizesMultiple = (rand()%94) + 6;
	}

	//计算积分
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//是否机器人
		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		for ( int nAnimalIndex = 0; nAnimalIndex < ANIMAL_MAX; ++nAnimalIndex )
		{
			if ( nWinAnimal == nAnimalIndex || 
				(nAnimalIndex == ANIMAL_BIRD && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BIRD ) ||
				(nAnimalIndex == ANIMAL_BEAST && CGameLogic::GetInstance()->AnimalType(nWinAnimal) == ANIMAL_TYPE_BEAST ) )
			{
				//玩家赢分
				LONGLONG lWinScore = m_lPlayBet[wChairID][nAnimalIndex] * m_nAnimalMultiple[nAnimalIndex];
				m_lPlayWinChip[wChairID] += lWinScore;
				m_lPlayWinAnimal[wChairID][nAnimalIndex] += lWinScore;

				if ( bFirst )
					m_lPlayWinScore[wChairID][0] += lWinScore;
				else
					m_lPlayWinScore[wChairID][1] += lWinScore;

				//系统输分
				if ( !bIsAndroidUser )
					lSystemScore -= (lWinScore - m_lPlayBet[wChairID][nAnimalIndex]);
			}
			else if( bFirst )
			{
				//玩家输分
				LONGLONG lLoseScore = m_lPlayBet[wChairID][nAnimalIndex];

				//系统赢分
				if ( !bIsAndroidUser )
					lSystemScore += lLoseScore;
			}
		}

/////////星之光QQ1090075705 测试用
		/*
if ( !bIsAndroidUser )
{
CHAR sztmp[32] ;
ZeroMemory(sztmp, sizeof(sztmp));
_snprintf(sztmp, sizeof(sztmp), "成绩:%ld,第一个：%ld 第二个：%ld" ,m_lPlayWinChip[wChairID],m_lPlayWinScore[wChairID][0],m_lPlayWinScore[wChairID][1] ) ;
MessageBoxA(NULL,sztmp,"标题", MB_OK );
}
*/
/////////星之光QQ1090075705 测试用




		//计算彩金
		if ( nPrizesMultiple != 0 && bFirst )
		{
			//玩家赢分
			LONGLONG lWinScore = (m_lPlayBet[wChairID][nWinAnimal] * nPrizesMultiple);
			m_lPlayWinChip[wChairID] += lWinScore;
			m_lPlayWinAnimal[wChairID][nWinAnimal] += lWinScore;
			m_lPlayWinPrizes[wChairID] += lWinScore;

			//系统输分
			if ( !bIsAndroidUser )
				lSystemScore -= lWinScore;
		}

	}

	return lSystemScore;
}

//结束写分
void CTableFrameSink::GameOverWriteScore()
{
	//写入积分
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		//扣除筹码
		m_lPlayChip[wChairID] += m_lPlayWinChip[wChairID];
		
		if ( m_lPlayChip[wChairID] < 0 )
		{
			ASSERT(FALSE);
			m_lPlayChip[wChairID] = 0;
		}

////星之光QQ1090075705 测试用
/*
IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
if ((pIServerUserItem != NULL) &&(!pIServerUserItem->IsAndroidUser()))
{
CHAR sztmp[32] ;
ZeroMemory(sztmp, sizeof(sztmp));
_snprintf(sztmp, sizeof(sztmp), "成绩:%ld,第一个：%ld 第二个：%ld" ,m_lPlayWinChip[wChairID],m_lPlayWinScore[wChairID][0],m_lPlayWinScore[wChairID][1] ) ;
MessageBoxA(NULL,sztmp,"标题", MB_OK );
}
*/
/////星之光QQ1090075705 测试用


	}

	//清空下注
	ZeroMemory(m_lPlayWinAnimal, sizeof(m_lPlayWinAnimal));
	ZeroMemory(m_lPlayWinChip, sizeof(m_lPlayWinChip));
	ZeroMemory(m_lPlayWinScore, sizeof(m_lPlayWinScore));
	ZeroMemory(m_lPlayWinPrizes, sizeof(m_lPlayWinPrizes));

	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(m_HPlayBet,sizeof(m_HPlayBet));
	ZeroMemory(m_FPlayBet,sizeof(m_FPlayBet));
	ZeroMemory(m_APlayBet,sizeof(m_APlayBet));
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
}

//读取配置文件
BYTE CTableFrameSink::GetProbility()
{
	return ::GetPrivateProfileInt(m_pGameServiceOption->szServerName, _T("Problility"), 30, szConfigFileName);
}

//////////////////////////////////////////////////////////////////////////////////
