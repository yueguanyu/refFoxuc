#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
//const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

#define	STOCK_TAX						2									//回扣宏量

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));
	ZeroMemory(m_cbOpenFinish,sizeof(m_cbOpenFinish));
	ZeroMemory(m_bInvestFinish,sizeof(m_bInvestFinish));

	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	m_wOperaCount=0;
	m_cbBalanceCount=0;
	m_lBalanceScore=0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lLoseScore,sizeof(m_lLoseScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询--检测相关信息版本
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
#ifdef __SPECIAL___
	QUERYINTERFACE(ITableUserActionEX,Guid,dwQueryVer);	
#endif
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

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);
	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//复位桌子
void  CTableFrameSink::RepositionSink()
{
	//游戏变量
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));
	ZeroMemory(m_cbOpenFinish,sizeof(m_cbOpenFinish));
	ZeroMemory(m_bInvestFinish,sizeof(m_bInvestFinish));

	m_wBankerUser=INVALID_CHAIR;				
	m_wCurrentUser=INVALID_CHAIR;				

	m_wOperaCount=0;
	m_cbBalanceCount=0;
	m_lBalanceScore=0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	ZeroMemory(m_lLoseScore,sizeof(m_lLoseScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	return;
}

////开始模式
//enStartMode  CTableFrameSink::GetGameStartMode()
//{
//	return m_GameStartMode;
//}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]==TRUE || m_lLoseScore[wChairID]<0));
	if(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]==TRUE || m_lLoseScore[wChairID]<0))return true;

	return false;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_TK_INVEST);

	//用户状态
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);

		if(pIServerUser==NULL)
		{
			m_cbPlayStatus[i]=FALSE;
		}
		else
		{
			m_cbPlayStatus[i]=TRUE;
		}
	}

	//首局随机
	if(m_wBankerUser==INVALID_CHAIR)
	{
		m_wBankerUser = rand()%m_wPlayerCount;
	}

	//确认庄家
	do
	{
		m_wBankerUser =(m_wBankerUser+1)%m_wPlayerCount;
	}while(m_cbPlayStatus[m_wBankerUser]!=TRUE);

	//当前用户
	m_wCurrentUser=m_wBankerUser;

	//设置变量
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;
	CopyMemory(GameStart.lUserScore,m_lUserScore,sizeof(m_lUserScore));

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	return true;
}

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_NORMAL;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(GameEnd.cbCardData));

			//判断强退
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE && m_lTotalScore[i]>0)
				{
					m_cbPlayStatus[i]=TRUE;
					ZeroMemory(m_cbHandCardData[i],sizeof(BYTE)*MAX_COUNT);
				}
			}

			//结束得分
			BYTE cbPlayCount=0;
			do
			{
				//辅助变量
				BYTE cbSpeCount = 2;
				struct LoseInfo
				{
					WORD wPlyObjUser[GAME_PLAYER];
					WORD wPlyObjCount;
				}LoseUserInof[GAME_PLAYER];
				ZeroMemory(LoseUserInof,sizeof(LoseUserInof));

				//判断输家
				BYTE cbLoseUser[GAME_PLAYER];
				ZeroMemory(cbLoseUser,sizeof(cbLoseUser));
				for(WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]==FALSE)continue;

					for(WORD j=0;j<m_wPlayerCount;j++)
					{
						if(m_cbPlayStatus[j]==FALSE || i==j)continue;

						//比较大小
						INT nHeadResult=m_GameLogic.CompareCard(m_cbHandCardData[i],m_cbHandCardData[j],cbSpeCount);
						INT nTailResult=m_GameLogic.CompareCard(&m_cbHandCardData[i][cbSpeCount],&m_cbHandCardData[j][cbSpeCount],cbSpeCount);
						if(nHeadResult==0xff)nHeadResult=-1;
						if(nTailResult==0xff)nTailResult=-1;

						if(nHeadResult+nTailResult<0)
						{
							cbLoseUser[i]=TRUE;
							break;
						}
					}
				}

				//查找输家
				for(WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]==FALSE || cbLoseUser[i]==FALSE)continue;

					for(WORD t=0;t<m_wPlayerCount;t++)
					{
						//庄家优先
						WORD j=(t+m_wBankerUser)%GAME_PLAYER;
						if(m_cbPlayStatus[j]==FALSE || i==j || cbLoseUser[j]==TRUE)continue;

						//比较大小
						INT nHeadResult=m_GameLogic.CompareCard(m_cbHandCardData[i],m_cbHandCardData[j],cbSpeCount);
						INT nTailResult=m_GameLogic.CompareCard(&m_cbHandCardData[i][cbSpeCount],&m_cbHandCardData[j][cbSpeCount],cbSpeCount);
						if(nHeadResult==0xff)nHeadResult=-1;
						if(nTailResult==0xff)nTailResult=-1;

						if(nHeadResult+nTailResult<0)
						{
							LoseUserInof[i].wPlyObjUser[LoseUserInof[i].wPlyObjCount++] = j;
						}
					}
				}

				//玩家得分
				LONGLONG lUserGetScore[GAME_PLAYER];
				ZeroMemory(lUserGetScore,sizeof(lUserGetScore));
				for(WORD i=0;i<m_wPlayerCount;i++)
				{
					LoseInfo *pLoseInfo = &LoseUserInof[i];
					WORD wPlyCount=pLoseInfo->wPlyObjCount;

					//计算得分
					while(wPlyCount>0 && m_lTotalScore[i]>0)
					{
						//查找头大
						WORD wIndex = 0;
						WORD wHeadMaxUser = pLoseInfo->wPlyObjUser[0];
						for(WORD j=1;j<wPlyCount;j++)
						{
							//当前用户
							WORD wCurUser = pLoseInfo->wPlyObjUser[j];
							if(m_cbPlayStatus[wCurUser]==FALSE)continue;

							//比较大小
							BYTE cbResult=m_GameLogic.CompareCard(m_cbHandCardData[wCurUser],m_cbHandCardData[wHeadMaxUser],cbSpeCount);
							if(cbResult==TRUE)
							{
								wIndex=j;
								wHeadMaxUser=wCurUser;
							}
						}

						//结算得分
						LONGLONG lTempScore=__min(m_lTotalScore[wHeadMaxUser],m_lTotalScore[i]);
						lUserGetScore[wHeadMaxUser]+=lTempScore;
						lUserGetScore[i]-=lTempScore;
						m_lTotalScore[i]-=lTempScore;

						//判断结束
						if(m_lTotalScore[i]==0 || wPlyCount==1)break;

						//清理用户
						WORD wTempUser[GAME_PLAYER],wTempCount=0;
						CopyMemory(wTempUser,pLoseInfo->wPlyObjUser,sizeof(WORD)*wPlyCount);
						wTempUser[wIndex]=INVALID_CHAIR;
						for(WORD j=0;j<wPlyCount;j++)
						{
							if(wTempUser[j]!=INVALID_CHAIR)
								pLoseInfo->wPlyObjUser[wTempCount++]=wTempUser[j];
						}
						ASSERT(wTempCount==wPlyCount-1);
						wPlyCount=wTempCount;
					}
				}

				//保存结果
				for(WORD i=0;i<GAME_PLAYER;i++)
				{
					GameEnd.lGameScore[i]+=lUserGetScore[i];
					if(lUserGetScore[i]>=0 || m_lTotalScore[i]==0)
					{
						m_lTotalScore[i]=0;
						m_cbPlayStatus[i]=FALSE;
					}
				}

				//判断结束
				cbPlayCount=0;
				for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]!=FALSE)cbPlayCount++;

			}while(cbPlayCount>=2);

			//计算税收
			LONGLONG lRevenue=m_pGameServiceOption->wRevenueRatio;
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				if(GameEnd.lGameScore[i]>=100)
				{
					GameEnd.lGameTax[i]=GameEnd.lGameScore[i]*lRevenue/1000;
					GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
				}

				//用户总币
				if(m_lUserScore[i]>0)m_lUserScore[i]+=GameEnd.lGameScore[i];
			}

			//发送信息
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//写入积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//放弃玩家
				if(m_lLoseScore[i]<0)
				{
					//扣除下注
					ASSERT(m_pITableFrame->GetTableUserItem(i)!=NULL);
					//m_pITableFrame->WriteUserScore(i,m_lLoseScore[i],0,enScoreKind_Lost);

					ScoreInfo[i].lScore=m_lLoseScore[i];
					ScoreInfo[i].cbType=SCORE_TYPE_LOSE;
					m_lUserScore[i]+=m_lLoseScore[i];
				}

				if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
				//enScoreKind nScoreKind=enScoreKind_Draw;
				ScoreInfo[i].cbType=SCORE_TYPE_DRAW;
				if(GameEnd.lGameScore[i]!=0)
					ScoreInfo[i].cbType=(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].lRevenue = GameEnd.lGameTax[i];
				//m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],GameEnd.lGameTax[i],nScoreKind);
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
		{
			//游戏状态
			WORD wGameStatus=m_pITableFrame->GetGameStatus();

			//用户放弃
			bool bGiveUp=false;
			if(WORD(wChairID-GAME_PLAYER)<WORD(GAME_PLAYER) && pIServerUserItem!=NULL)
			{
				bGiveUp=true;
				wChairID-=GAME_PLAYER;
			}

			//放弃强退
			bool bHaveGiveUp=false;
			if(m_cbPlayStatus[wChairID]==FALSE)bHaveGiveUp=true;

			//清理数据
			m_cbShowHand[wChairID]=FALSE;
			m_cbPlayStatus[wChairID]=FALSE;

			//用户人数
			WORD wUserCount =0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=FALSE)wUserCount++;
			}

			//扣除分数
			if(!bGiveUp || wUserCount<2)
			{
				//最大下注
				LONGLONG lMaxScore=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]==FALSE || i==wChairID)continue;

					if(lMaxScore<m_lUserScore[i])lMaxScore=m_lUserScore[i];
				}

				//最大输分
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_lLoseScore[i]==0)continue;
					LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);
					if(lMaxScore<lLose)lMaxScore=lLose;
				}

				//实质下注
				m_lLoseScore[wChairID]=__min(lMaxScore,m_lTotalScore[wChairID]);

				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

				//扣除下注
				LONGLONG lLoseScore=-m_lLoseScore[wChairID];

				ScoreInfo[wChairID].lScore = lLoseScore;
				ScoreInfo[wChairID].cbType = SCORE_TYPE_LOSE;
				//m_pITableFrame->WriteUserScore(wChairID,lLoseScore,0,enScoreKind_Lost);
				m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
				m_lUserScore[wChairID]-=m_lLoseScore[wChairID];
			}
			else 
			{
				m_lLoseScore[wChairID]=-m_lTotalScore[wChairID];
			}

			//发送消息
			if(!bHaveGiveUp)
			{
				CMD_S_GiveUp GiveUp;
				ZeroMemory(&GiveUp,sizeof(GiveUp));
				GiveUp.wGiveUpUser=wChairID;
				GiveUp.wGameStatus= wGameStatus;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
			}

			//结束游戏
			if(wUserCount<2)
			{
				//构造数据
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));
				GameEnd.cbEndReason=END_REASON_EXIT;

				//胜利玩家
				WORD wWinner=0;
				for(WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]!=FALSE)
					{
						wWinner=i;
						break;
					}
				}

				
				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
				//总注数目
				LONGLONG lAllGold=0,lCount=0;
				for(WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_lLoseScore[i]!=0 && i!=wWinner)
					{
						lCount++;
						LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);
						lAllGold+=lLose;
						GameEnd.lGameScore[i]=-lLose;

						//放弃玩家
						if(m_lLoseScore[i]<0)
						{
							
							ScoreInfo[i].lScore = m_lLoseScore[i];
							ScoreInfo[i].cbType = SCORE_TYPE_LOSE;
							
							//扣除下注
							//m_pITableFrame->WriteUserScore(i,m_lLoseScore[i],0,enScoreKind_Lost);
							m_lUserScore[i]+=m_lLoseScore[i];
							m_lLoseScore[i]*=-1;
						}
					}
				}
				//m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

				//实质赢注
				lAllGold = __min(lAllGold,m_lUserScore[wWinner]*lCount);

				//胜方得分
				LONGLONG lRevenue=m_pGameServiceOption->wRevenueRatio;
				GameEnd.lGameTax[wWinner]=lAllGold*lRevenue/1000;
				GameEnd.lGameScore[wWinner]=lAllGold-GameEnd.lGameTax[wWinner];
				
				
				ScoreInfo[wWinner].lScore = GameEnd.lGameScore[wWinner];
				ScoreInfo[wWinner].cbType = SCORE_TYPE_WIN;
				ScoreInfo[wWinner].lRevenue = GameEnd.lGameTax[wWinner];

				//m_pITableFrame->WriteUserScore(wWinner,GameEnd.lGameScore[wWinner],GameEnd.lGameTax[wWinner],enScoreKind_Win);
				m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			//用户放弃
			else if(wGameStatus==GS_TK_SCORE)
			{
				if(m_wCurrentUser==wChairID)OnUserAddScore(wChairID,0,true);
			}
			//下本放弃
			else if(wGameStatus==GS_TK_INVEST)
			{
				OnUserInvest(wChairID,0);
			}
			//开牌放弃
			else OnUserOpenCard(wChairID,NULL);

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
	case GAME_STATUS_FREE:			//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TK_INVEST:		//下本状态
		{
			//构造数据
			CMD_S_StatusInvest StatusInvest;
			ZeroMemory(&StatusInvest,sizeof(StatusInvest));

			//设置数据
			StatusInvest.wBankerUser=m_wBankerUser;
			StatusInvest.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusInvest.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusInvest.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
			CopyMemory(StatusInvest.bInvestFinish,m_bInvestFinish,sizeof(m_bInvestFinish));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusInvest,sizeof(StatusInvest));
		}
	case GS_TK_SCORE:		//下注状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));

			//设置数据
			StatusScore.wBankerUser=m_wBankerUser;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lTurnMinScore=m_lTurnMinScore;	
			StatusScore.lTurnMaxScore=m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
			StatusScore.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusScore.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusScore.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));

			//明牌数据
			BYTE cbSpeCount=2;
			if(m_cbBalanceCount>0)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]==FALSE)continue;

					//用户扑克
					CopyMemory(StatusScore.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*m_cbBalanceCount);
				}
			}

			//手上扑克
			if(pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusScore.cbHandCard,m_cbHandCardData[wChiarID],sizeof(BYTE)*cbSpeCount);
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TK_OPEN_CARD:	//开牌状态
		{
			//构造数据
			CMD_S_StatusOpenCard StatusOpenCard;
			ZeroMemory(&StatusOpenCard,sizeof(StatusOpenCard));

			//设置数据
			StatusOpenCard.wBankerUser=m_wBankerUser;
			StatusOpenCard.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusOpenCard.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusOpenCard.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(StatusOpenCard.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
			CopyMemory(StatusOpenCard.cbOpenFinish,m_cbOpenFinish,sizeof(m_cbOpenFinish));

			//明牌数据
			BYTE cbSpeCount=2;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				//用户扑克
				CopyMemory(StatusOpenCard.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*cbSpeCount);
			}

			//手上扑克
			if(pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusOpenCard.cbHandCard,m_cbHandCardData[wChiarID],sizeof(BYTE)*cbSpeCount);
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusOpenCard,sizeof(StatusOpenCard));
		}
	}
	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息处理 
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_USER_INVEST:			//用户下本
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserInvest));
			if (wDataSize!=sizeof(CMD_C_UserInvest)) return false;

			//变量定义
			CMD_C_UserInvest * pUserInvest=(CMD_C_UserInvest *)pDataBuffer;

			//用户效验
			//tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			return OnUserInvest(pUserData->wChairID,pUserInvest->lInvestGold);
		}
	case SUB_C_GIVE_UP:		//用户放弃
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
			if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
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
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
		}
	case SUB_C_OPEN_CARD:			//用户摊牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OpenCard));
			if (wDataSize!=sizeof(CMD_C_OpenCard)) return false;

			//变量定义
			CMD_C_OpenCard * pOpenCard=(CMD_C_OpenCard *)pDataBuffer;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]!=FALSE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE)return false;

			//消息处理
			return OnUserOpenCard(pUserData->wChairID,pOpenCard->cbCardData);
		}
	}

	return false;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(!bLookonUser)
	{
		//清理金币
		if(m_lUserScore[wChairID]!=0)m_lUserScore[wChairID]=0;

		//金币人数
		BYTE cbHaveGoldCount=0;
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			if(m_lUserScore[i]>0)cbHaveGoldCount++;
		}
		if(cbHaveGoldCount==1)ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	}

	return true;
}

