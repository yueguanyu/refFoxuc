#include "StdAfx.h"
#include "HgeGUI.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////
CHgeImage::CHgeImage(void)
{
	m_pHge = hgeCreate(HGE_VERSION);
	m_pImage = NULL;

	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_fItemWidth = 0.0f;
	m_fItemHeight = 0.0f;
}

CHgeImage::~CHgeImage(void)
{
	DestroyImage();
	if (m_pHge) 
	{
		m_pHge->Release();
		m_pHge = NULL;
	}
}


// 装载图片
bool CHgeImage::LoadImage(const char *filename, int nxNum/* = 1*/, 
						  int nyNum/* = 1*/, DWORD size/* = 0*/, bool bMipmap/* = false*/)
{
	if (NULL == m_pHge)
		return false;

	float fx = 0.0f,
		  fy = 0.0f;

	HTEXTURE  tex = m_pHge->Texture_Load(filename, size, bMipmap);
	
	fx = (float)m_pHge->Texture_GetWidth(tex,true);
	fy = (float)m_pHge->Texture_GetHeight(tex,true);

	hgeSprite* pSprite = new hgeSprite(tex, 0.0f, 0.0f, fx, fy);	

	if (NULL == pSprite)
		return false;

	if (NULL != m_pImage)
		DestroyImage();

	m_pImage = pSprite;
	m_fWidth = fx;
	m_fHeight = fy;
	m_fItemWidth = fx / nxNum;
	m_fItemHeight = fy / nyNum;

	return true;
}


// 卸载图片
void CHgeImage::DestroyImage(void)
{
	if (m_pHge && m_pImage && m_pImage->GetTexture())
	{
		m_pHge->Texture_Free(m_pImage->GetTexture());
		delete m_pImage;
		m_pImage = NULL;
	}
}

// 绘制图片
void CHgeImage::DrawImage(float x, float y)
{
	if (m_pHge && m_pImage)
		m_pImage->Render(x, y);
}


// 绘制图片
void CHgeImage::DrawImage(float x, float y, float rot, float hscale/* = 1.0f*/, float vscale/* = 0.0f*/)
{
	if (m_pHge && m_pImage)
		m_pImage->RenderEx(x, y, rot, hscale, vscale);
}


//////////////////////////////////////////////////////////////////////////
CHgeButton::CHgeButton()
{
	m_pHge = hgeCreate(HGE_VERSION);
	ZeroMemory(m_pBtSprite, sizeof(m_pBtSprite));

	m_bVisible = true;
	m_bEnable = true;
	m_bLoad = false;
	m_cbState = BT_Normal;

	m_ptBase.SetRect(0, 0, 0, 0);
}


CHgeButton::~CHgeButton()
{
	DestroyButton();
	if (m_pHge) 
	{
		m_pHge->Release();
		m_pHge = NULL;
	}
}

// 设置按钮位置
void CHgeButton::SetPos(CPoint pt)
{
	int nw = m_ptBase.Width();
	int nh = m_ptBase.Height();

	m_ptBase.left = pt.x;
	m_ptBase.top = pt.y;
	m_ptBase.right = pt.x + nw;
	m_ptBase.bottom = pt.y + nh;
}


// 获取按钮位置
CPoint CHgeButton::GetPos(void)
{
	return CPoint(m_ptBase.left, m_ptBase.top);
}

// 获取按钮大小
CSize CHgeButton::GetSize(void)
{
	return CSize(m_ptBase.Width(), m_ptBase.Height());
}


// 装载按钮资源
bool CHgeButton::LoadImage(const char *filename, DWORD size/* = 0*/, bool bMipmap/* = false*/)
{
	if (NULL == m_pHge || m_bLoad)
		return false;

	float fx = 0.0f,
		fy = 0.0f;

	HTEXTURE  tex = m_pHge->Texture_Load(filename, size, bMipmap);

	fx = (float)m_pHge->Texture_GetWidth(tex,true) / BUTTON_COUNT;
	fy = (float)m_pHge->Texture_GetHeight(tex,true);

	for (int i = 0; i < BUTTON_COUNT; ++i)
	{
		m_pBtSprite[i] = new hgeSprite(tex, i * fx, 0.0f, fx, fy);	
	}

	// 设置按钮
	m_ptBase.left = (LONG)0.0f;
	m_ptBase.right = (LONG)fx;
	m_ptBase.top = (LONG)0.0f;
	m_ptBase.bottom = (LONG)fy;

	m_bLoad = true;

	return true;
}

// 绘制按钮
void CHgeButton::DrawButton(void)
{
	if (!m_bVisible) return;
	if (NULL == m_pHge) return;

	ASSERT(m_cbState < BUTTON_COUNT);
	if (m_cbState >= BUTTON_COUNT) return;

	if (m_pBtSprite[m_cbState])
		m_pBtSprite[m_cbState]->Render((float)m_ptBase.left, (float)m_ptBase.top);
}

