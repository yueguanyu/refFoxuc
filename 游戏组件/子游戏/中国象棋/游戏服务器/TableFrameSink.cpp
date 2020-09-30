#include "Stdafx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

#define RECTIFY_TIME				3										//调整时间
#define RULE_STEP_TIME				30										//规则步时
#define RULE_LIMIT_TIME				1800									//规则限时
#define RULE_TIME_OUT_COUNT			3										//规则超时

//////////////////////////////////////////////////////////////////////////

//输出信息
void TraceMessage(LPCTSTR pszMessage)
{
	CFile File;
	if ((File.Open(TEXT("TraceData.txt"),CFile::modeWrite)==FALSE)&&
		(File.Open(TEXT("TraceData.txt"),CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_FULL_READY;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//规则变量
	m_bGameRule=false;
	m_wRuleRoundID=0L;
	ZeroMemory(&m_GameRuleInfo,sizeof(m_GameRuleInfo));

	//用户变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(&m_wLeaveDrawTime,sizeof(m_wLeaveDrawTime));

	//请求状态
	m_cbConcludeReason=0;
	ZeroMemory(&m_cbPeaceCount,sizeof(m_cbPeaceCount));
	ZeroMemory(&m_cbRegretCount,sizeof(m_cbRegretCount));
	ZeroMemory(&m_cbRequestStatus,sizeof(m_cbRequestStatus));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//设置规则
	m_GameRuleInfo.cbDirections=TRUE;
	m_GameRuleInfo.cbRegretFlag=TRUE;
	m_GameRuleInfo.cbCurrentMode=MODE_SLOW;
	m_GameRuleInfo.wRuleDrawTime=MODE_SLOW_DRAW_TIME;
	m_GameRuleInfo.wRuleStepTime=MODE_SLOW_STEP_TIME;
	m_GameRuleInfo.wRuleSecondTime=MODE_SLOW_SECOND_TIME;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;


	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	return true;
}

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//用户变量
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(&m_wLeaveDrawTime,sizeof(m_wLeaveDrawTime));

	//请求状态
	m_cbConcludeReason=0;
	ZeroMemory(&m_cbPeaceCount,sizeof(m_cbPeaceCount));
	ZeroMemory(&m_cbRegretCount,sizeof(m_cbRegretCount));
	ZeroMemory(&m_cbRequestStatus,sizeof(m_cbRequestStatus));

	return;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}
//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//设置变量
	m_wCurrentUser=m_wBankerUser;
	for (WORD i=0;i<GAME_PLAYER;i++) m_wLeaveDrawTime[i]=m_GameRuleInfo.wRuleDrawTime;

	//重置棋盘
	m_GameLogic.ResetChessBorad();

	//发送开始
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.GameRuleInfo=m_GameRuleInfo;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	return true;
}

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	try
	{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.cbReason=m_cbConcludeReason;

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray)); 

			//切换用户
			WORD wBankerUser=m_wBankerUser;
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;

			//算分判断
			if (m_GameLogic.GetStepCount(CHESS_WHITE)>=5)
			{
				//变量定义
				LONGLONG lGameScore[GAME_PLAYER];
				ZeroMemory(&lGameScore,sizeof(lGameScore));

				//成绩计算
				switch (m_pGameServiceOption->wServerType)
				{
				case GAME_GENRE_GOLD:		//金币模式
				case GAME_GENRE_MATCH:		//比赛模式
					{
						//金币计算
						if (wChairID!=INVALID_CHAIR)
						{
							lGameScore[wChairID]=1L;
							lGameScore[(wChairID+1)%GAME_PLAYER]=-1L;
						}

						break;
					}
				default:					//积分模式
					{
						//积分计算
						CalculateGameScore(wChairID,lGameScore);

						break;
					}
				}

				//统计积分
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					LONGLONG lScore=(LONGLONG)lGameScore[i]*m_pGameServiceOption->lCellScore;
					BYTE nScoreKind;
					if(wChairID!=INVALID_CHAIR)
					{
						if (lScore>0L) nScoreKind = SCORE_TYPE_WIN;
						else if(lScore<0L) nScoreKind = SCORE_TYPE_LOSE;
						else nScoreKind= SCORE_TYPE_DRAW;
					}

					//处理税收
					LONGLONG lRevenue=m_pITableFrame->CalculateRevenue(i,lScore);
					lScore-=lRevenue;

					GameEnd.lGameScore[i]=lScore/*-m_pITableFrame->AccountRevenue(ScoreInfoArray[i].lScore)*/;

					ScoreInfoArray[i].lScore=lScore;
					//设置类型
					ScoreInfoArray[i].cbType=(ScoreInfoArray[i].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					//计算税收
					ScoreInfoArray[i].lRevenue=m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore-=ScoreInfoArray[i].lRevenue;

				}
				//写入积分
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			}
			else
			{
				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			//结束信息
			TCHAR szEndMessage[1024]=TEXT("");
			_sntprintf(szEndMessage,CountArray(szEndMessage),TEXT("本局结束，成绩统计：\n %s ： %I64d 分\n %s ： %I64d 分\n"),
				m_pITableFrame->GetTableUserItem(0)->GetNickName(),GameEnd.lGameScore[0],
				m_pITableFrame->GetTableUserItem(1)->GetNickName(),GameEnd.lGameScore[1]);

			//发送结束信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				m_pITableFrame->SendGameMessage(pIServerUserItem,szEndMessage,SMT_CHAT);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//请求设置
			if ((m_bGameRule==true)&&(m_wBankerUser!=INVALID_CHAIR))
			{
				//设置变量
				m_wRuleRoundID++;
				m_bGameRule=false;

				//变量定义
				CMD_S_RequestRule RequestRule;
				ZeroMemory(&RequestRule,sizeof(RequestRule));

				//构造变量
				RequestRule.wRuleRoundID=m_wRuleRoundID;
				RequestRule.GameRuleInfo=m_GameRuleInfo;

				//发送数据
				m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_REQUEST_RULE,&RequestRule,sizeof(RequestRule));
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//请求设置
			if ((m_bGameRule==true)&&(m_wBankerUser!=INVALID_CHAIR))
			{
				//设置变量
				m_wRuleRoundID++;
				m_bGameRule=false;

				//变量定义
				CMD_S_RequestRule RequestRule;
				ZeroMemory(&RequestRule,sizeof(RequestRule));

				//构造变量
				RequestRule.wRuleRoundID=m_wRuleRoundID;
				RequestRule.GameRuleInfo=m_GameRuleInfo;

				//发送数据
				m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_REQUEST_RULE,&RequestRule,sizeof(RequestRule));
			}

			return true;
		}
	case GER_USER_LEAVE:	  //用户强退
	case GER_NETWORK_ERROR:   //网络异常
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.cbReason=REASON_USER_EXIT;

			//切换用户
			m_wBankerUser=(wChairID+1)%GAME_PLAYER;

			//算分判断
			if (m_GameLogic.GetStepCount(CHESS_WHITE)>=5)
			{
				//变量定义
				LONGLONG lGameScore[GAME_PLAYER];
				ZeroMemory(&lGameScore,sizeof(lGameScore));

				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray)); 

				//成绩计算
				switch (m_pGameServiceOption->wServerType)
				{
				case GAME_GENRE_GOLD:		//金币模式
				case GAME_GENRE_MATCH:		//比赛模式
					{
						//金币计算
						lGameScore[wChairID]=-m_pGameServiceOption->lCellScore;

						break;
					}
				default:					//积分模式
					{
						//积分计算
						CalculateGameScore(m_wBankerUser,lGameScore);

						//积分计算
						//lGameScore[wChairID]=-5L;

						break;
					}
				}

				//统计积分
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					LONGLONG lScore=(LONGLONG)lGameScore[i]*m_pGameServiceOption->lCellScore;
					BYTE nScoreKind;
					if(i==wChairID) nScoreKind=SCORE_TYPE_FLEE;
					else nScoreKind=SCORE_TYPE_WIN;

					//处理税收
					LONGLONG lRevenue=m_pITableFrame->CalculateRevenue(i,lScore);
					lScore-=lRevenue;

					GameEnd.lGameScore[i]=lScore;


					ScoreInfoArray[i].lScore=lScore;
					//设置类型
					ScoreInfoArray[i].cbType=nScoreKind;
					//计算税收
					ScoreInfoArray[i].lRevenue=m_pITableFrame->CalculateRevenue(i,ScoreInfoArray[i].lScore);
					if (ScoreInfoArray[i].lRevenue>0L) ScoreInfoArray[i].lScore-=ScoreInfoArray[i].lRevenue;

				}

				//写入积分
				m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			}
			else
			{
				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//请求设置
			if ((m_bGameRule==true)&&(m_wBankerUser!=INVALID_CHAIR))
			{
				//设置变量
				m_wRuleRoundID++;
				m_bGameRule=false;

				//变量定义
				CMD_S_RequestRule RequestRule;
				ZeroMemory(&RequestRule,sizeof(RequestRule));

				//构造变量
				RequestRule.wRuleRoundID=m_wRuleRoundID;
				RequestRule.GameRuleInfo=m_GameRuleInfo;

				//发送数据
				m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_REQUEST_RULE,&RequestRule,sizeof(RequestRule));
			}

			return true;
		}
	}
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_结束游戏"));
	}

	//错误断言
	ASSERT(FALSE);

	TraceMessage(TEXT("113444444_false结束游戏"));
	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	try
	{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			try
			{
				//设置变量
				StatusFree.wBankerUser=m_wBankerUser;
				StatusFree.wRuleRoundID=m_wRuleRoundID;
				StatusFree.cbGameRule=(m_bGameRule==true)?TRUE:FALSE;
				CopyMemory(&StatusFree.GameRuleInfo,&m_GameRuleInfo,sizeof(StatusFree.GameRuleInfo));
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_发送设置变量场景设置变量空闲状态"));
				return false;
			}

			try
			{
				//发送场景
				return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_发送场景空闲状态"));
				return false;
			}
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//获取棋谱
			INT_PTR nManualCount=0;
			tagChessManual * pChessManual=NULL;

			try
			{
				nManualCount=m_GameLogic.m_ChessManualArray.GetCount();
				pChessManual=m_GameLogic.m_ChessManualArray.GetData();
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_游戏状态"));
				return false;
			}

			try
			{
				//发送棋谱
				for (WORD i=0;i<nManualCount;i+=(4080/sizeof(tagChessManual)))
				{
					WORD wSendCount=(WORD)__min(nManualCount-i,4080/sizeof(tagChessManual));
					m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_CHESS_MANUAL,pChessManual+i,wSendCount*sizeof(tagChessManual));
				}
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_游戏状态发送棋谱"));
				return false;
			}

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			try
			{
				//游戏信息
				StatusPlay.wBankerUser=m_wBankerUser;
				StatusPlay.wCurrentUser=m_wCurrentUser;
				StatusPlay.GameRuleInfo=m_GameRuleInfo;
				CopyMemory(StatusPlay.wLeaveDrawTime,m_wLeaveDrawTime,sizeof(m_wLeaveDrawTime));
				CopyMemory(StatusPlay.cbRequestStatus,m_cbRequestStatus,sizeof(m_cbRequestStatus));
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_游戏信游戏信息23"));
				return false;
			}

			try
			{
				//发送场景
				return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			}
			catch(...)
			{
				TraceMessage(TEXT("113444444_游戏信息34523"));
				return false;
			}
		}
	}
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_发送场景"));
		return false;
	}

	//错误断言
	ASSERT(FALSE);

	TraceMessage(TEXT("113444444_false发送场景"));
	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	TraceMessage(TEXT("113444444_false定时器事件"));
	return false;
}

