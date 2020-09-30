#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//积分信息
struct tagScoreInfo
{
	//胡牌扑克
	BYTE							cbCardCount;							//扑克数目
	BYTE							cbCardData[MAX_COUNT];					//扑克数据

	//用户信息
	TCHAR							szUserName[GAME_PLAYER][LEN_NICKNAME];		//用户名字

	//积分信息
	WORD							wChiHuUser;								//胡牌玩家
	WORD							wProvideUser;							//供应用户
	BYTE							cbProvideCard;							//供应扑克
	SCORE						lGameScore[GAME_PLAYER];				//游戏积分
	BYTE							cbHuaCardCount;							//花牌个数
	BYTE							cbFanCount;								//总番数

	//胡牌结果
	DWORD							dwChiHuKind[GAME_PLAYER];				//胡牌类型
	bool							bMulWinner;								//一炮多响
};

//组合信息
struct tagWeaveInfo
{
	BYTE							cbWeaveCount;							//组合数目
	BYTE							cbCardCount[MAX_WEAVE];					//扑克数目
	BYTE							cbPublicWeave[MAX_WEAVE];				//公共组合
	BYTE							cbCardData[MAX_WEAVE][4];				//组合扑克
};

//////////////////////////////////////////////////////////////////////////


//游戏配置
class CGameScore : public CDialog
{
	//变量定义
protected:
	tagScoreInfo					m_ScoreInfo;							//积分信息
	CChiHuRight						m_ChiHuRight;							//胡牌信息

	//控件变量
protected:
	BYTE							m_cbWeaveCount;							//组合数目
	CWeaveCard						m_WeaveCard[MAX_WEAVE];					//组合扑克
	CSkinButton						m_btCloseScore;							//关闭按钮

	//资源变量
protected:
	CBitImage						m_ImageBack;							//背景图
	CPngImage						m_ImageZiMo;							//自摸
	CPngImage						m_ImageFangPao;							//放炮
	//CFont							m_InfoFont;								//描述字体

	//函数定义
public:
	//构造函数
	CGameScore();
	//析构函数
	virtual ~CGameScore();

	//功能函数
public:
	//复位数据
	void RestorationData();
	//设置积分
	void SetScoreInfo(const tagScoreInfo & ScoreInfo, const tagWeaveInfo & WeaveInfo, const CChiHuRight &ChiHuRight);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
