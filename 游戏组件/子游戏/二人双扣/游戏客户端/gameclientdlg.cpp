#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "cassert"
//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_START_GAME					201								//开始定时器
#define IDI_PASS_CARD					202								//弃牌定时器
#ifdef _DEBUG
#define TIME_OUT_CARD				30
#define TIME_PASS_CARD				2
#define TIME_START_GAME				15
#else
#define TIME_OUT_CARD				30
#define TIME_PASS_CARD				2
#define TIME_START_GAME				30
#endif


#define MAX_TIMEOUT_TIMES				3								//最大超时次数

//游戏定时器
#define IDI_WAIT_CLEAR					300								//清除等待
#define IDI_DISPATCH_CARD				301								//发牌定时器
#define IDI_LAST_TURN_CARD				302								//上轮定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_CARD_TYPE,OnCardType)
	ON_MESSAGE(IDM_REQ_HUNTER,OnReqHunter)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_SORT_BY_COUNT,OnSortByCount)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAST_TURN_CARD,OnLastTurnCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_bReqHumter=false;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	//胜利信息

	m_wWinOrder=0xFFFF;

	//配置变量
	m_bStustee=false;
	m_bSortCount=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//上轮扑克
	m_bLastTurn=false;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	memset(m_cbOutCardCount,255,sizeof(m_cbOutCardCount));

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//开始信息
	m_wGetRandCardID=INVALID_CHAIR;
	m_cbGetCardPosition=0xFF;
	m_cbRandCardData=0xFF;
	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{

	//全局对象
	//CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	//ASSERT(m_pGlobalUnits!=NULL);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;


	return true;
}


