/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"
#include "AdjOutline.h"

#define SKINNEDFVF D3DFVF_XYZB4|D3DFVF_TEX5|D3DFVF_TEXCOORDSIZE3(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE3(2)|D3DFVF_TEXCOORDSIZE4(3)|D3DFVF_TEXCOORDSIZE3(4)
#define INDEXSKINNEDFVF D3DFVF_XYZB5|D3DFVF_TEX7|D3DFVF_TEXCOORDSIZE3(0)|D3DFVF_TEXCOORDSIZE4(1)|D3DFVF_TEXCOORDSIZE1(2)|D3DFVF_TEXCOORDSIZE3(3)|D3DFVF_TEXCOORDSIZE4(4)|D3DFVF_TEXCOORDSIZE1(5)|D3DFVF_TEXCOORDSIZE3(6)|D3DFVF_LASTBETA_D3DCOLOR

const int x_dwAdjOutlineColor = D3DCOLOR_XRGB(0, 255, 255);

CAdjacencyOutline::CAdjacencyOutline()
    :m_cfvf(D3DFVF_XYZ),
     m_bSkinning(FALSE),
     m_bIndexSkinning(FALSE),
     m_bHWVertexShaders(FALSE),
     m_pVertexBuffer(NULL),
     m_rgaeAttributeTable(NULL),
     m_caeAttributeTable(0),
     m_pDecl(NULL)
{
}

CAdjacencyOutline::~CAdjacencyOutline()
{
    delete m_rgaeAttributeTable;

    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDecl);
}

void
CAdjacencyOutline::AppendPoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    UINT iPoint;
    PVOID pvPoint;
    DWORD cFloats;
    DWORD iFloat;
    float *pfTemp;

    GXASSERT(m_cfvf.CWeights() == cdMesh.CWeights());

    cFloats = 3 /*vector*/ + cdMesh.CWeights();

    pfTemp = (float*)_alloca(cFloats * sizeof(float));
    memset(pfTemp, 0, cFloats * sizeof(float));

    for (iPoint = 0; iPoint < 3; iPoint++)
    {
        pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace[iPoint]);
        for (iFloat = 0; iFloat < cFloats; iFloat++)
        {
            pfTemp[iFloat] += ((float*)pvPoint)[iFloat];
        }
    }

    for (iFloat = 0; iFloat < cFloats; iFloat++)
    {
        pfTemp[iFloat] /= 3.0f;
    }

    pvPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);
    memcpy(pvPoint, pfTemp, sizeof(float) * cFloats);

    iCurPoint += 1;
}

void
CAdjacencyOutline::AppendMidpoint
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    UINT iEdge, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PVOID pvPoint;
    DWORD cFloats;
    DWORD iFloat;
    float *pfTemp;

    GXASSERT(m_cfvf.CWeights() == cdMesh.CWeights());

    cFloats = 3 /*vector*/ + cdMesh.CWeights();

    pfTemp = (float*)_alloca(cFloats * sizeof(float));

    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[iEdge]);
    for (iFloat = 0; iFloat < cFloats; iFloat++)
    {
        pfTemp[iFloat] = ((float*)pvPoint)[iFloat];
    }

    pvPoint = cdMesh.GetArrayElem(pvMeshPoints, pwFace1[(iEdge+1)%3]);
    for (iFloat = 0; iFloat < cFloats; iFloat++)
    {
        pfTemp[iFloat] += ((float*)pvPoint)[iFloat];

        pfTemp[iFloat] /= 2.0f;
    }

    pvPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);
    memcpy(pvPoint, pfTemp, sizeof(float) * cFloats);

    iCurPoint += 1;
}

void CopyWeights
    (
    float *pfWeightSrc,
    DWORD cWeightsSrc,
    float *pfWeightDest,
    DWORD cWeightsDest
    )
{
    float fSum;
    DWORD iWeight;
    float fWeight;

    // there should always be more dest weights than source
    GXASSERT(cWeightsSrc < cWeightsDest);

    // first copy the weights that exist in the source
    fSum = 0.0f;
    for (iWeight = 0; iWeight < cWeightsSrc; iWeight++)
    {
        fWeight = pfWeightSrc[iWeight];
        fSum += fWeight;

        pfWeightDest[iWeight] = fWeight;
    }

    // now set the implied weight - always exists in dest
    pfWeightDest[iWeight] = 1.0f - fSum;
    iWeight += 1;

    // set any other weights that are unused to 0.0f
    for (; iWeight < cWeightsDest; iWeight++)
    {
        pfWeightDest[iWeight] = 0.0f;
    }
}

