#include "StdAfx.h"
#include "TableFrameSink.h"
#include "cmath"
//////////////////////////////////////////////////////////////////////////////////
//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
//////////////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	//逻辑变量
	m_cbPackCount=2;
	m_cbMainValue=0x02;
	m_cbMainColor=COLOR_ERROR;

	//连局信息
	m_cbValueOrder[0]=2;
	m_cbValueOrder[1]=2;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wFirstUser=0;
	m_bLandScore=120;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));
	ZeroMemory(m_bUserNoMain,sizeof(m_bUserNoMain));
	m_bSendNoMain=false;
	ZeroMemory(m_lKingScore,sizeof(m_lKingScore));

	m_bFourKing=false;
	m_bThreeKing=false;
	m_bKingCall=false;


	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;
	ZeroMemory(m_wUserScore,sizeof(m_wUserScore));

	//状态变量
	m_cbScoreCardCount=0;
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));
	ZeroMemory(m_btGiveUpScore,sizeof(m_btGiveUpScore));

	//出牌变量
	m_wTurnWinner=INVALID_CHAIR;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	m_GameLogic.SetCanThrow(false);
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
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	//逻辑变量
	m_cbPackCount=2;
	m_cbMainColor=COLOR_ERROR;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	m_bLandScore=120;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));
	m_GameLogic.SetCanThrow(false);

	m_bFourKing=false;
	m_bThreeKing=false;
	m_bKingCall=false;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;
	ZeroMemory(m_wUserScore,sizeof(m_wUserScore));

	//状态变量
	m_cbScoreCardCount=0;
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));
	ZeroMemory(m_btGiveUpScore,sizeof(m_btGiveUpScore));
	ZeroMemory(m_bUserNoMain,sizeof(m_bUserNoMain));
	m_bSendNoMain=false;
	ZeroMemory(m_lKingScore,sizeof(m_lKingScore));

	//出牌变量
	m_wTurnWinner=INVALID_CHAIR;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_UG_SCORE);

	//设置变量
	m_cbMainColor=COLOR_ERROR;

	//设置属性
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetMainValue(m_cbMainValue);
	m_GameLogic.SetPackCount(m_cbPackCount);

	//随机用户
	srand((DWORD)time(NULL));
	if(m_wBankerUser==INVALID_CHAIR) m_wBankerUser=3;
	m_wCurrentUser=m_wBankerUser;

	//发送扑克
	DispatchUserCard();

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_GIVE_UP:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			GameEnd.wConcealTime=1;

			//统计得分
			LONGLONG wWholeScore=0;

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);
			wWholeScore=GameEnd.wGameScore;

			//扣底
			if (wChairID!=m_wBankerUser && m_GameLogic.GetCardLogicColor(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID])==COLOR_NT)
			{
				//底分
				LONGLONG lBackScore=0;
				lBackScore=GameEnd.wConcealScore;

				//对牌扣底
				if( m_GameLogic.GetCardType(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID]) == CT_SAME_2 ) 
				{
					lBackScore*=2;
					GameEnd.wConcealTime=2;
				}

				//拖拉机扣底
				if( m_GameLogic.GetCardType(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID]) == CT_TRACKOR_2 )  
				{
					if(m_cbOutCardCount[wChairID]==4 ) 
					{
						lBackScore*=4;
						GameEnd.wConcealTime=4;
					}
					if(m_cbOutCardCount[wChairID]==6 ) 
					{
						lBackScore*=8;
						GameEnd.wConcealTime=8;
					}
				}

				wWholeScore=GameEnd.wGameScore+lBackScore;
				GameEnd.bAddConceal=true;
			}

			//计算得分
			LONGLONG lScoreTimes=1;
			if(m_bLandScore==0) lScoreTimes=4;
			if(m_bLandScore>=5&&m_bLandScore<=40) lScoreTimes=3;
			if(m_bLandScore>=45&&m_bLandScore<=80) lScoreTimes=2;
			if(m_bLandScore>=85&&m_bLandScore<=120) lScoreTimes=1;

			//玩家得分
			//庄家输赢倍数=3*叫分倍数*（|闲家得分-叫分|/40+1）（取整）
			LONGLONG lEndScore=1;
			lScoreTimes*=-1;
			GameEnd.bEndStatus=255;//1级

			//游戏分
			GameEnd.lScore[m_wBankerUser]=3*lScoreTimes*m_pGameServiceOption->lCellScore*lEndScore;
			GameEnd.lScore[(m_wBankerUser+2)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);
			GameEnd.lScore[(m_wBankerUser+1)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);
			GameEnd.lScore[(m_wBankerUser+3)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);

			//冲关分
			if(m_bThreeKing==true && m_lKingScore[wChairID]>0)
			{
				m_lKingScore[(wChairID+1)%GAME_PLAYER]=-1*m_pGameServiceOption->lCellScore;
				m_lKingScore[(wChairID+2)%GAME_PLAYER]=-1*m_pGameServiceOption->lCellScore;
				m_lKingScore[(wChairID+3)%GAME_PLAYER]=-1*m_pGameServiceOption->lCellScore;
				m_lKingScore[wChairID]=3*m_pGameServiceOption->lCellScore;
			}
			if(m_bFourKing==true && m_lKingScore[wChairID]>0)
			{
				m_lKingScore[(wChairID+1)%GAME_PLAYER]=-3*m_pGameServiceOption->lCellScore;
				m_lKingScore[(wChairID+2)%GAME_PLAYER]=-3*m_pGameServiceOption->lCellScore;
				m_lKingScore[(wChairID+3)%GAME_PLAYER]=-3*m_pGameServiceOption->lCellScore;
				m_lKingScore[wChairID]=9*m_pGameServiceOption->lCellScore;
			}
			CopyMemory(GameEnd.lKingScore,m_lKingScore,sizeof(GameEnd.lKingScore));

			//写分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for(int i=0;i<GAME_PLAYER;i++)
			{
				LONGLONG lScore=GameEnd.lScore[i]+m_lKingScore[i];
				LONGLONG lRevenue=0;
				BYTE ScoreKind;
				if(lScore>0)
				{
					ScoreKind=SCORE_TYPE_WIN;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
						lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/1000;
					lScore-=lRevenue;
				}
				else if(lScore==0) 	ScoreKind=SCORE_TYPE_DRAW;
				else				ScoreKind=SCORE_TYPE_LOSE;
				
				ScoreInfo[i].lScore =lScore;
				ScoreInfo[i].lRevenue = lRevenue;
				ScoreInfo[i].cbType = ScoreKind;
		
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
			//结束信息
			CString szEndMessage;
			szEndMessage.Format(TEXT("本局结束，成绩统计：\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n"),
				m_pITableFrame->GetTableUserItem(0)->GetNickName(),GameEnd.lScore[0],m_pITableFrame->GetTableUserItem(1)->GetNickName(),GameEnd.lScore[1],
				m_pITableFrame->GetTableUserItem(2)->GetNickName(),GameEnd.lScore[2],m_pITableFrame->GetTableUserItem(3)->GetNickName(),GameEnd.lScore[3]);
#ifndef DEBUG
			//发送结束信息
			m_pITableFrame->SendGameMessage(szEndMessage,SMT_CHAT);
#endif
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			//庄家切换
			if (GameEnd.lScore[m_wBankerUser]>0L) m_wBankerUser=m_wBankerUser;
			else m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;

			//主牌设置
			m_cbMainValue=m_cbValueOrder[m_wBankerUser%2];

			//切换用户
			m_wFirstUser=wChairID;

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_UG_FREE);

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_UG_FREE);

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			GameEnd.wConcealTime=1;

			//统计得分
			LONGLONG wWholeScore=0;

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);
			wWholeScore=GameEnd.wGameScore;

			//扣底
			if (wChairID!=m_wBankerUser && m_GameLogic.GetCardLogicColor(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID])==COLOR_NT)
			{
				//底分
				LONGLONG lBackScore=0;
				lBackScore=GameEnd.wConcealScore;

				//对牌扣底
				if( m_GameLogic.GetCardType(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID]) == CT_SAME_2 ) 
				{
					lBackScore*=2;
					GameEnd.wConcealTime=2;
				}

				//拖拉机扣底
				if( m_GameLogic.GetCardType(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID]) == CT_TRACKOR_2 )  
				{
					if(m_cbOutCardCount[wChairID]==4 ) 
					{
						lBackScore*=4;
						GameEnd.wConcealTime=4;
					}
					if(m_cbOutCardCount[wChairID]==6 ) 
					{
						lBackScore*=8;
						GameEnd.wConcealTime=8;
					}
				}

				wWholeScore=GameEnd.wGameScore+lBackScore;
				GameEnd.bAddConceal=true;
			}

			//计算得分
			LONGLONG lScoreTimes=1;
			if(m_bLandScore==0) lScoreTimes=4;
			if(m_bLandScore>=5&&m_bLandScore<=40) lScoreTimes=3;
			if(m_bLandScore>=45&&m_bLandScore<=80) lScoreTimes=2;
			if(m_bLandScore>=85&&m_bLandScore<=120) lScoreTimes=1;

			//玩家得分
			//庄家输赢倍数=3*叫分倍数*（|闲家得分-叫分|/40+1）（取整）
			LONGLONG lEndScore=1;
			if(wWholeScore<m_bLandScore || (m_bLandScore==0&&wWholeScore==0))//庄胜
			{
				lScoreTimes=lScoreTimes;
				if(wWholeScore==0)
				{
					lEndScore=3;
					GameEnd.bEndStatus=1;//大光
				}
				else if(wWholeScore<=30)
				{	
					lEndScore=2;
					GameEnd.bEndStatus=2;//小光
				}
				else 
				{
					lEndScore=1;
					GameEnd.bEndStatus=3;//过庄
				}
			}
			else	//庄负
			{
				lScoreTimes*=-1;
				lEndScore=abs(int(m_bLandScore-wWholeScore))/40+1;

				switch(lEndScore)
				{
				case 1: GameEnd.bEndStatus=4;break;//1级
				case 2: GameEnd.bEndStatus=5;break;//2级
				case 3: GameEnd.bEndStatus=6;break;//3级
				case 4: GameEnd.bEndStatus=7;break;//4级
				case 5: GameEnd.bEndStatus=8;break;//5级
				case 6: GameEnd.bEndStatus=9;break;//6级
				}
			}	

			//游戏分
			GameEnd.lScore[m_wBankerUser]=3*lScoreTimes*m_pGameServiceOption->lCellScore*lEndScore;
			GameEnd.lScore[(m_wBankerUser+2)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);
			GameEnd.lScore[(m_wBankerUser+1)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);
			GameEnd.lScore[(m_wBankerUser+3)%m_wPlayerCount]=-(GameEnd.lScore[m_wBankerUser]/3);

			//冲关分
			CopyMemory(GameEnd.lKingScore,m_lKingScore,sizeof(GameEnd.lKingScore));

			//写分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for(int i=0;i<GAME_PLAYER;i++)
			{
				LONGLONG lScore=GameEnd.lScore[i]+m_lKingScore[i];
				LONGLONG lRevenue=0;
				BYTE	ScoreKind;
				if(lScore>0)
				{
					ScoreKind=SCORE_TYPE_WIN;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
						lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/1000;
					lScore-=lRevenue;
				}
				else if(lScore==0) 	ScoreKind=SCORE_TYPE_DRAW;
				else				ScoreKind=SCORE_TYPE_LOSE;
				
				ScoreInfo[i].lScore =lScore;
				ScoreInfo[i].lRevenue =lRevenue;
				ScoreInfo[i].cbType =ScoreKind;
				
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
			//结束信息
			CString szEndMessage;
			szEndMessage.Format(TEXT("本局结束，成绩统计：\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n %s ： %I64d 分\n"),
				m_pITableFrame->GetTableUserItem(0)->GetNickName(),GameEnd.lScore[0],m_pITableFrame->GetTableUserItem(1)->GetNickName(),GameEnd.lScore[1],
				m_pITableFrame->GetTableUserItem(2)->GetNickName(),GameEnd.lScore[2],m_pITableFrame->GetTableUserItem(3)->GetNickName(),GameEnd.lScore[3]);
#ifndef DEBUG
			//发送结束信息
			m_pITableFrame->SendGameMessage(szEndMessage,SMT_CHAT);
#endif
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			//庄家切换
			if (GameEnd.lScore[m_wBankerUser]>0L) m_wBankerUser=m_wBankerUser;
			else m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;

			//主牌设置
			m_cbMainValue=m_cbValueOrder[m_wBankerUser%2];

			//切换用户
			m_wFirstUser=wChairID;

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_UG_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);

			//玩家得分
			GameEnd.lScore[wChairID]=-12*m_pGameServiceOption->lCellScore;
			//冲关分
			CopyMemory(GameEnd.lKingScore,m_lKingScore,sizeof(GameEnd.lKingScore));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//写分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for(int i=0;i<GAME_PLAYER;i++)
			{
				LONGLONG lScore=GameEnd.lScore[i]+m_lKingScore[i];
				LONGLONG lRevenue=0;
				BYTE ScoreKind;
				if(lScore>0)
				{
					ScoreKind=SCORE_TYPE_WIN;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
						lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/1000;
					lScore-=lRevenue;
				}
				else if(lScore==0) 	ScoreKind=SCORE_TYPE_DRAW;
				else				ScoreKind=SCORE_TYPE_LOSE;
				ScoreInfo[i].lScore =lScore;
				ScoreInfo[i].lRevenue =lRevenue;
				ScoreInfo[i].cbType =ScoreKind;
			}
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
#ifndef DEBUG
			//通知消息
			CString szMessage;
			szMessage.Format(TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetNickName());
			m_pITableFrame->SendGameMessage(szMessage,SMT_CHAT);
