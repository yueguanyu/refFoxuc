// AdminControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "AdminControlDlg.h"
#include "../游戏客户端/GameClientEngine.h"
#include ".\admincontroldlg.h"


// CAdminControlDlg 对话框

IMPLEMENT_DYNAMIC(CAdminControlDlg, IClientControlDlg)
CAdminControlDlg::CAdminControlDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CAdminControlDlg::IDD, pParent)
{
}

CAdminControlDlg::~CAdminControlDlg()
{
}

void CAdminControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdminControlDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_CHEAK, OnBnClickedBtCheak)
	ON_BN_CLICKED(IDC_BT_GET, OnBnClickedBtGet)
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
	SetDlgItemText(IDC_RADIO_BEN_40,TEXT("大虎×40"));
	SetDlgItemText(IDC_RADIO_BAO_30,TEXT("大狗×30"));
	SetDlgItemText(IDC_RADIO_AO_20,TEXT("大马×20"));
	SetDlgItemText(IDC_RADIO_DA_10,TEXT("大蛇×10"));
	SetDlgItemText(IDC_RADIO_BEN_5,TEXT("小虎×5"));
	SetDlgItemText(IDC_RADIO_BAO_5,TEXT("小狗×5"));
	SetDlgItemText(IDC_RADIO_AO_5,TEXT("小马×5"));
	SetDlgItemText(IDC_RADIO_DA_5,TEXT("小蛇×5"));
	SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));
	return TRUE;  
}

//设置颜色
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
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//本局控制
void CAdminControlDlg::OnBnClickedButtonSyn()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_UPDATE;

	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//开启控制
