#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000
#define IDM_SPE_COMMDN WM_USER+1001

//游戏控制基类
class ISpeClientControlDlg : public CDialog 
{
public:
	ISpeClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~ISpeClientControlDlg(void){}

	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
	//处理消息
	virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
};
