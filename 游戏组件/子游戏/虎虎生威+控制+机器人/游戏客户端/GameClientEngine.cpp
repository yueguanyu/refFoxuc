 #include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientEngine.h"
#include ".\GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FREE					99									//空闲时间
#define IDI_PLACE_JETTON			100									//下注时间
#define IDI_DISPATCH_CARD			301									//发牌时间
#define IDI_OPEN_CARD				302								    //发牌时间

#define IDI_ANDROID_BET				1000	


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_MESSAGE(IDM_CONTINUE_CARD, OnContinueCard)
	ON_MESSAGE(IDM_AUTO_OPEN_CARD, OnAutoOpenCard)	
	ON_MESSAGE(IDM_OPEN_CARD, OnOpenCard)	
	ON_MESSAGE(IDM_SHOW_RESULT,OnShowResult)
	ON_MESSAGE(IDM_SOUND,OnPlaySound)
	ON_MESSAGE(IDM_ADMIN_COMMDN, OnAdminControl)	
	ON_MESSAGE(IDM_GET_ACCOUNT, OnGetAccount)	
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//构造函数
CGameClientEngine::CGameClientEngine() 
{
	//限制信息
	m_lMeMaxScore=0L;			
	m_lAreaLimitScore=0L;		
	m_lApplyBankerCondition=0L;	
	m_GameEndTime = 0;

	m_blUsing = false;



	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//庄家信息
	m_lBankerScore=0L;
	m_wCurrentBanker=0L;
	m_cbLeftCardCount=0;

	//状态变量
	m_bMeApplyBanker=false;

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	
	//CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	//ASSERT(m_pGlobalUnits!=NULL);

	//设置标题
	SetWindowText(TEXT("虎虎生威游戏  --  Ver：6.6.0.3"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	////加载声音
	//VERIFY(m_DTSDBackground.Create(TEXT("BACK_GROUND")));
	//VERIFY(m_DTSDCheer[0].Create(TEXT("CHEER1")));
	//VERIFY(m_DTSDCheer[1].Create(TEXT("CHEER2")));
	//VERIFY(m_DTSDCheer[2].Create(TEXT("CHEER3")));
	//VERIFY(m_DTSDCheer[3].Create(TEXT("SPEEKSOUNDWAV")));

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//限制信息
	m_lMeMaxScore=0L;			

	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//庄家信息
	m_lBankerScore=0L;
	m_wCurrentBanker=0L;
	m_cbLeftCardCount=0;

	//状态变量
	m_bMeApplyBanker=false;

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{

	////构造数据
	//CGameOption GameOption;
	//GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
	////配置数据
	//if (GameOption.DoModal()==IDOK)
	//{
	//	//设置控件
	//	//EnableSound(GameOption.m_bEnableSound);

	//}

	return;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	if ((nTimerID==IDI_PLACE_JETTON)&&(nElapse==0))
	{
		//设置光标
		m_GameClientView.SetCurrentJetton(0L);

		//禁止按钮
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);		
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);		
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);	
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);


		//庄家按钮
		m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
		m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );
	}
	if(IDI_DISPATCH_CARD == nTimerID)
	{
		m_GameClientView.StartHandle_Leave();
		KillTimer(IDI_DISPATCH_CARD);

		if ( m_GameEndTime <= 0 || m_GameEndTime > 100)
			m_GameEndTime = 2;
		
		SetGameClock(GetMeChairID(),IDI_OPEN_CARD ,m_GameEndTime-1);
		return false;
	}

	if (IsEnableSound()) 
	{
		if (nTimerID==IDI_PLACE_JETTON&&nElapse<=5) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
	}

	if (IsEnableSound()) 
	{
		if (nTimerID==IDI_PLACE_JETTON)
		{
	
			switch (rand()%3)
			{
			case 0:
				PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER1"));
				break;
			case 1:
				PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER2"));
				break;
			case 2:
				PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER3"));
				break;
			}
		}
	}

	return true;
}

