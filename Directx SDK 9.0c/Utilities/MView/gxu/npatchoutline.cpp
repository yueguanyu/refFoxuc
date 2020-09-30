/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"

#include "NPatchOutline.h"

CNPatchOutline::CNPatchOutline()
  :m_pDevice(NULL),
    m_pIndexBuffer(NULL),
    m_rgaeAttributeTable(NULL),
    m_caeAttributeTable(0),
    m_pDeclObj(NULL),
    m_bEmulate32BitIndex(FALSE)
{
}

CNPatchOutline::~CNPatchOutline()
{
    GXRELEASE(m_pIndexBuffer);
    GXRELEASE(m_pDevice);
    GXRELEASE(m_pDeclObj);

    delete []m_rgaeAttributeTable;
}

// -------------------------------------------------------------------------------
//  method    CalculateCounts
//
//  devnote     Calculates the number of vertices and faces for a bezier triangle
//                  that is tessellated with the given level
//
//  returns     S_OK if suceeded, else error code
//
static void 
CalculateCounts(DWORD cSegments, PDWORD pcFaces)
{
    DWORD iLevel;
    DWORD cFacesPrev;
    DWORD cFacesOut;


    // UNDONE - should be a convenient formula to figure this one out
    cFacesOut = 1;
    cFacesPrev = 1;
    for (iLevel = 0; iLevel < cSegments-1; iLevel++)
    {
        cFacesPrev += 2;
        cFacesOut += cFacesPrev;
    }

    *pcFaces = cFacesOut;
}


template<class UINT_IDX>
void AddOutlineLineSegments(UINT_IDX *pwNPatchFaceIndices, DWORD cSegments, UINT_IDX *pwLineIndicesOut)
{
    DWORD cFacesCurLine;
    DWORD iLevel;
    DWORD iFaceCur;

    UINT_IDX *pwLineIndicesOrig = pwLineIndicesOut;

    // first deal with all triangles up to the last "strip" of tesselate triangles (three sides of lines, not just two for that row)
    cFacesCurLine = 1;
    for (iLevel = 0; iLevel < cSegments-1; iLevel++)
    {
        // Add the left edge
        *pwLineIndicesOut = pwNPatchFaceIndices[0];
        pwLineIndicesOut++;
        *pwLineIndicesOut = pwNPatchFaceIndices[1];
        pwLineIndicesOut++;

        // skip the interior triangles of this strip
        pwNPatchFaceIndices += 3 * (cFacesCurLine - 1);

        // add the right edge
        *pwLineIndicesOut = pwNPatchFaceIndices[0];
        pwLineIndicesOut++;
        *pwLineIndicesOut = pwNPatchFaceIndices[2];
        pwLineIndicesOut++;
        pwNPatchFaceIndices += 3;

        cFacesCurLine += 2;        
    }

    // add the left edge
    *pwLineIndicesOut = pwNPatchFaceIndices[0];
    pwLineIndicesOut++;
    *pwLineIndicesOut = pwNPatchFaceIndices[1];
    pwLineIndicesOut++;

    // add the bottom edges of all triangles in the middle of the bottom strip
    if (cFacesCurLine > 1)
    {
        // add the bottom edge
        for (iFaceCur = 0; iFaceCur < cFacesCurLine - 2; iFaceCur += 2)
        {
            *pwLineIndicesOut = pwNPatchFaceIndices[1];
            pwLineIndicesOut++;
            *pwLineIndicesOut = pwNPatchFaceIndices[2];
            pwLineIndicesOut++;

            pwNPatchFaceIndices += 6;
        }
    }

    // add the right edge of the last triangle
    *pwLineIndicesOut = pwNPatchFaceIndices[0];
    pwLineIndicesOut++;
    *pwLineIndicesOut = pwNPatchFaceIndices[2];
    pwLineIndicesOut++;

    // add the bottom edge of the last triangle
    *pwLineIndicesOut = pwNPatchFaceIndices[1];
    pwLineIndicesOut++;
    *pwLineIndicesOut = pwNPatchFaceIndices[2];
    pwLineIndicesOut++;

    GXASSERT(pwLineIndicesOrig + cSegments * 2 * 3 == pwLineIndicesOut);
}

