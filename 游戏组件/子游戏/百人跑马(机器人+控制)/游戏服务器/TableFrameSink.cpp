#include "StdAfx.h"
#include "TableFrameSink.h"


//////////////////////////////////////////////////////////////////////////

//空闲时间
#define IDI_FREE					1									//空闲时间
#define TIME_FREE					1									//空闲时间

//下注时间
#define IDI_BET_START				2									//开始下注
#define TIME_BET_START				10									//下注时间

//下注结束时间
#define IDI_BET_END					3									//开始下注
#define TIME_BET_END				1									//下注结束时间

//跑马时间
#define IDI_HORSES_START			4									//跑马开始
#define TIME_HORSES_START			50									//跑马时间

//跑马时间
#define IDI_HORSES_END				5									//跑马结束(强制客户端结束)
#define TIME_HORSES_END				(TIME_HORSES_START-5)				//跑马时间

//结束时间
#define IDI_STORAGE_INTERVAL		6									//结束时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_nStreak = 0;
	m_dwGameTime = 0;
	m_nFreeTime = TIME_FREE;
	m_nBetTime = TIME_BET_START;
	m_nBetEndTime = TIME_BET_END;
	m_nHorsesTime = TIME_HORSES_START;
	m_nHorsesEndTime = TIME_HORSES_END;
	CTime time = CTime::GetCurrentTime();					//获取当前时间. 
	m_nDay = time.GetDay();									//天

	//控制变量
	m_cbCLTimes = 0;
	m_bControl = false;
	m_cbCLArea = 255;
	for (int i = 0 ; i < AREA_ALL; ++i)
		m_nCLMultiple[i] = -1;


	//限制变量
	m_lAreaLimitScore = 0l;
	m_lUserLimitScore = 0l;

	//库存
	m_StorageStart = 0l;
	m_StorageStart = 1;

	//游戏结果
	m_cbGameResults = AREA_ALL;

	//分数
	m_nBetPlayerCount = 0;
	ZeroMemory(m_lPlayerBet,sizeof(m_lPlayerBet));
	ZeroMemory(m_lPlayerBetWin,sizeof(m_lPlayerBetWin));
	ZeroMemory(m_lPlayerBetAll,sizeof(m_lPlayerBetAll));
	ZeroMemory(m_lPlayerWinning,sizeof(m_lPlayerWinning));
	ZeroMemory(m_lPlayerReturnBet,sizeof(m_lPlayerReturnBet));
	ZeroMemory(m_lPlayerRevenue,sizeof(m_lPlayerRevenue));

	//区域倍数
	m_bMultipleControl = FALSE;
	for ( int i = 0; i < CountArray(m_nMultiple); ++i)
		m_nMultiple[i] = 1;

	//马匹信息
	ZeroMemory(m_nHorsesSpeed,sizeof(m_nHorsesSpeed));
	for ( int i = 0; i < CountArray(m_cbHorsesRanking); ++i)
		m_cbHorsesRanking[i] = HORSES_ALL;

	//马匹名字
	_sntprintf(m_szHorsesName[HORSES_ONE],sizeof(m_szHorsesName[HORSES_ONE]),TEXT("一号马"));
	_sntprintf(m_szHorsesName[HORSES_TWO],sizeof(m_szHorsesName[HORSES_TWO]),TEXT("二号马"));
	_sntprintf(m_szHorsesName[HORSES_THREE],sizeof(m_szHorsesName[HORSES_THREE]),TEXT("三号马"));
	_sntprintf(m_szHorsesName[HORSES_FOUR],sizeof(m_szHorsesName[HORSES_FOUR]),TEXT("四号马"));
	_sntprintf(m_szHorsesName[HORSES_FIVE],sizeof(m_szHorsesName[HORSES_FIVE]),TEXT("五号马"));
	_sntprintf(m_szHorsesName[HORSES_SIX],sizeof(m_szHorsesName[HORSES_SIX]),TEXT("六号马"));

	//房间信息
	ZeroMemory(m_szConfigFileName,sizeof(m_szConfigFileName));
	ZeroMemory(m_szGameRoomName,sizeof(m_szGameRoomName));

	//游戏记录
	m_GameRecords.RemoveAll();
	ZeroMemory(m_nWinCount,sizeof(m_nWinCount));

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
	
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

