#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////
//定时器
//空闲时间
#define IDI_FREE					101									//空闲时间
#define IDI_BET						102									//下注时间
#define IDI_END						103									//结束时间

#define BET_NUMBER					(10)

//////////////////////////////////////////////////////////////////////////////////

//声音路径
#define SOUND_MAX_COUNT				(13)
TCHAR g_szSoundPathT[SOUND_MAX_COUNT][MAX_PATH] = { 
													TEXT("\\Sound\\兔子.mp3"), 
													TEXT("\\Sound\\孔雀.mp3"), 
													TEXT("\\Sound\\激光.mp3"), 
													TEXT("\\Sound\\熊猫.mp3"), 
													TEXT("\\Sound\\燕子.mp3"), 
													TEXT("\\Sound\\狮子.mp3"), 
													TEXT("\\Sound\\猴子.mp3"), 
													TEXT("\\Sound\\空闲.mp3"), 
													TEXT("\\Sound\\老鹰.mp3"), 
													TEXT("\\Sound\\金鲨.mp3"), 
													TEXT("\\Sound\\金鲨银鲨.mp3"), 
													TEXT("\\Sound\\银鲨.mp3"),
													TEXT("\\Sound\\鸽子.mp3")	};
//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)

	//游戏消息
	ON_MESSAGE(IDM_UPDATE_CHIP,OnMessageUpDateChip)
	ON_MESSAGE(IDM_EXCHANGE_CHIP, OnMessageChip)
	ON_MESSAGE(IDM_PLAY_BET, OnMessagePlayBet)
	ON_MESSAGE(IDM_OPERATION_RENEWAL, OnMessageOperationRenewal)
	ON_MESSAGE(IDM_OPERATION_CANCEL, OnMessageOperationCancel)
	ON_MESSAGE(IDM_OPERATION_SWITCH, OnMessageOperationSwitch)
	ON_MESSAGE(IDM_ADMIN_COMMDN, OnAdminControl)

	//系统消息
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	m_lCellScore = 0;
	m_lPlayScore = 0;
	m_lPlayChip = 0;
	m_nTurnTableRecord = INT_MAX;
	m_lHasInChip = FALSE;

	//下注变量
	m_nCurrentNote = 0;
	m_nUnitNote = 0;
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayAllBet, sizeof(m_lPlayAllBet));
	ZeroMemory(m_hPlayAllBet, sizeof(m_hPlayAllBet));
	for ( int i = 0; i < ANIMAL_MAX; ++i )
		m_lPlayLastBet[i] = -1;
	m_lPlayWin = 0;

	//设置变量
	m_lAreaLimitScore = 0;
	m_lPlayLimitScore = 0;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
	//清除声音
	CD3DMusic::Delete();
}

//创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
	//设置属性
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	if ( !CGlobalUnits::GetInstance()->m_bAllowSound || CGlobalUnits::GetInstance()->m_bMuteStatuts )
		CD3DMusic::_Object()->ControlVolume(-10000);
	else
		CD3DMusic::_Object()->ControlVolume(0);

	return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
	//游戏变量
	m_lCellScore = 0;
	m_lPlayScore = 0;
	m_lPlayChip = 0;
	m_nTurnTableRecord = INT_MAX;
	m_lHasInChip = FALSE;

	//下注变量
	m_nCurrentNote = 0;
	m_nUnitNote = 0;
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayAllBet, sizeof(m_lPlayAllBet));
	ZeroMemory(m_hPlayAllBet, sizeof(m_hPlayAllBet));
	for ( int i = 0; i < ANIMAL_MAX; ++i )
		m_lPlayLastBet[i] = -1;
	m_lPlayWin = 0;

	//设置变量
	m_lAreaLimitScore = 0;
	m_lPlayLimitScore = 0;

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
	switch( wClockID )
	{
	case IDI_BET:
		{
			if ( nElapse <= 5 && nElapse > 1 )
			{
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\5-2倒计时.mp3"));
			}
			else if ( nElapse == 1 )
			{
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\最后倒计时.mp3"));
			}
		}
		break;
	}
	return true;
}

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//游戏消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch ( wSubCmdID )
	{
	case SUB_S_GAME_FREE:	//空闲时间
		return OnSubGameFree(pData, wDataSize);
	case SUB_S_GAME_START:	//游戏开始
		return OnSubGameStart(pData, wDataSize);
	case SUB_S_GAME_END:	//游戏结束
		return OnSubGameEnd(pData, wDataSize);
	case SUB_S_PLAY_BET:	//玩家下注
		return OnSubPlayBet(pData, wDataSize);
	case SUB_S_PLAY_BET_FAIL://下注失败
		return OnSubPlayBetFail(pData, wDataSize);
	case SUB_S_BET_CLEAR:	//清除下注
		return OnSubBetClear(pData, wDataSize);
	case SUB_S_AMDIN_COMMAND:		//设置
		{
			return OnSubAdminControl(pData,wDataSize);
		}	
	}
	return false;
}

