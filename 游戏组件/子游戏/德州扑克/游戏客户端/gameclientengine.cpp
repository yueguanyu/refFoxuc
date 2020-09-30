#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME				200									//开始定时器
#define IDI_USER_ADD_SCORE			201									//加注定时器

//时间标识
#define TIME_START_GAME				30	/*20*/								//开始定时器
#define TIME_USER_ADD_SCORE			30	/*20*/								//放弃定时器

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,		OnStart)
	ON_MESSAGE(IDM_EXIT,		OnExit)
	ON_MESSAGE(IDM_GIVEUP,		OnGiveUp)
	ON_MESSAGE(IDM_ADD_SCORE,	OnAddScore)
	ON_MESSAGE(IDM_MIN_SCORE,	OnMinScore)
	ON_MESSAGE(IDM_MAX_SCORE,	OnMaxScore)
	ON_MESSAGE(IDM_OK_SCORE,	OnOKScore)
	ON_MESSAGE(IDM_CANCEL_SCORE,OnCancelScore)
	ON_MESSAGE(IDM_PASS_CARD,	OnPassCard)
	ON_MESSAGE(IDM_FOLLOW,		OnFollow)
	ON_MESSAGE(IDM_SHOWHAND,	OnShowHand)
	ON_MESSAGE(IDM_START_TIMES,	OnStartTimes)
	ON_MESSAGE(IDM_AUTO_START,	OnAutoStart)
	ON_MESSAGE(IDM_SIT_DOWN,	OnSitDown)
	ON_MESSAGE(IDM_GAME_OVER,	OnGameOver)
	ON_MESSAGE(IDM_SEND_FINISH,	OnSendFinish)
	ON_MESSAGE(IDM_OPEN_CARD,	OnOpenCard)
END_MESSAGE_MAP()

