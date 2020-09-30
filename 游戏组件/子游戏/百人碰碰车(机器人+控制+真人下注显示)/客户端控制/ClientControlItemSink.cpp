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

	m_lStorage=0;
	m_lDeduct=0;
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
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BT_GET, OnBnClickedBtGet)
	ON_BN_CLICKED(IDC_BT_CHEAK, OnBnClickedBtCheak)
	ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE, OnBnClickedBtnUpdateStorage)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	IClientControlDlg::OnInitDialog();

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
	SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数："));
	SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制："));
	SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明："));
	SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先！"));
	SetDlgItemText(IDC_RADIO_BEN_40,TEXT("奔驰×40"));
	SetDlgItemText(IDC_RADIO_BAO_30,TEXT("宝马×30"));
	SetDlgItemText(IDC_RADIO_AO_20,TEXT("奥迪×20"));
	SetDlgItemText(IDC_RADIO_DA_10,TEXT("大众×10"));
	SetDlgItemText(IDC_RADIO_BEN_5,TEXT("奔驰×5"));
	SetDlgItemText(IDC_RADIO_BAO_5,TEXT("宝马×5"));
	SetDlgItemText(IDC_RADIO_AO_5,TEXT("奥迪×5"));
	SetDlgItemText(IDC_RADIO_DA_5,TEXT("大众×5"));
	SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局信息"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));;

	return TRUE; 
}


//更新库存
void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage()
{
	//获取字符
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	GetDlgItemText(IDC_EDIT_STORAGE,strStorage);
	//去掉空格
	strStorage.TrimLeft();
	strStorage.TrimRight();

	GetDlgItemText(IDC_EDIT_DEDUCT,strDeduct);
	//去掉空格
	strDeduct.TrimLeft();
	strDeduct.TrimRight();
	LONGLONG lStorage=0L,lDeduct=0L;
	lStorage = StrToInt(strStorage);
	lDeduct = StrToInt(strDeduct);

	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_SET_STORAGE;
	adminReq.lStorage=lStorage;
	adminReq.lStorageDeduct=lDeduct;


	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::RequestUpdateStorage()
{
	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.cbReqType=RQ_REFRESH_STORAGE;

	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) 
	{		
		pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
		//AfxMessageBox(_T("IDM_UPDATE_STORAGE  发送"));
	}

}


//更新库存
bool __cdecl CClientControlItemSinkDlg::UpdateStorage(const void * pBuffer)
{

	const CMD_S_UpdateStorage*pResult=(CMD_S_UpdateStorage*)pBuffer;

	//获取字符
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	m_lStorage=pResult->lStorage;
	m_lDeduct=pResult->lStorageDeduct;

	//获取字符
	strStorage.Format(TEXT("%I64d"),m_lStorage);
	strDeduct.Format(TEXT("%I64d"),m_lDeduct);

	SetDlgItemText(IDC_EDIT_STORAGE,strStorage);
	SetDlgItemText(IDC_EDIT_DEDUCT,strDeduct);

	SetWindowText(TEXT("库存更新成功！"));

	return true;
}

//设置颜色
HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO) 
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
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//本局控制
void CClientControlItemSinkDlg::OnBnClickedButtonSyn()
{
	CMD_C_ControlApplication ControlApplication;
	ZeroMemory(&ControlApplication, sizeof(ControlApplication));
	ControlApplication.cbControlAppType = C_CA_UPDATE;
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
}

//开启控制
void CClientControlItemSinkDlg::OnBnClickedButtonOk()
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
		if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控区域！"));
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


//更新控件
void __cdecl CClientControlItemSinkDlg::UpdateControl()
{

	RequestUpdateStorage();

}

//信息
void CClientControlItemSinkDlg::PrintingInfo( TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes )
{
	if (cbArea == 0xff )
	{
		_sntprintf(pText,cbCount,TEXT("暂时无控制。"));
		return;
	}
	TCHAR szDesc[32] = TEXT("");
	_sntprintf(pText,cbCount,TEXT("胜利区域："));
	if ( cbArea == 0 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("奔驰×40，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 1 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("宝马×30，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 2 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("奥迪×20，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 3 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大众×10，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 4 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("奔驰×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 5 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("宝马×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 6 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("奥迪×5，") );
		lstrcat( pText,szDesc );
	}
	else if ( cbArea == 7 )
	{
		_sntprintf( szDesc,CountArray(szDesc),TEXT("大众×5，") );
		lstrcat( pText,szDesc );
	}
	_sntprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
	lstrcat( pText,szDesc );
}

void CClientControlItemSinkDlg::OnBnClickedBtGet()
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_GET_ACCOUNT,0,0);
}

void CClientControlItemSinkDlg::OnBnClickedBtCheak()
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_CHEAK_ACCOUNT,0,0);
}


