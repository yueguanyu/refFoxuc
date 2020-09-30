#include "StdAfx.h"
#include "GameLogic.h"


//////////////////////////////////////////////////////////////////////////////////
CGameLogic			CGameLogic::m_GameLogic;

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//随机区域
INT CGameLogic::RandomArea( INT nLen, ... )
{ 
	ASSERT( nLen > 0 );
	if ( nLen <= 0 )
		return 0;

	va_list argptr;
	va_start(argptr, nLen);
	CWHArray<INT>		Array;
	for ( BYTE i = 0; i < nLen; ++i )
	{
		INT nTemp = va_arg( argptr, INT );
		if( nTemp >= 0 && nTemp < 100000 )
		{
			Array.Add(nTemp);
		}
		else
		{
			ASSERT(FALSE);
			break;
		}
	}
	va_end(argptr);


	INT nIndex = 0;
	INT nACTotal = 0;
	for (INT i = 0; i < Array.GetCount(); i++)
		nACTotal += Array[i];

	ASSERT( nACTotal > 0  );
	if ( nACTotal > 0 )
	{
		static UINT nRandAuxiliary = 0;
		nRandAuxiliary += rand()%10;
		INT nRandNum = ((UINT)(rand() + GetTickCount() + nRandAuxiliary)) % nACTotal;
		for (INT i = 0; i < Array.GetCount(); i++)
		{
			nRandNum -= Array[i];
			if (nRandNum < 0)
			{
				nIndex = i;
				break;
			}
		}
	}
	else
	{
		nIndex = rand()%nLen;
	}


	ASSERT( nIndex < nLen );
	Array.RemoveAll();
	return nIndex;
}

//判断类别
int CGameLogic::AnimalType( int nAnimal )
{
	switch(nAnimal)
	{
	case ANIMAL_LION:
	case ANIMAL_PANDA:
	case ANIMAL_MONKEY:
	case ANIMAL_RABBIT:
		return ANIMAL_TYPE_BEAST;
	case ANIMAL_EAGLE:
	case ANIMAL_PEACOCK:
	case ANIMAL_PIGEON:
	case ANIMAL_SWALLOW:
		return ANIMAL_TYPE_BIRD;
	case ANIMAL_SLIVER_SHARK:
		return ANIMAL_TYPE_SLIVER;
	case ANIMAL_GOLD_SHARK:
		return ANIMAL_TYPE_GOLD;
	default:
		return ANIMAL_TYPE_NULL;
	}
}

//转盘中动物
int CGameLogic::TurnTableAnimal( int nTableIndex )
{
	if ( nTableIndex >= TURAN_TABLE_MAX || nTableIndex < 0 )
	{
		ASSERT(FALSE);
		return ANIMAL_MAX;
	}

	static const int AnimalSpecies[TURAN_TABLE_MAX] = 
	{ 
		ANIMAL_SLIVER_SHARK,ANIMAL_EAGLE,ANIMAL_EAGLE,ANIMAL_EAGLE,
		ANIMAL_GOLD_SHARK, ANIMAL_LION, ANIMAL_LION, ANIMAL_LION, 
		ANIMAL_SLIVER_SHARK,ANIMAL_PANDA,ANIMAL_PANDA,ANIMAL_GOLD_SHARK,
		ANIMAL_MONKEY,ANIMAL_MONKEY,ANIMAL_SLIVER_SHARK,ANIMAL_RABBIT,
		ANIMAL_RABBIT,ANIMAL_RABBIT,ANIMAL_GOLD_SHARK,ANIMAL_SWALLOW,
		ANIMAL_SWALLOW,ANIMAL_SWALLOW,ANIMAL_SLIVER_SHARK,ANIMAL_PIGEON,
		ANIMAL_PIGEON,ANIMAL_GOLD_SHARK,ANIMAL_PEACOCK,ANIMAL_PEACOCK
	};
	return AnimalSpecies[nTableIndex];
}

//转盘中动物类型
int CGameLogic::TurnTableAnimalType( int nTableIndex )
{
	if ( nTableIndex >= TURAN_TABLE_MAX || nTableIndex < 0 )
	{
		ASSERT(FALSE);
		return ANIMAL_TYPE_NULL;
	}

	return AnimalType(TurnTableAnimal(nTableIndex));
}

//////////////////////////////////////////////////////////////////////////////////
