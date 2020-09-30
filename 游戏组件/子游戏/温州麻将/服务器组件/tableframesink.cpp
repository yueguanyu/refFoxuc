#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_lSiceCount=0;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_cbListenStatus,sizeof(m_cbListenStatus));
	m_cbQuanFeng = 0;
	m_cbQuanCounter = 1;
	m_cbLianZhuangCount = 0;
	m_wReplaceUser = INVALID_CHAIR;
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_bTianDiHu,sizeof(m_bTianDiHu) );
	ZeroMemory( m_bTuoPai,sizeof(m_bTuoPai) );
	ZeroMemory( m_bMaiDi, sizeof(m_bMaiDi) );
	ZeroMemory( m_bHaveMaiDi, sizeof(m_bHaveMaiDi) );

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	m_cbFengCardCount=0;
	ZeroMemory(m_cbFengCardData,sizeof(m_cbFengCardData));
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//财神信息
	m_bMagicIndex = MAX_INDEX;
	m_wMagicPos = INVALID_CHAIR;
	m_cbHeapMagic = 0;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	m_cbEndLeftCount = 0;

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind) );

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

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
	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_lSiceCount=0;
	ZeroMemory( m_cbCardIndex,sizeof(m_cbCardIndex) );
	ZeroMemory( m_bTrustee,sizeof(m_bTrustee) );
	ZeroMemory( m_cbListenStatus,sizeof(m_cbListenStatus) );
	m_wReplaceUser = INVALID_CHAIR;
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_bTianDiHu,sizeof(m_bTianDiHu) );
	ZeroMemory( m_bTuoPai,sizeof(m_bTuoPai) );
	ZeroMemory( m_bMaiDi, sizeof(m_bMaiDi) );
	ZeroMemory( m_bHaveMaiDi, sizeof(m_bHaveMaiDi) );

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	m_cbFengCardCount=0;
	ZeroMemory(m_cbFengCardData,sizeof(m_cbFengCardData));
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//财神信息
	m_bMagicIndex = MAX_INDEX;
	m_wMagicPos = INVALID_CHAIR;
	m_cbHeapMagic = 0;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	m_cbEndLeftCount = 0;

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_CHIP);

	//设置庄家
	if( m_wBankerUser == INVALID_CHAIR )
		m_lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));
	else
		m_lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),0);

	if( m_wBankerUser == INVALID_CHAIR )
	{
		WORD wSice = (WORD)(m_lSiceCount>>16);
		m_wBankerUser = (HIBYTE(wSice)+LOBYTE(wSice))%GAME_PLAYER;
	}

	//构造数据
	CMD_S_Chip  GameChip = {};
	GameChip.wBankerUser = m_wBankerUser;		
	GameChip.cbLianZhuangCount = m_cbLianZhuangCount;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHIP,&GameChip,sizeof(GameChip));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHIP,&GameChip,sizeof(GameChip));


	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//结束信息
			WORD wWinner = INVALID_CHAIR;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.dwChiHuKind[i]=m_dwChiHuKind[i];
				if( m_dwChiHuKind[i] == WIK_CHI_HU )
					wWinner = i;
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//如果不是流局
			if( wWinner != INVALID_CHAIR )
			{
				//权位过滤
				FiltrateRight( wWinner,m_ChiHuRight[wWinner] );
			}

			//统计积分
			if (m_wProvideUser!=INVALID_CHAIR)
			{
				//结束信息
				GameEnd.wProvideUser=m_wProvideUser;
				GameEnd.cbProvideCard=m_cbProvideCard;

				m_ChiHuRight[wWinner].GetRightData( GameEnd.dwChiHuRight,MAX_RIGHT_COUNT );

				GameEnd.cbFanCount = CalScore( wWinner, m_wBankerUser, GameEnd.lGameScore );
			}
			else
			{
				//流局结束
				GameEnd.wProvideUser=m_wProvideUser;
				GameEnd.cbProvideCard=m_cbProvideCard;
			}

			//统计积分
			LONGLONG lGameTax[GAME_PLAYER];
			ZeroMemory(lGameTax,sizeof(lGameTax));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//累加积分
				GameEnd.lGameScore[i]*=m_pGameServiceOption->lCellScore;
				//设置积分
				if( GAME_GENRE_SCORE != m_pGameServiceOption->wServerType )
				{
					if (GameEnd.lGameScore[i]>0L)
					{
						lGameTax[i] = GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000L;
						GameEnd.lGameScore[i] -= lGameTax[i];
					}
				}
			}
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				BYTE ScoreKind;
				if( GameEnd.lGameScore[i] > 0L ) ScoreKind = SCORE_TYPE_WIN;
				else if( GameEnd.lGameScore[i] < 0L ) ScoreKind = SCORE_TYPE_LOSE;
				else ScoreKind = SCORE_TYPE_DRAW;
				ScoreInfoArray[i].lScore   = GameEnd.lGameScore[i];
				ScoreInfoArray[i].lRevenue = lGameTax[i];
				ScoreInfoArray[i].cbType   = ScoreKind;

				//发送信息
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}


			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//设置变量
			if (wWinner != m_wBankerUser)
			{
				m_wBankerUser = (m_wBankerUser+GAME_PLAYER-1)%GAME_PLAYER;
				m_cbLianZhuangCount = 0;
				if( ++m_cbQuanCounter == 5 )
				{
					m_cbQuanCounter = 1;
					if( ++m_cbQuanFeng == 4 )
					{
						m_cbQuanFeng = 0;
					}
				}
			}
			else
			{
				m_cbLianZhuangCount++;
				if (m_cbLianZhuangCount >= 4)
				{
					m_cbLianZhuangCount = 0;
					m_wBankerUser = INVALID_CHAIR;
				}
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.wProvideUser=INVALID_CHAIR;

			//拷贝扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.wProvideUser=INVALID_CHAIR;

			//拷贝扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//积分
			GameEnd.lGameScore[wChairID] = -88L*m_pGameServiceOption->lCellScore;

			//通知消息
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetNickName());
			m_pITableFrame->SendGameMessage(szMessage,SMT_CHAT);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//写入积分
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo.cbType=SCORE_TYPE_FLEE;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

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
	case GAME_SCENE_FREE:	//空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_CHIP:	//买底状态
		{
			//变量定义
			CMD_S_StatusChip StatusChip;
			memset(&StatusChip,0,sizeof(StatusChip));

			//构造数据
			StatusChip.wBankerUser=m_wBankerUser;
			StatusChip.lCellScore=m_pGameServiceOption->lCellScore;
			StatusChip.cbLianZhuangCount=m_cbLianZhuangCount;
			CopyMemory(StatusChip.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			CopyMemory(StatusChip.bMaiDi,m_bMaiDi,sizeof(m_bMaiDi));
			CopyMemory(StatusChip.bHaveMaiDi,m_bHaveMaiDi,sizeof(m_bHaveMaiDi));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusChip,sizeof(StatusChip));
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			StatusPlay.wReplaceUser = m_wReplaceUser;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			StatusPlay.cbQuanFeng = m_cbQuanFeng;
			StatusPlay.bTuoPai = (wChairID!=m_wCurrentUser?false:m_bTuoPai[m_wCurrentUser]);
			CopyMemory(StatusPlay.cbFengCardData,m_cbFengCardData,sizeof(m_cbFengCardData));
			StatusPlay.cbFengCardCount=m_cbFengCardCount;

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChairID]==false)?m_cbUserAction[wChairID]:WIK_NULL;

			//听牌状态
			CopyMemory( StatusPlay.cbHearStatus,m_cbListenStatus,sizeof(m_cbListenStatus) );

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//堆立信息
			StatusPlay.wHeapHead = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

			//扑克数据
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID],StatusPlay.cbCardData);
			if( m_wReplaceUser == wChairID ) StatusPlay.cbSendCardData = m_cbSendCardData;
			else StatusPlay.cbSendCardData=(m_wProvideUser==wChairID)?m_cbProvideCard:0x00;

			//财神变量
			StatusPlay.bMagicIndex = m_bMagicIndex;
			StatusPlay.wMagicPos = m_wMagicPos;
			StatusPlay.cbHeapMagic = m_cbHeapMagic;

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
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pData;
			return OnUserOperateCard(pUserData->wChairID,pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_REPLACE_CARD:	//用户补牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ReplaceCard));
			if (wDataSize!=sizeof(CMD_C_ReplaceCard)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//变量定义
			CMD_C_ReplaceCard * pReplaceCard=(CMD_C_ReplaceCard *)pData;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserReplaceCard(pIServerUserItem->GetChairID(),pReplaceCard->cbCardData);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			m_bTrustee[pUserData->wChairID]=pTrustee->bTrustee;
			CMD_S_Trustee Trustee;
			Trustee.bTrustee=pTrustee->bTrustee;
			Trustee.wChairID = pUserData->wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

			return true;
		}
	case SUB_C_LISTEN:
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_Listen));
			if (wDataSize!=sizeof(CMD_C_Listen)) return false;
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			CMD_C_Listen *pListen = (CMD_C_Listen *)pData;
			return OnUserListenCard(pUserData->wChairID,pListen->cbListen);
		}
	case SUB_C_CHIP:
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_Chip));
			if (wDataSize!=sizeof(CMD_C_Chip)) return false;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//场景校验
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_CHIP) return true;

			CMD_C_Chip *pChip = (CMD_C_Chip *)pData;
			return OnUserChip(pUserData->wChairID,pChip->bMaiDi);
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
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	
	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser==false)
	{
		m_wBankerUser = INVALID_CHAIR;

		m_cbQuanCounter = 1;
		m_cbLianZhuangCount = 0;
		if(m_bTrustee[pIServerUserItem->GetChairID()])
		{
			m_bTrustee[pIServerUserItem->GetChairID()]=false;
			CMD_S_Trustee Trustee;
			Trustee.bTrustee=false;
			Trustee.wChairID = pIServerUserItem->GetChairID();
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		}
	}
	return true;
}