//场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
 //开启
 if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
 m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
	switch ( cbGameStatus )
	{
	case GAME_SCENE_FREE:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置状态
			SetGameStatus(GAME_SCENE_FREE);
			m_GameClientView.SetGameStatus(GAME_SCENE_FREE);

			//设置自己位置
			m_GameClientView.SetMineChairID(SwitchViewChairID(GetMeChairID()));

			//设置时间
			SetGameClock(GetMeChairID(), IDI_FREE, pStatusFree->cbTimeLeave);

			//设置变量
			m_lCellScore = pStatusFree->lCellScore;
			m_lPlayScore = pStatusFree->lPlayScore;
			m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			m_lPlayLimitScore = pStatusFree->lPlayLimitScore;
			m_lPlayChip = 0;

			//设置游戏记录
			for( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				if ( pStatusFree->nTurnTableRecord[i] != INT_MAX )
				{
					m_GameClientView.AddTurnTableRecord( CGameLogic::GetInstance()->TurnTableAnimal(pStatusFree->nTurnTableRecord[i]) );
				}
			}
			m_GameClientView.SetShowTurnTableRecord(TRUE);
			
			//兑换界面
			if ( !IsLookonMode() )
			{
				//设置单位注
				m_nUnitNote = BET_NUMBER;
				m_nCurrentNote = BET_NUMBER;

				m_GameClientView.SetCurrentNote(m_nCurrentNote);

				//设置兑换界面
				m_lHasInChip = FALSE;
				m_GameClientView.m_WindowChip.SetChipInfo(pStatusFree->lCellScore, pStatusFree->lPlayScore, 0);
				m_GameClientView.m_WindowChip.ShowWindow(true);
			}

			//启用鱼动画
			m_GameClientView.m_FishDraw.SetAddFish(TRUE);
			m_GameClientView.m_FishDraw.SetBackIndex(0);

			//设置遮罩
			m_GameClientView.m_WindowTip.SetKeepOut(TRUE);
			m_GameClientView.m_WindowTip.CloseFlicker();

			//播放声音
			CD3DMusic::_Object()->Stop();
			if ( rand()%2 == 0 )
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲.mp3"));
			else
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲2.mp3"));
		}
		return true;
	case GAME_SCENE_BET:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置状态
			SetGameStatus(GAME_SCENE_BET);
			m_GameClientView.SetGameStatus(GAME_SCENE_BET);

			//设置自己位置
			m_GameClientView.SetMineChairID(SwitchViewChairID(GetMeChairID()));

			//设置时间
			SetGameClock(GetMeChairID(), IDI_BET, pStatusPlay->cbTimeLeave);

			//设置变量
			m_lCellScore = pStatusPlay->lCellScore;
			m_lPlayScore = pStatusPlay->lPlayScore;
			m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			m_lPlayLimitScore = pStatusPlay->lPlayLimitScore;
			m_lPlayChip = 0;

			//设置游戏记录
			for( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				if ( pStatusPlay->nTurnTableRecord[i] != INT_MAX )
				{
					m_GameClientView.AddTurnTableRecord( CGameLogic::GetInstance()->TurnTableAnimal(pStatusPlay->nTurnTableRecord[i]) );
				}
			}
			m_GameClientView.SetShowTurnTableRecord(TRUE);

			//设置全局下注
			m_GameClientView.SetAnimalMultiple( pStatusPlay->nAnimalMultiple );
			m_GameClientView.SetPlayAllBet( pStatusPlay->lAllBet );
			m_GameClientView.m_BetWnd.SetPlayAllBet(pStatusPlay->lAllBet);
			if ( !IsLookonMode() )
				m_GameClientView.SetPlayBet( pStatusPlay->lPlayBet );

			//兑换界面
			if ( !IsLookonMode() )
			{
				//设置单位注
				m_nUnitNote = BET_NUMBER;
				m_nCurrentNote = BET_NUMBER;

				m_GameClientView.SetCurrentNote(m_nCurrentNote);

				//设置筹码
				m_lPlayChip = pStatusPlay->lPlayChip;
				if( m_lPlayChip == 0 )
				{
					//设置兑换界面
					m_lHasInChip = FALSE;
					m_GameClientView.m_WindowChip.SetChipInfo(pStatusPlay->lCellScore, pStatusPlay->lPlayScore, 0);
					m_GameClientView.m_WindowChip.ShowWindow(true);
				}
				else
				{
					//设置筹码
					m_GameClientView.SetPlayChip(m_lPlayChip);
				}

				//按钮
				for ( int i = 0; i < ANIMAL_MAX; ++i )
				{
					m_GameClientView.m_btSelectBet[i].EnableWindow(true);
				}
				m_GameClientView.m_btOperationRenewal.EnableWindow(true);
				m_GameClientView.m_btOperationCancel.EnableWindow(true);
				m_GameClientView.m_btOperationSwitch.EnableWindow(true);
			}

			//启用鱼动画
			m_GameClientView.m_FishDraw.SetAddFish(TRUE);
			m_GameClientView.m_FishDraw.SetBackIndex(0);

			//设置遮罩
			m_GameClientView.m_WindowTip.SetKeepOut(FALSE);
			m_GameClientView.m_WindowTip.CloseFlicker();

			//播放声音
			CD3DMusic::_Object()->Stop();
			if ( rand()%2 == 0 )
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲.mp3"));
			else
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲2.mp3"));
		}
		return true;
	case GAME_SCENE_END:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusEnd));
			if (wDataSize!=sizeof(CMD_S_StatusEnd)) return false;

			//消息处理
			CMD_S_StatusEnd * pStatusEnd=(CMD_S_StatusEnd *)pData;

			//设置状态
			SetGameStatus(GAME_SCENE_END);
			//这里设置界面为下注状态， 以显示时间
			m_GameClientView.SetGameStatus(GAME_SCENE_BET);

			//设置自己位置
			m_GameClientView.SetMineChairID(SwitchViewChairID(GetMeChairID()));

			//设置时间
			SetGameClock(GetMeChairID(), IDI_END, pStatusEnd->cbTimeLeave);

			//设置变量
			m_lCellScore = pStatusEnd->lCellScore;
			m_lPlayScore = pStatusEnd->lPlayScore;
			m_lAreaLimitScore = pStatusEnd->lAreaLimitScore;
			m_lPlayLimitScore = pStatusEnd->lPlayLimitScore;
			m_lPlayChip = 0;

			//设置游戏记录
			for( int i = 0; i < RECORD_COUNT_MAX; ++i )
			{
				if ( pStatusEnd->nTurnTableRecord[i] != INT_MAX )
				{
					m_GameClientView.AddTurnTableRecord( CGameLogic::GetInstance()->TurnTableAnimal(pStatusEnd->nTurnTableRecord[i]) );
				}
			}
			m_GameClientView.SetShowTurnTableRecord(TRUE);

			//设置全局下注
			m_GameClientView.SetAnimalMultiple( pStatusEnd->nAnimalMultiple );
			m_GameClientView.SetPlayAllBet( pStatusEnd->lAllBet );
			m_GameClientView.m_BetWnd.SetPlayAllBet(pStatusEnd->lAllBet);
			if ( !IsLookonMode() )
				m_GameClientView.SetPlayBet( pStatusEnd->lPlayBet );

			//兑换界面
			if ( !IsLookonMode() )
			{
				//设置单位注
				m_nUnitNote = BET_NUMBER;
				m_nCurrentNote = BET_NUMBER;

				m_GameClientView.SetCurrentNote(m_nCurrentNote);

				//设置筹码
				m_lPlayChip = pStatusEnd->lPlayChip;
				if( m_lPlayChip == 0 )
				{
					//设置兑换界面
					m_lHasInChip = FALSE;
					m_GameClientView.m_WindowChip.SetChipInfo(pStatusEnd->lCellScore, pStatusEnd->lPlayScore, 0);
					m_GameClientView.m_WindowChip.ShowWindow(true);
				}
				else
				{
					//设置筹码
					m_GameClientView.SetPlayChip(m_lPlayChip);
				}
			}

			//启用鱼动画
			m_GameClientView.m_FishDraw.SetAddFish(TRUE);
			m_GameClientView.m_FishDraw.SetBackIndex(0);

			//设置遮罩
			m_GameClientView.m_WindowTip.SetKeepOut(TRUE);
			m_GameClientView.m_WindowTip.CloseFlicker();

			//播放声音
			CD3DMusic::_Object()->Stop();
			if ( rand()%2 == 0 )
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲.mp3"));
			else
				CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲2.mp3"));
		}
		return true;
	}

	return false;
}


