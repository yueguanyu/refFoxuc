#include "Stdafx.h"
#include "AndroidUserItemSink.h"

CAndroidUserItemSink::CAndroidUserItemSink()
{

}

CAndroidUserItemSink::~CAndroidUserItemSink()
{

}

void * __cdecl CAndroidUserItemSink::QueryInterface( const IID & Guid, DWORD dwQueryVer )
{
	return NULL;
}

bool __cdecl CAndroidUserItemSink::InitUserItemSink( IUnknownEx * pIUnknownEx )
{
	return true;
}

bool __cdecl CAndroidUserItemSink::RepositUserItemSink()
{
	return true;
}

bool __cdecl CAndroidUserItemSink::OnEventTimer( UINT nTimerID )
{
	return true;
}

bool __cdecl CAndroidUserItemSink::OnEventGameMessage( WORD wSubCmdID, void * pData, WORD wDataSize )
{
	return true;
}

bool __cdecl CAndroidUserItemSink::OnEventFrameMessage( WORD wSubCmdID, void * pData, WORD wDataSize )
{
	return true;
}

bool __cdecl CAndroidUserItemSink::OnEventGameScene( BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize )
{
	return true;
}

void __cdecl CAndroidUserItemSink::OnEventUserEnter( IAndroidUserItem * pIAndroidUserItem, bool bLookonUser )
{

}

void __cdecl CAndroidUserItemSink::OnEventUserLeave( IAndroidUserItem * pIAndroidUserItem, bool bLookonUser )
{

}

void __cdecl CAndroidUserItemSink::OnEventUserScore( IAndroidUserItem * pIAndroidUserItem, bool bLookonUser )
{

}

void __cdecl CAndroidUserItemSink::OnEventUserStatus( IAndroidUserItem * pIAndroidUserItem, bool bLookonUser )
{

}

void __cdecl CAndroidUserItemSink::OnEventUserSegment( IAndroidUserItem * pIAndroidUserItem, bool bLookonUser )
{

}