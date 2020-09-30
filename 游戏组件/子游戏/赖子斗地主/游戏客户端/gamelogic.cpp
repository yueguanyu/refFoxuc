#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_bLaiZiCard=0;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
int CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
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
			return TransListToInt(0,CT_SINGLE);
		}
	case 2:	//对牌火箭
		{
			//牌型判断
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return TransListToInt(0,CT_MISSILE_CARD);
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return TransListToInt(0,CT_DOUBLE);

			//赖子判断
			if(GetCardLogicValue(cbCardData[0])!=GetCardLogicValue(cbCardData[1]))
			{
				if(GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(m_bLaiZiCard) || GetCardLogicValue(cbCardData[1]) == GetCardLogicValue(m_bLaiZiCard))
				{
					if((cbCardData[0]!=0x4F) && (cbCardData[0]!=0x4E) && (cbCardData[0]!=0x4F) && (cbCardData[1]!=0x4E))
					{
						return TransListToInt(0,CT_DOUBLE);
					}
				}
			}

			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	BYTE bLaiZiCount=GetLaiZiCount(cbCardData,cbCardCount);
	BYTE bCardDataTemp[MAX_COUNT];
	int dCardType=CT_ERROR;

	memset(bCardDataTemp,0,sizeof(bCardDataTemp));
	CopyMemory(bCardDataTemp,cbCardData,sizeof(BYTE)*cbCardCount);
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//都是赖子做普通牌
	bool SameCard=false;
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(m_bLaiZiCard))
			SameCard=true;
		else
		{
			SameCard=false;
			break;
		}
	}

	//赖子判断
	if(bLaiZiCount != 0 && !SameCard)
	{
		for(BYTE i=0;i<cbCardCount;i++)
		{
			if(GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(m_bLaiZiCard))
			{
				//一张赖子
				for(BYTE j=0;j<13;j++)
				{
					bCardDataTemp[i]=m_cbCardData[j];

					//两张赖子
					if(bLaiZiCount >= 2)
					{
						for(BYTE k=i+1;k<cbCardCount;k++)
						{
							if(GetCardLogicValue(cbCardData[k]) == GetCardLogicValue(m_bLaiZiCard))
							{
								for(BYTE z=0;z<13;z++)
								{
									bCardDataTemp[k]=m_cbCardData[z];

									//三张赖子
									if(bLaiZiCount >= 3)
									{
										for(BYTE g=k+1;g<cbCardCount;g++)
										{
											if(GetCardLogicValue(cbCardData[g]) == GetCardLogicValue(m_bLaiZiCard))
											{
												for(BYTE f=0;f<13;f++)
												{
													bCardDataTemp[g]=m_cbCardData[f];

													//四张赖子
													if(bLaiZiCount == 4)
													{
														for(BYTE h=g+1;h<cbCardCount;h++)
														{
															if(GetCardLogicValue(cbCardData[h]) == GetCardLogicValue(m_bLaiZiCard))
															{
																for(BYTE l=0;l<13;l++)
																{
																	if(l == 1)continue;
																	bCardDataTemp[h]=m_cbCardData[l];
																	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

																	AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
																	if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
																		continue;

																	if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
																		dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
																}
															}
															else
															{
																ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

																AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
																if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
																	continue;

																if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
																	dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
															}
														}
													}
													ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

													AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
													if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
														continue;

													if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
														dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
												}
											}
											else
											{
												ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

												AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
												if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
													continue;

												if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
													dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
											}
										}
									}
									ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

									AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
									if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
										continue;

									if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
										dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
								}
							}
							else
							{
								ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

								AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
								if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
									continue;

								if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
									dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
							}
						}
					}
					ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

					AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
					if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
						continue;

					if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
						dCardType=TransListToInt(dCardType,GetType(AnalyseResult,cbCardData,cbCardCount,cbCardData));
				}
			}
			else
			{
				ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

				AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
				if(GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData) == 0)
					continue;

				if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
					dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
			}
		}
	}
	else
	{
		ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

		AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);

		if((dCardType&(1<<GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData))) == 0)
			dCardType=TransListToInt(dCardType,GetType(AnalyseResult,bCardDataTemp,cbCardCount,cbCardData));
	}

	return dCardType;
}

