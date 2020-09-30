#pragma once

#include "Stdafx.h"
#include "resource.h"
#include "DirectSound.h"
#include "afxtempl.h"
#include <vector>
using namespace std;

#define WM_PLAYVOICE WM_USER+1000

// CDlgPlayVoice 对话框
class CDlgPlayVoice : public CSkinDialogEx
{
	//变量定义
public:
	//控件变量
public:
	CSkinButton						m_btOK;								//确定按钮
	//函数定义
public:
	//构造函数
	CDlgPlayVoice();
	//显隐窗体
	bool ShowWindow(bool bFlags);
	//获取状态
	bool GetWndState();
	//
	bool InitVoiceList();
    
	//
	void PlayVoice(CString&str);
	//析构函数
	virtual ~CDlgPlayVoice();
protected:
	CList<CDirectSound*,CDirectSound*> m_DSObjectList;
	bool m_bIsShow;
	CListCtrl m_ListCtrl;
	int     m_nVoiceNo[3];
	int     m_nCurSelect;
	bool    m_bTimerRun;
	//重载函数
public:
		CString m_PlayList[3][31];
		
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//
	virtual void OnCancel();
public:
	DECLARE_MESSAGE_MAP()
    
	//语言选择
	afx_msg void OnSelectChange();
	//选定声音
	afx_msg void OnNMDblclkListVoice(NMHDR *pNMHDR, LRESULT *pResult);
	//窗体激活
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT nIDEvent);
};

