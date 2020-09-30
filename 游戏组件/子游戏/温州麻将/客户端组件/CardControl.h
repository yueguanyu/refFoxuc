#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameLogic.h"
#include "cassert"

//////////////////////////////////////////////////////////////////////////

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//方向枚举
enum enDirection
{
	Direction_North=0,					//北向
	Direction_West,						//西向
	Direction_South,					//南向
	Direction_East,						//东向
};

//绘画偏移,用于堆牌,手上牌的绘画
#define Y_USER_LEFT_EXCUSION		25									//显示距离
#define Y_USER_RIGHT_EXCUSION		25									//显示距离
#define Y_TABLE_LEFT_EXCUSION		25									//显示距离
#define Y_TABLE_BOTTOM_EXCUSION		36									//显示距离
#define Y_TABLE_RIGHT_EXCUSION		25									//显示距离
#define Y_TABLE_TOP_EXCUSION		36									//显示距离
#define Y_HEAP_DOUBLE_V_EXCUSION	24									//显示距离

//////////////////////////////////////////////////////////////////////////

//扑克图片
class CCardListImage
{
	//位置变量
protected:
	int								m_nItemWidth;						//子项高度
	int								m_nItemHeight;						//子项宽度

	//资源变量
protected:
	CPngImageEx						m_CardListImage;					//图片资源
	CPngImageEx						m_ImageCardShadow;					//牌阴影图片

	//函数定义
public:
	//构造函数
	CCardListImage();
	//析构函数
	virtual ~CCardListImage();

	//信息函数
public:
	//获取宽度
	int GetViewWidth() { return m_nItemWidth; }
	//获取高度
	int GetViewHeight() { return m_nItemHeight; }

	//资源管理
public:
	//加载资源
	bool LoadResource(CWnd* pWnd,HINSTANCE hInstance,LPCTSTR pszResourceCard,LPCTSTR pszResourceShadow);
	//释放资源
	bool DestroyResource();

	//功能函数
public:
	//获取位置
	inline int GetImageIndex(BYTE cbCardData);
	//绘画扑克
	inline bool DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest, bool bShadow);
	//绘画扑克
	 bool DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest, bool bShadow, int nDstW, int nDstH);
};

//////////////////////////////////////////////////////////////////////////

//扑克资源
class CCardResource
{
	//用户扑克
public:
	CPngImageEx						m_ImageUserTop;						//图片资源
	CPngImageEx						m_ImageUserLeft;					//图片资源
	CPngImageEx						m_ImageUserRight;					//图片资源
	CCardListImage					m_ImageUserBottom;					//图片资源
	CCardListImage					m_ImageUserBtGrayed;				//图片资源

	//桌子扑克
public:
	CCardListImage					m_ImageTableTop;					//图片资源
	CCardListImage					m_ImageTableLeft;					//图片资源
	CCardListImage					m_ImageTableRight;					//图片资源
	CCardListImage					m_ImageTableBottom;					//图片资源

	//扑克阴影
public:
	CPngImageEx						m_ImageUserTopShadow;				//图片资源
	CPngImageEx						m_ImageUserLeftShadow;				//图片资源
	CPngImageEx						m_ImageUserRightShadow;				//图片资源
	CPngImageEx						m_ImageHeapSingleVShadow;			//图片资源
	CPngImageEx						m_ImageHeapSingleHShadow;			//图片资源
	CPngImageEx						m_ImageHeapDoubleVShadow;			//图片资源
	CPngImageEx						m_ImageHeapDoubleHShadow;			//图片资源

	//牌堆扑克
public:
	CPngImageEx						m_ImageHeapSingleV;					//图片资源
	CPngImageEx						m_ImageHeapSingleH;					//图片资源
	CPngImageEx						m_ImageHeapDoubleV;					//图片资源
	CPngImageEx						m_ImageHeapDoubleH;					//图片资源

	//函数定义
public:
	//构造函数
	CCardResource();
	//析构函数
	virtual ~CCardResource();

	//功能函数
public:
	//加载资源
	bool LoadResource(CWnd* pWnd);
	//消耗资源
	bool DestroyResource();
};

//////////////////////////////////////////////////////////////////////////

