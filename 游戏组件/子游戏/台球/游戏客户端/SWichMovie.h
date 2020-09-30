#pragma once

#include "HgeGUI.h"


/*
切换的动画
1.一组多张图片文件切换播放
2.一张图多帧的图片播放
*/
class CSWichMovie
{
public:
	CSWichMovie(void);
	~CSWichMovie(void);
	//设置资源。graphSprite指向一组图片资源，PtPos,位置，num:指定数组的大小。swithTime切换的时间
	void SetResource(CHgeSprite**graphSprite,int num,CPoint &ptPos,int swithTime,CWnd *pParent);
	void SetResource(CHgeSprite*graphSprite,int num,CPoint ptPos=CPoint(0,0),int switchTime=100,CWnd *pParent=NULL,int ex=2);
	//绘图
	void DrawMovice();
	//帧计算
	void OnCalcFrame(float dt);
	//q强制停止
	void Stop();
	//停止但是不发送消息
	void ForceStop();
	//开始播放
	void Start(CPoint pt,int switchTime,bool bLoop=false);
	void Start(int x,int y,int switchTime=200,bool bLoop=false);
	//设置消失延迟
	void SetDisappearTime(int time);
	//设置每一帧交换的资源数
	void SetPerExchangePic(int ex)
	{
		m_perExchangePic = ex;
	}
protected:
	CHgeSprite **m_pSprite;      //多张图片
	CHgeSprite  *m_pSingleSprite;//一张图片
	int           m_currentFrame;
	int           m_totalFrame;
	CPoint        m_ptPosition;
	int           m_swithTime;
	bool          m_bLoop;
	bool          m_bStop;
	CWnd         *m_pParent;
	int           m_disappearTime;//消失延迟
	int           m_perExchangePic;
};
