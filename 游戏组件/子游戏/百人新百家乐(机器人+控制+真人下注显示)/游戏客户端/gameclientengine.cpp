#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FREE					99									//空闲时间
#define IDI_PLACE_JETTON			100									//下注时间
#define IDI_DISPATCH_CARD			301									//发牌时间
#define IDI_PLACE_JETTON_BUFFER		302									//发牌时间

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_PALY_BET,OnPlayBet)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_MESSAGE(IDM_ADMIN_COMMDN,OnAdminCommand)
	ON_MESSAGE(IDM_UPDATE_STORAGE,OnUpdateStorage)
	ON_MESSAGE(IDM_PLAY_SOUND,OnPlaySound)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine() : CGameFrameEngine()
{
	//个人信息
	m_lPlayBetScore = 0L;			
	m_lPlayFreeSocre = 0L;		

	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));

	//庄家信息
	m_wBankerUser = INVALID_CHAIR;
	m_lBankerScore = 0l;
	m_lBankerWinScore = 0l;
	m_wBankerTime = 0;

	//系统坐庄
	m_bEnableSysBanker = false;

	//状态变量
	m_bMeApplyBanker = false;
	m_bBackGroundSound = false;

	//限制信息
	m_lAreaLimitScore = 0L;	
	m_lApplyBankerCondition = 0l;

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//设置标题
	SetWindowText(TEXT("百家乐游戏  --  Ver：6.6.1.0"));

	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	if (m_pIClientKernel!=NULL) m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//设置图标
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载声音
	//m_DTSDBackground.InitDirectSound(this);
	//m_dwBackID = m_DTSDBackground.Create(TEXT("BACK_GROUND"));

	m_PlaceBetArray.RemoveAll();


	//播放声音
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if ( pGlobalUnits->m_bAllowBackGroundSound )
	{
		m_bBackGroundSound = true;
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
	}

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//个人信息
	m_lPlayBetScore = 0L;			
	m_lPlayFreeSocre = 0L;		

	//下注数
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));

	//庄家信息
	m_wBankerUser = INVALID_CHAIR;
	m_lBankerScore = 0l;
	m_lBankerWinScore = 0l;
	m_wBankerTime = 0;

	//系统坐庄
	m_bEnableSysBanker = false;

	//状态变量
	m_bMeApplyBanker = false;

	//限制信息
	m_lAreaLimitScore = 0L;	
	m_lApplyBankerCondition = 0l;

	return true;
}
//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	if ((wClockID == IDI_PLACE_JETTON)&&(nElapse == 0))
	{
		//设置光标
		m_GameClientView.SetCurrentBet(0L);

		//禁止按钮
		m_GameClientView.m_btBet100.EnableWindow(FALSE);		
		m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
		m_GameClientView.m_btBet100000.EnableWindow(FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow(FALSE);

		//庄家按钮
		m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
		m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );

		return false;
	}

	if ( CGlobalUnits::GetInstance()->m_bAllowSound && wClockID == IDI_PLACE_JETTON && nElapse<=5 ) 
	{
		//PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
	}

	if((wClockID == IDI_DISPATCH_CARD) && (nElapse==0) ) 
	{
		return false;
	}

	return true;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:		//游戏空闲
		{
			return OnSubGameFree(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_PLACE_JETTON:	//用户加注
		{
			return OnSubPlaceBet(pData,wDataSize);
		}
	case SUB_S_APPLY_BANKER:	//申请做庄
		{
			return OnSubUserApplyBanker(pData, wDataSize);
		}
	case SUB_S_CANCEL_BANKER:	//取消做庄
		{
			return OnSubUserCancelBanker(pData, wDataSize);
		}
	case SUB_S_CHANGE_BANKER:	//切换庄家
		{
			return OnSubChangeBanker(pData, wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			while( m_PlaceBetArray.GetCount() > 0 )
			{
				CMD_S_PlaceBet &PlaceBet = m_PlaceBetArray[0];
				PlaceBet.cbAndroidUser = FALSE;
				OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet));
				m_PlaceBetArray.RemoveAt(0);
			}
			KillTimer(IDI_PLACE_JETTON_BUFFER);
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_SEND_RECORD:		//游戏记录
		{
			return OnSubGameRecord(pData,wDataSize);
		}
	case SUB_S_PLACE_JETTON_FAIL:	//下注失败
		{
			return OnSubPlaceBetFail(pData,wDataSize);
		}
	case SUB_S_AMDIN_COMMAND:
		{
			return OnSubReqResult(pData,wDataSize);
		}
	case SUB_S_UPDATE_STORAGE:	//更新库存
		{
			return OnSubUpdateStorage(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置状态
			SetGameStatus(GAME_SCENE_FREE);

			//设置时间
			SetGameClock(GetMeChairID(), IDI_FREE, pStatusFree->cbTimeLeave);

            //个人信息
			m_lPlayFreeSocre = pStatusFree->lPlayFreeSocre;
		
			//庄家信息
			m_wBankerUser = pStatusFree->wBankerUser;
			m_lBankerScore = pStatusFree->lBankerScore;
			m_lBankerWinScore = pStatusFree->lBankerWinScore;
			m_wBankerTime = pStatusFree->wBankerTime;

			// 系统坐庄
			m_bEnableSysBanker = pStatusFree->bEnableSysBanker;

			// 限制变量
			m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			m_lApplyBankerCondition = pStatusFree->lApplyBankerCondition;

			// 设置界面
			m_GameClientView.SetGameStatus(GAME_SCENE_FREE);
			m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));
			m_GameClientView.SetPlayBetScore(0);
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);
			m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);
			m_GameClientView.SetEnableSysBanker(m_bEnableSysBanker);

			// 控制按钮
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hControlInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
				for (int i=0;i<AREA_MAX;i++)
				{
					m_GameClientView.m_SkinListCtrl[i].ShowWindow(SW_SHOW);
				}
			}


			// 设置界面
			m_GameClientView.FlexAnimation(enFlexNULL,false);

			//更新控制
			UpdateButtonContron();

			return true;
		}
	case GAME_SCENE_PLAY:		//游戏状态
	case GAME_SCENE_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置状态
			SetGameStatus(pStatusPlay->cbGameStatus);

			//设置时间
			if( pStatusPlay->cbGameStatus == GAME_SCENE_END )
				SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD,pStatusPlay->cbTimeLeave);
			else
				SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

			//下注信息
			memcpy(m_lAllBet, pStatusPlay->lAllBet, sizeof(m_lAllBet));
			memcpy(m_lPlayBet, pStatusPlay->lPlayBet, sizeof(m_lPlayBet));
			memcpy(m_lPlayScore, pStatusPlay->lPlayScore, sizeof(m_lPlayScore));
			
			//个人信息
			m_lPlayBetScore = pStatusPlay->lPlayBetScore;
			m_lPlayFreeSocre = pStatusPlay->lPlayFreeSocre;

			//庄家信息
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_lBankerScore = pStatusPlay->lBankerScore;
			m_lBankerWinScore = pStatusPlay->lBankerWinScore;
			m_wBankerTime = pStatusPlay->wBankerTime;

			// 系统坐庄
			m_bEnableSysBanker = pStatusPlay->bEnableSysBanker;

			// 限制变量
			m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			m_lApplyBankerCondition = pStatusPlay->lApplyBankerCondition;

			// 设置界面
			m_GameClientView.SetGameStatus(pStatusPlay->cbGameStatus);
			m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));
			m_GameClientView.SetPlayBetScore(m_lPlayBetScore);
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);
			m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);
			m_GameClientView.SetEnableSysBanker(m_bEnableSysBanker);
			m_GameClientView.SetCardInfo(pStatusPlay->cbCardCount, pStatusPlay->cbTableCardArray);

			// 设置下注界面
			for( int i = 0 ; i < AREA_MAX; ++i )
			{
				m_GameClientView.SetPlayBet(i, m_lPlayBet[i]);
				m_GameClientView.SetAllBet(i, m_lAllBet[i]);
				m_GameClientView.AddChip(i, m_lAllBet[i]);
			}

			// 设置结束信息
			if ( pStatusPlay->cbGameStatus == GAME_SCENE_END && !IsLookonMode() )
			{
				//成绩信息
				m_GameClientView.SetCurGameScore(pStatusPlay->lPlayScore,pStatusPlay->lPlayAllScore);
			}

			// 设置界面
			if ( pStatusPlay->cbGameStatus == GAME_SCENE_END )
			{
				//设置扑克
				m_GameClientView.m_CardControl[INDEX_PLAYER].SetCardData(pStatusPlay->cbTableCardArray[INDEX_PLAYER], pStatusPlay->cbCardCount[INDEX_PLAYER]);
				m_GameClientView.m_CardControl[INDEX_BANKER].SetCardData(pStatusPlay->cbTableCardArray[INDEX_BANKER], pStatusPlay->cbCardCount[INDEX_BANKER]);

				// 设置界面
				m_GameClientView.FlexAnimation(enFlexDealCrad, true, false);

				//闪动
				m_GameClientView.FlashAnimation(true);
			}
			else
			{
				// 设置界面
				m_GameClientView.FlexAnimation(enFlexBetTip, true, false);
			}
	
			//控制按钮
			if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hControlInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
				for (int i=0;i<AREA_MAX;i++)
				{
					m_GameClientView.m_SkinListCtrl[i].ShowWindow(SW_SHOW);
				}
			}

			//更新按钮
			UpdateButtonContron();

			//设置按钮
			m_GameClientView.SetCurrentBet(0L);

			//禁止按钮
			m_GameClientView.m_btBet100.EnableWindow(FALSE);		
			m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
			m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
			m_GameClientView.m_btBet100000.EnableWindow(FALSE);
			m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
			m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
			m_GameClientView.m_btBet10000000.EnableWindow(FALSE);

			return true;
		}
	}

	return false;
}

