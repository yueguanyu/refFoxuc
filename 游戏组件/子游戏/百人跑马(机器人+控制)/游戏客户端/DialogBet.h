#pragma once


//消息定义

class CDialogPlayBet : public CDialog
{
	DECLARE_DYNAMIC(CDialogPlayBet)

public:
	CDialogPlayBet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogPlayBet();

	// 对话框数据
	enum { IDD = IDD_DIALOG_BET };

	//游戏变量
public:
	INT						m_nMultiple[AREA_ALL];	//区域倍数
	LONGLONG				m_lLastRound[AREA_ALL];	//上一轮积分
	LONGLONG				m_lPlayerScore;			//玩家积分
	LONGLONG				m_lTheNote;				//当前注数

	//图片
public:
	CPngImage				m_ImageBackdrop;		//背景

	//控件
public:
	CEdit					m_editInput[AREA_ALL];		//输入框
	CSkinButton				m_btAdd[AREA_ALL];			//增加按钮
	CSkinButton				m_btReduce[AREA_ALL];		//减少按钮
	CSkinButton				m_btBet1000;				//下注1000
	CSkinButton				m_btBet1W;					//下注1W
	CSkinButton				m_btBet10W;					//下注10W
	CSkinButton				m_btBet100W;				//下注100W
	CRect					m_rcBet1000;				//1000位置
	CRect					m_rcBet1W;					//1W位置
	CRect					m_rcBet10W;					//10W位置
	CRect					m_rcBet100W;				//100W位置
	CSkinButton				m_btDetermine;				//确定
	CSkinButton				m_btReset;					//重置
	CSkinButton				m_btRepeat;					//重复
	CSkinButton				m_btClosee;					//关闭

	//资源
public:
	CFont					m_InfoFont;
	CFont					m_MultipleFont;
	CBrush					m_InfoBrush;

	//内部函数
private:
	//区域加
	void					EditAdd(BYTE cbArea);	
	//区域减
	void					EditReduce(BYTE cbArea);	
	//区域限制
	void					EditLimit(BYTE cbArea);

	//控制函数
public:
	//设置积分
	void SetScore( LONGLONG lScore );
	//设置是否能下注
	void SetCanBet( BOOL bCanBet);
	//设置倍数
	void SetMultiple( INT nMultiple[AREA_ALL]);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton1000();
	afx_msg void OnBnClickedButton1w();
	afx_msg void OnBnClickedButton10w();
	afx_msg void OnBnClickedButton100w();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonReduce16();
	afx_msg void OnBnClickedButtonAdd16();
	afx_msg void OnBnClickedButtonReduce15();
	afx_msg void OnBnClickedButtonAdd15();
	afx_msg void OnBnClickedButtonReduce14();
	afx_msg void OnBnClickedButtonAdd14();
	afx_msg void OnBnClickedButtonReduce13();
	afx_msg void OnBnClickedButtonAdd13();
	afx_msg void OnBnClickedButtonReduce12();
	afx_msg void OnBnClickedButtonAdd12();
	afx_msg void OnBnClickedButtonReduce26();
	afx_msg void OnBnClickedButtonAdd26();
	afx_msg void OnBnClickedButtonReduce25();
	afx_msg void OnBnClickedButtonAdd25();
	afx_msg void OnBnClickedButtonReduce24();
	afx_msg void OnBnClickedButtonAdd24();
	afx_msg void OnBnClickedButtonReduce23();
	afx_msg void OnBnClickedButtonAdd23();
	afx_msg void OnBnClickedButtonReduce36();
	afx_msg void OnBnClickedButtonAdd36();
	afx_msg void OnBnClickedButtonReduce35();
	afx_msg void OnBnClickedButtonAdd35();
	afx_msg void OnBnClickedButtonReduce34();
	afx_msg void OnBnClickedButtonAdd34();
	afx_msg void OnBnClickedButtonReduce46();
	afx_msg void OnBnClickedButtonAdd46();
	afx_msg void OnBnClickedButtonReduce45();
	afx_msg void OnBnClickedButtonAdd45();
	afx_msg void OnBnClickedButtonReduce56();
	afx_msg void OnBnClickedButtonAdd56();
	afx_msg void OnBnClickedButtonDetermine();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonRepeat();
	afx_msg void OnBnClickedButtonCloseBet();
	afx_msg void OnEnChangeEdit16();
	afx_msg void OnEnChangeEdit15();
	afx_msg void OnEnChangeEdit14();
	afx_msg void OnEnChangeEdit13();
	afx_msg void OnEnChangeEdit12();
	afx_msg void OnEnChangeEdit26();
	afx_msg void OnEnChangeEdit25();
	afx_msg void OnEnChangeEdit24();
	afx_msg void OnEnChangeEdit23();
	afx_msg void OnEnChangeEdit36();
	afx_msg void OnEnChangeEdit35();
	afx_msg void OnEnChangeEdit34();
	afx_msg void OnEnChangeEdit46();
	afx_msg void OnEnChangeEdit45();
	afx_msg void OnEnChangeEdit56();
};