//用户听牌
bool CTableFrameSink::OnUserListenCard(WORD wChairID,BYTE cbListen)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

	//效验参数
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbListenStatus[wChairID]==0));
	if ((wChairID!=m_wCurrentUser)||(m_cbListenStatus[wChairID]!=0)) return false;

	//设置变量
	m_cbListenStatus[wChairID] = cbListen;
	m_bEnjoinChiPeng[wChairID] = true;

	//构造数据
	CMD_S_Listen ListenCard;
	ListenCard.wChairId=wChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LISTEN,&ListenCard,sizeof(ListenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LISTEN,&ListenCard,sizeof(ListenCard));

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return false;

	//脱牌判断
	//if (m_bTuoPai[wChairID] && m_GameLogic.IsTuoPai(cbCardData, &m_WeaveItemArray[wChairID][m_cbWeaveItemCount[wChairID]-1]))
	//	return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_bSendStatus=true;
	m_bGangStatus = false;
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;
	m_bTianDiHu[wChairID] = true;
	ZeroMemory(m_bTuoPai, sizeof(m_bTuoPai));

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//记录风牌
	if((m_cbOutCardData&MASK_COLOR)==0x30)
	{
		if(m_cbFengCardData[m_cbOutCardData-0x31]==0)
			m_cbFengCardCount++;
		m_cbFengCardData[m_cbOutCardData-0x31]=m_cbOutCardData;
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//用户切换
	m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//派发扑克
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3])
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY)
		return true;

	//效验用户
	ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//变量设置
	if (cbOperateCode != WIK_CHI_HU)
		m_bTianDiHu[wChairID] = true;

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		ASSERT(m_bResponse[wChairID]==false);
		ASSERT( m_cbUserAction[wChairID] != WIK_NULL );
		ASSERT((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)!=0));

		//效验状态
		if (m_bResponse[wChairID]==true) return true;
		if( m_cbUserAction[wChairID] == WIK_NULL ) return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0)) return true;

		//变量定义
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		if(cbOperateCard[0]==0)
			m_cbOperateCard[wChairID][0] = m_cbProvideCard;
		else CopyMemory( m_cbOperateCard[wChairID],cbOperateCard,sizeof(m_cbOperateCard[wChairID]) );

		//执行判断
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取动作
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			//发送扑克
			DispatchCardData(m_wResumeUser);

			return true;
		}

		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser][0];

		//出牌变量
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//胡牌操作
		if (cbTargetAction==WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard=cbTargetCard;

			for (WORD i=(m_wProvideUser+m_wPlayerCount-1)%GAME_PLAYER;i!=m_wProvideUser;i = (i+m_wPlayerCount-1)%GAME_PLAYER)
			{
				//过虑判断
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//胡牌判断
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
				m_dwChiHuKind[i] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);

				//插入扑克
				if (m_dwChiHuKind[i]!=WIK_NULL) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					wTargetUser = i;
//					break;
				}
			}

			//结束游戏
			ASSERT(m_dwChiHuKind[wTargetUser]!=WIK_NULL);
			OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

			return true;
		}

		//组合扑克
		ASSERT(m_cbWeaveItemCount[wTargetUser]<MAX_WEAVE);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
		m_WeaveItemArray[wTargetUser][wIndex].cbCardData[0] = cbTargetCard;
		if( cbTargetAction&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
		{
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = m_cbOperateCard[wTargetUser][1];
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = m_cbOperateCard[wTargetUser][2];
		}
		else
		{
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = cbTargetCard;
			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = cbTargetCard;
			if( cbTargetAction & WIK_GANG )
				m_WeaveItemArray[wTargetUser][wIndex].cbCardData[3] = cbTargetCard;
		}

		//删除扑克
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//上牌操作
			{
				//删除扑克
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_RIGHT:		//上牌操作
			{
				//删除扑克
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_CENTER:	//上牌操作
			{
				//删除扑克
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],&m_cbOperateCard[wTargetUser][1],2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		case WIK_GANG:		//杠牌操作
			{
				//删除扑克,被动动作只存在放杠
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) )
				{
					ASSERT( FALSE );
					return false;
				}

				break;
			}
		default:
			ASSERT( FALSE );
			return false;
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		ZeroMemory( &OperateResult,sizeof(OperateResult) );
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
		OperateResult.cbOperateCard[0] = cbTargetCard;
		OperateResult.bTuoPai = m_bTuoPai[wTargetUser];
		if( cbTargetAction&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
			CopyMemory( &OperateResult.cbOperateCard[1],&m_cbOperateCard[wTargetUser][1],2*sizeof(BYTE) );
		else if( cbTargetAction&WIK_PENG )
		{
			OperateResult.cbOperateCard[1] = cbTargetCard;
			OperateResult.cbOperateCard[2] = cbTargetCard;
		}

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//设置用户
		m_wCurrentUser=wTargetUser;

		//杠牌处理
		if (cbTargetAction==WIK_GANG)
		{
			m_bGangStatus = true;
			DispatchCardData(wTargetUser,true);
		}

		return true;
	}

	//主动动作
	if (m_wCurrentUser==wChairID)
	{
		//效验操作
		ASSERT((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)!=0));
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return false;

		//扑克效验
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||(m_GameLogic.IsValidCard(cbOperateCard[0])==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)&&(m_GameLogic.IsValidCard(cbOperateCard[0])==false)) 
			return false;

		//设置变量
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;

		//执行动作
		switch (cbOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				//变量定义
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard[0]);

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//寻找组合
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard[0])&&(cbWeaveKind==WIK_PENG))
						{
							bPublic=true;
							cbWeaveIndex=i;
							break;
						}
					}

					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//组合扑克
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[3]=cbOperateCard[0];
				}
				else
				{
					//扑克效验
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
						return false;

					//设置变量
					bPublic=false;
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
					for( BYTE j = 0; j < 4; j++ ) m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[j] = cbOperateCard[0];
				}

				//删除扑克
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				m_bGangStatus = true;

				//构造结果
				CMD_S_OperateResult OperateResult;
				ZeroMemory( &OperateResult,sizeof(OperateResult) );
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard[0]=cbOperateCard[0];

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				//效验动作
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard[0],EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID,true);
				}

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//吃牌权位
				if (m_cbOutCardCount==0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				if( !m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1) )
				{
					ASSERT( FALSE );
					return false;
				}
				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbProvideCard,
					m_ChiHuRight[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;

				//结束游戏
				OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

				return true;
			}
		}

		return true;
	}

	return false;
}