//声音控制
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));	

	return true;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//玩家信息
	m_lPlayBetScore = pGameStart->lPlayBetScore;
	m_lPlayFreeSocre = pGameStart->lPlayFreeSocre;

	//庄家信息
	m_wBankerUser = pGameStart->wBankerUser;
	m_lBankerScore = pGameStart->lBankerScore;

	//设置界面
	m_GameClientView.SetPlayBetScore(m_lPlayBetScore);
	m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore );

	//设置时间
	WORD wMeChair = GetMeChairID();
	SetGameClock(wMeChair,IDI_PLACE_JETTON,pGameStart->cbTimeLeave);

	//设置状态
	SetGameStatus(GAME_SCENE_BET);
	m_GameClientView.SetGameStatus(GAME_SCENE_BET);

	//更新控制
	UpdateButtonContron();

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	m_PlaceBetArray.RemoveAll();

	return true;
}

//游戏空闲
bool CGameClientEngine::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//消息处理
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	//

	//设置时间
	SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

	//设置状态
	SetGameStatus(GAME_SCENE_FREE);
	m_GameClientView.SetGameStatus(GAME_SCENE_FREE);

	//清空下注
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));

	//完成发牌
	m_GameClientView.FinishDispatchCard();

	//清理桌面
	m_GameClientView.CleanUserBet();

	//闪动
	m_GameClientView.FlashAnimation(false);

	//更新控件
	UpdateButtonContron();

	return true;
}

