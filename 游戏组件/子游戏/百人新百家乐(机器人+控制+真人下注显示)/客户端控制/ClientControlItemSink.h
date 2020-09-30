#pragma once
#include "../游戏客户端/ClientControl.h"


// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	BYTE m_cbWinArea;				//赢牌区域
	BYTE m_cbExcuteTimes;			//执行次数
	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer);
	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer);
	//更新控件
	virtual void __cdecl UpdateControl();

	virtual BOOL OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();

protected:
	virtual void OnCancel();

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnUpdateStorage();
	void RequestUpdateStorage();
};
