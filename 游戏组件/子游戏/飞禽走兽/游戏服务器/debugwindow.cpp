#include "stdafx.h"
#include "DebugWindow.h"

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

ConsoleWindow::ConsoleWindow()
{
	m_hConsole=NULL;
	m_bCreate=FALSE;
	if(AllocConsole())
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle(CONSOLE_TILE);
		SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT);
		COORD size={120,300};
		SetConsoleScreenBufferSize(m_hConsole,size);

		int nWidth= GetSystemMetrics(SM_CXFULLSCREEN);
		int nHeigh= GetSystemMetrics(SM_CYFULLSCREEN);
		SMALL_RECT rc = {nWidth/2-85,nHeigh/2-15,nWidth/2+85,nHeigh/2+15};
		SetConsoleWindowInfo(m_hConsole,true,&rc);

		m_bCreate=TRUE;
	}
	else
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		if(m_hConsole==INVALID_HANDLE_VALUE)
			m_hConsole=NULL;
	}
	if(m_hConsole)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
		if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
		{
			m_bAttrs=TRUE;
			m_OldColorAttrs = csbiInfo.wAttributes;      
		}
		else
		{
			m_bAttrs=FALSE;
			m_OldColorAttrs = 0;
		}
	}
}

ConsoleWindow::~ConsoleWindow()
{
	if(m_bCreate)
		FreeConsole();
}

BOOL ConsoleWindow::SetTile(LPCTSTR lpTile)
{
	return SetConsoleTitle(lpTile);
}

BOOL ConsoleWindow::WriteString(LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		TCHAR szTemp[500]=TEXT("");
		myprintf(szTemp, sizeof(szTemp),TEXT("%4d-%2d-%2d %2d:%2d:%2d %s \r\n"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond,lpString);
		ret=WriteConsole(m_hConsole,szTemp,_tcslen(szTemp),NULL,NULL);
	}
	return ret;
}

BOOL ConsoleWindow::WriteString(WORD Attrs,LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,Attrs);
		ret=WriteConsole(m_hConsole,lpString,DWORD(_tcslen(lpString)),NULL,NULL);
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
	}
	return ret; 
}

ConsoleWindow ConWindow;

#define MAX_BUF_LEN 4096

BOOL DBWindowTile(LPCTSTR tile)
{
	return ConWindow.SetTile(tile);
}

BOOL DBWindowWrite(LPCTSTR fmt,...)
{
		TCHAR   message[MAX_BUF_LEN];
		va_list cur_arg;
		va_start(cur_arg,fmt);
		_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
		va_end(cur_arg);
		return ConWindow.WriteString(message);
}

BOOL DBWindowWrite(WORD Attrs,LPCTSTR fmt,...)
{
TCHAR   message[MAX_BUF_LEN];
va_list cur_arg;
va_start(cur_arg,fmt);
_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
va_end(cur_arg);
return ConWindow.WriteString(Attrs,message); 
}