//用户加注
bool CGameClientEngine::OnSubPlaceBet(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceBet));
	if (wDataSize!=sizeof(CMD_S_PlaceBet)) return false;

	//消息处理
	CMD_S_PlaceBet * pPlaceBet=(CMD_S_PlaceBet *)pBuffer;	

	if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&pPlaceBet->cbAndroidUserT&& m_GameClientView.m_hControlInst)
	{
		return true;
	}

	//缓冲判断
	if (pPlaceBet->cbAndroidUser)
	{
		m_PlaceBetArray.Add(*pPlaceBet);
		if (m_PlaceBetArray.GetCount()==1) SetTimer(IDI_PLACE_JETTON_BUFFER,70,NULL);
		return true;
	}

	if (!pPlaceBet->cbAndroidUserT && CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hControlInst )
	{
		m_GameClientView.ControlBetDetection(pPlaceBet->cbBetArea,pPlaceBet->lBetScore,SwitchViewChairID(pPlaceBet->wChairID));
	}

	if (GetMeChairID() != pPlaceBet->wChairID || IsLookonMode())
	{
		//加注界面
		m_lAllBet[pPlaceBet->cbBetArea] += pPlaceBet->lBetScore;

		m_GameClientView.SetAllBet( pPlaceBet->cbBetArea, m_lAllBet[pPlaceBet->cbBetArea]);
		m_GameClientView.AddChip( pPlaceBet->cbBetArea,pPlaceBet->lBetScore );

		//播放声音
		if (CGlobalUnits::GetInstance()->m_bAllowSound) 
		{
			if (pPlaceBet->wChairID!=GetMeChairID() || IsLookonMode())
			{
				if (pPlaceBet->lBetScore == 10000000) 
					PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
				else 
					PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
			}
		}
	}
    
	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置时间
	SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

	//扑克信息
	m_GameClientView.SetCardInfo(pGameEnd->cbCardCount,pGameEnd->cbTableCardArray);

	//庄家信息
	m_GameClientView.SetBankerOverInfo( pGameEnd->lBankerTotallScore, pGameEnd->nBankerTime);

	//成绩信息
	m_GameClientView.SetCurGameScore(pGameEnd->lPlayScore,pGameEnd->lPlayAllScore);

	//设置状态
	SetGameStatus(GAME_SCENE_END);
	m_GameClientView.SetGameStatus(GAME_SCENE_END);

	//更新控件
	UpdateButtonContron();

	return true;
}