////输出信息
//void TraceMessage(LPCTSTR pszMessage)
//{
//	CFile File;
//	if ((File.Open(TEXT("TraceData.txt"),CFile::modeWrite)==FALSE)&&
//		(File.Open(TEXT("TraceData.txt"),CFile::modeWrite|CFile::modeCreate)==FALSE))
//	{
//		ASSERT(FALSE);
//		return;
//	}
//
//	File.SeekToEnd();
//	File.Write(pszMessage,lstrlen(pszMessage));
//	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
//
//	File.Flush();
//	File.Close();
//
//	return;
//}
//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
	//游戏变量
	//m_wMeChairID = INVALID_CHAIR;
	ZeroMemory(m_bUserName,sizeof(m_bUserName));
	m_wDUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_bOpenCard = false;
	m_bExitTag	= false;

	//加注信息
	m_lAddLessScore = 0L;
	m_lTurnLessScore = 0L;
	m_lBalanceScore = 0L;
	m_lCenterScore = 0L;
	m_lCellScore = 0L;
	//ZeroMemory(m_lCurrentScore,sizeof(m_lCurrentScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//扑克变量
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbOverCardData,sizeof(m_cbOverCardData));

	//状态变量
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_bAutoStart = FALSE;
	m_bReset =true;

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

	return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
	if(!m_bReset)return true;
	//游戏变量
	m_wDUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_bOpenCard = false;
	m_bExitTag	= false;

	//加注信息
	m_lAddLessScore = 0L;
	m_lTurnLessScore = 0L;
	m_lBalanceScore = 0L;
	m_lCenterScore = 0L;
	m_lCellScore = 0L;
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//扑克变量
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbOverCardData,sizeof(m_cbOverCardData));

	//状态变量
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_bAutoStart = FALSE;

	return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	//GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
	//GameOption.m_bAllowLookon=m_pGlobalUnits->m_bAllowLookon;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置配置
		//m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
		//m_pGlobalUnits->m_bAllowLookon=GameOption.m_bAllowLookon;
	}

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
	switch(wClockID)
	{
	case IDI_START_GAME:
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 					
					AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
				return true;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			return true;
		}
	case IDI_USER_ADD_SCORE:
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					//删除定时器
					KillGameClock(IDI_USER_ADD_SCORE);
					OnGiveUp(0,0);
				}
				return true;
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
	//判断旁观
	WORD wMeChiarID=GetMeChairID();
	if(IsLookonMode())
	{
		if(IsAllowLookon())
		{
			m_GameClientView.m_CardControl[wMeChiarID].SetCardData(m_cbHandCardData[wMeChiarID],MAX_COUNT);
			m_GameClientView.m_CardControl[wMeChiarID].SetDisplayItem(true);
		}
		else 
		{
			m_GameClientView.m_CardControl[wMeChiarID].SetCardData(NULL,0);
			m_GameClientView.m_CardControl[wMeChiarID].SetDisplayItem(false);
		}
	}
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
	case SUB_S_ADD_SCORE:		//用户加注
		{
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_OPEN_CARD:		//用户开牌
		{
			return OnSubOpenCard(pData,wDataSize);
		}
	}
	return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	//显示用户
	m_GameClientView.SetShowUserView(TRUE);

	//物理位置
	m_wMeChairID = GetMeChairID();
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetUserPost(i,SwitchViewChairID(i));
		m_GameClientView.SetMePost(m_wMeChairID);
	}

	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//设置状态
			SetGameStatus(GAME_STATUS_FREE);

			//下注信息
			m_GameClientView.SetTitleInfo(pStatusFree->lCellMinScore,pStatusFree->lCellMaxScore);

			//设置控件
			if (IsLookonMode()==false && GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				m_GameClientView.m_btExit.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoStart.ShowWindow(SW_SHOW);
				//设置时间
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}
			//旁观设置
			OnEventLookonMode(NULL,0);

			return true;
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置状态
			SetGameStatus(GAME_STATUS_PLAY);

			//设置变量
			m_wDUser = pStatusPlay->wDUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;//当前玩家
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));//下注数目
			CopyMemory(m_lTotalScore,pStatusPlay->lTotalScore,sizeof(m_lTotalScore));//下注数目
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));//用户游戏状态

			//加注信息
			m_lAddLessScore = pStatusPlay->lAddLessScore;
			m_lCellScore	= pStatusPlay->lCellScore;
			m_lTurnMaxScore = pStatusPlay->lTurnMaxScore;
			m_lTurnLessScore= pStatusPlay->lTurnLessScore;
			m_lCenterScore	= pStatusPlay->lCenterScore;
			CopyMemory(m_cbHandCardData[GetMeChairID()],pStatusPlay->cbHandCardData,MAX_COUNT);
			CopyMemory(m_cbCenterCardData,pStatusPlay->cbCenterCardData,sizeof(m_cbCenterCardData));

			//设置扑克
			if(!IsLookonMode())
			{
				if(m_cbPlayStatus[GetMeChairID()]==TRUE)
					m_GameClientView.m_CardControl[GetMeChairID()].SetCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
				m_GameClientView.m_btAutoStart.ShowWindow(SW_SHOW);
			}

			//中心扑克
			if(pStatusPlay->cbBalanceCount >0)
			{
				BYTE cbTempCount = pStatusPlay->cbBalanceCount+2;
				m_GameClientView.m_CenterCardControl.SetCardData(m_cbCenterCardData,min(cbTempCount,MAX_CENTERCOUNT));
			}

			//设置界面
			for (WORD i =0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i] == FALSE)continue;
				m_GameClientView.m_SmallCardControl[i].SetCardData(MAX_COUNT);
				m_GameClientView.SetUserTableScore(i,m_lTableScore[i]);
				m_GameClientView.SetTotalScore(i,m_lTotalScore[i]);
			}
			m_GameClientView.SetDFlag(m_wDUser);
			m_GameClientView.SetTitleInfo(m_lCellScore,pStatusPlay->lCellMaxScore);
			m_GameClientView.SetCenterScore(m_lCenterScore);

			//更新界面
			m_GameClientView.RefreshGameView();

			//当前玩家
			if((IsLookonMode() == false)&&(m_wCurrentUser == GetMeChairID()))
				UpdateScoreControl();

			//设置时间
			SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

			//旁观设置
			OnEventLookonMode(NULL,0);

			//坐下按钮
			if(IsLookonMode())
			{
				m_GameClientView.SetMyLookOn(GetMeChairID());

				//更新界面
				m_GameClientView.RefreshGameView();
			}

			return true;
		}
	}
	return false;
}

//框架消息
bool CGameClientEngine::OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	/*switch(wSubCmdID)
	{
	case SUB_GF_MESSAGE:		//系统消息
		{
			//效验参数
			CMD_GF_Message * pMessage=(CMD_GF_Message *)pBuffer;
			ASSERT(wDataSize>(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent)));
			if (wDataSize<=(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent))) return false;

			//关闭房间
			if (pMessage->wMessageType&SMT_CLOSE_GAME) 
			{
				//隐藏按钮
            	m_GameClientView.m_btAutoStart.ShowWindow(SW_HIDE); 
    	        m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	            m_GameClientView.m_btExit.ShowWindow(SW_SHOW);

             	//停止复位
	            m_bReset = false;
	            m_GameClientView.SetResetTag();
	            ZeroMemory(m_bUserName,sizeof(m_bUserName));
              	for (WORD i=0;i<GAME_PLAYER;i++)
	            {
	               //变量定义 
	               IClientUserItem * pClientUserItem=GetTableUserItem(i);
                   if(pClientUserItem!=NULL)
	               {
                       //用户名字
		               CopyMemory(&m_bUserName[i],pClientUserItem->szName,sizeof(pClientUserItem->szName));

			          //用户姓别
          		    	WORD wTemp = ((pClientUserItem->cbGender!=2)?2:1);
	             		m_GameClientView.SetUserGender(i,wTemp);
		           }
                }

            	//信息提示
            	TCHAR szBuffer[1024]=TEXT("");
	            _sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n［系统信息］"));
	            InsertGeneralString(szBuffer,RGB(255,0,0),true);

             	InsertGeneralString(pMessage->szContent,RGB(0,0,255),true);

            	//提示框
	            m_GameClientView.m_Prompt.SetString(pMessage->szContent);

             	//设置定时器
	            m_GameClientView.SetNoScoreDlg();

				return true;
			}
		}
	}*/
	return false;
}

