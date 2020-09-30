#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//宏定义

//牌背定义
#define CARD_LAND					0									//地主底面
#define CARD_BOOR					1									//农民底面

//常量定义
#define INVALID_ITEM				0xFF								//无效子项

//属性定义
#define MAX_CARD_COUNT				20									//扑克数目
#define SPACE_CARD_DATA				255									//间距扑克

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//间距定义
#define DEF_X_DISTANCE				22									//默认间距
#define DEF_Y_DISTANCE				18									//默认间距
#define DEF_SHOOT_DISTANCE			20									//默认间距

//间距定义
#define DEF_X_DISTANCE_SMALL		18									//默认间距
#define DEF_Y_DISTANCE_SMALL		17									//默认间距
#define DEF_SHOOT_DISTANCE_SMALL	20									//默认间距

//辅助消息
#define IDM_OUT_CARD_FINISH			(WM_USER+400)						//出牌完成

//////////////////////////////////////////////////////////////////////////////////
//枚举定义

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

//////////////////////////////////////////////////////////////////////////////////

//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};

//////////////////////////////////////////////////////////////////////////////////

//扑克资源
class CCardResource
{
	//友元定义
	friend class CCardControl;

	//扑克资源
protected:
	CSize							m_CardSize;							//扑克大小
	CSize							m_CardSizeSmall;					//扑克大小

	//扑克资源
protected:
	CD3DTexture						m_TextureCard;						//图片资源
	CD3DTexture						m_TextureSmallCard;					//图片资源

	//函数定义
public:
	//构造函数
	CCardResource();
	//析构函数
	virtual ~CCardResource();

	//资源管理
public:
	//加载资源
	VOID Initialize(CD3DDevice * pD3DDevice);
	//获取大小
	VOID GetCardSize(bool bSamllMode, CSize & CardSize);
	//获取资源
	VOID GetCardTexture(bool bSmallMode, CD3DTexture * * pD3DTexture);
};

//////////////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl
{
	//属性变量
protected:
	bool							m_bSmallMode;						//小牌模式
	bool							m_bHorizontal;						//显示方向
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayItem;						//显示标志
	BYTE							m_cbBackGround;						//背景标志

	//选择变量
protected:
	BYTE							m_cbHeadIndex;						//开始索引
	BYTE							m_cbTailIndex;						//结束索引

	//出牌变量
protected:
	bool							m_bOutCard;							//出牌标志
	WORD							m_wOutCardIndex;					//出牌索引
	CLapseCount						m_OutCardLapseCount;				//流逝计数

	//间隔变量
protected:
	UINT							m_nXDistance;						//横向间隔
	UINT							m_nYDistance;						//竖向间隔
	UINT							m_nShootDistance;					//弹起高度

	//位置变量
protected:
	CPoint							m_DispatchPos;						//派发位置
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//扑克数据
protected:
	BYTE							m_cbCardCount;						//扑克数目
	BYTE							m_cbShowCount;						//显示数目
	CPoint							m_CardItemPoint[MAX_CARD_COUNT];	//扑克位置
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//扑克数据

	//逻辑变量
protected:
	CGameLogic						m_GameLogic;						//逻辑变量

	//资源变量
public:
	static CCardResource			m_CardResource;						//扑克资源

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//设置扑克
public:
	//设置扑克
	bool SetCardData(BYTE cbCardCount);
	//设置扑克
	bool SetCardData(BYTE cbCardData[], BYTE cbCardCount);

	//弹起扑克
public:
	//设置扑克
	bool SetShootArea(BYTE cbHeadIndex, BYTE cbTailIndex);
	//设置扑克
	bool SetShootCard(BYTE cbCardData[], BYTE cbCardCount);
	//弹起相同点数
	bool SetShootSameCard(BYTE cbCardData);
	//弹起顺子
	bool SetShootOrderCard( BYTE cbCardOne, BYTE cbCardTwo );

	//派发扑克
public:
	//是否出牌
	bool IsOutCard() { return m_bOutCard; }
	//设置出牌
	bool ShowOutCard(BYTE cbCardData[], BYTE cbCardCount);

	//扑克控制
public:
	//删除扑克
	bool RemoveShootItem();
	//移动扑克
	bool MoveCardItem(BYTE cbTargerItem);
	//选择扑克
	bool SetSelectIndex(BYTE cbStartIndex, BYTE cbEndIndex);
	//交换扑克
	bool SwitchCardItem(BYTE cbSourceItem, BYTE cbTargerItem);

	//获取扑克
public:
	//获取扑克
	tagCardItem * GetCardFromIndex(BYTE cbIndex);
	//获取扑克
	tagCardItem * GetCardFromPoint(CPoint & MousePoint);

	//获取扑克
public:
	//扑克数目
	BYTE GetCardCount() { return m_cbCardCount; }
	//获取扑克
	BYTE GetCardData(BYTE cbCardData[], BYTE cbBufferCount);
	//获取扑克
	BYTE GetShootCard(BYTE cbCardData[], BYTE cbBufferCount);

	//状态查询
public:
	//查询方向
	bool GetDirection() { return m_bHorizontal; }
	//查询响应
	bool GetPositively() { return m_bPositively; }
	//查询显示
	bool GetDisplayItem() { return m_bDisplayItem; }

	//状态控制
public:
	//设置模式
	VOID SetSmallMode(bool bSmallMode) { m_bSmallMode=bSmallMode; }
	//设置方向
	VOID SetDirection(bool bHorizontal) { m_bHorizontal=bHorizontal; }
	//设置响应
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//设置显示
	VOID SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }

	//扑克控制
public:
	//设置显示
	VOID SetShowCount(BYTE cbShowCount);
	//设置背景
	VOID SetBackGround(BYTE cbBackGround);
	//设置距离
	VOID SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance);

	//控制函数
public:
	//获取中心
	VOID GetCenterPoint(CPoint & CenterPoint);
	//派发位置
	VOID SetDispatchPos(INT nXPos, INT nYPos);
	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//事件控制
public:
	//执行动画
	VOID CartoonMovie();
	//光标消息
	bool OnEventSetCursor(CPoint Point);
	//绘画扑克
	VOID DrawCardControl(CD3DDevice * pD3DDevice);

	//内部函数
public:
	//获取大小
	VOID GetControlSize(CSize & ControlSize);
	//获取原点
	VOID GetOriginPoint(CPoint & OriginPoint);
	//索引切换
	BYTE SwitchCardPoint(CPoint & MousePoint);

	//静态函数
public:
	//加载资源
	static VOID LoadCardResource(CD3DDevice * pD3DDevice);
};

//////////////////////////////////////////////////////////////////////////////////

#endif