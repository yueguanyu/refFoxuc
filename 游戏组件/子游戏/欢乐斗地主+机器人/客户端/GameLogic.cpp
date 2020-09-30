#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
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
			return CT_SINGLE;
		}
	case 2:	//对牌火箭
		{
			//牌型判断
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return CT_MISSILE_CARD;
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//四牌判断
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//牌型判断
		if ((AnalyseResult.cbBlockCount[3]==1) && (cbCardCount==4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3]==1) && (cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;

		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//连牌判断
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//变量定义
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//连牌判断
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//牌形判断
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbBlockCount[1]>=3)
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbBlockCount[0]>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//排序权位
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex=0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//提取扑克
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//拷贝扑克
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//设置索引
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
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

//删除扑克
bool CGameLogic::RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
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

//排列扑克
VOID CGameLogic::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//获取牌型
	BYTE cbCardType = GetCardType(cbCardData,cbCardCount);

	if( cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO )
	{
		//分析牌
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

		cbCardCount = AnalyseResult.cbBlockCount[2]*3;
		CopyMemory( cbCardData,AnalyseResult.cbCardData[2],sizeof(BYTE)*cbCardCount );
		for( int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i-- )
		{
			if( i == 2 ) continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory( &cbCardData[cbCardCount],AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i] );
				cbCardCount += (i+1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}
	else if( cbCardType == CT_FOUR_TAKE_ONE || cbCardType == CT_FOUR_TAKE_TWO )
	{
		//分析牌
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

		cbCardCount = AnalyseResult.cbBlockCount[3]*4;
		CopyMemory( cbCardData,AnalyseResult.cbCardData[3],sizeof(BYTE)*cbCardCount );
		for( int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i-- )
		{
			if( i == 3 ) continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory( &cbCardData[cbCardCount],AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i] );
				cbCardCount += (i+1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}

	return;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

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
	if (cbNextType==CT_MISSILE_CARD) return true;

	//炸弹判断
	if ((cbFirstType!=CT_BOMB_CARD)&&(cbNextType==CT_BOMB_CARD)) return true;
	if ((cbFirstType==CT_BOMB_CARD)&&(cbNextType!=CT_BOMB_CARD)) return false;

	//规则判断
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[3][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[3][0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}
	
	return false;
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//分析扑克
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//设置索引
		i+=cbSameCount-1;
	}

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

//出牌搜索
BYTE CGameLogic::SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, 
							   tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	ASSERT( pSearchCardResult != NULL );
	if( pSearchCardResult == NULL ) return 0;

	ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//变量定义
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//获取类型
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//提取各种牌型一组
			ASSERT( pSearchCardResult );
			if( !pSearchCardResult ) return 0;

			//是否一手出完
			if( GetCardType(cbCardData,cbCardCount) != CT_ERROR )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],cbCardData,
					sizeof(BYTE)*cbCardCount );
				cbResultCount++;
			}

			//如果最小牌不是单牌，则提取
			BYTE cbSameCount = 0;
			if( cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount-1]) == GetCardValue(cbCardData[cbCardCount-2]) )
			{
				cbSameCount = 1;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount-1];
				BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount-1]);
				for( int i = cbCardCount-2; i >= 0; i-- )
				{
					if( GetCardValue(cbCardData[i]) == cbCardValue )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
					}
					else break;
				}

				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
				cbResultCount++;
			}

			//单牌
			BYTE cbTmpCount = 0;
			if( cbSameCount != 1 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,1,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//对牌
			if( cbSameCount != 2 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,2,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//三条
			if( cbSameCount != 3 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,3,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//三带一单
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,1,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//三带一对
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,2,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//单连
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,1,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//连对
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,2,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//三连
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,3,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			////飞机
			//cbTmpCount = SearchThreeTwoLine( cbCardData,cbCardCount,&tmpSearchCardResult );
			//if( cbTmpCount > 0 )
			//{
			//	pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			//	CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
			//		sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
			//	cbResultCount++;
			//}

			//炸弹
			if( cbSameCount != 4 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,4,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//搜索火箭
			if ((cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
			{
				//设置结果
				pSearchCardResult->cbCardCount[cbResultCount] = 2;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
				pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

				cbResultCount++;
			}

			pSearchCardResult->cbSearchCount = cbResultCount;
			return cbResultCount;
		}
	case CT_SINGLE:					//单牌类型
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
		{
			//变量定义
			BYTE cbReferCard=cbTurnCardData[0];
			BYTE cbSameCount = 1;
			if( cbTurnOutType == CT_DOUBLE ) cbSameCount = 2;
			else if( cbTurnOutType == CT_THREE ) cbSameCount = 3;

			//搜索相同牌
			cbResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,pSearchCardResult );

			break;
		}
	case CT_SINGLE_LINE:		//单连类型
	case CT_DOUBLE_LINE:		//对连类型
	case CT_THREE_LINE:				//三连类型
		{
			//变量定义
			BYTE cbBlockCount = 1;
			if( cbTurnOutType == CT_DOUBLE_LINE ) cbBlockCount = 2;
			else if( cbTurnOutType == CT_THREE_LINE ) cbBlockCount = 3;

			BYTE cbLineCount = cbTurnCardCount/cbBlockCount;

			//搜索边牌
			cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTurnCardData[0],cbBlockCount,cbLineCount,pSearchCardResult );

			break;
		}
	case CT_THREE_TAKE_ONE:	//三带一单
	case CT_THREE_TAKE_TWO:	//三带一对
		{
			//效验牌数
			if( cbCardCount < cbTurnCardCount ) break;

			//如果是三带一或三带二
			if( cbTurnCardCount == 4 || cbTurnCardCount == 5 )
			{
				BYTE cbTakeCardCount = cbTurnOutType==CT_THREE_TAKE_ONE?1:2;

				//搜索三带牌型
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,cbTakeCardCount,pSearchCardResult );
			}
			else
			{
				//变量定义
				BYTE cbBlockCount = 3;
				BYTE cbLineCount = cbTurnCardCount/(cbTurnOutType==CT_THREE_TAKE_ONE?4:5);
				BYTE cbTakeCardCount = cbTurnOutType==CT_THREE_TAKE_ONE?1:2;

				//搜索连牌
				BYTE cbTmpTurnCard[MAX_COUNT] = {};
				CopyMemory( cbTmpTurnCard,cbTurnCardData,sizeof(BYTE)*cbTurnCardCount );
				SortOutCardList( cbTmpTurnCard,cbTurnCardCount );
				cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTmpTurnCard[0],cbBlockCount,cbLineCount,pSearchCardResult );

				//提取带牌
				bool bAllDistill = true;
				for( BYTE i = 0; i < cbResultCount; i++ )
				{
					BYTE cbResultIndex = cbResultCount-i-1;

					//变量定义
					BYTE cbTmpCardData[MAX_COUNT];
					BYTE cbTmpCardCount = cbCardCount;

					//删除连牌
					CopyMemory( cbTmpCardData,cbCardData,sizeof(BYTE)*cbCardCount );
					VERIFY( RemoveCard( pSearchCardResult->cbResultCard[cbResultIndex],pSearchCardResult->cbCardCount[cbResultIndex],
						cbTmpCardData,cbTmpCardCount ) );
					cbTmpCardCount -= pSearchCardResult->cbCardCount[cbResultIndex];

					//分析牌
					tagAnalyseResult  TmpResult = {};
					AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

					//提取牌
					BYTE cbDistillCard[MAX_COUNT] = {};
					BYTE cbDistillCount = 0;
					for( BYTE j = cbTakeCardCount-1; j < CountArray(TmpResult.cbBlockCount); j++ )
					{
						if( TmpResult.cbBlockCount[j] > 0 )
						{
							if( j+1 == cbTakeCardCount && TmpResult.cbBlockCount[j] >= cbLineCount )
							{
								BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
								CopyMemory( cbDistillCard,&TmpResult.cbCardData[j][(cbTmpBlockCount-cbLineCount)*(j+1)],
									sizeof(BYTE)*(j+1)*cbLineCount );
								cbDistillCount = (j+1)*cbLineCount;
								break;
							}
							else
							{
								for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
								{
									BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
									CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],
										sizeof(BYTE)*cbTakeCardCount );
									cbDistillCount += cbTakeCardCount;

									//提取完成
									if( cbDistillCount == cbTakeCardCount*cbLineCount ) break;
								}
							}
						}

						//提取完成
						if( cbDistillCount == cbTakeCardCount*cbLineCount ) break;
					}

					//提取完成
					if( cbDistillCount == cbTakeCardCount*cbLineCount )
					{
						//复制带牌
						BYTE cbCount = pSearchCardResult->cbCardCount[cbResultIndex];
						CopyMemory( &pSearchCardResult->cbResultCard[cbResultIndex][cbCount],cbDistillCard,
							sizeof(BYTE)*cbDistillCount );
						pSearchCardResult->cbCardCount[cbResultIndex] += cbDistillCount;
					}
					//否则删除连牌
					else
					{
						bAllDistill = false;
						pSearchCardResult->cbCardCount[cbResultIndex] = 0;
					}
				}

				//整理组合
				if( !bAllDistill )
				{
					pSearchCardResult->cbSearchCount = cbResultCount;
					cbResultCount = 0;
					for( BYTE i = 0; i < pSearchCardResult->cbSearchCount; i++ )
					{
						if( pSearchCardResult->cbCardCount[i] != 0 )
						{
							tmpSearchCardResult.cbCardCount[cbResultCount] = pSearchCardResult->cbCardCount[i];
							CopyMemory( tmpSearchCardResult.cbResultCard[cbResultCount],pSearchCardResult->cbResultCard[i],
								sizeof(BYTE)*pSearchCardResult->cbCardCount[i] );
							cbResultCount++;
						}
					}
					tmpSearchCardResult.cbSearchCount = cbResultCount;
					CopyMemory( pSearchCardResult,&tmpSearchCardResult,sizeof(tagSearchCardResult) );
				}
			}

			break;
		}
	case CT_FOUR_TAKE_ONE:		//四带两单
	case CT_FOUR_TAKE_TWO:		//四带两双
		{
			BYTE cbTakeCount = cbTurnOutType==CT_FOUR_TAKE_ONE?1:2;

			BYTE cbTmpTurnCard[MAX_COUNT] = {};
			CopyMemory( cbTmpTurnCard,cbTurnCardData,sizeof(BYTE)*cbTurnCardCount );
			SortOutCardList( cbTmpTurnCard,cbTurnCardCount );

			//搜索带牌
			cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTmpTurnCard[0],4,cbTakeCount,pSearchCardResult );

			break;
		}
	}

	//搜索炸弹
	if ((cbCardCount>=4)&&(cbTurnOutType!=CT_MISSILE_CARD))
	{
		//变量定义
		BYTE cbReferCard = 0;
		if (cbTurnOutType==CT_BOMB_CARD) cbReferCard=cbTurnCardData[0];

		//搜索炸弹
		BYTE cbTmpResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,4,&tmpSearchCardResult );
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}
	}

	//搜索火箭
	if (cbTurnOutType!=CT_MISSILE_CARD&&(cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
	{
		//设置结果
		pSearchCardResult->cbCardCount[cbResultCount] = 2;
		pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
		pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

		cbResultCount++;
	}

	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//同牌搜索
BYTE CGameLogic::SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
								tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//分析扑克
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

	BYTE cbReferLogicValue = cbReferCard==0?0:GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount-1;
	do
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbBlockIndex+1);
			if( GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue )
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//复制扑克
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],&AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE) );
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	}while( cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) );

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//带牌类型搜索(三带一，四带一等)
BYTE CGameLogic::SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, 
									tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//效验
	ASSERT( cbSameCount == 3 || cbSameCount == 4 );
	ASSERT( cbTakeCardCount == 1 || cbTakeCardCount == 2 );
	if( cbSameCount != 3 && cbSameCount != 4 )
		return cbResultCount;
	if( cbTakeCardCount != 1 && cbTakeCardCount != 2 )
		return cbResultCount;

	//长度判断
	if( cbSameCount == 4 && cbHandCardCount<cbSameCount+cbTakeCardCount*2 ||
		cbHandCardCount < cbSameCount+cbTakeCardCount )
		return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//搜索同张
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,&SameCardResult );

	if( cbSameCardResultCount > 0 )
	{
		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//需要牌数
		BYTE cbNeedCount = cbSameCount+cbTakeCardCount;
		if( cbSameCount == 4 ) cbNeedCount += cbTakeCardCount;

		//提取带牌
		for( BYTE i = 0; i < cbSameCardResultCount; i++ )
		{
			bool bMerge = false;

			for( BYTE j = cbTakeCardCount-1; j < CountArray(AnalyseResult.cbBlockCount); j++ )
			{
				for( BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++ )
				{
					//从小到大
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j]-k-1)*(j+1);

					//过滤相同牌
					if( GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]) )
						continue;

					//复制带牌
					BYTE cbCount = SameCardResult.cbCardCount[i];
					CopyMemory( &SameCardResult.cbResultCard[i][cbCount],&AnalyseResult.cbCardData[j][cbIndex],
						sizeof(BYTE)*cbTakeCardCount );
					SameCardResult.cbCardCount[i] += cbTakeCardCount;

					if( SameCardResult.cbCardCount[i] < cbNeedCount ) continue;

					if( pSearchCardResult == NULL ) return 1;

					//复制结果
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],SameCardResult.cbResultCard[i],
						sizeof(BYTE)*SameCardResult.cbCardCount[i] );
					pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
					cbResultCount++;

					bMerge = true;

					//下一组合
					break;
				}

				if( bMerge ) break;
			}
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//连牌搜索
BYTE CGameLogic::SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
									tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//定义变量
	BYTE cbLessLineCount = 0;
	if( cbLineCount == 0 )
	{
		if( cbBlockCount == 1 )
			cbLessLineCount = 5;
		else if( cbBlockCount == 2 )
			cbLessLineCount = 3;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if( cbReferCard != 0 )
	{
		ASSERT( GetCardLogicValue(cbReferCard)-cbLessLineCount>=2 );
		cbReferIndex = GetCardLogicValue(cbReferCard)-cbLessLineCount+1;
	}
	//超过A
	if( cbReferIndex+cbLessLineCount > 14 ) return cbResultCount;

	//长度判断
	if( cbHandCardCount < cbLessLineCount*cbBlockCount ) return cbResultCount;

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//分析扑克
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData,cbCardCount,Distributing);

	//搜索顺子
	BYTE cbTmpLinkCount = 0;
	for (BYTE cbValueIndex=cbReferIndex;cbValueIndex<13;cbValueIndex++)
	{
		//继续判断
		if ( Distributing.cbDistributing[cbValueIndex][cbIndexCount]<cbBlockCount )
		{
			if( cbTmpLinkCount < cbLessLineCount )
			{
				cbTmpLinkCount=0;
				continue;
			}
			else cbValueIndex--;
		}
		else 
		{
			cbTmpLinkCount++;
			//寻找最长连
			if( cbLineCount == 0 ) continue;
		}

		if( cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//复制扑克
			BYTE cbCount = 0;
			for( BYTE cbIndex = cbValueIndex+1-cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++ )
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if( cbLineCount != 0 )
			{
				cbTmpLinkCount--;
			}
			else 
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	//特殊顺子
	if( cbTmpLinkCount >= cbLessLineCount-1 && cbValueIndex == 13 )
	{
		if( Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//复制扑克
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for( BYTE cbIndex = cbValueIndex-cbTmpLinkCount; cbIndex < 13; cbIndex++ )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}
			//复制A
			if( Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(0,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//设置变量
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//搜索飞机
BYTE CGameLogic::SearchThreeTwoLine( const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult )
{
	//设置结果
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//变量定义
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//搜索连牌
	cbTmpResultCount = SearchLineCardType( cbHandCardData,cbHandCardCount,0,3,0,&tmpSearchResult );

	if( cbTmpResultCount > 0 )
	{
		//提取带牌
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//变量定义
			BYTE cbTmpCardData[MAX_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//不够牌
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3 )
			{
				BYTE cbNeedDelCount = 3;
				while( cbHandCardCount+cbNeedDelCount-tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 )
					cbNeedDelCount += 3;
				//不够连牌
				if( (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 < 2 )
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard( tmpSearchResult.cbResultCard[i],cbNeedDelCount,tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i] );
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if( pSearchCardResult == NULL ) return 1;

			//删除连牌
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
			VERIFY( RemoveCard( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],
				cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//组合飞机
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory( tmpSingleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
			CopyMemory( &tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount-cbNeedCount],sizeof(BYTE)*cbNeedCount );
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbNeedCount;

			//不够带翅膀
			if( cbTmpCardCount < tmpSearchResult.cbCardCount[i]/3*2 )
			{
				BYTE cbNeedDelCount = 3;
				while( cbTmpCardCount+cbNeedDelCount-tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3*2 )
					cbNeedDelCount += 3;
				//不够连牌
				if( (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 < 2 )
				{
					//废除连牌
					continue;
				}

				//拆分连牌
				RemoveCard( tmpSearchResult.cbResultCard[i],cbNeedDelCount,tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i] );
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//重新删除连牌
				CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
				VERIFY( RemoveCard( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],
					cbTmpCardData,cbTmpCardCount ) );
				cbTmpCardCount = cbHandCardCount-tmpSearchResult.cbCardCount[i];
			}

			//分析牌
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//提取翅膀
			BYTE cbDistillCard[MAX_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;
			for( BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{
					if( j+1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( cbDistillCard,&TmpResult.cbCardData[j][(cbTmpBlockCount-cbLineCount)*(j+1)],
							sizeof(BYTE)*(j+1)*cbLineCount );
						cbDistillCount = (j+1)*cbLineCount;
						break;
					}
					else
					{
						for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],
								sizeof(BYTE)*2 );
							cbDistillCount += 2;

							//提取完成
							if( cbDistillCount == 2*cbLineCount ) break;
						}
					}
				}

				//提取完成
				if( cbDistillCount == 2*cbLineCount ) break;
			}

			//提取完成
			if( cbDistillCount == 2*cbLineCount )
			{
				//复制翅膀
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//复制结果
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for( BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}

//////////////////////////////////////////////////////////////////////////////////