//游戏消息
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID,VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	try
	{
		switch (wSubCmdID)
		{
		case SUB_C_MOVE_CHESS:			//移动棋子
			{
				//效验数据
				ASSERT(wDataSize==sizeof(CMD_C_MoveChess));
				if (wDataSize!=sizeof(CMD_C_MoveChess)) 
				{
					TraceMessage(TEXT("113444444_移动棋子"));
					return false;
				}

				try
				{
					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//变量定义
					CMD_C_MoveChess * pMoveChess=(CMD_C_MoveChess *)pData;

					//消息处理
					WORD wChairID=pIServerUserItem->GetChairID();

					return OnUserMoveChess(wChairID,pMoveChess->cbXSourcePos,pMoveChess->cbYSourcePos,
						pMoveChess->cbXTargetPos,pMoveChess->cbYTargetPos,pMoveChess->wUsedTimeCount);
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserMoveChess"));
					return false;
				}
			}
		case SUB_C_PEACE_REQUEST:		//和棋请求
			{
				try
				{
					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//消息处理
					return OnUserPeaceRequest(pIServerUserItem->GetChairID());
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserPeaceRequest"));
					return false;
				}
			}
		case SUB_C_PEACE_RESPOND:		//和棋回应
			{
				try
				{
					//效验数据
					ASSERT(wDataSize==sizeof(CMD_C_PeaceRespond));
					if (wDataSize!=sizeof(CMD_C_PeaceRespond)) 
					{
						TraceMessage(TEXT("113444444_和棋回应"));
						return false;
					}

					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//变量定义
					CMD_C_PeaceRespond * pPeaceRespond=(CMD_C_PeaceRespond *)pData;

					//消息处理
					WORD wChairID=pIServerUserItem->GetChairID();
					return OnUserPeaceRespond(wChairID,pPeaceRespond->cbApprove);
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserPeaceRespond"));
					return false;
				}
			}
		case SUB_C_REGRET_REQUEST:		//悔棋请求
			{
				try
				{
					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//消息处理
					return OnUserRegretRequest(pIServerUserItem->GetChairID());
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserRegretRequest"));
					return false;
				}
			}
		case SUB_C_REGRET_RESPOND:		//悔棋回应
			{
				try
				{
					//效验数据
					ASSERT(wDataSize==sizeof(CMD_C_RegretRespond));
					if (wDataSize!=sizeof(CMD_C_RegretRespond)) 
					{
						TraceMessage(TEXT("113444444_悔棋回应"));
						return false;
					}

					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//变量定义
					CMD_C_RegretRespond * pRegretRespond=(CMD_C_RegretRespond *)pData;

					//消息处理
					WORD wChairID=pIServerUserItem->GetChairID();
					return OnUserRegretRespond(wChairID,pRegretRespond->cbApprove);
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserRegretRespond"));
					return false;
				}
			}
		case SUB_C_CONCLUDE_REQUEST:	//结束请求
			{
				try
				{
					//效验数据
					ASSERT(wDataSize==sizeof(CMD_C_ConcludeRequest));
					if (wDataSize!=sizeof(CMD_C_ConcludeRequest)) 
					{
						TraceMessage(TEXT("113444444_结束请求"));
						return false;
					}

					//状态效验
					ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
					if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

					//用户效验
					ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
					if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

					//变量定义
					CMD_C_ConcludeRequest * pConcludeRequest=(CMD_C_ConcludeRequest *)pData;

					//消息处理
					WORD wChairID=pIServerUserItem->GetChairID();
					return OnUserConcludeRequest(wChairID,pConcludeRequest->cbReason);
				}
				catch(...)
				{
					TraceMessage(TEXT("113444444_消息处理OnUserConcludeRequest"));
					return false;
				}
			}
		}
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_游戏消息"));
	}

	TraceMessage(TEXT("113444444_false游戏消息"));
	return false;
}

