#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "DlgInfomation.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏时间
#define IDI_OPERATE_SCORE			201									//加注定时器
#define IDI_START_GAME				202									//开始定时器
#define IDI_USER_SCORE				203									//下注定时器

//游戏时间
#define TIME_OPERATE_SCORE			30									//加注定时器
#define TIME_START_GAME				30									//开始定时器
#define TIME_USER_SCORE				30									//下注定时器

#define IDI_PERFORM_START			203									//执行开始
#define IDI_PERFORM_END				204									//执行结束

#define TIME_PERFORM_START			1000								//执行开始时间
#define TIME_PERFORM_END			1000								//执行结束时间

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_GIVE_CARD,OnGiveCard)
	ON_MESSAGE(IDM_STOP_CARD,OnStopCard)
	ON_MESSAGE(IDM_USER_SCORE,OnUserScore)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
	ON_MESSAGE(IDM_MOVE_JET_FINISH,OnMoveJetFinish)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_lCellScore = 0L;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	m_bAddScore = false;
	m_bGameStart = false;

	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;
	m_wBankerUser = INVALID_CHAIR;
	ZeroMemory( m_byUserStatus,sizeof(m_byUserStatus) );

	ZeroMemory( m_szAccounts,sizeof(m_szAccounts));

	m_bySendCardCount = 0;
	m_wStartChairId = INVALID_CHAIR;
	ZeroMemory( m_bySendCardData,sizeof(m_bySendCardData) );

	ZeroMemory( &m_GameStart,sizeof(m_GameStart) );
	ZeroMemory( &m_GameEnd,sizeof(m_GameEnd) );

	return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_lCellScore = 0L;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	m_bAddScore = false;
	m_bGameStart = false;

	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;
	m_wBankerUser = INVALID_CHAIR;
	ZeroMemory( m_byUserStatus,sizeof(m_byUserStatus) );

	ZeroMemory( m_szAccounts,sizeof(m_szAccounts));

	m_bySendCardCount = 0;
	m_wStartChairId = INVALID_CHAIR;
	ZeroMemory( m_bySendCardData,sizeof(m_bySendCardData) );

	ZeroMemory( &m_GameStart,sizeof(m_GameStart) );
	ZeroMemory( &m_GameEnd,sizeof(m_GameEnd) );

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	//构造数据
	//CGameOption GameOption;
	//GameOption.m_bEnableSound=IsEnableSound();
	//GameOption.m_bAllowLookon = IsAllowUserLookon();

	////配置数据
	//if (GameOption.DoModal()==IDOK)
	//{
	//	//设置控件
	//	EnableSound(GameOption.m_bEnableSound);
	//	AllowUserLookon(0,GameOption.m_bAllowLookon);
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
	case IDI_START_GAME:	//游戏开始
		{
			//关闭游戏
			if ((nElapse==0)&&(IsLookonMode()==false)&&(wChairID==GetMeChairID()))
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OPERATE_SCORE:		//用户加注
		{
			//自动处理
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStopCard(0,0);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_USER_SCORE:		//玩家下注
		{
			//自动处理
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnUserScore(1,0);
				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
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
	case SUB_S_GAME_START:	//游戏开始
		{
			PerformGameConclude();
			//结束动画
			m_GameClientView.StopMoveJettons();
			m_GameClientView.ConcludeDispatch();
			m_GameClientView.StopMoveNumber();
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:
		{
			//结束动画
			m_GameClientView.StopMoveJettons();
			m_GameClientView.ConcludeDispatch();
			return OnSubSendCard( pData,wDataSize );
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//结束动画
			m_GameClientView.StopMoveJettons();
			m_GameClientView.ConcludeDispatch();
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:
		{
			//结束动画
			m_GameClientView.StopMoveJettons();
			m_GameClientView.ConcludeDispatch();
			return OnSubUserStopCard( pData,wDataSize );
		}
	case SUB_S_ADD_SCORE:
		{
			//结束动画
			m_GameClientView.ConcludeDispatch();
			m_GameClientView.StopMoveJettons();
			return OnSubUserAddScore( pData,wDataSize );
		}
	case SUB_S_USER_LEFT:
		{
			return OnSubUserLeft( pData,wDataSize );
		}
	case SUB_S_GAME_PLAY:
		{
			PerformGameStart();
			return OnSubGamePlay(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TH_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//单元积分
			m_lCellScore = pStatusFree->lCellScore;

			//界面设置
			m_GameClientView.SetCellScore( m_lCellScore );

			//玩家设置
			if (IsLookonMode()==false && GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();

				//设置时间
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}

			return true;
		}
	case GS_TH_SCORE:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusScore));
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;

			//变量定义
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

			//设置变量
			m_wBankerUser = pStatusScore->wBankerUser;
			CopyMemory(m_byUserStatus,pStatusScore->bUserStatus,sizeof(m_byUserStatus));

			//玩家变量
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				//用户名字
				IClientUserItem * pClientUserItem=GetTableUserItem(i);
				if( pClientUserItem == NULL ) continue;
				lstrcpy( m_szAccounts[i],pClientUserItem->GetNickName() );
				
				//m_byUserStatus[i] = TRUE;
				m_bGameStart = true;

				m_GameClientView.SetScoreInfo( SwitchViewChairID(i),pClientUserItem->GetUserScore() );
			}

			//单元积分
			m_lCellScore = pStatusScore->lCellScore;

			//设置单元积分
			m_GameClientView.SetCellScore( m_lCellScore );

			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//当前用户
			if( !IsLookonMode() && !pStatusScore->cbHadScore && GetMeChairID() != m_wBankerUser )
			{
				//激活框架
				ActiveGameFrame();

				//更新控件
				UpdateScoreControl();

				//设置定时器
				SetGameClock( GetMeChairID(),IDI_USER_SCORE,TIME_USER_SCORE );
			}

			m_GameClientView.RefreshGameView( NULL );

			return true;
		}
	case GS_TH_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//变量定义
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//玩家变量
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				//用户名字
				IClientUserItem * pClientUserItem=GetTableUserItem(i);
				if( pClientUserItem == NULL ) continue;
				lstrcpy( m_szAccounts[i],pClientUserItem->GetNickName() );
				m_bGameStart = true;
				
				m_GameClientView.SetScoreInfo( SwitchViewChairID(i),pClientUserItem->GetUserScore() );
			}

			//单元积分
			m_lCellScore = pStatusPlay->lCellScore;
			m_wBankerUser = pStatusPlay->wBankerUser;

			//状态变量
			m_wCurrentUser = pStatusPlay->wCurrentUser;
			CopyMemory( m_byUserStatus,pStatusPlay->byUserStatus,sizeof(m_byUserStatus) );

			//下注变量
			CopyMemory( m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore) );

			//设置单元积分
			m_GameClientView.SetCellScore( m_lCellScore );

			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//如果非旁观用户
			WORD wMeChairId = GetMeChairID();
			if( !IsLookonMode() )
			{
				m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively( true );
			}

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				WORD wViewId = SwitchViewChairID( i );

				if( pStatusPlay->cbCardCount[i] == 0 || !pStatusPlay->byUserStatus[i] ) continue;

				//设置扑克
				m_GameClientView.m_CardControl[wViewId].SetCardData( pStatusPlay->cbHandCardData[i],
					pStatusPlay->cbCardCount[i] );

				//设置牌面分
				if( wViewId == MYSELF_VIEW_ID && !IsLookonMode() )
				{
					FLOAT fCardScore = m_GameLogic.GetCardGenre( pStatusPlay->cbHandCardData[i],
						pStatusPlay->cbCardCount[i] );
					m_GameClientView.SetCardScore( wViewId,fCardScore );
				}
				else
				{
					FLOAT fCardScore = m_GameLogic.GetCardGenre( &pStatusPlay->cbHandCardData[i][1],
						pStatusPlay->cbCardCount[i]-1 );
					m_GameClientView.SetCardScore( wViewId,fCardScore );
				}

				//设置桌面筹码
				m_GameClientView.OnUserAddJettons( wViewId,m_lTableScore[i] );
			}
			
			//停止筹码动画
			m_GameClientView.StopMoveJettons();

			//当前用户
			if( !IsLookonMode() && wMeChairId == m_wCurrentUser )
			{
				//激活框架
				ActiveGameFrame();

				//更新控件
				UpdateScoreControl();
			}

			//设置定时器
			if( m_wCurrentUser != INVALID_CHAIR )
				SetGameClock( m_wCurrentUser,IDI_OPERATE_SCORE,TIME_OPERATE_SCORE );

			m_GameClientView.RefreshGameView( NULL );

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_GameStart) );
	if( wDataSize != sizeof(CMD_S_GameStart) ) return false;

	CMD_S_GameStart *pGameStart = (CMD_S_GameStart *)pBuffer;

	//设置状态
	SetGameStatus(GS_TH_SCORE);

	if( IsLookonMode() ) KillGameClock( IDI_START_GAME );

	//保存数据
	CopyMemory( &m_GameStart,pGameStart,sizeof(m_GameStart) );

	//播放声音
	PlayGameSound( AfxGetInstanceHandle(),TEXT("GAME_START") );

	SetTimer( IDI_PERFORM_START,TIME_PERFORM_START,NULL );

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd( const void *pData, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//变量定义
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//设置状态
	SetGameStatus(GS_TH_FREE);
	KillGameClock( IDI_OPERATE_SCORE );
	KillGameClock( IDI_USER_SCORE );

	//隐藏控件
	HideScoreControl();

	//保存数据
	CopyMemory( &m_GameEnd,pGameEnd,sizeof(m_GameEnd) );

	//用户扑克
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_GameEnd.byCardData[i] != 0 )
		{
			//获取扑克
			BYTE byCardData[MAX_COUNT];
			WORD wViewChairID=SwitchViewChairID(i);
			BYTE wCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardData(byCardData,CountArray(byCardData));

			//设置扑克
			byCardData[0] = m_GameEnd.byCardData[i];
			FLOAT fCardScore;
			if( wCardCount > 1 )
				fCardScore = m_GameLogic.GetCardGenre(&byCardData[1],wCardCount-1);
			else fCardScore = m_GameLogic.GetCardGenre(byCardData,wCardCount);
			if( fCardScore != CT_ERROR )
			{
				m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(true);
				m_GameClientView.m_CardControl[wViewChairID].SetCardData(byCardData,wCardCount);
				fCardScore = m_GameLogic.GetCardGenre( byCardData,(BYTE)wCardCount );
				m_GameClientView.SetCardScore( wViewChairID,fCardScore );
			}
		}
	}

	//设置控件
	m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively( false );

	m_GameClientView.SetWaitUserScore(false);

	//播放声音
	LONGLONG lMeScore=m_GameEnd.lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//设置定时
	SetTimer( IDI_PERFORM_END,TIME_PERFORM_END,NULL );

	return true;
}

