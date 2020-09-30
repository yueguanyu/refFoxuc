#include "StdAfx.h"
#include "GameLogic.h"

#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

/****************************************************
*函数名：IsSpecialType 
*功能：  判断一副骰子是否是特殊骰型,并当骰型为豹子的
		 时候保存豹子骰子的骰值					  JJ
*参数：	 bDiceData			骰子数组(5)			IN
		 bDiceCount			数组数目(5)			IN
		 bHaveYellOne		是否喊过1			IN
		 pSpType			特殊信息			OUT
*返回值	 true	特殊骰型
		 false  普通骰型
****************************************************/
bool CGameLogic::IsSpecialType(BYTE bDiceData[], BYTE bDiceCount, bool bHaveYellOne, tagSpecialType *pSpType)
{
	//数据校验
	ASSERT(bDiceData!=NULL&&bDiceCount==5&&pSpType!=NULL);
	if (NULL == bDiceData || 5 != bDiceCount || NULL == pSpType)
		return false;

	ZeroMemory(pSpType, sizeof(tagSpecialType));

	//变量定义
	BYTE bTmpArr[6] = {0};
	BYTE bBaoziPoint = 0;
	
	//判断顺子
	for (WORD i = 0; i < bDiceCount; i++)
	{
		ASSERT(bDiceData[i]>=1&&bDiceData[i]<=6);

		if (bTmpArr[bDiceData[i]-1] != 0)
			break;
		
		bTmpArr[bDiceData[i]-1]++;

		if (i == bDiceCount - 1)
		{
			pSpType->bType = DICE_TYPE_STRAIGHT;
			return true;
		}
	}
	
	//判断豹子
	for (WORD i = 0; i < bDiceCount; i++)
	{
		ASSERT(bDiceData[i]>=1&&bDiceData[i]<=6);

		if (!bHaveYellOne && bDiceData[i] == 1 && i != bDiceCount - 1)
			continue;
		
		if (bBaoziPoint == 0)
			bBaoziPoint = bDiceData[i];
		else if (bBaoziPoint != bDiceData[i])
		{
			if (bDiceData[i] != 1 || bHaveYellOne)
				break;
		}

		if (i == bDiceCount - 1)
		{
			pSpType->bType = DICE_TYPE_BAOZI;
			pSpType->bBaiziPoint = bBaoziPoint;
			return true;
		}
	}

	pSpType->bType = DICE_TYPE_NORMAL;
	return false;
}

/****************************************************
*函数名：RandDice 
*功能：  随机产生骰子值							JJ
*参数：	 bDiceData			骰子数组(5)			OUT
		 bDiceCount			数组数目(5)			IN
*返回值	 无
****************************************************/
void CGameLogic::RandDice(BYTE bDiceData[], BYTE bDiceCount)
{
	//数据校验
	ASSERT(bDiceData!=NULL);
	if (NULL == bDiceData)	return;

	ZeroMemory(bDiceData, sizeof(BYTE)*bDiceCount);
	srand(GetTickCount());
	
	for (WORD i = 0; i < bDiceCount; i++)
		bDiceData[i] = rand()%6 + 1;
}

/****************************************************
*函数名：CalcDiceRange 
*功能：  根据当前最大喊话计算某个数目可喊得最小点数	JJ
*参数：	 pDiceYell			最大喊话信息		IN
		 bDiceCount			要喊得数目			IN
		 bFirstYell			是否游戏第一次喊话	IN
		 pbMinPoint			最小点数			OUT
		 pbMaxPoint			最大点数			OUT
*返回值	 0		 有点可喊
		 1		 无点可喊 (小于等于喊话数目)
		 2		 无点可喊 (到达最大喊话12个6)
		 other	 错误
****************************************************/
LRESULT CGameLogic::CalcDiceRange(tagDiceYell  *pDiceYell, BYTE bDiceCount, bool bFirstYell, BYTE *pbMinPoint, BYTE *pbMaxPoint)
{
	//数据校验
	ASSERT(pDiceYell!=NULL&&bDiceCount>=2&&bDiceCount<=12&&pbMinPoint!=NULL&&pbMaxPoint!=NULL);
	ASSERT(pDiceYell->bDiceCount>=0&&pDiceYell->bDiceCount<=12&&pDiceYell->bDicePoint>=0&&pDiceYell->bDicePoint<=6);
	if (pDiceYell == NULL || bDiceCount < 2 || bDiceCount > 12 || pbMinPoint == NULL || pbMaxPoint == NULL)
		return 3;
	
	//首轮喊话
	if (bFirstYell)
	{
		if (2 == bDiceCount)
		{
			*pbMinPoint = 1;
			*pbMaxPoint = 1;
		}
		else if (bDiceCount > 2 && bDiceCount < 13)
		{
			*pbMinPoint = 1;
			*pbMaxPoint = 6;
		}
		else	
			return 1;
	}
	else
	{
		//达到最大
		if (pDiceYell->bDiceCount == 12 && pDiceYell->bDicePoint == 6)
			return 2;
			
		if (bDiceCount < pDiceYell->bDiceCount)
			return 1;
		else if (bDiceCount == pDiceYell->bDiceCount)
		{
			if (pDiceYell->bDicePoint == 6)
				return 1;
			else
			{
				*pbMinPoint = pDiceYell->bDicePoint + 1;
				*pbMaxPoint = 6;
			}
		}
		else
		{
			*pbMinPoint = 1;
			*pbMaxPoint = 6;
		}
	}

	return 0;
}