void CAdminControlDlg::OnBnClickedButtonOk()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));

	BYTE cbSelectedIndex = 0xff;
	if(((CButton*)GetDlgItem(IDC_RADIO_BEN_40))->GetCheck())
		cbSelectedIndex = 0;
	else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_30))->GetCheck())
		cbSelectedIndex = 1;
	else if(((CButton*)GetDlgItem(IDC_RADIO_AO_20))->GetCheck())
		cbSelectedIndex = 2;
	else if(((CButton*)GetDlgItem(IDC_RADIO_DA_10))->GetCheck())
		cbSelectedIndex = 3;
	else if(((CButton*)GetDlgItem(IDC_RADIO_BEN_5))->GetCheck())
		cbSelectedIndex = 4;
	else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_5))->GetCheck())
		cbSelectedIndex = 5;
	else if(((CButton*)GetDlgItem(IDC_RADIO_AO_5))->GetCheck())
		cbSelectedIndex = 6;
	else if(((CButton*)GetDlgItem(IDC_RADIO_DA_5))->GetCheck())
		cbSelectedIndex = 7;

	int nSelectTimes = ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

	if ( cbSelectedIndex != 0xff && nSelectTimes >= 0 && nSelectTimes != 0xffffffff)
	{
		ControlApplication.cbControlAppType = C_CA_SET;
		ControlApplication.cbControlArea = cbSelectedIndex;
		ControlApplication.cbControlTimes = static_cast<BYTE>(nSelectTimes + 1);
		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控区域！"));
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
			myprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
			SetDlgItemText(IDC_STATIC_INFO,zsText);
			break;
		}
	case S_CR_SET_SUCCESS:
		{
			TCHAR zsText[256] = TEXT("");
			TCHAR zsTextTemp[256] = TEXT("");
			PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
			myprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
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
void CAdminControlDlg::PrintingInfo( TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes )
{
	if (cbArea == 0xff )
	{
		myprintf(pText,cbCount,TEXT("暂时无控制。"));
		return;
	}
	TCHAR szDesc[32] = TEXT("");
	myprintf(pText,cbCount,TEXT("胜利区域："));
	if ( cbArea == 0 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大虎×40，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 1 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大狗×30，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 2 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大马×20，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 3 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大蛇×10，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 4 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("小虎×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 5 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("小狗×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 6 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("小马×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 7 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("小蛇×5，") );
		lstrcat( pText,szDesc );
	}
	myprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
	lstrcat( pText,szDesc );
}

void CAdminControlDlg::ResetUserBet()
{
	CString strPrint;
	memset(m_lAllUserBet,0,sizeof(LONGLONG)*AREA_COUNT);
	SetDlgItemText(IDC_ST_AREA1,TEXT("大虎×40:-----"));
	SetDlgItemText(IDC_ST_AREA2,TEXT("大狗×30:-----"));
	SetDlgItemText(IDC_ST_AREA3,TEXT("大马×20:-----"));
	SetDlgItemText(IDC_ST_AREA4,TEXT("大蛇×10:-----"));
	SetDlgItemText(IDC_ST_AREA5,TEXT("小虎×5:-----"));
	SetDlgItemText(IDC_ST_AREA6,TEXT("小狗×5:-----"));
	SetDlgItemText(IDC_ST_AREA7,TEXT("小马×5:-----"));
	SetDlgItemText(IDC_ST_AREA8,TEXT("小蛇×5:-----"));

}

void CAdminControlDlg::SetUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	m_lAllUserBet[cbArea]+=lScore;
	CString strPrint;
	switch(cbArea)
	{
	case 0: strPrint.Format(TEXT("大虎×40:%I64d"),m_lAllUserBet[cbArea]);break;
	case 1: strPrint.Format(TEXT("大狗×30:%I64d"),m_lAllUserBet[cbArea]);break;
	case 2: strPrint.Format(TEXT("大马×20:%I64d"),m_lAllUserBet[cbArea]);break;
	case 3: strPrint.Format(TEXT("大蛇×10:%I64d"),m_lAllUserBet[cbArea]);break;
	case 4: strPrint.Format(TEXT("小虎×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 5: strPrint.Format(TEXT("小狗×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 6: strPrint.Format(TEXT("小马×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 7: strPrint.Format(TEXT("小蛇×5:%I64d"),m_lAllUserBet[cbArea]);break;

	default:false;
	}
	SetDlgItemText(IDC_ST_AREA1+cbArea,strPrint);
}

void CAdminControlDlg::SetUserGameScore(BYTE cbArea,LONGLONG lScore)
{
	CString strPrint;
	switch(cbArea)
	{
	case 0: strPrint.Format(TEXT("大虎×40:%I64d"),lScore);break;
	case 1: strPrint.Format(TEXT("大狗×30:%I64d"),lScore);break;
	case 2: strPrint.Format(TEXT("大马×20:%I64d"),lScore);break;
	case 3: strPrint.Format(TEXT("大蛇×10:%I64d"),lScore);break;
	case 4: strPrint.Format(TEXT("小虎×5:%I64d"),lScore);break;
	case 5: strPrint.Format(TEXT("小狗×5:%I64d"),lScore);break;
	case 6: strPrint.Format(TEXT("小马×5:%I64d"),lScore);break;
	case 7: strPrint.Format(TEXT("小蛇×5:%I64d"),lScore);break;

	default:false;
	}
	SetDlgItemText(IDC_ST_AREA9+cbArea,strPrint);
}

void CAdminControlDlg::SetUserNickName(CString strNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->AddString(strNickName);
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->SetCurSel(0);
}
void CAdminControlDlg::ResetUserNickName()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->ResetContent();
}

void CAdminControlDlg::OnBnClickedBtCheak()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_CHEAK;
	CString strNickName;
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->GetWindowText(strNickName); 
	//GetDlgItemText(IDC_ET_GAMEID,strNickName);
	myprintf(ControlApplication.dwUserAccount,_T("%s"),strNickName);
//AfxMessageBox(ControlApplication.dwUserAccount);
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

void CAdminControlDlg::OnBnClickedBtGet()
{
	// TODO: 在此添加控件通知处理程序代码,这功能很不想做，前无古人后有来者wsj迋尐茳
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_GET_ACCOUNT,0,0);
}
