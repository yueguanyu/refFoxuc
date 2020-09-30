#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_CHIP					201									//买底定时器
#define IDI_OPERATE_CARD			202									//操作定时器
#define IDI_REPLACE_CARD			301									//补牌定时器

//游戏定时器
#ifndef _DEBUG
#define TIME_START_GAME				30									//开始定时器
#define TIME_CHIP					5									//买底定时器
#define TIME_OPERATE_CARD			20									//操作定时器		
#else
#define TIME_START_GAME				99									//开始定时器
#define TIME_CHIP					20									//买底定时器
#define TIME_OPERATE_CARD			99									//操作定时器	
#endif
#define TIME_REPLACE_CARD			1000								//补牌定时器

#define MAX_TIME_OUT				3									//最大超时次数

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_CHIP,OnChip)
	ON_MESSAGE(IDM_SICE_TWO,OnSiceTwo)
	ON_MESSAGE(IDM_SICE_FINISH,OnSiceFinish)
	ON_MESSAGE(IDM_USER_ACTION,OnUserAction)
	ON_MESSAGE(IDM_MOVE_CARD_FINISH,OnMoveCardFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wReplaceUser = INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_cbListenStatus = 0;
	m_bWillHearStatus=false;
	m_lSiceCount = 0;
	m_bMagicIndex = MAX_INDEX;
	m_wMagicPos = INVALID_CHAIR;
	m_cbHeapMagic = 0;
	m_bTuoPai = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory( m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo) );

	//跟风变量
	m_cbFengCardCount=0;
	ZeroMemory(m_cbFengCardData,sizeof(m_cbFengCardData));

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


	return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wReplaceUser = INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_cbListenStatus = 0;
	m_bWillHearStatus=false;
	m_lSiceCount = 0;
	m_bMagicIndex = MAX_INDEX;
	m_wMagicPos = INVALID_CHAIR;
	m_cbHeapMagic = 0;
	m_bTuoPai = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;

	//跟风变量
	m_cbFengCardCount=0;
	ZeroMemory(m_cbFengCardData,sizeof(m_cbFengCardData));

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
	case IDI_START_GAME:		//开始游戏
		{
			//if( m_bStustee && nElapse < TIME_START_GAME )
			//{
			//	OnStart(0,0);
			//	return true;
			//}
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}
			if (nElapse<=5)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_CHIP:				//买底定时器
		{
			if(!IsLookonMode() && m_bStustee && nElapse < TIME_CHIP)
			{
				OnChip(0,0);
				return true;
			}
			if ( !IsLookonMode() && nElapse == 0)
			{
				OnChip(0,0);
				return true;
			}
			if (nElapse <= 5)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			//自动出牌
			bool bAutoOutCard=false;
			if ((bAutoOutCard==true)&&(m_GameClientView.m_ControlWnd.IsWindowVisible()))
				bAutoOutCard=false;
			if((bAutoOutCard==false)&&(m_bStustee==true))
			{
				bAutoOutCard=true;
			}

			//超时判断
			if ( (IsLookonMode()==false)&&
				( nElapse==0 || bAutoOutCard || (m_cbListenStatus&&!m_bWillHearStatus&&nElapse<TIME_OPERATE_CARD) ) )
			{
				//获取位置
				WORD wMeChairID=GetMeChairID();

				//动作处理
				if (wChairID==wMeChairID)
				{
					//玩家未听牌,且未托管,则累加超时次数
					if( m_cbListenStatus == 0 && m_bStustee==false && ++m_wTimeOutCount>=MAX_TIME_OUT )
					{
						m_wTimeOutCount = 0;
						OnStusteeControl(TRUE,0);
						if(m_pIStringMessage!=NULL)
							m_pIStringMessage->InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));
					}

					if (m_wCurrentUser==wMeChairID)
					{
						//获取扑克
						BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();

						//听牌状态
						if( m_cbListenStatus >= 2 )
						{
							if( m_cbActionMask & WIK_CHI_HU )
							{
								OnUserAction( WIK_CHI_HU,0 );
								return true;
							}
						}
						//如果在操作模式下
						if( m_GameClientView.m_HandCardControl.GetSelectMode() )
						{
							OnCardOperate(0,0);
							return true;
						}
						//出牌效验
						if (VerdictOutCard(cbCardData,m_cbFengCardData,m_cbFengCardCount)==false)
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								//出牌效验
								if (m_cbCardIndex[i]==0) continue;
								if (CheckOutCard(m_GameLogic.SwitchToCardData(i),m_cbFengCardData,m_cbFengCardCount) != 0) 
									continue;

								//设置变量
								cbCardData=m_GameLogic.SwitchToCardData(i);
							}
						}

						//出牌动作
						ASSERT( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbCardData)] > 0 );
						LRESULT bOutResult = OnOutCard(cbCardData, cbCardData);
						if (bOutResult == 2)
						{
							BYTE bCard[13] = {};
							BYTE bCardCount = m_GameLogic.SwitchToCardData(m_cbCardIndex, bCard);
							for (int i = bCardCount-1; i >= 0; i--)
							{
								bOutResult = OnOutCard(bCard[i], bCard[i]);
								if (bOutResult != 2)
									break;
								ASSERT(i!=0);
							}
						}
					}
					else 
					{
						//听牌状态,点炮或自摸和点炮
						if( m_cbListenStatus == 1 || m_cbListenStatus == 3 )
						{
							if( m_cbActionMask & WIK_CHI_HU )
							{
								OnUserAction( WIK_CHI_HU,0 );
								return true;
							}
						}
						//如果在操作模式下
						if( m_GameClientView.m_HandCardControl.GetSelectMode() )
						{
							OnCardOperate(0,0);
							return true;
						}
						OnUserAction(WIK_NULL,0);
					}
				}

				return true;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}

	return false;
}

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//扑克控制
	m_GameClientView.m_HandCardControl.SetDisplayItem(IsAllowLookon());
	m_GameClientView.InvalidGameView(0,0,0,0);
	return true;
}

//游戏消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//停止色子动画
	m_GameClientView.m_DrawSiceWnd.StopSicing();
	switch (wSubCmdID)
	{
	case SUB_S_CHIP:			//用户买底
		{
			return OnSubChip(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			StopMoveCard();
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			StopMoveCard();
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			StopMoveCard();
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pData,wDataSize);
		}
	case SUB_S_LISTEN:			//玩家听牌
		{
			return OnSubListen( pData,wDataSize );
		}
	case SUB_S_REPLACE_CARD:	//补花
		{
			return OnSubReplaceCard(pData,wDataSize);
		}
	case SUB_S_CHIP_RESULT:		//买底结果
		{
			return OnSubChipResult(pData,wDataSize);
		}
	}

	return true;
}

