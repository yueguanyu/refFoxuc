#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////
//时间标识
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_CALL_CARD					201								//叫牌定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_SEND_CONCEAL				203								//底牌定时器
#define IDI_LAND_SCORE					204								//叫分定时器

//时间范围
#define TIME_START_GAME					30								//开始时间
#define TIME_SEND_CONCEAL				50								//留底时间


//#ifdef DEBUG
//#define TIME_OUT_CARD					300								//出牌时间
//#define TIME_CALL_CARD					300								//叫牌时间
//#define TIME_LAND_SCORE					300								//叫牌时间
//#else
#define TIME_OUT_CARD					20								//出牌时间
#define TIME_CALL_CARD					15								//叫牌时间
#define TIME_LAND_SCORE					20								//叫牌时间
//#endif

//功能定时器
#define IDI_CONCEAL_CARD				300								//底牌定时器
#define IDI_DISPATCH_CARD				301								//发牌定时器
#define IDI_LAST_TURN_CARD				302								//上轮定时器
#define IDI_THROW_CARD_RESULT			303								//甩牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CALL_CARD,OnCallCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_SEND_CONCEAL,OnSendConceal)
	ON_MESSAGE(IDM_CONCEAL_CARD,OnConcealCard)
	//ON_MESSAGE(IDM_REQUEST_LEAVE,OnRuquestLeave)
	ON_MESSAGE(IDM_LAST_TURN_CARD,OnLastTurnCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)

	ON_MESSAGE(IDM_LAND_SCORE,OnLandScore)
	ON_MESSAGE(IDM_GIVEUP_GAME,OnGiveUpGame)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
LRESULT CGameClientEngine::OnGiveUpGame(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	SendSocketData(SUB_C_GIVEUP_GAME);
	return 0;
}


//构造函数
CGameClientEngine::CGameClientEngine()
{
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;
	m_lCellScore=1;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bUserNoMain,sizeof(m_bUserNoMain));

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//甩牌变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));
	m_GameLogic.SetCanThrow(false);

	//出牌变量
	m_bLastTurn=false;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//配置变量
	m_bStustee=false;
	m_bDeasilOrder=false;
	m_dwCardHSpace=18;//DEFAULT_PELS;
	m_bOverTimes=0;

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	//配置控件
	m_GameClientView.SetUserOrder(m_bDeasilOrder);
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,DEF_CARD_SHOOT);

	return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;
	m_GameLogic.SetCanThrow(false);

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bUserNoMain,sizeof(m_bUserNoMain));

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));
	m_GameClientView.SetCardScore(0);
	m_GameClientView.SetPlayerScore(0);
	m_GameClientView.SetMainCount(0,0);

	//甩牌变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));

	//出牌变量
	m_bLastTurn=false;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//配置变量
	m_bStustee=false;
	m_bOverTimes=0;

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//删除定时器
	KillTimer(IDI_CONCEAL_CARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAST_TURN_CARD);
	KillTimer(IDI_THROW_CARD_RESULT);
	return true;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}

//时钟信息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_LAND_SCORE:
		{
			//超时判断
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_LAND_SCORE))))
			{
				KillGameClock(IDI_LAND_SCORE);
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnLandScore(255,255);
				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;		
		}
	case IDI_OUT_CARD:			//用户出牌
		{
#ifdef DEBUG
			if (m_bStustee==true)
#else
			//超时判断
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_OUT_CARD))))
#endif
			{
				//if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) m_bOverTimes++;
				//超时5次
				//if(m_bOverTimes>=5) SendSocketData(SUB_C_EJECT);
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					AutomatismOutCard();
				}
				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_CALL_CARD:			//用户叫牌
		{
			//超时判断
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_CALL_CARD))))
			{
				//隐藏控件
				m_GameClientView.m_CallCardWnd.ShowWindow(SW_HIDE);

				//发送消息
				if (IsLookonMode()==false) 
				{
					//随机叫牌
					BYTE color[]={0x00,0x10,0x20,0x30};
					srand((DWORD)time(NULL));
					int randColor=rand()%4;

					//构造数据
					CMD_C_CallCard CallCard;
					CallCard.cbCallCard=color[randColor]+0x02;
					CallCard.cbCallCount=1;

					//发送数据
					SendSocketData(SUB_C_CALL_CARD,&CallCard,sizeof(CallCard));
					KillGameClock(IDI_CALL_CARD);
				}

				return false;
			}

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_SEND_CONCEAL:		//发送底牌
		{
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_SEND_CONCEAL))))
			{
				//自动留底
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
				{
					//删除定时器
					KillGameClock(IDI_SEND_CONCEAL);

					//删除扑克
					m_cbHandCardCount-=m_cbConcealCount;
					m_GameLogic.RemoveCard(m_cbConcealCard,m_cbConcealCount,m_cbHandCardData,m_cbHandCardCount+m_cbConcealCount);

					//设置界面
					m_GameClientView.m_btSendConceal.ShowWindow(SW_HIDE);
					m_GameClientView.m_btSendConceal.EnableWindow(FALSE);
					SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);

					//构造数据
					CMD_C_ConcealCard ConcealCard;
					ConcealCard.cbConcealCount=m_cbConcealCount;
					CopyMemory(ConcealCard.cbConcealCard,m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));

					//发送数据
					SendSocketData(SUB_C_CONCEAL_CARD,&ConcealCard,sizeof(ConcealCard));

					//提示信息
					if (m_bStustee==false && m_pIStringMessage!=NULL)
						m_pIStringMessage->InsertSystemString(TEXT("限定的时间到了，系统为您自动埋底牌"));

					return false;
				}

				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//扑克控制
	m_GameClientView.m_HandCardControl.SetDisplayFlag((IsAllowLookon()==true));

	//功能按钮
	m_GameClientView.m_btConcealCard.EnableWindow(((m_cbConcealCount>0)&&((IsLookonMode()==false)||(IsAllowLookon()==true)))?TRUE:FALSE);

	return true;
}

