#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////


//游戏逻辑类
class CGameLogic
{
	//静态变量
private:
	static CGameLogic			m_GameLogic;

	//函数定义
private:
	//构造函数
	CGameLogic();
	//析构函数		
	virtual ~CGameLogic();

public:
	//获取对象
	static CGameLogic* GetInstance()
	{
		return &m_GameLogic;
	}

public:
	//随机区域
	INT RandomArea(INT nLen, ...);
	//动物类型
	int	AnimalType( int nAnimal );

	//转盘中动物
	int TurnTableAnimal( int nTableIndex );
	//转盘中动物类型
	int TurnTableAnimalType( int nTableIndex );
};

//////////////////////////////////////////////////////////////////////////////////

#endif