#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_FULL_READY;	//开始模式
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameCustomRule=NULL;

	//游戏变量
	m_lChip = 0;
	m_wTurnCount = 0;
	m_wCurUser = INVALID_CHAIR;
	m_wChipUser = INVALID_CHAIR;
	m_wLoseUser = INVALID_CHAIR;
	m_bHaveYellOne = false;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));
	ZeroMemory(m_bHaveThrow, sizeof(m_bHaveThrow));
	ZeroMemory(m_bCompleteOpen, sizeof(m_bCompleteOpen));

	//配置变量
	m_nCfgCellScoreType = 0;
	m_nCfgMinScoreMul = 4;
	m_nCfgCellScore = 1;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
#ifdef __SPECIAL___
	QUERYINTERFACE(ITableUserActionEX,Guid,dwQueryVer);	
#endif
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
	
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);


	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//读取配置
	TCHAR szPath[MAX_PATH]=TEXT("");
	TCHAR szConfigFileName[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(szConfigFileName,sizeof(szConfigFileName),TEXT("%s\\LiarsDiceConfig.ini"),szPath);

	TCHAR szRoomName[32] = {};
	memcpy(szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	m_nCfgCellScoreType = GetPrivateProfileInt(szRoomName, TEXT("CellScoreType"), 0, szConfigFileName);
	m_nCfgMinScoreMul = GetPrivateProfileInt(szRoomName, TEXT("MinScoreMul"), 1, szConfigFileName);
	m_nCfgCellScore = GetPrivateProfileInt(szRoomName, TEXT("CellScore"), 1, szConfigFileName);

	if (m_nCfgCellScoreType < 0 || m_nCfgCellScoreType > 1)		m_nCfgCellScoreType = 0;
	if (m_nCfgMinScoreMul < 1)		m_nCfgMinScoreMul = 1;
	if (m_nCfgCellScore < 1)		m_nCfgCellScore = 1;

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_lChip = 0;
	m_wTurnCount = 0;
	m_wCurUser = INVALID_CHAIR;
	m_wChipUser = INVALID_CHAIR;
	m_bHaveYellOne = false;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));
	ZeroMemory(m_bHaveThrow, sizeof(m_bHaveThrow));
	ZeroMemory(m_bCompleteOpen, sizeof(m_bCompleteOpen));

	return;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//效验状态
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetTableID()==m_pITableFrame->GetTableID()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetTableID()!=m_pITableFrame->GetTableID())) return 0;

	//用户判断
	switch (pIServerUserItem->GetUserStatus())
	{
	case US_SIT:		//坐下状态
	case US_READY:		//准备状态
		{
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore;
		}
	case US_PLAYING:	//游戏状态
	case US_OFFLINE:	//断线状态
		{
			WORD wMaxTimes=m_pGameCustomRule->wMaxScoreTimes;
			return pIServerUserItem->GetUserScore()-m_pGameServiceOption->lCellScore*wMaxTimes;
		}
	case US_LOOKON:		//旁观状态
		{
			return pIServerUserItem->GetUserScore();
		}
	}

	return 0L;
}