/****************************************************
*函数名：CalcDiceCount 
*功能：  计算某个骰点的数目	JJ
*参数：	 bDiceData			骰子数组(5)			IN
		 bDiceCount			骰子数组数目(5)		IN
		 bDicePoint			要计数的骰点(1-6)	IN
		 bHaveYellOne		是否喊过1			IN
*返回值	 该骰点骰子数目(0-12)
****************************************************/
int CGameLogic::CalcDiceCount(BYTE *bDiceData, BYTE bDiceCount, BYTE bDicePoint, bool bHaveYellOne/* = false*/)
{
	//数据校验
	ASSERT(bDiceData!=NULL&&bDiceCount==5&&bDicePoint>=1&&bDicePoint<=6);
	if (bDiceData == NULL || bDiceCount !=5 || bDicePoint < 1 || bDiceCount > 6)
		return false;
	
	//变量定义
	tagSpecialType tst = {0};

	//特殊类型
	if (IsSpecialType(bDiceData, bDiceCount, bHaveYellOne, &tst))
	{
		if (tst.bType == DICE_TYPE_STRAIGHT)
			return 0;
		else if (tst.bType == DICE_TYPE_BAOZI)
		{
			if ( tst.bBaiziPoint == bDicePoint || (!bHaveYellOne && tst.bBaiziPoint==1) )
				return 6;
		}
		else
		{
			//MyMsgBox(_T("CGameLogic::CalcDice [(%d %d %d %d %d) %d  %d]"), bDiceData[0], bDiceData[1], bDiceData[2], 
			//					bDiceData[3], bDiceData[4], bDicePoint, tst.bType);
			return 0;
		}
	}

	BYTE bCount = 0;
	for (int i = 0; i < bDiceCount; i++)
	{
		if (bDiceData[i] == bDicePoint)
		{
			bCount++;
			continue;
		}
		if (bDiceData[i] == 1 && !bHaveYellOne)
			bCount++;
	}
	return bCount;
}

/****************************************************
*函数名：CalcLimitBigYell 
*功能：  计算比传入喊话仅大一点的值，若为12*6则返回false JJ
*参数：	 pDiceYell	喊话指针				INOUT
*返回值	 true 求出大值
		 false 错误值或者传入值达到12*6
****************************************************/
bool CGameLogic::CalcLimitBigYell(tagDiceYell *pDiceYell)
{
	//数据验证
	ASSERT(pDiceYell != NULL);
	if (pDiceYell == NULL)	return false;
	
	if	(pDiceYell->bDiceCount == 0 && pDiceYell->bDicePoint == 0)
	{
		pDiceYell->bDiceCount = 2;
		pDiceYell->bDicePoint = 1;
		return true;
	}

	ASSERT(pDiceYell->bDiceCount>=0&&pDiceYell->bDiceCount<=12&&pDiceYell->bDicePoint>=0&&pDiceYell->bDicePoint<=6);
	if (pDiceYell->bDiceCount<2 || pDiceYell->bDiceCount>12 || pDiceYell->bDicePoint<1 || pDiceYell->bDicePoint>6)
		return false;

	if (pDiceYell->bDiceCount == 12 && pDiceYell->bDicePoint == 6)
		return false;

	pDiceYell->bDiceCount = (pDiceYell->bDicePoint==6?pDiceYell->bDiceCount+1:pDiceYell->bDiceCount);
	pDiceYell->bDicePoint = (pDiceYell->bDicePoint==6?1:pDiceYell->bDicePoint+1);

	return true;
}
//////////////////////////////////////////////////////////////////////////
