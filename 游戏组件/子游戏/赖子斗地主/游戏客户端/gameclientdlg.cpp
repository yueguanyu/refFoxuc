#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_MOST_CARD					201								//最大定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_LAND_SCORE					203								//叫分定时器
#define IDI_DISPATCH_CARD				204								//发牌定时器
#define IDI_ADD							205								//加倍定时器
#define IDI_QIANG_LAND					206								//抢地主定时器
#define IDI_LAIZI						207								//赖子定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_LAND_SCORE,OnLandScore)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAND_AUTOPLAY,OnAutoPlay)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnMessageSortCard)
	//
	ON_MESSAGE(IDM_MING_CARD_STATE,OnMingCardState)
	ON_MESSAGE(IDM_MING_CARD,OnMingCard)
	ON_MESSAGE(IDM_QIANG_LAND,OnQiangLand)
	ON_MESSAGE(IDM_MING_OUT,OnMingOut)
	ON_MESSAGE(IDM_ADD,OnAdd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;
	m_bLaiZiData=0;
	memset(m_bMingCard,0,sizeof(m_bMingCard));
	memset(m_bHandCardUser,0,sizeof(m_bHandCardUser));

	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bChangeCard,0,sizeof(m_bChangeCard));
	memset(m_bChangeTurnOut,0,sizeof(m_bChangeTurnOut));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
	m_bLaiZiFrame=0;
    
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

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
	SetWindowText(TEXT("癞子斗地主游戏  --  Ver：6.0.1.0"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;
	m_bAllowLookon = AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.m_HandCardControl[1].SetCardSpace(m_dwCardHSpace,0,20);

	////创建成绩
	//if (m_GameClientView.m_ScoreView.m_hWnd==NULL)
	//{
	//	m_GameClientView.m_ScoreView.Create(IDD_GAME_SCORE,&m_GameClientView);
	//}
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;
	m_bLaiZiData=0;
	memset(m_bMingCard,0,sizeof(m_bMingCard));
	memset(m_bHandCardUser,0,sizeof(m_bHandCardUser));

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bChangeCard,0,sizeof(m_bChangeCard));
	memset(m_bChangeTurnOut,0,sizeof(m_bChangeTurnOut));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
	m_bLaiZiFrame=0;
    
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//删除定时
	KillGameClock(0);
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAIZI);

	//历史数据
	m_HistoryScore.ResetData();

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	////构造数据
	//CGameOption GameOption;
	//GameOption.m_dwCardHSpace=m_dwCardHSpace;
	//GameOption.m_bEnableSound=IsEnableSound();
	//GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();
	//GameOption.m_bAllowLookOn=IsAllowUserLookon();

	////配置数据
	//if (GameOption.DoModal()==IDOK)
	//{
	//	//获取参数
	//	m_bDeasilOrder=GameOption.m_bDeasilOrder;
	//	m_dwCardHSpace=GameOption.m_dwCardHSpace;

	//	//设置控件
	//	EnableSound(GameOption.m_bEnableSound);
	//	m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
	//	m_GameClientView.m_HandCardControl[1].SetCardSpace(m_dwCardHSpace,0,20);
	//	AllowUserLookon(0,GameOption.m_bAllowLookOn);

	//	//保存配置
	//	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	//	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
	//	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),m_bDeasilOrder?TRUE:FALSE);
	//	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	//}

	return;
}
//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//用户进入
VOID  CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//设置成绩
	if (bLookonUser==false)
	{
		//设置变量
		m_HistoryScore.OnEventUserEnter(pIClientUserItem->GetChairID());

		//更新界面
		m_GameClientView.RefreshGameView();
	}

	return;
}

