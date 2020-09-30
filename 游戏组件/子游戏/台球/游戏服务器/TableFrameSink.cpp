#include "StdAfx.h"
#include "TableFrameSink.h"


//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数


//////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量	 	 
	m_wCurrentUser=INVALID_CHAIR; 
	m_baseScore = 0;	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;	
	m_bHit = false;
	m_roundCount = 0;
	m_bGameStart = false;
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
VOID * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	//m_pITableFrame=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ITableFrame);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	m_baseScore = m_pGameServiceOption->lCellScore;

	return true;
}

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//游戏变量
	//m_wDouble=1;
	//m_bLandScore=0;
	//m_wBankerUser=INVALID_CHAIR;
	//m_wCurrentUser=INVALID_CHAIR;

	//memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));
	//ZeroMemory(m_byPeasantDouble,sizeof(m_byPeasantDouble));
	////运行信息
	//m_bTurnCardCount=0;
	//m_wTurnWiner=INVALID_CHAIR;
	//memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	////扑克信息
	//memset(m_bBackCard,0,sizeof(m_bBackCard));
	//memset(m_bCardCount,0,sizeof(m_bCardCount));
	//memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	//memset(m_bHandCardData,0,sizeof(m_bHandCardData));


	return;
}

 


//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//设置状态
 	const float yups =25*0.9160256f;  //*3BALLDIAMETER* _/-3/2
	m_ptBallPos[0].x = 222;
	m_ptBallPos[0].y = 244;
	float banjin = 12.5;

	m_currentComob = 0;
	FLOAT bp8[][3]={{0,0,0}
	,{0,0,1},
	{yups,banjin,4},{yups,-banjin,9},
	{yups*2,banjin*2,7},{yups*2,0,8},{yups*2,-banjin*2,3}
	,{yups*3,banjin*3,2},{yups*3,banjin,6},{yups*3,-banjin,5},{yups*3,-banjin*3,10}
	,{yups*4,banjin*4,15},{yups*4,banjin*2,13},{yups*4,0,11},{yups*4,-banjin*2,14},{yups*4,-banjin*4,12}};
	for(int i=1;i<BALL_NUM;i++)
	{
		m_ptBallPos[(int)bp8[i][2]].x = bp8[i][0]+577;
		m_ptBallPos[(int)bp8[i][2]].y = bp8[i][1]+222;
	}
	m_bHit = false;
	m_roundCount ++;
	ZeroMemory(m_cbPlayOutTimeNum,sizeof(m_cbPlayOutTimeNum));
	ZeroMemory(m_bWrongHit,sizeof(m_bWrongHit));
	ZeroMemory(m_ballNum,sizeof(m_ballNum));
	ZeroMemory(m_comob,sizeof(m_comob));
	ZeroMemory(m_bSendHitResult,sizeof(m_bSendHitResult));
	ZeroMemory(&m_hitBall,sizeof(m_hitBall));
	ZeroMemory(&m_hitResult,sizeof(m_hitResult));
	m_userBallType[0] = 100;
	m_userBallType[1] = 100;
	m_wCurrentUser = rand()%GAME_PLAYER;
	m_bFirstBall = true;
	ZeroMemory(m_ball,sizeof(m_ball));
	////设置用户	 	
	IServerUserItem *pServerUserItem = NULL;
	//	
	CMD_S_GameStart gameStart;
	gameStart.currentUser = m_wCurrentUser;
	gameStart.nBaseScore =  m_baseScore;
	gameStart.roundCount = m_roundCount;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		pServerUserItem = m_pITableFrame->GetTableUserItem( i ) ;		 
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&gameStart,sizeof(gameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&gameStart,sizeof(gameStart));
	}
	m_bGameStart = true;
	m_pITableFrame->SetGameTimer(IDI_SET_BASEBALL,TIME_SET_BASEBALL+5000,1,NULL);
	m_pITableFrame->SetGameStatus(GS_WK_SET_SETBASEBALL);	 
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	m_pITableFrame->SetGameStatus(GS_WK_FREE);
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			m_pITableFrame->KillGameTimer(IDI_SET_BASEBALL);
			m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
			m_pITableFrame->KillGameTimer(IDI_WAIT_SENDRESULT);
			m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
			////定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.endReason = m_endReason;
			//得分==底分+连击数之差*系数(低分/10); wChairID,赢的id
			int index = m_baseScore/10;
			if(index<1)
				index = 1;
			BYTE nextChair = (wChairID+1)%GAME_PLAYER;
			GameEnd.nGameScore[wChairID] = m_baseScore + (m_comob[wChairID]-m_comob[nextChair])*index;
			GameEnd.nGameScore[nextChair] = 0-GameEnd.nGameScore[wChairID];
		 		  
			//	//计算游戏得分

			for(BYTE i =0; i< GAME_PLAYER; i++)
			{
				GameEnd.comobNum[i] = this->m_comob[i];
			}

			BYTE temp;
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];

			temp = GameEnd.nGameScore[wChairID]>0? SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
			ScoreInfoArray[wChairID].lScore = GameEnd.nGameScore[wChairID];
			ScoreInfoArray[wChairID].cbType = temp;
			ScoreInfoArray[wChairID].lRevenue = m_pITableFrame->CalculateRevenue(wChairID,ScoreInfoArray[wChairID].lScore);
			
			temp = GameEnd.nGameScore[nextChair]>0? SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
			ScoreInfoArray[nextChair].lScore = GameEnd.nGameScore[nextChair];
			ScoreInfoArray[nextChair].cbType = temp;
			ScoreInfoArray[nextChair].lRevenue = m_pITableFrame->CalculateRevenue(nextChair,ScoreInfoArray[nextChair].lScore);
			
			m_pITableFrame->WriteTableScore(ScoreInfoArray,GAME_PLAYER);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			
			m_bGameStart = false;
			return true;
		}
	case	GER_USER_LEAVE:		//用户强退
	case	GER_DISMISS://游戏解散
	case	GER_NETWORK_ERROR:
		{
			
			m_pITableFrame->KillGameTimer(IDI_SET_BASEBALL);
			m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
			m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
			////定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.endReason = m_endReason;

			for(BYTE i =0; i< GAME_PLAYER; i++)
			{
				GameEnd.comobNum[i] =0;
			}

//			m_pITableFrame->WriteUserScore(WriteScoreInfo);
  	
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			m_pITableFrame->SetGameStatus(GS_WK_FREE);
			m_bGameStart = false; 
			return true;
		}
	}
	ASSERT(FALSE);
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)