//获取类型结果
int CGameLogic::GetType(tagAnalyseResult AnalyseResult,const BYTE cbCardData[],BYTE cbCardCount,const BYTE NoChangeCard[])
{
	BYTE bLaiZiCount=GetLaiZiCount(NoChangeCard,cbCardCount);

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

			//赖子判断
			if(GetCardLogicValue(cbCardData[0])!=GetCardLogicValue(cbCardData[1]))
			{
				if(GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(m_bLaiZiCard) || GetCardLogicValue(cbCardData[1]) == GetCardLogicValue(m_bLaiZiCard))
				{
					if((cbCardData[0]!=0x4F) && (cbCardData[0]!=0x4E) && (cbCardData[0]!=0x4F) && (cbCardData[1]!=0x4E))
					{
						return CT_DOUBLE;
					}
				}
			}

			return CT_ERROR;
		}
	}

	//四牌判断
	if (AnalyseResult.cbFourCount>0)
	{
		//牌型判断
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==4)&&(bLaiZiCount == 0)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbFourCount==1)&&(bLaiZiCount==4)) return CT_LAIZI_BOMB;
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==4)&&(bLaiZiCount!=0)) return CT_RUAN_BOMB;
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbSignedCount==2)&&(cbCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbDoubleCount==2)&&(cbCardCount==8)) return CT_FOUR_LINE_TAKE_TWO;


		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbThreeCount>0)
	{
		//三条类型
		if(AnalyseResult.cbThreeCount==1 && cbCardCount==3) 
			return CT_THREE ;

		//连牌判断
		if (AnalyseResult.cbThreeCount>1)
		{
			//变量定义
			BYTE cbCardData=AnalyseResult.cbThreeCardData[0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//连牌判断
			for (BYTE i=1;i<AnalyseResult.cbThreeCount;i++)
			{
				BYTE cbCardData=AnalyseResult.cbThreeCardData[i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) 
					return CT_ERROR;
			}
		}

		//牌形判断
		if (AnalyseResult.cbThreeCount*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbThreeCount*4==cbCardCount && AnalyseResult.cbSignedCount == cbCardCount/3) return CT_THREE_LINE_TAKE_ONE;
		if ((AnalyseResult.cbThreeCount*5==cbCardCount)&&(AnalyseResult.cbDoubleCount==AnalyseResult.cbThreeCount)) return CT_THREE_LINE_TAKE_TWO;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbDoubleCount>=3)
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
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) 
				return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbDoubleCount*2)==cbCardCount) 
			return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbSignedCount>=5)&&(AnalyseResult.cbSignedCount==cbCardCount))
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
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) 
				return CT_ERROR;
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
		ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
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

	////癞子排前面
	//BYTE bTempCard=0;
	//BYTE bTempCardData[MAX_COUNT];
	//CopyMemory(bTempCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//if(m_bLaiZiCard != 0)
	//{
	//	for(BYTE i=0;i<cbCardCount;i++)
	//	{
	//		if(GetCardValue(m_bLaiZiCard) == GetCardValue(bTempCardData[i]))
	//		{
	//			cbCardData[bTempCard++]=bTempCardData[i];

	//			//重新排列
	//			BYTE bCardCount=bTempCard;
	//			for(BYTE j=0;j<cbCardCount;j++)
	//			{
	//				if(GetCardValue(m_bLaiZiCard) != GetCardValue(bTempCardData[j]))
	//				{
	//					cbCardData[bCardCount++]=bTempCardData[j];
	//				}
	//			}
	//		}
	//	}
	//}

	return;
}