//用户离开
VOID  CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//设置成绩
	if (bLookonUser==false)
	{
		//设置变量
		m_HistoryScore.OnEventUserLeave(pIClientUserItem->GetChairID());

		//更新界面
		m_GameClientView.RefreshGameView();
	}

	return;
}
//用户状态
VOID  CGameClientEngine::OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}
//用户状态
VOID  CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	////变量定义
	//BYTE cbUserStatus=pClientUserItem->GetUserStatus();

	////自动开始
	//if ((pIClientUserItem!=pIMySelfUserItem)&&(cbUserStatus==US_READY)) PerformAutoStart();

	////停止开始
	//if ((pIClientUserItem==pIMySelfUserItem)&&(cbUserStatus==US_READY)) KillGameClock(IDI_START_GAME);

	return;
}
//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_OUT_CARD:			//用户出牌
		{
			//自动处理
			if ((((nElapse==0)||((m_bAutoPlay==true)&&(nElapse<=29))))&&(wChairID==GetMeChairID()))
			{
				//超时
				if(nElapse==0)
					m_wTimeOutCount++;

				//超时判断
				if ((IsLookonMode()==false)&&(m_bAutoPlay==false)&&((m_wTimeOutCount)>=3)) 
				{
					//状态切换
					m_wTimeOutCount=0;
					
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					if(m_pIStringMessage != NULL)
						m_pIStringMessage->InsertSystemString( TEXT( "由于您多次超时，切换为“系统托管”模式" ) );
					m_bAutoPlay = true;

					CMD_C_UserTrustee UserTrustee;
					ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
					UserTrustee.bTrustee = true;
					UserTrustee.wUserChairID = GetMeChairID();

					SendSocketData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );
				}

				//自动开始
				if ( m_GameClientView.m_btStart.IsWindowVisible() ) {
					OnStart( 0, 0 );
				}
				else {
					//自动出牌
					OnAutoOutCard( 0, 0 ) ;
					OnOutCard( 0, 0 ) ;
				}
				return true;
			}

			//播放声音
			if (m_bHandCardCount<m_bTurnCardCount) return true;
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
					//OnStart(0,0);
					//OnAutoPlay(1,1);

					CMD_C_UserTrustee UserTrustee;
					ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
					UserTrustee.bTrustee = true;
					UserTrustee.wUserChairID = GetMeChairID();

					SendSocketData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );
				}
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_LAND_SCORE:		//挖坑叫分
		{
			if ( nElapse==0 || ( nElapse <= 27 && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
					OnLandScore(255,255);
					//OnLandScore(3,3);
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_ADD:				//加倍
		{
			if(nElapse == 0 || ( nElapse <= 27 && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnAdd(0,0);
				return false;
			}

			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_QIANG_LAND:		//抢地主
		{
			if(nElapse == 0 || ( nElapse <= 27 && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
					OnQiangLand(0,0);
				return false;
			}

			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//旁观设置
	m_GameClientView.m_HandCardControl[1].SetDisplayFlag( ( IsLookonMode() == true ) && ( IsAllowLookon() == true ) );

	return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_LAND_SCORE:	//用户叫分
		{
			return OnSubLandScore(pData,wDataSize);
		}
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
	case SUB_C_TRUSTEE:			//玩家托管
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
			if (wDataSize != sizeof(CMD_C_UserTrustee)) return false;
			
			CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pData;

			m_GameClientView.SetUserTrustee( SwitchViewChairID( pUserTrustee->wUserChairID ), pUserTrustee->bTrustee );

			//显示托管
			if ( pUserTrustee->wUserChairID != GetMeChairID() || IsLookonMode() ) {
				
				CString strMsg;
				if ( pUserTrustee->bTrustee )
					strMsg.Format( TEXT( "[ %s ] 玩家选择了托管" ), GetTableUserItem( pUserTrustee->wUserChairID )->GetNickName() );
				else
					strMsg.Format( TEXT( "[ %s ] 玩家取消了托管" ), GetTableUserItem( pUserTrustee->wUserChairID )->GetNickName() );
				if(m_pIStringMessage != NULL)
					m_pIStringMessage->InsertSystemString( strMsg );

			}
			
			return true;
		}
	case SUB_S_MING_CARD:		//明牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_MingCard));
			if (wDataSize != sizeof(CMD_S_MingCard)) return false;

			CMD_S_MingCard *pMingCard = ( CMD_S_MingCard * )pData;

			m_bMingCard[pMingCard->wMingUser]=pMingCard->bMingCardStatus;

			//设置倍数
			m_wBombTime=pMingCard->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			return true;
		}
	case SUB_S_QIANG_LAND:		//抢地主
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_QiangLand));
			if (wDataSize != sizeof(CMD_S_QiangLand)) return false;

			CMD_S_QiangLand *pQiangLand = ( CMD_S_QiangLand * )pData;

			//设置界面
			//m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
			//m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
			m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
			m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);

			if(pQiangLand->bQiangLand != 0)
				m_GameClientView.SetLandScore(INVALID_CHAIR,0);

			//设置倍数
			m_wBombTime=pQiangLand->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			//设置时间
			KillGameClock(IDI_LAND_SCORE);
			KillGameClock(IDI_QIANG_LAND);

			if(pQiangLand->wQiangUser == GetMeChairID() && (IsLookonMode() == false))
			{
				m_GameClientView.m_btQiangLand.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoQiang.ShowWindow(SW_SHOW);
			}

			//设置时间
			SetGameClock(pQiangLand->wQiangUser,IDI_QIANG_LAND,30);

			return true;
		}
	case SUB_S_MING_OUT:		//出牌明牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_MingCard));
			if (wDataSize != sizeof(CMD_S_MingCard)) return false;

			CMD_S_MingCard *pMingCard = ( CMD_S_MingCard * )pData;

			m_bMingCard[pMingCard->wMingUser]=pMingCard->bMingCardStatus;

			m_GameClientView.m_HandCardControl[SwitchViewChairID(pMingCard->wMingUser)].SetDisplayFlag(true) ;

			//设置倍数
			m_wBombTime=pMingCard->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			return true;
		}
	case SUB_S_ADD:				//加倍
		{
			return OnSubAdd(pData,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置界面
			m_GameClientView.SetBaseScore(pStatusFree->lBaseScore);
			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);
			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				//
				m_GameClientView.m_btMingCardState.ShowWindow(SW_SHOW);
			}
			else
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}

			//设置扑克
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//设置时间
			SetGameClock(GetMeChairID(),IDI_START_GAME,30);

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusFree->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusFree->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	case GS_WK_SCORE:	//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

			//设置变量
			m_bHandCardCount=17;
			for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=17;
			CopyMemory(m_bHandCardData,pStatusScore->bCardData[GetMeChairID()],m_bHandCardCount);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);
				m_GameClientView.SetLandScore(wViewChairID,pStatusScore->bScoreInfo[i]);
			}
			m_GameClientView.ShowLandTitle(true);
			m_GameClientView.SetBaseScore(pStatusScore->lBaseScore);

			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);

			//设置倍数
			m_wBombTime=pStatusScore->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			//按钮控制
			if ((IsLookonMode()==false)&&(pStatusScore->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btThreeScore.ShowWindow( SW_SHOW );
				m_GameClientView.m_btThreeScore.EnableWindow(pStatusScore->bLandScore<=2?TRUE:FALSE);
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else 
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置扑克
			BYTE bCardData[3]={0,0,0};
			m_GameClientView.m_BackCardControl.SetCardData(bCardData,3);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusScore->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusScore->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(i == GetMeChairID())continue;
				if(pStatusScore->bMingCardStatus[i] != 0)
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(pStatusScore->bCardData[i],17);
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
				}
				else
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(cbCardData,17);
				}
			}

			//设置时间
			SetGameClock(pStatusScore->wCurrentUser,IDI_LAND_SCORE,30);

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusScore->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusScore->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnOutType=m_GameLogic.TransListToInt(m_bTurnOutType,m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount));
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData[GetMeChairID()],m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
			}

			//赖子牌
			m_GameLogic.SetLaiZiCard(pStatusPlay->bLaiZiCard);
			m_GameClientView.SetLaiZiCard(m_GameLogic.GetCardLogicValue(pStatusPlay->bLaiZiCard));
			m_GameClientView.SetLaiZiDisplay(true);

			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);
			m_GameClientView.SetBombTime(pStatusPlay->wBombTime);
			m_GameClientView.SetBaseScore(pStatusPlay->lBaseScore);
			m_GameClientView.m_BackCardControl.SetCardData(pStatusPlay->bBackCard,3);
			m_GameClientView.m_BackCardControl.SetDisplayFlag( true );
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
			m_GameClientView.SetLandUser(SwitchViewChairID(pStatusPlay->wLandUser),pStatusPlay->bLandScore);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(i == GetMeChairID())continue;
				if(pStatusPlay->bMingCardStatus[i] != 0)
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(pStatusPlay->bCardData[i],pStatusPlay->bCardCount[i]);
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
				}
				else
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(cbCardData,17);
				}
			}

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusPlay->bUserTrustee[ wChairID ] );

				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() &&pStatusPlay->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置定时器
			SetGameClock(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			//设置扑克
			m_GameClientView.m_HandCardControl[1].SetPositively(IsLookonMode()==false);
			//m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag(true);

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pStatusPlay->lTurnScore[i];
				pHistoryScore->lCollectScore=pStatusPlay->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	case GS_WK_QIANG_LAND: //抢地主
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusQiangLand)) return false;
			CMD_S_StatusQiangLand * pQiangLand=(CMD_S_StatusQiangLand *)pData;

			//设置变量
			m_bHandCardCount=17;
			for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=17;
			CopyMemory(m_bHandCardData,pQiangLand->bCardData[GetMeChairID()],m_bHandCardCount);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);
				m_GameClientView.SetLandScore(wViewChairID,pQiangLand->bScoreInfo[i]);
			}
			m_GameClientView.ShowLandTitle(true);
			m_GameClientView.SetBaseScore(pQiangLand->lBaseScore);

			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);

			//设置倍数
			m_wBombTime=pQiangLand->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			//按钮控制
			if ((IsLookonMode()==false)&&(pQiangLand->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btQiangLand.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoQiang.ShowWindow(SW_SHOW);
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else 
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置扑克
			BYTE bCardData[3]={0,0,0};
			m_GameClientView.m_BackCardControl.SetCardData(bCardData,3);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pQiangLand->bUserTrustee[ wChairID ] );

				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pQiangLand->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(i == GetMeChairID())continue;
				if(pQiangLand->bMingCardStatus[i] != 0)
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(pQiangLand->bCardData[i],17);
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
				}
				else
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(cbCardData,17);
				}
			}

			//设置时间
			SetGameClock(pQiangLand->wCurrentUser,IDI_QIANG_LAND,30);

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pQiangLand->lTurnScore[i];
				pHistoryScore->lCollectScore=pQiangLand->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	case GS_WK_ADD:			//加倍
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusAdd)) return false;
			CMD_S_StatusAdd * pAdd=(CMD_S_StatusAdd *)pData;

			//设置变量
			m_bHandCardCount=17;
			for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=17;
			CopyMemory(m_bHandCardData,pAdd->bCardData[GetMeChairID()],m_bHandCardCount);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);
				m_GameClientView.SetLandScore(wViewChairID,pAdd->bScoreInfo[i]);
			}
			m_GameClientView.ShowLandTitle(true);
			m_GameClientView.SetBaseScore(pAdd->lBaseScore);
			
			//赖子
			m_GameLogic.SetLaiZiCard(pAdd->bLaiZiCard);
			m_GameClientView.SetLaiZiCard(m_GameLogic.GetCardLogicValue(pAdd->bLaiZiCard));
			m_GameClientView.SetLaiZiDisplay(true);

			//控制界面
			m_GameClientView.m_btScore.ShowWindow(SW_SHOW);

			//设置倍数
			m_wBombTime=pAdd->wBombTime;
			m_GameClientView.SetBombTime(m_wBombTime);

			//按钮控制
			if ((IsLookonMode()==false)&&(pAdd->wCurrentUser==GetMeChairID()))
			{
				//加倍
				m_GameClientView.m_btAdd.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoAdd.ShowWindow(SW_SHOW);
				if(pAdd->bAddScore[GetMeChairID()] == false)
				{
					m_GameClientView.m_btAdd.EnableWindow(false);
					m_GameClientView.m_btNoAdd.EnableWindow(false);
				}
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else 
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置扑克
			BYTE bCardData[3]={0,0,0};
			m_GameClientView.m_BackCardControl.SetCardData(bCardData,3);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pAdd->bUserTrustee[ wChairID ] );

				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pAdd->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(i == GetMeChairID())continue;
				if(pAdd->bMingCardStatus[i] != 0)
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(pAdd->bCardData[i],17);
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
				}
				else
				{
					m_GameClientView.m_HandCardControl[SwitchViewChairID(i)].SetCardData(cbCardData,17);
				}
			}

			//设置时间
			SetGameClock(pAdd->wCurrentUser,IDI_ADD,30);

			//历史成绩
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				//获取变量
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置积分
				pHistoryScore->lTurnScore=pAdd->lTurnScore[i];
				pHistoryScore->lCollectScore=pAdd->lCollectScore[i];

				//绑定设置
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetHistoryScore(wViewChairID,pHistoryScore);
			}

			return true;
		}
	}

	return false;
}

