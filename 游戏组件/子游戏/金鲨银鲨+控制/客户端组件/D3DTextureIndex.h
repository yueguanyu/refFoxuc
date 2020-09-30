#pragma once

class CD3DTextureIndex : protected CD3DTexture
{
	//ÐÅÏ¢±äÁ¿
protected:
	int							m_nWidthNumber;				//ºáÊýÁ¿
	int							m_nHeightNumber;			//ÁÐÊýÁ¿

	//¹¹Ôìº¯Êý
public:
	CD3DTextureIndex(void);
	~CD3DTextureIndex(void);

	//Í¼Æ¬ÐÅÏ¢
public:
	//¼ÓÔØÅÐ¶Ï
	bool IsNull();
	//ÎÆÀí¿í¶È
	int GetWidth() { return m_ImageSize.cx/m_nWidthNumber; }
	//ÎÆÀí¸ß¶È
	int GetHeight() { return m_ImageSize.cy/m_nHeightNumber; }

	//¹¦ÄÜº¯Êý
public:
	//¼ÓÔØÎÆÀí
	bool LoadImage(CD3DDevice * pD3DDevice, HINSTANCE hInstance, LPCTSTR pszResource, DWORD dwColorKey, int nWidthNumber, int nHeightNumber );
	//¼ÓÔØÎÆÀí
	bool LoadImage(CD3DDevice * pD3DDevice, HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, int nWidthNumber, int nHeightNumber);
	////¼ÓÔØÎÆÀí
	//bool LoadImage(CD3DDevice * pD3DDevice, LPCTSTR pszTextureName, BOOL bCreateCache, int nWidthNumber, int nHeightNumber);

	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);

	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, BYTE cbAlpha, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha);

	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);


	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, BYTE cbAlpha, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha);


//	//-------------------------------------------------------------------------------------
//	//ÂËÉ«º¯Êý
//public:
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);
//
//	//»æ»­º¯Êý
//public:
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, BYTE cbAlpha, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha);
//
//	//»æ»­º¯Êý
//public:
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight);
//
//	//»æ»­º¯Êý
//public:
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, BYTE cbAlpha, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex);
//	//»æ»­ÂËÉ«Í¼Æ¬
//	bool DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha);

};