//用户补牌
bool CTableFrameSink::OnUserReplaceCard(WORD wChairID, BYTE cbCardData)
{
	//错误断言
	ASSERT(wChairID==m_wReplaceUser);
	ASSERT(m_GameLogic.IsHuaCard(cbCardData)==true);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wReplaceUser) return false;
	if (m_GameLogic.IsHuaCard(cbCardData)==false) return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false)  return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//状态变量
	m_bSendStatus=true;
	m_cbSendCardData=0x00;

	//丢弃扑克
	//m_cbDiscardCount[wChairID]++;
	//m_cbDiscardCard[wChairID][m_cbDiscardCount[wChairID]-1]=cbCardData;

	//构造数据
	CMD_S_ReplaceCard ReplaceCard;
	ReplaceCard.wReplaceUser=wChairID;
	ReplaceCard.cbReplaceCard=cbCardData;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_REPLACE_CARD,&ReplaceCard,sizeof(ReplaceCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_REPLACE_CARD,&ReplaceCard,sizeof(ReplaceCard));

	//派发扑克
	DispatchCardData(wChairID, true);

	return true;
}

//用户买底
bool CTableFrameSink::OnUserChip(WORD wChairID, BYTE cbMaidi)
{
	//检验数据
	ASSERT(!m_bHaveMaiDi[wChairID]);
	if (m_bHaveMaiDi[wChairID])	return true;

	//设置变量
	m_bMaiDi[wChairID] = cbMaidi;
	m_bHaveMaiDi[wChairID] = true;

	//发送消息
	CMD_S_Chip_Result chipResult = {};
	chipResult.bTotal = false;
	chipResult.wMaiDiUser = wChairID;
	chipResult.bMaidi[wChairID] = cbMaidi;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResult,sizeof(chipResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResult,sizeof(chipResult));

	if (IsAllState(m_bHaveMaiDi))
	{
		//发送消息
		CMD_S_Chip_Result chipResultTotal = {};
		chipResultTotal.bTotal = true;
		memcpy(chipResultTotal.bMaidi, m_bMaiDi, sizeof(m_bMaiDi));

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResultTotal,sizeof(chipResultTotal));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHIP_RESULT,&chipResultTotal,sizeof(chipResultTotal));

		//计算顶底
		if (m_bMaiDi[m_wBankerUser] != 0)
		{
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (i != m_wBankerUser && m_bMaiDi[i] != 0)	m_bMaiDi[i] += m_bMaiDi[m_wBankerUser];
			}
		}

		//设置状态
		m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

		m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
		m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

		//test JJWillDo 注释掉
		/*BYTE byTest[] = {
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						
		0x01,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						
		0x01,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,

		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x34,0x12,	

		0x11,0x11,0x21,0x11,0x15,0x15,0x15,0x15,
		0x06,0x06,0x06,0x32,0x19,0x19,0x27,0x37,

		0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,
		0x22,0x22,0x22,0x37,0x19,0x19,0x27,0x01,

		0x21,0x24,0x25,0x26,0x24,0x24,0x15,0x15,
		0x15,0x22,0x22,0x22,0x15,0x16,0x16,0x37,

		0x02,0x02,0x02,0x24,0x25,0x26,0x28,0x29,
		0x13,0x13,0x14,0x16,0x17,0x18,0x37,0x34
		};
		CopyMemory( m_cbRepertoryCard,byTest,sizeof(byTest) );*/
		//m_wBankerUser = 1;
		//end test

		//分发扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}

		//发送扑克
		m_cbSendCardCount++;
		m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
		m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_cbProvideCard=m_cbSendCardData;
		m_wProvideUser=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//堆立信息
		WORD wSice = WORD(m_lSiceCount&0xffff);
		BYTE cbSiceTakeCount= HIBYTE(wSice)+LOBYTE(wSice);
		WORD wTakeChairID=(m_wBankerUser+cbSiceTakeCount-1)%GAME_PLAYER;
		while( cbSiceTakeCount*2 > HEAP_FULL_COUNT )
		{
			wTakeChairID = (wTakeChairID+1)%GAME_PLAYER;
			cbSiceTakeCount -= HEAP_FULL_COUNT/2;
		}
		m_wHeapTail = wTakeChairID;
		BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//计算数目
			BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceTakeCount)*2:0);
			BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

			//提取扑克
			cbTakeCount-=cbRemoveCount;
			m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

			//完成判断
			if (cbTakeCount==0)
			{
				m_wHeapHead=wTakeChairID;
				break;
			}
			//切换索引
			wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
		}

		//设置财神
		WORD wMagicSice = wSice;
		BYTE cbMagicSiceCount = min(HIBYTE(wMagicSice),LOBYTE(wMagicSice));
		BYTE bMagicData =  m_cbRepertoryCard[cbMagicSiceCount*2-1];
		//test JJWillDo 注释下面
		//bMagicData = 0x27;
		//test
		m_bMagicIndex = m_GameLogic.SwitchToCardIndex(bMagicData);
		m_GameLogic.SetMagicIndex(m_bMagicIndex);

		if (cbMagicSiceCount > cbSiceTakeCount)
		{
			m_wMagicPos = (m_wHeapTail+3) % GAME_PLAYER;
			m_cbHeapMagic = (cbMagicSiceCount-1-cbSiceTakeCount) * 2;		
		}
		else
		{
			m_wMagicPos = m_wHeapTail;
			m_cbHeapMagic = m_cbHeapCardInfo[m_wHeapTail][1] + cbMagicSiceCount*2 - 2;
		}

