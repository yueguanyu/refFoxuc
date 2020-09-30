#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//索引变量
const BYTE cbIndexCount=5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	//0x4E,0x4F,
	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	//0x4E,0x4F,
	0x01,0x11,0x21,0x31,0x01,0x11,0x21,0x31,0x02,0x12,0x22,0x22,0x32,0x02,0x12,0x32,
	0x03,0x13,0x23,0x33,0x03,0x13,0x23,0x33,0x04,0x14,0x24,0x24,0x34,0x04,0x14,0x34,
	0x05,0x15,0x25,0x35,0x05,0x15,0x25,0x35,0x06,0x4E,0x16,0x26,0x26,0x36,0x06,0x16,
	0x36,0x07,0x17,0x27,0x37,0x07,0x17,0x27,0x37,0x08,0x18,0x28,0x28,0x38,0x08,0x18,
	0x38,0x4E,0x09,0x19,0x29,0x39,0x09,0x19,0x29,0x39,0x0A,0x1A,0x2A,0x2A,0x4F,0x3A,
	0x0A,0x1A,0x3A,0x0B,0x1B,0x2B,0x3B,0x0B,0x1B,0x2B,0x3B,0x0C,0x1C,0x2C,0x2C,0x3C,
	0x0C,0x1C,0x3C,0x0D,0x0D,0x1D,0x1D,0x2D,0x2D,0x3D,0x3D,0x4F,
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



//查看是否连续 可用于检查对子，同三张，姐妹对，连同三张,不带单牌的炸弹和顺子等牌型
bool CGameLogic::IsStructureLink(tagAnalyseResult & AnalyseResult,int cbSameCardNumber,int cbLinkLong)
{
	//如果牌的种类(除去大小王后)大于允许的种类数  
	if(AnalyseResult.m_cbCardStyleNumber>cbLinkLong)
		return false;

	if(AnalyseResult.m_cbCardStyleNumber==0)
	{
		AnalyseResult.m_MinCardValue=GetCardLogicValue(0x41);
	}
	else
	{
		int nOutIndex=AnalyseResult.m_cbCardStyleNumber-1;
		int nInIndex=0;
		int  nDifference=0;
		AnalyseResult.m_MinCardValue=GetCardLogicValue(AnalyseResult.m_CardStyle[nOutIndex].m_cbCard);
		BYTE nBeforeCard=GetCardLogicValue(AnalyseResult.m_CardStyle[nOutIndex].m_cbCard);

		if(GetCardLogicValue(AnalyseResult.m_CardStyle[0].m_cbCard)!=15)  //查看2是否出现在组合中
		{
			for(;nOutIndex>=0;nOutIndex--) //按照预期长度进行查找
			{
				//开始按照牌数进行补牌操作
				nDifference=cbSameCardNumber-AnalyseResult.m_CardStyle[nOutIndex].m_cbCounter;
				if(nDifference<0)   //如果选牌中同牌面得纸牌多余需求的则返回错误
				{
					return false;
				}
				else
				{
					if(nDifference<=AnalyseResult.m_cbJockerNumber)
					{
						AnalyseResult.m_CardStyle[nOutIndex].m_cbCounter+=nDifference;
						AnalyseResult.m_cbJockerNumber-=nDifference;
					}
					else
						return false;
				}
				//补牌操作结束



				if(nOutIndex!=AnalyseResult.m_cbCardStyleNumber-1)
				{
					int nResult=GetCardLogicValue(AnalyseResult.m_CardStyle[nOutIndex].m_cbCard)-nBeforeCard;
					if(nResult==1) //判断是否连续
					{
						nBeforeCard=GetCardLogicValue(AnalyseResult.m_CardStyle[nOutIndex].m_cbCard);
					}
					else           //如果不连续则进行填牌操作
					{
						if((nResult-1)*cbSameCardNumber<=AnalyseResult.m_cbJockerNumber)
						{
							AnalyseResult.m_cbJockerNumber-=(nResult-1)*cbSameCardNumber;
							nBeforeCard=GetCardLogicValue(AnalyseResult.m_CardStyle[nOutIndex].m_cbCard);
						}
						else
							return false;
					}
				}
			}


			//如果填补牌完毕且发现还有变牌，则将其加载到该组合的队首或队尾
			if(AnalyseResult.m_cbJockerNumber>0)
			{
				if(AnalyseResult.m_cbJockerNumber%cbSameCardNumber==0)
				{
					int k=0;
					for(k=0;k<AnalyseResult.m_cbJockerNumber/cbSameCardNumber;k++)
					{
						if((GetCardLogicValue(AnalyseResult.m_CardStyle[0].m_cbCard)+k)<GetCardValue(0x31)) 
							continue;
						else
						{
							AnalyseResult.m_MinCardValue-=1;
						}
					}
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

BYTE CGameLogic::GetNoSameCard(const BYTE cbCardData[], int cbCardCount,int&nCardCount,int&nKingCount)
{
	
}

BYTE CGameLogic::GetCardType(const BYTE cbCardData[], int cbCardCount,tagAnalyseResult & AnalyseResult)
{

	tagAnalyseResult bkupAnalyse;
	AnalysebCardData(cbCardData,cbCardCount,bkupAnalyse);
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));

	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return CT_ERROR;
		}
	case 1: //单牌
		{
			AnalyseResult.m_MinCardValue=GetCardLogicValue(cbCardData[0]);
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			if (IsStructureLink(AnalyseResult,2,1))
				return CT_DOUBLE;
			return CT_ERROR;
		}
	case 3:	//三条
		{
			if (IsStructureLink(AnalyseResult,3,1)) 
			{
				if(GetCardLogicValue(AnalyseResult.m_CardStyle[0].m_cbCard)==14)
					return CT_BOMB;
				else
					return CT_THREE;
			}
			return CT_ERROR;
		}
	}

	//分析扑克
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));
	if((cbCardCount==4||cbCardCount==5)&&IsBomb(AnalyseResult,cbCardCount))
		return CT_BOMB;

	//单连类型
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult)); 
	if (cbCardCount>=5&&IsStructureLink(AnalyseResult,1,cbCardCount))
		return CT_SINGLE_LINK;

	//对连类型
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));
	if(cbCardCount%2==0&&cbCardCount>=4&&IsStructureLink(AnalyseResult,2,cbCardCount/2))
		return CT_DOUBLE_LINK;

	//三连类型
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));
	if(cbCardCount%3==0&&cbCardCount>=6&&IsStructureLink(AnalyseResult,3,cbCardCount/3))
		return CT_THREE_LINK;

	//连炸类型
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));
	if(cbCardCount%3==0&&cbCardCount>=6&&IsStructureLink(AnalyseResult,3,cbCardCount/3))
		return CT_THREE_LINK;


	//三带二型
	memcpy(&AnalyseResult,&bkupAnalyse,sizeof(tagAnalyseResult));
	if (cbCardCount==5&&ThreeDoubleLink(AnalyseResult))
		return CT_THREE_DOUBLE_LINK;

	return CT_ERROR;

}