//游戏消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_LAND_SCORE:		//用户叫分
		{
			return OnSubLandScore(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_CALL_CARD:		//用户叫牌
		{
			return OnSubCallCard(pData,wDataSize);
		}
	case SUB_S_SEND_CONCEAL:	//发送底牌
		{
			return OnSubSendConceal(pData,wDataSize);
		}
	case SUB_S_GAME_PLAY:		//游戏开始
		{
			return OnSubGamePlay(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_THROW_RESULT:	//甩牌结果
		{
			return OnSubThrowResult(pData,wDataSize);
		}
	case SUB_S_TURN_BALANCE:	//一轮结算
		{
			return OnSubTurnBalance(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_GM_CARD:
		{
			return OnSubGMCard(pData,wDataSize);
		}
	}
	return true;
}

//场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_UG_SCORE:
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

			m_wCurrentUser=pStatusScore->wCurrentUser;
			m_wBankerUser=pStatusScore->wBankerUser;

			CopyMemory(m_cbConcealCard,pStatusScore->cbBackCardData,sizeof(pStatusScore->cbBackCardData));
			CopyMemory(m_cbHandCardData,pStatusScore->cbCardData,sizeof(pStatusScore->cbCardData));

			m_cbHandCardCount=25;
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
			m_GameClientView.m_BackCardControl.SetCardData(m_cbConcealCard,10);
			m_GameClientView.m_HandCardControl.SetCardData(m_cbHandCardData,m_cbHandCardCount);

			//玩家设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
				m_GameClientView.m_BackCardControl.SetDisplayFlag(false);
			}

			//设置控件
			if (IsLookonMode()==false&&pStatusScore->wCurrentUser==GetMeChairID())
			{
				m_GameClientView.m_CallScoreView.ShowWindow(SW_SHOW);
				m_GameClientView.SetCurrentCallScore(pStatusScore->bLandScore);
			}

			//时间设置
			SetGameClock(m_wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);
			//GM帐号
			if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
			{
				SendSocketData(SUB_C_GM_REQUEST);
			}
			return true;

		}
	case GS_UG_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;



			m_lCellScore=pStatusFree->lBaseScore;

			//玩家设置
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

			}
			//设置控件
			if (IsLookonMode()==false && GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}
			return true;
		}
	case GS_UG_CALL:	//叫牌状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;

			m_lCellScore=pStatusCall->lBaseScore;

			//逻辑变量
			m_cbPackCount=pStatusCall->cbPackCount;
			m_cbMainValue=pStatusCall->cbMainValue;
			m_wBankerUser=pStatusCall->wBankerUser;

			//叫牌信息
			m_cbCallCard=pStatusCall->cbCallCard;
			m_cbCallCount=pStatusCall->cbCallCount;
			m_wCallCardUser=pStatusCall->wCallCardUser;

			//设置扑克
			m_cbHandCardCount=pStatusCall->cbCardCount;
			CopyMemory(m_cbHandCardData,pStatusCall->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

			//逻辑组件
			m_GameLogic.SetPackCount(m_cbPackCount);
			m_GameLogic.SetMainColor(m_cbMainColor);
			m_GameLogic.SetMainValue(m_cbMainValue);

			//主牌信息
			WORD wMeChairID=GetMeChairID();
			BYTE cbValueMySelf=pStatusCall->cbValueOrder[wMeChairID%2];
			BYTE cbValueOther=pStatusCall->cbValueOrder[(wMeChairID+1)%2];
			m_GameClientView.SetValueOrder(m_cbMainValue,cbValueMySelf,cbValueOther);

			//叫牌信息
			if (m_wCallCardUser!=INVALID_CHAIR)
			{
				WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
				BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
				m_GameClientView.SetCallCardInfo(wViewChairID,cbCallColor,m_cbCallCount);
			}
			m_GameClientView.SetCurrentCallScore(pStatusCall->wLandScore);

			//叫牌界面
			BYTE cbCallCard[MAX_PACK];
			WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
			for (BYTE i=0;i<m_cbCallCount;i++) cbCallCard[i]=m_cbCallCard;
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCallCard,m_cbCallCount);

			//设置庄家
			if (m_wBankerUser!=INVALID_CHAIR) m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//玩家设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			//扑克设置
			if (pStatusCall->cbComplete==FALSE)
			{
				//控制设置
				m_GameClientView.m_CallCardWnd.ShowWindow(SW_SHOW);

				//派发扑克
				DispatchUserCard(m_cbHandCardData,m_cbHandCardCount);
			}
			else
			{
				//排列扑克
				m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

				//设置扑克
				SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
			}

			//GM帐号
			if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
			{
				SendSocketData(SUB_C_GM_REQUEST);
			}

			return true;
		}
	case GS_UG_BACK:	//留底状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusBack)) return false;
			CMD_S_StatusBack * pStatusBack=(CMD_S_StatusBack *)pData;

			//逻辑变量
			m_cbPackCount=pStatusBack->cbPackCount;
			m_cbMainColor=pStatusBack->cbMainColor;
			m_cbMainValue=pStatusBack->cbMainValue;
			m_lCellScore=pStatusBack->lBaseScore;

			//叫牌信息
			m_cbCallCard=pStatusBack->cbCallCard;
			m_cbCallCount=pStatusBack->cbCallCount;
			m_wCallCardUser=pStatusBack->wCallCardUser;

			//用户变量
			m_wBankerUser=pStatusBack->wBankerUser;
			m_wCurrentUser=pStatusBack->wCurrentUser;
			m_wCallCardUser=pStatusBack->wCallCardUser;

			//逻辑组件
			m_GameLogic.SetPackCount(m_cbPackCount);
			m_GameLogic.SetMainColor(m_cbMainColor);
			m_GameLogic.SetMainValue(m_cbMainValue);

			//设置底牌
			m_cbConcealCount=pStatusBack->cbConcealCount;
			CopyMemory(m_cbConcealCard,pStatusBack->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

			//设置扑克
			m_cbHandCardCount=pStatusBack->cbCardCount;
			CopyMemory(m_cbHandCardData,pStatusBack->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

			//主牌信息
			WORD wMeChairID=GetMeChairID();
			BYTE cbValueMySelf=pStatusBack->cbValueOrder[wMeChairID%2];
			BYTE cbValueOther=pStatusBack->cbValueOrder[(wMeChairID+1)%2];
			m_GameClientView.SetValueOrder(m_cbMainValue,cbValueMySelf,cbValueOther);

			//叫牌信息
			if (m_wCallCardUser!=INVALID_CHAIR)
			{
				WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
				BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
				m_GameClientView.SetCallCardInfo(wViewChairID,cbCallColor,m_cbCallCount);
			}
			m_GameClientView.SetCurrentCallScore(pStatusBack->wLandScore);

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

			//界面设置
			SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//玩家设置
			if (IsLookonMode()==false) 
			{
				//扑克设置
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

				//留底设置
				if (m_wCurrentUser==GetMeChairID())
				{
					m_GameClientView.m_btSendConceal.EnableWindow(FALSE);
					m_GameClientView.m_btSendConceal.ShowWindow(SW_SHOW);
					m_GameClientView.m_HandCardControl.SetPositively(true);
					m_GameClientView.m_HandCardControl.SetShootCard(m_cbConcealCard,m_cbConcealCount);
				}
				else m_GameClientView.SetWaitConceal(true);
			}
			else m_GameClientView.SetWaitConceal(true);

			//时间设置
			SetGameClock(m_wCurrentUser,IDI_SEND_CONCEAL,TIME_SEND_CONCEAL);

			//GM帐号
			if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
			{
				SendSocketData(SUB_C_GM_REQUEST);
			}

			return true;
		}
	case GS_UG_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//逻辑变量
			m_cbPackCount=pStatusPlay->cbPackCount;
			m_cbMainColor=pStatusPlay->cbMainColor;
			m_cbMainValue=pStatusPlay->cbMainValue;
			m_lCellScore=pStatusPlay->lBaseScore;

			//叫牌信息
			m_cbCallCard=pStatusPlay->cbCallCard;
			m_cbCallCount=pStatusPlay->cbCallCount;
			m_wCallCardUser=pStatusPlay->wCallCardUser;

			//用户变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_wFirstOutUser=pStatusPlay->wFirstOutUser;

			//逻辑组件
			m_GameLogic.SetPackCount(m_cbPackCount);
			m_GameLogic.SetMainColor(m_cbMainColor);
			m_GameLogic.SetMainValue(m_cbMainValue);

			//设置底牌
			m_cbConcealCount=pStatusPlay->cbConcealCount;
			CopyMemory(m_cbConcealCard,pStatusPlay->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

			//设置扑克
			m_cbHandCardCount=pStatusPlay->cbCardCount;
			CopyMemory(m_cbHandCardData,pStatusPlay->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

			//出牌变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbOutCardCount[i][0]=pStatusPlay->cbOutCardCount[i];
				CopyMemory(m_cbOutCardData[i][0],pStatusPlay->cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i][0]);
			}

			//得分信息
			m_cbScoreCardCount=pStatusPlay->cbScoreCardCount;
			CopyMemory(m_cbScoreCardData,pStatusPlay->cbScoreCardData,sizeof(BYTE)*m_cbScoreCardCount);

			//主牌信息
			WORD wMeChairID=GetMeChairID();
			BYTE cbValueMySelf=pStatusPlay->cbValueOrder[wMeChairID%2];
			BYTE cbValueOther=pStatusPlay->cbValueOrder[(wMeChairID+1)%2];
			m_GameClientView.SetValueOrder(m_cbMainValue,cbValueMySelf,cbValueOther);

			//叫牌信息
			if (m_wCallCardUser!=INVALID_CHAIR)
			{
				WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
				BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
				m_GameClientView.SetCallCardInfo(wViewChairID,cbCallColor,m_cbCallCount);
			}
			m_GameClientView.SetCurrentCallScore(pStatusPlay->wLandScore);

			//得分视图
			m_GameClientView.m_CardScore.SetCardData(m_cbScoreCardData,m_cbScoreCardCount);
			m_GameClientView.SetCardScore(m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount));

			//出牌界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][0],m_cbOutCardCount[i][0]);
			}

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

			//界面设置
			SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//玩家设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			//旁观设置
			if (IsAllowLookon()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

			//出牌设置
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			}

			//功能按钮
			m_GameClientView.m_btConcealCard.EnableWindow(((m_cbConcealCount>0)&&((IsLookonMode()==false)||(IsAllowLookon()==true)))?TRUE:FALSE);

			//时间设置
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);


			//GM帐号
			if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
			{
				SendSocketData(SUB_C_GM_REQUEST);
			}

			return true;
		}
	}
	return false;
}

