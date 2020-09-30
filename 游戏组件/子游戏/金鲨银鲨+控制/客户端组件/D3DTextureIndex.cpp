#include "StdAfx.h"
#include ".\d3dtextureindex.h"

CD3DTextureIndex::CD3DTextureIndex(void)
{
	m_nWidthNumber = 0;
	m_nHeightNumber = 0;
}

CD3DTextureIndex::~CD3DTextureIndex(void)
{

}

//º”‘ÿ≈–∂œ
bool CD3DTextureIndex::IsNull()
{
	return CD3DTexture::IsNull();
}

//º”‘ÿŒ∆¿Ì
bool CD3DTextureIndex::LoadImage( CD3DDevice * pD3DDevice, HINSTANCE hInstance, LPCTSTR pszResource, DWORD dwColorKey, int nWidthNumber, int nHeightNumber )
{
	//Ãıº˛≈–∂œ
	ASSERT(nWidthNumber > 0 && nHeightNumber>0);

	//…Ë÷√±‰¡ø
	m_nWidthNumber = nWidthNumber;
	m_nHeightNumber = nHeightNumber;

	//‘ÿ»ÎÕº∆¨
	return CD3DTexture::LoadImage( pD3DDevice, hInstance, pszResource, dwColorKey );
}

//º”‘ÿŒ∆¿Ì
bool CD3DTextureIndex::LoadImage( CD3DDevice * pD3DDevice, HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, int nWidthNumber, int nHeightNumber )
{
	//Ãıº˛≈–∂œ
	ASSERT(nWidthNumber > 0 && nHeightNumber>0);

	//…Ë÷√±‰¡ø
	m_nWidthNumber = nWidthNumber;
	m_nHeightNumber = nHeightNumber;

	//‘ÿ»ÎÕº∆¨
	return CD3DTexture::LoadImage( pD3DDevice, hInstance, pszResource, pszTypeName );
}

////º”‘ÿŒ∆¿Ì
//bool CD3DTextureIndex::LoadImage( CD3DDevice * pD3DDevice, LPCTSTR pszTextureName, BOOL bCreateCache, int nWidthNumber, int nHeightNumber )
//{
//	//Ãıº˛≈–∂œ
//	ASSERT(nWidthNumber > 0 && nHeightNumber>0);
//
//	//…Ë÷√±‰¡ø
//	m_nWidthNumber = nWidthNumber;
//	m_nHeightNumber = nHeightNumber;
//
//	//‘ÿ»ÎÕº∆¨
//	return CD3DTexture::LoadImage( pD3DDevice, pszTextureName, bCreateCache );
//}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nIndex )
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex)
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight )
{
	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, BYTE cbAlpha, int nIndex )
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, cbAlpha );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource, cbAlpha );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex)
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, cbAlpha );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha )
{
	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nIndex )
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex)
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight )
{
	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, BYTE cbAlpha, int nIndex )
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource, cbAlpha );

}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex)
{
	//≥¨π˝À˜“˝ ª≠»´Õº
	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
		return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );

	//…Ë÷√±‰¡ø
	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
	int nSourceWidth = GetWidth();
	int nSourceHeight = GetHeight();

	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );

}

//ªÊª≠Õº∆¨
bool CD3DTextureIndex::DrawImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha )
{
	//ª≠Õº
	return CD3DTexture::DrawImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
}

////-------------------------------------------------------------------------------------
////¬À…´∫Ø ˝
////
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nIndex )
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest,nIndex );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource,nIndex );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex)
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest,nIndex );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight )
//{
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, BYTE cbAlpha, int nIndex )
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, cbAlpha );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource, cbAlpha );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage(CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex)
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, cbAlpha );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
//
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha )
//{
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nIndex )
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest,nIndex);
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource );
//
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nIndex)
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest,nIndex);
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
//
//}
//
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight )
//{
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight );
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, BYTE cbAlpha, int nIndex )
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nSourceWidth, nSourceHeight, nXSource, nYSource, cbAlpha,nIndex );
//
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage(CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, BYTE cbAlpha, int nIndex)
//{
//	//≥¨π˝À˜“˝ ª≠»´Õº
//	if ( nIndex >= m_nWidthNumber * m_nHeightNumber )
//		return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, cbAlpha );
//
//	//…Ë÷√±‰¡ø
//	int nXSource = (nIndex%m_nWidthNumber) * GetWidth();
//	int nYSource = (nIndex/m_nWidthNumber) * GetHeight();
//	int nSourceWidth = GetWidth();
//	int nSourceHeight = GetHeight();
//
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
//
//}
//
////ªÊª≠¬À…´Õº∆¨
//bool CD3DTextureIndex::DrawColourFilterImage( CD3DDevice * pD3DDevice, CPoint ptRotationOffset, FLOAT fRadian, CHAR chDirection, int nXDest, int nYDest, int nDestWidth, int nDestHeight, int nXSource, int nYSource, int nSourceWidth, int nSourceHeight, BYTE cbAlpha )
//{
//	//ª≠Õº
//	return CD3DTexture::DrawColourFilterImage( pD3DDevice, ptRotationOffset, fRadian, chDirection, nXDest, nYDest, nDestWidth, nDestHeight, nXSource, nYSource, nSourceWidth, nSourceHeight, cbAlpha );
//}