//发牌
bool CGameClientEngine::OnSubSendCard( const void *pData, WORD wDataSize )
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_SendCard) );
	if( wDataSize != sizeof(CMD_S_SendCard) ) return false;

	//定义变量
	CMD_S_SendCard *pSendCard = (CMD_S_SendCard *)pData;

	//设置当前玩家
	m_wCurrentUser = pSendCard->wCurrentUser;

	//发牌
	if( !m_GameClientView.m_JettonControl.IsPlayMoving() )
	{
		m_GameClientView.DispatchUserCard( SwitchViewChairID(pSendCard->wSendCardUser),
			pSendCard->cbCardData );
	}
	else
	{
		m_bySendCardCount = 1;
		if( m_wStartChairId == INVALID_CHAIR )
			m_wStartChairId = pSendCard->wSendCardUser;
		m_bySendCardData[pSendCard->wSendCardUser][0] = pSendCard->cbCardData;
	}

	return true;
}

//开始消息
LRESULT CGameClientEngine::OnStart( WPARAM wParam, LPARAM lParam )
{
	KillGameClock( IDI_START_GAME );

	//游戏变量
	m_lCellScore = 0L;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	m_bAddScore = false;

	//设置当前玩家
	m_wCurrentUser = INVALID_CHAIR;
	m_wBankerUser = INVALID_CHAIR;
	ZeroMemory( m_byUserStatus,sizeof(m_byUserStatus) );
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));

	//隐藏按钮
	m_GameClientView.m_btStart.ShowWindow( SW_HIDE );

	//停止动画
	m_GameClientView.StopMoveJettons();
	m_GameClientView.StopMoveNumber();

	m_GameClientView.m_JettonControl.ResetControl();
	//重置控件
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		//数字控件
		m_GameClientView.m_NumberControl[i].ResetControl();
		//扑克控件
		m_GameClientView.m_CardControl[i].SetCardData( NULL,0 );	
		m_GameClientView.m_CardControl[i].SetDisplayHead( false );
	}

	//设置桌面分
	m_GameClientView.SetCardScore( INVALID_CHAIR,0 );

	//设置用户动作
	m_GameClientView.SetUserAction( INVALID_CHAIR,0 );

	//更新界面
	m_GameClientView.RefreshGameView( NULL );

	//发送准备
	SendUserReady( NULL,0 );

	return 0;
}

