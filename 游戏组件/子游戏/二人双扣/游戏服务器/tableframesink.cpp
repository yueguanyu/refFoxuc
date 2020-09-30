#include "StdAfx.h"
#include "TableFrameSink.h"
#include<cmath>

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;	//开始模式

char *g_strCardColor[]={"方片","梅花","红桃","黑桃",""};
char *g_strCardValue[]={"A","2","3","4","5","6","7","8","9","10","J","Q","K","小王","大王"};
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	memcpy(m_cbRandCard,CGameLogic::m_cbCardData,sizeof(m_cbRandCard));
	//开始信息
	m_nScoreGrade=0;
	m_cbCurGrade=0;
	
	//胜利信息
	m_wGameWiner=0xFFFF;
	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//炸弹记录
	ZeroMemory(m_cbBombList,sizeof(m_cbBombList));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

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
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
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
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//读取配置
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\ShuangKouExConfig.ini"),szPath);

	if(ReadConfigInformation(true)==false)
	{
		//AfxMessageBox(TEXT("加载配置失败，默认配置"));
		SetAcquiesceValue();
		//return true;
	}
	return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//胜利信息
	m_wGameWiner=0xFFFF;
	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
}

//开始模式
BYTE CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_SK_PLAYING);
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	//炸弹记录
	ZeroMemory(m_cbBombList,sizeof(m_cbBombList));

	//确定当前计分等级
	GetUserMinGrade(m_cbCurGrade);
	//混乱扑克
	ZeroMemory(m_cbRandCard,sizeof(m_cbRandCard));
	m_GameLogic.RandCardList(m_cbRandCard,FULL_COUNT);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHandCardCount[i]=MAX_COUNT;
		CopyMemory(&m_cbHandCardData[i],&m_cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
	}

//	BYTE cbCard[]={0x01,0x11,0x21,0x31,0x02,0x12,0x22,0x32,0x03,0x13,0x23,0x33};
//	memcpy(&m_cbHandCardData[0],cbCard,sizeof(cbCard));
    
	/*BYTE cbCard[54]={0x04,0x14,0x24,
					0x02,0x12,0x22,0x32,
					0x03,0x13,0x23,
					0x06,0x16,0x26,
					0x0D,0x1D,0x3D,
					0x01,0x11,0x21,
					0x4E,0x4F,0x4E,0x4F,
					0x34,0x33,0x14,0x24,

					0x04,0x14,0x24,
					0x05,0x15,0x25,0x39,
					0x03,0x13,0x23,
					0x06,0x16,0x26,
					0x07,0x17,0x27,
					0x08,0x18,0x28,
					0x4E,0x4F,0x12,0x4F,
					0x3A,0x3A,0x1A,0x2B
	};

	memcpy(m_cbHandCardData,cbCard,sizeof(cbCard));*/


	//定义变量
    m_wBankerUser =INVALID_CHAIR;
	BYTE cbStartRandcard=0XFF;
	WORD wGetRandCardID=INVALID_CHAIR;
	BYTE cbGetCardPosition=0xFF;
	BYTE cbMainCard=0xFF;
	//设置用户
	if(m_wBankerUser == INVALID_CHAIR)
	{
		int randNomber=rand()%(MAX_COUNT*GAME_PLAYER);
		cbMainCard=m_cbRandCard[randNomber-1];
		cbGetCardPosition=randNomber%MAX_COUNT;
		m_wBankerUser=randNomber/MAX_COUNT;
		wGetRandCardID=m_wBankerUser;

		TCHAR szMessage[128]=TEXT("");
		//通知消息
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		_sntprintf(szMessage,CountArray(szMessage),TEXT("[%s]抓到亮牌%s%s,该玩家为当前庄家"),pIServerUserItem->GetNickName(),g_strCardColor[m_GameLogic.GetCardColor(cbMainCard)>>4],g_strCardValue[m_GameLogic.GetCardValue(cbMainCard)-1]);
		SendTableMessage(szMessage,SMT_PROMPT);
	}

	//游戏变量
	m_wTurnWiner=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//设置变量
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.lCellScore[0]=m_lScoreList[m_cbCurGrade][1];
	GameStart.lCellScore[1]=m_lScoreList[m_cbCurGrade][2];
	GameStart.lCellScore[2]=m_lScoreList[m_cbCurGrade][3];
	CopyMemory(GameStart.bTrustee,m_bTrustee,sizeof(m_bTrustee));

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		GameStart.wGetRandCardID=wGetRandCardID;
		GameStart.cbGetCardPosition=cbGetCardPosition;
		GameStart.cbStartRandCard=cbMainCard;

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
	}

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
	}

	return true;
}


