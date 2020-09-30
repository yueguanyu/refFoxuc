// AdminControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "AdminControlDlg.h"
#include <set>
#include <algorithm>
#include ".\admincontroldlg.h"
using namespace std;


// CAdminControlDlg 对话框

IMPLEMENT_DYNAMIC(CAdminControlDlg, CDialog)
CAdminControlDlg::CAdminControlDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAdminControlDlg::IDD, pParent)
{
}

CAdminControlDlg::~CAdminControlDlg()
{
}

void CAdminControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdminControlDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_CHECK_LONG, OnBnClickedCheckLong)
	ON_BN_CLICKED(IDC_CHECK_PING, OnBnClickedCheckPing)
	ON_BN_CLICKED(IDC_CHECK_HU, OnBnClickedCheckHu)
	ON_BN_CLICKED(IDC_CHECK_2_13, OnBnClickedCheck213)
	ON_BN_CLICKED(IDC_CHECK_14, OnBnClickedCheck14)
	ON_BN_CLICKED(IDC_CHECK_15_26, OnBnClickedCheck1526)
	ON_BN_CLICKED(IDC_CHECK_2_6, OnBnClickedCheck26)
	ON_BN_CLICKED(IDC_CHECK_7_11, OnBnClickedCheck711)
	ON_BN_CLICKED(IDC_CHECK_12_16, OnBnClickedCheck1216)
	ON_BN_CLICKED(IDC_CHECK_17_21, OnBnClickedCheck1721)
	ON_BN_CLICKED(IDC_CHECK_22_26, OnBnClickedCheck2226)
END_MESSAGE_MAP()

// CAdminControlDlg 消息处理程序

BOOL CAdminControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//设置信息
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
	SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数："));
	SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制："));
	SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明："));
	SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先！"));
	SetDlgItemText(IDC_CHECK_LONG,TEXT("龍"));
	SetDlgItemText(IDC_CHECK_PING,TEXT("斗"));
	SetDlgItemText(IDC_CHECK_HU,TEXT("虎"));
	SetDlgItemText(IDC_CHECK_2_13,TEXT("2-13"));
	SetDlgItemText(IDC_CHECK_14,TEXT("14"));
	SetDlgItemText(IDC_CHECK_15_26,TEXT("15-26"));
	SetDlgItemText(IDC_CHECK_2_6,TEXT("2-6"));
	SetDlgItemText(IDC_CHECK_7_11,TEXT("7-11"));
	SetDlgItemText(IDC_CHECK_12_16,TEXT("12-16"));
	SetDlgItemText(IDC_CHECK_17_21,TEXT("17-21"));
	SetDlgItemText(IDC_CHECK_22_26,TEXT("22-26"));
	SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));

	return TRUE; 

}

//控件颜色
HBRUSH CAdminControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO) 
	{  
		pDC->SetTextColor(RGB(255,10,10)); 
	} 
	return hbr;
}


//取消控制
void CAdminControlDlg::OnBnClickedButtonReset()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_CANCELS;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//本局控制
void CAdminControlDlg::OnBnClickedButtonSyn()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_UPDATE;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//开启控制
void CAdminControlDlg::OnBnClickedButtonOk()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));

	//接受控制
	if ( ((CButton*)GetDlgItem(IDC_CHECK_LONG))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_LONG] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_PING))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_PING] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_HU))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_HU] = TRUE;
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_2_13))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_2_13] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_14))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_14] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_15_26))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_15_26] = TRUE;
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_2_6))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_2_6] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_7_11))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_7_11] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_12_16))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_12_16] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_17_21))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_17_21] = TRUE;
	}
	else if ( ((CButton*)GetDlgItem(IDC_CHECK_22_26))->GetCheck() )
	{
		ControlApplication.cbControlArea[AREA_22_26] = TRUE;
	}


	int nSelectTimes = ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

	//是否有控制
	bool bNoControl = false;
	for ( int i = 0; i < AREA_ALL; ++i)
	{
		if ( ControlApplication.cbControlArea[i] == TRUE )
		{
			bNoControl = true;
			break;
		}
	}

	if ( bNoControl && nSelectTimes >= 0 && nSelectTimes != 0xffffffff)
	{
		if ( !JudgeFeasibility(ControlApplication.cbControlArea) )
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("此选项控制无合适合集，可能会造成服务器瘫痪，请重新选择！"));
			return;
		}
		ControlApplication.cbControlAppType = C_CA_SET;
		ControlApplication.cbControlTimes = static_cast<BYTE>(nSelectTimes + 1);
		CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
	}
	else
	{
		if ( !bNoControl && nSelectTimes == 0xffffffff )
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控区域！"));
		}
		else if ( !bNoControl )
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控区域！"));
		}
		else if ( nSelectTimes == 0xffffffff )
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数！"));
		}
	}
}

//取消关闭
void CAdminControlDlg::OnBnClickedButtonCancel()
{
	ShowWindow(SW_HIDE);
}

//更新控制
void CAdminControlDlg::UpdateControl( CMD_S_ControlReturns* pControlReturns )
{
	switch(pControlReturns->cbReturnsType)
	{
	case S_CR_FAILURE:
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("操作失败！"));
			break;
		}
	case S_CR_UPDATE_SUCCES:
		{
			TCHAR zsText[256] = TEXT("");
			TCHAR zsTextTemp[256] = TEXT("");
			PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
			_sntprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
			SetDlgItemText(IDC_STATIC_INFO,zsText);
			break;
		}
	case S_CR_SET_SUCCESS:
		{
			TCHAR zsText[256] = TEXT("");
			TCHAR zsTextTemp[256] = TEXT("");
			PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
			_sntprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
			SetDlgItemText(IDC_STATIC_INFO,zsText);
			break;
		}
	case S_CR_CANCEL_SUCCESS:
		{
			SetDlgItemText(IDC_STATIC_INFO,TEXT("取消设置成功！"));
			break;
		}
	}
}

