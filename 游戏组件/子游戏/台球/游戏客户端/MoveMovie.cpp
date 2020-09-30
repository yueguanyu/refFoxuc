#include "StdAfx.h"
#include ".\movemovie.h"

CMoveMovie::CMoveMovie(void)
{
	m_bIsMoving = false;
	m_pParentWnd = NULL;
	m_beforeMoveSuspendTime = 0;
	m_pGraphSpriteNum = NULL;
	m_disapperTime = 0;
	m_showNum = 0;
}

CMoveMovie::~CMoveMovie(void)
{
}
void CMoveMovie::SetResource(CHgeSprite* pGraphSprite,CWnd *pParentWnd)
{
	m_pGraphSprite = pGraphSprite;
	m_pParentWnd = pParentWnd;
}
	//从ptStart移动到ptEnd,用时time
void CMoveMovie::StartMovie(CPoint &ptStart,CPoint &ptEnd,DWORD time)
{
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	m_ptCurrent = m_ptStart;
	m_bIsMoving = true;
	m_tickCount = time;
	m_beforeMoveSuspendTime = 0;
}
	//从ptStart移动到ptEnd,用时time,suspendTime:移动前的停滞时间
void CMoveMovie::StartMovie(CPoint &ptStart,CPoint &ptEnd,DWORD time,int suspendTime)
{
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	m_ptCurrent = m_ptStart;
	m_bIsMoving = true;
	m_tickCount = time;
	m_beforeMoveSuspendTime = suspendTime;
}
void CMoveMovie::DrawMovie()
{
	if(!m_bIsMoving)
		return ;
	m_pGraphSprite->Render(m_ptCurrent.x,m_ptCurrent.y);
	//数字
	if(m_pGraphSpriteNum)
	{
		int a = m_showNum/10;
		int b = m_showNum%10;
		int w = m_pGraphSpriteNum->GetWidth()/10;
		int h = m_pGraphSpriteNum->GetHeight();
		int offset = 0;
		if(a>0)
		{
			m_pGraphSpriteNum->SetTextureRect( a*w, 0, w, h );
			m_pGraphSpriteNum->Render(m_ptCurrent.x+m_numOffset.x,m_ptCurrent.y+m_numOffset.y);
			offset += w;
		} 
		if(b>0)
		{
			m_pGraphSpriteNum->SetTextureRect( b*w, 0, w, h );
			m_pGraphSpriteNum->Render(m_ptCurrent.x+m_numOffset.x+offset,m_ptCurrent.y+m_numOffset.y);
		}
	}
	if(m_ptCurrent==m_ptEnd&&m_disapperTime==0)
	{
		Stop();
	}
}
void CMoveMovie::CalcPosition(float dt)
{
	if(!m_bIsMoving)
		return ;
	if(m_beforeMoveSuspendTime>0)
	{
		m_beforeMoveSuspendTime -= dt*1000;
		if(m_beforeMoveSuspendTime<0)
			m_beforeMoveSuspendTime = 0;
		return ;
	}
	//if(m_ptCurrent==m_ptEnd)
	//{
	//	static float temp = 0;
	//	if(temp<float(m_disapperTime))
	//	{
	//		temp += dt*1000;
	//		float rate = temp/m_disapperTime;
	//		float a = (1-rate)*255;
	//		if(a<0)
	//		{
	//			m_disapperTime = 0;
	//			temp = 0;
	//			a = 0;
	//		}
	//		m_pGraphSprite->SetColor(ARGB(a,255,255,22));
	//	}
	//	else
	//	{
	//		m_disapperTime = 0;
	//		temp = 0;
	//	}
	//	return ;
	//}
	dt = 1000*dt;//单位是ms
	float xRate = (m_ptEnd.x - m_ptStart.x)/(float)m_tickCount;
	float yRate = (m_ptEnd.y - m_ptStart.y)/(float)m_tickCount;
	m_ptCurrent.x += xRate * dt;
	m_ptCurrent.y += yRate *dt;
	if(abs(m_ptCurrent.x-m_ptStart.x)>abs(m_ptEnd.x-m_ptStart.x))
	{
		m_ptCurrent = m_ptEnd;
	}
	if(abs(m_ptCurrent.y-m_ptStart.y)>abs(m_ptEnd.y-m_ptStart.y))
	{
		m_ptCurrent = m_ptEnd;
	}
}
	//结束
void CMoveMovie::Stop()
{
	m_bIsMoving = false;
	if(m_pParentWnd)
		SendMessage(m_pParentWnd->m_hWnd,WM_MOVE_END,DWORD(this),0);
}
	//不发送消息
void CMoveMovie::ForceStop()
{
	m_bIsMoving = false;
}
bool CMoveMovie::IsPlaying()
{
	return m_bIsMoving;
}
//设置动画开始移动前的停滞时间
void CMoveMovie::SetBeginMoveSuspendTime(int time )
{
	m_beforeMoveSuspendTime = time;
}

void CMoveMovie::ShowNum(int num,CPoint offset)
{
	m_showNum = num;
	m_numOffset = offset;
}