#pragma once
#include "../游戏服务器/ServerControl.h"


class CServerControlItemSink : public IServerControl
{
//控制变量
protected:
	BYTE							m_cbControlArea;						//控制区域
	BYTE							m_cbControlTimes;						//控制次数
public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	////服务器控制
	//virtual bool __cdecl RecvControlReq(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);
	////执行控制
	//virtual bool __cdecl ExcuteControl(BYTE *cbCard);
	//服务器控制
	virtual bool  RecvControlReq(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);
	//执行控制
	virtual bool  ExcuteControl(BYTE *cbCard);
};
