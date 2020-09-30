#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//下注数
protected:
	LONG							m_lCellScore;							//单位积分
	LONGLONG						m_lAllBet[ANIMAL_MAX];					//总下注
	LONGLONG						m_lPlayBet[GAME_PLAYER][ANIMAL_MAX];	//玩家下注
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LONGLONG						m_HPlayBet[ANIMAL_MAX];                 //会员玩家下注统计
	LONGLONG						m_FPlayBet[ANIMAL_MAX];					//非会员玩家下注统计
	LONGLONG						m_APlayBet[ANIMAL_MAX];					//机器人下注统计
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//玩家分数
protected:
	DWORD							m_dwDrawStartTime[GAME_PLAYER];			//玩家开始时间
	LONGLONG						m_lPlayOriginalScore[GAME_PLAYER];		//玩家原始分数
	LONGLONG						m_lPlayScore[GAME_PLAYER];				//玩家分数
	LONGLONG						m_lPlayChip[GAME_PLAYER];				//玩家筹码

	//筹码
protected:
	LONGLONG						m_lPlayWinAnimal[GAME_PLAYER][ANIMAL_MAX];	//玩家输赢（区域）
	LONGLONG						m_lPlayWinChip[GAME_PLAYER];			//玩家成绩（总）
	LONGLONG						m_lPlayWinScore[GAME_PLAYER][2];		//玩家得分（两阶段）
	LONGLONG						m_lPlayWinPrizes[GAME_PLAYER];			//玩家彩金

	//转盘路径
protected:
	BOOL							m_bTurnTwoTime;							//转2次
	int								m_nTurnTableTarget[2];					//转盘目标
	CWHArray<int>					m_ArrayTurnTableRecord;					//转盘纪录

	//倍数信息
protected:
	int								m_nLastMultipleIndex;					//上一次的索引
	int								m_nAnimalMultiple[ANIMAL_MAX];			//倍数

	//生成几率
protected:
	int								m_nGenerationChance[ANIMAL_MAX];		//生成几率

	//服务控制
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;

	//状态变量
protected:
	DWORD							m_dwBetTime;							//开始时间

	//时间设置
protected:
	BYTE							m_cbFreeTime;							//空闲时间
	BYTE							m_cbBetTime;							//下注时间
	BYTE							m_cbEndTime;							//结束时间

	//设置变量
protected:
	LONGLONG						m_lAreaLimitScore;						//区域限制
	LONGLONG						m_lPlayLimitScore;						//玩家限制
	static LONGLONG					m_lStorageStart;						//房间启动每桌子的库存数值，读取失败按 0 设置
	static LONGLONG					m_lStorageDeduct;						//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;							//框架接口
	tagGameServiceOption*			m_pGameServiceOption;					//配置参数

	TCHAR								szConfigFileName[MAX_PATH];			//配置文件
	TCHAR								m_szGameRoomName[LEN_SERVER];			//房间名称

	//机器人控制
protected:
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)
	LONGLONG						m_lRobotAreaLimit;						//机器人区域限制
	LONGLONG						m_lRobotBetCount;						//机器人下注个数
	LONGLONG						m_lRobotAreaScore[ANIMAL_MAX];			//机器人区域下注
	int								m_nRobotListMaxCount;					//最多人数
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
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();


	BYTE	GetProbility();


	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }

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
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore) {}

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//消息函数
public:
	//筹码兑换消息
	bool OnSubChip(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//用户下注消息
	bool OnSubPlayBet(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//清除下注消息
	bool OnSubBetClear(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);


	//功能函数
public:
	//生成转盘
	bool CreateTableTable();
	//游戏结束计算
	int GameOver();
	//计算得分
	LONGLONG CalculateScore( bool bFirst, int nWinAnimal, int& nPrizesMultiple );
	//结束写分
	void GameOverWriteScore();
};

//////////////////////////////////////////////////////////////////////////////////

#endif