VOID CTableFrameSink::Release()
{
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
//读取长整
LONGLONG CTableFrameSink::GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName)
{
	//变量定义
	TCHAR OutBuf[255] = _T("");
	TCHAR DefaultBuf[255] = {};
	LONGLONG lNumber = 0;

	GetPrivateProfileString(lpAppName, lpKeyName, DefaultBuf, OutBuf, 255, lpFileName);

	if (OutBuf[0] != 0)
		_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &lNumber);
	else
		lNumber = lDefault;

	return lNumber;
}
//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{

	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_pGameServiceAttrib!=NULL);
	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\HorseBattle.ini"),szPath);
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

	//控制变量
	TCHAR OutBuf[255];

	//分数限制
	m_lRobotScoreRange[0] = GetPrivateProfileInt64(m_szGameRoomName, _T("RobotScoreMin"), 10000000, m_szConfigFileName);
	m_lRobotScoreRange[1] = GetPrivateProfileInt64(m_szGameRoomName, _T("RobotScoreMax"), 100000000, m_szConfigFileName);

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	//提款数额
	m_lRobotBankGetScore = GetPrivateProfileInt64(m_szGameRoomName, _T("RobotBankGet"), 20000000, m_szConfigFileName);

	//提款数额 (庄家)
	m_lRobotBankGetScoreBanker = GetPrivateProfileInt64(m_szGameRoomName, _T("RobotBankGetBanker"), 30000000, m_szConfigFileName);

	//存款倍数
	m_nRobotBankStorageMul = GetPrivateProfileInt(m_szGameRoomName, _T("RobotBankStoMul"), 2, m_szConfigFileName);

	//库存更换间隔时间
	m_nStorageIntervalTime = GetPrivateProfileInt(m_szGameRoomName, _T("StorageIntervalTime"), 1, m_szConfigFileName);
	if ( m_nStorageIntervalTime<=0 ) m_nStorageIntervalTime=1;

	//库存数目
	m_nStorageCount =  GetPrivateProfileInt(m_szGameRoomName, _T("StorageCount"), 1, m_szConfigFileName);
	if ( m_nStorageCount<=0 ) m_nStorageCount=1;
	if ( m_nStorageCount==1 ) m_nStorageIntervalTime=0;//取消转换

	memset(m_StorageArray,0,sizeof(m_StorageArray));
	CString strStorageName=TEXT("");
	for (size_t begin=1; begin<=m_nStorageCount; ++begin)
	{
		strStorageName.Format(TEXT("%dStorage"),begin);
		memset(OutBuf,0,255);
		GetPrivateProfileString(m_szGameRoomName,strStorageName.GetBuffer(),TEXT("0"),OutBuf,255,m_szConfigFileName);
		_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%I64d"),&m_StorageArray[begin-1]);	
	}
	//库存降值
	m_StorageDeduct = GetPrivateProfileInt(m_szGameRoomName, TEXT("StorageDeduct"), 1, m_szConfigFileName);
	if ( m_StorageDeduct > 1000 )
		m_StorageDeduct = 900;

	//限制控制
	//----------------------------
	//区域限制
	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("AreaLimitScore"),TEXT("1000000000"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%I64d"),&m_lAreaLimitScore);
	//个人限制
	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szGameRoomName,TEXT("UserLimitScore"),TEXT("10000000"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%I64d"),&m_lUserLimitScore);

	//时间控制
	//----------------------------
	//空闲时间
	m_nFreeTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("FreeTime"), TIME_FREE, m_szConfigFileName);
	//下注时间
	m_nBetTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("BetTime"), TIME_BET_START, m_szConfigFileName);
	//下注结束时间
	m_nBetEndTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("BetEndTime"), TIME_BET_END, m_szConfigFileName);
	//跑马时间
	m_nHorsesTime = GetPrivateProfileInt(m_szGameRoomName, TEXT("HorsesTime"), TIME_HORSES_START, m_szConfigFileName);

	if ( m_nFreeTime < 10 )
		m_nFreeTime = 10;

	if ( m_nBetTime < 10 )
		m_nBetTime = 10;

	if ( m_nBetEndTime < 10 )
		m_nBetEndTime = 10;

	if ( m_nHorsesTime < 45 )
		m_nHorsesTime = 45;
	
	m_nHorsesEndTime = m_nHorsesTime - 5;

	//机器人控制
	//下注机器人数目
	m_nMaxChipRobot = GetPrivateProfileInt(m_szGameRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//机器人下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szGameRoomName, TEXT("RobotAreaLimit"), TEXT("50000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lRobotAreaLimit);
	
	m_nStorageNowNode=rand()%m_nStorageCount;
	m_StorageStart=m_StorageArray[m_nStorageNowNode];

	if ( m_nStorageIntervalTime>0 )
	{
		m_pITableFrame->SetGameTimer(IDI_STORAGE_INTERVAL,m_nStorageIntervalTime*60000,1,0L);
	}
	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏结果
	m_cbGameResults = AREA_ALL;

	//分数
	m_nBetPlayerCount = 0;
	ZeroMemory(m_lPlayerBet,sizeof(m_lPlayerBet));
	ZeroMemory(m_lPlayerBetWin,sizeof(m_lPlayerBetWin));
	ZeroMemory(m_lPlayerBetAll,sizeof(m_lPlayerBetAll));
	ZeroMemory(m_lPlayerWinning,sizeof(m_lPlayerWinning));
	ZeroMemory(m_lPlayerReturnBet,sizeof(m_lPlayerReturnBet));
	ZeroMemory(m_lPlayerRevenue,sizeof(m_lPlayerRevenue));

	//马匹信息
	ZeroMemory(m_nHorsesSpeed,sizeof(m_nHorsesSpeed));
	for ( int i = 0; i < CountArray(m_cbHorsesRanking); ++i)
		m_cbHorsesRanking[i] = HORSES_ALL;

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	return;
}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
	}
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{

	//定义变量
	CMD_S_BetStart stBetStart;
	ZeroMemory(&stBetStart, sizeof(stBetStart));
	stBetStart.nTimeLeave = m_nBetTime;
	stBetStart.nTimeBetEnd = m_nBetEndTime;

	//下注机器人数量
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}
	stBetStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	//机器人控制
	m_nChipRobotCount = 0;

	//发送消息 
	//----------------------------
	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_START, &stBetStart, sizeof(stBetStart));	

	//游戏玩家
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//设置积分
		stBetStart.lUserMaxScore = min(pIServerUserItem->GetUserScore(), m_lUserLimitScore);

		m_pITableFrame->SendTableData(wChairID,SUB_S_BET_START,&stBetStart,sizeof(stBetStart));	
	}
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			//跑马
			HorsesProcess();

			CMD_S_HorsesStart stHorsesStart;
			ZeroMemory(&stHorsesStart, sizeof(stHorsesStart));
			stHorsesStart.nTimeLeave = m_nHorsesTime;
			memcpy(stHorsesStart.nHorsesSpeed, m_nHorsesSpeed, sizeof(stHorsesStart.nHorsesSpeed));
			memcpy(stHorsesStart.cbHorsesRanking, m_cbHorsesRanking, sizeof(stHorsesStart.cbHorsesRanking));

			//发送消息
			//----------------------------
			//旁观玩家
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HORSES_START, &stHorsesStart, sizeof(stHorsesStart));	

			//游戏玩家
			for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
				if (pIServerUserItem==NULL) continue;

				//设置积分
				stHorsesStart.lPlayerWinning = m_lPlayerWinning[wChairID];
				stHorsesStart.lPlayerReturnBet = m_lPlayerReturnBet[wChairID];

				m_pITableFrame->SendTableData(wChairID,SUB_S_HORSES_START,&stHorsesStart,sizeof(stHorsesStart));	
			}
			
			return true;
		}
	case GER_USER_LEAVE:		//用户离开
		{

			//变量定义
			LONGLONG lScore=0l;

			//统计成绩
			for (int i = 0; i < AREA_ALL; ++i) 
			{
				lScore -= m_lPlayerBet[wChairID][i];
				m_lPlayerBet[wChairID][i] = 0;
				m_lPlayerBetWin[wChairID][i] = 0;
			}
			m_lPlayerWinning[wChairID] = 0;
			m_lPlayerReturnBet[wChairID] = 0;
			m_lPlayerRevenue[wChairID] = 0;

			//写入积分
			if ( lScore != 0 )
			{

				tagScoreInfo ScoreInfo;
				ZeroMemory(&ScoreInfo,sizeof(tagScoreInfo));
				ScoreInfo.lScore=lScore;
				ScoreInfo.cbType=SCORE_TYPE_LOSE;

				m_pITableFrame->WriteUserScore(pIServerUserItem->GetChairID(), ScoreInfo);
			}
			return true;
		}
	}

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		
		{
			//构造数据
			CMD_S_SceneFree SceneFree;
			ZeroMemory(&SceneFree,sizeof(SceneFree));	

			//时间
			INT dwPassTime = (INT)time(NULL) - m_dwGameTime;
			SceneFree.nTimeLeave = (m_nFreeTime - __min(dwPassTime,m_nFreeTime));

			//场次
			SceneFree.nStreak = m_nStreak;

			//倍率
			memcpy(SceneFree.nMultiple, m_nMultiple, sizeof(SceneFree.nMultiple));

			//马匹名称
			memcpy(SceneFree.szHorsesName, m_szHorsesName, sizeof(SceneFree.szHorsesName));

			//历史记录
			for ( int i = 0; i < m_GameRecords.GetCount(); ++i)
				SceneFree.GameRecords[i] = m_GameRecords[i];

			//全天赢场次
			memcpy(SceneFree.nWinCount, m_nWinCount, sizeof(SceneFree.nWinCount));

			//限制变量
			SceneFree.lAreaLimitScore = m_lAreaLimitScore;						//区域总限制
			SceneFree.lUserLimitScore = m_lUserLimitScore;						//个人区域限制

			//房间名称
			CopyMemory(SceneFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(SceneFree.szGameRoomName));
			
			//发送消息
			return m_pITableFrame->SendGameScene(pIServerUserItem,&SceneFree,sizeof(SceneFree));;
		}
	case GS_BET:	
		{
			//构造数据
			CMD_S_SceneBet SceneBet;
			ZeroMemory(&SceneBet,sizeof(SceneBet));	

			//时间
			INT dwPassTime = (INT)time(NULL) - m_dwGameTime;
			SceneBet.nTimeLeave = (m_nBetTime - __min(dwPassTime,m_nBetTime));
			SceneBet.nTimeLeave += m_nBetEndTime;

			//场次
			SceneBet.nStreak = m_nStreak;

			//倍率
			memcpy(SceneBet.nMultiple, m_nMultiple, sizeof(SceneBet.nMultiple));

			//马匹名称
			memcpy(SceneBet.szHorsesName, m_szHorsesName, sizeof(SceneBet.szHorsesName));

			//历史记录
			for ( int i = 0; i < m_GameRecords.GetCount(); ++i)
				SceneBet.GameRecords[i] = m_GameRecords[i];

			//全天赢场次
			memcpy(SceneBet.nWinCount, m_nWinCount, sizeof(SceneBet.nWinCount));

			//个人下注
			SceneBet.lUserMaxScore = GetPlayersMaxBet(wChiarID);

			//下注人数
			SceneBet.nBetPlayerCount = m_nBetPlayerCount;

			//玩家下注
			memcpy(SceneBet.lPlayerBet, m_lPlayerBet[wChiarID], sizeof(SceneBet.lPlayerBet));

			//所有下注
			memcpy(SceneBet.lPlayerBetAll, m_lPlayerBetAll, sizeof(SceneBet.lPlayerBetAll));

			//限制变量
			SceneBet.lAreaLimitScore = m_lAreaLimitScore;						//区域总限制
			SceneBet.lUserLimitScore = m_lUserLimitScore;						//个人区域限制

			//房间名称
			CopyMemory(SceneBet.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(SceneBet.szGameRoomName));

			//发送消息
			return m_pITableFrame->SendGameScene(pIServerUserItem,&SceneBet,sizeof(SceneBet));;
		}
	case GS_BET_END:			//下注结束	
		{
			//构造数据
			CMD_S_SceneBetEnd SceneBetEnd;
			ZeroMemory(&SceneBetEnd,sizeof(SceneBetEnd));	

			//时间
			INT dwPassTime = (INT)time(NULL) - m_dwGameTime;
			SceneBetEnd.nTimeLeave = (m_nBetEndTime - __min(dwPassTime,m_nBetEndTime));

			//场次
			SceneBetEnd.nStreak = m_nStreak;

			//倍率
			memcpy(SceneBetEnd.nMultiple, m_nMultiple, sizeof(SceneBetEnd.nMultiple));

			//马匹名称
			memcpy(SceneBetEnd.szHorsesName, m_szHorsesName, sizeof(SceneBetEnd.szHorsesName));

			//历史记录
			for ( int i = 0; i < m_GameRecords.GetCount(); ++i)
				SceneBetEnd.GameRecords[i] = m_GameRecords[i];

			//全天赢场次
			memcpy(SceneBetEnd.nWinCount, m_nWinCount, sizeof(SceneBetEnd.nWinCount));

			//下注人数
			SceneBetEnd.nBetPlayerCount = m_nBetPlayerCount;

			//玩家下注
			memcpy(SceneBetEnd.lPlayerBet, m_lPlayerBet[wChiarID], sizeof(SceneBetEnd.lPlayerBet));

			//所有下注
			memcpy(SceneBetEnd.lPlayerBetAll, m_lPlayerBetAll, sizeof(SceneBetEnd.lPlayerBetAll));

			//限制变量
			SceneBetEnd.lAreaLimitScore = m_lAreaLimitScore;						//区域总限制
			SceneBetEnd.lUserLimitScore = m_lUserLimitScore;						//个人区域限制

			//房间名称
			CopyMemory(SceneBetEnd.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(SceneBetEnd.szGameRoomName));

			//发送消息
			return m_pITableFrame->SendGameScene(pIServerUserItem,&SceneBetEnd,sizeof(SceneBetEnd));;
		}
	case GS_HORSES:			
		{
			//构造数据
			CMD_S_SceneHorses SceneHorses;
			ZeroMemory(&SceneHorses,sizeof(SceneHorses));	

			//时间
			INT dwPassTime = (INT)time(NULL) - m_dwGameTime;
			SceneHorses.nTimeLeave = (m_nHorsesTime - __min(dwPassTime,m_nHorsesTime));

			//场次
			SceneHorses.nStreak = m_nStreak;

			//倍率
			memcpy(SceneHorses.nMultiple, m_nMultiple, sizeof(SceneHorses.nMultiple));

			//马匹名称
			memcpy(SceneHorses.szHorsesName, m_szHorsesName, sizeof(SceneHorses.szHorsesName));

			//历史记录
			for ( int i = 0; i < m_GameRecords.GetCount(); ++i)
				SceneHorses.GameRecords[i] = m_GameRecords[i];

			//全天赢场次
			memcpy(SceneHorses.nWinCount, m_nWinCount, sizeof(SceneHorses.nWinCount));

			//下注人数
			SceneHorses.nBetPlayerCount = m_nBetPlayerCount;

			//玩家下注
			memcpy(SceneHorses.lPlayerBet, m_lPlayerBet[wChiarID], sizeof(SceneHorses.lPlayerBet));

			//所有下注
			memcpy(SceneHorses.lPlayerBetAll, m_lPlayerBetAll, sizeof(SceneHorses.lPlayerBetAll));

			//个人输赢
			SceneHorses.lPlayerWinning = m_lPlayerWinning[wChiarID];

			//限制变量
			SceneHorses.lAreaLimitScore = m_lAreaLimitScore;						//区域总限制
			SceneHorses.lUserLimitScore = m_lUserLimitScore;						//个人区域限制

			//房间名称
			CopyMemory(SceneHorses.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(SceneHorses.szGameRoomName));

			//发送消息
			return m_pITableFrame->SendGameScene(pIServerUserItem,&SceneHorses,sizeof(SceneHorses));;
		}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_STORAGE_INTERVAL:
		{
			m_pITableFrame->KillGameTimer(IDI_STORAGE_INTERVAL);


			m_StorageArray[m_nStorageNowNode] = m_StorageStart;
			m_nStorageNowNode				  = rand()%m_nStorageCount;
			m_StorageStart					  = m_StorageArray[m_nStorageNowNode];

			if ( m_nStorageIntervalTime>0 )
			{
				m_pITableFrame->SetGameTimer(IDI_STORAGE_INTERVAL,m_nStorageIntervalTime*60*1000,1,0L);
			}
			return true;
		}
		case IDI_FREE:
			{
				for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
				{
					//变量定义
					IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(wChairID);

					if ( (pUserItem==NULL) || (!pUserItem->IsAndroidUser())) 
					{
						continue;
					}

					LONGLONG lRobotScore = pUserItem->GetUserScore();
					LONGLONG lInsureScore = pUserItem->GetUserInsure();

					//判断存取
					if (lRobotScore > m_lRobotScoreRange[1])
					{
						//发送消息
						LONGLONG lStorageValue = lRobotScore - LONGLONG(lRobotScore/m_nRobotBankStorageMul);

						if (lStorageValue > 0 )
						{
							tagScoreInfo ScoreInfo;
							ZeroMemory(&ScoreInfo,sizeof(tagScoreInfo));
							ScoreInfo.lScore=lStorageValue*-1;
							ScoreInfo.cbType=SCORE_TYPE_LOSE;

							m_pITableFrame->WriteUserScore(pUserItem->GetChairID(),ScoreInfo);//存
						}
					}
					else if (lRobotScore < m_lRobotScoreRange[0])
					{
						//发送消息
						LONGLONG lGetValue = m_lRobotBankGetScore;

						if (lGetValue > 0)
						{
							tagScoreInfo ScoreInfo;
							ZeroMemory(&ScoreInfo,sizeof(tagScoreInfo));
							ScoreInfo.lScore=lGetValue;
							ScoreInfo.cbType=SCORE_TYPE_WIN;

							m_pITableFrame->WriteUserScore(pUserItem->GetChairID(),ScoreInfo);//取
						}
					}
				}

				//开始游戏
				m_pITableFrame->StartGame();

				//设置时间
				m_dwGameTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_BET_START,m_nBetTime*1000,1,0L);

				//设置状态
				m_pITableFrame->SetGameStatus(GS_BET);
			}
			return true;
		case IDI_BET_START:			//下注时间
			{
				//设置时间
				m_dwGameTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_BET_END,m_nBetEndTime*1000,1,0L);

				//设置状态
				m_pITableFrame->SetGameStatus(GS_BET_END);	

				//发送下注结束消息
				CMD_S_BetEnd stBetEnd;
				stBetEnd.nTimeLeave = m_nBetEndTime;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BET_END,&stBetEnd,sizeof(stBetEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BET_END,&stBetEnd,sizeof(stBetEnd));
			}
			return true;
		case IDI_BET_END:			//下注结束
			{
				//状态判断(防止强退重复设置)
				if (m_pITableFrame->GetGameStatus()!=GS_HORSES)
				{
					//设置状态
					m_pITableFrame->SetGameStatus(GS_HORSES);			

					//开始跑马(既结束游戏)
					OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

					//设置时间
					m_dwGameTime = (DWORD)time(NULL);
					m_pITableFrame->SetGameTimer(IDI_HORSES_START, m_nHorsesTime*1000, 1, 0L);
					m_pITableFrame->SetGameTimer(IDI_HORSES_END, m_nHorsesEndTime*1000, 1, 0L);		

				}	
			}
			return true;
		case IDI_HORSES_START:		//跑马时间
			{

				//写分
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if ( pIServerUserItem == NULL ) 
						continue;

					if ( m_lPlayerWinning[wChairID] != 0 )
					{
						tagScoreInfo ScoreInfo;
						ZeroMemory(&ScoreInfo,sizeof(tagScoreInfo));
						ScoreInfo.cbType=( m_lPlayerWinning[wChairID] > 0L ) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
						ScoreInfo.lScore=m_lPlayerWinning[wChairID];
						ScoreInfo.lRevenue=m_lPlayerRevenue[wChairID];
						
						//写入积分
						m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);
					}
				}



				//如有控制开启控制
				if( m_cbCLTimes > 0 )
					m_bControl = true;

				//倍数获得
				MultipleControl();

				//设置时间
				m_dwGameTime = (DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime*1000, 1, 0L);

				//发送消息
				CMD_S_HorsesEnd stHorsesEnd;
				ZeroMemory(&stHorsesEnd,sizeof(stHorsesEnd));
				stHorsesEnd.nTimeLeave = m_nFreeTime;
				stHorsesEnd.RecordRecord = m_GameRecords[m_GameRecords.GetCount() - 1];
				memcpy(stHorsesEnd.nWinCount, m_nWinCount, sizeof(stHorsesEnd.nWinCount));
				memcpy(stHorsesEnd.nMultiple, m_nMultiple, sizeof(stHorsesEnd.nMultiple));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HORSES_END, &stHorsesEnd, sizeof(stHorsesEnd));	

				//游戏玩家
				for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
				{
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem==NULL) continue;

					//设置积分
					memcpy(stHorsesEnd.lPlayerBet, m_lPlayerBet[wChairID], sizeof(stHorsesEnd.lPlayerBet));
					memcpy(stHorsesEnd.lPlayerWinning, m_lPlayerBetWin[wChairID], sizeof(stHorsesEnd.lPlayerWinning));

					m_pITableFrame->SendTableData(wChairID,SUB_S_HORSES_END,&stHorsesEnd,sizeof(stHorsesEnd));	
				}

				//结束游戏
			m_pITableFrame->ConcludeGame(GS_FREE);

				//设置状态
			//	m_pITableFrame->SetGameStatus(GS_FREE);
			}
			return true;
		case IDI_HORSES_END:		//跑马强制结束
			{
				//游戏玩家
				for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
				{
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem==NULL) 
						continue;

					m_pITableFrame->SendTableData(wChairID,SUB_S_MANDATOY_END, NULL ,0);	
				}
			}
			return true;
	}
	return false;
}

