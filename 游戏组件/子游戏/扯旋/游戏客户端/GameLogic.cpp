#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE CGameLogic::m_cbCardListData[32]=
{
	//0x4E,0x23,0x08,0x22,
	//0x29,0x2A,0x3A,0x28,
	//0x24,0x14,0x25,0x02,
	//0x34,0x0C,0x38,0x37,
	//0x1A,0x35,0x0A,0x04,
	//0x16,0x26,0x36,0x06,0x1B,
	//0x07,0x17,0x27,0x18,0x2C,
	//0x39,0x3B,
	
	0x02,0x22,0x23,0x04,0x14,0x24,0x34,
	0x25,0x35,0x06,0x16,0x26,0x36,
	0x07,0x17,0x27,0x37,0x08,0x18,0x28,0x38,
	0x29,0x39,0x0A,0x1A,0x2A,0x3A,0x1B,0x3B,0x0C,0x2C,
	0x4E,
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
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount==2);
	if(cbCardCount!=2)return D2H_VALUE0;

	//丁二皇型
	if(cbCardData[0]==0x23 && cbCardData[1]==0x4E)return D2H_D2H;
	if(cbCardData[1]==0x23 && cbCardData[0]==0x4E)return D2H_D2H;

	//对牌组型
	BYTE cbCardValue[]=
		{0x0c,0x02,0x08,0x04,
		0x14,0x16,0x1a,
		0x06,0x07,0x0a,0x1b,
		0x25,0x17,0x18,0x29};
	BYTE cbCardValueType[]=
		{D2H_DOBLUE_Q,D2H_DOBLUE_2,D2H_DOBLUE_8,D2H_DOBLUE_4,
		D2H_DOBLUE_46A,D2H_DOBLUE_46A,D2H_DOBLUE_46A,
		D2H_DOBLUE_67AJ,D2H_DOBLUE_67AJ,D2H_DOBLUE_67AJ,D2H_DOBLUE_67AJ,
		D2H_DOBLUE_9875,D2H_DOBLUE_9875,D2H_DOBLUE_9875,D2H_DOBLUE_9875};

	//查找对牌
	for(WORD j=0;j<CountArray(cbCardValue);j++)
	{
		BYTE cbValueData = GetCardValue(cbCardValue[j]);
		BYTE cbValueColor = (GetCardColor(cbCardValue[j])>>4);

		//清色对牌
		if(cbValueColor!=2)
		{
			for(WORD i=0;i<cbCardCount;i++)
			{
				BYTE cbColor = (GetCardColor(cbCardData[i])>>4);
				if(GetCardValue(cbCardData[i])!=cbValueData || cbColor%2!=cbValueColor)break;
			}
			if(i==cbCardCount)return cbCardValueType[j];
		}
		//混色对牌
		else
		{
			for(WORD i=0;i<cbCardCount;i++)
			{
				if(GetCardValue(cbCardData[i])!=cbValueData)break;
			}
			if(i==cbCardCount)
			{
				BYTE cbColor1 = (GetCardColor(cbCardData[0])>>4);
				BYTE cbColor2 = (GetCardColor(cbCardData[1])>>4);
				if(cbColor1%2!=cbColor2%2)return cbCardValueType[j];
			}
		}
	}

	//天王牌型
	BYTE cbFirstCard=GetCardValue(cbCardData[0]);
	BYTE cbSecondCard=GetCardValue(cbCardData[1]);
	if(cbFirstCard==12 && cbSecondCard==9)return D2H_TH;
	if(cbSecondCard==12 && cbFirstCard==9)return D2H_TH;

	//天杠牌型
	if(cbFirstCard==12 && cbSecondCard==8)return D2H_TG;
	if(cbSecondCard==12 && cbFirstCard==8)return D2H_TG;

	//地杠牌型
	if(cbFirstCard==2 && cbSecondCard==8)return D2H_DG;
	if(cbSecondCard==2 && cbFirstCard==8)return D2H_DG;

	//点数牌型
	BYTE cbTotalValue=0;
	for(BYTE i=0;i<cbCardCount;i++)cbTotalValue+=GetCardLogicValue(cbCardData[i]);
	if(cbTotalValue>=10)cbTotalValue%=10;

	return cbTotalValue;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
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
				cbCardData[i+1]=cbTempData;
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
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardColor==0x40)?(6):bCardValue;
}

