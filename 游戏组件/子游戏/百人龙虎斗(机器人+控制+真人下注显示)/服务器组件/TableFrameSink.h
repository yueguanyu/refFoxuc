#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "ServerControl.h"
#include "HistoryScore.h"

//////////////////////////////////////////////////////////////////////////////////
//历史记录
#define MAX_SCORE_HISTORY			16									//历史个数
//////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//总下注数
protected:
	LONGLONG							m_lAreaInAllScore[AREA_ALL];			//每个区域的总分				


	//个人下注
protected:
	LONGLONG							m_lUserInAllScore[GAME_PLAYER][AREA_ALL];//每个玩家每个区域的下注

	//控制变量
protected:
	bool								m_bRefreshCfg;							//每盘刷新
	LONGLONG							m_lAreaLimitScore;						//区域限制
	LONGLONG							m_lUserLimitScore;						//个人限制
	LONGLONG							m_lApplyBankerCondition;				//申请条件
	TCHAR								m_szConfigFileName[MAX_PATH];			//配置文件
	TCHAR								m_szGameRoomName[LEN_SERVER];			//房间名称
	LONGLONG							m_StorageStart;							//房间启动每桌子的库存数值，读取失败按 0 设置
	LONGLONG							m_StorageDeduct;						//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置


	//玩家成绩
protected:
	LONGLONG							m_lUserWinScore[GAME_PLAYER];			//玩家成绩
	LONGLONG							m_lUserReturnScore[GAME_PLAYER];		//返回下注
	LONGLONG							m_lUserRevenue[GAME_PLAYER];			//玩家税收

	//扑克信息
protected:
	BYTE							m_cbCardCount[2];						//扑克数目
	BYTE							m_cbTableCardArray[2][3];				//桌面扑克

	//状态变量
protected:
	DWORD							m_dwJettonTime;							//开始时间

	//庄家信息
protected:
	CWHArray<WORD>			m_ApplyUserArray;						//申请玩家
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wBankerTime;							//做庄次数
	LONGLONG						m_lBankerScore;							//庄家金币
	LONGLONG						m_lBankerWinScore;						//累计成绩
	LONGLONG						m_lBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄

	//服务控制
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;


	//机器人控制
protected:
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)
	LONGLONG						m_lRobotAreaLimit;						//机器人区域限制
	LONGLONG						m_lRobotBetCount;						//机器人下注个数
	LONGLONG						m_lRobotAreaScore[AREA_ALL];			//机器人区域下注
	int								m_nRobotListMaxCount;					//最多人数


	//庄家设置
protected:
	//加庄局数设置：当庄家坐满设定的局数之后(m_lBankerMAX)，
	//所带金币值还超过下面申请庄家列表里面所有玩家金币时，
	//可以再加坐庄m_lBankerAdd局，加庄局数可设置。
	LONGLONG						m_lBankerMAX;							//最大庄家数
	LONGLONG						m_lBankerAdd;							//庄家增加数

	//金币超过m_lBankerScoreMAX之后，就算是下面玩家的金币值大于他的金币值，他也可以再加庄m_lBankerScoreAdd局。
	LONGLONG						m_lBankerScoreMAX;						//庄家钱
	LONGLONG						m_lBankerScoreAdd;						//庄家钱大时,坐庄增加数

	//最大庄家数
	LONGLONG						m_lPlayerBankerMAX;						//玩家最大庄家数

	//换庄
	bool							m_bExchangeBanker;						//交换庄家

	//时间设置
protected:						
	BYTE							m_cbFreeTime;							//空闲时间
	BYTE							m_cbBetTime;							//下注时间
	BYTE							m_cbEndTime;							//结束时间

	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	CFile							m_FileRecord;							//记录结果
	DWORD							m_dwRecordCount;						//记录数目

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	tagGameServiceOption			* m_pGameServiceOption;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release();
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位接口
	virtual VOID RepositionSink();
	//配置接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//扣除服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};
	//游戏事件
protected:
	//加注事件
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//申请庄家
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//取消申请
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//辅助函数
private:
	//发送扑克
	bool DispatchTableCard();
	//发送庄家
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//更换庄家
	bool ChangeBanker(bool bCancelCurrentBanker);
	//发送记录
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//发送消息
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);

	//下注计算
private:
	//最大下注
	LONGLONG GetMaxPlayerScore( BYTE cbJettonArea, WORD wChairID );

	//游戏统计
private:
	//游戏结束计算
	LONGLONG GameOver();
	//计算得分
	bool CalculateScore(OUT LONGLONG& lBankerWinScore, OUT BYTE& cbResult, OUT BYTE& cbAndValues);
	//推断赢家
	void DeduceWinner(OUT INT nAreaWin[AREA_ALL], OUT BYTE& cbResult, OUT BYTE& cbAndValues);
	//读取配置
	void ReadConfigInformation(bool bReadFresh);
};

//////////////////////////////////////////////////////////////////////////////////

#endif