//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount,BYTE &cbStarLevel)
{
	//变量初始化
	cbStarLevel=0;

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
	}

	//排炸类型
	if (cbCardCount>=12)
	{
		//变量定义
		BYTE cbCardIndex=0;
		BYTE cbBlockCount=0;
		BYTE cbBombCardData[MAX_COUNT/4];
		BYTE cbSmallCount=0;
		BYTE cbMaxCount=0;
		ZeroMemory(cbBombCardData,sizeof(cbBombCardData));

		//扑克分析
		do
		{
			//变量定义
			BYTE cbSameCount=1;
			BYTE cbCardValue=GetCardLogicValue(cbCardData[cbCardIndex]);

			//同牌搜索
			for (BYTE i=cbCardIndex+1;i<cbCardCount;i++)
			{
				if (GetCardLogicValue(cbCardData[i])==cbCardValue)
				{
					cbSameCount++;
				}
				else break;
			}

			//连牌判断
			if (cbSameCount>=4)
			{
				cbBombCardData[cbBlockCount]=cbCardData[cbCardIndex];
				cbBlockCount++;
				cbCardIndex+=cbSameCount;
				if(cbSmallCount==0)
					cbSmallCount=cbSameCount;
				else
					cbSmallCount=min(cbSameCount,cbSmallCount);
				if(cbMaxCount==0)
					cbMaxCount=cbSameCount;
				else
					cbMaxCount=max(cbMaxCount,cbSameCount);

			}
			else break;

		} while (cbCardIndex<cbCardCount);

		//结果判断
		if ((cbBlockCount>=3)&&(cbCardIndex==cbCardCount)&&(cbCardCount%cbBlockCount==0)&&IsStructureLink(cbCardData,cbCardCount,cbCardCount/cbBlockCount,true)&&(cbSmallCount==cbMaxCount)) 
		{
			cbStarLevel=cbBlockCount+cbCardCount/cbBlockCount;
			return CT_BOMB_LINK;
		}
		if((cbBlockCount>=3)&&(cbCardIndex==cbCardCount)&&(IsStructureLink(cbBombCardData,cbBlockCount,1,true)))
		{
			cbStarLevel=cbBlockCount+cbSmallCount;
			return CT_BOMB_LINK;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);
	//3王炸弹
	if ((cbCardCount==3)&&(cbCardData[0]==0x4E)&&(cbCardData[2]==0x4E))
	{
		cbStarLevel= 6; 
		return CT_BOMB_3W;
	}

	//同牌判断
	if ((cbCardCount==3)&&(AnalyseResult.cbBlockCount[2]==1)) return CT_THREE; //3张同牌
	if ((cbCardCount==2)&&(AnalyseResult.cbBlockCount[1]==1)) return CT_DOUBLE;//对子牌型


	//天王炸弹
	if ((cbCardCount==4)&&(cbCardData[0]==0x4E)&&(cbCardData[3]==0x4E)&&(AnalyseResult.cbBlockCount[3]==1))
	{
		cbStarLevel= 7;//4王炸弹7星级 最大
		return CT_BOMB_TW;
	}
	//炸弹类型
	if ((cbCardCount>=4)&&(cbCardCount<=10)&&(AnalyseResult.cbBlockCount[cbCardCount-1]==1)) 
	{
		cbStarLevel= cbCardCount;
		return CT_BOMB;
	}

	//对连类型
	if ((cbCardCount>=6)&&((AnalyseResult.cbBlockCount[1]*2)==cbCardCount))
	{
		BYTE cbDoubleCount=AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1],cbDoubleCount,2)==true) return CT_DOUBLE_LINK;
	}

	//三连类型
	if ((cbCardCount>=9)&&((AnalyseResult.cbBlockCount[2]*3)==cbCardCount))
	{
		BYTE cbThreeCount=AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2],cbThreeCount,3)==true) return CT_THREE_LINK;
	}

	//顺子类型
	if ((cbCardCount>=5)&&AnalyseResult.cbBlockCount[0]==cbCardCount)
	{
		if (IsStructureLink(AnalyseResult.cbCardData[0],cbCardCount,1)==true) return CT_SHUNZI;
	}

	return CT_ERROR;
}