bool CGameClientEngine::OnSubGMCard(const void * pData, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_GMCard));

	CMD_S_GMCard *pGMCard=(CMD_S_GMCard *)pData;

	BYTE bCard[4][60];
	BYTE bCount[4];
	CopyMemory(bCard,pGMCard->cbCardData,sizeof(bCard));
	CopyMemory(bCount,pGMCard->cbCardCount,sizeof(bCount));

	for(WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_GmCardControl[SwitchViewChairID(i)].SetCardData(bCard[i],bCount[i]);

	m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	m_GameClientView.m_btConcealCard.EnableWindow(true);

	m_GameClientView.SetGMCard();


	m_GameClientView.InvalidGameView(0,0,0,0);
	return true;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	m_wLandScore=pGameStart->bLandScore;

	//设置状态
	SetGameStatus(GS_UG_CALL);

	//逻辑变量
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=pGameStart->cbMainValue;
	m_cbPackCount=pGameStart->cbPackCount;

	//叫牌变量
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;
	m_GameClientView.SetCurrentCallScore(pGameStart->bLandScore);
	bool gi[4];
	ZeroMemory(gi,sizeof(gi));
	m_GameClientView.SetGiveUpPlayer(gi);
	m_GameClientView.m_ScoreView.SetLandScore(pGameStart->wBankerUser, pGameStart->bLandScore);

	//底牌变量
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//状态变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=pGameStart->wBankerUser;

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));
	m_GameClientView.SetCardScore(0);

	//出牌变量
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//逻辑组件
	m_GameLogic.SetPackCount(m_cbPackCount);
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetMainValue(m_cbMainValue);

	//界面设置
	m_GameClientView.SetCardScore(0xFFFF);
	m_GameClientView.m_CardScore.SetCardData(NULL,0);
	m_GameClientView.SetCallCardInfo(INVALID_CHAIR,COLOR_ERROR,0);

	//底牌控制
	m_GameClientView.m_btConcealCard.EnableWindow(FALSE);
	m_GameClientView.m_ConcealCardView.ShowWindow(SW_HIDE);
	m_GameClientView.m_ConcealCardView.SetConcealCard(NULL,0);

	//主牌信息
	WORD wMeChairID=GetMeChairID();
	BYTE cbValueMySelf=pGameStart->cbValueOrder[wMeChairID%2];
	BYTE cbValueOther=pGameStart->cbValueOrder[(wMeChairID+1)%2];
	m_GameClientView.SetValueOrder(m_cbMainValue,cbValueMySelf,cbValueOther);

	//设置庄家
	m_wBankerUser=pGameStart->wBankerUser;
	if (m_wBankerUser==INVALID_CHAIR) m_GameClientView.SetBankerUser(INVALID_CHAIR);
	else m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	if(GetMeChairID()==m_wBankerUser)
	{
		//设置底牌
		m_cbConcealCount=pGameStart->cbConcealCount;
		CopyMemory(m_cbConcealCard,pGameStart->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

		//设置扑克
		CopyMemory(&m_cbHandCardData[m_cbHandCardCount],m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));
		m_cbHandCardCount+=m_cbConcealCount;

		//设置扑克
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
		m_GameClientView.m_HandCardControl.SetShootCard(NULL,0);
		//m_GameClientView.m_HandCardControl.SetShootCard(m_cbConcealCard,m_cbConcealCount);
	}

	WORD temp[4];
	ZeroMemory(temp,sizeof(temp));
	m_GameClientView.SetUserScore(temp);

	//设置界面
	if (IsLookonMode()==true)
	{
		SetHandCardControl(NULL,0);
		m_GameClientView.SetWaitConceal(false);
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.m_CardScore.SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[0].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[1].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[3].SetCardData(NULL,0);
	}

	if(pGameStart->wBankerUser==GetMeChairID())
	{
		//更新控制
		SetGameClock(pGameStart->wBankerUser,IDI_CALL_CARD,TIME_CALL_CARD);
		m_GameClientView.m_CallCardWnd.ShowWindow(SW_SHOW);
		m_GameClientView.m_CallCardWnd.UpdateCallControl(0,1,1,1,1);

	}


	m_GameClientView.m_BackCardControl.SetDisplayFlag(true);

	//控制设置
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetDisplayFlag(bPlayerMode);
	if ((IsLookonMode()==false)&&(m_bStustee==false)) ActiveGameFrame();

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//发送扑克
bool CGameClientEngine::OnSubSendCard(const void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pData;

	//效验数据
	if(wDataSize!=sizeof(CMD_S_SendCard)) 
	{
		ASSERT(FALSE);
		return false;
	}

	SetGameStatus(GS_UG_SCORE);

	//设置扑克
	m_cbHandCardCount=pSendCard->cbCardCount;
	CopyMemory(m_cbHandCardData,pSendCard->cbCardData,sizeof(BYTE)*m_cbHandCardCount);
	m_GameLogic.SetMainColor(COLOR_ERROR);
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
	CopyMemory(m_cbBackCard,pSendCard->cbBackCard,sizeof(m_cbBackCard));

	m_GameClientView.m_BackCardControl.ShowWindow(SW_SHOW);
	m_GameClientView.m_BackCardControl.SetCardData(m_cbBackCard,8);
	m_GameClientView.m_BackCardControl.SetDisplayFlag(false);

	//设置用户
	m_wCurrentUser=pSendCard->wCurrentUser;

	//派发扑克
	DispatchUserCard(m_cbHandCardData,m_cbHandCardCount);

	//环境设置
	if ((IsLookonMode()==false)&&(m_bStustee==false)) ActiveGameFrame();

	return true;
}

//用户叫牌
bool CGameClientEngine::OnSubCallCard(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CallCard));
	if (wDataSize!=sizeof(CMD_S_CallCard)) return false;

	//消息处理
	CMD_S_CallCard * pCallCard=(CMD_S_CallCard *)pData;

	//清理叫牌
	if ((m_wCallCardUser!=INVALID_CHAIR)&&(m_wCallCardUser!=pCallCard->wCallCardUser))
	{
		WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//设置变量
	m_cbCallCard=pCallCard->cbCallCard;
	m_cbCallCount=pCallCard->cbCallCount;
	m_wCallCardUser=pCallCard->wCallCardUser;

	//叫牌界面
	BYTE cbCallCard[MAX_PACK];
	WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
	for (BYTE i=0;i<m_cbCallCount;i++) cbCallCard[i]=m_cbCallCard;
	//m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCallCard,m_cbCallCount);

	//视图信息
	BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
	m_GameClientView.SetCallCardInfo(wViewChairID,cbCallColor,m_cbCallCount);

	//播放声音
	CString strCall;
	IClientUserItem *pUserItem = GetTableUserItem(pCallCard->wCallCardUser);


	if(pUserItem==NULL||pUserItem->GetUserGrade()==GENDER_MANKIND) 	strCall.Format(TEXT("M_"));
	else strCall.Format(TEXT("F_"));
	BYTE bColor=m_GameLogic.GetCardColor(pCallCard->cbCallCard);
	if(bColor==0x00) strCall.AppendFormat(TEXT("COLOR_FK"));
	if(bColor==0x10) strCall.AppendFormat(TEXT("COLOR_MH"));
	if(bColor==0x20) strCall.AppendFormat(TEXT("COLOR_RED"));
	if(bColor==0x30) strCall.AppendFormat(TEXT("COLOR_HT"));
	PlayGameSound(AfxGetInstanceHandle(),strCall);

	//更新控制
	UpdateCallCardControl();

	//设置扑克
	m_cbMainColor=cbCallColor;
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
	SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	m_GameClientView.InvalidGameView(0,0,0,0);

	if(m_wLandScore>=85 && GetMeChairID()==m_wBankerUser && IsLookonMode()==false) m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	return true;
}

