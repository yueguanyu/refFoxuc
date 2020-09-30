#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	return;
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_CALL_SCORE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_MAX_SCORE_TIMES))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_FLEE_SCORE_TIMES))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_TIME_HEAD_OUT_CARD))->LimitText(2);

	//更新参数
	FillDataToControl();

	return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//更新控件
bool CDlgCustomRule::FillDataToControl()
{
	//设置数据
	SetDlgItemInt(IDC_TIME_OUT_CARD,m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_TIME_START_GAME,m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_CALL_SCORE,m_CustomRule.cbTimeCallScore);
	SetDlgItemInt(IDC_TIME_HEAD_OUT_CARD,m_CustomRule.cbTimeHeadOutCard);

	//游戏控制
	SetDlgItemInt(IDC_MAX_SCORE_TIMES,m_CustomRule.wMaxScoreTimes);
	SetDlgItemInt(IDC_FLEE_SCORE_TIMES,m_CustomRule.wFleeScoreTimes);
	((CButton *)GetDlgItem(IDC_FLEE_SCORE_PATCH))->SetCheck((m_CustomRule.cbFleeScorePatch==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
	m_CustomRule.cbTimeOutCard=(BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeStartGame=(BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeCallScore=(BYTE)GetDlgItemInt(IDC_TIME_CALL_SCORE);
	m_CustomRule.cbTimeHeadOutCard=(BYTE)GetDlgItemInt(IDC_TIME_HEAD_OUT_CARD);

	//游戏控制
	m_CustomRule.wMaxScoreTimes=(WORD)GetDlgItemInt(IDC_MAX_SCORE_TIMES);
	m_CustomRule.wFleeScoreTimes=(WORD)GetDlgItemInt(IDC_FLEE_SCORE_TIMES);
	m_CustomRule.cbFleeScorePatch=(((CButton *)GetDlgItem(IDC_FLEE_SCORE_PATCH))->GetCheck()==BST_CHECKED);

	//开始时间
	if ((m_CustomRule.cbTimeStartGame<5)||(m_CustomRule.cbTimeStartGame>60))
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//叫分时间
	if ((m_CustomRule.cbTimeCallScore<5)||(m_CustomRule.cbTimeCallScore>60))
	{
		AfxMessageBox(TEXT("叫分时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//首出时间
	if ((m_CustomRule.cbTimeHeadOutCard<5)||(m_CustomRule.cbTimeHeadOutCard>60))
	{
		AfxMessageBox(TEXT("首出时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//出牌时间
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("出牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//最大倍数
	if ((m_CustomRule.wMaxScoreTimes<32)||(m_CustomRule.wMaxScoreTimes>512))
	{
		AfxMessageBox(TEXT("最大倍数设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//逃跑罚分
	if ((m_CustomRule.wFleeScoreTimes<2)||(m_CustomRule.wFleeScoreTimes>512))
	{
		AfxMessageBox(TEXT("逃跑倍数设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	return true;
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//设置变量
	m_CustomRule=CustomRule;

	//更新参数
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
