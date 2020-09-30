#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////////////

//派发索引
#define DISPATCH_MAX_INDEX			6									//最大索引
#define DISPATCH_DELAY_INDEX		2									//延时索引

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CCardResource CCardControl::m_CardResource;								//扑克资源

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CCardResource::CCardResource()
{
	//设置变量
	m_CardSize.SetSize(0,0);

	return;
}

//析构函数
CCardResource::~CCardResource()
{
}

//加载资源
VOID CCardResource::Initialize(CD3DDevice * pD3DDevice)
{
	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_TextureCard.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_CARD"),TEXT("PNG"));
	m_TextureSmallCard.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_CARD_SMALL"),TEXT("PNG"));

	//获取大小
	m_CardSize.SetSize(m_TextureCard.GetWidth()/13,m_TextureCard.GetHeight()/5);
	m_CardSizeSmall.SetSize(m_TextureSmallCard.GetWidth()/13,m_TextureSmallCard.GetHeight()/5);

	return;
}

//获取大小
VOID CCardResource::GetCardSize(bool bSamllMode, CSize & CardSize)
{
	//设置变量
	CardSize=(bSamllMode==false)?m_CardSize:m_CardSizeSmall;

	return;
}

//获取资源
VOID CCardResource::GetCardTexture(bool bSmallMode, CD3DTexture * * pD3DTexture)
{
	//设置变量
	if (bSmallMode==false)
	{
		(*pD3DTexture)=&m_TextureCard;
	}
	else
	{
		(*pD3DTexture)=&m_TextureSmallCard;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//属性变量
	m_bSmallMode=false;
	m_bHorizontal=true;
	m_bPositively=false;
	m_bDisplayItem=false;
	m_cbBackGround=CARD_BOOR;

	//选择变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//出牌变量
	m_bOutCard=false;
	m_wOutCardIndex=0;

	//扑克数据
	m_cbCardCount=0;
	m_cbShowCount=MAX_CARD_COUNT;
	ZeroMemory(m_CardItemPoint,sizeof(m_CardItemPoint));
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	//位置变量
	m_YCollocateMode=enYTop;
	m_XCollocateMode=enXLeft;
	m_DispatchPos.SetPoint(0,0);
	m_BenchmarkPos.SetPoint(0,0);

	//间隔变量
	m_nXDistance=DEF_X_DISTANCE;
	m_nYDistance=DEF_Y_DISTANCE;
	m_nShootDistance=DEF_SHOOT_DISTANCE;

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//设置扑克
bool CCardControl::SetCardData(BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//设置变量
	m_cbCardCount=cbCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}

//设置扑克
bool CCardControl::SetCardData(BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//扑克数目
	m_cbCardCount=cbCardCount;

	//设置扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	return true;
}

//设置扑克
bool CCardControl::SetShootArea(BYTE cbHeadIndex, BYTE cbTailIndex)
{
	//设置变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//设置扑克
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if ((i>=cbHeadIndex)&&(i<=cbTailIndex))
		{
			m_CardItemArray[i].bShoot=!m_CardItemArray[i].bShoot;
		}
	}

	return true;
}

//设置扑克
bool CCardControl::SetSelectIndex(BYTE cbHeadIndex, BYTE cbTailIndex)
{
	//设置变量
	m_cbTailIndex=cbTailIndex;
	m_cbHeadIndex=cbHeadIndex;

	return true;
}

//设置扑克
bool CCardControl::SetShootCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	bool bChangeStatus=false;

	//收起扑克
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//弹起扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		for (BYTE j=0;j<m_cbCardCount;j++)
		{
			if ((m_CardItemArray[j].bShoot==false)&&(m_CardItemArray[j].cbCardData==cbCardData[i])) 
			{
				bChangeStatus=true;
				m_CardItemArray[j].bShoot=true;
				break;
			}
		}
	}

	return bChangeStatus;
}

//弹起相同点数
bool CCardControl::SetShootSameCard(BYTE cbCardData)
{
	//变量定义
	bool bChangeStatus=false;

	//收起扑克
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//弹起扑克
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if( m_GameLogic.GetCardValue(m_CardItemArray[i].cbCardData) == m_GameLogic.GetCardValue(cbCardData) )
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=true;
		}
	}

	return bChangeStatus;
}

