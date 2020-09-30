#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000

//游戏控制基类
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
	virtual void __cdecl SetText(SCORE m_Score, int x ,int y,bool bXor=true) = NULL;
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
};
