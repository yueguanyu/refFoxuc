#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义
#define MAX_COUNT 4
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define D2H_VALUE0					0									//无效牌型
#define D2H_D2H						199									//丁二皇型
#define D2H_DOBLUE_Q				198									//天对牌型
#define D2H_DOBLUE_2				197									//地对牌型
#define D2H_DOBLUE_8				196									//仁对牌型
#define D2H_DOBLUE_4				195									//和对牌型
#define D2H_DOBLUE_46A				194									//中对牌型
#define D2H_DOBLUE_67AJ				193									//下对牌型
#define D2H_DOBLUE_9875				192									//花对牌型
#define D2H_TH						191									//天皇牌型
#define D2H_TG						190									//天杠牌型
#define D2H_DG						189									//地杠牌型

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
private:
	static BYTE						m_cbCardListData[32];				//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//功能函数
public:
	//特殊换牌
	void ChangeSpeCard( BYTE cbCardData[], BYTE cbCardCount);
	//特殊数值
	BYTE GetSpeCardValue(BYTE cbCardData[], BYTE cbCardCount);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