//加倍消息
LRESULT CGameClientEngine::OnAddScore( WPARAM wParam, LPARAM lParam )
{
	//效验
	ASSERT( !m_bAddScore );
	if( m_bAddScore ) return 0;

	WORD wMeChairId = GetMeChairID();

	//删除定时器
	KillGameClock( IDI_OPERATE_SCORE );

	//隐藏下注控件
	HideScoreControl();

	WORD wViewId = SwitchViewChairID( wMeChairId );

	//设置玩家动作
	m_GameClientView.SetUserAction( wViewId,AC_ADD_SCORE );

	//播放声音	
	PlayActionSound( wMeChairId,AC_ADD_SCORE );

	//下注动画
	m_GameClientView.OnUserAddJettons( wViewId,m_lTableScore[wMeChairId] );
	m_GameClientView.BeginMoveJettons();

	//设置变量
	m_bAddScore = true;
	m_lTableScore[wMeChairId] *= 2;

	//发送数据
	SendSocketData( SUB_C_ADD_SCORE );

	//更新界面
	m_GameClientView.RefreshGameView( NULL );

	return 0;
}

//要牌消息
LRESULT CGameClientEngine::OnGiveCard( WPARAM wParam, LPARAM lParam )
{
	//效验
	WORD wMeChairId = GetMeChairID();
	BYTE cbCardData[MAX_COUNT] = {};
	BYTE cbCardCount = (BYTE)m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardData(cbCardData,MAX_COUNT);
	FLOAT fCardScore = m_GameLogic.GetCardGenre(cbCardData,cbCardCount);
	ASSERT( cbCardCount < MAX_COUNT );
	if( cbCardCount == MAX_COUNT ) return 0;
	ASSERT( fCardScore != CT_ERROR );
	if( fCardScore == CT_ERROR ) return 0;
	ASSERT( !m_bAddScore );
	if( m_bAddScore ) return 0;

	//删除定时器
	KillGameClock( IDI_OPERATE_SCORE );

	//隐藏控件
	HideScoreControl();

	WORD wViewId = SwitchViewChairID( wMeChairId );

	//设置玩家操作
	m_GameClientView.SetUserAction( wViewId,AC_GIVE_CARD );

	//播放声音	
	PlayActionSound( wMeChairId,AC_GIVE_CARD );
		
	//发送数据
	SendSocketData( SUB_C_GIVE_CARD );

	//更新界面
	m_GameClientView.RefreshGameView( NULL );

	return 0;
}

