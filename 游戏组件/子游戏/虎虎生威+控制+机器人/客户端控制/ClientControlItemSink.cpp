// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_RESET_EX, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SYN_EX, OnBnClickedButtonSyn)
	ON_BN_CLICKED(IDC_BUTTON_OK_EX, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_EX, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_ZHUANG_EX, OnBnClickedCheckZhuang)
	ON_BN_CLICKED(IDC_CHECK_DONG_EX, OnBnClickedCheckDong)
	ON_BN_CLICKED(IDC_CHECK_NAN_EX, OnBnClickedCheckNan)
	ON_BN_CLICKED(IDC_CHECK_XI_EX, OnBnClickedCheckXi)
	ON_BN_CLICKED(IDC_CHECK_BEI_EX, OnBnClickedCheckBei)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	IClientControlDlg::OnInitDialog();

	//设置信息
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->AddString(TEXT("5"));
	SetDlgItemText(IDC_STATIC_TIMES_EX,TEXT("控制局数："));
	SetDlgItemText(IDC_STATIC_AREA_EX, TEXT("区域控制："));
	SetDlgItemText(IDC_STATIC_NOTIC_EX,TEXT("控制说明："));
	SetDlgItemText(IDC_STATIC_TEXT_EX,TEXT("区域输赢控制比游戏库存控制策略优先！"));
	SetDlgItemText(IDC_CHECK_ZHUANG_EX,TEXT("庄"));
	SetDlgItemText(IDC_CHECK_DONG_EX,TEXT("东"));
	SetDlgItemText(IDC_CHECK_NAN_EX,TEXT("南"));
	SetDlgItemText(IDC_CHECK_XI_EX,TEXT("西"));
	SetDlgItemText(IDC_CHECK_BEI_EX,TEXT("北"));
	SetDlgItemText(IDC_BUTTON_RESET_EX,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN_EX,TEXT("本局信息"));
	SetDlgItemText(IDC_BUTTON_OK_EX,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL_EX,TEXT("取消"));

	return TRUE; 
}

//设置颜色
HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO_EX) 
	{  
		pDC->SetTextColor(RGB(255,10,10)); 
	} 
	return hbr;
}

//取消控制
void CClientControlItemSinkDlg::OnBnClickedButtonReset()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_CANCELS;
	AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);

}

//本局控制
void CClientControlItemSinkDlg::OnBnClickedButtonSyn()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_UPDATE;
	AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//开启控制
void CClientControlItemSinkDlg::OnBnClickedButtonOk()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));

	BYTE cbSelectedIndex[MAX_INDEX] = {FALSE};
	bool bSeting = false;
	if(((CButton*)GetDlgItem(IDC_CHECK_ZHUANG_EX))->GetCheck())
	{
		bSeting = true;
		cbSelectedIndex[BANKER_INDEX] = TRUE;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_DONG_EX))->GetCheck())
	{
		bSeting = true;
		cbSelectedIndex[SHUN_MEN_INDEX] = TRUE;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_NAN_EX))->GetCheck())
	{
		bSeting = true;
		cbSelectedIndex[DUI_MEN_INDEX] = TRUE;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_XI_EX))->GetCheck())
	{
		bSeting = true;
		cbSelectedIndex[DAO_MEN_INDEX] = TRUE;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_BEI_EX))->GetCheck())
	{
		bSeting = true;
		cbSelectedIndex[HUAN_MEN_INDEX] = TRUE;
	}

	int nSelectTimes = ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES_EX))->GetCurSel();

	if ( bSeting && nSelectTimes >= 0 && nSelectTimes != 0xffffffff)
	{
		ControlApplication.cbControlAppType = C_CA_SET;
		memcpy(ControlApplication.cbControlArea, cbSelectedIndex, sizeof(ControlApplication.cbControlArea));
		ControlApplication.cbControlTimes = static_cast<BYTE>(nSelectTimes + 1);
		AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_INFO_EX,TEXT("请选择受控次数以及受控区域！"));
	}
}