//发送底牌
bool CGameClientEngine::OnSubSendConceal(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendConceal));
	if (wDataSize!=sizeof(CMD_S_SendConceal)) return false;

	//消息处理
	CMD_S_SendConceal * pSendConceal=(CMD_S_SendConceal *)pData;

	//设置变量
	m_cbMainColor=pSendConceal->cbMainColor;
	m_wBankerUser=pSendConceal->wBankerUser;
	m_wCurrentUser=pSendConceal->wCurrentUser;
	//GM帐号
	if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
	{
		SendSocketData(SUB_C_GM_REQUEST);
	}
	//状态设置
	SetGameStatus(GS_UG_BACK);
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	m_GameClientView.m_BackCardControl.ShowWindow(SW_HIDE);
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);

	//叫牌清理
	if (m_wCallCardUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//扑克设置
	if (m_wCurrentUser==GetMeChairID())
	{
		//设置控制
		if (IsLookonMode()==false) 
		{
			//激活框架
			if (m_bStustee==false) ActiveGameFrame();

			//设置控件
			m_GameClientView.m_btSendConceal.EnableWindow(FALSE);
			m_GameClientView.m_btSendConceal.ShowWindow(SW_SHOW);
			m_GameClientView.m_HandCardControl.SetPositively(true);
			//m_GameClientView.m_HandCardControl.SetShootCard(NULL,0);
			//m_GameClientView.m_HandCardControl.SetShootCard(m_cbConcealCard,m_cbConcealCount);

		}
		else 
		{
			//设置提示
			m_GameClientView.SetWaitConceal(true);
		}
	}
	else
	{
		//设置提示
		m_GameClientView.SetWaitConceal(true);

		////设置扑克
		//m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
		//SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	}

	//环境设置
	SetGameClock(m_wCurrentUser,IDI_SEND_CONCEAL,TIME_SEND_CONCEAL);

	return true;
}

//游戏开始
bool CGameClientEngine::OnSubGamePlay(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GamePlay));
	if (wDataSize!=sizeof(CMD_S_GamePlay)) return false;

	//消息处理
	CMD_S_GamePlay * pGamePlay=(CMD_S_GamePlay *)pData;

	//设置变量
	m_wCurrentUser=pGamePlay->wCurrentUser;
	m_wFirstOutUser=pGamePlay->wCurrentUser;
	m_cbConcealCount=pGamePlay->cbConcealCount;
	CopyMemory(m_cbConcealCard,pGamePlay->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

	//底牌设置
	if ((IsLookonMode()==true)&&(m_cbConcealCount!=0))
	{
		//删除扑克
		m_cbHandCardCount-=m_cbConcealCount;
		m_GameLogic.RemoveCard(m_cbConcealCard,m_cbConcealCount,m_cbHandCardData,m_cbHandCardCount+m_cbConcealCount);

		//设置扑克
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	}

	//设置底牌
	m_GameClientView.m_ConcealCardView.SetConcealCard(m_cbConcealCard,m_cbConcealCount);
	m_GameClientView.m_btConcealCard.EnableWindow(((m_cbConcealCount!=0)&&((IsLookonMode()==false)||(IsAllowLookon()==true)))?TRUE:FALSE);

	//设置状态
	SetGameStatus(GS_UG_PLAY);
	m_GameClientView.SetCardScore(0);
	m_GameClientView.SetWaitConceal(false);

	//控制设置
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);

	//当前玩家
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		if (m_bStustee==false) ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
	}

	//环境设置
	SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//GM帐号
	if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
	{
		SendSocketData(SUB_C_GM_REQUEST);
	}

	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(const void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	//WORD wFirstSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	////效验数据
	//ASSERT(wDataSize>=wFirstSize);
	//if (wDataSize<wFirstSize) return false;
	//ASSERT(wDataSize==(wFirstSize+pOutCard->cbCardCount*sizeof(BYTE)));
	if (wDataSize!=sizeof(CMD_S_OutCard)) 
	{
		ASSERT(FALSE);
		return false;
	}
	//GM帐号
	if((IsLookonMode()==true)/*&&GetUserData(GetMeChairID())->dwMasterRight>0*/)
	{
		SendSocketData(SUB_C_GM_REQUEST);
	}

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutCardUser=pOutCard->wOutCardUser;
	WORD wOutViewChairID=SwitchViewChairID(wOutCardUser);

	//出牌处理
	SwitchToThrowResult();
	KillGameClock(IDI_OUT_CARD);

	//扑克界面
	if ((IsLookonMode()==true)||(wMeChairID!=wOutCardUser))
	{
		//历史扑克
		if (wOutCardUser==m_wFirstOutUser)
		{
			for (WORD i=0;i<GAME_PLAYER;i++) 
			{
				if (i==wOutCardUser) continue;
				m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
			}
		}

		//控制界面
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->cbCardData,pOutCard->cbCardCount);
	}

	//历史扑克
	if ((wOutCardUser==m_wFirstOutUser)&&(m_cbOutCardCount[m_wFirstOutUser][0]!=0))
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			//保存扑克
			m_cbOutCardCount[i][1]=m_cbOutCardCount[i][0];
			CopyMemory(m_cbOutCardData[i][1],m_cbOutCardData[i][0],sizeof(BYTE)*m_cbOutCardCount[i][0]);

			//清理扑克
			m_cbOutCardCount[i][0]=0;
			ZeroMemory(m_cbOutCardData[i][0],sizeof(m_cbOutCardData[i][0]));
		}

		//控制按钮
		m_GameClientView.m_btLastTurnCard.EnableWindow(TRUE);
	}

	//出牌记录
	m_cbOutCardCount[wOutCardUser][0]=pOutCard->cbCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser][0],pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//当前界面
	SwitchToCurrentCard();

	//无主设置
	bool NoMainUser[4];
	BYTE bMainCount[4];
	BYTE bDoubleCount[4];

	ZeroMemory(NoMainUser,sizeof(NoMainUser));
	ZeroMemory(bMainCount,sizeof(bMainCount));
	ZeroMemory(bDoubleCount,sizeof(bDoubleCount));

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		NoMainUser[SwitchViewChairID(i)]=pOutCard->bNoMainUser[i];
		bMainCount[SwitchViewChairID(i)]=pOutCard->bMainCount[i];
		bDoubleCount[SwitchViewChairID(i)]=pOutCard->bDoubleCount[i];
	}
	m_GameClientView.SetNoMainUser(NoMainUser);
	m_GameClientView.SetMainCount(bMainCount,bDoubleCount);

	//玩家扑克
	if ((IsLookonMode()==true)&&(wOutCardUser==wMeChairID))
	{
		//删除扑克 
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=pOutCard->cbCardCount;
		m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

		//设置扑克
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	}

	//播放声音
	BYTE outType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount);
	IClientUserItem *pUserItem = GetTableUserItem(pOutCard->wOutCardUser);

	CString strOutCard;
	if(pUserItem!=NULL)
	{	if(pUserItem->GetGender()==1)
			strOutCard.Format(TEXT("M_"));
		else
			strOutCard.Format(TEXT("F_"));
	}
	else
		strOutCard.Format(TEXT("F_"));
	switch(outType)
	{
	case CT_SINGLE:
		{
			if(m_wFirstOutUser==pOutCard->wOutCardUser) 
			{
				BYTE bCard=pOutCard->cbCardData[0];
				BYTE bColor=m_GameLogic.GetCardLogicColor(bCard);
				if(bColor==COLOR_NT) strOutCard.AppendFormat(TEXT("DIAO"));
				else if(bColor==0x00) strOutCard.AppendFormat(TEXT("COLOR_FK"));
				else if(bColor==0x10) strOutCard.AppendFormat(TEXT("COLOR_MH"));
				else if(bColor==0x20) strOutCard.AppendFormat(TEXT("COLOR_RED"));
				else if(bColor==0x30) strOutCard.AppendFormat(TEXT("COLOR_HT"));
			}
			else
			{
				BYTE bFirstOutCard=m_cbOutCardData[m_wFirstOutUser][0][0];
				BYTE bOutCard=pOutCard->cbCardData[0];
				if(m_GameLogic.GetCardLogicColor(bFirstOutCard)!=COLOR_NT && m_GameLogic.GetCardLogicColor(bOutCard)==COLOR_NT )
					strOutCard.AppendFormat(TEXT("SHA"));
				else  strOutCard=TEXT("OUT_CARD");
			}
			break;
		}
	case CT_SAME_2	:	
		{
			strOutCard.AppendFormat(TEXT("DUI"));
			break;
		}
	case CT_TRACKOR_2	:
	case CT_TRACKOR_3	:
		{
			strOutCard.AppendFormat(TEXT("TUO"));
			break;
		}
	case CT_THROW_CARD:
	default:
		{
			if(m_wFirstOutUser==pOutCard->wOutCardUser) strOutCard.AppendFormat(TEXT("SHUAI"));
			else strOutCard=TEXT("OUT_CARD");
			break;
		}
		//default:
		//	strOutCard=TEXT("OUT_CARD");
	}

	PlayGameSound(AfxGetInstanceHandle(),strOutCard);

	//设置变量
	m_wCurrentUser=pOutCard->wCurrentUser;

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		if (m_bStustee==false) ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
	}


	//三人无主
	BYTE bNoMainCount=0;
	for(BYTE i=0;i<GAME_PLAYER;i++) if(NoMainUser[i]==true) bNoMainCount++;
	if(bNoMainCount==3) m_GameLogic.SetCanThrow(true);


	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		if(pOutCard->bLastTurn==true)
		{
			if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
			{
				SetGameClock(m_wCurrentUser,IDI_OUT_CARD,1);
				//AutomatismOutCard();
			}
		}
		else SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
	}

	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	return true;
}