//场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
		switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			////是否启用声音
			//if( !IsEnableSound() )
			//{
			//	for( BYTE i = 0; i < CountArray(m_DirectSound); i++ )
			//		m_DirectSound[i].EnableSound(FALSE);
			//}

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);

			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusFree->bTrustee[i]);
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				if(GetMeUserItem()->GetUserStatus()!=US_READY)
				{
					m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
					m_GameClientView.m_btStart.SetFocus();
					SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
				}
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
			}

			//丢弃效果
			m_GameClientView.SetDiscUser(INVALID_CHAIR);
			if(m_pIStringMessage!=NULL){
				TCHAR szCellScore[32]=TEXT("");
				_sntprintf(szCellScore,sizeof(szCellScore),TEXT("游戏底分：%I64d"),pStatusFree->lCellScore);
				m_pIStringMessage->InsertSystemString(szCellScore);
			}
			//更新界面
			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	case GAME_SCENE_CHIP:	//买底状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusChip)) return false;
			CMD_S_StatusChip * pStatusChip=(CMD_S_StatusChip *)pData;

			////是否启用声音
			//if( !IsEnableSound() )
			//{
			//	for( BYTE i = 0; i < CountArray(m_DirectSound); i++ )
			//		m_DirectSound[i].EnableSound(FALSE);
			//}

			//设置数据
			m_wBankerUser=pStatusChip->wBankerUser;
			m_GameClientView.SetCellScore(pStatusChip->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);

			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusChip->bTrustee[i]);
			}
			
			//模拟消息 (买底消息)
			if (IsLookonMode()==false && !pStatusChip->bHaveMaiDi[GetMeChairID()])
			{
				CMD_S_Chip chip = {};
				chip.cbLianZhuangCount = pStatusChip->cbLianZhuangCount;
				chip.wBankerUser = pStatusChip->wBankerUser;

				if (!OnSubChip((void*)&chip, sizeof(chip)))
					return false;
			}

			//模拟消息 (买底结果消息)
			m_GameClientView.SetMaiDiState(true);
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (pStatusChip->bHaveMaiDi)
				{
					//模拟消息
					CMD_S_Chip_Result chipResult = {};
					chipResult.bTotal = false;
					chipResult.wMaiDiUser = i;
					chipResult.bMaidi[i] = pStatusChip->bMaiDi[i];

					if (!OnSubChipResult((void*)&chipResult, sizeof(chipResult)))
						return false;
				}
			}

			//丢弃效果
			m_GameClientView.SetDiscUser(INVALID_CHAIR);
			if(m_pIStringMessage!=NULL){
				TCHAR szCellScore[32]=TEXT("");
				_sntprintf(szCellScore,sizeof(szCellScore),TEXT("游戏底分：%I64d"),pStatusChip->lCellScore);
				m_pIStringMessage->InsertSystemString(szCellScore);

			}
			//更新界面
			m_GameClientView.InvalidGameView(0,0,0,0);

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			////是否启用声音
			//if( !IsEnableSound() )
			//{
			//	for( BYTE i = 0; i < CountArray(m_DirectSound); i++ )
			//		m_DirectSound[i].EnableSound(FALSE);
			//}

			//辅助变量
			WORD wMeChairID=GetMeChairID();
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) wViewChairID[i]=SwitchViewChairID(i);

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;
			m_cbListenStatus = pStatusPlay->cbHearStatus[wMeChairID];
			m_wReplaceUser = pStatusPlay->wReplaceUser;
			m_wHeapHead = pStatusPlay->wHeapHead;
			m_wHeapTail = pStatusPlay->wHeapTail;
			m_bMagicIndex = pStatusPlay->bMagicIndex;
			m_wMagicPos = pStatusPlay->wMagicPos;
			m_cbHeapMagic = pStatusPlay->cbHeapMagic;
			m_bTuoPai = pStatusPlay->bTuoPai;

			//设置财神
			m_GameLogic.SetMagicIndex(pStatusPlay->bMagicIndex);
			m_GameClientView.m_HandCardControl.SetMagicCard(m_GameLogic.SwitchToCardData(pStatusPlay->bMagicIndex));

			//用户名称
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				IClientUserItem *pUserItem=GetTableUserItem(i);

				const tagUserInfo * pUserData=(pUserItem==NULL?NULL:pUserItem->GetUserInfo());
				ASSERT( pUserData != NULL );
				if( pUserData == NULL ) 
				{
					m_szAccounts[i][0] = 0;
					continue;
				}
				lstrcpy( m_szAccounts[i],pUserData->szNickName );
			}

			//旁观
			if( IsLookonMode()==true )
				m_GameClientView.m_HandCardControl.SetDisplayItem(IsAllowLookon());

			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusPlay->bTrustee[i]);
			}
			
			if( pStatusPlay->bTrustee[wMeChairID] )
			{
				OnStusteeControl(0,0);
			}

			m_wTimeOutCount=0;
			if(pStatusPlay->bTrustee[wMeChairID])
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
			else
				m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);

			//扑克变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//风牌记录
			CopyMemory(m_cbFengCardData,pStatusPlay->cbFengCardData,sizeof(m_cbFengCardData));
			m_cbFengCardCount=pStatusPlay->cbFengCardCount;

			//界面设置
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);
			m_GameClientView.SetMagicCard(m_bMagicIndex);	

			/*if (!m_GameClientView.m_HeapCard[SwitchViewChairID(m_wMagicPos)].IsEmpty())
				m_GameClientView.m_HeapCard[SwitchViewChairID(m_wMagicPos)].SetCardData(m_cbHeapMagic, m_GameLogic.SwitchToCardData(m_bMagicIndex));
			else
			{
				for (int i = 1; i < GAME_PLAYER; i++)
				{
					WORD wPos = (m_wMagicPos+i)%GAME_PLAYER;
					if  (!m_GameClientView.m_HeapCard[SwitchViewChairID(wPos)].IsEmpty())
					{
						if (m_GameClientView.m_HeapCard[SwitchViewChairID(wPos)].IsNotEmptyPos(m_cbHeapMagic))
							m_GameClientView.m_HeapCard[SwitchViewChairID(wPos)].SetCardData(m_cbHeapMagic, m_GameLogic.SwitchToCardData(m_bMagicIndex));
					}
				}
			}*/
			m_GameClientView.m_HeapCard[SwitchViewChairID(m_wMagicPos)].SetCardData(m_cbHeapMagic, m_GameLogic.SwitchToCardData(m_bMagicIndex));
			
			//玩家花牌
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				WORD wViewChair = SwitchViewChairID(i);
				for (int j = 0; j < 8; j++)
				{
					if (pStatusPlay->cbHuaCardData[i][j] != 0)
						m_GameClientView.SetHuaCard(wViewChair, pStatusPlay->cbHuaCardData[i][j], false);
				}
			}

			//组合扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wOperateViewID = SwitchViewChairID(i);
				for (BYTE j=0;j<m_cbWeaveCount[i];j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=(cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT|WIK_PENG))?3:4;
					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(m_WeaveItemArray[i][j].cbCardData,cbWeaveCardCount);
					if ((cbWeaveKind&WIK_GANG)&&(m_WeaveItemArray[i][j].wProvideUser==i)) 
						m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
					WORD wProviderViewID = SwitchViewChairID(m_WeaveItemArray[i][j].wProvideUser);
					m_GameClientView.m_WeaveCard[wOperateViewID][j].SetDirectionCardPos((BYTE)wProviderViewID*2);

				}
			}

			//用户扑克
			if (m_wCurrentUser==wMeChairID)
			{
				SetHandCardControl(m_cbCardIndex,pStatusPlay->cbSendCardData);
			}
			else
			{
				SetHandCardControl(m_cbCardIndex,0x00);
			}

			//扑克设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//用户扑克
				if (i!=wMeChairID)
				{
					BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[i]*3-1;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}

				//丢弃扑克
				m_GameClientView.m_DiscardCard[wViewChairID[i]].SetCardData(pStatusPlay->cbDiscardCard[i],pStatusPlay->cbDiscardCount[i]);
			}

			//丢弃效果
			if(pStatusPlay->wOutCardUser != INVALID_CHAIR)
			{
				m_GameClientView.SetDiscUser(SwitchViewChairID(pStatusPlay->wOutCardUser));
				m_GameClientView.SetOutCardInfo( wViewChairID[pStatusPlay->wOutCardUser],pStatusPlay->cbOutCardData );
				m_GameClientView.m_DiscardCard[wViewChairID[pStatusPlay->wOutCardUser]].AddCardItem( pStatusPlay->cbOutCardData );
			}

			//控制设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
			}

			//堆立界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetHeapCardInfo(pStatusPlay->cbHeapCardInfo[i][0],pStatusPlay->cbHeapCardInfo[i][1]);
			}

			//听牌状态
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( pStatusPlay->cbHearStatus[i] > 0 )
					m_GameClientView.SetUserListenStatus( wViewChairID[i],true );
			}

			//操作界面
			if ((IsLookonMode()==false)&&(pStatusPlay->cbActionMask!=WIK_NULL))
			{
				//获取变量
				m_cbActionMask=pStatusPlay->cbActionMask;
				m_cbActionCard=pStatusPlay->cbActionCard;

				//设置界面
				if (m_wCurrentUser==INVALID_CHAIR)
					SetGameClock(wMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
				if (IsLookonMode()==false) 
					m_GameClientView.m_ControlWnd.SetControlInfo(m_cbActionMask);
			}

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR)
			{
				//计算时间
				WORD wTimeCount=TIME_OPERATE_CARD;

				//设置时间
				SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);
			}
			else if( m_wReplaceUser == wMeChairID && !IsLookonMode() )
				SetTimer( IDI_REPLACE_CARD,TIME_REPLACE_CARD,NULL );

			//设置圈风
			m_GameClientView.SetFengQuan( pStatusPlay->cbQuanFeng );

			if(m_pIStringMessage!=NULL){
				TCHAR szCellScore[32]=TEXT("");
				_sntprintf(szCellScore,sizeof(szCellScore),TEXT("游戏底分：%I64d"),pStatusPlay->lCellScore);
				m_pIStringMessage->InsertSystemString(szCellScore);

			}

			//更新界面
			m_GameClientView.InvalidGameView(0,0,0,0);

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
	if (wDataSize!=sizeof(CMD_S_GameStart)) 
		return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
	SetGameStatus(GAME_SCENE_PLAY);
	m_GameClientView.SetMaiDiState(false);

	//设置变量
	m_wBankerUser = pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wReplaceUser = pGameStart->wReplaceUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;
	m_cbActionMask = pGameStart->cbUserAction;
	m_lSiceCount = pGameStart->lSiceCount;
	m_bMagicIndex = pGameStart->bMagicIndex;
	m_wMagicPos = pGameStart->wMagicPos;
	m_cbHeapMagic = pGameStart->cbHeapMagic;

	//设置财神
	m_GameLogic.SetMagicIndex(pGameStart->bMagicIndex);
	m_GameClientView.m_HandCardControl.SetMagicCard(m_GameLogic.SwitchToCardData(pGameStart->bMagicIndex));

	//设置扑克
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置堆牌头尾
	m_wHeapHead = pGameStart->wHeapHead;
	m_wHeapTail = pGameStart->wHeapTail;
	CopyMemory( m_cbHeapCardInfo,pGameStart->cbHeapCardInfo,sizeof(m_cbHeapCardInfo) );

	//旁观界面
	if (IsLookonMode()==true)
	{
		//设置变量
		m_cbListenStatus = 0;
		m_bWillHearStatus = false;

		//组合扑克
		ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
		ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

		m_GameClientView.m_GameScore.RestorationData();
		m_GameClientView.SetCurrentUser(INVALID_CHAIR);

		//设置界面
		m_GameClientView.SetDiscUser(INVALID_CHAIR);
		m_GameClientView.SetHuangZhuang(false);
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetBankerUser(INVALID_CHAIR);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.SetUserListenStatus(INVALID_CHAIR,false);

		//扑克设置
		m_GameClientView.m_UserCard[0].SetCardData(0,false);
		m_GameClientView.m_UserCard[1].SetCardData(0,false);
		m_GameClientView.m_UserCard[2].SetCardData(0,false);
		m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

		//扑克设置
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
			for( BYTE j = 0; j < CountArray(m_GameClientView.m_WeaveCard[i]); j++ )
				m_GameClientView.m_WeaveCard[i][j].SetCardData(NULL,0);
		}

		//堆立扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_HeapCard[i].ResetCardData();
			m_GameClientView.m_HeapCard[i].SetHeapCardInfo(0,0);
		}
	}

	//设置界面
	m_GameClientView.SetFengQuan( pGameStart->cbQuanFeng );
	if( WORD(pGameStart->lSiceCount>>16) == 0 )
		m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//播放开始声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//环境处理
	PlayGameSound(AfxGetInstanceHandle(),TEXT("DRAW_SICE"));

	//摇色子动画
	m_GameClientView.m_DrawSiceWnd.SetSiceInfo(m_GameClientView.GetDC(),200,(WORD)(pGameStart->lSiceCount>>16),(WORD)pGameStart->lSiceCount);
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_SHOW);

	return true;
}

//用户买底
bool CGameClientEngine::OnSubChip(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_Chip));
	if (wDataSize!=sizeof(CMD_S_Chip)) return false;

	//设置状态
	SetGameStatus(GAME_SCENE_CHIP);
	m_GameClientView.SetMaiDiState(true);

	CMD_S_Chip * pChip = (CMD_S_Chip*)pBuffer;

	//设置变量
	m_wBankerUser = pChip->wBankerUser;
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//用户名称
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		IClientUserItem *pUserItem=GetTableUserItem(i);
		
		const tagUserInfo * pUserData=(pUserItem==NULL?NULL:pUserItem->GetUserInfo());
		ASSERT( pUserData != NULL );
		if( pUserData == NULL ) 
		{
			m_szAccounts[i][0] = 0;
			continue;
		}
		lstrcpy( m_szAccounts[i],pUserData->szNickName );
	}

	//玩家设置
	if(IsLookonMode()==false)
	{
		//显示买底
		m_GameClientView.m_btScoreOne.ShowWindow(SW_SHOW);
		m_GameClientView.m_btScoreTwo.ShowWindow(SW_SHOW);
		m_GameClientView.m_btScoreNone.ShowWindow(SW_SHOW);
	}

	//设置定时期
	SetGameClock(GetMeChairID(),IDI_CHIP,TIME_CHIP);

	//坐庄情况
	if(m_pIStringMessage!=NULL)
	{
		CString strZhuang;
		if (pChip->cbLianZhuangCount == 0)
			strZhuang.Format(TEXT("玩家 %s 坐庄。"), m_szAccounts[m_wBankerUser]);
		else
			strZhuang.Format(TEXT("玩家 %s 坐庄，连庄%d次。"), m_szAccounts[m_wBankerUser], pChip->cbLianZhuangCount);
		m_pIStringMessage->InsertSystemString(strZhuang);
	}

	return true;
}

