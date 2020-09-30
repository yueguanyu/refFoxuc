#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\游戏客户端\GameLogic.h"
#include "..\消息定义\CMD_LiarsDice.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink :  public ITableFrameSink, public ITableUserAction
{
	//组件变量
protected:
	CGameLogic							m_GameLogic;										//游戏逻辑
	ITableFrame							* m_pITableFrame;									//框架接口
	const tagGameServiceOption			* m_pGameServiceOption;								//配置参数
	tagGameServiceAttrib *				m_pGameServiceAttrib;								//游戏属性
	tagCustomRule *						m_pGameCustomRule;									//自定规则

	//属性变量
protected:
	static const WORD					m_wPlayerCount;										//游戏人数
	static const BYTE					m_GameStartMode;									//开始模式

	//游戏变量
protected:
	BYTE								m_bDiceData[GAME_PLAYER][5];						//骰子数组	
	LONGLONG							m_lChip;											//下注大小
	LONGLONG							m_lMaxChip;											//最大下注
	WORD								m_wCurUser;											//当前玩家
	WORD								m_wChipUser;										//下注玩家
	WORD								m_wLoseUser;										//失败玩家
	tagDiceYell							m_UserYell[GAME_PLAYER];							//最后喊话
	WORD								m_wTurnCount;										//喊话轮数
	bool								m_bHaveYellOne;										//是否喊过1
	bool								m_bHaveThrow[GAME_PLAYER];							//是否摇骰
	bool								m_bCompleteOpen[GAME_PLAYER];						//完成开骰

	//配置变量
protected:
	int									m_nCfgCellScoreType;								//基分类型	(0 按玩家最少金币的百分比 1 直接设置数值 other 算0)
	int									m_nCfgMinScoreMul;									//配置倍数	(比如1，则最小为1/4 最大为1倍, 20，则最小为1/80 最大为1/20)
	int									m_nCfgCellScore;									//配置分数	

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { }
	//是否有效
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//信息接口
public:
	//开始模式
	virtual BYTE  GetGameStartMode();
	//游戏状态
	virtual bool  IsUserPlaying(WORD wChairID);

	//消息处理
protected:
	//设置下注
	bool OnSubChip(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);
	//玩家摇骰
	bool OnSubThrowDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);
	//摇骰完毕
	bool OnSubThrowFinish(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);
	//玩家喊话
	bool OnSubYellDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);
	//玩家开骰
	bool OnSubOpenDice(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);
	//开骰结束
	bool OnSubCompleteOpen(const WORD wChairID, const void * pDataBuffer, WORD wDataSize);

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

	//////////////////////////////////////////////////////////////////////////
	//请求事件
public:
	//请求同意
	virtual bool  OnEventUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem){return true ;}
	//请求断线
	virtual bool  OnEventUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem){return true ;}
	//请求重入
	virtual bool  OnEventUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem){return true ;}
	//请求坐下
	virtual bool  OnEventUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon){return true ;}
	//请求起来
	virtual bool  OnEventUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon){return true ;}

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
};

//////////////////////////////////////////////////////////////////////////

#endif