//计算得分
bool CTableFrameSink::CalculationRates(tagScoreInfo ScoreInfo[])
{
	return true;
}

//算分等级
BYTE CTableFrameSink::GetUserMinGrade(BYTE& cbGrade)
{
	cbGrade=0;
	LONGLONG lMinScore=0x0FFFFFFFFFFFFFFF;
	IServerUserItem*pServerUserIntem=NULL;
	int nCurGrade=-1;
	//获取最小分数
	for(WORD wChairID=0;wChairID<GAME_PLAYER;wChairID++)
	{
		if((pServerUserIntem=m_pITableFrame->GetTableUserItem(wChairID))!=NULL)
			lMinScore=min(lMinScore,pServerUserIntem->GetUserScore());		
	}
	//获取算分等级
	for(BYTE cbIndex=0;cbIndex<m_nScoreGrade;cbIndex++)
	{
		if(lMinScore>=m_lScoreList[cbIndex][0])
			nCurGrade=cbIndex;
		else  break;
	}
	
	cbGrade=(BYTE)(min(nCurGrade+1,m_nScoreGrade-1));
	return cbGrade;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			tagScoreInfo ScoreInfo[GAME_PLAYER];
 			ZeroMemory(&GameEnd,sizeof(GameEnd));
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

				//剩余扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}
			
			int cbFinishType=2;
			if(m_cbHandCardCount[(m_wGameWiner==0)?1:0]>=12)
				cbFinishType=3;
			else
			{
				if (m_cbHandCardCount[(m_wGameWiner==0)?1:0]<=6)
					cbFinishType=1;
			}

			//基础得分翻倍
			int cbBombGrade=m_GameLogic.GetMaxBombGrade(m_cbBombList[m_wGameWiner]);
			//5倍开始计算翻倍
			int  nMutile=(int)pow(2,cbBombGrade);
			//游戏积分
			LONGLONG  lMutileScore=m_lScoreList[m_cbCurGrade][cbFinishType]*nMutile;
			//贡献得分			
			LONGLONG lBombScore[GAME_PLAYER];
			ZeroMemory(lBombScore,sizeof(lBombScore));
			GetBombScore(lBombScore);

		
			ScoreInfo[m_wGameWiner].lScore =lMutileScore+(lBombScore[m_wGameWiner]-lBombScore[(m_wGameWiner+1)%GAME_PLAYER]);
			if(ScoreInfo[m_wGameWiner].lScore>0)
			{
				ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_WIN;
				//金币场积分调整
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					LONGLONG lScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
					if(ScoreInfo[m_wGameWiner].lScore>lScore)
						ScoreInfo[m_wGameWiner].lScore=lScore;
				}
				ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_LOSE;
				ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;
			}
			else
			{		
				ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_LOSE;
				//金币场积分调整
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					LONGLONG lScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
					if(ScoreInfo[m_wGameWiner].lScore<(-lScore))
						ScoreInfo[m_wGameWiner].lScore=-lScore;
				}
				ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_WIN;
				ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;
			}

			//游戏积分 kk jia
			for (WORD i =0;i<GAME_PLAYER;i++)
			{
				GameEnd.lGameScore[i] = ScoreInfo[i].lScore;
			}

			//负分处理
			if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				IServerUserItem * pUserItem=m_pITableFrame->GetTableUserItem(0);
				SCORE lScore=pUserItem->GetUserScore();
				bool bScore=false;

				//最少分数
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					pUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pUserItem->GetUserScore()+ScoreInfo[i].lScore < 0)
					{
						lScore=pUserItem->GetUserScore();
						bScore=true;
					}
				}

				//计算分数
				if(bScore == true)
				{
					for(BYTE i=0;i<GAME_PLAYER;i++)
					{
						if(ScoreInfo[i].lScore > 0)
						{
							ScoreInfo[i].lScore=lScore;
							GameEnd.lGameScore[i]=lScore;
						}
						else
						{
							ScoreInfo[i].lScore=-lScore;
							GameEnd.lGameScore[i]=-lScore;
						}
					}
				}
			}
				
			//计算税收 写入积分
			for (WORD wIndex=0;wIndex<GAME_PLAYER;wIndex++)
			{
				//扣税变量
				WORD wRevenue=m_pGameServiceOption->wRevenueRatio;
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&ScoreInfo[wIndex].lScore>0L)
				{
					//积分调整
					ScoreInfo[wIndex].lRevenue=m_pITableFrame->CalculateRevenue(wIndex,ScoreInfo[wIndex].lScore);
					GameEnd.lGameTax=ScoreInfo[wIndex].lRevenue;
				}
				GameEnd.lGameScore[wIndex]=ScoreInfo[wIndex].lScore-ScoreInfo[wIndex].lRevenue;

				m_lAllTurnScore[wIndex]+=GameEnd.lGameScore[wIndex];
				m_lLastTurnScore[wIndex]=GameEnd.lGameScore[wIndex];
			}
			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));


			memcpy(GameEnd.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));
			memcpy(GameEnd.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));

			memcpy(GameEnd.lBombScore,lBombScore,sizeof(lBombScore));
			memcpy(GameEnd.cbBombList,m_cbBombList,sizeof(m_cbBombList));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//切换庄家
			m_wBankerUser=m_wGameWiner;
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			return true;
		}
	case GER_HUNTER:		//玩家抄底
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			//贡献得分			
			LONGLONG lBombScore[GAME_PLAYER];
			ZeroMemory(lBombScore,sizeof(lBombScore));
			GetBombScore(lBombScore);
			m_wGameWiner=(lBombScore[0]>lBombScore[1])?0:1;
			ScoreInfo[m_wGameWiner].lScore =lBombScore[m_wGameWiner]-lBombScore[(m_wGameWiner+1)%GAME_PLAYER];
			ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_WIN;
			//金币场积分调整
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				LONGLONG lScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
				if(ScoreInfo[m_wGameWiner].lScore>lScore)
					ScoreInfo[m_wGameWiner].lScore=lScore;
			}
			ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_LOSE;
			ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;
			
			//游戏积分 kk jia
			for (WORD i =0;i<GAME_PLAYER;i++)
			{
				GameEnd.lGameScore[i] = ScoreInfo[i].lScore;
			}

			//负分处理
			if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				IServerUserItem * pUserItem=m_pITableFrame->GetTableUserItem(0);
				SCORE lScore=pUserItem->GetUserScore();

				//最少分数
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					pUserItem=m_pITableFrame->GetTableUserItem(i);
						if(pUserItem->GetUserScore()-ScoreInfo[i].lScore < 0)
						{
							lScore=pUserItem->GetUserScore();
						}
				}

				//计算分数
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					if(ScoreInfo[i].lScore > 0)
					{
						ScoreInfo[i].lScore=lScore;
						GameEnd.lGameScore[i]=lScore;
					}
					else
					{
						ScoreInfo[i].lScore=-lScore;
						GameEnd.lGameScore[i]=-lScore;
					}
				}
			}

			//计算税收 写入积分
			for (WORD wIndex=0;wIndex<GAME_PLAYER;wIndex++)
			{
				//扣税变量
				WORD wRevenue=m_pGameServiceOption->wRevenueRatio;
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&ScoreInfo[wIndex].lScore>0L)
				{
					//积分调整
					ScoreInfo[wIndex].lRevenue=m_pITableFrame->CalculateRevenue(wIndex,ScoreInfo[wIndex].lScore);
					GameEnd.lGameTax=ScoreInfo[wIndex].lRevenue;
				}
				GameEnd.lGameScore[wIndex]=ScoreInfo[wIndex].lScore-ScoreInfo[wIndex].lRevenue;
				
				m_lAllTurnScore[wIndex]+=GameEnd.lGameScore[wIndex];
				m_lLastTurnScore[wIndex]=GameEnd.lGameScore[wIndex];
			}

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			memcpy(GameEnd.lBombScore,lBombScore,sizeof(lBombScore));
			memcpy(GameEnd.cbBombList,m_cbBombList,sizeof(m_cbBombList));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//切换庄家
			m_wBankerUser=m_wGameWiner;
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			return true;
		}
	case GER_USER_LEAVE:	//用户强退
	case GER_NETWORK_ERROR:	//网络中断
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			if(GameEnd.lGameScore[wChairID]==0)
			{
				GameEnd.lGameScore[wChairID] -=m_pGameServiceOption->lCellScore*15 ;
			}
			//历史积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==wChairID)
				{
					m_lAllTurnScore[i]-=GameEnd.lGameScore[i];
					m_lLastTurnScore[i]=GameEnd.lGameScore[i];

				}
				GameEnd.lAllTurnScore[i]=m_lAllTurnScore[i];
				GameEnd.lLastTurnScore[i]=m_lLastTurnScore[i];
			}

			//变量定义
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

			//修改积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (i==wChairID)
				{
					ScoreInfo[i].cbType=SCORE_TYPE_FLEE;
					ScoreInfo[i].lScore=GameEnd.lGameScore[wChairID];
				}
				else
				{
					ScoreInfo[i].cbType=SCORE_TYPE_NULL;
					ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				}
			}

			//负分处理
			if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				IServerUserItem * pUserItem=m_pITableFrame->GetTableUserItem(0);
				SCORE lScore=pUserItem->GetUserScore();
				bool bScore=false;

				//最少分数
				for(BYTE i=0;i<GAME_PLAYER;i++)
				{
					pUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pUserItem->GetUserScore()+ScoreInfo[i].lScore < 0)
					{
						lScore=pUserItem->GetUserScore();
						bScore=true;
					}
				}

				//计算分数
				if(bScore == true)
				{
					for(BYTE i=0;i<GAME_PLAYER;i++)
					{
						if(ScoreInfo[i].lScore < 0)
						{
							ScoreInfo[i].lScore=-lScore;
							GameEnd.lGameScore[i]=-lScore;
						}
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//切换庄家
			m_wBankerUser=INVALID_CHAIR;
			return true;
		}
	}
	ASSERT(FALSE);

	return false;
}

