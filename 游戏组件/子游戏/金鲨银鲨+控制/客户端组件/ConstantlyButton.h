#pragma once

class CConstantlyButton : public CVirtualButton
{

public:
	BOOL								m_bSend;				//发送消息
	CLapseCount							m_WaitingTime;			//等待计时
	CLapseCount							m_SendTime;				//发送计时

public:
	CConstantlyButton(void);
	~CConstantlyButton(void);

	//系统事件
protected:
	//动画消息
	virtual VOID OnWindowMovie();

	//重载函数
protected:
	//鼠标事件
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);

	//获取资源
public:
	CD3DTexture* GetD3DTexture() { return &m_D3DTextureButton; }
};