//游戏消息处理
bool CTableFrameSink:: OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
	case SUB_C_PLAYER_BET:
			return OnSubPlayBet(pData, wDataSize, pIServerUserItem);

	case SUB_C_ADMIN_COMMDN:
			return OnAdminControl(pData, wDataSize, pIServerUserItem);
	}
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( bLookonUser == false && m_dwGameTime == 0)
	{
		//设置种子
		srand(GetTickCount());

		//倍数获得
		MultipleControl();

		//获取时间
		m_dwGameTime = (DWORD)time(NULL);

		//开启定时器
		m_pITableFrame->SetGameTimer(IDI_FREE, m_nFreeTime*1000, 1, NULL);

		//设置状态
		m_pITableFrame->SetGameStatus(GS_FREE);
	}
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	return true;
}

//加注事件
bool CTableFrameSink::OnSubPlayBet(const void * pBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_PlayerBet));
	if (wDataSize!=sizeof(CMD_C_PlayerBet)) return false;

	//用户效验
	if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

	//消息处理
	CMD_C_PlayerBet * pPlayerBet = (CMD_C_PlayerBet *)pBuffer;

	//状态验证
	if ( m_pITableFrame->GetGameStatus() != GS_BET)
	{
		//发送下注失败消息
		CMD_S_PlayerBetFail stPlayerBetFail;
		stPlayerBetFail.cbFailType = FAIL_TYPE_TIME_OVER;
		stPlayerBetFail.wChairID = pIServerUserItem->GetChairID();
		memcpy(stPlayerBetFail.lBetScore, pPlayerBet->lBetScore, sizeof(stPlayerBetFail.lBetScore));
		m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(),SUB_S_PLAYER_BET_FAIL,&stPlayerBetFail,sizeof(stPlayerBetFail));
		return true;
	}

	//判断下注是否合法
	for ( int i = 0 ; i < AREA_ALL ; ++i)
	{
		if ( pPlayerBet->lBetScore[i] > GetPlayersMaxBet(pIServerUserItem->GetChairID(), i) )
		{
			//发送下注失败消息
			CMD_S_PlayerBetFail stPlayerBetFail;
			stPlayerBetFail.cbFailType = FAIL_TYPE_OVERTOP;
			stPlayerBetFail.wChairID = pIServerUserItem->GetChairID();
			memcpy(stPlayerBetFail.lBetScore, pPlayerBet->lBetScore, sizeof(stPlayerBetFail.lBetScore));
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(),SUB_S_PLAYER_BET_FAIL,&stPlayerBetFail,sizeof(stPlayerBetFail));
			return true;
		}
	}

	//机器人验证
	if(pIServerUserItem->IsAndroidUser())
	{
		for ( int i = 0 ; i < AREA_ALL; ++i )
		{
			if ( m_lRobotAreaScore[i] + pPlayerBet->lBetScore[i] > m_lRobotAreaLimit )
				return true;
		}

		//数目限制
		bool bHaveChip = false;
		for (int i = 0; i < AREA_ALL; i++)
		{
			if (m_lPlayerBet[pIServerUserItem->GetChairID()][i] != 0)
				bHaveChip = true;
		}

		if (!bHaveChip)
		{
			if ( m_nChipRobotCount + 1 > m_nMaxChipRobot )
				return true;
			else
				m_nChipRobotCount++;
		}
	}

	//添加人数
	LONGLONG lTempPlayerAllBet = 0l;
	for(int i = 0 ; i < AREA_ALL ; ++i)
		lTempPlayerAllBet += m_lPlayerBet[pIServerUserItem->GetChairID()][i];

	//玩家第一次下注,则添加人数
	if ( lTempPlayerAllBet == 0l )
		m_nBetPlayerCount++;

	//添加注
	for ( int i = 0 ; i < AREA_ALL ; ++i)
	{
		m_lPlayerBet[pIServerUserItem->GetChairID()][i] += pPlayerBet->lBetScore[i];
		m_lPlayerBetAll[i] += pPlayerBet->lBetScore[i];
	}

	//发送消息
	CMD_S_PlayerBet stPlayerBet;
	memcpy(stPlayerBet.lBetScore, pPlayerBet->lBetScore, sizeof(stPlayerBet.lBetScore));
	stPlayerBet.wChairID = pIServerUserItem->GetChairID();
	stPlayerBet.nBetPlayerCount = m_nBetPlayerCount;
	stPlayerBet.bIsAndroid=pIServerUserItem->IsAndroidUser();
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLAYER_BET,&stPlayerBet,sizeof(stPlayerBet));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAYER_BET,&stPlayerBet,sizeof(stPlayerBet));

	return true;
}