//用户出牌
bool CGameClientEngine::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//脱牌设置
	m_bTuoPai = false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//记录风牌
	if((pOutCard->cbOutCardData&MASK_COLOR)==0x30)
	{
		if(m_cbFengCardData[pOutCard->cbOutCardData-0x31]==0)
			m_cbFengCardCount++;
		m_cbFengCardData[pOutCard->cbOutCardData-0x31]=pOutCard->cbOutCardData;
	}

	if( IsLookonMode() || pOutCard->wOutCardUser != GetMeChairID() )
	{
		COutCardItem *pOutCardItem = NULL;
		try
		{
			//创建动画项目
			pOutCardItem = new COutCardItem(pOutCard);
		}
		catch(...)
		{
			ASSERT(FALSE);
			return false;
		}

		//添加动画
		m_MoveCardItemArray.Add(pOutCardItem);

		//播放动画
		BeginMoveCard();
	}

	return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//消息处理
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	CSendCardItem *pSendCardItem = NULL;
	try
	{
		//创建动画项目
		pSendCardItem = new CSendCardItem(pSendCard);
	}
	catch(...)
	{
		ASSERT(FALSE);
		return false;
	}

	//添加动画
	m_MoveCardItemArray.Add(pSendCardItem);

	//播放动画
	BeginMoveCard();

	return true;
}

//补牌消息
bool CGameClientEngine::OnSubReplaceCard(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_ReplaceCard));
	if (wDataSize!=sizeof(CMD_S_ReplaceCard)) return false;

	//变量定义
	CMD_S_ReplaceCard * pReplaceCard=(CMD_S_ReplaceCard *)pBuffer;

	if( IsLookonMode() || pReplaceCard->wReplaceUser != GetMeChairID() )
	{
		CReplaceCardItem *pReplaceCardItem = NULL;
		try
		{
			//创建动画项目
			pReplaceCardItem = new CReplaceCardItem(pReplaceCard);
		}
		catch(...)
		{
			ASSERT(FALSE);
			return false;
		}

		//添加动画
		m_MoveCardItemArray.Add(pReplaceCardItem);

		//播放动画
		BeginMoveCard();
	}

	return true;
}

//买底结果
bool CGameClientEngine::OnSubChipResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Chip_Result));
	if (wDataSize!=sizeof(CMD_S_Chip_Result)) 
		return false;

	CMD_S_Chip_Result *pChipResult = (CMD_S_Chip_Result*)pBuffer;
	
	//定义变量
	CString strInfo;
	BYTE bTrueMaidi = 0;

	if (pChipResult->bTotal)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			bTrueMaidi = pChipResult->bMaidi[i];
			m_GameClientView.SetMaiDiScore(SwitchViewChairID(i), bTrueMaidi);
			if(m_pIStringMessage!=NULL)
			{
				strInfo.Format(TEXT("玩家 %s %s %d"), m_szAccounts[i], (i==m_wBankerUser?TEXT("买底"):TEXT("顶底")), bTrueMaidi);
				m_pIStringMessage->InsertSystemString(strInfo);
			}
		}
	}
	else
	{
		bTrueMaidi = pChipResult->bMaidi[pChipResult->wMaiDiUser];

		//设置界面
		m_GameClientView.SetMaiDiScore(SwitchViewChairID(pChipResult->wMaiDiUser), bTrueMaidi);

		//if (bTrueMaidi == 2 || bTrueMaidi == 4)
		//strInfo.Format(TEXT("玩家%s%s。"), m_szAccounts[pChipResult->wMaiDiUser], (pChipResult->wMaiDiUser==m_wBankerUser?TEXT("买底"):TEXT("顶底")));
		//if(m_pIStringMessage!=NULL)
			//m_pIStringMessage->InsertSystemString(strInfo);
	}

	return true;
}

//操作提示
bool CGameClientEngine::OnSubOperateNotify(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) 
		return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//用户界面
	if ((IsLookonMode()==false)&&(pOperateNotify->cbActionMask!=WIK_NULL))
	{
		//获取变量
		WORD wMeChairID=GetMeChairID();
		m_cbActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard = pOperateNotify->cbActionCard;

		//如果手上牌属于操作模式,则取消
		if( m_GameClientView.m_HandCardControl.GetSelectMode() )
			m_GameClientView.m_HandCardControl.SetSelectMode(false);

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_ControlWnd.SetControlInfo(m_cbActionMask);

		//设置时间
		m_GameClientView.SetCurrentUser(INVALID_CHAIR);
		SetGameClock(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	}

	return true;
}

//操作结果
bool CGameClientEngine::OnSubOperateResult(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) 
		return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard[3];
	CopyMemory( cbOperateCard,pOperateResult->cbOperateCard,sizeof(cbOperateCard) );
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);
	WORD wProviderViewID = SwitchViewChairID(pOperateResult->wProvideUser);

	//设置变量
	m_wCurrentUser = pOperateResult->wOperateUser;
	m_bTuoPai = (m_wCurrentUser==GetMeChairID() && pOperateResult->bTuoPai);

	//设置组合
	if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
	{
		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard[0])&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData[3] = cbCenterCard;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex==0xFF)
		{
			//暗杠判断
			cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

			//设置扑克
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
			for( BYTE i = 0; i < 4; i++ ) m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData[i] = cbOperateCard[0];

			m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDirectionCardPos((BYTE)wProviderViewID*2);
		}

		//组合界面
		BYTE cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=4;
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData,
			cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem((cbPublicCard==TRUE)?true:false);

		//扑克设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbOperateCard[0])]=0;
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			SetHandCardControl( m_cbCardIndex,0x00 );
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,false);
		}
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		CopyMemory( m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCardData,cbOperateCard,sizeof(cbOperateCard) );

		//组合界面
		BYTE cbWeaveCard[3],cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=3;
		CopyMemory( cbWeaveCard,cbOperateCard,sizeof(cbWeaveCard) );
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDirectionCardPos((BYTE)wProviderViewID*2);

		//删除扑克
		if (GetMeChairID()==wOperateUser)
		{
			VERIFY( m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard[0],1) );
			if( !m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1) )
			{
				ASSERT( FALSE );
				return false;
			}
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			SetHandCardControl( m_cbCardIndex,0x00 );
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}
	}

	//设置界面
	//如果提供玩家不是自己,则去除提供玩家的已出牌
	if( pOperateResult->wProvideUser != wOperateUser )
	{
		if( m_GameClientView.m_DiscardCard[wProviderViewID].GetCardCount() > 0 )
			m_GameClientView.m_DiscardCard[wProviderViewID].RemoveCard(1);
	}
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserAction(wOperateViewID,pOperateResult->cbOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);
	//如果手上牌属于操作模式,则取消
	if( m_GameClientView.m_HandCardControl.GetSelectMode() )
		m_GameClientView.m_HandCardControl.SetSelectMode(false);

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	//环境设置
	PlayActionSound(wOperateUser,pOperateResult->cbOperateCode);

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//听牌判断
		if( !IsLookonMode() && m_cbListenStatus == 0 && m_wCurrentUser==GetMeChairID() )
		{
			WORD wMeChairId = GetMeChairID();

			//m_cbActionMask = m_GameLogic.AnalyseTingCard(m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]); //取消听牌

			if( m_cbActionMask != WIK_NULL )
			{
				m_GameClientView.m_ControlWnd.SetControlInfo(m_cbActionMask);
			}
		}

		//计算时间
		WORD wTimeCount=TIME_OPERATE_CARD;

		//设置时间
		m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
		SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);
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

	//设置状态
	SetGameStatus(GAME_SCENE_FREE);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetMaiDiState(false);

	//删除定时器
	KillGameClock(IDI_OPERATE_CARD);
	KillGameClock(IDI_CHIP);

	//设置控件
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetMaiDiScore(INVALID_CHAIR, 0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	m_GameClientView.SetCurrentUser(INVALID_CHAIR);	
	m_GameClientView.m_btScoreNone.ShowWindow(SW_HIDE);
	m_GameClientView.m_btScoreOne.ShowWindow(SW_HIDE);
	m_GameClientView.m_btScoreTwo.ShowWindow(SW_HIDE);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		for( BYTE j = 0; j < CountArray(m_GameClientView.m_WeaveCard[i]); j++ )
			m_GameClientView.m_WeaveCard[i][j].SetDisplayItem(true);
	}

	//变量定义
	tagScoreInfo ScoreInfo;
	tagWeaveInfo WeaveInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ZeroMemory(&WeaveInfo,sizeof(WeaveInfo));

	//成绩变量
	ScoreInfo.wProvideUser=pGameEnd->wProvideUser;
	ScoreInfo.cbProvideCard=pGameEnd->cbProvideCard;

	//设置积分
	CString strTemp ,strEnd = TEXT("本局结束,成绩统计:\r\n");
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		strTemp.Format(TEXT("%s: %I64d\n"),m_szAccounts[i],pGameEnd->lGameScore[i]);
		strEnd += strTemp;
	}
	//消息积分
	if(m_pIStringMessage!=NULL)
		m_pIStringMessage->InsertSystemString(strEnd);

	WORD wFirstWinner = INVALID_CHAIR;
	CChiHuRight chr;
	if( pGameEnd->wProvideUser != INVALID_CHAIR )
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (pGameEnd->dwChiHuKind[i] == WIK_CHI_HU)
			{
				if (wFirstWinner == INVALID_CHAIR)
					wFirstWinner = i;
				else
					ScoreInfo.bMulWinner = true;
			}
		}
		ASSERT(wFirstWinner!=INVALID_CHAIR);
		if (wFirstWinner==INVALID_CHAIR)	wFirstWinner = 0;
		wFirstWinner = (pGameEnd->dwChiHuKind[GetMeChairID()]==WIK_CHI_HU?GetMeChairID():wFirstWinner);

		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//胡牌类型
			ScoreInfo.dwChiHuKind[i]=pGameEnd->dwChiHuKind[i];

			//设置成绩
			ScoreInfo.lGameScore[i]=pGameEnd->lGameScore[i];
			lstrcpyn(ScoreInfo.szUserName[i],m_szAccounts[i],CountArray(ScoreInfo.szUserName[i]));

			//胡牌扑克
			if ((ScoreInfo.cbCardCount==0)&&(pGameEnd->dwChiHuKind[i]!=WIK_NULL))
			{
				ScoreInfo.wChiHuUser = i;
				//组合扑克
				WeaveInfo.cbWeaveCount=m_cbWeaveCount[i];
				for (BYTE j=0;j<WeaveInfo.cbWeaveCount;j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					WeaveInfo.cbPublicWeave[j]=m_WeaveItemArray[i][j].cbPublicCard;
					WeaveInfo.cbCardCount[j]=(cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT|WIK_PENG))?3:4;
					CopyMemory( WeaveInfo.cbCardData[j],m_WeaveItemArray[i][j].cbCardData,sizeof(m_WeaveItemArray[i][j].cbCardData) );
				}

				//设置扑克
				ScoreInfo.cbCardCount=pGameEnd->cbCardCount[i];
				CopyMemory(ScoreInfo.cbCardData,&pGameEnd->cbCardData[i],ScoreInfo.cbCardCount*sizeof(BYTE));

				//提取胡牌
				for (BYTE j=0;j<ScoreInfo.cbCardCount;j++)
				{
					if (ScoreInfo.cbCardData[j]==pGameEnd->cbProvideCard)
					{
						MoveMemory(&ScoreInfo.cbCardData[j],&ScoreInfo.cbCardData[j+1],(ScoreInfo.cbCardCount-j-1)*sizeof(BYTE));
						ScoreInfo.cbCardData[ScoreInfo.cbCardCount-1]=pGameEnd->cbProvideCard;
						break;
					}
				}
			}
		}

		ScoreInfo.cbFanCount = pGameEnd->cbFanCount;
		chr.SetRightData( pGameEnd->dwChiHuRight,CountArray(pGameEnd->dwChiHuRight) );

		//成绩界面
		m_GameClientView.m_GameScore.SetScoreInfo(ScoreInfo,WeaveInfo,chr);
	}
	else m_GameClientView.SetHuangZhuang(true);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (pGameEnd->dwChiHuKind[i]!=WIK_NULL) m_GameClientView.SetUserAction(wViewChairID,WIK_CHI_HU);
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//设置扑克
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//播放声音
	if( !chr.IsEmpty() )
	{
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			if( pGameEnd->dwChiHuKind[i] != WIK_NULL )
			{
				PlayActionSound( i,WIK_CHI_HU );
				break;
			}
		}
	}
	else 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));
	}

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.SetFocus();
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//取消托管
	if(m_bStustee)
		OnStusteeControl(0,0);

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

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

	if ((IsLookonMode()||pTrustee->wChairID!=GetMeChairID())&&m_pIStringMessage!=NULL)
	{
		IClientUserItem *pUserItem = GetTableUserItem(pTrustee->wChairID);
		const tagUserInfo * pUserData=(pUserItem==NULL?NULL:pUserItem->GetUserInfo());
		TCHAR szBuffer[256]=TEXT("");
		if(pTrustee->bTrustee==true)
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]选择了托管功能."),pUserData->szNickName);
		else
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]取消了托管功能."),pUserData->szNickName);
		m_pIStringMessage->InsertSystemString(szBuffer);
	}

	return true;
}