bool CTableFrameSink::SendTableMessage(LPCTSTR lpszMessage, WORD wMessageType)
{
	IServerUserItem * pISendUserItem=NULL;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		pISendUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pISendUserItem!=NULL) 
			m_pITableFrame->SendGameMessage(pISendUserItem,lpszMessage,wMessageType);
	}

	WORD wIndex=0;
	do
	{
		pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
		if (pISendUserItem==NULL) break;
		m_pITableFrame->SendGameMessage(pISendUserItem,lpszMessage,wMessageType);
	} while (true);
	return true;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_SK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
            //游戏变量
			StatusFree.wBankerUser=m_wBankerUser;
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_SK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore[0]=m_lScoreList[m_cbCurGrade][1];
			StatusPlay.lCellScore[1]=m_lScoreList[m_cbCurGrade][2];
			StatusPlay.lCellScore[2]=m_lScoreList[m_cbCurGrade][3];
		
			//历史积分
			CopyMemory(StatusPlay.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
			CopyMemory(StatusPlay.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));

			//庄家用户
			StatusPlay.wBankerUser=m_wBankerUser;

			//胜利信息
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));
		    CopyMemory(&(StatusPlay.LastOutCard),&m_LastOutCard,sizeof(tagAnalyseResult));


			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
		
			//出牌变量
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				StatusPlay.cbOutCardCount[i]=m_cbOutCardCount[i];
				CopyMemory(StatusPlay.cbOutCardData[i],m_cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i]);
			}
			//扑克信息
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return false;
}