//控制
bool CTableFrameSink::OnAdminControl(const void * pBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_ControlApplication));
	if(wDataSize!=sizeof(CMD_C_ControlApplication)) 
		return false;

	//用户效验
	if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
		return false;

	//消息处理
	CMD_C_ControlApplication * pControlApplication=(CMD_C_ControlApplication *)pBuffer;

	switch(pControlApplication->cbControlAppType)
	{
	case C_CA_UPDATE:	//更新
		{
			CMD_S_ControlReturns ControlReturns;
			ZeroMemory(&ControlReturns,sizeof(ControlReturns));
			ControlReturns.cbReturnsType = S_CR_UPDATE_SUCCES;
			ControlReturns.cbControlTimes = m_cbCLTimes;
			ControlReturns.cbControlArea = m_cbCLArea;
			for (int i = 0 ; i < AREA_ALL; ++i)
				ControlReturns.nControlMultiple[i] = m_nCLMultiple[i];
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
		}
		return true;
	case C_CA_SET:		//设置
		{
			//设置变量
			m_cbCLTimes = pControlApplication->cbControlTimes;
			m_cbCLArea = pControlApplication->cbControlArea;
			for (int i = 0 ; i < AREA_ALL; ++i)
				m_nCLMultiple[i] = pControlApplication->nControlMultiple[i];

			m_bControl = pControlApplication->bAuthoritiesExecuted == TRUE;

			//定义变量
			CMD_S_ControlReturns ControlReturns;

			//设置变量
			ZeroMemory(&ControlReturns,sizeof(ControlReturns));
			ControlReturns.cbReturnsType = S_CR_SET_SUCCESS;
			ControlReturns.cbControlTimes = pControlApplication->cbControlTimes;
			ControlReturns.cbControlArea = pControlApplication->cbControlArea;
			ControlReturns.bAuthoritiesExecuted = pControlApplication->cbControlArea;
			for (int i = 0 ; i < AREA_ALL; ++i)
				ControlReturns.nControlMultiple[i] = pControlApplication->nControlMultiple[i];

			//发送消息
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
		}
		return true;
	case C_CA_CANCELS:	//取消
		{
			//设置变量
			m_cbCLTimes = 0;
			m_bControl = false;
			m_cbCLArea = 255;
			for (int i = 0 ; i < AREA_ALL; ++i)
				m_nCLMultiple[i] = -1;

			//定义变量
			CMD_S_ControlReturns ControlReturns;

			//设置变量
			ZeroMemory(&ControlReturns,sizeof(ControlReturns));
			ControlReturns.cbReturnsType = S_CR_CANCEL_SUCCESS;
			ControlReturns.cbControlArea = 255;
			ControlReturns.cbControlTimes = 0;
			for ( int i = 0; i < AREA_ALL; ++i)
				ControlReturns.nControlMultiple[i] = -1;

			//发送消息
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
		}
		return true;
	}
	return false;

}

