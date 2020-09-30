#pragma once
#include "SHButton.h"


#define PYOE_LEFT                     0x00000000
#define PYOE_CENTER                   0x00000001
#define PYOE_RIGHT                    0x00000002
#define PYOE_TOP                      0x00000000
#define PYOE_VCENTER                  0x00000004
#define PYOE_BOTTOM                   0x00000008

class CPlayOperate;

//玩家输入
class CEditOperate : public CEdit
{

public:
	LONGLONG					m_lMaxScore;		//最大下注
	LONGLONG					m_lMinScore;		//最小下注
	CPlayOperate*				m_pPlayOperate;		//玩家操作类

protected:
	CBrush						m_Brush;			//背景颜色

public:
	CEditOperate();
	~CEditOperate();

	// 背景颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// 背景颜色
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	// 判断字符
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	// 屏蔽右键
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	// 键盘点击
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	DECLARE_MESSAGE_MAP()
};

// 玩家操作
class CPlayOperate
{
	// 变量定义
protected:
	bool							m_bShow;				// 显示标示
	bool							m_bMouseDown;			// 鼠标点击
	LONG							m_lCellScore;			// 游戏底注
	int								m_lDragSite;			// 拖动位置

	// 属性定义
protected:
	CPoint							m_ptPos;				// 位置信息
	CSize							m_sizeBakc;				// 大小信息
	CRect							m_rectDrag;				// 拖动区域
	CSize							m_sizeDrag;				// 拖动图片大小

	// 资源定义
public:
	CPngImage						m_ImageBack;			// 背景图片
	CPngImage						m_ImageDrag;			// 拖动图片
	CSkinButton						m_btOneScore;			// 1倍按钮
	CSkinButton						m_btTwoScore;			// 2倍按钮
	CSkinButton						m_btThreeScore;			// 3倍按钮	
	CSkinButton						m_btFourScore;			// 4倍按钮
	CSkinButton						m_btAddScore;			// 加注按钮
	CSkinButton						m_btFollow;				// 跟注按钮
	CSkinButton						m_btGiveUp;				// 放弃按钮
	CSkinButton						m_btShowHand;			// 梭哈按钮
	CSHButton						m_btADD;				// 梭哈按钮
	CSHButton						m_btSUB;				// 梭哈按钮

	CFont							m_fontScoreInput;		// 积分输入
	CEditOperate					m_edieScoreInput;		// 积分输入

public:
	CPlayOperate(void);
	~CPlayOperate(void);

	// 属性函数
public:
	// 初始化
	void Initialization(CWnd* pParent);
	// 按钮消息
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// 绘画函数
public:
	// 绘画函数
	void DrawPlayOperate(CDC * pDC, INT nWidth, INT nHeight);

	// 设置函数
public:
	// 设置位置
	void SetOperatePos( int nPosX, int nPosY, UINT nFlags);
	// 设置信息
	void SetCellScore( LONG lCellScore);
	// 设置下注范围数
	void SetBetCount( LONGLONG lMaxBetCount, LONGLONG lMinBetCount );

	// 获取函数
public:
	// 获取下载数量
	LONGLONG GetBetCount();
	// 获取位置
	CRect GetOperateRect();
	// 获取显示状态
	bool	 IsShow() { return m_bShow; }
	// 获取加注状态
	bool	 IsAddShow() { return m_btAddScore.IsWindowEnabled()&&m_btAddScore.IsWindowVisible(); }


	// 功能函数
public:
	// 显示函数
	void ShowOperate( bool bShow );
	// 消息解释
	BOOL PreTranslateMessage(MSG * pMsg);
	// 设置积分
	void SetEditScore();
	// 设置积分
	void SetDragScore();
	// 设置积分
	void ScoreADD();
	// 设置积分
	void ScoreSUB();
	// 添加逗号
	CString AddComma( LONGLONG lScore );
	// 添加逗号
	CString AddComma( CString strScore );
	// 删除逗号
	LONGLONG DeleteComma( CString strScore );
};
