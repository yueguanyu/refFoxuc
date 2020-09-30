#pragma once

class CBetWnd : public CWnd
{

	//变量信息
public:
	LONGLONG						m_lPlayAllBet[ANIMAL_MAX];			//所有玩家下注
	CPngImage						m_ImageBack;						//背景
	CPngImage						m_ImageNumber;						//背景

	//构造函数
public:
	CBetWnd(void);
	~CBetWnd(void);

	//设置函数
public:
	//设置所有玩家下注
	void SetPlayAllBet( LONGLONG lPlayAllBet[ANIMAL_MAX] );
	//设置所有玩家下注
	void SetPlayAllBet( LONGLONG lPlayAllBet, int nAnimalIndex );

	//系统函数
public:
	//绘画
	afx_msg void OnPaint();
	//绘画
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//创建函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	// 界面函数
private:
	// 绘画数字
	void DrawNumber(CDC * pDC, CPngImage* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);	

};
