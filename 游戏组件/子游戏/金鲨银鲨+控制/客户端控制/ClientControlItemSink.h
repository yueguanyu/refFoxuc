#pragma once
#include "../客户端组件/ClientControl.h"


// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	BYTE m_cbWinArea[2];				//赢牌区域
	BYTE m_cbExcuteTimes;				//执行次数

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//更新控制
	virtual void __cdecl OnAllowControl(bool bEnable);
	//申请结果
	virtual bool __cdecl ReqResult(const void * pBuffer);

	virtual void __cdecl SetText(SCORE m_Score,int x,int y,bool bXor);

	virtual BOOL  OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnSexClicked(UINT nCmdID);

protected:
	virtual void OnCancel();

public:
//	afx_msg void OnTimer(UINT nIDEvent);
	
};