//取消关闭
void CClientControlItemSinkDlg::OnBnClickedButtonCancel()
{
	ShowWindow(SW_HIDE);
}

//更新控制
void __cdecl CClientControlItemSinkDlg::UpdateControl( CMD_S_ControlReturns* pControlReturns )
{
	switch(pControlReturns->cbReturnsType)
	{
	case S_CR_FAILURE:
		{
			SetDlgItemText(IDC_STATIC_INFO_EX,TEXT("操作失败！"));
			break;
		}
	case S_CR_UPDATE_SUCCES:
		{
			TCHAR zsText[256] = TEXT("");
			TCHAR zsTextTemp[256] = TEXT("");
			PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
			_snprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
			SetDlgItemText(IDC_STATIC_INFO_EX,zsText);
			break;
		}
	case S_CR_SET_SUCCESS:
		{
			TCHAR zsText[256] = TEXT("");
			TCHAR zsTextTemp[256] = TEXT("");
			PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
			_snprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
			SetDlgItemText(IDC_STATIC_INFO_EX,zsText);
			break;
		}
	case S_CR_CANCEL_SUCCESS:
		{
			SetDlgItemText(IDC_STATIC_INFO_EX,TEXT("取消设置成功！"));
			break;
		}
	}
}

//信息
void CClientControlItemSinkDlg::PrintingInfo( TCHAR* pText, WORD cbCount, BYTE cbArea[MAX_INDEX], BYTE cbTimes )
{
	if (cbArea[BANKER_INDEX] != TRUE 
		&& cbArea[SHUN_MEN_INDEX] != TRUE 
		&& cbArea[DUI_MEN_INDEX] != TRUE 
		&& cbArea[DAO_MEN_INDEX] != TRUE
		&& cbArea[HUAN_MEN_INDEX] != TRUE)
	{
		_snprintf(pText,cbCount,TEXT("暂时无控制。"));
		return;
	}
	TCHAR szDesc[32] = TEXT("");
	_snprintf(pText,cbCount,TEXT("胜利区域："));
	if ( cbArea[BANKER_INDEX] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("庄，") );
		lstrcat( pText,szDesc );
	}
	if ( cbArea[SHUN_MEN_INDEX] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("东，") );
		lstrcat( pText,szDesc );
	}
	if ( cbArea[DUI_MEN_INDEX] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("南，") );
		lstrcat( pText,szDesc );
	}
	if ( cbArea[DAO_MEN_INDEX] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("西，") );
		lstrcat( pText,szDesc );
	}
	if ( cbArea[HUAN_MEN_INDEX] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("北，") );
		lstrcat( pText,szDesc );
	}
	_snprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
	lstrcat( pText,szDesc );
}

//庄
void CClientControlItemSinkDlg::OnBnClickedCheckZhuang()
{
	((CButton*)GetDlgItem(IDC_CHECK_DONG_EX))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_NAN_EX))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_XI_EX))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_BEI_EX))->SetCheck(FALSE);
}

//东
void CClientControlItemSinkDlg::OnBnClickedCheckDong()
{
	((CButton*)GetDlgItem(IDC_CHECK_ZHUANG_EX))->SetCheck(FALSE);
}

//南
void CClientControlItemSinkDlg::OnBnClickedCheckNan()
{
	((CButton*)GetDlgItem(IDC_CHECK_ZHUANG_EX))->SetCheck(FALSE);
}

//西
void CClientControlItemSinkDlg::OnBnClickedCheckXi()
{
	((CButton*)GetDlgItem(IDC_CHECK_ZHUANG_EX))->SetCheck(FALSE);
}

//北
void CClientControlItemSinkDlg::OnBnClickedCheckBei()
{
	((CButton*)GetDlgItem(IDC_CHECK_ZHUANG_EX))->SetCheck(FALSE);
}