//加注信息
void CTableFrameSink::SendUserAddInfo(WORD wAddUser,LONGLONG lScore,WORD wNextUser,LONGLONG lMinScore,LONGLONG lMaxScore)
{
	//构造数据
	CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore,sizeof(AddScore));
	AddScore.lAddScoreCount=lScore;
	AddScore.wAddScoreUser=wAddUser;
	AddScore.wCurrentUser=wNextUser;
	AddScore.lTurnMinScore=lMinScore;
	AddScore.lTurnMaxScore=lMaxScore;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	return;
}

//发牌信息
void CTableFrameSink::SendSendCardInfo(BYTE cbCardCount,WORD wCurrentUser)
{
	ASSERT(cbCardCount>=2 && cbCardCount<=MAX_COUNT);

	//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.cbCardCount=cbCardCount;
	SendCard.wCurrentUser=wCurrentUser;
	//SendCard.bStartOpen = bStartOpen;

	//明牌数据
	BYTE cbMingCount = cbCardCount-2;
	if(cbMingCount>0)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;

			//用户扑克
			CopyMemory(SendCard.cbMingCard[i],&m_cbHandCardData[i][2],cbMingCount);
		}
	}

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;

		//用户扑克
		CopyMemory(SendCard.cbHandCard,m_cbHandCardData[i],sizeof(SendCard.cbHandCard));
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return;
}