//设置事件
bool CGameClientEngine::OnGameOptionChange()
{
	if ( !CGlobalUnits::GetInstance()->m_bAllowSound || CGlobalUnits::GetInstance()->m_bMuteStatuts )
		CD3DMusic::_Object()->ControlVolume(-10000);
	else
		CD3DMusic::_Object()->ControlVolume(0);

	return true;
}

//声音控制
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	if ( !CGlobalUnits::GetInstance()->m_bAllowSound || CGlobalUnits::GetInstance()->m_bMuteStatuts )
		CD3DMusic::_Object()->ControlVolume(-10000);
	else
		CD3DMusic::_Object()->ControlVolume(0);

	return true;
}

//空闲时间
bool CGameClientEngine::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//消息处理
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;
	WORD wMeChair = GetMeChairID();
	if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
		m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

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
	WORD wMeChair = GetMeChairID();

	//预执行消息

	//添加的赢的钱
	m_lPlayChip += m_lPlayWin;
	m_lPlayWin = 0;
	m_GameClientView.SetPlayChip(m_lPlayChip);
	m_GameClientView.SetPlayShowWin(0);
	m_GameClientView.SetPlayShowPrizes(0);

	//设置倍数
	int nAnimalMultiple[ANIMAL_MAX] = {0};
	ZeroMemory(nAnimalMultiple, sizeof(nAnimalMultiple));
	m_GameClientView.SetAnimalMultiple(nAnimalMultiple);

	//保存上一次
	for( int i = 0; i < ANIMAL_MAX; ++i )
	{
		if ( m_lPlayBet[i] != 0 )
		{
			CopyMemory(m_lPlayLastBet, m_lPlayBet, sizeof(m_lPlayBet));
			break;
		}
	}

	//玩家下注清空
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayAllBet, sizeof(m_lPlayAllBet));
	ZeroMemory(m_hPlayAllBet, sizeof(m_hPlayAllBet));

	//设置记录
	if ( m_nTurnTableRecord != INT_MAX )
	{
		m_GameClientView.AddTurnTableRecord( CGameLogic::GetInstance()->TurnTableAnimal(m_nTurnTableRecord) );
	}
	m_GameClientView.SetShowTurnTableRecord(TRUE);

	//设置界面
	m_GameClientView.EndWin();
	m_GameClientView.SetPlayBet(m_lPlayBet);
	m_GameClientView.SetPlayAllBet(m_lPlayAllBet);
	m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet);
	m_GameClientView.EndTurnTable();
	m_GameClientView.m_WindowOver.ShowWindow(false);
	m_GameClientView.m_FishDraw.SetBackIndex(0);

	//启用鱼动画
	m_GameClientView.m_FishDraw.SetAddFish(TRUE);

	//设置遮罩
	m_GameClientView.m_WindowTip.SetKeepOut(TRUE);
	m_GameClientView.m_WindowTip.CloseFlicker();

	//设置是否关闭
	static bool bClose = false;
	if ( m_lPlayChip <= 0 && m_lHasInChip && !bClose )
	{
		bClose = true;
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您的筹码已用完，请退出游戏增购筹码继续游戏！"));
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return true;
	}

	//播放声音
	CD3DMusic::_Object()->Stop();
	if ( rand()%2 == 0 )
		CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲.mp3"));
	else
		CD3DMusic::_Object()->Start(TEXT("\\Sound\\空闲2.mp3"));

	//原始游戏开始下注消息
	//设置状态
	SetGameStatus(GAME_SCENE_BET);
	m_GameClientView.SetGameStatus(GAME_SCENE_BET);

	//设置时间
	SetGameClock(wMeChair, IDI_BET, pGameStart->cbTimeLeave);

	//设置倍数
	m_GameClientView.SetAnimalMultiple(pGameStart->nAnimalMultiple);

	//玩家下注清空
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayAllBet, sizeof(m_lPlayAllBet));
	ZeroMemory(m_hPlayAllBet, sizeof(m_hPlayAllBet));

	//设置界面
	m_GameClientView.EndWin();
	m_GameClientView.SetPlayBet(m_lPlayBet);
	m_GameClientView.SetPlayAllBet(m_lPlayAllBet);
	m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet);
	m_GameClientView.EndTurnTable();
	m_GameClientView.m_WindowOver.ShowWindow(false);

	if ( !IsLookonMode() )
	{
		//开启按钮
		for ( int i = 0; i < ANIMAL_MAX; ++i )
		{
			m_GameClientView.m_btSelectBet[i].EnableWindow(true);
		}
		m_GameClientView.m_btOperationRenewal.EnableWindow(true);
		m_GameClientView.m_btOperationCancel.EnableWindow(true);
		m_GameClientView.m_btOperationSwitch.EnableWindow(true);
	}

	//设置遮罩
	m_GameClientView.m_WindowTip.SetKeepOut(FALSE);
	m_GameClientView.m_WindowTip.CloseFlicker();
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_BIRD],55,83,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_BEAST],168,83,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_GOLD_SHARK],55,130,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_SLIVER_SHARK],168,130,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_LION],28,200,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PANDA],162,200,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_MONKEY],28,258,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_RABBIT],162,258,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_EAGLE],28,315,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PEACOCK],162,315,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PIGEON],28,378,false);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_SWALLOW],162,378,false);
	if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
		m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
	return true;
}

