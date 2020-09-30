#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrameSink.h"

//自定义配置
struct tagCustomConfig
{
	//上庄信息
	LONGLONG						lApplyBankerCondition;			//申请条件
	LONGLONG						lBankerMAX;						//最大庄家数
	LONGLONG						lBankerAdd;						//庄家增加数
	LONGLONG						lBankerScoreMAX;				//庄家钱
	LONGLONG						lBankerScoreAdd;				//庄家钱大时,坐庄增加数
	BOOL							nEnableSysBanker;				//系统做庄

	//时间
	BYTE							cbFreeTime;						//空闲时间
	BYTE							cbBetTime;						//下注时间
	BYTE							cbEndTime;						//结束时间

	//库存
	LONGLONG						StorageStart;					//房间启动每桌子的库存数值，读取失败按 0 设置
	LONGLONG						StorageDeduct;					//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置

	//服务费
	int								nServiceCharge;					//服务费

	//区域限制
	LONGLONG						lAreaLimitScore;				//区域限制
	LONGLONG						lUserLimitScore;				//区域限制


	//构造函数
	tagCustomConfig()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		lApplyBankerCondition = 10000000;
		lBankerMAX = 10;
		lBankerAdd = 10;
		lBankerScoreMAX = 100000000;
		lBankerScoreAdd = 10;
		nEnableSysBanker = TRUE;

		cbFreeTime = 10;
		cbBetTime = 15;
		cbEndTime = 20;

		StorageStart = 0;
		StorageDeduct = 1;

		nServiceCharge = 0;

		lAreaLimitScore = 1000000000;
		lUserLimitScore = 100000000;
	}
};


class CDlgCustomRule : public CDialog
{
	//配置变量
protected:
	WORD							m_wCustomSize;						//配置大小
	LPBYTE							m_pcbCustomRule;					//配置缓冲

	//配置结构
protected:
	tagCustomConfig					m_CustomConfig;						//自定配置

	//函数定义
public:
	//构造函数
	CDlgCustomRule();
	//析构函数
	virtual ~CDlgCustomRule();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//设置配置
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//保存数据
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//默认数据
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);

	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////////////////////////////////////////

#endif