//最少积分
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0L;
}
//开始模式
BYTE  CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_SETCHIP);

	//变量设置
	m_lChip = 0;
	m_wTurnCount = 0;
	m_wCurUser = INVALID_CHAIR;
	m_bHaveYellOne = false;
	ZeroMemory(m_bDiceData, sizeof(m_bDiceData));
	ZeroMemory(m_UserYell, sizeof(m_UserYell));
	ZeroMemory(m_bHaveThrow, sizeof(m_bHaveThrow));
	ZeroMemory(m_bCompleteOpen, sizeof(m_bCompleteOpen));

	//轮流下注
	if (m_wChipUser == INVALID_CHAIR)
		m_wChipUser = rand()%2;
	else
		m_wChipUser = (m_wChipUser+1)%2;

	ASSERT(m_wChipUser>=0&&m_wChipUser<GAME_PLAYER);
	if (m_wChipUser < 0 || m_wChipUser >= GAME_PLAYER)
		m_wChipUser = 0;

	//下注上限
	LONGLONG lMinUserGold = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{	
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		LONGLONG lUserGold = pIServerUserItem->GetUserScore();
		if (i == 0)		lMinUserGold = lUserGold;
		lMinUserGold = (lUserGold < lMinUserGold ? lUserGold : lMinUserGold);
	}

	if (lMinUserGold < 0)	lMinUserGold = 0;

	if(m_pGameServiceOption->lRestrictScore != 0)
		m_lMaxChip = __min(LONGLONG(lMinUserGold/100) * 100,m_pGameServiceOption->lRestrictScore);
	else
		m_lMaxChip = LONGLONG(lMinUserGold/100) * 100;

	if (m_lMaxChip <= 0)	m_lMaxChip = 100;	
	//调整下注
	if (m_nCfgCellScoreType == 1)
	{
		if (m_nCfgCellScore*4 < m_lMaxChip)
			m_lMaxChip = m_nCfgCellScore*4;
	}
	else
	{
		m_lMaxChip /= m_nCfgMinScoreMul;
	}
	if (m_lMaxChip <= 0)
		m_lMaxChip = 4;
	
	//金币场需下注
	if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
	{
		//发送消息
		CMD_S_Chip_Start chipStart = {0};
		chipStart.wUserID = m_wChipUser;
		chipStart.lMaxChip = m_lMaxChip;

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_START_CHIP,&chipStart,sizeof(chipStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_START_CHIP,&chipStart,sizeof(chipStart)); 
	}
	else
	{
		LONGLONG lScore = 1;
		LONGLONG lUserScore[GAME_PLAYER] = {0};
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem)
				lUserScore[i] = pIServerUserItem->GetUserScore();
		}
		LONGLONG lMinScore = (lUserScore[0]>lUserScore[1]?lUserScore[0]:lUserScore[1]);

		if (lMinScore <= 50)								lScore = 1;
		else if (lMinScore > 50 && lMinScore <= 200)		lScore = 2;
		else if (lMinScore > 200 && lMinScore <= 500)		lScore = 3;
		else if (lMinScore > 500 && lMinScore <= 1000)		lScore = 4;
		else if (lMinScore > 1000 && lMinScore <= 2000)		lScore = 5;
		else if (lMinScore > 2000)							lScore = 6;

		m_lMaxChip  = lScore * 4;

		//MyDebug(_T("积分下注：%d 最大 %I64d  下注 %I64d"), m_wChipUser, m_lMaxChip, lScore);

		//跳过下注	(模拟消息)
		CMD_C_Chip chip = {0};
		chip.lChip = lScore;
		chip.wUserID = m_wChipUser;

		OnSubChip(m_wChipUser, (void*)&chip, sizeof(chip));
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//MyDebug(_T("End: %d"), cbReason);
	switch (cbReason)
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<GAME_PLAYER);

			//构造数据
			CMD_S_GameEnd GameEnd = {0};
			GameEnd.bEndMode = GER_DISMISS ;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//效验参数
			ASSERT(wChairID<GAME_PLAYER);

			//变量定义
			CMD_S_GameEnd GameEnd = {0};
			GameEnd.bEndMode = GER_NORMAL;

			//计算积分
			WORD wBeOpenUser = (m_wCurUser+1)%2;
			int nCount = 0;
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				nCount += m_GameLogic.CalcDiceCount(m_bDiceData[i], 5, m_UserYell[wBeOpenUser].bDicePoint, m_bHaveYellOne);
				//MyDebug(_T("结算：%d号玩家 %d 个 %d"), i, m_GameLogic.CalcDiceCount(m_bDiceData[i], 5, m_UserYell[wBeOpenUser].bDicePoint, m_bHaveYellOne), 
					//m_UserYell[wBeOpenUser].bDicePoint);
			}

			BYTE bDicePoint = m_UserYell[wBeOpenUser].bDicePoint;
			int nDiceCount = m_UserYell[wBeOpenUser].bDiceCount;

			GameEnd.lGameScore[m_wCurUser] = (nCount<nDiceCount?1:-1) * m_lChip;
			GameEnd.lGameScore[wBeOpenUser] = (nCount<nDiceCount?-1:1) * m_lChip;

			//MyDebug(_T("结算： %I64d %I64d 税收： %d"), GameEnd.lGameScore[0], GameEnd.lGameScore[1], m_pGameServiceOption->wRevenue);

			//记录失败
			m_wLoseUser = (nCount<nDiceCount?wBeOpenUser:m_wCurUser);
			//MyDebug(_T("结算： 当前玩家： %d  失败玩家：  %d"), m_wCurUser, m_wLoseUser);

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				LONGLONG lTmpTax = 0;
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				for (int i=0; i < GAME_PLAYER; i++)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if(pIServerUserItem)	
					{
						//MyDebug(_T("%d 金币 %d 积分 %d 经验 %d 存储 %d"), i, pIServerUserItem->GetUserScore()->lGameGold, pIServerUserItem->GetUserScore()->lScore,
							//pIServerUserItem->GetUserScore()->lExperience, pIServerUserItem->GetUserScore()->lInsureScore);

						lTmpTax = 0;
						if (GameEnd.lGameScore[i] > 0)
							lTmpTax = (LONGLONG)((LONGLONG)GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000);
						GameEnd.lGameTax += lTmpTax;
						GameEnd.lGameScore[i] = GameEnd.lGameScore[i] - lTmpTax;

						ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
						ScoreInfoArray[i].lRevenue=GameEnd.lGameTax;                                  
						ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
					}
				}
				//修改分数
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			}
			else
			{
				LONGLONG lScore = 1;
				LONGLONG lUserScore[GAME_PLAYER] = {0};
				for (int i = 0; i < GAME_PLAYER; i++)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem)
						lUserScore[i] = pIServerUserItem->GetUserScore();
				}
				LONGLONG lMinScore = (lUserScore[0]>lUserScore[1]?lUserScore[0]:lUserScore[1]);

				if (lMinScore <= 50)								lScore = 1;
				else if (lMinScore > 50 && lMinScore <= 200)		lScore = 2;
				else if (lMinScore > 200 && lMinScore <= 500)		lScore = 3;
				else if (lMinScore > 500 && lMinScore <= 1000)		lScore = 4;
				else if (lMinScore > 1000 && lMinScore <= 2000)		lScore = 5;
				else if (lMinScore > 2000)							lScore = 6;

				GameEnd.lGameScore[m_wCurUser] = (nCount<nDiceCount?1:-1) * lScore;
				GameEnd.lGameScore[wBeOpenUser] = (nCount<nDiceCount?-1:1) * lScore;

				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
				for (int i = 0; i < GAME_PLAYER; i++)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem)
					{
						ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
						ScoreInfoArray[i].lRevenue=GameEnd.lGameTax;                                  
						ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
					}
				}
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);		

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			//定义变量
			CMD_S_GameEnd GameEnd = {0};
			GameEnd.bEndMode = GER_USER_LEAVE ;

			LONGLONG lPunish = m_lChip;
			if (lPunish <= 0)
				lPunish = m_lMaxChip / 4;
			if (lPunish <= 0)
				lPunish = 1;

			//统计积分
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (i == wChairID)
					GameEnd.lGameScore[i] -= lPunish;
				else
					GameEnd.lGameScore[i] += (LONGLONG)(lPunish/(GAME_PLAYER-1));
			}

			LONGLONG lTmpTax = 0;
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem==NULL) continue;

				BYTE nScoreKind;
				if(i==wChairID) nScoreKind=SCORE_TYPE_FLEE;
				else nScoreKind=SCORE_TYPE_WIN;

				//计算税收
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				{
					lTmpTax = 0;
					if (GameEnd.lGameScore[i] > 0)
							lTmpTax = (LONGLONG)(GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000);
					GameEnd.lGameTax += lTmpTax;
					GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lTmpTax;
				}
				ScoreInfoArray[i].cbType =nScoreKind;
				ScoreInfoArray[i].lRevenue=GameEnd.lGameTax;                                  
				ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];				
			}
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

            return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:			//空闲状态
		{
					
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
			StatusFree.wLeftTimer=10;
			//发送场景
			
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SETCHIP:			//下注状态 
		{
						
			CMD_S_StatusSetChip	StatusSetChip = {0};
			StatusSetChip.lMaxChip = m_lMaxChip;
			StatusSetChip.wChipUser = m_wChipUser;
			//发送场景
			
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusSetChip,sizeof(StatusSetChip));
		}
	case GS_WK_THROWDICE:		//摇骰状态
		{
						
			CMD_S_StatusThrow StatusThrow = {0};
			StatusThrow.lChip = m_lChip;
			memcpy(StatusThrow.bHaveThrow, m_bHaveThrow, sizeof(m_bHaveThrow));
			memcpy(StatusThrow.bDiceData, m_bDiceData[wChairID], sizeof(BYTE)*5);

			//MyDebug(_T("GS_WK_THROWDICE [%d %d  %d %d %d %d %d]"), m_bHaveThrow[0], m_bHaveThrow[1], 
				//m_bDiceData[wChairID][0], m_bDiceData[wChairID][1], m_bDiceData[wChairID][2], m_bDiceData[wChairID][3], m_bDiceData[wChairID][4]);

			//发送场景
			
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusThrow,sizeof(StatusThrow));
		}
	case GS_WK_PLAYING :		//游戏状态
		{
			
			CMD_S_StatusPlay StatusPlay = {0};
			StatusPlay.wGameTurn = m_wTurnCount;
			StatusPlay.wCurUser = m_wCurUser;
			StatusPlay.lChip = m_lChip;
			memcpy(StatusPlay.userYell, m_UserYell, sizeof(m_UserYell));
			memcpy(StatusPlay.bDiceData, m_bDiceData[wChairID], sizeof(BYTE)*5);

			
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GS_WK_OPENDICE:		//开骰状态
		{

			CMD_S_StatusOpen StatusOpen = {0};

			//计算数目
			WORD wBeOpenUser = (m_wCurUser+1)%2;
			int nCount = 0;
			BYTE bDicePoint = m_UserYell[wBeOpenUser].bDicePoint;
			int nDiceCount = m_UserYell[wBeOpenUser].bDiceCount;

			for (int i = 0; i < GAME_PLAYER; i++)
				nCount += m_GameLogic.CalcDiceCount(m_bDiceData[i], 5, bDicePoint, m_bHaveYellOne);	

			StatusOpen.nDiceRealCount = nCount;
			StatusOpen.bOpenRight = (nCount<nDiceCount);
			StatusOpen.wOpenUser = m_wCurUser;
			StatusOpen.lChip = m_lChip;
			memcpy(&StatusOpen.curYell, &m_UserYell[wBeOpenUser], sizeof(StatusOpen.curYell));
			memcpy(StatusOpen.bDiceData, m_bDiceData, sizeof(m_bDiceData));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusOpen,sizeof(StatusOpen));
		}
	}

	return true;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return true;
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}
//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}
//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

	switch (wSubCmdID)
	{
	case SUB_C_CHIP:		//玩家下注
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubChip(pUserData->wChairID , pData, wDataSize);
		}
	case SUB_C_THROWDICE:	//玩家摇骰
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubThrowDice(pUserData->wChairID , pData, wDataSize);
		}
	case SUB_C_THROWFINISH:	//摇骰完毕
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubThrowFinish(pUserData->wChairID , pData, wDataSize);
		}
	case SUB_C_YELLDICE:	//玩家喊话
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubYellDice(pUserData->wChairID , pData, wDataSize);
		}
	case SUB_C_OPENDICE:	//玩家开骰
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubOpenDice(pUserData->wChairID , pData, wDataSize);
		}
	case SUB_C_COMPLETEOPEN://开骰结束
		{
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();	
			return OnSubCompleteOpen(pUserData->wChairID , pData, wDataSize);
		}
	}
	
	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(false==bLookonUser)
	{
		m_wChipUser = INVALID_CHAIR;
		m_wLoseUser = INVALID_CHAIR;
	}

	return true;
}

