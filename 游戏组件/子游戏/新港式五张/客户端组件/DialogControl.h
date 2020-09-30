#pragma once


// 玩家控制对话框 对话框

class CDialogControl : public CDialog
{
	DECLARE_DYNAMIC(CDialogControl)

public:
	CDialogControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogControl();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONRTOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
