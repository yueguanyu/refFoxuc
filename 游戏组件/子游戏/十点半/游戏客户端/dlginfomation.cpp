#include "Stdafx.h"
#include "DlgInfomation.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgInfomation, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInfomation::CDlgInfomation() : CSkinDialog(IDD_INFOMATION)
{
}

//析构函数
CDlgInfomation::~CDlgInfomation()
{
}

//控件绑定
void CDlgInfomation::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
}

//初始化函数
BOOL CDlgInfomation::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏通知"));


	return TRUE;
}

//确定消息
void CDlgInfomation::OnOK()
{
	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