//放弃消息
LRESULT CGameClientEngine::OnStopCard( WPARAM wParam, LPARAM lParam )
{
	WORD wMeChairId = GetMeChairID();
	ASSERT( m_byUserStatus[wMeChairId] );
	if( !m_byUserStatus[wMeChairId] ) return 0;

	//删除定时器
	KillGameClock( IDI_OPERATE_SCORE );

	//隐藏控件
	HideScoreControl();

	WORD wViewId = SwitchViewChairID( wMeChairId );
	m_byUserStatus[wMeChairId] = FALSE;

	//设置游戏状态
	m_GameClientView.SetUserAction( wViewId,AC_STOP_CARD );

	//发送数据
	SendSocketData( SUB_C_GIVE_UP );

	//播放声音	
	PlayActionSound( wMeChairId,AC_STOP_CARD );

	//更新界面
	m_GameClientView.RefreshGameView( NULL );

	return 0;
}

//下注消息
LRESULT CGameClientEngine::OnUserScore( WPARAM wParam, LPARAM lParam )
{
	ASSERT( GetGameStatus() == GS_TH_SCORE );
	if( GetGameStatus() != GS_TH_SCORE ) return 0;
	BYTE cbTime = (BYTE)wParam;
	ASSERT( cbTime <= 4 );
	if( cbTime > 4 ) return 0;

	//删除定时器
	KillGameClock( IDI_USER_SCORE );

	//隐藏控件
	HideScoreControl();

	//发送数据
	CMD_C_Score Score = {};
	Score.lScore = m_lCellScore*cbTime;
	SendSocketData( SUB_C_SCORE,&Score,sizeof(Score) );

	return 0;
}