//用户下注
bool CTableFrameSink::OnSubChip(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID && sizeof(CMD_C_Chip)==wDataSize);
	if (INVALID_CHAIR == wChairID || sizeof(CMD_C_Chip) != wDataSize)	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_SETCHIP);
	if (GS_WK_SETCHIP != m_pITableFrame->GetGameStatus())	return true;

	CMD_C_Chip	*pChip = (CMD_C_Chip*)pDataBuffer;
	ASSERT(pChip->lChip<=m_lMaxChip&&pChip->lChip>0&& wChairID==m_wChipUser);
	if (pChip->lChip > m_lMaxChip || pChip->lChip <= 0 || wChairID != m_wChipUser)	return false;

	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_THROWDICE);

	m_lChip = pChip->lChip;

	srand(GetTickCount());

	//发送消息
	CMD_S_Chip_Result chipResult = {0};
	chipResult.lChip = m_lChip;
	chipResult.wUserID = wChairID;
	
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResult,sizeof(chipResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResult,sizeof(chipResult)); 

	//随机骰组
	BYTE bDiceData[2][5] = {0};
	m_GameLogic.RandDice((BYTE*)bDiceData, 10);

	memcpy(m_bDiceData, bDiceData, sizeof(BYTE)*10);
	
	//MyDebug(_T("喊话1： %d   %d"), m_wCurUser, m_wLoseUser);
	//先喊话者
	if (m_wLoseUser == INVALID_CHAIR)
		m_wCurUser = rand()%2;
	else
		m_wCurUser = m_wLoseUser;

	//发送消息
	CMD_S_SendDice	sendDice = {0};
	sendDice.wCurUser = m_wCurUser;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		memcpy(sendDice.bDiceData, m_bDiceData[i], sizeof(BYTE)*5);
		
		m_pITableFrame->SendTableData(i,SUB_S_SEND_DICE,&sendDice,sizeof(sendDice));
		m_pITableFrame->SendLookonData(i,SUB_S_SEND_DICE,&sendDice,sizeof(sendDice)); 
	}

	return true;
}

