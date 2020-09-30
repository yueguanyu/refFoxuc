#pragma once

//¸ù¾Ý»ùµã»­Í¼.. ÏÔÊ¾Æ÷ÍâÍ¼Æ¬²»»­
struct tagBASE
{
	CPoint		ptBase;		//»ùµã
	int			nWidth;		//¿í
	int			nHeight;	//¸ß
};

class CMyPngImage : public CPngImage
{
public:
	CMyPngImage(void){}
	~CMyPngImage(void){}

	//»æ»­
public:
	//»æ»­Í¼Ïñ
	bool DrawImage(CDC * pDC, tagBASE* pBase, INT nXPos, INT nYPos);
	//»æ»­Í¼Ïñ
	bool DrawImage(CDC * pDC, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc);
	//»æ»­Í¼Ïñ
	bool DrawImage(CDC * pDC, tagBASE* pBase, int nHeight, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight);

};

class CMySkinImage : public CBitImage
{
public:
	CMySkinImage(void){}
	~CMySkinImage(void){}

	//»æÍ¼º¯Êý
public:
	//»æ»­Í¼Ïñ
	BOOL BitBlt( CDC * pDestDC,  tagBASE* pBase, INT nXPos, INT nYPos );
	//Í¸Ã÷»æ»­
	bool AlphaDrawImage(CDC * pDestDC, tagBASE* pBase, INT nXPos, INT nYPos, COLORREF crTransColor);
	//Í¸Ã÷»æ»­
	bool AlphaDrawImage(CDC * pDestDC, tagBASE* pBase, INT nXPos, INT nYPos, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor);

};

class CMyD3DTexture : public CD3DTexture
{
public:
	CMyD3DTexture(void){}
	~CMyD3DTexture(void){}

	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

	//»æ»­º¯Êý
public:
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, BYTE cbAlpha);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	//»æ»­Í¼Æ¬
	bool DrawImage(CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

};