//弹起顺子
bool CCardControl::SetShootOrderCard( BYTE cbCardOne, BYTE cbCardTwo )
{
	//变量定义
	bool bChangeStatus = false;

	//变量定义
	BYTE cbCradOneValue = m_GameLogic.GetCardLogicValue(cbCardOne);
	BYTE cbCradTwoValue = m_GameLogic.GetCardLogicValue(cbCardTwo);
	BYTE cbBeginCradValue = min(cbCradOneValue, cbCradTwoValue);
	BYTE cbEndCradValue = max(cbCradOneValue, cbCradTwoValue);
	BYTE cbOrderAllCount = cbEndCradValue - cbBeginCradValue + 1;

	//不成顺子
	if ( cbOrderAllCount < 5 )
		return false;

	//顺子保存
	BYTE cbOrderCard[MAX_CARD_COUNT];
	BYTE cbOrderCount = 0;
	ZeroMemory(cbOrderCard, sizeof(cbOrderCard));

	//原始数据
	BYTE cbCardData[MAX_CARD_COUNT];
	ZeroMemory(cbCardData, sizeof(cbCardData));

	//拷贝扑克
	for ( BYTE i = 0; i < m_cbCardCount; i++ )
		cbCardData[i] = m_CardItemArray[i].cbCardData;

	//排序
	m_GameLogic.SortCardList(cbCardData, m_cbCardCount, ST_ORDER);

	//寻找顺子
	for ( BYTE i = 0; i < m_cbCardCount; i++ )
	{
		if( m_GameLogic.GetCardLogicValue(cbCardData[i]) == cbEndCradValue - cbOrderCount )
		{
			cbOrderCard[cbOrderCount] = cbCardData[i];
			cbOrderCount++;

			if ( m_GameLogic.GetCardValue(cbCardData[i]) == 0x02 )
			{
				return false;
			}
			if ( cbOrderCount == cbOrderAllCount )
			{
				break;
			}
		}
	}

	//弹起顺子
	BYTE cbShootCount = 0;

	//检验顺子
	for (BYTE i = 0;i < cbOrderCount; i++)
	{
		for (BYTE j = 0;j < m_cbCardCount; j++)
		{
			if ( m_CardItemArray[j].cbCardData == cbOrderCard[i] ) 
			{
				cbShootCount++;
				break;
			}
		}
	}

	//弹起顺子
	if ( cbShootCount == cbOrderAllCount )
	{
		//收起扑克
		for ( BYTE i = 0; i < m_cbCardCount; i++ )
		{
			m_CardItemArray[i].bShoot = false;
		}

		//弹起顺子
		for (BYTE i = 0;i < cbOrderCount; i++)
		{
			for (BYTE j = 0;j < m_cbCardCount; j++)
			{
				if ( m_CardItemArray[j].cbCardData == cbOrderCard[i] ) 
				{
					m_CardItemArray[j].bShoot = true;
					break;
				}
			}
		}

		return true;
	}

	return false;
}

//派发扑克
bool CCardControl::ShowOutCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT((cbCardCount>0)&&(cbCardCount<=CountArray(m_CardItemArray)));
	if ((cbCardCount==0)||(cbCardCount>CountArray(m_CardItemArray))) return false;

	//设置状态
	m_bOutCard=true;
	m_wOutCardIndex=0;

	//设置变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//扑克数目
	m_cbCardCount=cbCardCount;

	//设置扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	//启动时间
	m_OutCardLapseCount.Initialization();

	return true;
}

//删除扑克
bool CCardControl::RemoveShootItem()
{
	//设置变量
	m_cbTailIndex=INVALID_ITEM;
	m_cbHeadIndex=INVALID_ITEM;

	//保存扑克
	tagCardItem	CardItemArray[MAX_CARD_COUNT];
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_cbCardCount);

	//删除扑克
	WORD wRemoveCount=0;
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if (CardItemArray[i].bShoot==true)
		{
			wRemoveCount++;
			CardItemArray[i].cbCardData=0x00;
		}
	}

	//设置扑克
	if (wRemoveCount>0)
	{
		//设置扑克
		BYTE cbInsertCount=0;
		for (BYTE i=0;i<m_cbCardCount;i++)
		{
			if (CardItemArray[i].cbCardData!=0x00)
			{
				m_CardItemArray[cbInsertCount++]=CardItemArray[i];
			}
		}

		//设置变量
		m_cbCardCount=m_cbCardCount-wRemoveCount;
	}

	return true;
}