//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//删除定时
	KillGameClock(0);
	KillTimer(IDI_WAIT_CLEAR);
	KillTimer(IDI_DISPATCH_CARD);
	
	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//配置变量
	m_bStustee=false;
	m_bSortCount=false;

	//胜利信息
	m_wWinOrder=0xFFFF;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//上轮扑克
	m_bLastTurn=false;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	memset(m_cbOutCardCount,255,sizeof(m_cbOutCardCount));

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//开始信息
	m_wGetRandCardID=INVALID_CHAIR;
	m_cbGetCardPosition=0xFF;
	m_cbRandCardData=0xFF;

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	////构造数据
	//CGameOption GameOption;
	//GameOption.m_dwCardHSpace=m_dwCardHSpace;
	//GameOption.m_bHaveVoiceCard=m_bHaveVoiceCard;
	//GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;

	////配置数据
	//if (GameOption.DoModal()==IDOK)
	//{
	//	//获取参数
	//	m_dwCardHSpace=GameOption.m_dwCardHSpace;

	//	//设置控件
	//	if(m_bHaveVoiceCard)
	//	   m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
	//	
	//	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardSpace(m_dwCardHSpace,0,20);
	//	//保存配置
	//	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	//}

	return;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_OUT_CARD:			//用户出牌
		{
			//超次托管
			if((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
			{
				//如果时间耗尽且玩家为非托管状态
				if(nElapse==0||(m_bStustee&&nElapse<TIME_OUT_CARD-2))
				{
					KillGameClock(IDI_OUT_CARD);
					m_wTimeOutCount++;
					AutomatismOutCard();
				}
				//托管处理
				if (m_wTimeOutCount>=MAX_TIMEOUT_TIMES&&!m_bStustee)
				{
					OnStusteeControl(0,0);
					if(m_pIStringMessage != NULL)
						m_pIStringMessage->InsertSystemString(TEXT("由于您多次超时，切换为系统托管”模式."));
					m_bStustee=true;
				}
				
			}
			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)/*&&m_pGlobalUnits->m_bAllowSound*/) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			return true;
		}
	case IDI_PASS_CARD:
		{
			if(nElapse==0)
				OnPassCard(0,0);
			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
						AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}

	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//旁观设置
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pData,wDataSize);
		}
	}

	//非法消息
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_SK_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree))return false;

			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			
			if (IsLookonMode()==false)
			{
				if(GetMeUserItem()->GetUserStatus()!=US_READY)
				{
					//设置控件
					m_GameClientView.m_btStart.ShowWindow(TRUE);
					m_GameClientView.m_btStart.SetFocus();

					//设置时间
					SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
				}
				//设置扑克
				m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag(true);

				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
			}
			
			m_GameClientView.m_lLastTurnScore[ME_VIEW_CHAIR]=pStatusFree->lLastTurnScore;
			m_GameClientView.m_lAllTurnScore[ME_VIEW_CHAIR]=pStatusFree->lAllTurnScore;

			//设置界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);
			m_GameClientView.RefreshGameView();

			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertSystemString(TEXT("游戏结束,对方手中剩余1～6张为平扣，7～11张为单扣，12张以上为双扣！"));

			return true;
		}
	case GS_SK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;
			//变量定义
			WORD wMeChairID=GetMeChairID();
			//初始变量
			m_bLastTurn=false;
			ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
			memset(m_cbOutCardCount,255,sizeof(m_cbOutCardCount));
			//设置变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			//出牌变量
			m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克数据
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(&m_LastOutCard,&(pStatusPlay->LastOutCard),sizeof(tagAnalyseResult));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
			
			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);
			m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0xFFFF);
	
			//设置扑克
			BYTE cbCardData[MAX_COUNT];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			
			m_wTimeOutCount=0;
			m_bStustee=pStatusPlay->bTrustee[GetMeChairID()];
			
			//设置扑克
			for (BYTE cbIndex=0;cbIndex<GAME_PLAYER;cbIndex++)
			{
				//设置牌面
				WORD wViewChairID=SwitchViewChairID(cbIndex);
				//设置牌数
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->cbHandCardCount[cbIndex]);
				//积分设置
				m_GameClientView.m_lAllTurnScore[wViewChairID]=pStatusPlay->lAllTurnScore[cbIndex];
				m_GameClientView.m_lLastTurnScore[wViewChairID]=pStatusPlay->lLastTurnScore[cbIndex];	

				if (wViewChairID!=ME_VIEW_CHAIR || !IsAllowLookon())
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,m_cbHandCardCount[cbIndex]);
				else 
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[cbIndex]);
				//托管设置
				m_GameClientView.SetTrustee(SwitchViewChairID(cbIndex),pStatusPlay->bTrustee[cbIndex]);
				//出牌列表
				m_cbOutCardCount[cbIndex][0]=pStatusPlay->cbOutCardCount[cbIndex];
				CopyMemory(m_cbOutCardData[cbIndex][0],pStatusPlay->cbOutCardData[cbIndex],sizeof(BYTE)*m_cbOutCardCount[cbIndex][0]);
			}
			//玩家设置
			if (!IsLookonMode())
			{
				if(pStatusPlay->wCurrentUser==GetMeChairID())
				{
					m_GameClientView.m_btOutCard.EnableWindow(FALSE);
					m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
					m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
					m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount!=0)?TRUE:FALSE);
				}
					//托管按钮
				m_GameClientView.m_btStusteeControl.SetButtonImage((m_bStustee==true)?IDB_BT_STOP_TRUSTEE:IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively(true);
				m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag(true);
			}
			//桌面设置
			if (m_cbTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wTurnWiner);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbTurnCardData,m_cbTurnCardCount);
			}
			//设置定时器
			SetGameClock(pStatusPlay->wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			//显示提示信息
			CString strTemp;
			strTemp.Format(_T("当前单元积分:平扣:%I64d--单扣:%I64d--双扣:%I64d"),pStatusPlay->lCellScore[0],pStatusPlay->lCellScore[1],pStatusPlay->lCellScore[2]);
			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertSystemString(strTemp);

			return true;
		}
	}
	return true;
}


bool  CGameClientEngine::OnTimerDispatchCard()
{
	//派发控制
	if (m_cbRemnantCardCount>0)
	{
		//扑克数据
		BYTE cbUserCard[MAX_COUNT];
		ZeroMemory(cbUserCard,sizeof(cbUserCard));

		m_cbRemnantCardCount--;
		m_cbDispatchCardCount++;
		//设置界面
		WORD wViewChairID=0;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			wViewChairID=SwitchViewChairID(i);
			if((m_wGetRandCardID==i)&&(m_cbGetCardPosition==m_cbDispatchCardCount))
			{	
				BYTE cbRandCard[]={m_cbRandCardData};
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbRandCard,1);
			}
			if (wViewChairID==ME_VIEW_CHAIR)
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount); 
			else
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbUserCard,m_cbDispatchCardCount);				
		}
		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	//中止判断
	if (m_cbRemnantCardCount==0)
	{
		//删除定时器
		KillTimer(IDI_DISPATCH_CARD);
		//扑克数目
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.SetCardCount(SwitchViewChairID(i),m_cbHandCardCount[i]);
		}
		//当前玩家
		if (!IsLookonMode())
		{
			if(m_wCurrentUser==GetMeChairID())
			{
				ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btHunter.ShowWindow(m_bReqHumter?SW_SHOW:SW_HIDE);
			}
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_cbHandCardData,MAX_COUNT);
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively(IsLookonMode()==false);
		}
		//播放剩余
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	}
	//更新界面
	m_GameClientView.RefreshGameView();
	return true;
}


