#pragma once

#ifndef ADJOUTLINE_H
#define ADJOUTLINE_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: adjoutline.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "crackdecl.h"
#include "stripoutline.h"

class CAdjacencyOutline
{
public:
	CAdjacencyOutline();
	~CAdjacencyOutline();

    HRESULT Init( LPD3DXBASEMESH ptmDrawMesh, DWORD *rgdwNeighbors);


    HRESULT Draw( LPDIRECT3DDEVICE9 pDevice, DWORD iSubset,
                    LPD3DXEFFECT pfxShowNormals, 
                    LPD3DXEFFECT pfxColor, 
                    DWORD dwColor,
                    D3DXMATRIX *rgmIndexedMatrices, DWORD cIndexedMatrices);

private:
    HRESULT CreateEmptyOutline();
    void    AppendPoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFaceCur, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendMidpoint(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, UINT iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendPointSkinned(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, UINT iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendMidpointSkinned(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, UINT iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendPointIndexSkinned(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, UINT iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);
    void    AppendMidpointIndexSkinned(CD3DXCrackDecl1 &cdMesh, PUINT pwFace1, UINT iEdge, PBYTE pvMeshPoints, PBYTE pvLinePoints, UINT &iCurPoint);

    DXCrackFVF  m_cfvf;
    BOOL m_bSkinning;
    BOOL m_bIndexSkinning;
    BOOL m_bHWVertexShaders;

    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;
};

#endif
