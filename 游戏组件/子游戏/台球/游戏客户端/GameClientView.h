#pragma once

#include "Stdafx.h"
 
#include "movemovie.h" 
#include "SWichMovie.h"
#include "BallRender.h"
#include "PoolPhysics.h"
#include "InterfaceView.h"

//#define DEBUG_GAME
//////////////////////////////////////////////////////////////////////////
//消息定义
#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_SETBASERULE				(WM_USER+110)						//设置基础分
#define IDI_SUSPEND_SEND                2000                                //暂停发送时间

//按钮ID 
#define IDC_START						1									//开始按钮
#define IDC_RESTART     				2									//开始按钮 
#define IDC_SOUND						3									//语音
#define IDC_GIVEPU                      4									//认输
#define IDC_REBEGIN                     5

// 声音ID
#define IDS_BACK						1
#define IDS_CLOCK						2
#define IDS_ENTER						3
#define IDS_COMBO						4
#define IDS_BEG							5
#define IDS_WIN							6
#define IDS_FAILURE						7
#define IDS_HIT							8
#define IDS_COIN						9
#define IDS_GOOD						10
#define IDS_COLL						11
#define IDS_HOLE						12

class CGameClientDlg;
enum BASE_BALL_STATE
{
	nothing= 0,//无效
	waitSet = 1,//等待设置
	waitHit = 2,//等待击球
	startHit = 3 ,//开始击球
	afterHit = 4,//击过球了
	allState = 5,//全部静止
};

//游戏视图
class CGameClientView : public CGameFrameViewGDI, public ITQView
{

public:
	physics                        m_physics;                          //物理系统
	CBallRender					   m_balls[BALL_NUM];				   // 球
	CBallRender					   m_visibleBalls[BALL_NUM];		   // 没有打进去的球
	CPoint                         m_ptMousePos;                       //鼠标的位置
	CPoint                         m_ptInBallDesk;                     //鼠标在球桌上的位置
	CPoint                         m_ptHitMark;                        //调整杆法的球的点
	CPoint                         m_ptCenterHitMark;                  //调整杆法的球的中心点
	float                          m_ClubAnge;                         //杆子的角度
	int                            m_offsetx;						   //球桌相对应窗口的偏移量
	int                            m_offsety;
	float                          m_hitForce;                         // 打击力
	HCURSOR                        m_cursor;                           //光标
	__int64						   m_nBaseScore;					   //基础分数
	DWORD						   m_dwLastClickLButtonTime;		   //最后点左键的时间
	int                            m_offsetDistance;                   //打球时候，球杆的缩杆距离
	BYTE                           m_comobNum[GAME_PLAYER];            //最高连击数
	BYTE                           m_baseBallState;                    // 白球的状态
	bool                           m_bFirstInBall;                     //是否是第一个进球
	bool                           m_bHit;                             //是否打球了
	__int64                        m_endScore[GAME_PLAYER];
	BYTE						   m_userBallType[GAME_PLAYER];        //1：是大球，0：是小球
	BYTE                           m_roundCount;                       //局数
	bool                           m_bSingleMode ;                     //是否是单机模式
	float                          m_angle;                            //角度
public:
	CGameClientDlg					*m_pParent;							//

	
	BOOL							m_IsGameEnd;						//是否游戏结束	
 	//状态变量
	long							m_lWidth;							//界面宽
	long							m_lHeight;							//界面高
	CList<CString,CString&>         m_systemInfoList;                   //输出信息



	//位置信息
public:
	 
	CPoint                          m_ptUserInfo[GAME_PLAYER];          //玩家信息。
	CPoint                          m_ptSeat[GAME_PLAYER];              //座位的位置
	CPoint                          m_ptBaseScore;                      //底分
	PT                              m_ptBallClub;                       //对方移动球杆
	bool                            m_bMoveBallClub;                    //是否移动球杆