////旁观状态
//void CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
//{
//}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:		//游戏空闲
		{
			return OnSubGameFree(pBuffer,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_PLACE_JETTON:	//用户加注
		{
			return OnSubPlaceJetton(pBuffer,wDataSize,true);
		}
	case SUB_S_APPLY_BANKER:	//申请做庄
		{
			return OnSubUserApplyBanker(pBuffer, wDataSize);
		}
	case SUB_S_CANCEL_BANKER:	//取消做庄
		{
			return OnSubUserCancelBanker(pBuffer, wDataSize);
		}
	case SUB_S_CHANGE_BANKER:	//切换庄家
		{
			return OnSubChangeBanker(pBuffer, wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_SEND_RECORD:		//游戏记录
		{
			return OnSubGameRecord(pBuffer,wDataSize);
		}
	case SUB_S_PLACE_JETTON_FAIL:	//下注失败
		{
			return OnSubPlaceJettonFail(pBuffer,wDataSize);
		}
	case SUB_S_CHECK_IMAGE:
		{
			return OnSubCheckImageIndex(pBuffer,wDataSize);
		}
	case SUB_S_ADMIN_COMMDN:		//设置
		{
			return OnSubAdminControl(pBuffer,wDataSize);
		}	
	case SUB_S_ADMIN_CHEAK:
		{
			CMD_S_UserJettonScore* pResult = (CMD_S_UserJettonScore*)pBuffer;
			for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
				m_GameClientView.m_pAdminControl->SetUserGameScore(nAreaIndex - 1,pResult->lUserJettonScore[nAreaIndex]);
			return true;
		}
	case SUB_S_SEND_ACCOUNT:		//设置
		{
			CMD_C_SendAccount* pResult = (CMD_C_SendAccount*)pBuffer;
			m_GameClientView.m_pAdminControl->ResetUserNickName();
			for (int i=0; i<pResult->m_UserCount; i++ )
			{
				IClientUserItem *pUserData=GetTableUserItem(pResult->m_UserID[i]);
				m_GameClientView.m_pAdminControl->SetUserNickName(pUserData->GetNickName());
			}
			
			return true;
		}	
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{

	switch (cbGameStation)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
			m_bEnableSysBanker=pStatusFree->bEnableSysBanker;

		
		
			m_GameClientView.m_ShowImageIndex = pStatusFree->CheckImage;

            //玩家信息
			m_lMeMaxScore=pStatusFree->lUserMaxScore;
			m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
			//IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

			IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

			m_GameClientView.SetMeChairID(pMeUserData->GetUserID());

            //庄家信息
			SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->lBankerScore);
			m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->lBankerWinScore);
		
			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

            //控制信息
			m_lApplyBankerCondition=pStatusFree->lApplyBankerCondition;
			m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
			//TCHAR str[222];
			//sprintf(str,"%d",m_pIClientKernel->GetUserAttribute()->dwUserRight);
			//AfxMessageBox(str);

			//设置状态
			SetGameStatus(GAME_STATUS_FREE);

			//更新控制
			UpdateButtonContron();
			m_GameClientView.RefreshGameView();

			//设置时间
			SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);
			
			//播放声音
			/*if (IsEnableSound()) m_DTSDBackground.Play(0,true);
			else m_DTSDBackground.Stop();*/
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

			return true;
		}
	case GS_PLACE_JETTON:	//游戏状态
	case GS_GAME_END:		//结束状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			
			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			m_GameClientView.m_ShowImageIndex = pStatusPlay->CheckImage;

			//下注信息
			for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
			{
				m_GameClientView.PlaceUserJetton(nAreaIndex,pStatusPlay->lAllJettonScore[nAreaIndex]);
				SetMePlaceJetton(nAreaIndex,pStatusPlay->lUserJettonScore[nAreaIndex]);
			}

			//玩家积分
			m_lMeMaxScore=pStatusPlay->lUserMaxScore;			
			m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
			IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
			m_GameClientView.SetMeChairID(pMeUserData->GetUserID());

			//控制信息
			m_lApplyBankerCondition=pStatusPlay->lApplyBankerCondition;
			m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

			if (pStatusPlay->cbGameStatus==GS_GAME_END)
			{
				//扑克信息
				m_GameClientView.SetCardInfo(pStatusPlay->cbTableCardArray);
				m_GameClientView.FinishDispatchCard(false);

				//设置成绩
				m_GameClientView.SetCurGameScore(pStatusPlay->lEndUserScore,pStatusPlay->lEndUserReturnScore,pStatusPlay->lEndBankerScore,pStatusPlay->lEndRevenue);
			}
			else
			{
				m_GameClientView.SetCardInfo(NULL);
				m_blUsing = true;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));
			}

			//播放声音
			/*if (IsEnableSound()) m_DTSDBackground.Play(0,true);
			else m_DTSDBackground.Stop();*/
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

			//庄家信息
			SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->lBankerScore);
			m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->lBankerWinScore);
			m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

			//开启
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			//TCHAR str[222];
			//sprintf(str,"%d",m_pIClientKernel->GetUserAttribute()->dwUserRight);
			//AfxMessageBox(str);
			//设置状态
			SetGameStatus(pStatusPlay->cbGameStatus);

			//设置时间
			UINT nTimerID = pStatusPlay->cbGameStatus==GS_GAME_END ? IDI_OPEN_CARD : IDI_PLACE_JETTON;
			SetGameClock(GetMeChairID(), nTimerID, pStatusPlay->cbTimeLeave);

			m_GameClientView.StartRandShowSide();

			//更新按钮
			UpdateButtonContron();
			m_GameClientView.RefreshGameView();

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	m_GameClientView.SwithToNormalView();

	m_GameClientView.KillCardTime();

	m_blUsing = true;

	//庄家信息
	SetBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);

	//玩家信息
	m_lMeMaxScore=pGameStart->lUserMaxScore;
	m_GameClientView.SetMeMaxScore(m_lMeMaxScore);

	//设置时间
	SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pGameStart->cbTimeLeave);

	m_GameClientView.StartRandShowSide();

	//设置状态
	SetGameStatus(GS_PLACE_JETTON);
	SetTimer(IDI_ANDROID_BET, 100, NULL);

	m_GameClientView.m_bShowBao = true;


	//更新控制
	UpdateButtonContron();

	//设置提示
	m_GameClientView.SetDispatchCardTip(pGameStart->bContiueCard ? enDispatchCardTip_Continue : enDispatchCardTip_Dispatch);

	

	//播放声音
	/*if (IsEnableSound()) 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
		m_DTSDBackground.Play(0,true);
	}
	else 
	{
		m_DTSDBackground.Stop();
	}*/

	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	PlayGameSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));
	
	if (m_GameClientView.m_pAdminControl->GetSafeHwnd())		 
	{
		m_GameClientView.m_pAdminControl->ResetUserBet();
	}

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

	m_blUsing = true;

	//设置时间
	SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

	//m_GameClientView.StartMove();

	//设置状态
	SetGameStatus(GAME_STATUS_FREE);

	//清理时间
	KillTimer(IDI_ANDROID_BET);

	////清理桌面
	bool blWin[AREA_COUNT];
	for (int i = 0;i<AREA_COUNT;i++){

			blWin[i]=false;
	}

	m_GameClientView.FinishDispatchCard();

	m_GameClientView.SetWinnerSide(blWin, false);



	for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) SetMePlaceJetton(nAreaIndex,0);

	m_GameClientView.CleanUserJetton();
	
	//更新成绩
	for (WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex)
	{
		//tagUserData const *pUserData = GetTableUserItem(wUserIndex);
		
		IClientUserItem * pClientUserItem=GetTableUserItem(wUserIndex);

		if ( pClientUserItem == NULL ) continue;
		tagUserInfo* pUserInfo =  pClientUserItem->GetUserInfo();
		tagApplyUser ApplyUser ;

		//更新信息
		ApplyUser.lUserScore = pClientUserItem->GetUserScore();
		ApplyUser.strUserName = pUserInfo->szNickName;
		m_GameClientView.m_ApplyUser.UpdateUser(ApplyUser);
	

	}

	

	/*if(m_GameClientView.m_btApplyBanker.IsWindowVisible())

	{
		if(m_GameClientView.m_ApplyUser.GetItemCount()>=8)
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(BT_APPLY_BANKER_FULL,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
			if(m_GameClientView.m_btApplyBanker.IsWindowVisible())
				;
			else
				m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);

		}else
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);


		}
	}
	else
		m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);*/



	//完成发牌




	m_GameClientView.SwitchToCheck();

	//更新控件
	UpdateButtonContron();

	return true;
}

