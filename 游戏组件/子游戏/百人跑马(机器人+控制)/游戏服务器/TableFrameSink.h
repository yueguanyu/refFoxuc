#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	LONGLONG						m_lRobotScoreRange[2];				//最大范围
	LONGLONG						m_lRobotBankGetScore;				//提款数额
	LONGLONG						m_lRobotBankGetScoreBanker;			//提款数额 (庄家)
	int								m_nRobotBankStorageMul;				//存款倍数

	INT								m_nStreak;								//场次
	INT								m_nDay;									//天数
	DWORD							m_dwGameTime;							//游戏时间
	INT								m_nFreeTime;							//空闲时间
	INT								m_nBetTime;								//下注时间
	INT								m_nBetEndTime;							//下注结束时间
	INT								m_nHorsesTime;							//跑马时间
	INT								m_nHorsesEndTime;						//跑马结束时间

	//控制变量
protected:
	INT								m_nCLMultiple[AREA_ALL];				//区域倍数
	BYTE							m_cbCLTimes;							//控制次数
	BYTE							m_cbCLArea;								//控制区域
	bool							m_bControl;								//是否控制

	//游戏结果
protected:
	BYTE							m_cbGameResults;						//跑马名次(2马)

	//库存
protected:
	LONGLONG						m_StorageStart;							//房间启动每桌子的库存数值，读取失败按 0 设置
	int								m_nStorageNowNode;						//当前库存点
	int								m_nStorageIntervalTime;					//库存更换间隔时间
	int								m_nStorageCount;						//库存数目
	LONGLONG						m_StorageArray[30];			//房间启动每桌子的库存数值，读取失败按 0 设置
	INT								m_StorageDeduct;						//每局游戏结束后扣除的库存比例，读取失败按 1.00 设置

	//分数
protected:
	INT								m_nBetPlayerCount;						//下注人数
	LONGLONG						m_lPlayerBet[GAME_PLAYER][AREA_ALL];	//玩家下注
	LONGLONG						m_lPlayerBetWin[GAME_PLAYER][AREA_ALL];	//玩家区域输赢
	LONGLONG						m_lPlayerBetAll[AREA_ALL];				//所有下注
	LONGLONG						m_lPlayerWinning[GAME_PLAYER];			//玩家输赢
	LONGLONG						m_lPlayerReturnBet[GAME_PLAYER];		//玩家返回下注
	LONGLONG						m_lPlayerRevenue[GAME_PLAYER];			//玩家税收

	//区域倍数
protected:
	BOOL							m_bMultipleControl;						//倍数控制
	INT								m_nMultiple[AREA_ALL];					//区域倍数

	//马匹变量
protected:
	INT								m_nHorsesSpeed[HORSES_ALL][COMPLETION_TIME];	//每匹马的每秒速度
	BYTE							m_cbHorsesRanking[RANKING_NULL];					//马匹名次
	TCHAR							m_szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字

	//限制变量
protected:
	LONGLONG						m_lAreaLimitScore;						//区域总限制
	LONGLONG						m_lUserLimitScore;						//个人区域限制

	//房间信息
protected:
	TCHAR							m_szConfigFileName[MAX_PATH];			//配置文件
	TCHAR							m_szGameRoomName[SERVER_LEN];			//房间名称

	//游戏记录
protected:
	CWHArray<tagHistoryRecord> m_GameRecords;							//游戏记录
	INT								m_nWinCount[HORSES_ALL];				//全天赢的场次

	//机器人控制
protected:
	int								m_nMaxChipRobot;						//最大数目 (下注机器人)
	int								m_nChipRobotCount;						//人数统计 (下注机器人)
	LONGLONG						m_lRobotAreaLimit;						//机器人区域限制
	LONGLONG						m_lRobotAreaScore[AREA_ALL];			//机器人区域下注


	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	tagGameServiceOption			* m_pGameServiceOption;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性

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
	//读取长整
	LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);

	//管理接口
public:
	//复位接口
	virtual VOID RepositionSink();
	//配置接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};
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
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);	

	//游戏事件
protected:
	//加注事件
	bool OnSubPlayBet(const void * pBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//控制
protected:
	bool OnAdminControl(const void * pBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//跑马
protected:
	//跑马过程
	void HorsesProcess();
	//跑马开始
	void HorsesStart();
	//跑马结束
	bool HorsesEnd();
	//最后结果
	bool FinalResults();

	//控制
protected:
	//需要控制
	bool NeedControl();
	//满足控制
	bool MeetControl();
	//完成控制
	bool CompleteControl();
	//倍数控制
	void MultipleControl();
	//随机获得倍数
	void RandomMultiples();
	//混乱数组
	void ChaosArray(INT nArray[], INT nCount);
	//混乱数组
	void ChaosArray(INT nArrayOne[], INT nCountOne, INT nArrayTwo[], INT nCountTwo);
	//计算
protected:
	//结果计算
	bool CalculateScore();
	//玩家最大下分
	LONGLONG GetPlayersMaxBet( WORD wChiarID, BYTE cbArea = AREA_ALL );


};

//////////////////////////////////////////////////////////////////////////

#endif
