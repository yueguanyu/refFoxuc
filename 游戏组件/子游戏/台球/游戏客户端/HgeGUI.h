#pragma once


#include <math.h>
#include <list>


//////////////////////////////////////////////////////////////////////////
#define TY_BTSHOW		0x0001					// 显示
#define TY_BTHIDE		0x0002					// 隐藏
#define TY_BTENABLE		0x0004					// 可用
#define TY_BTUNENABLE	0x0008					// 不可用

// 类声明
class CGameClientView;
//////////////////////////////////////////////////////////////////////////
// 图片类
class CHgeImage
{
public:
	CHgeImage(void);
	virtual ~CHgeImage(void);

	// 内部数据
private:
	HGE*								m_pHge;
	hgeSprite*							m_pImage;

	// 图片数据
private:
	float								m_fWidth;
	float								m_fHeight;
	float								m_fItemWidth;
	float								m_fItemHeight;

	// 辅助函数
public:
	// 获取数据
	float GetWidth(void) { return m_fWidth; }
	// 获取数据
	float GetHeight(void) { return m_fHeight; }
	// 获取数据
	float GetItemWidth(void) { return m_fItemWidth; }
	// 获取数据
	float GetItemHeight(void) { return m_fItemHeight; }

	// 功能函数
public:
	// 装载图片
	bool LoadImage(const char *filename, int nxNum = 1, int nyNum = 1, DWORD size = 0, bool bMipmap = false);
	// 卸载图片
	void DestroyImage(void);
	// 绘制图片
	void DrawImage(float x, float y);
	// 绘制图片
	void DrawImage(float x, float y, float rot, float hscale = 1.0f, float vscale = 0.0f);
};

//////////////////////////////////////////////////////////////////////////
const int BUTTON_COUNT = 4;

// 按钮类 
class CHgeButton
{
public:
	CHgeButton();
	virtual ~CHgeButton();

	// 状态
public:
	enum { BT_Normal = 0, BT_Move, BT_Down, BT_UnEable };

	// 控制状态
private:
	bool							m_bLoad;
	bool							m_bVisible;
	bool							m_bEnable;
	BYTE							m_cbState;

	// 显示状态
private:
	HGE*							m_pHge;
	hgeSprite*						m_pBtSprite[BUTTON_COUNT];

	// 区域控制
private:
	CRect							m_ptBase;

	// 辅助函数
public:
	// 设置可见
	void SetVisible(bool b) { m_bVisible = b; }
	// 设置可用
	void SetEnable(bool b) { m_bEnable = b; }
	// 获取可见
	bool GetVisible(void) { return m_bVisible; }
	// 获取可用
	bool GetEnable(void) { return m_bEnable; }
	// 设置状态
	void SetState(BYTE cb) { m_cbState = cb; }
	// 获取状态
	BYTE GetState(void) { return m_cbState; }
	// 设置按钮位置
	void SetPos(CPoint pt);
	// 获取按钮位置
	CPoint GetPos(void);
	// 获取按钮大小
	CSize GetSize(void);
	// 在按钮中
	bool GetInBt(CPoint pt) { return m_ptBase.PtInRect(pt); }
	// 功能函数
public:
	// 装载按钮资源
	bool LoadImage(const char *filename, DWORD size = 0, bool bMipmap = false);
	// 绘制按钮
	void DrawButton(void);
	// 删除按钮
	void DestroyButton(void);
};

//////////////////////////////////////////////////////////////////////////
struct tagBtItem
{
	CHgeButton* pBt;
	BYTE		cbID;
};

typedef CWHArray<tagBtItem> HgeBtArray;

// 按钮控制
class CHgeButtonMgr
{
public:
	CHgeButtonMgr(CGameClientView* pFrameView);
	virtual ~CHgeButtonMgr();

	// 按钮数组
private:
	CGameClientView*				m_pIGameFrameView;
	HgeBtArray						m_BtArray;

	// 鼠标状态
private:
	bool							m_bMLDown;
	bool							m_bMLUP;
	CPoint							m_ptMouse;

	// 功能函数
public:
	// 增加按钮
	void AddButton(BYTE cbID, const char *filename, DWORD size = 0, bool bMipmap = false);
	// 删除按钮
	void DelButton(BYTE cbID);
	// 删除按钮
	void DelButton(void);
	// 更新按钮
	void UpdateButton(void);
	// 绘制按钮
	void DrawButton(void);
	// 设置位置
	void SetButtonPt(BYTE cbID, CPoint pt);
	// 设置鼠标位置
	void SetMMove(CPoint pt);
	// 设置左键点击
	void SetMLDown(CPoint pt);
	// 设置左键点击
	void SetMLUp(CPoint pt);
	// 获取按钮存在
	bool IsHaveButton(BYTE cbID);
	// 设置显示
	void SetButtonShow(BYTE cbID, WORD wType);
	// 获取大小
	void GetButtonSize(BYTE cbID, CSize& size);
};

//////////////////////////////////////////////////////////////////////////
// 精灵类
class CHgeSprite : public hgeSprite
{
	// 构造
public:
	CHgeSprite(HTEXTURE tex, float x, float y, float w, float h, int nItem = 1);

