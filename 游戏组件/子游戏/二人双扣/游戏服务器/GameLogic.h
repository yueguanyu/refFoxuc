#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_VALUE					2									//数值排序

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINK				4									//单连类型
#define CT_DOUBLE_LINK				5									//姐妹对型
#define CT_THREE_LINK				6									//姐妹对型
#define CT_BOMB						7									//炸弹类型
#define CT_BOMB_3W					8									//3王炸弹
#define CT_BOMB_TW					9									//天王炸弹
#define CT_BOMB_LINK				10									//排炸类型

/////////////////////////////////////




//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//////////////////////////////////////////////////////////////////////////



//游戏逻辑类
class CGameLogic
{
	//变量定义
public:
	static const BYTE				m_cbCardData[FULL_COUNT];							//扑克数据
    static const BYTE				m_cbCardGrade[TYPE_COUNT];							//牌型等级
	static const BYTE				m_cbTransTable[BOMB_TYPE_COUNT][BOMB_TYPE_COUNT];	//跃迁列表
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//属性函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//控制函数
public:
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//混乱扑克（带标记）
	BYTE RandCardList(BYTE cbCardList[],BYTE cbBufferCount,BYTE cbMainCard,WORD cbOutPut[]);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//逻辑函数
public:
	//炸弹转换
	bool TransBombGrade(BYTE cbBombInfo[]);
	//获得最高翻倍
	BYTE GetMaxBombGrade(BYTE *cbBomblist);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//逻辑转牌面
	BYTE TransForLogicValue(BYTE cbCardData);
	//逻辑转牌面
	BYTE GetCardLogicValueEx(BYTE cbCardData);
	//对比扑克
	bool CompareCard(tagAnalyseResult& PerOutCard,tagAnalyseResult& CurOutCard);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);

	//内部函数
public:
	//炸弹分析
	bool IsBombCard(tagAnalyseResult& AnalyseResult);
	//查找目标牌型
	bool FindValidCard(tagAnalyseResult& CurOutCard,tagAnalyseResult& cbPerOutCard);
	//补充牌数
	bool FillCardCount(tagAnalyseResult&AnalyseResultEx,int nMinCount);
	//填充牌链
	bool FillCardSeque(tagAnalyseResult&AnalyseResult,int cbMinCount,int cbMinLink,bool bAllowCycle);
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[],int cbCardCount,tagAnalyseResult& AnalyseResult);
	//查找连牌
	bool FindLinkCard(tagAnalyseResult& AnalyseResult,BYTE cbMinCardCount,BYTE cbMinLinkLong,bool bLockCount,bool bAllowCycle);
};

//////////////////////////////////////////////////////////////////////////

#endif