//框架消息
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID,VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	/*WORD wMsg[]={SUB_GF_INFO,SUB_GF_USER_READY,SUB_GF_USER_CHAT,SUB_GF_LOOKON_CONTROL,SUB_GF_KICK_TABLE_USER};
	for(WORD i=0;i<CountArray(wMsg);i++)
	{
		if(wSubCmdID==wMsg[i])return false;
	}

	TCHAR tchInfo[512]=TEXT("");
	_sntprintf(tchInfo,sizeof(tchInfo),TEXT("wSubCmdID=%d"),wSubCmdID);
	TraceMessage(tchInfo);*/
	return false;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}


//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR)) m_wBankerUser=pIServerUserItem->GetChairID();

	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	//庄家设置
	if ((bLookonUser==false)&&(pIServerUserItem->GetChairID()==m_wBankerUser))
	{
		//设置庄家
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if ((m_wBankerUser!=i)&&(m_pITableFrame->GetTableUserItem(i)!=NULL))
			{
				//设置变量
				m_wBankerUser=i;
				m_wRuleRoundID++;
				m_bGameRule=false;

				//变量定义
				CMD_S_RequestRule RequestRule;
				ZeroMemory(&RequestRule,sizeof(RequestRule));

				//构造变量
				RequestRule.wRuleRoundID=m_wRuleRoundID;
				RequestRule.GameRuleInfo=m_GameRuleInfo;

				//发送数据
				m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_REQUEST_RULE,&RequestRule,sizeof(RequestRule));

				break;
			}
		}

		//庄家设置
		if (i==GAME_PLAYER) 
		{
			//设置变量
			m_wRuleRoundID++;
			m_bGameRule=false;
			m_wBankerUser=INVALID_CHAIR;

			//规则变量
			m_GameRuleInfo.cbDirections=TRUE;
			m_GameRuleInfo.cbRegretFlag=TRUE;
			m_GameRuleInfo.cbCurrentMode=MODE_SLOW;
			m_GameRuleInfo.wRuleDrawTime=MODE_SLOW_DRAW_TIME;
			m_GameRuleInfo.wRuleStepTime=MODE_SLOW_STEP_TIME;
			m_GameRuleInfo.wRuleSecondTime=MODE_SLOW_SECOND_TIME;
		}
	}

	return true;
}