//跑马过程
void CTableFrameSink::HorsesProcess()
{
	while(true)
	{
		HorsesStart();

		//先看结果是否符合常规条件或控制
		//再看积分最后是否正确
		if( HorsesEnd() && CalculateScore() )
			break ;
	}

	//记录
	m_nStreak++;
	CTime time = CTime::GetCurrentTime();					//获取当前时间. 
	int nDay = time.GetDay();								//天
	if( m_nStreak == INT_MAX || m_nDay != nDay )
	{
		m_nStreak = 1;
		m_nDay = nDay;
		ZeroMemory(m_nWinCount,sizeof(m_nWinCount));
	}

	tagHistoryRecord HistoryRecord;
	HistoryRecord.nStreak = m_nStreak;
	HistoryRecord.nRanking = m_cbGameResults;
	HistoryRecord.nRiskCompensate = m_nMultiple[m_cbGameResults];
	HistoryRecord.nHours = time.GetHour();
	HistoryRecord.nMinutes = time.GetMinute();
	HistoryRecord.nSeconds = time.GetSecond();
	m_nWinCount[m_cbHorsesRanking[RANKING_FIRST]]++;

	m_GameRecords.Add(HistoryRecord);
	if ( m_GameRecords.GetCount() > MAX_SCORE_HISTORY )
		m_GameRecords.RemoveAt(0);
}


