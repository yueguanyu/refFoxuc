#pragma once

class CWindowOver : public CVirtualWindow
{
	//变量信息
public:
	LONGLONG							m_lPlayScore;			//玩家得分

	//背景图片
public:
	CD3DTexture					m_ImageBack;			//背景图片
	CD3DTexture					m_ImageNumber;			//数字图片

	//构造函数
public:
	CWindowOver(void);
	~CWindowOver(void);

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

	// 界面函数
private:
	// 绘画数字
	void DrawNumber(CD3DDevice * pD3DDevice , CD3DTexture* pImageNumber, TCHAR* szImageNum, TCHAR* szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT, BYTE cbAlpha = 255);	
	//绘画字符
	VOID DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont,  LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);

	//设置函数
public:
	//设置积分
	void SetPlayScore( LONGLONG lPlayScore ) { m_lPlayScore = lPlayScore; }
};