//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbSortType==ST_VALUE) 
		{
			cbSortValue[i]=GetCardValue(cbCardData[i]);
		}
		else 
		{
			cbSortValue[i]=GetCardLogicValue(cbCardData[i]);
		}
	}

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
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount,bool bSecond)
{
	
	
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//工作目录
	TCHAR m_szWorkDirectory[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),m_szWorkDirectory,sizeof(m_szWorkDirectory));
	int nModuleLen=lstrlen(m_szWorkDirectory);
	TCHAR m_szDllName[MAX_PATH];
	CopyMemory(m_szDllName,AfxGetApp()->m_pszExeName,sizeof(TCHAR)*MAX_PATH);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	m_szWorkDirectory[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(m_szWorkDirectory);

	//读取扑克
	TCHAR szFile[MAX_PATH]=TEXT("");
	BYTE cbPosition=0;
	_sntprintf(szFile,CountArray(szFile),TEXT("%s\\ShuangKouQB.INI"),m_szWorkDirectory);
	cbPosition=GetPrivateProfileInt(TEXT("ShuangKouQB"),TEXT("DisCardCount"),4,szFile);
	if(cbPosition>8) cbPosition=4;

	//混乱扑克
	BYTE cbRandCount=0,cbTempRandCount=0;
	if(bSecond)
	{
		do
		{
			cbTempRandCount=0;
			cbPosition=0;
			cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
			cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
			cbTempRandCount++;
			while ((cbPosition<cbBufferCount-cbRandCount) && (cbTempRandCount<4))
			{
				cbPosition++;
				cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
				cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
				cbTempRandCount++;
			}

		} while (cbRandCount<cbBufferCount);

	}
	else
	{
		do
		{
			cbTempRandCount=0;
			cbPosition=rand()%(cbBufferCount-cbRandCount);
			cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
			cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
			cbTempRandCount++;
			while ((cbPosition<cbBufferCount-cbRandCount) && (cbTempRandCount<4))
			{
				cbPosition++;
				cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
				cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
				cbTempRandCount++;
			}

		} while (cbRandCount<cbBufferCount);
	}


	//CopyMemory(cbCardBuffer,m_cbCardData,sizeof(m_cbCardData));
	
	/*
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
	*/

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
	ASSERT(cbDeleteCount==cbRemoveCount);
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
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;
	if(cbCardData==0x4F||cbCardData==0x4E) return true;
	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//王牌扑克
	if (cbCardValue>=0x0E) return cbCardValue+2;

	//转换数值
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	
	//引用变量
	BYTE cbFirstStarLevel;
	BYTE cbNextStarLevel;
	
	//类型判断
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount,cbNextStarLevel);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount,cbFirstStarLevel);

	//炸弹过虑
	if ((cbFirstType>=CT_BOMB)||(cbNextType>=CT_BOMB))
	{
		//炸弹类型
		if (cbNextType<CT_BOMB) return false;
		if (cbFirstType<CT_BOMB) return true;

		//星级比较
		if (cbFirstStarLevel != cbNextStarLevel)
		{
			return cbNextStarLevel>cbFirstStarLevel;
		}
		else
		{
			//45相比较
			if((cbFirstStarLevel<=5)&&(cbFirstStarLevel>=4))
			{
				//变量定义
				BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
				BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);
				return cbConsultNext>cbConsultFirst;
			}
			//6相比较
			if(cbFirstStarLevel==6)
			{
				//首家3王
				if(cbFirstCount==3)
				{
					return true;
				}
				//下家3王
				if(cbNextCount==3)
				{
					return false;
				}

				//变量定义
				BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
				BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);
				return cbConsultNext>cbConsultFirst;

			}
			//7相比较
			if(cbFirstStarLevel==7)
			{
				//首家天王
				if(cbFirstCount==4) //天王是7相中最大的
				{
					return false;
				}
				//下家天王
				if(cbNextCount==4)
				{
					return true;
				}
				//同异比较
				if ((cbFirstType==CT_BOMB)&&(cbNextType==CT_BOMB_LINK))
				{
					return false;

				}
				//同异比较
				if ((cbFirstType==CT_BOMB_LINK)&&(cbNextType==CT_BOMB))
				{
					return true;

				}
				//同相比较
				if ((cbFirstType==CT_BOMB)&&(cbNextType)==CT_BOMB)
				{
					//变量定义
					BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
					BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);
					return cbConsultNext>cbConsultFirst;

				}
				//连炸比较
				if((cbFirstType==CT_BOMB_LINK)&&(cbNextType==CT_BOMB_LINK))
				{

					//变量定义
					BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
					BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);

					//扑克分析
					if (cbNextType==CT_BOMB_LINK)
					{
						//扑克搜索
						for (BYTE i=0;i<cbNextCount;i++)
						{
							BYTE cbCardValue=GetCardLogicValue(cbNextCard[i]);
							if (cbCardValue<cbConsultNext) cbConsultNext=cbCardValue;
						}
					}

					//扑克分析
					if (cbFirstType==CT_BOMB_LINK)
					{

						//扑克搜索
						for (BYTE i=0;i<cbFirstCount;i++)
						{
							BYTE cbCardValue=GetCardLogicValue(cbFirstCard[i]);
							if (cbCardValue<cbConsultFirst) cbConsultFirst=cbCardValue;
						}
					}
					return cbConsultNext>cbConsultFirst;

				}

				ASSERT(FALSE);
				return false;
			}

			//8星级
			if(cbFirstStarLevel>=8)
			{
				//同异比较
				if ((cbFirstType==CT_BOMB)&&(cbNextType==CT_BOMB_LINK))
				{
					return false;

				}
				//同异比较
				if ((cbFirstType==CT_BOMB_LINK)&&(cbNextType==CT_BOMB))
				{
					return true;

				}
				//同相比较
				if ((cbFirstType==CT_BOMB)&&(cbNextType)==CT_BOMB)
				{
					//变量定义
					BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
					BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);
					return cbConsultNext>cbConsultFirst;

				}
				if((cbFirstType==CT_BOMB_LINK)&&(cbNextType==CT_BOMB_LINK))
				{

					//变量定义
					BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
					BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);


					//首牌相同
					if (cbNextType==CT_BOMB_LINK)
					{

						//扑克搜索
						for (BYTE i=0;i<cbNextCount;i++)
						{
							BYTE cbCardValue=GetCardValue(cbNextCard[i]);
							if (cbCardValue<cbConsultNext) cbConsultNext=cbCardValue;
						}
					}

					//扑克分析
					if (cbFirstType==CT_BOMB_LINK)
					{

						//扑克搜索
						for (BYTE i=0;i<cbFirstCount;i++)
						{
							BYTE cbCardValue=GetCardValue(cbFirstCard[i]);
							if (cbCardValue<cbConsultFirst) cbConsultFirst=cbCardValue;
						}
					}
					return cbConsultNext>cbConsultFirst;

				}

				ASSERT(FALSE);
				return false;
			}

		}
	}

	//不同类型
	if (cbFirstType!=cbNextType) return false;

	//相同类型
	switch (cbFirstType)
	{
	case CT_SINGLE:				//单牌类型
	case CT_DOUBLE:				//对子类型
	case CT_THREE:				//三条类型
	case CT_SHUNZI:				//顺子类型	
		{
			//数目判断
			if (cbFirstCount!=cbNextCount) return false;

			//变量定义
			BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
			BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);

			return cbConsultNext>cbConsultFirst;
		}
	case CT_DOUBLE_LINK:		//对连类型
	case CT_THREE_LINK:			//三连类型
		{
			//数目判断
			if (cbFirstCount!=cbNextCount) return false;

			//变量定义
			BYTE cbConsultNext=GetCardValue(cbNextCard[0]);
			BYTE cbConsultFirst=GetCardValue(cbFirstCard[0]);

			//数值转换
			if (cbConsultNext!=2)
			{
				//设置变量
				cbConsultNext=0;

				//扑克搜索
				for (BYTE i=0;i<cbNextCount;i++)
				{
					BYTE cbLogicValue=GetCardLogicValue(cbNextCard[i]);
					if (cbLogicValue>cbConsultNext) cbConsultNext=cbLogicValue;
				}
			}

			//数值转换
			if (cbConsultFirst!=2)
			{
				//设置变量
				cbConsultFirst=0;

				//扑克搜索
				for (BYTE i=0;i<cbFirstCount;i++)
				{
					BYTE cbLogicValue=GetCardLogicValue(cbFirstCard[i]);
					if (cbLogicValue>cbConsultFirst) cbConsultFirst=cbLogicValue;
				}
			}

			return cbConsultNext>cbConsultFirst;
		}

	}

	//错误断言
	ASSERT(FALSE);
	
	return false;
}
//对比扑克
bool CGameLogic::CompareCard(BYTE cbFirstCard, BYTE cbNextCard)
{
	//获取数值
	BYTE cbNextValue=GetCardLogicValue(cbNextCard);
	BYTE cbFirstValue=GetCardLogicValue(cbFirstCard);
	
	//比较大小
	return (cbNextValue>cbFirstValue);
}
//变幻扑克
bool CGameLogic::MagicCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT])
{
	//变量定义
	BYTE cbNormalCard[MAX_COUNT];
	BYTE cbMagicCardCount=0,cbNormalCardCount=0;

	//变幻准备
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0x4F||cbCardData[i]==0x4E) cbMagicCardCount++;
		else cbNormalCard[cbNormalCardCount++]=cbCardData[i];
	}

	//失败判断
	if ((cbMagicCardCount==0)||(cbNormalCardCount==0)) 
	{
		CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
		return false;
	}

	//获取数值
	BYTE cbLogicValue=GetCardLogicValue(cbNormalCard[0]);

	//同牌变幻
	for (BYTE i=1;i<cbNormalCardCount;i++)
	{
		if (GetCardLogicValue(cbNormalCard[i])!=cbLogicValue) break;
	}
	//特殊判断
	if(1 == cbNormalCardCount)
	{
		i=cbNormalCardCount;
	}
	if (i==cbNormalCardCount)
	{
		//设置结果
		for (BYTE j=0;j<cbMagicCardCount;j++) cbResultCard[j]=cbNormalCard[0];
		for (BYTE j=0;j<cbNormalCardCount;j++) cbResultCard[j+cbMagicCardCount]=cbNormalCard[j];
		return true;
	}

	//扑克分析
	BYTE cbMaxSameCount=1,cbSameCount=1,cbBlock=0;
	for (BYTE i=1;i<cbNormalCardCount;i++)
	{
		//获取扑克
		BYTE cbCardValueTemp=GetCardLogicValue(cbNormalCard[i]);
		if (cbCardValueTemp==cbLogicValue) cbSameCount++;

		//结果处理
		if ((cbCardValueTemp!=cbLogicValue)||(i==(cbNormalCardCount-1)))
		{
			cbMaxSameCount=__max(cbSameCount,cbMaxSameCount);
			cbBlock++;
		}

		//恢复变量
		if ((cbCardValueTemp!=cbLogicValue)&&(i!=(cbNormalCardCount-1)))
		{
			cbSameCount=1;
			cbLogicValue=cbCardValueTemp;
		}
	}

	//等同变换
	if((cbCardCount>=12)&&(cbCardCount%cbBlock==0)&&(cbBlock>=3))
	{
		//最大牌数
		BYTE cbMaxSameCount1=cbCardCount/cbBlock; 

		//连炸判断
		if(cbMaxSameCount1>=4) 
		{
			//分析分布
			tagDistributing Distributing;
			AnalysebDistributing(cbNormalCard,cbNormalCardCount,Distributing);
			
			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<13;i++)
			{
				//变量定义
				BYTE cbIndex=i;

				//填充判断
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<cbMaxSameCount1) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<cbMaxSameCount1;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,0);
					}

					//设置变量
					cbFillStatus[cbIndex]=cbMaxSameCount;
				}
			}
			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}
	//等同变换
	if((cbCardCount>=12)&&(cbCardCount%cbBlock==0)&&(cbBlock>=3))
	{
		//最大牌数
		BYTE cbMaxSameCount1=cbCardCount/cbBlock; 

		//连炸判断
		if(cbMaxSameCount1>=4) 
		{
			//分析分布
			tagDistributing Distributing;
			AnalyseRealDistributing(cbNormalCard,cbNormalCardCount,Distributing);

			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<13;i++)
			{
				//变量定义
				BYTE cbIndex=i;

				//填充判断
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<cbMaxSameCount) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeRealCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<cbMaxSameCount1;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeRealCardData(cbIndex,0);
					}

					//设置变量
					cbFillStatus[cbIndex]=cbMaxSameCount;
				}
			}
			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}

	//不等变换
	if((cbMaxSameCount>=4)&&(cbBlock>=3)&&(cbCardCount>=12)) //3连排炸
	{
		//分析分布
		tagDistributing Distributing;
		AnalysebDistributing(cbNormalCard,cbNormalCardCount,Distributing);

		//填充分析
		for (BYTE cbTimes=0;cbTimes<2;cbTimes++)
		{
			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<13;i++)
			{
				//变量定义
				BYTE cbIndex=(cbTimes==1)?(cbIndex=(13-i)%13):i;

				//填充判断  
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//过滤炸弹
					if(Distributing.cbDistributing[cbIndex][cbIndexCount]>=4) 
					{
						//记录数据
						cbFillStatus[cbIndex]=Distributing.cbDistributing[cbIndex][cbIndexCount];
					}

					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<4) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<4;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,0);
					}

					//设置变量
					if(cbTurnFillCount<4)
						cbFillStatus[cbIndex]=4;
					else
						cbFillStatus[cbIndex]=cbTurnFillCount;
				}
			}

			//剩余填充
			if ((cbLeaveCount==0)&&(cbUseableCount>0))
			{
				//现在先查找最小填充数
				BYTE cbMinFillCount=0;
				BYTE cbMaxFillCount=0;
				BYTE cbMinIndex=0;//最小索引
				for (BYTE i=0;i<13;i++)
				{
					if (cbFillStatus[i]==0) continue;//过滤没有填充的

					if(cbMinIndex==0 &&cbMinFillCount==0)
					{
						cbMinFillCount=cbFillStatus[i];
						cbMinIndex=i;
					}

					//开始寻找 取大
					if(cbFillStatus[i]<cbMinFillCount)
					{
						cbMinFillCount=cbFillStatus[i];
						cbMinIndex=i;
					}
					cbMaxFillCount=max(cbMaxFillCount,cbFillStatus[i]);
			}

				for (BYTE i=cbMinIndex;i<13;i++)
				{
					//变量定义
					BYTE cbIndex=(cbTimes==1)?(cbIndex=(13-i)%13):i;
					if (cbFillStatus[cbIndex]==0) continue;//过滤没有填充的


					//填充扑克
					for (BYTE j=cbFillStatus[cbIndex];j<cbMaxFillCount;j++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,0);
						//完成判断
						if (cbUseableCount==0) break;

					}

					//完成判断
					if (cbUseableCount==0) break;
				}
				//如果都填充满了  还有多余的王 那就从大到小的填充   毕竟王牌只有2个  一次就够了阿 
				if(cbUseableCount>0)
				{
					for (BYTE i=12;i>=0;i--)
					{
						//填充扑克
						if(cbFillStatus[i]>0)
						{
							cbUseableCount--;
							cbResultCard[cbFillCount++]=MakeCardData(i,0);
							cbFillStatus[i]++;
						}

						//完成判断
						if (cbUseableCount==0) break;
					}
				}
			}

			

			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}

	//不等变换
	if((cbMaxSameCount>=4)&&(cbBlock>=3)&&(cbCardCount>=12)) //3连排炸
	{
		//分析分布
		tagDistributing Distributing;
		AnalyseRealDistributing(cbNormalCard,cbNormalCardCount,Distributing);

		//填充分析
		for (BYTE cbTimes=0;cbTimes<2;cbTimes++)
		{
			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<13;i++)
			{
				//变量定义
				BYTE cbIndex=(cbTimes==1)?(cbIndex=(13-i)%13):i;

				//填充判断  
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//过滤炸弹
					if(Distributing.cbDistributing[cbIndex][cbIndexCount]>=4) 
					{
						//记录数据
						cbFillStatus[cbIndex]=Distributing.cbDistributing[cbIndex][cbIndexCount];
					}

					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<4) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeRealCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<4;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeRealCardData(cbIndex,0);
					}

					//设置变量
					if(cbTurnFillCount<4)
						cbFillStatus[cbIndex]=4;
					else
						cbFillStatus[cbIndex]=cbTurnFillCount;
				}
			}

			//剩余填充
			if ((cbLeaveCount==0)&&(cbUseableCount>0))
			{
				//现在先查找最小填充数
				BYTE cbMinFillCount=0;
				BYTE cbMaxFillCount=0;
				BYTE cbMinIndex=0;//最小索引
				for (BYTE i=0;i<13;i++)
				{
					if (cbFillStatus[i]==0) continue;//过滤没有填充的

					if(cbMinIndex==0 &&cbMinFillCount==0)
					{
						cbMinFillCount=cbFillStatus[i];
						cbMinIndex=i;
					}

					//开始寻找 取大
					if(cbFillStatus[i]<cbMinFillCount)
					{
						cbMinFillCount=cbFillStatus[i];
						cbMinIndex=i;
					}
					cbMaxFillCount=max(cbMaxFillCount,cbFillStatus[i]);
			}

				for (BYTE i=cbMinIndex;i<13;i++)
				{
					//变量定义
					BYTE cbIndex=(cbTimes==1)?(cbIndex=(13-i)%13):i;
					if (cbFillStatus[cbIndex]==0) continue;//过滤没有填充的


					//填充扑克
					for (BYTE j=cbFillStatus[cbIndex];j<cbMaxFillCount;j++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeRealCardData(cbIndex,0);
						//完成判断
						if (cbUseableCount==0) break;

					}

					//完成判断
					if (cbUseableCount==0) break;
				}
				//如果都填充满了  还有多余的王 那就从大到小的填充   毕竟王牌只有2个  一次就够了阿 
				if(cbUseableCount>0)
				{
					for (BYTE i=12;i>=0;i--)
					{
						//填充扑克
						if(cbFillStatus[i]>0)
						{
							cbUseableCount--;
							cbResultCard[cbFillCount++]=MakeRealCardData(i,0);
							cbFillStatus[i]++;
						}

						//完成判断
						if (cbUseableCount==0) break;
					}
				}
			}



			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}
	//连牌变幻
	if ((cbMaxSameCount<=3)&&((cbCardCount%cbMaxSameCount)==0)&&(cbCardCount/cbMaxSameCount<=13))
	{
		//分析分布
		tagDistributing Distributing;
		AnalysebDistributing(cbNormalCard,cbNormalCardCount,Distributing);

		//填充分析
		for (BYTE cbTimes=0;cbTimes<2;cbTimes++)
		{
			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<12;i++)
			{
				//变量定义
				BYTE cbIndex=(cbTimes==1)?(cbIndex=(11-i)%12):i;

				//填充判断
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<cbMaxSameCount) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<cbMaxSameCount;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,0);
					}

					//设置变量
					cbFillStatus[cbIndex]=cbMaxSameCount;
				}
			}

			//剩余填充
			if ((cbLeaveCount==0)&&(cbUseableCount>0))
			{
				for (BYTE i=0;i<12;i++)
				{
					//变量定义
					BYTE cbIndex=(cbTimes==0)?(cbIndex=(11-i)%12):i;

					//填充扑克
					for (BYTE j=cbFillStatus[cbIndex];j<cbMaxSameCount;j++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,0);
					}

					//完成判断
					if (cbUseableCount==0) break;
				}
			}

			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}
	//设置扑克
	CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));

	return false;
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
		for (BYTE j=0;j<cbSameCount;j++)
			AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

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
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbLogicValue-3][cbIndexCount]++;
		Distributing.cbDistributing[cbLogicValue-3][cbCardColor>>4]++;
	}


	return;
}

