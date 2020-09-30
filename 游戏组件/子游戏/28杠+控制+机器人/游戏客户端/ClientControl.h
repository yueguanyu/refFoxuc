#pragma once
#define IDM_ADMIN_COMMDN     WM_USER+1000
#define IDM_GET_ACCOUNT		 WM_USER+1001
#define IDM_CHEAK_ACCOUNT	 WM_USER+1002
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
	//重置下注
	virtual void __cdecl ResetUserBet() = NULL;
	//玩家下注
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore) = NULL;
	virtual void __cdecl SetUserNickName(CString &strNickName) = NULL;
	virtual void __cdecl ResetUserNickName() = NULL;

	virtual void __cdecl SetUserGameScore(BYTE cbArea,LONGLONG lScore) = NULL;
};