//移动扑克
bool CCardControl::MoveCardItem(BYTE cbTargerItem)
{
	//效验参数
	ASSERT(cbTargerItem<m_cbCardCount);
	if (cbTargerItem>=m_cbCardCount) return false;

	//统计扑克
	BYTE cbShootCount=0;
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			cbShootCount++;
		}
	}

	//移动扑克
	if ((cbShootCount>0)&&(cbShootCount<m_cbCardCount))
	{
		//调整索引
		cbTargerItem=__min(m_cbCardCount-cbShootCount,cbTargerItem);

		//变量定义
		BYTE cbShootIndex=cbTargerItem;
		BYTE cbNormalIndex=(cbTargerItem==0)?(cbTargerItem+cbShootCount):0;

		//提取扑克
		tagCardItem CardItemTemp[MAX_CARD_COUNT];
		CopyMemory(CardItemTemp,m_CardItemArray,sizeof(tagCardItem)*m_cbCardCount);

		//调整扑克
		for (BYTE i=0;i<m_cbCardCount;i++)
		{
			if (CardItemTemp[i].bShoot==false)
			{
				//设置扑克
				m_CardItemArray[cbNormalIndex++]=CardItemTemp[i];

				//调整索引
				if (cbNormalIndex==cbTargerItem) cbNormalIndex=cbTargerItem+cbShootCount;
			}
			else
			{
				//设置扑克
				m_CardItemArray[cbShootIndex++]=CardItemTemp[i];
			}
		}

		return true;
	}

	return false;
}

//交换扑克
bool CCardControl::SwitchCardItem(BYTE cbSourceItem, BYTE cbTargerItem)
{
	//效验参数
	if (cbSourceItem==cbTargerItem) return false;
	if ((cbSourceItem>=m_cbCardCount)||(cbTargerItem>=m_cbCardCount)) return false;

	//保存扑克
	tagCardItem CardItem=m_CardItemArray[cbSourceItem];

	//移动扑克
	if (cbSourceItem>cbTargerItem)
	{
		BYTE cbMoveCount=cbSourceItem-cbTargerItem;
		MoveMemory(&m_CardItemArray[cbTargerItem+1],&m_CardItemArray[cbTargerItem],sizeof(tagCardItem)*cbMoveCount);
	}
	else
	{
		BYTE cbMoveCount=cbTargerItem-cbSourceItem;
		MoveMemory(&m_CardItemArray[cbSourceItem],&m_CardItemArray[cbSourceItem+1],sizeof(tagCardItem)*cbMoveCount);
	}

	//插入目标
	m_CardItemArray[cbTargerItem]=CardItem;

	return true;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromIndex(BYTE cbIndex)
{
	return (cbIndex<m_cbCardCount)?&m_CardItemArray[cbIndex]:NULL;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromPoint(CPoint & MousePoint)
{
	WORD cbIndex=SwitchCardPoint(MousePoint);
	return (cbIndex!=INVALID_ITEM)?&m_CardItemArray[cbIndex]:NULL;
}

//获取扑克
BYTE CCardControl::GetCardData(BYTE cbCardData[], BYTE cbBufferCount)
{
	//效验参数
	ASSERT(cbBufferCount>=m_cbCardCount);
	if (cbBufferCount<m_cbCardCount) return 0;

	//拷贝扑克
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		cbCardData[i]=m_CardItemArray[i].cbCardData;
	}

	return m_cbCardCount;
}

//获取扑克
BYTE CCardControl::GetShootCard(BYTE cbCardData[], BYTE cbBufferCount)
{
	//变量定义
	BYTE cbShootCount=0;

	//拷贝扑克
	for (BYTE i=0;i<m_cbCardCount;i++) 
	{
		//效验参数
		ASSERT(cbBufferCount>cbShootCount);
		if (cbBufferCount<=cbShootCount) break;

		//拷贝扑克
		if (m_CardItemArray[i].bShoot==true) cbCardData[cbShootCount++]=m_CardItemArray[i].cbCardData;
	}

	return cbShootCount;
}

//设置显示
VOID CCardControl::SetShowCount(BYTE cbShowCount)
{
	//设置变量
	m_cbShowCount=cbShowCount;

	return;
}

//设置背景
VOID CCardControl::SetBackGround(BYTE cbBackGround)
{
	//设置变量
	m_cbBackGround=cbBackGround;

	return;
}

//设置距离
VOID CCardControl::SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance)
{
	//设置变量
	m_nXDistance=nXDistance;
	m_nYDistance=nYDistance;
	m_nShootDistance=nShootDistance;

	return;
}

