#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "CardControl.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//动画参数
#define BOMB_CARTOON_COUNT			6									//爆炸数目
#define PLAN_CARTOON_COUNT			3									//飞机数目
#define CARD_WARNING_COUNT			5									//报警数目
#define BANKER_MOVIE_COUNT			4									//翻牌数目
#define REVERSAL_CARD_COUNT			4									//翻牌数目

//洗牌动画
#define RIFFLE_CARD_COUNT_X			4									//洗牌数目
#define RIFFLE_CARD_COUNT_Y			6									//洗牌数目
#define RIFFLE_CARD_COUNT_ALL		RIFFLE_CARD_COUNT_X*RIFFLE_CARD_COUNT_Y

//发牌动画
#define DISPATCH_CARD_TIMES			9									//发牌次数
#define DISPATCH_CARD_SPACE			2									//发牌间隔
#define DISPATCH_HEAP_DISTANCE		6									//牌堆间距

//////////////////////////////////////////////////////////////////////////////////

//提示按钮
#define IDC_BT_START				100									//开始按钮
#define IDC_BT_SCORE				101									//查分按钮

//叫分按钮
#define IDC_CALL_SCORE_1			200									//叫分按钮
#define IDC_CALL_SCORE_2			201									//叫分按钮
#define IDC_CALL_SCORE_3			202									//叫分按钮
#define IDC_CALL_SCORE_NONE			203									//叫分按钮

//游戏按钮
#define IDC_BT_OUT_CARD				300									//出牌按钮
#define IDC_BT_PASS_CARD			301									//PASS按钮
#define IDC_BT_OUT_PROMPT           302                             	//提示按钮

//功能按钮
#define IDC_BT_TRUSTEE				400									//托管控制
#define IDC_BT_CANCEL_TRUSTEE		401									//托管控制
#define IDC_BT_SORT_CARD			402									//扑克排序
#define IDC_BT_LAST_TURN			403									//上轮扑克

//提示按钮
#define IDC_CARD_PROMPT_1			500									//提示按钮
#define IDC_CARD_PROMPT_2			501									//提示按钮
#define IDC_CARD_PROMPT_3			502									//提示按钮
#define IDC_CARD_PROMPT_4			503									//提示按钮
#define IDC_CARD_PROMPT_5			504									//提示按钮
#define IDC_CARD_PROMPT_6			505									//提示按钮

