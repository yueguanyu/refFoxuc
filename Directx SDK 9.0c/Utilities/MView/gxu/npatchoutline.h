#pragma once

#ifndef NPATCHOUTLINE_H
#define NPATCHOUTLINE_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: edgeoutline.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"

class CNPatchOutline
{
public:
	CNPatchOutline();
	~CNPatchOutline();

    HRESULT Init(LPD3DXBASEMESH ptmDrawMesh, DWORD cTesselateSegments);
    HRESULT Draw(LPD3DXBASEMESH ptmDrawMesh, DWORD iSubset);
    HRESULT DrawFace(LPD3DXBASEMESH ptmDrawMesh, DWORD iFace, DWORD iSubset);

    HRESULT CreateEmptyOutline();
private:
    HRESULT DrawInternal(LPD3DXBASEMESH ptmDrawMesh, DWORD iSubset, DWORD iStartLine, DWORD cLines);

    LPDIRECT3DDEVICE9 m_pDevice;
    LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

    LPDIRECT3DVERTEXDECLARATION9 m_pDeclObj;
    D3DVERTEXELEMENT9 m_pDecl[MAX_FVF_DECL_SIZE];
    DWORD m_cBytesPerVertex;

    DWORD m_cLinesPerOrigFace;
    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;

    BOOL m_bEmulate32BitIndex;
};

#endif