//摇骰完毕
bool CTableFrameSink::OnSubThrowDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID);
	if (INVALID_CHAIR == wChairID)	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_THROWDICE);
	if (GS_WK_THROWDICE != m_pITableFrame->GetGameStatus())	return true;

	//发送消息
	CMD_S_ThrowDice throwDice = {0};
	throwDice.wThrowUser = wChairID;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THROW_DICE,&throwDice,sizeof(CMD_S_ThrowDice));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THROW_DICE,&throwDice,sizeof(CMD_S_ThrowDice)); 

	return true;
}

//玩家摇骰
bool CTableFrameSink::OnSubThrowFinish(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID);
	if (INVALID_CHAIR == wChairID)	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_THROWDICE);
	if (GS_WK_THROWDICE != m_pITableFrame->GetGameStatus())	return true;

	ASSERT(!m_bHaveThrow[wChairID]);
	m_bHaveThrow[wChairID] = true;

	//计算人数
	if (m_bHaveThrow[0] && m_bHaveThrow[1])
	{
		//设置状态
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

		//发送消息
		CMD_S_YellDice yellDice = {0};
		yellDice.wCurUser = m_wCurUser;
		yellDice.wGameTurn = m_wTurnCount;
		ZeroMemory(&yellDice.maxYell, sizeof(tagDiceYell));

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_YELL_DICE,&yellDice,sizeof(yellDice));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_YELL_DICE,&yellDice,sizeof(yellDice)); 
	}

	return true;
}

