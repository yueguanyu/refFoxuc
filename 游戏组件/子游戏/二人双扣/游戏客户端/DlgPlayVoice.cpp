// DlgPlayVoice.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgPlayVoice.h"
#include "Windows.h"
#include ".\dlgplayvoice.h"

// CDlgPlayVoice 对话框

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgPlayVoice, CSkinDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1,OnSelectChange)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VOICE, OnNMDblclkListVoice)
    ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


//构造函数
CDlgPlayVoice::CDlgPlayVoice() : CSkinDialogEx(IDD_DLG_PLAYVOICE)
{
	m_nCurSelect=0;
	m_bTimerRun=false;
	return;
}

//析构函数
CDlgPlayVoice::~CDlgPlayVoice()
{
	CDirectSound*pDirectSound=NULL;
	while(m_DSObjectList.GetSize()>0)
	{
		pDirectSound=m_DSObjectList.RemoveHead();
		pDirectSound->Stop();
		delete pDirectSound;
	}
}

bool CDlgPlayVoice::GetWndState()
{
	return m_bIsShow;
}

bool CDlgPlayVoice::InitVoiceList()
{
m_PlayList[0][0]="打快打快";
m_PlayList[0][1]="动作快点";
m_PlayList[0][2]="没炸弹啊？";
m_PlayList[0][3]="破牌";
m_PlayList[0][4]="全部当算";
m_PlayList[0][5]="全大";
m_PlayList[0][6]="全散";
m_PlayList[0][7]="没吃不倒霉";
m_PlayList[0][8]="脑都炸散了";
m_PlayList[0][9]="你想要什么牌啊，讲来那";
m_PlayList[0][10]="没意思没意思";
m_PlayList[0][11]="睡着了，你动都不动";
m_PlayList[0][12]="卡你怎么打的这么苦啊";
m_PlayList[0][13]="炸弹不炸你带家啊";
m_PlayList[0][14]="把你炸晕过去！";
m_PlayList[0][15]="卡你屋里造炸弹啊";
m_PlayList[0][16]="风头霉头两隔壁";
m_PlayList[0][17]="跟你打，靠造化，打赢的";
m_PlayList[0][18]="当炸不炸，炸起秘密炸";
m_PlayList[0][19]="拔了打死得";
m_PlayList[0][20]="动作抓紧点";
m_PlayList[0][21]="对子";
m_PlayList[0][22]="放心，我帮你看着";
m_PlayList[0][23]="黄天,里牌摸来,当数北";
m_PlayList[0][24]="卡你打，真打霉";
m_PlayList[0][25]="卡你卡么牌";
m_PlayList[0][26]="卡你怎么打的这么苦啊";
m_PlayList[0][27]="里牌都打的出，人都被你气死";
m_PlayList[0][28]="你怎么了，动都不动";
m_PlayList[0][29]="三带一";
m_PlayList[0][30]="真没意思啊";

m_PlayList[1][0]="拔老打死到";
m_PlayList[1][1]="把你炸晕过去";
m_PlayList[1][2]="打快点2";
m_PlayList[1][3]="当炸不炸，炸气秘密炸";
m_PlayList[1][4]="动作快点1";
m_PlayList[1][5]="风头霉头两隔壁2";
m_PlayList[1][6]="跟你打，靠造化，打赢啊";
m_PlayList[1][7]="关公门前舞大刀啊1";
m_PlayList[1][8]="没吃不倒霉1";
m_PlayList[1][9]="没吃不倒霉2";
m_PlayList[1][10]="没味道，没味道1";
m_PlayList[1][11]="没炸弹1";
m_PlayList[1][12]="脑都被炸散了1";
m_PlayList[1][13]="你家里造炸弹的1";
m_PlayList[1][14]="破牌";
m_PlayList[1][15]="全部当算2";
m_PlayList[1][16]="全大1";
m_PlayList[1][17]="全散1";
m_PlayList[1][18]="听唱词啊你，都不动2";
m_PlayList[1][19]="想要和什么牌，讲来1";
m_PlayList[1][20]="哑巴吃黄连有苦讲不出1";
m_PlayList[1][21]="炸弹不炸你带回家啊";
m_PlayList[1][22]="站在哪里看大字报啊，都不动";

m_PlayList[2][0]="被颠人砖头扔了";
m_PlayList[2][1]="打快打快";
m_PlayList[2][2]="打快点那";
m_PlayList[2][3]="动作快点呢";
m_PlayList[2][4]="对家没料";
m_PlayList[2][5]="放心我帮你瞄着";
m_PlayList[2][6]="和你搭伙痛苦";
m_PlayList[2][7]="没吃不倒霉";
m_PlayList[2][8]="没炸弹啊";
m_PlayList[2][9]="你是什么牌型";
m_PlayList[2][10]="破牌";
m_PlayList[2][11]="全大";
m_PlayList[2][12]="全散";
m_PlayList[2][13]="让他打，白卵车";
m_PlayList[2][14]="三拖一";
m_PlayList[2][15]="双拖双";
m_PlayList[2][16]="偷鸡";
m_PlayList[2][17]="形势看透";
m_PlayList[2][18]="牙列开糖金杏列了";
m_PlayList[2][19]="炸弹不炸带回家啊";
m_PlayList[2][20]="炸的跟南斯拉夫一样";
m_PlayList[2][21]="这盘没有救了";
m_PlayList[2][22]="自己打错了，不怨";

m_nVoiceNo[0]=31;
m_nVoiceNo[1]=23;
m_nVoiceNo[2]=23;

  SetTimer(1,3000,0);
int index=0;
CString str;
for(;index<m_nVoiceNo[m_nCurSelect];index++)
{
	str.Format("%d.%s",index+1,m_PlayList[m_nCurSelect][index]);
	m_ListCtrl.InsertItem(index,"1");
	m_ListCtrl.SetItemText(index,0,str);
}
((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(m_nCurSelect);
   return true;
}



bool CDlgPlayVoice::ShowWindow(bool bFlags)
{
    bool bReturn=m_bIsShow;
	m_bIsShow=bFlags;
	int nCmdShow=SW_SHOW;
	__super::ShowWindow(bFlags?SW_SHOW:SW_HIDE);
	return bReturn;
}

//控件绑定
void CDlgPlayVoice::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX,IDC_LIST_VOICE,m_ListCtrl);
}