//成绩窗口
#define IDC_GAME_SCORE				600									//成绩窗口

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewD3D)

	//系统消息
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//控制按钮
	ON_BN_CLICKED(IDC_BT_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_BT_SCORE, OnBnClickedScore)
	ON_BN_CLICKED(IDC_BT_SORT_CARD, OnBnClickedSortCard)
	ON_BN_CLICKED(IDC_BT_LAST_TURN, OnBnClickedLastTurn)
	ON_BN_CLICKED(IDC_BT_TRUSTEE, OnBnClickedTrustee)
	ON_BN_CLICKED(IDC_BT_CANCEL_TRUSTEE, OnBnClickedCancelTrustee)

	//游戏按钮
	ON_BN_CLICKED(IDC_BT_OUT_CARD, OnBnClickedOutCard)
	ON_BN_CLICKED(IDC_BT_PASS_CARD, OnBnClickedPassCard)
	ON_BN_CLICKED(IDC_BT_OUT_PROMPT, OnBnClickedOutPrompt)

	//叫分按钮
	ON_BN_CLICKED(IDC_CALL_SCORE_1, OnBnClickedCallScore1)
	ON_BN_CLICKED(IDC_CALL_SCORE_2, OnBnClickedCallScore2)
	ON_BN_CLICKED(IDC_CALL_SCORE_3, OnBnClickedCallScore3)
	ON_BN_CLICKED(IDC_CALL_SCORE_NONE, OnBnClickedCallScoreNone)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//游戏变量
	m_cbBankerScore=0;
	m_wBankerUser=INVALID_CHAIR;

	//爆炸动画
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//飞机动画
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//火箭动画
	m_bCartoonRocket=false;
	m_wCartoonRocketIndex=0l;

	//庄家动画
	m_wBankerMovieIndex=0L;

	//翻牌动画
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//发牌动画
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//游戏变量
	m_lCellScore=0L;
	m_cbBombCount=0L;
	ZeroMemory(m_cbCallScore,sizeof(m_cbCallScore));

	//发牌数据
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//位置变量
	ZeroMemory(&m_ptPass,sizeof(m_ptPass));
	ZeroMemory(&m_ptBanker,sizeof(m_ptBanker));
	ZeroMemory(&m_ptCountWarn,sizeof(m_ptCountWarn));
	ZeroMemory(&m_ptCallScore,sizeof(m_ptCallScore));
	ZeroMemory(&m_ptOrderFlag,sizeof(m_ptOrderFlag));

	//设置变量
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//报警动画
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//游戏变量
	m_cbBankerScore=0;
	m_wBankerUser=INVALID_CHAIR;

	//爆炸动画
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//飞机动画
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//火箭动画
	m_bCartoonRocket=false;
	m_wCartoonRocketIndex=0l;

	//庄家动画
	m_wBankerMovieIndex=0L;

	//翻牌动画
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//发牌动画
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//游戏变量
	m_lCellScore=0L;
	m_cbBombCount=0L;
	ZeroMemory(m_cbCallScore,sizeof(m_cbCallScore));

	//发牌数据
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//设置变量
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//报警动画
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	//设置界面
	m_btScore.ShowWindow(false);
	m_btStart.ShowWindow(false);
	m_btOutCard.ShowWindow(false);
	m_btPassCard.ShowWindow(false);
	m_btOutPrompt.ShowWindow(false);

	//游戏控件
	m_btTrustee.ShowWindow(true);
	m_btCancelTrustee.ShowWindow(false);
	m_btTrustee.EnableWindow(false);
	m_btLastTurn.EnableWindow(false);
	m_btSortCard.EnableWindow(false);

	//叫分按钮
	m_btCallScore1.ShowWindow(false);
	m_btCallScore2.ShowWindow(false);
	m_btCallScore3.ShowWindow(false);
	m_btCallScoreNone.ShowWindow(false);

	//提示按钮
	m_btCardPrompt1.EnableWindow(false);
	m_btCardPrompt2.EnableWindow(false);
	m_btCardPrompt3.EnableWindow(false);
	m_btCardPrompt4.EnableWindow(false);
	m_btCardPrompt5.EnableWindow(false);
	m_btCardPrompt6.EnableWindow(false);

	//游戏控件
	m_ScoreControl.CloseControl();
	m_BackCardControl.SetCardData(NULL,0);
	m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(NULL,0);
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);
	}

	//销毁资源
	if (m_TextureUserPass.IsNull()==false) m_TextureUserPass.Destory();
	if (m_TextureLastTurn.IsNull()==false) m_TextureLastTurn.Destory();
	if (m_TextureCallScore.IsNull()==false) m_TextureCallScore.Destory();
	if (m_TextureCountWarn.IsNull()==false) m_TextureCountWarn.Destory();
	if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
	if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();
	if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();
	if (m_TextureReversalCard.IsNull()==false) m_TextureReversalCard.Destory();
	if (m_TextureWaitCallScore.IsNull()==false) m_TextureWaitCallScore.Destory();

	//按钮图片
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(&m_D3DDevice,TEXT("BT_SORT_CARD_ORDER"),TEXT("PNG"),hResInstance);

	return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//头像位置
	m_ptAvatar[0].SetPoint(10,nHeight/2-155);
	m_ptAvatar[1].SetPoint(nWidth/2-332,nHeight-54);
	m_ptAvatar[2].SetPoint(nWidth-79,nHeight/2-155);

	//帐号位置
	m_ptNickName[0].SetPoint(10,m_ptAvatar[0].y+53);
	m_ptNickName[1].SetPoint(m_ptAvatar[1].x+54,m_ptAvatar[1].y+1);
	m_ptNickName[2].SetPoint(nWidth-78,m_ptAvatar[2].y+53);

	//时间位置
	m_ptClock[0].SetPoint(nWidth*21/80,nHeight/2-100);
	m_ptClock[1].SetPoint(nWidth/2,nHeight-335);
	m_ptClock[2].SetPoint(nWidth*59/80,nHeight/2-100);

	//准备位置
	m_ptReady[0].SetPoint(nWidth*24/80,nHeight/2-90);
	m_ptReady[1].SetPoint(nWidth/2,nHeight-250);
	m_ptReady[2].SetPoint(nWidth*56/80,nHeight/2-90);

	//放弃位置
	m_ptPass[0].SetPoint(nWidth*24/80,nHeight/2-90);
	m_ptPass[1].SetPoint(nWidth/2,nHeight-315);
	m_ptPass[2].SetPoint(nWidth*56/80,nHeight/2-90);

	//叫分位置
	m_ptCallScore[0].SetPoint(nWidth*24/80,nHeight/2-90);
	m_ptCallScore[1].SetPoint(nWidth/2,nHeight-315);
	m_ptCallScore[2].SetPoint(nWidth*56/80,nHeight/2-90);

	//庄家标志
	m_ptBanker[0].SetPoint(8,m_ptAvatar[0].y-75);
	m_ptBanker[1].SetPoint(nWidth/2+275,nHeight-105);
	m_ptBanker[2].SetPoint(nWidth-85,m_ptAvatar[2].y-75);

	//报警位置
	m_ptCountWarn[0].SetPoint(25,m_ptAvatar[0].y+150);
	m_ptCountWarn[1].SetPoint(nWidth/2-322,nHeight-118);
	m_ptCountWarn[2].SetPoint(nWidth-60,m_ptAvatar[2].y+150);

	//等级位置
	m_ptOrderFlag[0].SetPoint(61,nHeight/2-160);
	m_ptOrderFlag[1].SetPoint(nWidth/2-359,nHeight-58);
	m_ptOrderFlag[2].SetPoint(nWidth-27,nHeight/2-160);

	//出牌位置
	m_UserCardControl[0].SetDispatchPos(115,nHeight/2-95);
	m_UserCardControl[1].SetDispatchPos(nWidth/2,nHeight-100);
	m_UserCardControl[2].SetDispatchPos(nWidth-115,nHeight/2-95);

	//底牌扑克
	m_BackCardControl.SetBenchmarkPos(nWidth/2,14,enXCenter,enYTop);

	//用户扑克
	m_HandCardControl[0].SetBenchmarkPos(95,nHeight/2-95,enXLeft,enYCenter);
	m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-70,enXCenter,enYBottom);
	m_HandCardControl[2].SetBenchmarkPos(nWidth-95,nHeight/2-95,enXRight,enYCenter);

	//出牌扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth*17/80,nHeight/2-95,enXLeft,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-320,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth*63/80,nHeight/2-95,enXRight,enYCenter);

	//虚拟按钮
	if (m_bInitD3D==true)
	{
		//查分按钮
		CRect rcScore;
		m_btScore.GetClientRect(rcScore);
		m_btScore.SetWindowPos(nWidth-rcScore.Width()-5,8,0,0,SWP_NOSIZE);

		//开始按钮
		CRect rcStart;
		m_btStart.GetClientRect(rcStart);
		m_btStart.SetWindowPos((nWidth-rcStart.Width())/2,nHeight-rcStart.Height()-195,0,0,SWP_NOSIZE);

		//游戏按钮
		CRect rcGameButton;
		m_btOutCard.GetClientRect(rcGameButton);
		m_btPassCard.SetWindowPos(nWidth/2-rcGameButton.Width()*3/2-20,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);
		m_btOutPrompt.SetWindowPos(nWidth/2-rcGameButton.Width()/2,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);
		m_btOutCard.SetWindowPos(nWidth/2+rcGameButton.Width()/2+20,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);

		//叫分按钮
		CRect rcCallButton;
		m_btCallScore1.GetClientRect(rcCallButton);
		m_btCallScore1.SetWindowPos(nWidth/2-rcCallButton.Width()*2-30,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScore2.SetWindowPos(nWidth/2-rcCallButton.Width()-10,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScore3.SetWindowPos(nWidth/2+10,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScoreNone.SetWindowPos(nWidth/2+rcCallButton.Width()+30,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);

		//功能按钮
		CRect rcTrustee;
		m_btTrustee.GetClientRect(rcTrustee);
		m_btSortCard.SetWindowPos(nWidth/2+113+rcTrustee.Width()*1,nHeight-rcTrustee.Height()-4,0,0,SWP_NOSIZE);
		m_btLastTurn.SetWindowPos(nWidth/2+113+rcTrustee.Width()*2,nHeight-rcTrustee.Height()-4,0,0,SWP_NOSIZE);
		m_btTrustee.SetWindowPos(nWidth/2+113+rcTrustee.Width()*3,nHeight-rcTrustee.Height()-4,0,0,SWP_NOSIZE);
		m_btCancelTrustee.SetWindowPos(nWidth/2+113+rcTrustee.Width()*3,nHeight-rcTrustee.Height()-4,0,0,SWP_NOSIZE);

		//提示按钮
		CRect rcPromptButton;
		m_btCardPrompt1.GetClientRect(rcPromptButton);
		m_btCardPrompt1.SetWindowPos(nWidth/2-200,nHeight-rcPromptButton.Height()-5,0,0,SWP_NOSIZE);
		m_btCardPrompt2.SetWindowPos(nWidth/2-200+rcPromptButton.Width()*1,nHeight-rcPromptButton.Height()-4,0,0,SWP_NOSIZE);
		m_btCardPrompt3.SetWindowPos(nWidth/2-200+rcPromptButton.Width()*2,nHeight-rcPromptButton.Height()-4,0,0,SWP_NOSIZE);
		m_btCardPrompt4.SetWindowPos(nWidth/2-200+rcPromptButton.Width()*3,nHeight-rcPromptButton.Height()-4,0,0,SWP_NOSIZE);
		m_btCardPrompt5.SetWindowPos(nWidth/2-200+rcPromptButton.Width()*4,nHeight-rcPromptButton.Height()-4,0,0,SWP_NOSIZE);
		m_btCardPrompt6.SetWindowPos(nWidth/2-200+rcPromptButton.Width()*5,nHeight-rcPromptButton.Height()-4,0,0,SWP_NOSIZE);

		//成绩窗口
		if (m_ScoreControl.IsWindowActive()==true)
		{
			CRect rcScoreControl;
			m_ScoreControl.GetWindowRect(rcScoreControl);
			m_ScoreControl.SetWindowPos((nWidth-rcScoreControl.Width())/2,(nHeight-rcScoreControl.Height())*2/5,0,0,SWP_NOSIZE);
		}
	}

	return;
}

//动画驱动
VOID CGameClientView::CartoonMovie()
{
	//扑克动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].CartoonMovie();
		m_UserCardControl[i].CartoonMovie();
	}

	//飞机动画
	if ((m_bCartoonPlan==true)&&(m_CartoonPlanLapseCount.GetLapseCount(10)>0L))
	{
		//设置变量
		m_wCartoonPlanIndex++;

		//设置位置	
		m_ptCartoonPlan.x -= 30;

		//停止判断
		if (m_ptCartoonPlan.x < -200 )
		{
			//设置变量
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			m_ptCartoonPlan.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();
		}
	}

	//火箭动画
	if ((m_bCartoonRocket==true)&&(m_CartoonRocketLapseCount.GetLapseCount(10)>0L))
	{
		//设置变量
		m_wCartoonRocketIndex++;

		//设置位置	
		m_ptCartoonRocket.y -= 30;

		//停止判断
		if (m_ptCartoonRocket.y < -200 )
		{
			//设置变量
			m_bCartoonRocket=false;
			m_wCartoonRocketIndex=0L;

			m_ptCartoonRocket.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonRocket.IsNull()==false);
			if (m_TextureCartoonRocket.IsNull()==false) m_TextureCartoonRocket.Destory();
		}
	}

	//爆炸动画
	if ((m_bCartoonBomb==true)&&(m_CartoonBombLapseCount.GetLapseCount(180)>0L))
	{
		//设置变量
		m_wCartoonBombIndex++;

		//停止判断
		if (m_wCartoonBombIndex>=BOMB_CARTOON_COUNT)
		{
			//设置变量
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
		}
	}

	//翻牌动画
	if ((m_bReversalCard==true)&&(m_ReversalCardLapseCount.GetLapseCount(180)>0L))
	{
		//设置变量
		m_wReversalCardIndex++;

		//停止判断
		if (m_wReversalCardIndex>=REVERSAL_CARD_COUNT)
		{
			//设置变量
			m_bReversalCard=false;
			m_wReversalCardIndex=0L;

			//设置扑克
			m_BackCardControl.SetShowCount(MAX_CARD_COUNT);

			//卸载资源
			ASSERT(m_TextureReversalCard.IsNull()==false);
			if (m_TextureReversalCard.IsNull()==false) m_TextureReversalCard.Destory();

			//发送消息
			SendEngineMessage(IDM_REVERSAL_FINISH,0L,0L);
		}
	}

	//庄家动画
	if ((m_wBankerUser!=INVALID_TABLE)&&(m_BankerMovieLapseCount.GetLapseCount(500)>0L))
	{
		//设置变量
		m_wBankerMovieIndex=(m_wBankerMovieIndex+1)%BANKER_MOVIE_COUNT;
	}

	//洗牌动画
	if ((m_bRiffleCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(100)>0L))
	{
		//设置变量
		m_wDispatchCardIndex++;

		//停止判断
		if (m_wDispatchCardIndex>=(RIFFLE_CARD_COUNT_ALL+6))
		{
			//设置变量
			m_bRiffleCard=false;
			m_bDispatchCard=true;
			m_wDispatchCardIndex=0L;

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}
	}

	//发牌动画
	if ((m_bDispatchCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(10)>0L))
	{
		//设置变量
		m_wDispatchCardIndex++;

		//变量定义
		WORD wBaseIndex=DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES;

		//设置扑克
		if ((m_wDispatchCardIndex>=wBaseIndex)&&((m_wDispatchCardIndex-wBaseIndex)%DISPATCH_CARD_SPACE==0))
		{
			//计算索引
			WORD wCardIndex=m_wDispatchCardIndex-wBaseIndex;
			WORD wUserIndex=(m_wStartUser+wCardIndex/DISPATCH_CARD_SPACE)%GAME_PLAYER;

			//设置扑克
			if (wUserIndex==MYSELF_VIEW_ID)
			{
				//获取扑克
				BYTE cbCardData[MAX_COUNT];
				BYTE cbCardCount=m_HandCardControl[wUserIndex].GetCardData(cbCardData,CountArray(cbCardData));

				//插入扑克
				cbCardData[cbCardCount]=m_cbHandCardData[cbCardCount++];
				m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

				//设置扑克
				m_HandCardControl[wUserIndex].SetCardData(cbCardData,cbCardCount);
			}
			else
			{
				//玩家扑克
				m_HandCardControl[wUserIndex].SetCardData(m_HandCardControl[wUserIndex].GetCardCount()+1);
			}
		}

		//停止判断
		if (m_wDispatchCardIndex>=DISPATCH_COUNT*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES)
		{
			//设置变量
			m_bDispatchCard=false;
			m_wDispatchCardIndex=0L;

			//卸载资源
			ASSERT(m_TextureDispatchCard.IsNull()==false);
			if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

			//发送消息
			SendEngineMessage(IDM_DISPATCH_FINISH,0L,0L);
		}
	}

	//报警动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_bCountWarn[i]==true)&&(m_CountWarnLapseCount[i].GetLapseCount(180)>0L))
		{
			//设置变量
			m_wCountWarnIndex[i]=(m_wCountWarnIndex[i]+1)%CARD_WARNING_COUNT;
		}
	}

	return;
}