//用户同意
bool  CTableFrameSink::OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//庄家用户
	if (pIServerUserItem->GetChairID()==m_wBankerUser)
	{
		//效验大小
		ASSERT(wDataSize==sizeof(CMD_C_CustomizeRult));
		if (wDataSize!=sizeof(CMD_C_CustomizeRult)) 
		{
			TraceMessage(TEXT("113444444_庄家用户"));
			return false;
		}

		//效验状态
		ASSERT(m_bGameRule==false);
		if (m_bGameRule!=false) 
		{
			TraceMessage(TEXT("113444444_m_bGameRule效验状态"));
			return false;
		}

		//消息处理
		CMD_C_CustomizeRult * pCustomizeRult=(CMD_C_CustomizeRult *)pData;

		//规则效验
		pCustomizeRult->GameRuleInfo.wRuleDrawTime=__min(pCustomizeRult->GameRuleInfo.wRuleDrawTime,MAX_DRAW_TIME);
		pCustomizeRult->GameRuleInfo.wRuleStepTime=__min(pCustomizeRult->GameRuleInfo.wRuleStepTime,MAX_STEP_TIME);
		pCustomizeRult->GameRuleInfo.wRuleSecondTime=__min(pCustomizeRult->GameRuleInfo.wRuleSecondTime,MAX_SECOND_TIME);

		//设置规则
		m_bGameRule=true;
		m_GameRuleInfo=pCustomizeRult->GameRuleInfo;

		//请求同意
		WORD wReqChairID=(m_wBankerUser+1)%GAME_PLAYER;
		if (m_pITableFrame->GetTableUserItem(wReqChairID)!=NULL)
		{
			//变量定义
			CMD_S_RequestAgree RequestAgree;
			ZeroMemory(&RequestAgree,sizeof(RequestAgree));

			//构造消息
			RequestAgree.wRuleRoundID=m_wRuleRoundID;
			RequestAgree.GameRuleInfo=m_GameRuleInfo;

			//发送消息
			m_pITableFrame->SendTableData(wReqChairID,SUB_S_REQUEST_AGREE,&RequestAgree,sizeof(RequestAgree));
		}
	}

	//白棋用户
	if ((pIServerUserItem->GetChairID()!=m_wBankerUser)&&(m_bGameRule==false))
	{
		TraceMessage(TEXT("113444444_白棋用户"));
		return false;
	}

	return true;
}

