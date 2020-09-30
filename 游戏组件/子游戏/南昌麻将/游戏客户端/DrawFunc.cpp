#include "StdAfx.h"
#include ".\drawfunc.h"

inline BYTE GetRed(COLORREF clr)
{
	return (BYTE)(clr & 0x000000FF);
}
inline BYTE GetGreen(COLORREF clr)
{
	return (BYTE)((clr >> 8)  & 0x000000FF);
}
inline BYTE GetBlue(COLORREF clr)
{
	return (BYTE)((clr >> 16)  & 0x000000FF);
}

CDrawFunc::CDrawFunc(void)
{
}

bool CDrawFunc::IsValidRect(LPRECT lpRect)
{
	if(lpRect == 0)
	{
		return false;
	}
	if(lpRect->right < lpRect->left
		|| lpRect->bottom < lpRect->top)
	{
		return false;
	}

	return true;
}

bool CDrawFunc::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
	ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
	if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
	{
		return false;
	}
	//È«Í¸Ã÷
	if(fAlpha == 0.0f)
	{
		return true;
	}

	for(LONG l=lpRect->top-1; l<lpRect->bottom-1; l++)
	{
		for(LONG k=lpRect->left-1; k<lpRect->right-1; k++)
		{
			COLORREF clrBk = pDC->GetPixel(k, l);
			COLORREF clrBlend = RGB(GetRed(clrBk)*(1-fAlpha)+GetRed(clr)*fAlpha, 
									GetGreen(clrBk)*(1-fAlpha)+GetGreen(clr)*fAlpha,
									GetBlue(clrBk)*(1-fAlpha)+GetBlue(clr)*fAlpha);
			pDC->SetPixel(k, l, clrBlend);
		}
	}
	
	return true;
}