//开始按钮
LRESULT	CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_START_GAME);
	m_GameClientView.SetGameEndEnd();
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置界面	
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_btExit.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.SetStartTimes(false);
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//加注变量
	m_lCenterScore = 0L;
	m_lCellScore = 0L;

	//设置界面
	for (WORD i = 0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetUserTableScore(i,m_lTableScore[i]);
		m_GameClientView.SetTotalScore(i,0L);
		m_GameClientView.m_SmallCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
	}
	m_GameClientView.m_CenterCardControl.SetCardData(NULL,0);
	m_GameClientView.SetDFlag(INVALID_CHAIR);
	m_GameClientView.SetCenterScore(0L);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//离开按钮
LRESULT	CGameClientEngine::OnExit(WPARAM wParam, LPARAM lParam)
{
	//界面设置
	//KillGameClock(IDI_USER_ADD_SCORE);
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));
#ifdef _DEBUG
	//m_GameClientView.UpdateFrameSize();
	//return 0;
#endif
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return 0;
}

//放弃按钮
LRESULT CGameClientEngine::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//界面设置
	KillGameClock(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//发送消息
	SendSocketData(SUB_C_GIVE_UP);

	HideScoreControl();

	return 0;
}

//跟注按钮
LRESULT CGameClientEngine::OnFollow(WPARAM wParam,LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);

	//获取筹码
	WORD wMeChairID=GetMeChairID();
	m_lTableScore[wMeChairID] +=m_lTurnLessScore;
	m_lTotalScore[wMeChairID] += m_lTurnLessScore;
	m_GameClientView.SetTotalScore(wMeChairID,m_lTotalScore[wMeChairID]);

	if(m_lTableScore[wMeChairID]!=0L)
		m_GameClientView.DrawMoveAnte(wMeChairID,CGameClientView::AA_BASEFROM_TO_BASEDEST,m_lTurnLessScore);

	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	HideScoreControl();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=m_lTurnLessScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//加注按钮 
LRESULT	CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	if (m_GameClientView.m_GoldControl.IsWindowVisible()==FALSE)
	{
		m_GameClientView.m_GoldControl.SetMaxGold((LONGLONG)m_lTurnMaxScore);
		m_GameClientView.m_GoldControl.SetMinGold((LONGLONG)(min(m_lAddLessScore,m_lTurnMaxScore)));
		m_GameClientView.m_GoldControl.SetGold((LONGLONG)(min(m_lAddLessScore,m_lTurnMaxScore)));
		m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);
	}

	return 0; 
}

//最少加注
LRESULT	CGameClientEngine::OnMinScore(WPARAM wParam, LPARAM lParam)
{	
	//最少筹码
	m_GameClientView.m_GoldControl.SetGold((LONGLONG)(min(m_lAddLessScore,m_lTurnMaxScore)));

	return 0;
}

//最大加注
LRESULT	CGameClientEngine::OnMaxScore(WPARAM wParam, LPARAM lParam)
{	
	//最大筹码
	m_GameClientView.m_GoldControl.SetGold((LONGLONG)m_lTurnMaxScore);

	return 0;
}

//确定消息
LRESULT CGameClientEngine::OnOKScore(WPARAM wParam,LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);
	HideScoreControl();

	//获取筹码
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=m_GameClientView.m_GoldControl.GetGold();
	m_lTableScore[GetMeChairID()] += lCurrentScore;
	m_lTotalScore[GetMeChairID()] += lCurrentScore;
	m_GameClientView.SetTotalScore(wMeChairID,m_lTotalScore[wMeChairID]);

	if(lCurrentScore>0L)
		m_GameClientView.DrawMoveAnte(wMeChairID,CGameClientView::AA_BASEFROM_TO_BASEDEST,lCurrentScore);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//取消消息
LRESULT CGameClientEngine::OnCancelScore(WPARAM wParam,LPARAM lParam)
{
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);

	return 0;
}

//让牌消息
LRESULT CGameClientEngine::OnPassCard(WPARAM wParam,LPARAM lParam)
{
	//删除定时器
	KillGameClock(IDI_USER_ADD_SCORE);
	HideScoreControl();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=0L;
	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//声音效果
	PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));

	return 0;
}

