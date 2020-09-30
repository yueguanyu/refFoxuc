#pragma once

#include "Stdafx.h"
#include <list>
//#include <iostream>
#include <gdiplus.h>
#include <math.h>
//#include "Picture.h"
/*
	此类用于画动画控制
	by liu
*/
using namespace std;
class CMoviceControl
{
public:
	CMoviceControl();
	bool  Create(LPCTSTR LoadPatch,BYTE MaxFrame,LPCTSTR ImgType,Color colorAlpha);
	void  Destroy();
	void  DrawMovice(CDC *pDc);
	//对帧数累加
	bool  AddFrame(bool bIsAlwaysDraw = false);
	//复位
	void  ResetPlace(int beginx,int beginy,int Pace = 0,BYTE DrawNum = 1,int Angle = 0,bool IsRotate = false);
	void  EndMovice();
	BOOL  IsDrawMove(){return m_bIsDraw;}
	CRect GetDrawRect();
	int   GetWidth(){return m_iWidth;}
	int   GetHeight(){return m_iHeight;}
private:
	BYTE		m_byCurFrame;	//当前动画的帧数
	BYTE		m_byMaxFrame;	//最大帧数
	BYTE		m_byCopyDrawNum;//同时画的次数
	BOOL		m_bIsDraw;		//是否画



	list<Image*>	m_ListMovice;		//图片
	
	ImageAttributes m_remapAttributes;  //画图的属性
	Point			m_beginPoints[3];	//起始位子
	Point			m_drawPoints[3];	//画的位子

	int				m_iPace;			//速度
	Point			m_endPoint;			//结束点
	int				m_iAngle;			//角度
	int			    m_iWidth;			//宽度
	int 			m_iHeight;			//高度
	bool			m_bIsRotate;		//是否旋转
};