//特殊换牌
void CGameLogic::ChangeSpeCard(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbSpeCount=2;
	BYTE cbCardType = GetCardType(&cbCardData[cbSpeCount],cbSpeCount);
	if(cbCardType==D2H_D2H)return;

	//查找牌型
	BYTE cbThree=0xff,cbKing=0xff;
	for(BYTE i=0;i<MAX_COUNT;i++)
	{
		if(cbThree==0xff && cbCardData[i]==0x23)cbThree=i;
		if(cbKing==0xff && cbCardData[i]==0x4e)cbKing=i;
	}

	//最大牌型
	if(cbThree!=0xff && cbKing!=0xff)
	{
		BYTE cbCount1=0,cbCount2=0;
		BYTE cbTempData1[MAX_COUNT];
		BYTE cbTempData2[MAX_COUNT];
		ZeroMemory(cbTempData1,sizeof(cbTempData1));
		ZeroMemory(cbTempData2,sizeof(cbTempData2));

		for(WORD i=0;i<MAX_COUNT;i++)
		{
			if(cbThree!=i && cbKing!=i)cbTempData1[cbCount1++]=cbCardData[i];
			else cbTempData2[cbCount2++]=cbCardData[i];
		}

		CopyMemory(cbCardData,cbTempData1,sizeof(BYTE)*2);
		CopyMemory(&cbCardData[cbSpeCount],cbTempData2,sizeof(BYTE)*2);
	}

	return;
}

//特殊数值
BYTE CGameLogic::GetSpeCardValue(BYTE cbCardData[], BYTE cbCardCount)
{
	//特殊数值
	BYTE cbCardValue[]=
		{0x0c,0x02,0x08,0x04,
		0x14,0x16,0x1a,
		0x06,0x07,0x0a,0x1b,
		0x25,0x17,0x18,0x29,
		0x0E,0x23};
	BYTE cbCardValueType[]=
		{9,8,7,6,
		5,5,5,
		4,4,4,4,
		3,3,3,3,
		2,1};

	//查找数值
	BYTE cbAddValue = 0;
	for(WORD j=0;j<CountArray(cbCardValue);j++)
	{
		BYTE cbValueData = GetCardValue(cbCardValue[j]);
		BYTE cbValueColor = (GetCardColor(cbCardValue[j])>>4);

		//清色对牌
		if(cbValueColor!=2)
		{
			for(WORD i=0;i<cbCardCount;i++)
			{
				BYTE cbColor = (GetCardColor(cbCardData[i])>>4);
				if(GetCardValue(cbCardData[i])==cbValueData && cbColor%2==cbValueColor)
				{
					cbAddValue=cbCardValueType[j];
					break;
				}
			}
		}
		//混色对牌
		else
		{
			for(WORD i=0;i<cbCardCount;i++)
			{
				if(GetCardValue(cbCardData[i])!=cbValueData)continue;

				cbAddValue=cbCardValueType[j];
				break;
			}
		}

		//查找结束
		if(cbAddValue!=0)break;
	}

	return cbAddValue;
}

//对比扑克
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount==2);
	if(cbCardCount!=2)return 0xff;

	//特殊对比
	if(cbFirstData[0]==0 && cbNextData[0]!=0)return 0xff;
	if(cbFirstData[0]!=0 && cbNextData[0]==0)return TRUE;
	if(cbFirstData[0]==0 && cbNextData[0]==0)return FALSE;

	//获取牌型
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//数值比较
	if(cbNextType<D2H_DG && cbFirstType<D2H_DG)
	{
		if(cbNextType!=cbFirstType)return (cbFirstType>cbNextType)?TRUE:0xff;

		if(cbNextType!=0)cbNextType+=GetSpeCardValue(cbNextData,cbCardCount);
		if(cbFirstType!=0)cbFirstType+=GetSpeCardValue(cbFirstData,cbCardCount);
	}

	if(cbFirstType==cbNextType)return FALSE;
	return (cbFirstType>cbNextType)?TRUE:0xff;
}

//////////////////////////////////////////////////////////////////////////