//用户加注
bool CGameClientEngine::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize,bool bGameMes)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return false;

	//消息处理
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	if (pPlaceJetton->cbAndroid==FALSE && (m_GameClientView.m_pAdminControl->GetSafeHwnd())&&bGameMes)
	{
		m_GameClientView.m_pAdminControl->SetUserBetScore(pPlaceJetton->cbJettonArea - 1,pPlaceJetton->lJettonScore);
	}


	if (GetMeChairID()!=pPlaceJetton->wChairID || IsLookonMode())
	{


		//加注界面
		if(pPlaceJetton->cbAndroid == TRUE)
		{
			//保存
			static WORD wStFluc = 1;	//随机辅助
			tagAndroidBet androidBet = {};
			androidBet.cbJettonArea = pPlaceJetton->cbJettonArea;
			androidBet.lJettonScore = pPlaceJetton->lJettonScore;
			androidBet.wChairID = pPlaceJetton->wChairID;
			androidBet.nLeftTime = ((rand()+androidBet.wChairID+wStFluc*3)%10+1)*100;
			wStFluc = wStFluc%3 + 1;

			m_ListAndroid.AddTail(androidBet);
		}
		else
		{
			m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);

			//播放声音
			if (IsEnableSound()) 
			{
				if (pPlaceJetton->wChairID!=GetMeChairID() || IsLookonMode())
				{
					if (pPlaceJetton->lJettonScore==5000000) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
					else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));

					switch (rand()%3)
					{
					case 0:
						PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER1"));
						break;
					case 1:
						PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER2"));
						break;
					case 2:
						PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER3"));
						break;
					}
				}
			}
		}
	}
	UpdateButtonContron();
    
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

	m_GameClientView.m_blMoveFinish = false;

	
	m_GameEndTime = pGameEnd->cbTimeLeave;

	m_blUsing = true;

	//设置时间
	SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

	//扑克信息
	m_GameClientView.SetCardInfo(pGameEnd->cbTableCardArray);
	m_GameClientView.ClearAreaFlash();

	//設置撲克移動
	m_GameClientView.StartRunCar(20);

	//设置状态
	SetGameStatus(GS_GAME_END);

	m_cbLeftCardCount=pGameEnd->cbLeftCardCount;

	//庄家信息
	m_GameClientView.SetBankerScore(pGameEnd->nBankerTime, pGameEnd->lBankerTotallScore);
	//成绩信息
	m_GameClientView.SetCurGameScore(pGameEnd->lUserScore,pGameEnd->lUserReturnScore,pGameEnd->lBankerScore,pGameEnd->lRevenue);

	m_GameClientView.PerformAllBetAnimation();

	m_TempData.a = pGameEnd->lUserScore;
	m_TempData.b = pGameEnd->lUserReturnScore;
	m_TempData.c = pGameEnd->lBankerScore;
	m_TempData.d = pGameEnd->lRevenue;
	
	//更新控件
	UpdateButtonContron();

	////停止声音
	//for (int i=0; i<CountArray(m_DTSDCheer); ++i) m_DTSDCheer[i].Stop();

	return true;
}

