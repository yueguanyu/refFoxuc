#pragma once

#ifndef STRIPOUTLINE_H
#define STRIPOUTLINE_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"
#include "crackdecl.h"

struct SLine 
{
    WORD m_iFirstPoint;
    WORD m_iSecondPoint;
};

class CStripOutline
{
public:
	CStripOutline();
	~CStripOutline();

    HRESULT Init( LPD3DXBASEMESH ptmDrawMesh);


    HRESULT Draw( LPDIRECT3DDEVICE9 pDevice, DWORD iSubset,
                    LPD3DXEFFECT pfxShowNormals, 
                    LPD3DXEFFECT pfxColor, 
                    DWORD dwColor,
                    D3DXMATRIX *rgmIndexedMatrices, DWORD cIndexedMatrices);

private:
    HRESULT CreateEmptyOutline();
    void    AppendPoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFaceCur, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendMidpoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, PUINT pwFace2, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendSkinnedPoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFaceCur, DWORD iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendSkinnedMidpoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, PUINT pwFace2, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendIndexSkinnedPoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFaceCur, DWORD iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendIndexSkinnedMidpoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, PUINT pwFace2, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);

    DXCrackFVF  m_cfvf;
    BOOL m_bSkinning;
    BOOL m_bIndexSkinning;
    BOOL m_bHWVertexShaders;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;

    UINT        m_cLines;
    UINT        m_cVertices;
};

#endif
