#pragma once

#ifndef SHOWNORMAL_H
#define SHOWNORMAL_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: shownormal.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "gxcrackfvf.h"

class CShowNormals
{
public:
	CShowNormals();
	~CShowNormals();

    // dwTexStage is the texture stage to use for drawing vectors, 
    //                  if 0xffffffff then the normal field will be used
    HRESULT Init(LPD3DXBASEMESH ptmDrawMesh, DWORD dwTexStage, float fLength);
    HRESULT Draw(DWORD iSubset,DWORD iVertex, LPD3DXEFFECT pfxShowNormals,
                    LPD3DXEFFECT pfxColor, DWORD dwColor,
                    D3DXMATRIX *rgmIndexedMatrices, DWORD cIndexedMatrices);

private:
    HRESULT CreateEmptyOutline();

    LPDIRECT3DDEVICE9 m_pDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

    BOOL m_bSkinning;
    BOOL m_bIndexedSkinning;
    BOOL m_bHWVertexShaders;
    float m_fScale;

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;
};

#endif