//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}
//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:		//用户出牌
		{
			//变量定义
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if(wDataSize!=sizeof(CMD_C_OutCard)) return false;
			
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			//消息处理
			return OnSubOutCard(pUserData->wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:		//用户Pass	
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;
			//消息处理
			return OnSubPassCard(pUserData->wChairID);
		}
	case SUB_C_TRUSTEE:			//用户托管
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			//变量设置
			m_bTrustee[pUserData->wChairID]=pTrustee->bTrustee;
			return OnSubTrustee(pUserData->wChairID,pTrustee->bTrustee);
		}
	case SUB_C_REQ_HUNTERS: //抄底请求
		{
			return OnSubReqHunter(pIServerUserItem->GetUserInfo()->wChairID,pIServerUserItem);
		}
	default: break;
	}

	ASSERT(FALSE);
	return false;
}




//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

	return false;
}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
	{
		//m_wBankerUser=pIServerUserItem->GetChairID();
	}


	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
//	WORD wChairID=pIServerUserItem->GetUserInfo()->wChairID;

	//庄家设置
	if (bLookonUser==false) 
	{
		m_wBankerUser=INVALID_CHAIR;

		//积分设置
		m_lAllTurnScore[wChairID]=0L;
		m_lLastTurnScore[wChairID]=0L;
	}


	return true;
}