//获取中心
VOID CCardControl::GetCenterPoint(CPoint & CenterPoint)
{
	//获取原点
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//获取位置
	CSize ControlSize;
	GetControlSize(ControlSize);

	//设置中心
	CenterPoint.x=OriginPoint.x+ControlSize.cx/2;
	CenterPoint.y=OriginPoint.y+ControlSize.cy/2;

	return;
}

//派发位置
VOID CCardControl::SetDispatchPos(INT nXPos, INT nYPos)
{
	//设置坐标
	if (m_bSmallMode==false)
	{
		m_DispatchPos.x=nXPos-m_CardResource.m_CardSize.cx/2;
		m_DispatchPos.y=nYPos-m_CardResource.m_CardSize.cy/2;
	}
	else
	{
		m_DispatchPos.x=nXPos-m_CardResource.m_CardSizeSmall.cx/2;
		m_DispatchPos.y=nYPos-m_CardResource.m_CardSizeSmall.cy/2;
	}

	return;
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

//执行动画
VOID CCardControl::CartoonMovie()
{
	if ((m_bOutCard==true)&&(m_OutCardLapseCount.GetLapseCount(20)>0L))
	{
		//设置变量
		m_wOutCardIndex++;

		//停止判断
		if ((m_cbCardCount==0)||(m_wOutCardIndex>=((m_cbCardCount-1)*DISPATCH_DELAY_INDEX+DISPATCH_MAX_INDEX)))
		{
			//设置变量
			m_bOutCard=false;
			m_wOutCardIndex=0L;

			//发送消息
			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
			if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_OUT_CARD_FINISH,(WPARAM)this,0L);
		}
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
		BYTE cbHoverItem=SwitchCardPoint(Point);

		//更新判断
		if (cbHoverItem!=INVALID_ITEM)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return true;
		}
	}

	return false;
}