	// 图片变量
public:
	CHgeSprite					*m_pSpriteBack;						//游戏背景
	CHgeSprite					*m_pSpriteLogo;						//游戏背景	
	CHgeSprite					*m_pSpriteResult;					//结算框
	CHgeSprite                    *m_pSpriteClub;                     //球杆
	CHgeSprite                    *m_pSpriteSightBall;                //瞄准球	
	CHgeSprite                    *m_pSpriteHoleBall;					//洞中球
	CHgeSprite                    *m_pSpriteMovieSingle;              //球进洞动的画
	CHgeSprite                    *m_pSpriteInfo[2];                  //玩家信息 
	CHgeSprite                    *m_pSpriteCenterInfo;               //中间的框 
	CHgeSprite                    *m_pSpriteHitMark;                  //十字
	CHgeSprite                    *m_pSpriteHitBigBall;               //瞄准球，大球
	CHgeSprite                    *m_pSpriteHitForce_left;            //力量条
	CHgeSprite                    *m_pSpriteHitForce_right;           //力量条
	CHgeSprite                    *m_pHitAngle_left;					//角度
	CHgeSprite                    *m_pHitAnge_right;                  //角度
	CHgeText					  *m_pTextForce;                      //力量
	CHgeSprite                    *m_pSpriteCoinObverse[72];          //硬币的正面
	CHgeSprite                    *m_pSpriteCoinReverse[72];          //硬币的反面
	CHgeSprite                    *m_pSpriteObverse;                  //正面开球
	CHgeSprite                    *m_pSpriteReverse;                  //反面开球
	CHgeSprite                    *m_pSpriteHand;                     //手、
	CHgeSprite                    *m_pSpriteFailOpen;                 //开局失败，继续击球
	CHgeSprite                    *m_pSpriteContinue;                 //连杆机会，继续击球
	CHgeSprite                    *m_pSpriteChange;                   //普通击球，交换对手
	CHgeSprite                    *m_pSpriteSelect;                   //选定花色，继续击球
	CHgeSprite                    *m_pSpriteSelect_hua;               //选定花色，继续击球
	CHgeSprite                    *m_pSpriteFailHit;                  //击球犯规，交换对手
	CHgeSprite                    *m_pSpriteWin;                      //赢
	CHgeSprite                    *m_pSpriteLose;                     //输
	CHgeSprite                    *m_pSpriteGameStart;                //球局开始
	CHgeSprite                    *m_pSpriteNum;		                //数字
	CHgeSprite                    *m_pSpriteBallNumBk;                //显示球号码的底图 
	CHgeSprite                    *m_pSpriteComob;                    //；连击
	CHgeSprite                    *m_pSpriteTimeBar[5];               //时间条
	CHgeSprite                    *m_pSpriteTimeNum;                  //时间数字 
	CHgeSprite                    *m_pSpriteDirection[2];             // 箭头


	//文本框
protected:
	CHgeText						*m_pTextUserInfo[GAME_PLAYER];		 //玩家信息
	CHgeText						*m_pTextUserScore[GAME_PLAYER];		 //玩家银子
	CHgeText						*m_pTextUserCombom[GAME_PLAYER];	 //连击数目
	CHgeText						*m_pTextUserBaseScore[GAME_PLAYER];  //底分
	CHgeText						*m_pTextBallNum;                     //球号
	CHgeText						*m_pTextEndResult[12];               //结算框上的文本输出  
	CHgeText						*m_pTextSystemInfo[12];              //游戏信息

	//动画
public:	
	CSWichMovie						m_liziMovie[NUMHOLE];               //粒子动画,每个洞口一个动画
	CSWichMovie                     m_coinMovieObverse;                 //投硬币的正面
	CSWichMovie                     m_coinMovieReverse;                 //投币动画反面
	CSWichMovie                     m_directionMovie;                   //方向
	CMoveMovie                      m_movieObverse;                     //正面
	CMoveMovie                      m_movieReverse;                     //反面
	CMoveMovie                      m_movieHitResult;                   //击球结果
	CMoveMovie                      m_movieGameStart;                   //球局开始
	CMoveMovie                      m_movieComob;                       //连击

	//扑克控件
public:
	CPoint							m_ptTimer[GAME_PLAYER];
	CPoint                          m_ptReady[GAME_PLAYER];


//////////////////////////////////////////////////////////////////////////
	// HGE 引擎
public:
	CHgeSound*						m_pHgeSound;						// 声音控制
	CHgeLoad						m_HgeLoad;							// 资源装载
	static HGE*						m_pHge;								// HGE指针
	static CHgeButtonMgr*			m_pBtMgr;							// 按钮控制	

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();


