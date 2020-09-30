#include "StdAfx.h"
#include "MoveCardItem.h"

///////////////////////////////////////////////////////////////////////////////////

//构造函数
COutCardItem::COutCardItem()
{
	//
	m_MoveKind = MK_OutCard;

	//初始化
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;
}

//构造函数
COutCardItem::COutCardItem( const CMD_S_OutCard *pOutCard )
{
	//
	m_MoveKind = MK_OutCard;

	m_wOutCardUser = pOutCard->wOutCardUser;
	m_cbOutCardData = pOutCard->cbOutCardData;
}

//析构函数
COutCardItem::~COutCardItem()
{
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CSendCardItem::CSendCardItem()
{
	//
	m_MoveKind = MK_SendCard;

	//初始化
	m_cbCardData = 0;						
	m_cbActionMask = 0;					
	m_wCurrentUser = INVALID_CHAIR;						
	m_wReplaceUser = INVALID_CHAIR;						
	m_wSendCardUser = INVALID_CHAIR;		
	m_bTail = false;
}

//构造函数
CSendCardItem::CSendCardItem( const CMD_S_SendCard *pSendCard )
{
	//
	m_MoveKind = MK_SendCard;

	m_cbCardData = pSendCard->cbCardData;
	m_cbActionMask = pSendCard->cbActionMask;
	m_wCurrentUser = pSendCard->wCurrentUser;
	m_wReplaceUser = pSendCard->wReplaceUser;
	m_wSendCardUser = pSendCard->wSendCardUser;
	m_bTail = pSendCard->bTail;
}

//析构函数
CSendCardItem::~CSendCardItem()
{
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CReplaceCardItem::CReplaceCardItem()
{
	//
	m_MoveKind = MK_ReplaceCard;

	//初始化
	m_cbReplaceCard = 0;										
	m_wReplaceUser = INVALID_CHAIR;										
}

//构造函数
CReplaceCardItem::CReplaceCardItem( const CMD_S_ReplaceCard *pReplaceCard )
{
	//
	m_MoveKind = MK_ReplaceCard;

	m_cbReplaceCard = pReplaceCard->cbReplaceCard;
	m_wReplaceUser = pReplaceCard->wReplaceUser;
}

//析构函数
CReplaceCardItem::~CReplaceCardItem()
{
}

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CStartCardItem::CStartCardItem()
{
	//
	m_MoveKind = MK_StartCard;

	//初始化
	m_wChairId = INVALID_CHAIR;
	m_cbCardCount = 0;					
	ZeroMemory( m_cbCardData,sizeof(m_cbCardData) );
	m_wHeapId = INVALID_CHAIR;
	m_wHeapCardIndex = 0;
	m_bLastItem = false;
}

//析构函数
CStartCardItem::~CStartCardItem()
{
}

///////////////////////////////////////////////////////////////////////////////////