//游戏结算
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	WORD wMeChair = GetMeChairID();

	//设置状态
	SetGameStatus(GAME_SCENE_END);
	m_GameClientView.SetGameStatus(GAME_SCENE_END);

	//设置时间
	SetGameClock(wMeChair, IDI_END, pGameEnd->cbTimeLeave);

	//结束动画
	m_GameClientView.GameOver(pGameEnd->bTurnTwoTime, pGameEnd->nTurnTableTarget, pGameEnd->nPrizesMultiple, pGameEnd->lPlayWin, pGameEnd->lPlayPrizes );

	//保存赢的钱
	m_lPlayWin = pGameEnd->lPlayWin[0] + pGameEnd->lPlayWin[1] + pGameEnd->lPlayPrizes;

	//保存记录
	m_nTurnTableRecord = pGameEnd->nTurnTableTarget[0];
	m_GameClientView.SetShowTurnTableRecord(FALSE);

	//关闭按钮
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		m_GameClientView.m_btSelectBet[i].EnableWindow(false);
	}
	m_GameClientView.m_btOperationRenewal.EnableWindow(false);
	m_GameClientView.m_btOperationCancel.EnableWindow(false);
	m_GameClientView.m_btOperationSwitch.EnableWindow(false);
	m_GameClientView.m_FishDraw.SetBackIndex(2);

	//关闭鱼动画
	m_GameClientView.m_FishDraw.SetAddFish(FALSE);

	//设置遮罩
	m_GameClientView.m_WindowTip.SetKeepOut(TRUE);

	//设置彩金
	m_GameClientView.SetPlayShowPrizes(pGameEnd->lPlayShowPrizes);
	if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
		m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

	return true;
}

