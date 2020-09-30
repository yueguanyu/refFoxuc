/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"

#include "EdgeOutline.h"

CEdgeOutline::CEdgeOutline()
  :m_pDevice(NULL),
    m_pIndexBuffer(NULL),
    m_rgaeAttributeTable(NULL),
    m_caeAttributeTable(0),
    m_pDeclObj(NULL),
    m_bEmulate32BitIndex(FALSE)
{
}

CEdgeOutline::~CEdgeOutline()
{
    GXRELEASE(m_pIndexBuffer);
    GXRELEASE(m_pDevice);
    GXRELEASE(m_pDeclObj);

    delete []m_rgaeAttributeTable;
}

static inline DWORD FindNeighbor
    (
    CONST DWORD *pdwAdjacency, 
    DWORD iFace
    )
{
    if (pdwAdjacency[0] == iFace)
        return 0;
    else if (pdwAdjacency[1] == iFace)
        return 1;
    else 
        return 2;
}

template<class UINT_IDX>
BOOL BCrease(DWORD iFace, DWORD iPoint, DWORD *rgdwAdjacency, UINT_IDX *pwFaceIndices)
{
    DWORD iNeighbor;
    DWORD iEdge;

    GXASSERT(iPoint < 3);

    iNeighbor = rgdwAdjacency[(iFace * 3) + iPoint];
    if (iNeighbor == UNUSED32)
        return TRUE;

    GXASSERT(rgdwAdjacency[(iFace * 3) + iPoint] != UNUSED32);

    iEdge = FindNeighbor(&rgdwAdjacency[iNeighbor * 3], iFace);

    if ((pwFaceIndices[iNeighbor * 3 + iEdge] == pwFaceIndices[iFace * 3 + ((iPoint+1)%3)])
        && (pwFaceIndices[iNeighbor * 3 +((iEdge+1)%3)] == pwFaceIndices[iFace * 3 + iPoint]))
    {
        return FALSE;
    }
    else 
    {
        return TRUE;
    }
}


