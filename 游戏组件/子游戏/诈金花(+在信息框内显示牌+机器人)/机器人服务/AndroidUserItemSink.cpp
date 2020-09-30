#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS					1									//最少时间
#define TIME_DISPATCH				5									//发牌时间

//游戏时间
#define TIME_START_GAME				8									//开始时间
#define TIME_CALL_SCORE				6									//叫分时间
#define TIME_GAME_OPENEND			4
#define TIME_GAME_COMPAREEND		8


//定时器标识
#define IDI_USER_ADD_SCORE		(0)									//加注定时器
#define IDI_START_GAME			(1)									//开始定时器
#define IDI_USER_COMPARE_CARD	(2)									//选比牌用户定时器
#define IDI_GIVE_UP				(3)
#define IDI_LOOK_CARD			(4)
#define IDI_OPEN_CARD			(5)
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	m_lCurrentTimes=1L;
	m_lMaxScore=0L;												//封顶数目
	m_lMaxCellScore=0L;											//单元上限
	m_lCellScore=0L;											//单元下注
	m_lUserMaxScore=0L;											//最大分数
	m_cbPlayerCount=0;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));			//游戏玩家
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));			//玩家下注
	
	//手上扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllUserCardData,sizeof(m_cbAllUserCardData));
	ZeroMemory(m_bMingZhu,sizeof(m_bMingZhu));
	//接口变量
	m_pIAndroidUserItem=NULL;;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	m_lCurrentTimes=1L;
	m_lMaxScore=0L;												//封顶数目
	m_lMaxCellScore=0L;											//单元上限
	m_lCellScore=0L;											//单元下注
	m_lUserMaxScore=0L;											//最大分数
	m_cbPlayerCount=0;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));			//游戏玩家
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));			//玩家下注
	
	//手上扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllUserCardData,sizeof(m_cbAllUserCardData));
	ZeroMemory(m_bMingZhu,sizeof(m_bMingZhu));

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_USER_ADD_SCORE:	//用户加注
		{
			//查找上家
			for (LONGLONG j=m_pIAndroidUserItem->GetChairID()-1;;j--)
			{
				if(j==-1)j=GAME_PLAYER-1;
				if(m_cbPlayStatus[j]==TRUE)break;
			}
			//一为跟注
			WORD wTemp=(m_lTableScore[j]==m_lCellScore)?0:1;
			//获取筹码
			LONGLONG lCurrentScore=(wTemp==0)?m_lCellScore:(m_lCellScore*m_lCurrentTimes);

			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			//明注加倍
			if(m_bMingZhu[wMeChairID])lCurrentScore*=2;
			//发送消息
			CMD_C_AddScore AddScore;
			AddScore.wState=0;
			AddScore.lScore=lCurrentScore;
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));
			
			return true;
		}
	case IDI_GIVE_UP:
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP,NULL,0);
			return true;
		}
	case IDI_LOOK_CARD:
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD,NULL,0);
			return true;
		}
	case IDI_OPEN_CARD:
		{


			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			//判断明注
			LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);

			//发送消息
			CMD_C_AddScore AddScore;
			AddScore.wState=2;
			AddScore.lScore=lCurrentScore;
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			if(m_cbPlayerCount==2)
			{
				CMD_C_CompareCard CompareCard;
				//查找上家
				for (LONGLONG i=(LONGLONG)wMeChairID-1;;i--)
				{
					if(i==-1)i=GAME_PLAYER-1;
					if(m_cbPlayStatus[i]==TRUE)
					{
						CompareCard.wCompareUser=(WORD)i;
						break;
					}
				}
				m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
				return true;
			}
			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD,NULL,0);
			return true;
		}
	case IDI_USER_COMPARE_CARD:		//用户比牌
		{
			//判断明注
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);

			//发送消息
			CMD_C_AddScore AddScore;
			AddScore.wState=TRUE;
			AddScore.lScore=lCurrentScore;
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//构造变量
			CMD_C_CompareCard CompareCard;
			//庄家在第一轮没下注只能跟上家比牌 或 只剩下两人
			if(m_wBankerUser==wMeChairID && (m_lTableScore[wMeChairID]-lCurrentScore)==m_lCellScore || m_cbPlayerCount==2)
			{
				//查找上家
				for (LONGLONG i=(LONGLONG)wMeChairID-1;;i--)
				{
					if(i==-1)i=GAME_PLAYER-1;
					if(m_cbPlayStatus[i]==TRUE)
					{
						CompareCard.wCompareUser=(WORD)i;
						break;
					}
				}
			}
			else	//选择玩家比牌
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE,NULL,0);

				WORD wCompare=INVALID_CHAIR;
				for(int i=0;i<GAME_PLAYER;i++)
				{
					if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i&& m_GameLogic.CompareMyCard(wMeChairID,i))
					{
						wCompare=(WORD)i;
						break;
					}
				}
				if(wCompare==INVALID_CHAIR)
				{
					for(int i=0;i<GAME_PLAYER;i++)
					{
						if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
						{
							wCompare=(WORD)i;
							break;
						}
					}
				}

				CompareCard.wCompareUser=wCompare;
			}
			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
			return true;
		}
	}

	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			//消息处理
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_LOOK_CARD:		//看牌消息
		{
			return OnSubLookCard(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//用户下注
		{
			//消息处理
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_COMPARE_CARD:	//比牌消息
		{
			//消息处理
			return OnSubCompareCard(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			//消息处理
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_PLAYER_EXIT:		//用户强退
		{
			//消息处理
			return OnSubPlayerExit(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			//消息处理
			return OnSubGameEnd(pData,wDataSize);
		}
	}
	//错误断言
//	ASSERT(FALSE);

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//玩家设置
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

						//扑克数据
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*3);

			//玩家设置
			if (pStatusPlay->wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
			}

			return true;
		}
	}

	return false;
}

//处理控制
void CAndroidUserItemSink::UpdataControl(WORD wCurrentUser)
{
	BYTE cbMyCardLevel=0;
	bool bMingZhu=false;
	bool bGiveUp=false;
	bool bAddScore=false;
	bool bCompare=false;

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if((!m_cbPlayStatus[i])||(i==wCurrentUser)) continue;
		if(m_GameLogic.CompareMyCard(wCurrentUser,i)) cbMyCardLevel++;
	}

	LONGLONG m_lCurrentScore=m_bMingZhu[wCurrentUser]?(2*m_lCellScore*m_lCurrentTimes):(m_lCellScore*m_lCurrentTimes);
	if(cbMyCardLevel==0)
	{
		if((m_cbPlayerCount>2)&&(m_lTableScore[wCurrentUser]==m_lCellScore))
		{
			bMingZhu=true;
			bGiveUp=true;
		}
		else
		{
			bAddScore=((m_lTableScore[wCurrentUser]+m_lCurrentScore)<2*m_lCellScore)?true:false;
			bMingZhu=true;
			if((!bAddScore)&&((m_lTableScore[wCurrentUser])>5*m_lCellScore))
			{
				if(wCurrentUser==m_wBankerUser || m_lTableScore[wCurrentUser]>=2*m_lCellScore)
				{
					bCompare=true;
				}else{
					bAddScore=true;
				}
			}else{
				bGiveUp=true;
			}
		}
	}
	else if(1==(m_cbPlayerCount-cbMyCardLevel))
	{
		bMingZhu=(m_lTableScore[wCurrentUser]>=m_lMaxCellScore)?true:false;
		bAddScore=((m_lTableScore[wCurrentUser]+m_lCurrentScore) <= m_lUserMaxScore)?true:false;
		if(!bAddScore)	bCompare=true;
	}
	else
	{
		bMingZhu=(m_lTableScore[wCurrentUser]>=3*m_lMaxCellScore)?true:false;
		bAddScore=((m_lTableScore[wCurrentUser]+m_lCurrentScore) < m_lUserMaxScore/10)?true:false;
		if(!bAddScore) bCompare=true;
	}

	//看牌按钮
	if((!m_bMingZhu[wCurrentUser])&&bMingZhu)
	{
		UINT nElapse=rand()%TIME_GAME_OPENEND+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_LOOK_CARD,nElapse);
		return;
	}
	if(bGiveUp)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_GIVE_UP,nElapse);
		return;
	}
	//判断开牌
	LONGLONG lTemp=(m_bMingZhu[wCurrentUser])?6:5;
	if((m_lUserMaxScore-m_lTableScore[wCurrentUser]) >= (m_lMaxCellScore*lTemp))
	{
		//加注按钮
		if(bAddScore)
		{
			UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS+TIME_LESS;
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
			return;
		}
		//比牌按钮
		if(bCompare)
		{
			if(wCurrentUser==m_wBankerUser || m_lTableScore[wCurrentUser]>=2*m_lCellScore)
			{
				UINT nElapse=rand()%TIME_GAME_COMPAREEND+TIME_LESS+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD,nElapse);
				return;
			}
		}
	}
	else 
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD,nElapse);
	}

	return;
}
//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_AndroidCard));
	if (wDataSize!=sizeof(CMD_S_AndroidCard)) return false;

	//变量定义
	CMD_S_AndroidCard * pAndroidCard=(CMD_S_AndroidCard *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_PLAY);

	//手上扑克
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		if(wMeChairID==wChairID)
			CopyMemory(m_cbHandCardData,pAndroidCard->cbHandCard[wChairID],sizeof(BYTE)*MAX_COUNT);
		m_GameLogic.SetUserCard(wChairID, pAndroidCard->cbHandCard[wChairID]) ;
	}
	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,3);

	//数据信息
	m_lCellScore=pAndroidCard->lCellScore;
	m_lMaxCellScore=pAndroidCard->lMaxScore;
	m_lCurrentTimes=pAndroidCard->lCurrentTimes;
	m_wBankerUser=pAndroidCard->m_wBankerUser;
	m_lUserMaxScore=pAndroidCard->lUserMaxScore;

	//设置变量
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		if (!pAndroidCard->cbPlayStatus[i]) continue;
		//游戏信息		
		m_cbPlayStatus[i]=TRUE;
		m_cbPlayerCount++;
		m_lTableScore[i]=m_lCellScore;
	}

	//玩家处理
	if (m_pIAndroidUserItem->GetChairID()==pAndroidCard->wCurrentUser)
	{
		UpdataControl(pAndroidCard->wCurrentUser);
	}

	return true;
}