//玩家下注
bool CGameClientEngine::OnSubPlayBet(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayBet));
	if (wDataSize!=sizeof(CMD_S_PlayBet)) return false;

	//消息处理
	CMD_S_PlayBet * pPlayBet=(CMD_S_PlayBet *)pBuffer;

	//非自己才处理
	if ( GetMeChairID() != pPlayBet->wChairID || IsLookonMode() )
	{
		//设置变量
		m_lPlayAllBet[pPlayBet->nAnimalIndex] += pPlayBet->lBetChip;

		//设置界面
		m_GameClientView.SetPlayAllBet(m_lPlayAllBet[pPlayBet->nAnimalIndex], pPlayBet->nAnimalIndex);
		m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet[pPlayBet->nAnimalIndex], pPlayBet->nAnimalIndex);
	}
	m_hPlayAllBet[pPlayBet->nAnimalIndex] += pPlayBet->mHBet;
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_BIRD],55,83);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_BEAST],168,83);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_GOLD_SHARK],55,130);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_SLIVER_SHARK],168,130);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_LION],28,200);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PANDA],162,200);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_MONKEY],28,258);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_RABBIT],162,258);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_EAGLE],28,315);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PEACOCK],162,315);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_PIGEON],28,378);
	m_GameClientView.m_pClientControlDlg->SetText(m_hPlayAllBet[ANIMAL_SWALLOW],162,378);
	if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
		m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
	return true;
}

