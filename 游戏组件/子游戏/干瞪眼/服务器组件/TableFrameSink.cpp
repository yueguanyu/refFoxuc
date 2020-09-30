#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bScoreInfo,sizeof(m_bScoreInfo));

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	ZeroMemory(m_bUserTrustee,  sizeof( m_bUserTrustee ) );
	ZeroMemory(m_bUserActive,sizeof(m_bUserActive));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_bBackCard,sizeof(m_bBackCard));
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bOutCardCount,sizeof(m_bOutCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));
	m_bLeftCardCount=0;
	ZeroMemory(m_bAllCardData,sizeof(m_bAllCardData));

	m_pITableFrame=NULL;							
	m_pGameServiceOption=NULL;					
	m_pGameServiceAttrib=NULL;					
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
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
						
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();					
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();	

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{

	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bScoreInfo,sizeof(m_bScoreInfo));

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	ZeroMemory(m_bUserTrustee,  sizeof( m_bUserTrustee ) );

	//扑克信息
	ZeroMemory(m_bBackCard,sizeof(m_bBackCard));
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bOutCardCount,sizeof(m_bOutCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));
	m_bLeftCardCount=0;
	ZeroMemory(m_bAllCardData,sizeof(m_bAllCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));
	return;
}

//消费能力
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{

	return (SCORE)0;
}

//最少积分
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return (SCORE)0;
}
//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
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
//设置基数
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{

}