//堆立扑克
class CHeapCard
{
	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//扑克变量
protected:
	WORD							m_wFullCount;						//全满数目
	WORD							m_wMinusHeadCount;					//头部空缺
	WORD							m_wMinusLastCount;					//尾部空缺

	BYTE							m_cbCardData[HEAP_FULL_COUNT];		//牌值

	//函数定义
public:
	//构造函数
	CHeapCard();
	//析构函数
	virtual ~CHeapCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetHeapCardInfo(WORD wMinusHeadCount, WORD wMinusLastCount);
	//设置翻牌,wIndex从牌尾的上面那只牌算起
	void SetCardData( WORD wIndex, BYTE cbCardData );
	//获取出牌位置
	CPoint GetDispatchCardPos( bool bTail );
	//获取出牌位置,参数wIndex表示从牌尾上面那只牌算起
	CPoint GetDispatchCardPos( WORD wIndex );
	//从牌堆移除牌,bTail指是否从牌尾移除
	bool DeductionHeapCard( bool bTail, bool & bCgMagciPos);
	//重置翻牌
	void ResetCardData();
	//设置牌为空,wIndex从牌尾的上面那只算起
	void SetCardEmpty( WORD wIndex, BYTE cbEmptyCount );
	//是否有财神
	bool HaveMagicCard();
	//是否空的
	bool IsEmpty()	 { return (m_wMinusHeadCount+m_wMinusLastCount>=HEAP_FULL_COUNT); };
	//位置是否空的	(从尾部数起)
	bool IsNotEmptyPos(BYTE bPosCount);

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//组合扑克
class CWeaveCard
{
	//控制变量
protected:
	bool							m_bDisplayItem;						//显示标志
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向		
	BYTE							m_cbDirectionCardPos;				//方向扑克	
	CSize							m_sizeImageDirection;				//操作方向图大小

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[4];					//扑克数据

	//图片资源
protected:
	CPngImageEx						m_ImageDirectionPos;				//操作方向

	//函数定义
public:
	//构造函数
	CWeaveCard();
	//析构函数
	virtual ~CWeaveCard();

	//功能函数
public:
	//加载资源
	bool LoadResource(CWnd* pWnd);
	//绘画扑克
	void DrawCardControl(CDC * pDC, bool bShadow);
	//绘画扑克
	void DrawCardControl(CDC * pDC, int nXPos, int nYPos, bool bShadow);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//控件控制
public:
	//设置显示
	void SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
	//方向牌
	void SetDirectionCardPos(BYTE cbPos) { m_cbDirectionCardPos = cbPos;}