//甩牌结果
bool CGameClientEngine::OnSubThrowResult(const void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_ThrowResult * pThrowResult=(CMD_S_ThrowResult *)pData;
	ASSERT(wDataSize==sizeof(CMD_S_ThrowResult));
	//WORD wCardCount=pThrowResult->cbThrowCardCount+pThrowResult->cbResultCardCount;
	//WORD wFirstSize=sizeof(CMD_S_ThrowResult)-sizeof(pThrowResult->cbCardDataArray);

	////效验数据
	//ASSERT(wDataSize>=wFirstSize);
	//if (wDataSize<wFirstSize) return false;
	//ASSERT(wDataSize==(wFirstSize+wCardCount*sizeof(BYTE)));
	//if (wDataSize!=(wFirstSize+wCardCount*sizeof(BYTE))) return false;

	//用户得分
	int tempScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		tempScore[SwitchViewChairID(i)]=pThrowResult->PlayerScore[(i)];
	}
	m_GameClientView.SetPlayerScore(tempScore);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutCardUser=pThrowResult->wOutCardUser;
	WORD wOutViewChairID=SwitchViewChairID(wOutCardUser);

	//扑克变量
	BYTE cbThrowCardCount=pThrowResult->cbThrowCardCount;
	BYTE cbResultCardCount=pThrowResult->cbResultCardCount;
	BYTE * pcbThrowCardData=&pThrowResult->cbCardDataArray[0];
	BYTE * pcbResultCardData=&pThrowResult->cbCardDataArray[pThrowResult->cbThrowCardCount];

	//还原界面
	SwitchToCurrentCard();

	//出牌处理
	KillGameClock(IDI_OUT_CARD);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("THROW_RESULT"));

	//清理扑克
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		//设置扑克
		if (i!=wOutCardUser) m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
	}

	//历史扑克
	if (m_cbOutCardCount[m_wFirstOutUser][0]!=0)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			//保存扑克
			m_cbOutCardCount[i][1]=m_cbOutCardCount[i][0];
			CopyMemory(m_cbOutCardData[i][1],m_cbOutCardData[i][0],sizeof(BYTE)*m_cbOutCardCount[i][0]);

			//清理扑克
			m_cbOutCardCount[i][0]=0;
			ZeroMemory(m_cbOutCardData[i][0],sizeof(m_cbOutCardData[i][0]));
		}
	}

	//出牌界面
	m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pcbThrowCardData,cbThrowCardCount);

	//出牌记录
	m_cbOutCardCount[wOutCardUser][0]=cbResultCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser][0],pcbResultCardData,cbResultCardCount*sizeof(BYTE));

	//玩家扑克
	if (wOutCardUser==wMeChairID)
	{
		//填补扑克
		if (IsLookonMode()==false)
		{
			//设置扑克
			BYTE cbSourceCount=m_cbHandCardCount;
			m_cbHandCardCount+=cbThrowCardCount;
			CopyMemory(&m_cbHandCardData[cbSourceCount],pcbThrowCardData,sizeof(BYTE)*cbThrowCardCount);

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
		}

		//删除扑克
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=cbResultCardCount;
		m_GameLogic.RemoveCard(pcbResultCardData,cbResultCardCount,m_cbHandCardData,cbSourceCount);

		//设置界面
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	}

	//设置变量
	m_bThrowCard=true;
	m_cbResultCardCount=cbResultCardCount;
	m_wCurrentUser=pThrowResult->wCurrentUser;
	m_wThrowCardUser=pThrowResult->wOutCardUser;
	CopyMemory(m_cbResultCardData,pcbResultCardData,sizeof(BYTE)*cbResultCardCount);

	//设置时间
	SetTimer(IDI_THROW_CARD_RESULT,2000,NULL);

	//上轮设置
	m_GameClientView.m_btLastTurnCard.EnableWindow(FALSE);

	return true;
}

