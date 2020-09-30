#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#ifndef _UNICODE

#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#else

#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#endif

#define UR_GAME_CONTROL					0x20000000L				//游戏特殊控制

//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						107									//游戏 I D
#define GAME_NAME					TEXT("百人龙虎斗")					//游戏名字

//组件属性
#define GAME_PLAYER					MAX_CHAIR									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_PLACE_JETTON				GAME_STATUS_PLAY					//等待开始
#define GAME_SCENE_GAME_END					GAME_STATUS_PLAY+1					//游戏进行
//////////////////////////////////////////////////////////////////////////

//区域倍数multiple
#define MULTIPLE_KONG				0								//龙 索引
#define MULTIPLE_LONG				2								//龙 倍数
#define MULTIPLE_PING				8								//平 倍数
#define MULTIPLE_HU					2								//虎 倍数
#define MULTIPLE_2_13				2								//2  - 13 倍数
#define MULTIPLE_14					8								//14 倍数
#define MULTIPLE_15_26				2								//15 - 26 倍数
#define MULTIPLE_2_6				8								//2  - 6 倍数
#define MULTIPLE_7_11				4								//7  - 11 倍数
#define MULTIPLE_12_16				3								//12 - 16 倍数
#define MULTIPLE_17_21				4								//17 - 21 倍数
#define MULTIPLE_22_26				8								//22 - 26 倍数

//区域索引area
#define AREA_KONG					0								//龙 索引
#define AREA_LONG					1								//龙 索引
#define AREA_PING					2								//平 索引
#define AREA_HU						3								//虎 索引
#define AREA_2_13					4								//2  - 13 索引
#define AREA_14						5								//14 索引
#define AREA_15_26					6								//15 - 26 索引
#define AREA_2_6					7								//2  - 6 索引
#define AREA_7_11					8								//7  - 11 索引
#define AREA_12_16					9								//12 - 16 索引
#define AREA_17_21					10								//17 - 21 索引
#define AREA_22_26					11								//22 - 26 索引
#define AREA_ALL					12								//合计索引

//计算结果
#define RESULT_WIN					1								//赢
#define RESULT_LOSE					-1								//输

//记录信息
struct tagServerGameRecord
{
	BYTE							cbResult;							//龙 平 虎
	BYTE							cbAndValues;						//和 值
	BYTE							cbLong;								//龙点
	BYTE							cbHu;								//虎点
};


//机器人信息
struct tagRobotInfo
{
	int nChip[6];														//筹码定义
	int nAreaChance[AREA_ALL];											//区域几率
	TCHAR szCfgFileName[MAX_PATH];										//配置文件
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		int nTmpChip[6] = {100,1000,10000,100000,1000000,5000000};
		int nTmpAreaChance[AREA_ALL] = {11, 1, 11, 11, 4, 11, 8, 8, 8, 8, 8};
		TCHAR szTmpCfgFileName[MAX_PATH] = _T("LongHuDouBattle.ini");

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
	}
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			//接受
#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};
//////////////////////////////////////////////////////////////////////////////////
//命令定义-服务器

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请
#define SUB_S_AMDIN_COMMAND			109									//系统控制


//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONGLONG							lPlaceScore;						//当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	DOUBLE							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	DOUBLE							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	TCHAR							szCancelUser[32];					//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	LONGLONG							lBankerScore;						//庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	LONGLONG						lUserMaxScore;							//玩家金币

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家成绩
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	TCHAR							szGameRoomName[32];			//房间名称
};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局下注
	LONGLONG						lAreaInAllScore[AREA_ALL];			//每个区域的总分				

	//个人下注
	LONGLONG						lUserInAllScore[AREA_ALL];//每个玩家每个区域的下注

	//玩家积分
	LONGLONG						lUserMaxScore;						//最大下注							

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	//扑克信息
	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][3];				//桌面扑克

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//结束信息
	LONGLONG						lEndBankerScore;					//庄家成绩
	LONGLONG						lEndUserScore;						//玩家成绩
	LONGLONG						lEndUserReturnScore;				//返回积分
	LONGLONG						lEndRevenue;						//游戏税收

	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态

	TCHAR							szGameRoomName[32];					//房间名称
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
	INT64                             nListUserCount;						//列表人数
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家位置
	LONGLONG							lBankerScore;						//庄家金币
	LONGLONG							lUserMaxScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间
	int								nChipRobotCount;					//人数上限 (下注机器人)
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG							lJettonScore;						//加注数目
	BYTE							cbAndroid;							//机器人
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][3];				//桌面扑克

	//庄家信息
	LONGLONG							lBankerScore;						//庄家成绩
	LONGLONG							lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	LONGLONG							lUserScore;							//玩家成绩
	LONGLONG							lUserReturnScore;					//返回积分

	//全局信息
	LONGLONG							lRevenue;							//游戏税收
};

//////////////////////////////////////////////////////////////////////////////////
//命令定义-客户端

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家
#define SUB_C_CANCEL_BANKER			3									//取消申请
#define SUB_C_AMDIN_COMMAND			4									//系统控制

struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据
};

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define IDM_ADMIN_COMMDN WM_USER+1000

//控制区域信息
struct tagControlInfo
{
	INT  nAreaWin[AREA_ALL];		//控制区域
};

//服务器控制返回
#define	 S_CR_FAILURE				0		//失败
#define  S_CR_UPDATE_SUCCES			1		//更新成功
#define	 S_CR_SET_SUCCESS			2		//设置成功
#define  S_CR_CANCEL_SUCCESS		3		//取消成功
struct CMD_S_ControlReturns
{
	BYTE cbReturnsType;				//回复类型
	BYTE cbControlArea[AREA_ALL];	//控制区域
	BYTE cbControlTimes;			//控制次数
};


//客户端控制申请
#define  C_CA_UPDATE				1		//更新
#define	 C_CA_SET					2		//设置
#define  C_CA_CANCELS				3		//取消
struct CMD_C_ControlApplication
{
	BYTE cbControlAppType;			//申请类型
	BYTE cbControlArea[AREA_ALL];	//控制区域
	BYTE cbControlTimes;			//控制次数
};

//////////////////////////////////////////////////////////////////////////////////

#endif