	//基础接口
public:
	//释放对象
	virtual VOID  Release() {  delete this; return ; }
	//接口查询
	virtual void *  QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	// 视图接口
public:
	// 获取HGE指针
	virtual HGE* __cdecl GetHge(void) { return m_pHge; }
	// 增加按钮
	virtual void __cdecl AddButton(BYTE cbID, const char *filename, DWORD size = 0, bool bMipmap = false);
	// 删除按钮
	virtual void __cdecl DelButton(BYTE cbID, bool bAll = false);
	// 设置按钮
	virtual void __cdecl SetButtonPos(BYTE cbID, CPoint pt);
	// 设置显示
	virtual void __cdecl SetButtonShow(BYTE cbID, WORD wType);
	// 获取大小
	virtual void __cdecl GetButtonSize(BYTE cbID, CSize& size);
	// 按钮事件
	virtual void __cdecl OnEventButton(WORD wButtonID, BYTE cbButtonType);
	// 增加按钮
	virtual void __cdecl AddSound(WORD wID, const char* fileName);
	// 删除声音
	virtual void __cdecl DelSound(WORD wID, bool bAll = false);
	// 播放声音
	virtual void __cdecl PlayGameSound(WORD wID, bool bLoop = false);

	//继承函数
public:
	virtual void ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight) {}
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }

	//界面更新
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight){}

public:
	// 绘制函数
	virtual bool RenderViewEx(void);
	// 绘制函数
	virtual bool FrameViewEx(void);
	// HGE初始化后
	virtual void OnInitHge(void);

	// 静态函数
protected:
	// 静态绘制接口
	static bool RenderView(void);
	// 静态绘制接口
	static bool FrameView(void);

	// 运行函数
public:
	// 初始化HGE
	void InitHge(HWND hWnd);	
	// 运行HGE
	void RunHge(float fDelTime = 0);
	// 装载资源
	bool LoadResource(void); 

public:	
	//花球杆
	void  DrawBallClub();
	//设置按钮位置
	void RectifyButton(); 


	//画图函数
private:
	//画背景
	void DrawBack( CRect *pRect );
 	//画玩家
	void DrawUser( CRect *pRect );
	//画结算框
	void DrawResultFrame( CRect *pRect );
	//画提示信息
	void DrawFront( CRect *pRect );
	//画游戏信息
	void DrawGameInfo( CRect *pRect );
	//画时间
	void DrawTime( int nTime, CPoint point );
	void DrawTableWaitTime();

	//其他
protected:
 
	//往字符串后加空格
	void FillStringWithSpace( TCHAR str[], int nLen );
	//得到球杆的长度
	int  GetBallClubLength();
	//拆分字符串
	int SplitString(CString &str,int splitLen);

 
	//------------------------------------------------------------------------------//

	//配置函数
public:
	void SetGameEnd();
	//功能函数
public:
	
	//基础分数
	void SetBaseScore(__int64 nBaseScore);
	//设置放弃
	__int64 GetBaseScore() { return m_nBaseScore; }
	//重设UI
	void ResetUI(); 
	void ResetBall();
	//初始化在下面显示每个人没被打进去的球，leftLitterNum：左边是小球否
	void InitVisibleBall(bool leftLitterNum,bool visible=true);
	//设置游戏开始
	void SetGameStart();
	// 击球
	void HitBall(float fHitPx,float fHitPy,float mousex,float mousey,float fHitForce);
	//打击球后的检测
	bool BallStateCheck(float dt);
	//设置进球
	void SetHitBallInHole(int ballNum);
	//void 对方移动球杆
	void SetBallClubMove( PT pt);
	
	//按钮事件
protected:
	//开始按钮
	void OnStart();
	//语音
	void OnSound();
public:
	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//销毁窗口
	afx_msg void OnDestroy( );
	//时间函数
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 按钮控制
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// 按钮控制
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	// 按钮控制
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// 大小
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//动画结束的消息通知
	afx_msg LRESULT OnMovieEnd(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