	// 图片数据
private:
	float								m_fWidth;
	float								m_fHeight;
	float								m_fItemWidth;
	float								m_fItemHeight;

	// 辅助函数
public:
	// 获取数据
	float GetWidth(void) { return m_fWidth; }
	// 获取数据
	float GetHeight(void) { return m_fHeight; }
	// 获取数据
	float GetItemWidth(void) { return m_fItemWidth; }
	// 获取数据
	float GetItemHeight(void) { return m_fItemHeight; }
};

//////////////////////////////////////////////////////////////////////////
struct tagSdItem
{
	HEFFECT hSound;
	WORD	wSoundID;
};

typedef CWHArray<tagSdItem> HgeSdArray;

// 声音类
class CHgeSound
{
	// 构造
public:
	CHgeSound(CGameClientView* pFrameView);
	virtual~CHgeSound();

	// 内部数据
protected:
	CGameClientView*				m_pIGameFrameView;
	HgeSdArray						m_HgeSdArray;
	HGE*							m_pHge;

	// 功能函数
public:
	// 增加声音
	void AddSound(WORD wID, const char* fileName);
	// 删除声音
	void DelSound(WORD wID);
	// 删除声音
	void DelSound();
	// 播放声音
	void PlayGameSound(WORD wID, bool bLoop = false);
	// 判断声音存在
	bool IsHaveSound(WORD wID);

	// 辅助函数
private:
	// 获取声音位置
	int GetSoundIndex(WORD wID);
};

//////////////////////////////////////////////////////////////////////////
// 精灵字体结构
typedef struct tagEngineFontGlyph
{
	float		x;
	float		y;
	float		w;
	float		h;
	HTEXTURE	t;
	wchar_t		c;
}TENGINEFONTGLYPH;
typedef CWHArray<HTEXTURE> HgeTextureArray;

// 文字类
class CHgeFont
{
public:
	//CHgeFont(const char* lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);

	CHgeFont(
		const char* lpsFontName = "新宋体",
		int nFaceSize=12,
		BOOL bBold = FALSE,
		BOOL bItalic = FALSE,
		BOOL bAntialias = FALSE);
	~CHgeFont(void);

public:
	// 渲染文本
	virtual void	Print( float x, float y, const char *format, ... );
	virtual void	PrintCenter( float x, float y, const char *format, ... );
	virtual void	Render(float x, float y, const wchar_t* text );

	// 设置与获取颜色
	virtual void	SetColor( DWORD dwColor, int i = -1 );
	virtual DWORD	GetColor( int i = 0 );

	// 获取文本宽高
	virtual SIZE	GetTextSize( const wchar_t* text );

	// 设置字间距
	virtual void	SetKerningWidth( float kerning );
	virtual void	SetKerningHeight( float kerning );

	// 获取字间距
	virtual float	GetKerningWidth();
	virtual float	GetKerningHeight();

	// 字体大小
	virtual float	GetFontSize();

public:
	// 根据字符获取轮廓
	void		 	GetGlyphByCharacter( wchar_t c );
	float			GetWidthFromCharacter(wchar_t c);
	void			CacheCharacter(wchar_t c);
	//char wchar_t转换函数
	bool			MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);

private:
	typedef struct tagEngineFontGlyph
	{
		HTEXTURE	t;
		float		w;
		float		h;
		float		x;
		float		y;
		float		c;
		wchar_t		wcIndex;
	}TENGINEFONTGLYPH;

	typedef std::list<TENGINEFONTGLYPH *> GlyphList;
	typedef GlyphList::iterator GlyphListIterator;
	GlyphList			m_GlyphList;

	UINT				m_nAntialias;//反锯齿
	LONG				m_nAscent;//基线
	DWORD				m_dwFontColor;
	float				m_nFontSize;
	float				m_nKerningWidth;
	float				m_nKerningHeight;

	HGE*				m_pHGE;
	hgeSprite*			m_pSprite;

	// GDI设备
	HDC					m_hMemDC;
	HFONT				m_hFont;
};

//////////////////////////////////////////////////////////////////////////
// 文本类
class CHgeText : public hgeGUIObject
{
public:
	CHgeText(int id, float x, float y, float w, float h, CHgeFont *fnt);
	virtual ~CHgeText();

	// 虚函数的实现
protected:
	CHgeText(const CHgeText &go);
	CHgeText&	operator= (const CHgeText &go);
	
	// 字体函数
private:
	CHgeFont*					m_pHgeFont;
	float						m_tx;
	float						m_ty;
	int							m_nAlign;
	char						m_text[256];

	// 功能函数
public:
	void			SetMode(int _align);
	void			SetPos(int x, int y);
	void			SetText(const char *_text);
	void			printf(const char *format, ...);
	void			printfEx(const char *format, ...);
	virtual void	Render();	
};


//////////////////////////////////////////////////////////////////////////
// 装载类
class CHgeLoad
{
public:
	CHgeLoad(void);
	virtual ~CHgeLoad(void);

	// HGE
private:
	HGE*							m_pHge;

	// 功能函数
public:
	// 装载精灵
	CHgeSprite* LoadHgeSprite(const char* fileName, int nItem = 1);
};