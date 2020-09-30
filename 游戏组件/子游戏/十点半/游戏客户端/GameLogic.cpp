#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE CGameLogic::m_cbCardListData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4e,0x4f
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
FLOAT CGameLogic::GetCardGenre( const BYTE cbCardData[], BYTE cbCardCount)
{
	FLOAT fCardScore = CT_ERROR;

	for( BYTE i = 0; i < cbCardCount; i++ )
	{
		fCardScore += GetCardLogicValue(cbCardData[i]);
	}

	//爆牌
	if( fCardScore > 10.5 ) return CT_ERROR;

	//大天王
	if( cbCardCount == MAX_COUNT && fCardScore == 10.5 )
		return CT_DA_TIAN_WANG;
	//九小
	if( cbCardCount == MAX_COUNT )
		return CT_JIU_XIAO;
	//八小
	if( cbCardCount == MAX_COUNT-1 )
		return CT_BA_XIAO;
	//七小
	if( cbCardCount == MAX_COUNT-2 )
		return CT_QI_XIAO;
	//六小
	if( cbCardCount == MAX_COUNT-3 )
		return CT_LIU_XIAO;
	//天王
	if( cbCardCount == MAX_COUNT-4 && fCardScore == 10.5 )
		return CT_TIAN_WANG;

	if( cbCardCount == MAX_COUNT-4 )
	{
		for( BYTE i = 0; i < cbCardCount; i++ )
			if( GetCardLogicValue(cbCardData[i]) != 0.5 ) break;
		//人五小
		if( i == cbCardCount ) return CT_REN_WU_XIAO;
		//五小
		return CT_WU_XIAO;
	}

	return fCardScore;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	FLOAT cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	FLOAT cbTempData;
	BYTE bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=(BYTE)cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//逻辑数值
FLOAT CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//转换数值
	BYTE cbCardValue=GetCardValue(cbCardData);

	FLOAT fLogicValue;
	if( cbCardValue <= 0x0a )
		fLogicValue = cbCardValue;
	else fLogicValue = 0.5;

	return fLogicValue;
}

//对比扑克
BOOL CGameLogic::CompareCard( const BYTE cbFirstCardData[], BYTE cbFirstCardCount, const BYTE cbNextCardData[], BYTE cbNextCardCount )
{
	FLOAT fFirst = GetCardGenre(cbFirstCardData,cbFirstCardCount);
	FLOAT fNext = GetCardGenre(cbNextCardData,cbNextCardCount);

	if( fFirst != fNext ) return fNext>fFirst;

	//两者都爆牌，则庄家胜
	if( fNext == CT_ERROR ) return true;

	return cbNextCardCount>=cbFirstCardCount;
}

//////////////////////////////////////////////////////////////////////////