#endif
			//结束游戏
			m_pITableFrame->ConcludeGame(GS_UG_FREE);

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
	case GS_UG_SCORE:		//叫分状态
		{
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));

			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.bLandScore=m_bLandScore;
			StatusScore.wBankerUser=m_wBankerUser;

			CopyMemory(StatusScore.cbCardData,m_cbHandCardData[wChairID],sizeof(StatusScore.cbCardData));
			CopyMemory(StatusScore.cbBackCardData,m_cbConcealCard,sizeof(StatusScore.cbBackCardData));
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_UG_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_UG_CALL:		//叫牌状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//游戏变量
			StatusCall.wBankerUser=m_wBankerUser;
			StatusCall.cbPackCount=m_cbPackCount;
			StatusCall.cbMainValue=m_cbMainValue;
			StatusCall.cbValueOrder[0]=m_cbValueOrder[0];
			StatusCall.cbValueOrder[1]=m_cbValueOrder[1];
			StatusCall.lBaseScore=m_pGameServiceOption->lCellScore;

			//发牌变量
			StatusCall.cbCardCount=m_cbHandCardCount[wChairID];
			CopyMemory(StatusCall.cbCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);

			//叫牌信息
			StatusCall.cbCallCard=m_cbCallCard;
			StatusCall.cbCallCount=m_cbCallCount;
			StatusCall.wCallCardUser=m_wCallCardUser;
			StatusCall.wLandScore=m_bLandScore;
			StatusCall.cbComplete=(m_bCallCard[wChairID]==true)?TRUE:FALSE;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_UG_BACK:		//底牌状态
		{
			//构造数据
			CMD_S_StatusBack StatusBack;
			ZeroMemory(&StatusBack,sizeof(StatusBack));

			//游戏变量
			StatusBack.cbPackCount=m_cbPackCount;
			StatusBack.cbMainValue=m_cbMainValue;
			StatusBack.cbMainColor=m_cbMainColor;
			StatusBack.cbValueOrder[0]=m_cbValueOrder[0];
			StatusBack.cbValueOrder[1]=m_cbValueOrder[1];
			StatusBack.lBaseScore=m_pGameServiceOption->lCellScore;

			//叫牌变量
			StatusBack.cbCallCard=m_cbCallCard;
			StatusBack.cbCallCount=m_cbCallCount;
			StatusBack.wCallCardUser=m_wCallCardUser;
			StatusBack.wLandScore=m_bLandScore;

			//用户变量
			StatusBack.wBankerUser=m_wBankerUser;
			StatusBack.wCurrentUser=m_wCurrentUser;

			//扑克变量
			StatusBack.cbCardCount=m_cbHandCardCount[wChairID];
			CopyMemory(StatusBack.cbCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);

			//底牌信息
			if (wChairID==m_wBankerUser)
			{
				StatusBack.cbConcealCount=m_cbConcealCount;
				CopyMemory(StatusBack.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusBack,sizeof(StatusBack));
		}
	case GS_UG_PLAY:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.cbPackCount=m_cbPackCount;
			StatusPlay.cbMainValue=m_cbMainValue;
			StatusPlay.cbMainColor=m_cbMainColor;
			StatusPlay.cbValueOrder[0]=m_cbValueOrder[0];
			StatusPlay.cbValueOrder[1]=m_cbValueOrder[1];
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;

			//叫牌变量
			StatusPlay.cbCallCard=m_cbCallCard;
			StatusPlay.cbCallCount=m_cbCallCount;
			StatusPlay.wCallCardUser=m_wCallCardUser;
			StatusPlay.wLandScore=m_bLandScore;

			//用户变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wFirstOutUser=m_wFirstOutUser;

			//扑克变量
			StatusPlay.cbCardCount=m_cbHandCardCount[wChairID];
			CopyMemory(StatusPlay.cbCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);

			//底牌信息
			if (wChairID==m_wBankerUser)
			{
				StatusPlay.cbConcealCount=m_cbConcealCount;
				CopyMemory(StatusPlay.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			}

			//出牌变量
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				StatusPlay.cbOutCardCount[i]=m_cbOutCardCount[i];
				CopyMemory(StatusPlay.cbOutCardData[i],m_cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i]);
			}

			//得分变量
			StatusPlay.cbScoreCardCount=m_cbScoreCardCount;
			CopyMemory(StatusPlay.cbScoreCardData,m_cbScoreCardData,sizeof(StatusPlay.cbScoreCardData));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GS_UG_WAIT:	//等待状态
		{
			//构造数据
			CMD_S_StatusWait StatusWait;
			ZeroMemory(&StatusWait,sizeof(StatusWait));

			//用户变量
			StatusWait.wBankerUser=m_wBankerUser;
			StatusWait.lBaseScore=m_pGameServiceOption->lCellScore;

			//游戏变量
			StatusWait.cbPackCount=m_cbPackCount;
			StatusWait.cbMainValue=m_cbMainValue;
			StatusWait.cbValueOrder[0]=m_cbValueOrder[0];
			StatusWait.cbValueOrder[1]=m_cbValueOrder[1];

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusWait,sizeof(StatusWait));
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
	case SUB_C_GIVEUP_GAME:
		{
			return OnEventGameConclude(pIServerUserItem->GetChairID(),pIServerUserItem,GER_GIVE_UP);
		}
	case SUB_C_GM_REQUEST:
		{
			DWORD dwRight=pIServerUserItem->GetMasterRight();
			if(dwRight==0) return true;

			CMD_S_GMCard GMCard;
			CopyMemory(GMCard.cbCardData,m_cbHandCardData,sizeof(GMCard.cbCardData));
			CopyMemory(GMCard.cbCardCount,m_cbHandCardCount,sizeof(GMCard.cbCardCount));

			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(),SUB_S_GM_CARD,&GMCard,sizeof(GMCard));
			return true;
		}
	case SUB_C_EJECT:
		{
			OnEventGameConclude(pIServerUserItem->GetChairID(),pIServerUserItem,GER_USER_LEAVE);
			m_pITableFrame->SendRoomMessage(pIServerUserItem,NULL,SMT_CLOSE_GAME);
			return true;
		}
	case SUB_C_LAND_SCORE:	//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_LandScore));
			if (wDataSize!=sizeof(CMD_C_LandScore)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_LandScore * pLandScore=(CMD_C_LandScore *)pData;
			return OnUserLandScore(pIServerUserItem->GetChairID(),pLandScore->bLandScore);
		}
	case SUB_C_CALL_CARD:		//用户叫牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallCard));
			if (wDataSize!=sizeof(CMD_C_CallCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_CallCard * pCallCard=(CMD_C_CallCard *)pData;
			return OnUserCallCard(pIServerUserItem->GetChairID(),pCallCard->cbCallCard,pCallCard->cbCallCount);
		}
	case SUB_C_CALL_FINISH:		//叫牌完成
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserCallFinish(pIServerUserItem->GetChairID());
		}
	case SUB_C_CONCEAL_CARD:	//底牌扑克
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ConcealCard));
			if (wDataSize!=sizeof(CMD_C_ConcealCard)) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_ConcealCard * pConcealCard=(CMD_C_ConcealCard *)pData;
			return OnUserConcealCard(pIServerUserItem->GetChairID(),pConcealCard->cbConcealCard,pConcealCard->cbConcealCount);
		}
	case SUB_C_OUT_CARD:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

			//用户效验
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData,pOutCard->cbCardCount);
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
	//设置变量
	if (bLookonUser==false)
	{
		m_cbMainValue=0x02;
		m_cbValueOrder[0]=0x02;
		m_cbValueOrder[1]=0x02;
		m_wBankerUser=INVALID_CHAIR;
	}

	return true;
}