//用户听牌
bool CGameClientEngine::OnSubListen(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Listen));
	if (wDataSize!=sizeof(CMD_S_Listen)) return false;

	CMD_S_Listen *pListen = (CMD_S_Listen *)pBuffer;
	WORD wViewId = SwitchViewChairID( pListen->wChairId );

	//设置听牌状态
	if( IsLookonMode() || pListen->wChairId != GetMeChairID() )
	{
		m_GameClientView.SetUserListenStatus( wViewId,true );
	}

	//播放声音
	PlayActionSound( pListen->wChairId,WIK_LISTEN );

	m_GameClientView.SetOutCardInfo( INVALID_CHAIR,0 );
	m_GameClientView.SetUserAction( wViewId,WIK_LISTEN );

	//设置状态
	m_GameClientView.SetStatusFlag( (IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),m_wCurrentUser!=GetMeChairID() );

	return true;
}

//播放出牌声音
void CGameClientEngine::PlayCardSound(WORD wChairID, BYTE cbCardData)
{
	if(m_GameLogic.IsValidCard(cbCardData) == false)
	{
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}
	//判断性别
	IClientUserItem *pUseItem=GetTableUserItem(wChairID);
	const tagUserInfo * pUserData=pUseItem==NULL?NULL:pUseItem->GetUserInfo();
	
	if(pUserData == NULL)
	{
		assert(0 && "得不到玩家信息");
		return;
	}
	bool bBoy = (pUserData->cbGender == 2 ? false : true);
	BYTE cbType= (cbCardData & MASK_COLOR);
	BYTE cbValue= (cbCardData & MASK_VALUE);
	CString strSoundName;
	switch(cbType)
	{
	case 0X30:	//风
		{
			switch(cbValue) 
			{
			case 1:
				{
					strSoundName = TEXT("F_1");
					break;
				}
			case 2:
				{
					strSoundName = TEXT("F_2");
					break;
				}
			case 3:
				{
					strSoundName = TEXT("F_3");
					break;
				}
			case 4:
				{
					strSoundName = TEXT("F_4");
					break;
				}
			case 5:
				{
					strSoundName = TEXT("F_5");
					break;
				}
			case 6:
				{
					strSoundName = TEXT("F_6");
					break;
				}
			case 7:
				{
					strSoundName = TEXT("F_7");
					break;
				}
			default:
				{
					strSoundName=TEXT("BU_HUA");
				}

			}
			break;
		}		
	case 0X20:	//筒
		{
			strSoundName.Format(TEXT("T_%d"), cbValue);
			break;
		}

	case 0X10:	//索
		{
			strSoundName.Format(TEXT("S_%d"), cbValue);
			break;
		}
	case 0X00:	//万
		{
			strSoundName.Format(TEXT("W_%d"), cbValue);
			break;
		}
	}	

	if(bBoy)
	{
		strSoundName = TEXT("BOY_") +strSoundName;
	}
	else
	{
		strSoundName = TEXT("GIRL_") + strSoundName;
	}
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

//播放声音
void CGameClientEngine::PlayActionSound(WORD wChairID,BYTE cbAction)
{
	//判断性别
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}
	IClientUserItem *pUseItem=GetTableUserItem(wChairID);
	const tagUserInfo * pUserData=pUseItem==NULL?NULL:pUseItem->GetUserInfo();

	if(pUserData == 0)
	{
		assert(0 && "得不到玩家信息");
		return;
	}

	bool bBoy = (pUserData->cbGender == 2 ? false : true);

	switch (cbAction)
	{
	case WIK_LEFT:
	case WIK_CENTER:
	case WIK_RIGHT:		//上牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI"));		
			break;
		}
	case WIK_PENG:		//碰牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_PENG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_PENG"));	
			break;
		}
	case WIK_GANG:		//杠牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_GANG"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_GANG"));		
			break;
		}
	case WIK_CHI_HU:	//吃胡
		{

			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_CHI_HU"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_CHI_HU"));		
			break;
		}
	case WIK_REPLACE:	//替换花牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_REPLACE"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_REPLACE"));		
			break;
		}
	case WIK_LISTEN:	//听牌
		{
			if(bBoy)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOY_TING"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GIRL_TING"));		
			break;
		}
	}

	return;
}

//出牌判断
bool CGameClientEngine::VerdictOutCard(BYTE cbPreCardData,BYTE cbAlreadyCardData[8],BYTE cbFengCardCount)
{
	BOOL bRet = CheckOutCard(cbPreCardData, cbAlreadyCardData, cbFengCardCount);

	ASSERT(bRet >= 0 && bRet <= 3);

	if(m_pIStringMessage!=NULL)
	{
		if (bRet == 1)
			m_pIStringMessage->InsertSystemString(TEXT("不可出财神！"));
		else if (bRet == 2)
			m_pIStringMessage->InsertSystemString(TEXT("必须先出完风牌！"));
		else if (bRet == 3)
			m_pIStringMessage->InsertSystemString(TEXT("必须跟出风牌！"));
	}

	return (bRet==0);
}

//是否可出	(0 可出 1 不可出财神 2 必须先出完风牌 3 必须跟出风牌)
BOOL CGameClientEngine::CheckOutCard(BYTE cbPreCardData,BYTE cbAlreadyCardData[8],BYTE cbFengCardCount)
{
	//财神判断
	if (m_GameLogic.IsMagicCard(cbPreCardData, false))
		return 1;

	//定义变量
	BYTE bPreIndex = m_GameLogic.SwitchToCardIndex(cbPreCardData);
	bool bFendMagic = (m_bMagicIndex>=27 && m_bMagicIndex<=32);

	//跟风判断
	if (cbFengCardCount != 0)
	{
		for (int i = 0; i < (bFendMagic?7:6); i++)
		{
			if (cbAlreadyCardData[i] == 0 || m_GameLogic.IsMagicCard(i, true)) continue;
			if (cbPreCardData == cbAlreadyCardData[i] && m_cbCardIndex[bPreIndex] == 1)
				return 0;
		}

		for (int i = 0; i < (bFendMagic?7:6); i++)
		{		
			if (cbAlreadyCardData[i] == 0 || m_GameLogic.IsMagicCard(i, true)) continue;
			if (m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbAlreadyCardData[i])] == 1)
				return 3;
		}
	}

	//单风判断
	if ( bPreIndex >= 27 && bPreIndex <= (bFendMagic?33:32) && m_cbCardIndex[bPreIndex] == 1 )
		return 0;
	else
	{
		//手上是否有单风
		for (int Index=27; Index<=(bFendMagic?33:32); Index++)
		{
			if (m_GameLogic.IsMagicCard(Index, true))	continue;
			if (m_cbCardIndex[Index]==1)
				return 2;
		}
	}

	return 0;	
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//环境设置
	KillGameClock(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_GameScore.RestorationData();
	m_GameClientView.SetCurrentUser(INVALID_CHAIR);

	//设置界面
	m_GameClientView.SetDiscUser(INVALID_CHAIR);
	m_GameClientView.SetHuangZhuang(false);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.SetUserListenStatus(INVALID_CHAIR,false);
	m_GameClientView.SetHuaCard(0, 0, true);
	
	//扑克设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		for( BYTE j = 0; j < CountArray(m_GameClientView.m_WeaveCard[i]); j++ )
			m_GameClientView.m_WeaveCard[i][j].SetCardData(NULL,0);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HeapCard[i].ResetCardData();
		m_GameClientView.m_HeapCard[i].SetHeapCardInfo(0,0);
	}

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wReplaceUser = INVALID_CHAIR;
	m_cbListenStatus = 0;
	m_bWillHearStatus = false;
	m_cbActionMask = 0;
	m_cbActionCard = 0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	m_cbFengCardCount = 0;
	ZeroMemory(m_cbFengCardData, sizeof(m_cbFengCardData));

	//发送消息
	SendUserReady(NULL,0);


	/*m_GameLogic.SetMagicIndex(1);
	m_GameClientView.m_HandCardControl.SetMagicCard(0x02);
	m_cbCardIndex[1] = 3;
	m_cbCardIndex[2] = 3;
	m_cbCardIndex[3] = 3;
	m_cbCardIndex[4] = 3;
	m_cbCardIndex[5] = 3;
	m_cbCardIndex[33] = 2;
	SetHandCardControl(m_cbCardIndex, 2);*/

	return 0;
}

