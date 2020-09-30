#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_FULL_READY;	//开始模式

//宏定义
#define	GOOD_CARD_PERCENT				4									//好牌概率

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lAddScore=8000;
	m_wBomb=1;
	m_wQiangLandDouble=1;
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));
	memset(m_bMingCardStatus,0,sizeof(m_bMingCardStatus));
	memset(m_bQiangLand,false,sizeof(m_bQiangLand));
	memset(m_bUserDouble,1,sizeof(m_bUserDouble));
	memset(m_bAddScore,false,sizeof(m_bAddScore));
	memset(m_wPlayTime,0,sizeof(m_wPlayTime));


	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	m_bLaiZiCard=0;
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	
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
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}
//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(wChairID);

	return true;
}

//用户起立
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserLeave(wChairID);
	if (bLookonUser==false) m_wPlayTime[wChairID]=0;

	return true;
}
//复位桌子
void  CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lAddScore=8000;
	m_wBomb=1;
	m_wQiangLandDouble=1;
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));
	//
	memset(m_bMingCardStatus,0,sizeof(m_bMingCardStatus));
	memset(m_bQiangLand,false,sizeof(m_bQiangLand));
	memset(m_bUserDouble,1,sizeof(m_bUserDouble));
	memset(m_bAddScore,false,sizeof(m_bAddScore));

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	m_bLaiZiCard=0;
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	return;
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
	m_pITableFrame->SetGameStatus(GS_WK_SCORE);

	BYTE cbSendGoodCard = rand () % GOOD_CARD_PERCENT ;
	BYTE cbAndroidUserCount = 0 ;
	WORD wAndroidUser[ GAME_PLAYER ] = {0};

	//机器人个数
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem( wChairID ) ;
		if ( pServerUserItem->IsAndroidUser() )
		{
			wAndroidUser[ cbAndroidUserCount ] = ( wChairID ) ;
			++cbAndroidUserCount ;
		}
	}

	WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
	if ( 0 < cbAndroidUserCount ) wHaveGoodCardAndroidUser = wAndroidUser[ rand() % cbAndroidUserCount ] ;

	//发送好牌
	if ( 1 == cbSendGoodCard && wHaveGoodCardAndroidUser != INVALID_CHAIR )
	{
		//混乱扑克
		BYTE bRandCard[54];
		m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

		BYTE cbGoodCard[ NORMAL_COUNT ] ;
		m_GameLogic.GetGoodCardData( cbGoodCard ) ;

		//抽取好牌
		m_GameLogic.RemoveGoodCardData( cbGoodCard, NORMAL_COUNT, bRandCard, FULL_COUNT ) ;

		//分发扑克
		CopyMemory( m_bHandCardData[ wHaveGoodCardAndroidUser ], cbGoodCard, NORMAL_COUNT ) ;
		m_bCardCount[ wHaveGoodCardAndroidUser ] = 17;
		for ( WORD i = 0, j = 0; i < m_wPlayerCount; i++ )
		{
			if ( i != wHaveGoodCardAndroidUser )
			{
				m_bCardCount[ i ] = 17;
				CopyMemory( &m_bHandCardData[ i ], &bRandCard[ j * 17 ], sizeof( BYTE ) * 17 );
				++j ;
			}
		}
		CopyMemory( m_bBackCard, &bRandCard[ 34 ], sizeof( m_bBackCard ) );
		m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
	}
	else
	{
		//混乱扑克
		BYTE bRandCard[ 54 ];
		m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

		//分发扑克
		for ( WORD i = 0; i < m_wPlayerCount; i++ )
		{
			m_bCardCount[ i ] = 17;
			CopyMemory( &m_bHandCardData[ i ], &bRandCard[ i * m_bCardCount[ i ] ], sizeof( BYTE ) * m_bCardCount[ i ] );
		}
		CopyMemory( m_bBackCard, &bRandCard[ 51 ], sizeof( m_bBackCard ) );
		m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
	}

	//设置用户
	m_wCurrentUser = rand() % GAME_PLAYER;
	m_wFirstUser=m_wCurrentUser;

	//发送扑克
	CMD_S_SendAllCard SendAllCard ;
	SendAllCard.wCurrentUser = m_wCurrentUser ;

	//text
	//BYTE bCardData[12]={0x0d,0x2d,0x3c,0x0c,0x1c,0x26,0x06,0x18,0x28,0x03,0x04,0x06};
	//CopyMemory(m_bHandCardData[0],bCardData,sizeof(BYTE)*12);
	//
	//手上扑克
	CopyMemory( SendAllCard.bCardData, m_bHandCardData, sizeof( m_bHandCardData ) ) ;
	//设置明牌
	CopyMemory( SendAllCard.bMingCard, m_bMingCardStatus, sizeof( m_bMingCardStatus ) ) ;

	//赖子牌
	m_bLaiZiCard=m_GameLogic.GetLaiZiCard();
	SendAllCard.bLaiZiCard=m_bLaiZiCard;
	SendAllCard.wBombTime=m_wBombTime;

	//底牌扑克
	CopyMemory( SendAllCard.bBackCardData, m_bBackCard, 3 ) ;
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_pITableFrame->SendTableData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
		m_pITableFrame->SendLookonData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
	}

	//排列扑克
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );
	}

	return true;
}

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch ( cbReason )
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT( pIServerUserItem != NULL );
			ASSERT( wChairID < m_wPlayerCount );

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
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
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory( &GameEnd, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//变量定义
			LONGLONG lCellScore = m_pGameServiceOption->lCellScore;
			bool bLandWin = ( m_bCardCount[ m_wBankerUser ] == 0 ) ? true : false;

			//春天判断
			if ( wChairID == m_wBankerUser )
			{
				WORD wUser1 = ( m_wBankerUser + 1 ) % GAME_PLAYER;
				WORD wUser2 = ( m_wBankerUser + 2 ) % GAME_PLAYER;
				if ( ( m_bOutCardCount[ wUser1 ] == 0 ) && ( m_bOutCardCount[ wUser2 ] == 0 ) )
				{
					m_wBombTime *= 2;
					GameEnd.iOtherAdd[4]=1;
				}
				else
					GameEnd.iOtherAdd[4]=2;
			}
			else
			{
				if ( m_bOutCardCount[ m_wBankerUser ] == 1 ) m_wBombTime *= 2;
			}

			//加倍倍数
			BYTE bUserDouble=1;
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				bUserDouble*=m_bUserDouble[i];
			}

			//游戏积分
			LONGLONG lScore=0;
			LONGLONG lRevenue=0;
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			m_bLandScore=__max(m_bLandScore,1);

			//统计积分
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{

				lScore=0;
				lRevenue=0;

				//统计积分
				if ( i == m_wBankerUser )
				{
					lScore = m_wBombTime * m_bLandScore * lCellScore * bUserDouble*( ( bLandWin == true ) ? 2 : -2 );
					GameEnd.lGameScore[ i ] = lScore;
					ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
				}
				else 
				{
					lScore = m_wBombTime * m_bLandScore * lCellScore *  bUserDouble *( ( bLandWin == true ) ? -1 : 1 );

					GameEnd.lGameScore[ i ] = lScore;
					ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
				}

				//胜利类型
				ScoreInfoArray[i].cbType=(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				//计算税收
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						//计算税收
						GameEnd.lGameTax+= m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
						lRevenue = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
						ScoreInfoArray[i].lRevenue=lRevenue;

						//积分调整
						lScore=lScore-lRevenue;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
						ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
					}
				}
				//历史积分
				//m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}

			//负分处理
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				SCORE lScore=0l;
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem * pIServerUser=m_pITableFrame->GetTableUserItem(i);
					if(GameEnd.lGameScore[i] < 0 && pIServerUser->GetUserScore() < -GameEnd.lGameScore[i])
					{
						if(lScore < pIServerUser->GetUserScore() || lScore == 0)
							lScore = pIServerUser->GetUserScore();
					}
				}

				if(lScore != 0)
				{
					if(GameEnd.lGameScore[m_wBankerUser] < 0)
					{
						GameEnd.lGameScore[m_wBankerUser]=-(lScore);
						GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER]=(lScore/2);
						GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER]=(lScore/2);

						ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
						ScoreInfoArray[(m_wBankerUser+1)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER];
						ScoreInfoArray[(m_wBankerUser+2)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER];
					}
					else
					{
						GameEnd.lGameScore[m_wBankerUser]=(lScore*2);
						GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER]=-(lScore);
						GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER]=-(lScore);

						ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
						ScoreInfoArray[(m_wBankerUser+1)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER];
						ScoreInfoArray[(m_wBankerUser+2)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER];
					}

					for(BYTE i=0;i<GAME_PLAYER;i++)
					{
						if (GameEnd.lGameScore[i]>=100L)
						{
							//计算税收
							GameEnd.lGameTax+= m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
							lRevenue = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
							ScoreInfoArray[i].lRevenue=lRevenue;

							//积分调整
							lScore=lScore-lRevenue;
							GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
							ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
						}
					}
				}
			}

			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}

			//修改分数
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));


			GameEnd.iOtherAdd[0]=m_wBombTime;
			GameEnd.iOtherAdd[2]=m_wQiangLandDouble;
			GameEnd.iOtherAdd[3]=m_wBomb;
			CopyMemory(GameEnd.bUserAdd,m_bUserDouble,sizeof(m_bUserDouble));

			if(GameEnd.iOtherAdd[4] == 0)
				GameEnd.iOtherAdd[4]=1;

			//明牌倍数
			BYTE bMingCard=0;
			for(BYTE i=0;i<CountArray(m_bMingCardStatus);i++)
			{
				if(m_bMingCardStatus[i] < bMingCard)
					bMingCard=m_bMingCardStatus[i];
			}
			if(bMingCard == 3)
				GameEnd.iOtherAdd[1]=2;
			else if(bMingCard == 2)
				GameEnd.iOtherAdd[1]=4;
			else if(bMingCard == 1)
				GameEnd.iOtherAdd[1]=5;
			else
				GameEnd.iOtherAdd[1]=1;

			//局数
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(m_pITableFrame->GetTableUserItem(i) != NULL)
				{
					m_wPlayTime[i]++;
					GameEnd.wPlayTime[i]=m_wPlayTime[i];
				}
				else
				{
					m_wPlayTime[i]=0;
					GameEnd.wPlayTime[i]=m_wPlayTime[i];
				}
			}

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				GameEnd.lGameResult[i]=pHistoryScore->lCollectScore;
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//切换用户
			m_wFirstUser=wChairID;

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);

			return true;
		}
	case GER_USER_LEAVE:	//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//炸弹限制
			m_wBombTime=__min(m_wBombTime,16);
			m_bLandScore=__max(m_bLandScore,1);

			//修改积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//游戏积分
				LONGLONG lScore=0;
				LONGLONG lRevenue=0;
				ScoreInfoArray[i].cbType=SCORE_TYPE_DRAW;


				//构造变量
				if (i==wChairID)
				{
					ScoreInfoArray[i].cbType=SCORE_TYPE_FLEE;
					lScore	 =-m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*2;
					GameEnd.lGameScore[i]=-m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*2;
					ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
				}
				else if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					//统计积分
					//ScoreKind=SCORE_TYPE_WIN;
					//lScore=(m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*4)/2;
					//GameEnd.lGameScore[i]=(m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*4)/2;

					////计算税收
					//if (lScore>=100L)
					//{
					//	//计算税收
					//	GameEnd.lGameTax+=GameEnd.lGameScore[i]*m_pGameServiceOption->cbRevenue/100L;
					//	lRevenue = GameEnd.lGameScore[i]*m_pGameServiceOption->cbRevenue/100L;

					//	//积分调整
					//	lScore=lScore-lRevenue ;
					//	GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue ;
					//}
				}

				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}
			//负分处理
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				SCORE lScore=0l;
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem * pIServerUser=m_pITableFrame->GetTableUserItem(i);
					if(GameEnd.lGameScore[i] < 0 && pIServerUser->GetUserScore() < -GameEnd.lGameScore[i])
					{
						if(lScore < pIServerUser->GetUserScore() || lScore == 0)
							lScore = pIServerUser->GetUserScore();
					}
				}

				if(lScore != 0)
				{
					if(GameEnd.lGameScore[m_wBankerUser] < 0)
					{
						GameEnd.lGameScore[m_wBankerUser]=-(lScore);
						GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER]=(lScore/2);
						GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER]=(lScore/2);

						ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
						ScoreInfoArray[(m_wBankerUser+1)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER];
						ScoreInfoArray[(m_wBankerUser+2)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER];
					}
					else
					{
						GameEnd.lGameScore[m_wBankerUser]=(lScore*2);
						GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER]=-(lScore);
						GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER]=-(lScore);

						ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
						ScoreInfoArray[(m_wBankerUser+1)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+1)%GAME_PLAYER];
						ScoreInfoArray[(m_wBankerUser+2)%GAME_PLAYER].lScore=GameEnd.lGameScore[(m_wBankerUser+2)%GAME_PLAYER];
					}

					for(BYTE i=0;i<GAME_PLAYER;i++)
					{
						if (GameEnd.lGameScore[i]>=100L)
						{
							//计算税收
							GameEnd.lGameTax+= m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);
							ScoreInfoArray[i].lRevenue=GameEnd.lGameTax;

							//积分调整
							GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-GameEnd.lGameTax;
							ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
						}
					}
				}
			}

			//修改积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			GameEnd.iOtherAdd[0]=m_wBombTime;
			GameEnd.iOtherAdd[2]=m_wQiangLandDouble;
			GameEnd.iOtherAdd[3]=m_wBomb;
			CopyMemory(GameEnd.bUserAdd,m_bUserDouble,sizeof(m_bUserDouble));

			if(GameEnd.iOtherAdd[4] == 0)
				GameEnd.iOtherAdd[4]=1;

			//明牌倍数
			BYTE bMingCard=0;
			for(BYTE i=0;i<CountArray(m_bMingCardStatus);i++)
			{
				if(m_bMingCardStatus[i] < bMingCard)
					bMingCard=m_bMingCardStatus[i];
			}
			if(bMingCard == 3)
				GameEnd.iOtherAdd[1]=2;
			else if(bMingCard == 2)
				GameEnd.iOtherAdd[1]=4;
			else if(bMingCard == 1)
				GameEnd.iOtherAdd[1]=5;
			else
				GameEnd.iOtherAdd[1]=1;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_WK_FREE);

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

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
			memset(&StatusScore,0,sizeof(StatusScore));

			//设置变量
			StatusScore.bLandScore=m_bLandScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusScore.wBombTime=m_wBombTime;
			CopyMemory(StatusScore.bMingCardStatus,m_bMingCardStatus,sizeof(StatusScore.bMingCardStatus));
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
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wBombTime=m_wBombTime;
			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.bLandScore=m_bLandScore;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusPlay.bLaiZiCard=m_bLaiZiCard;
			CopyMemory(StatusPlay.bMingCardStatus,m_bMingCardStatus,sizeof(StatusPlay.bMingCardStatus));
			CopyMemory(StatusPlay.bBackCard,m_bBackCard,sizeof(m_bBackCard));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
	//		CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData,sizeof(BYTE)*sizeof(m_bHandCardData));
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			
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
	case GS_WK_QIANG_LAND:	//抢地主
		{
			//构造数据
			CMD_S_StatusQiangLand QiangLand;
			memset(&QiangLand,0,sizeof(QiangLand));

			//设置变量
			QiangLand.wCurrentUser=m_wCurrentUser;
			QiangLand.lBaseScore=m_pGameServiceOption->lCellScore;
			QiangLand.wBombTime=m_wBombTime;
			CopyMemory(QiangLand.bMingCardStatus,m_bMingCardStatus,sizeof(QiangLand.bMingCardStatus));
			CopyMemory(QiangLand.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
			CopyMemory(QiangLand.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( QiangLand.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			CopyMemory(QiangLand.bMingCard,m_bMingCardStatus,sizeof(m_bMingCardStatus));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				QiangLand.lTurnScore[i]=pHistoryScore->lTurnScore;
				QiangLand.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&QiangLand,sizeof(QiangLand));
		}
	case GS_WK_ADD:			//加倍
		{
			//构造数据
			CMD_S_StatusAdd Add;
			memset(&Add,0,sizeof(Add));

			//设置变量
			Add.wLandUser=m_wBankerUser;
			Add.wCurrentUser=m_wCurrentUser;
			Add.lBaseScore=m_pGameServiceOption->lCellScore;
			Add.wBombTime=m_wBombTime;
			Add.bLaiZiCard=m_bLaiZiCard;
			CopyMemory(Add.bMingCardStatus,m_bMingCardStatus,sizeof(Add.bMingCardStatus));
			CopyMemory(Add.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
			CopyMemory(Add.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( Add.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			CopyMemory(Add.bMingCard,m_bMingCardStatus,sizeof(m_bMingCardStatus));
			CopyMemory(Add.bAddScore,m_bAddScore,sizeof(m_bAddScore));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				Add.lTurnScore[i]=pHistoryScore->lTurnScore;
				Add.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&Add,sizeof(Add));
		}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}
//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_LAND_SCORE:	//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_LandScore));
			if (wDataSize!=sizeof(CMD_C_LandScore)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_LandScore * pLandScore=(CMD_C_LandScore *)pData;
			return OnUserLandScore(pUserData->wChairID,pLandScore->bLandScore);
		}
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
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount,pOutCard->bChangeCard);
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			return OnUserPassCard(pUserData->wChairID);
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
	case SUB_C_MINGSTATE:		//发牌明牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_MingState));
			if (wDataSize!=sizeof(CMD_C_MingState)) return false;

			//消息处理
			CMD_C_MingState * pMingCard=(CMD_C_MingState *)pData;
			return OnMingState(pMingCard->bMingState,pMingCard->wUserChairID,pIServerUserItem);
		}
	case SUB_C_QIANGLAND:		//抢地主
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_QiangLand));
			if (wDataSize!=sizeof(CMD_C_QiangLand)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_QiangLand * pQiangLand=(CMD_C_QiangLand *)pData;
			return OnQiangLand(pUserData->wChairID,pQiangLand->bQiangLand);
		}
	case SUB_C_MINGOUT:			//出牌明牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			return OnMingOut(pUserData->wChairID);
		}
	case SUB_C_ADD:				//加倍
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ADD));
			if (wDataSize!=sizeof(CMD_C_ADD)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_ADD * pAdd=(CMD_C_ADD *)pData;

			return OnAdd(pUserData->wChairID,pAdd->bAdd);
		}
	}

	return true;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//叫分事件
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bLandScore)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_SCORE) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//效验参数
	if (((bLandScore>3)&&(bLandScore!=255))||(bLandScore<=m_bLandScore)) return false;

	//设置变量
	if (bLandScore!=255)
	{
		m_bLandScore=bLandScore;
		m_wBankerUser=m_wCurrentUser;
		m_bQiangLand[wChairID]=false;

	}
	else		//抢地主权限
		m_bQiangLand[wChairID]=true;

	m_bScoreInfo[wChairID]=bLandScore;

	//开始判断
	if ((m_bLandScore==3)||(m_wFirstUser==(wChairID+1)%m_wPlayerCount))
	{
		//重新发牌
		if (m_bLandScore==0 && m_wBankerUser == INVALID_CHAIR) 
		{
			BYTE cbSendGoodCard = rand() % GOOD_CARD_PERCENT ;
			BYTE cbAndroidUserCount=0 ;
			WORD wAndroidUser[GAME_PLAYER]={0};

			//重置变量
			ZeroMemory(m_bScoreInfo, sizeof(m_bScoreInfo)) ;

			//机器人个数
			for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID) ;
				if(pServerUserItem->IsAndroidUser())
				{
					wAndroidUser[cbAndroidUserCount] = (wChairID) ;
					++cbAndroidUserCount ;

				}
			}

			WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
			if(cbAndroidUserCount>0) wHaveGoodCardAndroidUser = wAndroidUser[rand()%cbAndroidUserCount] ;

			//发送好牌
			if( 0 == cbSendGoodCard && wHaveGoodCardAndroidUser!=INVALID_CHAIR)
			{
				//混乱扑克
				BYTE bRandCard[54];
				m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

				BYTE cbGoodCard[NORMAL_COUNT] ;
				m_GameLogic.GetGoodCardData(cbGoodCard) ;

				//抽取好牌
				m_GameLogic.RemoveGoodCardData(cbGoodCard, NORMAL_COUNT, bRandCard, FULL_COUNT ) ;

				//分发扑克
				CopyMemory(m_bHandCardData[wHaveGoodCardAndroidUser], cbGoodCard, NORMAL_COUNT) ;
				m_bCardCount[wHaveGoodCardAndroidUser]=17;
				for (WORD i=0, j=0;i<m_wPlayerCount;i++)
				{
					if(i!=wHaveGoodCardAndroidUser)
					{
						m_bCardCount[i]=17;
						CopyMemory(&m_bHandCardData[i],&bRandCard[j*17],sizeof(BYTE)*17);
						++j ;
					}
				}
				CopyMemory(m_bBackCard,&bRandCard[34],sizeof(m_bBackCard));
				m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
			}
			else
			{
				//混乱扑克
				BYTE bRandCard[54];
				m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

				//分发扑克
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					m_bCardCount[i]=17;
					CopyMemory(&m_bHandCardData[i],&bRandCard[i*m_bCardCount[i]],sizeof(BYTE)*m_bCardCount[i]);
				}
				CopyMemory(m_bBackCard,&bRandCard[51],sizeof(m_bBackCard));
				m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
			}

			//设置用户
			m_wCurrentUser = rand() % GAME_PLAYER;
			m_wFirstUser=m_wCurrentUser;

			//发送扑克
			CMD_S_SendAllCard SendAllCard ;
			SendAllCard.wCurrentUser = m_wCurrentUser ;

			//手上扑克
			CopyMemory(SendAllCard.bCardData, m_bHandCardData, sizeof(m_bHandCardData)) ;
			//底牌扑克
			CopyMemory(SendAllCard.bBackCardData, m_bBackCard, 3) ;
			//赖子牌
			m_bLaiZiCard=m_GameLogic.GetLaiZiCard();
			SendAllCard.bLaiZiCard=m_bLaiZiCard;
			//设置明牌
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(m_bMingCardStatus[i] > 1)
					m_bMingCardStatus[i]=0;
			}
			CopyMemory( SendAllCard.bMingCard, m_bMingCardStatus, sizeof( m_bMingCardStatus ) ) ;

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
				m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
			}

			//排列扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i],ST_ORDER);
			}

			return true;
		}

		//发送消息
		CMD_S_LandScore LandScore;
		LandScore.bLandUser=wChairID;
		LandScore.bLandScore=bLandScore;
		LandScore.wCurrentUser=m_wCurrentUser;
		LandScore.bCurrentScore=m_bLandScore;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

		m_pITableFrame->SetGameStatus(GS_WK_QIANG_LAND);

		//抢地主
		CMD_S_QiangLand QiangLand;
		WORD wBankerUser=m_wBankerUser;
		do
		{
			wBankerUser=(wBankerUser+1)%m_wPlayerCount;
			if(wBankerUser == m_wBankerUser)break;
			if(m_bQiangLand[wBankerUser] == true)continue;
			if(m_bScoreInfo[wBankerUser] != 255)
			{
				QiangLand.wQiangUser=wBankerUser;
				QiangLand.bQiangLand=0;
				QiangLand.wBombTime=m_wBombTime;

				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_QIANG_LAND,&QiangLand,sizeof(QiangLand));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_QIANG_LAND,&QiangLand,sizeof(QiangLand));
				return true;
			}
		}while(true);

		//可否加倍
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			LONGLONG lScore=m_pITableFrame->GetTableUserItem(i)->GetUserScore();
			if(lScore > m_lAddScore)
			{
				m_bAddScore[i]=true;
			}
		}
		//地主加倍
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			if(m_bAddScore[m_wBankerUser] == true)
			{
				if(m_bAddScore[i] == true)
					m_bAddScore[m_wBankerUser]=true;
				else
				{
					m_bAddScore[m_wBankerUser]=false;
					break;
				}
			}
		}
		//农民加倍
		if(m_bAddScore[m_wBankerUser] == false)
		{
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				m_bAddScore[i]=false;
			}
		}

		//开始判断
		//设置状态
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

		//发送底牌
		m_bCardCount[m_wBankerUser]=20;
		CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
		m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

		//出牌信息
		m_bTurnCardCount=0;
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//发送消息
		CMD_S_GameStart GameStart;
		GameStart.wLandUser=m_wBankerUser;
		GameStart.bLandScore=m_bLandScore;
		GameStart.wCurrentUser=m_wCurrentUser;
		GameStart.wBombTime=m_wBombTime;
		CopyMemory(GameStart.bAddScore,m_bAddScore,sizeof(m_bAddScore));
		CopyMemory(GameStart.bBackCard,m_bBackCard,sizeof(m_bBackCard));

		GameStart.bIsAdd=false;
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			if(m_bAddScore[i] == true)
			{
				GameStart.bIsAdd=true;
				break;
			}
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		return true;
		
	}


	//设置用户
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

	//发送消息
	CMD_S_LandScore LandScore;
	LandScore.bLandUser=wChairID;
	LandScore.bLandScore=bLandScore;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount,BYTE bChangeCard[])
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//排序扑克
	m_GameLogic.SortCardList(bCardData, bCardCount, ST_ORDER);

	//类型判断
	int bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);

	//炸弹判断
	if ((bCardType&(1<<CT_BOMB_CARD)) != 0 ||(bCardType&(1<<CT_MISSILE_CARD)) != 0||(bCardType&(1<<CT_LAIZI_BOMB))!= 0) 
	{
		m_wBombTime*=4;
		m_wBomb*=4;
	}
	else if((bCardType&(1<<CT_RUAN_BOMB))!= 0)
	{
		m_wBombTime*=2;
		m_wBomb*=2;
	}
	
	//切换用户
	m_wTurnWiner=wChairID;
	if (m_bCardCount[wChairID]!=0)
	{
		if ((bCardType&(1<<CT_MISSILE_CARD)) == 0)m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}
	else m_wCurrentUser=INVALID_CHAIR;

	for(BYTE i=0;i<m_bTurnCardCount;i++)
	{
		if(bChangeCard[i] == NULL)
			ASSERT(FALSE);
		if(m_bTurnCardData[i] == NULL)
			ASSERT(FALSE);
	}

	//构造数据
	CMD_S_OutCard OutCard;
	ZeroMemory(&OutCard,sizeof(OutCard));
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bChangeCard,bChangeCard,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	if ((bCardType&(1<<CT_MISSILE_CARD)) != 0) m_bTurnCardCount=0;

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
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;

	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}