//下本事件
bool CTableFrameSink::OnUserInvest(WORD wChairID, LONGLONG lInvestCount)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_INVEST);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_INVEST) return false;

	//用户积分
	IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if(pIServerUserItem==NULL)return false;
	LONGLONG lUserGold=pIServerUserItem->GetUserScore();

	if(m_cbPlayStatus[wChairID]==TRUE)
	{
		//效验信息
		ASSERT(lInvestCount>=0l && (lInvestCount+m_lUserScore[wChairID])<=lUserGold);
		if(!(lInvestCount>=0l && (lInvestCount+m_lUserScore[wChairID])<=lUserGold))return false;
		if(m_lUserScore[wChairID]==0)
		{
			ASSERT(lInvestCount>=m_pGameServiceOption->lCellScore);
			if(lInvestCount<m_pGameServiceOption->lCellScore)return false;
		}

		//设置变量
		m_bInvestFinish[wChairID]=true;
		m_lUserScore[wChairID]+=lInvestCount;

		//构造数据
		CMD_S_UserInvest UserInvest;
		ZeroMemory(&UserInvest,sizeof(UserInvest));
		UserInvest.wChairID=wChairID;
		UserInvest.lScore=m_lUserScore[wChairID];

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
			m_pITableFrame->SendTableData(i,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
	}

	//下本人数
	WORD wInvestFinishCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==TRUE && m_bInvestFinish[i]) wInvestFinishCount++;
		else if(m_cbPlayStatus[i]!=TRUE) wInvestFinishCount++;
	}

	//下个环节
	if(wInvestFinishCount==m_wPlayerCount)
	{
		//下低注数
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			m_lTotalScore[i] = m_pGameServiceOption->lCellScore;
		}

		//随机扑克
		BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
		m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
		}

		//判断梭哈
		WORD wPlayCount = 0,wShowCount = 0;
		for (BYTE i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=TRUE)continue;

			if(m_lTotalScore[i]==m_lUserScore[i])wShowCount++;
			wPlayCount++;
		}

		//设置状态
		BYTE cbStatus = ((wPlayCount>wShowCount+1)?GS_TK_SCORE:GS_TK_OPEN_CARD);
		m_pITableFrame->SetGameStatus(cbStatus);

		//下注状态
		if(wPlayCount>wShowCount+1)
		{
			//梭哈用户
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				if(m_lTotalScore[i]==m_lUserScore[i])m_cbShowHand[i]=TRUE;
			}

			//查找下位
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				m_wCurrentUser=(m_wBankerUser+i)%m_wPlayerCount;
				if(m_cbPlayStatus[m_wCurrentUser]==FALSE)continue;

				if(m_lTotalScore[m_wCurrentUser]<m_lUserScore[m_wCurrentUser])break;
			}

			SendSendCardInfo(2,m_wCurrentUser);
		}
		//开牌状态
		else 
		{
			SendSendCardInfo(MAX_COUNT,INVALID_CHAIR);

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);
		}
	}

	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	OnEventGameConclude(wChairID+GAME_PLAYER,m_pITableFrame->GetTableUserItem(wChairID),GER_USER_LEAVE);

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore,bool bGiveUp)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return false;

	//效验用户
	ASSERT(m_wCurrentUser==wChairID);
	if(m_wCurrentUser!=wChairID)return false;

	//用户操作
	if(!bGiveUp)
	{
		//校验金币
		ASSERT(lScore>=m_lTurnMinScore && (lScore+m_lTotalScore[wChairID])<=m_lUserScore[wChairID]);
		if (lScore<m_lTurnMinScore || (lScore+m_lTotalScore[wChairID])>m_lUserScore[wChairID])return false;

		//累计金币
		m_lTableScore[wChairID] += lScore;
		m_lTotalScore[wChairID] += lScore;

		//平衡下注
		if(m_lTableScore[wChairID] > m_lBalanceScore)
		{
			m_lBalanceScore = m_lTableScore[wChairID];
		}

		//梭哈判断
		if(m_lTotalScore[wChairID]==m_lUserScore[wChairID])m_cbShowHand[wChairID] = TRUE;
	}

	//用户切换
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

		//继续判断
		if((m_cbPlayStatus[wNextPlayer]==TRUE)&&(m_cbShowHand[wNextPlayer]==FALSE))break;
	}

	//判断等注
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//判断等注
			if ((m_cbPlayStatus[i]==TRUE)&&(m_lTableScore[i]<m_lBalanceScore)&&(m_cbShowHand[i]==FALSE)) 
				break;
		}

		//当轮结束
		if (i==m_wPlayerCount) bFinishTurn=true;
	}

	//判断梭哈
	WORD wPlayCount = 0,wShowCount = 0;
	for (BYTE i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=TRUE)continue;

		if(m_cbShowHand[i]==TRUE)wShowCount++;
		wPlayCount++;
	}
	//只剩一玩家没梭(因为其下注大于所有玩家)或者全梭
	if(!bFinishTurn && wPlayCount<=wShowCount+1 && m_lTableScore[wNextPlayer]>=m_lBalanceScore) bFinishTurn=true;

	//不下判断
	if(bFinishTurn && wShowCount==0)
	{
		//让牌人数
		WORD wNoAddCount=0;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayStatus[i]==TRUE && m_lTableScore[i]==0)wNoAddCount++;
		}

		//让牌结束
		if (wPlayCount==wNoAddCount)
		{
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_PASS;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//放弃玩家
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lLoseScore[i]<0 || m_cbPlayStatus[i]==TRUE)GameEnd.lGameScore[i]=1;
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	//继续加注
	if (!bFinishTurn)
	{
		//当前用户
		m_wCurrentUser=wNextPlayer;

		//小大下注
		m_lTurnMinScore = min(m_lBalanceScore-m_lTableScore[m_wCurrentUser],m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser]);
		m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

		//发送信息
		SendUserAddInfo(wChairID,lScore,m_wCurrentUser,m_lTurnMinScore,m_lTurnMaxScore);

		return true;
	}

	//清理数据
	m_wOperaCount=0;
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//平衡次数
	m_cbBalanceCount++;

	//判断开牌
	bool bOpenStatus = (m_cbBalanceCount!=3)?false:true;

	//发送信息
	if(bOpenStatus)SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);

	//继续发牌
	if(!bOpenStatus)
	{
		//梭哈判断
		BYTE cbSendCount=0;
		if(wPlayCount>0 && wShowCount>=wPlayCount-1)
		{
			//设置变量
			bOpenStatus = true;
			cbSendCount = MAX_COUNT;
		}
		else cbSendCount = m_cbBalanceCount+2;

		//当前用户
		if(!bOpenStatus)
		{
			//牌面大小
			WORD wHeadMaxUser=INVALID_CHAIR;
			BYTE cbSingleCard[m_wPlayerCount];
			ZeroMemory(cbSingleCard,sizeof(cbSingleCard));
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				WORD wCurUser = (i+m_wBankerUser)%GAME_PLAYER;
				if(m_cbPlayStatus[wCurUser]!=FALSE)
				{
					if(wHeadMaxUser==INVALID_CHAIR)wHeadMaxUser=wCurUser;
					cbSingleCard[wCurUser]=m_GameLogic.GetSpeCardValue(&m_cbHandCardData[wCurUser][cbSendCount-1],1);
				}
			}

			//比较大小
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				WORD wCurUser = (i+m_wBankerUser)%GAME_PLAYER;
				if(m_cbPlayStatus[wCurUser]==FALSE || wCurUser==wHeadMaxUser || m_cbShowHand[wCurUser]==TRUE)continue;
				if(cbSingleCard[wCurUser]>cbSingleCard[wHeadMaxUser])wHeadMaxUser=wCurUser;
			}

			//当前用户
			m_wCurrentUser = wHeadMaxUser;

			//小大下注
			m_lTurnMinScore = 0L;
			m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

			//发送信息
			SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,m_lTurnMinScore,m_lTurnMaxScore);
		}
		//梭哈结束
		else SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);

		//发送信息
		SendSendCardInfo(cbSendCount,((!bOpenStatus)?m_wCurrentUser:INVALID_CHAIR));
	}

	//用户开牌
	if(bOpenStatus)
	{	
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_OPEN_CARD);

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);
	}

	return true;
}

