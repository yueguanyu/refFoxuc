#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//应用程序对象
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientApp::CGameClientApp()
{
	//AfxMessageBox("ok");
}

//析构函数
CGameClientApp::~CGameClientApp() 
{
}

//创建引擎
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
	//版本检测
	if (InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//创建引擎
	return new CGameClientDlg;
}

//////////////////////////////////////////////////////////////////////////////////
