#ifndef GAME_OPTION_HEAD_FILE
#define GAME_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//注册表项

#define REG_GAME_CLIENT_OPTION				TEXT("GameOption")				//游戏信息

//////////////////////////////////////////////////////////////////////////


//游戏选项
class CGameOption
{
	//播放动画
public:
	BOOL 							m_bMusicVoice;								//音乐声音
	BOOL 							m_bSoundVoice;								//音效声音
	INT								m_nCurrentVolumeMusic;						//音乐大小
	INT								m_nCurrentVolumeSound;						//音效大小

	UINT							m_nStartKey;								//开始按钮值
	UINT							m_nLineCountUpKey;							//线数增加值
	UINT							m_nLineCountDownKey;						//线数减少值
	UINT							m_nLineNumberUpKey;							//线值增加值
	UINT							m_nLineNumberDownKey;						//线值减少值

	//属性变量
protected:
	TCHAR							m_szRegKeyName[16];							//注册项名

	//函数定义
public:
	//构造函数
	CGameOption();
	//析构函数
	virtual ~CGameOption();

	//功能函数
public:
	//加载参数
	VOID LoadParameter();
	//保存参数
	VOID SaveParameter();
	//默认参数
	VOID DefaultParameter();

	//配置函数
public:
	//配置参数
	bool InitParameter(LPCTSTR pszKeyName);
};


//////////////////////////////////////////////////////////////////////////

//对象说明
extern CGameOption				g_GameOption;						//全局配置

//////////////////////////////////////////////////////////////////////////

#endif