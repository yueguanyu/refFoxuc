#include "StdAfx.h"
#include ".\myimage.h"


bool CMyPngImage::DrawImage( CDC * pDC, tagBASE* pBase, INT nXPos, INT nYPos )
{
	CRect rcImage( nXPos, nYPos, nXPos + GetWidth(), nYPos + GetHeight());
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CPngImage::DrawImage(pDC, nXPos - pBase->ptBase.x, nYPos - pBase->ptBase.y);
	}
	return false;
}

bool CMyPngImage::DrawImage( CDC * pDC, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CPngImage::DrawImage(pDC, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXScr, YSrc);
	}
	return false;
}

bool CMyPngImage::DrawImage( CDC * pDC, tagBASE* pBase, int nHeight, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CPngImage::DrawImage(pDC, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXScr, YSrc, nSrcWidth, nSrcHeight);
	}
	return false;
}

//-------------------------------------------------------------------------------------------


BOOL CMySkinImage::BitBlt( CDC * pDestDC, tagBASE* pBase, INT nXPos, INT nYPos )
{
	CRect rcImage( nXPos, nYPos, nXPos + GetWidth(), nYPos + GetHeight());
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CBitImage::BitBlt(pDestDC->GetSafeHdc(), nXPos - pBase->ptBase.x, nYPos - pBase->ptBase.y);
	}
	return false;
}

bool CMySkinImage::AlphaDrawImage( CDC * pDestDC, tagBASE* pBase, INT nXPos, INT nYPos, COLORREF crTransColor )
{
	CRect rcImage( nXPos, nYPos, nXPos + GetWidth(), nYPos + GetHeight());
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CBitImage::TransDrawImage(pDestDC, nXPos - pBase->ptBase.x, nYPos - pBase->ptBase.y, crTransColor);
	}
	return false;

}

bool CMySkinImage::AlphaDrawImage( CDC * pDestDC, tagBASE* pBase, INT nXPos, INT nYPos, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor )
{
	CRect rcImage( nXPos, nYPos, nXPos + cxDest, nYPos + cyDest);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CBitImage::TransDrawImage(pDestDC, nXPos - pBase->ptBase.x, nYPos - pBase->ptBase.y, cxDest, cyDest, xSrc, ySrc, crTransColor);
	}
	return false;
}


bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest )
{
	CRect rcImage( nXDest, nYDest, nXDest + GetWidth(), nYDest + GetHeight());
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y);
	}
	return false;
}

bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXSource, nYSource);
	}
	return false;
}

bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight);
	}
	return false;
}

bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, BYTE cbAlpha )
{
	CRect rcImage( nXDest, nYDest, nXDest + GetWidth(), nYDest + GetHeight());
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, cbAlpha);
	}
	return false;
}

bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXSource, nYSource, cbAlpha);
	}
	return false;
}

bool CMyD3DTexture::DrawImage( CD3DDevice * pD3DDevice, tagBASE* pBase, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha )
{
	CRect rcImage( nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight);
	CRect rcBack( pBase->ptBase.x, pBase->ptBase.y, pBase->ptBase.x + pBase->nWidth, pBase->ptBase.y + pBase->nHeight );
	CRect rcTemp;
	if ( IntersectRect(rcTemp, rcImage, rcBack) )
	{
		return CD3DTexture::DrawImage(pD3DDevice, nXDest - pBase->ptBase.x, nYDest - pBase->ptBase.y, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha);
	}
	return false;
}