#pragma once


//动画索引
#define GEM_SHOW_INDEX				12


//顶点类
struct DPOINT
{
	DOUBLE				x;
	DOUBLE				y;

	DPOINT() {  x = 0.0; y = 0.0; }
	DPOINT( POINT& Par ) {  x = Par.x; y = Par.y; }
	DPOINT( DPOINT& Par ) {  x = Par.x; y = Par.y; }
	DPOINT( DOUBLE ParX, DOUBLE ParY ) { x = ParX; y = ParY; }
	void operator+= (DPOINT& point) { x += point.x; y += point.y; }
	inline void SetPoint( DOUBLE ParX, DOUBLE ParY ) { x = ParX; y = ParY; }
};

//对象移动类
class CObjectMove
{
	//变量
public:
	BYTE							cbFrameIndex;			//动作索引
	BYTE							bMove;					//是否移动
	UINT							nCreateTime;			//创建时间
	UINT							nMoveTime;				//移动时间
	UINT							nFrameTime;				//换帧时间
	DPOINT							ptPosition;				//位置
	DPOINT							ptEnd;					//结束位置
	DPOINT							ptBegin;				//开始位置
	CLapseCount						LapseCount;				//流逝计数

public:
	CObjectMove()
	{
		bMove			=	FALSE;
		cbFrameIndex	=	0;
		nCreateTime		=	0;
		nMoveTime		=	1;
		nFrameTime		=	1;
		ptPosition.SetPoint(0, 0);
		ptBegin.SetPoint(0, 0);
		ptEnd.SetPoint(0, 0);
	}

	CObjectMove( DPOINT ParBegin, DPOINT ParEnd, UINT nParMoveTime, UINT nParFrameTime )
	{
		if( nParMoveTime == 0 )
			nParMoveTime = 1;

		if( nParFrameTime == 0 )
			nParFrameTime = 1;

		bMove			=	TRUE;
		cbFrameIndex	=	0;
		nCreateTime		=	GetTickCount();
		nMoveTime		=	nParMoveTime;
		nFrameTime		=	nParFrameTime;
		ptPosition		=   ParBegin;
		ptBegin			=	ParBegin;
		ptEnd			=	ParEnd;
		LapseCount.Initialization();
	}

	~CObjectMove(){}

	VOID BeginMove( DPOINT ParBegin, DPOINT ParEnd, UINT nParMoveTime, UINT nParFrameTime )
	{
		if( nParMoveTime == 0 )
			nParMoveTime = 1;

		if( nParFrameTime == 0 )
			nParFrameTime = 1;

		bMove			=	TRUE;
		cbFrameIndex	=	0;
		nCreateTime		=	GetTickCount();
		nMoveTime		=	nParMoveTime;
		nFrameTime		=	nParFrameTime;
		ptPosition		=   ParBegin;
		ptBegin			=	ParBegin;
		ptEnd			=	ParEnd;
		LapseCount.Initialization();
	}

	BOOL Move()
	{
		//帧数转换
		if ( LapseCount.GetLapseCount(nFrameTime) > 0 )
			cbFrameIndex++;

		if ( nMoveTime == 0 || !bMove )
			return FALSE;

		//偏移时间
		UINT nOffSetTime = GetTickCount() - nCreateTime;

		//位置移动
		DOUBLE dOffSet = (DOUBLE)nOffSetTime / (DOUBLE)nMoveTime;
		if ( dOffSet > 1.00 )
			dOffSet = 1.00;

		ptPosition.x = (ptEnd.x - ptBegin.x) * dOffSet + ptBegin.x;
		ptPosition.y = (ptEnd.y - ptBegin.y) * dOffSet + ptBegin.y;

		if ( nOffSetTime >= nMoveTime )
		{
			ptPosition = ptEnd;
			bMove = FALSE;
			return TRUE;
		}
		return FALSE;
	}
};


//鱼鱼绘画结构
#define FISH_STEP				5
struct tgaFishDraw
{

public:
	INT					nFishIndex;				//鱼索引
	BYTE				nFishType;				//鱼种类
	BYTE				bLinear;				//直线游
	BOOL				bWhetherToCreate;		//是否创建

	BYTE				cbFrameIndex;			//动作索引

	UINT				nOffsetTime;			//偏移时间
	BYTE				cbCreateTime;			//创建时间
	UINT				nClientTime;			//客户端时间

	BYTE				cbMoveCount;			//移动个数
	BYTE				cbMoveTime[FISH_STEP];	//总步数
	DPOINT				ptMovePos[FISH_STEP];	//移动位置

	DPOINT				ptLast;					//上次位置
	DPOINT				ptPosition;				//当前位置

	CLapseCount			LapseCount;				//流逝计数

public:
	tgaFishDraw( INT nParFishIndex, BYTE nParFishType, BYTE bParLinear, 
		UINT nParOffsetTime, BYTE cbParCreateTime, UINT nParClientTime,
		BYTE cbParMoveCount , POINT* pParMovePos , BYTE* pParMoveTime )
	{
		nFishIndex = nParFishIndex;
		nFishType = nParFishType;
		bLinear = bParLinear;

		cbFrameIndex = rand()%255;

		nOffsetTime = nParOffsetTime;
		cbCreateTime = cbParCreateTime;
		nClientTime = nParClientTime;

		cbMoveCount = cbParMoveCount;
		for(INT i = 0; i < FISH_STEP; ++i )
		{
			ptMovePos[i].SetPoint( pParMovePos[i].x, pParMovePos[i].y );
			cbMoveTime[i] = pParMoveTime[i];
		}

		ptLast.SetPoint( pParMovePos[0].x, pParMovePos[0].y );
		ptPosition.SetPoint( pParMovePos[0].x, pParMovePos[0].y );
		bWhetherToCreate = FALSE;
		LapseCount.Initialization();
	}

