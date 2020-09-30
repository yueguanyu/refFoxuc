#pragma once
#include "HgeGUI.h"
//////////一张图移动(从a到b)的动画///////////////////////////////////////////////////////
#define WM_MOVE_END                 (WM_USER+020)                       //动画结束消息
class CMoveMovie
{
public:
	CMoveMovie(void);
	~CMoveMovie(void);
	void SetResource(CHgeSprite* pGraphSprite,CWnd *pParentWnd);
	//从ptStart移动到ptEnd,用时time
	void StartMovie(CPoint &ptStart,CPoint &ptEnd,DWORD time);
	//从ptStart移动到ptEnd,用时time,suspendTime:移动前的停滞时间
	void StartMovie(CPoint &ptStart,CPoint &ptEnd,DWORD time,int suspendTime);
	void DrawMovie();
	void CalcPosition(float dt);
	//结束
	void Stop();
	//不发送消息
	void ForceStop();
	bool IsPlaying();
	//设置动画开始移动前的停滞时间
	void SetBeginMoveSuspendTime(int time );
	//设置动画消失前的停滞时间
	void SetDisapperTime(int time)
	{
		m_disapperTime = time;
	}
	//附加数字
	void SetNumResource(CHgeSprite *num)
	{
		m_pGraphSpriteNum = num;
	}
	void ShowNum(int num,CPoint offset);
private:
	CHgeSprite   *m_pGraphSprite;
	CHgeSprite   *m_pGraphSpriteNum;//数字图片
	CPoint          m_ptStart;
	CPoint          m_ptEnd;
	CPoint          m_ptCurrent;
	DWORD           m_tickCount;
	bool            m_bIsMoving;
	CWnd          * m_pParentWnd;
	int             m_beforeMoveSuspendTime;//动画移动前的停滞时间
	int             m_disapperTime;         //动画消失前的停滞时间
	int             m_showNum;//需要显示的数字
	CPoint          m_numOffset;
};