HRESULT
CEdgeOutline::Init(ID3DXBaseMesh *ptmMesh, DWORD *rgdwAdjacency)
{
    HRESULT hr = S_OK;
    BOOL b16BitIndex;
    DWORD cBytesPerIndex;
    DWORD iPoint;
    DWORD iFace;
    D3DXATTRIBUTERANGE *rgaeAttributeTableMesh = NULL;
    PBYTE pbFaceIndices = NULL;
    PBYTE pbLineIndices = NULL;
    PBYTE pbSrc;
    PBYTE pbDest;
    DWORD iLineOffset;
    DWORD iAttr;
    DWORD dwNeighbor;
    DWORD iFaceEnd;
    BOOL bCrease;
    DWORD cFacesCur;
    D3DCAPS9 Caps;

    // is the mesh 16 bit?
    b16BitIndex = !(ptmMesh->GetOptions() & D3DXMESH_32BIT);
    cBytesPerIndex = (b16BitIndex ? 2:4);

    hr = CreateEmptyOutline();
    if (FAILED(hr))
        goto e_Exit;

    ptmMesh->GetDeclaration(m_pDecl);
    m_cBytesPerVertex = D3DXGetDeclVertexSize(m_pDecl, 0);

    ptmMesh->GetDevice(&m_pDevice);

    hr = m_pDevice->CreateVertexDeclaration(m_pDecl, &m_pDeclObj);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->GetAttributeTable(NULL, &m_caeAttributeTable);
    if (FAILED(hr))
        goto e_Exit;

    // check for no attribute table and/or no adjacency
    if ((m_caeAttributeTable == 0) || (rgdwAdjacency == NULL))
    {
        // not attribute sorted!  just return for now
        goto e_Exit;
    }

    rgaeAttributeTableMesh = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
    m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
    if ((rgaeAttributeTableMesh == NULL) || (rgaeAttributeTableMesh == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = ptmMesh->GetAttributeTable(rgaeAttributeTableMesh, NULL);
    if (FAILED(hr))
        goto e_Exit;

    // now initialize the edge attribute table
    for (iAttr = 0; iAttr < m_caeAttributeTable; iAttr++)
    {
        m_rgaeAttributeTable[iAttr] = rgaeAttributeTableMesh[iAttr];
        m_rgaeAttributeTable[iAttr].FaceStart = 0;
        m_rgaeAttributeTable[iAttr].FaceCount = 0;
    }

    ptmMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbFaceIndices);

    for (iAttr = 0; iAttr < m_caeAttributeTable; iAttr++)
    {
        // first figure out the counts for the number of creases per attribute group
        iFaceEnd = rgaeAttributeTableMesh[iAttr].FaceStart + rgaeAttributeTableMesh[iAttr].FaceCount;
        for (iFace = rgaeAttributeTableMesh[iAttr].FaceStart; iFace < iFaceEnd; iFace++)
        {
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                dwNeighbor = rgdwAdjacency[iFace * 3 + iPoint];

                // if the neigbor is less than the current face, we have already looked at this edge
                //    if unused, just skip... not a crease
                if (dwNeighbor < iFace)
                    continue;

                if (b16BitIndex)
                {
                    bCrease = BCrease<UINT16>(iFace, iPoint, rgdwAdjacency, (UINT16*)pbFaceIndices);
                }
                else
                {
                    bCrease = BCrease<UINT32>(iFace, iPoint, rgdwAdjacency, (UINT32*)pbFaceIndices);
                }

                if (bCrease)
                {
                    // add one to the count of creases for this attribute
                    m_rgaeAttributeTable[iAttr].FaceCount += 1;
                }
            }
        }
    }

    // now setup the table to have the correct offsets into the index buffer
    //   now that we have calculated the FaceCount field (calc the FaceStarts...)
    cFacesCur = 0;
    for (iAttr = 0; iAttr < m_caeAttributeTable; iAttr++)
    {
        m_rgaeAttributeTable[iAttr].FaceStart = cFacesCur;
        cFacesCur += m_rgaeAttributeTable[iAttr].FaceCount;

        // reset the face count, to be used in the second pass as current position in buffer
        //   will become the count by the end of the pass
        m_rgaeAttributeTable[iAttr].FaceCount = 0;
    }

    // just exit if there are no creases
    if (cFacesCur == 0)
        goto e_Exit;

    m_pDevice->GetDeviceCaps(&Caps);

    // if there are too many vertices, or the max vertex index is below 16bit (means no 32 bit support)
    //    then we need to emulate
    if (!b16BitIndex && ((Caps.MaxVertexIndex < ptmMesh->GetNumVertices()) || (Caps.MaxVertexIndex <= 0xffff) || (Caps.MaxPrimitiveCount < cFacesCur)))
    {
        m_bEmulate32BitIndex = TRUE;
    }
    else
    {
        m_bEmulate32BitIndex = FALSE;
    }

    hr = m_pDevice->CreateIndexBuffer(cFacesCur * 2 * cBytesPerIndex, D3DUSAGE_SOFTWAREPROCESSING, 
                                        (b16BitIndex ? D3DFMT_INDEX16:D3DFMT_INDEX32),
                                        D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    if (FAILED(hr))
        goto e_Exit;

    m_pIndexBuffer->Lock(0, 0, (PVOID*)&pbLineIndices, D3DLOCK_NOSYSLOCK);


    // next go through and fill the index buffer
    for (iAttr = 0; iAttr < m_caeAttributeTable; iAttr++)
    {
        // first figure out the counts for the number of creases per attribute group
        iFaceEnd = rgaeAttributeTableMesh[iAttr].FaceStart + rgaeAttributeTableMesh[iAttr].FaceCount;
        for (iFace = rgaeAttributeTableMesh[iAttr].FaceStart; iFace < iFaceEnd; iFace++)
        {
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                dwNeighbor = rgdwAdjacency[iFace * 3 + iPoint];

                // if the neigbor is less than the current face, we have already looked at this edge
                //    if unused, just skip... not a crease
                if (dwNeighbor < iFace)
                    continue;

                if (b16BitIndex)
                {
                    bCrease = BCrease<UINT16>(iFace, iPoint, rgdwAdjacency, (UINT16*)pbFaceIndices);
                }
                else
                {
                    bCrease = BCrease<UINT32>(iFace, iPoint, rgdwAdjacency, (UINT32*)pbFaceIndices);
                }

                if (bCrease)
                {
                    iLineOffset = m_rgaeAttributeTable[iAttr].FaceStart + m_rgaeAttributeTable[iAttr].FaceCount;
                    m_rgaeAttributeTable[iAttr].FaceCount += 1;

                    pbSrc  = pbFaceIndices + ((iFace * 3 + iPoint) * cBytesPerIndex);
                    pbDest = pbLineIndices + iLineOffset * 2 * cBytesPerIndex;
                    memcpy(pbDest, pbSrc, cBytesPerIndex);
                
                    pbSrc  = pbFaceIndices + ((iFace * 3 + ((iPoint+1)%3)) * cBytesPerIndex);
                    pbDest += cBytesPerIndex;
                    memcpy(pbDest, pbSrc, cBytesPerIndex);                
                }
            }
        }
    }

e_Exit:
    delete []rgaeAttributeTableMesh;

    if (pbFaceIndices != NULL)
        ptmMesh->UnlockIndexBuffer();

    if (pbLineIndices != NULL)
        m_pIndexBuffer->Unlock();

    return hr;
}

