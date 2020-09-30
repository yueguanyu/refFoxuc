#ifndef _DBWINDOW_
#define _DBWINDOW_

#include <windows.h>

//控制台输出时的文本颜色
#define WDS_T_RED    FOREGROUND_RED
#define WDS_T_GREEN  FOREGROUND_GREEN
#define WDS_T_BLUE   FOREGROUND_BLUE
//控制台输出时的文本背景颜色
#define WDS_BG_RED   BACKGROUND_RED
#define WDS_BG_GREEN BACKGROUND_GREEN
#define WDS_BG_BLUE   BACKGROUND_BLUE

//设置控制台输出窗口标题
BOOL DBWindowTile(LPCTSTR tile);
//格式化文本输出
BOOL DBWindowWrite(LPCTSTR fmt,...);
//带颜色格式化文本输出
BOOL DBWindowWrite(WORD Attrs,LPCTSTR fmt,...);

#else
#define DBWindowTile
#define DBWindowWrite
#endif


#ifndef _UNICODE
#define myprintf	_snprintf
#else
#define myprintf	swprintf
#endif

#define CONSOLE_TILE _T("飞禽走兽服务端调试窗口")

class ConsoleWindow 
{
public:
	ConsoleWindow();
	virtual ~ConsoleWindow();

	BOOL SetTile(LPCTSTR lpTile);
	BOOL WriteString(LPCTSTR lpString);
	BOOL WriteString(WORD Attrs,LPCTSTR lpString);
private:
	HANDLE m_hConsole;
	BOOL   m_bCreate;
	BOOL   m_bAttrs;
	WORD   m_OldColorAttrs; 
};



