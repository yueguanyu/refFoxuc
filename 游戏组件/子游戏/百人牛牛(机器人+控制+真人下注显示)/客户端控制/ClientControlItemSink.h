#pragma once
#include "../客户端组件/ClientControl.h"

#define CONTROL_AREA		4
// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	bool m_bWinArea[CONTROL_AREA];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;

	
	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;



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
	virtual void  OnAllowControl(bool bEnable);
	//申请结果
	virtual bool  ReqResult(const void * pBuffer);

public:
	//重置界面
	void ReSetAdminWnd();

public:
	//显示状态
	void PrintCurStatus();

public:
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
	
	afx_msg void OnBnClickedBtnUpdateStorage();
	void RequestUpdateStorage();
	//更新控件
	virtual void __cdecl UpdateControl();
	
	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer);

protected:
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
};
