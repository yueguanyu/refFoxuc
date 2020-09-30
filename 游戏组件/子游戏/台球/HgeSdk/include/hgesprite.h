/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeSprite helper class header 为精灵实体所作的HGE辅助类
*/


#ifndef HGESPRITE_H
#define HGESPRITE_H


#include "hge.h"
#include "hgerect.h"


/*
** HGE Sprite class
*/
class hgeSprite
{
public:
	hgeSprite(HTEXTURE tex, float x, float y, float w, float h);
	hgeSprite(const hgeSprite &spr);		// 拷贝构造函数
	~hgeSprite() { hge->Release(); }
	
	// 描绘精灵到屏幕上
	void		Render(float x, float y);

	// 用缩放比例和旋转描绘精灵; 
	void		RenderEx(float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);

	// 描绘伸长精灵
	void		RenderStretch(float x1, float y1, float x2, float y2);

	// 描绘精灵到屏幕上的任意四边形
	void		Render4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

	// 设置材质为精灵所用
	void		SetTexture(HTEXTURE tex);

	// 设置材质的特定区域为精灵所用
	void		SetTextureRect(float x, float y, float w, float h, bool adjSize = true);

	// 为指定的顶点或整个精灵设置色彩和透明度
	void		SetColor(DWORD col, int i=-1);

	// 为指定的顶点或整个精灵设定z轴
	void		SetZ(float z, int i=-1);

	// 设置精灵的混和模式
	void		SetBlendMode(int blend) { quad.blend=blend; }

	// 设定精灵的锚点(缩放，旋转和位移操作的中心点,相对于图片左上角的位移坐标)
	void		SetHotSpot(float x, float y) { hotX=x; hotY=y; }

	// 水平或垂直翻转精灵; 若bHotSpot=true;翻转将与精灵的锚点关联作用,否则只是图像被翻转而锚点不变.
	void		SetFlip(bool bX, bool bY, bool bHotSpot = false);

	// 返回当前精灵的材质
	HTEXTURE	GetTexture() const { return quad.tex; }

	// 返回被当前精灵所用的特定材质区域
	void		GetTextureRect(float *x, float *y, float *w, float *h) const { *x=tx; *y=ty; *w=width; *h=height; }

	// 返回指定精灵顶点的颜色
	DWORD		GetColor(int i=0) const { return quad.v[i].col; }

	// 返回指定精灵顶点的z轴坐标
	float		GetZ(int i=0) const { return quad.v[i].z; }

	// 返回当前精灵的混合模式
	int			GetBlendMode() const { return quad.blend; }

	// 返回精灵的锚点(锚点是缩放，旋转和位移操作的中心点)
	void		GetHotSpot(float *x, float *y) const { *x=hotX; *y=hotY; }

	// 返回当前精灵的翻转参数
	void		GetFlip(bool *bX, bool *bY) const { *bX=bXFlip; *bY=bYFlip; }

	// 返回精灵的宽度
	float		GetWidth() const { return width; }

	// 返回精灵的高度
	float		GetHeight() const { return height; }

	// 返回精灵的包围盒范围
	hgeRect*	GetBoundingBox(float x, float y, hgeRect *rect) const { rect->Set(x-hotX, y-hotY, x-hotX+width, y-hotY+height); return rect; }

	// 返回缩放和旋转精灵的包围盒范围
	hgeRect*	GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  hgeRect *rect) const;

protected:
	hgeSprite();
	static HGE	*hge;		// 全局唯一实例, 由HgeImpl::HgeCreate(version)创建

	hgeQuad		quad;		// v[0]表示左上角坐标，v[1]表示右上角坐标，v[2]表示右下角坐标，v[3]表示左下角坐标; 顶点索引: 012023 顺时针绘制
	float		tx, ty, width, height;		// 纹理的起始截取位置及宽高
	float		tex_width, tex_height;		// 整张纹理的宽高
	float		hotX, hotY;					// 设置纹理中心点, 距离精灵左上角的相对位移
	bool		bXFlip, bYFlip, bHSFlip;	// 是否水平、垂直或绕中心点进行翻转
};


#endif
