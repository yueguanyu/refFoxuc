#include "StdAfx.h"
#include ".\swichmovie.h"

CSWichMovie::CSWichMovie(void)
{
	m_currentFrame = 100;
	m_totalFrame = 0;
	m_pSprite = NULL;
	m_pSingleSprite = NULL;
	m_bLoop = false;
	m_bStop = true;
	m_pParent = NULL;
	m_disappearTime = 0;
	m_perExchangePic = 1;
}

CSWichMovie::~CSWichMovie(void)
{
}
void CSWichMovie::SetResource(CHgeSprite * *graphSprite,int num,CPoint &ptPos,int switchTime,CWnd *pParent)
{
	
	m_pSprite = graphSprite;
	m_totalFrame = num;
	m_ptPosition = ptPos;
	m_swithTime = switchTime;
	m_currentFrame = num + 2;
	m_pParent = pParent;
}

void CSWichMovie::SetResource(CHgeSprite*graphSprite,int num,CPoint ptPos,int switchTime,CWnd *pParent,int ex)
{
	m_pParent = pParent;
	m_pSingleSprite = graphSprite;
	m_totalFrame = num;
	m_ptPosition = ptPos;
	m_swithTime = switchTime;
	m_currentFrame = num + 2;
	m_perExchangePic = ex;
}
void CSWichMovie::DrawMovice()
{
	if(m_currentFrame>m_totalFrame)
		return ;
	if(m_bStop&&m_disappearTime==0)
		return ;
	if(m_bStop)
	{
		if(m_disappearTime>0)
			m_currentFrame = m_totalFrame-1;
		else m_currentFrame = m_totalFrame+2;
	}
	if(m_pSingleSprite)
	{
		int w = m_pSingleSprite->GetWidth()/m_totalFrame;
		int h = m_pSingleSprite->GetHeight();
		m_pSingleSprite->SetTextureRect(m_currentFrame*w,0,w,h);
		m_pSingleSprite->Render(m_ptPosition.x,m_ptPosition.y);
	}
	else if(m_pSprite)
		m_pSprite[m_currentFrame]->Render(m_ptPosition.x,m_ptPosition.y);
	else ASSERT(0);
}
void CSWichMovie::OnCalcFrame(float dt)
{
	if(m_currentFrame>m_totalFrame&&!m_bLoop)
		return ;
	if(m_bStop)
	{
		if(m_disappearTime>0)
			m_disappearTime -= dt*1000;
		if(m_disappearTime<0)
			m_disappearTime = 0;
		return ;
	}
	static float curDt = dt*1000;
	if(curDt>m_swithTime)
	{
		m_currentFrame += m_perExchangePic;
		if(m_currentFrame>=m_totalFrame)
		{
			if(m_bLoop)
				m_currentFrame = 0;
			else
			{
				curDt = 0.0;
				Stop();
				return ;
			}
		}
		curDt = 0/*dt*1000*/;
	}
	else
	{
		curDt += dt*1000;
	}
}
void CSWichMovie::Stop()
{
	m_bStop = true;
	m_currentFrame = m_totalFrame;
	if(m_pParent)
		SendMessage(m_pParent->m_hWnd,WM_MOVE_END,DWORD(this),0);
}
//停止但是不发送消息
void CSWichMovie::ForceStop()
{
	m_bStop = true;
	m_currentFrame = m_totalFrame+2;
}
void CSWichMovie::Start(CPoint pt,int switchTime,bool bLoop)
{
	m_currentFrame = 0;
	m_swithTime = switchTime;
	m_ptPosition = pt;
	m_bLoop = bLoop;
	m_bStop = false;
}
void CSWichMovie::Start(int x,int y,int switchTime,bool bLoop)
{
	m_ptPosition.x = x;
	m_ptPosition.y = y;
	m_swithTime = switchTime;
	m_currentFrame = 0;
	m_bLoop = bLoop;
	m_bStop = false;
}
	//设置消失延迟
void CSWichMovie::SetDisappearTime(int time)
{
	if(time>0)
		m_disappearTime = time;
}