//排列扑克
void CGameLogic::SortCardListEx(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
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
		ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
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

	//癞子排前面
	BYTE bTempCard=0;
	BYTE bTempCardData[MAX_COUNT];
	CopyMemory(bTempCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	if(m_bLaiZiCard != 0)
	{
		for(BYTE i=0;i<cbCardCount;i++)
		{
			if(GetCardValue(m_bLaiZiCard) == GetCardValue(bTempCardData[i]))
			{
				cbCardData[bTempCard++]=bTempCardData[i];

				//重新排列
				BYTE bCardCount=bTempCard;
				for(BYTE j=0;j<cbCardCount;j++)
				{
					if(GetCardValue(m_bLaiZiCard) != GetCardValue(bTempCardData[j]))
					{
						cbCardData[bCardCount++]=bTempCardData[j];
					}
				}
			}
		}
	}
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
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	if(cbCardData == 0)return 0;
	ASSERT(cbCardData != 0);

	//转换数值
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//获取类型
	int cbNextType=GetCardType(cbNextCard,cbNextCount);
	int cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//类型判断
	if (cbNextType==CT_ERROR) return false;
	if ((cbNextType&(1<<CT_MISSILE_CARD)) != 0) return true;

	//炸弹判断
	if((cbFirstType&(1<<CT_MISSILE_CARD)) == 0 &&(cbNextType&(1<<CT_LAIZI_BOMB)) != 0)return true;
	if((cbFirstType&(1<<CT_LAIZI_BOMB)) == 0 &&(cbNextType&(1<<CT_BOMB_CARD)) != 0)return true;
	if ((cbFirstType&(1<<CT_BOMB_CARD)) == 0 &&(cbNextType&(1<<CT_BOMB_CARD)) != 0) return true;
	

	if((cbFirstType&(1<<CT_LAIZI_BOMB)) != 0 && (cbNextType&(1<<CT_MISSILE_CARD)) == 0)return false;
	if((cbFirstType&(1<<CT_BOMB_CARD)) != 0 && (cbNextType&(1<<CT_LAIZI_BOMB)) == 0 && (cbNextType&(1<<CT_MISSILE_CARD)) == 0)return false;
	if((cbFirstType&(1<<CT_RUAN_BOMB)) != 0 && (cbNextType&(1<<CT_RUAN_BOMB)) == 0
		&& (cbNextType&(1<<CT_LAIZI_BOMB)) == 0 && (cbNextType&(1<<CT_MISSILE_CARD)) == 0)return false;

	if((cbFirstType&(1<<CT_MISSILE_CARD)) == 0 && (cbFirstType&(1<<CT_LAIZI_BOMB)) == 0 && 
		(cbFirstType&(1<<CT_BOMB_CARD)) == 0 && (cbFirstType&(1<<CT_RUAN_BOMB)) == 0 && (cbNextType&(1<<CT_RUAN_BOMB)) != 0)return true;


	//规则判断
	if (((cbFirstType&cbNextType) == 0||(cbFirstCount!=cbNextCount)) && (cbFirstType&(1<<CT_BOMB_CARD)) == 0 && (cbFirstType&(1<<CT_RUAN_BOMB)) == 0 &&
		(cbNextType&(1<<CT_BOMB_CARD)) == 0 && (cbNextType&(1<<CT_RUAN_BOMB)) == 0) return false;

	//开始对比
	if((cbNextType&(1<<CT_BOMB_CARD)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_BOMB_CARD))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_BOMB_CARD))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbFourCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbFourCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbFourCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_RUAN_BOMB)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_RUAN_BOMB))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_RUAN_BOMB))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbFourCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbFourCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbFourCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_SINGLE)) != 0 && cbFirstCount == cbNextCount)
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_SINGLE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_SINGLE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbSignedCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbSignedCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbSignedCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbSignedCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbSignedCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbSignedCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbSignedCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_DOUBLE)) != 0 && cbFirstCount == cbNextCount)
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_DOUBLE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_DOUBLE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbDoubleCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbDoubleCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbDoubleCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbDoubleCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbDoubleCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbDoubleCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbDoubleCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_THREE)) != 0 && cbFirstCount == cbNextCount) 
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_THREE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbThreeCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbThreeCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbThreeCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_SINGLE_LINE)) != 0 && cbFirstCount == cbNextCount) 
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_SINGLE_LINE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_SINGLE_LINE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbSignedCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbSignedCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbSignedCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbSignedCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbSignedCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbSignedCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbSignedCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_DOUBLE_LINE)) != 0 && cbFirstCount == cbNextCount) 
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_DOUBLE_LINE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_DOUBLE_LINE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbDoubleCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbDoubleCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbDoubleCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbDoubleCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbDoubleCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbDoubleCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbDoubleCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_THREE_LINE)) != 0 && cbFirstCount == cbNextCount)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_THREE_LINE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbThreeCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbThreeCardData[i]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[i]);
		}
		for(BYTE i=0;i<NextResult.cbThreeCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[i]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}

	else if((cbNextType&(1<<CT_THREE_LINE_TAKE_ONE)) != 0 && cbFirstCount == cbNextCount)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE_TAKE_ONE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_THREE_LINE_TAKE_ONE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbThreeCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbThreeCardData[i*3]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[i*3]);
		}
		for(BYTE i=0;i<NextResult.cbThreeCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[i*3]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_THREE_LINE_TAKE_TWO)) != 0 && cbFirstCount == cbNextCount) 
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;

		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE_TAKE_TWO))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_THREE_LINE_TAKE_TWO))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbThreeCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbThreeCardData[i*3]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[i*3]);
		}
		for(BYTE i=0;i<NextResult.cbThreeCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[i*3]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_FOUR_LINE_TAKE_ONE)) != 0 && cbFirstCount == cbNextCount)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_FOUR_LINE_TAKE_ONE))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_FOUR_LINE_TAKE_ONE))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbFourCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbFourCardData[i*4]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[i*4]);
		}
		for(BYTE i=0;i<NextResult.cbFourCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[i*4]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}
	else if((cbNextType&(1<<CT_FOUR_LINE_TAKE_TWO)) != 0 && cbFirstCount == cbNextCount)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		ZeroMemory(&NextResult,sizeof(NextResult));
		ZeroMemory(&FirstResult,sizeof(FirstResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_FOUR_LINE_TAKE_TWO))return false;
		if(!AnalysebCompare(cbFirstCard,cbFirstCount,FirstResult,CT_FOUR_LINE_TAKE_TWO))return false;

		//获取数值
		BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
		BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

		//出牌最大值
		for(BYTE i=0;i<FirstResult.cbFourCount;i++)
		{
			if(cbFirstLogicValue < GetCardLogicValue(NextResult.cbFourCardData[i*4]))
				cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[i*4]);
		}
		for(BYTE i=0;i<NextResult.cbFourCount;i++)
		{
			cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[i*4]);

			if(cbNextLogicValue>cbFirstLogicValue)
				return true;
		}
	}

	return false;
}

