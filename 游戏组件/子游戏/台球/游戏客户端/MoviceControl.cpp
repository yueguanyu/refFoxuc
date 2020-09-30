#include "StdAfx.h"
#include "MoviceControl.h"

CMoviceControl::CMoviceControl()
:m_byCurFrame(0),m_byMaxFrame(0),m_bIsDraw(false)
{
	m_bIsRotate =false;

}
bool CMoviceControl::Create(LPCTSTR LoadPatch,BYTE MaxFrame,LPCTSTR ImgType,Color colorAlpha)
{

	m_byMaxFrame = MaxFrame;
	m_byCopyDrawNum = 0;
	char str[250];

	m_iWidth = 0;			//宽度
	m_iHeight = 0;			//高度
	USES_CONVERSION;
	Image* pImage;
	for(int i =0; i < m_byMaxFrame; i++)
	{
		sprintf(str,"%s\\%d.%s",LoadPatch,i,ImgType);
		pImage = Image::FromFile(A2W(str));

		if(!pImage||pImage->GetLastStatus()!=Ok)
			return false;

		if(i == 0)
		{
			m_iWidth = pImage->GetWidth();
			m_iHeight = pImage->GetHeight();
		}
		
		m_ListMovice.push_back(pImage);
	}
	m_remapAttributes.SetColorKey(colorAlpha,colorAlpha);

	ZeroMemory(m_beginPoints,sizeof(m_beginPoints));


	return true;
}
void CMoviceControl::Destroy()
{

	list<Image*>::iterator it= m_ListMovice.begin();
	//Image* image;
	for(;it != m_ListMovice.end();it++)
	{
		delete *it;
	}
}
void CMoviceControl::DrawMovice(CDC *pDc)
{
	//if(!m_bIsDraw||pDc == NULL || m_ImgMovice == NULL)
	//	return;

	if(m_byCurFrame >=m_byMaxFrame)
		return;
	if(!m_bIsDraw)
		return;
	Image *pImage = NULL;
	list<Image*>::iterator it= m_ListMovice.begin();
	for(int i= 0; i <= m_byCurFrame&&it != m_ListMovice.end();it++,i++)
	{
		if(i == m_byCurFrame)
		{
			pImage = *it;
			break;
		}

	}
	//Image *pImage = m_ListMovice
	if(pImage==NULL || pImage->GetLastStatus()!=Ok)
		return;
	Graphics graphics(pDc->m_hDC);

	
	
	int w = pImage->GetWidth();
	int h = pImage->GetHeight();
	for(int i = 0; i < m_byCopyDrawNum; i++)
	{
		if(i != 0)
		{
			m_drawPoints[0].X += w*0.5f;
			m_drawPoints[1].X += w*0.5f;
			m_drawPoints[2].X += w*0.5f;
		}
		graphics.DrawImage(pImage,m_drawPoints,3,0,0,
			w,h,UnitPixel,&m_remapAttributes,NULL,NULL);
	}

}
void CMoviceControl::ResetPlace(int beginx,int beginy,int Pace,BYTE DrawNum,int Angle,bool IsRotate)
{

	m_bIsDraw = true;
	m_byCopyDrawNum = DrawNum;
	m_byCurFrame = 0;

	m_iPace = Pace;
	m_iAngle = Angle;
	float fsin = sin(Angle/180.0f*3.1415926538f);
	float fcos = cos(Angle/180.0f*3.1415926538f);
	m_beginPoints[0].X = beginx;
	m_beginPoints[0].Y = beginy;
	m_beginPoints[1].X = beginx + (*m_ListMovice.begin())->GetWidth()*fcos;
	m_beginPoints[1].Y = beginy - (*m_ListMovice.begin())->GetWidth()*fsin;
	m_beginPoints[2].X = beginx + (*m_ListMovice.begin())->GetHeight()*fsin;
	m_beginPoints[2].Y = beginy + (*m_ListMovice.begin())->GetHeight()*fcos;

	memcpy(m_drawPoints,m_beginPoints,sizeof(m_drawPoints));
	m_bIsRotate = IsRotate;
	if(m_bIsRotate)
	{
		list<Image*>::iterator it= m_ListMovice.begin();
		for(;it != m_ListMovice.end();it++)
		{
			(*it)->RotateFlip(Rotate180FlipY);
		}
	}

}
void CMoviceControl::EndMovice()
{
	m_bIsDraw =false;
	if(m_bIsRotate)
	{
		list<Image*>::iterator it= m_ListMovice.begin();
		for(;it != m_ListMovice.end();it++)
		{
			(*it)->RotateFlip(Rotate180FlipY);
		}
	}
}
bool CMoviceControl::AddFrame(bool bIsAlwaysDraw)
{
	if(!m_bIsDraw)
		return false;
	if(m_byCurFrame+1>= m_byMaxFrame) 
	{
		//是不是一直画
		if(!bIsAlwaysDraw)
			m_bIsDraw = false;

		m_byCurFrame = 0;
	}
	else
	{

		m_byCurFrame++;
	}
	if(m_iPace!=0)
	{
		int distancex = m_iPace*sin(m_iAngle/180.0f*3.1415926538f);
		int distancey = m_iPace*cos(m_iAngle/180.0f*3.1415926538f);

		m_drawPoints[0].X -= distancex;
		m_drawPoints[0].Y -= distancey;
		m_drawPoints[1].X -= distancex;
		m_drawPoints[1].Y -= distancey;
		m_drawPoints[2].X -= distancex;
		m_drawPoints[2].Y -= distancey;
	}
	else
	{
		memcpy(m_drawPoints,m_beginPoints,sizeof(m_drawPoints));
	}
	return true;
}
CRect CMoviceControl::GetDrawRect()
{
	CRect  DrawRect;
	if(m_iAngle == 0&&m_iPace==0 )
	{
		DrawRect.left = m_beginPoints[0].X;
		DrawRect.top = m_beginPoints[0].Y;
		DrawRect.right = DrawRect.left+m_iWidth*(m_byCopyDrawNum-1)*0.5+m_iWidth;
		DrawRect.bottom = DrawRect.top+m_iHeight;
	}
	else
	{
		int x =m_drawPoints[0].X;
		int y = m_drawPoints[0].Y;
		for(int i =0; i < 2; i++)
		{
			if(x < m_drawPoints[i+1].X)
				x = m_drawPoints[i+1].X;
			if(y < m_drawPoints[i+1].Y)
				y = m_drawPoints[i+1].Y;

		}
		int l= sqrt((float)(m_iWidth*m_iWidth+m_iHeight*m_iHeight));
		DrawRect.left = x;
		DrawRect.right = x+l;
		DrawRect.top = y;
		DrawRect.bottom = y+l;

	}
	return DrawRect;
}