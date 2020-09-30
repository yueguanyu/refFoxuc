/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"

#include "StripOutline.h"

#define SKINNEDFVF D3DFVF_XYZB4|D3DFVF_TEX5|D3DFVF_TEXCOORDSIZE3(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE3(2)|D3DFVF_TEXCOORDSIZE4(3)|D3DFVF_TEXCOORDSIZE3(4)
#define INDEXSKINNEDFVF D3DFVF_XYZB5|D3DFVF_TEX7|D3DFVF_TEXCOORDSIZE3(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE1(2)|D3DFVF_TEXCOORDSIZE3(3)|D3DFVF_TEXCOORDSIZE4(4)|D3DFVF_TEXCOORDSIZE1(5)|D3DFVF_TEXCOORDSIZE3(6)|D3DFVF_LASTBETA_D3DCOLOR


void CopyWeights
    (
    float *pfWeightSrc,
    DWORD cWeightsSrc,
    float *pfWeightDest,
    DWORD cWeightsDest
    );

void CopyIndexedWeights
    (
    float *pfWeightSrc,
    DWORD cWeightsSrc,
    float *pfWeightDest,
    DWORD cWeightsDest
    );

CStripOutline::CStripOutline()
    :m_cfvf(D3DFVF_XYZ),
     m_bSkinning(FALSE),
     m_bIndexSkinning(FALSE),
     m_bHWVertexShaders(FALSE),
     m_pVertexBuffer(NULL),
     m_rgaeAttributeTable(NULL),
     m_caeAttributeTable(0),
     m_cLines(0),
     m_cVertices(0),
     m_pDecl(NULL)
{
}

CStripOutline::~CStripOutline()
{
    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDecl);
    delete []m_rgaeAttributeTable;
}

bool BConnected(PUINT pwFace1, PUINT pwFace2)
{
    UINT cSharedPoints;
    UINT iPoint1;
    UINT iPoint2;

    cSharedPoints = 0;
    for (iPoint1 = 0; iPoint1 < 3; iPoint1++)
    {
        for (iPoint2 = 0; iPoint2 < 3; iPoint2++)
        {
            if (pwFace1[iPoint1] == pwFace2[iPoint2])
            {
                cSharedPoints += 1;

                break;
            }
        }
    }

    GXASSERT(cSharedPoints <= 3);

    return cSharedPoints >= 2;
}

void
CStripOutline::AppendPoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    D3DXVECTOR3 vTemp(0,0,0);
    UINT iPoint;
    PVOID pvPoint;

    for (iPoint = 0; iPoint < 3; iPoint++)
    {
        pvPoint = cdMesh.GetArrayElem((PBYTE)pvMeshPoints, pwFace[iPoint]);
        vTemp += *cdMesh.PvGetPosition((PBYTE)pvPoint);
    }

    vTemp /= 3.0f;

    pvPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    m_cfvf.SetPosition(pvPoint, (D3DXVECTOR3*)&vTemp);

    iCurPoint += 1;
}

void
CStripOutline::AppendMidpoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    PUINT pwFace2, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    D3DXVECTOR3 vTemp(0,0,0);
    UINT iPoint1;
    UINT iPoint2;
    PVOID pvPoint;

    GXASSERT(BConnected(pwFace1, pwFace2));

    for (iPoint1 = 0; iPoint1 < 3; iPoint1++)
    {
        for (iPoint2 = 0; iPoint2 < 3; iPoint2++)
        {
            if (pwFace1[iPoint1] == pwFace2[iPoint2])
            {
                pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[iPoint1]);
                vTemp += *cdMesh.PvGetPosition((PBYTE)pvPoint);

                break;
            }
        }
    }

    vTemp /= 2.0f;

    pvPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    m_cfvf.SetPosition(pvPoint, (D3DXVECTOR3*)&vTemp);

    iCurPoint += 1;
}