//更新控制
void CGameClientEngine::UpdateButtonContron()
{
	//置能判断
	bool bEnablePlaceBet = true;
	if ( (m_bEnableSysBanker == false && m_wBankerUser == INVALID_CHAIR )
		|| (GetGameStatus() != GAME_SCENE_BET)
		|| (m_wBankerUser == GetMeChairID())
		|| (IsLookonMode()) )
	{
		bEnablePlaceBet = false;
	}

	//下注按钮
	if ( bEnablePlaceBet )
	{
		//计算积分
		LONGLONG lCurrentBet = m_GameClientView.GetCurrentBet();

		LONGLONG lLeaveScore = 0;
		for ( int i = 0; i < AREA_MAX; ++i )
		{
			lLeaveScore = max( lLeaveScore, m_GameClientView.GetMaxPlayerScore(i) );
		}

		//设置光标
		if (lCurrentBet>lLeaveScore)
		{
			if (lLeaveScore>=10000000L) m_GameClientView.SetCurrentBet(10000000L);
			else if (lLeaveScore>=5000000L) m_GameClientView.SetCurrentBet(5000000L);
			else if (lLeaveScore>=1000000L) m_GameClientView.SetCurrentBet(1000000L);
			else if (lLeaveScore>=100000L) m_GameClientView.SetCurrentBet(100000L);
			else if (lLeaveScore>=10000L) m_GameClientView.SetCurrentBet(10000L);
			else if (lLeaveScore>=1000L) m_GameClientView.SetCurrentBet(1000L);
			else if (lLeaveScore>=100L) m_GameClientView.SetCurrentBet(100L);
			else m_GameClientView.SetCurrentBet(0L);
		}

		//控制按钮
		m_GameClientView.m_btBet100.EnableWindow((lLeaveScore>=100)?TRUE:FALSE);
		m_GameClientView.m_btBet1000.EnableWindow((lLeaveScore>=1000)?TRUE:FALSE);
		m_GameClientView.m_btBet10000.EnableWindow((lLeaveScore>=10000)?TRUE:FALSE);
		m_GameClientView.m_btBet100000.EnableWindow((lLeaveScore>=100000)?TRUE:FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow((lLeaveScore>=1000000)?TRUE:FALSE);		
		m_GameClientView.m_btBet5000000.EnableWindow((lLeaveScore>=5000000)?TRUE:FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow((lLeaveScore>=10000000)?TRUE:FALSE);
	}
	else
	{
		//设置光标
		m_GameClientView.SetCurrentBet(0L);

		//禁止按钮
		m_GameClientView.m_btBet100.EnableWindow(FALSE);		
		m_GameClientView.m_btBet1000.EnableWindow(FALSE);		
		m_GameClientView.m_btBet10000.EnableWindow(FALSE);	
		m_GameClientView.m_btBet100000.EnableWindow(FALSE);
		m_GameClientView.m_btBet1000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet5000000.EnableWindow(FALSE);
		m_GameClientView.m_btBet10000000.EnableWindow(FALSE);
	}

	//庄家按钮
	if ( !IsLookonMode() )
	{
		//获取信息
		IClientUserItem* pMeUserItem = GetMeUserItem();

		//申请按钮
		bool bEnableApply = true;
		if (m_wBankerUser == GetMeChairID() || m_bMeApplyBanker || m_lPlayFreeSocre < m_lApplyBankerCondition || pMeUserItem->GetUserScore() < m_lApplyBankerCondition) 
			bEnableApply = false;

		m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);

		//取消按钮
		bool bEnableCancel = true;
		if ( m_wBankerUser == GetMeChairID() && GetGameStatus() != GAME_SCENE_FREE ) bEnableCancel=false;
		if ( m_bMeApplyBanker == false ) bEnableCancel=false;
		m_GameClientView.m_btCancelBanker.EnableWindow(bEnableCancel?TRUE:FALSE);
		m_GameClientView.m_btCancelBanker.SetButtonImage(m_wBankerUser==GetMeChairID()?IDB_BT_CANCEL_BANKER:IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

		//显示判断
		if (m_bMeApplyBanker)
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);
		}
		else
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
		m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
		m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
		m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
	}
	//银行按钮
	m_GameClientView.m_btBankerDraw.EnableWindow(TRUE);
	m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
	m_GameClientView.m_btBankerStorage.ShowWindow(SW_SHOW);
	m_GameClientView.m_btBankerStorage.EnableWindow((GetGameStatus()==GAME_STATUS_FREE || IsLookonMode())?TRUE:FALSE);
	if(m_wBankerUser==GetMeChairID()&&IsLookonMode()==false)
	{
		//m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
	}


	if(!IsLookonMode())
	{
		switch (GetGameStatus())
		{
		case GAME_SCENE_FREE:
			{
				m_GameClientView.SetInsureOption(false, true);
				break;
			}
		case GAME_SCENE_PLAY:
		case GAME_SCENE_END:
			{				
				m_GameClientView.SetInsureOption(false, false);
				break;
			}
		default:break;
		}
	}
	else
	{
		m_GameClientView.SetInsureOption(false, false);
	}

	m_GameClientView.InvalidGameView(0,0,0,0);

	return;
}

