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

void CClientControlItemSinkDlg::ReSetAdminWnd()
{

	if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
	}

	if(m_cbControlStyle==CS_BET_AREA)
	{
	   ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	}
	((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN))->SetCheck(m_bWinArea[0]);
	((CButton*)GetDlgItem(IDC_CHECK_DUIMEN))->SetCheck(m_bWinArea[1]);
	((CButton*)GetDlgItem(IDC_CHECK_DAOMEN))->SetCheck(m_bWinArea[2]);
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?m_cbExcuteTimes-1:-1);

	OnRadioClick();
}

//获取昵称
void __cdecl CClientControlItemSinkDlg::GetUserNickName(CString &strNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->GetWindowText(strNickName); 
}

void CClientControlItemSinkDlg::SetUserNickName(LPCTSTR szNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->AddString(szNickName);
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->SetCurSel(0);
}
void CClientControlItemSinkDlg::ResetUserNickName()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->ResetContent();
}

//申请结果
bool __cdecl CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{
	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	CString strTemp=TEXT("");
	switch(pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  				
				
				if(m_cbControlStyle==CS_BANKER_WIN || m_cbControlStyle==CS_BANKER_LOSE)
				{
					strTemp=TEXT("设置数据成功！\r\n胜利区域：");

					if(m_cbControlStyle==CS_BANKER_WIN) str.Format(TEXT("庄家赢，执行次数%d！"),m_cbExcuteTimes);
					else str.Format(TEXT("庄家输，执行次数%d！"),m_cbExcuteTimes);
				}
				else if(m_cbControlStyle==CS_BET_AREA)
				{
					strTemp=TEXT("设置数据成功！\r\n胜利区域：");

					CString strShun=TEXT(""),strDui=TEXT(""),strDao=TEXT("");
					if(m_bWinArea[0]) strShun=TEXT(" 顺 ");
					if(m_bWinArea[1]) strDui=TEXT(" 对 ");
					if(m_bWinArea[2]) strDao=TEXT(" 倒 ");

					str.Format(TEXT("，执行次数%d！"),m_cbExcuteTimes);

					str=strShun+strDui+strDao+str;
				}
				else str=TEXT("暂时无控制");
			}
			else
			{
				str.Format(TEXT("请求失败!"));
				m_cbExcuteTimes=0;
				m_cbControlStyle=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				m_cbControlStyle=0;
				m_cbExcuteTimes=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
				str="重置请求已接受!";
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				//str="服务器同步请求已接受!";
				
				tagAdminReq*pAdminReq=(tagAdminReq*)pResult->cbExtendData;
				m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
				m_cbControlStyle=pAdminReq->m_cbControlStyle;
				memcpy(m_bWinArea,pAdminReq->m_bWinArea,sizeof(m_bWinArea));

				

				if(m_cbControlStyle==CS_BANKER_WIN || m_cbControlStyle==CS_BANKER_LOSE)
				{
					strTemp=TEXT("更新数据成功！\r\n胜利区域：");

					if(m_cbControlStyle==CS_BANKER_WIN) str.Format(TEXT("庄家赢，执行次数%d！"),m_cbExcuteTimes);
					else str.Format(TEXT("庄家输，执行次数%d！"),m_cbExcuteTimes);
				}
				else if(m_cbControlStyle==CS_BET_AREA)
				{
					strTemp=TEXT("更新数据成功！\r\n胜利区域：");

					CString strShun=TEXT(""),strDui=TEXT(""),strDao=TEXT("");
					if(m_bWinArea[0]) strShun=TEXT(" 顺 ");
					if(m_bWinArea[1]) strDui=TEXT(" 对 ");
					if(m_bWinArea[2]) strDao=TEXT(" 倒 ");

					str.Format(TEXT("，执行次数%d！"),m_cbExcuteTimes);

					str=strShun+strDui+strDao+str;
				}
				else str=TEXT("暂时无控制");

				ReSetAdminWnd();
			}
			else
			{
				str.Format(TEXT("请求失败!"));
			}
			break;
		}

	default: break;
	}


	//SetWindowText(str);
	str=strTemp+str;
	SetDlgItemText(IDC_STATIC_INFO,str);
	return true;
}


void CClientControlItemSinkDlg::ResetUserBet()
{
	CString strPrint;
	memset(m_lAllUserBet,0,sizeof(LONGLONG)*AREA_COUNT);
	SetDlgItemText(IDC_ST_AREA1,TEXT("顺门:-----"));
	SetDlgItemText(IDC_ST_AREA2,TEXT("顺角:-----"));
	SetDlgItemText(IDC_ST_AREA3,TEXT("桥:-----"));
	SetDlgItemText(IDC_ST_AREA4,TEXT("对门:-----"));
	SetDlgItemText(IDC_ST_AREA5,TEXT("倒门:-----"));
	SetDlgItemText(IDC_ST_AREA6,TEXT("倒角:-----"));

	SetDlgItemText(IDC_ST_AREA7,TEXT("顺门:-----"));
	SetDlgItemText(IDC_ST_AREA8,TEXT("顺角:-----"));
	SetDlgItemText(IDC_ST_AREA9,TEXT("桥:-----"));
	SetDlgItemText(IDC_ST_AREA10,TEXT("对门:-----"));
	SetDlgItemText(IDC_ST_AREA11,TEXT("倒门:-----"));
	SetDlgItemText(IDC_ST_AREA12,TEXT("倒角:-----"));

}

