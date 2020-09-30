#pragma once
#include "../游戏服务器/ServerControl.h"


class CServerControlItemSink : public IServerControl
{
	//控制操作
protected:
	BYTE							m_cbControlArea;						//控制区域
	BYTE							m_cbControlTimes;						//控制次数


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
	virtual bool __cdecl ReturnControlArea(tagControlInfo& ControlInfo);


};