//跑马开始
void CTableFrameSink::HorsesStart()
{

	//马匹信息
	ZeroMemory(m_nHorsesSpeed,sizeof(m_nHorsesSpeed));

	//------------------------------------------------------
	//此是得到每秒的速度.. 模糊速度.. 但是数据小.
	for ( int nHorses = 0 ; nHorses < HORSES_ALL; ++nHorses )
	{
		for ( int i = 0 ; i < COMPLETION_TIME; ++i)
		{
			//高速 
			if ( i % 2 == 0 )
			{
				m_nHorsesSpeed[nHorses][i] = (rand() + GetTickCount() + nHorses) % HIGH_SPEED + 10;
			}
			//低速
			else
			{
				m_nHorsesSpeed[nHorses][i] = (rand() + GetTickCount() + nHorses) % LOW_SPEED;
				m_nHorsesSpeed[nHorses][i] = -m_nHorsesSpeed[nHorses][i];
			}
		}
		m_nHorsesSpeed[nHorses][COMPLETION_TIME - 1] = m_nHorsesSpeed[nHorses][COMPLETION_TIME - 1] / 20 * 20;
	}

	//------------------------------------------------------
	//此是得到每30毫秒的速度.. 精确速度.. 但是由于数据太大. 不利传输.. 放弃.
	//for ( int nHorses = 0 ; nHorses < HORSES_ALL; ++nHorses )
	//{
	//	//速步索引
	//	int nSpeedIndex = 0;	
	//	//速度
	//	int nVelocity = 1;	
	//	//加速度
	//	int nAcceleration = ACCELERATION;
	//	//高速维持
	//	int nMaintenance = 0;
	//	//频率
	//	int nFrequency = (rand() + GetTickCount()*2 ) % FREQUENCY + 1;
	//	//最高速度
	//	int nHighestSpeed = (rand() + GetTickCount() + nHorses) % HIGH_SPEED;
	//	//最低速度
	//	int nMinimumSpeed = (rand() + GetTickCount() + nHorses) % LOW_SPEED;
	//	nMinimumSpeed = -nMinimumSpeed;

	//	if ( nHighestSpeed == nMinimumSpeed )
	//		nHighestSpeed = nMinimumSpeed + rand()%HIGH_SPEED + 1;

	//	//周期循环
	//	bool bCycle = false;
	//	bool bHighSpeedAppeared = false;
	//	while( nSpeedIndex < STEP_SPEED )
	//	{
	//		m_nHorsesSpeed[nHorses][nSpeedIndex] = nVelocity;

	//		//设置加速度
	//		if ( nVelocity >= nHighestSpeed )
	//		{
	//			bHighSpeedAppeared = true;
	//			nAcceleration = ((rand() + GetTickCount())%ACCELERATION + 1);
	//			nAcceleration = -nAcceleration;
	//		}
	//		else if ( nVelocity <= nMinimumSpeed )
	//		{
	//			nAcceleration = ((rand() + GetTickCount())%ACCELERATION + 2);
	//		}

	//		//设置下一步速度
	//		if( nMaintenance < nFrequency )
	//		{
	//			nMaintenance++;
	//		}
	//		else
	//		{
	//			nFrequency = (rand() + GetTickCount()*2 ) % (FREQUENCY/2) + 1;
	//			nMaintenance = 0;
	//			nVelocity += nAcceleration;
	//		}

	//		//一周期完成
	//		if ( nVelocity == nMinimumSpeed && bHighSpeedAppeared)
	//			bCycle = true;

	//		if ( bCycle )
	//		{
	//			bCycle = false;
	//			bHighSpeedAppeared = false;
	//			nAcceleration = 1;
	//			nMaintenance = 0;
	//			nFrequency = (rand() + GetTickCount()*2 ) % FREQUENCY + 1;
	//			nHighestSpeed = (rand() + GetTickCount() + nHorses) % HIGH_SPEED;
	//			nMinimumSpeed = (rand() + GetTickCount() + nHorses) % LOW_SPEED;
	//			nMinimumSpeed = -nMinimumSpeed;

	//			if ( nHighestSpeed == nMinimumSpeed )
	//				nHighestSpeed = nMinimumSpeed + rand()%HIGH_SPEED + 1;
	//		}

	//		nSpeedIndex++;
	//	}
	//}
}

//跑马结束
bool CTableFrameSink::HorsesEnd()
{
	//马速度和
	INT nSpeedTotal[HORSES_ALL] = {0};

	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		nSpeedTotal[nHorses] += m_nHorsesSpeed[nHorses][COMPLETION_TIME - 1];
	}

	//错误判断
	for ( int nHorsesX = 0; nHorsesX < HORSES_ALL; ++nHorsesX )
	{
		for (int nHorsesY = 0; nHorsesY < HORSES_ALL; ++nHorsesY)
		{
			//如果有相等的就错误
			if ( nSpeedTotal[nHorsesX] == nSpeedTotal[nHorsesY] && nHorsesX != nHorsesY )
			{
				return false;
			}
		}
	}

	//找出名次
	INT nFirst = HORSES_ALL;
	INT nSecond = HORSES_ALL;
	INT nThird = HORSES_ALL;
	INT nFourth = HORSES_ALL;
	INT nFifth = HORSES_ALL;
	INT nSixth = HORSES_ALL;
	
	//第一名
	INT nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] )
		{
			nTemp = nSpeedTotal[nHorses];
			nFirst = nHorses;
		}
	}

	//第二名
	nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nHorses != nFirst && ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] ))
		{
			nTemp = nSpeedTotal[nHorses];
			nSecond = nHorses;
		}
	}

	//第三名
	nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nHorses != nFirst && nHorses != nSecond && ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] ))
		{
			nTemp = nSpeedTotal[nHorses];
			nThird = nHorses;
		}
	}

	//第四名
	nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nHorses != nFirst &&  nHorses != nSecond && nHorses != nThird && ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] ))
		{
			nTemp = nSpeedTotal[nHorses];
			nFourth = nHorses;
		}
	}

	//第五名
	nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nHorses != nFirst && nHorses != nSecond && nHorses != nThird && nHorses != nFourth && ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] ))
		{
			nTemp = nSpeedTotal[nHorses];
			nFifth = nHorses;
		}
	}

	//第六名
	nTemp = 0;
	for ( int nHorses = 0; nHorses < HORSES_ALL; ++nHorses )
	{
		if ( nHorses != nFirst && nHorses != nSecond && nHorses != nThird && nHorses != nFourth && nHorses != nFifth && ( nTemp == 0 || nTemp < nSpeedTotal[nHorses] ))
		{
			nTemp = nSpeedTotal[nHorses];
			nSixth = nHorses;
		}
	}

	//错误判断
	if( nFirst == HORSES_ALL || nSecond == HORSES_ALL || nThird == HORSES_ALL 
		|| nFourth == HORSES_ALL || nFifth == HORSES_ALL || nSixth == HORSES_ALL)
	{
		ASSERT(FALSE);
		return false;
	}

	//马匹信息
	for ( int i = 0; i < CountArray(m_cbHorsesRanking); ++i)
		m_cbHorsesRanking[i] = HORSES_ALL;

	m_cbHorsesRanking[RANKING_FIRST] = nFirst;
	m_cbHorsesRanking[RANKING_SECOND] = nSecond;
	m_cbHorsesRanking[RANKING_THIRD] = nThird;
	m_cbHorsesRanking[RANKING_FOURTH] = nFourth;
	m_cbHorsesRanking[RANKING_FIFTH] = nFifth;
	m_cbHorsesRanking[RANKING_SIXTH] = nSixth;

	//最后结果是否可行
	return FinalResults();
}