//出牌操作
LRESULT CGameClientEngine::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//出牌判断
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID()))
		return 0;

	//跟风判断
	BYTE cbOutCardData=(BYTE)wParam;
	if (VerdictOutCard(cbOutCardData,m_cbFengCardData,m_cbFengCardCount)==false)
	{
		if(m_pIStringMessage!=NULL)
			m_pIStringMessage->InsertSystemString(TEXT("出此牌不符合游戏规则!"));
		return 1;
	}

	KillGameClock(IDI_OPERATE_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	if( !m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData) )
	{
		ASSERT( FALSE );
		return 0;
	}

	//设置扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
	ASSERT( (cbCardCount-1)%3==0 );
	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);

	//设置界面
	m_GameClientView.SetCurrentUser( INVALID_CHAIR );
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	if( m_GameClientView.m_DlgListen.m_hWnd )
		m_GameClientView.m_DlgListen.EndDialog(IDCANCEL);


	//构造出牌动画
	COutCardItem *pOutCardItem = NULL;
	try { pOutCardItem = new COutCardItem(); }
	catch(...) { ASSERT(FALSE); return 0; }

	pOutCardItem->m_cbOutCardData = cbOutCardData;
	pOutCardItem->m_wOutCardUser = GetMeChairID();
	m_MoveCardItemArray.Add( pOutCardItem );

	tagMoveCardItem mci;
	mci.cbCardCount = 1;
	mci.cbCardData = cbOutCardData;
	mci.ptFrom = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
	mci.ptTo = m_GameClientView.m_DiscardCard[MYSELF_VIEW_ID].GetDispatchCardPos();
	mci.wViewChairId = MYSELF_VIEW_ID;
	m_GameClientView.OnMoveCardItem( mci );

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	//播放声音
	PlayCardSound(GetMeChairID(),cbOutCardData);

	m_GameClientView.InvalidGameView(0,0,0,0);

	return 0;
}

//扑克操作
LRESULT CGameClientEngine::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbOperateCode;
	BYTE cbOperateCard[3] = {0,0,0};

	//删除时间
	KillGameClock(IDI_OPERATE_CARD);

	//提取选择信息
	tagSelectCardInfo si;
	m_GameClientView.m_HandCardControl.GetUserSelectResult( si );
	
	//构造操作信息
	cbOperateCode = (BYTE)si.wActionMask;
	cbOperateCard[0] = si.cbActionCard;
	CopyMemory( &cbOperateCard[1],si.cbCardData,sizeof(BYTE)*2 );

	m_cbActionMask = 0;
	m_cbActionCard = 0;

	//取消操作模式
	m_GameClientView.m_HandCardControl.SetSelectMode( false );

	//如果是听牌
	if( m_bWillHearStatus && (cbOperateCode&WIK_LISTEN) )
	{
		m_bWillHearStatus = false;
		return OnOutCard( cbOperateCard[0],0 );
	}

	m_wCurrentUser = INVALID_CHAIR;

	//环境设置
	m_GameClientView.SetStatusFlag(false,true);
	
	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	CopyMemory( OperateCard.cbOperateCard,cbOperateCard,sizeof(cbOperateCard) );
	SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	return 0;
}