//移动棋子
bool CTableFrameSink::OnUserMoveChess(WORD wChairID, BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, WORD wUserTimeCount)
{
	//效验参数
	ASSERT((cbXSourcePos<COUNT_X)&&(cbYSourcePos<COUNT_Y));
	if ((cbXSourcePos>=COUNT_X)||(cbYSourcePos>=COUNT_Y))
	{
		TraceMessage(TEXT("113444444_cbXSourcePos>=COUNT_X"));
		return false;
	}

	//效验参数
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));
	if ((cbXTargetPos>=COUNT_X)||(cbYTargetPos>=COUNT_Y)) 
	{
		TraceMessage(TEXT("113444444_(cbXTargetPos>=COUNT_X)||(cbYTargetPos>=COUNT_Y"));
		return false;
	}

	//效验时间
	ASSERT(wUserTimeCount<=m_GameRuleInfo.wRuleDrawTime);
	if (wUserTimeCount>m_GameRuleInfo.wRuleDrawTime)
	{
		TraceMessage(TEXT("113444444_(wUserTimeCount>m_GameRuleInfo.wRuleDrawTime)"));
		return false;
	}

	//效验时间
	ASSERT(wUserTimeCount>=(m_GameRuleInfo.wRuleDrawTime-m_wLeaveDrawTime[wChairID]));
	if (wUserTimeCount<(m_GameRuleInfo.wRuleDrawTime-m_wLeaveDrawTime[wChairID])) 
	{
		TraceMessage(TEXT("113444444_(效验时间>m_GameRuleInfo.wRuleDrawTime)"));
		return false;
	}

	//效验用户
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) 
	{
		TraceMessage(TEXT("113444444_(m_wCurrentUsereDrawTime)"));
		return false;
	}

	//点棋判断
	BYTE cbUserColor;
	tagChessItem * pSourceChessItem=NULL;
	try
	{
		cbUserColor=(wChairID==m_wBankerUser)?CHESS_BLACK:CHESS_WHITE;
		pSourceChessItem=m_GameLogic.GetChessItem(cbXSourcePos,cbYSourcePos);
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_(m_GameLogic.GetChessItem)"));
		return false;
	}

	//效验点棋
	ASSERT((pSourceChessItem!=NULL)&&(pSourceChessItem->cbColor==cbUserColor));
	if ((pSourceChessItem==NULL)||(pSourceChessItem->cbColor!=cbUserColor)) 
	{
		if(pSourceChessItem==NULL)
		{
			TraceMessage(TEXT("113444444_(pSourceChessItem==NULL)"));
			return false;
		}
		TCHAR tchInfo[512]=TEXT("");
		_sntprintf(tchInfo,sizeof(tchInfo),TEXT("pSourceChessItem->cbColor=%d,cbUserColor=%d"),pSourceChessItem->cbColor,cbUserColor);
		TraceMessage(tchInfo);
		return false;
	}

	//移动棋子
	tagMoveChessResult MoveChessResult;
	bool bSucceed = false;
	try
	{
		bSucceed = m_GameLogic.IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos);
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_(效验pSourceChessItem,cbXTargetPos,cbYTargetPDrawTime)"));
	}
	if (bSucceed==false) return false;

	try
	{
		bSucceed = m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult,NULL);
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_(效验MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPDrawTime)"));
	}
	if (bSucceed==false) return false;

	//结束变量
	tagConcludeInfo ConcludeInfo;
	ZeroMemory(&ConcludeInfo,sizeof(ConcludeInfo));
	try
	{
		m_GameLogic.IsGameFinish((wChairID==m_wBankerUser)?CHESS_BLACK:CHESS_WHITE,ConcludeInfo);
	}
	catch(...)
	{
		TraceMessage(TEXT("113444444_(结束变量,IsGameFinish)"));
		return false;
	}

	//设置变量
	m_wLeaveDrawTime[wChairID]=m_GameRuleInfo.wRuleDrawTime-wUserTimeCount;
	m_wCurrentUser=(ConcludeInfo.bGameConclude==true)?INVALID_CHAIR:(m_wCurrentUser+1)%GAME_PLAYER;

	//变量定义
	CMD_S_MoveChess MoveChess;
	ZeroMemory(&MoveChess,sizeof(MoveChess));

	//用户信息
	MoveChess.wActionUser=wChairID;
	MoveChess.wCurrentUser=m_wCurrentUser;

	//走棋信息
	MoveChess.cbXSourcePos=cbXSourcePos;
	MoveChess.cbYSourcePos=cbYSourcePos;
	MoveChess.cbXTargetPos=cbXTargetPos;
	MoveChess.cbYTargetPos=cbYTargetPos;
	MoveChess.wLeaveDrawTime=m_wLeaveDrawTime[wChairID];

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&MoveChess,sizeof(MoveChess));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&MoveChess,sizeof(MoveChess));

	//结束游戏
	if (ConcludeInfo.bGameConclude==true) 
	{
		//变量定义
		WORD wWinnerUser=INVALID_CHAIR;
		BYTE cbWinnerColor=ConcludeInfo.cbWinnerColor;

		//胜利分析
		if (cbWinnerColor!=CHESS_NULL)
		{
			WORD wBlackUser=m_wBankerUser;
			WORD wWhiteUser=(m_wBankerUser+1)%GAME_PLAYER;
			wWinnerUser=(cbWinnerColor==CHESS_BLACK)?wBlackUser:wWhiteUser;
		}

		//结束游戏
		m_cbConcludeReason = ConcludeInfo.cbReason;
		OnEventGameConclude(wWinnerUser,NULL,GER_NORMAL);
	}

	return true;
}