//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,BYTE bChangeCard[])
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
	int cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	//出牌分析
	if(cbTurnOutType == CT_ERROR)					//错误类型
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
	if((cbTurnOutType&(1<<CT_THREE)) != 0 || (cbTurnOutType&(1<<CT_DOUBLE)) != 0 || (cbTurnOutType&(1<<CT_SINGLE)) != 0)
	{
		//获取数值
		BYTE cbLogicValue=GetCardLogicValue(bChangeCard[0]);

		//分析扑克
		tagAnalyseResult AnalyseResult;
		ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//寻找单牌
		if (cbTurnCardCount<=1)
		{
			for (BYTE i=0;i<AnalyseResult.cbSignedCount;i++)
			{
				BYTE cbIndex=AnalyseResult.cbSignedCount-i-1;
				if (GetCardLogicValue(AnalyseResult.cbSignedCardData[cbIndex])>cbLogicValue)
				{
					//设置结果
					OutCardResult.cbCardCount=cbTurnCardCount;
					CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbSignedCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

					return true;
				}
			}
		}

		//寻找对牌
		if (cbTurnCardCount<=2)
		{
			for (BYTE i=0;i<AnalyseResult.cbDoubleCount;i++)
			{
				BYTE cbIndex=(AnalyseResult.cbDoubleCount-i-1)*2;

				//赖子判断
				BYTE cbLogicValueTmp=GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex]);
				if(cbLogicValueTmp != GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex+1]))
				{
					if(cbLogicValueTmp == GetCardLogicValue(m_bLaiZiCard))
						cbLogicValueTmp=GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex+1]);
				}

				if (cbLogicValueTmp > cbLogicValue)
				{
					//设置结果
					OutCardResult.cbCardCount=cbTurnCardCount;
					CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbDoubleCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

					return true;
				}
			}
		}

		//寻找三牌
		if (cbTurnCardCount<=3)
		{
			for (BYTE i=0;i<AnalyseResult.cbThreeCount;i++)
			{
				BYTE cbIndex=(AnalyseResult.cbThreeCount-i-1)*3;

				//赖子判断
				BYTE cbLogicValueTmp=GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex]);
				for(BYTE j=0;j<3;j++)
				{
					if(GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex+j]) != GetCardLogicValue(m_bLaiZiCard))
					{
						cbLogicValueTmp=GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex+j]);
					}
				}

				if (cbLogicValueTmp>cbLogicValue)
				{
					//设置结果
					OutCardResult.cbCardCount=cbTurnCardCount;
					CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbThreeCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

					return true;
				}
			}
		}
	}
	if((cbTurnOutType&(1<<CT_SINGLE_LINE)) != 0) 		//单连类型
	{
		//长度判断
		if (cbCardCount<cbTurnCardCount) return false;

		//获取数值
		BYTE cbLogicValue=GetCardLogicValue(bChangeCard[0]);

		//赖子判断
		//for(BYTE i=0;i<cbTurnCardCount;i++)
		//{
		//	if(cbLogicValue == GetCardLogicValue(m_bLaiZiCard))
		//		cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
		//	else
		//		break;
		//}

		//搜索连牌
		for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
		{
			//获取数值
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue>=15) break;
			if (cbHandLogicValue<=cbLogicValue) continue;

			//搜索连牌
			BYTE cbLineCount=0;
			for (BYTE j=(cbCardCount-i-1);j<cbCardCount;j++)
			{
				if ((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue) 
				{
					//增加连数
					OutCardResult.cbResultCard[cbLineCount++]=cbCardData[j];

					//完成判断
					if (cbLineCount==cbTurnCardCount)
					{
						OutCardResult.cbCardCount=cbTurnCardCount;
						return true;
					}
				}
			}
		}
	}
	if((cbTurnOutType&(1<<CT_DOUBLE_LINE)) != 0) 		//对连类型
	{
		//长度判断
		if (cbCardCount<cbTurnCardCount)  return false;

		//获取数值
		BYTE cbLogicValue=GetCardLogicValue(bChangeCard[0]);

		//搜索连牌
		for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
		{
			//获取数值
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue<=cbLogicValue) continue;
			if ((cbTurnCardCount>1)&&(cbHandLogicValue>=15)) break;

			//搜索连牌
			BYTE cbLineCount=0;
			for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-1);j++)
			{
				if (((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue)
					&&((GetCardLogicValue(cbCardData[j+1])+cbLineCount)==cbHandLogicValue))
				{
					//增加连数
					OutCardResult.cbResultCard[cbLineCount*2]=cbCardData[j];
					OutCardResult.cbResultCard[(cbLineCount++)*2+1]=cbCardData[j+1];

					//完成判断
					if (cbLineCount*2==cbTurnCardCount)
					{
						OutCardResult.cbCardCount=cbTurnCardCount;
						return true;
					}
				}
			}
		}
	}
	if((cbTurnOutType&(1<<CT_THREE_LINE_TAKE_TWO)) != 0 || (cbTurnOutType&(1<<CT_THREE_LINE)) != 0
		 || (cbTurnOutType&(1<<CT_THREE_LINE_TAKE_ONE)) != 0 && (cbTurnOutType&(1<<CT_RUAN_BOMB)) != 0 && 
		 (cbTurnOutType&(1<<CT_BOMB_CARD)) != 0 && (cbTurnOutType&(1<<CT_LAIZI_BOMB)) != 0)
	{
		//长度判断
		if (cbCardCount<cbTurnCardCount)  return false;

		//获取数值
		BYTE cbLogicValue=0;
		for (BYTE i=0;i<cbTurnCardCount-2;i++)
		{
			cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
			if (GetCardLogicValue(cbTurnCardData[i+1])!=cbLogicValue) continue;
			if (GetCardLogicValue(cbTurnCardData[i+2])!=cbLogicValue) continue;
			break;
		}

		//属性数值
		BYTE cbTurnLineCount=0;
		if ((cbTurnOutType&(1<<CT_THREE_LINE_TAKE_ONE)) != 0) cbTurnLineCount=cbTurnCardCount/4;
		else if ((cbTurnOutType&(1<<CT_THREE_LINE_TAKE_TWO)) != 0) cbTurnLineCount=cbTurnCardCount/5;
		else cbTurnLineCount=cbTurnCardCount/3;

		//搜索连牌
		for (BYTE i=cbTurnLineCount*3-1;i<cbCardCount;i++)
		{
			//获取数值
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue<=cbLogicValue) continue;
			if ((cbTurnLineCount>1)&&(cbHandLogicValue>=15)) break;

			//搜索连牌
			BYTE cbLineCount=0;
			for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-2);j++)
			{
				//设置变量
				OutCardResult.cbCardCount=0;

				//三牌判断
				if ((GetCardLogicValue(cbCardData[j])+cbLineCount)!=cbHandLogicValue) continue;
				if ((GetCardLogicValue(cbCardData[j+1])+cbLineCount)!=cbHandLogicValue) continue;
				if ((GetCardLogicValue(cbCardData[j+2])+cbLineCount)!=cbHandLogicValue) continue;

				//增加连数
				OutCardResult.cbResultCard[cbLineCount*3]=cbCardData[j];
				OutCardResult.cbResultCard[cbLineCount*3+1]=cbCardData[j+1];
				OutCardResult.cbResultCard[(cbLineCount++)*3+2]=cbCardData[j+2];

				//完成判断
				if (cbLineCount==cbTurnLineCount)
				{
					//连牌设置
					OutCardResult.cbCardCount=cbLineCount*3;

					//构造扑克
					BYTE cbLeftCardData[MAX_COUNT];
					BYTE cbLeftCount=cbCardCount-OutCardResult.cbCardCount;
					CopyMemory(cbLeftCardData,cbCardData,sizeof(BYTE)*cbCardCount);
					RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbLeftCardData,cbCardCount);

					//分析扑克
					tagAnalyseResult AnalyseResultLeft;
					ZeroMemory(&AnalyseResultLeft,sizeof(AnalyseResultLeft));
					AnalysebCardData(cbLeftCardData,cbLeftCount,AnalyseResultLeft);

					//单牌处理
					if ((cbTurnOutType&(1<<CT_THREE_LINE_TAKE_ONE))!= 0)
					{
						//提取单牌
						for (BYTE k=0;k<AnalyseResultLeft.cbSignedCount;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=AnalyseResultLeft.cbSignedCount-k-1;
							BYTE cbSignedCard=AnalyseResultLeft.cbSignedCardData[cbIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
						}

						//提取对牌
						for (BYTE k=0;k<AnalyseResultLeft.cbDoubleCount*2;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbDoubleCount*2-k-1);
							BYTE cbSignedCard=AnalyseResultLeft.cbDoubleCardData[cbIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
						}

						//提取三牌
						for (BYTE k=0;k<AnalyseResultLeft.cbThreeCount*3;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbThreeCount*3-k-1);
							BYTE cbSignedCard=AnalyseResultLeft.cbThreeCardData[cbIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
						}

						//提取四牌
						for (BYTE k=0;k<AnalyseResultLeft.cbFourCount*4;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbFourCount*4-k-1);
							BYTE cbSignedCard=AnalyseResultLeft.cbFourCardData[cbIndex];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
						}
					}

					//对牌处理
					if ((cbTurnOutType&(1<<CT_THREE_LINE_TAKE_TWO)) != 0)
					{
						//提取对牌
						for (BYTE k=0;k<AnalyseResultLeft.cbDoubleCount;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbDoubleCount-k-1)*2;
							BYTE cbCardData1=AnalyseResultLeft.cbDoubleCardData[cbIndex];
							BYTE cbCardData2=AnalyseResultLeft.cbDoubleCardData[cbIndex+1];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
						}

						//提取三牌
						for (BYTE k=0;k<AnalyseResultLeft.cbThreeCount;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbThreeCount-k-1)*3;
							BYTE cbCardData1=AnalyseResultLeft.cbThreeCardData[cbIndex];
							BYTE cbCardData2=AnalyseResultLeft.cbThreeCardData[cbIndex+1];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
						}

						//提取四牌
						for (BYTE k=0;k<AnalyseResultLeft.cbFourCount;k++)
						{
							//中止判断
							if (OutCardResult.cbCardCount==cbTurnCardCount) break;

							//设置扑克
							BYTE cbIndex=(AnalyseResultLeft.cbFourCount-k-1)*4;
							BYTE cbCardData1=AnalyseResultLeft.cbFourCardData[cbIndex];
							BYTE cbCardData2=AnalyseResultLeft.cbFourCardData[cbIndex+1];
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
						}
					}

					//完成判断
					if (OutCardResult.cbCardCount==cbTurnCardCount) return true;
				}
			}
		}
	}

	//搜索炸弹
	if ((cbCardCount>=4)&&(cbTurnOutType&(1<<CT_MISSILE_CARD)) == 0)
	{
		//变量定义
		BYTE cbLogicValue=0;
		if ((cbTurnOutType&(1<<CT_BOMB_CARD)) != 0) cbLogicValue=GetCardLogicValue(bChangeCard[0]);
		if ((cbTurnOutType&(1<<CT_RUAN_BOMB)) != 0)
		{
			for(BYTE i=0;i<cbTurnCardCount;i++)
			{
				if(GetCardLogicValue(m_bLaiZiCard) == GetCardLogicValue(cbTurnCardData[i]))
					continue;
				cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
			}
		}

		//搜索炸弹
		for (BYTE i=0;i<cbCardCount;i++)
		{
			//跳过王牌
			if(cbCardData[cbCardCount-i-1] == 0x4F || cbCardData[cbCardCount-i-1] == 0x4E)continue;

			//获取数值
			BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue<=cbLogicValue) continue;

			//炸弹判断
			BYTE cbTempLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);
			BYTE cbLaiZiCount=GetLaiZiCount(cbCardData,cbCardCount);
			BYTE j=1;
			for (j=1;j<i;j++)
			{
				if (GetCardLogicValue(cbCardData[cbCardCount+j-i-1])!=cbTempLogicValue) break;
			}
			if (j!=4 && cbTempLogicValue != GetCardLogicValue(m_bLaiZiCard) && 
				(cbTurnOutType&(1<<CT_LAIZI_BOMB)) == 0 && (cbTurnOutType&(1<<CT_BOMB_CARD)) == 0) 
			{
				//软炸弹
				if((j+cbLaiZiCount) >= 4)
				{
					OutCardResult.cbCardCount=0;
					for(BYTE k=0;k<4;k++)
					{
						if (GetCardLogicValue(cbCardData[cbCardCount+k-i-1])==cbTempLogicValue)
						{
							OutCardResult.cbResultCard[k]=cbCardData[cbCardCount+k-i-1];
							OutCardResult.cbCardCount++;
						}
						else
							break;
					}

					//赖子填充
					if(OutCardResult.cbCardCount < 4 && GetCardLogicValue(OutCardResult.cbResultCard[0]) != GetCardLogicValue(m_bLaiZiCard))
					{
						for(BYTE k=0;k<cbCardCount;k++)
						{
							if (GetCardLogicValue(cbCardData[cbCardCount-k-1])==GetCardLogicValue(m_bLaiZiCard))
							{
								OutCardResult.cbResultCard[OutCardResult.cbCardCount]=cbCardData[cbCardCount-k-1];
								OutCardResult.cbCardCount++;
							}
							if(OutCardResult.cbCardCount >= 4)
								break;
						}
					}
					return true;
				}
				continue;
			}
			else if(j == 4 && (cbTurnOutType&(1<<CT_LAIZI_BOMB)) == 0)
			{
				//设置结果
				OutCardResult.cbCardCount=4;
				OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-i-1];
				OutCardResult.cbResultCard[1]=cbCardData[cbCardCount-i];
				OutCardResult.cbResultCard[2]=cbCardData[cbCardCount-i+1];
				OutCardResult.cbResultCard[3]=cbCardData[cbCardCount-i+2];

				return true;
			}
		}
	}

	//搜索火箭
	if ((cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
	{
		//设置结果
		OutCardResult.cbCardCount=2;
		OutCardResult.cbResultCard[0]=cbCardData[0];
		OutCardResult.cbResultCard[1]=cbCardData[1];

		return true;
	}

	return false;
}

//获取赖子
BYTE CGameLogic::GetLaiZiCard()
{
	m_bLaiZiCard=m_cbCardData[rand()%52];
	return m_bLaiZiCard;
}

//赖子数目
BYTE CGameLogic::GetLaiZiCount(const BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	BYTE bLaiZiCount=0;
	for(BYTE i=0;i<cbHandCardCount;i++)
	{
		if(GetCardLogicValue(cbHandCardData[i]) == GetCardLogicValue(m_bLaiZiCard))
			bLaiZiCount++;
	}

	return bLaiZiCount;
}
//分析扑克
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置变量
	BYTE bCardData[MAX_COUNT];

	memset(bCardData,0,sizeof(bCardData));
	CopyMemory(bCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	SortCardList(bCardData,cbCardCount,ST_ORDER);

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(bCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(bCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				BYTE cbIndex=AnalyseResult.cbSignedCount++;
				AnalyseResult.cbSignedCardData[cbIndex*cbSameCount]=bCardData[i];
				break;
			}
		case 2:		//两张
			{
				BYTE cbIndex=AnalyseResult.cbDoubleCount++;
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount]=bCardData[i];
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount+1]=bCardData[i+1];
				break;
			}
		case 3:		//三张
			{
				BYTE cbIndex=AnalyseResult.cbThreeCount++;
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount]=bCardData[i];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+1]=bCardData[i+1];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+2]=bCardData[i+2];
				break;
			}
		case 4:		//四张
			{
				BYTE cbIndex=AnalyseResult.cbFourCount++;
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount]=bCardData[i];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+1]=bCardData[i+1];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+2]=bCardData[i+2];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+3]=bCardData[i+3];
				break;
			}
		}

		//设置索引
		i+=cbSameCount-1;
	}


	return;
}