//初始化函数
BOOL CDlgPlayVoice::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏语音"));

	DWORD   dwStyle   =   m_ListCtrl.GetExtendedStyle(); 
	dwStyle=dwStyle|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_ListCtrl.ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0); 

	m_ListCtrl.SetExtendedStyle(dwStyle);   //设置扩展风格 
    CRect ListRect;
	m_ListCtrl.SetTextBkColor(RGB(248,249,251));
	m_ListCtrl.SetTextColor(RGB(87,100,110));

	m_ListCtrl.GetClientRect(&ListRect);
	m_ListCtrl.InsertColumn(0,"声音列表",LVCFMT_CENTER,ListRect.Width()-17);
	return TRUE;
}

//确定消息
void CDlgPlayVoice::OnOK()
{
	static bool flags=true;
	if(flags)
	{
     //  SetTimer(1,1000,0);
	   flags=false;
	   m_bTimerRun=true;
	}

	POSITION   pos   =  m_ListCtrl.GetFirstSelectedItemPosition();   
	if   (pos   !=   NULL)
	{
         int nItem   =   m_ListCtrl.GetNextSelectedItem(pos); 
		 int nSel    =   ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
		 AfxGetMainWnd()->PostMessage(WM_PLAYVOICE,nItem,nSel);
	}   
}

void CDlgPlayVoice::OnCancel()
{
	__super::ShowWindow(SW_HIDE);
	m_bIsShow=false;
}

void CDlgPlayVoice::OnSelectChange()
{
  int k=((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
   
  if(k!=m_nCurSelect)
  {
	 int index=0;
	 CString str;
     m_ListCtrl.DeleteAllItems();
	 for(;index<m_nVoiceNo[k];index++)
	 {
		 str.Format("%d.%s",index+1,m_PlayList[k][index]);
		 m_ListCtrl.InsertItem(index,"1");
		 m_ListCtrl.SetItemText(index,0,str);
	 }
	 m_nCurSelect=k;
	 ((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(k);
  }

}


void CDlgPlayVoice::OnNMDblclkListVoice(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
	*pResult = 0;
}

void CDlgPlayVoice::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if(pWndOther!=this)
	{
		ShowWindow(SW_HIDE);
        m_bIsShow=false;	
	}
	CSkinDialogEx::OnActivate(nState, pWndOther, bMinimized);
	// TODO: 在此处添加消息处理程序代码
}

void CDlgPlayVoice::PlayVoice(CString&str)
{
	CDirectSound*pDirectSound=new CDirectSound();
	if(pDirectSound->Create(str,this))
	{
		m_DSObjectList.AddTail(pDirectSound);
		pDirectSound->Play();
	}
	else
	{
		delete pDirectSound;
		AfxMessageBox("音频文件加载失败!");
	}
}


void CDlgPlayVoice::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

      CDirectSound*pDirectSound=NULL;
	  while(m_DSObjectList.GetSize()>0)
	  {
           pDirectSound=m_DSObjectList.GetHead();
		   if(pDirectSound->IsEnd())
		   {
			   pDirectSound=m_DSObjectList.RemoveHead();
			   pDirectSound->Stop();
			   delete pDirectSound;
		   }
		   else
			   break;
	  }
	  
	CSkinDialogEx::OnTimer(nIDEvent);
}