//发送扑克
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard)) return false;

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pBuffer;

	//关闭成绩
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_ScoreView.DestroyWindow();
	}

	//设置数据
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_bHandCardCount=17;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(i == GetMeChairID())
			CopyMemory(m_bHandCardData,pSendCard->bCardData[GetMeChairID()],sizeof(pSendCard->bCardData[GetMeChairID()]));
		CopyMemory(m_bHandCardUser[i],pSendCard->bCardData[i],sizeof(pSendCard->bCardData[i]));
	}

	for (WORD i=0;i<GAME_PLAYER;i++) 
		m_bCardCount[i]=CountArray(pSendCard->bCardData);

	for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		m_bCardCount[wChairID] = 17 ;
	}

	//明牌数据
	CopyMemory(m_bMingCard,pSendCard->bMingCard,sizeof(pSendCard->bMingCard));

	//倍数
	m_wBombTime=pSendCard->wBombTime;
	m_GameClientView.SetBombTime(m_wBombTime);

	//赖子牌
	m_bLaiZiData=pSendCard->bLaiZiCard;
	
	
	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetLandScore(i,0);
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}

	//设置界面
	m_GameClientView.ShowLandTitle(true);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);

	//设置扑克
	BYTE bBackCard[3]={0,0,0};
	m_GameClientView.m_BackCardControl.SetCardData(bBackCard,CountArray(bBackCard));
	m_GameClientView.m_HandCardControl[1].SetDisplayFlag(((IsLookonMode()==false)||(IsAllowLookon()==true)));

	//排放扑克
	DispatchUserCard(pSendCard->bCardData,m_bHandCardCount);
	m_GameLogic.SortCardListEx(m_bHandCardData,m_bHandCardCount,ST_ORDER);
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardListEx(m_bHandCardUser[i],m_bCardCount[i],ST_ORDER);
	}

