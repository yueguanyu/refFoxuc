#pragma once

#include "Stdafx.h"

//顶点类
struct DPOINT
{
	DOUBLE				x;
	DOUBLE				y;

	DPOINT() {  x = 0.0; y = 0.0; }
	DPOINT( POINT& Par ) {  x = Par.x; y = Par.y; }
	DPOINT( DPOINT& Par ) {  x = Par.x; y = Par.y; }
	DPOINT( DOUBLE ParX, DOUBLE ParY ) { x = ParX; y = ParY; }
	void operator+= (POINT& ptPar) { x += ptPar.x; y += ptPar.y; }
	void operator+= (DPOINT& ptPar) { x += ptPar.x; y += ptPar.y; }
	inline void SetPoint( DOUBLE ParX, DOUBLE ParY ) { x = ParX; y = ParY; }
};

// 筹码移动
struct stuMoveChip
{
public:
	WORD		wChipIndex;				// 筹码索引
	WORD		wChipChairID;			// 筹码位置ID
	UINT		nTheTime;				// 当前步数
	UINT		nAllTime;				// 总步数
	DPOINT		ptBeginPos;				// 开始位置
	DPOINT		ptEndPos;				// 结束位置
	DPOINT		ptCurrentPos;			// 当前位置

	DPOINT		ptInitialVelocity;		// 初速度
	DPOINT		ptFriction;				// 摩擦力

	BOOL		bDeceleratedMotion;		// 减速运动
	BOOL		bShow;					// 是否显示


public:
	stuMoveChip( WORD wParChipIndex, WORD wParChipChairID, UINT nParAllTime, DPOINT ptParBeginPos, DPOINT ptParEndPos, BOOL bParDeceleratedMotion)
	{
		BeginMove(wParChipIndex, wParChipChairID, nParAllTime, ptParBeginPos, ptParEndPos, bParDeceleratedMotion);
	}

	void BeginMove( WORD wParChipIndex, WORD wParChipChairID, UINT nParAllTime, DPOINT ptParBeginPos, DPOINT ptParEndPos, BOOL bParDeceleratedMotion )
	{
		bShow        = TRUE;
		wChipIndex	 = wParChipIndex;
		wChipChairID = wParChipChairID;
		nTheTime	 = 0;
		nAllTime	 = nParAllTime;
		ptBeginPos	 = ptParBeginPos;
		ptEndPos     = ptParEndPos;
		ptCurrentPos = ptParBeginPos;
		bDeceleratedMotion = bParDeceleratedMotion;

		if ( nParAllTime != 0 )
		{
			// 减速运动
			if( bDeceleratedMotion )
			{
				// 初速度
				ptInitialVelocity.x = (ptParEndPos.x - ptParBeginPos.x)/nParAllTime;
				ptInitialVelocity.y = (ptParEndPos.y - ptParBeginPos.y)/nParAllTime;

				// 摩擦力
				ptFriction.x = -(ptInitialVelocity.x / nParAllTime / 2.035);
				ptFriction.y = -(ptInitialVelocity.y / nParAllTime / 2.035);
			}
			// 匀速运动
			else
			{
				// 速度
				ptInitialVelocity.x = (ptParEndPos.x - ptParBeginPos.x)/nParAllTime;
				ptInitialVelocity.y = (ptParEndPos.y - ptParBeginPos.y)/nParAllTime;
			}
		}
	}

	void SetShow( BOOL bParShow )
	{
		bShow = bParShow;
	}

	BOOL Move()
	{
		if ( nAllTime == 0 )
			return FALSE;

		// 减速运动
		if( bDeceleratedMotion )
		{
			// 移动
			BOOL	bMove = FALSE;

			// 时间
			nTheTime++;

			// 位移
			DPOINT  ptDisplacement(ptInitialVelocity.x + ptFriction.x * nTheTime, ptInitialVelocity.y + ptFriction.y * nTheTime);

			// 减速运动
			if ( (ptInitialVelocity.x > 0 && ptDisplacement.x > 0)
				|| (ptInitialVelocity.x < 0 && ptDisplacement.x < 0) )
			{
				ptCurrentPos.x += ptDisplacement.x;
				bMove = TRUE;
			}

			if ( (ptInitialVelocity.y > 0 && ptDisplacement.y > 0)
				|| (ptInitialVelocity.y < 0 && ptDisplacement.y < 0) )
			{
				ptCurrentPos.y += ptDisplacement.y;
				bMove = TRUE;
			}

			return bMove;
		}
		// 匀速运动
		else
		{
			if ( nTheTime == nAllTime)
				return FALSE;

			// 移动时间
			nTheTime++;

			// 移动
			ptCurrentPos += ptInitialVelocity;

			
			return TRUE;
		}
	}
};