//拖管控制
LRESULT CGameClientEngine::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;

	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false,false);
		if( FALSE == (BOOL)wParam && m_pIStringMessage!=NULL ) 
			m_pIStringMessage->InsertSystemString(TEXT("您取消了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false,false);
		if( FALSE == (BOOL)wParam && m_pIStringMessage!=NULL )
			m_pIStringMessage->InsertSystemString(TEXT("您选择了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendSocketData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}

	return 0;
}

//买底消息
LRESULT CGameClientEngine::OnChip(WPARAM wParam, LPARAM lParam)
{
	if (IsLookonMode())	return 0;

	KillGameClock(IDI_CHIP);

	m_GameClientView.m_btScoreNone.ShowWindow(SW_HIDE);
	m_GameClientView.m_btScoreOne.ShowWindow(SW_HIDE);
	m_GameClientView.m_btScoreTwo.ShowWindow(SW_HIDE);

	//发送消息
	CMD_C_Chip	chip = {};
	chip.bMaiDi = (BYTE)wParam;

	SendSocketData(SUB_C_CHIP,&chip,sizeof(chip));

	return 0;
}

//第二次摇色子消息
LRESULT CGameClientEngine::OnSiceTwo(WPARAM wParam, LPARAM lParam)
{
	//环境处理
	PlayGameSound(AfxGetInstanceHandle(),TEXT("DRAW_SICE"));

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	return 0;
}

//摇色子结束消息
LRESULT CGameClientEngine::OnSiceFinish(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	m_GameClientView.m_DrawSiceWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HeapCard[SwitchViewChairID(m_wMagicPos)].SetCardData(m_cbHeapMagic, m_GameLogic.SwitchToCardData(m_bMagicIndex));
	m_GameClientView.SetMagicCard(m_GameLogic.SwitchToCardData(m_bMagicIndex));		

	WORD wMeChairID = GetMeChairID();
	bool bPlayerMode = (IsLookonMode()==false);

	//构造发牌动画
	BYTE cbHandCardData[MAX_COUNT];
	m_GameLogic.SwitchToCardData( m_cbCardIndex,cbHandCardData );
	//混乱扑克
	m_GameLogic.RandCardData(cbHandCardData,(m_wBankerUser==wMeChairID)?MAX_COUNT:MAX_COUNT-1,cbHandCardData);
	//确定堆牌删除位置
	WORD wHeapId = m_wHeapTail;
	WORD wHeapCardIndex = 0;
	if( m_cbHeapCardInfo[m_wHeapTail][1] == 0 )
	{
		wHeapId = (wHeapId+1)%GAME_PLAYER;
		wHeapCardIndex = HEAP_FULL_COUNT-1;
	}
	else wHeapCardIndex = m_cbHeapCardInfo[m_wHeapTail][1]-1;
	//发4圈，最后1圈发一张
	for( BYTE i = 0; i < 4; i++ )
	{
		//从庄家开始
		WORD wId = m_wBankerUser;
		for( WORD j = 0; j < GAME_PLAYER; j++ )
		{
			WORD wChairId = (wId+GAME_PLAYER-j)%GAME_PLAYER;
			CStartCardItem *pStartCardItem = NULL;
			//创建动画项目
			try{ pStartCardItem = new CStartCardItem(); }
			catch(...) { ASSERT(FALSE); return 0; }
			//发牌数
			pStartCardItem->m_cbCardCount = i<3?4:1;
			//发牌值
			if( wChairId == wMeChairID )
				CopyMemory( pStartCardItem->m_cbCardData,&cbHandCardData[i*4],sizeof(BYTE)*(i<3?4:1) );
			//发牌玩家
			pStartCardItem->m_wChairId = wChairId;
			//发牌位置
			pStartCardItem->m_wHeapId = wHeapId;
			//堆牌位置
			pStartCardItem->m_wHeapCardIndex = wHeapCardIndex;
			//加入动画数组
			m_MoveCardItemArray.Add(pStartCardItem);

			//推进
			if( i < 3 )
			{
				if( wHeapCardIndex <= 3 )
				{
					wHeapId = (wHeapId+1)%GAME_PLAYER;
					wHeapCardIndex = HEAP_FULL_COUNT-(3-wHeapCardIndex)-1;
				}
				else wHeapCardIndex -= 4;
			}
			else
			{
				if( wHeapCardIndex == 0 )
				{
					wHeapId = (wHeapId+1)%GAME_PLAYER;
					wHeapCardIndex = HEAP_FULL_COUNT-1;
				}
				else wHeapCardIndex--;
			}
		}
	}
	//最后一张，发给庄家
	CStartCardItem *pStartCardItem = NULL;
	//创建动画项目
	try{ pStartCardItem = new CStartCardItem(); }
	catch(...) { ASSERT(FALSE); return 0; }
	//发牌数
	pStartCardItem->m_cbCardCount = 1;
	//发牌值
	if( m_wBankerUser == wMeChairID )
		pStartCardItem->m_cbCardData[0] = cbHandCardData[MAX_COUNT-1];
	//发牌玩家
	pStartCardItem->m_wChairId = m_wBankerUser;
	//发牌位置
	pStartCardItem->m_wHeapId = wHeapId;
	//堆牌位置
	pStartCardItem->m_wHeapCardIndex = wHeapCardIndex;
	//最后一张
	pStartCardItem->m_bLastItem = true;
	//加入动画数组
	m_MoveCardItemArray.Add(pStartCardItem);

	//开始动画
	BeginMoveCard();

	return 0;
}

//设置扑克
void CGameClientEngine::SetHandCardControl(BYTE cbCardIndex[MAX_INDEX], BYTE cbAdvanceCard)
{
	//组合数目
	WORD wMeChairID=GetMeChairID();
	BYTE cbWeaveCardCount=m_cbWeaveCount[wMeChairID]*3;

	//转换扑克
	BYTE cbHandCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(cbCardIndex,cbHandCardData);

	//调整扑克
	if ( (cbWeaveCardCount+cbCardCount)==MAX_COUNT )
	{
		if( cbAdvanceCard != 0x00 )
		{
			//删除扑克
			BYTE cbRemoveCard[]={cbAdvanceCard};
			VERIFY( m_GameLogic.RemoveCard(cbHandCardData,cbCardCount,cbRemoveCard,1) );

			//设置扑克
			cbHandCardData[cbCardCount-1]=cbAdvanceCard;
		}
		
		m_GameClientView.m_HandCardControl.SetCardData(cbHandCardData,cbCardCount-1,cbHandCardData[cbCardCount-1]);
	}
	else m_GameClientView.m_HandCardControl.SetCardData(cbHandCardData,cbCardCount,0x00);

	return;
}

//定时器消息
VOID CGameClientEngine::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_REPLACE_CARD:	//用户补牌
		{
			//删除时间
			KillTimer(IDI_REPLACE_CARD);

			//设置界面
			m_GameClientView.SetUserAction( INVALID_CHAIR,0 );

			//寻找扑克
			for (BYTE i=MAX_INDEX-MAX_HUA_CARD;i<MAX_INDEX;i++)
			{	
				if (m_cbCardIndex[i] > 0 && !m_GameLogic.IsMagicCard(i,true))
				{
					//变量定义
					BYTE cbReplaceCard=m_GameLogic.SwitchToCardData(i);
					if( !m_GameLogic.RemoveCard(m_cbCardIndex,cbReplaceCard) )
					{
						ASSERT( FALSE );
						return;
					}

					//设置扑克
					SetHandCardControl(m_cbCardIndex,0x00);

					//播放声音
					PlayActionSound(GetMeChairID(),WIK_REPLACE);

					//设置界面
					m_GameClientView.SetUserAction( MYSELF_VIEW_ID,WIK_REPLACE );

					CReplaceCardItem *pReplaceCardItem = NULL;
					try{ pReplaceCardItem = new CReplaceCardItem(); }
					catch(...) { ASSERT(FALSE); return; }
					pReplaceCardItem->m_cbReplaceCard = cbReplaceCard;
					pReplaceCardItem->m_wReplaceUser = GetMeChairID();
					m_MoveCardItemArray.Add( pReplaceCardItem );

					//构造出牌动画
					tagMoveCardItem mci;
					mci.cbCardCount = 1;
					mci.cbCardData = cbReplaceCard;
					mci.wViewChairId = MYSELF_VIEW_ID;
					mci.ptFrom = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
					//mci.ptTo = m_GameClientView.m_DiscardCard[MYSELF_VIEW_ID].GetDispatchCardPos();
					mci.ptTo = m_GameClientView.GetHuaCardTailPos(MYSELF_VIEW_ID);
					m_GameClientView.OnMoveCardItem( mci );

					//发送数据
					CMD_C_ReplaceCard ReplaceCard;
					ReplaceCard.cbCardData=cbReplaceCard;
					SendSocketData(SUB_C_REPLACE_CARD,&ReplaceCard,sizeof(ReplaceCard));

					break;
				}
			}
		
			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//玩家操作控件
LRESULT CGameClientEngine::OnUserAction( WPARAM wParam, LPARAM lParam )
{
	//变量定义
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard[3] = {0,0,0};

	//隐藏操作控件
	m_GameClientView.m_ControlWnd.ShowWindow( SW_HIDE );

	//状态判断
	if (cbOperateCode==WIK_NULL)
	{
		//设置变量
		m_cbActionMask = WIK_NULL;
		m_cbActionCard = 0;
		if( m_wCurrentUser == INVALID_CHAIR )
		{
			//删除定时器
			KillGameClock( IDI_OPERATE_CARD );
			//发送消息
			CMD_C_OperateCard OperateCard;
			OperateCard.cbOperateCode = WIK_NULL;
			ZeroMemory( OperateCard.cbOperateCard,sizeof(OperateCard.cbOperateCard) );
			SendSocketData( SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard) );
		}
		return 0;
	}

	if( cbOperateCode & WIK_LISTEN )
	{
		ASSERT( m_cbListenStatus == 0 );
		if( m_cbListenStatus != 0 ) return 0;

		//胡牌选项
		if( m_GameClientView.m_DlgListen.DoModal() == IDOK )
		{
			//选择自摸和抓炮
			if( m_GameClientView.m_DlgListen.m_bZhuaPao && m_GameClientView.m_DlgListen.m_bZiMo )
				m_cbListenStatus = 3;
			//只自摸
			else if( m_GameClientView.m_DlgListen.m_bZiMo )
				m_cbListenStatus = 2;
			//只抓炮
			else if( m_GameClientView.m_DlgListen.m_bZhuaPao )
				m_cbListenStatus = 1;
			//如果选择了,则发送听牌消息
			if( m_cbListenStatus > 0 )
			{
				//设置即将听牌标志
				m_bWillHearStatus = true;

				//设置变量
				m_cbActionMask = WIK_NULL;
				m_cbActionCard = 0;

				//设置听牌状态
				m_GameClientView.SetUserListenStatus( 2,true );

				//发送听牌命令
				CMD_C_Listen Listen;
				Listen.cbListen = m_cbListenStatus;
				SendSocketData( SUB_C_LISTEN,&Listen,sizeof(Listen) );

				//获取选择组合
				tagSelectCardInfo sci[MAX_COUNT];
				BYTE cbInfoCount = GetSelectCardInfo( cbOperateCode,sci );
				
				//如果只听一张牌
				if( cbInfoCount == 1 )
				{
					m_bWillHearStatus = false;
					OnOutCard( sci[0].cbActionCard,0 );
				}
				//否则由玩家选择
				else
				{
					//设置操作事件
					m_GameClientView.m_HandCardControl.OnEventUserAction( sci,cbInfoCount );
					//提示消息
					if(m_pIStringMessage!=NULL )
						m_pIStringMessage->InsertSystemString(TEXT("请选择胡口牌"));
				}
			}
		}

		return 0;
	}

	bool bDone = false;
	//胡牌
	if( cbOperateCode & WIK_CHI_HU )
		bDone = true;
	else
	{
		//获取选择组合
		tagSelectCardInfo sci[MAX_COUNT];
		BYTE cbInfoCount = GetSelectCardInfo( cbOperateCode,sci );

		//设置操作事件
		bDone = m_GameClientView.m_HandCardControl.OnEventUserAction( sci,cbInfoCount );

		//如果完成操作
		if( bDone )
		{
			//设置操作结果
			tagSelectCardInfo si;
			m_GameClientView.m_HandCardControl.GetUserSelectResult( si );
			cbOperateCode = (BYTE)si.wActionMask;
			cbOperateCard[0] = si.cbActionCard;
			CopyMemory( &cbOperateCard[1],si.cbCardData,2*sizeof(BYTE) );

			m_GameClientView.m_HandCardControl.SetSelectMode( false );
		}
		//否则，设置等待选择
		else m_GameClientView.SetStatusFlag( true,false );
	}

	//如果操作完成，直接发送操作命令
	if( bDone )
	{
		//删除定时器
		KillGameClock( IDI_OPERATE_CARD );

		//设置变量
		m_wCurrentUser = INVALID_CHAIR;
		m_cbActionMask = WIK_NULL;
		m_cbActionCard = 0;

		CMD_C_OperateCard OperateCard;
		OperateCard.cbOperateCode = cbOperateCode;
		CopyMemory( OperateCard.cbOperateCard,cbOperateCard,sizeof(cbOperateCard) );
		SendSocketData( SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard) );
	}

	return 0;
}

//获取操作信息
BYTE CGameClientEngine::GetSelectCardInfo( WORD wOperateCode, tagSelectCardInfo SelectInfo[MAX_COUNT] )
{
	//初始化
	BYTE cbSelectCount = 0;

	if( wOperateCode == WIK_NULL ) return 0;

	//听牌
	if( wOperateCode & WIK_LISTEN )
	{
		//提取可丢弃牌
		BYTE cbCardData[MAX_COUNT],cbCardCount=0;
		BYTE cbCardIndex[MAX_INDEX];
		CopyMemory( cbCardIndex,m_cbCardIndex,sizeof(cbCardIndex) );
		WORD wMeChairId = GetMeChairID();
		CChiHuRight chr;
		for( BYTE i = 0; i < MAX_INDEX/*-MAX_HUA_CARD*/; i++ )
		{	//091231JJ
			if( cbCardIndex[i] == 0) continue;
			if (i >= MAX_INDEX-MAX_HUA_CARD && i < MAX_INDEX && !m_GameLogic.IsMagicCard(i,true)) continue;	

			cbCardIndex[i]--;
			for( BYTE j = 0; j < MAX_INDEX-MAX_HUA_CARD; j++ )
			{
				BYTE cbCurrentCard = m_GameLogic.SwitchToCardData(j);
				if( WIK_CHI_HU == m_GameLogic.AnalyseChiHuCard( cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],
					cbCurrentCard,chr) )
				{
					cbCardData[cbCardCount++] = m_GameLogic.SwitchToCardData(i);
					break;
				}
			}
			cbCardIndex[i]++;
		}
		for( BYTE i = 0; i < cbCardCount; i++ )
		{
			SelectInfo[cbSelectCount].cbActionCard = cbCardData[i];
			SelectInfo[cbSelectCount].wActionMask = WIK_LISTEN;
			SelectInfo[cbSelectCount].cbCardCount = 1;
			SelectInfo[cbSelectCount++].cbCardData[0] = cbCardData[i];
		}
	}
	//吃牌
	else if( wOperateCode&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) )
	{
		//效验
		ASSERT( m_cbActionCard != 0 );
		if( m_cbActionCard == 0 ) return 0;

		//替换白板 (换成财神计算牌值)
		if (m_cbActionCard == 0x37 && m_bMagicIndex != 33)
			m_cbActionCard = m_GameLogic.SwitchToCardData(m_bMagicIndex);

		if( m_cbActionMask & WIK_LEFT )
		{
			SelectInfo[cbSelectCount].cbActionCard = m_cbActionCard;
			SelectInfo[cbSelectCount].wActionMask = WIK_LEFT;
			SelectInfo[cbSelectCount].cbCardCount = 2;
			SelectInfo[cbSelectCount].cbCardData[0] = m_cbActionCard+1;
			SelectInfo[cbSelectCount++].cbCardData[1] = m_cbActionCard+2;
		}
		if( m_cbActionMask & WIK_CENTER )
		{
			SelectInfo[cbSelectCount].cbActionCard = m_cbActionCard;
			SelectInfo[cbSelectCount].wActionMask = WIK_CENTER;
			SelectInfo[cbSelectCount].cbCardCount = 2;
			SelectInfo[cbSelectCount].cbCardData[0] = m_cbActionCard-1;
			SelectInfo[cbSelectCount++].cbCardData[1] = m_cbActionCard+1;
		}
		if( m_cbActionMask & WIK_RIGHT )
		{
			SelectInfo[cbSelectCount].cbActionCard = m_cbActionCard;
			SelectInfo[cbSelectCount].wActionMask = WIK_RIGHT;
			SelectInfo[cbSelectCount].cbCardCount = 2;
			SelectInfo[cbSelectCount].cbCardData[0] = m_cbActionCard-2;
			SelectInfo[cbSelectCount++].cbCardData[1] = m_cbActionCard-1;
		}

		//替换白板	(换回来)
		if ( m_cbActionMask & (WIK_RIGHT|WIK_LEFT|WIK_CENTER) )
		{
			for (int i = 0; i < cbSelectCount; i++)
			{
				if (SelectInfo[i].cbActionCard == m_GameLogic.SwitchToCardData(m_bMagicIndex))
					SelectInfo[i].cbActionCard = 0x37;

				for (int j = 0; j < 2; j++)
				{
					if (SelectInfo[i].cbCardData[j] == m_GameLogic.SwitchToCardData(m_bMagicIndex))
						SelectInfo[i].cbCardData[j] = 0x37;
				}
			}
		}
	}
	//碰牌
	else if( wOperateCode & WIK_PENG )
	{
		//效验
		ASSERT( m_cbActionCard != 0 );
		if( m_cbActionCard == 0 ) return 0;
		SelectInfo[cbSelectCount].cbActionCard = m_cbActionCard;
		SelectInfo[cbSelectCount].wActionMask = WIK_PENG;
		SelectInfo[cbSelectCount].cbCardCount = 2;
		SelectInfo[cbSelectCount].cbCardData[0] = m_cbActionCard;
		SelectInfo[cbSelectCount++].cbCardData[1] = m_cbActionCard;
	}
	//杠牌
	else if( wOperateCode & WIK_GANG )
	{
		//如果是自己杠牌
		if( m_wCurrentUser == GetMeChairID() )
		{
			//寻找是否有多个杠牌
			WORD wMeChairId = GetMeChairID();
			tagGangCardResult gcr;
			ZeroMemory( &gcr,sizeof(gcr) );
			m_GameLogic.AnalyseGangCard( m_cbCardIndex,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],gcr );
			ASSERT( gcr.cbCardCount > 0 );
			for( BYTE i = 0; i < gcr.cbCardCount; i++ )
			{
				SelectInfo[cbSelectCount].cbActionCard = gcr.cbCardData[i];
				SelectInfo[cbSelectCount].wActionMask = WIK_GANG;				
				if( m_cbCardIndex[m_GameLogic.SwitchToCardIndex(gcr.cbCardData[i])] == 1 )
				{
					SelectInfo[cbSelectCount].cbCardCount = 1;
					SelectInfo[cbSelectCount].cbCardData[0] = gcr.cbCardData[i];
				}
				else
				{
					SelectInfo[cbSelectCount].cbCardCount = m_GameLogic.GetWeaveCard(WIK_GANG,gcr.cbCardData[i],
						SelectInfo[cbSelectCount].cbCardData);
				}
				cbSelectCount++;
			}
		}
		else
		{
			ASSERT( m_cbActionCard != 0 );
			if( m_cbActionCard == 0 ) return 0;
			SelectInfo[cbSelectCount].cbActionCard = m_cbActionCard;
			SelectInfo[cbSelectCount].wActionMask = WIK_GANG;
			SelectInfo[cbSelectCount].cbCardCount = 3;
			SelectInfo[cbSelectCount].cbCardData[0] = m_cbActionCard;
			SelectInfo[cbSelectCount].cbCardData[1] = m_cbActionCard;
			SelectInfo[cbSelectCount++].cbCardData[2] = m_cbActionCard;
		}
	}

	return cbSelectCount;
}