//更新控制
void CGameClientEngine::UpdateButtonContron()
{
	/*if(__TEST__)
	{
		return ;
	}*/
	//置能判断
	bool bEnablePlaceJetton=true;

	if(m_wCurrentBanker==INVALID_CHAIR)
	{
		bEnablePlaceJetton = true;

	}
	if (GetGameStatus()!=GS_PLACE_JETTON)
	{
		bEnablePlaceJetton=false;

	}
	if (m_wCurrentBanker==GetMeChairID()) 
	{
		bEnablePlaceJetton=false;
	}
	if (IsLookonMode())
	{
		bEnablePlaceJetton=false;
	}
	if (m_bEnableSysBanker==false&&m_wCurrentBanker==INVALID_CHAIR) 
	{
		bEnablePlaceJetton=false;
	}

	if(GetGameStatus()==GS_GAME_END)
	{
		m_GameClientView.m_btOpenCard.EnableWindow(false);
		m_GameClientView.m_btAutoOpenCard.EnableWindow(false);

	}else
	{
		m_GameClientView.m_btOpenCard.EnableWindow(true);
		m_GameClientView.m_btAutoOpenCard.EnableWindow(true);

	}

	m_GameClientView.SetEnablePlaceJetton(bEnablePlaceJetton);

	//下注按钮
	if (bEnablePlaceJetton==true)
	{
		
		//计算积分
		LONGLONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
		LONGLONG lLeaveScore=m_lMeMaxScore;
		for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex) lLeaveScore -= m_lUserJettonScore[nAreaIndex];

		//最大下注
		LONGLONG lUserMaxJetton = 0;

		for (int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
		{
			if(lUserMaxJetton==0&&nAreaIndex == 1)
			{
				lUserMaxJetton = m_GameClientView.GetUserMaxJetton(nAreaIndex);

			}else
			{
				if(m_GameClientView.GetUserMaxJetton(nAreaIndex)>lUserMaxJetton)
				{
					lUserMaxJetton = m_GameClientView.GetUserMaxJetton(nAreaIndex);
				}
			}
		}
		lLeaveScore = min((lLeaveScore),lUserMaxJetton); //用户可下分 和最大分比较 由于是五倍 

		//设置光标
		if (lCurrentJetton>lLeaveScore)
		{
			if (lLeaveScore>=5000000L) m_GameClientView.SetCurrentJetton(5000000L);
			else if (lLeaveScore>=1000000L) m_GameClientView.SetCurrentJetton(1000000L);
			else if (lLeaveScore>=100000L) m_GameClientView.SetCurrentJetton(100000L);
			else if (lLeaveScore>=10000L) m_GameClientView.SetCurrentJetton(10000L);
			else if (lLeaveScore>=1000L) m_GameClientView.SetCurrentJetton(1000L);
			else if (lLeaveScore>=100L) m_GameClientView.SetCurrentJetton(100L);
			else m_GameClientView.SetCurrentJetton(0L);
		}

		//控制按钮
		int iTimer = 1;

		if(m_blUsing==false)
		{
			lLeaveScore = 0;
			lUserMaxJetton = 0;

		}
		m_GameClientView.m_btJetton100.EnableWindow((lLeaveScore>=100*iTimer && lUserMaxJetton>=100*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow((lLeaveScore>=1000*iTimer && lUserMaxJetton>=1000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow((lLeaveScore>=10000*iTimer && lUserMaxJetton>=10000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton100000.EnableWindow((lLeaveScore>=100000*iTimer && lUserMaxJetton>=100000*iTimer)?TRUE:FALSE);
		m_GameClientView.m_btJetton1000000.EnableWindow((lLeaveScore>=1000000*iTimer && lUserMaxJetton>=1000000*iTimer)?TRUE:FALSE);		
		m_GameClientView.m_btJetton5000000.EnableWindow((lLeaveScore>=5000000*iTimer && lUserMaxJetton>=5000000*iTimer)?TRUE:FALSE);
	}
	else
	{
		//设置光标
		m_GameClientView.SetCurrentJetton(0L);

		//禁止按钮
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);		
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton50000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
	
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);
	}

	


	//庄家按钮
	if (!IsLookonMode())
	{
		//获取信息
		IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

		//申请按钮
		bool bEnableApply=true;
		if (m_wCurrentBanker==GetMeChairID()) bEnableApply=false;
		if (m_bMeApplyBanker) bEnableApply=false;
		if (pMeUserData->GetUserScore()<m_lApplyBankerCondition) bEnableApply=false;
		m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);

		//取消按钮
		bool bEnableCancel=true;
		if (m_wCurrentBanker==GetMeChairID() && GetGameStatus()!=GAME_STATUS_FREE) bEnableCancel=false;
		if (m_bMeApplyBanker==false) bEnableCancel=false;
		m_GameClientView.m_btCancelBanker.EnableWindow(bEnableCancel?TRUE:FALSE);
		m_GameClientView.m_btCancelBanker.SetButtonImage(m_wCurrentBanker==GetMeChairID()?IDB_BT_CANCEL_BANKER:IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

		//显示判断
		if (m_bMeApplyBanker)
		{
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
			
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
	//获取信息
	IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
	//银行按钮
	m_GameClientView.m_btBankDraw.EnableWindow(TRUE);
	m_GameClientView.m_btBankStorage.EnableWindow(FALSE);
	m_GameClientView.m_btBankStorage.ShowWindow(SW_SHOW);
	m_GameClientView.m_btBankStorage.EnableWindow((GetGameStatus()==GAME_STATUS_FREE || IsLookonMode())?TRUE:FALSE);
	if(m_wCurrentBanker==GetMeChairID()&&IsLookonMode()==false)
	{
		m_GameClientView.m_btBankStorage.EnableWindow(FALSE);
	}
	
	if (m_wCurrentBanker==GetMeChairID()) m_GameClientView.m_blCanStore = false;
	else  m_GameClientView.m_blCanStore = true;
	m_GameClientView.SetInsureOption(false, m_GameClientView.m_blCanStore);

	ReSetBankCtrol(GetGameStatus());
	return;
}
bool CGameClientEngine::OnSubCheckImageIndex(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_CheckImage));
	if (wDataSize!=sizeof(CMD_C_CheckImage)) return false;

	CMD_C_CheckImage * pCheckImage=(CMD_C_CheckImage *)pBuffer;
	m_GameClientView.m_ShowImageIndex = pCheckImage->Index;

	return true;


}
//加注消息
LRESULT CGameClientEngine::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbJettonArea=(BYTE)wParam;
	LONGLONG lJettonScore=(LONGLONG)(*((LONGLONG*)lParam));

	//合法判断
	ASSERT(cbJettonArea>=1 && cbJettonArea<=AREA_COUNT);
	if (!(cbJettonArea>=1 && cbJettonArea<=AREA_COUNT)) return 0;

	//庄家判断
	if ( GetMeChairID() == m_wCurrentBanker ) return true;

	
	//状态判断
	if (GetGameStatus()!=GS_PLACE_JETTON)
	{
		UpdateButtonContron();
		return true;
	}
	
	//设置变量
	m_lUserJettonScore[cbJettonArea] += lJettonScore;
	m_GameClientView.SetMePlaceJetton(cbJettonArea, m_lUserJettonScore[cbJettonArea]);



	//变量定义
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
	PlaceJetton.cbJettonArea=cbJettonArea;
	PlaceJetton.lJettonScore=lJettonScore;

	//发送消息
	if(!SendSocketData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton)))
	{
		
		return 0;
	};

	//更新按钮
	UpdateButtonContron();

	//预先显示
	m_GameClientView.PlaceUserJetton(cbJettonArea,lJettonScore);

	//播放声音
	if (IsEnableSound()) 
	{
		if (lJettonScore==5000000) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
		switch (rand()%3)
		{
		case 0:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER1"));
			break;
		case 1:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER2"));
			break;
		case 2:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER3"));
			break;
		}
	}

	return 0;

}
LRESULT  CGameClientEngine::OnPlaySound(WPARAM wParam, LPARAM lParam)
{
	
	if (IsEnableSound()) 
	{
		if(lParam == 0)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("PLEASEJETTONWAV"));
		}
		else if(lParam==1)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("LEAVEHANDLEWAV"));
		}
		else if(lParam==3)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("IDC_SNDWAV"));
		}
		else if(lParam==4)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
		}
		else if(lParam==5)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
		}
		else if(lParam==6)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
		}
		else if(lParam==7)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
		}
	}
	

	return 1;
		
}