//分析分布
VOID CGameLogic::AnalyseRealDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
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
}

//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//设置结果
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount,ST_ORDER);
	
	//获取类型
	BYTE cbStarLevel;
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount,cbStarLevel);

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[cbCardCount-1]);

			//多牌判断
			BYTE cbSameCount=1;
			for (BYTE i=1;i<cbCardCount;i++)
			{
				if (GetCardLogicValue(cbCardData[cbCardCount-i-1])==cbLogicValue) cbSameCount++;
				else break;
			}

			//完成处理
			if (cbSameCount>1)
			{
				OutCardResult.cbCardCount=cbSameCount;
				for (BYTE j=0;j<cbSameCount;j++) OutCardResult.cbResultCard[j]=cbCardData[cbCardCount-1-j];
				return true;
			}

			//单牌处理
			OutCardResult.cbCardCount=1;
			OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-1];

			return true;
		}

	}
	return false;
}

//搜索顺子
bool CGameLogic::SearchLinkCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbFirstCardData,const BYTE cbLastCardData,tagOutCardResult &OutCardResult )
{
	ASSERT( IsValidCard(cbFirstCardData) && IsValidCard(cbLastCardData) );

	//获取首位
	BYTE byFirstCardValue = GetCardLogicValue(cbFirstCardData);
	BYTE byLastCardValue = GetCardLogicValue(cbLastCardData);

	//交换数据
	if( byFirstCardValue > byLastCardValue )
	{
		BYTE byTemp = byFirstCardValue;
		byFirstCardValue = byLastCardValue;
		byLastCardValue = byTemp;
	}

	//判断顺子
	if( byFirstCardValue > 14 || byLastCardValue > 14 || byLastCardValue-byFirstCardValue<4 )
		return false;

	//定义变量
	tagDistributing Distribute;
	BYTE byCardData[MAX_COUNT],byCardCount = cbHandCardCount;
	CopyMemory(byCardData,cbHandCardData,sizeof(BYTE)*byCardCount);
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//分析扑克
	AnalysebDistributing(byCardData,byCardCount,Distribute);

	//变幻准备
	BYTE cbMagicCardCount=0;
	for (BYTE i=0;i<byCardCount;i++)
	{
		if (byCardData[i]==0x4F) cbMagicCardCount++;
	}

	//构造数据
	OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = cbFirstCardData;
	OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = cbLastCardData;
	for( BYTE i = byFirstCardValue-2; i < byLastCardValue-3; i++ )
	{
		if( Distribute.cbDistributing[i][cbIndexCount] > 0 )
		{
			//获取花色
			BYTE cbHeadColor=0xFF;
			for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
			{
				if (Distribute.cbDistributing[i][3-cbColorIndex]>0)
				{
					cbHeadColor=3-cbColorIndex;
					break;
				}
			}
			ASSERT(cbHeadColor!=0xFF);
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(i,cbHeadColor);

		}
		else 
		{
			//王牌替换
			if(cbMagicCardCount>0)
			{
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = 0x4F;
				
				//王牌递减
				cbMagicCardCount--;
			}
			else
				break;
		}
	}
	if( i == byLastCardValue-3 ) 
	{

		SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount);
		return true;
	}

	//结果清零
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));
	return false;
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	//构造扑克
	switch (cbValueIndex)
	{
	case 11:	//A 扑克
		{
			return (cbColorIndex<<4)|0x01;
		}
	case 12:	//2 扑克
		{
			return (cbColorIndex<<4)|0x02;
		}

	case 13:	//小鬼扑克
		{
			return (4<<4)|0x0E;
		}
	case 14:	//大鬼扑克
		{
			return (4<<4)|0x0F;
		}
	default:	//其他扑克
		{
			return (cbColorIndex<<4)|(cbValueIndex+3);
		}
	}

	return 0x00;
}

