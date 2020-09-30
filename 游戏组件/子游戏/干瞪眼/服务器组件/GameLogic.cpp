#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量
//索引变量
const BYTE cbIndexCount=5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
		0x4E,0x4F,
};

const BYTE	CGameLogic::m_cbGoodcardData[GOOD_CARD_COUTN]=
{
	0x01,0x02,
		0x11,0x12,
		0x21,0x22,
		0x31,0x32,
		0x4E,0x4F,
		0x07,0x08,0x09,
		0x17,0x18,0x19,
		0x27,0x28,0x29,
		0x37,0x38,0x39,
		0x0A,0x0B,0x0C,0x0D,
		0x1A,0x1B,0x1C,0x1D,
		0x2A,0x2B,0x2C,0x2D,
		0x3A,0x3B,0x3C,0x3D
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//AI变量
	m_lBankerOutCardCount = 0 ;
	m_bCanOutSingleKing=false;
}

//析构函数
CGameLogic::~CGameLogic()
{
#ifdef _CHECK_DUG
	file.Close() ;
#endif
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return CT_ERROR;
		}
	case 1: //单牌
		{
			if(GetCardColor(cbCardData[0])==0x40) 
			{
				if(m_bCanOutSingleKing==true) return CT_SINGLE;
				return CT_ERROR;
			}
			return CT_SINGLE;
		}
	case 2:	//对牌火箭
		{
			BYTE bValue1=GetCardValue(cbCardData[0]);
			BYTE bValue2=GetCardValue(cbCardData[1]);

			if(bValue1==0x0F && bValue2==0x0E)	return CT_BOMB_KING;

			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

			if(bValue1==0x0E || bValue1==0x0F) return CT_DOUBLE;

			return CT_ERROR;
		}
	case 3://炸弹
		{
			BYTE bValue1=GetCardValue(cbCardData[0]);
			BYTE bValue2=GetCardValue(cbCardData[1]);
			BYTE bValue3=GetCardValue(cbCardData[2]);

			//硬炸弹
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[2])) return CT_BOMB_3A;	

			//软炸弹
			if(bValue1==0x0E || bValue1==0x0F )
			{
				if(bValue2==0x0E || bValue2==0x0F) return CT_BOMB_3;
				if(bValue2==bValue3) return CT_BOMB_3;
			}
			break;
		}
	case 4://氢弹
		{
			BYTE bValue1=GetCardValue(cbCardData[0]);
			BYTE bValue2=GetCardValue(cbCardData[1]);
			BYTE bValue3=GetCardValue(cbCardData[2]);
			BYTE bValue4=GetCardValue(cbCardData[3]);

			//硬氢弹
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[3])) return CT_BOMB_4A;	

			//软氢弹
			if(bValue1==0x0E || bValue1==0x0F)
			{
				if( bValue2==0x0E || bValue2==0x0F)
				{
					if(bValue3==bValue4) return CT_BOMB_4;
				}
				if(bValue2==bValue4) return CT_BOMB_4;
			}
			break;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//炸弹判断
	if ( 4 < cbCardCount && cbCardCount <= 8 )
	{
		BYTE bColor1=GetCardColor(cbCardData[0]);
		BYTE bColor2=GetCardColor(cbCardData[1]);
		//牌型判断
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==5) && bColor1==0x40) return CT_BOMB_KING;
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==6) && bColor1==0x40 && bColor2==0x40) return CT_BOMB_KING;
	}

	//两张类型
	if (AnalyseResult.cbDoubleCount>=2)
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbDoubleCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbDoubleCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbDoubleCardData[i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbDoubleCount*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbSignedCount>=3)&&(AnalyseResult.cbSignedCount==cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbSignedCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbSignedCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbSignedCardData[i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

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

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//分析扑克
		BYTE cbIndex=0;
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//拷贝四牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbFourCardData,sizeof(BYTE)*AnalyseResult.cbFourCount*4);
		cbIndex+=AnalyseResult.cbFourCount*4;

		//拷贝三牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbThreeCardData,sizeof(BYTE)*AnalyseResult.cbThreeCount*3);
		cbIndex+=AnalyseResult.cbThreeCount*3;

		//拷贝对牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbDoubleCardData,sizeof(BYTE)*AnalyseResult.cbDoubleCount*2);
		cbIndex+=AnalyseResult.cbDoubleCount*2;

		//拷贝单牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbSignedCardData,sizeof(BYTE)*AnalyseResult.cbSignedCount);
		cbIndex+=AnalyseResult.cbSignedCount;
	}

	return;
}
//得到好牌
void CGameLogic::GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT])
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbGoodcardData)];
	BYTE cbCardBuffer[CountArray(m_cbGoodcardData)];
	CopyMemory(cbCardData,m_cbGoodcardData,sizeof(m_cbGoodcardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	BYTE cbBufferCount=CountArray(m_cbGoodcardData);
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	//复制好牌
	CopyMemory(cbGoodCardData, cbCardBuffer, NORMAL_COUNT) ;
}

//删除好牌
bool CGameLogic::RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount) 
{
	//检验数据
	ASSERT(cbGoodCardCount<=cbCardCount);
	if(cbGoodCardCount>cbCardCount)
		return false ;

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[FULL_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbGoodCardCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbGoodcardData[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbGoodCardCount) ;
	if (cbDeleteCount!=cbGoodCardCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}
//分析分布
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//设置变量
	ZeroMemory(&Distributing,sizeof(Distributing));

	//设置变量
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//获取属性
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}
//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);
	if(cbRemoveCount>cbCardCount)
		return false ;

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

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//获取属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	if(cbCardData==0) return 0;
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);
	ASSERT(cbCardValue>0 && cbCardValue<=(MASK_VALUE&0x4f)) ;

	//转换数值
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//类型判断
	if (cbNextType==CT_ERROR) return false;

	//炸弹判断
	if ((cbFirstType<CT_BOMB_3)&&(cbNextType>=CT_BOMB_3)) return true;
	if ((cbFirstType>=CT_BOMB_3)&&(cbNextType<CT_BOMB_3)) return false;

	//炸弹比较
	if((cbFirstType>=CT_BOMB_3)&&(cbNextType>=CT_BOMB_3))
	{
		if(cbFirstType!=cbNextType) return cbNextType>cbFirstType;
		if(cbNextCount!=cbFirstCount) return cbNextCount>cbFirstCount;
	}

	if(cbFirstCount!=cbNextCount) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE://单牌
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//对比扑克
			return (cbNextLogicValue==cbFirstLogicValue+1|| ((cbNextLogicValue>cbFirstLogicValue)&&cbNextLogicValue==15));
		}
	case CT_DOUBLE://对子
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[1]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[1]);

			bool bFirstKing=(GetCardColor(cbFirstCard[0])==0x40?true:false);
			bool bNextKing=(GetCardColor(cbNextCard[0])==0x40?true:false);

			//同点的时候比较王
			if(cbNextLogicValue==cbFirstLogicValue)
			{
				//都是王对
				if(bNextKing==true && bFirstKing==true) return false;
				//先出的王对
				if(bFirstKing==true && bNextKing==false) return true;
				//后出的王对
				if(bFirstKing==false && bNextKing==true) return false;
				//都不是王对
				if(bFirstKing==false && bNextKing==false) return false;
			}

			//对比扑克
			return (cbNextLogicValue==cbFirstLogicValue+1|| ((cbNextLogicValue>cbFirstLogicValue)&&cbNextLogicValue==15));
		}
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[cbNextCount-1]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[cbFirstCount-1]);

			//对比扑克
			return cbNextLogicValue==cbFirstLogicValue+1;
		}
	case CT_BOMB_3:
	case CT_BOMB_3A:
	case CT_BOMB_4:
	case CT_BOMB_4A:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[cbNextCount-1]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[cbFirstCount-1]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	default:
		return false;

	}

	return false;
}