LRESULT  CGameClientEngine::OnShowResult(WPARAM wParam, LPARAM lParam)
{

	TCHAR szBuffer[128] = TEXT("");
	myprintf(szBuffer,CountArray(szBuffer),TEXT(" 本局分数 %I64d 返回分数 %I64d 庄家分数 %I64d "),m_TempData.a,m_TempData.b,m_TempData.c);
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertCustomString(szBuffer,RGB(255,0,255));

	return 1;

}
//手工搓牌
LRESULT  CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	if (GetGameStatus()==GS_GAME_END)
	{
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertCustomString(TEXT("［系统提示］开牌时间不能切换腾牌模式！"),RGB(255,0,255));
		return 1;

	}
	m_GameClientView.m_blAutoOpenCard = false;
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertCustomString(TEXT("［系统提示］您选择了手工腾牌模式，发牌后您可以使用鼠标拖动扑克！"),RGB(255,0,255));
	//m_GameClientView.m_btOpenCard.EnableWindow(false);
	//m_GameClientView.m_btAutoOpenCard.EnableWindow(true);
	return 1;
}
//自动搓牌
LRESULT  CGameClientEngine::OnAutoOpenCard(WPARAM wParam, LPARAM lParam)
{
	if (GetGameStatus()==GS_GAME_END)
	{
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertCustomString(TEXT("［系统提示］开牌时间不能切换腾牌模式！"),RGB(255,0,255));
		return 1;

	}
	m_GameClientView.m_blAutoOpenCard = true;
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertCustomString(TEXT("［系统提示］您选择了自动腾牌模式，开牌后系统将自动开出各家的牌！"),RGB(255,0,255));
	//m_GameClientView.m_btOpenCard.EnableWindow(true);
	//m_GameClientView.m_btAutoOpenCard.EnableWindow(false);
	return 1;
}
//继续发牌
LRESULT CGameClientEngine::OnContinueCard(WPARAM wParam, LPARAM lParam)
{
	//合法判断
	if (GetMeChairID()!=m_wCurrentBanker) return 0;
	if (GetGameStatus()!=GAME_STATUS_FREE) return 0;
	if (m_cbLeftCardCount < 8) return 0;
	if (IsLookonMode()) return 0;

	//发送消息
	//SendSocketData(SUB_C_CONTINUE_CARD);
    CMD_C_CheckImage CheckImage;
    CheckImage.Index =   m_GameClientView.m_CheckImagIndex;
	SendSocketData(SUB_C_CHECK_IMAGE,&CheckImage,sizeof(CheckImage));
	//设置按钮

	return 0;
}