bool CGameClientEngine::OnCanReqHunter(const BYTE cbCardData[],BYTE cbCardCount)
{
	bool bReturn=false;
	tagAnalyseResult AnalyseResult;
	ZeroMemory(&AnalyseResult,sizeof(tagAnalyseResult));
	m_GameLogic.AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	bool bHaveBomb=false;
	for(BYTE cbIndex=0;cbIndex<13;cbIndex++)
	{
		if(AnalyseResult.m_CardStyle[cbIndex].m_cbCount>=4)
		{
			bHaveBomb=true;
			break;
		}
	}
	//如果无炸弹且王的个数大于等于三个则有权进行抄底
	if(!bHaveBomb&&AnalyseResult.m_nJockerCount>=3) bReturn=true;
	return bReturn;
}
//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	m_wBankerUser=pGameStart->wCurrentUser;
	//游戏变量
	m_wCurrentUser=pGameStart->wCurrentUser;
	//游戏变量
	m_wTimeOutCount=0;
	m_cbTurnCardCount=0;
	//开始信息
	m_cbRandCardData=pGameStart->cbStartRandCard;
	m_wGetRandCardID=pGameStart->wGetRandCardID;
	m_cbGetCardPosition=pGameStart->cbGetCardPosition;
	//胜利信息
	m_wWinOrder=0xFFFF;
	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//上轮扑克
	m_bLastTurn=false;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	memset(m_cbOutCardCount,255,sizeof(m_cbOutCardCount));


	//玩家扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		if (wViewChairID==ME_VIEW_CHAIR)
		{
			bool bShowCard=((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetPositively(false);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(bShowCard);
		}
		else
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
		}
	}


	//设置扑克
	WORD wMeChairID=GetMeChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_cbHandCardCount[i]=CountArray(pGameStart->cbCardData);
		m_GameClientView.SetCardCount(i,m_cbHandCardCount[i]);
	}
	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
	//是否可以抄底
	m_bReqHumter=OnCanReqHunter(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
	//派发扑克
	DispatchUserCard(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
	
	//托管设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetTrustee(SwitchViewChairID(i),pGameStart->bTrustee[i]);
	}
	//更新界面
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0xFFFF);
	//更新界面
	m_GameClientView.RefreshGameView();
	//环境设置
	if ((IsLookonMode()==false)) ActiveGameFrame(); 
	//播放声音 
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
 	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//显示提示信息
	CString strTemp;
	strTemp.Format(_T("当前单元积分:平扣:%I64d--单扣:%I64d--双扣:%I64d"),pGameStart->lCellScore[0],pGameStart->lCellScore[1],pGameStart->lCellScore[2]);
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString(strTemp);

	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if(wDataSize!=sizeof(CMD_S_OutCard)) return false;
 	//变量定义
 	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	//停止发牌
	DoncludeDispatchCard();

 	//删除定时器
 	KillTimer(IDI_WAIT_CLEAR);
 	KillGameClock(IDI_OUT_CARD);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser); 
	bool bPlayGameSound=((IsLookonMode()==false)&&(wMeChairID==pOutCard->wOutCardUser));


	//历史清理
	if (m_cbTurnCardCount==0)
	{
		//用户扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//桌面清理
			if (i!=pOutCard->wOutCardUser)
			{
				if(m_cbOutCardCount[i][0]!=255)
				{
					//保存扑克
					m_cbOutCardCount[i][1]=m_cbOutCardCount[i][0];
					CopyMemory(m_cbOutCardData[i][1],m_cbOutCardData[i][0],sizeof(BYTE)*m_cbOutCardCount[i][0]);
				}
				//清理扑克
				m_cbOutCardCount[i][0]=255;
				ZeroMemory(m_cbOutCardData[i][0],sizeof(BYTE)*MAX_COUNT);

				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}
		//放弃标志
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//当前界面
	SwitchToCurrentCard();
	
	//出牌变量
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_cbTurnCardCount=pOutCard->cbCardCount;
 	CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,sizeof(BYTE)*pOutCard->cbCardCount);	

	//获取牌型
	tagAnalyseResult analyseResult;
	memset(&analyseResult,0,sizeof(analyseResult));
	m_GameLogic.AnalysebCardData(pOutCard->cbCardData,pOutCard->cbCardCount,analyseResult);
	m_GameLogic.FindValidCard(analyseResult,m_LastOutCard);
	memcpy(&m_LastOutCard,&analyseResult,sizeof(tagAnalyseResult));

	//炸弹判断
	if(m_LastOutCard.m_cbCardType>=CT_BOMB)
	{
		//播放动画
		m_GameClientView.SetBombEffect(true);
	}
	
	//播放声音
	if ((bPlayGameSound==false)&&((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser)))
	{
		if (m_LastOutCard.m_cbCardType>=CT_BOMB)
		{
			//播放声音
			if (bPlayGameSound==false)
			{
				bPlayGameSound=true;
				PlayBombSound(pOutCard->wOutCardUser,m_LastOutCard.m_cbBombGrade);
			}
		}
		else
		{
			if (bPlayGameSound==false)
			{
				bPlayGameSound=true;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
			}
		}
	}

 
	//上轮扑克
	if(m_cbOutCardCount[pOutCard->wOutCardUser][0]!=255)
	{
		//出牌记录
		m_cbOutCardCount[pOutCard->wOutCardUser][1]=m_cbOutCardCount[pOutCard->wOutCardUser][0];
		if(m_cbOutCardCount[pOutCard->wOutCardUser][0]!=0)
			CopyMemory(m_cbOutCardData[pOutCard->wOutCardUser][1],m_cbOutCardData[pOutCard->wOutCardUser][0],sizeof(BYTE)*m_cbOutCardCount[pOutCard->wOutCardUser][0]);
		else
			ZeroMemory(m_cbOutCardData[pOutCard->wOutCardUser][1],MAX_COUNT*sizeof(BYTE));

	}
	m_cbOutCardCount[pOutCard->wOutCardUser][0]=pOutCard->cbCardCount;
	CopyMemory(m_cbOutCardData[pOutCard->wOutCardUser][0],pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));
	

	//出牌设置
	if ((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(pOutCard->cbCardData,pOutCard->cbCardCount);
		//删除扑克
		if (wViewChairID==ME_VIEW_CHAIR)
		{
			//删除扑克
			BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
		}
		else
		{
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			//设置扑克
			BYTE cbCardCount[MAX_COUNT];
			ZeroMemory(cbCardCount,sizeof(cbCardCount));
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardCount,m_cbHandCardCount[pOutCard->wOutCardUser]);
		}
	}

	m_GameClientView.SetCardCount(wViewChairID,m_cbHandCardCount[pOutCard->wOutCardUser]);

	//最大判断
	if (m_wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_cbTurnCardCount=0;
		m_wCurrentUser=INVALID_CHAIR;
		m_wMostUser=pOutCard->wCurrentUser;
		ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
		//放弃动作
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//用户过虑
			if (pOutCard->wOutCardUser==i) continue;
			//放弃界面
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.SetPassFlag(wViewChairID,true);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}

		//播放声音
		if (bPlayGameSound==false)
		{
			bPlayGameSound=true;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
		}
		//设置定时器
		SetTimer(IDI_WAIT_CLEAR,3000,NULL);
		return true;
	}

	//玩家设置
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

		//上轮扑克
		if(m_cbOutCardCount[m_wCurrentUser][0]!=255)
		{
			//出牌记录
			m_cbOutCardCount[m_wCurrentUser][1]=m_cbOutCardCount[m_wCurrentUser][0];
			if(m_cbOutCardCount[m_wCurrentUser][0]!=0)
				CopyMemory(m_cbOutCardData[m_wCurrentUser][1],m_cbOutCardData[m_wCurrentUser][0],sizeof(BYTE)*m_cbOutCardCount[m_wCurrentUser][0]);
			else
				ZeroMemory(m_cbOutCardData[m_wCurrentUser][1],MAX_COUNT*sizeof(BYTE));
		}
		m_cbOutCardCount[m_wCurrentUser][0]=255;
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		if (m_bStustee==false) ActiveGameFrame();
		BYTE cbCardCount=m_cbHandCardCount[pOutCard->wCurrentUser];
		if(cbCardCount==1&&pOutCard->cbCardCount>=2)
		{
			m_GameClientView.m_btPassCard.EnableWindow(TRUE);
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutCard.EnableWindow(FALSE);
			SetGameClock(m_wCurrentUser,IDI_PASS_CARD,TIME_PASS_CARD);
		}
		else
		{
			m_GameClientView.m_btPassCard.EnableWindow(TRUE);
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
		}
	}
	else
	{
		if (pOutCard->wCurrentUser!=INVALID_CHAIR)
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
	}

	//控制按钮
	m_GameClientView.m_btLastTurnCard.EnableWindow(TRUE);

	return true;
}