//明牌开始
bool CTableFrameSink::OnMingState(BYTE bMingState,WORD wUserChairID,IServerUserItem * pIServerUserItem)
{
	//明牌开始
	if (pIServerUserItem->GetUserStatus()!=GAME_STATUS_PLAY)
	{
		m_bMingCardStatus[wUserChairID]=bMingState;
		if(m_wBankerUser == INVALID_CHAIR)
			m_wBankerUser=pIServerUserItem->GetChairID();

		//设置倍数
		m_wBombTime=5;

		return true;
	}
	else		
	{
		//设置倍数
		if(m_wBombTime == 1)
			m_wBombTime*=4;

		//发牌明牌
		CMD_S_MingCard MingCard;
		MingCard.bMingCardStatus=bMingState;
		MingCard.wMingUser=wUserChairID;
		MingCard.wBombTime=m_wBombTime;
		if(m_wBankerUser == INVALID_CHAIR)
			m_wBankerUser=pIServerUserItem->GetChairID();

		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MING_CARD,&MingCard,sizeof(MingCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MING_CARD,&MingCard,sizeof(MingCard));
	}

	return true;
}
//出牌明牌
bool CTableFrameSink::OnMingOut(WORD wCurrentUser)
{
	//明牌状态
	m_bMingCardStatus[wCurrentUser]=3;

	//设置倍数
	if(m_wBombTime == 1)
		m_wBombTime*=2;

	CMD_S_MingCard MingCard;
	MingCard.bMingCardStatus=3;
	MingCard.wMingUser=wCurrentUser;
	MingCard.wBombTime=m_wBombTime;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MING_OUT,&MingCard,sizeof(MingCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MING_OUT,&MingCard,sizeof(MingCard));


	return true;
}
//抢地主
bool CTableFrameSink::OnQiangLand(WORD wChairID,bool bQiangLand)
{
	//状态判断
	if(m_pITableFrame->GetGameStatus()!=GS_WK_QIANG_LAND)return false;

	//抢地主
	if(bQiangLand)
	{
		m_wBankerUser=wChairID;

		//设置倍数
		m_wBombTime*=2;
		m_wQiangLandDouble*=2;
	}
	m_bQiangLand[wChairID]=true;

	//抢地主轮换
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_bQiangLand[i] == false)
		{
			CMD_S_QiangLand QiangLand;
			QiangLand.wQiangUser=i;
			QiangLand.bQiangLand=(BYTE)bQiangLand+1;
			QiangLand.wBombTime=m_wBombTime;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_QIANG_LAND,&QiangLand,sizeof(QiangLand));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_QIANG_LAND,&QiangLand,sizeof(QiangLand));

			return true;
		}
	}

	//可否加倍
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		LONGLONG lScore=m_pITableFrame->GetTableUserItem(i)->GetUserScore();
		if(lScore > m_lAddScore)
		{
			m_bAddScore[i]=true;
		}
	}
	//地主加倍
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_bAddScore[m_wBankerUser] == true)
		{
			if(m_bAddScore[i] == true)
				m_bAddScore[m_wBankerUser]=true;
			else
			{
				m_bAddScore[m_wBankerUser]=false;
				break;
			}
		}
	}
	//农民加倍
	if(m_bAddScore[m_wBankerUser] == false)
	{
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			m_bAddScore[i]=false;
		}
	}

	//开始判断
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

	//发送底牌
	m_bCardCount[m_wBankerUser]=20;
	CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
	m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

	//出牌信息
	m_bTurnCardCount=0;
	m_wTurnWiner=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//发送消息
	CMD_S_GameStart GameStart;
	GameStart.wLandUser=m_wBankerUser;
	GameStart.bLandScore=m_bLandScore;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBombTime=m_wBombTime;
	CopyMemory(GameStart.bAddScore,m_bAddScore,sizeof(m_bAddScore));
	CopyMemory(GameStart.bBackCard,m_bBackCard,sizeof(m_bBackCard));

	GameStart.bIsAdd=false;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_bAddScore[i] == true)
		{
			GameStart.bIsAdd=true;
			break;
		}
	}
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	return true;

}

//用户加倍
bool CTableFrameSink::OnAdd(WORD wCurrentUser,bool bAdd)
{
	//状态判断
	if(m_pITableFrame->GetGameStatus() != GS_WK_PLAYING)return false;

	//是否加倍
	if(bAdd == true)
		m_bUserDouble[wCurrentUser]=2;

	//用户切换
	CMD_S_Add Add;
	WORD wNextUser=(wCurrentUser+1)%GAME_PLAYER;
	if(m_wBankerUser != wNextUser)
	{
		Add.bAdd=bAdd;
		Add.wAddUser=wCurrentUser;
		Add.wCurrentUser=wNextUser;
		Add.bIsAdd=m_bAddScore[wNextUser];
	}
	else
	{
		Add.bAdd=bAdd;
		Add.wAddUser=wCurrentUser;
		Add.wCurrentUser=INVALID_CHAIR;
		Add.bIsAdd=false;

		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);
	}

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD,&Add,sizeof(Add));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD,&Add,sizeof(Add));

	return true;
}
//////////////////////////////////////////////////////////////////////////