//梭哈消息
LRESULT CGameClientEngine::OnShowHand(WPARAM wParam,LPARAM lParam)
{
	//构造数据
	ShowHand ShowHandObj;

	//配置数据
	if (ShowHandObj.DoModal()==IDOK)
	{
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			IClientUserItem * pClientUserItem = GetTableUserItem(i);
			if(pClientUserItem!=NULL && pClientUserItem->GetUserStatus()==US_PLAYING)break;
		}
		if(i<GAME_PLAYER)
		{
			//删除定时器
			KillGameClock(IDI_USER_ADD_SCORE);

			//获取筹码
			WORD wMeChairID=GetMeChairID();
			m_lTableScore[GetMeChairID()] +=m_lTurnMaxScore;
			m_lTotalScore[GetMeChairID()] += m_lTurnMaxScore;
			m_GameClientView.SetTotalScore(GetMeChairID(),m_lTotalScore[GetMeChairID()]);
			m_GameClientView.SetUserTableScore(GetMeChairID(),m_lTableScore[GetMeChairID()]);

			m_GameClientView.RefreshGameView();
			HideScoreControl();

			//发送消息
			CMD_C_AddScore AddScore;
			AddScore.lScore=m_lTurnMaxScore;
			SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//声音效果
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
		}
	}

	return 0;
}

//开始倒计
LRESULT CGameClientEngine::OnStartTimes(WPARAM wParam,LPARAM lParam)
{
	//隐藏控件
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	if(m_bAutoStart!=TRUE)
	{
		SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}
	else //自动开始
	{
		OnStart(0,0);
	}

	return 0;
}

//自动开始
LRESULT CGameClientEngine::OnAutoStart(WPARAM wParam,LPARAM lParam)
{
	m_bAutoStart = !m_bAutoStart;
	HINSTANCE hInstance=AfxGetInstanceHandle();
	if(m_bAutoStart==TRUE)
	{
		if(m_GameClientView.m_btStart.IsWindowVisible()==TRUE)
		{
			OnStart(0,0);
		}
		m_GameClientView.m_btAutoStart.SetButtonImage(IDB_AUTO_START_TRUE,hInstance,false,false);
	}
	else
	{
		IClientUserItem *pClientUserItem = GetTableUserItem(GetMeChairID());
		if(pClientUserItem->GetUserStatus()!=US_READY && pClientUserItem->GetUserStatus()!=US_PLAYING &&
			m_GameClientView.m_btStart.IsWindowVisible()==FALSE)
		{
			m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
			m_GameClientView.m_btExit.ShowWindow(SW_SHOW);
		}
		m_GameClientView.m_btAutoStart.SetButtonImage(IDB_AUTO_START_FALSE,hInstance,false,false);
	}

	return 0;
}

//坐下按钮
LRESULT CGameClientEngine::OnSitDown(WPARAM wParam,LPARAM lParam)
{
	//状态过滤
	//MessageBox("此桌正在游戏中,您暂时不能加入。");
	//if(GetGameStatus()==GS_PLAYING)return 0; 

	//隐藏控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_btSitDown[i].ShowWindow(SW_HIDE);
	}

	//取消状态
	m_GameClientView.SetMyLookOn(INVALID_CHAIR);

	//坐下游戏
	//JoinInGameTable(LOWORD(wParam));
	//IClientKernel * pIClientKernel=(IClientKernel*)GetClientKernel(IID_IClientKernel,VER_IClientKernel);

	////构造数据
	//IPC_JoinInGame JoinInGame;
	//JoinInGame.wTableID=pIClientKernel->GetMeUserInfo()->wTableID;
	//JoinInGame.wChairID=LOWORD(wParam);

	////发送数据
	//ASSERT(pIClientKernel!=NULL);
	//if(pIClientKernel==NULL) return 0;
	//pIClientKernel->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_JOIN_IN_GAME,&JoinInGame,sizeof(JoinInGame));

	return 0;
}

