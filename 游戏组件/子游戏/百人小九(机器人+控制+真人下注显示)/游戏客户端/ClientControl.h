#pragma once

//游戏控制基类
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

public:
	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer) = NULL;

	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
	//更新控件
	virtual void __cdecl UpdateControl() = NULL;
};
