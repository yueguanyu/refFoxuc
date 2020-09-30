#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "afxtempl.h"
#include "..\消息定义\CMD_LiarsDice.h"
//////////////////////////////////////////////////////////////////////////	

//骰子类型
#define		DICE_TYPE_NORMAL		0x00		//普通类型
#define		DICE_TYPE_STRAIGHT		0x01		//顺子类型
#define		DICE_TYPE_BAOZI			0x02		//豹子类型
#define		DICE_TYPE_ERROR			0x03		//错误类型

//特殊骰型
struct	tagSpecialType
{
	BYTE	bType;			//类型
	BYTE	bBaiziPoint;	//当上面类型为豹子的时候, 此值为豹子代替的点数
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////	
//调试函数
static void MyMsgBox(LPCSTR lpszFormat, ...)
{
//	va_list args;
//	int		nBuf;
//	TCHAR	szBuffer[512];
//
//	va_start(args, lpszFormat);
//	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
//#ifdef _DEBUG
//	AfxMessageBox(szBuffer);
//#else
//	TCHAR	szFormat[512];
//	_stprintf(szFormat, _T("[JJ]: %s"), szBuffer);
//	OutputDebugString(szFormat);
//#endif
//
//	va_end(args);
}

static void MyDebug(LPCSTR lpszFormat, ...)
{
	//va_list args;
	//int		nBuf;
	//TCHAR	szBuffer[512];
	//TCHAR	szFormat[512];

	//va_start(args, lpszFormat);
	//nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
	//_stprintf(szFormat, _T("[JJ]: %s"), szBuffer);
	//OutputDebugString(szFormat);

	//va_end(args);
}
//////////////////////////////////////////////////////////////////////////


//游戏逻辑类
class CGameLogic
{
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

public:
	//特殊类型
	bool IsSpecialType(BYTE bDiceData[], BYTE bDiceCount, bool bHaveYellOne, tagSpecialType *pSpType);

	//随机骰值
	void RandDice(BYTE bDiceData[], BYTE bDiceCount);

	//可喊点数
	LRESULT CalcDiceRange(tagDiceYell  *pDiceYell, BYTE bDiceCount, bool bFirstYell, BYTE *pbMinPoint, BYTE *pbMaxPoint);

	//计算数目
	int CalcDiceCount(BYTE bDiceData[], BYTE bDiceCount, BYTE bDicePoint, bool bHaveYellOne = false);

	//最小大值
	bool CalcLimitBigYell(tagDiceYell *pDiceYell);
};

//////////////////////////////////////////////////////////////////////////

#endif