void
CStripOutline::AppendSkinnedMidpoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    PUINT pwFace2, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    D3DXVECTOR3 vMidPoint(0.5f, 0.5f, 0.0f);
    DWORD iPoint1;
    DWORD iPoint2;
    DWORD iEdge = UNUSED32;

    GXASSERT(BConnected(pwFace1, pwFace2));

    // find the edge connecting the two faces
    for (iPoint1 = 0; iPoint1 < 3; iPoint1++)
    {
        for (iPoint2 = 0; iPoint2 < 3; iPoint2++)
        {
            if ((pwFace1[iPoint1] == pwFace2[(iPoint2+1)%3]) && (pwFace1[(iPoint1+1)%3] == pwFace2[iPoint2]))
            {
                iEdge = iPoint1;
                break;
            }
        }
    }

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    AppendSkinnedPoint(cdMesh, pwFace1, iEdge, pvMeshPoints, pvLinePoints, iCurPoint);

    // the third position should be ignored!
    m_cfvf.SetTexCoord(pvDestPoint, 4, &vMidPoint);
}

void
CStripOutline::AppendSkinnedPoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    DWORD iEdge,
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    PBYTE pvPoint;
    D3DXVECTOR3 vAvg(1.0f/3.0f,1.0f/3.0f,1.0f/3.0f);

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    // Point 0 - first point on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[iEdge]);

    // first position goes in standard position spot
    m_cfvf.SetPosition(pvDestPoint, cdMesh.PvGetPosition(pvPoint));

    // first set of weights go in standard spot - expand to standard length
    CopyWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights(), m_cfvf.PfGetWeights(pvDestPoint), m_cfvf.CWeights());

    // Point 1 - other point on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[(iEdge+1)%3]);

    // second position goes in first texture coord
    m_cfvf.SetTexCoord(pvDestPoint, 0, cdMesh.PvGetPosition(pvPoint));

    // second set of weights go in second texture coord- expand to standard length
    CopyWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights(), (float*)m_cfvf.PxyzwGetTexCoord(pvDestPoint, 1), m_cfvf.CWeights());

    // Point 2 - point NOT on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[(iEdge+2)%3]);

    // third position goes in the third texture coordinate
    m_cfvf.SetTexCoord(pvDestPoint, 2, cdMesh.PvGetPosition(pvPoint));

    // third set of weights go in fourth texture coord- expand to standard length
    CopyWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights(), (float*)m_cfvf.PxyzwGetTexCoord(pvDestPoint, 3), m_cfvf.CWeights());


    // the third position should be added in
    m_cfvf.SetTexCoord(pvDestPoint, 4, &vAvg);

    iCurPoint += 1;
}

void
CStripOutline::AppendIndexSkinnedMidpoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    PUINT pwFace2, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    D3DXVECTOR3 vMidPoint(0.5f, 0.5f, 0.0f);
    DWORD iPoint1;
    DWORD iPoint2;
    DWORD iEdge = UNUSED32;

    GXASSERT(BConnected(pwFace1, pwFace2));

    // find the edge connecting the two faces
    for (iPoint1 = 0; iPoint1 < 3; iPoint1++)
    {
        for (iPoint2 = 0; iPoint2 < 3; iPoint2++)
        {
            if ((pwFace1[iPoint1] == pwFace2[(iPoint2+1)%3]) && (pwFace1[(iPoint1+1)%3] == pwFace2[iPoint2]))
            {
                iEdge = iPoint1;
                break;
            }
        }
    }

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    AppendIndexSkinnedPoint(cdMesh, pwFace1, iEdge, pvMeshPoints, pvLinePoints, iCurPoint);

    // the third position should be ignored!
    m_cfvf.SetTexCoord(pvDestPoint, 6, &vMidPoint);
}

