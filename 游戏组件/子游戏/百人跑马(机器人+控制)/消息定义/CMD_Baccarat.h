#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						114								//游戏 I D
#define GAME_PLAYER					100								//游戏人数
#define GAME_NAME					TEXT("百人跑马")				//游戏名字

#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本
//状态定义
#define GS_FREE						GAME_STATUS_FREE				//等待开始
#define	GS_BET						GAME_STATUS_PLAY				//下注状态
#define	GS_BET_END					GAME_STATUS_PLAY+1				//下注结束状态
#define	GS_HORSES					GAME_STATUS_PLAY+2				//结束状态

//游戏消息
#define IDM_PLAYER_BET				(WM_USER + 1000)				//加住信息
#define IDM_ADMIN_COMMDN			(WM_USER + 999)					//控制信息

#define SERVER_LEN					32								//房间长度

//历史记录
#define MAX_SCORE_HISTORY			20								//历史个数

//马匹索引
#define	HORSES_ONE					0								//1号马
#define	HORSES_TWO					1								//2号马
#define	HORSES_THREE				2								//3号马
#define	HORSES_FOUR					3								//4号马
#define	HORSES_FIVE					4								//5号马
#define	HORSES_SIX					5								//6号马
#define HORSES_ALL					6								//合计索引

//马匹位置
#define	HORSES_X_POS				180									//起始X位置
#define	HORSES_ONE_Y_POS			315 								//1号马
#define	HORSES_TWO_Y_POS			285 								//2号马
#define	HORSES_THREE_Y_POS			255 								//3号马
#define	HORSES_FOUR_Y_POS			220									//4号马
#define	HORSES_FIVE_Y_POS			190									//5号马
#define	HORSES_SIX_Y_POS			160									//6号马

//下注区域索引
#define AREA_1_6					0								//1_6 索引
#define AREA_1_5					1								//1_5 索引
#define AREA_1_4					2								//1_4 索引
#define AREA_1_3					3								//1_3 索引
#define AREA_1_2					4								//1_2 索引
#define AREA_2_6					5								//2_6 索引
#define AREA_2_5					6								//2_5 索引
#define AREA_2_4					7								//2_4 索引
#define AREA_2_3					8								//2_3 索引
#define AREA_3_6					9								//3_6 索引
#define AREA_3_5					10								//3_5 索引
#define AREA_3_4					11								//3_4 索引
#define AREA_4_6					12								//4_6 索引
#define AREA_4_5					13								//4_5 索引
#define AREA_5_6					14								//5_6 索引
#define AREA_ALL					15								//合计索引

//跑马名次
#define RANKING_FIRST				0								//第一名
#define RANKING_SECOND				1								//第二名
#define RANKING_THIRD				2								//第三名
#define RANKING_FOURTH				3								//第四名
#define RANKING_FIFTH				4								//第五名
#define RANKING_SIXTH				5								//第六名
#define RANKING_NULL				6								//无名次

//下注失败类型
#define FAIL_TYPE_OVERTOP			1								//超出限制
#define FAIL_TYPE_TIME_OVER			0								//超时

//跑道长度
#define HAND_LENGTH					170							//头
#define TAIL_LENGTH					185							//尾

//预计完成时间
#define COMPLETION_TIME				25								//25秒

//速度跨步
#define STEP_SPEED					950//(COMPLETION_TIME*10)			//

//中间背景重复次数
#define BACK_COUNT					25

//基础速度
#define BASIC_SPEED					20

//高速度
#define HIGH_SPEED					300

//低速度
#define LOW_SPEED					300

//加速频率
#define FREQUENCY					5

//加速度
#define ACCELERATION				3

//马匹名字
#define HORSES_NAME_LENGTH			32

//机器人信息
struct tagRobotInfo
{
	int nChip[4];														//筹码定义
	int nAreaChance[AREA_ALL];											//区域几率
	TCHAR szCfgFileName[MAX_PATH];										//配置文件