//用户坐下
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{

	return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(wChairID);
	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserLeave(wChairID);
	return true;
}
//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{

	return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) 
{
	return true;

}
//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

	m_GameLogic.SetCanOutSingleKing(false);

	//检测用户
	ZeroMemory(m_bUserActive,sizeof(m_bUserActive));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)!=NULL)	m_bUserActive[i]=true;
	}

	//设置用户
	if(m_wFirstUser==INVALID_CHAIR)
	{
		do
		{
			m_wFirstUser = rand() % GAME_PLAYER;
		}while(m_bUserActive[m_wFirstUser]!=true);
	}
	else
	{
		do
		{
			m_wFirstUser = (m_wFirstUser+1) % GAME_PLAYER;
		}while(m_bUserActive[m_wFirstUser]!=true);
	}
	m_wCurrentUser=m_wFirstUser;
	m_wBankerUser=m_wCurrentUser;

	//设置牌数
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_bUserActive[i]==true)
		{
			m_bCardCount[i] = 5;
			if(i==m_wBankerUser) m_bCardCount[i] = 6;
		}
	}

	//混乱扑克
	BYTE bRandCard[ 54 ];
	m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

	//分发扑克
	BYTE bSendCount=0;		//发送张数
	for ( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if(m_bUserActive[i]!=true) continue;
		CopyMemory( &m_bHandCardData[i], &bRandCard[bSendCount], sizeof(BYTE)*m_bCardCount[i]);
		bSendCount+=m_bCardCount[i];
	}
	CopyMemory( m_bAllCardData, &bRandCard[bSendCount], sizeof(m_bAllCardData));
	m_bLeftCardCount=54-bSendCount;

	//排列扑克
	for ( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );
	}

	CMD_S_GameStart GameStart;
	GameStart.wLandUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.bLeftCardCount=m_bLeftCardCount;
	CopyMemory(GameStart.bUserCardCount,m_bCardCount,sizeof(GameStart.bUserCardCount));

	for ( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if(m_bUserActive[i]!=true) continue;
		GameStart.bCardCount=m_bCardCount[i];
		CopyMemory(&GameStart.bCardData,m_bHandCardData[i],m_bCardCount[i]);
		m_pITableFrame->SendTableData( i, SUB_S_GAME_START, &GameStart, sizeof( GameStart ) );
		m_pITableFrame->SendLookonData( i, SUB_S_GAME_START, &GameStart, sizeof( GameStart ) );
		ZeroMemory(GameStart.bCardData,sizeof(GameStart.bCardData));
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
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory( &GameEnd, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//变量定义
			LONGLONG lCellScore = m_pGameServiceOption->lCellScore;

			//春天判断
			bool bHaoChun=false;
			bool bChunTian[GAME_PLAYER];
			ZeroMemory(bChunTian,sizeof(bChunTian));
			BYTE bActUser=0;
			BYTE bChunTianUser=0;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_bUserActive[i]==true) bActUser++;
				if(m_bCardCount[i]==5)
				{
					bChunTianUser++;
					bChunTian[i]=true;
				}
			}

			//庄家一把就出完，算豪春
			if(  m_bLeftCardCount==(54-(bActUser*5+1))) bHaoChun=true;
			if(bHaoChun==true) m_wBombTime*=8;

			//算分
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bUserActive[i]!=true||i==wChairID) continue;
				if(bChunTian[i]==true && bHaoChun==false) m_wBombTime*=2;
				GameEnd.lGameScore[i]= - m_wBombTime*m_bCardCount[i]*lCellScore;
				GameEnd.lGameScore[wChairID]+= - GameEnd.lGameScore[i];
			}

			//写分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bUserActive[i]!=true) continue;

				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].lRevenue=0;
				if(ScoreInfo[i].lScore>0)
				{
					ScoreInfo[i].cbType=SCORE_TYPE_WIN;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
						ScoreInfo[i].lRevenue=ScoreInfo[i].lScore*m_pGameServiceOption->wRevenueRatio/1000;
					ScoreInfo[i].lScore-=ScoreInfo[i].lRevenue;
				}
				else if(ScoreInfo[i].lScore==0) 	ScoreInfo[i].cbType=SCORE_TYPE_DRAW;
				else				ScoreInfo[i].cbType=SCORE_TYPE_LOSE;

				GameEnd.lGameScore[i]=ScoreInfo[i].lScore;
				GameEnd.lGameTax+=ScoreInfo[i].lRevenue;
				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}

			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//切换用户
			m_wFirstUser=wChairID;

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);
			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT( pIServerUserItem != NULL );
			ASSERT( wChairID < GAME_PLAYER );

			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory( &GameEnd,  sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//发送信息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);
			return true;
		}
	case GER_USER_LEAVE:	//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<GAME_PLAYER);

			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//炸弹限制
			//m_wBombTime=__min(m_wBombTime,16);
			//m_bLandScore=__max(m_bLandScore,1);
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			GameEnd.lGameScore[wChairID]=-3*m_wBombTime*m_bCardCount[wChairID]*m_pGameServiceOption->lCellScore;

			//修改积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(i==wChairID)
				{
					ScoreInfo[i].cbType=SCORE_TYPE_FLEE;
					ScoreInfo[i].lScore=GameEnd.lGameScore[wChairID];
				}
				else
					ScoreInfo[i].cbType =SCORE_TYPE_DRAW;
			}			

			//历史积分
			m_HistoryScore.OnEventUserScore(wChairID,GameEnd.lGameScore[wChairID]);
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);
			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;
			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));

			//设置变量
			StatusScore.bLandScore=m_bLandScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusScore.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
			//		CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);
			CopyMemory(StatusScore.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( StatusScore.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusScore.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusScore.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wBombTime=m_wBombTime;
			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.bLandScore=m_bLandScore;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.bLeftCardCount=m_bLeftCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bBackCard,m_bBackCard,sizeof(m_bBackCard));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			//		CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData,sizeof(BYTE)*sizeof(m_bHandCardData));
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			CopyMemory(StatusPlay.bMagicData,m_cbMagicCardData,sizeof(StatusPlay.bMagicData));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CART:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->bCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;
			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_TRUSTEE:			//玩家托管
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
			if (wDataSize != sizeof(CMD_C_UserTrustee)) return false;

			CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pData;

			m_bUserTrustee[ pUserTrustee->wUserChairID ] = pUserTrustee->bTrustee;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_C_TRUSTEE, (void*)pData,wDataSize);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_C_TRUSTEE, (void*)pData,wDataSize);

			return true;
		}
	}

	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//排序扑克
	m_GameLogic.SortCardList(bCardData, bCardCount, ST_ORDER);

	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(bCardData,bCardCount,cbMagicCardData,m_cbMagicCardData,m_bTurnCardCount);

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(cbMagicCardData,bCardCount);
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(m_cbMagicCardData,cbMagicCardData,m_bTurnCardCount,bCardCount)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);
	CopyMemory(m_cbMagicCardData,cbMagicCardData,sizeof(BYTE)*bCardCount);

	//炸弹判断
	if ((bCardType==CT_BOMB_3)||(bCardType==CT_BOMB_3A)) m_wBombTime*=2;
	else if((bCardType==CT_BOMB_KING)&&bCardCount==5)  m_wBombTime*=8;
	else if((bCardType==CT_BOMB_KING)&&bCardCount==6)  m_wBombTime*=16;
	else if ((bCardType==CT_BOMB_4)||(bCardType==CT_BOMB_4A)||(bCardType==CT_BOMB_KING)) m_wBombTime*=4;

	//切换用户
	m_wTurnWiner=wChairID;
	if (m_bCardCount[wChairID]!=0)
	{
		do
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		}while(m_bUserActive[m_wCurrentUser]!=true);
	}
	else m_wCurrentUser=INVALID_CHAIR;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bMagicData,m_cbMagicCardData,m_bTurnCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bUserCardCount,m_bCardCount,sizeof(OutCard.bUserCardCount));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;

	//设置变量	
	do
	{
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	}while(m_bUserActive[m_wCurrentUser]!=true);
	if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;

	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;

	//新一轮
	bool bNewTurn=(m_bTurnCardCount==0)?true:false;
	//最后一轮没有牌发的时候王可以单出
	if(bNewTurn&&m_bLeftCardCount==0) 
	{
		m_GameLogic.SetCanOutSingleKing(true);
		PassCard.bCanOutSingleKing=true;
	}
	if(bNewTurn&&m_bLeftCardCount>0)
	{
		ASSERT(m_bLeftCardCount>0);
		if(m_bLeftCardCount==0) goto pass;

		//增加一张牌
		m_bCardCount[m_wTurnWiner]+=1;
		m_bHandCardData[m_wTurnWiner][m_bCardCount[m_wTurnWiner]-1]=m_bAllCardData[m_bLeftCardCount-1];
		m_GameLogic.SortCardList(m_bHandCardData[m_wTurnWiner],m_bCardCount[m_wTurnWiner],ST_ORDER);
		m_bLeftCardCount--;
	}
pass:
	PassCard.bLeftCardCount=m_bLeftCardCount;
	PassCard.bNewTurn=bNewTurn;
	CopyMemory(PassCard.bUserCardCount,m_bCardCount,sizeof(m_bCardCount));
	CopyMemory(PassCard.bCardData,m_bHandCardData,sizeof(m_bHandCardData));
	CopyMemory(PassCard.bUserCardCount,m_bCardCount,sizeof(PassCard.bUserCardCount));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