//一轮结算
bool CGameClientEngine::OnSubTurnBalance(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_TurnBalance));
	if (wDataSize!=sizeof(CMD_TurnBalance)) return false;

	//消息处理
	CMD_TurnBalance * pTurnBalance=(CMD_TurnBalance *)pData;

	//用户得分
	int tempScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		tempScore[SwitchViewChairID(i)]=pTurnBalance->PlayerScore[(i)];
	}
	m_GameClientView.SetPlayerScore(tempScore);

	//计算得分
	WORD wWinnerUser=pTurnBalance->wTurnWinner;
	if ((wWinnerUser!=m_wBankerUser)/*&&((wWinnerUser+2)%GAME_PLAYER!=m_wBankerUser)*/)
	{
		//获取分牌
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			BYTE * pcbScoreCard=&m_cbScoreCardData[m_cbScoreCardCount];
			BYTE cbMaxCount=CountArray(m_cbScoreCardData)-m_cbScoreCardCount;
			m_cbScoreCardCount+=m_GameLogic.GetScoreCard(m_cbOutCardData[i][0],m_cbOutCardCount[i][0],pcbScoreCard,cbMaxCount);
		}

		//设置界面
		m_GameClientView.m_CardScore.SetCardData(m_cbScoreCardData,m_cbScoreCardCount);
		m_GameClientView.SetCardScore(m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount));
	}

	//设置用户
	m_wCurrentUser=pTurnBalance->wCurrentUser;
	m_wFirstOutUser=pTurnBalance->wCurrentUser;

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		if (m_bStustee==false) ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
	}

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR) SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//游戏定时器
	KillGameClock(IDI_OUT_CARD);
	KillGameClock(IDI_CALL_CARD);
	KillGameClock(IDI_SEND_CONCEAL);

	//功能定时器
	KillTimer(IDI_CONCEAL_CARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAST_TURN_CARD);
	KillTimer(IDI_THROW_CARD_RESULT);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_CallCardWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSendConceal.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSendConceal.EnableWindow(FALSE);
	m_GameClientView.m_CallCardWnd.ShowWindow(SW_HIDE);

	//控制界面
	m_GameClientView.m_btConcealCard.EnableWindow(FALSE);
	m_GameClientView.m_btLastTurnCard.EnableWindow(FALSE);
	m_GameClientView.m_ConcealCardView.ShowWindow(SW_HIDE);
	m_GameClientView.m_ConcealCardView.SetConcealCard(NULL,0);
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_BackCardControl.SetDisplayFlag(true);

	//设置控件
	SwitchToCurrentCard();
	SwitchToThrowResult();
	m_GameClientView.SetWaitConceal(false);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);

	//设置结束
	tagScoreViewInfo ScoreViewInfo;
	ScoreViewInfo.wGameScore=pGameEnd->wGameScore;
	ScoreViewInfo.wConcealTime=pGameEnd->wConcealTime;
	ScoreViewInfo.wConcealScore=pGameEnd->wConcealScore;
	ScoreViewInfo.cbConcealCount=pGameEnd->cbConcealCount;
	ScoreViewInfo.bAddConceal=pGameEnd->bAddConceal;
	ScoreViewInfo.bEndStatus=pGameEnd->bEndStatus;
	CopyMemory(ScoreViewInfo.cbConcealCard,pGameEnd->cbConcealCard,sizeof(BYTE)*pGameEnd->cbConcealCount);
	m_GameClientView.m_ScoreView.SetScoreViewInfo(ScoreViewInfo);
	m_GameClientView.m_CallScoreView.SetCurrentCallScore(120);

	m_GameClientView.m_BackCardControl.SetCardData(pGameEnd->cbConcealCard,8);
	m_GameClientView.m_BackCardControl.SetDisplayFlag(true);
	m_GameClientView.m_BackCardControl.ShowWindow(SW_SHOW);

	m_GameLogic.SetCanThrow(false);

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IClientUserItem *pUserItem =GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserItem->GetNickName(),pGameEnd->lScore[i],pGameEnd->lKingScore[i]);
	}

	//显示成绩
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//播放声音
	//WORD wMeChairID=GetMeChairID();
	//LONGLONG lMeScore=pGameEnd->lScore[GetMeChairID()];
	//if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	//else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	//else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));
	CString strEnd;

	IClientUserItem *pUserItem =GetMeUserItem();
	if(pUserItem->GetGender()==1&&pUserItem!=NULL) strEnd.Format(TEXT("M_"));
	else strEnd.Format(TEXT("F_"));
	strEnd.AppendFormat(TEXT("END_%d"),pGameEnd->bEndStatus);
	PlayGameSound(AfxGetInstanceHandle(),strEnd);


	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	m_GameClientView.m_CallScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
	SetGameStatus(GS_UG_FREE);

	return true;
}

//出牌判断
bool CGameClientEngine::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(cbCardData,CountArray(cbCardData));

	//出牌判断
	if (cbShootCount>0L)
	{
		//排列扑克
		m_GameLogic.SortCardList(cbCardData,cbShootCount);

		//首出牌者
		if (m_wFirstOutUser==GetMeChairID())
		{
			//类型判断
			BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbShootCount);
			if (cbCardType==CT_ERROR) return false;

			return true;
		}

		//跟随出牌
		BYTE cbTurnCardCount=m_cbOutCardCount[m_wFirstOutUser][0];
		bool bLegality=m_GameLogic.EfficacyOutCard(cbCardData,cbShootCount,m_cbOutCardData[m_wFirstOutUser][0],
			cbTurnCardCount,m_cbHandCardData,m_cbHandCardCount);

		return bLegality;
	}

	return false;
}

//自动出牌
bool CGameClientEngine::AutomatismOutCard()
{
	//变量定义
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//搜索出牌
	BYTE cbTurnCardCount=(m_wCurrentUser==m_wFirstOutUser)?0:m_cbOutCardCount[m_wFirstOutUser][0];
	m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbOutCardData[m_wFirstOutUser][0],cbTurnCardCount,OutCardResult);

	//设置变量
	KillGameClock(IDI_OUT_CARD);
	m_wCurrentUser=INVALID_CHAIR;

	//设置控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//设置扑克
	if (m_wFirstOutUser==GetMeChairID())
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=2) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		}
	}

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=OutCardResult.cbCardCount;
	CopyMemory(OutCard.cbCardData,OutCardResult.cbResultCard,sizeof(BYTE)*OutCard.cbCardCount);
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	//预先显示
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.cbCardData,OutCard.cbCardCount);

	//预先删除
	BYTE cbSourceCount=m_cbHandCardCount;
	m_cbHandCardCount-=OutCard.cbCardCount;
	m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_cbHandCardData,cbSourceCount);

	//设置扑克
	SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);

	return true;
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
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][0],m_cbOutCardCount[i][0]);
	}

	return true;
}

//甩牌结果
bool CGameClientEngine::SwitchToThrowResult()
{
	//处理判断
	if (m_bThrowCard==false) return false;
	if (m_wThrowCardUser==INVALID_CHAIR) return false;

	//时间设置
	KillTimer(IDI_THROW_CARD_RESULT);

	//设置界面
	WORD wViewChairID=SwitchViewChairID(m_wThrowCardUser);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbResultCardData,m_cbResultCardCount);

	//界面设置
	if (m_cbOutCardCount[0][1]!=0) m_GameClientView.m_btLastTurnCard.EnableWindow(TRUE);

	//设置变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));

	return true;
}

//更新叫牌
bool CGameClientEngine::UpdateCallCardControl()
{
	//状态过滤
	if (IsLookonMode()==true) return false;
	if (GetGameStatus()!=GS_UG_CALL) return false;

	//变量定义
	BYTE cbCallMask=CALL_NONE;
	BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);

	//辅助变量
	BYTE cbCardCount[6]={0,0,0,0,0,0};
	BYTE cbCallMaskArray[6]={CALL_FANG_KUAI,CALL_MEI_HUA,CALL_HONG_TAO,CALL_HEI_TAO,CALL_NT,CALL_NT};
	BYTE cbCallColorArray[6]={COLOR_FANG_KUAI,COLOR_MEI_HUA,COLOR_HONG_TAO,COLOR_HEI_TAO,COLOR_NT,COLOR_NT};

	//更新调整
	if (m_wCallCardUser!=GetMeChairID())
	{
		//扑克统计
		for (BYTE i=0;i<m_cbDispatchCardCount;i++)
		{
			//获取属性
			BYTE cbCardColor=m_GameLogic.GetCardColor(m_cbDispatchCardData[i]);
			BYTE cbCardValue=m_GameLogic.GetCardValue(m_cbDispatchCardData[i]);

			//数据统计
			if (cbCardColor==COLOR_NT) cbCardCount[4+cbCardValue-0x0E]++;
			else if (cbCardValue==m_cbMainValue) cbCardCount[cbCardColor>>4]++;
		}

		//主牌控制
		for (BYTE i=0;i<CountArray(cbCardCount);i++)
		{
			//规则判断
			if ((cbCardCount[i]==0)||(cbCardCount[i]<m_cbCallCount)) continue;

			//数目判断
			if ((i<4)&&(cbCardCount[i]>m_cbCallCount))
			{
				cbCallMask|=cbCallMaskArray[i];
				continue;
			}

			//无主判断
			if ((i>=4)&&(cbCardCount[i]>m_cbCallCount)&&(cbCardCount[i]>=2))
			{
				cbCallMask|=cbCallMaskArray[i];
				continue;
			}

			//花色判断
			BYTE cbPackCount=m_GameLogic.GetPackCount();
			if ((cbCardCount[i]==cbPackCount)&&(cbCallColorArray[i]>cbCallColor))
			{
				cbCallMask|=cbCallMaskArray[i];
				continue;
			}
		}
	}
	else
	{
		//扑克统计
		BYTE cbCallCardCount=0;
		for (BYTE i=0;i<m_cbDispatchCardCount;i++)
		{
			if (m_cbDispatchCardData[i]==m_cbCallCard) cbCallCardCount++;
		}

		//叫牌控制
		if (cbCallCardCount>m_cbCallCount) cbCallMask=cbCallMaskArray[cbCallColor>>4];
	}

	//更新控制
	m_GameClientView.m_CallCardWnd.UpdateCallControl(cbCallMask);

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