//申请消息
LRESULT CGameClientEngine::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
	//合法判断
	//tagUserData const *pMeUserData = GetTableUserItem( GetMeChairID() );

	IClientUserItem * pClientUserItem=GetTableUserItem(GetMeChairID());

	if (pClientUserItem->GetUserScore() < m_lApplyBankerCondition) return true;

	//旁观判断
	if (IsLookonMode()) return true;

	//转换变量
	bool bApplyBanker = wParam ? true:false;

	//当前判断
	if (m_wCurrentBanker == GetMeChairID() && bApplyBanker) return true;

	if (bApplyBanker)
	{
		//发送消息
		SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

		//m_bMeApplyBanker=true;
	}
	else
	{
		//发送消息
		SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

		//m_bMeApplyBanker=false;
	}

	//设置按钮
	UpdateButtonContron();

	return true;
}

//申请做庄
bool CGameClientEngine::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//消息处理
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	//获取玩家
	//tagUserData const *pUserData=GetTableUserItem(pApplyBanker->wApplyUser);
	
	IClientUserItem * pClientUserItem=GetTableUserItem(pApplyBanker->wApplyUser);
	tagUserInfo* pUserInfo =  pClientUserItem->GetUserInfo();
	//插入玩家
	if (m_wCurrentBanker!=pApplyBanker->wApplyUser)
	{
		tagApplyUser ApplyUser;
		ApplyUser.strUserName=pUserInfo->szNickName;
		ApplyUser.lUserScore=pClientUserItem->GetUserScore();
		m_GameClientView.m_ApplyUser.InserUser(ApplyUser);

	}

	//自己判断
	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) m_bMeApplyBanker=true;

	//更新控件
	UpdateButtonContron();


	/*if(m_GameClientView.m_btApplyBanker.IsWindowVisible())

	{
		if(m_GameClientView.m_ApplyUser.GetItemCount()>=8)
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(BT_APPLY_BANKER_FULL,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
			if(m_GameClientView.m_btApplyBanker.IsWindowVisible())
				;
			else
				m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);

		}else
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);


		}
	}
	else
		m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);*/


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

	

	/*if(m_GameClientView.m_ApplyUser.GetItemCount()>4)
	{
		m_GameClientView.m_btUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btUp.EnableWindow(true);
		m_GameClientView.m_btDown.ShowWindow(SW_SHOW);
		m_GameClientView.m_btDown.EnableWindow(true);  

	}else
	{
		m_GameClientView.m_btUp.ShowWindow(SW_HIDE);
		m_GameClientView.m_btUp.EnableWindow(true);
		m_GameClientView.m_btDown.ShowWindow(SW_HIDE);
		m_GameClientView.m_btDown.EnableWindow(true); 

	}*/
	tagApplyUser ApplyUser;
	ApplyUser.strUserName=pCancelBanker->szCancelUser;
	ApplyUser.lUserScore=0;
	m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);

	//自己判断
	IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
	tagUserInfo* pUserInfo =  pMeUserData->GetUserInfo();

	if (IsLookonMode()==false && lstrcmp(pUserInfo->szNickName,pCancelBanker->szCancelUser)==0) m_bMeApplyBanker=false;

	
	//更新控件
	UpdateButtonContron();

	//删除玩家
	

	
	/*if(m_GameClientView.m_btApplyBanker.IsWindowVisible())

	{
		if(m_GameClientView.m_ApplyUser.GetItemCount()>=8)
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(BT_APPLY_BANKER_FULL,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
			if(m_GameClientView.m_btApplyBanker.IsWindowVisible())
				;
			else
				m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);

		}else
		{
			HINSTANCE hResInstance=AfxGetInstanceHandle();
			m_GameClientView.m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false);
			m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);


		}
	}
	else
		m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);*/


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
	m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

	//自己判断
	if (m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	{
		m_bMeApplyBanker=false;
	}
	else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	{
		m_bMeApplyBanker=true;
	}

	//庄家信息
	SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->lBankerScore);
	m_GameClientView.SetBankerScore(0,0);

	//删除玩家
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//tagUserData const *pBankerUserData=GetTableUserItem(m_wCurrentBanker);
		IClientUserItem * pClientUserItem=GetTableUserItem(m_wCurrentBanker);
		tagUserInfo* pUserInfo =  pClientUserItem->GetUserInfo();
		if (pClientUserItem != NULL)
		{
			tagApplyUser ApplyUser;
			ApplyUser.strUserName = pUserInfo->szNickName;
			m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);
		}
	}

	//更新界面
	UpdateButtonContron();
	m_GameClientView.RefreshGameView();

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

		m_GameClientView.SetGameHistory(pServerGameRecord->bWinMen);
	}

	return true;
}