//加注消息
LRESULT CGameClientEngine::OnPlayBet(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbBetArea=(BYTE)wParam;
	LONGLONG lBetScore= m_GameClientView.GetCurrentBet();

	//庄家判断
	if ( GetMeChairID() == m_wBankerUser ) return true;

	//状态判断
	if (GetGameStatus() != GAME_SCENE_BET)
	{
		UpdateButtonContron();
		return true;
	}

	//设置变量
	m_lAllBet[cbBetArea] += lBetScore;
	m_lPlayBet[cbBetArea] += lBetScore;

	//设置界面
	m_GameClientView.SetPlayBet(cbBetArea, m_lPlayBet[cbBetArea]);
	m_GameClientView.SetAllBet(cbBetArea, m_lAllBet[cbBetArea]);
	m_GameClientView.AddChip(cbBetArea, lBetScore);

	//变量定义
	CMD_C_PlaceBet PlaceBet;
	ZeroMemory(&PlaceBet,sizeof(PlaceBet));

	//构造变量
	PlaceBet.cbBetArea = cbBetArea;
	PlaceBet.lBetScore = lBetScore;

	//发送消息
	SendSocketData(SUB_C_PLACE_JETTON,&PlaceBet,sizeof(PlaceBet));

	//更新按钮
	UpdateButtonContron();

	//播放声音
	if (CGlobalUnits::GetInstance()->m_bAllowSound) 
	{
		if (lBetScore == 10000000) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
		else 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
	}

	return 0;
}