//玩家喊话
bool CTableFrameSink::OnSubYellDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID&&sizeof(CMD_C_YellDice)==wDataSize&&wChairID==m_wCurUser);
	if (INVALID_CHAIR == wChairID || sizeof(CMD_C_YellDice)!=wDataSize || wChairID != m_wCurUser)	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_PLAYING);
	if (GS_WK_PLAYING != m_pITableFrame->GetGameStatus())	return true;
	
	CMD_C_YellDice *pYellDice = (CMD_C_YellDice*)pDataBuffer;
	BYTE bRange[2] = {0};
	LRESULT ret = m_GameLogic.CalcDiceRange(&m_UserYell[(m_wCurUser+1)%2], pYellDice->curYell.bDiceCount, (m_wTurnCount==0), &bRange[0], &bRange[1]);

	ASSERT(ret == 0 || ret == 2);
	if (ret != 0 && ret != 2)	return false;

	ASSERT(pYellDice->curYell.bDicePoint>=bRange[0]&&pYellDice->curYell.bDicePoint<=bRange[1]);
	if (pYellDice->curYell.bDicePoint<bRange[0]&&pYellDice->curYell.bDicePoint>bRange[1])	return false;

	//发送消息
	CMD_S_YellDice_Result yellResult = {0};
	memcpy(&yellResult.curYell, &pYellDice->curYell, sizeof(tagDiceYell));
	yellResult.wYellUser = m_wCurUser;
	
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_YELL_RESULT,&yellResult,sizeof(CMD_S_YellDice_Result));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_YELL_RESULT,&yellResult,sizeof(CMD_S_YellDice_Result));

	//更新变量
	m_wTurnCount++;
	m_wCurUser = (m_wCurUser+1)%GAME_PLAYER;
	memcpy(&m_UserYell[wChairID], &pYellDice->curYell, sizeof(tagDiceYell));

	//计算喊话
	if (pYellDice->curYell.bDicePoint == 1)
		m_bHaveYellOne = true;

	//发送消息
	CMD_S_YellDice	yellDice = {0};
	yellDice.wCurUser = m_wCurUser;
	yellDice.wGameTurn = m_wTurnCount;
	memcpy(&yellDice.maxYell, &pYellDice->curYell, sizeof(tagDiceYell));
	
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_YELL_DICE,&yellDice,sizeof(CMD_S_YellDice));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_YELL_DICE,&yellDice,sizeof(CMD_S_YellDice));

	return true;
}