//		MyDebug(_T("Start:财神  庄家 %d 值 %d 方位 %d 位置 %d 点数 [%d %d] 尾部信息 %d %d"), m_wBankerUser, m_bMagicIndex, 
//			m_wMagicPos, m_cbHeapMagic, HIBYTE(wMagicSice), LOBYTE(wMagicSice), m_wHeapTail, m_cbHeapCardInfo[m_wHeapTail][1]);

		//动作分析
		bool bAroseAction=false;

		//庄家判断
		if ( m_wReplaceUser==INVALID_CHAIR )
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wBankerUser],NULL,0,GangCardResult);

			//胡牌判断
			CChiHuRight chr;
			m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
			m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wBankerUser],NULL,0,m_cbSendCardData,chr);
			m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
		}

		//构造数据
		CMD_S_GameStart GameStart;
		GameStart.lSiceCount=m_lSiceCount;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.wReplaceUser = m_wReplaceUser;
		GameStart.wCurrentUser= m_wReplaceUser==INVALID_CHAIR?m_wCurrentUser:INVALID_CHAIR;
		GameStart.cbQuanFeng = m_cbQuanFeng;
		GameStart.wHeapHead = m_wHeapHead;
		GameStart.wHeapTail = m_wHeapTail;
		GameStart.bMagicIndex = m_bMagicIndex;
		GameStart.wMagicPos = m_wMagicPos;
		GameStart.cbHeapMagic = m_cbHeapMagic;
		GameStart.cbLianZhuangCount = m_cbLianZhuangCount;
		CopyMemory(GameStart.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//设置变量
			GameStart.cbUserAction=m_cbUserAction[i];
			m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
	}

	return true;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bTail)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;
	ASSERT( m_bSendStatus );
	if( m_bSendStatus == false ) return false;

	//丢弃扑克
	if ((m_wReplaceUser==INVALID_CHAIR)&&(m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	//荒庄结束
	if (m_cbLeftCardCount==m_cbEndLeftCount)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}

	//设置变量
	if( m_wReplaceUser == INVALID_CHAIR )
	{
		m_cbOutCardData=0;
		m_wCurrentUser=wCurrentUser;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//发送扑克
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];

	//财神位置
	BYTE bMagicCount = min(HIBYTE(WORD(m_lSiceCount&0xffff)),LOBYTE(WORD(m_lSiceCount&0xffff)));
	if (m_cbLeftCardCount == bMagicCount*2-1)
	{
		m_cbSendCardCount++;
		m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
	}

	//加牌
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	m_wReplaceUser=INVALID_CHAIR;

	if( m_wCurrentUser == wCurrentUser )
	{
		//设置变量
		m_wProvideUser=wCurrentUser;
		m_cbProvideCard=m_cbSendCardData;
	}

	if( m_wReplaceUser == INVALID_CHAIR )
	{
		if( !m_bEnjoinChiHu[m_wCurrentUser] )
		{
			//胡牌判断
			CChiHuRight chr;
			m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]--;
			BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wCurrentUser],m_WeaveItemArray[m_wCurrentUser],
				m_cbWeaveItemCount[m_wCurrentUser],m_cbProvideCard,chr);
			m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

			//检查4台
			if (cbAction&WIK_CHI_HU)
			{
				FiltrateRight(m_wCurrentUser, chr);
				BYTE bFanshu = m_GameLogic.GetChiHuActionRank( chr, true );
			}

			m_cbUserAction[m_wCurrentUser] |= cbAction;
		}

		//杠牌判断
		if ((m_bEnjoinChiPeng[m_wCurrentUser]==false)&&(m_cbLeftCardCount>m_cbEndLeftCount))
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
				m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);
		}
	}

	//堆立信息
	ASSERT( m_wHeapHead != INVALID_CHAIR && m_wHeapTail != INVALID_CHAIR );
	if( !bTail )
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHead][0]+m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapHead=(m_wHeapHead+1)%CountArray(m_cbHeapCardInfo);
		m_cbHeapCardInfo[m_wHeapHead][0]++;
	}
	else
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapTail=(m_wHeapTail+3)%CountArray(m_cbHeapCardInfo);
		m_cbHeapCardInfo[m_wHeapTail][1]++;
	}

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wSendCardUser = wCurrentUser;
	SendCard.wReplaceUser = m_wReplaceUser;
	SendCard.wCurrentUser= m_wReplaceUser==INVALID_CHAIR?m_wCurrentUser:INVALID_CHAIR;
	SendCard.bTail = bTail;
	SendCard.cbActionMask=m_wReplaceUser==INVALID_CHAIR?m_cbUserAction[m_wCurrentUser]:WIK_NULL;
	SendCard.cbCardData=(m_wCurrentUser==wCurrentUser)?m_cbProvideCard:m_cbSendCardData;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (wCenterUser==i) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			//吃碰判断
			if (m_bEnjoinChiPeng[i]==false)
			{
				//碰牌判断
				m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

				//吃牌判断
				WORD wEatUser=(wCenterUser+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser==i)
					m_cbUserAction[i]|=m_GameLogic.EstimateEatCard(m_cbCardIndex[i],cbCenterCard);

				//杠牌判断
				if (m_cbLeftCardCount>m_cbEndLeftCount) 
				{
					m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
				}
			}
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false )
		{
			//吃胡判断
			CChiHuRight chr;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
			
			//检查4台
			if (cbAction&WIK_CHI_HU)
			{
				FiltrateRight(i, chr);
				BYTE bFanshu = m_GameLogic.GetChiHuActionRank( chr, true );
			}

			m_cbUserAction[i] |= cbAction;	
		}

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		return true;
	}

	return false;
}