void CClientControlItemSinkDlg::SetUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	m_lAllUserBet[cbArea]+=lScore;
	CString strPrint;
	switch(cbArea)
	{
	case 0: 
		{
			strPrint.Format(TEXT("顺门:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA1,strPrint);
			break;
		}
	case 1: 
		{
			strPrint.Format(TEXT("顺角:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA2,strPrint);
			break;
		}
	case 2: 
		{
			strPrint.Format(TEXT("桥:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA3,strPrint);
			break;
		}
	case 3:
		{
			strPrint.Format(TEXT("对门:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA4,strPrint);
			break;
		}
		
	case 4: 
		{
			strPrint.Format(TEXT("倒门:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA5,strPrint);
			break;
		}
	case 5: 
		{
			strPrint.Format(TEXT("倒角:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA6,strPrint);
			break;
		}	

	default:false;
	}
}

void CClientControlItemSinkDlg::SetUserGameScore(BYTE cbArea,LONGLONG lScore)
{
	CString strPrint;
	switch(cbArea)
	{
	case 1: 
		{
			strPrint.Format(TEXT("顺门:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA7,strPrint);
			break;
		}
	case 2: 
		{
			strPrint.Format(TEXT("顺角:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA8,strPrint);
			break;
		}
	case 3: 
		{
			strPrint.Format(TEXT("桥:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA9,strPrint);
			break;
		}
	case 4:
		{
			strPrint.Format(TEXT("对门:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA10,strPrint);
			break;
		}

	case 5: 
		{
			strPrint.Format(TEXT("倒门:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA11,strPrint);
			break;
		}
	case 6: 
		{
			strPrint.Format(TEXT("倒角:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA12,strPrint);
			break;
		}	


	default:false;
	}
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BT_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BT_CANCEL,OnCancel)
	ON_BN_CLICKED(IDC_BT_EXCUTE,OnExcute)
	ON_BN_CLICKED(IDC_BT_CURSET,OnRefresh)
	ON_BN_CLICKED(IDC_RADIO_CT_AREA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CT_BANKER,OnRadioClick)
	ON_BN_CLICKED(IDC_BT_GET, OnBnClickedBtGet)
	ON_BN_CLICKED(IDC_BT_CHEAK, OnBnClickedBtCheak)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

void CClientControlItemSinkDlg::OnRadioClick()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck()==1)
	{
		GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(FALSE);
	}
	else
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(TRUE);
		}
	}
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

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnExcute()
{
	BYTE cbIndex=0;
	bool bFlags=false;
	m_cbControlStyle=0;
	m_cbExcuteTimes=0;
	ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

	//控制庄家
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
		{
			m_cbControlStyle=CS_BANKER_WIN;
			bFlags=true;
		}
		else
		{
			if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
			{
				m_cbControlStyle=CS_BANKER_LOSE;
				bFlags=true;
			}
		}
	}
	else //控制区域
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			m_cbControlStyle=CS_BET_AREA;
			for(cbIndex=0;cbIndex<CONTROL_AREA;cbIndex++)
				m_bWinArea[cbIndex]=((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN+cbIndex))->GetCheck()?true:false;
			bFlags=true;
		}
	}

	m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
	//获取执行次数
	if(m_cbExcuteTimes<=0)
	{
		bFlags=false;
	}
	

	if(bFlags) //参数有效
	{
		CMD_C_AdminReq adminReq;
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		tagAdminReq*pAdminReq=(tagAdminReq*)adminReq.cbExtendData;
		pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
		pAdminReq->m_cbControlStyle=m_cbControlStyle;
		memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	}
	else
	{
		//AfxMessageBox(TEXT("请选择受控次数以及受控方式!"));
		SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控方式!"));
		//OnRefresh();
	}
}

void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_BT_RESET,TEXT("取消控制"));	
	SetDlgItemText(IDC_BT_CURSET,TEXT("当前设置"));
	SetDlgItemText(IDC_BT_EXCUTE,TEXT("执行"));	
	SetDlgItemText(IDC_BT_CANCEL,TEXT("取消"));	
	SetDlgItemText(IDC_RADIO_WIN,TEXT("庄家赢"));	
	SetDlgItemText(IDC_RADIO_LOSE,TEXT("庄家输"));			 
	SetDlgItemText(IDC_RADIO_CT_BANKER,TEXT("庄家控制"));	
	SetDlgItemText(IDC_RADIO_CT_AREA,TEXT("区域控制"));	
	SetDlgItemText(IDC_CHECK_SHUNMEN,TEXT("顺门"));	
	SetDlgItemText(IDC_CHECK_DUIMEN	,TEXT("对门"));	
	SetDlgItemText(IDC_CHECK_DAOMEN	,TEXT("倒门"));		
	SetDlgItemText(IDC_STATIC_TIMES	,TEXT("控制次数"));	
	SetDlgItemText(IDC_STATIC_CHOICE,TEXT("控制选项"));	
	SetDlgItemText(IDC_STATIC_NOTICE,TEXT("说明"));
	SetDlgItemText(IDC_STATIC_NOTICE1,TEXT("1.区域输赢控制比游戏库存控制策略优先。"));	
	SetDlgItemText(IDC_STATIC_NOTICE2,TEXT("2.区域控制，选中为胜，未选中为输。"));	

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));

	((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	OnRadioClick();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BT_EXCUTE)->EnableWindow(bEnable);
}




HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO) 
	{  
		pDC->SetTextColor(RGB(255,10,10)); 
	} 

	return hbr;
}