//游戏结束
LRESULT CGameClientEngine::OnGameOver(WPARAM wParam,LPARAM lParam)
{
	//设置状态
	SetGameStatus(GAME_STATUS_FREE);

	if(!m_bExitTag)
	{
		if(m_cbPlayStatus[m_wMeChairID] == TRUE)
			m_GameClientView.m_CardControl[m_wMeChairID].SetCardData(m_cbHandCardData[m_wMeChairID],2);
	}
	else
	{
		//胜利列表
		UserWinList WinnerList;

		//临时数据
		BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT];
		CopyMemory(bTempData,m_cbOverCardData,GAME_PLAYER*MAX_CENTERCOUNT);

		//查找胜利者
		m_GameLogic.SelectMaxUser(bTempData,WinnerList,NULL);
		ASSERT(WinnerList.bSameCount>0);

		//设置扑克
		for (WORD i = 0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i] == TRUE) m_GameClientView.m_CardControl[i].SetCardData(m_cbHandCardData[i],2);
			else m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		}

		//特效变量
		bool wIsMyWin =false ;
		WORD wWinnerID = INVALID_CHAIR;		
		BYTE cbEffectHandCard[MAX_COUNT];
		BYTE cbEffectCenterCardData[MAX_CENTERCOUNT];
		ZeroMemory(cbEffectHandCard,sizeof(cbEffectHandCard));
		ZeroMemory(cbEffectCenterCardData,sizeof(cbEffectCenterCardData));
		BYTE bTempCount1,bTempCount2;

		//查找胜利扑克
		for (WORD i=0;i<WinnerList.bSameCount;i++)
		{
			wWinnerID=WinnerList.wWinerList[i];
			if(!wIsMyWin && m_wMeChairID==WinnerList.wWinerList[i])
			{
				wIsMyWin = true;
			}

			//查找扑克数据
			BYTE bTempCount1=m_GameLogic.GetSameCard(m_cbHandCardData[wWinnerID],bTempData[wWinnerID],MAX_COUNT,MAX_CENTERCOUNT,cbEffectHandCard);
			BYTE bTempCount2=m_GameLogic.GetSameCard(m_cbCenterCardData,bTempData[wWinnerID],MAX_CENTERCOUNT,MAX_CENTERCOUNT,cbEffectCenterCardData);
			ASSERT(bTempCount1+bTempCount2<=MAX_CENTERCOUNT);

			//设置扑克特效数据
			m_GameClientView.m_CardControl[wWinnerID].SetCardEffect(cbEffectHandCard,bTempCount1);
			m_GameClientView.m_CenterCardControl.SetCardEffect(cbEffectCenterCardData,bTempCount2);
		}

		//自己扑克
		if(!wIsMyWin)
		{
			wWinnerID = m_wMeChairID;

			//自己扑克数据
			ZeroMemory(cbEffectHandCard,sizeof(cbEffectHandCard));
			ZeroMemory(cbEffectCenterCardData,sizeof(cbEffectCenterCardData));

			//查找扑克数据
			bTempCount1=m_GameLogic.GetSameCard(m_cbHandCardData[wWinnerID],bTempData[wWinnerID],MAX_COUNT,MAX_CENTERCOUNT,cbEffectHandCard);
			bTempCount2=m_GameLogic.GetSameCard(m_cbCenterCardData,bTempData[wWinnerID],MAX_CENTERCOUNT,MAX_CENTERCOUNT,cbEffectCenterCardData);
			ASSERT(bTempCount1+bTempCount2<=MAX_CENTERCOUNT);

			//设置标志扑克数据
			m_GameClientView.m_CardControl[wWinnerID].SetMyCard(cbEffectHandCard,bTempCount1);
			m_GameClientView.m_CenterCardControl.SetMyCard(cbEffectCenterCardData,bTempCount2);
		}

		//游戏结束
		m_GameClientView.SetGameEndStart();
	}

	//赢金币
	for (WORD i =0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i] == FALSE) continue;
		if(m_dEndScore[i]>0L)
		{
			m_lCenterScore = m_lCenterScore -m_dEndScore[i]-m_lTotalScore[i];
			m_GameClientView.UpdateWindow();
			m_GameClientView.DrawMoveAnte(i,CGameClientView::AA_CENTER_TO_BASEFROM,m_dEndScore[i]+m_lTotalScore[i]);
			m_GameClientView.SetCenterScore(m_lCenterScore);
		}
		else if(m_dEndScore[i] == 0L)
		{
			m_GameClientView.DrawMoveAnte(i,CGameClientView::AA_CENTER_TO_BASEFROM,m_lTotalScore[i]);
			m_lCenterScore = m_lCenterScore-m_lTotalScore[i];
			m_GameClientView.SetCenterScore(m_lCenterScore);
			m_GameClientView.UpdateWindow();
		}
		m_GameClientView.SetTotalScore(i,0L);
		m_GameClientView.UpdateWindow();
	}

	//播放声音
	if (IsLookonMode()==false)
	{
		if (m_dEndScore[m_wMeChairID]>=0L) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	if(m_cbPlayStatus[m_wMeChairID]==TRUE && !IsLookonMode())
	{
		//调整位置
		CRect rcControl;
		m_GameClientView.m_ScoreView.GetWindowRect(&rcControl);
		CRect rcView ;
		m_GameClientView.GetWindowRect( &rcView );
		m_GameClientView.m_ScoreView.MoveWindow(rcView.left+5,rcView.bottom-15-rcControl.Height()*3/2,rcControl.Width(),rcControl.Height()/*nWidth/2-rcControl.Width()/2,nHeight/2+56,0,0,SWP_NOZORDER|SWP_NOSIZE*/);

		m_GameClientView.m_ScoreView.SetGameScore(m_wMeChairID,m_dEndScore[m_wMeChairID]);
		m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
		m_GameClientView.m_ScoreView.SetShowTimes();
	}
	if(!IsLookonMode())
	{
		//开牌按钮
		if(m_bOpenCard)
		{
			m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
		}

		if (m_GameClientView.m_ScoreView.IsWindowVisible()==TRUE)
		{
			m_GameClientView.m_ScoreView.SetStartTimes();
		}
		else if(m_bAutoStart==FALSE)
		{
			SetGameClock(m_wMeChairID,IDI_START_GAME,TIME_START_GAME);
		}
		else //自动开始
		{
			m_GameClientView.m_ScoreView.SetStartTimes();
			m_GameClientView.m_ScoreView.SetShowTimes();
			//OnStart(0,0);
		}
	}

	//状态设置
	KillGameClock(IDI_USER_ADD_SCORE);

	//开始按钮
	if (!IsLookonMode() && m_bAutoStart==FALSE)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btExit.ShowWindow(SW_SHOW);
	}

	//成绩显示在即时聊天对话框
	if(m_pIStringMessage!=NULL)
	{
		TCHAR szBuffer[256]=TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("本局结束,成绩统计:"));
		m_pIStringMessage->InsertSystemString(szBuffer);

		if(m_bReset)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;	
				IClientUserItem * pClientUserItem=GetTableUserItem(i);
				//成绩输出
				if (pClientUserItem!=NULL)
				{
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n%s：")SCORE_STRING,/*%s玩家\n得分:%ld*/
						pClientUserItem->GetNickName(),m_dEndScore[i]);
					m_pIStringMessage->InsertNormalString(szBuffer);
				}
				else
				{
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n用户已离开：")SCORE_STRING,-m_lTotalScore[i]);/*\n得分:%ld*/
					m_pIStringMessage->InsertNormalString(szBuffer);
				}
			}
		}
		else	//不足金额
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_lTotalScore[i]==0)continue;
				//成绩输出
				if (m_bUserName[i]!=NULL)
				{
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s：")SCORE_STRING,
						&m_bUserName[i],m_dEndScore[i]);
					m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,128,255),true);
				}
				else
				{
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("用户已离开：")SCORE_STRING,-m_lTotalScore[i]);
					m_pIStringMessage->InsertCustomString(szBuffer,RGB(0,128,255),true);
				}
			}
		}
	}

	//重值变量
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lCenterScore = 0L;

	//if(m_bDelayCloseGame==true)
	//	ProcFrameMessage();
	return 0;
}

