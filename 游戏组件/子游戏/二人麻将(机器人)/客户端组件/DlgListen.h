#ifndef DLG_RESPONSE_LEAVE_HEAD_FILE
#define DLG_RESPONSE_LEAVE_HEAD_FILE

#pragma once


#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//请求离开回应
class CDlgListen : public CSkinDialog
{
	//定义变量
protected:
	WORD							m_wTimer;							//定时时间
	CString							m_strCaption;						//对话框标题

public:
	bool							m_bZhuaPao;							//抓炮
	bool							m_bZiMo;							//自摸

	//控件变量
public:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CDlgListen(CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CDlgListen();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

#endif