//算分
BYTE CTableFrameSink::CalScore( WORD wWinner, WORD wBanker, LONGLONG lScore[GAME_PLAYER] )
{
	//初始化
	ZeroMemory( lScore,sizeof(lScore[0])*GAME_PLAYER );

	//变量定义
	WORD wFanShu = 0;
	LONGLONG lMagicScore[GAME_PLAYER] = {};
	LONGLONG lLianZhuangScore = 2 * (m_cbLianZhuangCount + 1);
	LONGLONG lMaidiMul[GAME_PLAYER] = {0};

	//胡牌番数
	wFanShu = m_GameLogic.GetChiHuActionRank( m_ChiHuRight[wWinner] );
	ASSERT(wFanShu!=0);
	if (wFanShu == 0)	wFanShu = 1;

	//财神分数
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		LONGLONG lMagicCount = m_cbCardIndex[i][m_bMagicIndex];
		if (i == wWinner && !(m_ChiHuRight[wWinner]&CHR_THREE_KING).IsEmpty())
			lMagicCount *= 2;
		for (int j = 0; j < GAME_PLAYER; j++)
		{
			if (j == i)	lMagicScore[j] += lMagicCount * (GAME_PLAYER-1);
			else		lMagicScore[j] -= lMagicCount;
		}
	}

	//买底倍数
	if (wWinner == wBanker)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i == wWinner)	continue;
			lMaidiMul[i] = (m_bMaiDi[i]==0?m_bMaiDi[wBanker]:m_bMaiDi[i]);
		}
	}
	else
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i != wBanker)	continue;
			lMaidiMul[i] = m_bMaiDi[wWinner]; 
		}
	}

	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( i != wWinner )
		{	
			BYTE bZhuang = ((wWinner==wBanker||i==wBanker)?2:1);
			LONGLONG lGameScore = wFanShu * (lLianZhuangScore/2*lMaidiMul[i] + lLianZhuangScore*bZhuang) - lMagicScore[i];
			lScore[i] = -lGameScore;
			lScore[wWinner] += lGameScore;
		}
	}

