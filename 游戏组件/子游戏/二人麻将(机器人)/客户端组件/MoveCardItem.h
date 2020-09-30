#ifndef MOVE_CARD_ITEM_HEAD_FILE
#define MOVE_CARD_ITEM_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//移动类型
enum MoveKind
{
	MK_NULL					= 0,							//
	MK_OutCard,												//出牌
	MK_SendCard,											//发牌
	MK_ReplaceCard,											//补花
	MK_StartCard											//开局发牌
};

//////////////////////////////////////////////////////////////////////////

//移动扑克类
class CMoveCardItem
{
	//移动类型
protected:
	MoveKind						m_MoveKind;					//移动类型

public:
	//构造函数
	CMoveCardItem() {}

	//析构函数
	virtual ~CMoveCardItem() {}

public:
	//获取移动类型
	virtual MoveKind GetMoveKind() { return m_MoveKind; }
};


//出牌类
class COutCardItem:public CMoveCardItem
{
public:
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克

public:
	//构造函数
	COutCardItem();
	//构造函数
	COutCardItem( const CMD_S_OutCard *pOutCard );

	//析构函数
	virtual ~COutCardItem();
};

//发牌类
class CSendCardItem:public CMoveCardItem
{
public:
	BYTE							m_cbCardData;						//扑克数据
	BYTE							m_cbActionMask;						//动作掩码
	WORD							m_wCurrentUser;						//当前用户
	WORD							m_wReplaceUser;						//补牌用户
	WORD							m_wSendCardUser;					//发牌用户
	bool							m_bTail;							//是否从尾部发牌

public:
	//构造函数
	CSendCardItem();
	//构造函数
	CSendCardItem( const CMD_S_SendCard *pSendCard );

	//析构函数
	virtual ~CSendCardItem();
};

//补花类
class CReplaceCardItem:public CMoveCardItem
{
public:
	WORD							m_wReplaceUser;						//补牌用户
	BYTE							m_cbReplaceCard;					//补牌扑克

public:
	//构造函数
	CReplaceCardItem();
	//构造函数
	CReplaceCardItem( const CMD_S_ReplaceCard *pReplaceCard );

	//析构函数
	virtual ~CReplaceCardItem();
};

//开局发牌类
class CStartCardItem:public CMoveCardItem
{
public:
	WORD							m_wChairId;							//发牌玩家
	BYTE							m_cbCardCount;						//发牌数
	BYTE							m_cbCardData[4];					//发牌值
	WORD							m_wHeapId;							//发牌位置
	WORD							m_wHeapCardIndex;					//堆牌位置
	bool							m_bLastItem;						//是否最后一张牌

public:
	//构造函数
	CStartCardItem();
	//析构函数
	virtual ~CStartCardItem();
};

//////////////////////////////////////////////////////////////////////////

#endif