//和棋请求
bool CTableFrameSink::OnUserPeaceRequest(WORD wChairID)
{
	//用户状态
	ASSERT((m_cbRequestStatus[wChairID]&GU_WAIT_PEACE)==0);
	if ((m_cbRequestStatus[wChairID]&GU_WAIT_PEACE)!=0) 
	{
		TraceMessage(TEXT("113444444_(效验MoveChess(用户状态,cbYSourcePos,cbXTargetPDrawTime)"));
		return false;
	}

	//次数判断
	if (m_cbPeaceCount[wChairID]>=3)
	{
		//构造消息
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountChar(szMessage),TEXT("您已经 [ 求和 ] %d 次了，请求不能处理。"),m_cbPeaceCount[wChairID]);

		//发送消息
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

		return true;
	}

	//设置变量
	m_cbPeaceCount[wChairID]++;
	m_cbRequestStatus[wChairID]|=GU_WAIT_PEACE;

	//发送消息
	m_pITableFrame->SendTableData((wChairID+1)%GAME_PLAYER,SUB_S_PEACE_REQUEST);

	return true;
}

//和棋回应
bool CTableFrameSink::OnUserPeaceRespond(WORD wChairID, BYTE cbApprove)
{
	//用户状态
	WORD wRequestUser=(wChairID+1)%GAME_PLAYER;
	if ((m_cbRequestStatus[wRequestUser]&GU_WAIT_PEACE)==0) return true;

	//设置变量
	m_cbRequestStatus[wRequestUser]&=~GU_WAIT_PEACE;

	//结果处理
	if (cbApprove==TRUE) 
	{
		m_cbConcludeReason = REASON_AGREE_DRAW;
		OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}
	else m_pITableFrame->SendTableData(wRequestUser,SUB_S_PEACE_RESPOND);

	return true;
}