//配置设备
VOID CGameClientView::InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//游戏按钮
	m_btStart.ActiveWindow(rcCreate,0,IDC_BT_START,&m_VirtualEngine,NULL);
	m_btScore.ActiveWindow(rcCreate,0,IDC_BT_SCORE,&m_VirtualEngine,NULL);
	m_btOutCard.ActiveWindow(rcCreate,0,IDC_BT_OUT_CARD,&m_VirtualEngine,NULL);
	m_btPassCard.ActiveWindow(rcCreate,0,IDC_BT_PASS_CARD,&m_VirtualEngine,NULL);
	m_btOutPrompt.ActiveWindow(rcCreate,0,IDC_BT_OUT_PROMPT,&m_VirtualEngine,NULL);

	//叫分按钮
	m_btCallScore1.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_1,&m_VirtualEngine,NULL);
	m_btCallScore2.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_2,&m_VirtualEngine,NULL);
	m_btCallScore3.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_3,&m_VirtualEngine,NULL);
	m_btCallScoreNone.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_NONE,&m_VirtualEngine,NULL);

	//功能按钮
	m_btCancelTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_CANCEL_TRUSTEE,&m_VirtualEngine,NULL);
	m_btTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_TRUSTEE,&m_VirtualEngine,NULL);
	m_btLastTurn.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_LAST_TURN,&m_VirtualEngine,NULL);
	m_btSortCard.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_SORT_CARD,&m_VirtualEngine,NULL);
	m_btCancelTrustee.ShowWindow(false);

	//提示按钮
	m_btCardPrompt1.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_1,&m_VirtualEngine,NULL);
	m_btCardPrompt2.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_2,&m_VirtualEngine,NULL);
	m_btCardPrompt3.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_3,&m_VirtualEngine,NULL);
	m_btCardPrompt4.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_4,&m_VirtualEngine,NULL);
	m_btCardPrompt5.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_5,&m_VirtualEngine,NULL);
	m_btCardPrompt6.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_CARD_PROMPT_6,&m_VirtualEngine,NULL);

	//游戏按钮
	m_btStart.SetButtonImage(pD3DDevice,TEXT("BT_START"),TEXT("PNG"),hResInstance);
	m_btScore.SetButtonImage(pD3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btOutCard.SetButtonImage(pD3DDevice,TEXT("BT_OUT_CARD"),TEXT("PNG"),hResInstance);
	m_btPassCard.SetButtonImage(pD3DDevice,TEXT("BT_PASS_CARD"),TEXT("PNG"),hResInstance);
	m_btOutPrompt.SetButtonImage(pD3DDevice,TEXT("BT_OUT_PROMPT"),TEXT("PNG"),hResInstance);

	//功能按钮
	m_btLastTurn.SetButtonImage(pD3DDevice,TEXT("BT_LAST_TURN"),TEXT("PNG"),hResInstance);
	m_btTrustee.SetButtonImage(pD3DDevice,TEXT("BT_START_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btCancelTrustee.SetButtonImage(pD3DDevice,TEXT("BT_STOP_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(pD3DDevice,TEXT("BT_SORT_CARD_COUNT"),TEXT("PNG"),hResInstance);

	//叫分按钮
	m_btCallScore1.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_1"),TEXT("PNG"),hResInstance);
	m_btCallScore2.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_2"),TEXT("PNG"),hResInstance);
	m_btCallScore3.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_3"),TEXT("PNG"),hResInstance);
	m_btCallScoreNone.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_NONE"),TEXT("PNG"),hResInstance);

	//提示按钮
	m_btCardPrompt1.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_1"),TEXT("PNG"),hResInstance);
	m_btCardPrompt2.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_2"),TEXT("PNG"),hResInstance);
	m_btCardPrompt3.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_3"),TEXT("PNG"),hResInstance);
	m_btCardPrompt4.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_4"),TEXT("PNG"),hResInstance);
	m_btCardPrompt5.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_5"),TEXT("PNG"),hResInstance);
	m_btCardPrompt6.SetButtonImage(pD3DDevice,TEXT("BT_CARD_PROMPT_6"),TEXT("PNG"),hResInstance);

	//扑克资源
	CCardControl::LoadCardResource(pD3DDevice);

	//背景资源
	m_TextureBack.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_BACK"),TEXT("PNG"));
	m_TextureFill.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_FILL"),TEXT("PNG"));
	m_TextureUserPlayer.LoadImage(pD3DDevice,hResInstance,TEXT("USER_FRAME_PLAYER"),TEXT("PNG"));
	m_TextureUserMySelf.LoadImage(pD3DDevice,hResInstance,TEXT("USER_FRAME_MYSELF"),TEXT("PNG"));
	m_TextureMemberOrder.LoadImage(pD3DDevice,hResInstance,TEXT("MEMBER_FLAG"),TEXT("PNG"));

	//加载纹理
	m_TextureCellNumber.LoadImage(pD3DDevice,hResInstance,TEXT("CELL_NUMBER"),TEXT("PNG"));
	m_TextureBankerFlag.LoadImage(pD3DDevice,hResInstance,TEXT("BANKER_FLAG"),TEXT("PNG"));
	m_TextureCellScoreBack.LoadImage(pD3DDevice,hResInstance,TEXT("CELL_SCORE_BACK"),TEXT("PNG"));
	m_TextureBankerCardBack.LoadImage(pD3DDevice,hResInstance,TEXT("BANKER_CARD_BACK"),TEXT("PNG"));
	m_TextureRemnantCardNumber.LoadImage(pD3DDevice,hResInstance,TEXT("REMNANT_CARD_NUMBER"),TEXT("PNG"));

	m_TextureFenWei.LoadImage(pD3DDevice,hResInstance,TEXT("FEN_WEI"),TEXT("PNG"));
	m_TextureFenTou.LoadImage(pD3DDevice,hResInstance,TEXT("FEN_TOU"),TEXT("PNG"));

	//底牌扑克
	m_BackCardControl.SetSmallMode(true);
	m_BackCardControl.SetDisplayItem(true);
	m_BackCardControl.SetBackGround(CARD_LAND);
	m_BackCardControl.SetCardDistance(70L,0L,0L);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//玩家扑克
		if ((i==0)||(i==2))
		{
			m_HandCardControl[i].SetSmallMode(true);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE_SMALL,15,0);
		}

		//用户扑克
		if (i==MYSELF_VIEW_ID)
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetSmallMode(false);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
		}

		//出牌扑克
		m_UserCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetCardDistance(DEF_X_DISTANCE,0,0);
	}

	//启动渲染
	StartRenderThread();

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//获取大小
	CSize SizeBack;
	CSize SizeUserUserMySelf;
	SizeBack.SetSize(m_TextureBack.GetWidth(),m_TextureBack.GetHeight());
	SizeUserUserMySelf.SetSize(m_TextureUserMySelf.GetWidth(),m_TextureUserMySelf.GetHeight());

	//绘画背景
	DrawViewImage(pD3DDevice,m_TextureFill,DRAW_MODE_SPREAD);
	m_TextureBack.DrawImage(pD3DDevice,(nWidth-SizeBack.cx)/2,(nHeight-SizeBack.cy)*2/5,SizeBack.cx,SizeBack.cy,0,0);

	//底牌背景
	m_TextureBankerCardBack.DrawImage(pD3DDevice,(nWidth-m_TextureBankerCardBack.GetWidth())/2,5);

	//叫分
	TCHAR szOutNumT[128] = {0};
	m_TextureCellScoreBack.DrawImage(pD3DDevice,(nWidth)/2,111);
	if ( m_cbBankerScore != 0 && m_cbBankerScore != 255 )
	{
		_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%d"),m_cbBankerScore);
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 + 53 ,113, DT_CENTER);
	}

	//绘画单位积分
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%d"),m_lCellScore);
	int nOutNumber = lstrlen(szOutNumT);
	if ( nOutNumber > 3 )
	{
		m_TextureFenWei.DrawImage(pD3DDevice,(nWidth)/2 - 20,114);
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 - 25 ,113, DT_RIGHT);
		m_TextureFenTou.DrawImage(pD3DDevice,(nWidth)/2 - 60 - nOutNumber * (m_TextureCellNumber.GetWidth()/10),113);
	}
	else
	{
		m_TextureFenWei.DrawImage(pD3DDevice,(nWidth)/2 - 15,114);
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 - 37 ,113, DT_CENTER);
		m_TextureFenTou.DrawImage(pD3DDevice,(nWidth)/2 - 55 - 3 * (m_TextureCellNumber.GetWidth()/10),113);
	}

	//玩家框架
	m_TextureUserPlayer.DrawImage(pD3DDevice,m_ptAvatar[0].x-9,m_ptAvatar[0].y-10);
	m_TextureUserPlayer.DrawImage(pD3DDevice,m_ptAvatar[2].x-9,m_ptAvatar[2].y-10);
	m_TextureUserMySelf.DrawImage(pD3DDevice,(nWidth-SizeUserUserMySelf.cx)/2,nHeight-SizeUserUserMySelf.cy);

	//底牌扑克
	m_BackCardControl.DrawCardControl(pD3DDevice);

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//用户属性
		DrawUserClock(pD3DDevice,m_ptClock[i].x,m_ptClock[i].y,GetUserClock(i));
		DrawUserAvatar(pD3DDevice,m_ptAvatar[i].x,m_ptAvatar[i].y,pIClientUserItem);

		//位置定义
		CRect rcNickName(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+72,m_ptNickName[i].y+12);
		CRect rcUserScore(m_ptNickName[i].x,m_ptNickName[i].y+17,m_ptNickName[i].x+72,m_ptNickName[i].y+29);
		CRect rcUserLevel(m_ptNickName[i].x,m_ptNickName[i].y+34,m_ptNickName[i].x+72,m_ptNickName[i].y+46);

		//绘画用户
		LPCTSTR pszNickName=pIClientUserItem->GetNickName();
		DrawTextString(pD3DDevice,pszNickName,rcNickName,DT_LEFT|DT_END_ELLIPSIS,D3DCOLOR_XRGB(153,255,0),D3DCOLOR_XRGB(0,50,0));

		//用户积分
		TCHAR szUserScore[64]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),SCORE_STRING,pIClientUserItem->GetUserScore());
		DrawTextString(pD3DDevice,szUserScore,rcUserScore,DT_LEFT|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));

		//游戏等级
		LPCTSTR pszUserLevel=GetLevelDescribe(pIClientUserItem);
		DrawTextString(pD3DDevice,pszUserLevel,rcUserLevel,DT_LEFT|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,50,0));

		//庄家标志
		if (m_wBankerUser!=INVALID_CHAIR)
		{
			//获取大小
			CSize SizeBankerFlag;
			SizeBankerFlag.SetSize(m_TextureBankerFlag.GetWidth()/(BANKER_MOVIE_COUNT*2),m_TextureBankerFlag.GetHeight());

			//绘画标志
			if ((GetUserClock(i)>0))
			{
				m_TextureBankerFlag.DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y,SizeBankerFlag.cx,SizeBankerFlag.cy,
					SizeBankerFlag.cx*((i==m_wBankerUser)?m_wBankerMovieIndex:BANKER_MOVIE_COUNT+m_wBankerMovieIndex),0);
			}
			else
			{
				m_TextureBankerFlag.DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y,SizeBankerFlag.cx,SizeBankerFlag.cy,
					SizeBankerFlag.cx*((i==m_wBankerUser)?0:BANKER_MOVIE_COUNT),0);
			}
		}

		//会员等级
		if (pIClientUserItem->GetMemberOrder()!=0)
		{
			//变量定义
			BYTE cbMemberOrder=pIClientUserItem->GetMemberOrder();

			//获取大小
			CSize SizeMember;
			SizeMember.SetSize(m_TextureMemberOrder.GetWidth()/5,m_TextureMemberOrder.GetHeight());

			//绘画标志
			m_TextureMemberOrder.DrawImage(pD3DDevice,m_ptOrderFlag[i].x,m_ptOrderFlag[i].y,SizeMember.cx,SizeMember.cy,SizeMember.cx*(cbMemberOrder - 1),0);
			
		}
	}

	//绘画扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].DrawCardControl(pD3DDevice);
		m_UserCardControl[i].DrawCardControl(pD3DDevice);
	}


	//剩余扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (i!=MYSELF_VIEW_ID)
		{
			//变量定义
			WORD wCardCount=m_HandCardControl[i].GetCardCount();

			//绘画数据
			if (wCardCount>0)
			{
				//获取位置
				CSize ControlSize;
				CPoint OriginPoint;
				m_HandCardControl[i].GetControlSize(ControlSize);
				m_HandCardControl[i].GetOriginPoint(OriginPoint);

				//绘画文字
				INT nXPos=OriginPoint.x+ControlSize.cx;
				INT nYPos=OriginPoint.y+ControlSize.cy;
				DrawNumberString(pD3DDevice,m_TextureRemnantCardNumber,wCardCount,nXPos-26,nYPos+18);
				DrawTextString(pD3DDevice,TEXT("剩余      张"),nXPos-67,nYPos+12,DT_LEFT,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(0,30,0));
			}
		}
	}

	//用户标志
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//同意标志
		BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
		if (cbUserStatus==US_READY) DrawUserReady(pD3DDevice,m_ptReady[i].x,m_ptReady[i].y);
	}

	//放弃状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			CSize SizeUserPass;
			SizeUserPass.SetSize(m_TextureUserPass.GetWidth(),m_TextureUserPass.GetHeight());
			m_TextureUserPass.DrawImage(pD3DDevice,m_ptPass[i].x-SizeUserPass.cx/2,m_ptPass[i].y-SizeUserPass.cy/2);
		}
	}

	//用户叫分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbCallScore[i]!=0)
		{
			//获取大小
			CSize SizeCallScore;
			SizeCallScore.SetSize(m_TextureCallScore.GetWidth()/4,m_TextureCallScore.GetHeight());

			//绘画标志
			INT nImageIndex=(m_cbCallScore[i]==0xFF)?3:(m_cbCallScore[i]-1);
			m_TextureCallScore.DrawImage(pD3DDevice,m_ptCallScore[i].x-SizeCallScore.cx/2,m_ptCallScore[i].y-SizeCallScore.cy/2,
				SizeCallScore.cx,SizeCallScore.cy,nImageIndex*SizeCallScore.cx,0);
		}
	}

	//上轮标志
	if (m_bLastTurnState==true)
	{
		CSize SizeLastTurn;
		SizeLastTurn.SetSize(m_TextureLastTurn.GetWidth(),m_TextureLastTurn.GetHeight());
		m_TextureLastTurn.DrawImage(pD3DDevice,(nWidth-SizeLastTurn.cx)/2,(nHeight-SizeLastTurn.cy)/2-100);
	}

	//等待叫分
	if (m_bWaitCallScore==true)
	{
		CSize SizeWaitCallScore;
		SizeWaitCallScore.SetSize(m_TextureWaitCallScore.GetWidth(),m_TextureWaitCallScore.GetHeight());
		m_TextureWaitCallScore.DrawImage(pD3DDevice,(nWidth-SizeWaitCallScore.cx)/2,(nHeight-SizeWaitCallScore.cy)/2-135);
	}

	//飞机动画
	if (m_bCartoonPlan==true)
	{
		//获取大小
		CSize SizeCartoonPlan;
		SizeCartoonPlan.SetSize(m_TextureCartoonPlan.GetWidth()/PLAN_CARTOON_COUNT,m_TextureCartoonPlan.GetHeight());

		//绘画效果
		m_TextureCartoonPlan.DrawImage(pD3DDevice,m_ptCartoonPlan.x,m_ptCartoonPlan.y - SizeCartoonPlan.cy/2,SizeCartoonPlan.cx,SizeCartoonPlan.cy,
			SizeCartoonPlan.cx*(m_wCartoonPlanIndex%PLAN_CARTOON_COUNT),0);
	}

	//火箭动画
	if (m_bCartoonRocket==true)
	{
		//获取大小
		CSize SizeCartoonRocket;
		SizeCartoonRocket.SetSize(m_TextureCartoonRocket.GetWidth()/3,m_TextureCartoonRocket.GetHeight());

		//绘画效果
		m_TextureCartoonRocket.DrawImage(pD3DDevice,m_ptCartoonRocket.x - SizeCartoonRocket.cx/2,m_ptCartoonRocket.y,SizeCartoonRocket.cx,SizeCartoonRocket.cy,
			SizeCartoonRocket.cx*(m_wCartoonRocketIndex%3),0);
	}

	//爆炸动画
	if (m_bCartoonBomb==true)
	{
		//获取大小
		CSize SizeCartoonBomb;
		SizeCartoonBomb.SetSize(m_TextureCartoonBomb.GetWidth()/BOMB_CARTOON_COUNT,m_TextureCartoonBomb.GetHeight());

		//绘画效果
		INT nYPosScreen=__max(nHeight-SizeCartoonBomb.cy-250,0);
		m_TextureCartoonBomb.DrawImage(pD3DDevice,(nWidth-SizeCartoonBomb.cx)/2,nYPosScreen,SizeCartoonBomb.cx,SizeCartoonBomb.cy,
			SizeCartoonBomb.cx*(m_wCartoonBombIndex%BOMB_CARTOON_COUNT),0);
	}

	//翻牌动画
	if (m_bReversalCard==true)
	{
		//获取大小
		CSize SizeReversalCard;
		SizeReversalCard.SetSize(m_TextureReversalCard.GetWidth()/REVERSAL_CARD_COUNT,m_TextureReversalCard.GetHeight());

		//绘画效果
		for (BYTE i=0;i<3;i++)
		{
			m_TextureReversalCard.DrawImage(pD3DDevice,nWidth/2+i*70-102,13,SizeReversalCard.cx,SizeReversalCard.cy,
				SizeReversalCard.cx*(m_wReversalCardIndex%REVERSAL_CARD_COUNT),0);
		}
	}

	//洗牌动画
	if (m_bRiffleCard==true)
	{
		if (m_wDispatchCardIndex>=RIFFLE_CARD_COUNT_ALL)
		{
			//获取大小
			CSize SizeCardItem;
			CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

			//获取纹理
			CD3DTexture * pD3DTexture=NULL;
			CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

			//计算位置
			INT nDistance=__min(DISPATCH_HEAP_DISTANCE-2,m_wDispatchCardIndex-RIFFLE_CARD_COUNT_ALL)+2;
			INT nXSourcePos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*nDistance)/2;

			//获取纹理
			for (BYTE i=0;i<DISPATCH_COUNT;i++)
			{
				INT nXDrawPos=nXSourcePos+i*nDistance;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*2,SizeCardItem.cy*4);
			}
		}
		else
		{
			//获取大小
			CSize SizeDispatchCard;
			SizeDispatchCard.SetSize(m_TextureDispatchCard.GetWidth()/RIFFLE_CARD_COUNT_X,m_TextureDispatchCard.GetHeight()/RIFFLE_CARD_COUNT_Y);

			//绘画效果
			m_TextureDispatchCard.DrawImage(pD3DDevice,(nWidth-SizeDispatchCard.cx)/2,nHeight/2-220,SizeDispatchCard.cx,SizeDispatchCard.cy,
				SizeDispatchCard.cx*(m_wDispatchCardIndex%RIFFLE_CARD_COUNT_X),SizeDispatchCard.cy*(m_wDispatchCardIndex/RIFFLE_CARD_COUNT_X));
		}
	}

	//发牌动画
	if (m_bDispatchCard==true)
	{
		//变量定义
		CSize SizeControl[GAME_PLAYER];
		CPoint PointControl[GAME_PLAYER];

		//获取大小
		CSize SizeCardItem;
		CCardControl::m_CardResource.GetCardSize(true,SizeCardItem);

		//获取纹理
		CD3DTexture * pD3DTexture=NULL;
		CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

		//计算位置
		INT nYHeapPos=nHeight/2-200;
		INT nXHeapPos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*DISPATCH_HEAP_DISTANCE)/2;

		//获取位置
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_HandCardControl[i].GetControlSize(SizeControl[i]);
			m_HandCardControl[i].GetOriginPoint(PointControl[i]);
		}

		//绘画牌堆
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((DISPATCH_COUNT-i)*DISPATCH_CARD_SPACE>m_wDispatchCardIndex)
			{
				INT nXDrawPos=nXHeapPos+i*DISPATCH_HEAP_DISTANCE;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-200,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*2,SizeCardItem.cy*4);
			}
		}

		//绘画发牌
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((m_wDispatchCardIndex>=(i+1)*DISPATCH_CARD_SPACE)&&(m_wDispatchCardIndex<(i+1)*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES))
			{
				//变量定义
				WORD wSendUser=(m_wStartUser+i)%GAME_PLAYER;
				WORD wSendIndex=m_wDispatchCardIndex-(i+1)*DISPATCH_CARD_SPACE;

				//目标位置
				INT nXTargetPos=PointControl[wSendUser].x+SizeControl[wSendUser].cx/2;
				INT nYTargetPos=PointControl[wSendUser].y+SizeControl[wSendUser].cy/2;

				//起点位置
				INT nYSourcePos=nYHeapPos+SizeCardItem.cy/2;
				INT nXSourcePos=nXHeapPos+(DISPATCH_COUNT-i-1)*DISPATCH_HEAP_DISTANCE+SizeCardItem.cx/2;

				//绘画扑克
				INT nXDrawPos=nXSourcePos+wSendIndex*(nXTargetPos-nXSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cx/2;
				INT nYDrawPos=nYSourcePos+wSendIndex*(nYTargetPos-nYSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cy/2;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nYDrawPos,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*2,SizeCardItem.cy*4);
			}
		}
	}

	//报警动画
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			//获取大小
			CSize SizeCountWarn;
			SizeCountWarn.SetSize(m_TextureCountWarn.GetWidth()/CARD_WARNING_COUNT,m_TextureCountWarn.GetHeight());

			//绘画效果
			m_TextureCountWarn.DrawImage(pD3DDevice,m_ptCountWarn[i].x,m_ptCountWarn[i].y,SizeCountWarn.cx,SizeCountWarn.cy,
				SizeCountWarn.cx*m_wCountWarnIndex[i],0);
		}
	}


	//历史积分
	if (m_bShowScore==true)
	{
		//获取大小
		CSize SizeHistoryScore;
		SizeHistoryScore.SetSize(m_TextureHistoryScore.GetWidth(),m_TextureHistoryScore.GetHeight());

		//积分背景
		m_TextureHistoryScore.DrawImage(pD3DDevice,nWidth-SizeHistoryScore.cx-40,5);

		//计算位置
		INT nYBenchmark=5;
		INT nXBenchmark=nWidth-SizeHistoryScore.cx-40;

		//绘画信息
		for (WORD i=0;i<GAME_PLAYER;i++)
		{			
			//获取用户
			IClientUserItem * pIClientUserItem=GetClientUserItem(i);
			if ((pIClientUserItem==NULL)||(m_pHistoryScore[i]==NULL)) continue;

			//位置计算
			CRect rcAccounts(nXBenchmark+13,nYBenchmark+31+i*23,nXBenchmark+83,nYBenchmark+43+i*23);
			CRect rcTurnScore(nXBenchmark+85,nYBenchmark+31+i*23,nXBenchmark+135,nYBenchmark+43+i*23);
			CRect rcCollectScore(nXBenchmark+139,nYBenchmark+31+i*23,nXBenchmark+185,nYBenchmark+43+i*23);

			//构造信息
			TCHAR szTurnScore[16]=TEXT("");
			TCHAR szCollectScore[16]=TEXT("");
			_sntprintf(szTurnScore,CountArray(szTurnScore),SCORE_STRING,m_pHistoryScore[i]->lTurnScore);
			_sntprintf(szCollectScore,CountArray(szCollectScore),SCORE_STRING,m_pHistoryScore[i]->lCollectScore);

			//绘画信息
			D3DCOLOR crColor1=D3DCOLOR_XRGB(255,153,0);
			D3DCOLOR crColor2=D3DCOLOR_XRGB(0,255,153);
			LPCTSTR pszNickName=pIClientUserItem->GetNickName();
			DrawTextString(pD3DDevice,pszNickName,&rcAccounts,DT_END_ELLIPSIS|DT_LEFT,D3DCOLOR_XRGB(255,255,255));
			DrawTextString(pD3DDevice,szTurnScore,&rcTurnScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lTurnScore>=0)?crColor1:crColor2);
			DrawTextString(pD3DDevice,szCollectScore,&rcCollectScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lCollectScore>=0)?crColor1:crColor2);
		}
	}

	return;
}