//分析比较
bool CGameLogic::AnalysebCompare(const BYTE cbCardData[],BYTE cbCardCount, tagAnalyseResult & AnalyseResult,int dCardType)
{
	//分析扑克
	BYTE bLaiZiCount=GetLaiZiCount(cbCardData,cbCardCount);
	BYTE bCardDataTemp[MAX_COUNT];
	int dCardTypeTemp=CT_ERROR;
	tagAnalyseResult  AnalyseResultTemp;
	dCardType=1<<dCardType;

	memset(bCardDataTemp,0,sizeof(bCardDataTemp));
	CopyMemory(bCardDataTemp,cbCardData,sizeof(BYTE)*cbCardCount);
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
	ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

	//都是赖子做普通牌
	bool SameCard=false;
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(m_bLaiZiCard))
			SameCard=true;
		else
		{
			SameCard=false;
			break;
		}
	}

	//赖子判断
	if(bLaiZiCount != 0 && !SameCard)
	{
		for(BYTE i=0;i<cbCardCount;i++)
		{
			if(GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(m_bLaiZiCard))
			{
				//一张赖子
				for(BYTE j=0;j<13;j++)
				{
					bCardDataTemp[i]=m_cbCardData[j];

					//两张赖子
					if(bLaiZiCount >= 2)
					{
						for(BYTE k=i+1;k<cbCardCount;k++)
						{
							if(GetCardLogicValue(cbCardData[k]) == GetCardLogicValue(m_bLaiZiCard))
							{
								for(BYTE z=0;z<13;z++)
								{
									bCardDataTemp[k]=m_cbCardData[z];

									//三张赖子
									if(bLaiZiCount >= 3)
									{
										for(BYTE g=k+1;g<cbCardCount;g++)
										{
											if(GetCardLogicValue(cbCardData[g]) == GetCardLogicValue(m_bLaiZiCard))
											{
												for(BYTE f=0;f<13;f++)
												{
													bCardDataTemp[g]=m_cbCardData[f];

													//四张赖子
													if(bLaiZiCount == 4)
													{
														for(BYTE h=g+1;h<cbCardCount;h++)
														{
															if(GetCardLogicValue(cbCardData[h]) == GetCardLogicValue(m_bLaiZiCard))
															{
																for(BYTE l=0;l<13;l++)
																{
																	bCardDataTemp[h]=m_cbCardData[l];

																	ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

																	AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
																	if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
																		continue;
																	if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
																		AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
																}
															}
															else
															{
																ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

																AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
																if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
																	continue;

																if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
																	AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
															}
														}
													}
													ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

													AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
													if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
														continue;

													if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
														AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
												}
											}
											else
											{
												ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

												AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
												if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
													continue;

												if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
													AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
											}
										}
									}
									ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

									AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
									if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
										continue;

									if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
										AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
								}
							}
							else
							{
								ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

								AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
								if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
									continue;

								if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
									AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
							}
						}
					}
					ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

					AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
					if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
						continue;

					if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
						AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
				}
			}
			else
			{
				ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

				AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);
				if(GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData) == 0)
					continue;

				if((dCardType&(1<<GetType(AnalyseResultTemp,bCardDataTemp,cbCardCount,cbCardData))) != 0)
					AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
			}
		}
	}
	else
	{
		ZeroMemory(&AnalyseResultTemp,sizeof(AnalyseResultTemp));

		AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResultTemp);

		if((dCardType&(1<<GetType(AnalyseResultTemp,cbCardData,cbCardCount,cbCardData))) != 0)
			AnalysebCardData(bCardDataTemp,cbCardCount,AnalyseResult);
	}

	return true;
}
//牌组转换	(将32张牌的使用情况转化为一个32位整型)
int CGameLogic::TransListToInt(int iOldType,int iNewType)
{
	//数据校验
	if(iNewType == 0) return 0;

	//变量定义
	int nList = 0;
	ASSERT(iNewType != -1);
	nList |= (1<<iNewType);

	return iOldType|nList;
}

