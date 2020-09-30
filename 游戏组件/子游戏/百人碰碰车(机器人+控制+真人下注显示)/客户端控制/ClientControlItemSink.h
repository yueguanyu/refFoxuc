#pragma once
#include "../游戏客户端/ClientControl.h"


// CClientControlItemSinkDlg 对话框

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClientControlItemSinkDlg();

// 对话框数据
	enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:		
	LONGLONG  m_lAllUserBet[AREA_COUNT];				//用户下注

	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;


public:
	//重置界面
	virtual void __cdecl ResetUserBet();
	//玩家昵称
	virtual void __cdecl ResetUserNickName();
	//获取昵称
	virtual void __cdecl GetCheckNickName(CString &strNickName);
	//玩家昵称
	virtual void __cdecl SetUserNickName(LPCTSTR lpszNickName);
	//下注信息
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore);
	//下注信息
	virtual void __cdecl SetAllUserBetScore(BYTE cbArea,LONGLONG lScore);
	//更新控制
	virtual void __cdecl UpdateControl(CMD_S_ControlReturns* pControlReturns);

	//更新库存
	virtual bool __cdecl UpdateStorage(const void * pBuffer);
	

protected:
	//信息解析
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
	afx_msg void OnBnClickedBtGet();
	afx_msg void OnBnClickedBtCheak();

	afx_msg void OnBnClickedBtnUpdateStorage();
	void RequestUpdateStorage();
	//更新控件
	virtual void __cdecl UpdateControl();

};