	//已知圆心,半径,弧度 求圆上任意一
	DPOINT RotatePoint( DPOINT ptCircle, DOUBLE dRadius, DOUBLE dRadian )
	{
		DPOINT point;
		point.x = dRadius*cos(dRadian) + ptCircle.x;
		point.y = dRadius*sin(dRadian) + ptCircle.y;
		return point;
	}

	BOOL WhetherToCreate()
	{
		return bWhetherToCreate;
	}


	BOOL Move()
	{
		UINT unTime = (GetTickCount() + nOffsetTime - nClientTime);

		if (unTime < (UINT)cbCreateTime * 1000 )
		{
			bWhetherToCreate = FALSE;
			return TRUE;
		}

		unTime -= cbCreateTime * 1000 ;
		UINT unAllTime = 0;
		INT  nMoveIndex = 0;
		BOOL bSuccess = FALSE;
		for ( int i = 0; i < cbMoveCount; ++i )
		{
			unAllTime += (cbMoveTime[i] * 1000);
			if ( unTime <= unAllTime )
			{
				nMoveIndex = i;
				bSuccess = TRUE;
				break;
			}
		}

		if ( !bSuccess || nMoveIndex >= (cbMoveCount - 1) )
		{
			bWhetherToCreate = FALSE;
			return FALSE;
		}

		for( int i = 0; i < nMoveIndex; ++i )
		{
			unTime -= cbMoveTime[i] * 1000;
		}

		DPOINT ptLastTemp( ptPosition.x, ptPosition.y );
		DOUBLE dProportion = (DOUBLE)(unTime)/(DOUBLE)(cbMoveTime[nMoveIndex] * 1000);

		if( bLinear )
		{
			ptPosition.x = (ptMovePos[nMoveIndex + 1].x - ptMovePos[nMoveIndex].x) * ( dProportion ) + ptMovePos[nMoveIndex].x;
			ptPosition.y = (ptMovePos[nMoveIndex + 1].y - ptMovePos[nMoveIndex].y) * ( dProportion ) + ptMovePos[nMoveIndex].y;
		}
		else
		{
			DPOINT ptStart(ptMovePos[nMoveIndex].x, ptMovePos[nMoveIndex].y);
			DPOINT ptObjective(ptMovePos[nMoveIndex + 1].x, ptMovePos[nMoveIndex + 1].y);
			DOUBLE dDistance = sqrt(pow(abs(ptObjective.x - ptStart.x),2) + pow(abs(ptObjective.y - ptStart.y),2));
			DPOINT ptCircle (	(max(ptObjective.x,ptStart.x) - min(ptObjective.x,ptStart.x))/2 + min(ptObjective.x,ptStart.x), 
				(max(ptObjective.y,ptStart.y) - min(ptObjective.y,ptStart.y))/2 + min(ptObjective.y,ptStart.y)	);
			DOUBLE dAngleDeviation = atan2( ptObjective.y - ptStart.y, ptObjective.x - ptStart.x);
			DOUBLE dAngleRotating = 0.0;
			DOUBLE dAngle = D3DX_PI * (((nMoveIndex%2) == 0) ? 1 : -1);
			

			//移动弧度
			if ( dAngleDeviation <= D3DX_PI/2 && dAngleDeviation > -D3DX_PI/2)
				dAngleRotating = dAngle * ( 1.0 - dProportion);
			else
				dAngleRotating = dAngle * ( dProportion );

			//基础偏移弧度
			if( dAngleDeviation <= D3DX_PI/2 && dAngleDeviation >= 0.0 )
				dAngleRotating += dAngleDeviation;
			else if ( dAngleDeviation > D3DX_PI/2 && dAngleDeviation <= D3DX_PI )
				dAngleRotating -= (D3DX_PI - dAngleDeviation);
			else if ( dAngleDeviation < 0.0 && dAngleDeviation >= -D3DX_PI/2 )
				dAngleRotating += dAngleDeviation;
			else if ( dAngleDeviation < -D3DX_PI/2 && dAngleDeviation > -D3DX_PI )
				dAngleRotating += D3DX_PI - abs(dAngleDeviation);

			ptPosition = RotatePoint( ptCircle, dDistance/2 ,dAngleRotating );
		}


		if( ptLastTemp.x - ptPosition.x != 0.0 || ptLastTemp.y - ptPosition.y != 0.0 )
		{
			ptLast = ptLastTemp;
		}

		if ( nMoveIndex >= (cbMoveCount - 1) )
		{
			bWhetherToCreate = FALSE;
			return FALSE;
		}

		bWhetherToCreate = TRUE;
		return TRUE;
	}
	~tgaFishDraw(void){}

};

struct tgaAnimalsDraw
{
	bool					bBright;					//亮
	bool					bflicker;					//闪动
	CRect					rectDraw;					//绘画矩形

	tgaAnimalsDraw()
	{
		bBright = false;
		bflicker = false;
		rectDraw.SetRect(0, 0, 0, 0);
	};
	~tgaAnimalsDraw(){};
};