//发牌结束
LRESULT CGameClientEngine::OnSendFinish(WPARAM wParam,LPARAM lParam)
{
	//控制界面
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}
	if(m_wCurrentUser<GAME_PLAYER)
	{
		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return 0;
}

//开牌信息
LRESULT CGameClientEngine::OnOpenCard(WPARAM wParam,LPARAM lParam)
{
	//隐藏控件
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//发送消息
	SendSocketData(SUB_C_OPEN_CARD,NULL,0);

	return 0;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	m_GameClientView.SetCenterCount();

	//设置状态
	SetGameStatus(GAME_STATUS_PLAY);

	//旁观玩家
	if(IsLookonMode())
	{
		//清理界面
		m_GameClientView.SetGameEndEnd();
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.m_btExit.ShowWindow(SW_HIDE);
		m_GameClientView.m_ScoreView.SetStartTimes(false);
		ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
		ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
		ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
		ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
		for (WORD i = 0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.SetUserTableScore(i,m_lTableScore[i]);
			m_GameClientView.SetTotalScore(i,0L);
			m_GameClientView.m_SmallCardControl[i].SetCardData(NULL,0);
			m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		}
		m_GameClientView.m_CenterCardControl.SetCardData(NULL,0);
		m_GameClientView.SetDFlag(INVALID_CHAIR);
		m_GameClientView.SetCenterScore(0L);
		m_lCenterScore = 0L;
		m_lCellScore = 0L;
	}

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置变量
	m_wDUser = pGameStart->wDUser;
	m_wCurrentUser = pGameStart->wCurrentUser;
	m_lAddLessScore = pGameStart->lAddLessScore;
	m_lTurnLessScore = pGameStart->lTurnLessScore;
	m_lTurnMaxScore = pGameStart->lTurnMaxScore;
	m_lCellScore = pGameStart->lCellScore;

	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if (pClientUserItem!=NULL) 
		{
			//游戏信息
			//m_GameClientView.SetUserIdInfo(i,pClientUserItem);
			m_cbPlayStatus[i]=TRUE;
		}
		else 
		{
			//m_GameClientView.SetUserIdInfo(i,NULL);
			m_cbPlayStatus[i]=FALSE;
		}
	}

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//加注信息
	m_lTableScore[pGameStart->wDUser] += m_lCellScore;
	m_lTableScore[pGameStart->wMaxChipInUser] = 2*m_lCellScore;
	m_lTotalScore[pGameStart->wDUser] =  m_lCellScore;
	m_lTotalScore[pGameStart->wMaxChipInUser] = 2*m_lCellScore;

	//设置界面
	m_GameClientView.SetDFlag(m_wDUser);
	m_GameClientView.DrawMoveAnte(pGameStart->wDUser,CGameClientView::AA_BASEFROM_TO_BASEDEST,m_lTotalScore[pGameStart->wDUser]);
	m_GameClientView.DrawMoveAnte(pGameStart->wMaxChipInUser,CGameClientView::AA_BASEFROM_TO_BASEDEST,m_lTotalScore[pGameStart->wMaxChipInUser]);	
	m_GameClientView.SetTotalScore(pGameStart->wDUser,m_lTotalScore[pGameStart->wDUser]);
	m_GameClientView.SetTotalScore(pGameStart->wMaxChipInUser,m_lTotalScore[pGameStart->wMaxChipInUser]);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//发送暗牌
	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(m_cbHandCardData));
	for (BYTE j = 0;j<2;j++)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (m_cbPlayStatus[i]==TRUE)
			{
				if(GetMeChairID()==i)
				{
					m_GameClientView.DrawMoveCard(i,TO_USERCARD,m_cbHandCardData[i][j]);
				}
				else 
				{
					m_GameClientView.DrawMoveCard(i,TO_USERCARD,0);
				}
			}
		}
	}

	////控件设置
	//if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	//{
	//	ActiveGameFrame();
	//	UpdateScoreControl();
	//}
	//SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return true;
}