//用户叫牌
bool CTableFrameSink::OnUserCallCard(WORD wChairID, BYTE cbCallCard, BYTE cbCallCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_CALL) return true;

	//用户效验
	ASSERT(m_bCallCard[wChairID]==false);
	if (m_bCallCard[wChairID]==true) 
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_cbCallCard=cbCallCard;
	m_wCallCardUser=wChairID;
	m_cbCallCount=cbCallCount;

	//构造数据
	CMD_S_CallCard CallCard;
	CallCard.cbCallCard=m_cbCallCard;
	CallCard.cbCallCount=m_cbCallCount;
	CallCard.wCallCardUser=m_wCallCardUser;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_CARD,&CallCard,sizeof(CallCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_CARD,&CallCard,sizeof(CallCard));

	OnUserCallFinish(wChairID);
	return true;
}

//叫牌完成
bool CTableFrameSink::OnUserCallFinish(WORD wChairID)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_CALL) return true;

	//用户效验
	ASSERT(m_bCallCard[wChairID]==false);
	if (m_bCallCard[wChairID]==true) 
	{
		ASSERT(FALSE);
		return false;
	}

	//变量设置
	m_bCallCard[wChairID]=true;

	//重新发牌
	if (m_wCallCardUser==INVALID_CHAIR)
	{
		//设置状态
		ZeroMemory(m_bCallCard,sizeof(m_bCallCard));

		//发送扑克
		DispatchUserCard();

		return true;
	}

	//花色调整
	if (m_wCallCardUser==INVALID_CHAIR) m_cbMainColor=COLOR_NT;
	else m_cbMainColor=m_GameLogic.GetCardColor(m_cbCallCard);

	//庄家设置
	if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wCallCardUser;

	//设置变量
	m_wCurrentUser=m_wBankerUser;
	m_wFirstOutUser=m_wBankerUser;
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_pITableFrame->SetGameStatus(GS_UG_BACK);

	//设置底牌
	BYTE cbHandCardCount=m_cbHandCardCount[m_wCurrentUser];
	CopyMemory(&m_cbHandCardData[m_wCurrentUser][cbHandCardCount],m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));
	m_cbHandCardCount[m_wCurrentUser]+=m_cbConcealCount;

	//构造数据
	CMD_S_SendConceal SendConceal;
	SendConceal.cbMainColor=m_cbMainColor;
	SendConceal.wBankerUser=m_wBankerUser;
	SendConceal.wCurrentUser=m_wCurrentUser;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//发送消息
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CONCEAL,&SendConceal,sizeof(SendConceal));
		m_pITableFrame->SendLookonData(i,SUB_S_SEND_CONCEAL,&SendConceal,sizeof(SendConceal));
	}

	//检测冲关分
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//第3张牌不是王，就跳过
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
		if(m_GameLogic.GetCardColor(m_cbHandCardData[i][2])!=0x40) continue;
		BYTE bTempCardCount=m_cbHandCardCount[i];
		BYTE bTempCard[MAX_COUNT];
		ZeroMemory(bTempCard,sizeof(bTempCard));
		CopyMemory(bTempCard,m_cbHandCardData[i],sizeof(bTempCard));
		m_GameLogic.SortCardList(bTempCard,bTempCardCount);

		//检测72
		BYTE bMain7Count=0;
		BYTE bFu7Count=0;
		BYTE bMain2Count=0;
		BYTE bFu2Count=0;
		for(BYTE j=0;j<bTempCardCount;j++)
		{
			BYTE bCard=bTempCard[j];
			if(m_GameLogic.GetCardColor(bCard)==m_cbMainColor && m_GameLogic.GetCardValue(bCard)==7) bMain7Count++;
			if(m_GameLogic.GetCardColor(bCard)!=m_cbMainColor && m_GameLogic.GetCardValue(bCard)==7)
			{
				if(bTempCard[j+1]==bTempCard[j]) bFu7Count++;
			}
			if(m_GameLogic.GetCardColor(bCard)==m_cbMainColor && m_GameLogic.GetCardValue(bCard)==2) bMain2Count++;
			if(m_GameLogic.GetCardColor(bCard)!=m_cbMainColor && m_GameLogic.GetCardValue(bCard)==2) 
			{
				if(bTempCard[j+1]==bTempCard[j]) bFu2Count++;
			}
		}

		//4个王
		LONGLONG lKingScore=0;
		if(bTempCard[3]==0x4E&&m_btGiveUpScore[i]==false)
		{
			m_bFourKing=true;
			if		(bMain7Count>=2 && bFu7Count>=2 && bMain2Count>=2 && bFu2Count>=2)	lKingScore=7;
			else if	(bMain7Count>=2 && bFu7Count>=2 && bMain2Count>=2 )					lKingScore=6;
			else if	(bMain7Count>=2 && bFu7Count>=2 )									lKingScore=5;
			else if	(bMain7Count>=2 )													lKingScore=4;
			else																		lKingScore=3;
		}
		else if(m_btGiveUpScore[i]==false||m_bLandScore<85)
		{		
			m_bThreeKing=true;
			if		(bMain7Count>=2 && bFu7Count>=2 && bMain2Count>=2 && bFu2Count>=2)	lKingScore=5;
			else if	(bMain7Count>=2 && bFu7Count>=2 && bMain2Count>=2 )					lKingScore=4;
			else if	(bMain7Count>=2 && bFu7Count>=2 )									lKingScore=3;
			else if	(bMain7Count>=2 )													lKingScore=2;
			else																		lKingScore=1;
		}

		m_lKingScore[(i+1)%GAME_PLAYER]=-1*lKingScore*m_pGameServiceOption->lCellScore;
		m_lKingScore[(i+2)%GAME_PLAYER]=-1*lKingScore*m_pGameServiceOption->lCellScore;
		m_lKingScore[(i+3)%GAME_PLAYER]=-1*lKingScore*m_pGameServiceOption->lCellScore;
		m_lKingScore[i]=3*lKingScore*m_pGameServiceOption->lCellScore;
	}

	return true;
}