//放弃出牌
bool CGameClientEngine::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameClock(IDI_OUT_CARD);

	//设置变量
	m_wCurrentUser=pPassCard->wCurrentUser;

	//放弃设置
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
	{
		WORD wPassCardUser=pPassCard->wPassCardUser;
		m_GameClientView.SetPassFlag(SwitchViewChairID(wPassCardUser),true);
	}

	//上轮扑克
	if(m_cbOutCardCount[pPassCard->wPassCardUser][0]!=255)
	{
		//出牌记录
		m_cbOutCardCount[pPassCard->wPassCardUser][1]=m_cbOutCardCount[pPassCard->wPassCardUser][0];
		if(m_cbOutCardCount[pPassCard->wPassCardUser][0]!=0)
			CopyMemory(m_cbOutCardData[pPassCard->wPassCardUser][1],m_cbOutCardData[pPassCard->wPassCardUser][0],sizeof(BYTE)*m_cbOutCardCount[pPassCard->wPassCardUser][0]);
		else
			ZeroMemory(m_cbOutCardData[pPassCard->wPassCardUser][1],MAX_COUNT*sizeof(BYTE));
	}
	m_cbOutCardCount[pPassCard->wPassCardUser][0]=0;
	ZeroMemory(m_cbOutCardData[pPassCard->wPassCardUser][0],MAX_COUNT*sizeof(BYTE));
	if(m_wCurrentUser != INVALID_CHAIR)
	{
		//视图位置
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		//设置界面
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		//上轮扑克
		if(m_cbOutCardCount[m_wCurrentUser][0]!=255)
		{
			//出牌记录
			m_cbOutCardCount[m_wCurrentUser][1]=m_cbOutCardCount[m_wCurrentUser][0];
			if(m_cbOutCardCount[m_wCurrentUser][0]!=0)
				CopyMemory(m_cbOutCardData[m_wCurrentUser][1],m_cbOutCardData[m_wCurrentUser][0],sizeof(BYTE)*m_cbOutCardCount[m_wCurrentUser][0]);
			else
				ZeroMemory(m_cbOutCardData[m_wCurrentUser][1],MAX_COUNT*sizeof(BYTE));
		}
		m_cbOutCardCount[m_wCurrentUser][0]=255;
	}

	//一轮判断
	if (pPassCard->cbTurnOver==TRUE)
	{
		//出牌变量
		m_cbTurnCardCount=0;
		ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
		//清除界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		ActiveGameFrame();
		//设置按钮
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btHunter.ShowWindow(m_bReqHumter?SW_SHOW:SW_HIDE);
	}
	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

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
	//设置状态
	SetGameStatus(GS_SK_FREE);
	//停止发牌
	DoncludeDispatchCard();
	//删除定时器
	KillTimer(IDI_WAIT_CLEAR);
	KillGameClock(IDI_OUT_CARD);
	KillTimer(IDI_DISPATCH_CARD);
	
	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btHunter.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btLastTurnCard.EnableWindow(FALSE);

	//当前界面
	SwitchToCurrentCard();

	//设置积分
	CString strTemp=_T("结果统计:");
	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pClientUserItem->GetNickName(),pGameEnd->lGameScore[i]);

		if(pGameEnd->lGameScore[i]>0)
			strTemp.AppendFormat(_T("\n--%s: %+I64d\n"),pClientUserItem->GetNickName(),pGameEnd->lGameScore[i]);
		else
			strTemp.AppendFormat(_T("\n--%s: %I64d\n"),pClientUserItem->GetNickName(),pGameEnd->lGameScore[i]);

		if(pGameEnd->lBombScore[i]>0)
			strTemp.AppendFormat(_T("----算得贡献得分为:%+I64d\n"),pGameEnd->lBombScore[i]);
		else
			strTemp.AppendFormat(_T("----算得贡献得分为:%I64d\n"),pGameEnd->lBombScore[i]);

		strTemp+=_T("----其中炸弹情况如下:\n");
		for(BYTE cbIndex=0;cbIndex<BOMB_TYPE_COUNT;cbIndex++)
		{
			if(pGameEnd->cbBombList[i][cbIndex]>0)
				strTemp.AppendFormat(_T("----[%d项]X%d\n"),cbIndex+4,pGameEnd->cbBombList[i][cbIndex]);
		}
	}

	//消息积分
 	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString((LPCTSTR)strTemp);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag(true);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONGLONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];	
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively(false);

	//取消托管
	if(m_bStustee)
		OnStusteeControl(0,0);
	
	//取消排序
	m_bSortCount=false;

	//设置界面
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_lAllTurnScore[wViewChairID]=pGameEnd->lAllTurnScore[i];
		m_GameClientView.m_lLastTurnScore[wViewChairID]=pGameEnd->lLastTurnScore[i];
	}
	//开始信息
	m_wGetRandCardID=INVALID_CHAIR;
	m_cbGetCardPosition=0xFF;
	m_cbRandCardData=0xFF;

	return true;
}