//扑克动画
bool CGameClientEngine::BeginMoveCard()
{
	if( m_MoveCardItemArray.GetCount() == 0 ) return false;
	if( m_GameClientView.IsMovingCard() ) return true;

	MoveKind enMoveKind = m_MoveCardItemArray[0]->GetMoveKind();

	//开局发牌
	if( enMoveKind == MK_StartCard )
		return BeginMoveStartCard( m_MoveCardItemArray[0] );
	//出牌
	else if( enMoveKind == MK_OutCard )
		return BeginMoveOutCard( m_MoveCardItemArray[0] );
	//发牌
	else if( enMoveKind == MK_SendCard )
		return BeginMoveSendCard( m_MoveCardItemArray[0] );
	//补花
	else if( enMoveKind == MK_ReplaceCard )
		return BeginMoveReplaceCard( m_MoveCardItemArray[0] );

	return false;
}

//停止动画
void CGameClientEngine::StopMoveCard()
{
	if( m_GameClientView.IsMovingCard() )
		m_GameClientView.StopMoveCard();

	while( BeginMoveCard() ) {
		m_GameClientView.StopMoveCard();
	}
}

//出牌动画
bool CGameClientEngine::BeginMoveOutCard( const CMoveCardItem *pMoveCardItem )
{
	const COutCardItem *pOutCardItem = (const COutCardItem *)pMoveCardItem;

	//定义变量
	WORD wMeChairId = GetMeChairID();
	WORD wOutCardUser = pOutCardItem->m_wOutCardUser;
	WORD wViewOutCardUser = SwitchViewChairID(wOutCardUser);
	BYTE cbOutCardData = pOutCardItem->m_cbOutCardData;
	
	if( IsLookonMode() || wOutCardUser != wMeChairId )
	{
		//环境设置
		KillGameClock(IDI_OPERATE_CARD); 
		
		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//计算起始,结束点
		CPoint ptFrom,ptTo;
		if( wOutCardUser == wMeChairId )
			ptFrom = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
		else
		{
			WORD wViewId = wOutCardUser>=3?2:wViewOutCardUser;
			ptFrom = m_GameClientView.m_UserCard[wViewId].GetDispatchCardPos();
		}
		ptTo = m_GameClientView.m_DiscardCard[wViewOutCardUser].GetDispatchCardPos();

		//播放声音
		PlayCardSound(wOutCardUser,cbOutCardData);

		//设置界面
		m_GameClientView.SetCurrentUser( INVALID_CHAIR );
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wViewOutCardUser,cbOutCardData);
		
		//设置扑克
		if (wViewOutCardUser==MYSELF_VIEW_ID)
		{
			//删除扑克
			if( !m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData) )
			{
				ASSERT( FALSE );
				return false;
			}

			SetHandCardControl( m_cbCardIndex,0x00 );
		}
		else
		{
			WORD wUserIndex=(wViewOutCardUser>2)?2:wViewOutCardUser;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}

		tagMoveCardItem mci;
		mci.ptFrom = ptFrom;
		mci.ptTo = ptTo;
		mci.cbCardCount = 1;
		mci.cbCardData = cbOutCardData;
		mci.wViewChairId = wViewOutCardUser;
		m_GameClientView.OnMoveCardItem( mci );
	}

	return true;
}

//发牌动画
bool CGameClientEngine::BeginMoveSendCard( const CMoveCardItem *pMoveCardItem )
{
	const CSendCardItem *pSendCardItem = (const CSendCardItem *)pMoveCardItem;

	WORD wSendUserViewId = SwitchViewChairID(pSendCardItem->m_wSendCardUser);

	//计算起始,结束点
	CPoint ptFrom,ptTo;
	bool bCgMagicPos = false;
	if( !pSendCardItem->m_bTail )
	{
		ASSERT( m_wHeapHead != INVALID_CHAIR );
		ptFrom = m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapHead)].GetDispatchCardPos(false);
		//删除牌头
		if( !m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapHead)].DeductionHeapCard(false, bCgMagicPos) )
		{
			m_wHeapHead = (m_wHeapHead+1)%GAME_PLAYER;
			ptFrom = m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapHead)].GetDispatchCardPos(false);
			VERIFY( m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapHead)].DeductionHeapCard(false, bCgMagicPos) );
		}
	}
	else
	{
		ASSERT( m_wHeapTail != INVALID_CHAIR );
		ptFrom = m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapTail)].GetDispatchCardPos(true);
		WORD wNextHeapTail = (m_wHeapTail+GAME_PLAYER-1)%GAME_PLAYER;
		//删除牌尾
		if( !m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapTail)].DeductionHeapCard(true, bCgMagicPos) )
		{
			m_wHeapTail = (m_wHeapTail+GAME_PLAYER-1)%GAME_PLAYER;
			ptFrom = m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapTail)].GetDispatchCardPos(true);
			VERIFY( m_GameClientView.m_HeapCard[SwitchViewChairID(m_wHeapTail)].DeductionHeapCard(true, bCgMagicPos) );
		}
		bool bHaveMagic = m_GameClientView.m_HeapCard[SwitchViewChairID(wNextHeapTail)].HaveMagicCard();
		if (!bHaveMagic && bCgMagicPos && !m_GameClientView.m_HeapCard[SwitchViewChairID(wNextHeapTail)].IsEmpty())
			m_GameClientView.m_HeapCard[SwitchViewChairID(wNextHeapTail)].SetCardData(0, m_GameLogic.SwitchToCardData(m_bMagicIndex));
	}
	if( wSendUserViewId == MYSELF_VIEW_ID )
		ptTo = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
	else
	{
		WORD wViewId = wSendUserViewId>=3?2:wSendUserViewId;
		ptTo = m_GameClientView.m_UserCard[wViewId].GetDispatchCardPos();
	}

	//播放发牌声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

	//构造动画
	tagMoveCardItem mci;
	mci.ptFrom = ptFrom;
	mci.ptTo = ptTo;
	mci.cbCardCount = 1;
	mci.cbCardData = 0;
	mci.wViewChairId = SwitchViewChairID(pSendCardItem->m_wSendCardUser);
	m_GameClientView.OnMoveCardItem( mci );

	return true;
}

//补花动画
bool CGameClientEngine::BeginMoveReplaceCard( const CMoveCardItem *pMoveCardItem )
{
	const CReplaceCardItem *pReplaceCardItem = (const CReplaceCardItem *)pMoveCardItem;

	//定义变量
	WORD wMeChairId = GetMeChairID();
	WORD wReplaceUser = pReplaceCardItem->m_wReplaceUser;
	WORD wReplaceUserViewId = SwitchViewChairID(wReplaceUser);
	BYTE cbReplaceCard = pReplaceCardItem->m_cbReplaceCard;
	
	if( IsLookonMode() || wReplaceUser != wMeChairId )
	{
		//播放声音
		PlayActionSound(wReplaceUser,WIK_REPLACE);

		//计算起始,结束点
		CPoint ptFrom,ptTo;
		if( wReplaceUser == wMeChairId )
			ptFrom = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
		else
		{
			WORD wViewId = wReplaceUserViewId>=3?2:wReplaceUserViewId;
			ptFrom = m_GameClientView.m_UserCard[wViewId].GetDispatchCardPos();
		}
		//ptTo = m_GameClientView.m_DiscardCard[wReplaceUserViewId].GetDispatchCardPos();
		ptTo = m_GameClientView.GetHuaCardTailPos(wReplaceUserViewId);

		//设置动作
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetUserAction( wReplaceUserViewId,WIK_REPLACE );

		//设置扑克
		if (wReplaceUserViewId==MYSELF_VIEW_ID)
		{
			//删除扑克
			if( !m_GameLogic.RemoveCard(m_cbCardIndex,cbReplaceCard) )
			{
				ASSERT( FALSE );
				return false;
			}

			//设置扑克
			SetHandCardControl(m_cbCardIndex,0x00);
		}
		else
		{
			//设置扑克
			WORD wUserIndex=(wReplaceUserViewId>2)?2:wReplaceUserViewId;
			if( m_GameClientView.m_UserCard[wUserIndex].IsHaveCurrentCard() )
				m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
			else
			{
				WORD wCardCount = m_GameClientView.m_UserCard[wUserIndex].GetCardCount();
				m_GameClientView.m_UserCard[wUserIndex].SetCardData(wCardCount-1,false);
			}
		}

		tagMoveCardItem mci;
		mci.ptFrom = ptFrom;
		mci.ptTo = ptTo;
		mci.cbCardCount = 1;
		mci.cbCardData = cbReplaceCard;
		mci.wViewChairId = wReplaceUserViewId;
		m_GameClientView.OnMoveCardItem( mci );
	}

	return true;
}