//底牌扑克
bool CTableFrameSink::OnUserConcealCard(WORD wChairID, BYTE cbConcealCard[], BYTE cbConcealCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_BACK);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_BACK) return true;

	//效验数据
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbConcealCount==cbConcealCount));
	if ((wChairID!=m_wCurrentUser)||(m_cbConcealCount!=cbConcealCount))
	{
		ASSERT(FALSE);
		return false;
	}

	//删除底牌
	if (m_GameLogic.RemoveCard(cbConcealCard,cbConcealCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置扑克
	m_cbHandCardCount[wChairID]-=cbConcealCount;
	CopyMemory(m_cbConcealCard,cbConcealCard,cbConcealCount*sizeof(BYTE));

	//扑克排列
	for (WORD i=0;i<m_wPlayerCount;i++) 
	{
		ASSERT(m_cbHandCardCount[i]==m_GameLogic.GetDispatchCount());
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
	}

	//设置状态
	m_pITableFrame->SetGameStatus(GS_UG_PLAY);

	//构造数据
	CMD_S_GamePlay GamePlay;
	GamePlay.wCurrentUser=m_wCurrentUser;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//构造底牌
		if (i==m_wCurrentUser)
		{
			GamePlay.cbConcealCount=m_cbConcealCount;
			CopyMemory(GamePlay.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
		}
		else 
		{
			GamePlay.cbConcealCount=0;
			ZeroMemory(GamePlay.cbConcealCard,sizeof(GamePlay.cbConcealCard));
		}

		//发送消息
		m_pITableFrame->SendTableData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_PLAY) return true;

	//效验用户
	ASSERT((wChairID==m_wCurrentUser)&&(cbCardCount<=MAX_COUNT));
	if ((wChairID!=m_wCurrentUser)||(cbCardCount>MAX_COUNT))
	{
		ASSERT(FALSE);
		return false;
	}

	//首出牌者
	if (m_wFirstOutUser==wChairID)
	{
		//类型判断
		BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
		if (cbCardType==CT_ERROR)
		{
			ASSERT(FALSE);
			return false;
		}

		//甩牌处理
		if (cbCardType==CT_THROW_CARD)
		{
			//甩牌判断
			tagOutCardResult OutCardResult;
			ZeroMemory(&OutCardResult,sizeof(OutCardResult));
			bool bThrowSuccess=m_GameLogic.EfficacyThrowCard(cbCardData,cbCardCount,wChairID,m_cbHandCardData,m_cbHandCardCount[wChairID],OutCardResult);

			//结果处理
			if (bThrowSuccess==false)
			{
#ifndef DEBUG
				//通知消息
				TCHAR szMessage[128]=TEXT("");
				if(wChairID==m_wBankerUser)
					_sntprintf(szMessage,CountArray(szMessage),TEXT("庄家甩牌失败，扣 10 分"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("甩牌失败，庄家加 10 分"));
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					IServerUserItem * pISendUserItem=m_pITableFrame->GetTableUserItem(i);
					if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_CHAT);
				}
#endif
				//删除扑克
				if (m_GameLogic.RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false) return false;
				m_cbHandCardCount[wChairID]-=OutCardResult.cbCardCount;

				//出牌记录
				m_cbOutCardCount[wChairID]=OutCardResult.cbCardCount;
				CopyMemory(m_cbOutCardData[wChairID],OutCardResult.cbResultCard,OutCardResult.cbCardCount);

				//切换用户
				m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

				//错误扣分
				if(wChairID==m_wBankerUser)
					m_PlayerScore[m_wBankerUser]-=10;
				else
					m_PlayerScore[m_wBankerUser]+=10;

				//构造数据
				CMD_S_ThrowResult ThrowResult;
				ThrowResult.wOutCardUser=wChairID;
				ThrowResult.wCurrentUser=m_wCurrentUser;
				ThrowResult.cbThrowCardCount=cbCardCount;
				ThrowResult.cbResultCardCount=OutCardResult.cbCardCount;
				CopyMemory(&ThrowResult.cbCardDataArray[0],cbCardData,sizeof(BYTE)*cbCardCount);
				CopyMemory(&ThrowResult.cbCardDataArray[cbCardCount],OutCardResult.cbResultCard,sizeof(BYTE)*OutCardResult.cbCardCount);

				CopyMemory(ThrowResult.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));

				//发送计算
				BYTE cbCardDataCount=ThrowResult.cbThrowCardCount+ThrowResult.cbResultCardCount;
				//WORD wSendSize=sizeof(ThrowResult)-sizeof(ThrowResult.cbCardDataArray)+cbCardDataCount*sizeof(BYTE);
				WORD wSendSize=sizeof(CMD_S_ThrowResult);

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THROW_RESULT,&ThrowResult,wSendSize);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THROW_RESULT,&ThrowResult,wSendSize);

				return true;
			}
		}
	}
	else
	{
		//数目判断
		ASSERT(cbCardCount==m_cbOutCardCount[m_wFirstOutUser]);
		if (cbCardCount!=m_cbOutCardCount[m_wFirstOutUser]) 
		{
			ASSERT(FALSE);
			return false;
		}

		//出牌效验
		if (m_GameLogic.EfficacyOutCard(cbCardData,cbCardCount,m_cbOutCardData[m_wFirstOutUser],
			m_cbOutCardCount[m_wFirstOutUser],m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}
	m_cbHandCardCount[wChairID]-=cbCardCount;

	//出牌记录
	m_cbOutCardCount[wChairID]=cbCardCount;
	CopyMemory(m_cbOutCardData[wChairID],cbCardData,cbCardCount);

	//切换用户
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wFirstOutUser) m_wCurrentUser=INVALID_CHAIR;

	//检查无主
	if(m_bUserNoMain[wChairID]==false && m_GameLogic.GetCardLogicColor(m_cbHandCardData[wChairID][0])!=COLOR_NT)
	{
		m_bUserNoMain[wChairID]=true;

		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		//发送信息
		CString szEndMessage;
		szEndMessage.Format(TEXT("[ %s ]： 无主牌。"),pUserItem!=NULL?pUserItem->GetNickName():TEXT("获取用户名失败"));
#ifndef DEBUG
		//发送信息
		m_pITableFrame->SendGameMessage(szEndMessage,SMT_CHAT);
#endif
	}	
	//两人无主
	BYTE bNoMainCount=0;
	for(BYTE i=0;i<GAME_PLAYER;i++) if(m_bUserNoMain[i]==true) bNoMainCount++;
	BYTE bUserMainCount[GAME_PLAYER];
	BYTE bDoubleCount[GAME_PLAYER];
	ZeroMemory(bUserMainCount,sizeof(bUserMainCount));
	ZeroMemory(bDoubleCount,sizeof(bDoubleCount));
	//if(m_bSendNoMain==false)
	{
		if(bNoMainCount>=2)
		{
			m_bSendNoMain=true;
			//详细信息
			TCHAR szMessage[2048]=TEXT("");
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				//主牌张数
				BYTE bMainCount=0;
				for(BYTE j=0;j<m_cbHandCardCount[i];j++) if(m_GameLogic.GetCardLogicColor(m_cbHandCardData[i][j])==COLOR_NT) bMainCount++;
				//对子个数
				BYTE bDuiZiCount=0;
				for(BYTE k=0;k<m_cbHandCardCount[i]-1;k++) if(m_cbHandCardData[i][k]==m_cbHandCardData[i][k+1]&&m_GameLogic.GetCardLogicColor(m_cbHandCardData[i][k])==COLOR_NT) bDuiZiCount++;

				bUserMainCount[i]=bMainCount;
				bDoubleCount[i]=bDuiZiCount;
				TCHAR szBuffer[512]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n[ %s ]： 还有 %d 张主牌，%d 个对子。"),m_pITableFrame->GetTableUserItem(i)->GetNickName(),bMainCount,bDuiZiCount);

				lstrcat(szMessage,szBuffer);
			}	
#ifndef DEBUG
			//发送信息
			for (WORD n=0;n<GAME_PLAYER;n++) m_pITableFrame->SendGameMessage(m_pITableFrame->GetTableUserItem(n),szMessage,SMT_CHAT);	
#endif
		}
	}
	if(bNoMainCount==3) m_GameLogic.SetCanThrow(true);

	//构造数据
	CMD_S_OutCard OutCard;
	if(m_cbHandCardCount[wChairID]==0) OutCard.bLastTurn=true;
	else OutCard.bLastTurn=false;
	if(m_wFirstOutUser==wChairID)	OutCard.bFirstOut=true;
	else OutCard.bFirstOut=false;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bNoMainUser,m_bUserNoMain,sizeof(m_bUserNoMain));
	CopyMemory(OutCard.bMainCount,bUserMainCount,sizeof(bUserMainCount));
	CopyMemory(OutCard.bDoubleCount,bDoubleCount,sizeof(bDoubleCount));

	//发送数据
	WORD wSendSize=sizeof(CMD_S_OutCard);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//一轮结算
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//判断胜者
		m_wTurnWinner=m_GameLogic.CompareCardArray(m_cbOutCardData,m_cbOutCardCount[0],m_wFirstOutUser);

		//计算得分
		if ((m_wTurnWinner!=m_wBankerUser)/*&&((m_wTurnWinner+2)%m_wPlayerCount!=m_wBankerUser)*/)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				BYTE * pcbScoreCard=&m_cbScoreCardData[m_cbScoreCardCount];
				BYTE cbMaxCount=CountArray(m_cbScoreCardData)-m_cbScoreCardCount;
				m_cbScoreCardCount+=m_GameLogic.GetScoreCard(m_cbOutCardData[i],m_cbOutCardCount[i],pcbScoreCard,cbMaxCount);
			}
		}

		//本轮胜利者得分
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			m_PlayerScore[m_wTurnWinner]+=m_GameLogic.GetCardScore(m_cbOutCardData[i],m_cbOutCardCount[i]);
		}

		//设置变量
		if (m_cbHandCardCount[m_wTurnWinner]!=0)
		{
			m_wFirstOutUser=m_wTurnWinner;
			ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
			ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
		}

		//用户切换
		m_wCurrentUser=(m_cbHandCardCount[m_wTurnWinner]==0)?INVALID_CHAIR:m_wTurnWinner;

		//构造数据
		CMD_TurnBalance TurnBalance;
		TurnBalance.wTurnWinner=m_wTurnWinner;
		TurnBalance.wCurrentUser=m_wCurrentUser;

		CopyMemory(TurnBalance.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
	}

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		try{
			OnEventGameConclude(m_wTurnWinner,NULL,GER_NORMAL);
		}catch(...)
		{
			ASSERT(0);
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchUserCard()
{
	//混乱扑克
	BYTE cbRandCard[CELL_PACK*MAX_PACK];
	m_GameLogic.RandCardList(cbRandCard,CELL_PACK*m_cbPackCount);

	//用户扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbHandCardCount[i]=m_GameLogic.GetDispatchCount();
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
	}

	//指定发牌
	//#ifdef _DEBUG
	//BYTE test1[25]={0x4F,0x4F,0x4E,0x4E,0x17,0x17,0x02,0x02,0x12,0x12,0x22,0x22,0x32,0x32,0x27,0x27,0x27,0x27,0x27,0x27,0x26,0x26,0x25,0x25,0x27};
	//BYTE test2[25]={0x16,0x16,0x17,0x17,0x18,0x18,0x06,0x06,0x07,0x07,0x08,0x08,0x26,0x26,0x27,0x27,0x28,0x28,0x16,0x16,0x17,0x17,0x18,0x18,0x18};
	//BYTE test3[25]={0x16,0x16,0x17,0x17,0x18,0x18,0x16,0x16,0x17,0x17,0x08,0x08,0x26,0x26,0x27,0x27,0x28,0x28,0x16,0x16,0x17,0x17,0x18,0x18,0x18};
	//BYTE test4[25]={0x16,0x16,0x17,0x17,0x18,0x18,0x16,0x16,0x17,0x17,0x08,0x08,0x26,0x26,0x27,0x27,0x18,0x18,0x16,0x16,0x17,0x17,0x18,0x18,0x18};
	//CopyMemory(m_cbHandCardData[0],test1,sizeof(m_cbHandCardData[1]));
	//CopyMemory(m_cbHandCardData[1],test2,sizeof(m_cbHandCardData[2]));
	//CopyMemory(m_cbHandCardData[2],test3,sizeof(m_cbHandCardData[3]));
	//CopyMemory(m_cbHandCardData[3],test4,sizeof(m_cbHandCardData[4]));
	//#endif

	//底牌扑克
	m_cbConcealCount=CELL_PACK*m_cbPackCount-m_wPlayerCount*m_GameLogic.GetDispatchCount();
	CopyMemory(m_cbConcealCard,&cbRandCard[CELL_PACK*m_cbPackCount-m_cbConcealCount],sizeof(BYTE)*m_cbConcealCount);
	m_GameLogic.SortCardList(m_cbConcealCard,m_cbConcealCount);

	//发送开始
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(SendCard.cbBackCard,m_cbConcealCard,sizeof(m_cbConcealCard));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//效验状态
		ASSERT(m_cbHandCardCount[i]<=CountArray(SendCard.cbCardData));
		if (m_cbHandCardCount[i]>CountArray(SendCard.cbCardData)) 
		{
			ASSERT(FALSE);
			return false;
		}

		//拷贝扑克
		SendCard.cbCardCount=m_cbHandCardCount[i];
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
		CopyMemory(SendCard.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送统计
		//WORD wHeadSize=sizeof(SendCard)-sizeof(SendCard.cbCardData);
		//WORD wSendSize=wHeadSize+sizeof(SendCard.cbCardData[0])*SendCard.cbCardCount;
		WORD wSendSize=sizeof(SendCard);

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,wSendSize);
		m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendCard,wSendSize);
	}

	return true;
}

