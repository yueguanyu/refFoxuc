#pragma once

class CUserFaceWnd : public CWnd
{
public:
	CUserFaceWnd(void);
	~CUserFaceWnd(void);

protected:
	//绘画指针
	CGameFrameViewD3D*		m_pFrameView;
	//头像指针
	const IClientUserItem*	m_pUserData;

public:
	//设置绘画指针
	void SetFrameView(CGameFrameViewD3D* pFrameView);
	//设置头像指针
	void SetUserData(const IClientUserItem* pUserData);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
