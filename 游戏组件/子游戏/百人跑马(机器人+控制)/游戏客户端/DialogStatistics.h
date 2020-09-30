#pragma once


// CDialogStatistics 对话框

class CDialogStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDialogStatistics)

public:
	CDialogStatistics(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogStatistics();

// 对话框数据
	enum { IDD = IDD_DIALOG_TONGJI };

	//游戏变量
protected:
	INT					m_nWinCount[HORSES_ALL];			//全天赢的场次

	//资源变量
protected:
	CFont				m_InfoFont;							//字体
	CBitImage			m_ImageLine;						//线
	CBitImage			m_ImageBackdrop;					//背景
	CSkinButton			m_btClose;							//关闭
	CSkinButton			m_btDetermine;						//确定


	//控制函数
public:
	//设置积分
	void SetWinCount( INT nWinCount[HORSES_ALL] );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonTjClose();
	afx_msg void OnBnClickedButtonTjClose2();
};