//叫分事件
bool CTableFrameSink::OnUserLandScore(WORD wChairID, WORD bLandScore)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_UG_SCORE) return true;
	if (wChairID!=m_wCurrentUser)
	{
		ASSERT(FALSE);
		return false;
	}

	//效验参数
	if (bLandScore>=m_bLandScore&&bLandScore!=255)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	if (bLandScore!=255&&bLandScore<m_bLandScore)
	{
		m_bLandScore=bLandScore;
		m_wCurrentUser=wChairID;
		m_wBankerUser=m_wCurrentUser;
	}


	//放弃叫分
	if(bLandScore==255) m_btGiveUpScore[wChairID]=true;
	m_wUserScore[wChairID]=bLandScore;

	//冲关者叫分
	if(bLandScore!=255 && m_lKingScore[wChairID]>0) m_bKingCall=true;
	if(bLandScore==255 && m_bLandScore<85 &&  m_lKingScore[wChairID]>0 && m_bFourKing==false) m_bKingCall=true;

	//开始判断
	BYTE GiveUpPlayer=0;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_btGiveUpScore[i]==true) GiveUpPlayer++;
	}
	if ((m_bLandScore==0)||GiveUpPlayer==4||(GiveUpPlayer==3&&m_bLandScore<120))
	{
		//冲关者未叫分
		if(m_bKingCall==false) ZeroMemory(m_lKingScore,sizeof(m_lKingScore));

		//设置变量
		if (m_bLandScore==255) m_bLandScore=120;
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

		//设置状态
		m_pITableFrame->SetGameStatus(GS_UG_CALL);

		//出牌信息
		m_wCurrentUser=m_wBankerUser;

		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			//构造数据
			CMD_S_GameStart GameStart;
			GameStart.bLandScore=m_bLandScore;
			GameStart.wBankerUser=m_wBankerUser;
			GameStart.cbPackCount=m_cbPackCount;
			GameStart.cbMainValue=m_cbMainValue;
			GameStart.cbValueOrder[0]=m_cbValueOrder[0];
			GameStart.cbValueOrder[1]=m_cbValueOrder[1];

			//构造底牌
			if (i==m_wCurrentUser)
			{
				GameStart.cbConcealCount=m_cbConcealCount;
				CopyMemory(GameStart.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			}
			else 
			{
				GameStart.cbConcealCount=0;
				ZeroMemory(GameStart.cbConcealCard,sizeof(GameStart.cbConcealCard));
			}

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		return true;
	}

	//设置用户
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

	//最多连续检测两次
	if(m_btGiveUpScore[m_wCurrentUser]==true)	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if(m_btGiveUpScore[m_wCurrentUser]==true)	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;

	//发送消息
	CMD_S_LandScore LandScore;
	LandScore.bLandUser=wChairID;
	LandScore.bLandScore=m_bLandScore;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	CopyMemory(LandScore.wUserScore,m_wUserScore,sizeof(m_wUserScore));
	CopyMemory(LandScore.bGiveUpScore,m_btGiveUpScore,sizeof(m_btGiveUpScore));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
