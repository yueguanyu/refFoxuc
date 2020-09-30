#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//数目定义

//游戏定时器
#define IDI_GAME_START					11								//游戏开始
#define IDI_SET_BASEBALL     			17								//设置母球
#define IDI_HIT_BALL                    18                              //击球
#define IDI_WAIT_SENDRESULT             19                              //等待发送击球结果的时间

 #define	TIME_GAME_START			(30*1000)							//游戏开始
 #define	TIME_SET_BASEBALL		(10*1000)							//设置母球
 #define TIME_HIT_BALL          (20*1000)							//击球
 #define TIME_WAIT_SENDRESULT  (40*1000)                            //等待发送击球结果的时间

//公共宏定义
#define KIND_ID						188							    	//游戏 I D
#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					TEXT("美式8球")						//游戏名字
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)					//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)					//程序版本
#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)		//游戏类型
#define BALL_NUM                    16

//游戏状态
#define GS_WK_FREE					GAME_STATUS_FREE								//等待开始
#define GS_WK_SET_SETBASEBALL       GAME_STATUS_FREE+1                           //设置母球
#define GS_WK_WAIT_HITBALL          GAME_STATUS_FREE+2                           //等待击球
#define GS_WK_PLAYING				GAME_STATUS_PLAY+4						//游戏进行
#define MAX_DOUBLE					200									//游戏倍数
//////////////////////////////////////////////////////////////////////////
//服务器命令结构


#define SUB_S_GAME_START			104									//游戏开始
#define SUB_S_GAME_END				107									//游戏结束
#define SUB_S_GAME_CONFIG			115									//配置
#define SUB_S_HITBALL               116                                 //击球
#define SUB_S_SETBASEBALL           117                                 //设置母球
#define SUB_S_END_SETBASEBALL       118                                 //设置母球结束
#define SUB_S_END_SETBASEBALL_TIMEOUT  119                              //设置母球结束，超时
#define SUB_S_CLUBPOS               120                                 //球杆
#define SUB_S_HIT_RESULT            121                                 //打击结果
#define SUB_S_HIT_TIMEOUT           122                                 //击球超时
#define SUB_S_GIVEUP                123                                 //放弃
#define SUB_S_STARTHITBALL          124                                 //开球
 
 
struct CMD_S_HIT_TIMEOUT
{
	WORD currentUser;
	int  timeOutCount;
};
 
//游戏状态
struct CMD_S_StatusFree
{
	WORD							wServerType;						//服务器类型
};
struct PT
{
	float x;
	float y;
};
//游戏过程中
struct CMD_S_StatusGamePlay
{
	BYTE      currentUser;		      //当前的玩家	
	PT		  ptBall[BALL_NUM];       //每个球的位置
	BOOL	  ballState[BALL_NUM];    //球是否可见
	BYTE      inHoleBall[2][8];       //进洞的球
	BYTE      maxComob[2];               //最高连击数
	BYTE      userBallType[2];        //大小球
	BYTE      gameStatus;             //当前的状态
	__int64   baseScore;              //低分
}; 
//游戏开始
struct CMD_S_GameStart
{
	__int64                        nBaseScore;
	BYTE                           currentUser;
	BYTE                           roundCount;
};
//设置母球
struct CMD_S_SET_BASEBALL
{
	float mousex;
	float mousey;
};
//设置结束
struct CMD_S_END_SET_BASEBALL
{
	float ballPosx;
	float ballPosy;
};
struct CMD_S_END_SET_BASEBALL_TIMEOUT
{
	float ballPosx;
	float ballPosy;
	int   timeOutCount;//超时次数
};
//球杆
struct CMD_S_BALLCLUB
{
	PT ptMousePos;			//鼠标位置,相对桌子的位置
	PT ptBaseBallPos;		//白球位置
	float  m_hitForce;          //打击力度
};
//击球
struct CMD_S_HitBall
{
	float                           fHitForce;               //力量
	float                           fMousePosx;              //击球点
	float                           fMousePosy;              //击球点
	float                           fHitPosx;                //击球点
	float                           fHitPosy;                //击球点
	float                           fbaseBallPosx;           //白球
	float                           fbaseBallPosy;           //白球
	float                           fAngle;                  //角度
	WORD                            wHitUser;                //击球用户
	int                             nCurHitNum;                //当前第几杆
};

//进球结果
struct CMD_S_HitResult
{
	bool    bHitWithWall;           //是否有碰墙
	BYTE    inHoleNum;				//进球数
	BYTE    ballInHole[BALL_NUM];	//进的球
	BYTE    firstBallNum;           //第一个碰到的球
	PT      ptBall[BALL_NUM];       //每个球的状态
};
enum HIT_RESULT
{
	openFail=1,                     //开球失败
	firtIn=2,                       //首次进球 
	noInHole=3,						//没有进球
	rightInHole=4,					//正常进球
	wrongInHole=5,                  //犯规
	gameEnd=6,                      //游戏结束
};

//发向客服端的消息
struct CMD_S_CLIENT_HITRESLUT
{
	BYTE    inHoleNum;				//进球数
	BYTE    ballInHole[BALL_NUM];	//进的球
	BYTE    firstBallNum;           //第一个碰到的球
	PT      ptBall[BALL_NUM];       //每个球的位置
	BOOL    ballState[BALL_NUM];    //球的状态
	BYTE    currentUser;            //下一个打球的玩家
	BYTE    hitResult;              //打球的结果
	BYTE    comobNum;               //连击数
	BYTE    totalComobNum[2]  ;     //每个人的连击数
	BYTE    userBallType[2];        // 1：是大球，0：是小球
};
//游戏结束
struct CMD_S_GameEnd
{
	__int64							nGameScore[GAME_PLAYER];						//游戏积分
	__int64							nBaseScore;										//基础分
	BYTE                            comobNum[GAME_PLAYER];                          //连击数
	BYTE                            endReason; //1:正常结算，2，认输，3，犯规,4超时,5数据错误，游戏解散 //结算原因。

};

//////////////////////////////////////////////////////////////////////////

#endif