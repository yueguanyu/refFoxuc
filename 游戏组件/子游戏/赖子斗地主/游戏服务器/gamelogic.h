#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序

//////////////////////////////////////////////////////////////////////////
//数目定义

#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目
#define GOOD_CARD_COUTN				38									//好牌数目
#define BACK_COUNT					3									//底牌数目
#define NORMAL_COUNT				17									//常规数目

//////////////////////////////////////////////////////////////////////////

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//扑克类型
//#define CT_ERROR					0x000000000									//错误类型
//#define CT_SINGLE					0x000000001									//单牌类型
//#define CT_DOUBLE					0x000000002									//对牌类型
//#define CT_THREE					0x000000004									//三条类型
//#define CT_SINGLE_LINE				0x000000008									//单连类型
//#define CT_DOUBLE_LINE				0x000000010									//对连类型
//#define CT_THREE_LINE				0x000000020									//三连类型
//#define CT_THREE_LINE_TAKE_ONE		0x000000040									//三带一单
//#define CT_THREE_LINE_TAKE_TWO		0x000000080									//三带一对
//#define CT_FOUR_LINE_TAKE_ONE		0x000000100									//四带两单
//#define CT_FOUR_LINE_TAKE_TWO		0x000000200									//四带两对
//#define CT_RUAN_BOMB				0x000000400									//软炸弹
//#define CT_BOMB_CARD				0x000000800									//炸弹类型
//#define CT_LAIZI_BOMB				0x000001000									//赖子炸弹
//#define CT_MISSILE_CARD				0x000002000									//火箭类型

#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_LINE_TAKE_ONE		7									//三带一单
#define CT_THREE_LINE_TAKE_TWO		8									//三带一对
#define CT_FOUR_LINE_TAKE_ONE		9									//四带两单
#define CT_FOUR_LINE_TAKE_TWO		10									//四带两对
#define CT_RUAN_BOMB				11									//软炸弹
#define CT_BOMB_CARD				12									//炸弹类型
#define CT_LAIZI_BOMB				13									//赖子炸弹
#define CT_MISSILE_CARD				14									//火箭类型

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
	BYTE							cbFourCardData[60];			//四张扑克
	BYTE							cbThreeCardData[200];			//三张扑克
	BYTE							cbDoubleCardData[200];		//两张扑克
	BYTE							cbSignedCardData[200];		//单张扑克
	bool							cbLaiZi;							//赖子类型
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

#define MAX_TYPE_COUNT 254
struct tagOutCardTypeResult 
{
	int								cbCardType;							//扑克类型
	BYTE							cbCardTypeCount;					//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//每手个数
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//扑克数据
};


//扑克信息
struct tagHandCardInfo {
	BYTE						cbHandCardData[ MAX_COUNT ];				//扑克数据
	BYTE						cbHandCardCount;							//扑克数目
	tagOutCardTypeResult		CardTypeResult[ 14 + 1 ] ;					//分析数据

	//初始数据
	tagHandCardInfo( void ) {
		ZeroMemory( cbHandCardData, sizeof( cbHandCardData ) ) ;
		cbHandCardCount = 0;
		ZeroMemory( &CardTypeResult, sizeof( CardTypeResult ) );
	}
};

//类型定义
typedef CWHArray< tagHandCardInfo * > tagHandCardInfoArray;

//栈结构
class tagStackHandCardInfo {

	//内联函数
public:

	//构造函数
	tagStackHandCardInfo( void ) { 
		m_HandCardInfoFreeArray.RemoveAll(); 
		m_HandCardInfoArray.RemoveAll();
	}

	//析构函数
	~tagStackHandCardInfo( void ) {

		//清空栈
		ClearAll();
	}