HRESULT
CNPatchOutline::Init(ID3DXBaseMesh *ptmMesh, DWORD cTesselateSegments)
{
    HRESULT hr = S_OK;
    BOOL b16BitIndex;
    DWORD cBytesPerIndex;
    DWORD iPoint;
    DWORD iOrigFace;
    DWORD cFaces;
    DWORD cOrigFaces;
    D3DXATTRIBUTERANGE *rgaeAttributeTableMesh = NULL;
    DWORD *rgdwAttr = NULL;
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
    PBYTE pbFaceIndicesCur;
    PBYTE pbLineIndicesCur;
    DWORD cNPatchFacesPerFace;

    // is the mesh 16 bit?
    b16BitIndex = !(ptmMesh->GetOptions() & D3DXMESH_32BIT);
    cBytesPerIndex = (b16BitIndex ? 2:4);

    // get the number of faces
    cFaces = ptmMesh->GetNumFaces();

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
    if (m_caeAttributeTable == 0) 
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

    CalculateCounts(cTesselateSegments, &cNPatchFacesPerFace);
    m_cLinesPerOrigFace = cTesselateSegments * 3;

    // should be the same number of new faces in relation to the old
    GXASSERT((cFaces % cNPatchFacesPerFace) == 0);
    cOrigFaces = cFaces / cNPatchFacesPerFace;

    hr = ptmMesh->GetAttributeTable(rgaeAttributeTableMesh, NULL);
    if (FAILED(hr))
        goto e_Exit;

    ptmMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbFaceIndices);

    // now initialize the edge attribute table
    cFacesCur = 0;
    for (iAttr = 0; iAttr < m_caeAttributeTable; iAttr++)
    {
        m_rgaeAttributeTable[iAttr] = rgaeAttributeTableMesh[iAttr];

        // 3 * Segments primitives per original triangle - original triangle = NewFaces / NumNewFacesPerOld
        m_rgaeAttributeTable[iAttr].FaceCount = (m_rgaeAttributeTable[iAttr].FaceCount / cNPatchFacesPerFace) * cTesselateSegments * 3;

        // setup the correct face start
        m_rgaeAttributeTable[iAttr].FaceStart = cFacesCur;
        cFacesCur += m_rgaeAttributeTable[iAttr].FaceCount;
    }

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

    hr = m_pDevice->CreateIndexBuffer(cOrigFaces * 3 * cTesselateSegments * 2 * cBytesPerIndex, D3DUSAGE_SOFTWAREPROCESSING, 
                                        (b16BitIndex ? D3DFMT_INDEX16:D3DFMT_INDEX32),
                                        D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
    if (FAILED(hr))
        goto e_Exit;

    m_pIndexBuffer->Lock(0, 0, (PVOID*)&pbLineIndices, D3DLOCK_NOSYSLOCK);


    // next go through and fill the index buffer
    pbFaceIndicesCur = pbFaceIndices;
    pbLineIndicesCur = pbLineIndices;
    for (iOrigFace = 0; iOrigFace < cOrigFaces; iOrigFace++)
    {
        if (b16BitIndex)
        {
            AddOutlineLineSegments<UINT16>((UINT16*)pbFaceIndicesCur, cTesselateSegments, (UINT16*)pbLineIndicesCur);
        }
        else
        {
            AddOutlineLineSegments<UINT32>((UINT32*)pbFaceIndicesCur, cTesselateSegments, (UINT32*)pbLineIndicesCur);
        }

        pbFaceIndicesCur += cBytesPerIndex * 3 * cNPatchFacesPerFace;
        pbLineIndicesCur += cBytesPerIndex * 2 * 3 * cTesselateSegments;
    }

e_Exit:
    delete []rgaeAttributeTableMesh;
    delete []rgdwAttr;

    if (pbFaceIndices != NULL)
        ptmMesh->UnlockIndexBuffer();

    if (pbLineIndices != NULL)
        m_pIndexBuffer->Unlock();

    return hr;
}

HRESULT
CNPatchOutline::DrawInternal(LPD3DXBASEMESH ptmDrawMesh, DWORD iAttrib, DWORD iStartIndex, DWORD cLines)
{
    HRESULT hr;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
    DWORD cBytesPerVertex;
    DWORD dwFVF;
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
        if ((iStartIndex == UNUSED32) || (cLines == UNUSED32))
        {
            iStartIndex = m_rgaeAttributeTable[iSubset].FaceStart;
            cLines = m_rgaeAttributeTable[iSubset].FaceCount;
        }

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
                                             iStartIndex * 2, 
                                             cLines);
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

                iLineStart = iStartIndex * 2;
                iLineEnd = iLineStart + cLines * 2;
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
CNPatchOutline::Draw(LPD3DXBASEMESH ptmDrawMesh, DWORD iSubset)
{
    if (iSubset < m_caeAttributeTable)
        return DrawInternal(ptmDrawMesh, iSubset, UNUSED32, UNUSED32);
    else
        return S_OK;
}

HRESULT
CNPatchOutline::DrawFace(LPD3DXBASEMESH ptmDrawMesh, DWORD iFace, DWORD iSubset)
{
    if (iSubset < m_caeAttributeTable)
        return DrawInternal(ptmDrawMesh, iSubset, iFace * m_cLinesPerOrigFace, m_cLinesPerOrigFace);
    else
        return S_OK;
}

HRESULT
CNPatchOutline::CreateEmptyOutline()
{
    delete []m_rgaeAttributeTable;
    m_rgaeAttributeTable = NULL;
    m_caeAttributeTable = 0;
    GXRELEASE(m_pIndexBuffer);
    GXRELEASE(m_pDevice);
    GXRELEASE(m_pDeclObj);

    return S_OK;
}