//最后结果
bool CTableFrameSink::FinalResults()
{
	//游戏结果
	if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_ONE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_SIX)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_ONE&& m_cbHorsesRanking[RANKING_FIRST] == HORSES_SIX)  )
	{
		m_cbGameResults = AREA_1_6;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_ONE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FIVE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_ONE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FIVE)  )
	{
		m_cbGameResults = AREA_1_5;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_ONE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FOUR)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_ONE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FOUR)  )
	{
		m_cbGameResults = AREA_1_4;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_ONE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_THREE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_ONE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_THREE)  )
	{
		m_cbGameResults = AREA_1_3;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_ONE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_TWO)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_ONE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_TWO)  )
	{
		m_cbGameResults = AREA_1_2;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_TWO && m_cbHorsesRanking[RANKING_SECOND] == HORSES_SIX)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_TWO && m_cbHorsesRanking[RANKING_FIRST] == HORSES_SIX)  )
	{
		m_cbGameResults = AREA_2_6;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_TWO && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FIVE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_TWO && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FIVE)  )
	{
		m_cbGameResults = AREA_2_5;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_TWO && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FOUR)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_TWO && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FOUR)  )
	{
		m_cbGameResults = AREA_2_4;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_TWO && m_cbHorsesRanking[RANKING_SECOND] == HORSES_THREE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_TWO && m_cbHorsesRanking[RANKING_FIRST] == HORSES_THREE)  )	
	{
		m_cbGameResults = AREA_2_3;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_THREE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_SIX)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_THREE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_SIX)  )	
	{
		m_cbGameResults = AREA_3_6;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_THREE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FIVE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_THREE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FIVE)  )	
	{
		m_cbGameResults = AREA_3_5;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_THREE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FOUR)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_THREE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FOUR)  )	
	{
		m_cbGameResults = AREA_3_4;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_FOUR && m_cbHorsesRanking[RANKING_SECOND] == HORSES_SIX)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_FOUR && m_cbHorsesRanking[RANKING_FIRST] == HORSES_SIX)  )	
	{
		m_cbGameResults = AREA_4_6;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_FOUR && m_cbHorsesRanking[RANKING_SECOND] == HORSES_FIVE)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_FOUR && m_cbHorsesRanking[RANKING_FIRST] == HORSES_FIVE)  )	
	{
		m_cbGameResults = AREA_4_5;
	}
	else if ( (m_cbHorsesRanking[RANKING_FIRST] == HORSES_FIVE && m_cbHorsesRanking[RANKING_SECOND] == HORSES_SIX)
		||(m_cbHorsesRanking[RANKING_SECOND] == HORSES_FIVE && m_cbHorsesRanking[RANKING_FIRST] == HORSES_SIX)  )	
	{
		m_cbGameResults = AREA_5_6;
	}
	else
	{
		ASSERT(FALSE);
		return false;
	}

	if (NeedControl())
	{
		return MeetControl();
	}

	return true;
}

//是否需要控制
bool CTableFrameSink::NeedControl()
{
	if ( m_cbCLTimes > 0 && m_bControl )
	{
		return true;
	}
	return false;
}

//是否满足控制
bool CTableFrameSink::MeetControl()
{
	if ( m_cbCLArea != 255 && m_cbCLArea >= AREA_1_6 && m_cbCLArea < AREA_ALL && m_cbGameResults != m_cbCLArea )
	{
		return false;
	}

	return true;
}

//完成控制
bool CTableFrameSink::CompleteControl()
{
	m_cbCLTimes--;
	if ( m_cbCLTimes == 0 )
	{
		m_cbCLTimes = 0;
		m_bControl = false;
		m_cbCLArea = 255;
		for (int i = 0 ; i < AREA_ALL; ++i)
			m_nCLMultiple[i] = -1;
	}
	return true;
}

//结果计算
bool CTableFrameSink::CalculateScore()
{
	//玩家输赢
	LONGLONG lPlayerLost[GAME_PLAYER];
	ZeroMemory(lPlayerLost, sizeof(lPlayerLost));
	ZeroMemory(m_lPlayerWinning,sizeof(m_lPlayerWinning));
	ZeroMemory(m_lPlayerReturnBet,sizeof(m_lPlayerReturnBet));
	ZeroMemory(m_lPlayerRevenue,sizeof(m_lPlayerRevenue));
	ZeroMemory(m_lPlayerBetWin,sizeof(m_lPlayerBetWin));
	

	//系统输赢
	LONGLONG lSystemScore = 0l;

	//计算积分
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

		//每个人定注数
		for (WORD wAreaIndex = 0; wAreaIndex < AREA_ALL; ++wAreaIndex)
		{
			if ( m_cbGameResults == wAreaIndex )
			{
				m_lPlayerWinning[wChairID] += m_lPlayerBet[wChairID][wAreaIndex] * (m_nMultiple[wAreaIndex] - 1);
				m_lPlayerReturnBet[wChairID] += m_lPlayerBet[wChairID][wAreaIndex];
				m_lPlayerBetWin[wChairID][wAreaIndex] = m_lPlayerBet[wChairID][wAreaIndex] * m_nMultiple[wAreaIndex];

				//系统得分
				if(!bIsAndroidUser)
					lSystemScore -= (m_lPlayerBet[wChairID][wAreaIndex] * (m_nMultiple[wAreaIndex] - 1));
			}
			else
			{
				lPlayerLost[wChairID] -= m_lPlayerBet[wChairID][wAreaIndex];
				m_lPlayerBetWin[wChairID][wAreaIndex] = -m_lPlayerBet[wChairID][wAreaIndex];

				//系统得分
				if(!bIsAndroidUser)
					lSystemScore += m_lPlayerBet[wChairID][wAreaIndex];
			}
		}

		//总的分数
		m_lPlayerWinning[wChairID] += lPlayerLost[wChairID];

		//计算税收
		if (0 < m_lPlayerWinning[wChairID])
		{
			DOUBLE fRevenuePer = DOUBLE( (DOUBLE)m_pGameServiceOption->wRevenueRatio / (DOUBLE)1000.000000 );
			m_lPlayerRevenue[wChairID]  = LONGLONG(m_lPlayerWinning[wChairID] * fRevenuePer);
			m_lPlayerWinning[wChairID] -= m_lPlayerRevenue[wChairID];
		}
	}

	if ( NeedControl() )
	{
		return CompleteControl();
	}


	//系统分值计算
	if ((lSystemScore + m_StorageStart) < 0l)
	{
		return false;
	}
	else
	{
		m_StorageStart += lSystemScore;
		m_StorageStart = m_StorageStart - (m_StorageStart * m_StorageDeduct / 1000);
		return true;
	}
}

