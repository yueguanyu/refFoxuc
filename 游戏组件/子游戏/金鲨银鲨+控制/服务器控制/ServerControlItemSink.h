#pragma once
#include "../服务器组件/ServerControl.h"
#define UR_GAME_CONTROL					0x20000000L

class CServerControlItemSink : public IServerControl
{
	//控制变量
protected:
	BYTE							m_cbWinSideControl[2];				//控制输赢
	BYTE							m_cbExcuteTimes;					//执行次数
	int								m_nSendCardCount;					//发送次数

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//服务器控制
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

	//需要控制
	virtual bool __cdecl NeedControl();

	//满足控制
	virtual bool __cdecl MeetControl(tagControlInfo ControlInfo);

	//完成控制
	virtual bool __cdecl CompleteControl();

	//返回控制区域
	virtual bool __cdecl ControlResult(int	cbTableCardArray[]);

	//改变输赢控制
	virtual void __cdecl ChangeValue(BYTE m_Change,BYTE m_Change2);

};