//用户托管
bool CGameClientEngine::OnSubTrustee(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;
	//消息处理
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;
	m_GameClientView.SetTrustee(SwitchViewChairID(pTrustee->wChairID),pTrustee->bTrustee);
	if ((IsLookonMode()==true)||(pTrustee->wChairID!=GetMeChairID()))
	{
		IClientUserItem * pClientUserItem=GetTableUserItem(pTrustee->wChairID);
		TCHAR szBuffer[256];
		if(pTrustee->bTrustee==true)
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]选择了托管功能."),pClientUserItem->GetNickName());
		else
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]取消了托管功能."),pClientUserItem->GetNickName());
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertSystemString(szBuffer);
	}
	return true;
}


//出牌判断
bool CGameClientEngine::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;
	if (m_wCurrentUser!=GetMeChairID()) return false;

	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(cbCardData,CountArray(cbCardData));
	//出牌判断
	if (cbShootCount>0L)
	{
		memset(&m_OutCardInfo,0,sizeof(m_OutCardInfo));
		m_GameLogic.AnalysebCardData(cbCardData,cbShootCount,m_OutCardInfo);
		return m_GameLogic.CompareCard(m_OutCardInfo,m_LastOutCard);
	}
	return false;
}

//放弃判断
bool CGameClientEngine::VerdictPassCard()
{
	//首出判断
	if (m_cbTurnCardCount==0) return false;


	return true;
}

