#pragma once

//信息结构
struct BetRecordInfo
{
	INT			nStreak;			//场次
	INT			nRanking;			//排名
	LONGLONG	lBet;				//下注
	LONGLONG	lWin;				//输赢
	INT			nHours;				//小时
	INT			nMinutes;			//分钟
};

//列表标题栏
class CColorHeaderCtrl : public CHeaderCtrl
{
public:
	CColorHeaderCtrl(){}
	~CColorHeaderCtrl(){}

public:
	//标题栏重绘
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

//列表
class CMyListCtrl : public CListCtrl
{
public:
	CMyListCtrl(){}
	~CMyListCtrl(){}


	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

// CDialogBetRecord 对话框
class CDialogBetRecord : public CDialog
{
	DECLARE_DYNAMIC(CDialogBetRecord)

public:
	CDialogBetRecord(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogBetRecord();

// 对话框数据
	enum { IDD = IDD_DIALOG_BET_LOG };

	//变量
public:
	//列表
	CMyListCtrl m_listBetRecord;
	//标题栏
	CColorHeaderCtrl m_ColorHeader;

	//资源
public:
	CFont					m_InfoFont;
	CBitImage				m_ImageBackdrop;		//背景
	CSkinButton				m_btClosee;				//关闭

	//内部函数
private:
	//区域辨认
	CString IdentifyAreas(BYTE cbArea);

	//控制函数
public:
	//添加信息
	void		AddInfo(BetRecordInfo* pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonBrOk();
};