//申请消息
LRESULT CGameClientEngine::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
	//合法判断
	IClientUserItem* pMeUserItem = GetMeUserItem();
	if (pMeUserItem->GetUserScore() < m_lApplyBankerCondition) return true;
	if (m_lPlayFreeSocre < m_lApplyBankerCondition) return true;

	//旁观判断
	if (IsLookonMode()) return true;

	//转换变量
	bool bApplyBanker = (wParam!=0) ? true:false;

	//当前判断
	if (m_wBankerUser == GetMeChairID() && bApplyBanker) return true;

	if (bApplyBanker)
	{
		//发送消息
		SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

		m_bMeApplyBanker=true;
	}
	else
	{
		//发送消息
		SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

		m_bMeApplyBanker=false;
	}

	//设置按钮
	UpdateButtonContron();

	return true;
}

//声音消息
LRESULT CGameClientEngine::OnPlaySound(WPARAM wParam, LPARAM lParam)
{
	if( !CGlobalUnits::GetInstance()->m_bAllowSound )
		return 0;

	PlayGameSound(AfxGetInstanceHandle(), (TCHAR*)wParam);
	return 0;
}

//申请做庄
bool CGameClientEngine::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//消息处理
	CMD_S_ApplyBanker * pApplyBanker = (CMD_S_ApplyBanker *)pBuffer;

	//获取玩家
	IClientUserItem* pClientUserItem = GetTableUserItem(pApplyBanker->wApplyUser);

	if ( pClientUserItem == NULL )
		return true;

	//插入玩家
	if (m_wBankerUser != pApplyBanker->wApplyUser)
	{
		m_GameClientView.m_ValleysList.Add( SwitchViewChairID(pApplyBanker->wApplyUser) );
	}

	//自己判断
	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) m_bMeApplyBanker=true;

	//更新控件
	UpdateButtonContron();

	return true;
}

//取消做庄
bool CGameClientEngine::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//消息处理
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

	//删除玩家
	for( int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i )
	{
		if ( SwitchViewChairID(pCancelBanker->wCancelUser) == m_GameClientView.m_ValleysList[i] )
		{
			m_GameClientView.m_ValleysList.RemoveAt(i);
			break;
		}
	}

	//自己判断
	if ( IsLookonMode() == false && pCancelBanker->wCancelUser == GetMeChairID() ) 
		m_bMeApplyBanker = false;

	//更新控件
	UpdateButtonContron();

	return true;
}

//切换庄家
bool CGameClientEngine::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//消息处理
	CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

	//显示图片
	m_GameClientView.ShowChangeBanker(m_wBankerUser!=pChangeBanker->wBankerUser);

	//自己判断
	if (m_wBankerUser==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	{
		m_bMeApplyBanker=false;
	}
	else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	{
		m_bMeApplyBanker=true;
	}

	//庄家信
	m_wBankerUser = pChangeBanker->wBankerUser;
	m_lBankerScore = pChangeBanker->lBankerScore;
	m_lBankerWinScore = 0;
	m_wBankerTime = 0;
	m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wBankerUser), m_lBankerScore, m_lBankerWinScore, m_wBankerTime);

	//删除玩家
	if (m_wBankerUser != INVALID_CHAIR)
	{
		for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
		{
			if( SwitchViewChairID(m_wBankerUser) == m_GameClientView.m_ValleysList[i] )
			{
				m_GameClientView.m_ValleysList.RemoveAt(i);
				break;
			}
		}
	}

	//更新界面
	UpdateButtonContron();

	return true;
}