//用户出牌
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_SK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//出牌判断

		tagAnalyseResult CurOutCard;
		memset(&CurOutCard,0,sizeof(tagAnalyseResult));
		m_GameLogic.AnalysebCardData(cbCardData,cbCardCount,CurOutCard);

		//对比扑克
		if (m_GameLogic.CompareCard(CurOutCard,m_LastOutCard))
		{
			//删除扑克
			if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
			{
				ASSERT(FALSE);
				return false;
			}

			if(CurOutCard.m_cbCardType>=CT_BOMB)
				m_cbBombList[wChairID][CurOutCard.m_cbBombGrade-4]++;
			//设置变量
			m_cbTurnCardCount=cbCardCount;
			m_cbHandCardCount[wChairID]-=cbCardCount;
			CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);
			memcpy(&m_LastOutCard,&CurOutCard,sizeof(tagAnalyseResult));
			m_cbOutCardCount[wChairID]=cbCardCount;
			CopyMemory(m_cbOutCardData[wChairID],cbCardData,sizeof(BYTE)*cbCardCount);

			//胜利判断
			if (m_cbHandCardCount[wChairID]==0) 
			{
				//设置用户
				m_wGameWiner=wChairID;
				m_wCurrentUser=INVALID_CHAIR;
			}
			else
			{
				//设置用户
				m_wTurnWiner=wChairID;
				m_wCurrentUser=(wChairID+1)%GAME_PLAYER;
			}

			//构造数据
			CMD_S_OutCard OutCard;
			OutCard.wOutCardUser=wChairID;
			OutCard.cbCardCount=cbCardCount;
			OutCard.wCurrentUser=m_wCurrentUser;
			CopyMemory(OutCard.cbCardData,m_cbTurnCardData,sizeof(m_cbTurnCardData));
			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));

			//结束判断
			if (m_wCurrentUser==INVALID_CHAIR) OnEventGameConclude(wChairID,NULL,GER_NORMAL);
	
		}
		else
		{
			return false;
		}
	return true;
}

//用户放弃
bool CTableFrameSink::OnSubPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_SK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	//设置用户
	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

	//完成判断
	if (m_wCurrentUser==m_wTurnWiner)
	{
		//设置变量
		m_cbTurnCardCount=0;
		ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	}

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}



//用户托管
bool CTableFrameSink::OnSubTrustee(WORD wChairID,bool bTrustee)
{
	//数据校验
	ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
	if(wChairID>GAME_PLAYER) return false;

	//构造变量
	CMD_S_Trustee Trustee;
	Trustee.bTrustee=bTrustee;
	Trustee.wChairID = wChairID;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

	return true;
}