//开局发牌动画
bool CGameClientEngine::BeginMoveStartCard( const CMoveCardItem *pMoveCardItem )
{
	const CStartCardItem *pStartCardItem = (const CStartCardItem *)pMoveCardItem;

	//起始,结束点
	CPoint ptFrom,ptTo;

	//发牌位置
	WORD wHeapId = pStartCardItem->m_wHeapId;
	WORD wHeapViewId = SwitchViewChairID(wHeapId);
	//堆牌位置
	WORD wHeapCardIndex = pStartCardItem->m_wHeapCardIndex;
	if( pStartCardItem->m_cbCardCount > wHeapCardIndex+1 )
	{
		ASSERT( pStartCardItem->m_cbCardCount == 4 );
		//删除前面的堆牌
		m_GameClientView.m_HeapCard[wHeapViewId].SetCardEmpty( wHeapCardIndex,wHeapCardIndex+1 );

		wHeapId = (wHeapId+1)%GAME_PLAYER;
		wHeapViewId = SwitchViewChairID(wHeapId);
		wHeapCardIndex = HEAP_FULL_COUNT-(4-(wHeapCardIndex+1))-1;

		//获取起始点
		ptFrom = m_GameClientView.m_HeapCard[wHeapViewId].GetDispatchCardPos(WORD(wHeapCardIndex-1));

		//删除堆牌
		m_GameClientView.m_HeapCard[wHeapViewId].SetCardEmpty( HEAP_FULL_COUNT-1,
			pStartCardItem->m_cbCardCount-pStartCardItem->m_wHeapCardIndex-1 );
	}
	else
	{
		WORD wHeapPos = wHeapCardIndex;
		if( pStartCardItem->m_cbCardCount == 4 )
			wHeapPos -= 3;
		else
		{
			ASSERT( pStartCardItem->m_cbCardCount == 1 );
			if( wHeapPos%2 != 0 ) wHeapPos--;
			else wHeapPos++;
		}
		//获取起始点
		ptFrom = m_GameClientView.m_HeapCard[wHeapViewId].GetDispatchCardPos(wHeapPos);

		//删除堆牌
		if( pStartCardItem->m_cbCardCount == 4 )
			m_GameClientView.m_HeapCard[wHeapViewId].SetCardEmpty( wHeapCardIndex,4 );
		else
		{
			ASSERT( pStartCardItem->m_cbCardCount == 1 );
			if( wHeapCardIndex%2 != 0 )
				m_GameClientView.m_HeapCard[wHeapViewId].SetCardEmpty( wHeapCardIndex-1,pStartCardItem->m_cbCardCount );
			else m_GameClientView.m_HeapCard[wHeapViewId].SetCardEmpty( wHeapCardIndex+1,pStartCardItem->m_cbCardCount );
		}
	}

	//获取结束点
	WORD wViewChairId = SwitchViewChairID(pStartCardItem->m_wChairId);
	if( wViewChairId == MYSELF_VIEW_ID )
		ptTo = m_GameClientView.m_HandCardControl.GetDispatchCardPos();
	else
	{
		WORD wUserIndex = wViewChairId>2?2:wViewChairId;
		ptTo = m_GameClientView.m_UserCard[wUserIndex].GetDispatchCardPos();
	}
	
	//播放发牌声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

	//构造动画
	tagMoveCardItem mci;
	mci.ptFrom = ptFrom;
	mci.ptTo = ptTo;
	mci.cbCardCount = pStartCardItem->m_cbCardCount;
	mci.cbCardData = 0;
	mci.wViewChairId = wViewChairId;
	m_GameClientView.OnMoveCardItem( mci );

	return true;
}

//动画完成消息
LRESULT CGameClientEngine::OnMoveCardFinish( WPARAM wParam, LPARAM lParam )
{
	//校验
	ASSERT( m_MoveCardItemArray.GetCount() > 0 );
	if( m_MoveCardItemArray.GetCount() == 0 ) return 0;

	MoveKind enMoveKind = m_MoveCardItemArray[0]->GetMoveKind();

	//派送消息
	if( enMoveKind == MK_StartCard )
	{
		CStartCardItem StartCardItem = *((CStartCardItem *)m_MoveCardItemArray[0]);

		//删除动画项目
		SafeDelete(m_MoveCardItemArray[0]);
		m_MoveCardItemArray.RemoveAt(0);

		VERIFY( OnMoveStartCardFinish(&StartCardItem) );
	}
	else if( enMoveKind == MK_OutCard )
	{
		COutCardItem OutCardItem = *((COutCardItem *)m_MoveCardItemArray[0]);

		//删除动画项目
		SafeDelete(m_MoveCardItemArray[0]);
		m_MoveCardItemArray.RemoveAt(0);

		VERIFY( OnMoveOutCardFinish(&OutCardItem) );
	}
	else if( enMoveKind == MK_SendCard )
	{
		CSendCardItem SendCardItem = *((CSendCardItem *)m_MoveCardItemArray[0]);

		//删除动画项目
		SafeDelete(m_MoveCardItemArray[0]);
		m_MoveCardItemArray.RemoveAt(0);

		VERIFY( OnMoveSendCardFinish(&SendCardItem) );
	}
	else if( enMoveKind == MK_ReplaceCard )
	{
		CReplaceCardItem ReplaceCardItem = *((CReplaceCardItem *)m_MoveCardItemArray[0]);

		//删除动画项目
		SafeDelete(m_MoveCardItemArray[0]);
		m_MoveCardItemArray.RemoveAt(0);

		VERIFY( OnMoveReplaceCardFinish(&ReplaceCardItem) );
	}
	else 
	{
		ASSERT(FALSE);
		return 0;
	}

	//播放下一个动画
	BeginMoveCard();
		
	return 0;
}

//出牌动画完成
bool CGameClientEngine::OnMoveOutCardFinish( const COutCardItem *pOutCardItem )
{
	WORD wOutUserViewId = SwitchViewChairID(pOutCardItem->m_wOutCardUser);
	BYTE cbOutCardData = pOutCardItem->m_cbOutCardData;

	//添加到已丢弃牌
	m_GameClientView.m_DiscardCard[wOutUserViewId].AddCardItem( cbOutCardData );

	//设置出牌指示
	m_GameClientView.SetDiscUser( wOutUserViewId );

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return true;
}

//发牌动画完成
bool CGameClientEngine::OnMoveSendCardFinish( const CSendCardItem *pSendCardItem )
{
	//设置变量
	WORD wMeChairID = GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pSendCardItem->m_wSendCardUser);
	WORD wChairId = pSendCardItem->m_wSendCardUser;
	BYTE cbSendCardData = pSendCardItem->m_cbCardData;
	m_wCurrentUser = pSendCardItem->m_wCurrentUser;
	m_wReplaceUser = pSendCardItem->m_wReplaceUser;

	//取牌界面
	if (wViewChairID!=MYSELF_VIEW_ID)
	{
		WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
		BYTE cbCardCount = m_GameClientView.m_UserCard[wUserIndex].GetCardCount()+m_cbWeaveCount[wChairId]*3;
		m_GameClientView.m_UserCard[wUserIndex].AddCardData(cbCardCount==MAX_COUNT-1?true:false);
	}
	else
	{
		//插入扑克
		m_cbCardIndex[m_GameLogic.SwitchToCardIndex(cbSendCardData)]++;

		//设置扑克
		SetHandCardControl(m_cbCardIndex,cbSendCardData);
	}

	//当前用户
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//激活框架
		ActiveGameFrame();

		m_cbActionMask = pSendCardItem->m_cbActionMask;
		if( m_cbListenStatus == 0 )
		{
			//m_cbActionMask |= m_GameLogic.AnalyseTingCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID]); //取消听牌
		}
		m_cbActionCard = pSendCardItem->m_cbCardData;

		//动作处理
		if (m_cbActionMask!=WIK_NULL)
		{
			//设置界面
			m_GameClientView.m_ControlWnd.SetControlInfo(m_cbActionMask);
		}
	}

	//补牌操作
	if ((IsLookonMode()==false)&&(pSendCardItem->m_wReplaceUser==wMeChairID))
	{
		//设置时间
		SetTimer(IDI_REPLACE_CARD,TIME_REPLACE_CARD,NULL);
	}

	//出牌提示
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID),false);

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	//计算时间
	WORD wTimeCount=TIME_OPERATE_CARD;

	//设置时间
	if( m_wCurrentUser != INVALID_CHAIR )
	{
		m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
		SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,wTimeCount);
	}
	else m_GameClientView.SetCurrentUser( INVALID_CHAIR );

	return true;
}

//补花动画完成
bool CGameClientEngine::OnMoveReplaceCardFinish( const CReplaceCardItem *pReplaceCardItem )
{
	WORD wReplaceUserViewId = SwitchViewChairID(pReplaceCardItem->m_wReplaceUser);
	BYTE cbReplaceCardData = pReplaceCardItem->m_cbReplaceCard;

	//添加到已丢弃牌
	//m_GameClientView.m_DiscardCard[wReplaceUserViewId].AddCardItem( cbReplaceCardData );

	//设置出牌指示
	m_GameClientView.SetDiscUser( wReplaceUserViewId );

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	m_GameClientView.SetHuaCard(wReplaceUserViewId, cbReplaceCardData, false);

	return true;
}

//开局动画完成
bool CGameClientEngine::OnMoveStartCardFinish( const CStartCardItem *pStartCardItem )
{
	//设置变量
	WORD wMeChairId = GetMeChairID();
	WORD wChairId = pStartCardItem->m_wChairId;
	WORD wViewChairId = SwitchViewChairID(wChairId);

	//添加扑克
	if( wViewChairId == MYSELF_VIEW_ID )
		m_GameClientView.m_HandCardControl.AddCardData( pStartCardItem->m_cbCardData,pStartCardItem->m_cbCardCount );
	else
	{
		WORD wUserIndex = wViewChairId>2?2:wViewChairId;
		if( pStartCardItem->m_bLastItem )
			m_GameClientView.m_UserCard[wUserIndex].AddCardData(true);
		else
		{
			for( BYTE i = 0; i < pStartCardItem->m_cbCardCount; i++ )
				m_GameClientView.m_UserCard[wUserIndex].AddCardData(false);
		}
	}

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

	//判断是否最后一张牌
	if( pStartCardItem->m_bLastItem )
	{
		//激活框架
		if (!IsLookonMode())
			ActiveGameFrame();

		//堆立扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetHeapCardInfo(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1]);
		}

		//手上扑克
		SetHandCardControl(m_cbCardIndex,0x00);
		
		m_GameClientView.m_HandCardControl.SetPositively(!IsLookonMode());

		//补牌操作
		if (!IsLookonMode()&&(m_wReplaceUser==wMeChairId))
		{
			//设置时间
			SetTimer(IDI_REPLACE_CARD,TIME_REPLACE_CARD,NULL);
		}

		//出牌提示,动作处理
		if (!IsLookonMode()&&(m_wCurrentUser==wMeChairId))
		{
			m_GameClientView.SetStatusFlag(true,false);
			
			//m_cbActionMask |= m_GameLogic.AnalyseTingCard(m_cbCardIndex,NULL,0);	//取消听牌

			if( m_cbActionMask != WIK_NULL )
				m_GameClientView.m_ControlWnd.SetControlInfo(m_cbActionMask);
		}
		else if( m_wCurrentUser != wMeChairId && m_wCurrentUser != INVALID_CHAIR )
			m_GameClientView.SetStatusFlag(false,true);

		//设置时间
		if (m_wCurrentUser!=INVALID_CHAIR)
		{
			m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));
			SetGameClock(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
		}
	}

	//更新界面
	m_GameClientView.InvalidGameView(0,0,0,0);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
