#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_PLACE_JETTON1			103									//下注定时
#define IDI_PLACE_JETTON2			104									//下注定时
#define IDI_PLACE_JETTON3			105									//下注定时
#define IDI_PLACE_JETTON4			106									//下注定时
#define IDI_PLACE_JETTON5			107									//下注定时
#define IDI_CHECK_BANKER			108									//检查上庄
#define IDI_REQUEST_BANKER			101									//申请定时
#define IDI_GIVEUP_BANKER			102									//下庄定时
#define IDI_PLACE_JETTON			110									//下注定义 (预留110-160)

//////////////////////////////////////////////////////////////////////////


//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_lMaxChipUser = 0;
	m_wCurrentBanker = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	m_cbTimeLeave = 0;
	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool  CAndroidUserItemSink::RepositionSink()
{
	//游戏变量
	m_lMaxChipUser = 0;
	m_wCurrentBanker = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	m_cbTimeLeave = 0;
	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	if (nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON+MAX_CHIP_TIME)
	{
		srand(GetTickCount());

		//变量定义
		int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
		LONGLONG lMaxChipLmt = m_lMaxChipUser;			//最大可下注值
		WORD wMyID = m_pIAndroidUserItem->GetChairID();
		for (int i = 0; i < AREA_ALL; i++)
			nACTotal += m_RobotInfo.nAreaChance[i];

		//统计次数
		m_nChipTimeCount++;

		//检测退出
		if (lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])	return false;
		for (int i = 0; i < AREA_ALL; i++)
		{
			if (m_lAreaChip[i] >= m_RobotInfo.nChip[m_nChipLimit[0]])	break;
			if (i == AREA_ALL - 1)	return false;
		}

		//下注区域
		ASSERT(nACTotal>0);
		static int nStFluc = 1;				//随机辅助
		if (nACTotal <= 0)	return false;
		do {
			nRandNum = (rand()+wMyID+nStFluc*3) % nACTotal;
			for (int i = 0; i < AREA_ALL; i++)
			{
				nRandNum -= m_RobotInfo.nAreaChance[i];
				if (nRandNum < 0)
				{
					nChipArea = i;
					break;
				}
			}
		}
		while (m_lAreaChip[nChipArea] < m_RobotInfo.nChip[m_nChipLimit[0]]);
		nStFluc = nStFluc%3 + 1;

		//下注大小
		if (m_nChipLimit[0] == m_nChipLimit[1])
			nCurChip = m_nChipLimit[0];
		else
		{
			//设置变量
			lMaxChipLmt = __min(lMaxChipLmt, m_lAreaChip[nChipArea]);
			nCurJetLmt[0] = m_nChipLimit[0];
			nCurJetLmt[1] = m_nChipLimit[0];

			//计算当前最大筹码
			for (int i = m_nChipLimit[1]; i > m_nChipLimit[0]; i--)
			{
				if (lMaxChipLmt > m_RobotInfo.nChip[i]) 
				{
					nCurJetLmt[1] = i;
					break;
				}
			}
	
			//随机下注
			nRandNum = (rand()+wMyID) % (nCurJetLmt[1]-nCurJetLmt[0]+1);
			nCurChip = nCurJetLmt[0] + nRandNum;

			//多下控制 (当庄家金币较少时会尽量保证下足次数)
			if (m_nChipTimeCount < m_nChipTime)
			{
				LONGLONG lLeftJetton = LONGLONG( (lMaxChipLmt-m_RobotInfo.nChip[nCurChip])/(m_nChipTime-m_nChipTimeCount) );

				//不够次数 (即全用最小限制筹码下注也少了)
				if (lLeftJetton < m_RobotInfo.nChip[m_nChipLimit[0]] && nCurChip > m_nChipLimit[0])
					nCurChip--;
			}
		}

		LONGLONG Gold = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();		

		int nMaxIndex=0;			

		for (int i=0; i < CountArray(m_RobotInfo.nChip); i++)
			if (Gold>m_RobotInfo.nChip[i]) nMaxIndex=i;//

		nCurChip=rand()%(nMaxIndex+1);
		//变量定义
		CMD_C_PlayerBet stPlayerBet;
		ZeroMemory(&stPlayerBet, sizeof(stPlayerBet));
		stPlayerBet.lBetScore[nChipArea] = m_RobotInfo.nChip[nCurChip];

		//发送消息
		m_pIAndroidUserItem->SendSocketData(SUB_C_PLAYER_BET, &stPlayerBet, sizeof(stPlayerBet));
	}

	m_pIAndroidUserItem->KillGameTimer(nTimerID);
	return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_S_BET_START:			//开始下注
		return OnSubBetStart(pBuffer, wDataSize);

	case SUB_S_BET_END:				//下注结束
		return true;

	case SUB_S_HORSES_START:		//跑马开始
		return true;

	case SUB_S_HORSES_END:			//跑马结束
		return true;

	case SUB_S_PLAYER_BET:			//用户下注
		return OnSubPlayerBet(pBuffer, wDataSize);

	case SUB_S_PLAYER_BET_FAIL:		//下注失败
		return true;

	case SUB_S_NAMED_HORSES:		//马屁冠名
		return true;

	case SUB_S_MANDATOY_END:		//强制结束
		return true;

	case SUB_S_ADMIN_COMMDN:		//设置
		return true;

	}
	//错误断言
	ASSERT(FALSE);
	return true;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneFree));
			if (wDataSize!=sizeof(CMD_S_SceneFree)) return false;

			//消息处理
			CMD_S_SceneFree * pSceneFree=(CMD_S_SceneFree *)pData;

			m_lUserLimitScore = pSceneFree->lUserLimitScore;
			m_lAreaLimitScore = pSceneFree->lAreaLimitScore;

			memcpy(m_szRoomName, pSceneFree->szGameRoomName, sizeof(m_szRoomName));
			ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);
			return true;
		}
	case GS_BET:	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneBet));
			if (wDataSize!=sizeof(CMD_S_SceneBet)) return false;

			//消息处理
			CMD_S_SceneBet * pSceneBet=(CMD_S_SceneBet *)pData;

			m_lUserLimitScore = pSceneBet->lUserLimitScore;
			m_lAreaLimitScore = pSceneBet->lAreaLimitScore;
			memcpy(m_szRoomName, pSceneBet->szGameRoomName, sizeof(m_szRoomName));

			ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);
			return true;
		}
	case GS_BET_END:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneBetEnd));
			if (wDataSize!=sizeof(CMD_S_SceneBetEnd)) return false;

			//消息处理
			CMD_S_SceneBetEnd * pSceneBetEnd=(CMD_S_SceneBetEnd *)pData;

			m_lUserLimitScore = pSceneBetEnd->lUserLimitScore;
			m_lAreaLimitScore = pSceneBetEnd->lAreaLimitScore;
			memcpy(m_szRoomName, pSceneBetEnd->szGameRoomName, sizeof(m_szRoomName));

			ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);
			return true;
		}
	case GS_HORSES:	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_SceneHorses));
			if (wDataSize!=sizeof(CMD_S_SceneHorses)) return false;

			//消息处理
			CMD_S_SceneHorses * pSceneHorses=(CMD_S_SceneHorses *)pData;

			m_lUserLimitScore = pSceneHorses->lUserLimitScore;
			m_lAreaLimitScore = pSceneHorses->lAreaLimitScore;
			memcpy(m_szRoomName, pSceneHorses->szGameRoomName, sizeof(m_szRoomName));

			ReadConfigInformation(m_RobotInfo.szCfgFileName, m_szRoomName, true);
			return true;
		}
	}

	return true;
}