// 删除按钮
void CHgeButton::DestroyButton(void)
{
	if (m_pHge)
	{
		if (m_pBtSprite[0] && m_pBtSprite[0]->GetTexture())
			m_pHge->Texture_Free(m_pBtSprite[0]->GetTexture());

		for(int i = 0; i < BUTTON_COUNT; ++i)
		{
			if (m_pBtSprite[i])
			{
				delete m_pBtSprite[i];
				m_pBtSprite[i] = NULL;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CHgeButtonMgr::CHgeButtonMgr(CGameClientView* pFrameView)
{
	m_pIGameFrameView = pFrameView;

	m_bMLDown = false;
	m_bMLUP = false;
	m_ptMouse.SetPoint(0, 0);
}


CHgeButtonMgr::~CHgeButtonMgr()
{
	DelButton();
}


// 获取按钮存在
bool CHgeButtonMgr::IsHaveButton(BYTE cbID)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (m_BtArray[i].cbID == cbID) return true;
	}

	return false;
}

// 获取大小
void CHgeButtonMgr::GetButtonSize(BYTE cbID, CSize& size)
{
	size.SetSize(0, 0);

	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (m_BtArray[i].cbID == cbID &&
			NULL != m_BtArray[i].pBt)
		{
			size = m_BtArray[i].pBt->GetSize();
			return;
		}
	}
}

// 设置显示
void CHgeButtonMgr::SetButtonShow(BYTE cbID, WORD wType)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (cbID == m_BtArray[i].cbID)
		{
			// 显示
			if ((wType&TY_BTSHOW)!= 0)
			{
				if (NULL != m_BtArray[i].pBt)
					m_BtArray[i].pBt->SetVisible(true);
			}
			else if ((wType&TY_BTHIDE)!= 0)
			{
				if (NULL != m_BtArray[i].pBt)
					m_BtArray[i].pBt->SetVisible(false);
			}

			// 可用
			if ((wType&TY_BTENABLE)!= 0)
			{
				if (NULL != m_BtArray[i].pBt)
					m_BtArray[i].pBt->SetEnable(true);
			}
			else if ((wType&TY_BTUNENABLE)!= 0)
			{
				if (NULL != m_BtArray[i].pBt)
					m_BtArray[i].pBt->SetEnable(false);
			}
			break;
		}
	}
}

// 增加按钮
void CHgeButtonMgr::AddButton(BYTE cbID, const char *filename, DWORD size/* = 0*/, bool bMipmap/* = false*/)
{
	tagBtItem btItem;
	ZeroMemory(&btItem, sizeof(tagBtItem));

	btItem.cbID = cbID;
	btItem.pBt = new CHgeButton();

	if (btItem.pBt)
	{
		btItem.pBt->LoadImage(filename, size, bMipmap);
		m_BtArray.Add(btItem);
	}
}

// 删除按钮
void CHgeButtonMgr::DelButton(BYTE cbID)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (m_BtArray[i].cbID == cbID)
		{
			delete m_BtArray[i].pBt;
			m_BtArray.RemoveAt(i);
			return;
		}
	}
}

// 删除按钮
void CHgeButtonMgr::DelButton()
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
		delete m_BtArray[i].pBt;

	m_BtArray.RemoveAll();
}

// 更新按钮
void CHgeButtonMgr::UpdateButton(void)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		// 不可见
		if (!m_BtArray[i].pBt->GetVisible()) continue;

		// 不可用
		if(!m_BtArray[i].pBt->GetEnable())
		{
			m_BtArray[i].pBt->SetState(CHgeButton::BT_UnEable);
			continue;
		}

		// 正常
		m_BtArray[i].pBt->SetState(CHgeButton::BT_Normal);	

		// 检测鼠标
		bool bMouseIn = m_BtArray[i].pBt->GetInBt(m_ptMouse);

		// 鼠标左键松开
		if (m_bMLUP && bMouseIn) 
		{
			m_bMLUP = false;
			// 执行函数
			if (m_pIGameFrameView)
				m_pIGameFrameView->OnEventButton(m_BtArray[i].cbID, 0);
		}
		
		// 左键按下
		if (m_bMLDown && bMouseIn)
		{
			m_BtArray[i].pBt->SetState(CHgeButton::BT_Down);
		}
		
		// 鼠标滑过
		if (bMouseIn && !m_bMLDown && !m_bMLUP)
		{
			m_BtArray[i].pBt->SetState(CHgeButton::BT_Move);
		}
	}
}


// 绘制按钮
void CHgeButtonMgr::DrawButton(void)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (!m_BtArray[i].pBt->GetVisible()) continue;
		m_BtArray[i].pBt->DrawButton();
	}
}

// 设置位置
void CHgeButtonMgr::SetButtonPt(BYTE cbID, CPoint pt)
{
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (m_BtArray[i].cbID == cbID &&
			m_BtArray[i].pBt)
		{
			m_BtArray[i].pBt->SetPos(pt);
			break;
		}
	}
}

// 设置鼠标位置
void CHgeButtonMgr::SetMMove(CPoint pt)
{
	m_ptMouse = pt;

	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (!m_BtArray[i].pBt->GetVisible() || 
			!m_BtArray[i].pBt->GetEnable())
			continue;

		// 检测鼠标
		bool bMouseIn = m_BtArray[i].pBt->GetInBt(m_ptMouse);

		if (bMouseIn) return;
	}

	m_bMLDown = false;
}

