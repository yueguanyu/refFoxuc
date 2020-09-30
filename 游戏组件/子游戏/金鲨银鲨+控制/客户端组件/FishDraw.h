#pragma once
#include "ObjectMove.h"
#include "D3DTextureIndex.h"

#define FISH_MAX					6
class CFishDraw
{
	//信息变量
public:
	int											m_nBackIndex;				//背景索引
	CPoint										m_ptBullet[4];				//发射子弹位置
	CPoint										m_ptCannon[4];				//炮台位置
	int											m_nCannonAlpha;				//炮台渐隐

	//鱼变量
public:
	CWHArray<tgaFishDraw*>						m_ArrayFishDraw;
	CLapseCount									m_LapseCount;				//流逝计数
	BOOL										m_bAddFish;					//添加鱼

	CObjectMove*								m_pBulletMove[4];			//子弹移动
	CObjectMove*								m_pNetMove[4];				//渔网移动
	CLapseCount									m_BulletCount;				//流逝计数

	//资源
public:
	CD3DTexture							m_ImgaeBack[7];				//背景
	CD3DTexture							m_ImgaeWater[12];			//水纹
	CD3DTextureIndex							m_ImgaeFish[FISH_MAX];		//鱼

	CD3DTextureIndex							m_ImgaeGun;					//跑管
	CD3DTextureIndex							m_ImgaeCannon;				//跑坐

	CD3DTexture							m_ImgaeBullet;				//子弹
	CD3DTextureIndex							m_ImgaeNet;					//网

	//位置
public:
	CRect										m_rectFishPond;
	
	//构造函数
public:
	CFishDraw(void);
	~CFishDraw(void);


	//设置函数
public:
	//是否添加鱼
	void SetAddFish( BOOL bAddFish ) { m_bAddFish = bAddFish; }
	//设置鱼池
	VOID SetFishPond( CRect	rectFishPond );
	//设置背景索引
	VOID SetBackIndex( int nBackIndex ) { m_nBackIndex = nBackIndex; }


	//获取函数
	int GetBackIndex() { return m_nBackIndex; }

	//系统事件
public:
	//动画
	VOID OnFishMovie();
	//创建
	VOID OnFishCreate(CD3DDevice * pD3DDevice);

	//重载函数
public:
	//绘画
	VOID OnFishDraw(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//函数
public:
	//鱼路径
	BYTE FishPath( CPoint* pFishMove );
};