//下注失败
bool CGameClientEngine::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize!=sizeof(CMD_S_PlaceJettonFail)) return false;

	//消息处理
	CMD_S_PlaceJettonFail * pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;

	//效验参数
	BYTE cbViewIndex=pPlaceJettonFail->lJettonArea;
	ASSERT(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT);
	if (!(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT)) return false;

	//加注界面
	m_GameClientView.PlaceUserJetton(pPlaceJettonFail->lJettonArea,-pPlaceJettonFail->lPlaceScore);

	//自己判断
	if (GetMeChairID()==pPlaceJettonFail->wPlaceUser && false==IsLookonMode())
	{
		LONGLONG lJettonCount=pPlaceJettonFail->lPlaceScore;
		//合法校验
		ASSERT(m_lUserJettonScore[cbViewIndex]>=lJettonCount);
		if (lJettonCount>m_lUserJettonScore[cbViewIndex]) return false;

		//设置下注
		m_lUserJettonScore[cbViewIndex]-=lJettonCount;
		m_GameClientView.SetMePlaceJetton(cbViewIndex,m_lUserJettonScore[cbViewIndex]);
	}

	return true;
}

//设置庄家
void CGameClientEngine::SetBankerInfo(WORD wBanker,LONGLONG lScore)
{
	m_wCurrentBanker=wBanker;
	m_lBankerScore=lScore;
	//tagUserData const *pUserData=m_wCurrentBanker==INVALID_CHAIR ? NULL : GetTableUserItem(m_wCurrentBanker);
	IClientUserItem * pClientUserItem=m_wCurrentBanker==INVALID_CHAIR ? NULL : GetTableUserItem(m_wCurrentBanker);
	DWORD dwBankerUserID = (NULL==pClientUserItem) ? 0 : pClientUserItem->GetUserID();
	m_GameClientView.SetBankerInfo(dwBankerUserID,m_lBankerScore);

	//if (IsLookonMode()==false && GetMeChairID()==m_wCurrentBanker) m_bMeApplyBanker=true;
	//else m_bMeApplyBanker = false;
	//UpdateButtonContron();

}

//个人下注
void CGameClientEngine::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//合法判断
	ASSERT(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT);
	if (!(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT)) return;

	//设置变量
	m_lUserJettonScore[cbViewIndex]=lJettonCount;

	//设置界面
	m_GameClientView.SetMePlaceJetton(cbViewIndex,lJettonCount);
}
//////////////////////////////////////////////////////////////////////////
//控制
LRESULT CGameClientEngine::OnAdminControl( WPARAM wParam, LPARAM lParam )
{
	CMD_C_ControlApplication* pData = (CMD_C_ControlApplication*)wParam;
	CMD_C_ControlApplication Application;
	ZeroMemory(&Application,sizeof(Application));
	Application.cbControlAppType = pData->cbControlAppType;
	Application.cbControlArea = pData->cbControlArea;
	Application.cbControlTimes = pData->cbControlTimes;
	StrCpy(Application.dwUserAccount,pData->dwUserAccount);
	//TCHAR str[222];
	//myprintf(str,_T("类型：%d，区域：%d"),pData->cbControlAppType,pData->cbControlArea);
	//AfxMessageBox(str);
	SendSocketData(SUB_C_ADMIN_COMMDN,&Application,sizeof(Application));
	return true;
}

//控制
LRESULT CGameClientEngine::OnGetAccount( WPARAM wParam, LPARAM lParam )
{
	SendSocketData(SUB_C_GET_ACCOUNT);
	return true;
}

//控制
bool CGameClientEngine::OnSubAdminControl( const void * pBuffer, WORD wDataSize )
{
	ASSERT(wDataSize==sizeof(CMD_S_ControlReturns));
	if(wDataSize!=sizeof(CMD_S_ControlReturns)) return false;

	if(m_GameClientView.m_pAdminControl!=NULL)
	{
		CMD_S_ControlReturns* pResult = (CMD_S_ControlReturns*)pBuffer;
		m_GameClientView.m_pAdminControl->UpdateControl(pResult);
	}
	return true;
}