//重置界面
void __cdecl CClientControlItemSinkDlg::ResetUserBet()
{
	memset(m_lAllUserBet,0,sizeof(LONGLONG)*AREA_COUNT);

	SetDlgItemText(IDC_ST_AREA1,TEXT("奔驰×40:-----"));
	SetDlgItemText(IDC_ST_AREA2,TEXT("宝马×30:-----"));
	SetDlgItemText(IDC_ST_AREA3,TEXT("奥迪×20:-----"));
	SetDlgItemText(IDC_ST_AREA4,TEXT("大众×10:-----"));
	SetDlgItemText(IDC_ST_AREA5,TEXT("奔驰×5:-----"));
	SetDlgItemText(IDC_ST_AREA6,TEXT("宝马×5:-----"));
	SetDlgItemText(IDC_ST_AREA7,TEXT("奥迪×5:-----"));
	SetDlgItemText(IDC_ST_AREA8,TEXT("大众×5:-----"));

	SetDlgItemText(IDC_ST_AREA9, TEXT("奔驰×40:-----"));
	SetDlgItemText(IDC_ST_AREA10,TEXT("宝马×30:-----"));
	SetDlgItemText(IDC_ST_AREA11,TEXT("奥迪×20:-----"));
	SetDlgItemText(IDC_ST_AREA12,TEXT("大众×10:-----"));
	SetDlgItemText(IDC_ST_AREA13,TEXT("奔驰×5:-----"));
	SetDlgItemText(IDC_ST_AREA14,TEXT("宝马×5:-----"));
	SetDlgItemText(IDC_ST_AREA15,TEXT("奥迪×5:-----"));
	SetDlgItemText(IDC_ST_AREA16,TEXT("大众×5:-----"));
}

//玩家昵称
void __cdecl CClientControlItemSinkDlg::ResetUserNickName()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->ResetContent();
}

//获取昵称
void __cdecl CClientControlItemSinkDlg::GetCheckNickName(CString &strNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->GetWindowText(strNickName); 
}

//玩家昵称
void __cdecl CClientControlItemSinkDlg::SetUserNickName(LPCTSTR lpszNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->AddString(lpszNickName);
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->SetCurSel(0);
}

//下注信息
void __cdecl CClientControlItemSinkDlg::SetUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	CString strPrint;
	if(cbArea>0)cbArea=cbArea-1;

	TRACE("cbArea=%d\n",cbArea);

	switch(cbArea)
	{
	case 0: strPrint.Format(TEXT("奔驰×40:%I64d"),lScore);break;
	case 1: strPrint.Format(TEXT("宝马×30:%I64d"),lScore);break;
	case 2: strPrint.Format(TEXT("奥迪×20:%I64d"),lScore);break;
	case 3: strPrint.Format(TEXT("大众×10:%I64d"),lScore);break;
	case 4: strPrint.Format(TEXT("奔驰×5:%I64d"),lScore);break;
	case 5: strPrint.Format(TEXT("宝马×5:%I64d"),lScore);break;
	case 6: strPrint.Format(TEXT("奥迪×5:%I64d"),lScore);break;
	case 7: strPrint.Format(TEXT("大众×5:%I64d"),lScore);break;

	default:false;
	}
	SetDlgItemText(IDC_ST_AREA9+cbArea,strPrint);
}
//下注信息
void __cdecl CClientControlItemSinkDlg::SetAllUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	TRACE("AllUserBet[cbArea=%d Score=%I64d]\n",cbArea,lScore);

	m_lAllUserBet[cbArea]+=lScore;
	CString strPrint;
	switch(cbArea)
	{
	case 0: strPrint.Format(TEXT("奔驰×40:%I64d"),m_lAllUserBet[cbArea]);break;
	case 1: strPrint.Format(TEXT("宝马×30:%I64d"),m_lAllUserBet[cbArea]);break;
	case 2: strPrint.Format(TEXT("奥迪×20:%I64d"),m_lAllUserBet[cbArea]);break;
	case 3: strPrint.Format(TEXT("大众×10:%I64d"),m_lAllUserBet[cbArea]);break;
	case 4: strPrint.Format(TEXT("奔驰×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 5: strPrint.Format(TEXT("宝马×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 6: strPrint.Format(TEXT("奥迪×5:%I64d"),m_lAllUserBet[cbArea]);break;
	case 7: strPrint.Format(TEXT("大众×5:%I64d"),m_lAllUserBet[cbArea]);break;

	default:false;
	}
	SetDlgItemText(IDC_ST_AREA1+cbArea,strPrint);
}