//游戏记录
bool CGameClientEngine::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return false;

	//结果变量
	tagGameRecord GameRecord;
	ZeroMemory(&GameRecord,sizeof(GameRecord));

	//设置记录
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

		m_GameClientView.SetGameHistory(enOperateResult_NULL, pServerGameRecord->cbPlayerCount, pServerGameRecord->cbBankerCount,
			pServerGameRecord->cbKingWinner,pServerGameRecord->bPlayerTwoPair,pServerGameRecord->bBankerTwoPair);
	}

	return true;
}

//下注失败
bool CGameClientEngine::OnSubPlaceBetFail(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceBetFail));
	if (wDataSize!=sizeof(CMD_S_PlaceBetFail)) return false;

	//消息处理
	CMD_S_PlaceBetFail * pPlaceBetFail=(CMD_S_PlaceBetFail *)pBuffer;

	//效验参数
	BYTE cbViewIndex = pPlaceBetFail->lBetArea;
	ASSERT(cbViewIndex < AREA_MAX);
	if (cbViewIndex >= AREA_MAX) return false;

	//自己判断
	if ( GetMeChairID() == pPlaceBetFail->wPlaceUser && !IsLookonMode() )
	{
		//设置变量
		m_lPlayBet[cbViewIndex] -= pPlaceBetFail->lPlaceScore;
		m_lAllBet[cbViewIndex] -= pPlaceBetFail->lPlaceScore;
		
		//设置界面
		m_GameClientView.SetPlayBet(cbViewIndex, m_lPlayBet[cbViewIndex]);
		m_GameClientView.SetAllBet(cbViewIndex, m_lAllBet[cbViewIndex]);
		m_GameClientView.AddChip(pPlaceBetFail->lBetArea, -pPlaceBetFail->lPlaceScore);
	}

	m_GameClientView.InvalidGameView(0,0,0,0);

	return true;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CGameClientEngine::OnAdminCommand(WPARAM wParam,LPARAM lParam)
{
	SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
	return true;
}

//更新库存
LRESULT CGameClientEngine::OnUpdateStorage(WPARAM wParam,LPARAM lParam)
{
	SendSocketData(SUB_C_UPDATE_STORAGE,(CMD_C_UpdateStorage*)wParam,sizeof(CMD_C_UpdateStorage));
	return true;
}

bool CGameClientEngine::OnSubReqResult(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(CMD_S_CommandResult));
	if(wDataSize!=sizeof(CMD_S_CommandResult)) return false;

	m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
	return true;
}

//更新库存
bool CGameClientEngine::OnSubUpdateStorage(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_UpdateStorage));
	if(wDataSize!=sizeof(CMD_S_UpdateStorage)) return false;

	if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
	{
		m_GameClientView.m_pClientControlDlg->UpdateStorage(pBuffer);
	}

	return true;
}

void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	if (IDI_PLACE_JETTON_BUFFER == nIDEvent)
	{
		if (m_PlaceBetArray.GetCount()>0)
		{
			CMD_S_PlaceBet &PlaceBet=m_PlaceBetArray[0];
			PlaceBet.cbAndroidUser=FALSE;
			OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet));
			m_PlaceBetArray.RemoveAt(0);

			if(m_PlaceBetArray.GetCount()>30)
			{
				CMD_S_PlaceBet &PlaceBet=m_PlaceBetArray[0];
				PlaceBet.cbAndroidUser=FALSE;
				OnSubPlaceBet(&PlaceBet,sizeof(PlaceBet));
				m_PlaceBetArray.RemoveAt(0);
			}
		}

		if (m_PlaceBetArray.GetCount() == 0) KillTimer(IDI_PLACE_JETTON_BUFFER);
		return;
	}

	CGameFrameEngine::OnTimer(nIDEvent);
}