//设置扑克
bool CGameClientEngine::SetHandCardControl(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置过滤
	if (cbCardCount>0)
	{
		//变量定义
		BYTE cbRectifyCount=1;
		BYTE cbRectifyCard[MAX_COUNT+4]={cbCardData[0]};

		//获取属性
		BYTE cbLogicColor=m_GameLogic.GetCardLogicColor(cbCardData[0]);

		//变换扑克
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//获取属性
			BYTE cbCurrentColor=m_GameLogic.GetCardLogicColor(cbCardData[i]);

			//插入判断
			if (cbCurrentColor!=cbLogicColor)
			{
				cbLogicColor=cbCurrentColor;
				cbRectifyCard[cbRectifyCount++]=255;
				cbRectifyCard[cbRectifyCount++]=cbCardData[i];
			}
			else cbRectifyCard[cbRectifyCount++]=cbCardData[i];
		}

		//设置扑克
		m_GameClientView.m_HandCardControl.SetCardData(cbRectifyCard,cbRectifyCount);

		return true;
	}
	else m_GameClientView.m_HandCardControl.SetCardData(NULL,0);

	return true;
}

//定时器消息
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_CONCEAL_CARD:		//查看底牌
		{
			//显示设置
			KillTimer(IDI_CONCEAL_CARD);
			m_GameClientView.m_ConcealCardView.ShowWindow(SW_HIDE);

			return;
		}
	case IDI_DISPATCH_CARD:		//派发扑克
		{
			//派发控制
			if (m_cbRemnantCardCount>0)
			{
				//获取属性
				BYTE cbCardColor=m_GameLogic.GetCardColor(m_cbDispatchCardData[m_cbDispatchCardCount]);
				BYTE cbCardValue=m_GameLogic.GetCardValue(m_cbDispatchCardData[m_cbDispatchCardCount]);

				//派发扑克
				m_cbRemnantCardCount--;
				m_cbDispatchCardCount++;
				m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount);
				SetHandCardControl(m_cbDispatchCardData,m_cbDispatchCardCount);

				//叫牌控制
				if ((cbCardColor==COLOR_NT)||(cbCardValue==m_cbMainValue)) UpdateCallCardControl();

				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
			}

			//中止判断
			if (m_cbRemnantCardCount==0) 
			{
				//删除定时器
				KillTimer(IDI_DISPATCH_CARD);

				//环境设置
				SetGameClock(m_wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);
				if ((IsLookonMode()==false)&&(m_bStustee==false))
				{
					ActiveGameFrame();
					//设置首叫用户
					if(m_wCurrentUser==GetMeChairID())
					{
						ActiveGameFrame();
						m_GameClientView.m_CallScoreView.ShowWindow(SW_SHOW);
						m_GameClientView.SetCurrentCallScore(120);
					}
				}
			}

			return;
		}
	case IDI_LAST_TURN_CARD:	//上轮扑克
		{
			//还原界面
			SwitchToCurrentCard();

			return;
		}
	case IDI_THROW_CARD_RESULT:	//甩牌结果
		{
			//还原界面
			SwitchToThrowResult();

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				if (m_bStustee==false) ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			}

			//设置定时器
			SetGameClock(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置环境
	KillGameClock(IDI_START_GAME);

	//设置界面
	m_GameClientView.SetCardScore(0xFFFF);
	m_GameClientView.SetWaitConceal(false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetCallCardInfo(INVALID_CHAIR,COLOR_ERROR,0);
	int clearScore[4];
	ZeroMemory(clearScore,sizeof(clearScore));
	m_GameClientView.SetPlayerScore(clearScore);
	m_GameClientView.SetCurrentCallScore(120);
	WORD clearCallScore[4];
	ZeroMemory(clearCallScore,sizeof(clearCallScore));
	m_GameClientView.SetUserScore(clearCallScore);
	bool clearGiveup[4];
	ZeroMemory(clearGiveup,sizeof(clearGiveup));
	m_GameClientView.SetGiveUpPlayer(clearGiveup);

	//无主设置
	bool NoMainUser[4];
	ZeroMemory(NoMainUser,sizeof(NoMainUser));
	m_GameClientView.SetNoMainUser(NoMainUser);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.m_CallScoreView.ShowWindow(SW_HIDE);

	//底牌控制
	m_GameClientView.m_btConcealCard.EnableWindow(FALSE);
	m_GameClientView.m_ConcealCardView.ShowWindow(SW_HIDE);
	m_GameClientView.m_ConcealCardView.SetConcealCard(NULL,0);

	//设置扑克
	SetHandCardControl(NULL,0);
	m_GameClientView.m_CardScore.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_BackCardControl.SetDisplayFlag(false);
	m_GameClientView.m_BackCardControl.ShowWindow(SW_HIDE);

	PlayGameSound(AfxGetInstanceHandle(),TEXT("CLICK_START"));
	//发送消息
	if(IsLookonMode()==false) SendUserReady(NULL,0);

	return 0;
}

//出牌消息
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return 0;

	//设置变量
	KillGameClock(IDI_OUT_CARD);
	m_wCurrentUser=INVALID_CHAIR;

	//设置控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//设置扑克
	if (m_wFirstOutUser==GetMeChairID())
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=2) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		}
	}

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.cbCardData,CountArray(OutCard.cbCardData));
	m_GameLogic.SortCardList(OutCard.cbCardData,OutCard.cbCardCount);
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	//预先显示
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.cbCardData,OutCard.cbCardCount);

	//预先删除
	BYTE cbSourceCount=m_cbHandCardCount;
	m_cbHandCardCount-=OutCard.cbCardCount;
	m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_cbHandCardData,cbSourceCount);

	//设置扑克
	SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);

	return 0;
}

