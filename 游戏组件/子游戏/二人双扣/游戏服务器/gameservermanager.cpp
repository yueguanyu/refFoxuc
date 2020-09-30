#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//全局变量
//static CGameServiceManager			g_GameServiceManager;				//管理变量

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager(void)
{
	//设置属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=FALSE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("ShuangKouEx.exe"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("ShuangKouExServer.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager(void)
{
}

//接口查询
void * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建游戏桌
void * CGameServiceManager::CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)
{
	//变量定义
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//建立对象
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");

		//查询接口
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//获取属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return true;
}

//参数修改
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & pGameServiceOption)
{
	//效验参数
	ASSERT(&pGameServiceOption!=NULL);
	if (&pGameServiceOption==NULL) return false;

	//单元积分
	pGameServiceOption.lCellScore=__max(1L,pGameServiceOption.lCellScore);

	//积分下限
	if (pGameServiceOption.wServerType==GAME_GENRE_GOLD)
	{
		pGameServiceOption.lMinTableScore=__max(pGameServiceOption.lCellScore*15L,pGameServiceOption.lMinTableScore);
	}

	//输分限制(原来的积分上限)
	if (pGameServiceOption.lRestrictScore!=0L)
	{
		pGameServiceOption.lRestrictScore=__max(pGameServiceOption.lRestrictScore,pGameServiceOption.lMinEnterScore);
	}

	//积分上限
	if (pGameServiceOption.wServerType!=GAME_GENRE_SCORE)
	{
		if(pGameServiceOption.lMaxEnterScore<=pGameServiceOption.lMinEnterScore)
			pGameServiceOption.lMaxEnterScore=0L;
		else if(pGameServiceOption.lRestrictScore>0)
			pGameServiceOption.lMaxEnterScore=__min(pGameServiceOption.lMaxEnterScore,pGameServiceOption.lRestrictScore);
	}


	return true;
}

//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}
//创建数据
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}
//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	////效验状态
	//ASSERT(m_pDlgCustomRule!=NULL);
	//if (m_pDlgCustomRule==NULL) return false;

	////变量定义
	//ASSERT(wCustonSize>=sizeof(tagCustomRule));
	//tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	////获取配置
	//if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	//{
	//	return false;
	//}

	return true;
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	////变量定义
	//ASSERT(wCustonSize>=sizeof(tagCustomRule));
	//tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	////设置变量
	//pCustomRule->cbTimeOutCard=20;
	//pCustomRule->cbTimeStartGame=30;
	//pCustomRule->cbTimeCallScore=20;
	//pCustomRule->cbTimeHeadOutCard=30;

	////游戏控制
	//pCustomRule->wMaxScoreTimes=32;
	//pCustomRule->wFleeScoreTimes=12;
	//pCustomRule->cbFleeScorePatch=FALSE;

	return true;
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	////创建窗口
	//if (m_pDlgCustomRule==NULL)
	//{
	//	m_pDlgCustomRule=new CDlgCustomRule;
	//}

	////创建窗口
	//if (m_pDlgCustomRule->m_hWnd==NULL)
	//{
	//	//设置资源
	//	AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

	//	//创建窗口
	//	m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

	//	//还原资源
	//	AfxSetResourceHandle(GetModuleHandle(NULL));
	//}

	////设置变量
	//ASSERT(wCustonSize>=sizeof(tagCustomRule));
	//m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	////显示窗口
	//m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	//return m_pDlgCustomRule->GetSafeHwnd();
	return NULL;
}
//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
