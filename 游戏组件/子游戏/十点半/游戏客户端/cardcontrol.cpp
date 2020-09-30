#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define INVALID_ITEM				0xFFFF								//无效子项

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//状态变量
	m_bPositively=false;
	m_bDisplayHead=false;
	m_nXDistance=DEF_X_DISTANCE;

	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

	//位置变量
	m_YCollocateMode=enYTop;
	m_XCollocateMode=enXLeft;
	m_BenchmarkPos.SetPoint(0,0);

	//加载资源
	m_ImageCard.LoadFromResource(AfxGetInstanceHandle(),IDB_GAME_CARD);

	//获取大小
	m_CardSize.SetSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//获取扑克
WORD CCardControl::GetCardData(BYTE cbCardData[], WORD wBufferCount)
{
	//效验参数
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//拷贝扑克
	CopyMemory(cbCardData,m_cbCardData,sizeof(BYTE)*m_wCardCount);

	return m_wCardCount;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData)) return false;

	//设置变量
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(BYTE)*m_wCardCount);

	return true;
}

//基准位置
VOID CCardControl::SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//基准位置
VOID CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//绘画扑克
VOID CCardControl::DrawCardControl(CDC * pDC)
{
	//获取位置
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//绘画扑克
	for (WORD i=0;i<m_wCardCount;i++)
	{
		//变量定义
		INT nXImagePos=0,nYImagePos=0;
		BYTE cbCardData=m_cbCardData[i];

		//图片位置
		if ((cbCardData!=0)&&(cbCardData!=0xFF)&&((i>0)||(m_bDisplayHead==true)))
		{
			if( cbCardData == 0x4e || cbCardData == 0x4f )
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)%14)*m_CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
			}
			else
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
			}
		}
		else
		{
			nXImagePos=m_CardSize.cx*2;
			nYImagePos=m_CardSize.cy*4;
		}

		//绘画扑克
		m_ImageCard.TransDrawImage(pDC,OriginPoint.x+m_nXDistance*i,OriginPoint.y,m_CardSize.cx,m_CardSize.cy,nXImagePos,nYImagePos,RGB(255,0,255));
	}

	return;
}

//光标消息
bool CCardControl::OnEventSetCursor(CPoint Point)
{
	//光标处理
	if (m_bPositively==true)
	{
		//获取索引
		WORD wHoverItem=SwitchCardPoint(Point);

		//更新判断
		if (wHoverItem<1)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return true;
		}
	}

	return false;
}

//鼠标消息
bool CCardControl::OnEventLeftMouseDown(CPoint Point)
{
	//光标处理
	if ((m_bPositively==true)&&(SwitchCardPoint(Point)<1))
	{
		//设置变量
		m_bDisplayHead=!m_bDisplayHead;

		return true;
	}

	return false;
}

//获取大小
VOID CCardControl::GetControlSize(CSize & ControlSize)
{
	//获取大小
	ControlSize.cy=m_CardSize.cy;
	ControlSize.cx=(m_wCardCount>0)?(m_CardSize.cx+(m_wCardCount-1)*m_nXDistance):0;

	return;
}

//索引切换
WORD CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//获取位置
	CSize ControlSize;
	CPoint OriginPoint;
	GetControlSize(ControlSize);
	GetOriginPoint(OriginPoint);

	//基准位置
	INT nXPos=MousePoint.x-OriginPoint.x;
	INT nYPos=MousePoint.y-OriginPoint.y;

	//越界判断
	if ((nXPos<=0)||(nXPos>=ControlSize.cx)) return INVALID_ITEM;
	if ((nYPos<=0)||(nYPos>=ControlSize.cy)) return INVALID_ITEM;

	//计算索引
	WORD wCardIndex=nXPos/m_nXDistance;
	if (wCardIndex>=m_wCardCount) wCardIndex=(m_wCardCount-1);

	return wCardIndex;
}

//获取原点
VOID CCardControl::GetOriginPoint(CPoint & OriginPoint)
{
	//获取位置
	CSize ControlSize;
	GetControlSize(ControlSize);

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ OriginPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ OriginPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	return;
}

//获取牌尾位置
CPoint CCardControl::GetTailPos()
{
	CPoint ptTail;

	//计算大小
	INT nSpaceWidth = m_wCardCount*m_nXDistance;
	CSize ControlSize;
	GetControlSize(ControlSize);

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ ptTail.x=m_BenchmarkPos.x+nSpaceWidth; break; }
	case enXCenter: { ptTail.x=m_BenchmarkPos.x-ControlSize.cx/2+nSpaceWidth; break; }
	case enXRight:	{ ptTail.x=m_BenchmarkPos.x-ControlSize.cx+nSpaceWidth; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ ptTail.y=m_BenchmarkPos.y; break; }
	case enYCenter: { ptTail.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { ptTail.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	return ptTail;
}

//获取牌头位置
CPoint CCardControl::GetHeadPos()
{
	CPoint ptHead;

	//计算大小
	CSize ControlSize;
	GetControlSize(ControlSize);

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ ptHead.x=m_BenchmarkPos.x; break; }
	case enXCenter: { ptHead.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ ptHead.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ ptHead.y=m_BenchmarkPos.y; break; }
	case enYCenter: { ptHead.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { ptHead.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}
	
	return ptHead;
}

//获取控件区域
VOID CCardControl::GetControlRect( CRect &rc )
{
	//计算大小
	CSize ControlSize;
	GetControlSize(ControlSize);
	
	CPoint ptHead;

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ ptHead.x=m_BenchmarkPos.x; break; }
	case enXCenter: { ptHead.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ ptHead.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ ptHead.y=m_BenchmarkPos.y; break; }
	case enYCenter: { ptHead.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { ptHead.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	rc.SetRect(ptHead.x,ptHead.y,ptHead.x+ControlSize.cx,ptHead.y+ControlSize.cy);
}

//////////////////////////////////////////////////////////////////////////