//自动出牌
bool CGameClientEngine::AutomatismOutCard()
{
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return false;

	//当前弹起
	CSkinButton & btOutCard=m_GameClientView.m_btOutCard;
	if ((btOutCard.IsWindowEnabled())&&(btOutCard.IsWindowVisible()))
	{
		OnOutCard(0,0);
		return true;
	}

	//出牌处理
	if ((m_cbTurnCardCount==0))
	{
		//分析结果
		tagOutCardResult OutCardResult;
		m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbTurnCardData,m_cbTurnCardCount,OutCardResult);

		//设置界面
		if (OutCardResult.cbCardCount>0)
		{
			//设置界面
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

			//出牌动作
			OnOutCard(0,0);

			return true;
		}
	}

	//放弃出牌
	if (m_cbTurnCardCount>0) 
		OnPassCard(0,0);

	return true;
}

//停止发牌
bool CGameClientEngine::DoncludeDispatchCard()
{
	//停止发牌
	if (m_cbRemnantCardCount>0)
	{
		//删除时间
		KillTimer(IDI_DISPATCH_CARD);

		//扑克数据
		BYTE cbUserCard[MAX_COUNT];
		ZeroMemory(cbUserCard,sizeof(cbUserCard));

		//设置界面
		WORD wViewChairID=0;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			wViewChairID=SwitchViewChairID(i);
			if (wViewChairID ==ME_VIEW_CHAIR)
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
			else
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbUserCard,MAX_COUNT);
		}
		//设置变量
		m_cbRemnantCardCount=0;
		m_cbDispatchCardCount=0;
		ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));
	}
	return true;
}