{	
	//发送游戏配置
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{		
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
			//设置变量
			StatusFree.wServerType = m_pGameServiceOption->wServerType;
			//发送场景
			m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
			return true;
		}
	case GS_WK_SET_SETBASEBALL:		 
	case GS_WK_WAIT_HITBALL:
		{
			CMD_S_StatusGamePlay  statusGamePlay;
			statusGamePlay.currentUser = m_wCurrentUser;
			statusGamePlay.gameStatus = m_pITableFrame->GetGameStatus();
			statusGamePlay.baseScore = m_baseScore;
			memcpy(statusGamePlay.ptBall,m_ptBallPos,sizeof(m_ptBallPos));
			for(int i=0;i<16;i++)
			{
				statusGamePlay.ballState[i] = true;
			}
			for(int i=0;i<2;i++)
			{
				for(int j=0;j<m_ballNum[i];j++)
				{
					statusGamePlay.inHoleBall[i][j] = m_ball[i][j];
					statusGamePlay.ballState[m_ball[i][j]] = false;
				}
				statusGamePlay.inHoleBall[i][7] = m_ballNum[i];
				statusGamePlay.maxComob[i] = m_comob[i];
				statusGamePlay.userBallType[i] = m_userBallType[i];
			}
			m_pITableFrame->SendGameScene(pIServerUserItem,&statusGamePlay,sizeof(statusGamePlay));
			if(cbGameStatus==GS_WK_WAIT_HITBALL&&m_bHit&&m_bSendHitResult[0]==false&&m_bSendHitResult[1]==false)
			{//等待击球,并且已经击过球了、双方都没有收到击球的结果。这是发送击球的消息
				m_pITableFrame->SendTableData(wChairID,SUB_S_HITBALL,&m_hitBall,sizeof(CMD_S_HitBall));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HITBALL,&m_hitBall,sizeof(CMD_S_HitBall));
			}
			return true;
		}
 	case GS_WK_PLAYING:		//游戏状态
		{
			return true;
		}
	}

	return false;
}