//绘画扑克
VOID CCardControl::DrawCardControl(CD3DDevice * pD3DDevice)
{
	//获取位置
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//控制位置
	CSize ControlSize;
	GetControlSize(ControlSize);

	//变量定义
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;

	//资源变量
	CSize CardSize;
	CD3DTexture * pD3DTexture=NULL;
	m_CardResource.GetCardSize(m_bSmallMode,CardSize);
	m_CardResource.GetCardTexture(m_bSmallMode,&pD3DTexture);

	//绘画扑克
	for (BYTE i=0;i<m_cbCardCount;i++)
	{
		//获取扑克
		bool bShoot=m_CardItemArray[i].bShoot;
		BYTE cbCardData=m_CardItemArray[i].cbCardData;

		//间隙过滤
		if (i>=m_cbShowCount) continue;
		if (cbCardData==SPACE_CARD_DATA) continue;

		//图片位置
		if ((m_bDisplayItem==true)&&(cbCardData!=0))
		{
			if ((cbCardData==0x4E)||(cbCardData==0x4F))
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)%14)*CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*CardSize.cy;
			}
			else
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*CardSize.cy;
			}
		}
		else
		{
			nYImagePos=CardSize.cy*4;
			nXImagePos=CardSize.cx*(2+m_cbBackGround);
		}

		//屏幕位置
		if (m_bHorizontal==true)
		{
			nXDrawPos=m_nXDistance*i;
			nYDrawPos=(bShoot==false)?m_nShootDistance:0;
		}
		else
		{
			nXDrawPos=0;
			nYDrawPos=m_nYDistance*i;
		}

		//屏幕输出
		if (m_bOutCard==true)
		{
			//过程位置
			if (m_wOutCardIndex>=(i*DISPATCH_DELAY_INDEX))
			{
				//计算索引
				INT nIndex=__min(DISPATCH_MAX_INDEX,m_wOutCardIndex-(i*DISPATCH_DELAY_INDEX));

				//输出位置
				INT nXSource=m_DispatchPos.x;
				INT nYSource=m_DispatchPos.y;
				INT nXDispatch=nXSource+(OriginPoint.x+nXDrawPos-nXSource)*nIndex/DISPATCH_MAX_INDEX;
				INT nYDispatch=nYSource+(OriginPoint.y+nYDrawPos-nYSource)*nIndex/DISPATCH_MAX_INDEX;

				//绘画扑克
				pD3DTexture->DrawImage(pD3DDevice,nXDispatch,nYDispatch,CardSize.cx,CardSize.cy,nXImagePos,nYImagePos);
			}
		}
		else
		{
			//绘画扑克
			pD3DTexture->DrawImage(pD3DDevice,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,CardSize.cx,CardSize.cy,nXImagePos,nYImagePos);

			//绘画选择
			if ((m_cbHeadIndex<=i)&&(i<=m_cbTailIndex))
			{
				pD3DTexture->DrawImage(pD3DDevice,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,CardSize.cx,CardSize.cy,CardSize.cx*12,CardSize.cy*4);
			}
		}
	}

	return;
}

//获取大小
VOID CCardControl::GetControlSize(CSize & ControlSize)
{
	//扑克大小
	CSize CardSize;
	m_CardResource.GetCardSize(m_bSmallMode,CardSize);

	//获取大小
	if (m_bHorizontal==true)
	{
		ControlSize.cy=CardSize.cy+m_nShootDistance;
		ControlSize.cx=(m_cbCardCount>0)?(CardSize.cx+(m_cbCardCount-1)*m_nXDistance):0;
	}
	else
	{
		ControlSize.cx=CardSize.cx;
		ControlSize.cy=(m_cbCardCount>0)?(CardSize.cy+(m_cbCardCount-1)*m_nYDistance):0;
	}

	return;
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

//索引切换
BYTE CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//横向模式
	if ((m_bHorizontal==true)&&(m_cbCardCount>0))
	{
		//获取位置
		CSize ControlSize;
		CPoint OriginPoint;
		GetControlSize(ControlSize);
		GetOriginPoint(OriginPoint);

		//扑克大小
		CSize CardSize;
		m_CardResource.GetCardSize(m_bSmallMode,CardSize);

		//基准位置
		INT nXPos=MousePoint.x-OriginPoint.x;
		INT nYPos=MousePoint.y-OriginPoint.y;

		//越界判断
		if ((nXPos<0)||(nXPos>ControlSize.cx)) return INVALID_ITEM;
		if ((nYPos<0)||(nYPos>ControlSize.cy)) return INVALID_ITEM;

		//计算索引
		BYTE cbCardIndex=nXPos/m_nXDistance;
		if (cbCardIndex>=m_cbCardCount) cbCardIndex=(m_cbCardCount-1);

		//扑克搜索
		for (BYTE i=0;i<=cbCardIndex;i++)
		{
			//变量定义
			BYTE cbCurrentIndex=cbCardIndex-i;

			//显示判断
			if (cbCurrentIndex>=m_cbShowCount) continue;

			//横向测试
			if (nXPos>(INT)(cbCurrentIndex*m_nXDistance+CardSize.cx)) break;

			//竖向测试
			bool bShoot=m_CardItemArray[cbCurrentIndex].bShoot;
			if ((bShoot==true)&&(nYPos<=CardSize.cy)) return cbCurrentIndex;
			if ((bShoot==false)&&(nYPos>=(INT)m_nShootDistance)) return cbCurrentIndex;
		}
	}

	return INVALID_ITEM;
}

//加载资源
VOID CCardControl::LoadCardResource(CD3DDevice * pD3DDevice)
{
	//加载资源
	m_CardResource.Initialize(pD3DDevice);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