//信息
void CAdminControlDlg::PrintingInfo( TCHAR* pText, WORD cbCount, BYTE cbArea[AREA_ALL], BYTE cbTimes )
{
	bool bNoControl = false;
	for ( int i = 0; i < AREA_ALL; ++i)
	{
		if ( cbArea[i] == TRUE )
		{
			bNoControl = true;
			break;
		}
	}
	if ( !bNoControl )
	{
		_sntprintf(pText,cbCount,TEXT("暂时无控制。"));
		return;
	}

	TCHAR szDesc[32] = TEXT("");
	_sntprintf(pText,cbCount,TEXT("胜利区域："));

	if ( cbArea[AREA_LONG] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【龍】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_PING] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【平】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_HU] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【虎】，") );
		lstrcat( pText,szDesc );
	}

	if ( cbArea[AREA_2_13] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【2-13】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_14] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【14】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_15_26] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【15-16】，") );
		lstrcat( pText,szDesc );
	}


	if ( cbArea[AREA_2_6] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【2-6】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_7_11] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【7-11】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_12_16] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【12-16】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_17_21] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【17-21】，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea[AREA_22_26] == TRUE )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("【22-26】，") );
		lstrcat( pText,szDesc );
	}


	_sntprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
	lstrcat( pText,szDesc );
}

//判断可行性
bool CAdminControlDlg::JudgeFeasibility( BYTE cbArea[AREA_ALL] )
{
	// 1 - 26
	// 共3个区域. 龙虎斗最后额外判断. 数组判读则只有2个区域
	set<BYTE>  setStudyOne;
	set<BYTE>  setStudyTwo;

	if ( cbArea[AREA_2_13] == TRUE )
	{
		BYTE cbTemp[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyOne.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_14] == TRUE )
	{
		BYTE cbTemp[] = {14};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyOne.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_15_26] == TRUE )
	{
		BYTE cbTemp[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyOne.insert(cbTemp[i]);
	}
	else
	{
		BYTE cbTemp[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyOne.insert(cbTemp[i]);
	}

	if ( cbArea[AREA_2_6] == TRUE )
	{
		BYTE cbTemp[] = {2, 3, 4, 5, 6};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_7_11] == TRUE )
	{
		BYTE cbTemp[] = {7, 8, 9, 10, 11};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_12_16] == TRUE )
	{
		BYTE cbTemp[] = {12, 13, 14, 15, 16};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_17_21] == TRUE )
	{
		BYTE cbTemp[] = {17, 18, 19, 20, 21};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}
	else if ( cbArea[AREA_22_26] == TRUE )
	{
		BYTE cbTemp[] = {22, 23, 24, 25, 26};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}
	else
	{
		BYTE cbTemp[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
		for (int i = 0; i < CountArray(cbTemp); ++i)
			setStudyTwo.insert(cbTemp[i]);
	}


	if ( setStudyOne.size() > 26 || setStudyTwo.size() > 26 )
	{
		ASSERT(FALSE);
		return false;
	}

	set<int> setIntersectionTempOne;
	set<int>::iterator itOne = setIntersectionTempOne.begin();
	set_intersection(setStudyOne.begin(), setStudyOne.end(), setStudyTwo.begin(), setStudyTwo.end(), inserter(setIntersectionTempOne,itOne));

	if ( setIntersectionTempOne.size() > 0 )
	{
		if ( cbArea[AREA_LONG] == TRUE || cbArea[AREA_HU] == TRUE )
		{
			for ( set<int>::iterator itr = setIntersectionTempOne.begin(); itr != setIntersectionTempOne.end(); itr++)
			{
				if ( (*itr) > 2 )
				{
					return true;
				}
			}
			return false;
		}
		else if ( cbArea[AREA_PING] == TRUE )
		{
			for ( set<int>::iterator itr = setIntersectionTempOne.begin(); itr != setIntersectionTempOne.end(); itr++)
			{
				if ( (*itr) % 2 == 0 )
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
	return false;
}


void CAdminControlDlg::OnBnClickedCheckLong()
{
	((CButton*)GetDlgItem(IDC_CHECK_PING))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_HU))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheckPing()
{
	((CButton*)GetDlgItem(IDC_CHECK_LONG))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_HU))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheckHu()
{
	((CButton*)GetDlgItem(IDC_CHECK_PING))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_LONG))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck213()
{
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_15_26))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck14()
{
	((CButton*)GetDlgItem(IDC_CHECK_2_13))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_15_26))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck1526()
{
	((CButton*)GetDlgItem(IDC_CHECK_2_13))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck26()
{
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_15_26))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_12_16))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck711()
{
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_15_26))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_12_16))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck1216()
{
	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck1721()
{
	((CButton*)GetDlgItem(IDC_CHECK_2_13))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_12_16))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_22_26))->SetCheck(FALSE);
}

void CAdminControlDlg::OnBnClickedCheck2226()
{
	((CButton*)GetDlgItem(IDC_CHECK_2_13))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_14))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_2_6))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_7_11))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_12_16))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CHECK_17_21))->SetCheck(FALSE);
}
