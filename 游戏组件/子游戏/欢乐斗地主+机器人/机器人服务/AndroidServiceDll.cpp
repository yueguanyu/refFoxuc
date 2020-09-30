#include "Stdafx.h"
#include "AfxDllx.h"

//////////////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE AndroidServiceDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////////////

//导出函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(AndroidServiceDLL, hInstance)) return 0;
		new CDynLinkLibrary(AndroidServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(AndroidServiceDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