//悔棋请求
bool CTableFrameSink::OnUserRegretRequest(WORD wChairID)
{
	//状态判断
	if ((m_cbRequestStatus[wChairID]&GU_WAIT_REGRET)!=0) 
	{
		TraceMessage(TEXT("113444444_(效验悔棋请求(用户状态,cbYSourcePos,cbXTargetPDrawTime)"));
		return false;
	}
	if (m_GameLogic.GetStepCount((wChairID==m_wBankerUser)?CHESS_BLACK:CHESS_WHITE)==0) 
	{
		TraceMessage(TEXT("113444444_(wChairID==m_wBankerUser)?CHESS(用户状态,cbYSourcePos,cbXTargetPDrawTime)"));
		return false;
	}

	//规则限制
	if (m_GameRuleInfo.cbRegretFlag==FALSE)
	{
		//发送消息
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_RULE_LIMIT;
		m_pITableFrame->SendTableData(wChairID,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));

		//系统消息
		LPCTSTR pszMessage=TEXT("根据规则设置，此盘游戏不允许“悔棋”。");
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

		return true;
	}

	//判断次数
	if (m_cbRegretCount[wChairID]==5)
	{
		//发送消息
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_COUNT_LIMIT;
		m_pITableFrame->SendTableData(wChairID,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));

		//构造消息
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountChar(szMessage),TEXT("您已经 [ 悔棋 ] %d 次了，不能再次悔棋。"),m_cbRegretCount[wChairID]);

		//发送消息
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

		return true;
	}

	//设置变量
	m_cbRegretCount[wChairID]++;
	m_cbRequestStatus[wChairID]|=GU_WAIT_REGRET;

	//发送消息
	m_pITableFrame->SendTableData((wChairID+1)%GAME_PLAYER,SUB_S_REGRET_REQUEST);

	return true;
}