	//元素压栈
	void Push( tagHandCardInfo * pHandCardInfo ) {

		//是否还有空间
		if ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			//获取空间
			tagHandCardInfo * pHandCardInfoFree = m_HandCardInfoFreeArray[ 0 ];
			m_HandCardInfoFreeArray.RemoveAt( 0 );

			//元素赋值
			CopyMemory( pHandCardInfoFree->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pHandCardInfoFree->cbHandCardData ) );
			pHandCardInfoFree->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pHandCardInfoFree->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//压入栈顶
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pHandCardInfoFree );
		}
		else {
			//申请空间
			tagHandCardInfo * pNewHandCardInfo = new tagHandCardInfo ;

			//元素赋值
			CopyMemory( pNewHandCardInfo->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pNewHandCardInfo->cbHandCardData ) );
			pNewHandCardInfo->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pNewHandCardInfo->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//压入栈顶
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pNewHandCardInfo );
		}
		
	}

	//弹出栈顶
	void Pop() {

		//非空判断
		if ( IsEmpty() ) return ;

		//获取元素
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		tagHandCardInfo * pTopHandCardInfo = m_HandCardInfoArray.GetAt( nECount - 1 );

		//移除元素
		m_HandCardInfoArray.RemoveAt( nECount - 1 );

		//保存空间
		m_HandCardInfoFreeArray.Add( pTopHandCardInfo );		
	}

	//初始栈
	void InitStack() {

		//保存空间
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			m_HandCardInfoArray.RemoveAt( 0 );
			m_HandCardInfoFreeArray.Add( pHandCardInfo );
		}
	}

	//清空栈
	void ClearAll() {

		//释放内存
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoArray.RemoveAt( 0 );
		}

		//释放内存
		while ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoFreeArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoFreeArray.RemoveAt( 0 );
		}
	}

	//获取栈顶
	void GetTop( tagHandCardInfo * & pHandCardInfo ) {

		//非空判断
		if ( IsEmpty() ) {
			ASSERT( false );
			return;
		}

		//获取元素
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		pHandCardInfo = m_HandCardInfoArray[ nECount - 1 ];
	}

	//空判断
	bool IsEmpty() {
		return m_HandCardInfoArray.IsEmpty();
	}

	//成员变量
private:
	tagHandCardInfoArray			m_HandCardInfoFreeArray;					//扑克信息
	tagHandCardInfoArray			m_HandCardInfoArray;						//扑克信息

};


//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据
	static const BYTE				m_cbGoodcardData[GOOD_CARD_COUTN];	//好牌数据
	BYTE							m_bLaiZiCard;						//赖子牌
	
	//AI变量
public:
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//所有扑克
	BYTE							m_cbLandScoreCardData[MAX_COUNT];	//叫牌扑克
	BYTE							m_cbUserCardCount[GAME_PLAYER];		//扑克数目
	WORD							m_wBankerUser;						//地主玩家
	LONGLONG							m_lBankerOutCardCount ;				//出牌次数
	tagStackHandCardInfo			m_StackHandCardInfo;				//栈变量

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	int GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//获取类型结果
	int GetType(tagAnalyseResult AnalyseResult,const BYTE cbCardData[],BYTE cbCardCount,const BYTE NoChangeCard[]);

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//得到好牌
	void GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT]) ;
	//删除好牌
	bool RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount) ;
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//随机扑克
	BYTE GetRandomCard(void) ;

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//获取赖子
	BYTE GetLaiZiCard();
	//赖子数目
	BYTE GetLaiZiCount(const BYTE cbHandCardData[], BYTE cbHandCardCount);
	//设置赖子
	void SetLaiZiCard(BYTE bLaiZiCard){m_bLaiZiCard=GetCardValue(bLaiZiCard);};

	//内部函数
public:
	//分析扑克
	void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//分析比较
	bool AnalysebCompare(const BYTE cbCardData[],BYTE cbCardCount,tagAnalyseResult & AnalyseResult,int dCardType);
	//牌组转换	(将32张牌的使用情况转化为一个32位整型)
	int TransListToInt(int iOldType,int iNewType);
	//变换后扑克
	void ChangeCard(int iType,BYTE cbNextCard[],BYTE cbNextCount,BYTE bChangeCard[]);

};


#endif
