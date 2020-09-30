#pragma once
#include "Resource.h"
#include "../游戏客户端/ClientControl.h"

// CAdminControlDlg 对话框


class AFX_EXT_CLASS CAdminControlDlg :public IClientControlDlg
{
	DECLARE_DYNAMIC(CAdminControlDlg)

public:
	CAdminControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdminControlDlg();

	// 对话框数据
	enum { IDD = IDD_DIALOG_SYSTEM };
protected:		
	LONGLONG  m_lAllUserBet[AREA_COUNT];				//用户下注
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//重置下注
	virtual void __cdecl ResetUserBet();
	//玩家下注
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore);
	//玩家下注
	virtual void __cdecl SetUserGameScore(BYTE cbArea,LONGLONG lScore);
	//更新控制
	virtual void __cdecl UpdateControl(CMD_S_ControlReturns* pControlReturns);
	//玩家1
	virtual void __cdecl SetUserNickName(CString strNickName);
	virtual void __cdecl ResetUserNickName();

protected:
	//信息
	void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);

public:
	//初始化
	virtual BOOL OnInitDialog();
	//设置颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//取消控制
	afx_msg void OnBnClickedButtonReset();
	//本局控制
	afx_msg void OnBnClickedButtonSyn();
	//开启控制
	afx_msg void OnBnClickedButtonOk();
	//取消关闭
	afx_msg void OnBnClickedButtonCancel();

	afx_msg void OnBnClickedBtCheak();
	afx_msg void OnBnClickedBtGet();
};