//定时器消息
void CGameClientEngine::OnTimer( UINT nIDEvent )
{
	switch( nIDEvent )
	{
	case IDI_PERFORM_START:
		{
			KillTimer( IDI_PERFORM_START );
			PerformGameStart();
			return;
		}
	case IDI_PERFORM_END:
		{
			KillTimer( IDI_PERFORM_END );
			PerformGameConclude();
			return;
		}
	}
	__super::OnTimer( nIDEvent );
}

//隐藏控件
VOID CGameClientEngine::HideScoreControl()
{
	//隐藏按钮
	m_GameClientView.m_btGiveCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btStopCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFourScore.ShowWindow(SW_HIDE);
}

//更新控件
VOID CGameClientEngine::UpdateScoreControl()
{
	if( GetGameStatus() == GS_TH_PLAY  && m_byUserStatus[GetMeChairID()] == TRUE)
	{
		WORD wMeChairID = GetMeChairID();
		if ((m_wCurrentUser==wMeChairID)&&(m_byUserStatus[wMeChairID]==TRUE)&&!m_bAddScore)
		{
			BYTE cbCardData[MAX_COUNT] = {};
			BYTE cbCardCount = (BYTE)m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardData(cbCardData,MAX_COUNT);
			FLOAT fCardScore = m_GameLogic.GetCardGenre(cbCardData,cbCardCount);

			//显示按钮
			m_GameClientView.m_btGiveCard.ShowWindow( SW_SHOW );
			m_GameClientView.m_btStopCard.ShowWindow( SW_SHOW );
			if( wMeChairID != m_wBankerUser )
				m_GameClientView.m_btAddScore.ShowWindow( SW_SHOW );
			else m_GameClientView.m_btAddScore.ShowWindow( SW_HIDE );

			//使能按钮
			if( fCardScore != CT_ERROR && fCardScore != 10.5 && cbCardCount < MAX_COUNT )
				m_GameClientView.m_btGiveCard.EnableWindow(TRUE);
			else m_GameClientView.m_btGiveCard.EnableWindow(FALSE);
			if( wMeChairID != m_wBankerUser )
			{
				if( !m_bAddScore && fCardScore != CT_ERROR && fCardScore != 10.5 && cbCardCount < MAX_COUNT )
					m_GameClientView.m_btAddScore.EnableWindow(TRUE);
				else m_GameClientView.m_btAddScore.EnableWindow(FALSE);
			}
		}
	}
	else if( GetGameStatus() == GS_TH_SCORE  && m_byUserStatus[GetMeChairID()] == TRUE)
	{
		TCHAR szTitle[4][32] = {};
		for( WORD i = 0; i < 4; i++ )
		{
			_sntprintf( szTitle[i],CountArray(szTitle[i]),TEXT("%I64d"),m_lCellScore*(i+1) );
		}
		m_GameClientView.m_btOneScore.SetWindowText(szTitle[0]);
		m_GameClientView.m_btTwoScore.SetWindowText(szTitle[1]);
		m_GameClientView.m_btThreeScore.SetWindowText(szTitle[2]);
		m_GameClientView.m_btFourScore.SetWindowText(szTitle[3]);

		m_GameClientView.m_btOneScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btTwoScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btThreeScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btFourScore.ShowWindow(SW_SHOW);
	}
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish( WPARAM wParam, LPARAM lParam )
{
	if( !IsLookonMode() )
		m_GameClientView.m_CardControl[MYSELF_VIEW_ID].SetPositively( true );

	//设置牌面分
	BYTE byCardCount = 0;
	BYTE byCardData[MAX_COUNT];
	WORD wMeChairId = GetMeChairID();
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_byUserStatus[i] ) continue;
		WORD wViewId = SwitchViewChairID( i );
		byCardCount = (BYTE)m_GameClientView.m_CardControl[wViewId].GetCardData( byCardData,MAX_COUNT );
		if( byCardCount > 1 )
		{
			FLOAT fCardScore = CT_ERROR;
			if( !IsLookonMode() && i == wMeChairId )
				fCardScore = m_GameLogic.GetCardGenre( byCardData,byCardCount );
			else
				fCardScore = m_GameLogic.GetCardGenre( &byCardData[1],byCardCount-1 );
			m_GameClientView.SetCardScore( wViewId,fCardScore );
		}
	}

	//设置控制
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();

		UpdateScoreControl();
	}

	m_GameClientView.RefreshGameView( NULL );

	//设置时间
	if( m_wCurrentUser != INVALID_CHAIR )
		SetGameClock(m_wCurrentUser,IDI_OPERATE_SCORE,TIME_OPERATE_SCORE);

	return 0;
}