	//内部函数
private:
	//获取扑克
	inline BYTE GetCardData(WORD wIndex);
};

//////////////////////////////////////////////////////////////////////////

//用户扑克
class CUserCard
{
	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	bool							m_bCurrentCard;						//当前标志

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CUserCard();
	//析构函数
	virtual ~CUserCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetCurrentCard(bool bCurrentCard);
	//设置扑克
	bool SetCardData(WORD wCardCount, bool bCurrentCard);
	//添加扑克
	bool AddCardData(bool bCurrentCard);
	//获取牌数
	WORD GetCardCount() { return m_wCardCount; }
	//是否有当前牌
	bool IsHaveCurrentCard() { return m_bCurrentCard; }
	//获取出牌位置
	CPoint GetDispatchCardPos();

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//丢弃牌
struct tagDiscardItem 
{
	BYTE				cbCardData;										//扑克数据
	CPoint				ptExcusion;										//相对基准位置
};

//丢弃牌模板
typedef	CWHArray<tagDiscardItem>	CDiscardItemArray;

//丢弃扑克
class CDiscardCard
{
	//扑克数据
protected:
	BYTE							m_cbCurrentMaxCount;				//当前最大排列数目
	BYTE							m_cbCurrentCount;					//当前排数
	CDiscardItemArray				m_ItemArray;						//扑克数组

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CDiscardCard();
	//析构函数
	virtual ~CDiscardCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//增加扑克
	bool AddCardItem(BYTE cbCardData);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);
	//获取最后一只牌位置
	CPoint GetLastCardPosition();
	//获取出牌位置
	CPoint GetDispatchCardPos();
	//从丢弃牌中移除牌,cbRemoveCount指移除数目
	void RemoveCard( BYTE cbRemoveCount );
	//获取牌数
	INT_PTR GetCardCount() { return m_ItemArray.GetCount(); }

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction);
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//桌面扑克
class CTableCard
{
	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[MAX_COUNT];			//扑克数据

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CTableCard();
	//析构函数
	virtual ~CTableCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	bool							bGrayed;							//
	BYTE							cbCardData;							//扑克数据
};

//选择扑克信息
struct tagSelectCardInfo
{
	WORD							wActionMask;						//操作码
	BYTE							cbActionCard;						//操作牌
	BYTE							cbCardCount;						//选择数目
	BYTE							cbCardData[MAX_COUNT];				//选择牌
};

//扑克控件
class CCardControl
{
	//状态变量
protected:
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayItem;						//显示标志
	bool							m_bSelectMode;						//选择模式
	BYTE							m_cbSelectInfoCount;				//选择扑克组合数目
	tagSelectCardInfo				m_SelectCardInfo[MAX_COUNT];		//选择扑克组合信息
	CRect							m_rcSelectCard[MAX_COUNT];			//选择扑克组合区域
	BYTE							m_cbCurSelectIndex;					//当前选择索引
	BYTE							m_cbMagicCard;						//财神扑克

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	WORD							m_wHoverItem;						//盘旋子项
	tagCardItem						m_CurrentCard;						//当前扑克
	tagCardItem						m_CardItemArray[MAX_COUNT-1];		//扑克数据

	//资源变量
protected:
	CSize							m_ControlSize;						//控件大小
	CPoint							m_ControlPoint;						//控件位置
	CPngImageEx						m_ImageCardShadow;					//阴影图片
	CBitImageEx						m_ImageBaida;						//百搭图片

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	bool LoadResource(CWnd* pWnd);

	//查询函数
public:
	//获取扑克
	BYTE GetHoverCard();
	//获取扑克
	BYTE GetCurrentCard() { return m_CurrentCard.cbCardData; }

	//扑克控制
public:
	//设置扑克
	bool SetCurrentCard(BYTE cbCardData);
	//设置扑克
	bool SetCurrentCard(tagCardItem CardItem);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard);
	//设置扑克
	bool SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount);
	//添加扑克
	bool AddCardData(const BYTE cbCardData[], WORD wCardCount);
	//设置财神
	void SetMagicCard(const BYTE cbMagicCard) { m_cbMagicCard = cbMagicCard; }
	//是否财神
	bool IsMagicCard(const BYTE cbMagicCard);

	//控件控制
public:
	//设置响应
	void SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//设置显示
	void SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//基准位置
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//设置操作模式
	void SetSelectMode( bool bSelectMode );
	//获取操作模式
	bool GetSelectMode() { return m_bSelectMode; }
	//设置所有牌是否灰色
	void SetAllCardGray( bool bGray );
	//设置牌是否灰色
	void SetCardGray( BYTE cbCardData[], BYTE cbCardCount, bool bGray );
	//获取灰色牌
	BYTE GetGrayCard( BYTE cbCardData[], BYTE cbMaxCount, bool bGetGray );
	//获取出牌位置
	CPoint GetDispatchCardPos();
	//获取玩家操作结果
	void GetUserSelectResult( tagSelectCardInfo &SelectInfo );

	//事件控制
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//光标消息
	bool OnEventSetCursor(CPoint Point, CRect &rcRePaint);
	//鼠标按下
	bool OnEventLeftHitCard();
	//玩家吃牌
	bool OnEventUserAction( const tagSelectCardInfo SelectInfo[MAX_COUNT], BYTE cbInfoCount );

	//内部函数
private:
	//索引切换
	WORD SwitchCardPoint(CPoint & MousePoint);
	//获取区域
	CRect GetCardRect( WORD wItemIndex );
};


//////////////////////////////////////////////////////////////////////////

//变量声明
//导出变量
extern CCardResource				g_CardResource;						//扑克资源

//////////////////////////////////////////////////////////////////////////

#endif