//悔棋回应
bool CTableFrameSink::OnUserRegretRespond(WORD wChairID, BYTE cbApprove)
{
	//状态判断
	WORD wRequestUser=(wChairID+1)%GAME_PLAYER;
	if ((m_cbRequestStatus[wRequestUser]&GU_WAIT_REGRET)==0) 
	{
		TraceMessage(TEXT("113444444_(wChairID==m_wBankerUser)?CHESS(用户状态,悔棋回应,cbXTargetPDrawTime)"));
		return false;
	}

	//设置变量
	m_cbRequestStatus[wRequestUser]&=~GU_WAIT_REGRET;

	//拒绝处理
	if (cbApprove==FALSE)
	{
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_PLAYER_OPPOSE;
		m_pITableFrame->SendTableData(wRequestUser,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));
		return true;
	}

	//构造数据
	CMD_S_RegretResult RegretResult;
	ZeroMemory(&RegretResult,sizeof(RegretResult));

	//计算次数
	WORD wRegretCount=(wRequestUser==m_wCurrentUser)?2:1;
	WORD wCurrentStep=(WORD)m_GameLogic.m_ChessManualArray.GetCount();
	RegretResult.wTargetStep=(wCurrentStep>wRegretCount)?(wCurrentStep-wRegretCount):0;

	//悔棋操作
	m_GameLogic.RegretChess(RegretResult.wTargetStep);

	//设置用户
	m_wCurrentUser=((RegretResult.wTargetStep%2)==0)?m_wBankerUser:(m_wBankerUser+1)%GAME_PLAYER;

	//构造数据
	RegretResult.wRegretUser=wRequestUser;
	RegretResult.wCurrentUser=m_wCurrentUser;
	CopyMemory(RegretResult.wLeaveDrawTime,m_wLeaveDrawTime,sizeof(RegretResult.wLeaveDrawTime));

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_REGRET_RESULT,&RegretResult,sizeof(RegretResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_REGRET_RESULT,&RegretResult,sizeof(RegretResult));

	return true;
}

//认输请求
bool CTableFrameSink::OnUserConcludeRequest(WORD wChairID, BYTE cbReason)
{
	//设置变量
	m_cbConcludeReason=cbReason;

	//结束游戏
	OnEventGameConclude((wChairID+1)%GAME_PLAYER,NULL,GER_NORMAL);

	return true;
}

//积分计算
bool CTableFrameSink::CalculateGameScore(WORD wWinnerUser, LONGLONG lGameScore[GAME_PLAYER])
{
	//变量定义
	WORD wUserLevel[GAME_PLAYER];
	ZeroMemory(&wUserLevel,sizeof(wUserLevel));

	//获取属性
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		LONGLONG lUserScore=0L;

		//获取属性
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL) lUserScore=pIServerUserItem->GetUserScore();

		//等级计算
		if (lUserScore>=0L)
		{
			//变量定义
			LONGLONG lLevelScore=0L;
			LONGLONG lScoreIncrease=20L;

			//等级计算
			for (WORD wLevel=1;wLevel<20;wLevel++)
			{
				lScoreIncrease+=wLevel*10;
				lLevelScore+=lScoreIncrease;
				if (lUserScore<lLevelScore) break;
			}

			//设置等级
			wUserLevel[i]=wLevel;
		}
	}

	//积分统计
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		//变量定义
		WORD wOtherChairID=(i+1)%GAME_PLAYER;

		//胜局计算
		if (i==wWinnerUser)
		{
			if (wUserLevel[i]<=(wUserLevel[wOtherChairID]+4))
			{
				lGameScore[i]=(wUserLevel[i]>=(wUserLevel[wOtherChairID]+2))?2:5;
			}
			continue;
		}

		//输局计算
		if (wWinnerUser==wOtherChairID)
		{
			lGameScore[i]=(wUserLevel[i]>=(wUserLevel[wOtherChairID]+4))?-3:-2;
			continue;
		}

		//平局计算
		if (wWinnerUser==INVALID_CHAIR)
		{
			if (wUserLevel[i]<=(wUserLevel[wOtherChairID]+2)) lGameScore[i]=1;
			continue;
		}
	}

	//十内无效
	if (m_GameLogic.GetStepCount(CHESS_WHITE)<10)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if(lGameScore[i]>0)lGameScore[i]=0;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