//玩家最大下分
LONGLONG CTableFrameSink::GetPlayersMaxBet( WORD wChiarID, BYTE cbArea /*= AREA_ALL*/)
{
	//找出玩家
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChiarID);
	if ( pIServerUserItem == NULL || cbArea > AREA_ALL ) 
		return 0l;

	//玩家分数
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//减去自己已下注
	for(int i = 0; i < AREA_ALL; ++i)
	{
		lUserScore -= m_lPlayerBet[wChiarID][i];
	}

	//如果是区域下注. 还要判断区域限制
	if ( cbArea < AREA_ALL )
	{
		//个人区域限制
		LONGLONG lUserLimit = m_lUserLimitScore - m_lPlayerBet[wChiarID][cbArea];
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

//倍数控制
void CTableFrameSink::MultipleControl()
{
	//倍数控制
	//----------------------------
	INT nControl = GetPrivateProfileInt(m_szGameRoomName, TEXT("MultipleControl"), 0, m_szConfigFileName);
	m_bMultipleControl == nControl == 1 ? TRUE : FALSE;

	//需要控制
	if(m_bMultipleControl)
	{
		m_nMultiple[AREA_1_6] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_1_6"), 1, m_szConfigFileName);
		m_nMultiple[AREA_1_5] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_1_5"), 1, m_szConfigFileName);
		m_nMultiple[AREA_1_4] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_1_4"), 1, m_szConfigFileName);
		m_nMultiple[AREA_1_3] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_1_3"), 1, m_szConfigFileName);
		m_nMultiple[AREA_1_2] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_1_2"), 1, m_szConfigFileName);
		m_nMultiple[AREA_2_6] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_2_6"), 1, m_szConfigFileName);
		m_nMultiple[AREA_2_5] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_2_5"), 1, m_szConfigFileName);
		m_nMultiple[AREA_2_4] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_2_4"), 1, m_szConfigFileName);
		m_nMultiple[AREA_2_3] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_2_3"), 1, m_szConfigFileName);
		m_nMultiple[AREA_3_6] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_3_6"), 1, m_szConfigFileName);
		m_nMultiple[AREA_3_5] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_3_5"), 1, m_szConfigFileName);
		m_nMultiple[AREA_3_4] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_3_4"), 1, m_szConfigFileName);
		m_nMultiple[AREA_4_6] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_4_6"), 1, m_szConfigFileName);
		m_nMultiple[AREA_4_5] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_4_5"), 1, m_szConfigFileName);
		m_nMultiple[AREA_5_6] = GetPrivateProfileInt(m_szGameRoomName, TEXT("AREA_5_6"), 1, m_szConfigFileName);

		for ( int i = 0;  i < AREA_ALL; ++i)
		{
			if ( m_nMultiple[i] < 1 )
			{
				m_nMultiple[i] = 1;
			}
		}
	}
	else
	{
		//随机换倍数
		RandomMultiples();
	}

	//管理员控制
	if ( NeedControl() )
	{
		for ( int i = 0; i < AREA_ALL; ++i)
		{
			if ( m_nCLMultiple[i] > 0 )
			{
				m_nMultiple[i] = m_nCLMultiple[i];
			}
		}
	}
}

//随机获得倍数
void CTableFrameSink::RandomMultiples()
{
	DWORD wTick = GetTickCount();
	INT nMultiples[]	=					{ 3, 4, 5, 6, 7, 8, 9, 10, 18, 20, 25, 28, 30, 35, 38, 40, 45 };//倍数
	INT nChance[CountArray(nMultiples)] =	{ 6, 6, 6, 6, 6, 6, 6, 4,  4,  3,  3,  3,  2,  2,  2,  1,  1 };	//几率
	INT nBigAreaMultiples[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };							//大倍几率

	//混乱
	ChaosArray(nBigAreaMultiples, CountArray(nBigAreaMultiples));
	ChaosArray(nMultiples, CountArray(nMultiples), nChance, CountArray(nChance));

	//大倍率
	INT nBigArea = nBigAreaMultiples[(rand() + wTick)%CountArray(nBigAreaMultiples)];
	m_nMultiple[nBigArea] = 88;

	//随机倍数
	for ( int i = 0; i < AREA_ALL; ++i )
	{
		if ( i == nBigArea )
			continue;

		//几率和值
		INT nChanceAndValue = 0;
		for ( int n = 0; n < CountArray(nChance); ++n )
			nChanceAndValue += nChance[n];

		INT nMuIndex = 0;
		int nRandNum = 0;					//随机辅助
		static int nStFluc = 1;				
		nRandNum = (rand() + wTick + nStFluc*3 + i) % nChanceAndValue;
		for (int j = 0; j < CountArray(nChance); j++)
		{
			nRandNum -= nChance[j];
			if (nRandNum < 0)
			{
				nMuIndex = j;
				break;
			}
		}
		nStFluc = nStFluc%3 + 1;

		m_nMultiple[i] = nMultiples[nMuIndex];
		nChance[nMuIndex] = 0;
	}
}

//混乱数组
void CTableFrameSink::ChaosArray( INT nArray[], INT nCount )
{
	DWORD wTick = GetTickCount();
	for (int i = 0; i < nCount; ++i)
	{
		int nTempIndex = (rand()+wTick)%nCount;
		int nTempValue = nArray[i];
		nArray[i] = nArray[nTempIndex];
		nArray[nTempIndex] = nTempValue;
	}
}

//混乱数组
void CTableFrameSink::ChaosArray( INT nArrayOne[], INT nCountOne, INT nArrayTwo[], INT nCountTwo )
{
	ASSERT( nCountOne == nCountTwo );
	if( nCountTwo != nCountOne )
		return;

	DWORD wTick = GetTickCount();
	for (int i = 1; i < nCountOne; ++i)
	{
		int nTempIndex = (rand()+wTick)%nCountOne;

		int nTempValueOne = nArrayOne[i];
		nArrayOne[i] = nArrayOne[nTempIndex];
		nArrayOne[nTempIndex] = nTempValueOne;

		int nTempValueTwo = nArrayTwo[i];
		nArrayTwo[i] = nArrayTwo[nTempIndex];
		nArrayTwo[nTempIndex] = nTempValueTwo;
	}
}

//////////////////////////////////////////////////////////////////////////

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{

	return true;
}
//////////////////////////////////////////////////////////////////////////