#include "StdAfx.h"
#include ".\switchmovemovie.h"

CSwitchMoveMovie::CSwitchMoveMovie(void)
{
}

CSwitchMoveMovie::~CSwitchMoveMovie(void)
{
}
void CSwitchMoveMovie::Start(CPoint ptStart,CPoint ptEnd,DWORD switchTime)
{
	__super::Start(ptStart,switchTime);
	m_totalTime = switchTime * m_totalFrame;
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	m_playingTime = 0;

}
void CSwitchMoveMovie::OnCalcFrame(float dt)
{
	if(m_playingTime>m_totalTime)
	{
		Stop();
		return ;
	}
	m_playingTime += dt * 1000;
	m_currentFrame = m_totalFrame * (float)m_playingTime/m_totalTime ;
	float rate = (float)m_playingTime /m_totalTime;
	int x = (m_ptEnd.x - m_ptStart.x) * rate;
	int y = (m_ptEnd.y - m_ptStart.y) * rate;
	m_ptPosition.x = m_ptStart.x + x;
	m_ptPosition.y = m_ptStart.y + y;
	if(m_playingTime>m_totalTime||(m_ptStart.x!=m_ptEnd.x&&abs(m_ptPosition.x-m_ptStart.x)>abs(m_ptEnd.x-m_ptStart.x))||
		(m_ptStart.y!=m_ptEnd.y&&abs(m_ptPosition.y-m_ptStart.y)>abs(m_ptEnd.y-m_ptStart.y)))
	{
		m_currentFrame = m_totalFrame-1;
		m_ptPosition = m_ptEnd;
	}
}