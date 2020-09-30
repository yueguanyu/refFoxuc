#pragma once
#include "stdafx.h"
#include "Resource.h"

// CAdminControl dialog
#define IDM_ADMIN_COMMDN WM_USER+1000

class CAdminControl : public CDialog
{
	DECLARE_DYNAMIC(CAdminControl)

public:
	CAdminControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdminControl();

// Dialog Data
	enum { IDD = IDD_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedControlBanker();
	afx_msg void OnBnClickedControlArea();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