//叫牌消息
LRESULT CGameClientEngine::OnCallCard(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbCallCount=0;
	BYTE cbCallCard=0x00;
	BYTE cbCallColor=(BYTE)lParam;

	////数据统计
	//if (cbCallColor==COLOR_NT)
	//{
	//	//数目统计
	//	BYTE cbNTCount[2]={0,0};
	//	for (BYTE i=0;i<m_cbDispatchCardCount;i++)
	//	{
	//		BYTE cbCardData=m_cbDispatchCardData[i];
	//		if ((cbCardData==0x4E)||(cbCardData==0x4F)) cbNTCount[cbCardData-0x4E]++;
	//	}

	//	//叫牌数据
	//	if (cbNTCount[0]>=cbNTCount[1])
	//	{
	//		cbCallCard=0x4E;
	//		cbCallCount=cbNTCount[0];
	//	}
	//	else
	//	{
	//		cbCallCard=0x4F;
	//		cbCallCount=cbNTCount[1];
	//	}
	//}
	//else
	//{
	//	//数目统计
	//	for (BYTE i=0;i<m_cbDispatchCardCount;i++)
	//	{
	//		//获取属性
	//		BYTE cbCardData=m_cbDispatchCardData[i];
	//		BYTE cbCardColor=m_GameLogic.GetCardColor(cbCardData);
	//		BYTE cbCardValue=m_GameLogic.GetCardValue(cbCardData);

	//		//数据统计
	//		if ((cbCardValue==m_cbMainValue)&&(cbCardColor==cbCallColor)) 
	//		{
	//			cbCallCount++;
	//			cbCallCard=cbCardData;
	//		}
	//	}
	//}

	//更新界面

	cbCallCard=(cbCallColor&0xF0)+0x02;
	cbCallCount=1;

	//构造数据
	CMD_C_CallCard CallCard;
	CallCard.cbCallCard=cbCallCard;
	CallCard.cbCallCount=cbCallCount;

	//发送数据
	SendSocketData(SUB_C_CALL_CARD,&CallCard,sizeof(CallCard));
	m_GameClientView.m_CallCardWnd.ShowWindow(SW_HIDE);
	return 0;
}

//出牌提示
LRESULT CGameClientEngine::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//搜索出牌
	BYTE cbTurnCardCount=(m_wCurrentUser==m_wFirstOutUser)?0:m_cbOutCardCount[m_wFirstOutUser][0];
	bool bSuccess=m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbOutCardData[m_wFirstOutUser][0],cbTurnCardCount,OutCardResult);

	//失败判断
	if (bSuccess==false)
	{
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(TEXT("自动提示出牌错误，请你把此状态信息截图并给我们提供，谢谢！"));
		return false;
	}

	//设置界面
	m_GameClientView.m_btOutCard.EnableWindow(TRUE);
	m_GameClientView.m_HandCardControl.SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

	return 0;
}

//留底消息
LRESULT CGameClientEngine::OnSendConceal(WPARAM wParam, LPARAM lParam)
{
	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(cbCardData,CountArray(cbCardData));
	if(cbCardCount!=m_cbConcealCount) return 0;
	//删除扑克
	m_cbHandCardCount-=cbCardCount;
	m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData,m_cbHandCardCount+cbCardCount);

	//设置界面
	KillGameClock(IDI_SEND_CONCEAL);
	m_GameClientView.m_btSendConceal.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSendConceal.EnableWindow(FALSE);
	SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);

	//构造数据
	CMD_C_ConcealCard ConcealCard;
	ConcealCard.cbConcealCount=cbCardCount;
	CopyMemory(ConcealCard.cbConcealCard,cbCardData,cbCardCount*sizeof(BYTE));

	//发送数据
	SendSocketData(SUB_C_CONCEAL_CARD,&ConcealCard,sizeof(ConcealCard));

	return 0;
}

//查看底牌
LRESULT CGameClientEngine::OnConcealCard(WPARAM wParam, LPARAM lParam)
{
	//查看设置
	if (m_GameClientView.m_ConcealCardView.IsWindowVisible())
	{
		KillTimer(IDI_CONCEAL_CARD);
		m_GameClientView.m_ConcealCardView.ShowWindow(SW_HIDE);
	}
	else
	{
		SetTimer(IDI_CONCEAL_CARD,5000,NULL);
		m_GameClientView.m_ConcealCardView.ShowWindow(SW_SHOW);
	}

	return 0;
}

////请求离开
//LRESULT CGameClientEngine::OnRuquestLeave(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}

//上轮扑克
LRESULT CGameClientEngine::OnLastTurnCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][1],m_cbOutCardCount[i][1]);
		}

		//环境设置
		m_bLastTurn=true;
		m_GameClientView.SetLastTurnCard(true);
		SetTimer(IDI_LAST_TURN_CARD,3000,NULL);
	}
	else SwitchToCurrentCard();

	return 0;
}

//拖管控制
LRESULT CGameClientEngine::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
	}

	return 0;
}

//左键扑克
LRESULT CGameClientEngine::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//获取状态
	BYTE cbGameStatue=GetGameStatus();

	//留底状态
	switch (cbGameStatue)
	{
	case GS_UG_BACK:	//留底状态
		{
			//状态控制
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				//获取扑克
				BYTE cbCardData[MAX_COUNT];
				BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(cbCardData,CountArray(cbCardData));

				//控制按钮
				m_GameClientView.m_btSendConceal.EnableWindow((cbCardCount==m_cbConcealCount)?TRUE:FALSE);
			}

			return 0;
		}
	case GS_UG_PLAY:	//游戏状态
		{
			//设置控件
			bool bOutCard=VerdictOutCard();
			m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

			return 0;
		}
	}

	return 0;
}

//右键扑克
LRESULT CGameClientEngine::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}


//叫分消息
LRESULT CGameClientEngine::OnLandScore(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameClock(IDI_LAND_SCORE);
	m_GameClientView.m_CallScoreView.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=(BYTE)wParam;
	SendSocketData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	return 0;
}

//用户叫分
bool CGameClientEngine::OnSubLandScore(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;

	//消息处理
	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pData;

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
	m_GameClientView.SetLandScore(wViewChairID,pLandScore->bLandScore);
	m_GameClientView.SetCurrentCallScore(pLandScore->bLandScore);
	m_GameClientView.m_CallScoreView.SetCurrentCallScore(pLandScore->bLandScore);

	//用户放弃
	bool GiveUp[4];
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		GiveUp[SwitchViewChairID(i)]=pLandScore->bGiveUpScore[i];
	}
	m_GameClientView.SetGiveUpPlayer(GiveUp);

	//用户叫分
	WORD UserScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		UserScore[SwitchViewChairID(i)]=pLandScore->wUserScore[i];
	}
	m_GameClientView.SetUserScore(UserScore);

	//玩家设置
	if ((IsLookonMode()==false)&&(pLandScore->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_CallScoreView.ShowWindow(SW_SHOW);
	}

	//播放声音
	IClientUserItem *pUserItem=GetTableUserItem(pLandScore->bLandUser);
	PlayLandScoreSound(pUserItem,pLandScore->wUserScore[pLandScore->bLandUser]);

	//设置时间
	SetGameClock(pLandScore->wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);

	return true;
}

//播放叫分声音
void CGameClientEngine::PlayLandScoreSound(IClientUserItem *pUserItem,WORD wLandScore)
{
	if(pUserItem == NULL) return;
	CString strScore;
	if(pUserItem->GetGender()==1)	strScore.Format(TEXT("M_"));
	else 	strScore.Format(TEXT("F_"));

	if(wLandScore==255) strScore.AppendFormat(TEXT("NOCALL"));
	else strScore.AppendFormat(TEXT("%d"),wLandScore);
	PlayGameSound(AfxGetInstanceHandle(),strScore);
	return;
}

//////////////////////////////////////////////////////////////////////////
//取消消息
//VOID CGameClientEngine::OnCancel()
//{
//	//退出提示
//	if (GetGameStatus()!=GS_UG_FREE && IsLookonMode()==false)
//	{
//
//		const IClientUserItem * pUserItem=GetMeUserItem();
//		if (pUserItem!=NULL)
//		{
//			CString strMessage;
//			strMessage.Format(TEXT("你正在游戏中，强行退出将被扣 %ld 分，确实要强退吗？"),12*m_lCellScore);
//			int iRet=AfxMessageBox(strMessage,MB_YESNO|MB_ICONWARNING);
//			if (iRet!=IDYES) 
//			{
//				AfxGetMainWnd()->PostMessage(WM_CLOSE);
//				return;
//			}
//		}
//	}
//
////	return __super::OnCancel();
//}
//////////////////////////////////////////////////////////////////////////////////
