#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wCurrentUser;						//当前玩家	
	__int64							m_nTatolScore[GAME_PLAYER];			//总的输赢分数
	__int64							m_nTurnScore[GAME_PLAYER];			//上一轮输赢
	__int64                         m_baseScore;                        //底注
 	BYTE							m_cbPlayOutTimeNum[GAME_PLAYER];	//游戏超时次数,超时5次算输
	BYTE                            m_ball[GAME_PLAYER][8];              //每个人打进去的球
	BYTE                            m_ballNum[GAME_PLAYER];
	BYTE						    m_userBallType[GAME_PLAYER];         //1：是大球，0：是小球
	bool                            m_bFirstBall;                        //是否是第一个进球
	bool                            m_bWrongHit[GAME_PLAYER];            //是否击球犯规
	bool                            m_bHit;
	PT                              m_ptBallPos[BALL_NUM];               //每个球的位置
	BYTE                            m_comob[GAME_PLAYER];                //连击
	BYTE                            m_currentComob;                      //当前的连击数
	BYTE                            m_roundCount;                        //第几局
	bool                            m_bGameStart;
	bool                            m_bSendHitResult[GAME_PLAYER];       //是否收到击球结果
	CMD_S_HitResult                 m_hitResult;                         //击球结果
	CMD_S_HitBall                   m_hitBall;                           //击球数据
	BYTE                            m_endReason;                         //结束原因

	//组件变量
protected:
	ITableFrame						* m_pITableFrame;					//框架接口
	tagGameServiceOption *			m_pGameServiceOption;					//游戏配置
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
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();


	//游戏事件
public:
	//游戏开始
	virtual bool  OnEventGameStart();
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

	
	//用户动作事件
public:
	//用户断线
	virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool  OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool  OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool  OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//查询服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//事件接口
public:
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//设置积分
	virtual void SetGameBaseScore(LONG lBaseScore) { }
	//游戏事件
protected:	
	//开球失败，从新开球
	void OnFaultBegin();
	//打进球
	bool OnUserHitBallResult(const void * pDataBuffer);
	
};

//////////////////////////////////////////////////////////////////////////

#endif