HRESULT
CEdgeOutline::Draw(LPD3DXBASEMESH ptmDrawMesh, DWORD iAttrib)
{
    HRESULT hr;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
    DWORD iSubset;

    DWORD *pdwLineIndices;
    PBYTE pbLine;
    PBYTE pbPoints;
    DWORD iLine;
    DWORD iLineStart;
    DWORD iLineEnd;

    if ((iAttrib < m_caeAttributeTable) && (m_rgaeAttributeTable[iAttrib].AttribId == iAttrib))
    {
        iSubset = iAttrib;
    }
    else
    {
        // look for the correct attribute table entry to draw
        for (iSubset = 0; iSubset < m_caeAttributeTable; iSubset++)
        {
            if (m_rgaeAttributeTable[iSubset].AttribId == iAttrib)
            {
                break;
            }
        }
    }

    if (iSubset < m_caeAttributeTable)
    {
        if (m_rgaeAttributeTable[iSubset].FaceCount > 0)
        {
            if (!m_bEmulate32BitIndex)
            {
                ptmDrawMesh->GetVertexBuffer(&pVertexBuffer);
                m_pDevice->SetVertexDeclaration(m_pDeclObj);
                m_pDevice->SetStreamSource(0, pVertexBuffer, 0, m_cBytesPerVertex);
                m_pDevice->SetIndices(m_pIndexBuffer);

                hr = m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 
                                             0, 0, m_rgaeAttributeTable[iSubset].VertexStart + m_rgaeAttributeTable[iSubset].VertexCount,
                                             m_rgaeAttributeTable[iSubset].FaceStart * 2, 
                                             m_rgaeAttributeTable[iSubset].FaceCount);
                if (FAILED(hr))
                    return hr;

                GXRELEASE(pVertexBuffer);
            }
            else
            {
                GXASSERT(ptmDrawMesh->GetOptions() & D3DXMESH_32BIT);

                hr = ptmDrawMesh->LockVertexBuffer(D3DLOCK_READONLY, (PVOID*)&pbPoints);
                if (FAILED(hr))
                    return hr;

                hr = m_pIndexBuffer->Lock(0, 0, (PVOID*)&pdwLineIndices, D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY);
                if (FAILED(hr))
                    return hr;

                pbLine = (PBYTE)_alloca(m_cBytesPerVertex * 2);

                iLineStart = m_rgaeAttributeTable[iSubset].FaceStart * 2;
                iLineEnd = iLineStart + m_rgaeAttributeTable[iSubset].FaceCount * 2;
                for (iLine = iLineStart; iLine < iLineEnd; iLine+=2)
                {
                    memcpy(pbLine, pbPoints + pdwLineIndices[iLine]*m_cBytesPerVertex, m_cBytesPerVertex);
                    memcpy(pbLine + m_cBytesPerVertex, pbPoints + pdwLineIndices[iLine+1]*m_cBytesPerVertex, m_cBytesPerVertex);

                    hr = m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, pbLine, m_cBytesPerVertex);
                    if (FAILED(hr))
                        return hr;
                }

                ptmDrawMesh->UnlockVertexBuffer();
                m_pIndexBuffer->Unlock();
            }
        }
    }
    return S_OK;
}

HRESULT
CEdgeOutline::CreateEmptyOutline()
{
    delete []m_rgaeAttributeTable;
    m_rgaeAttributeTable = NULL;
    m_caeAttributeTable = 0;
    GXRELEASE(m_pIndexBuffer);
    GXRELEASE(m_pDevice);
    GXRELEASE(m_pDeclObj);

    return S_OK;
}