//构造扑克
BYTE CGameLogic::MakeRealCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	//构造扑克
	switch (cbValueIndex)
	{

	case 13:	//小鬼扑克
		{
			return (4<<4)|0x0E;
		}
	case 14:	//大鬼扑克
		{
			return (4<<4)|0x0F;
		}

	default:	//其他扑克
		{
			return (cbColorIndex<<4)|(cbValueIndex+1);
		}
	}

	return 0x00;
}


//是否连牌
bool CGameLogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount,bool bSpecial)
{
	//数目判断
	ASSERT((cbCardCount%cbCellCount)==0);
	if ((cbCardCount%cbCellCount)!=0) return false;
	
	//变量定义
	BYTE cbBlockCount=cbCardCount/cbCellCount;
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	if(bSpecial)
	{
		//特殊处理  2在连炸中
		if (cbFirstValue==15)
		{
			for (BYTE i=1;i<cbBlockCount;i++)
			{
				//获取数值
				BYTE cbLogicValue=GetCardLogicValue(cbCardData[i*cbCellCount]);

				//连牌判断
				if ((cbFirstValue!=(cbLogicValue+i))&&(cbFirstValue!=(cbLogicValue+i+(15-cbBlockCount-2)))) return false;
			}

			return true;
		}
	}

	//大牌过滤
	if(cbFirstValue>14) return false;

	//分析处理
	for (BYTE i=1;i<cbBlockCount;i++)
	{
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i*cbCellCount])+i)) return false;
	}

	return true;
}

