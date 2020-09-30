// DialogControl.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogControl.h"


// CDialogControl 对话框

IMPLEMENT_DYNAMIC(CDialogControl, CDialog)
CDialogControl::CDialogControl(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControl::IDD, pParent)
{
}

CDialogControl::~CDialogControl()
{
}

void CDialgControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogControl, CDialog)
END_MESSAGE_MAP()


// CDialogControl 消息处理程序