#ifdef VIDEO_GAME
	//隐藏扑克
	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
#endif

	return true;
}

//用户叫分
bool CGameClientEngine::OnSubLandScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;

	//消息处理
	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;

	//停止发牌
	DoncludeDispatchCard();

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
	m_GameClientView.SetLandScore(wViewChairID,pLandScore->bLandScore);

	//玩家设置
	if ((IsLookonMode()==false)&&(pLandScore->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
		
		////m_GameClientView.m_btOneScore.ShowWindow( SW_SHOW );
		//m_GameClientView.m_btTwoScore.ShowWindow( SW_SHOW );
		m_GameClientView.m_btThreeScore.ShowWindow( SW_SHOW );

		//m_GameClientView.m_btOneScore.EnableWindow(pLandScore->bCurrentScore==0?TRUE:FALSE);
		//m_GameClientView.m_btTwoScore.EnableWindow(pLandScore->bCurrentScore<=1?TRUE:FALSE);
		m_GameClientView.m_btThreeScore.EnableWindow(pLandScore->bCurrentScore<=2?TRUE:FALSE);
	}

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("CALL_CARD"));

	//设置时间
	SetGameClock(pLandScore->wCurrentUser,IDI_LAND_SCORE,30);

	return true;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//删除时间
	KillGameClock(IDI_QIANG_LAND);

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置变量
	//m_wBombTime=1;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wLandUser=pGameStart->wLandUser;
	m_bCardCount[pGameStart->wLandUser]=20;
	m_wCurrentUser=pGameStart->wCurrentUser;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

	//关闭成绩
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_ScoreView.DestroyWindow();
	}

	//设置控件
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.m_BackCardControl.SetCardData(pGameStart->bBackCard,CountArray(pGameStart->bBackCard));
	m_GameClientView.m_BackCardControl.SetDisplayFlag( true );

	//设置界面
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0);
	m_GameClientView.SetCardCount(SwitchViewChairID(pGameStart->wLandUser),m_bCardCount[pGameStart->wLandUser]);

	//设置倍数
	m_wBombTime=pGameStart->wBombTime;
	m_GameClientView.SetBombTime(m_wBombTime);

	//地主设置
	if (pGameStart->wLandUser==GetMeChairID())
	{
		BYTE bCardCound=m_bHandCardCount;
		m_bHandCardCount+=CountArray(pGameStart->bBackCard);
		CopyMemory(&m_bHandCardData[bCardCound],pGameStart->bBackCard,sizeof(pGameStart->bBackCard));
		m_GameLogic.SortCardListEx(m_bHandCardData,m_bHandCardCount,ST_ORDER);
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
		m_GameClientView.m_HandCardControl[1].SetNoChangeCard(m_bHandCardData,m_bHandCardCount);
	}
	else
	{
		CopyMemory(&m_bHandCardUser[pGameStart->wLandUser][17],pGameStart->bBackCard,sizeof(pGameStart->bBackCard));
		m_GameLogic.SortCardListEx(m_bHandCardUser[pGameStart->wLandUser],m_bCardCount[pGameStart->wLandUser],ST_ORDER);
		m_GameClientView.m_HandCardControl[SwitchViewChairID(pGameStart->wLandUser)].SetCardData(m_bHandCardUser[pGameStart->wLandUser],m_bCardCount[pGameStart->wLandUser]);
	}

	//设置界面
	m_GameClientView.SetLandUser(SwitchViewChairID(pGameStart->wLandUser),pGameStart->bLandScore);

	//设置数据
	CopyMemory(m_bAddScore,pGameStart->bAddScore,sizeof(pGameStart->bAddScore));
	m_bIsAdd=pGameStart->bIsAdd;

	//动画
	SetTimer(IDI_LAIZI,100,NULL);

	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	//效验数据
	ASSERT(wDataSize>=wHeadSize);
	if (wDataSize<wHeadSize) return false;
	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameClock(IDI_OUT_CARD);
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//动画
	m_GameClientView.SetLaiZiKuang(false);

	//加倍信息
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_GameClientView.SetAddStates(i,false);

	//排列扑克
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardListEx(m_bHandCardUser[i],m_bCardCount[i],ST_ORDER);
	}


	for(BYTE i=0;i<pOutCard->bCardCount;i++)
	{
		if(pOutCard->bCardData[i] == NULL)
			ASSERT(FALSE);
		if(pOutCard->bChangeCard[i] == NULL)
			ASSERT(FALSE);
	}
	//出牌设置
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bChangeCard,pOutCard->bCardCount);
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetNoChangeCard(pOutCard->bCardData,pOutCard->bCardCount);
	}

	//清理桌面
	if (m_bTurnCardCount==0)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if (i==pOutCard->wOutCardUser) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//记录出牌
	memset(m_bChangeTurnOut,0,sizeof(m_bChangeTurnOut));
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);
	CopyMemory(m_bChangeTurnOut,pOutCard->bChangeCard,sizeof(BYTE)*pOutCard->bCardCount);

	//炸弹判断
	if ((m_bTurnOutType&(1<<CT_BOMB_CARD)) != 0||(m_bTurnOutType&(1<<CT_MISSILE_CARD))!= 0||(m_bTurnOutType&(1<<CT_RUAN_BOMB))!= 0||(m_bTurnOutType&(1<<CT_LAIZI_BOMB))!= 0)
	{
		if((m_bTurnOutType&(1<<CT_RUAN_BOMB))!= 0)
			m_wBombTime*=2;
		else
			m_wBombTime*=4;

		m_GameClientView.SetBombTime(m_wBombTime);

		//播放动画
		m_GameClientView.SetBombEffect(true);
	}

	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.SetCardCount(wOutViewChairID,m_bHandCardCount);
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//界面设置
	if(pOutCard->wOutCardUser!=GetMeChairID())
	{
		BYTE bSourceCount=m_bCardCount[pOutCard->wOutCardUser];
		m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
		m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);

		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardUser[pOutCard->wOutCardUser],bSourceCount);
		m_GameClientView.m_HandCardControl[wOutViewChairID].SetCardData(m_bHandCardUser[pOutCard->wOutCardUser],m_bCardCount[pOutCard->wOutCardUser]);
	}

	//最大判断
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		m_wMostUser=pOutCard->wCurrentUser;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetPassFlag(wViewChairID,true);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		//炸弹判断
		if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
		else
			PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	}

	//玩家设置
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btPassCard.EnableWindow(TRUE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
	}

	//设置时间
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
		SetGameClock(pOutCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}

	return true;
}

