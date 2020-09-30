// AdminControl.cpp : implementation file
//

#include "stdafx.h"
#include "AdminControl.h"


// CAdminControl dialog

IMPLEMENT_DYNAMIC(CAdminControl, CDialog)
CAdminControl::CAdminControl(CWnd* pParent /*=NULL*/)
	: CDialog(CAdminControl::IDD, pParent)
{
}

CAdminControl::~CAdminControl()
{
}

void CAdminControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdminControl, CDialog)
	ON_BN_CLICKED(IDC_CONTROL_BANKER, OnBnClickedControlBanker)
	ON_BN_CLICKED(IDC_CONTROL_AREA, OnBnClickedControlArea)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAdminControl message handlers

void CAdminControl::OnBnClickedControlBanker()
{
	// TODO: Add your control notification handler code here
	if(((CButton*)GetDlgItem(IDC_CONTROL_BANKER))->GetCheck() == 1)
	{
		((CButton*)GetDlgItem(IDC_CONTROL_AREA))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_BANKER_LOST))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BANKER_WIN))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_AREA_SUN))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_AREA_DUI))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_AREA_DAO))->EnableWindow(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BANKER_LOST))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BANKER_WIN))->EnableWindow(FALSE);
	}
}

void CAdminControl::OnBnClickedControlArea()
{
	// TODO: Add your control notification handler code here
	if(((CButton*)GetDlgItem(IDC_CONTROL_AREA))->GetCheck() == 1)
	{
		((CButton*)GetDlgItem(IDC_CONTROL_BANKER))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_BANKER_LOST))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BANKER_WIN))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_AREA_SUN))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_AREA_DUI))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_AREA_DAO))->EnableWindow(TRUE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_AREA_SUN))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_AREA_DUI))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_AREA_DAO))->EnableWindow(FALSE);
	}
}

void CAdminControl::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CMD_C_AdminControl AdminCommand;
	memset(&AdminCommand, 0, sizeof(AdminCommand));

	if(((CButton*)GetDlgItem(IDC_CONTROL_BANKER))->GetCheck() == 1)
		AdminCommand.bCommandType = ADMIN_COMMAND_TYPE_BANKCONTROL;
	else if(((CButton*)GetDlgItem(IDC_CONTROL_AREA))->GetCheck() == 1)
		AdminCommand.bCommandType = ADMIN_COMMAND_TYPE_AREACONTROL;
	else 
		return;

	AdminCommand.bBankWin = (((CButton*)GetDlgItem(IDC_BANKER_WIN))->GetCheck() == 1) ? TRUE : FALSE;

	if(((CButton*)GetDlgItem(IDC_AREA_SUN))->GetCheck() == 1)
		AdminCommand.bWinArea = SHUN_MEN_INDEX;
	else if(((CButton*)GetDlgItem(IDC_AREA_DUI))->GetCheck() == 1)
		AdminCommand.bWinArea = DUI_MEN_INDEX;
	else if(((CButton*)GetDlgItem(IDC_AREA_DAO))->GetCheck() == 1)
		AdminCommand.bWinArea = DAO_MEN_INDEX;

	AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&AdminCommand,0);
	OnOK();
}

BOOL CAdminControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CONTROL_AREA))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BANKER_LOST))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BANKER_WIN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_AREA_SUN))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_AREA_DUI))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_AREA_DAO))->SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