void CopyIndexedWeights
    (
    float *pfWeightSrc,
    DWORD cWeightsSrc,
    float *pfWeightDest,
    DWORD cWeightsDest
    )
{
    float fSum;
    DWORD iWeight;
    float fWeight;

    // there should always be more dest weights than source
    GXASSERT(cWeightsSrc < cWeightsDest);

    // first copy the weights that exist in the source
    fSum = 0.0f;
    for (iWeight = 0; iWeight < cWeightsSrc - 1; iWeight++)
    {
        fWeight = pfWeightSrc[iWeight];
        fSum += fWeight;

        pfWeightDest[iWeight] = fWeight;
    }

    // now set the implied weight - always exists in dest
    pfWeightDest[iWeight] = 1.0f - fSum;
    iWeight += 1;

    // set any other weights that are unused to 0.0f
    for (; iWeight < cWeightsDest - 1; iWeight++)
    {
        pfWeightDest[iWeight] = 0.0f;
    }

    // copy the indices straight from one to the other
    memcpy(&pfWeightDest[cWeightsDest-1], &pfWeightSrc[cWeightsSrc-1], sizeof(DWORD));
}

void
CAdjacencyOutline::AppendPointSkinned
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    UINT iEdge, 
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
CAdjacencyOutline::AppendMidpointSkinned
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    UINT iEdge, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    D3DXVECTOR3 vMidPoint(0.5f, 0.5f, 0.0f);

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    AppendPointSkinned(cdMesh, pwFace1, iEdge, pvMeshPoints, pvLinePoints, iCurPoint);

    // the third position should be ignored!
    m_cfvf.SetTexCoord(pvDestPoint, 4, &vMidPoint);
}

void
CAdjacencyOutline::AppendPointIndexSkinned
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    UINT iEdge, 
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

void
CAdjacencyOutline::AppendMidpointIndexSkinned
    (
    CD3DXCrackDecl1 &cdMesh, 
    PUINT pwFace1, 
    UINT iEdge, 
    PBYTE pvMeshPoints, 
    PBYTE pvLinePoints, 
    UINT &iCurPoint
    )
{
    PBYTE pvDestPoint;
    D3DXVECTOR3 vMidPoint(0.5f, 0.5f, 0.0f);

    pvDestPoint = m_cfvf.GetArrayElem(pvLinePoints, iCurPoint);

    AppendPointIndexSkinned(cdMesh, pwFace1, iEdge, pvMeshPoints, pvLinePoints, iCurPoint);

    // the third position should be ignored!
    m_cfvf.SetTexCoord(pvDestPoint, 6, &vMidPoint);
}