//放弃出牌
bool CGameClientEngine::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameClock(IDI_OUT_CARD);

	//玩家设置
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))
	{
		WORD wViewChairID=SwitchViewChairID(pPassCard->wPassUser);
		m_GameClientView.SetPassFlag(wViewChairID,true);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//一轮判断
	if (pPassCard->bNewTurn==TRUE)
	{
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	}

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pPassCard->wCurrentUser);
	m_GameClientView.SetPassFlag(wViewChairID,false);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btAutoOutCard.EnableWindow( TRUE );
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	//设置时间
	if (m_bTurnCardCount!=0)
	{
		BYTE bCardCount=m_bCardCount[pPassCard->wCurrentUser];
		SetGameClock(pPassCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}
	else SetGameClock(pPassCard->wCurrentUser,IDI_OUT_CARD,30);

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

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameClock(IDI_OUT_CARD);
	KillGameClock(IDI_LAND_SCORE);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAIZI);
	KillGameClock(IDI_QIANG_LAND);
	KillGameClock(IDI_ADD);

	//停止发牌
	DoncludeDispatchCard();

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSortCard.EnableWindow(FALSE);
	m_GameClientView.m_btMingCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btQiangLand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoQiang.ShowWindow(SW_HIDE);
	m_GameClientView.m_btMingOut.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAdd.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoAdd.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//创建成绩
	if (m_GameClientView.m_ScoreView.m_hWnd==NULL)
	{
		m_GameClientView.m_ScoreView.Create(IDD_GAME_SCORE,&m_GameClientView);
	}

	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString( TEXT( " \n本局结束，成绩统计：" ));

	//设置积分
	CString strScore;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pClientUserItem->GetNickName(),pGameEnd->lGameScore[i],pGameEnd->bUserAdd,pGameEnd->iOtherAdd,
			pGameEnd->wPlayTime[GetMeChairID()],pGameEnd->lGameResult[GetMeChairID()]);
		strScore.Format( TEXT( " \n%s ：%I64d" ), pClientUserItem->GetNickName(),pGameEnd->lGameScore[i]);
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertNormalString( strScore);
	}

	////游戏类型
	//IClientKernel *pIClientKernel = ( IClientKernel * )GetClientKernel( IID_IClientKernel, VER_IClientKernel );
	//const tagServerAttribute *pServerAttribute  = pIClientKernel->GetServerAttribute();

	//if ( pServerAttribute->wGameGenre == GAME_GENRE_GOLD ) {
	//	strScore.Format( TEXT( "扣税：%I64d" ), pGameEnd->lGameTax);
	//	InsertShtickString( strScore);
	//
	//}

	//m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);

	//创建成绩
	CRect scoreRect, gameViewRect ;
	m_GameClientView.GetWindowRect( gameViewRect ) ;
	m_GameClientView.m_ScoreView.GetWindowRect( scoreRect ) ;
	m_GameClientView.m_ScoreView.MoveWindow( gameViewRect.left + ( gameViewRect.Width() - scoreRect.Width() ) / 2, 
		gameViewRect.top + ( gameViewRect.Height() - scoreRect.Height() ) / 2-20, scoreRect.Width(), scoreRect.Height()) ;
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//显示扑克
	BYTE cbCardPos=0 ;
	for(BYTE i=0; i<GAME_PLAYER; ++i)
	{
		WORD wViewID = SwitchViewChairID(i) ;
		m_GameClientView.m_HandCardControl[wViewID].SetCardData(pGameEnd->bCardData+cbCardPos, pGameEnd->bCardCount[i]) ;
		m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
		cbCardPos += pGameEnd->bCardCount[i] ;
	}

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
		m_GameClientView.m_btMingCardState.ShowWindow(SW_SHOW);
		SetGameClock(GetMeChairID(),IDI_START_GAME,30);

		//托管功能
		 m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		OnAutoPlay( false, 0 );
	}
	m_GameClientView.ShowLandTitle(false);

