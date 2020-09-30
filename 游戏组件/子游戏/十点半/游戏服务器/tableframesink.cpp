#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define		IDI_GAME_END			1						//游戏结束定时器

#define		TIME_GAME_END			2000					//游戏结束时间


//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_lCellScore = 0L;
	m_wCurrentUser = INVALID_CHAIR;
	m_wBankerUser = INVALID_CHAIR;
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );

	//状态变量
	ZeroMemory(m_byUserStatus,sizeof(m_byUserStatus));
	ZeroMemory(m_bAddScore,sizeof(m_bAddScore));

	//下注变量
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//扑克变量
	m_bySendCardCount = 0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_byCardCount,sizeof(m_byCardCount));
	ZeroMemory(m_byHandCardData,sizeof(m_byHandCardData));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameCustomRule=NULL;

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
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
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

	//单元积分
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//单元积分
	m_lCellScore=0L;
	
	//游戏变量
	m_wCurrentUser = INVALID_CHAIR;
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );

	//状态变量
	ZeroMemory(m_byUserStatus,sizeof(m_byUserStatus));
	ZeroMemory(m_bAddScore,sizeof(m_bAddScore));

	//下注变量
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//扑克变量
	m_bySendCardCount = 0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_byCardCount,sizeof(m_byCardCount));
	ZeroMemory(m_byHandCardData,sizeof(m_byHandCardData));

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
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_TH_SCORE);

	//设置底注
	LONGLONG lMinUserScore = 0L;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
		if( pUserItem == NULL )
		{
			m_byUserStatus[i] = FALSE;
			continue;
		}

		m_byUserStatus[i] = TRUE;

		if( i == m_wBankerUser )
			m_lCellScore = __max(m_pGameServiceOption->lCellScore,pUserItem->GetUserScore()/240L);

		if( lMinUserScore == 0L || lMinUserScore > pUserItem->GetUserScore() )
			lMinUserScore = pUserItem->GetUserScore();
	}
	m_lCellScore = __min( lMinUserScore/8,m_lCellScore );

	//构造数据
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.lCellScore = m_lCellScore;
	GameStart.wBankerUser = m_wBankerUser;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

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
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//玩家底牌
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( !m_byUserStatus[i] ) continue;

				GameEnd.byCardData[i] = m_byHandCardData[i][0];
			}

			//牌点比较
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				GameEnd.lGameScore[i] -= m_lGameScore[i];

				if( !m_byUserStatus[i] || i == m_wBankerUser ) continue;

				//庄家大
				if( m_GameLogic.CompareCard(m_byHandCardData[i],m_byCardCount[i],m_byHandCardData[m_wBankerUser],m_byCardCount[m_wBankerUser]) )
				{
					GameEnd.lGameScore[i] -= m_lTableScore[i];
					GameEnd.lGameScore[m_wBankerUser] += m_lTableScore[i];
				}
				else
				{
					//玩家牌型
					FLOAT fCardScore = m_GameLogic.GetCardGenre(m_byHandCardData[i],m_byCardCount[i]);

					BYTE cbTimes = 1;
					//大天王：10倍
					if( fCardScore == CT_DA_TIAN_WANG )
						cbTimes = 10;
					//九小：9倍
					else if( fCardScore == CT_JIU_XIAO )
						cbTimes = 9;
					//八小：8倍
					else if( fCardScore == CT_BA_XIAO )
						cbTimes = 8;
					//七小：7倍
					else if( fCardScore == CT_QI_XIAO )
						cbTimes = 7;
					//六小：6倍
					else if( fCardScore == CT_LIU_XIAO )
						cbTimes = 6;
					//天王：5倍
					else if( fCardScore == CT_TIAN_WANG )
						cbTimes = 5;
					//人五小：4倍
					else if( fCardScore == CT_REN_WU_XIAO )
						cbTimes = 4;
					//五小：3倍
					else if( fCardScore == CT_WU_XIAO )
						cbTimes = 3;
					//十点半：2倍
					else if( fCardScore == 10.5 )
						cbTimes = 2;

					GameEnd.lGameScore[i] = m_lTableScore[i]*cbTimes;
					GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
				}
			}
			

			//统计积分
			LONGLONG lGameTax[GAME_PLAYER];
			ZeroMemory( lGameTax,sizeof(lGameTax) );
			if( GAME_GENRE_SCORE != m_pGameServiceOption->wServerType )
			{
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if( !m_byUserStatus[i] ) continue;

					if( GameEnd.lGameScore[i] > 0L )
						lGameTax[i] = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);

					GameEnd.lGameScore[i]-=lGameTax[i];
				}
			}

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//写入积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( !m_byUserStatus[i] ) continue;

				if( GameEnd.lGameScore[i] > 0L ) ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
				else if( GameEnd.lGameScore[i] < 0L )  ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
				else  ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;

				 ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
				 ScoreInfoArray[i].lRevenue=lGameTax[i];  
			}
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//庄家切换
			if( GameEnd.lGameScore[m_wBankerUser] < 0 )
			{
				m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER;
				while( !m_byUserStatus[m_wBankerUser] ) m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER;
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
		{
			//庄家强退
			if( wChairID == m_wBankerUser )
			{
				//扣所有玩家初始下注数之和
				LONGLONG lScore[GAME_PLAYER] = {0};
				LONGLONG lGameTax[GAME_PLAYER] = {0};
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( !m_byUserStatus[i] || i == wChairID ) continue;

					lScore[i] = m_lTableScore[i];

					if( GAME_GENRE_SCORE != m_pGameServiceOption->wServerType )
					{
						lGameTax[i] = m_pITableFrame->CalculateRevenue(i,lScore[i]);
						lScore[i] -= lGameTax[i];
					}

					lScore[m_wBankerUser] -= m_lTableScore[i];
				}

				//发送消息
				CMD_S_GameEnd ge = {};
				CopyMemory( ge.lGameScore,lScore,sizeof(ge.lGameScore) );
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&ge,sizeof(ge));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&ge,sizeof(ge));

				//写分
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( !m_byUserStatus[i] ) continue;

					if( ge.lGameScore[i] > 0L ) ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
					else if( ge.lGameScore[i] < 0L ) ScoreInfoArray[i].cbType = SCORE_TYPE_FLEE;
					else ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;

					ScoreInfoArray[i].lRevenue=lGameTax[i];                                  
					ScoreInfoArray[i].lScore=ge.lGameScore[i];

				}
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				//通知消息
				TCHAR szMessage[128]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("由于庄家 [ %s ] 逃跑，游戏结束！"),pIServerUserItem->GetNickName());
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					IServerUserItem * pISendUserItem=m_pITableFrame->GetTableUserItem(i);
					if (pISendUserItem!=NULL) 
						m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_PROMPT);
				}

				WORD wIndex=0;
				do
				{
					IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
					if (pISendUserItem==NULL)
						break;
					m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_PROMPT);
				} while (true);

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			else
			{
				//扣玩家分
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				m_lGameScore[wChairID] = m_lTableScore[wChairID];
				ScoreInfoArray[wChairID].lScore=-m_lTableScore[wChairID];
				ScoreInfoArray[wChairID].cbType=SCORE_TYPE_FLEE;

				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				//扣分给庄家
				LONGLONG lScore = m_lTableScore[wChairID];
				LONGLONG lTax = 0L;
				if( GAME_GENRE_SCORE != m_pGameServiceOption->wServerType )
				{
					lTax = m_pITableFrame->CalculateRevenue(wChairID,lScore);
					lScore -= lTax;
				}

				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

				ScoreInfoArray[m_wBankerUser].cbType =SCORE_TYPE_WIN;                           
				ScoreInfoArray[m_wBankerUser].lScore=lScore;

				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				//剩下玩家
				BYTE cbUserCount = 0;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( m_byUserStatus[i] ) cbUserCount++;
				}
				bool bGameEnd = cbUserCount==2?true:false;

				//如果只剩庄家
				if( bGameEnd )
				{
					//庄家得分
					LONGLONG lScore[GAME_PLAYER] = {0};
					LONGLONG lGameTax[GAME_PLAYER] = {0};
					for( WORD i = 0; i < GAME_PLAYER; i++ )
					{
						if( i == m_wBankerUser || !m_byUserStatus[i] ) continue;

						lScore[i] -= m_lTableScore[i];

						lScore[m_wBankerUser] += m_lTableScore[i];
					}

					m_byUserStatus[wChairID] = FALSE;

					if( GAME_GENRE_SCORE != m_pGameServiceOption->wServerType )
					{
						lGameTax[m_wBankerUser] = m_pITableFrame->CalculateRevenue(m_wBankerUser,lScore[m_wBankerUser]);
						lScore[m_wBankerUser] -= lGameTax[m_wBankerUser];
					}
					
					//发送消息
					CMD_S_GameEnd ge = {};
					CopyMemory( ge.lGameScore,lScore,sizeof(ge.lGameScore) );
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&ge,sizeof(ge));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&ge,sizeof(ge));
				}
				else
				{
					m_byUserStatus[wChairID] = FALSE;

					if( wChairID == m_wCurrentUser )
					{
						m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
						while( !m_byUserStatus[m_wCurrentUser] ) m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
					}

					//发送消息
					CMD_S_UserLeft ul = {};
					ul.wLeftUser = wChairID;
					ul.wCurrentUser = m_wCurrentUser;
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_LEFT,&ul,sizeof(ul));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_LEFT,&ul,sizeof(ul));
				}

				//通知消息
				TCHAR szMessage[128]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("闲家 [ %s ] 逃跑，扣除%ld分！"),pIServerUserItem->GetNickName(),m_lTableScore[wChairID]);
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					IServerUserItem * pISendUserItem=m_pITableFrame->GetTableUserItem(i);
					if (pISendUserItem!=NULL) 
						m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_PROMPT);
				}

				WORD wIndex=0;
				do
				{
					IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
					if (pISendUserItem==NULL)
						break;
					m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_PROMPT);
				} while (true);

				//下注状态
				if( !bGameEnd && m_pITableFrame->GetGameStatus() == GS_TH_SCORE )
				{
					bool bScoreFinish = true;
					for( WORD i = 0; i < GAME_PLAYER; i++ )
					{
						if( !m_byUserStatus[i] || i == m_wBankerUser ) continue;

						if( m_lTableScore[i] == 0 )
						{
							bScoreFinish = false;
							break;
						}
					}

					if( bScoreFinish )
					{
						SendGamePlay();
						return true;
					}
				}
				
				//结束游戏
				if( bGameEnd )
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
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
	case GS_TH_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TH_SCORE:	//下注状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore = {};

			//单无积分
			StatusScore.lCellScore = m_lCellScore;

			//状态变量
			StatusScore.wBankerUser = m_wBankerUser;
			CopyMemory(StatusScore.bUserStatus,m_byUserStatus,sizeof(m_byUserStatus));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TH_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//单无积分
			StatusPlay.lCellScore = m_lCellScore;

			//状态变量
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.wBankerUser = m_wBankerUser;
			CopyMemory( StatusPlay.byUserStatus,m_byUserStatus,sizeof(m_byUserStatus) );

			//下注变量
			CopyMemory( StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore) );

			//扑克变量
			CopyMemory( StatusPlay.cbCardCount,m_byCardCount,sizeof(m_byCardCount) );
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( m_byCardCount[i] == 0 ) continue;
				CopyMemory( &StatusPlay.cbHandCardData[i][1],&m_byHandCardData[i][2],
					(m_byCardCount[i]-1)*sizeof(BYTE) );
				if( m_byUserStatus[i] && i == wChairID && bSendSecret )
					StatusPlay.cbHandCardData[i][0] = m_byHandCardData[i][0];
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
//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
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
	case SUB_C_ADD_SCORE:	//下注
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TH_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_TH_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnSubUserAddScore( pIServerUserItem->GetChairID() );
		}
	case SUB_C_GIVE_UP:
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TH_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_TH_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnSubUserGiveUp( pIServerUserItem->GetChairID() );
		}
	case SUB_C_GIVE_CARD:
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TH_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GS_TH_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnSubUserGiveCard( pIServerUserItem->GetChairID() );
		}
	case SUB_C_SCORE:
		{
			ASSERT( wDataSize == sizeof(CMD_C_Score) );
			if( wDataSize != sizeof(CMD_C_Score) ) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TH_SCORE);
			if (m_pITableFrame->GetGameStatus()!=GS_TH_SCORE) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息定义
			CMD_C_Score *pScore = (CMD_C_Score *)pData;
			return OnSubUserScore(pIServerUserItem->GetChairID(),pScore->lScore);
		}
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//玩家下注
bool CTableFrameSink::OnSubUserAddScore( WORD wChairId )
{
	//校验
	ASSERT( wChairId == m_wCurrentUser );
	if( wChairId != m_wCurrentUser ) return false;
	ASSERT( !m_bAddScore[wChairId] );
	if( m_bAddScore[wChairId] ) return true;

	ASSERT( m_byCardCount[wChairId] < MAX_COUNT );
	if( m_byCardCount[wChairId] >= MAX_COUNT ) return false;

	FLOAT fCardScore = m_GameLogic.GetCardGenre(m_byHandCardData[wChairId],m_byCardCount[wChairId]);
	ASSERT( fCardScore != CT_ERROR );
	if( fCardScore == CT_ERROR ) return false;

	m_bAddScore[wChairId] = true;
	//更新桌面下注
	m_lTableScore[wChairId] *= 2;

	//加牌
	BYTE cbSendCardData = m_cbRepertoryCard[m_bySendCardCount++];
	m_byHandCardData[wChairId][m_byCardCount[wChairId]++] = cbSendCardData;

	//切换下一位玩家
	m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
	while( !m_byUserStatus[m_wCurrentUser] ) m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;

	//发送下注信息
	CMD_S_AddScore AddScore;
	AddScore.wAddScoreUser = wChairId;
	AddScore.wCurrentUser = m_wCurrentUser;
	AddScore.cbCardData = cbSendCardData;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore) );

	return true;
}