//加注消息
bool CGameClientEngine::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;

	//加注处理
	if ((IsLookonMode()==true)||(pAddScore->wAddScoreUser!=wMeChairID))
	{
		//加注界面
		if(pAddScore->lAddScoreCount>0)
		{
			m_GameClientView.DrawMoveAnte(pAddScore->wAddScoreUser,CGameClientView::AA_BASEFROM_TO_BASEDEST,pAddScore->lAddScoreCount);
			//m_GameClientView.SetUserTableScore(pAddScore->wAddScoreUser,pAddScore->lAddScoreCount+m_lTableScore[pAddScore->wAddScoreUser]);
			m_lTotalScore[pAddScore->wAddScoreUser] += pAddScore->lAddScoreCount;
			m_GameClientView.SetTotalScore(pAddScore->wAddScoreUser,m_lTotalScore[pAddScore->wAddScoreUser]);
			m_lTableScore[pAddScore->wAddScoreUser] +=pAddScore->lAddScoreCount;
		}

		//播放声音
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			//播放声音
			if (pAddScore->lAddScoreCount==0L) 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
			else if (pAddScore->lAddScoreCount==m_lTurnMaxScore)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
			else if ((pAddScore->lAddScoreCount)==m_lTurnLessScore)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
			else 
				PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
	}

	//设置变量
	m_wCurrentUser=pAddScore->wCurrentUser;
	m_lTurnLessScore=pAddScore->lTurnLessScore;
	m_lTurnMaxScore = pAddScore->lTurnMaxScore;
	m_lAddLessScore = pAddScore->lAddLessScore;

	//控制界面
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	if (m_wCurrentUser==INVALID_CHAIR) 
	{
		KillGameClock(IDI_USER_ADD_SCORE);
		//一轮平衡
		//中心积分累计
		for (WORD i =0;i<GAME_PLAYER;i++)
		{			
			m_lCenterScore += m_lTableScore[i];
		}

		//筹码移动
		for (WORD i =0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i] == FALSE) continue;
			if(m_lTableScore[i]!=0L)
			{
				//m_GameClientView.SetUserTableScore(i,m_lTableScore[i]);
				m_GameClientView.DrawMoveAnte(i,CGameClientView::AA_BASEDEST_TO_CENTER,m_lTableScore[i]);
				m_GameClientView.SetUserTableScore(i,0L);
			}
		}

		//m_GameClientView.SetCenterScore(m_lCenterScore);

		ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
		m_GameClientView.RefreshGameView();
		m_GameClientView.UpdateWindow();
	}
	else if(m_wCurrentUser<GAME_PLAYER)SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return true;
}