//派发扑克
bool CGameClientEngine::DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置变量
	m_cbDispatchCardCount=0;
	m_cbRemnantCardCount=cbCardCount;
	CopyMemory(m_cbDispatchCardData,cbCardData,sizeof(BYTE)*cbCardCount);
	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,100,NULL);
	return true;
}

//播放出牌声音
void CGameClientEngine::PlayBombSound(WORD wChairID, BYTE cbStarLevel)
{
    	//判断性别
		IClientUserItem * pClientUserItem=GetTableUserItem(wChairID);
		if(pClientUserItem==0||cbStarLevel<4||cbStarLevel>12)	return;
		CString strSoundName;
		strSoundName.Format(_T("%d"),cbStarLevel);
		if(pClientUserItem->GetGender()!=1)
			strSoundName = _T("BOY_") +strSoundName;
		else
			strSoundName = _T("GIRL_") + strSoundName;
		PlayGameSound(AfxGetInstanceHandle(), strSoundName);
	
}

//当前扑克
bool CGameClientEngine::SwitchToCurrentCard()
{
	//处理判断
	if (m_bLastTurn==false) return false;

	//环境设置
	m_bLastTurn=false;
	KillTimer(IDI_LAST_TURN_CARD);
	m_GameClientView.SetLastTurnCard(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if(m_cbOutCardCount[i][0]!=255)
		{
			if(m_cbOutCardCount[i][0]==0)
			{
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
				m_GameClientView.SetPassFlag(wViewChairID,true);
			}
			else
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][0],m_cbOutCardCount[i][0]);
		}	
		else
		{
			m_GameClientView.SetPassFlag(wViewChairID,false);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
	}

	return true;
}

//定时器消息
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
		case IDI_WAIT_CLEAR:
			{
				//变量定义
				WORD wCurrentUser=m_wMostUser;
				m_wMostUser=INVALID_CHAIR;
				//删除定时器
				KillTimer(IDI_WAIT_CLEAR);
				//设置界面
				m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
				for (WORD i=0;i<GAME_PLAYER;i++) 
					m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

				//玩家设置
				if ((IsLookonMode()==false)&&(wCurrentUser==GetMeChairID()))
				{
					ActiveGameFrame();
					m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
					m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
					m_GameClientView.m_btPassCard.EnableWindow(FALSE);
					m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
					m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
				}
				//设置时间
				SetGameClock(wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
				break;
			}
		case IDI_DISPATCH_CARD: OnTimerDispatchCard();break;
		case IDI_LAST_TURN_CARD:SwitchToCurrentCard();break;
		default:break;
	}
	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{   

	//胜利信息
	m_wWinOrder=0xFFFF;
	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;
	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//设置界面
	KillGameClock(IDI_START_GAME);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0xFFFF);

	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//隐藏控件
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}
	m_GameClientView.m_HandCardControl[i].SetPositively(false);

	//发送消息
	SendUserReady(NULL,0);
	return 0;
}

//出牌消息
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	m_bReqHumter=false;
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameClock(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btHunter.ShowWindow(SW_HIDE);
	
	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(cbCardData,CountArray(cbCardData));

	//排列扑克
	m_GameLogic.SortCardList(cbCardData,cbCardCount);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];


	//炸弹判断
	if (m_OutCardInfo.m_cbCardType>=CT_BOMB)
		PlayBombSound(GetMeChairID(),m_OutCardInfo.m_cbBombGrade);
	else  
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(cbCardData,cbCardCount);


	//删除扑克
	m_cbHandCardCount[wMeChairID]-=cbCardCount;
	m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData,cbSourceCount);
	BYTE cbHandCardData[MAX_COUNT];
	CopyMemory(cbHandCardData,m_cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
	m_GameLogic.SortCardList(cbHandCardData,m_cbHandCardCount[wMeChairID]);
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=cbCardCount;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount);
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(CMD_C_OutCard));

	//更新界面
	m_GameClientView.RefreshGameView();
	return 0;
}