	tagRobotInfo()
	{
		int nTmpChip[4] = {1000,10000,100000,1000000};
		int nTmpAreaChance[AREA_ALL] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
		TCHAR szTmpCfgFileName[MAX_PATH] = _T("HorseBattle.ini");

		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
	}
};


//记录信息
struct tagHistoryRecord
{
	INT								nStreak;						//场次
	INT								nRanking;						//排名
	INT								nRiskCompensate;				//赔率
	INT								nHours;							//小时
	INT								nMinutes;						//分钟
	INT								nSeconds;						//秒钟
};

//空闲场景
struct CMD_S_SceneFree
{
	INT								nTimeLeave;							//剩余时间
	INT								nStreak;							//场次
	INT								nMultiple[AREA_ALL];				//区域倍数
	TCHAR							szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字
	tagHistoryRecord				GameRecords[MAX_SCORE_HISTORY];		//游戏记录
	INT								nWinCount[HORSES_ALL];				//全天赢的场次

	LONGLONG						lAreaLimitScore;						//区域总限制
	LONGLONG						lUserLimitScore;						//个人区域限制

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
};

//下注场景
struct CMD_S_SceneBet
{
	INT								nTimeLeave;							//剩余时间
	INT								nStreak;							//场次
	INT								nMultiple[AREA_ALL];				//区域倍数
	TCHAR							szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字
	tagHistoryRecord				GameRecords[MAX_SCORE_HISTORY];		//游戏记录
	INT								nWinCount[HORSES_ALL];				//全天赢的场次

	INT								nBetPlayerCount;					//下注人数
	LONGLONG						lUserMaxScore;						//玩家最大下分数
	LONGLONG						lPlayerBet[AREA_ALL];				//玩家下注
	LONGLONG						lPlayerBetAll[AREA_ALL];			//所有下注

	LONGLONG						lAreaLimitScore;						//区域总限制
	LONGLONG						lUserLimitScore;						//个人区域限制

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
};

//下注结束场景
struct CMD_S_SceneBetEnd
{
	INT								nTimeLeave;							//剩余时间
	INT								nStreak;							//场次
	INT								nMultiple[AREA_ALL];				//区域倍数
	TCHAR							szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字
	tagHistoryRecord				GameRecords[MAX_SCORE_HISTORY];		//游戏记录
	INT								nWinCount[HORSES_ALL];				//全天赢的场次

	INT								nBetPlayerCount;					//下注人数
	LONGLONG						lPlayerBet[AREA_ALL];				//玩家下注
	LONGLONG						lPlayerBetAll[AREA_ALL];			//所有下注

	LONGLONG						lAreaLimitScore;						//区域总限制
	LONGLONG						lUserLimitScore;						//个人区域限制

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
};

//跑马场景
struct CMD_S_SceneHorses
{
	INT								nTimeLeave;							//剩余时间
	INT								nStreak;							//场次
	INT								nMultiple[AREA_ALL];				//区域倍数
	TCHAR							szHorsesName[HORSES_ALL][HORSES_NAME_LENGTH];	//马匹名字
	tagHistoryRecord				GameRecords[MAX_SCORE_HISTORY];		//游戏记录
	INT								nWinCount[HORSES_ALL];				//全天赢的场次

	INT								nBetPlayerCount;					//下注人数
	LONGLONG						lPlayerBet[AREA_ALL];				//玩家下注
	LONGLONG						lPlayerBetAll[AREA_ALL];			//所有下注
	LONGLONG						lPlayerWinning;						//玩家输赢