//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{

	switch (dwTimerID)
	{
	case IDI_SET_BASEBALL:
		{
			if(m_bGameStart==false)
				return true;
			if(m_pITableFrame->GetGameStatus()!=GS_WK_SET_SETBASEBALL)
			{
				ASSERT(0);
				return true;
			}
			m_cbPlayOutTimeNum[m_wCurrentUser]++;
			if(m_cbPlayOutTimeNum[m_wCurrentUser]>=5)
			{
				m_endReason = 4;//超时
				OnEventGameConclude((m_wCurrentUser+1)%2,NULL,0);
				return true;
			}
			m_currentComob = 0;
			//强制设置母球
			CMD_S_END_SET_BASEBALL_TIMEOUT  endSetBaseBall;
			endSetBaseBall.ballPosx = 222;
			endSetBaseBall.ballPosy = 244;
			endSetBaseBall.timeOutCount = m_cbPlayOutTimeNum[m_wCurrentUser];
			m_pITableFrame->SetGameStatus(GS_WK_WAIT_HITBALL);
			m_pITableFrame->KillGameTimer(IDI_SET_BASEBALL);
			m_pITableFrame->SetGameTimer(IDI_HIT_BALL,TIME_HIT_BALL,1,0);			
			bool res = m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_END_SETBASEBALL_TIMEOUT,&endSetBaseBall,sizeof(endSetBaseBall));
			res = m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_END_SETBASEBALL_TIMEOUT,&endSetBaseBall,sizeof(endSetBaseBall));
			return true;

		}
		break;
	case IDI_HIT_BALL:
		{

			if(m_bGameStart==false)
				return true;
			m_bHit = false;
			m_pITableFrame->SetGameStatus(GS_WK_WAIT_HITBALL);
			m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
			m_pITableFrame->SetGameTimer(IDI_HIT_BALL,TIME_HIT_BALL,1,0);
			if(m_pITableFrame->GetGameStatus()!=GS_WK_WAIT_HITBALL)
			{
				ASSERT(0);
				return true;
			}
			m_cbPlayOutTimeNum[m_wCurrentUser]++;
			if(m_cbPlayOutTimeNum[m_wCurrentUser]>=5)
			{
				m_endReason = 4;//超时
				OnEventGameConclude((m_wCurrentUser+1)%2,NULL,0);
				return true;
			}
			CMD_S_HIT_TIMEOUT hitTimeOut;
			hitTimeOut.timeOutCount = m_cbPlayOutTimeNum[m_wCurrentUser];
			m_bWrongHit[m_wCurrentUser] = false;
			m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
			m_bWrongHit[m_wCurrentUser] = false;
			m_currentComob = 0;
			hitTimeOut.currentUser = m_wCurrentUser;			
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_HIT_TIMEOUT,&hitTimeOut,sizeof(hitTimeOut));		
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HIT_TIMEOUT,&hitTimeOut,sizeof(hitTimeOut));
			return true;
		}
		break;
	case IDI_WAIT_SENDRESULT:
		{
			m_pITableFrame->KillGameTimer(IDI_WAIT_SENDRESULT);
			if(m_bSendHitResult[m_wCurrentUser]||m_bSendHitResult[(m_wCurrentUser+1)%2])
			{//只要有一个人发过来，就算
				OnUserHitBallResult(&m_hitResult);
				return true;
			}
			else
			{//
				m_endReason = 5;//游戏解散
				OnEventGameConclude(0,NULL,GER_DISMISS);
			}
			return true;
		}
		break;
	}
	return true;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{	 
	case SUB_S_SETBASEBALL:
		{
			if(m_pITableFrame->GetGameStatus()!=GS_WK_SET_SETBASEBALL)
				return true;
			if(m_bGameStart==false)
				return true;
			CMD_S_SET_BASEBALL  *setBaseBall = (CMD_S_SET_BASEBALL*)pDataBuffer;
			if(wDataSize!=sizeof(CMD_S_SET_BASEBALL))
			{
				ASSERT(0);
				return false;
			}
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			BYTE user = (m_wCurrentUser+1)%GAME_PLAYER;
			m_pITableFrame->SendTableData(user,SUB_S_SETBASEBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_SET_BASEBALL));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SETBASEBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_SET_BASEBALL));
			return true;
		}

	case SUB_S_END_SETBASEBALL:
		{
			if(m_bGameStart==false)
				return true;
			if(m_pITableFrame->GetGameStatus()!=GS_WK_SET_SETBASEBALL)
			{
				ASSERT(0);
				return true;
			}
			CMD_S_END_SET_BASEBALL  *setBaseBall = (CMD_S_END_SET_BASEBALL*)pDataBuffer;
			if(wDataSize!=sizeof(CMD_S_END_SET_BASEBALL))
			{
				ASSERT(0);
				return false;
			}
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			//m_logger.LogMsg("chairid = %d,currentUser=%d,SUB_S_END_SETBASEBALL",pUserData->wChairID,m_wCurrentUser);

			m_pITableFrame->SetGameStatus(GS_WK_WAIT_HITBALL);
			m_pITableFrame->KillGameTimer(IDI_SET_BASEBALL);
			m_pITableFrame->SetGameTimer(IDI_HIT_BALL,TIME_HIT_BALL+2000,1,0);

			//BYTE user = (m_wCurrentUser+1)%GAME_PLAYER;
			bool res = m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_END_SETBASEBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_SET_BASEBALL));
			res = m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_END_SETBASEBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_SET_BASEBALL));
			return true;
		}
	case SUB_S_CLUBPOS:
		{
			if(m_bGameStart==false)
				return true;
			if(m_pITableFrame->GetGameStatus()!=GS_WK_WAIT_HITBALL)
				return true;
			CMD_S_BALLCLUB *club = (CMD_S_BALLCLUB*)pDataBuffer;
			if(wDataSize!=sizeof(CMD_S_BALLCLUB))
			{
				ASSERT(0);
				return false;
			}
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			if(m_wCurrentUser!=pUserData->wChairID)
				return true;
			BYTE user = (m_wCurrentUser+1)%GAME_PLAYER;
			IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(user);
			 
 			if(pUserItem->GetUserStatus()==US_OFFLINE)
			{//掉线就不用发了
				return true;
			}
			m_pITableFrame->SendTableData(user,SUB_S_CLUBPOS,const_cast<void*>(pDataBuffer),sizeof(CMD_S_BALLCLUB));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CLUBPOS,const_cast<void*>(pDataBuffer),sizeof(CMD_S_BALLCLUB));
			return true;

		}
	case SUB_S_HITBALL:
		{
			if(m_bGameStart==false)
				return true;
			if(m_pITableFrame->GetGameStatus()!=GS_WK_WAIT_HITBALL)
				return true;
			if(wDataSize!=sizeof(CMD_S_HitBall))
			{
				ASSERT(0);
				return false;
			}			
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			//m_logger.LogMsg("chairid = %d,currentUser=%d,SUB_S_HITBALL",pUserData->wChairID,m_wCurrentUser);
			if(m_bHit==true)
			{
				return true;
			}
			if(pUserData->wChairID!=m_wCurrentUser)
			{
				return true;
			}
			memcpy(&m_hitBall,pDataBuffer,sizeof(m_hitBall));
			m_bSendHitResult[0] = false;
			m_bSendHitResult[1] = false;
			m_bHit = true;
			m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
			BYTE user = (m_wCurrentUser+1)%GAME_PLAYER;
			m_pITableFrame->SendTableData(user,SUB_S_HITBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_HitBall));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HITBALL,const_cast<void*>(pDataBuffer),sizeof(CMD_S_HitBall));
			m_pITableFrame->SetGameTimer(IDI_WAIT_SENDRESULT,TIME_WAIT_SENDRESULT,1,0);
			return true;
		}
	case SUB_S_HIT_RESULT:
		{//打球结果
			if(m_bGameStart==false)
				return true;
			if(!m_bHit)
			{//如果没有打球，就收到打球的消息。。
				//ASSERT(0);			 	 
				return true;
			}
			if(wDataSize!=sizeof(CMD_S_HitResult))
			{
				ASSERT(0);
				return false;
			}
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
		/*	m_logger.LogMsg("chairid = %d,currentUser=%d,SUB_S_HIT_RESULT",pUserData->wChairID,m_wCurrentUser);*/					if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			m_bSendHitResult[pUserData->wChairID] = true;
			if(pUserData->wChairID==m_wCurrentUser)
			{
				memcpy(&m_hitResult,pDataBuffer,sizeof(m_hitResult));
			}
			else
			{
				if(m_bSendHitResult[m_wCurrentUser]==false)
				{//对家先发送消息过来。。
					memcpy(&m_hitResult,pDataBuffer,sizeof(m_hitResult));
				}
			}
			BYTE user = (m_wCurrentUser+1)%2;
			IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(user);
			IServerUserItem *pCurrentItem = m_pITableFrame->GetTableUserItem(m_wCurrentUser);
  
			if(m_bSendHitResult[0]&&m_bSendHitResult[1])
			{
				m_bHit = false;
				OnUserHitBallResult(&m_hitResult);
			}
			else if(m_bSendHitResult[m_wCurrentUser]&&(pUserItem->GetUserStatus()==US_OFFLINE))
			{//如果对方掉线
				m_bHit = false;
				OnUserHitBallResult(&m_hitResult);
			}
			else if(m_bSendHitResult[user]&&pCurrentItem->GetUserStatus()==US_OFFLINE)
			{//如果当前玩家掉线
				m_bHit = false;
				OnUserHitBallResult(&m_hitResult);
			}
			return true;
		}
	case SUB_S_GIVEUP:
		{
			if(m_bGameStart==false)
				return true;
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_LOOKON) 
				return true;
			m_endReason = 2;//认输
			OnEventGameConclude((pUserData->wChairID+1)%2,NULL,0);
			return true;
		}
	}

	return true;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}