//玩家放弃
bool CTableFrameSink::OnSubUserGiveUp( WORD wChairId )
{
	//效验
	ASSERT( m_wCurrentUser == wChairId );
	if( m_wCurrentUser != wChairId ) return false;

	//判断是否结束
	bool bFinish = wChairId==m_wBankerUser?true:false;

	//如果游戏结束,则设置无当前玩家
	if( bFinish ) 
		m_wCurrentUser = INVALID_CHAIR;
	else
	{
		//寻找下一个玩家
		m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
		while( !m_byUserStatus[m_wCurrentUser] ) m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
	}

	//发送停牌信息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser = wChairId;
	GiveUp.wCurrentUser = m_wCurrentUser;
	m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp) );
	m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp) );

	//如果结束
	if( bFinish )
	{
		OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );
	}

	return true;
}

//玩家要牌
bool CTableFrameSink::OnSubUserGiveCard( WORD wChairId )
{
	//效验
	ASSERT( m_wCurrentUser == wChairId );
	if( m_wCurrentUser != wChairId ) return false;

	ASSERT( m_byCardCount[wChairId] < MAX_COUNT );
	if( m_byCardCount[wChairId] >= MAX_COUNT ) return false;

	FLOAT fCardScore = m_GameLogic.GetCardGenre(m_byHandCardData[wChairId],m_byCardCount[wChairId]);
	ASSERT( fCardScore != CT_ERROR );
	if( fCardScore == CT_ERROR ) return false;

	//加牌
	BYTE cbSendCardData = m_cbRepertoryCard[m_bySendCardCount++];
	m_byHandCardData[wChairId][m_byCardCount[wChairId]++] = cbSendCardData;

	//当前玩家
	BYTE cbCardData[MAX_COUNT] = {};
	BYTE cbCardCount = wChairId==m_wBankerUser?m_byCardCount[wChairId]:m_byCardCount[wChairId]-1;
	CopyMemory( cbCardData,&m_byHandCardData[wChairId][wChairId==m_wBankerUser?0:1],sizeof(BYTE)*cbCardCount );
	FLOAT fCurrentScore = m_GameLogic.GetCardGenre(cbCardData,cbCardCount);
	//如果爆牌
	if( fCurrentScore == CT_ERROR )
	{
		if( m_wBankerUser == wChairId )
			m_wCurrentUser = INVALID_CHAIR;
		else
		{
			m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
			while( !m_byUserStatus[m_wCurrentUser] ) m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;
		}
	}

	//发送信息
	CMD_S_SendCard sc = {};
	sc.wCurrentUser = m_wCurrentUser;
	sc.wSendCardUser = wChairId;
	sc.cbCardData = cbSendCardData;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&sc,sizeof(sc));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&sc,sizeof(sc));

	//如果是庄家爆牌，则直接结束
	if( fCurrentScore == CT_ERROR && m_wCurrentUser == INVALID_CHAIR )
	{
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//玩家下注
bool CTableFrameSink::OnSubUserScore(WORD wChairId,LONGLONG lScore)
{
	//效验
	ASSERT( m_pITableFrame->GetGameStatus() == GS_TH_SCORE );
	if( m_pITableFrame->GetGameStatus() != GS_TH_SCORE ) return false;
	ASSERT( lScore%m_lCellScore==0 && lScore/m_lCellScore >= 1 && lScore/m_lCellScore <= 4 );
	if( lScore%m_lCellScore != 0 || lScore/m_lCellScore == 0 || lScore/m_lCellScore > 4 ) return false;
	ASSERT( m_lTableScore[wChairId] == 0 );
	if( m_lTableScore[wChairId] != 0 ) return true;

	//设置下注
	m_lTableScore[wChairId] = lScore;

	//查看是否全部下注
	bool bFinishScore = true;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_byUserStatus[i] || i == m_wBankerUser ) continue;
		if( m_lTableScore[i] == 0 )
		{
			bFinishScore = false;
			break;
		}
	}
	if( !bFinishScore ) return true;

	SendGamePlay();

	return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( !bLookonUser && m_wBankerUser == INVALID_CHAIR )
	{
		m_wBankerUser = wChairID;
	}

	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if( !bLookonUser && m_wBankerUser == wChairID )
	{
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
			if( pUserItem )
			{
				m_wBankerUser = i;
				break;
			}
		}
		if( i == GAME_PLAYER ) m_wBankerUser = INVALID_CHAIR;
	}

	return true;
}

//开始发牌
VOID CTableFrameSink::SendGamePlay()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_TH_PLAY);

	//随机取牌
	m_GameLogic.RandCardList(m_cbRepertoryCard,sizeof(m_byHandCardData));

	//确定当前玩家
	m_wCurrentUser = (m_wBankerUser+1)%GAME_PLAYER;
	while( !m_byUserStatus[m_wCurrentUser] ) m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER;

	m_bySendCardCount = 0;
	//设置玩家牌
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_byUserStatus[i] ) continue;
		m_byHandCardData[i][0] = m_cbRepertoryCard[m_bySendCardCount++];
		m_byCardCount[i] = 1;
	}

	//构造数据
	CMD_S_GamePlay GamePlay = {};
	GamePlay.wCurrentUser = m_wCurrentUser;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
		GamePlay.lTableScore[i] = m_lTableScore[i];

	//发送数据
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_byUserStatus[i] )
		{
			GamePlay.byCardData[i] = m_byHandCardData[i][0];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
			GamePlay.byCardData[i] = 0;
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