//玩家抄底
bool CTableFrameSink::OnSubReqHunter(WORD wChairID,IServerUserItem * pIServerUserItem)
{

	bool bReturn=true;
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult,sizeof(tagAnalyseResult));
	m_GameLogic.AnalysebCardData(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID],AnalyseResult);
	
	bool bHaveBomb=false;
	for(BYTE cbIndex=0;cbIndex<13;cbIndex++)
	{
		if(AnalyseResult.m_CardStyle[cbIndex].m_cbCount>=4)
		{
			bHaveBomb=true;
			break;
		}
	}
	
	//如果无炸弹且王的个数大于等于三个则有权进行抄底
	if(!bHaveBomb&&AnalyseResult.m_nJockerCount>=3)
	{
		//王变炸弹
		m_cbBombList[wChairID][((AnalyseResult.m_nJockerCount==3)?6:7)-4]++;
		//分析对家
		wChairID=(wChairID+1)%GAME_PLAYER;
		ZeroMemory(&AnalyseResult,sizeof(tagAnalyseResult));
		m_GameLogic.AnalysebCardData(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID],AnalyseResult);

		//进行补炸弹操作
		for(BYTE cbIndex=0;cbIndex<13;cbIndex++)
		{
			if(AnalyseResult.m_CardStyle[cbIndex].m_cbCount>=4)
			{
				if(AnalyseResult.m_nJockerCount>0&&AnalyseResult.m_CardStyle[cbIndex].m_cbCount==AnalyseResult.m_nMaxCount)
				{
					m_cbBombList[wChairID][AnalyseResult.m_CardStyle[cbIndex].m_cbCount+AnalyseResult.m_nJockerCount-4]++;	
					AnalyseResult.m_nJockerCount=0;
				}
				else
				{
					m_cbBombList[wChairID][AnalyseResult.m_CardStyle[cbIndex].m_cbCount-4]++;
				}
			}
		}
		OnEventGameConclude(NULL,NULL,GER_HUNTER);
	}
	else
		bReturn=false;
	return bReturn;
}


//获取炸弹得分/贡献分
bool CTableFrameSink::GetBombScore(LONGLONG *lBombScore)
{
	try
	{
		//初始化变量
		for(WORD wChairID=0;wChairID<GAME_PLAYER;wChairID++)
		{
			m_GameLogic.TransBombGrade(m_cbBombList[wChairID]);
			for(int nIndex=0;nIndex<BOMB_TYPE_COUNT;nIndex++)
				lBombScore[wChairID]+=m_lScoreList[m_cbCurGrade][nIndex+4]*m_cbBombList[wChairID][nIndex];
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}

//读取配置
bool CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	TCHAR OutBuf[255] = TEXT("");
	TCHAR SeachBuf[255] = TEXT("");
	m_nScoreGrade=GetPrivateProfileInt(m_szGameRoomName, TEXT("Row"), -1, m_szConfigFileName);

	if(m_nScoreGrade<= 0||m_nScoreGrade>MAX_SCOREGRADE) return false;

	int col=GetPrivateProfileInt(m_szGameRoomName, TEXT("Col"), -1, m_szConfigFileName);
	if(col<0||col>13) return false;

	for (int nrow = 0;nrow<m_nScoreGrade;nrow++)
	{
		for (int ncol=0;ncol<col;ncol++)
		{
			ZeroMemory(OutBuf,sizeof(OutBuf));
			ZeroMemory(SeachBuf,sizeof(SeachBuf));
			switch(ncol)
			{
			case 0:
				_sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("GradeScore_%d"),nrow);
				break;
			case 1:
				_sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("PingKou_%d"),nrow);
				break;
			case 2:
				_sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("DanKou_%d"),nrow);
				break;
			case 3:
				_sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("ShuangKou_%d"),nrow);
				break;
			default:
				_sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("Bomb%d_%d"),nrow,ncol);
				break;
			}
			GetPrivateProfileString(m_szGameRoomName, SeachBuf, TEXT("-1"), OutBuf, 255, m_szConfigFileName);
			_sntscanf(OutBuf, sizeof(OutBuf), _TEXT("%I64d"), &m_lScoreList[nrow][ncol]);
			if(m_lScoreList[nrow][ncol] == -1) return false;
		}
	}
	return true;
}