	LONGLONG						lAreaLimitScore;						//区域总限制
	LONGLONG						lUserLimitScore;						//个人区域限制

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_BET_START				100									//开始下注
#define SUB_S_BET_END				101									//下注结束
#define SUB_S_HORSES_START			102									//跑马开始
#define SUB_S_PLAYER_BET			103									//用户下注
#define SUB_S_PLAYER_BET_FAIL		104									//下注失败
#define SUB_S_CONTROL_SYSTEM		105									//系统控制
#define SUB_S_NAMED_HORSES			106									//马屁冠名
#define SUB_S_HORSES_END			107									//跑马结束
#define SUB_S_MANDATOY_END			108									//强制结束
#define SUB_S_ADMIN_COMMDN			109									//系统控制

//开始下注
struct CMD_S_BetStart
{
	INT								nTimeLeave;							//剩余时间
	LONGLONG						lUserMaxScore;						//玩家最大下分数
	INT								nTimeBetEnd;						//下注结束时间
	int								nChipRobotCount;					//人数上限 (下注机器人)
};

//下注结束
struct CMD_S_BetEnd
{
	INT								nTimeLeave;							//剩余时间
};


//跑马开始
struct CMD_S_HorsesStart
{
	INT								nTimeLeave;							//剩余时间
	INT								nHorsesSpeed[HORSES_ALL][COMPLETION_TIME];	//每匹马的每秒速度
	BYTE							cbHorsesRanking[RANKING_NULL];		//名次

	//玩家成绩
	LONGLONG						lPlayerWinning;						//玩家输赢
	LONGLONG						lPlayerReturnBet;					//玩家返回下注
};


//跑马结束
struct CMD_S_HorsesEnd
{
	INT								nTimeLeave;							//剩余时间
	tagHistoryRecord				RecordRecord;						//历史记录	
	INT								nWinCount[HORSES_ALL];				//全天赢的场次

	//倍数
	INT								nMultiple[AREA_ALL];				//区域倍数

	//玩家成绩
	LONGLONG						lPlayerWinning[AREA_ALL];			//玩家输赢
	LONGLONG						lPlayerBet[AREA_ALL];				//下注
};

//玩家下注
struct CMD_S_PlayerBet
{
	WORD							wChairID;							//位置
	LONGLONG						lBetScore[AREA_ALL];				//玩家下注
	INT								nBetPlayerCount;					//下注人数
	bool							bIsAndroid;							//是否机器人
};

//玩家下注失败
struct CMD_S_PlayerBetFail
{
	WORD							wChairID;							//位置
	BYTE							cbFailType;							//失败类型
	LONGLONG						lBetScore[AREA_ALL];				//玩家下注
};

//系统控制
struct CMD_S_ControlSystem
{

};

//马屁冠名
struct CMD_S_NamedHorses
{

};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_PLAYER_BET			1									//用户下注
#define SUB_C_NAMED_HORSES			2									//马屁冠名
#define SUB_C_ADMIN_COMMDN			3									//系统控制

//玩家下注
struct CMD_C_PlayerBet
{
	LONGLONG						lBetScore[AREA_ALL];				//玩家下注
};
//马屁冠名
struct CMD_C_NamedHorses
{

};


//////////////////////////////////////////////////////////////////////////
//控制
//服务器控制返回
#define	 S_CR_FAILURE				0		//失败
#define  S_CR_UPDATE_SUCCES			1		//更新成功
#define	 S_CR_SET_SUCCESS			2		//设置成功
#define  S_CR_CANCEL_SUCCESS		3		//取消成功
struct CMD_S_ControlReturns
{
	BYTE cbReturnsType;					//回复类型
	BYTE cbControlTimes;				//控制次数
	BYTE cbControlArea;					//控制区域
	BYTE bAuthoritiesExecuted;			//当局执行
	INT  nControlMultiple[AREA_ALL];	//控制倍率
};


//客户端控制申请
#define  C_CA_UPDATE				1		//更新
#define	 C_CA_SET					2		//设置
#define  C_CA_CANCELS				3		//取消
struct CMD_C_ControlApplication
{
	BYTE cbControlAppType;				//申请类型
	BYTE cbControlTimes;				//控制次数
	BYTE cbControlArea;					//控制区域
	BYTE bAuthoritiesExecuted;			//当局执行
	INT  nControlMultiple[AREA_ALL];	//控制倍率
	
};

#endif
