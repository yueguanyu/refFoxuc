#include "StdAfx.h"
#include "Resource.h"
#include "NumberControl.h"

//////////////////////////////////////////////////////////////////////////

#define		MAX_SCROLL_COUNT			1
#define		MAX_EXCUSION				2

//////////////////////////////////////////////////////////////////////////

//构造函数
CNumberControl::CNumberControl()
{
	//位置变量
	m_BenchmarkPos.SetPoint(0,0);
	m_XCollocateMode = enXLeft;
	m_rcDraw.SetRectEmpty();

	//数字变量
	m_lScore = 0L;

	//动画变量
	m_nYExcusion = 0;
	m_nScrollCount = 0;
	m_nScrollIndex = 0;
	m_bMoving = FALSE;

	//位图变量
	HINSTANCE hInst = AfxGetInstanceHandle();
	m_PngScoreNum.LoadImage(hInst,TEXT("SCORE_NUM"));
	m_PngScoreBack.LoadImage(hInst,TEXT("SCORE_BACK"));
}

//析构函数
CNumberControl::~CNumberControl()
{
}

//设置基准
VOID CNumberControl::SetBencbmarkPos( INT nXPos, INT nYPos, enXCollocateMode XCollocateMode )
{
	//如果绘画区域非空,更新
	if( !m_rcDraw.IsRectEmpty() )
	{
		CRect rc = m_rcDraw;
		rc.OffsetRect( nXPos,nYPos );
		m_rcDraw |= rc;
	}
	m_BenchmarkPos.SetPoint(nXPos,nYPos);
	m_XCollocateMode = XCollocateMode;
}

//设置数字
VOID CNumberControl::SetScore( LONGLONG lScore )
{
	m_lScore = lScore;
}

//开始滚动
BOOL CNumberControl::BeginScrollNumber()
{
	//若正在动画,返回FALSE
	if( m_bMoving ) return FALSE;

	//若分数为0,则返回
	if( m_lScore == 0 ) return FALSE;

	//重置变量
	m_arScoreNum.RemoveAll();
	m_rcDraw.SetRectEmpty();

	//构造动画变量
	LONGLONG lScore = m_lScore;
	if( lScore < 0 ) lScore = -lScore;
	BYTE byCellNum = 0;
	do
	{
		if( lScore != 0 )
			byCellNum = BYTE(lScore%10);
		lScore /= 10L;
		m_arScoreNum.Add(byCellNum);
	}while( lScore != 0 );
	m_nYExcusion = 0;
	m_nScrollIndex = 0;
	m_nScrollCount = (rand()%MAX_SCROLL_COUNT)*10;
	m_nScrollCount += m_arScoreNum[0];

	//设置为0
	m_arScoreNum[0] = 0;
	m_bMoving = TRUE;

	return TRUE;
}

//滚动数字
BOOL CNumberControl::PlayScrollNumber()
{
	if( m_lScore == 0 ) return FALSE;

	//检查是否完成动画
	if( m_nScrollCount == 0 )
	{
		//是否全部完成动画
		if( m_nScrollIndex + 1 == m_arScoreNum.GetCount() ) 
		{
			m_bMoving = FALSE;
			return FALSE;
		}

		//增加数字索引
		m_nScrollIndex++;

		//设置偏移
		m_nYExcusion = 0;

		//生成滚动步数
		m_nScrollCount = (rand()%MAX_SCROLL_COUNT)*10;
		m_nScrollCount += m_arScoreNum[m_nScrollIndex];

		//置0
		m_arScoreNum[m_nScrollIndex] = 0;
	};

	//增加偏移
	if( ++m_nYExcusion == MAX_EXCUSION )
	{
		m_nYExcusion = 0;

		//自减滚动次数
		m_nScrollCount--;

		//增加滚动数字
		BYTE &byCellNum = m_arScoreNum[m_nScrollIndex];
		byCellNum = (byCellNum+1)%10;
	}
	return TRUE;
}

//停止滚动数字
BOOL CNumberControl::FinishScrollNumber()
{
	if( !m_bMoving && m_lScore == 0 ) return FALSE;

	//如果动画未完成
	if( m_arScoreNum.GetCount() >= m_nScrollIndex+1 && m_nScrollCount > 0 )
	{
		//设置当前滚动数字
		BYTE &byCellNum = m_arScoreNum[m_nScrollIndex];
		byCellNum = (byCellNum+m_nScrollCount)%10;
	}
	//如果动画未开始
	else if( m_arScoreNum.GetCount() == 0 )
	{
		LONGLONG lScore = m_lScore;
		BYTE byCellNum = 0;
		do
		{
			if( lScore != 0 )
				byCellNum = BYTE(lScore%10);
			lScore /= 10L;
			m_arScoreNum.Add(byCellNum);
		}while( lScore != 0 );
	}

	//设置滚动索引
	m_nScrollIndex = m_arScoreNum.GetCount()-1;

	//置滚动步数0
	m_nScrollCount = 0;

	//置偏移0
	m_nYExcusion = 0;

	m_bMoving = FALSE;

	//设置更新绘画区域
	m_rcDraw.SetRectEmpty();

	return TRUE;
}