//用户进入
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}


//开始下注
bool CAndroidUserItemSink::OnSubBetStart(const void * pBuffer, WORD wDataSize)
{	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BetStart));
	if (wDataSize!=sizeof(CMD_S_BetStart)) return false;

	//消息处理
	CMD_S_BetStart * pGameStart=(CMD_S_BetStart *)pBuffer;

	srand(GetTickCount());

	//自己当庄或无下注机器人
	if ( pGameStart->nChipRobotCount <= 0 )
		return true;

	//设置变量
	m_lMaxChipUser = __min(m_lUserLimitScore, m_pIAndroidUserItem->GetMeUserItem()->GetUserScore());
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	for (int i = 0; i < AREA_ALL; i++)
		m_lAreaChip[i] = m_lAreaLimitScore;

	//计算下注次数
	int nElapse = 0;												
	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
		m_nChipTime = m_nRobotBetTimeLimit[0];
	else
		m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
	ASSERT(m_nChipTime>=0);		
	if (m_nChipTime <= 0)	return false;								//的确,2个都带等于
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//限定MAX_CHIP次下注

	//计算范围
	if (!CalcJettonRange(m_lMaxChipUser, m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
		return true;

	//设置时间
	int nTimeGrid = int(pGameStart->nTimeLeave-2)*800/m_nChipTime;		//时间格,前2秒不下注,所以-2,800表示机器人下注时间范围千分比
	for (int i = 0; i < m_nChipTime; i++)
	{
		int nRandRage = int( nTimeGrid * i / (1500*sqrt((double)m_nChipTime)) ) + 1;		//波动范围
		nElapse = 2 + (nTimeGrid*i)/1000 + ( (rand()+wMyID)%(nRandRage*2) - (nRandRage-1) );
		ASSERT(nElapse>=2&&nElapse<=pGameStart->nTimeLeave);
		if (nElapse < 2 || nElapse > pGameStart->nTimeLeave)	continue;
		
		m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
	}
	return true;
}

//用户加注
bool CAndroidUserItemSink::OnSubPlayerBet(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayerBet));
	if (wDataSize!=sizeof(CMD_S_PlayerBet)) return false;

	//消息处理
	CMD_S_PlayerBet * pPlayerBet=(CMD_S_PlayerBet *)pBuffer;

	//设置变量
	for(int i = 0; i < AREA_ALL; ++i)
	{
		m_lAreaChip[i] -= pPlayerBet->lBetScore[i];
	}
	if (pPlayerBet->wChairID == m_pIAndroidUserItem->GetChairID())
	{
		for(int i = 0; i < AREA_ALL; ++i)
		{
			m_lMaxChipUser -= pPlayerBet->lBetScore[i];
		}
	}

	return true;
}