#ifdef VIDEO_GAME
	//显示扑克
	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_SHOW );
	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_SHOW );
#endif

	//用户成绩
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if (pClientUserItem==NULL) continue;

		//设置成绩
		m_HistoryScore.OnEventUserScore(i,pGameEnd->lGameScore[i]);
	}
	return true;
}

//加倍
bool CGameClientEngine::OnSubAdd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Add));
	if (wDataSize != sizeof(CMD_S_Add)) return false;

	CMD_S_Add *pAdd = ( CMD_S_Add * )pBuffer;

	KillGameClock(IDI_ADD);

	//设置界面
	m_GameClientView.SetLaiZiKuang(false);
	if(pAdd->wCurrentUser == GetMeChairID() && IsLookonMode() == false)
	{
		m_GameClientView.m_btAdd.ShowWindow(SW_SHOW);
		m_GameClientView.m_btNoAdd.ShowWindow(SW_SHOW);
	}
	if(pAdd->bIsAdd == false)
	{
		m_GameClientView.m_btAdd.EnableWindow(false);
		//m_GameClientView.m_btNoAdd.EnableWindow(false);
	}
	else
	{
		m_GameClientView.m_btAdd.EnableWindow(true);
		m_GameClientView.m_btNoAdd.EnableWindow(true);
	}

	//结束加倍
	if(pAdd->wCurrentUser == INVALID_CHAIR && m_wLandUser == GetMeChairID() && IsLookonMode() == false)
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);

		//未明牌
		if(m_bMingCard[GetMeChairID()] == 0)
			m_GameClientView.m_btMingOut.ShowWindow(SW_SHOW);
	}
	//加倍信息
	m_GameClientView.SetAddStates(SwitchViewChairID(pAdd->wAddUser),true);
	m_GameClientView.SetAdd(SwitchViewChairID(pAdd->wAddUser),pAdd->bAdd);

	//设置时间
	if(pAdd->wCurrentUser == INVALID_CHAIR)
		SetGameClock(m_wLandUser,IDI_OUT_CARD,30);
	else if(pAdd->bIsAdd == false)
		SetGameClock(pAdd->wCurrentUser,IDI_ADD,3);
	else
		SetGameClock(pAdd->wCurrentUser,IDI_ADD,30);

	return true;
}
//出牌判断
bool CGameClientEngine::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[20];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//牌型扑克
		m_GameLogic.SortCardListEx(bCardData, bShootCount, ST_ORDER);

		//分析类型
		int bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);
		m_GameLogic.ChangeCard(bCardType,bCardData,bShootCount,m_bChangeCard);

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		if(m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bShootCount))
		{
			m_GameLogic.ChangeCard(bCardType,bCardData,bShootCount,m_bChangeCard);
			return true;
		}
		else
			return false;
	}

	return false;
}

//自动出牌
bool CGameClientEngine::AutomatismOutCard()
{
	//先出牌者
	if (m_bTurnCardCount==0)
	{
		//控制界面
		KillGameClock(IDI_OUT_CARD);
		m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[m_bHandCardCount-1];
		SendSocketData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
	}
	else OnPassCard(0,0);

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

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID = SwitchViewChairID(i) ;
			if (wViewChairID!=1)
			{
				BYTE cbCardData[20] ;
				ZeroMemory(cbCardData, sizeof(cbCardData)) ;
				//m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,17);
			}
			else m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount); 
		}

		//设置变量
		m_cbRemnantCardCount=0;
		m_cbDispatchCardCount=0;
		ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));
	}

	return true;
}

//赖子动画停止
bool CGameClientEngine::DoncludeFrame()
{
	//设置界面
	m_GameClientView.SetLaiZiDisplay(true);
	m_GameClientView.m_HandCardControl[1].SetLaiZivalue(m_GameLogic.GetCardValue(m_bLaiZiData));
	m_GameLogic.SetLaiZiCard(m_bLaiZiData);
	m_GameClientView.SetLaiZiCard(m_GameLogic.GetCardLogicValue(m_bLaiZiData));
	m_GameLogic.SortCardListEx(m_bHandCardData,m_bHandCardCount,ST_ORDER);
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
	m_GameClientView.m_HandCardControl[1].SetNoChangeCard(m_bHandCardData,m_bHandCardCount);

	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_UserCardControl[i].SetLaiZivalue(m_GameLogic.GetCardValue(m_bLaiZiData));
	}

	//玩家设置
	m_GameClientView.m_HandCardControl[1].SetPositively( IsLookonMode()==false || IsAllowLookon() );

	//当前玩家
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()) && m_bIsAdd)
	{
		//加倍
		m_GameClientView.m_btAdd.ShowWindow(SW_SHOW);
		m_GameClientView.m_btNoAdd.ShowWindow(SW_SHOW);

		if(m_bAddScore[GetMeChairID()] == false)
		{
			m_GameClientView.m_btAdd.EnableWindow(false);
			//m_GameClientView.m_btNoAdd.EnableWindow(false);
		}
		else
		{
			m_GameClientView.m_btAdd.EnableWindow(true);
			m_GameClientView.m_btNoAdd.EnableWindow(true);
		}
	}

	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()) && !m_bIsAdd)
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
		m_GameClientView.m_btMingOut.ShowWindow(SW_SHOW);
	}

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	if(m_bAddScore[GetMeChairID()] == false && m_bIsAdd == true)
		SetGameClock(m_wCurrentUser,IDI_ADD,3);
	else if(m_bAddScore[GetMeChairID()] == true && m_bIsAdd == true)
		SetGameClock(m_wCurrentUser,IDI_ADD,30);
	else if(m_bIsAdd == false)
		SetGameClock(m_wCurrentUser,IDI_OUT_CARD,30);
	return true;
}
//派发扑克
bool CGameClientEngine::DispatchUserCard(BYTE cbCardData[GAME_PLAYER][20], BYTE cbCardCount)
{
	//设置变量
	m_cbDispatchCardCount=0;
	m_cbRemnantCardCount=cbCardCount;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{ 
		CopyMemory(m_cbDispatchCardData[i],cbCardData[i],sizeof(BYTE)*cbCardCount);
	}
	//if(m_bMingCard[GetMeChairID()] == 0)
		//m_GameClientView.m_btMingCard.ShowWindow(SW_SHOW);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,220,NULL);

	return true;
}