//重置控件
VOID CNumberControl::ResetControl()
{
	//重置变量
	m_lScore = 0;
	m_nScrollCount = 0;
	m_nScrollIndex = 0;
	m_nYExcusion = 0;
	m_arScoreNum.RemoveAll();
	m_rcDraw.SetRectEmpty();
	m_bMoving = FALSE;
}

//绘画控件
VOID CNumberControl::DrawNumberControl( CDC *pDC )
{
	//获取数字
	if( m_arScoreNum.GetCount() == 0 ) return;
	
	//定义绘画点
	CPoint ptOrg;
	GetOriginPoint(ptOrg);
	INT nXPos = ptOrg.x;
	INT nYPos = ptOrg.y;

	//绘画正负
	INT nScoreBackWidth = m_PngScoreBack.GetWidth()/4;
	INT nScoreBackHeight = m_PngScoreBack.GetHeight();
	m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
		(m_lScore<0L)?nScoreBackWidth:0,0);
	nXPos += nScoreBackWidth;

	INT nNumWidth = m_PngScoreNum.GetWidth()/10;
	INT nNumHeight = m_PngScoreNum.GetHeight();

	//绘画数字
	for( INT_PTR i = m_arScoreNum.GetCount()-1; i >= 0; i-- )
	{
		//如果大于或小于当前滚动索引,画0
		if( i > m_nScrollIndex || i < m_nScrollIndex )
		{
			m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
				nScoreBackWidth*(m_lScore<0L?3:2),0);

			//单元数字
			BYTE byCellNum = 0;
			if( i < m_nScrollIndex )
				byCellNum = m_arScoreNum[i];
			m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4,nNumWidth,nNumHeight,byCellNum*nNumWidth,0);
		}
		//如果等于当前滚动索引
		else
		{
			m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
				nScoreBackWidth*(m_lScore<0L?3:2),0);

			//偏移
			INT nYExcusion = m_nYExcusion*nNumHeight/MAX_EXCUSION;
			BYTE byCellNum = m_arScoreNum[i];
			if( nNumHeight - nYExcusion > 0 )
				m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4,nNumWidth,nNumHeight-nYExcusion,byCellNum*nNumWidth,nYExcusion);
			byCellNum = (byCellNum+1)%10;
			if( nYExcusion > 0 )
				m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4+nNumHeight-nYExcusion,nNumWidth,nYExcusion,byCellNum*nNumWidth,0);
		}
		nXPos += nNumWidth;
	}
}

//获取绘画初始位置
VOID CNumberControl::GetOriginPoint( CPoint &OriginPoint )
{
	//获取位置
	CSize ControlSize;
	ControlSize.cy = m_PngScoreBack.GetHeight();
	ControlSize.cx = m_PngScoreBack.GetWidth()/4*2+(LONG)((m_arScoreNum.GetCount()-1)*m_PngScoreNum.GetWidth()/10);

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ OriginPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}
	OriginPoint.y = m_BenchmarkPos.y;
	return;
}

//获取更新绘画区域
VOID CNumberControl::GetDrawRect( CRect &rc )
{
	//如果数字索引为0,获取整个控件区域
	if( m_nScrollIndex == 0 )
	{
		CPoint ptOrg;
		GetOriginPoint(ptOrg);
		CSize ControlSize;
		ControlSize.cy = m_PngScoreBack.GetHeight();
		ControlSize.cx = m_PngScoreBack.GetWidth()/4*2+(LONG)((m_arScoreNum.GetCount()-1)*m_PngScoreNum.GetWidth()/10);
		rc.SetRect(ptOrg.x,ptOrg.y,ptOrg.x+ControlSize.cx,ptOrg.y+ControlSize.cy);
	}
	//获取滚动区域
	else if( m_arScoreNum.GetCount() >= m_nScrollIndex+1 && m_nScrollCount > 0 )
	{
		CPoint ptOrg;
		GetOriginPoint(ptOrg);
		INT nScoreBackWidht = m_PngScoreBack.GetWidth()/4;
		INT nScoreBackHeith = m_PngScoreBack.GetHeight();
		INT nScoreWidth = m_PngScoreNum.GetWidth()/10;
		INT nScoreHeight = m_PngScoreNum.GetHeight();
		ptOrg.Offset(nScoreBackWidht+nScoreWidth*(INT)(m_arScoreNum.GetCount()-m_nScrollIndex-1),0);
		rc.SetRect(ptOrg.x,ptOrg.y,ptOrg.x+nScoreWidth,ptOrg.y+nScoreBackHeith);
		m_rcDraw = rc;
	}
	else
	{
		rc = m_rcDraw;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////