void
CStripOutline::AppendIndexSkinnedPoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    DWORD iEdge,
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    PBYTE pvPoint;
    D3DXVECTOR3 vAvg(1.0f/3.0f,1.0f/3.0f,1.0f/3.0f);

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    // Point 0 - first point on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[iEdge]);

    // first position goes in standard position spot
    m_cfvf.SetPosition(pvDestPoint, cdMesh.PvGetPosition(pvPoint));

    // first set of weights go in standard spot - expand to standard length
    CopyIndexedWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights() + 1, m_cfvf.PfGetWeights(pvDestPoint), m_cfvf.CWeights());

    // Point 1 - other point on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[(iEdge+1)%3]);

    // second position goes in first texture coord
    m_cfvf.SetTexCoord(pvDestPoint, 0, cdMesh.PvGetPosition(pvPoint));

    // second set of weights go in second texture coord- expand to standard length
    CopyIndexedWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights() + 1, (float*)m_cfvf.PxyzwGetTexCoord(pvDestPoint, 1), m_cfvf.CWeights());

    // Point 2 - point NOT on edge shared with another triangle
    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[(iEdge+2)%3]);

    // third position goes in the third texture coordinate
    m_cfvf.SetTexCoord(pvDestPoint, 3, cdMesh.PvGetPosition(pvPoint));

    // third set of weights go in fourth texture coord- expand to standard length
    CopyIndexedWeights(cdMesh.PfGetWeights(pvPoint), cdMesh.CWeights() + 1, (float*)m_cfvf.PxyzwGetTexCoord(pvDestPoint, 4), m_cfvf.CWeights());


    // the third position should be added in
    m_cfvf.SetTexCoord(pvDestPoint, 6, &vAvg);

    iCurPoint += 1;
}