//放弃出牌
LRESULT CGameClientEngine::OnPassCard(WPARAM wParam, LPARAM lParam)
{

	m_bReqHumter=false;
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowVisible()==FALSE) return 0;
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameClock(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btHunter.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//发送数据
	SendSocketData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(NULL,0);
	m_GameClientView.SetPassFlag(ME_VIEW_CHAIR,true);

	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	return 0;
}

//出牌提示
LRESULT CGameClientEngine::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult={0};
//	m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbMagicCardData,m_cbTurnCardCount,OutCardResult);

	//设置界面
	if (OutCardResult.cbCardCount>0)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		//更新界面
		m_GameClientView.RefreshGameView();
	}
	else 
	{
		//放弃出牌
		OnPassCard(0,0);
	}
	return 0;
}


LRESULT CGameClientEngine::OnReqHunter(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_REQ_HUNTERS,NULL,0);
	return 1;
}

//数目排序
LRESULT CGameClientEngine::OnSortByCount(WPARAM wParam, LPARAM lParam)
{
	//设置状态
	if (m_bSortCount==true)
	{
		m_bSortCount=false;
//		m_GameClientView.m_btSortByCount.SetButtonImage(IDB_BT_SORT_BY_COUNT,AfxGetInstanceHandle(),false);
	}
	else
	{
		m_bSortCount=true;
//		m_GameClientView.m_btSortByCount.SetButtonImage(IDB_BT_SORT_BY_ORDER,AfxGetInstanceHandle(),false);
	}
	if (m_cbHandCardCount[GetMeChairID()]>0)
	{
		BYTE cbHandCardData[MAX_COUNT];
		CopyMemory(cbHandCardData,m_cbHandCardData,sizeof(m_cbHandCardData));
		if(m_bSortCount)
			m_GameLogic.SortCardList(cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
		else
			m_GameLogic.SortCardList(cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(cbHandCardData,m_cbHandCardCount[GetMeChairID()]);
		
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SORT_CARD"));
	}
	return 0;
}


//右键扑克
LRESULT CGameClientEngine::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);
	//BYTE cbCardData[MAX_COUNT];
	//ZeroMemory(cbCardData,sizeof(cbCardData));
	//WORD byCardCount = m_GameClientView.m_HandCardControl[2].GetShootCard(cbCardData,MAX_COUNT);
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("SELECT"));

	////弹起顺子
	//if( byCardCount == 2 )
	//{
	//	tagOutCardResult OutCardResult;
	//	if( m_GameLogic.SearchLinkCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],cbCardData[0],cbCardData[1],OutCardResult))
	//	{
	//		m_GameClientView.m_HandCardControl[2].ShootAllCard(false);
	//		m_GameClientView.m_HandCardControl[2].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);
	//	}
	//}

	////设置控件
	//bool bOutCard=VerdictOutCard();
	//m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);
 //   	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	return 0;
}

//左键扑克
LRESULT CGameClientEngine::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);
	return 0;
}
//拖管控制
LRESULT CGameClientEngine::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_wTimeOutCount=0;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertSystemString(_T("您取消了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));

	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertSystemString(_T("您选择了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}
	return 0;
}


//类型提示
LRESULT CGameClientEngine::OnCardType(WPARAM wParam,LPARAM lParam)
{
	
	return 0;
}
//上轮扑克
LRESULT CGameClientEngine::OnLastTurnCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID=SwitchViewChairID(i);
			if(m_cbOutCardCount[i][1]!=255)
			{
				if(m_cbOutCardCount[i][1]==0)
				{
					m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
					m_GameClientView.SetPassFlag(wViewChairID,true);
				}
				else
					m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][1],m_cbOutCardCount[i][1]);
			}
			else
			{
				m_GameClientView.SetPassFlag(wViewChairID,false);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//环境设置
		m_bLastTurn=true;
		m_GameClientView.SetLastTurnCard(true);
		m_GameClientView.RefreshGameView();
		SetTimer(IDI_LAST_TURN_CARD,1000,NULL);
	}
	else SwitchToCurrentCard();
	return 0;
}


//LRESULT CGameClientEngine::OnVoiceCreateFail(WPARAM wParam, LPARAM lParam)
//{
//	//m_pGlobalUnits->m_bAllowSound=false;
//	m_bHaveVoiceCard=false;
//	return true;
//}

//////////////////////////////////////////////////////////////////////////
