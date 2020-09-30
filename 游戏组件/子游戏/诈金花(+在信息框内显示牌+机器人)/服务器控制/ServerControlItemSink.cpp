#include "StdAfx.h"
#include "servercontrolitemsink.h"

//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl( BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame )
{
	CString strInfo = TEXT("\n");
	for( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem* pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		strInfo += pTableUserItem->GetNickName();
		strInfo += TEXT("\n");

		for( int j = 0; j < MAX_COUNT; ++j )
		{
			strInfo += GetCradInfo(cbHandCardData[i][j]);
		}
		strInfo += TEXT("\n");
	}

	for( int i = 0; i < GAME_PLAYER; ++i )
	{
		IServerUserItem*  pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(pTableUserItem->GetUserRight()) )
			pITableFrame->SendGameMessage(pTableUserItem, strInfo, SMT_CHAT);
	}

	int nLookonCount = 0;
	IServerUserItem* pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	while( pLookonUserItem )
	{
		if( CUserRight::IsGameCheatUser(pLookonUserItem->GetUserRight()) )
			pITableFrame->SendGameMessage(pLookonUserItem, strInfo, SMT_CHAT);

		nLookonCount++;
		pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	}

	return true;
}

//获取牌信息
CString CServerControlItemSink::GetCradInfo( BYTE cbCardData )
{
	CString strInfo;
	if( (cbCardData&LOGIC_MASK_COLOR) == 0x00 )
		strInfo += TEXT("[方块 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x10 )
		strInfo += TEXT("[梅花 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x20 )
		strInfo += TEXT("[红桃 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x30 )
		strInfo += TEXT("[黑桃 ");

	if( (cbCardData&LOGIC_MASK_VALUE) == 0x01 )
		strInfo += TEXT("A] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x02 )
		strInfo += TEXT("2] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x03 )
		strInfo += TEXT("3] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x04 )
		strInfo += TEXT("4] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x05 )
		strInfo += TEXT("5] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x06 )
		strInfo += TEXT("6] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x07 )
		strInfo += TEXT("7] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x08 )
		strInfo += TEXT("8] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x09 )
		strInfo += TEXT("9] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0A )
		strInfo += TEXT("10] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0B )
		strInfo += TEXT("J] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0C )
		strInfo += TEXT("Q] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0D )
		strInfo += TEXT("K] ");

	return strInfo;
}