void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == IDI_ANDROID_BET)
	{
		if (m_ListAndroid.GetCount() != 0)
		{
			POSITION pos = m_ListAndroid.GetTailPosition();
			POSITION posTmp = pos;

			//遍历下注链表
			while (true)
			{
				if (pos == 0)
					break;

				posTmp = pos;
				tagAndroidBet & androidBet = m_ListAndroid.GetPrev(pos);

				androidBet.nLeftTime -= 100;
				if (androidBet.nLeftTime <= 0)
				{		
					//模拟消息
					CMD_S_PlaceJetton placeJetton = {};
					placeJetton.cbAndroid = FALSE;
					placeJetton.cbJettonArea = androidBet.cbJettonArea;
					placeJetton.lJettonScore = androidBet.lJettonScore;
					placeJetton.wChairID = androidBet.wChairID;

					OnSubPlaceJetton((void*)&placeJetton, sizeof(placeJetton),false);

					//删除元素
					m_ListAndroid.RemoveAt(posTmp);
				}
			}
		}
	}
	CGameFrameEngine::OnTimer(nIDEvent);
}


void CGameClientEngine::ReSetBankCtrol(int nGameState)
{

	if(!IsLookonMode())
	{
		switch (GetGameStatus())
		{
		case GAME_STATUS_FREE:
			{
				if(GetMeChairID()!=m_wCurrentBanker)
				{					
					m_GameClientView.m_btBankDraw.EnableWindow(TRUE);
					m_GameClientView.m_btBankStorage.EnableWindow(TRUE);
				}
				else
				{					
					m_GameClientView.m_btBankDraw.EnableWindow(TRUE);
					m_GameClientView.m_btBankStorage.EnableWindow(FALSE);
				}
				break;
			}
		case GS_GAME_END:
		case GS_PLACE_JETTON:
		case GS_MOVECARD_END:
			{
				if(GetMeChairID()!=m_wCurrentBanker)
				{
					m_GameClientView.m_btBankDraw.EnableWindow(TRUE);
					m_GameClientView.m_btBankStorage.EnableWindow(FALSE);
				}
				else
				{
					m_GameClientView.m_btBankDraw.EnableWindow(FALSE);
					m_GameClientView.m_btBankStorage.EnableWindow(FALSE);
				}				
				break;
			}
		default:break;
		}
	}
	else
	{
		m_GameClientView.m_btBankDraw.EnableWindow(TRUE);
		m_GameClientView.m_btBankStorage.EnableWindow(TRUE);
	}

}

////用户进入
//VOID  CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
//{
//	//设置成绩
//	if (bLookonUser==false)
//	{
//		
//		//更新界面
//		m_GameClientView.RefreshGameView();
//	}
//
//	return;
//}
//
////用户离开
//VOID  CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
//{
//	////设置成绩
//	//if (bLookonUser==false)
//	//{
//	//	
//
//	//	WORD wViewChairId = SwitchViewChairID(pIClientUserItem->GetChairID());
//	//	
//	//	//更新界面
//	//	m_GameClientView.RefreshGameView();
//	//}
//
//	return;
//}
//
////用户状态
//VOID  CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
//{
//	////变量定义
//	//IClientUserItem * pIMySelfUserItem=GetMeUserItem();
//	//BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
//
//	////自动开始
//	//if ((pIClientUserItem!=pIMySelfUserItem)&&(cbUserStatus==US_READY)) PerformAutoStart();
//
//	////停止开始
//	//if ((pIClientUserItem==pIMySelfUserItem)&&(cbUserStatus==US_READY)) KillGameClock(IDI_START_GAME);
//
//	return;
//}

//选择事件
VOID  CGameClientEngine::OnEventPhraseSelect(INT nPhraseIndex)
{
	////效验
	//ASSERT( m_PhraseVoiceHelper.GetInterface() );
	//if( m_PhraseVoiceHelper.GetInterface() == NULL ) return;

	////获取文字
	//LPCTSTR lpszPhrase = m_PhraseVoiceHelper->GetPhraseString(nPhraseIndex,NULL,0);

	////聊天发送
	//if( SendChatMessage(0,lpszPhrase,CGlobalGameOption::GetGlobalOption()->m_crChatTX) )
	//{
	//	//播放声音
	//	if( CGlobalGameOption::GetGlobalOption()->m_bAllowSound )
	//	{
	//		IClientUserItem *pIMyselfUsetItem = m_pIClientKernel->GetMeUserItem();
	//		if( pIMyselfUsetItem == NULL ) return;
	//		m_PhraseVoiceHelper->PlayPhraseVoice(pIMyselfUsetItem->GetChairID(),nPhraseIndex,pIMyselfUsetItem->GetGender());
	//	}

	//	//发送消息
	//	CMD_C_Phrase Phrase = {};
	//	Phrase.nPhraseIndex = nPhraseIndex;
	//	SendSocketData( SUB_C_PHRASE,&Phrase,sizeof(Phrase) );
	//}
}

//播放结束
VOID  CGameClientEngine::OnEventSoundStop( DWORD dwSoundID )
{
	//播放紧接声音
	/*for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_dwSoundId[i] == dwSoundID )
		{
			//报单
			if( m_cbHandCardCount[i] == 1 )
				PlayActionSound( i,TEXT("BAO_JING1") );
			报双
			else if( m_cbHandCardCount[i] == 2 )
				PlayActionSound( i,TEXT("BAO_JING2") );

			m_dwSoundId[i] = 0;

			break;
		}
	}*/
	return;
}

bool	CGameClientEngine::IsEnableSound()
{
	return true;
}