// 设置左键点击
void CHgeButtonMgr::SetMLDown(CPoint pt)
{
	m_ptMouse = pt;
	
	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (!m_BtArray[i].pBt->GetVisible() || 
			!m_BtArray[i].pBt->GetEnable())
			continue;

		// 检测鼠标
		bool bMouseIn = m_BtArray[i].pBt->GetInBt(m_ptMouse);

		if (bMouseIn)
		{
			m_bMLDown = true;
			break;
		}
		else
		{
			m_bMLDown = false;
		}
	}
}


// 设置左键点击
void CHgeButtonMgr::SetMLUp(CPoint pt)
{
	m_ptMouse = pt;

	// 状态检测
	if(!m_bMLDown) 
	{
		m_bMLUP = false;
		return;
	}

	for (int i = 0; i < m_BtArray.GetCount(); ++i)
	{
		if (!m_BtArray[i].pBt->GetVisible() || 
			!m_BtArray[i].pBt->GetEnable())
			continue;

		// 检测鼠标
		bool bMouseIn = m_BtArray[i].pBt->GetInBt(m_ptMouse);

		if (bMouseIn)
		{
			m_bMLUP = true;
			break;
		}
		else
		{
			m_bMLUP = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CHgeSprite::CHgeSprite(HTEXTURE tex, float x, float y, float w, float h, int nItem/* = 1*/)
:hgeSprite(tex,x,y,w,h)
{
	m_fWidth = w;
	m_fHeight = h;

	m_fItemWidth = w/nItem;
	m_fItemHeight = h/nItem;

}

//////////////////////////////////////////////////////////////////////////
CHgeSound::CHgeSound(CGameClientView* pFrameView)
{
	m_pIGameFrameView = pFrameView;
	m_pHge =  hgeCreate(HGE_VERSION);
}

CHgeSound::~CHgeSound()
{
	DelSound();
	if (m_pHge) 
	{
		m_pHge->Release();
		m_pHge = NULL;
	}
}

// 增加声音
void CHgeSound::AddSound(WORD wID, const char* fileName)
{
	if (NULL == fileName) return;

	ASSERT(m_pHge != NULL);
	if (NULL == m_pHge) return;

	// 构造数据
	tagSdItem sdItem;
	ZeroMemory(&sdItem, sizeof(sdItem));
	
	sdItem.wSoundID = wID;
	sdItem.hSound = m_pHge ->Effect_Load(fileName);

	m_HgeSdArray.Add(sdItem);
}


// 删除声音
void CHgeSound::DelSound(WORD wID)
{
	ASSERT(m_pHge != NULL);
	if (NULL == m_pHge) return;

	for (int i = 0; i < m_HgeSdArray.GetCount(); ++i)
	{
		if (m_HgeSdArray[i].wSoundID == wID)
		{
			m_pHge->Effect_Free(m_HgeSdArray[i].hSound);
			m_HgeSdArray.RemoveAt(i);
			return;
		}
	}
}


// 删除声音
void CHgeSound::DelSound()
{
	ASSERT(m_pHge != NULL);
	if (NULL == m_pHge) return;

	for (int i = 0; i < m_HgeSdArray.GetCount(); ++i)
	{
		m_pHge->Effect_Free(m_HgeSdArray[i].hSound);
	}

	m_HgeSdArray.RemoveAll();
}

// 播放声音
void CHgeSound::PlayGameSound(WORD wID, bool bLoop/* = false*/)
{
	ASSERT(m_pHge != NULL);
	if (NULL == m_pHge) return;

	int nIndex = GetSoundIndex(wID);
	if (-1 == nIndex) return;

	if (!bLoop)
		m_pHge->Effect_Play(m_HgeSdArray[nIndex].hSound);
	else
		m_pHge->Effect_PlayEx(m_HgeSdArray[nIndex].hSound,100,0,1.0f,true);
}

// 判断声音存在
bool CHgeSound::IsHaveSound(WORD wID)
{
	return (-1 != GetSoundIndex(wID));
}

// 获取声音位置
int CHgeSound::GetSoundIndex(WORD wID)
{
	for (int i = 0; i < m_HgeSdArray.GetCount(); ++i)
	{
		if (m_HgeSdArray[i].wSoundID == wID)
		{
			return i;
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////
//// 构造
//CHgeFont::CHgeFont(const char* lpsFontName, int nFaceSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, BOOL bAntialias)
//{
//	m_pHGE = hgeCreate(HGE_VERSION);
//
//	// 创建GDI相关设备
//	HDC hDC = GetDC(m_pHGE->System_GetState(HGE_HWND));
//	m_hMemDC = CreateCompatibleDC(hDC);
//	if (NULL == m_hMemDC) return;
//	ReleaseDC(m_pHGE->System_GetState(HGE_HWND),hDC);
//
//	::SetMapMode(m_hMemDC, MM_TEXT);
//	::SetTextAlign(m_hMemDC, TA_TOP);
//	::SetTextColor(m_hMemDC,RGB(255,255,255));
//	::SetBkColor(m_hMemDC,RGB(0,0,0));
//
//	m_hBrush = CreateSolidBrush(RGB(0,0,0));
//	if (NULL == m_hBrush) return;
//
//	m_bItalic = bItalic;
//	m_hFont = CreateFont(
//		-nFaceSize,
//		0,
//		0,
//		0,
//		(bBold) ? FW_BOLD : FW_NORMAL,
//		bItalic,
//		bUnderline,
//		FALSE,
//		GB2312_CHARSET,
//		OUT_DEFAULT_PRECIS,
//		CLIP_DEFAULT_PRECIS,
//		(bAntialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
//		FF_DONTCARE | DEFAULT_PITCH,
//		lpsFontName);
//	if (NULL == (m_hFont)) return;
//	SelectObject(m_hMemDC, m_hFont);
//
//	HTEXTURE hTexLetter = m_pHGE->Texture_Create(tex_size,tex_size);
//	if (NULL == (hTexLetter)) return;
//
//	BITMAPINFO bmi;
//	memset(&bmi,0, sizeof(bmi));
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biBitCount = 32;
//	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biWidth = m_pHGE->Texture_GetWidth(hTexLetter);
//	bmi.bmiHeader.biHeight = -m_pHGE->Texture_GetHeight(hTexLetter);
//
//	m_pBuffer = NULL;
//	m_hBitmap = CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, &m_pBuffer, NULL, 0);
//	if (NULL == (m_hBitmap)) return;
//	SelectObject(m_hMemDC, m_hBitmap);
//
//	//
//	m_FontTextures.RemoveAll();
//	memset(m_FontGlyphs,0,sizeof(TENGINEFONTGLYPH)*font_count);
//
//	m_nFontSize = static_cast<float>(nFaceSize);
//	m_nKerningWidth = 0;
//	m_nKerningHeight = 0;
//
//	m_pSprite = new hgeSprite( 0, 0, 0, 0, 0 );
//	m_pSprite->SetColor( ARGB( 255, 255, 255, 255 ) );
//
//	memset(&m_ptLetter,0,sizeof(m_ptLetter));
//
//	m_FontTextures.Add(hTexLetter);
//	m_hTexLetter = hTexLetter;
//
//	// 构造
//	StaticCacheCharacter(L"0123456789");
//	StaticCacheCharacter(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
//	StaticCacheCharacter(L"abcdefghijklmnopqrstuvwxyz");
//	StaticCacheCharacter(L"`-=\\[];\',./~!@#$%^&*()_+|{}:\"<>?");
//}
//
//CHgeFont::~CHgeFont(void)
//{
//	for (int i = 0; i < m_FontTextures.GetCount(); ++i)
//		m_pHGE->Texture_Free(m_FontTextures[i]);
//	m_FontTextures.RemoveAll();
//
//	if ((m_hBitmap)) DeleteObject(m_hBitmap);
//	if ((m_hFont)) DeleteObject(m_hFont);
//	if ((m_hBrush)) DeleteObject(m_hBrush);
//	if ((m_hMemDC)) DeleteDC(m_hMemDC);
//
//	if (m_pSprite) { delete m_pSprite; m_pSprite = NULL; }
//	if (m_pHGE) { m_pHGE->Release(); m_pHGE = NULL; }
//}
//
//void CHgeFont::StaticCacheCharacter(const wchar_t* text)
//{
//	while(*text)
//	{
//		GetGlyphByCharacter(*text);
//		++text;
//	}
//}
bool CHgeFont::MByteToWChar(LPCSTR lpcszStr/*char*/, LPWSTR lpwszStr/*wchar_t*/, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);

	if (dwSize < dwMinSize)
	{
		return false;
	}
	MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);  

	return true;

}
//
//// 释放一个TypeFont精灵对象
//void CHgeFont::Release()
//{
//	if (this) { delete this; }
//}
//
//// 渲染文本
//void CHgeFont::Printf(float x, float y, const wchar_t *format, ...)
//{
//	va_list l;
//	va_start( l, format );
//
//	wchar_t text[10240];
//	wvsprintfW( text, format, l );
//
//	Render( x, y, text );
//}
//
//void CHgeFont::Render(float x, float y, const wchar_t* text)
//{
//	float offsetX = x;
//	float offsetY = y;
//
//	while(*text)
//	{
//		if (*text == L'\n' || *text == L'\r')
//		{
//			offsetX = x;
//			offsetY += (m_nFontSize + m_nKerningHeight);
//		}
//		else
//		{
//			unsigned int idx = GetGlyphByCharacter(*text);
//			if ( idx > 0)
//			{
//				m_pSprite->SetTexture( m_FontGlyphs[idx].t );
//				m_pSprite->SetTextureRect( m_FontGlyphs[idx].x, m_FontGlyphs[idx].y, m_FontGlyphs[idx].w, m_FontGlyphs[idx].h );
//				m_pSprite->Render(offsetX, offsetY);
//				offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
//			}
//			else
//			{
//				offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
//			}
//		}
//
//		++text;
//	}
//}
//
//void CHgeFont::RenderEx(float x, float y, const wchar_t* text, float scale)
//{
//	Render(x,y,text);
//}
//
//
//// 设置与获取颜色
//void CHgeFont::SetColor(DWORD dwColor, int i)
//{
//	m_pSprite->SetColor(dwColor,i);
//}
//
//DWORD CHgeFont::GetColor(int i)
//{
//	return m_pSprite->GetColor(i);
//}
//
//// 获取文本宽高
//SIZE CHgeFont::GetTextSize(const wchar_t* text)
//{
//	SIZE dim = {0, static_cast<LONG>(m_nFontSize)};
//	float nRowWidth = 0;
//
//	while(*text)
//	{
//		if (*text == L'\n' || *text == L'\r')
//		{
//			dim.cy += static_cast<LONG>(m_nFontSize + m_nKerningHeight);
//			if (dim.cx < static_cast<LONG>(nRowWidth))
//				dim.cx = static_cast<LONG>(nRowWidth);
//			nRowWidth = 0;
//		}
//		else
//			nRowWidth += (GetWidthFromCharacter(*text) + m_nKerningWidth);
//		++text;
//	}
//
//	if (dim.cx < static_cast<LONG>(nRowWidth))
//		dim.cx = static_cast<LONG>(nRowWidth);
//
//	return dim;
//}
//
//// 根据坐标获取字符
//wchar_t CHgeFont::GetCharacterFromPos(const wchar_t* text, float pixel_x, float pixel_y)
//{
//	float x = 0;
//	float y = 0;
//
//	while (*text)
//	{
//		if (*text == L'\n' || *text == L'\r')
//		{
//			x = 0;
//			y += (m_nFontSize+m_nKerningHeight);
//			text++;
//
//			if (!(*text))
//				break;
//		}
//
//		float w = GetWidthFromCharacter(*text);
//		if (pixel_x > x && pixel_x <= x + w &&
//			pixel_y > y && pixel_y <= y + m_nFontSize)
//			return *text;
//
//		x += (w+m_nKerningWidth);
//
//		text++;
//	}
//
//	return L'\0';
//}
//
//// 设置字间距
//void CHgeFont::SetKerningWidth(float kerning)
//{
//	m_nKerningWidth = kerning;
//}
//void CHgeFont::SetKerningHeight(float kerning)
//{
//	m_nKerningHeight = kerning;
//}
//
//// 获取字间距
//float CHgeFont::GetKerningWidth()
//{
//	return m_nKerningWidth;
//}
//float CHgeFont::GetKerningHeight()
//{
//	return m_nKerningHeight;
//}	
//
//// 字体大小
//float CHgeFont::GetFontSize()
//{
//	return m_nFontSize;
//}
//
//// 根据字符获取轮廓
//unsigned int CHgeFont::GetGlyphByCharacter(wchar_t c)
//{
//	unsigned int idx = (unsigned int)c;
//	if (NULL == (m_FontGlyphs[idx].t)) CacheCharacter(idx,c);
//	return idx;
//}
//inline float CHgeFont::GetWidthFromCharacter(wchar_t c)
//{
//	unsigned int idx = GetGlyphByCharacter(c);
//	if (idx > 0 && idx < font_count)
//		return m_FontGlyphs[idx].w;
//
//	if (c >= 0x2000)
//	{
//		return	m_nFontSize;
//	}
//	else
//	{
//		return	floorf(m_nFontSize / 2);
//
//	}
//}
//inline void CHgeFont::CacheCharacter(unsigned int idx, wchar_t c)
//{
//	if (idx < font_count && NULL == (m_FontGlyphs[idx].t))
//	{
//		wchar_t sChar[2];
//		sChar[0] = c;
//		sChar[1] = L'\0';
//
//		SIZE szChar = {0,0};
//		GetTextExtentPoint32W(m_hMemDC,sChar,1,&szChar);
//
//		int font_w = szChar.cx;
//		int font_h = szChar.cy;
//
//		if (m_ptLetter.x + font_w >= tex_size)
//		{
//			m_ptLetter.x = 0;
//			if (m_ptLetter.y + font_h >= tex_size - font_h)
//			{
//				m_ptLetter.y = 0;
//
//				HTEXTURE hTexLetter = m_pHGE->Texture_Create(tex_size,tex_size);
//				if (NULL == (hTexLetter)) return;
//				m_FontTextures.Add(hTexLetter);
//				m_hTexLetter = hTexLetter;
//
//				RECT rcFill = {0,0,tex_size,tex_size};
//				FillRect(m_hMemDC,&rcFill,m_hBrush);
//			}
//			else
//				m_ptLetter.y += font_h;
//		}
//
//		TextOutW(m_hMemDC,m_ptLetter.x,m_ptLetter.y,(LPCWSTR)sChar,1);
//
//		DWORD *target_pixels = m_pHGE->Texture_Lock(m_hTexLetter,FALSE);
//
//		if (target_pixels)
//		{
//			BYTE* pbm = (BYTE*)m_pBuffer;
//
//			int tex_w = m_pHGE->Texture_GetWidth(m_hTexLetter,true);
//			int tex_h = m_pHGE->Texture_GetHeight(m_hTexLetter,true);
//
//			// 不带背景色绘制字体
//			for (int y=0;y<tex_w;y++)
//			{
//				for (int x=0;x<tex_h;x++)
//				{
//					BYTE alpha = 0;
//
//					// 绘制字体
//					target_pixels[y*tex_w+x] = ARGB(0xFFFFFF == RGB(pbm[2],pbm[1],pbm[0])?0xFF:0x0,0xFF,0xFF,0xFF);
//
//					pbm+=4;
//				}
//			}
//		}
//
//		m_pHGE->Texture_Unlock(m_hTexLetter);
//
//		m_FontGlyphs[idx].x = static_cast<float>(m_ptLetter.x);
//		m_FontGlyphs[idx].y = static_cast<float>(m_ptLetter.y);
//		m_FontGlyphs[idx].w = static_cast<float>(font_w);
//		m_FontGlyphs[idx].h = static_cast<float>(font_h);
//		m_FontGlyphs[idx].t = m_hTexLetter;
//		m_FontGlyphs[idx].c = c;
//
//		m_ptLetter.x += font_w;
//	}
//}

__inline float _floor(float f)
{
	static int _n;
	_asm fld f
	_asm fistp _n
	return (float)_n;
}

// 65级灰度表
const unsigned char g_byAlphaLevel[65] = 
{
	0,  4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
	52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,100,
	104,108,112,116,120,124,128,132,136,140,144,148,152,
	156,160,164,168,172,176,180,184,188,192,196,200,204,
	208,212,216,220,224,228,232,236,240,244,248,252,255
};

CHgeFont::CHgeFont(const char* lpsFontName, int nFaceSize,BOOL bBold, BOOL bItalic, BOOL bAntialias)
{
	m_pHGE = hgeCreate(HGE_VERSION);

	// 创建GDI相关设备
	HDC hDC = GetDC(m_pHGE->System_GetState(HGE_HWND));
	m_hMemDC = CreateCompatibleDC(hDC);
	if (NULL == m_hMemDC) return;
	ReleaseDC(m_pHGE->System_GetState(HGE_HWND),hDC);

	::SetMapMode(m_hMemDC, MM_TEXT);
	::SetTextColor(m_hMemDC,RGB(255,255,255));
	::SetBkColor(m_hMemDC,RGB(0,0,0));

	TCHAR tstr[100];
	MultiByteToWideChar(CP_ACP, 0, lpsFontName, -1, tstr, 100);
	m_hFont = CreateFont(
		-nFaceSize,
		0,
		0,
		0,
		(bBold) ? FW_BOLD : FW_NORMAL,
		bItalic,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		tstr);
	if (NULL == (m_hFont)) return;
	SelectObject(m_hMemDC, m_hFont);

	m_nAntialias	= bAntialias ? GGO_GRAY8_BITMAP : GGO_BITMAP;

	TEXTMETRIC tm;
	::GetTextMetrics(m_hMemDC,&tm);
	m_nAscent		= tm.tmAscent;

	m_nFontSize		= static_cast<float>(nFaceSize);
	m_nKerningWidth	= 0;
	m_nKerningHeight= 0;

	m_pSprite = new hgeSprite(0, 0, 0, 0, 0);
	m_pSprite->SetColor(ARGB(255, 255, 255, 255));
}

CHgeFont::~CHgeFont(void)
{
	//释放资源
	GlyphListIterator it;
	for (it = m_GlyphList.begin(); it != m_GlyphList.end(); it++)
	{
		m_pHGE->Texture_Free((*it)->t);
		delete (*it);
	}
	m_GlyphList.clear();

	if ((m_hFont)) DeleteObject(m_hFont);
	if ((m_hMemDC)) DeleteDC(m_hMemDC);

	if(m_pSprite) delete m_pSprite;
	if(m_pHGE) m_pHGE->Release();
}

// 渲染文本
void CHgeFont::Print( float x, float y, const char *format, ... )
{
	char sBuffer[1024] = {0};
	char *lpsArg=(char*)&format+sizeof(format);
	vsprintf(sBuffer, format, lpsArg);

	Render(x,y,CA2W(sBuffer));
}

void CHgeFont::PrintCenter( float x, float y, const char *format, ... )
{
	char sBuffer[1024] = {0};
	char *lpsArg=(char*)&format+sizeof(format);
	vsprintf(sBuffer, format, lpsArg);

	wchar_t* text = CA2W(sBuffer);
	UINT nLen = (UINT)wcslen(text);
	UINT nTotalWidth = 0;
	for (UINT i=0; i < nLen;i++)
	{
		nTotalWidth += (UINT)GetWidthFromCharacter(text[i]);
	}

	x -= nTotalWidth/2;

	Render(x,y,CA2W(sBuffer));
}

void CHgeFont::Render(float x, float y, const wchar_t* text )
{
	float offsetX = x;
	float offsetY = y;

	while(*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			offsetX = x;
			offsetY += (m_nFontSize + m_nKerningHeight);
		}
		else
		{
			GetGlyphByCharacter(*text);
			GlyphListIterator it;
			for (it = m_GlyphList.begin(); it != m_GlyphList.end(); it++)
			{
				if ((*it)->wcIndex == *text)
				{
					m_pSprite->SetTexture((*it)->t);
					m_pSprite->SetTextureRect(0, 0, (*it)->w, (*it)->h);
					m_pSprite->Render(offsetX - (*it)->x, offsetY - (*it)->y);
					offsetX += (GetWidthFromCharacter(*text) + m_nKerningWidth);
				}
			}
		}

		++text;
	}
}

// 设置与获取颜色
void CHgeFont::SetColor( DWORD dwColor, int i )
{
	m_pSprite->SetColor(dwColor,i);
}

DWORD CHgeFont::GetColor(int i)
{
	return m_pSprite->GetColor(i);
}

// 获取文本宽高
SIZE CHgeFont::GetTextSize( const wchar_t* text )
{
	SIZE dim = {0, static_cast<LONG>(m_nFontSize)};
	float nRowWidth = 0;

	while(*text)
	{
		if (*text == L'\n' || *text == L'\r')
		{
			dim.cy += static_cast<LONG>(m_nFontSize + m_nKerningHeight);
			if (dim.cx < static_cast<LONG>(nRowWidth))
				dim.cx = static_cast<LONG>(nRowWidth);
			nRowWidth = 0;
		}
		else
			nRowWidth += (GetWidthFromCharacter(*text) + m_nKerningWidth);
		++text;
	}

	if (dim.cx < static_cast<LONG>(nRowWidth))
		dim.cx = static_cast<LONG>(nRowWidth);

	return dim;
}

// 设置字间距
void CHgeFont::SetKerningWidth( float kerning )
{
	m_nKerningWidth = kerning;
}
void CHgeFont::SetKerningHeight( float kerning )
{
	m_nKerningHeight = kerning;
}

// 获取字间距
float CHgeFont::GetKerningWidth()
{
	return m_nKerningWidth;
}
float CHgeFont::GetKerningHeight()
{
	return m_nKerningHeight;
}	

// 字体大小
float CHgeFont::GetFontSize()
{
	return m_nFontSize;
}

// 根据字符获取轮廓
void CHgeFont::GetGlyphByCharacter( wchar_t c )
{
	GlyphListIterator it;
	for (it = m_GlyphList.begin(); it != m_GlyphList.end(); it++)
	{
		if ((*it)->wcIndex == c)
		{
			return;
		}
	}

	CacheCharacter(c);
}

float CHgeFont::GetWidthFromCharacter(wchar_t c)
{
	GlyphListIterator it;
	for (it = m_GlyphList.begin(); it != m_GlyphList.end(); it++)
	{
		if ((*it)->wcIndex == c)
		{
			return (*it)->c;
		}
	}

	return _floor(m_nFontSize / 2);
}

void CHgeFont::CacheCharacter(wchar_t c)
{
	UINT nChar = (UINT)c;

	MAT2 mat2 = {{0,1},{0,0},{0,0},{0,1}};
	GLYPHMETRICS gm;
	DWORD nLen = ::GetGlyphOutlineW(m_hMemDC,nChar,m_nAntialias,&gm,0,NULL,&mat2);

	HTEXTURE hTex = m_pHGE->Texture_Create(gm.gmBlackBoxX,gm.gmBlackBoxY);
	if (NULL == hTex) return;

	if((signed)nLen > 0)
	{
		LPBYTE lpBuf = new BYTE[nLen];

		if (nLen == ::GetGlyphOutlineW(m_hMemDC,nChar,m_nAntialias,&gm,nLen,lpBuf,&mat2))
		{
			BYTE*	lpSrc = lpBuf;
			DWORD*	lpDst = m_pHGE->Texture_Lock(hTex,FALSE);

			if (GGO_BITMAP == m_nAntialias)
			{
				LONG nSrcPitch = (gm.gmBlackBoxX / 32 + (gm.gmBlackBoxX % 32 == 0 ? 0 : 1)) * 4;
				LONG nDstPitch = m_pHGE->Texture_GetWidth(hTex,false);

				for (UINT y = 0; y < gm.gmBlackBoxY; ++y)
				{
					for (UINT x = 0; x < gm.gmBlackBoxX; ++x)
					{
						for(UINT k = 0; k < 8; ++k)   
						{
							UINT i = 8 * x + k;
							if (i >= gm.gmBlackBoxX)
							{
								x+=7;
								break;
							}
							lpDst[i] = ((lpSrc[x] >> (7 - k)) & 1) ? 0xFFFFFFFF : 0x0;
						}
					}

					lpSrc += nSrcPitch;
					lpDst += nDstPitch;
				}
			}
			else
			{
				LONG nSrcPitch = (gm.gmBlackBoxX / 4 + (gm.gmBlackBoxX % 4 == 0 ? 0 : 1)) * 4;
				LONG nDstPitch = m_pHGE->Texture_GetWidth(hTex,false);

				for (UINT y = 0; y < gm.gmBlackBoxY; ++y)
				{
					for (UINT x = 0; x < gm.gmBlackBoxX; ++x)
					{
						lpDst[x] = ARGB(g_byAlphaLevel[lpSrc[x]],0xFF,0xFF,0xFF);
					}

					lpSrc += nSrcPitch;
					lpDst += nDstPitch;
				}
			}

			m_pHGE->Texture_Unlock(hTex);
		}

		delete lpBuf;
	}
	else
	{
		// 非正常显示字符
	}

	TENGINEFONTGLYPH * pglyph = new TENGINEFONTGLYPH;
	pglyph->t = hTex;
	pglyph->w = static_cast<float>(gm.gmBlackBoxX);
	pglyph->h = static_cast<float>(gm.gmBlackBoxY);
	pglyph->x = static_cast<float>(-gm.gmptGlyphOrigin.x);
	pglyph->y = static_cast<float>(-m_nAscent + gm.gmptGlyphOrigin.y);
	pglyph->c = static_cast<float>(gm.gmCellIncX);
	pglyph->wcIndex = c;

	m_GlyphList.push_back(pglyph);
}

//////////////////////////////////////////////////////////////////////////
CHgeText::CHgeText(int _id, float x, float y, float w, float h, CHgeFont *fnt)
{
	id = _id;
	bStatic = true;
	bVisible = true;
	bEnabled = true;
	rect.Set(x, y, x+w, y+h);

	m_tx=x;
	m_ty=y+(h-fnt->GetFontSize())/2.0f;

	m_text[0]=0;

	m_pHgeFont = fnt;
}

CHgeText::~CHgeText()
{

}

//////////////////////////////////////////////////////////////////////////
CHgeText::CHgeText(const CHgeText &go)
{
	*this = go;
}


CHgeText& CHgeText::operator= (const CHgeText &go)
{
	m_pHgeFont = go.m_pHgeFont;
	m_tx = go.m_tx;
	m_ty = go.m_ty;
	m_nAlign = go.m_nAlign;
	CopyMemory(m_text, go.m_text, sizeof(m_text));

	bEnabled = go.bEnabled;
	bStatic = go.bStatic;
	bVisible = go.bVisible;
	color = go.color;
	gui = go.gui;
	hge = go.hge;
	id = go.id;
	next = go.next;
	prev = go.prev;
	rect.Set(go.rect.x1, go.rect.y1, go.rect.x2, go.rect.y2);
	
	return *this;
}
//////////////////////////////////////////////////////////////////////////

void CHgeText::SetMode(int _align)
{
	m_nAlign=_align;
	if (m_nAlign==HGETEXT_RIGHT) m_tx=rect.x2;
	else if (m_nAlign==HGETEXT_CENTER) m_tx=(rect.x1+rect.x2)/2.0f;
	else m_tx=rect.x1;
}

void CHgeText::SetPos(int x, int y)
{
	m_tx = x;
	m_ty = y+(abs(rect.y2- rect.y1)-m_pHgeFont->GetFontSize())/2.0f;
}

void CHgeText::SetText(const char *_text)
{
	strcpy(m_text, _text);
}

void CHgeText::printf(const char *format, ...)
{
	vsprintf(m_text, format, (char *)&format+sizeof(format));
}

void CHgeText::printfEx(const char *format, ...)
{
	vsprintf(m_text, format, (char *)&format+sizeof(format));
	//sprintf(m_text,format);

	Render();
}

void CHgeText::Render()
{
	wchar_t wchar[256];
	m_pHgeFont->MByteToWChar(m_text, wchar, sizeof(wchar));
	m_pHgeFont->SetColor(color);
	m_pHgeFont->Render(m_tx, m_ty, wchar/*m_text*/);
}

//////////////////////////////////////////////////////////////////////////
CHgeLoad::CHgeLoad(void)
{
	m_pHge = hgeCreate(HGE_VERSION);
}


CHgeLoad::~CHgeLoad(void)
{
	if (m_pHge)
	{
		m_pHge->Release();
		m_pHge = NULL;
	}
}


// 装载精灵
CHgeSprite* CHgeLoad::LoadHgeSprite(const char* fileName, int nItem/* = 1*/)
{
	if (NULL == m_pHge || NULL == fileName) return NULL;

	HTEXTURE tex = m_pHge->Texture_Load(fileName);
	if (tex == 0)
	{
#ifdef _DEBUG

		FILE * pFile = fopen("c:\\errorsprite.txt","a");
		if (pFile)
		{
			fprintf(pFile,fileName);
			fprintf(pFile,"\n");
			fclose(pFile);
		}
#endif
		return NULL;

	}

	CHgeSprite* pHgeSprite = new CHgeSprite(tex, 0.0f, 0.0f, 
		m_pHge->Texture_GetWidth(tex,true), m_pHge->Texture_GetHeight(tex,true), nItem);

	return pHgeSprite;
}