//玩家开骰
bool CTableFrameSink::OnSubOpenDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID&&wChairID==m_wCurUser);
	if (INVALID_CHAIR == wChairID || wChairID != m_wCurUser)	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_PLAYING);
	if (GS_WK_PLAYING != m_pITableFrame->GetGameStatus())	return true;

	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_OPENDICE);

	//计算数目
	WORD wBeOpenUser = (m_wCurUser+1)%2;
	int nCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
		nCount += m_GameLogic.CalcDiceCount(m_bDiceData[i], 5, m_UserYell[wBeOpenUser].bDicePoint, m_bHaveYellOne);

	BYTE bDicePoint = m_UserYell[wBeOpenUser].bDicePoint;
	int nDiceCount = m_UserYell[wBeOpenUser].bDiceCount;
	
	//发送信息
	CMD_S_OpenDice	openDice = {0};
	openDice.wOpenUser = wChairID;
	openDice.bDicePoint = bDicePoint;
	openDice.nDiceOpenCount = nDiceCount;
	openDice.nDiceRealCount = nCount;
	openDice.bOpenRight = (nCount<nDiceCount);
	memcpy(openDice.bDiceData, m_bDiceData, sizeof(m_bDiceData));

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_DICE,&openDice,sizeof(CMD_S_OpenDice));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_DICE,&openDice,sizeof(CMD_S_OpenDice));

	return true;
}

//开骰结束
bool CTableFrameSink::OnSubCompleteOpen(const WORD wChairID, const void * pDataBuffer, WORD wDataSize)
{
	//MyDebug(_T("CTableFrameSink::OnSubCompleteOpen [%d  %d %d  %d]"), wChairID, m_bCompleteOpen[0], m_bCompleteOpen[1], 
	//				m_pITableFrame->GetGameStatus());
	//数据验证
	ASSERT(INVALID_CHAIR!=wChairID&&!m_bCompleteOpen[wChairID]);
	if (INVALID_CHAIR == wChairID || m_bCompleteOpen[wChairID])	return false;

	ASSERT(m_pITableFrame->GetGameStatus() == GS_WK_OPENDICE);
	if (GS_WK_OPENDICE != m_pITableFrame->GetGameStatus())	return true;

	//检查结束
	m_bCompleteOpen[wChairID] = true;
	
	if (m_bCompleteOpen[0] && m_bCompleteOpen[1])
		return OnEventGameConclude(wChairID,NULL,GER_NORMAL);

	return true;
}