//	MyDebug(_T("算分 赢家 %d 庄家 %d 番数 %d 连庄 %d"), wWinner, wBanker, wFanShu, lLianZhuangScore);
//	MyDebug(_T("算分 买底倍数 [%I64d %I64d %I64d %I64d] * %I64d 财神分数 [%I64d %I64d %I64d %I64d]"),
//		lMaidiMul[0], lMaidiMul[1], lMaidiMul[2], lMaidiMul[3],  lLianZhuangScore/2, lMagicScore[0], lMagicScore[1], lMagicScore[2], lMagicScore[3]);
//	MyDebug(_T("算分 合计 [%I64d %I64d %I64d %I64d]"), lScore[0], lScore[1], lScore[2], lScore[3]);

	return BYTE(wFanShu);
}

//从系统翻牌获取钻牌索引
BYTE CTableFrameSink::GetMagicIndex( BYTE cbFanCard )
{
	BYTE cbZPaiIndex = m_GameLogic.SwitchToCardIndex(cbFanCard);
	if( cbZPaiIndex < 27 )
	{
		if( (cbZPaiIndex+1)%9 == 0 )
			cbZPaiIndex -= 8;
		else
			cbZPaiIndex++;
	}
	else
	{
		if( cbZPaiIndex + 1 == MAX_INDEX )
			cbZPaiIndex = 27;
		else
			cbZPaiIndex++;
	}
	return cbZPaiIndex;
}

