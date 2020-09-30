#pragma once

//筹码界面
class CWindowChip : public CVirtualWindow
{
	//信息变量
public:
	LONG								m_lCellScore;			//底分
	LONGLONG							m_lPlayOriginalScore;	//玩家原始积分
	LONGLONG							m_lPlayScore;			//玩家积分
	LONGLONG							m_lPlayChip;			//玩家筹码

	//字体变量
public:
	CD3DFont							m_D3DFontText;			//D3D字体
	CD3DFont							m_D3DFontBold;			//D3D字体

	//背景图片
public:
	CD3DTexture							m_ImageBack;			//背景图片

	CVirtualButton						m_btChip100;			//兑换100
	CVirtualButton						m_btChip1000;			//兑换1000
	CVirtualButton						m_btChip10000;			//兑换10000
	CVirtualButton						m_btChipMax;			//最大兑换
	CVirtualButton						m_btChipEmpty;			//清空兑换

	CVirtualButton						m_btOK;					//确定
	CVirtualButton						m_btClose;				//关闭
	CVirtualButton						m_btCloseX;				//关闭X

	//构造函数
public:
	CWindowChip(void);
	~CWindowChip(void);

	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie();
	//创建消息
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos);
	//按钮事件
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos);
	//绘画窗口
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos);

	//窗口控制
public:
	//显示窗口
	VOID ShowWindow(bool bVisible);

	//功能函数
public:
	//设置信息
	VOID SetChipInfo( LONG lCellScore, LONGLONG	 lPlayScore, LONGLONG lPlayChip);
	//按钮激活
	VOID ButtonEnable();
	//绘画字符
	VOID DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont,  LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);


};
