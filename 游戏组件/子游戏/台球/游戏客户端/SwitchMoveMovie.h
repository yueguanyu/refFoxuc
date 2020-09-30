#pragma once
#include "swichmovie.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////这是一个多张图片切换并移动播放的动画
class CSwitchMoveMovie :
	public CSWichMovie
{
public:
	CSwitchMoveMovie(void);
	~CSwitchMoveMovie(void);
	//开始动画
	void Start(CPoint ptStart,CPoint ptEnd,DWORD switchTime);
	void OnCalcFrame(float dt);
private:
	CPoint m_ptStart;
	CPoint m_ptEnd;
	DWORD  m_totalTime;//用时间
	DWORD  m_playingTime;//播放时间
};
