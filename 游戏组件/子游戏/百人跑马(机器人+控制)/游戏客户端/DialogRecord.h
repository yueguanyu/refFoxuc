#pragma once


// CDialogRecord 对话框

class CDialogRecord : public CDialog
{
	DECLARE_DYNAMIC(CDialogRecord)

public:
	CDialogRecord(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogRecord();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOG };

	//游戏记录
public:
	CWHArray<tagHistoryRecord> m_GameRecords;			//游戏记录

	//游戏变量
protected:
	LONGLONG			m_lPlayerScore;						//玩家积分
	LONGLONG			m_lAllBet;							//本局投注
	LONGLONG			m_lBetMumber;						//投注人数

	//资源变量
protected:
	CSize				m_szTotalSize;						//总大小
	CFont				m_InfoFont;							//字体
	CBitImage			m_ImageBackdropHand;				//背景头
	CBitImage			m_ImageBackdropTail;				//背景尾
	CBitImage			m_ImageBackdrop;					//背景中
	CSkinButton			m_btOk;								//OK


	//内部函数
private:
	//区域辨认
	CString IdentifyAreas(BYTE cbArea);

	//控制函数
public:
	//设置积分
	void SetScore( LONGLONG lScore, LONGLONG lAllBet, LONGLONG lBetMumber );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonLogok();
};
#pragma once


