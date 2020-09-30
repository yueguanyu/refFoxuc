#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_ITEM				0xFFFF								//无效子项
//间距定义
#define DEF_X_DISTANCE				17									//默认间距
#define DEF_Y_DISTANCE				17									//默认间距
#define DEF_SHOOT_DISTANCE			20									//默认间距

//间距定义
#define DEF_X_DISTANCE_SMALL		17									//默认间距
#define DEF_Y_DISTANCE_SMALL		17									//默认间距
#define DEF_SHOOT_DISTANCE_SMALL	20									//默认间距

//消息定义
#define	IDM_LEFT_HIT_CARD			(WM_USER+500)						//左击扑克
#define	IDM_RIGHT_HIT_CARD			(WM_USER+501)						//右击扑克

#define MAX_CARD_COUNT				27									//扑克数目
#define SPACE_CARD_DATA				255									//间距扑克

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};
//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};

//编译环境
#include "..\..\..\SDKCondition.h"

#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境
#include "..\消息定义\CMD_Game.h"
#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

#include "..\消息定义\CMD_Game.h"
#include "..\..\..\..\系统模块\客户端组件\游戏框架\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../系统模块/链接库/Ansi/WHImage.lib")
//#pragma comment (lib,"../../../系统模块/链接库/Ansi/D3DEngine.lib")
#pragma comment (lib,"../../../系统模块/链接库/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../系统模块/链接库/Ansi/SkinControl.lib")
#else
#pragma comment (lib,"../../../系统模块/链接库/Unicode/WHImage.lib")
//#pragma comment (lib,"../../../系统模块/链接库/Unicode/D3DEngine.lib")
#pragma comment (lib,"../../../系统模块/链接库/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../系统模块/链接库/Unicode/SkinControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../系统模块/链接库/Ansi/WHImageD.lib")
//#pragma comment (lib,"../../../系统模块/链接库/Ansi/D3DEngineD.lib")
#pragma comment (lib,"../../../系统模块/链接库/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../系统模块/链接库/Ansi/SkinControlD.lib")
#else
#pragma comment (lib,"../../../系统模块/链接库/Unicode/WHImageD.lib")
//#pragma comment (lib,"../../../系统模块/链接库/Unicode/D3DEngineD.lib")
#pragma comment (lib,"../../../系统模块/链接库/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../系统模块/链接库/Unicode/SkinControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////