HRESULT
CStripOutline::Init(ID3DXBaseMesh *ptmMesh)
{
    HRESULT hr = S_OK;
    DWORD cFaces;
    DWORD iFace;
    DWORD iEndOfStrips;
    UINT *rgiStrips = NULL;
    DWORD cConnections;
    DWORD cStrips;
    DWORD cPointsMax;
    DWORD iStripIndex;
    UINT iCurPoint;
    DWORD iCurLine;
    PBYTE pvLinePoints = NULL;
    PBYTE pvMeshPoints = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVBVertices = NULL;
    LPDIRECT3DINDEXBUFFER9 pibFaces= NULL;
    PUINT pdwFaceCur;
    PUINT pdwFacePrev;
    PUINT rgdwFaces = NULL;
    CD3DXCrackDecl1 cdMesh;
    PWORD rgwFaces = NULL;
    bool b16BitMesh;
    UINT iIndex;
    LPDIRECT3DDEVICE9 pDevice;
    D3DXATTRIBUTERANGE *rgaeAttributeTableMesh = NULL;
    DWORD iattr;
    DWORD cStripsCur;
    DWORD cVerticesCur;
    DWORD dwFVFAdjVertices;
    D3DCAPS9 Caps;
    DWORD iFaceEnd;
    DWORD iFaceStart;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;

    hr = CreateEmptyOutline();
    if (FAILED(hr))
        goto e_Exit;


    if (ptmMesh == NULL)
        goto e_Exit;

    ptmMesh->GetDeclaration(pDecl);
    cdMesh.SetDeclaration(pDecl);

    ptmMesh->GetDevice(&pDevice);
    pDevice->Release();

    pDevice->GetDeviceCaps(&Caps);
    m_bHWVertexShaders = Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);

    // is the mesh 16 bit?
    b16BitMesh = !(ptmMesh->GetOptions() & D3DXMESH_32BIT);

    hr = ptmMesh->GetAttributeTable(NULL, &m_caeAttributeTable);
    if (FAILED(hr))
        goto e_Exit;

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

    hr = ptmMesh->GetAttributeTable(rgaeAttributeTableMesh, NULL);
    if (FAILED(hr))
        goto e_Exit;

    cFaces = 0;
    for (iattr = 0; iattr < m_caeAttributeTable; iattr++)
    {
        cFaces = max(cFaces, rgaeAttributeTableMesh[iattr].FaceStart + rgaeAttributeTableMesh[iattr].FaceCount);
    }

    if (cdMesh.CWeights() > 0)
    {
        m_bSkinning = TRUE;

        if (cdMesh.BIndexedWeights())
        {
            dwFVFAdjVertices = INDEXSKINNEDFVF;
            m_bIndexSkinning = TRUE;
        }
        else
        {
            dwFVFAdjVertices = SKINNEDFVF;
            m_bIndexSkinning = FALSE;
        }
    }
    else
    {
        dwFVFAdjVertices = D3DFVF_XYZ;
        m_bSkinning = FALSE;
        m_bIndexSkinning = FALSE;
    }
    m_cfvf = DXCrackFVF(dwFVFAdjVertices);

    // allocate a temporary array to track the triangles to connect with lines
    rgiStrips = new UINT[cFaces*2];
    if (rgiStrips == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    if (b16BitMesh)
    {
        // allocate a temporary, so that the follow code
        //   can work on just 32bit indices
        rgdwFaces = new UINT[cFaces*3];
        if (rgdwFaces == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // get the index buffer
        hr = ptmMesh->GetIndexBuffer(&pibFaces);
        if (FAILED(hr))
            goto e_Exit;

        hr = pibFaces->Lock(0,0, (PVOID*)&rgwFaces, D3DLOCK_NOSYSLOCK );
        if (FAILED(hr))
            goto e_Exit;

        // UpConvert from 16 bit to 32 bit
        for (iIndex = 0; iIndex < cFaces * 3; iIndex++)
        {
            if (rgwFaces[iIndex] != UNUSED16)
                rgdwFaces[iIndex] = rgwFaces[iIndex];
            else
                rgdwFaces[iIndex] = UNUSED32;
        }
    }
    else
    {
        // get the index buffer
        hr = ptmMesh->GetIndexBuffer(&pibFaces);
        if (FAILED(hr))
            goto e_Exit;

        hr = pibFaces->Lock(0,0, (PVOID*)&rgdwFaces, D3DLOCK_NOSYSLOCK );
        if (FAILED(hr))
            goto e_Exit;
    }


    // start out with zero entries in each attribute table
            // fixup the rest of the entries after figuring out the counts
    for (iattr = 0; iattr < m_caeAttributeTable; iattr++)
    {
        m_rgaeAttributeTable[iattr].FaceCount = 0;
        m_rgaeAttributeTable[iattr].VertexCount = 0;

        m_rgaeAttributeTable[iattr].AttribId = rgaeAttributeTableMesh[iattr].AttribId;

    }

    iEndOfStrips = 0;
    cConnections = 0;
    cStrips = 0;

    for (iattr = 0; iattr < m_caeAttributeTable; iattr++)
    {
        if (rgaeAttributeTableMesh[iattr].FaceCount == 0)
            continue;

        m_rgaeAttributeTable[iattr].VertexCount = 0;

        iFaceStart = rgaeAttributeTableMesh[iattr].FaceStart;
        iFaceEnd = iFaceStart + rgaeAttributeTableMesh[iattr].FaceCount;

        pdwFacePrev = &rgdwFaces[iFaceStart * 3];
        pdwFaceCur = pdwFacePrev + 3;
        rgiStrips[iEndOfStrips] = iFaceStart;
        iEndOfStrips += 1;
        cStrips += 1;
        for (iFace = iFaceStart + 1; iFace < iFaceEnd; iFace++)
        {
            // if the part of the same strip, extend the strip
            if (BConnected(pdwFaceCur, pdwFacePrev))
            {
                rgiStrips[iEndOfStrips] = iFace;
                iEndOfStrips += 1;

                m_rgaeAttributeTable[iattr].FaceCount += 2;
                m_rgaeAttributeTable[iattr].VertexCount += 4;

                // how many faces are connected to another in a strip-like fashion
                cConnections += 1;
            }
            else // end the strip, and restart a new one with the current face
            {
                rgiStrips[iEndOfStrips] = UNUSED32;
                iEndOfStrips += 1;


                rgiStrips[iEndOfStrips] = iFace;
                iEndOfStrips += 1;

                cStrips += 1;
            }

            // move the current to the previous, then increment the current one
            pdwFacePrev = pdwFaceCur;
            pdwFaceCur += 3;
        }

        // for consistency end the last strip
        rgiStrips[iEndOfStrips] = UNUSED32;
        iEndOfStrips += 1;
    }


    // start out with zero entries in each attribute table
            // fixup the rest of the entries after figuring out the counts
    cStripsCur = 0;
    cVerticesCur = 0;
    for (iattr = 0; iattr < m_caeAttributeTable; iattr++)
    {
        m_rgaeAttributeTable[iattr].FaceStart = cStripsCur;
        m_rgaeAttributeTable[iattr].VertexStart = cVerticesCur;

        cStripsCur += m_rgaeAttributeTable[iattr].FaceCount;
        cVerticesCur += m_rgaeAttributeTable[iattr].VertexCount;
    }

    cPointsMax = cConnections * 4;

    if (cConnections == 0)
        goto e_Exit;

    hr = pDevice->CreateVertexBuffer(m_cfvf.m_cBytesPerVertex * cPointsMax, 
                    D3DUSAGE_WRITEONLY|(m_bHWVertexShaders ? 0:D3DUSAGE_SOFTWAREPROCESSING), 
                    m_cfvf.m_dwFVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    if (FAILED(hr))
        goto e_Exit;

    hr = m_pVertexBuffer->Lock(0,0, (PVOID*)&pvLinePoints, D3DLOCK_NOSYSLOCK );
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->GetVertexBuffer(&pVBVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = pVBVertices->Lock(0,0, (PVOID*)&pvMeshPoints, D3DLOCK_NOSYSLOCK );
    if (FAILED(hr))
        goto e_Exit;


    iStripIndex = 0;
    iCurPoint = 0;
    iCurLine = 0;
    while (iStripIndex < iEndOfStrips)
    {
        iFace = rgiStrips[iStripIndex];
        iStripIndex += 1;

        // for pref reasons, there are a lot of unconnected triangles sometimes
        if (iFace == UNUSED32)
            continue;

        pdwFacePrev = &rgdwFaces[iFace * 3];
        if (pdwFacePrev[0] == UNUSED32)
            continue;


        while(rgiStrips[iStripIndex] != UNUSED32)
        {
            if (m_bIndexSkinning)
                AppendIndexSkinnedPoint(cdMesh, pdwFacePrev, 0, pvMeshPoints, pvLinePoints, iCurPoint);
            else if (m_bSkinning)
                AppendSkinnedPoint(cdMesh, pdwFacePrev, 0, pvMeshPoints, pvLinePoints, iCurPoint);
            else
                AppendPoint(cdMesh, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);

            iFace = rgiStrips[iStripIndex];

            pdwFaceCur = &rgdwFaces[iFace * 3];
            if (pdwFaceCur[0] == UNUSED32)
                break;

            iCurLine += 1;

            if (m_bIndexSkinning)
                AppendIndexSkinnedMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);
            else if (m_bSkinning)
                AppendSkinnedMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);
            else
                AppendMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);

            if (m_bIndexSkinning)
                AppendIndexSkinnedMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);
            else if (m_bSkinning)
                AppendSkinnedMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);
            else
                AppendMidpoint(cdMesh, pdwFaceCur, pdwFacePrev, pvMeshPoints, pvLinePoints, iCurPoint);

            iCurLine += 1;

            if (m_bIndexSkinning)
                AppendIndexSkinnedPoint(cdMesh, pdwFaceCur, 0, pvMeshPoints, pvLinePoints, iCurPoint);
            else if (m_bSkinning)
                AppendSkinnedPoint(cdMesh, pdwFaceCur, 0, pvMeshPoints, pvLinePoints, iCurPoint);
            else
                AppendPoint(cdMesh, pdwFaceCur, pvMeshPoints, pvLinePoints, iCurPoint);

            iStripIndex += 1;

            pdwFacePrev = pdwFaceCur;
        }
    }
    // make sure that the points generated by walking the strips is
    //   not greater than what is possible
    GXASSERT(iCurPoint == cPointsMax);
    GXASSERT(iCurLine == cConnections*2);

    m_cLines = iCurLine;
    m_cVertices = cPointsMax;

    // create a vertex declaration, so that we can have D3DCOLOR instead of UBYTE4 for the matrix indices 
    hr = D3DXDeclaratorFromFVF(m_cfvf.m_dwFVF, pDecl);
    if (FAILED(hr))
        goto e_Exit;

    // swap the type of any blendilndices to D3DCOLOR
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff)
    {
        if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) || (pDeclCur->Type == D3DDECLTYPE_FLOAT1))
        {
            //pDeclCur->Type = D3DDECLTYPE_UBYTE4;
            pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
        }
        pDeclCur++;
    }

    GXRELEASE(m_pDecl);
    hr = pDevice->CreateVertexDeclaration(pDecl, &m_pDecl);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    // if a 16 bit mesh, then rgdwFaces is a temporary array
    if (b16BitMesh)
    {
        delete []rgdwFaces;

        if (rgwFaces != NULL)
        {
            pibFaces->Unlock();
        }
    }
    else
    {
        if (rgdwFaces != NULL)
        {
            pibFaces->Unlock();
        }
    }
    GXRELEASE(pibFaces);

    if (pvMeshPoints)
    {
        GXASSERT(pVBVertices != NULL);
        pVBVertices->Unlock();
    }
    GXRELEASE(pVBVertices);

    delete []rgiStrips;
    delete []rgaeAttributeTableMesh;

    if (pvLinePoints != NULL)
    {
        m_pVertexBuffer->Unlock();
        pvLinePoints = NULL;
    }

    return hr;
}

