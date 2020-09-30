#pragma once


// CDialogMessage 对话框
#include "../游戏服务器/GameLogic.h"
class CDialogMessage : public CDialog
{
	DECLARE_DYNAMIC(CDialogMessage)

public:
	CDialogMessage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogMessage();

// 对话框数据
	enum { IDD = IDD_DIALOG_MES };
	
	//变量
private:
	CFont				m_InfoFont;			//消息字体
	TCHAR				m_szMessage[128];	//消息
	CBitImage			m_ImageBackdrop;	//背景
	CSkinButton			m_btDetermine;		//确定
	CSkinButton			m_btClosee;			//关闭

	//设置函数
public:
	//设置消息
	void SetMessage( LPCTSTR lpszString );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonClose();
};