//获取桌面牌数
BYTE CTableFrameSink::GetDiscardCount( BYTE cbCardData )
{
	BYTE cbCount = 0;
	//遍历丢弃牌
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		for( BYTE j = 0; j < m_cbDiscardCount[i]; j++ )
		{
			if( m_cbDiscardCard[i][j] == cbCardData )
				if( ++cbCount == 4 ) break;
		}
	}
	if( cbCount < 4 )
	{
		//检查杠牌
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			for( BYTE j = 0; j < m_cbWeaveItemCount[i]; j++ )
			{
				if( !m_WeaveItemArray[i][j].cbPublicCard ) continue;
				BYTE cbCardBuffer[4];
				BYTE cbCardCount = m_GameLogic.GetWeaveCard( m_WeaveItemArray[i][j].cbWeaveKind,
					m_WeaveItemArray[i][j].cbCenterCard,cbCardBuffer );
				for( BYTE k = 0; k < cbCardCount; k++ )
					if( cbCardBuffer[k] == cbCardData )
						if( ++cbCount == 4 ) break;
			}
		}
	}

	return cbCount;
}

//权位过滤
void CTableFrameSink::FiltrateRight( WORD wWinner, CChiHuRight &chr )
{
	/*	增加权位	*/

	//自摸
	if( wWinner == m_wProvideUser )
	{
		chr |= CHR_ZI_MO;
		chr |= CHR_YING_HU;
	}

	//天地胡   
	if (!m_bTianDiHu[wWinner])
	{
		if (wWinner == m_wBankerUser)
			chr |= CHR_TIAN_HU;
		else
			chr |= CHR_DI_HU;
	}

	/*	修改权位	*/


	/*	调整权位	*/

	//硬八对不计八对
	if( !(chr&CHR_BA_DUI_YING).IsEmpty() )
		chr &= ~CHR_BA_DUI;

	//双翻不计硬胡 软胡
	if( !(chr&CHR_BA_DUI_YING).IsEmpty() || !(chr&CHR_THREE_KING).IsEmpty() || !(chr&CHR_TIAN_HU).IsEmpty() ||
		!(chr&CHR_DI_HU).IsEmpty()       || !(chr&CHR_DAN_DIAO).IsEmpty() )
	{
		chr |= CHR_SHUANG_FAN;
		chr &= ~CHR_YING_HU;
		chr &= ~CHR_RUAN_HU;
	}
	else if( !(chr&CHR_YING_HU).IsEmpty() )	//硬胡不计软胡
		chr &= ~CHR_RUAN_HU;
	else
		chr |= CHR_RUAN_HU;
}