//下注失败
bool CGameClientEngine::OnSubPlayBetFail(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlayBetFail));
	if (wDataSize!=sizeof(CMD_S_PlayBetFail)) return false;

	//消息处理
	CMD_S_PlayBetFail * pPlayBetFail = (CMD_S_PlayBetFail *)pBuffer;

	//自己才处理
	if ( GetMeChairID() == pPlayBetFail->wChairID && !IsLookonMode() )
	{
		//设置变量
		m_lPlayChip += pPlayBetFail->lBetChip;
		m_lPlayBet[pPlayBetFail->nAnimalIndex] -= pPlayBetFail->lBetChip;
		m_lPlayAllBet[pPlayBetFail->nAnimalIndex] -= pPlayBetFail->lBetChip;

		//设置界面
		m_GameClientView.SetPlayChip(m_lPlayChip);
		m_GameClientView.SetPlayBet(m_lPlayBet[pPlayBetFail->nAnimalIndex], pPlayBetFail->nAnimalIndex);
		m_GameClientView.SetPlayAllBet(m_lPlayAllBet[pPlayBetFail->nAnimalIndex], pPlayBetFail->nAnimalIndex);
		m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet[pPlayBetFail->nAnimalIndex], pPlayBetFail->nAnimalIndex);
	}

	return true;
}