//筹码移动完成
LRESULT CGameClientEngine::OnMoveJetFinish( WPARAM wParam, LPARAM lParam )
{
	if( m_bySendCardCount > 0 && GetGameStatus()==GS_TH_PLAY )
	{
		bool bGameStart = m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardCount()==0?true:false;
		//发牌
		for( BYTE i = 0; i < m_bySendCardCount; i++ )
		{
			WORD wId = m_wStartChairId;
			if( wId == INVALID_CHAIR ) wId = 0;
			for( WORD j = 0; j < GAME_PLAYER; j++ )
			{
				WORD wChairId = (wId+j)%GAME_PLAYER;
				if( m_byUserStatus[wChairId] && (m_bySendCardData[wChairId][i] != 0 || bGameStart) )
				{
					WORD wViewId = SwitchViewChairID( wChairId );
					m_GameClientView.DispatchUserCard( wViewId,m_bySendCardData[wChairId][i] );
				}
			}
		}
		m_bySendCardCount = 0;
		m_wStartChairId = INVALID_CHAIR;
		ZeroMemory( m_bySendCardData,sizeof(m_bySendCardData) );
	}
	return 0;
}

//播放操作声音
VOID CGameClientEngine::PlayActionSound( WORD wChairId, BYTE byAction )
{
	//效验
	ASSERT( wChairId != INVALID_CHAIR );
	IClientUserItem * pClientUserItem=GetTableUserItem(wChairId);
	if( pClientUserItem == NULL ) return ;

	if( byAction != AC_GIVE_CARD )
	{
		TCHAR szSoundSrc[64] = TEXT("");

		switch( byAction )
		{
		case AC_STOP_CARD:
			lstrcpy( szSoundSrc,TEXT("STOP_CARD") );
			break;
		case AC_ADD_SCORE:
			lstrcat( szSoundSrc,TEXT("ADD_SCORE") );
			break;
		default:
			ASSERT( FALSE );
			return;
		}

		//播放
		PlayGameSound( AfxGetInstanceHandle(),szSoundSrc );
	}
}

//执行结束
VOID CGameClientEngine::PerformGameConclude() 
{
	if( m_bGameStart == false ) return;

	KillTimer( IDI_PERFORM_END );

	m_GameClientView.ShowAddJettonInfo(false);
	//设置动画
	if( m_GameEnd.lGameScore[m_wBankerUser] < 0 )
		m_GameClientView.OnUserAddJettons(SwitchViewChairID(m_wBankerUser),-m_GameEnd.lGameScore[m_wBankerUser]);
	m_GameClientView.OnUserAddJettons(INVALID_CHAIR,0);
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_GameEnd.lGameScore[i] > 0 )
			m_GameClientView.OnUserRemoveJettons(SwitchViewChairID(i),m_GameEnd.lGameScore[i]+(i!=m_wBankerUser?m_lTableScore[i]:0));
	}	
	//开始动画
	m_GameClientView.BeginMoveJettons();

	//数字滚动动画
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_GameEnd.lGameScore[i] == 0 ) continue;
		WORD wViewId = SwitchViewChairID(i);	
		m_GameClientView.m_NumberControl[wViewId].SetScore( m_GameEnd.lGameScore[i] );
	}
	m_GameClientView.BeginMoveNumber();

	//游戏变量
	m_lCellScore = 0L;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	m_bGameStart = false;

	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;
	ZeroMemory( m_byUserStatus,sizeof(m_byUserStatus) );

	//得分信息
	TCHAR szBuffer[128] = TEXT("");
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString(TEXT("本局结束,成绩统计:"));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( m_GameEnd.lGameScore[i] != 0L && m_szAccounts[i][0]!=0 )
		{
			//在聊天框显示成绩
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT(" \n%s: %+I64d分"),m_szAccounts[i],m_GameEnd.lGameScore[i]);
			if(m_pIStringMessage != NULL)
				m_pIStringMessage->InsertNormalString(szBuffer);
		}
	}

	//开始设置
	if ((IsLookonMode()==false)&&(GetMeChairID()!=INVALID_CHAIR))
	{
		SetGameClock( GetMeChairID(),IDI_START_GAME,TIME_START_GAME );
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	}

	//更新界面
	m_GameClientView.RefreshGameView(NULL);

	return;
}

