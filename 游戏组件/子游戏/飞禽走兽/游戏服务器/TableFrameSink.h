#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////
//历史记录
#define MAX_SCORE_HISTORY			65									//历史个数
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//总下注数
protected:
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];	//全体总注
	
	//个人下注
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//个人总注

	//控制变量
protected:
	LONGLONG						m_lAreaLimitScore;						//区域限制
	LONGLONG						m_lUserLimitScore;						//区域限制
	LONGLONG						m_lApplyBankerCondition;				//申请条件

	//玩家成绩
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//玩家成绩
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//返回下注
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//玩家税收
		

	//控制操作
protected:
	BYTE							m_cbControlArea;						//控制区域
	BYTE							m_cbControlTimes;						//控制次数

	//扑克信息
protected:
	
    BYTE							m_cbTableCardArray[2];					//桌面扑克
	BYTE                            m_cbShaYuAddMulti;                      //附加赔率
#ifdef _DEBUG
	BYTE							m_cbSelectAnimalIndex;					//动物索引
	BYTE							m_cbAnotherAnimalIndex;					//动物索引
#endif
	
	INT								m_nMultiple[AREA_ALL];					//区域倍数
	INT								m_nAnimalPercent[AREA_ALL];				//开中比例
	INT								m_nAnimalTimes[AREA_ALL];				//出现次数


	//状态变量
protected:
	DWORD							m_dwJettonTime;							//开始时间

	//庄家信息
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//申请玩家
	WORD							m_wCurrentBanker;						//当前庄家
	WORD							m_wBankerTime;							//做庄次数
	LONGLONG						m_lBankerWinScore;						//累计成绩
	LONGLONG						m_lBankerCurGameScore;					//当前成绩
	bool							m_bEnableSysBanker;						//系统做庄

	//记录变量
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//游戏记录
	int								m_nRecordFirst;							//开始记录
	int								m_nRecordLast;							//最后记录
	
	
	//控制变量
protected:
	LONGLONG						m_StorageStart;							//启动库存
	LONGLONG						m_StorageDeduct;						//扣除比例
	TCHAR							m_szConfigFileName[MAX_PATH];			//配置文件
	TCHAR							m_szGameRoomName[SERVER_LEN];			//房间名称

	//机器人控制
protected:
	int								m_nMaxChipRobot;						//最大数目 
	int								m_nChipRobotCount;						//人数统计 
	LONGLONG						m_lRobotAreaLimit;						//区域限制
	LONGLONG						m_lRobotBetCount;						//下注个数
	LONGLONG						m_lRobotAreaScore[AREA_COUNT+1];		//区域下注

	//庄家设置
protected:
	
	LONGLONG						m_lBankerMAX;							//最大庄数
	LONGLONG						m_lBankerAdd;							//庄家增数

	LONGLONG						m_lBankerScoreMAX;						//庄家钱
	LONGLONG						m_lBankerScoreAdd;						//坐庄增数

	//最大庄家数
	LONGLONG						m_lPlayerBankerMAX;						//玩家庄数

	//换庄
	bool							m_bExchangeBanker;						//交换庄家

	//时间设置
protected:						
	BYTE							m_cbFreeTime;							//空闲时间
	BYTE							m_cbBetTime;							//下注时间
	BYTE							m_cbEndTime;							//结束时间
	BYTE                            m_cbAddTime;							//附加时间

	//组件变量
protected:
	//服务控制
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerControl;

	CGameLogic						m_GameLogic;							//游戏逻辑

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;							//框架接口
	tagGameServiceOption *			m_pGameServiceOption;					//服务配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	//static ConsoleWindow			m_DebugWindow;							//调试窗体
	
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() {delete this;}

	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();

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
	//信息接口
public:
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);


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
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);
	//扣除服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//动作事件
public:
	//用户断线
	virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;

	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }

	//用户坐下
	virtual bool  OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool  OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool  OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }

#ifdef __SPECIAL___
	//银行操作
public:
	virtual bool  OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);////
#endif

	//控制
protected:
	bool OnAdminControl(CMD_C_ControlApplication* pData, IServerUserItem * pIServerUserItem);

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
	bool FindSuitBanker();
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
	void   GetAllWinArea(BYTE bcWinArea[],BYTE bcAreaCount,BYTE InArea);
	//最大下注
	LONGLONG GetUserMaxJetton(WORD wChairID, BYTE Area);

	void RandList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//获取数值
	BYTE GetRandCardValue();
	//计算比例
	void CalculateAnimalPercent();
	//获取索引
	BYTE GetAnimalIndex(BYTE cbAllIndex);

	//游戏统计
private:
	//计算得分
    LONGLONG CalculateScore();
	//试探性判断
	bool ProbeJudge();
	//读取配置
	void ReadConfigInformation();
};

//////////////////////////////////////////////////////////////////////////

#endif