//清除下注
bool CGameClientEngine::OnSubBetClear(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BetClear));
	if (wDataSize!=sizeof(CMD_S_BetClear)) return false;

	//消息处理
	CMD_S_BetClear * pBetClear=(CMD_S_BetClear *)pBuffer;

	//非自己才处理
	if ( GetMeChairID() != pBetClear->wChairID || IsLookonMode() )
	{
		//清空变量
		for ( int i = 0; i < ANIMAL_MAX; ++i )
		{
			if ( pBetClear->lPlayBet[i] > 0 )
			{
				m_lPlayAllBet[i] -= pBetClear->lPlayBet[i];

				//设置界面
				m_GameClientView.SetPlayAllBet(m_lPlayAllBet[i], i);
				m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet[i], i);
			}
		}
	}
	return true;
}

//定时器
void CGameClientEngine::OnTimer(UINT nIDEvent)
{
	CGameFrameEngine::OnTimer(nIDEvent);
}

//更新筹码
LRESULT CGameClientEngine::OnMessageUpDateChip(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//兑换筹码消息
LRESULT CGameClientEngine::OnMessageChip( WPARAM wParam, LPARAM lParam )
{
	//转换变量
	LONGLONG lPlayChip = *((LONGLONG*)wParam);

	//判断条件
	if ( lPlayChip * m_lCellScore > m_lPlayScore )
		return 0;

	//设置变量
	m_lPlayChip = lPlayChip;

	//设置界面
	m_GameClientView.SetPlayChip(m_lPlayChip);

	//发送消息
	CMD_C_Chip CMDChip;
	CMDChip.lChip = m_lPlayChip;
	SendSocketData(SUB_C_EXCHANGE_CHIP, &CMDChip, sizeof(CMDChip));

	//已经兑换筹码
	m_lHasInChip = TRUE;

	//设置焦点
	m_GameClientView.SetFocus();

	return 0;
}

//下注消息
LRESULT CGameClientEngine::OnMessagePlayBet(WPARAM wParam, LPARAM lParam)
{
	//判断信息
	if ( wParam < 0 || wParam >= ANIMAL_MAX )
		return 0;

	if ( m_lPlayChip < m_nCurrentNote )
		return 0;

	if ( GetGameStatus() != GAME_SCENE_BET  )
		return 0;

	//定义变量
	int nAnimalIndex = (int)wParam;

	//判断超额
	if ( (m_lAreaLimitScore != 0 && m_lPlayAllBet[nAnimalIndex] + m_nCurrentNote > m_lAreaLimitScore)
		|| (m_lPlayLimitScore != 0 && m_lPlayBet[nAnimalIndex] + m_nCurrentNote > m_lPlayLimitScore) )
	{
		return 0;
	}

	//设置变量
	m_lPlayChip -= m_nCurrentNote;
	m_lPlayBet[nAnimalIndex] += m_nCurrentNote;
	m_lPlayAllBet[nAnimalIndex] += m_nCurrentNote;

	//设置界面
	m_GameClientView.SetPlayChip(m_lPlayChip);
	m_GameClientView.SetPlayBet(m_lPlayBet[nAnimalIndex], nAnimalIndex);
	m_GameClientView.SetPlayAllBet(m_lPlayAllBet[nAnimalIndex], nAnimalIndex);
	m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet[nAnimalIndex], nAnimalIndex);

	//发送消息
	CMD_C_PlayBet CMDPlayBet;
	CMDPlayBet.lBetChip = m_nCurrentNote;
	CMDPlayBet.nAnimalIndex = (int)wParam;
	SendSocketData(SUB_C_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));

	if ( CGlobalUnits::GetInstance()->m_bAllowSound && !CGlobalUnits::GetInstance()->m_bMuteStatuts )
	{
		CD3DMusic::_Object()->Start(TEXT("\\Sound\\压分.mp3"));
	}

	return 0;
}