bool CTableFrameSink::OnUserHitBallResult(const void * pDataBuffer)
{
	m_pITableFrame->KillGameTimer(IDI_WAIT_SENDRESULT);
	CMD_S_HitResult  *hitResult = (CMD_S_HitResult*)pDataBuffer;
	memcpy(m_ptBallPos,hitResult->ptBall,sizeof(m_ptBallPos));
//	static BYTE combomNum = 0;//连击数目

	CMD_S_CLIENT_HITRESLUT  clientHitResult;
	BYTE tempCurrentUser = m_wCurrentUser;
	bool firstHit = m_bFirstBall;
	bool bGameEnd = false;
	bool baseBallInHole = false;//白球是否进洞
	bool exChangeUser = false;
	bool bHit8Num = false;//是否是打黑8
	if(m_ballNum[m_wCurrentUser]==7)
		bHit8Num = true;
	BYTE endReason = 0;
	clientHitResult.hitResult = 0;
	if(hitResult->inHoleNum==0)
	{//如果没有打进去球，交换玩家
		exChangeUser = true;
		clientHitResult.hitResult = noInHole;
	}
	else
	{//如果有球打进去了，		
		for(int i=0;i<hitResult->inHoleNum;i++)
		{
			if(hitResult->ballInHole[i]==8)
			{//游戏结束			 
				m_ball[m_wCurrentUser][m_ballNum[m_wCurrentUser]++] = hitResult->ballInHole[i];
				bGameEnd = true;
				if(bHit8Num&&hitResult->firstBallNum==8)
				{//如果是最后一个球了。并且打的是8，并且白球不能进去
					endReason = 1;//正确进球
					clientHitResult.hitResult = rightInHole;
					exChangeUser = false;
				}
				else
				{
					endReason = 2;//犯规了
					clientHitResult.hitResult = wrongInHole;
					exChangeUser = true;
				}
			}
			else if(hitResult->ballInHole[i]==0)
			{//白球进洞
				baseBallInHole = true;
				exChangeUser = true;
			}
			else if(hitResult->ballInHole[i]<8)
			{
				if(m_userBallType[m_wCurrentUser]==1)
					m_ball[(m_wCurrentUser+1)%2][m_ballNum[(m_wCurrentUser+1)%2]++] = hitResult->ballInHole[i];
				else
				{
					m_ball[m_wCurrentUser][m_ballNum[m_wCurrentUser]++] = hitResult->ballInHole[i];
					clientHitResult.hitResult = rightInHole;
				}
			}
			else
			{
				if(m_userBallType[m_wCurrentUser]==0)
					m_ball[(m_wCurrentUser+1)%2][m_ballNum[(m_wCurrentUser+1)%2]++] = hitResult->ballInHole[i];
				else
				{
					m_ball[m_wCurrentUser][m_ballNum[m_wCurrentUser]++] = hitResult->ballInHole[i];
					clientHitResult.hitResult = rightInHole;
				}
			}
		}		
		if(m_userBallType[0]==100&&m_userBallType[1] ==100)
		{
			firstHit = true;
			//否则首次进球，确定大小球
			if(hitResult->ballInHole[0]>8&&(baseBallInHole==false||m_bFirstBall==false))
			{//进的是大球
				m_bFirstBall = false;
				m_userBallType[m_wCurrentUser] = 1;
				m_userBallType[(m_wCurrentUser+1)%2] = 0;
			}
			else if(hitResult->ballInHole[0]<8&&hitResult->ballInHole[0]>0&&(baseBallInHole==false||m_bFirstBall==false))
			{
				m_bFirstBall = false;
				m_userBallType[m_wCurrentUser] = 0;
				m_userBallType[(m_wCurrentUser+1)%2] = 1;
			}
			else
			{//白球或则黑球进洞
			}
		}
	}
	//第一个碰到的球
	if(firstHit)
	{//如果是第一次开球
		if((baseBallInHole||hitResult->firstBallNum==17||hitResult->bHitWithWall==false)&&m_bFirstBall)
		{//白球进洞，或者什么球都没打到,或者没有球碰墙
			exChangeUser = true;	
			clientHitResult.hitResult = openFail;
			m_bFirstBall = true;
			ZeroMemory(m_ball,sizeof(m_ball));
			ZeroMemory(m_ballNum,sizeof(m_ballNum));
			ZeroMemory(m_bWrongHit,sizeof(m_bWrongHit));
		}
		else if (hitResult->inHoleNum==0)
		{
			exChangeUser = true;	
			clientHitResult.hitResult = noInHole;
			m_bFirstBall = false;
		}
		else if(baseBallInHole)
		{
			exChangeUser = true;	
			clientHitResult.hitResult = wrongInHole;
			m_bWrongHit[m_wCurrentUser] = true;
			m_bFirstBall = false;
		}
		else if (hitResult->inHoleNum!=0)
		{
			m_bFirstBall = false;
			clientHitResult.hitResult = firtIn;
		}
		else
		{
		}
	}
	else if(baseBallInHole)
	{	
		if(!firstHit)
		{
			m_bWrongHit[m_wCurrentUser] = true;
			clientHitResult.hitResult = wrongInHole;
			exChangeUser = true;
		}
		if(bHit8Num&&hitResult->firstBallNum==8)
		{//打黑8的时候白球进洞，游戏结束
			bGameEnd = true;
			endReason = 2;
			m_bWrongHit[m_wCurrentUser] = true;
			clientHitResult.hitResult = wrongInHole;
			exChangeUser = true;
		}		
	}
	else if(hitResult->firstBallNum==8)
	{
		if(m_ballNum[m_wCurrentUser]==7)
		{
			clientHitResult.hitResult = noInHole;
			exChangeUser = true;	
			m_bWrongHit[m_wCurrentUser] = false;
		}
		else if(m_ballNum[m_wCurrentUser]==8)
		{
			bGameEnd = true;
			endReason = 1;
		}
		else
		{
			clientHitResult.hitResult = wrongInHole;
			exChangeUser = true;
			m_bWrongHit[m_wCurrentUser] = true;
		}
	}
	else if(hitResult->firstBallNum==17)
	{//没有碰到任何球
		clientHitResult.hitResult = wrongInHole;
		exChangeUser = true;
		m_bWrongHit[m_wCurrentUser] = true;
	}
	else if (hitResult->firstBallNum>8&&m_userBallType[m_wCurrentUser]==0)
	{//
		clientHitResult.hitResult = wrongInHole;
		exChangeUser = true;
		m_bWrongHit[m_wCurrentUser] = true;

	}
	else if(hitResult->firstBallNum<8&&m_userBallType[m_wCurrentUser]==1)
	{
		clientHitResult.hitResult = wrongInHole;
		exChangeUser = true;
		m_bWrongHit[m_wCurrentUser] = true;
	}
	else if(clientHitResult.hitResult==0)
	{
		clientHitResult.hitResult = noInHole;
		m_bWrongHit[m_wCurrentUser] = false;
		exChangeUser = true;//没有自己的球进洞，交换玩家
		//m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;	
	}
	if(hitResult->bHitWithWall==false&&hitResult->inHoleNum==0)
	{//没有球碰墙，并且没有球进洞
		if(clientHitResult.hitResult != openFail)
		{
			m_bWrongHit[m_wCurrentUser] = true;
			clientHitResult.hitResult = wrongInHole;
			exChangeUser = true;
		}
	}
	if(m_bWrongHit[m_wCurrentUser])
	{
		m_bWrongHit[(m_wCurrentUser+1)%GAME_PLAYER] = false;
	}
	
	if(exChangeUser)
	{
		m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;	
		m_currentComob = 0;
		//combomNum = 0;
	}
	else
	{
		m_currentComob ++;
		//combomNum ++;
	}
	if(m_comob[m_wCurrentUser]<m_currentComob&&m_currentComob>=1)
			m_comob[m_wCurrentUser] = m_currentComob -1;

	ASSERT(clientHitResult.hitResult>0);
	if(exChangeUser&&clientHitResult.hitResult!=wrongInHole)
	{//如果对方犯规了，可以连击
		if(m_bWrongHit[m_wCurrentUser])
		{
			m_bWrongHit[m_wCurrentUser] = false;
			m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
		}
	}
	
	//TRACE("before=%d,current=%d,exChangeUser=%d",tempCurrentUser,m_wCurrentUser,exChangeUser);

	for(int i=0;i<BALL_NUM;i++)
		clientHitResult.ballState[i] = true;
	//发送消息
	for(int i=0;i<GAME_PLAYER;i++)
	{
		for(int j=0;j<m_ballNum[i];j++)
		{
			if(m_ball[i][j]>0)
				clientHitResult.ballState[m_ball[i][j]] = false;
		}
	}
	m_bWrongHit[m_wCurrentUser] = false;
	if(baseBallInHole)
		clientHitResult.ballState[0] = false;//白球进洞
	if(bGameEnd)
	{
		clientHitResult.hitResult = gameEnd;
	}
	memcpy(clientHitResult.ballInHole,hitResult->ballInHole,sizeof(clientHitResult.ballInHole));
	memcpy(clientHitResult.ptBall,hitResult->ptBall,sizeof(clientHitResult.ptBall));
	memcpy(clientHitResult.totalComobNum,this->m_comob,sizeof(m_comob));
	memcpy(clientHitResult.userBallType,this->m_userBallType,sizeof(m_userBallType));
	clientHitResult.currentUser = m_wCurrentUser;
	clientHitResult.firstBallNum = hitResult->firstBallNum;
	clientHitResult.inHoleNum = hitResult->inHoleNum;
	clientHitResult.comobNum = m_currentComob-1;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_HIT_RESULT,&clientHitResult,sizeof(clientHitResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_HIT_RESULT,&clientHitResult,sizeof(clientHitResult));
	if(baseBallInHole)
	{
		m_pITableFrame->SetGameStatus(GS_WK_SET_SETBASEBALL);
		m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
		m_pITableFrame->SetGameTimer(IDI_SET_BASEBALL,TIME_SET_BASEBALL,1,0);
	}
	else
	{
		m_pITableFrame->SetGameStatus(GS_WK_WAIT_HITBALL);
		m_pITableFrame->KillGameTimer(IDI_HIT_BALL);
		m_pITableFrame->SetGameTimer(IDI_HIT_BALL,TIME_HIT_BALL+2000,1,0);
	}
	if(bGameEnd)
	{
		m_currentComob = 0;
		if(endReason==1)
		{//打进黑8，游戏结束
			m_endReason = 1;//正常结束
			OnEventGameConclude(tempCurrentUser,NULL,0);
		}
		else
		{
			m_endReason = 3;//犯规
			OnEventGameConclude((tempCurrentUser+1)%GAME_PLAYER,NULL,0);
		}
	}
	return true;
}
//开球失败，从新开球
void CTableFrameSink::OnFaultBegin()
{

}


	//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
	//用户重入
bool CTableFrameSink::OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
	//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{	
	if(!bLookonUser)
	{
		memset(m_nTatolScore,0,sizeof(m_nTatolScore));			//总的输赢分数
		memset(m_nTurnScore,0,sizeof(m_nTatolScore));			//上一轮输赢
	}
	return true;
}
	//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(!bLookonUser)
	{ 
		memset(m_nTatolScore,0,sizeof(m_nTatolScore));			//总的输赢分数
		memset(m_nTurnScore,0,sizeof(m_nTatolScore));			//上一轮输赢
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
