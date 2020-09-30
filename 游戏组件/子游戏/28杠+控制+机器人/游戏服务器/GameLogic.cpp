#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
const BYTE CGameLogic::m_cbCardListData[CARD_COUNT]=
{
	   0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,	//方块 A - 10
	   0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,	//梅花 A - 10
	   0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,	//红桃 A - 10
	   0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,	//黑桃 A - 10
		
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

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
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

//获取牌型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//合法判断
	ASSERT(2==cbCardCount);
	if (2!=cbCardCount) return CT_ERROR;

	//排序扑克
	BYTE cbCardDataSort[CARD_COUNT];
	CopyMemory(cbCardDataSort,cbCardData,sizeof(BYTE)*cbCardCount);
	SortCardList(cbCardDataSort,cbCardCount,ST_LOGIC);

	//获取点数
	BYTE cbFirstCardValue=GetCardValue(cbCardDataSort[0]);
	BYTE cbSecondCardValue=GetCardValue(cbCardDataSort[1]);

	//获取花色
	BYTE cbFistCardColor=GetCardColor(cbCardDataSort[0]);
	BYTE cbSecondCardColor=GetCardColor(cbCardDataSort[1]);

	if( cbFirstCardValue == cbSecondCardValue)
		return CT_SPECIAL;
	//点数牌型
	return CT_POINT;
}

//大小比较
/*
cbNextCardData>cbFirstCardData  返回1
cbNextCardData<cbFirstCardData  返回-1
cbNextCardData==cbFirstCardData 返回0
*/
int CGameLogic::CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount)
{
	//合法判断
	ASSERT(2==cbFirstCardCount && 2==cbNextCardCount);
	if (!(2==cbFirstCardCount && 2==cbNextCardCount)) return 0;

	//获取牌型
	BYTE cbFirstCardType=GetCardType(cbFirstCardData, cbFirstCardCount);
	BYTE cbNextCardType=GetCardType(cbNextCardData, cbNextCardCount);

	//牌型比较
	if (cbFirstCardType != cbNextCardType) 
	{
		if (cbNextCardType > cbFirstCardType) return 1;
		else return -1;
	}
	else
	{
		if(CT_SPECIAL == cbFirstCardType )
		{
			BYTE bcCardTmp[2];
			BYTE bcCardTmp2[2];
			CopyMemory(bcCardTmp,cbFirstCardData,2);
			CopyMemory(bcCardTmp2,cbNextCardData,2);

			SortCardList(bcCardTmp,cbFirstCardCount,2);
			SortCardList(bcCardTmp2,cbNextCardCount,2);

			if(GetCardLogicValue(bcCardTmp[0])>=GetCardLogicValue(bcCardTmp2[0]))
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
	}

	//获取点数
	BYTE cbFirstPip=GetCardListPip(cbFirstCardData, cbFirstCardCount);
	BYTE cbNextPip=GetCardListPip(cbNextCardData, cbNextCardCount);

	//点数比较

	if (cbFirstPip != cbNextPip)
	{
		if (cbNextPip >cbFirstPip)
		{	
			return 1;
		}
		else return -1;
	}
	if(cbNextPip==cbFirstPip)
	{
		//BYTE bcCardTmp[2];
		//BYTE bcCardTmp2[2];
		//CopyMemory(bcCardTmp,cbFirstCardData,2);
		//CopyMemory(bcCardTmp2,cbNextCardData,2);

		//SortCardList(bcCardTmp,cbFirstCardCount,2);
		//SortCardList(bcCardTmp2,cbNextCardCount,2);

		//if(GetCardLogicValue(bcCardTmp[0])>=GetCardLogicValue(bcCardTmp2[0]))
		//{
		//	return -1;
		//}else
		//{
		//	return 1;
		//}

		//牌点一样大的话，算庄家胜出。
		return -1;
	}

	return 0;
}

//获取牌点
BYTE CGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	BYTE cbPipCount=0;

	//获取牌点
	BYTE cbCardValue=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		cbCardValue=GetCardValue(cbCardData[i]);
		if(cbCardValue>10)
		{
			cbCardValue = 10;
		}
		cbPipCount+=(1==cbCardValue ? 1 : cbCardValue);
	}

	return (cbPipCount%10);
}

//逻辑大小
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{

	BYTE bCardValue=GetCardValue(cbCardData);

	/*if(bCardValue>10)
	{
		bCardValue = 10;
	}*/

	//转换数值
	return (bCardValue<=1)?(bCardValue+13):bCardValue;
	
	return 0;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[CARD_COUNT];
	if (ST_VALUE==cbSortType)
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardValue(cbCardData[i]);	
	}
	else 
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	
	}

	//排序操作
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return;
}

//////////////////////////////////////////////////////////////////////////