//续压消息
LRESULT CGameClientEngine::OnMessageOperationRenewal(WPARAM wParam, LPARAM lParam)
{
	//第一局
	if ( m_lPlayLastBet[0] == -1 )
		return 0;
	
	//先清空
	SendSocketData(SUB_C_BET_CLEAR);
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		m_lPlayChip += m_lPlayBet[i];
		m_lPlayAllBet[i] -= m_lPlayBet[i];
		m_lPlayBet[i] = 0;
	}

	//判断是否够下注
	LONGLONG lPlayChip = 0;
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		//设置变量
		lPlayChip += m_lPlayLastBet[i];
	}
	if( lPlayChip > m_lPlayChip )
	{
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您的筹码不足，无法续压上轮下注！"));
		return 0;
	}

	//重新下注
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		//设置变量
		m_lPlayChip -= m_lPlayLastBet[i];
		m_lPlayBet[i] += m_lPlayLastBet[i];
		m_lPlayAllBet[i] += m_lPlayLastBet[i];

		//发送消息
		CMD_C_PlayBet CMDPlayBet;
		CMDPlayBet.lBetChip = m_lPlayLastBet[i];
		CMDPlayBet.nAnimalIndex = i;
		SendSocketData(SUB_C_PLAY_BET, &CMDPlayBet, sizeof(CMDPlayBet));
	}

	//设置界面
	m_GameClientView.SetPlayChip(m_lPlayChip);
	m_GameClientView.SetPlayBet(m_lPlayBet);
	m_GameClientView.SetPlayAllBet(m_lPlayAllBet);
	m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet);

	if ( CGlobalUnits::GetInstance()->m_bAllowSound && !CGlobalUnits::GetInstance()->m_bMuteStatuts )
	{
		CD3DMusic::_Object()->Start(TEXT("\\Sound\\压分.mp3"));
	}

	return 0;
}

//取消消息
LRESULT CGameClientEngine::OnMessageOperationCancel(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendSocketData(SUB_C_BET_CLEAR);

	//清空变量
	for ( int i = 0; i < ANIMAL_MAX; ++i )
	{
		m_lPlayChip += m_lPlayBet[i];
		m_lPlayAllBet[i] -= m_lPlayBet[i];
		m_lPlayBet[i] = 0;

		//设置界面
		m_GameClientView.SetPlayChip(m_lPlayChip);
		m_GameClientView.SetPlayBet(m_lPlayBet[i], i);
		m_GameClientView.SetPlayAllBet(m_lPlayAllBet[i], i);
		m_GameClientView.m_BetWnd.SetPlayAllBet(m_lPlayAllBet[i], i);
	}

	return 0;
}

//切换消息
LRESULT CGameClientEngine::OnMessageOperationSwitch(WPARAM wParam, LPARAM lParam)
{
	//切换单位值
	int nNote[] = { BET_NUMBER, BET_NUMBER*10, BET_NUMBER*100 };
	for ( int i = 0; i < CountArray(nNote); ++i )
	{
		if ( m_nUnitNote == nNote[i] )
		{
			m_nUnitNote = nNote[(i + 1)%CountArray(nNote)];
			m_nCurrentNote = m_nUnitNote;
			break;
		}
	}

	//设置界面
	m_GameClientView.SetCurrentNote(m_nCurrentNote);

	return 0;
}
//控制
bool CGameClientEngine::OnSubAdminControl(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_CommandResult));
	if(wDataSize!=sizeof(CMD_S_CommandResult)) return false;

	m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
	return true;
}

//控制
LRESULT CGameClientEngine::OnAdminControl( WPARAM wParam, LPARAM lParam )
{
	
	SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
	
	return true;
}



