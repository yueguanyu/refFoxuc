#include "StdAfx.h"
#include "HistoryRecord.h"

#define new DEBUG_NEW

//////////////////////////////////////////////////////////////////////////////////
//宏定义

//索引定义
#define INDEX_CURRENT				0									//当前索引
#define INDEX_HISTORY				1									//历史索引

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CHistoryCard::CHistoryCard()
{
	//设置变量
	m_bHistoryCard=false;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbEventFlag,sizeof(m_cbEventFlag));

	return;
}

//析构函数
CHistoryCard::~CHistoryCard()
{
}

//重置数据
VOID CHistoryCard::ResetData()
{
	//设置变量
	m_bHistoryCard=false;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbEventFlag,sizeof(m_cbEventFlag));

	return;
}

//获取动作
BYTE CHistoryCard::GetCurrentEvent(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return AF_NULL;

	return m_cbEventFlag[wChairID][INDEX_CURRENT];
}

//获取动作
BYTE CHistoryCard::GetHistoryEvent(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return AF_NULL;

	return m_cbEventFlag[wChairID][INDEX_HISTORY];
}

//获取扑克
BYTE CHistoryCard::GetCurrentCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//效验参数
	ASSERT((wChairID<GAME_PLAYER)&&(cbBufferCount>=MAX_COUNT));
	if ((wChairID>=GAME_PLAYER)||(cbBufferCount<MAX_COUNT)) return 0;

	//拷贝扑克
	BYTE cbCardCount=m_cbCardCount[wChairID][INDEX_CURRENT];
	CopyMemory(cbCardBuffer,m_cbCardData[wChairID][INDEX_CURRENT],cbCardCount);

	return cbCardCount;
}

//获取扑克
BYTE CHistoryCard::GetHistoryCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//效验参数
	ASSERT((wChairID<GAME_PLAYER)&&(cbBufferCount>=MAX_COUNT));
	if ((wChairID>=GAME_PLAYER)||(cbBufferCount<MAX_COUNT)) return 0;

	//拷贝扑克
	BYTE cbCardCount=m_cbCardCount[wChairID][INDEX_HISTORY];
	CopyMemory(cbCardBuffer,m_cbCardData[wChairID][INDEX_HISTORY],cbCardCount);

	return cbCardCount;
}

//保存事件
VOID CHistoryCard::OnEventSaveData(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//保存数据
	if (m_cbEventFlag[wChairID][INDEX_CURRENT]!=AF_NULL)
	{
		//发送消息
		if (m_bHistoryCard==false)
		{
			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
			if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(WM_ENABLE_HISTORY,0,0);
		}

		//设置标志
		m_bHistoryCard=true;

		//保存数据
		m_cbEventFlag[wChairID][INDEX_HISTORY]=m_cbEventFlag[wChairID][INDEX_CURRENT];
		m_cbCardCount[wChairID][INDEX_HISTORY]=m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*MAX_COUNT);

		//清理数据
		m_cbCardCount[wChairID][INDEX_CURRENT]=0;
		m_cbEventFlag[wChairID][INDEX_CURRENT]=AF_NULL;
		ZeroMemory(m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*MAX_COUNT);
	}

	return;
}

//用户放弃
VOID CHistoryCard::OnEventUserPass(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//效验记录
	//ASSERT(m_cbCardCount[wChairID][INDEX_CURRENT]==0);
	//ASSERT(m_cbEventFlag[wChairID][INDEX_CURRENT]==AF_NULL);

	if ( m_cbCardCount[wChairID][INDEX_CURRENT] != 0 )
	{
		m_cbCardCount[wChairID][INDEX_HISTORY] = m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*m_cbCardCount[wChairID][INDEX_CURRENT]);
	}

	if ( m_cbEventFlag[wChairID][INDEX_CURRENT] != AF_NULL )
	{
		m_cbEventFlag[wChairID][INDEX_HISTORY] = m_cbEventFlag[wChairID][INDEX_CURRENT];
	}

	//动作记录
	m_cbEventFlag[wChairID][INDEX_CURRENT] = AF_PASS;

	return;
}

//用户出牌
VOID CHistoryCard::OnEventUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//效验记录
	//ASSERT(m_cbCardCount[wChairID][INDEX_CURRENT]==0);
	//ASSERT(m_cbEventFlag[wChairID][INDEX_CURRENT]==AF_NULL);


	if ( m_cbCardCount[wChairID][INDEX_CURRENT] != 0 )
	{
		m_cbCardCount[wChairID][INDEX_HISTORY] = m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*m_cbCardCount[wChairID][INDEX_CURRENT]);
	}

	if ( m_cbEventFlag[wChairID][INDEX_CURRENT] != AF_NULL )
	{
		m_cbEventFlag[wChairID][INDEX_HISTORY] = m_cbEventFlag[wChairID][INDEX_CURRENT];
	}

	//动作记录
	m_cbEventFlag[wChairID][INDEX_CURRENT]=AF_OUT_CARD;
	m_cbCardCount[wChairID][INDEX_CURRENT]=cbCardCount;
	CopyMemory(m_cbCardData[wChairID][INDEX_CURRENT],cbCardData,sizeof(BYTE)*cbCardCount);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CHistoryScore::CHistoryScore()
{
	//设置变量
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//析构函数
CHistoryScore::~CHistoryScore()
{
}

//重置数据
VOID CHistoryScore::ResetData()
{
	//设置变量
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//获取积分
tagHistoryScore * CHistoryScore::GetHistoryScore(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return NULL;

	//获取积分
	return &m_HistoryScore[wChairID];
}

//用户进入
VOID CHistoryScore::OnEventUserEnter(WORD wChairID)
{
	//效验椅子
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//设置变量
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//用户离开
VOID CHistoryScore::OnEventUserLeave(WORD wChairID)
{
	//效验椅子
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//设置变量
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//用户积分
VOID CHistoryScore::OnEventUserScore(WORD wChairID, SCORE lGameScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	m_HistoryScore[wChairID].lTurnScore=lGameScore;
	m_HistoryScore[wChairID].lCollectScore+=lGameScore;

	return;
}

//////////////////////////////////////////////////////////////////////////////////