//定时器消息
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	if ((nIDEvent==IDI_MOST_CARD)&&(m_wMostUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wCurrentUser=m_wMostUser;
		m_wMostUser=INVALID_CHAIR;

		//删除定时器
		KillTimer(IDI_MOST_CARD);

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
			m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		}
		//
		m_GameClientView.m_btMingCardState.ShowWindow(SW_HIDE);
		m_GameClientView.m_btMingCard.ShowWindow(SW_HIDE);

		//设置时间
		SetGameClock(wCurrentUser,IDI_OUT_CARD,30);

		return;
	}

	//派发扑克
	if (nIDEvent==IDI_DISPATCH_CARD)
	{
		//派发控制
		if (m_cbRemnantCardCount>0)
		{
			//扑克数据
			BYTE cbUserCard[20];
			ZeroMemory(cbUserCard,sizeof(cbUserCard));

			//派发扑克
			m_cbRemnantCardCount--;
			m_cbDispatchCardCount++;
			//
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				m_GameLogic.SortCardListEx(m_cbDispatchCardData[i],m_cbDispatchCardCount,ST_ORDER);
				m_GameLogic.SortCardListEx(m_bHandCardUser[i],m_bCardCount[i],ST_ORDER);
			}

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_cbDispatchCardData[i],m_cbDispatchCardCount);
					if(m_bMingCard[i] != 0)
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
					else
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false) ;
				}
				else m_GameClientView.m_HandCardControl[1].SetCardData(m_cbDispatchCardData[1],m_cbDispatchCardCount); 
			}

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}

		//中止判断
		if (m_cbRemnantCardCount==0)
		{
			//删除定时器
			KillTimer(IDI_DISPATCH_CARD);

			//扑克数目
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);
			}

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				ActiveGameFrame();
				//m_GameClientView.m_btOneScore.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btOneScore.EnableWindow(TRUE);
				//m_GameClientView.m_btTwoScore.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btTwoScore.EnableWindow(TRUE);
				m_GameClientView.m_btThreeScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btThreeScore.EnableWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.EnableWindow(TRUE);
			}
			//
			m_GameClientView.m_btMingCardState.ShowWindow(SW_HIDE);
			m_GameClientView.m_btMingCard.ShowWindow(SW_HIDE);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					BYTE cbCardData[20] ;
					ZeroMemory(cbCardData, sizeof(cbCardData)) ;
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_cbDispatchCardData[i],17);
					if(m_bMingCard[i] != 0)
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
					else
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false) ;
				}
				else 
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData,m_bHandCardCount); 
					if(false==IsLookonMode())
					{
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
						m_GameClientView.m_HandCardControl[wViewID].SetPositively(true) ;
					}
				}
			}

			if(IsLookonMode()==false) m_GameClientView.m_btSortCard.EnableWindow(TRUE);

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR) SetGameClock(m_wCurrentUser,IDI_LAND_SCORE,30);
		}

		return;
	}


	//赖子动画 
	if(nIDEvent == IDI_LAIZI)
	{
		m_GameClientView.SetLaiZiKuang(true);

		//动画
		BYTE bRand=rand()%52;
		m_GameClientView.SetFrame(bRand%13,bRand%4);

		m_bLaiZiFrame++;

		m_GameClientView.RefreshGameView();

		//结束动画
		if(m_bLaiZiFrame >= 20)
		{
			//删除定时器
			KillTimer(IDI_LAIZI);

			m_GameClientView.SetFrame(m_GameLogic.GetCardValue(m_bLaiZiData)-1,bRand%4);

			DoncludeFrame();
		}
	}
	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wBombTime=1;
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bLaiZiFrame=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	m_bLaiZiData=0;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//设置界面
	KillGameClock(IDI_START_GAME);
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.SetLaiZiCard(0);
	m_GameClientView.SetLaiZiDisplay(false);

	//动画
	m_GameClientView.SetLaiZiKuang(false);

	//明牌开始
	m_GameClientView.m_btMingCardState.ShowWindow(SW_HIDE);

	//加倍信息
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_GameClientView.SetAddStates(i,false);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL) m_GameClientView.m_ScoreView.DestroyWindow();

	//设置扑克
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl[1].SetPositively(false);
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		WORD wViewID = SwitchViewChairID(i) ;
		if(wViewID==1)	m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true);
		else m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false);
	}

	m_GameClientView.m_HandCardControl[1].SetLaiZivalue(m_GameLogic.GetCardValue(NULL));
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_UserCardControl[i].SetLaiZivalue(m_GameLogic.GetCardValue(NULL));
	}

	//发送消息
	SendUserReady(NULL,0);

	////创建成绩
	//if (m_GameClientView.m_ScoreView.m_hWnd==NULL)
	//{
	//	m_GameClientView.m_ScoreView.Create(IDD_GAME_SCORE,&m_GameClientView);
	//	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//}

	return 0;
}