// matrix names for variables in FX file
static const char *x_rgszMatrix[] =
{
 "mWd1", "mWd2", "mWd3", "mWd4", "mWd5", "mWd6", "mWd7", "mWd8", "mWd9",
 "mWd10", "mWd11", "mWd12", "mWd13", "mWd14", "mWd15", "mWd16", "mWd17", "mWd18", "mWd19",
 "mWd20", "mWd21", "mWd22", "mWd23", "mWd24", "mWd25", "mWd26", "mWd27", "mWd28"
};

HRESULT
CStripOutline::Draw
    (
    LPDIRECT3DDEVICE9 pDevice, 
    DWORD iAttrib,
    LPD3DXEFFECT pfxSkinnedAdjacency, 
    LPD3DXEFFECT pfxColor, 
    DWORD dwColor,
    D3DXMATRIX *rgmIndexedMatrices,
    DWORD cIndexedMatrices
    )
{
    HRESULT hr;
    LPD3DXEFFECT pfxCurrent;
    DWORD dwFVF;
    UINT iPass;
    UINT cPasses;
    DWORD cBytesPerVertex;
    D3DXMATRIX matWorld;
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matTot;
    D3DXCOLOR vClr(dwColor);
    DWORD cLines;
    DWORD cLinesCur;
    DWORD iVertexStart;
	BOOL  dwSoftwareMode;
    BOOL bChangedSWMode = FALSE;
    DWORD iMatrix;
    DWORD iSubset;

    if (m_bSkinning)
    {
        if (!m_bIndexSkinning)
        {
            // non-indexed skinning setup
            pfxCurrent = pfxSkinnedAdjacency;
            pfxSkinnedAdjacency->SetTechnique(pfxSkinnedAdjacency->GetTechnique(0));

            pDevice->GetTransform(D3DTS_VIEW,&matView);
            pDevice->GetTransform(D3DTS_PROJECTION,&matProj);
            pDevice->GetTransform(D3DTS_WORLD,&matWorld);

            D3DXMatrixMultiply(&matTot,&matView,&matProj);
            pfxSkinnedAdjacency->SetMatrix("mTot", &matTot);                       

            pDevice->GetTransform(D3DTS_WORLD,&matWorld);
            pfxSkinnedAdjacency->SetMatrix("mWd1", &matWorld);                       

            pDevice->GetTransform(D3DTS_WORLD1,&matWorld);
            pfxSkinnedAdjacency->SetMatrix("mWd2", &matWorld);                       

            pDevice->GetTransform(D3DTS_WORLD2,&matWorld);
            pfxSkinnedAdjacency->SetMatrix("mWd3", &matWorld);                       

            pDevice->GetTransform(D3DTS_WORLD3,&matWorld);
            pfxSkinnedAdjacency->SetMatrix("mWd4", &matWorld);        
        }        
        else  // indexed skinning setup
        {
            pfxCurrent = pfxSkinnedAdjacency;
            pfxSkinnedAdjacency->SetTechnique(pfxSkinnedAdjacency->GetTechnique(1));

            pDevice->GetTransform(D3DTS_VIEW,&matView);
            pDevice->GetTransform(D3DTS_PROJECTION,&matProj);

            D3DXMatrixMultiply(&matTot,&matView,&matProj);
            pfxSkinnedAdjacency->SetMatrix("mTot", &matTot);                       

            pfxSkinnedAdjacency->SetMatrixArray("mWorlds", rgmIndexedMatrices, cIndexedMatrices);

            for (iMatrix = 0; iMatrix < cIndexedMatrices; iMatrix++)
            {
                pfxSkinnedAdjacency->SetMatrix(x_rgszMatrix[iMatrix], &rgmIndexedMatrices[iMatrix]);
            }
        }

        pfxSkinnedAdjacency->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       

        dwSoftwareMode = pDevice->GetSoftwareVertexProcessing();
        if (!m_bHWVertexShaders)
        {
            bChangedSWMode = TRUE;
            pDevice->SetSoftwareVertexProcessing(TRUE);
        }
        // if in the wrong mode, then switch
        else if (dwSoftwareMode && m_bHWVertexShaders)
        {
            bChangedSWMode = TRUE;
            pDevice->SetSoftwareVertexProcessing(FALSE);
        }
    }
    else
    {
        pfxCurrent = pfxColor;

        pfxCurrent->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       
    }


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
        DWORD iVertexStart;
        if (m_rgaeAttributeTable[iSubset].FaceCount > 0)
        {
            pfxCurrent->Begin(&cPasses, 0);

            for (iPass = 0; iPass < cPasses; iPass++)
            {
                pDevice->SetVertexDeclaration(m_pDecl);

                pfxCurrent->BeginPass(iPass);

                pDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_cfvf.m_cBytesPerVertex);

                iVertexStart = m_rgaeAttributeTable[iSubset].VertexStart;
                cLines = m_rgaeAttributeTable[iSubset].VertexCount / 2;

                while (cLines > 0)
                {
                    cLinesCur = min(cLines, (0xffff / 2));

                    hr = pDevice->DrawPrimitive(D3DPT_LINELIST, 
                                                 iVertexStart, 
                                                 cLinesCur);
                    if (FAILED(hr))
                        return hr;

                    iVertexStart += cLinesCur * 2;
                    cLines -= cLinesCur;
                }

                pfxCurrent->EndPass();
            }

            pfxCurrent->End();
        }
    }

    if (bChangedSWMode)
    {
        pDevice->SetSoftwareVertexProcessing(dwSoftwareMode);
    }

    return S_OK;
}

HRESULT
CStripOutline::CreateEmptyOutline()
{
    GXRELEASE(m_pVertexBuffer);
    m_caeAttributeTable = 0;
    delete []m_rgaeAttributeTable;
    m_rgaeAttributeTable = NULL;

    m_cLines = 0;
    m_cVertices = 0;
    m_bSkinning = FALSE;

    return S_OK;
}