//默认配置
void CTableFrameSink::SetAcquiesceValue()
{
	m_lScoreList[0][0]=0;
	m_lScoreList[0][1] 	= 0 ;
	m_lScoreList[0][2] 	= 0	;
	m_lScoreList[0][3] 	= 2	;
	m_lScoreList[0][0] 	= 3 ;

	m_lScoreList[0][4]	= 0	;
	m_lScoreList[0][5]	= 0	;
	m_lScoreList[0][6]	= 6	;
	m_lScoreList[0][7]	= 7	;
	m_lScoreList[0][8]	= 8	;
	m_lScoreList[0][9]	= 9	;
	m_lScoreList[0][10]	= 10;
	m_lScoreList[0][11]	= 11;
	m_lScoreList[0][12]	= 12;

	m_lScoreList[1][0] 	= 10;
	m_lScoreList[1][1] 	= 0;
	m_lScoreList[1][2] 	= 12;
	m_lScoreList[1][3] 	= 13;

	m_lScoreList[1][4]	= 0	;
	m_lScoreList[1][5]	= 0	;
	m_lScoreList[1][6]	= 16;
	m_lScoreList[1][7]	= 17;
	m_lScoreList[1][8]	= 18;
	m_lScoreList[1][9]	= 19;
	m_lScoreList[1][10]	= 20;
	m_lScoreList[1][11]	= 21;
	m_lScoreList[1][12]	= 22;

	m_lScoreList[2][0] 	= 20 	;
	m_lScoreList[2][1] 	= 0	;
	m_lScoreList[2][2] 	= 22	;
	m_lScoreList[2][3] 	= 23 	;

	m_lScoreList[2][4]	= 0	;
	m_lScoreList[2][5]	= 0	;
	m_lScoreList[2][6]	= 26;
	m_lScoreList[2][7]	= 27;
	m_lScoreList[2][8]	= 28;
	m_lScoreList[2][9]	= 29;
	m_lScoreList[2][10]	= 30;
	m_lScoreList[2][11]	= 31;
	m_lScoreList[2][12]	= 32;

	m_lScoreList[3][0] 	= 30 ;
	m_lScoreList[3][1] 	= 0	;
	m_lScoreList[3][2] 	= 32;
	m_lScoreList[3][3] 	= 33 ;

	m_lScoreList[3][4]	= 0	;
	m_lScoreList[3][5]	= 0	;
	m_lScoreList[3][6]	= 36;
	m_lScoreList[3][7]	= 37;
	m_lScoreList[3][8]	= 38;
	m_lScoreList[3][9]	= 39;
	m_lScoreList[3][10]	= 40;
	m_lScoreList[3][11]	= 41;
	m_lScoreList[3][12]	= 42;

	m_lScoreList[4][0] 	= 40 	;
	m_lScoreList[4][1] 	= 0	;
	m_lScoreList[4][2] 	= 42	;
	m_lScoreList[4][3] 	= 43 	;

	m_lScoreList[4][4]		= 0	;
	m_lScoreList[4][5]		= 0	;
	m_lScoreList[4][6]		= 46	;
	m_lScoreList[4][7]		= 47	;
	m_lScoreList[4][8]		= 48	;
	m_lScoreList[4][9]		= 49	;
	m_lScoreList[4][10]	= 50	;
	m_lScoreList[4][11]	= 51	;
	m_lScoreList[4][12]	= 52	;

	m_lScoreList[5][0] 	= 50 	;
	m_lScoreList[5][1] 	= 0	;
	m_lScoreList[5][2] 	= 52	;
	m_lScoreList[5][3] 	= 53 	;

	m_lScoreList[5][4]		= 0	;
	m_lScoreList[5][5]		= 0	;
	m_lScoreList[5][6]		= 56	;
	m_lScoreList[5][7]		= 57	;
	m_lScoreList[5][8]		= 58	;
	m_lScoreList[5][9]		= 59	;
	m_lScoreList[5][10]	= 60	;
	m_lScoreList[5][11]	= 61	;
	m_lScoreList[5][12]	= 62	;

	m_lScoreList[6][0] 	= 60 	;
	m_lScoreList[6][1] 	= 0	;
	m_lScoreList[6][2] 	= 62	;
	m_lScoreList[6][3] 	= 63 	;

	m_lScoreList[6][4]		= 0	;
	m_lScoreList[6][5]		= 0	;
	m_lScoreList[6][6]		= 66	;
	m_lScoreList[6][7]		= 67	;
	m_lScoreList[6][8]		= 68	;
	m_lScoreList[6][9]		= 69	;
	m_lScoreList[6][10]	= 70	;
	m_lScoreList[6][11]	= 71	;
	m_lScoreList[6][12]	= 72	;

	m_lScoreList[7][0] 	= 70 	;
	m_lScoreList[7][1] 	= 0	;
	m_lScoreList[7][2] 	= 72	;
	m_lScoreList[7][3] 	= 73 	;

	m_lScoreList[7][4]		= 0	;
	m_lScoreList[7][5]		= 0	;
	m_lScoreList[7][6]		= 76	;
	m_lScoreList[7][7]		= 77	;
	m_lScoreList[7][8]		= 78	;
	m_lScoreList[7][9]		= 79	;
	m_lScoreList[7][10]	= 80	;
	m_lScoreList[7][11]	= 81	;
	m_lScoreList[7][12]	= 82	;

	m_lScoreList[8][0] 	= 80 	;
	m_lScoreList[8][1] 	= 0	;
	m_lScoreList[8][2] 	= 82	;
	m_lScoreList[8][3] 	= 83 	;

	m_lScoreList[8][4]		= 0	;
	m_lScoreList[8][5]		= 0	;
	m_lScoreList[8][6]		= 86	;
	m_lScoreList[8][7]		= 87	;
	m_lScoreList[8][8]		= 88	;
	m_lScoreList[8][9]		= 89	;
	m_lScoreList[8][10]	= 90	;
	m_lScoreList[8][11]	= 91	;
	m_lScoreList[8][12]	= 92	;

	m_lScoreList[9][0] 	= 90 	;
	m_lScoreList[9][1] 	= 0	;
	m_lScoreList[9][2] 	= 92	;
	m_lScoreList[9][3] 	= 93 	;

	m_lScoreList[9][4]		= 0	;
	m_lScoreList[9][5]		= 0	;
	m_lScoreList[9][6]		= 96	;
	m_lScoreList[9][7]		= 97	;
	m_lScoreList[9][8]		= 98	;
	m_lScoreList[9][9]		= 99	;
	m_lScoreList[9][10]	= 100	;
	m_lScoreList[9][11]	= 101	;
	m_lScoreList[9][12]	= 102	;

	m_lScoreList[10][0] 	= 100 	;
	m_lScoreList[10][1] 	= 0	;
	m_lScoreList[10][2] 	= 102	;
	m_lScoreList[10][3] 	= 103 	;

	m_lScoreList[10][4]	= 0	;
	m_lScoreList[10][5]	= 0	;
	m_lScoreList[10][6]	= 106	;
	m_lScoreList[10][7]	= 107	;
	m_lScoreList[10][8]	= 108	;
	m_lScoreList[10][9]	= 109	;
	m_lScoreList[10][10]	= 110	;
	m_lScoreList[10][11]	= 111	;
	m_lScoreList[10][12]	= 0112	;

	m_lScoreList[11][0] 	= 110 	;
	m_lScoreList[11][1]	= 0	;
	m_lScoreList[11][2] 	= 112	;
	m_lScoreList[11][3]	= 113 	;

	m_lScoreList[11][4]	= 0	;
	m_lScoreList[11][5]	= 0	;
	m_lScoreList[11][6]	= 116	;
	m_lScoreList[11][7]	= 117	;
	m_lScoreList[11][8]	= 118	;
	m_lScoreList[11][9]	= 119	;
	m_lScoreList[11][10]	= 120	;
	m_lScoreList[11][11]	= 121	;
	m_lScoreList[11][12]	= 122	;
}
//////////////////////////////////////////////////////////////////////////
