#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

#define IDI_GAME_END				1										//结束标识
#define IDI_PASS_CARD				2										//放弃标识

#define TIME_PASS_CARD				50000									//

#define SHOW_HAND_SCORE				0x7FFFFFFFFFFFFFFF								//

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wOperaCount=0;
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	m_wLastMostUser = INVALID_CHAIR;
	m_bGameEnd = false;

	//用户状态
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));
	ZeroMemory(m_lLostScore,sizeof(m_lLostScore));

	//扑克变量
	m_cbSendCardCount=0;
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxScore = 0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	m_lShowHandScore = 0L;

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_wOperaCount=0;
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	m_wLastMostUser = INVALID_CHAIR;
	m_bGameEnd = false;

	//用户状态
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));
	ZeroMemory(m_lLostScore,sizeof(m_lLostScore));

	//扑克变量
	m_cbSendCardCount=0;
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxScore = 0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	m_lShowHandScore=0L;

	return;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	//最大下注
	m_lShowHandScore = m_pGameServiceOption->lRestrictScore==0?SHOW_HAND_SCORE:m_pGameServiceOption->lRestrictScore;

	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//获取积分
		ASSERT(pIServerUserItem->GetUserScore()>=m_pGameServiceOption->lCellScore);

		//设置变量
		m_cbPlayStatus[i]=TRUE;
		m_lUserMaxScore[i]=__min(pIServerUserItem->GetUserScore(),m_lShowHandScore);
		if ((m_lMaxScore==0L)||(m_lUserMaxScore[i]<m_lMaxScore)) m_lMaxScore=m_lUserMaxScore[i];
	}
	//下注变量
	m_lTurnMaxScore=m_lMaxScore/2L;
	m_lCellScore=__max(m_lMaxScore/100L,m_pGameServiceOption->lCellScore);
	m_lTurnLessScore=m_lCellScore;
	

	//分发扑克
	m_cbSendCardCount=2;
	m_GameLogic.RandCardList(m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));

	//用户设置
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[2*i+1]=m_lCellScore;
			m_cbCardCount[i]=m_cbSendCardCount;
		}
	}

	//设置用户
	m_wOperaCount=0;
	m_wCurrentUser=EstimateWinner(1,1);
	m_wLastMostUser = m_wCurrentUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lCellScore=m_lCellScore;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.lTurnMaxScore=m_lTurnMaxScore;
	GameStart.lTurnLessScore=m_lTurnLessScore;
	GameStart.lShowHandScore = m_lShowHandScore;

	//设置扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==FALSE) GameStart.cbCardData[i]=0;
		else GameStart.cbCardData[i]=m_cbHandCardData[i][1];
	}

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		GameStart.cbObscureCard=0;
		if (m_cbPlayStatus[i]==TRUE)
		{
			GameStart.cbObscureCard=m_cbHandCardData[i][0];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		GameStart.cbObscureCard=0;
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//设置时间
	m_pITableFrame->SetGameTimer(IDI_PASS_CARD,TIME_PASS_CARD,1,m_wCurrentUser);

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	case GER_NO_PLAYER:		//没有玩家
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//变量定义
			WORD wWinerUser=EstimateWinner(0,4);
			LONGLONG lMaxLostScore=m_lTableScore[wWinerUser*2]+m_lTableScore[wWinerUser*2+1];

			//统计信息
			LONGLONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//底牌信息
				if ((cbReason==GER_NO_PLAYER)||(m_cbPlayStatus[i]==FALSE)) GameEnd.cbCardData[i]=0;
				else GameEnd.cbCardData[i]=m_cbHandCardData[i][0];

				//积分信息
				if (i!=wWinerUser)
				{
					LONGLONG lLostScore=m_lTableScore[i*2]+m_lTableScore[i*2+1];
					if (m_cbPlayStatus[i]==TRUE)
					{
						lWinnerScore+=__min(lLostScore,lMaxLostScore);
						GameEnd.lGameScore[i]=-__min(lLostScore,lMaxLostScore);
					}
					else
					{
						lWinnerScore+=-m_lLostScore[i];
						GameEnd.lGameScore[i]=m_lLostScore[i];
					}
				}
			}

			//胜者得分
			GameEnd.lGameScore[wWinerUser]=lWinnerScore;
			
			//扣税
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
			{
				GameEnd.lGameTax[wWinerUser]=m_pITableFrame->CalculateRevenue(wWinerUser,GameEnd.lGameScore[wWinerUser]);
				GameEnd.lGameScore[wWinerUser] -= GameEnd.lGameTax[wWinerUser];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			tagScoreInfo UserScore[GAME_PLAYER];
			ZeroMemory(UserScore,sizeof(UserScore));

			//修改积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if( m_cbPlayStatus[i] == TRUE )
				{	
					UserScore[i].lScore=GameEnd.lGameScore[i];
					UserScore[i].lRevenue=GameEnd.lGameTax[i];
					UserScore[i].cbType=SCORE_TYPE_DRAW;
					if(GameEnd.lGameScore[i]>0) UserScore[i].cbType=SCORE_TYPE_WIN;
					else UserScore[i].cbType=SCORE_TYPE_LOSE;
				}
			}

			m_pITableFrame->WriteTableScore(UserScore,CountArray(UserScore));
			
			//删除时间
			m_pITableFrame->KillGameTimer(IDI_GAME_END);
			m_pITableFrame->KillGameTimer(IDI_PASS_CARD);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:		//网络中断
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			if( m_cbPlayStatus[wChairID] )
				OnUserGiveUp(wChairID,true);

			return true;
		}
	case GER_DISMISS:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//删除时间
			m_pITableFrame->KillGameTimer(IDI_GAME_END);
			m_pITableFrame->KillGameTimer(IDI_PASS_CARD);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//标志变量
			StatusPlay.bShowHand=(m_bShowHand==true)?TRUE:FALSE;

			//加注信息
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lTurnMaxScore=m_lTurnMaxScore;
			StatusPlay.lTurnLessScore=m_lTurnLessScore;
			StatusPlay.lShowHandScore = m_lShowHandScore;

			//设置变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));

			//设置扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					StatusPlay.cbCardCount[i]=m_cbCardCount[i];
					if ((i==wChiarID)&&(pIServerUserItem->GetUserStatus()!=US_LOOKON)) StatusPlay.cbHandCardData[i][0]=m_cbHandCardData[i][0];
					else StatusPlay.cbHandCardData[i][0]=0;
					CopyMemory(&StatusPlay.cbHandCardData[i][1],&m_cbHandCardData[i][1],(m_cbCardCount[i]-1)*sizeof(BYTE));
				}
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParam)
{
	switch (dwTimerID)
	{
	case IDI_GAME_END:	//游戏结束
		{
			if (m_pITableFrame->GetGameStatus()==GAME_STATUS_PLAY)
			{
				OnEventGameConclude(INVALID_CHAIR,NULL,(BYTE)dwBindParam);
				return true;
			}
		}
	case IDI_PASS_CARD:	//放弃加注
		{
			if ((dwBindParam==m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR))
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
				
				//用户判断
				if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()!=US_OFFLINE))
				{
					OnUserGiveUp(m_wCurrentUser);
				}
				else m_pITableFrame->SetGameTimer(IDI_PASS_CARD,TIME_PASS_CARD,1,m_wCurrentUser);

				return true;
			}
		}
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ADD_SCORE:			//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false);
		}
	case SUB_C_GIVE_UP:				//用户放弃
		{
			//用户效验
			tagUserInfo* pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID,VOID* pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户断线
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	if (pIServerUserItem->GetChairID()==m_wCurrentUser)
	{
		m_pITableFrame->KillGameTimer(IDI_PASS_CARD);
	}

	return true;
}

