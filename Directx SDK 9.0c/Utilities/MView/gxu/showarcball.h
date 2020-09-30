#pragma once

#ifndef SHOWARCBALL_H
#define SHOWARCBALL_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: shownormal.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"

class CShowArcball
{
public:
	CShowArcball();
	~CShowArcball();

    HRESULT Init(LPDIRECT3DDEVICE9 pDevice, DWORD cSegments);
    HRESULT Draw(LPD3DXEFFECT pfxColor, DWORD dwColor);

private:
    HRESULT CreateEmptyOutline();

    LPDIRECT3DDEVICE9 m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    DWORD m_cSegments;
};

#endif
