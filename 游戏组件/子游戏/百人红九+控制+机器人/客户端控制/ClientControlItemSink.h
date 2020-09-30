#pragma once
#include "../游戏客户端/ClientControl.h"


// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	bool m_bWinArea[CONTROL_AREA];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;
protected:		
	LONGLONG  m_lAllUserBet[AREA_COUNT];				//用户下注

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADMIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable);
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer);
	//重置下注
	virtual void __cdecl ResetUserBet();
	//玩家下注
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore);
	//获取昵称
	virtual void __cdecl GetUserNickName(CString &strNickName);
	//玩家1
	virtual void __cdecl SetUserNickName(LPCTSTR szNickName);
	virtual void __cdecl ResetUserNickName();

	//玩家下注
	virtual void __cdecl SetUserGameScore(BYTE cbArea,LONGLONG lScore);

public:
	//重置界面
	void ReSetAdminWnd();

public:
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
	afx_msg void OnBnClickedBtGet();
	afx_msg void OnBnClickedBtCheak();
protected:
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
