#pragma once

#ifdef _UNICODE
#define VER_ITQView INTERFACE_VERSION(1,1)
static const GUID IID_ITQView={0x12f43bfc,0xf886,0x42f5,0x0099,0xf2,0x40,0xa8,0xd8,0xf1,0x3b,0xd8};
#else
#define VER_ITQView INTERFACE_VERSION(1,1)
static const GUID IID_ITQView={0xf6f9222d,0x6e8b,0x4834,0x009d,0xa5,0x31,0x7d,0x27,0xce,0x1e,0x33};
#endif


//游戏视图接口
interface ITQView : public IUnknownEx
{
public:
	// 获取HGE指针
	virtual HGE* __cdecl GetHge(void)=NULL;

public:
	// 增加按钮
	virtual void __cdecl AddButton(BYTE cbID, const char *filename, DWORD size = 0, bool bMipmap = false)=NULL;
	// 删除按钮
	virtual void __cdecl DelButton(BYTE cbID, bool bAll = false)=NULL;
	// 设置按钮
	virtual void __cdecl SetButtonPos(BYTE cbID, CPoint pt)=NULL;
	// 设置显示
	virtual void __cdecl SetButtonShow(BYTE cbID, WORD wType)=NULL;
	// 获取大小
	virtual void __cdecl GetButtonSize(BYTE cbID, CSize& size)=NULL;
	// 按钮事件
	virtual void __cdecl OnEventButton(WORD wButtonID, BYTE cbButtonType)=NULL;

public:
	// 增加按钮
	virtual void __cdecl AddSound(WORD wID, const char* fileName)=NULL;
	// 删除声音
	virtual void __cdecl DelSound(WORD wID, bool bAll = false)=NULL;
	// 播放声音
	virtual void __cdecl PlayGameSound(WORD wID, bool bLoop = false)=NULL;
};