//用户重入
bool  CTableFrameSink::OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem)
{
	if (pIServerUserItem->GetChairID()==m_wCurrentUser)
	{
		m_pITableFrame->SetGameTimer(IDI_PASS_CARD,TIME_PASS_CARD,1,m_wCurrentUser);
	}

	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bLeave)
{
	//效验
	ASSERT( m_cbPlayStatus[wChairID] );
	if( !m_cbPlayStatus[wChairID] ) return false;

	if( m_bGameEnd )
	{
		m_pITableFrame->KillGameTimer( IDI_GAME_END );
		return OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );
	}

	//设置数据
	m_cbCardCount[wChairID]=0;
	m_cbPlayStatus[wChairID]=FALSE;

	//计算扣分
	LONGLONG lScore = m_lTableScore[wChairID*2]+m_lTableScore[wChairID*2+1];
	lScore = -__min( m_lMaxScore,lScore );

	m_lLostScore[wChairID] = lScore;

	//重新获取最大下注
	if( m_lUserMaxScore[wChairID] == m_lMaxScore )
	{
		for( WORD i = 0; i < GAME_PLAYER; i ++ )
		{
			if( !m_cbPlayStatus[i] ) continue;
			m_lMaxScore = m_lUserMaxScore[i];
			break;
		}
		for( ; i < GAME_PLAYER; i++ )
		{
			if( m_cbPlayStatus[i] )
				m_lMaxScore = __min(m_lMaxScore,m_lUserMaxScore[i]);
		}
		m_lTurnMaxScore = m_cbSendCardCount>=3?m_lMaxScore:m_lMaxScore/2;
	}

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	GiveUp.lTurnMaxScore = m_lTurnMaxScore;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//写入积分
	tagScoreInfo ScoreInfo[GAME_PLAYER];
	ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
	ScoreInfo[wChairID].cbType=SCORE_TYPE_FLEE;
	ScoreInfo[wChairID].lScore=m_lLostScore[wChairID];
	m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0L,true);
	}
	else
	{
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp)
{
	//效验
	if( m_bGameEnd )
	{
		m_pITableFrame->KillGameTimer( IDI_GAME_END );
		return OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );
	}

	m_pITableFrame->KillGameTimer(IDI_PASS_CARD);

	//设置数据
	if (bGiveUp==false)
	{
		//状态效验
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) return false;

		//金币效验
		ASSERT( !m_bShowHand && lScore+m_lTableScore[wChairID*2+1] >= m_lTurnLessScore && lScore+m_lTableScore[wChairID*2+1]<=m_lTurnMaxScore
			|| lScore+m_lTableScore[wChairID*2+1]==m_lUserMaxScore[wChairID]);
		if ( (m_bShowHand || lScore+m_lTableScore[wChairID*2+1] < m_lTurnLessScore || lScore+m_lTableScore[wChairID*2+1] > m_lTurnMaxScore)
			&& lScore+m_lTableScore[wChairID*2+1]!=m_lUserMaxScore[wChairID]) 
			return false;

		m_lTableScore[wChairID*2] = lScore;

		//设置变量
		if( !m_bShowHand && m_lTableScore[wChairID*2]+m_lTableScore[wChairID*2+1] == m_lUserMaxScore[wChairID] ) 
			m_bShowHand = true;

		m_lTurnLessScore=__max(m_lTableScore[wChairID*2]+m_lTableScore[wChairID*2+1],m_lTurnLessScore);
	}

	//用户切换
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=0;i<m_wPlayerCount-1;i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i+1)%m_wPlayerCount;

		//继续判断
		if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
	}
	
	//完成判断
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		LONGLONG lTableScore=0L;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayStatus[i]==TRUE)
			{
				lTableScore=m_lTableScore[i*2];
				break;
			}
		}
		while (i<m_wPlayerCount)
		{
			if (m_cbPlayStatus[i]==TRUE) 
			{
				if( !m_bShowHand&&m_lTableScore[i*2]!=lTableScore ||
					m_bShowHand&&m_lTableScore[i*2]+m_lTableScore[i*2+1]<m_lUserMaxScore[i] )
					break;
			}
			i++;
		}
		if (i==m_wPlayerCount) bFinishTurn=true;
	}

	//继续加注
	if (bFinishTurn==false)
	{
		//设置用户
		m_wCurrentUser=wNextPlayer;

		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=m_wCurrentUser;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lAddScoreCount=m_lTableScore[wChairID*2];
		AddScore.bShowHand = m_bShowHand;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

		//设置时间
		m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);

		return true;
	}
	else
	{
		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=INVALID_CHAIR;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lAddScoreCount=m_lTableScore[wChairID*2];
		AddScore.bShowHand = m_bShowHand;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//结束判断
	if (m_cbSendCardCount==MAX_COUNT) 
	{
		m_bGameEnd = true;
		m_pITableFrame->SetGameTimer( IDI_GAME_END,2000,1,GER_NORMAL );
		return true;
	}

	//累计金币
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_lTableScore[i*2+1]+=m_lTableScore[i*2];
		m_lTableScore[i*2]=0L;
	}

	//设置变量
	m_wOperaCount=0;

	//派发扑克
	BYTE cbSourceCount=m_cbSendCardCount;
	if (m_bShowHand==false) m_cbSendCardCount++;
	else m_cbSendCardCount=MAX_COUNT;

	//上次最大用户
	WORD wLastMostUser = m_wLastMostUser;
	//当前用户
	if (m_bShowHand==false)
	{
		//用户设置
		m_wCurrentUser=EstimateWinner(1,m_cbSendCardCount-1);
		m_wLastMostUser = m_wCurrentUser;
		
		//下注设置
		m_lTurnMaxScore=(m_cbSendCardCount>=3)?m_lMaxScore:m_lMaxScore/2L;
	}
	else m_wCurrentUser=INVALID_CHAIR; 

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=m_wCurrentUser;
	SendCard.wLastMostUser = wLastMostUser;
	SendCard.lTurnMaxScore=m_lTurnMaxScore;
	SendCard.lTurnLessScore=m_lTurnLessScore;
	SendCard.cbSendCardCount=m_cbSendCardCount-cbSourceCount;

	//发送扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//扑克数目
		if (!m_cbPlayStatus[i]) continue;

		m_cbCardCount[i]=m_cbSendCardCount;

		//派发扑克
		for (BYTE j=0;j<(m_cbSendCardCount-cbSourceCount);j++)
		{
			ASSERT(j<CountArray(SendCard.cbCardData[i]));
			SendCard.cbCardData[i][j]=m_cbHandCardData[i][cbSourceCount+j];
		}
	}

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	//结束处理
	if (m_wCurrentUser==INVALID_CHAIR) 
	{
		m_pITableFrame->KillGameTimer(IDI_PASS_CARD);
		m_bGameEnd = true;
		m_pITableFrame->SetGameTimer(IDI_GAME_END,3000,1,0);
	}
	else m_pITableFrame->SetGameTimer(IDI_PASS_CARD,TIME_PASS_CARD,1,m_wCurrentUser);

	return true;
}

//推断胜者
WORD CTableFrameSink::EstimateWinner(BYTE cbStartPos, BYTE cbConcludePos)
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//寻找玩家
	for (WORD wWinner=0;wWinner<m_wPlayerCount;wWinner++)
	{
		if (m_cbPlayStatus[wWinner]==TRUE) 
		{
			//排列扑克
			m_GameLogic.SortCardList(cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1);
			break;
		}
	}

	//对比玩家
	WORD wId = wWinner;
	for (WORD i=0;i<GAME_PLAYER-1;i++)
	{
		wId = (wId+1)%GAME_PLAYER;

		//用户过滤
		if (m_cbPlayStatus[wId]==FALSE) continue;

		//排列扑克
		m_GameLogic.SortCardList(cbUserCardData[wId]+cbStartPos,cbConcludePos-cbStartPos+1);

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[wId]+cbStartPos,cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1)==true) 
		{
			wWinner=wId;
		}
	}

	return wWinner;
}

//////////////////////////////////////////////////////////////////////////