//读取配置
void CAndroidUserItemSink::ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh)
{
	//设置文件名
	TCHAR szPath[MAX_PATH] = TEXT("");
	TCHAR szConfigFileName[MAX_PATH] = TEXT("");
	TCHAR OutBuf[255] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	_sntprintf(szConfigFileName, sizeof(szConfigFileName), _T("%s\\%s"), szPath, szFileName);

	//筹码限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("RobotMaxJetton"), _T("100000"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lRobotJettonLimit[1]);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(szRoomName, TEXT("RobotMinJetton"), _T("100"), OutBuf, 255, szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lRobotJettonLimit[0]);

	if (m_lRobotJettonLimit[1] > 5000000)					m_lRobotJettonLimit[1] = 5000000;
	if (m_lRobotJettonLimit[0] < 1000)						m_lRobotJettonLimit[0] = 100;
	if (m_lRobotJettonLimit[1] < m_lRobotJettonLimit[0])	m_lRobotJettonLimit[1] = m_lRobotJettonLimit[0];

	//次数限制
	m_nRobotBetTimeLimit[0] = GetPrivateProfileInt(szRoomName, _T("RobotMinBetTime"), 4, szConfigFileName);;
	m_nRobotBetTimeLimit[1] = GetPrivateProfileInt(szRoomName, _T("RobotMaxBetTime"), 8, szConfigFileName);;

	if (m_nRobotBetTimeLimit[0] < 0)							m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0])		m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];
}

//计算范围	(返回值表示是否可以通过下降下限达到下注)
bool CAndroidUserItemSink::CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[])
{
	//定义变量
	bool bHaveSetMinChip = false;

	//不够一注
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//配置范围
	for (int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
	{
		if (!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
		{ 
			lJetLmt[0] = i;
			bHaveSetMinChip = true;
		}
		if (m_RobotInfo.nChip[i] <= lChipLmt[1])
			lJetLmt[1] = i;
	}
	if (lJetLmt[0] > lJetLmt[1])	lJetLmt[0] = lJetLmt[1];

	return true;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