//执行开始
VOID CGameClientEngine::PerformGameStart()
{
	if( m_bGameStart ) return;

	KillTimer( IDI_PERFORM_START );

	//设置变量
	m_lCellScore = m_GameStart.lCellScore;
	m_wBankerUser = m_GameStart.wBankerUser;
	ZeroMemory( m_lTableScore,sizeof(m_lTableScore) );
	m_GameClientView.SetCellScore( m_lCellScore );
	m_bGameStart = true;

	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);
		IClientUserItem* pClientUserItem=GetTableUserItem(i);
		if( pClientUserItem == NULL ) 
		{
			m_byUserStatus[i] = FALSE;
			continue;
		}

		m_byUserStatus[i] = TRUE;

		//保存玩家名
		lstrcpy( m_szAccounts[i],pClientUserItem->GetNickName() );
	}

	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
	//设置界面
	if( IsLookonMode() )
	{
		//重置筹码控件
		m_GameClientView.m_JettonControl.ResetControl();
		//设置扑克
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			m_GameClientView.m_CardControl[i].SetCardData( NULL,0 );
			m_GameClientView.m_CardControl[i].SetDisplayHead( false );
			m_GameClientView.m_CardControl[i].SetPositively( false );
			m_GameClientView.m_NumberControl[i].ResetControl();
		}
		//设置牌面分
		m_GameClientView.SetCardScore( INVALID_CHAIR,0 );
	}
	m_GameClientView.SetWaitUserScore(true);
	m_GameClientView.ShowAddJettonInfo(true);

	//设置定时器
	if( IsLookonMode() == false && GetMeChairID() != m_wBankerUser )
	{
		ActiveGameFrame();

		UpdateScoreControl();
	}

	if( GetMeChairID() != m_wBankerUser )
		SetGameClock(GetMeChairID(),IDI_USER_SCORE,TIME_USER_SCORE);

	return;
}

//玩家下注
bool CGameClientEngine::OnSubUserAddScore( const void *pData, WORD wDataSize )
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_AddScore) );
	if( wDataSize != sizeof(CMD_S_AddScore) ) return false;

	//定义变量
	CMD_S_AddScore *pAddScore = (CMD_S_AddScore *)pData;

	ASSERT( m_wCurrentUser == pAddScore->wAddScoreUser );
	if( pAddScore->wAddScoreUser != m_wCurrentUser ) return false;

	//删除定时器
	KillGameClock(IDI_OPERATE_SCORE);
	
	//停止之前筹码动画
	if( IsLookonMode() || pAddScore->wAddScoreUser != GetMeChairID() )
		m_GameClientView.StopMoveJettons();

	//设置变量
	m_wCurrentUser = pAddScore->wCurrentUser;
	BYTE cbSendCardData = pAddScore->cbCardData;

	WORD wMeChairId = GetMeChairID();
	//如果是旁观者或非自己
	if( IsLookonMode() || wMeChairId != pAddScore->wAddScoreUser )
	{
		//播放声音
		PlayActionSound( pAddScore->wAddScoreUser,AC_ADD_SCORE );

		WORD wViewId = SwitchViewChairID(pAddScore->wAddScoreUser);

		//设置玩家动作
		m_GameClientView.SetUserAction( wViewId,AC_ADD_SCORE );

		//下注动画
		m_GameClientView.OnUserAddJettons( wViewId,m_lTableScore[pAddScore->wAddScoreUser] );
		m_GameClientView.BeginMoveJettons();

		//设置积分
		m_lTableScore[pAddScore->wAddScoreUser] *= 2;
	}

	//发牌
	if( !m_GameClientView.m_JettonControl.IsPlayMoving() )
	{
		m_GameClientView.DispatchUserCard( SwitchViewChairID(pAddScore->wAddScoreUser),
			cbSendCardData );
	}
	else
	{
		m_bySendCardCount = 1;
		if( m_wStartChairId == INVALID_CHAIR )
			m_wStartChairId = pAddScore->wAddScoreUser;
		m_bySendCardData[pAddScore->wAddScoreUser][0] = cbSendCardData;
	}

	//更新界面
	m_GameClientView.RefreshGameView( NULL );

	return true;
}

