#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//胜率控制账号信息
struct ControlAccount	
{
	DWORD dwUserID;
	int	  nOdds;
};

//开始模式
enum enStartMode
{
	enStartMode_FullReady,				//满人开始
	enStartMode_AllReady,				//所有准备
	enStartMode_Symmetry,				//对称开始
	enStartMode_TimeControl,			//时间控制
};

//分数类型
enum enScoreKind
{
	enScoreKind_Win,					//胜
	enScoreKind_Lost,					//输
	enScoreKind_Draw,					//和
	enScoreKind_Flee,					//逃
	enScoreKind_Service,				//服务
	enScoreKind_Present,				//赠送
};
//#define START_MODE_ALL_READY		0x00								//所有准备
//#define START_MODE_FULL_READY		0x01								//满人开始
//#define START_MODE_PAIR_READY		0x02								//配对开始
//#define START_MODE_TIME_CONTROL		0x10								//时间控制
//#define START_MODE_MASTER_CONTROL	0x11								//管理控制

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//总下注数
protected:
	__int64								m_iTotalJettonScore[JETTON_AREA_COUNT];				//各区域下注总额数组
	__int64								m_iUserAreaScore[GAME_PLAYER][JETTON_AREA_COUNT];	//各区域玩家所下总注额数组
	__int64								m_iAreaScoreLimit[JETTON_AREA_COUNT];				//各区域当前可下分
	__int64								m_iSysLimitScore;									//系统限注，即系统做庄时的下注限额
	static const	int					m_iOddsArray[JETTON_AREA_COUNT];					//各区域赔率
	const	int							m_iMaxAreaScore;									//各区域限制最大可下分
	bool								m_bControlWin;										//是否控制胜率，读配置
	int									m_nMaxBankerTimes;									//一个庄家周期内可以玩多少局，读配置
	int									m_nHasWinTimes;										//某庄家做庄以来，共赢了多少次

	CWHArray<ControlAccount>		m_CtrlAccountArray;									//账号控制数组


	//控制变量
protected:
	__int64							m_iUserLimitScore;						//个人限注
	__int64							m_iApplyBankerCondition;				//上庄条件
	TCHAR							m_szConfigFileName[MAX_PATH];			//配置文件

	//玩家成绩
protected:
	__int64							m_iUserWinScore[GAME_PLAYER];			//玩家成绩
	__int64							m_iUserReturnScore[GAME_PLAYER];		//返回下注
	__int64							m_iUserRevenue[GAME_PLAYER];			//玩家税收

	//扑克信息
protected:
	BYTE							m_iGoalAnimal;							//本次游戏会开出的动物
	__int64							m_iGameTimes;							//当前是游戏启动以来的第几局

	//状态变量
protected:
	DWORD							m_dwJettonTime;							//下注时间

	//庄家信息
protected:
	CWHArray<WORD>			m_ApplyUserArray;						//申请上庄玩家列表
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wBankerTime;							//做庄次数
	__int64							m_iBankerWinScore;						//累计成绩
	__int64							m_iBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄，读配置

	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	CFile							m_FileRecord;							//记录结果
	DWORD							m_dwRecordCount;						//记录数目

	//组件变量
protected:
	ITableFrame						* m_pITableFrame;						//框架接口
//	ITableFrameControl				* m_pITableFrameControl;				//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID)	{return true;}
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore)	{ }

	//继承接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) {return 0;}

	//管理接口
public:
	//复位接口
	virtual VOID RepositionSink();
	//配置接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//老陈很2加
	virtual bool __cdecl GetGameBGode(WORD nChair){return true;};
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//xxx
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) {return true;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return true;}


	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//动作事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }


	//游戏事件
protected:
	//加注事件
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 iJettonScore);
	//申请庄家
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//取消申请
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//发送函数
private:
	//发送扑克
	bool DispatchTableCard();

	//常规发牌
	void NormalDispatchCard();

	//控制发牌
	void CtrlDispatchCard(int nCtrlWinOdds = 100 );

	//发送庄家
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//更换庄家
	bool ChangeBanker(bool bCancelCurrentBanker);
	//发送记录
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//发送消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);

	//每局游戏开始时刻，计算各区域可下分值
	bool __cdecl CalculateAreaInitLimite();


	//游戏统计
private:
	//计算得分
    __int64 CalculateScore();

	//胜率计算，系统做庄或作弊时用。返回值为最有利的胜利区域，范围在0-7
	int	CalculateWinOdds();

	//计算某个区域的输赢情况，返回输赢的金币数
	bool GetOneAreaWinOdds(int nAreaIndex, __int64 & iResult);
};

//////////////////////////////////////////////////////////////////////////

#endif