//分析扑克
bool CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
		if(cbLogicValue<=0) 
			return false;

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				BYTE cbIndex=AnalyseResult.cbSignedCount++;
				AnalyseResult.cbSignedCardData[cbIndex*cbSameCount]=cbCardData[i];
				break;
			}
		case 2:		//两张
			{
				BYTE cbIndex=AnalyseResult.cbDoubleCount++;
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				break;
			}
		case 3:		//三张
			{
				BYTE cbIndex=AnalyseResult.cbThreeCount++;
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				break;
			}
		case 4:		//四张
			{
				BYTE cbIndex=AnalyseResult.cbFourCount++;
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				break;
			}
		case 5:		//五张
			{
				BYTE cbIndex=AnalyseResult.cbFiveCount++;
				AnalyseResult.cbFiveCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
				break;
			}
		case 6:		//六张
			{
				BYTE cbIndex=AnalyseResult.cbSixCount++;
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
				AnalyseResult.cbSixCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
				break;
			}
		case 7:		//七张
			{
				BYTE cbIndex=AnalyseResult.cbSevenCount++;
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
				AnalyseResult.cbSevenCardData[cbIndex*cbSameCount+6]=cbCardData[i+6];
				break;
			}
		case 8:		//八张
			{
				BYTE cbIndex=AnalyseResult.cbEightCount++;
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+6]=cbCardData[i+6];
				AnalyseResult.cbEightCardData[cbIndex*cbSameCount+7]=cbCardData[i+7];
				break;
			}
		}

		//设置索引
		i+=cbSameCount-1;
	}

	return true;
}
//随机扑克
BYTE CGameLogic::GetRandomCard(void)
{
	size_t cbIndex = rand()%(sizeof(m_cbCardData)) ;
	return m_cbCardData[cbIndex] ;
}
//变幻扑克
bool CGameLogic::MagicCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT], BYTE cbTurnCardData[MAX_COUNT],BYTE cbTurnCardCount)
{
	//检测是否需要变牌//只需要变顺子
	BYTE bTurnType=CT_ERROR;
	if(cbTurnCardCount>0)
	{
		bTurnType=GetCardType(cbTurnCardData,cbTurnCardCount);
		if(bTurnType!=CT_DOUBLE_LINE && bTurnType!=CT_SINGLE_LINE)
		{
			CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
			return false;		
		}
	}

	//变量定义
	BYTE cbNormalCard[MAX_COUNT];
	BYTE cbMagicCardCount=0,cbNormalCardCount=0;

	//变幻准备
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//扑克属性
		BYTE cbCardColor=GetCardColor(cbCardData[i]);

		//变幻统计
		if (cbCardColor==0x40) cbMagicCardCount++;
		else cbNormalCard[cbNormalCardCount++]=cbCardData[i];
	}

	//失败判断
	if ((cbMagicCardCount==0)||(cbNormalCardCount==0))
	{
		CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
		return false;
	}

	//自己变牌
	if(bTurnType==CT_ERROR)
	{
		BYTE bTempCard[MAX_COUNT];
		ZeroMemory(bTempCard,sizeof(bTempCard));

		//1个的变牌
		if(cbMagicCardCount==1)
		{
			for(BYTE i=14;i>0;i--) 
			{
				if(i==14) bTempCard[0]=1;
				else bTempCard[0]=i;
				CopyMemory(&bTempCard[1],cbNormalCard,sizeof(BYTE)*cbNormalCardCount);
				SortCardList(bTempCard,cbCardCount,ST_ORDER);
				if(GetCardType(bTempCard,cbCardCount)!=CT_ERROR)
				{
					CopyMemory(cbResultCard,bTempCard,sizeof(BYTE)*cbCardCount);
					return true;
				}
			}
		}
		//2个的变牌
		if(cbMagicCardCount==2)
		{
			for(BYTE i=14;i>0;i--) for(BYTE j=14;j>0;j--)
			{
				if(i==14) bTempCard[0]=1;
				else bTempCard[0]=i;
				if(j==14) bTempCard[1]=1;
				else bTempCard[1]=j;
				CopyMemory(&bTempCard[2],cbNormalCard,sizeof(BYTE)*cbNormalCardCount);
				SortCardList(bTempCard,cbCardCount,ST_ORDER);
				if(GetCardType(bTempCard,cbCardCount)!=CT_ERROR)
				{
					CopyMemory(cbResultCard,bTempCard,sizeof(BYTE)*cbCardCount);
					return true;
				}
			}
		}
	}

	//获取基准数值
	BYTE cbBaseValue=GetCardLogicValue(cbTurnCardData[0]);
	//A以上的顺子最大，返回
	if(cbBaseValue==14)
	{
		CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
		return false;		
	}

	//分析分布
	tagDistributing Distributing;
	AnalysebDistributing(cbNormalCard,cbNormalCardCount,Distributing);
	//移动A数据
	CopyMemory(Distributing.cbDistributing[13],Distributing.cbDistributing[0],sizeof(Distributing.cbDistributing[13]));

	//跟随出单顺
	if(bTurnType==CT_SINGLE_LINE)
	{
		BYTE bNeedCount=0;
		//检查需要变的个数
		for(BYTE i=cbBaseValue;i>(cbBaseValue-cbTurnCardCount);i--)
		{
			if(Distributing.cbDistributing[i][5]==0) bNeedCount++;
		}
		//不够变的或者有多
		if(bNeedCount!=cbMagicCardCount)
		{
			CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
			return false;
		}
		//构造结果
		CopyMemory(cbResultCard,cbNormalCard,sizeof(BYTE)*cbNormalCardCount);
		BYTE bTempCount=cbNormalCardCount;

		//增加变牌
		for(BYTE i=cbBaseValue;i>(cbBaseValue-cbTurnCardCount);i--)
		{
			if(Distributing.cbDistributing[i][5]==0)
			{
				if(i==13) cbResultCard[bTempCount++]=1;
				else cbResultCard[bTempCount++]= i+1;
			}
		}
		SortCardList(cbResultCard,cbTurnCardCount,ST_ORDER);
		return true;
	}

	//跟出对牌
	if(bTurnType==CT_DOUBLE_LINE)
	{
		BYTE bTempCard[MAX_COUNT];
		ZeroMemory(bTempCard,sizeof(bTempCard));

		//1个的变牌
		if(cbMagicCardCount==1)
		{
			for(BYTE i=14;i>0;i--) 
			{
				if(i==14) bTempCard[0]=1;
				else bTempCard[0]=i;
				CopyMemory(&bTempCard[1],cbNormalCard,sizeof(BYTE)*cbNormalCardCount);
				SortCardList(bTempCard,cbCardCount,ST_ORDER);
				if(CompareCard(cbTurnCardData,bTempCard,cbTurnCardCount,cbCardCount)==true)
				{
					CopyMemory(cbResultCard,bTempCard,sizeof(BYTE)*cbCardCount);
					return true;
				}
			}
		}
		//2个的变牌
		if(cbMagicCardCount==2)
		{
			for(BYTE i=14;i>0;i--) for(BYTE j=14;j>0;j--)
			{
				if(i==14) bTempCard[0]=1;
				else bTempCard[0]=i;
				if(j==14) bTempCard[1]=1;
				else bTempCard[1]=j;
				CopyMemory(&bTempCard[2],cbNormalCard,sizeof(BYTE)*cbNormalCardCount);
				SortCardList(bTempCard,cbCardCount,ST_ORDER);
				if(CompareCard(cbTurnCardData,bTempCard,cbTurnCardCount,cbCardCount)==true)
				{
					CopyMemory(cbResultCard,bTempCard,sizeof(BYTE)*cbCardCount);
					return true;
				}
			}
		}
	}

	//设置扑克
	CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));

	return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//首出牌
	if(cbTurnCardCount==0)
	{
		OutCardResult.cbCardCount=1;
		OutCardResult.cbResultCard[0]=cbHandCardData[cbHandCardCount-1];

		return true;
	}

	//跟随出牌
	BYTE bTurnType=GetCardType(cbTurnCardData,cbTurnCardCount);
	BYTE bTurnLogicValue=GetCardLogicValue(cbTurnCardData[0]);

	switch(bTurnType)
	{
	case CT_SINGLE:
		{
			//寻找大一点的牌
			for(BYTE i=0;i<cbHandCardCount;i++)
			{
				if(GetCardLogicValue(cbHandCardData[i])==bTurnLogicValue+1 && GetCardColor(cbHandCardData[i])!=0x40 )
				{
					OutCardResult.cbCardCount=1;
					OutCardResult.cbResultCard[0]=cbHandCardData[i];
					return true;
				}
			}
			//寻找2
			for(BYTE i=0;i<cbHandCardCount;i++)
			{
				if(GetCardLogicValue(cbHandCardData[i])==15 && bTurnLogicValue!=15)
				{
					OutCardResult.cbCardCount=1;
					OutCardResult.cbResultCard[0]=cbHandCardData[i];
					return true;
				}
			}
			break;
		}
	case CT_DOUBLE:
		{
			bTurnLogicValue=GetCardLogicValue(cbTurnCardData[1]);
			//取得索引
			BYTE bOutIndex=GetCardValue(cbTurnCardData[1])-1;
			//跳过2
			if(bOutIndex==1) break;

			//分析手牌
			tagDistributing Distribute;
			AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
			//移动A
			CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));

			//王对
			bool bKingDouble=false;
			if(GetCardColor(cbTurnCardData[0])==0x40) bKingDouble=true;
			if(bKingDouble)
			{
				//搜索相邻
				if(Distribute.cbDistributing[bOutIndex+1][5]>=2 && (bOutIndex)!=14)
				{
					OutCardResult.cbCardCount=0;
					for(BYTE j=0;j<4;j++)
					{
						if(Distribute.cbDistributing[bOutIndex][j]>0)
						{			
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(bOutIndex,j);
							if(OutCardResult.cbCardCount==2) return true;
						}					
					}
				}
				//搜索对2
				if(Distribute.cbDistributing[1][5]>=2)
				{
					OutCardResult.cbCardCount=0;
					for(BYTE j=0;j<4;j++)
					{
						if(Distribute.cbDistributing[1][j]>0)
						{			
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(1,j);
							if(OutCardResult.cbCardCount==2) return true;
						}					
					}
				}	
			}

			//搜索相邻
			if(Distribute.cbDistributing[bOutIndex+1][5]>=2 && (bOutIndex+1)!=14)
			{
				OutCardResult.cbCardCount=0;
				for(BYTE j=0;j<4;j++)
				{
					if(Distribute.cbDistributing[bOutIndex+1][j]>0)
					{			
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(bOutIndex+1,j);
						if(OutCardResult.cbCardCount==2) return true;
					}					
				}
			}
			//搜索对2
			if(Distribute.cbDistributing[1][5]>=2)
			{
				OutCardResult.cbCardCount=0;
				for(BYTE j=0;j<4;j++)
				{
					if(Distribute.cbDistributing[1][j]>0)
					{			
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(1,j);
						if(OutCardResult.cbCardCount==2) return true;
					}					
				}
			}
			//搜索变牌
			if( (GetCardColor(cbHandCardData[0])==0x40) && Distribute.cbDistributing[bOutIndex+1][5]==1 && bOutIndex!=13)
			{

				for(BYTE j=0;j<4;j++)
				{
					if(Distribute.cbDistributing[bOutIndex+1][j]>0)
					{			
						if((bOutIndex+1)==13) OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(0,j);
						else OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(bOutIndex+1,j);
						break;
					}					
				}
				//增加变牌
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
				if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true) return true;
			}
			//搜索变牌对2
			if( (GetCardColor(cbHandCardData[0])==0x40) && Distribute.cbDistributing[1][5]==1)
			{
				if(Distribute.cbDistributing[14][5]>0)
				{
					OutCardResult.cbCardCount=0;
					for(BYTE j=0;j<4;j++)
					{
						if(Distribute.cbDistributing[1][j]>0)
						{			
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(1,j);
						}					
					}

					//增加变牌
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
					if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true) return true;
				}
			}
			break;
		}
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
		{
			//取得当前牌索引
			BYTE bIndex=GetCardValue(cbTurnCardData[0])-1;
			//A封顶
			if(bIndex==0) break;

			//分析手牌
			tagDistributing Distribute;
			AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
			//移动A
			CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
			//清除王
			ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

			//查找相邻
			BYTE bLinkCount=0;
			BYTE bCellCount=1;
			if(bTurnType==CT_SINGLE_LINE) bLinkCount=cbTurnCardCount;
			if(bTurnType==CT_DOUBLE_LINE) {bLinkCount=cbTurnCardCount/2;bCellCount=2;}

			//开始位置
			BYTE bStartIndex=bIndex-bLinkCount+2;
			if(bStartIndex<3) break;

			//判断是否存在连牌
			bool bHaveLink=true;
			for(BYTE i=bStartIndex;i<bStartIndex+bLinkCount;i++)
			{
				if(Distribute.cbDistributing[i][5]<bCellCount) 
				{
					bHaveLink=false;
					break;
				}
			}
			//存在连牌
			if(bHaveLink==true)
			{
				OutCardResult.cbCardCount=0;
				for(BYTE i=bStartIndex;i<bStartIndex+bLinkCount;i++)
				{
					BYTE bGetCellCount=0;
					for(BYTE j=0;j<4;j++)
					{
						if(Distribute.cbDistributing[i][j]>0)
						{			
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
							bGetCellCount++;
							if(bGetCellCount==bCellCount) break;
						}					
					}
				}
				if(OutCardResult.cbCardCount==cbTurnCardCount) return true;
			}
			break;
		}
	default:
		break;

	}

	//搜索3个的软炸弹
	if(bTurnType<=CT_BOMB_3)
	{
		//取得当前牌索引
		BYTE bIndex=GetCardValue(cbTurnCardData[0])-1;

		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		BYTE bKingCount=0;
		if(GetCardColor(cbHandCardData[0])==0x40) bKingCount=1;
		if(GetCardColor(cbHandCardData[1])==0x40) bKingCount=2;
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		if(bKingCount>0)
		{
			for(BYTE i=0;i<14;i++)
			{
				if(Distribute.cbDistributing[i][5]==2)
				{
					if(i>bIndex || bTurnType<CT_BOMB_3)
					{
						for(BYTE j=0;j<4;j++)
						{
							for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
							{
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
							}					
						}
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
						if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;	
					}
				}
			}	
		}
	}

	//搜索3个的硬炸弹
	if(bTurnType<=CT_BOMB_3A)
	{
		//取得当前牌索引
		BYTE bIndex=GetCardValue(cbTurnCardData[0])-1;

		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		for(BYTE i=0;i<14;i++)
		{
			if(Distribute.cbDistributing[i][5]==3)
			{
				if(i>bIndex || bTurnType<CT_BOMB_3)
				{
					for(BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
						{
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
						}					
					}
					if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
				}
			}
		}		
	}

	//搜索4个的软炸弹
	if(bTurnType<=CT_BOMB_4A)
	{
		//取得当前牌索引
		BYTE bIndex=GetCardValue(cbTurnCardData[0])-1;

		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		BYTE bKingCount=0;
		if(GetCardColor(cbHandCardData[0])==0x40) bKingCount=1;
		if(GetCardColor(cbHandCardData[1])==0x40) bKingCount=2;
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		if(bKingCount==1)
		{
			for(BYTE i=0;i<14;i++)
			{
				if(Distribute.cbDistributing[i][5]==3)
				{
					if(i>bIndex || bTurnType<CT_BOMB_4)
					{
						for(BYTE j=0;j<4;j++)
						{
							for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
							{
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
							}					
						}
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
						if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
					}
				}
			}
		}
	}

	//搜索4个的硬炸弹
	if(bTurnType<=CT_BOMB_4A)
	{
		//取得当前牌索引
		BYTE bIndex=GetCardValue(cbTurnCardData[0])-1;

		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		for(BYTE i=0;i<14;i++)
		{
			if(Distribute.cbDistributing[i][5]>=4)
			{
				if(i>bIndex || bTurnType<CT_BOMB_4)
				{
					for(BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
						{
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
						}					
					}
					if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
				}
			}
		}
	}

	//搜索双王炸弹
	if(cbHandCardCount>=2 &&  cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		if(bTurnType<CT_BOMB_KING)
		{
			OutCardResult.cbCardCount=2;
			OutCardResult.cbResultCard[0]=cbHandCardData[0];
			OutCardResult.cbResultCard[1]=cbHandCardData[1];
			if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
		}
	}	

	//搜索双王炸弹 5个的
	if(bTurnType==CT_BOMB_KING) 
	{
		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		BYTE bKingCount=0;
		if(GetCardColor(cbHandCardData[0])==0x40) bKingCount=1;
		if(GetCardColor(cbHandCardData[1])==0x40) bKingCount=2;
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		if(bKingCount>=1)
		{
			for(BYTE i=0;i<14;i++)
			{
				if(Distribute.cbDistributing[i][5]==4)
				{
					if(bTurnType<CT_BOMB_KING)
					{
						for(BYTE j=0;j<4;j++)
						{
							for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
							{
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
							}					
						}
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
						if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
					}
				}
			}
		}
	}

	//搜索双王炸弹 6个的
	if(bTurnType==CT_BOMB_KING) 
	{
		//分析手牌
		tagDistributing Distribute;
		AnalysebDistributing(cbHandCardData,cbHandCardCount,Distribute);
		//移动A
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));
		//清除王
		BYTE bKingCount=0;
		if(GetCardColor(cbHandCardData[0])==0x40) bKingCount=1;
		if(GetCardColor(cbHandCardData[1])==0x40) bKingCount=2;
		ZeroMemory(Distribute.cbDistributing[14],sizeof(Distribute.cbDistributing[14]));

		if(bKingCount==2)
		{
			for(BYTE i=0;i<14;i++)
			{
				if(Distribute.cbDistributing[i][5]==4)
				{
					if( bTurnType<CT_BOMB_KING)
					{
						for(BYTE j=0;j<4;j++)
						{
							for (BYTE k=0;k<Distribute.cbDistributing[i][j];k++)
							{
								OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=MakeCardData(i,j);
							}					
						}
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[0];
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbHandCardData[1];
						if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)) return true;
					}
				}
			}
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameLogic::SetCanOutSingleKing(bool bCanOut)
{
	m_bCanOutSingleKing=bCanOut;
	return;
}