//变换后扑克
void CGameLogic::ChangeCard(int iType,BYTE cbNextCard[],BYTE cbNextCount,BYTE bChangeCard[])
{
	memset(bChangeCard,0,sizeof(bChangeCard));

	//是否有癞子
	bool bLaiZi=false;
	for(BYTE i=0;i<cbNextCount;i++)
	{
		if(GetCardValue(cbNextCard[i]) == m_bLaiZiCard)
			bLaiZi=true;
	}
	if(bLaiZi == false)
	{
		CopyMemory(bChangeCard,cbNextCard,sizeof(BYTE)*cbNextCount);
		return;
	}

	if((iType&(1<<CT_MISSILE_CARD)) != 0)
		CopyMemory(bChangeCard,cbNextCard,sizeof(cbNextCard));

	if((iType&(1<<CT_BOMB_CARD)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_BOMB_CARD))return ;

		bChangeCard[0]=NextResult.cbFourCardData[0];
		bChangeCard[1]=NextResult.cbFourCardData[1];
		bChangeCard[2]=NextResult.cbFourCardData[2];
		bChangeCard[3]=NextResult.cbFourCardData[3];
		return ;
	}
	else if((iType&(1<<CT_RUAN_BOMB)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_RUAN_BOMB))return;

		bChangeCard[0]=NextResult.cbFourCardData[0];
		bChangeCard[1]=NextResult.cbFourCardData[1];
		bChangeCard[2]=NextResult.cbFourCardData[2];
		bChangeCard[3]=NextResult.cbFourCardData[3];
		return ;
	}
	else if((iType&(1<<CT_SINGLE)) != 0)
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_SINGLE))return;

		bChangeCard[0]=NextResult.cbSignedCardData[0];
		return ;
	}
	else if((iType&(1<<CT_DOUBLE)) != 0)
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_DOUBLE))return;

		bChangeCard[0]=NextResult.cbDoubleCardData[0];
		bChangeCard[1]=NextResult.cbDoubleCardData[1];
		return ;
	}
	else if((iType&(1<<CT_THREE)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE))return;

		bChangeCard[0]=NextResult.cbThreeCardData[0];
		bChangeCard[1]=NextResult.cbThreeCardData[1];
		bChangeCard[2]=NextResult.cbThreeCardData[2];
		return ;
	}
	else if((iType&(1<<CT_SINGLE_LINE)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_SINGLE_LINE))return;

		for(BYTE j=0;j<cbNextCount;j++)
		{
			bChangeCard[j]=NextResult.cbSignedCardData[j];
		}
		return ;
	}
	else if((iType&(1<<CT_DOUBLE_LINE)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_DOUBLE_LINE))return;

		for(BYTE j=0;j<cbNextCount;j++)
		{
			bChangeCard[j]=NextResult.cbDoubleCardData[j];
		}
		return ;
	}
	else if((iType&(1<<CT_THREE_LINE)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE))return;

		for(BYTE j=0;j<cbNextCount;j++)
		{
			bChangeCard[j]=NextResult.cbThreeCardData[j];
		}
		return ;
	}

	else if((iType&(1<<CT_THREE_LINE_TAKE_ONE)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE_TAKE_ONE))return;

		for(BYTE j=0;j<cbNextCount-cbNextCount%3;j++)
		{
			bChangeCard[j]=NextResult.cbThreeCardData[j];
		}

		BYTE bThreeCount=cbNextCount-cbNextCount/3;
		for(BYTE j=0;j<cbNextCount/3;j++)
		{
			if(NextResult.cbSignedCount > 0)
				bChangeCard[bThreeCount++]=NextResult.cbSignedCardData[j];
		}
		return ;
	}
	else if((iType&(1<<CT_THREE_LINE_TAKE_TWO)) != 0) 
	{
		//分析扑克
		tagAnalyseResult NextResult;

		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_THREE_LINE_TAKE_TWO))return;

		for(BYTE j=0;j<3*cbNextCount/5;j++)
		{
			bChangeCard[j]=NextResult.cbThreeCardData[j];
		}

		BYTE bThreeCount=3*cbNextCount/5;
		for(BYTE j=0;j<cbNextCount-3*cbNextCount/5;j++)
		{
			if(NextResult.cbDoubleCount > 0)
			{
				bChangeCard[bThreeCount++]=NextResult.cbDoubleCardData[j*2];
				bChangeCard[bThreeCount++]=NextResult.cbDoubleCardData[j*2+1];
			}
		}

		return ;
	}
	else if((iType&(1<<CT_FOUR_LINE_TAKE_ONE)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_FOUR_LINE_TAKE_ONE))return;

		for(BYTE j=0;j<4;j++)
		{
			bChangeCard[j]=NextResult.cbFourCardData[j];
		}
		if(NextResult.cbSignedCount > 1)
		{
			bChangeCard[4]=NextResult.cbSignedCardData[0];
			bChangeCard[5]=NextResult.cbSignedCardData[1];
		}
		return ;
	}
	else if((iType&(1<<CT_FOUR_LINE_TAKE_TWO)) != 0)  
	{
		//分析扑克
		tagAnalyseResult NextResult;
		ZeroMemory(&NextResult,sizeof(NextResult));

		if(!AnalysebCompare(cbNextCard,cbNextCount,NextResult,CT_FOUR_LINE_TAKE_TWO))return;

		for(BYTE j=0;j<4;j++)
		{
			bChangeCard[j]=NextResult.cbFourCardData[j];
		}
		if(NextResult.cbSignedCount > 1)
		{
			bChangeCard[4]=NextResult.cbSignedCardData[0];
			bChangeCard[5]=NextResult.cbSignedCardData[1];
		}
		if(NextResult.cbDoubleCount > 1)
		{
			bChangeCard[4]=NextResult.cbDoubleCardData[0];
			bChangeCard[5]=NextResult.cbDoubleCardData[1];
			bChangeCard[6]=NextResult.cbDoubleCardData[2];
			bChangeCard[7]=NextResult.cbDoubleCardData[3];
		}
		return ;
	}
}
//////////////////////////////////////////////////////////////////////////
