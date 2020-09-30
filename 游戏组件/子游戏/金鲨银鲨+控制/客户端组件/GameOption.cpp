#include "StdAfx.h"
#include "Resource.h"
#include "GameOption.h"

#define new DEBUG_NEW

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption()
{
	//属性变量
	ZeroMemory(m_szRegKeyName,sizeof(m_szRegKeyName));

	//加载参数
	DefaultParameter();

	return;
}

//析构函数
CGameOption::~CGameOption()
{
}

//加载参数
VOID CGameOption::LoadParameter()
{
	//变量定义
	CWHRegKey RegOptionItem;

	//配置表项
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,false)==true)
	{
		m_bMusicVoice=RegOptionItem.GetValue(TEXT("bMusicVoice"),m_bMusicVoice);
		m_bSoundVoice=RegOptionItem.GetValue(TEXT("bSoundVoice"),m_bSoundVoice);
		m_nCurrentVolumeMusic=RegOptionItem.GetValue(TEXT("nVolumeMusic"),m_nCurrentVolumeMusic);
		m_nCurrentVolumeSound=RegOptionItem.GetValue(TEXT("nVolumeSound"),m_nCurrentVolumeSound);

		m_nStartKey=RegOptionItem.GetValue(TEXT("nStartKey"),m_nStartKey);
		m_nLineCountUpKey=RegOptionItem.GetValue(TEXT("nLineCountUpKey"),m_nLineCountUpKey);
		m_nLineCountDownKey=RegOptionItem.GetValue(TEXT("nLineCountDownKey"),m_nLineCountDownKey);
		m_nLineNumberUpKey=RegOptionItem.GetValue(TEXT("nLineNumberUpKey"),m_nLineNumberUpKey);
		m_nLineNumberDownKey=RegOptionItem.GetValue(TEXT("nLineNumberDownKey"),m_nLineNumberDownKey);
	}

	return;
}

//保存参数
VOID CGameOption::SaveParameter()
{
	//变量定义
	CWHRegKey RegOptionItem;

	//配置表项
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_CLIENT_OPTION,true)==true)
	{
		RegOptionItem.WriteValue(TEXT("bMusicVoice"),m_bMusicVoice);
		RegOptionItem.WriteValue(TEXT("bSoundVoice"),m_bSoundVoice);
		RegOptionItem.WriteValue(TEXT("nVolumeMusic"),m_nCurrentVolumeMusic);
		RegOptionItem.WriteValue(TEXT("nVolumeSound"),m_nCurrentVolumeSound);

		RegOptionItem.WriteValue(TEXT("nStartKey"),m_nStartKey);
		RegOptionItem.WriteValue(TEXT("nLineCountUpKey"),m_nLineCountUpKey);
		RegOptionItem.WriteValue(TEXT("nLineCountDownKey"),m_nLineCountDownKey);
		RegOptionItem.WriteValue(TEXT("nLineNumberUpKey"),m_nLineNumberUpKey);
		RegOptionItem.WriteValue(TEXT("nLineNumberDownKey"),m_nLineNumberDownKey);
	}

	return;
}

//默认参数
VOID CGameOption::DefaultParameter()
{
	m_bMusicVoice = TRUE;
	m_bSoundVoice = TRUE;
	m_nCurrentVolumeMusic = -1000;
	m_nCurrentVolumeSound = 0;

	m_nStartKey = VK_RETURN;
	m_nLineCountUpKey = VK_UP;
	m_nLineCountDownKey = VK_DOWN;
	m_nLineNumberUpKey = VK_RIGHT;
	m_nLineNumberDownKey = VK_LEFT;

	return;
}

//配置参数
bool CGameOption::InitParameter(LPCTSTR pszKeyName)
{
	//设置变量
	lstrcpyn( m_szRegKeyName,pszKeyName,CountArray(m_szRegKeyName) );

	LoadParameter();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//全局配置
CGameOption g_GameOption;

//////////////////////////////////////////////////////////////////////////