//单元积分
VOID CGameClientView::SetCellScore(LONGLONG lCellScore)
{
	if (m_lCellScore!=lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//炸弹次数
VOID CGameClientView::SetBombCount(BYTE cbBombCount)
{
	if (m_cbBombCount!=cbBombCount)
	{
		//设置变量
		m_cbBombCount=cbBombCount;

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//设置庄家
VOID CGameClientView::SetBankerUser(WORD wBankerUser)
{
	if (m_wBankerUser!=wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//设置倍数
VOID CGameClientView::SetBankerScore(BYTE cbBankerScore)
{
	if (m_cbBankerScore!=cbBankerScore)
	{
		//设置变量
		m_cbBankerScore=cbBankerScore;

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//显示底牌
VOID CGameClientView::ShowBankerCard(BYTE cbCardData[3])
{
	//设置变量
	m_bReversalCard=true;
	m_wReversalCardIndex=0;

	//启动时间
	m_ReversalCardLapseCount.Initialization();

	//设置控件
	m_BackCardControl.SetShowCount(0);
	m_BackCardControl.SetCardData(cbCardData,3);

	//加载资源
	ASSERT(m_TextureReversalCard.IsNull()==true);
	if (m_TextureReversalCard.IsNull()==true) m_TextureReversalCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"),TEXT("PNG"));

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置结束
VOID CGameClientView::ShowGameScoreInfo(tagScoreInfo & ScoreInfo)
{
	//设置信息
	m_ScoreControl.SetScoreInfo(ScoreInfo);

	//创建控件
	if (m_ScoreControl.IsWindowActive()==false)
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//创建窗口
		CRect rcCreate(0,0,0,0);
		m_ScoreControl.ActiveWindow(rcCreate,WS_VISIBLE,IDC_GAME_SCORE,&m_VirtualEngine,NULL);

		//移动位置
		CRect rcScoreControl;
		m_ScoreControl.GetWindowRect(rcScoreControl);
		m_ScoreControl.SetWindowPos((rcClient.Width()-rcScoreControl.Width())/2,(rcClient.Height()-rcScoreControl.Height())*2/5,0,0,SWP_NOSIZE);
	}

	return;
}



// 绘画数字
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// 绘画数字
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// 绘画数字
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
	// 加载资源
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//飞机动画
VOID CGameClientView::ShowCartoonPlan(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonPlan=true;
		m_wCartoonPlanIndex=0L;

		//启动时间
		m_CartoonPlanLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonPlan.IsNull()==true)
		{
			m_TextureCartoonPlan.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_PLAN"),TEXT("PNG"));
		}

		//窗口大小
		CRect rcClient;
		GetClientRect(&rcClient);

		//设置位置
		m_ptCartoonPlan.SetPoint( rcClient.Width() , rcClient.Height()/2 );

		//更新界面
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//停止动画
		if (m_bCartoonPlan==true)
		{
			//设置变量
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			//设置位置
			m_ptCartoonPlan.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();

			//更新界面
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//爆炸动画
VOID CGameClientView::ShowCartoonBomb(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonBomb=true;
		m_wCartoonBombIndex=0L;

		//启动时间
		m_CartoonBombLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonBomb.IsNull()==true)
		{
			m_TextureCartoonBomb.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_BOMB"),TEXT("PNG"));
		}

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//停止动画
		if (m_bCartoonBomb==true)
		{
			//设置变量
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//卸载资源
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();

			//更新界面
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//火箭动画
VOID CGameClientView::ShowCartoonRocket(bool bCartoon)
{
	if (bCartoon==true)
	{
		//设置变量
		m_bCartoonRocket=true;
		m_wCartoonRocketIndex=0L;

		//启动时间
		m_CartoonRocketLapseCount.Initialization();

		//加载资源
		if (m_TextureCartoonRocket.IsNull()==true)
		{
			m_TextureCartoonRocket.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("ROCKET"),TEXT("PNG"));
		}

		//窗口大小
		CRect rcClient;
		GetClientRect(&rcClient);

		//设置位置
		m_ptCartoonRocket.SetPoint( rcClient.Width() /2 , rcClient.Height() );

		//更新界面
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//停止动画
		if (m_bCartoonRocket==true)
		{
			//设置变量
			m_bCartoonRocket=false;
			m_wCartoonRocketIndex=0L;

			//设置位置
			m_ptCartoonRocket.SetPoint( 0, 0 );

			//卸载资源
			ASSERT(m_TextureCartoonRocket.IsNull()==false);
			if (m_TextureCartoonRocket.IsNull()==false) m_TextureCartoonRocket.Destory();

			//更新界面
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//设置上轮
VOID CGameClientView::SetLastTurnState(bool bLastTurnState)
{
	if (m_bLastTurnState!=bLastTurnState)
	{
		//设置变量
		m_bLastTurnState=bLastTurnState;

		//资源管理
		if (m_bLastTurnState==true)
		{
			ASSERT(m_TextureLastTurn.IsNull()==true);
			if (m_TextureLastTurn.IsNull()==true) m_TextureLastTurn.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("LAST_TURN"),TEXT("PNG"));
		}
		else
		{
			ASSERT(m_TextureLastTurn.IsNull()==false);
			if (m_TextureLastTurn.IsNull()==false) m_TextureLastTurn.Destory();
		}

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//等待叫分
VOID CGameClientView::SetWaitCallScore(bool bWaitCallScore)
{
	if (m_bWaitCallScore!=bWaitCallScore)
	{
		//设置变量
		m_bWaitCallScore=bWaitCallScore;

		//资源管理
		if (m_bWaitCallScore==true)
		{
			ASSERT(m_TextureWaitCallScore.IsNull()==true);
			if (m_TextureWaitCallScore.IsNull()==true) m_TextureWaitCallScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("WAIT_CALL_SCORE"),TEXT("PNG"));
		}
		else
		{
			ASSERT(m_TextureWaitCallScore.IsNull()==false);
			if (m_TextureWaitCallScore.IsNull()==false) m_TextureWaitCallScore.Destory();
		}

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//设置放弃
VOID CGameClientView::SetUserPassState(WORD wChairID, bool bUserPass)
{
	//变量定义
	bool bLoadImage=false;

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		//设置变量
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bUserPass[i]=bUserPass;
		}
	}
	else
	{
		//设置变量
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//状态检测
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	//资源管理
	if (bLoadImage==true)
	{
		if (m_TextureUserPass.IsNull()==true)
		{
			m_TextureUserPass.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("USER_PASS"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureUserPass.IsNull()==false)
		{
			m_TextureUserPass.Destory();
		}
	}

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置警告
VOID CGameClientView::SetUserCountWarn(WORD wChairID, bool bCountWarn)
{
	//变量定义
	bool bLoadImage=false;

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		//设置变量
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bCountWarn[i]=bCountWarn;
		}
	}
	else
	{
		//设置变量
		ASSERT(wChairID<GAME_PLAYER);
		m_bCountWarn[wChairID]=bCountWarn;
	}

	//状态检测
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	//资源管理
	if (bLoadImage==true)
	{
		if (m_TextureCountWarn.IsNull()==true)
		{
			m_TextureCountWarn.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("COUNT_WARN"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureCountWarn.IsNull()==false)
		{
			m_TextureCountWarn.Destory();
		}
	}

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置叫分
VOID CGameClientView::SetUserCallScore(WORD wChairID, BYTE cbCallScore)
{
	//变量定义
	bool bLoadImage=false;

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		//设置变量
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_cbCallScore[i]=cbCallScore;
		}
	}
	else
	{
		//设置变量
		ASSERT(wChairID<GAME_PLAYER);
		m_cbCallScore[wChairID]=cbCallScore;
	}

	//状态检测
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbCallScore[i]!=0)
		{
			bLoadImage=true;
			break;
		}
	}

	//资源管理
	if (bLoadImage==true)
	{
		if (m_TextureCallScore.IsNull()==true)
		{
			m_TextureCallScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CALL_SCORE"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureCallScore.IsNull()==false)
		{
			m_TextureCallScore.Destory();
		}
	}

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//设置积分
VOID CGameClientView::SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	m_pHistoryScore[wChairID]=pHistoryScore;

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//发牌动画
VOID CGameClientView::StopDispatchCard()
{
	//停止动画
	if ((m_bRiffleCard==true)||(m_bDispatchCard==true))
	{
		//设置变量
		m_bRiffleCard=false;
		m_bDispatchCard=false;
		m_wDispatchCardIndex=0L;

		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i==MYSELF_VIEW_ID)
			{
				m_HandCardControl[i].SetCardData(m_cbHandCardData,CountArray(m_cbHandCardData));
			}
			else
			{
				m_HandCardControl[i].SetCardData(NORMAL_COUNT);
			}
		}

		//卸载资源
		ASSERT(m_TextureDispatchCard.IsNull()==false);
		if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//发牌动画
VOID CGameClientView::ShowDispatchCard(BYTE cbCardData[NORMAL_COUNT], WORD wStartUser)
{
	//设置变量
	m_bRiffleCard=true;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//设置变量
	m_wStartUser=wStartUser;
	CopyMemory(m_cbHandCardData,cbCardData,sizeof(BYTE)*NORMAL_COUNT);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(0);
	}

	//启动时间
	m_DispatchCardLapseCount.Initialization();

	//加载资源
	ASSERT(m_TextureDispatchCard.IsNull()==true);
	if (m_TextureDispatchCard.IsNull()==true) m_TextureDispatchCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"),TEXT("PNG"));

	//更新界面
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"));

	return;
}

//开始按钮
VOID CGameClientView::OnBnClickedStart()
{
	//发送消息
	SendEngineMessage(IDM_START,0,0);

	return;
}

//积分按钮
VOID CGameClientView::OnBnClickedScore()
{
	//设置变量
	m_bShowScore=!m_bShowScore;

	//加载资源
	if (m_bShowScore==true)
	{
		ASSERT(m_TextureHistoryScore.IsNull()==true);
		m_TextureHistoryScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("HISTORY_SCORE"),TEXT("PNG"));
	}
	else
	{
		ASSERT(m_TextureHistoryScore.IsNull()==false);
		if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();
	}

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,(m_bShowScore==true)?TEXT("BT_CLOSE_SCORE"):TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);

	return;
}

//出牌按钮
VOID CGameClientView::OnBnClickedOutCard()
{
	//发送消息
	SendEngineMessage(IDM_OUT_CARD,0,0);

	return;
}

//排序按钮
VOID CGameClientView::OnBnClickedSortCard()
{
	//发送消息
	SendEngineMessage(IDM_SORT_HAND_CARD,0,0);

	return;
}

//上轮按钮
VOID CGameClientView::OnBnClickedLastTurn()
{
	//发送消息
	SendEngineMessage(IDM_LAST_TURN_CARD,0,0);

	return;
}

//托管按钮
VOID CGameClientView::OnBnClickedTrustee()
{
	//发送消息
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//托管按钮
VOID CGameClientView::OnBnClickedCancelTrustee()
{
	//发送消息
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//PASS按钮
VOID CGameClientView::OnBnClickedPassCard()
{
	//发送消息
	SendEngineMessage(IDM_PASS_CARD,0,0);

	return;
}

//提示按钮
VOID CGameClientView::OnBnClickedOutPrompt()
{
	//发送消息
	SendEngineMessage(IDM_OUT_PROMPT,0,0);

	return;
}

//一分按钮
VOID CGameClientView::OnBnClickedCallScore1()
{
	//发送消息
	SendEngineMessage(IDM_CALL_SCORE,1,1);

	return;
}

//二分按钮
VOID  CGameClientView::OnBnClickedCallScore2()
{
	//发送消息
	SendEngineMessage(IDM_CALL_SCORE,2,2);

	return;
}

//三分按钮
VOID  CGameClientView::OnBnClickedCallScore3()
{
	//发送消息
	SendEngineMessage(IDM_CALL_SCORE,3,3);

	return;
}

//放弃按钮
VOID  CGameClientView::OnBnClickedCallScoreNone()
{
	//发送消息
	SendEngineMessage(IDM_CALL_SCORE,0xFF,0xFF);

	return;
}

//鼠标消息
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//发送出牌
	if ( m_HandCardControl[MYSELF_VIEW_ID].GetPositively() )
	{
		SendEngineMessage(IDM_OUT_CARD,1,1);
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	//获取弹起数量扑克
	BYTE cbOldShootCard[MAX_CARD_COUNT];
	BYTE cbOldShootCrad = m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbOldShootCard, MAX_CARD_COUNT);

	//默认处理
	if (m_bMoveMouse==false)
	{
		//获取扑克
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItem) pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

		//设置扑克
		if (pCardItem!=NULL)
		{
			//设置扑克
			pCardItem->bShoot=!pCardItem->bShoot;

			//发送消息
			SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

			//更新界面
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	//选择处理
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		if (m_cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootArea(m_cbMouseDownItem,m_cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootArea(m_cbHoverCardItem,m_cbMouseDownItem);
		}

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//交换处理
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);
		SendEngineMessage(IDM_SORT_HAND_CARD,ST_CUSTOM,ST_CUSTOM);

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//获取扑克
	BYTE cbShootCard[MAX_CARD_COUNT];
	ZeroMemory(cbShootCard, sizeof(cbShootCard));
	BYTE cbShootCrad = m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbShootCard, MAX_CARD_COUNT);

	//弹起顺子
	if ( cbOldShootCrad == 1 && cbShootCrad == 2 && !m_bSwitchCard && !m_bMoveMouse &&m_HandCardControl[MYSELF_VIEW_ID].SetShootOrderCard(cbShootCard[0], cbShootCard[1]) )
	{
		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//释放鼠标
	ReleaseCapture();


	//设置变量
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//设置索引
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbHoverCardItem=INVALID_ITEM;

	return;
}

//鼠标消息
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
	__super::OnMouseMove(nFlags, MousePoint);

	//变量定义
	BYTE cbHoverCardItem=INVALID_ITEM;

	//移动判断
	if (m_cbHoverCardItem!=INVALID_ITEM)
	{
		//扑克大小
		CSize ControlSize;
		m_HandCardControl[MYSELF_VIEW_ID].GetControlSize(ControlSize);

		//扑克位置
		CPoint OriginPoint;
		m_HandCardControl[MYSELF_VIEW_ID].GetOriginPoint(OriginPoint);

		//横行调整
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//获取索引
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		cbHoverCardItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(MousePoint);

		//移动变量
		if (cbHoverCardItem!=m_cbHoverCardItem) m_bMoveMouse=true;
	}

	////交换扑克
	//if ((m_bSwitchCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	//{
	//	//目标判断
	//	BYTE cbTargetCardItem=INVALID_ITEM;
	//	cbTargetCardItem=(m_cbSwitchCardItem>cbHoverCardItem)?0:cbHoverCardItem-m_cbSwitchCardItem;

	//	//设置扑克
	//	m_HandCardControl[MYSELF_VIEW_ID].MoveCardItem(cbTargetCardItem);

	//	//更新界面
	//	CRect rcClient;
	//	GetClientRect(&rcClient);
	//	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	//}

	//选择扑克
	if ((m_bSelectCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	{
		//设置扑克
		if (cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(m_cbMouseDownItem,cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(cbHoverCardItem,m_cbMouseDownItem);
		}

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//设置变量
	m_cbHoverCardItem=cbHoverCardItem;

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);


	//状态判断
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//获取扑克
		m_cbMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);

		//设置扑克
		if (m_cbMouseDownItem!=INVALID_ITEM)
		{
			//获取扑克
			tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_cbMouseDownItem);

			//设置变量
			m_bMoveMouse=false;
			m_cbHoverCardItem=m_cbMouseDownItem;

			//操作变量
			if (pCardItem->bShoot==true)
			{
				//设置变量
				m_bSwitchCard=true;
				m_cbSwitchCardItem=0;

				//设置状态
				for (BYTE i=0;i<m_cbMouseDownItem;i++)
				{
					tagCardItem * pCardItemTemp=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i);
					if ((pCardItemTemp!=NULL)&&(pCardItemTemp->bShoot==true)) m_cbSwitchCardItem++;
				}
			}
			else
			{
				m_bSelectCard=true;
			}

			//设置鼠标
			SetCapture();
		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//获取扑克
	tagCardItem * pCardItem = m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

	//设置扑克
	if ( pCardItem == NULL )
	{
		//禁止按钮
		m_btOutCard.EnableWindow(FALSE);

		//收起扑克
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else if ( m_HandCardControl[MYSELF_VIEW_ID].GetPositively()  )
	{
		//收起扑克
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//弹起同点
		m_HandCardControl[MYSELF_VIEW_ID].SetShootSameCard(pCardItem->cbCardData);

		//发送消息
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_HandCardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true)
	{
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//消息命令
BOOL CGameClientView::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch( LOWORD(wParam) )
	{
	case IDC_CARD_PROMPT_1:
	case IDC_CARD_PROMPT_2:
	case IDC_CARD_PROMPT_3:	
	case IDC_CARD_PROMPT_4:
	case IDC_CARD_PROMPT_5:	
	case IDC_CARD_PROMPT_6:
		{
			//发送消息
			SendEngineMessage(IDM_SEARCH_CARD,LOWORD(wParam)-IDC_CARD_PROMPT_1,0);

			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////////