//开牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE cbCardData[])
{
	//状态效验
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_OPEN_CARD);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_OPEN_CARD) return false;

	//开牌数据
	if(cbCardData!=NULL)
	{
		//效验数据
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,cbCardData,sizeof(cbTempData));
		bool bRemove = m_GameLogic.RemoveCard(m_cbHandCardData[wChairID],MAX_COUNT,cbTempData,MAX_COUNT);
		ASSERT(bRemove);
		if(!bRemove)return false;

		//头尾大小
		BYTE cbSpeCount = 2;
		BYTE cbValue=m_GameLogic.CompareCard(cbCardData,&cbCardData[cbSpeCount],cbSpeCount);
		ASSERT(cbValue!=TRUE);
		if(cbValue==TRUE)return false;

		//特殊处理
		m_GameLogic.ChangeSpeCard(cbCardData,MAX_COUNT);//!

		//用户扑克
		CopyMemory(m_cbHandCardData[wChairID],cbCardData,sizeof(BYTE)*MAX_COUNT);
		m_cbOpenFinish[wChairID] = TRUE;

		//构造变量
		CMD_S_Open_Card OpenCard;
		ZeroMemory(&OpenCard,sizeof(OpenCard));
		OpenCard.wChairID=wChairID;

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
			m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	
	}

	//开牌人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbOpenFinish[i]==TRUE && m_cbPlayStatus[i]==TRUE)bUserCount++;
		else if(m_cbPlayStatus[i]==FALSE)bUserCount++;
	}

	//结束游戏
	if(bUserCount == m_wPlayerCount)
	{
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