//出牌消息
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	//设置界面
	KillGameClock(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btMingOut.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	CopyMemory(OutCard.bChangeCard,m_bChangeCard,sizeof(m_bChangeCard));
	SendSocketData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	int cbOutCardType = m_GameLogic.GetCardType(OutCard.bCardData,OutCard.bCardCount);
	if ( (cbOutCardType&(1<<CT_BOMB_CARD)) != 0 || (cbOutCardType&(1<<CT_MISSILE_CARD)) != 0 ||(m_bTurnOutType&(1<<CT_RUAN_BOMB))!= 0||(m_bTurnOutType&(1<<CT_LAIZI_BOMB))!= 0) 
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);
	m_GameClientView.m_UserCardControl[1].SetNoChangeCard(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
	m_GameClientView.m_HandCardControl[1].SetNoChangeCard(m_bHandCardData,m_bHandCardCount);

	//排序扑克
	BYTE cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;
	m_GameLogic.SortCardListEx(m_bHandCardData,m_bHandCardCount,cbSortType);
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//放弃出牌
LRESULT CGameClientEngine::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameClock(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

	//发送数据
	SendSocketData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.SetPassFlag(1,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//叫分消息
LRESULT CGameClientEngine::OnLandScore(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameClock(IDI_LAND_SCORE);
	//m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=(BYTE)wParam;
	SendSocketData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	return 0;
}

//出牌提示
LRESULT CGameClientEngine::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult;
	m_GameLogic.SearchOutCard(m_bHandCardData,m_bHandCardCount,m_bTurnCardData,m_bTurnCardCount,OutCardResult,m_bChangeTurnOut);

	//设置界面
	if (OutCardResult.cbCardCount>0)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[1].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);
	}
	else 
	{
		//放弃出牌
		OnPassCard(0,0);
	}

	return 0;
}

//右键扑克
LRESULT CGameClientEngine::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

	return 0;
}

//左键扑克
LRESULT CGameClientEngine::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CGameClientEngine::OnAutoPlay(WPARAM wParam, LPARAM lParam)
{
    bool bAutoPlay = (wParam == 1);
    if (bAutoPlay == m_bAutoPlay)
        return 0;

    m_bAutoPlay = bAutoPlay;

    if (m_bAutoPlay)
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertSystemString(TEXT("使用托管功能, 程序进入自动代打状态!"));
    else
		if(m_pIStringMessage != NULL)
			m_pIStringMessage->InsertSystemString(TEXT("取消托管功能. "));

	//发送消息
	CMD_C_UserTrustee UserTrustee;
	ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
	UserTrustee.bTrustee = m_bAutoPlay;
	UserTrustee.wUserChairID = GetMeChairID();

	SendSocketData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );

    return 0;
}


LRESULT CGameClientEngine::OnMessageSortCard(WPARAM wParam, LPARAM lParam)
{

	//排列扑克
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardListEx(m_bHandCardData,m_bHandCardCount,m_cbSortType);

	//获取扑克
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

	//设置变量
	m_cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;

	//设置按钮
	UINT uImageID=(m_cbSortType==ST_ORDER)?IDB_ORDER_SORT:IDB_COUNT_SORT;
	m_GameClientView.m_btSortCard.SetButtonImage(uImageID,AfxGetInstanceHandle(),false,false);

	//更新界面
	m_GameClientView.RefreshGameView();

	return 0;
}
//
LRESULT CGameClientEngine::OnMingCardState(WPARAM wParam,LPARAM lParam)
{
		//设置变量
	m_wBombTime=1;
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//设置界面
	KillGameClock(IDI_START_GAME);
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.SetLaiZiCard(0);
	m_GameClientView.SetLaiZiDisplay(false);


	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL) m_GameClientView.m_ScoreView.DestroyWindow();

	//设置扑克
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl[1].SetPositively(false);
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		WORD wViewID = SwitchViewChairID(i) ;
		if(wViewID==1)	m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true);
		else m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false);
	}

	//发送消息
	CMD_C_MingState MingState;
	MingState.bMingState=1;
	MingState.wUserChairID=GetMeChairID();

	SendSocketData(SUB_C_MINGSTATE,&MingState,sizeof(MingState));
	SendUserReady(NULL,0);

	return 0;
}

//明牌
LRESULT CGameClientEngine::OnMingCard(WPARAM wParam,LPARAM lParam)
{
	CMD_C_MingState MingState;
	MingState.bMingState=2;
	MingState.wUserChairID=GetMeChairID();

	SendSocketData(SUB_C_MINGSTATE,&MingState,sizeof(MingState));

	return 0;
}

//抢地主
LRESULT CGameClientEngine::OnQiangLand(WPARAM wParam,LPARAM lParam)
{
	CMD_C_QiangLand QiangLand;
	QiangLand.bQiangLand=(bool)wParam;

	SendSocketData(SUB_C_QIANGLAND,&QiangLand,sizeof(QiangLand));

	//设置界面
	m_GameClientView.m_btQiangLand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoQiang.ShowWindow(SW_HIDE);

	return 0;
}
//出牌明牌
LRESULT CGameClientEngine::OnMingOut(WPARAM wParam,LPARAM lParam)
{
	SendSocketData(SUB_C_MINGOUT);
	return 0;
}
//加倍
LRESULT CGameClientEngine::OnAdd(WPARAM wParam,LPARAM lParam)
{
	//设置时间
	KillGameClock(IDI_ADD);

	//设置界面
	m_GameClientView.m_btAdd.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoAdd.ShowWindow(SW_HIDE);

	CMD_C_ADD Add;
	Add.bAdd=(bool)wParam;

	SendSocketData(SUB_C_ADD,&Add,sizeof(Add));
	return 0;
}