HRESULT
CAdjacencyOutline::Init(ID3DXBaseMesh *ptmMesh, DWORD *rgdwNeighbors)
{
    HRESULT hr = S_OK;
    UINT cFaces;
    UINT iFace;
    UINT cConnections;
    UINT cPointsMax;
    UINT iline;
    PBYTE pvLinePoints = NULL;
    PBYTE pvMeshPoints = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVBVertices = NULL;
    LPDIRECT3DINDEXBUFFER9 pibFaces = NULL;
    PUINT pdwFaceCur;
    PUINT rgdwFaces = NULL;
    CD3DXCrackDecl1 cdMesh;
    PWORD rgwFaces = NULL;
    bool b16BitMesh;
    UINT iIndex;
    UINT iPoint;
    DWORD *pdwNeighbor;
    UINT iCurPoint;
    LPDIRECT3DDEVICE9 pDevice;
    D3DXATTRIBUTERANGE *rgaeAttributeTableMesh = NULL;
    DWORD iCurLineOffset;
    DWORD iCurVertexOffset;
    DWORD iFaceEnd;
    DWORD iae;
    DWORD dwFVFAdjVertices;
    D3DCAPS9 Caps;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;

    ptmMesh->GetDeclaration(pDecl);
    cdMesh.SetDeclaration(pDecl);

    ptmMesh->GetDevice(&pDevice);
    pDevice->Release();

    // is the mesh 16 bit?
    b16BitMesh = !(ptmMesh->GetOptions() & D3DXMESH_32BIT);

    hr = CreateEmptyOutline();
    if (FAILED(hr))
        goto e_Exit;

    pDevice->GetDeviceCaps(&Caps);
    m_bHWVertexShaders = Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);

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

    if (rgdwNeighbors == NULL)
        goto e_Exit;

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

    cFaces = rgaeAttributeTableMesh[m_caeAttributeTable-1].FaceStart + rgaeAttributeTableMesh[m_caeAttributeTable-1].FaceCount;

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

    // first count the number of links
    cConnections = 0;
    iCurLineOffset = 0;
    iCurVertexOffset = 0;
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        m_rgaeAttributeTable[iae].AttribId = rgaeAttributeTableMesh[iae].AttribId;

        // initialize line counts
        m_rgaeAttributeTable[iae].FaceStart = iCurLineOffset;
        m_rgaeAttributeTable[iae].FaceCount = 0;

        iFaceEnd = rgaeAttributeTableMesh[iae].FaceStart + rgaeAttributeTableMesh[iae].FaceCount;
        for (iFace = rgaeAttributeTableMesh[iae].FaceStart; iFace < iFaceEnd; iFace++)
        {
            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (rgdwNeighbors[iFace*3 + iPoint] != UNUSED32)
                {
                    cConnections += 1;
                    m_rgaeAttributeTable[iae].FaceCount += 1;
                }
            }
        }

        m_rgaeAttributeTable[iae].VertexStart = iCurVertexOffset;
        m_rgaeAttributeTable[iae].VertexCount = m_rgaeAttributeTable[iae].FaceCount*2;

        iCurLineOffset += m_rgaeAttributeTable[iae].FaceCount;
        iCurVertexOffset += m_rgaeAttributeTable[iae].VertexCount;
    }

    // should be even, otherwise the adjacency info is definitely incorrect
    GXASSERT((cConnections & 1) == 0);
    cPointsMax = cConnections * 2;

    // UNDONE UNDONE, need to handle large line lists
    //if (cConnections >= UNUSED16)
      //  goto e_Exit;

    if (cPointsMax == 0)
        goto e_Exit;

    hr = pDevice->CreateVertexBuffer(m_cfvf.m_cBytesPerVertex * cPointsMax, 
                        D3DUSAGE_WRITEONLY|(m_bHWVertexShaders ? 0:D3DUSAGE_SOFTWAREPROCESSING), 
                        m_cfvf.m_dwFVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    if (FAILED(hr))
        goto e_Exit;

    m_pVertexBuffer->Lock(0, 0, (PVOID*)&pvLinePoints, 0);

    hr = ptmMesh->GetVertexBuffer(&pVBVertices);
    if (FAILED(hr))
        goto e_Exit;

    hr = pVBVertices->Lock(0,0, (PVOID*)&pvMeshPoints, D3DLOCK_NOSYSLOCK );
    if (FAILED(hr))
        goto e_Exit;

    iCurPoint = 0;
    iline = 0;
    for (iae = 0; iae < m_caeAttributeTable; iae++)
    {
        iFaceEnd = rgaeAttributeTableMesh[iae].FaceStart + rgaeAttributeTableMesh[iae].FaceCount;
        for (iFace = rgaeAttributeTableMesh[iae].FaceStart; iFace < iFaceEnd; iFace++)
        {
            pdwNeighbor = &rgdwNeighbors[iFace * 3];
            pdwFaceCur = &rgdwFaces[iFace * 3];
            if (pdwFaceCur[0] == UNUSED32)
                break;

            for (iPoint = 0; iPoint < 3; iPoint++)
            {
                if (pdwNeighbor[iPoint] != UNUSED32)
                {
                    if (m_bIndexSkinning)
                    {
                        AppendPointIndexSkinned(cdMesh, pdwFaceCur, iPoint, pvMeshPoints, pvLinePoints, iCurPoint);

                        AppendMidpointIndexSkinned(cdMesh, pdwFaceCur, iPoint, pvMeshPoints, pvLinePoints, iCurPoint);
                    }
                    else if (m_bSkinning)
                    {
                        AppendPointSkinned(cdMesh, pdwFaceCur, iPoint, pvMeshPoints, pvLinePoints, iCurPoint);

                        AppendMidpointSkinned(cdMesh, pdwFaceCur, iPoint, pvMeshPoints, pvLinePoints, iCurPoint);
                    }
                    else
                    {
                        AppendPoint(cdMesh, pdwFaceCur, pvMeshPoints, pvLinePoints, iCurPoint);

                        AppendMidpoint(cdMesh, pdwFaceCur, iPoint, pvMeshPoints, pvLinePoints, iCurPoint);
                    }

                    iline += 1;
                }
            }
        }
    }

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
    if (pvLinePoints != NULL)
    {
        m_pVertexBuffer->Unlock();
    }

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

    delete []rgaeAttributeTableMesh;

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
CAdjacencyOutline::Draw
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

            for (iMatrix = 0; iMatrix < cIndexedMatrices; iMatrix++)
            {
                pfxSkinnedAdjacency->SetMatrix(x_rgszMatrix[iMatrix], &rgmIndexedMatrices[iMatrix]);
            }

            pfxSkinnedAdjacency->SetMatrixArray("mWorlds", rgmIndexedMatrices, cIndexedMatrices);
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
CAdjacencyOutline::CreateEmptyOutline()
{
    GXRELEASE(m_pVertexBuffer);
    delete []m_rgaeAttributeTable;
    m_rgaeAttributeTable = NULL;

    m_caeAttributeTable = 0;

    return S_OK;
}
