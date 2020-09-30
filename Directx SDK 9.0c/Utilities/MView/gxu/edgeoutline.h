#pragma once

#ifndef EDGEOUTLINE_H
#define EDGEOUTLINE_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: edgeoutline.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"

class CEdgeOutline
{
public:
	CEdgeOutline();
	~CEdgeOutline();

    HRESULT Init(LPD3DXBASEMESH ptmDrawMesh, DWORD *rgdwAdjacency);
    HRESULT Draw(LPD3DXBASEMESH ptmDrawMesh, DWORD iSubset);

private:
    HRESULT CreateEmptyOutline();

    LPDIRECT3DDEVICE9 m_pDevice;
    LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

    LPDIRECT3DVERTEXDECLARATION9 m_pDeclObj;
    D3DVERTEXELEMENT9 m_pDecl[MAX_FVF_DECL_SIZE];
    DWORD m_cBytesPerVertex;

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;

    BOOL m_bEmulate32BitIndex;
};

#endif