//用户叫分
bool CAndroidUserItemSink::OnSubAddScore(VOID * pData, WORD wDataSize)
{
//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pData;
	ASSERT(pAddScore->wCurrentUser < GAME_PLAYER);

	m_lCurrentTimes=pAddScore->lCurrentTimes;
	//下注金币
	m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;
	if(m_pIAndroidUserItem->GetChairID()==pAddScore->wCurrentUser)UpdataControl(m_pIAndroidUserItem->GetChairID());

	return true;
}

//比牌消息
bool CAndroidUserItemSink::OnSubCompareCard(VOID * pData, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_CompareCard)) return false;
	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pData;

	//设置变量
	m_cbPlayStatus[pCompareCard->wLostUser]=FALSE;
	m_cbPlayerCount--;
	//状态设置
	if (pCompareCard->wLostUser==m_pIAndroidUserItem->GetChairID())	
	{
		m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_FREE);
		RepositionSink();
	}
	//当前用户
	if(m_pIAndroidUserItem->GetChairID()==pCompareCard->wCurrentUser)UpdataControl(pCompareCard->wCurrentUser);
	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubGiveUp(VOID * pData, WORD wDataSize)
{
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pData;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;
	m_cbPlayerCount--;
	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	//状态设置
	if (pGiveUp->wGiveUpUser==m_pIAndroidUserItem->GetChairID()) 
	{
		m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_FREE);
		RepositionSink();
	}
	return true;
}
//用户看牌
bool CAndroidUserItemSink::OnSubLookCard(VOID * pData, WORD wDataSize)
{
	if (wDataSize!=sizeof(CMD_S_LookCard)) return false;
	CMD_S_LookCard * pLookCard=(CMD_S_LookCard *)pData;
	m_bMingZhu[pLookCard->wLookCardUser]=TRUE;
	if(m_pIAndroidUserItem->GetChairID()==pLookCard->wLookCardUser)
	UpdataControl(pLookCard->wLookCardUser);
	return true;
}
bool CAndroidUserItemSink::OnSubPlayerExit(VOID * pData, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PlayerExit)) return false;
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pData;

	WORD wID=pPlayerExit->wPlayerID;

	//游戏信息
	ASSERT(m_cbPlayStatus[wID]==TRUE);
	m_cbPlayStatus[wID]=FALSE;
	m_cbPlayerCount--;
	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(VOID * pData, WORD wDataSize)
{
	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_FREE);
	
	//设置变量
	RepositionSink();

	//开始设置
	UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//用户进入
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