//用户放弃
bool CGameClientEngine::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;

	//界面设置
	m_GameClientView.m_SmallCardControl[pGiveUp->wGiveUpUser].SetCardData(NULL,0);
	m_GameClientView.m_CardControl[pGiveUp->wGiveUpUser].SetCardData(NULL,0);
	m_GameClientView.UpdateWindow();
	m_GameClientView.DrawMoveCard(pGiveUp->wGiveUpUser,TO_GIVEUP_CARD,0);

	//状态设置
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID()))
		SetGameStatus(GAME_STATUS_FREE);

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;

	//环境设置
	if (wGiveUpUser==GetClockChairID())
		KillGameClock(IDI_USER_ADD_SCORE);

	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID()))
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//显示积分
	if (wGiveUpUser==GetMeChairID())
	{
		if(m_lTableScore[wGiveUpUser]!=0L)
		{
			m_GameClientView.DrawMoveAnte(wGiveUpUser,CGameClientView::AA_BASEDEST_TO_CENTER,m_lTableScore[wGiveUpUser]);
			m_GameClientView.SetUserTableScore(wGiveUpUser,0L);
			m_lTableScore[wGiveUpUser] = 0L;
		}
		if(!IsLookonMode())
		{
			//调整位置
			CRect rcControl;
			m_GameClientView.m_ScoreView.GetWindowRect(&rcControl);
			CRect rcView ;
			m_GameClientView.GetWindowRect( &rcView );
			m_GameClientView.m_ScoreView.MoveWindow(rcView.left+5,rcView.bottom-15-rcControl.Height()*3/2,rcControl.Width(),rcControl.Height());

			m_GameClientView.m_ScoreView.SetGameScore(wGiveUpUser,pGiveUp->lLost);
			m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
			m_GameClientView.m_ScoreView.SetShowTimes();
		}
	}
	else
	{
		if(m_lTableScore[wGiveUpUser]!=0L)
		{
			m_GameClientView.DrawMoveAnte(wGiveUpUser,CGameClientView::AA_BASEDEST_TO_CENTER,m_lTableScore[wGiveUpUser]);
			m_GameClientView.SetUserTableScore(wGiveUpUser,0L);
			m_lTableScore[wGiveUpUser] = 0L;
		}
	}

	return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//校验数据
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//当前玩家
	m_wCurrentUser = pSendCard->wCurrentUser;
	CopyMemory(m_cbCenterCardData,pSendCard->cbCenterCardData,sizeof(BYTE)*(pSendCard->cbSendCardCount));

	//发送共牌
	if((pSendCard->cbSendCardCount >= 3)&&(pSendCard->cbSendCardCount <= 5)&&(pSendCard->cbPublic==0))
	{
		//发送共牌 
		if((pSendCard->cbSendCardCount == 3))
		{
			for (BYTE j = 0;j<pSendCard->cbSendCardCount;j++)
			{
				m_GameClientView.DrawMoveCard(GAME_PLAYER,TO_CENTER_CARD,pSendCard->cbCenterCardData[j]);
			}
		}
		else if((pSendCard->cbSendCardCount >3))
		{
			BYTE bTemp = pSendCard->cbSendCardCount-1;
			m_GameClientView.DrawMoveCard(GAME_PLAYER,TO_CENTER_CARD,pSendCard->cbCenterCardData[bTemp]);
		}
	}

	if((pSendCard->cbSendCardCount == 5)&&(pSendCard->cbPublic >= 1))
	{
		BYTE bFirstCard = pSendCard->cbPublic ;
		if(bFirstCard==1)bFirstCard = 0;
		else if(bFirstCard==2)bFirstCard = 3;
		else if(bFirstCard==3)bFirstCard = 4;
		for (BYTE j = bFirstCard;j<pSendCard->cbSendCardCount;j++)
		{
			m_GameClientView.DrawMoveCard(GAME_PLAYER,TO_CENTER_CARD,pSendCard->cbCenterCardData[j]);
		}
	}

	//更新界面
	m_GameClientView.RefreshGameView();

	return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	HideScoreControl();

	//开牌标志
	m_bOpenCard = false;

	//桌面金币移至中间
	for (WORD i = 0;i<GAME_PLAYER;i++)
	{
		if((m_lTableScore[i]>0)&&(m_cbPlayStatus[i]==TRUE))
		{
			m_GameClientView.DrawMoveAnte(i,CGameClientView::AA_BASEDEST_TO_CENTER,m_lTableScore[i]);			
			m_GameClientView.SetUserTableScore(i,0L);
		}
	}

	//积分信息
	CopyMemory(m_dEndScore,pGameEnd->lGameScore,sizeof(m_dEndScore));
	CopyMemory(m_cbHandCardData,pGameEnd->cbCardData,sizeof(m_cbHandCardData));
	CopyMemory(m_cbOverCardData,pGameEnd->cbLastCenterCardData,sizeof(m_cbOverCardData));

	//保存信息
	if(pGameEnd->cbTotalEnd == 1)
	{
		m_bExitTag = true;
	}
	else 
	{
		if(m_dEndScore[GetMeChairID()]>0L)
		{
			//开牌标志
			m_bOpenCard = true;
		}
		m_bExitTag = false;
	}


	//盐时定时器
	if(m_GameClientView.IsMoveing())
	{
		m_GameClientView.SetOverTimer();
	}
	else OnGameOver(0,0);

	
	return true;
}

//开牌消息
bool CGameClientEngine::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OpenCard)) return false;
	CMD_S_OpenCard * pOpenCard=(CMD_S_OpenCard *)pBuffer;

	WORD wMeChiarID = GetMeChairID();
	IClientUserItem * pClientUserItem=GetTableUserItem(wMeChiarID);
	if(pClientUserItem!=NULL && pClientUserItem->GetUserStatus()!=US_READY)
	{
		m_GameClientView.m_CardControl[pOpenCard->wWinUser].SetCardData(m_cbHandCardData[pOpenCard->wWinUser],2);
		m_GameClientView.m_CardControl[pOpenCard->wWinUser].SetDisplayItem(true);
		m_GameClientView.RefreshGameView();
	}

	return true;
}

//隐藏控制
void CGameClientEngine::HideScoreControl()
{
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAdd.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);

	return;
}

//处理框架消息
void CGameClientEngine::ProcFrameMessage()
{
	
}

//更新控制
void CGameClientEngine::UpdateScoreControl()
{
	//显示按钮
	if(m_lTurnLessScore>0L)
	{
		m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btFollow.ShowWindow((m_lTurnLessScore==m_lTurnMaxScore)?SW_HIDE:SW_SHOW);
	}
	else
	{
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	}

	//要整理
	if(m_lAddLessScore>m_lTurnMaxScore)
	{
		m_GameClientView.m_btAdd.ShowWindow(SW_HIDE);
	}
	else
	{
		m_GameClientView.m_btAdd.ShowWindow(SW_SHOW);
	}

	m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	m_GameClientView.m_btShowHand.ShowWindow(SW_SHOW);

	return;
}

//////////////////////////////////////////////////////////////////////////