//玩家放弃
bool CGameClientEngine::OnSubUserStopCard( const void *pData, WORD wDataSize )
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_GiveUp) );
	if( wDataSize != sizeof(CMD_S_GiveUp) ) return false;

	//定义变量
	CMD_S_GiveUp *pGiveUp = (CMD_S_GiveUp *)pData;

	WORD wMeChairId = GetMeChairID();

	//删除定时器
	KillGameClock( IDI_OPERATE_SCORE );

	//如果是旁观者或非自己
	if( IsLookonMode() || wMeChairId != pGiveUp->wGiveUpUser )
	{
		//播放声音
		PlayActionSound( pGiveUp->wGiveUpUser,AC_STOP_CARD );

		WORD wViewId = SwitchViewChairID(pGiveUp->wGiveUpUser);
		//设置玩家动作
		m_GameClientView.SetUserAction( wViewId,AC_STOP_CARD );
	}

	//设置当前玩家
	m_wCurrentUser = pGiveUp->wCurrentUser;

	if( !IsLookonMode() && m_wCurrentUser == wMeChairId )
	{
		ActiveGameFrame();

		UpdateScoreControl();
	}

	//设置定时器
	if( m_wCurrentUser != INVALID_CHAIR )
		SetGameClock( m_wCurrentUser,IDI_OPERATE_SCORE,TIME_OPERATE_SCORE );

	//更新界面
	m_GameClientView.RefreshGameView( NULL );
	
	return true;
}

//玩家逃跑
bool CGameClientEngine::OnSubUserLeft( const void *pData, WORD wDataSize )
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_UserLeft) );
	if( wDataSize != sizeof(CMD_S_UserLeft) ) return false;

	//定义变量
	CMD_S_UserLeft *pUserLeft = (CMD_S_UserLeft *)pData;

	m_byUserStatus[pUserLeft->wLeftUser] = FALSE;

	//逃跑玩家是当前玩家
	if( pUserLeft->wLeftUser == m_wCurrentUser )
	{
		KillGameClock( IDI_OPERATE_SCORE );

		m_wCurrentUser = pUserLeft->wCurrentUser;

		if( !IsLookonMode() && m_wCurrentUser == GetMeChairID() )
		{
			ActiveGameFrame();

			UpdateScoreControl();
		}

		if( m_wCurrentUser != INVALID_CHAIR )
			SetGameClock( m_wCurrentUser,IDI_OPERATE_SCORE,TIME_OPERATE_SCORE );
	}

	return true;
}

//游戏开始
bool CGameClientEngine::OnSubGamePlay( const void *pData, WORD wDataSize )
{
	//效验
	ASSERT( wDataSize == sizeof(CMD_S_GamePlay) );
	if( wDataSize != sizeof(CMD_S_GamePlay) ) return false;

	//定义变量
	CMD_S_GamePlay *pGamePlay = (CMD_S_GamePlay *)pData;

	//设置状态
	SetGameStatus(GS_TH_PLAY);

	//设置变量
	m_wCurrentUser = pGamePlay->wCurrentUser;

	CopyMemory( m_lTableScore,pGamePlay->lTableScore,sizeof(pGamePlay->lTableScore) );

	m_GameClientView.SetWaitUserScore(false);

	//保存牌数据
	WORD wMeChairId = GetMeChairID();
	m_wStartChairId = m_wBankerUser;
	m_bySendCardCount = 1;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
		m_bySendCardData[i][0] = pGamePlay->byCardData[i];

	//下筹动画
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_byUserStatus[i] ) continue;
		WORD wViewId = SwitchViewChairID(i);
		if( i != m_wBankerUser )
			m_GameClientView.OnUserAddJettons( wViewId,m_lTableScore[i] );
	}
	m_GameClientView.BeginMoveJettons();


	return true;
}

//////////////////////////////////////////////////////////////////////////

