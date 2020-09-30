#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

#include "GameLogic.h"

#define GER_HUNTER			0x04			 //抄底结束
#define MAX_SCOREGRADE		100				 //最高等级
struct tagStack
{
	WORD wChairID;
	LONGLONG wBombScore;
};

//////////////////////////////////////////////////////////////////////////
//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	BYTE							m_cbCurGrade;						//几分等级
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_cbRandCard[FULL_COUNT];			//混乱扑克
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
	
	//历史积分
protected:
	LONGLONG						m_lAllTurnScore[GAME_PLAYER];		//总局得分
	LONGLONG						m_lLastTurnScore[GAME_PLAYER];		//上局得分
	

	//胜利信息
protected:
	WORD							m_wGameWiner;									//胜利列表
	BYTE							m_cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT];		//炸弹计数,最多12项
	
	//出牌信息
protected:
	WORD							m_wTurnWiner;								//胜利玩家
	BYTE							m_cbTurnCardCount;							//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];				//出牌数据
	BYTE							m_cbOutCardCount[GAME_PLAYER];				//出牌数目
	BYTE							m_cbOutCardData[GAME_PLAYER][MAX_COUNT];	//出牌列表
	tagAnalyseResult				m_LastOutCard;								//最后出牌

	//扑克信息
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];				//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame						* m_pITableFrame;					//框架接口
	tagGameServiceAttrib *			m_pGameServiceAttrib;					//游戏属性
	tagGameServiceOption *			m_pGameServiceOption;					//游戏配置

	//属性变量
protected:
	static const WORD				m_wPlayerCount;						//游戏人数
	static const BYTE				m_GameStartMode;					//开始模式
	//配置信息
protected:
	int								m_nScoreGrade;
	LONGLONG						m_lScoreList[MAX_SCOREGRADE][13];				//积分列表
	TCHAR							m_szGameRoomName[LEN_SERVER];			//房间名称
	TCHAR						    m_szConfigFileName[MAX_PATH];			//配置文件
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() {  delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);


	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID RepositionSink();


	//信息接口
public:
	//开始模式
	virtual BYTE GetGameStartMode();
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize) { return true; }
	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}
	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){}
	//游戏事件
protected:
	//用户放弃
	bool OnSubPassCard(WORD wChairID);
	//用户托管
	bool OnSubTrustee(WORD wChairID,bool bTrustee);
	//用户出牌
	bool OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//玩家抄底
	bool OnSubReqHunter(WORD wChairID,IServerUserItem * pIServerUserItem);
	
	//辅助函数
protected:
	//炸弹转换
	bool OnBombTrans(BYTE cbBombInfo[]);
	//获取算分等级
	BYTE GetUserMinGrade(BYTE& cbGrade);
	//获取贡献分
	bool GetBombScore(LONGLONG *lBombScore);
	//计算得分
	bool CalculationRates(tagScoreInfo ScoreInfo[]);
	//发送消息
	bool SendTableMessage(LPCTSTR lpszMessage, WORD wMessageType);
	//读取配置
	bool ReadConfigInformation(bool bReadFresh);
	//默认配置
	void SetAcquiesceValue(); 
};

//////////////////////////////////////////////////////////////////////////

#endif