//状态检查
bool CTableFrameSink::IsAllState(bool bState[GAME_PLAYER], WORD wIgnorePos/* = INVALID_CHAIR*/, bool bCheckTrue/* = true*/)
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wIgnorePos)	continue;

		if (bState[i] != bCheckTrue)
				return false;
	}
	return true;
}

//是否双包
bool CTableFrameSink::IsBaoUser(WORD wUserFir, WORD wUserSec)
{
	if (m_cbWeaveItemCount[wUserFir] >= 3)
	{
		BYTE cbProWeave = 0;
		for (int i = 0; i < m_cbWeaveItemCount[wUserFir]; i++)
		{
			if (m_WeaveItemArray[wUserFir][i].wProvideUser == wUserSec)
				cbProWeave++;
			if (cbProWeave >= 3)	
				return true;
		}
	}

	if (m_cbWeaveItemCount[wUserSec] >= 3)
	{
		BYTE cbProWeave = 0;
		for (int i = 0; i < m_cbWeaveItemCount[wUserSec]; i++)
		{
			if (m_WeaveItemArray[wUserSec][i].wProvideUser == wUserFir)
				cbProWeave++;
			if (cbProWeave >= 3)	
				return true;
		}
	}


	return false;
}


//////////////////////////////////////////////////////////////////////////////////
