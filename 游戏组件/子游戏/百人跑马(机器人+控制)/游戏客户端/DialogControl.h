#pragma once


// CDialogControl 对话框

class CDialogControl : public CDialog
{
	DECLARE_DYNAMIC(CDialogControl)

public:
		LONGLONG  m_lAllUserBet[AREA_ALL];				//用户下注
	CDialogControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogControl();

	// 对话框数据
	enum { IDD = IDD_DIALOG_SYSTEM };

	//控件
public:
	CEdit					m_editInput[AREA_ALL];		//输入框
	CButton					m_radioArea[AREA_ALL];		//控制区域

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	//更新控制
	void UpdateControl(CMD_S_ControlReturns* pControlReturns);

protected:
	//信息
	void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, int nMultiple[AREA_ALL], BYTE cbTimes);
	//获取区域
	CString	ObtainArea( BYTE cbArea );


	DECLARE_MESSAGE_MAP()
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
	//当局生效按钮
	afx_msg void OnBnClickedRadioNwe();
	//下局生效按钮
	afx_msg void OnBnClickedRadioNext();
	//重置下注
	void ResetUserBet();
	//玩家下注
	void SetUserBetScore(LONGLONG lBetScore[AREA_ALL]);
};
