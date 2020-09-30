/*//////////////////////////////////////////////////////////////////////////////
//
// File: meshop.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

LRESULT CALLBACK 
DlgProcOutput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT SplitMesh
        (
            LPD3DXMESH  pMesh,              // ASSUMPTION:  *pMesh is attribute sorted & has a valid attribute table
            DWORD       iAttrSplit,         // **ppMeshB gets the mesh comprising of this attribute range onward
            DWORD*      rgiAdjacency, 
            DWORD       optionsA, 
            DWORD       optionsB, 
            LPD3DXMESH* ppMeshA, 
            LPD3DXMESH* ppMeshB
        );

// used when searching a remaping for the new selected value
DWORD FindDWORD
    (
    DWORD dwFind, 
    DWORD *rgdwSrc, 
    DWORD cDWords
    )
{
    DWORD iDWord;
    for (iDWord = 0; iDWord < cDWords; iDWord++)
    {
        if (rgdwSrc[iDWord] == dwFind)
        {
            return iDWord;
        }
    }

    return UNUSED32;
}
void 
TrivialData::NormalizeNormals()
{
    HRESULT hr;
    LPD3DXBUFFER pbufVertRemap = NULL;
    CD3DXCrackDecl1 cd;
    DWORD cVertices;
    D3DXVECTOR3 *pvNormal;
    DWORD iVertex;
    PBYTE pvPoints;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
        
    if ((m_pmcSelectedMesh == NULL) || m_pmcSelectedMesh->bPMMeshMode || (m_pmcSelectedMesh->pMesh == NULL))
        return;

    hr = m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pvPoints);
    if (FAILED(hr))
        return;

    m_pmcSelectedMesh->pMesh->GetDeclaration(pDecl);
    cd.SetDeclaration(pDecl);

    // force normalized normals on mesh to tesselate
    cVertices = m_pmcSelectedMesh->pMesh->GetNumVertices();
    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        pvNormal = cd.PvGetNormal(cd.GetArrayElem(pvPoints, iVertex));
        D3DXVec3Normalize(pvNormal, pvNormal);
    }

    m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
    return;
}

void ConvertCarriageReturns(char *szInput, char **pszOutput)
{
    DWORD cch;
    DWORD cLineFeeds;
    char *pchSrc;
    char *pchDest;

    GXASSERT(pszOutput != NULL);

    cch = 1;  // for the null char
    cLineFeeds = 0;
    pchSrc = szInput;
    while (*pchSrc != '\0')
    {
        if (*pchSrc == '\n')
            cLineFeeds += 1;

        cch += 1;
        pchSrc++;
    }

    *pszOutput = new char[cch + cLineFeeds];
    if (*pszOutput == NULL)
        return;

    pchSrc = szInput;
    pchDest = *pszOutput;
    while (*pchSrc != '\0')
    {
        if (*pchSrc == '\n')
        {
            *pchDest = '\r';
            pchDest++;
        }

        *pchDest = *pchSrc;

        pchSrc++;
        pchDest++;
    }
    *pchDest = '\0';

    return;
}

void 
TrivialData::CleanMesh()
{
    LPD3DXMESH pMeshFixed;
    HRESULT hr;
    DWORD *pdwSwap;
    LPD3DXBUFFER pbufVertRemap = NULL;
    LPD3DXBUFFER pbufOutput = NULL;
    char *pbOutput;

    if ((m_pmcSelectedMesh == NULL) || m_pmcSelectedMesh->bPMMeshMode || (m_pmcSelectedMesh->pMesh == NULL))
        return;

    hr = D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &pMeshFixed, m_pmcSelectedMesh->rgdwAdjacency, &pbufOutput);
    if (pbufOutput)
    {
        ConvertCarriageReturns((char*)pbufOutput->GetBufferPointer(), &pbOutput);
        pvDialogData = (PVOID)pbOutput;

        DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

        delete []pbOutput;
        GXRELEASE(pbufOutput);
    }

    if (FAILED(hr))
    {
        goto e_Exit;
    }

    GXRELEASE(m_pmcSelectedMesh->pMesh);
    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
    m_pmcSelectedMesh->pMesh = pMeshFixed;
    m_pmcSelectedMesh->ptmDrawMesh = pMeshFixed;
    pMeshFixed->AddRef();

    hr = D3DXValidMesh(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &pbufOutput);
    if (pbufOutput)
    {
        ConvertCarriageReturns((char*)pbufOutput->GetBufferPointer(), &pbOutput);
        pvDialogData = (PVOID)pbOutput;

        DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

        delete []pbOutput;
    }

    if (FAILED(hr))
    {
        goto e_Exit;
    }

e_Exit:
    GXRELEASE(pbufOutput);
    return;
}

// control window to w
INT_PTR CALLBACK
DlgProcSplitMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[1024];
    char *pTmp;
    HRESULT hr;
    DWORD iSplitSize = (DWORD)(UINT_PTR)g_pData->pvDialogData;

    GXASSERT(g_pData->m_pmcSelectedMesh != NULL);
    GXASSERT(g_pData->m_pmcSelectedMesh->ptmDrawMesh != NULL);

    switch (message)
    {
    case WM_SETFONT:
        return TRUE;

    case WM_INITDIALOG:
        sprintf(szBuf, "%d", g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces());
        SetDlgItemText(hDlg, IDC_SPLITMESH_FACES, szBuf);

        sprintf(szBuf, "%d", g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumVertices());
        SetDlgItemText(hDlg, IDC_SPLITMESH_VERTICES, szBuf);

        sprintf(szBuf, "%d", iSplitSize);
        SetDlgItemText(hDlg, IDC_SPLITMESH_SPLITSIZE, szBuf);

        return TRUE;

    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_SPLITMESH_SPLITSIZE, szBuf, 256);
            iSplitSize = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            g_pData->pvDialogData = (PVOID)(UINT_PTR)iSplitSize;

            EndDialog(hDlg, 0);

            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

e_Exit:
    MessageBox(NULL, "Please enter a valid number", "SplitMesh", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

HRESULT TrivialData::SplitMesh()
{
    HRESULT hr = S_OK;

    DWORD ipattr;
    DWORD iMesh;
    DWORD dwOptions;
    DWORD iSplitSize;
    INT_PTR dwRet;
    LPD3DXBUFFER pbufMeshes = NULL;
    LPD3DXBUFFER pbufAdjacencies = NULL;
    LPD3DXMESH *rgpMeshes = NULL;
    DWORD **rgdwAdjacencies;
    DWORD cNewMeshes;
    SMeshContainer *pmcNew;
    SMeshContainer *pmcTemp;

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        if (m_pmcSelectedMesh->bPMMeshMode || m_pmcSelectedMesh->bSimplifyMode || m_pmcSelectedMesh->bNPatchMode || m_pmcSelectedMesh->bTesselateMode)
            goto e_Exit;

        pvDialogData = (PVOID)(UINT_PTR)1000;
        dwRet = DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_SPLITMESH), m_hwnd, (DLGPROC) DlgProcSplitMesh);
        if (dwRet != 0)
        {
            hr = E_FAIL;
            goto e_Exit;
        }
        iSplitSize = (DWORD)(UINT_PTR)pvDialogData;
        
        GXASSERT(m_pmcSelectedMesh->ptmDrawMesh != NULL);
        GXASSERT(m_pmcSelectedMesh->pMesh != NULL);

        GXASSERT(m_pmcSelectedMesh->pPMMesh == NULL);
        GXASSERT(m_pmcSelectedMesh->pSimpMesh == NULL);
        GXASSERT(m_pmcSelectedMesh->pMeshToTesselate == NULL);
        GXASSERT(m_pmcSelectedMesh->pPatchMesh == NULL);

        // if split size over 16 bit barrier, leave alone, otherwise remove 32bit flag
        if (iSplitSize >= UNUSED16)
            dwOptions = m_pmcSelectedMesh->pMesh->GetOptions();
        else // 16 bit, so remove 32 bit flag if present
            dwOptions = m_pmcSelectedMesh->pMesh->GetOptions() & ~D3DXMESH_32BIT;

        hr = D3DXSplitMesh(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, iSplitSize, 
                                            dwOptions, &cNewMeshes, &pbufMeshes, &pbufAdjacencies, 
                                            NULL, NULL);
        if (FAILED(hr))
            goto e_Exit;

        // do this now, otherwise the Optimize call will have problems
        m_dwFaceSelected = UNUSED32;
        m_dwVertexSelected = UNUSED32;
        UpdateSelectionInfo();

        rgpMeshes = (LPD3DXMESH*)pbufMeshes->GetBufferPointer();
        rgdwAdjacencies = (DWORD**)pbufAdjacencies->GetBufferPointer();

#if 0
        DWORD **rgpdwVertRemaps, **rgpdwFaceRemaps;
        rgpdwVertRemaps = (DWORD**)pbufVertRemaps->GetBufferPointer();
        rgpdwFaceRemaps = (DWORD**)pbufFaceRemaps->GetBufferPointer();

        {
            DXCrackFVF cfvf(m_pmcSelectedMesh->pMesh->GetFVF());
            PBYTE pbSrc;
            PBYTE pbDest;
            DWORD iMesh;
            DWORD iVert;
            m_pmcSelectedMesh->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbSrc);

            for (iMesh = 0; iMesh < cNewMeshes; iMesh++)
            {
                rgpMeshes[iMesh]->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbDest);

                for (iVert = 0; iVert < rgpMeshes[iMesh]->GetNumVertices();iVert++)
                {
                    if (memcmp(cfvf.GetArrayElem(pbDest, iVert), cfvf.GetArrayElem(pbSrc, rgpdwVertRemaps[iMesh][iVert]), cfvf.m_cBytesPerVertex) != 0)
                    {
                        // vertex remap is wrong!
                        GXASSERT(0);
                    }

                }

                rgpMeshes[iMesh]->UnlockVertexBuffer();
            }

            m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
        }
#endif

        // remove references to the original mesh from the mesh container
        //   that is being reused
        GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
        GXRELEASE(m_pmcSelectedMesh->pMesh);
        delete []m_pmcSelectedMesh->rgdwAdjacency;
        m_pmcSelectedMesh->rgdwAdjacency = NULL;


        m_pmcSelectedMesh->ptmDrawMesh = rgpMeshes[0];
        m_pmcSelectedMesh->ptmDrawMesh->AddRef();
        m_pmcSelectedMesh->pMesh = rgpMeshes[0];
        m_pmcSelectedMesh->pMesh->AddRef();
        m_pmcSelectedMesh->rgdwAdjacency = new DWORD[rgpMeshes[0]->GetNumFaces()*3];
        if (m_pmcSelectedMesh->rgdwAdjacency == NULL) 
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        memcpy(m_pmcSelectedMesh->rgdwAdjacency, rgdwAdjacencies[0], sizeof(DWORD)*3*rgpMeshes[0]->GetNumFaces());

        Optimize(D3DXMESHOPT_ATTRSORT);

        // link in all the new mesh containers
        for (iMesh = 1; iMesh < cNewMeshes; iMesh++)
        {
            pmcNew = new SMeshContainer();
            if (pmcNew == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            pmcNew->ptmDrawMesh = rgpMeshes[iMesh];
            pmcNew->ptmDrawMesh->AddRef();
            pmcNew->pMesh = rgpMeshes[iMesh];
            pmcNew->pMesh->AddRef();
            pmcNew->rgdwAdjacency = new DWORD[rgpMeshes[iMesh]->GetNumFaces()*3];
            if (pmcNew->rgdwAdjacency == NULL) 
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            memcpy(pmcNew->rgdwAdjacency, rgdwAdjacencies[iMesh], sizeof(DWORD)*3*rgpMeshes[iMesh]->GetNumFaces());
    
            pmcTemp = m_pmcSelectedMesh;
            m_pmcSelectedMesh = pmcNew;
            Optimize(D3DXMESHOPT_ATTRSORT);
            m_pmcSelectedMesh = pmcTemp;

            // copy the attribute bundle
            {
                pmcNew->m_cAttributeGroups = m_pmcSelectedMesh->m_cAttributeGroups;
                pmcNew->NumMaterials = m_pmcSelectedMesh->NumMaterials;
                pmcNew->m_rgpfxAttributes = new LPD3DXEFFECT[pmcNew->NumMaterials];
                pmcNew->m_rgEffectInfo = new SEffectInfo[pmcNew->NumMaterials];
                pmcNew->rgMaterials = new D3DXMATERIAL[pmcNew->NumMaterials];
                if ((pmcNew->m_rgpfxAttributes == NULL) || (pmcNew->m_rgEffectInfo == NULL) || (pmcNew->rgMaterials == NULL))
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                for (ipattr = 0; ipattr < pmcNew->NumMaterials; ipattr++)
                {
                    pmcNew->rgMaterials[ipattr] = m_pmcSelectedMesh->rgMaterials[ipattr];
                    if (pmcNew->rgMaterials[ipattr].pTextureFilename != NULL)
                    {
                        DWORD cchFilename = strlen(pmcNew->rgMaterials[ipattr].pTextureFilename)+1;

                        pmcNew->rgMaterials[ipattr].pTextureFilename = new char[cchFilename];
                        if (pmcNew->rgMaterials[ipattr].pTextureFilename == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }

                        memcpy(pmcNew->rgMaterials[ipattr].pTextureFilename, m_pmcSelectedMesh->rgMaterials[ipattr].pTextureFilename, cchFilename);
                    }

                    // UNDONE UNDONE Need to replicate effects!
                    pmcNew->m_rgpfxAttributes[ipattr] = NULL;
                    memset(&pmcNew->m_rgEffectInfo[ipattr], 0xff, sizeof(SEffectInfo));
                }
            }

            pmcNew->pNextMeshContainer = m_pmcSelectedMesh->pNextMeshContainer;
            m_pmcSelectedMesh->pNextMeshContainer = pmcNew;
        }
    }

    UpdateMeshMenu();
e_Exit:

    if (rgpMeshes)
    {
        for (iMesh = 0; iMesh < cNewMeshes; iMesh++)
        {
            GXRELEASE(rgpMeshes[iMesh]);
        }
    }
    GXRELEASE(pbufMeshes);
    GXRELEASE(pbufAdjacencies);

    return hr;
}

void
TrivialData::Displace()
{
    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        // standard mesh selected, displace to your heart's content


        MessageBox(NULL, "h", "h", MB_OK);
    }
}


// -------------------------------------------------------------------------------
//  function    FindEdge
//
//   devnote    Simple helper function to determine which edge of a neighbor
//                  face, points back to the given face
//                  the pwIndices array is a three element array specifiying 
//                  neighbors to a triangle face  
//
//   returns    if < 3, then iFaceSearch is the position in the given neighbor posiistion
//                      else when >= 3, the face is not in the given array
//
template<class UINT_IDX>
inline UINT
FindEdge(UINT_IDX *rgiIndices, UINT_IDX iFaceSearch)
{
    UINT iNeighborEdge;

    // find the edge that points to this triangle in the neighbor
    for (iNeighborEdge = 0; iNeighborEdge < 3; iNeighborEdge++)
    {
        if (rgiIndices[iNeighborEdge] == iFaceSearch)
        {
            break;
        }
    }

    return iNeighborEdge;
}
#if 0
// return true if two normals are not binary equivalent
BOOL BNormalsDifferent
    (
    DXCrackFVF &cfvf, 
    PBYTE pbVertex1,
    PBYTE pbVertex2
    )
{
    return (memcmp(cfvf.PvGetNormal(pbVertex1), cfvf.PvGetNormal(pbVertex2), sizeof(D3DXVECTOR3)) != 0);
}
 
template<class UINT_IDX>
HRESULT WINAPI 
D3DXFixNPatchCreasesEx
    (
    LPD3DXMESH pMeshIn, 
    DWORD *rgdwAdjacencyIn,
    LPD3DXMESH *ppMeshOut,
    LPD3DXBUFFER *ppbufAdjacencyOut,
    UINT_IDX Bogus
    )
{
    HRESULT hr = S_OK;
    DWORD iFace;
    DWORD cFaces;
    DWORD cVertices;
    DWORD dwOptions;
    UINT_IDX *rgwIndices = NULL;
    UINT_IDX *rgwIndicesOut = NULL;
    LPDIRECT3DDEVICE9 pDevice = NULL;
    DWORD iEdge1; 
    DWORD iEdge2; 
    DWORD iNEdge1;
    DWORD iNEdge2;
    PBYTE pbVertices = NULL;
    PBYTE pbVerticesOut = NULL;
    DWORD *rgdwAttribs = NULL;
    DWORD *rgdwAttribsOut = NULL;
    DWORD iPoint;
    DWORD iNeighborPoint;
    DWORD iNeighbor;
    DWORD *rgdwAdjCur;
    DWORD cNewFaces;
    DWORD cFacesOut;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    LPD3DXMESH pMeshOut;
    DWORD iFaceOutCur;
    DWORD iFaceOutNext;
    DWORD cAttribs;

    if ((pMeshIn == NULL) || (rgdwAdjacencyIn == NULL) || (ppMeshOut == NULL))
    {
#ifdef DBG
        if (pMeshIn == NULL)
            DPF(0, "D3DXFixNPatchCreases: A mesh must be provided");
        else if (rgdwAdjacencyIn == NULL)
            DPF(0, "D3DXFixNPatchCreases: Input adjacency must be provided");
        else if (ppMeshOut == NULL)
            DPF(0, "D3DXFixNPatchCreases: Output mesh pointer must be provided");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    dwOptions = pMeshIn->GetOptions();
    if (dwOptions & D3DXMESH_WRITEONLY)
    {
        DPF(0, "D3DXFixNPatchCreases: Input mesh index/vertex buffers must not be WRITE-ONLY");
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    cFaces = pMeshIn->GetNumFaces();
    cVertices = pMeshIn->GetNumVertices();

    cfvf = DXCrackFVF(pMeshIn->GetFVF());

    hr = pMeshIn->GetDevice(&pDevice);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshIn->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&rgwIndices);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshIn->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVertices);
    if (FAILED(hr))
        goto e_Exit;

    cNewFaces = 0;
    rgdwAdjCur = rgdwAdjacencyIn;
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // if adjacent to another face, and this faces index is less, then look to see if there is a crease
            if ((rgdwAdjCur[iPoint] > iFace) && (rgdwAdjCur[iPoint] != UNUSED32))
            {
                iNeighbor = rgdwAdjCur[iPoint];
                iNeighborPoint = FindEdge(rgdwAdjacencyIn + iNeighbor*3, iFace);

                iEdge1 = iFace*3 + iPoint;
                iEdge2 = iFace*3 + (iPoint+1)%3;
                iNEdge1 = iNeighbor*3 + iNeighborPoint;
                iNEdge2 = iNeighbor*3 + (iNeighborPoint+1)%3;


                if ((rgwIndices[iEdge1] != rgwIndices[iNEdge2])
                       && BNormalsDifferent(cfvf, cfvf.GetArrayElem(pbVertices, rgwIndices[iEdge1]),
                                                  cfvf.GetArrayElem(pbVertices, rgwIndices[iNEdge2])))
                {
                    cNewFaces += 1;
                }

                if ((rgwIndices[iEdge2] != rgwIndices[iNEdge1])
                       && BNormalsDifferent(cfvf, cfvf.GetArrayElem(pbVertices, rgwIndices[iEdge2]),
                                                  cfvf.GetArrayElem(pbVertices, rgwIndices[iNEdge1])))
                {
                    cNewFaces += 1;
                }
            }
        }

        rgdwAdjCur += 3;  // move to next face
    }

    cFacesOut = cFaces + cNewFaces;

    hr = D3DXCreateMeshFVF(cFacesOut, cVertices, dwOptions, pMeshIn->GetFVF(), pDevice, &pMeshOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshOut->LockVertexBuffer(0, (LPVOID*)&pbVerticesOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshIn->LockAttributeBuffer(D3DLOCK_READONLY, &rgdwAttribs);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshOut->LockAttributeBuffer(0, &rgdwAttribsOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMeshOut->LockIndexBuffer(0, (LPVOID*)&rgwIndicesOut);
    if (FAILED(hr))
        goto e_Exit;

    memcpy(pbVerticesOut, pbVertices, cfvf.m_cBytesPerVertex * cVertices);

    rgdwAdjCur = rgdwAdjacencyIn;

    iFaceOutCur = 0;
    iFaceOutNext = 0;
    
    for (iFace = 0; iFace < cFaces; iFace++)
    {

        // copy the current face
        memcpy(rgwIndicesOut + (iFaceOutNext*3), rgwIndices + (iFace*3), sizeof(UINT_IDX)*3);
        rgdwAttribsOut[iFaceOutNext] = rgdwAttribs[iFace];

        iFaceOutNext += 1;
        
        // now decide if we need to insert any faces
        for (iPoint = 0; iPoint < 3; iPoint++)
        {
            // if adjacent to another face, and this faces index is less, then look to see if there is a crease
            if ((rgdwAdjCur[iPoint] > iFace) && (rgdwAdjCur[iPoint] != UNUSED32))
            {
                iNeighbor = rgdwAdjCur[iPoint];
                iNeighborPoint = FindEdge(rgdwAdjacencyIn + iNeighbor*3, iFace);

                iEdge1 = iFace*3 + iPoint;
                iEdge2 = iFace*3 + (iPoint+1)%3;
                iNEdge1 = iNeighbor*3 + iNeighborPoint;
                iNEdge2 = iNeighbor*3 + (iNeighborPoint+1)%3;

                if ((rgwIndices[iEdge1] != rgwIndices[iNEdge2])
                       && BNormalsDifferent(cfvf, cfvf.GetArrayElem(pbVertices, rgwIndices[iEdge1]),
                                                  cfvf.GetArrayElem(pbVertices, rgwIndices[iNEdge2])))
                {
                    rgwIndicesOut[iFaceOutNext*3 + 0] = rgwIndices[iEdge2];
                    rgwIndicesOut[iFaceOutNext*3 + 1] = rgwIndices[iEdge1];
                    rgwIndicesOut[iFaceOutNext*3 + 2] = rgwIndices[iNEdge2];
                    rgdwAttribsOut[iFaceOutNext] = rgdwAttribs[iFace];
                    iFaceOutNext += 1;
                }

                if ((rgwIndices[iEdge2] != rgwIndices[iNEdge1])
                       && BNormalsDifferent(cfvf, cfvf.GetArrayElem(pbVertices, rgwIndices[iEdge2]),
                                                  cfvf.GetArrayElem(pbVertices, rgwIndices[iNEdge1])))
                {
                    rgwIndicesOut[iFaceOutNext*3 + 0] = rgwIndices[iNEdge2];
                    rgwIndicesOut[iFaceOutNext*3 + 1] = rgwIndices[iNEdge1];
                    rgwIndicesOut[iFaceOutNext*3 + 2] = rgwIndices[iEdge2];
                    rgdwAttribsOut[iFaceOutNext] = rgdwAttribs[iFace];
                    iFaceOutNext += 1;
                }
            }
        }

        iFaceOutCur = iFaceOutNext;
        rgdwAdjCur += 3;  // move to next face
    }

    hr = pMeshIn->GetAttributeTable(NULL, &cAttribs);
    if (FAILED(hr))
        goto e_Exit;

    // if already attribute sorted, then resort now.  should be a no-op, just rebuild the table
    if (cAttribs > 0)
    {
        hr = pMeshOut->OptimizeInplace(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);
        if (FAILED(hr))
            goto e_Exit;
    }

    if (ppbufAdjacencyOut)
    {
        // adjacency not yet supported for output
        GXASSERT(0);

        hr = D3DXCreateBuffer(sizeof(DWORD)*3*cFacesOut, ppbufAdjacencyOut);
        if (FAILED(hr))
            goto e_Exit;

        // for now, set everything to UNUSED
        memset((*ppbufAdjacencyOut)->GetBufferPointer(), 0xff, sizeof(DWORD)*3*cFacesOut);
    }

    *ppMeshOut = pMeshOut;
    pMeshOut->AddRef();

e_Exit:
    if (rgwIndices != NULL)
    {
        pMeshIn->UnlockIndexBuffer();
    }
    if (pbVertices != NULL)
    {
        pMeshIn->UnlockVertexBuffer();
    }
    if (rgwIndicesOut != NULL)
    {
        pMeshOut->UnlockIndexBuffer();
    }
    if (pbVertices != NULL)
    {
        pMeshOut->UnlockVertexBuffer();
    }

    if (rgdwAttribs != NULL)
    {
        pMeshIn->UnlockAttributeBuffer();
    }
    if (rgdwAttribsOut != NULL)
    {
        pMeshOut->UnlockAttributeBuffer();
    }

    GXRELEASE(pDevice);
    GXRELEASE(pMeshOut);

    return hr;
}

HRESULT WINAPI 
D3DXFixNPatchCreases
    (
    LPD3DXMESH pMeshIn, 
    DWORD *rgdwAdjacencyIn,
    LPD3DXMESH *ppMeshOut,
    LPD3DXBUFFER *ppbufAdjacencyOut
    )
{
    unsigned int dwBogus = 0;
    unsigned short wBogus = 0;

    if (pMeshIn == NULL) 
    {
        DPF(0, "D3DXFixNPatchCreases: A mesh must be provided");
        return D3DERR_INVALIDCALL;
    }

    if (pMeshIn->GetOptions() & D3DXMESH_32BIT)
        return D3DXFixNPatchCreasesEx<unsigned int>(pMeshIn, rgdwAdjacencyIn, ppMeshOut, ppbufAdjacencyOut, dwBogus);
    else
        return D3DXFixNPatchCreasesEx<unsigned short>(pMeshIn, rgdwAdjacencyIn, ppMeshOut, ppbufAdjacencyOut, wBogus);

}

void 
TrivialData::FixNPatchCreases()
{
    HRESULT hr;
    LPD3DXMESH pMeshNew;
    LPD3DXBUFFER pbufAdjacencyNew;

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        hr = D3DXFixNPatchCreases(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &pMeshNew, &pbufAdjacencyNew);
        if (FAILED(hr))
            goto e_Exit;

        GXRELEASE(m_pmcSelectedMesh->pMesh);
        GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
        m_pmcSelectedMesh->pMesh = pMeshNew;
        m_pmcSelectedMesh->ptmDrawMesh = pMeshNew;
        m_pmcSelectedMesh->ptmDrawMesh->AddRef();

        delete m_pmcSelectedMesh->rgdwAdjacency;

        m_pmcSelectedMesh->rgdwAdjacency     = new DWORD [pbufAdjacencyNew->GetBufferSize() / sizeof(DWORD)];
        if(m_pmcSelectedMesh->rgdwAdjacency == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy(m_pmcSelectedMesh->rgdwAdjacency, pbufAdjacencyNew->GetBufferPointer(), pbufAdjacencyNew->GetBufferSize());

        m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
    }

e_Exit:
    return;
}
#endif

HRESULT TrivialData::TesselateFrame(SFrame *pframe)
{
    SMeshContainer *pmc;
    HRESULT hr;

    if (pframe == NULL)
        return S_OK;

    pmc = pframe->pmcMesh;
    while (pmc != NULL)
    {
        hr = Tesselate(pmc, FALSE);
        if (FAILED(hr))
            return hr;

        pmc = (SMeshContainer*)pmc->pNextMeshContainer;
    }

    return S_OK;
}

HRESULT TrivialData::Tesselate(SMeshContainer *pmc, BOOL bSkipAdjacencyAndEdges)
{
    HRESULT hr = S_OK;
    LPD3DXMESH pMeshOut = NULL;
    LPD3DXBUFFER pAdjacencyOut = NULL;
    DWORD iVertex;
    DWORD cVertices;
    PBYTE pvPoints;
    D3DXVECTOR3 *pvNormal;
    CD3DXCrackDecl1 cd;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    if (pmc == NULL)
        return S_OK;

    if (pmc->pPatchMesh != NULL)
    {
        LPD3DXMESH pMesh;
        D3DVERTEXELEMENT9 pDeclIn[MAX_FVF_DECL_SIZE];
        D3DVERTEXELEMENT9 pDeclOut[MAX_FVF_DECL_SIZE];
        DWORD cTriangles,cVertices,cTotTriangles,cTotVertices;

        D3DXVECTOR4 Trans = D3DXVECTOR4(0,0,1,1);

        hr = pmc->pPatchMesh->GetTessSize(pmc->cTesselateLevel + 1.0f, FALSE, &cTriangles,&cVertices);
        if(FAILED(hr))
            goto e_Exit;

        hr = pmc->pPatchMesh->GetDeclaration(pDeclIn);
        if (FAILED(hr))
            goto e_Exit;

        hr = D3DXGenerateOutputDecl(pDeclOut, pDeclIn);
        if (FAILED(hr))
            goto e_Exit;

        hr = D3DXCreateMesh(cTriangles,cVertices,D3DXMESH_MANAGED,pDeclOut,m_pDevice,&pMesh);
        if (FAILED(hr))
            goto e_Exit;

  //      pmc->pPatchMesh->ComputeAdj(.05f,100000);


        hr = pmc->pPatchMesh->Tessellate(pmc->cTesselateLevel + 1.0f, pMesh);
        if (FAILED(hr))
            goto e_Exit;

        delete []pmc->rgdwAdjacency;
        pmc->rgdwAdjacency = new DWORD[pMesh->GetNumFaces()*3];
        if (pmc->rgdwAdjacency == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        pMesh->ConvertPointRepsToAdjacency(NULL, pmc->rgdwAdjacency);

        GXRELEASE(pmc->ptmDrawMesh);
        pmc->ptmDrawMesh = pMesh;
        pmc->pMesh = pMesh;
        pMesh->AddRef();

        AdjustScrollbar();

        pmc->pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);

        if (m_pdeSelected != NULL)
            pmc->UpdateViews(m_pdeSelected);

    }
    else if (pmc->pMesh != NULL)
    {
        if (!pmc->bNPatchMode)
        {
            if ( !(pmc->pMesh->GetFVF() & D3DFVF_NORMAL) )
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }

            pmc->pMesh->GetDeclaration(pDecl);
            hr = pmc->pMesh->CloneMesh(pmc->pMesh->GetOptions()|D3DXMESH_NPATCHES,
                                    pDecl, m_pDevice, &pmc->pMeshToTesselate);
            if (FAILED(hr))
                goto e_Exit;

            GXRELEASE(pmc->pMesh);
            GXRELEASE(pmc->ptmDrawMesh);
            pmc->pMesh = pmc->pMeshToTesselate;
            pmc->pMesh->AddRef();
            pmc->ptmDrawMesh = pmc->pMeshToTesselate;
            pmc->ptmDrawMesh->AddRef();

            hr = pmc->pMeshToTesselate->LockVertexBuffer(0, (LPVOID*)&pvPoints);
            if (FAILED(hr))
                goto e_Exit;

            cd.SetDeclaration(pDecl);

            // force normalized normals on mesh to tesselate
            cVertices = pmc->pMeshToTesselate->GetNumVertices();
            for (iVertex = 0; iVertex < cVertices; iVertex++)
            {
                pvNormal = cd.PvGetNormal(cd.GetArrayElem(pvPoints, iVertex));
                D3DXVec3Normalize(pvNormal, pvNormal);
            }

            pmc->pMeshToTesselate->UnlockVertexBuffer();

            pmc->rgdwAdjacencyTesselate = pmc->rgdwAdjacency;
            pmc->rgdwAdjacency = NULL;

            pmc->cTesselateLevel = 1;
            pmc->bNPatchMode = TRUE;
        }

        //if (!m_bHWNPatches)
        {

            // only update SW skinned version if needed for drawing or not doing a "quick" normal update re-tesselate
            if (!bSkipAdjacencyAndEdges || !m_bHWNPatches)
            {
                hr = D3DXTessellateNPatches(pmc->pMeshToTesselate, 
                                        pmc->rgdwAdjacencyTesselate,
                                        (float)(pmc->cTesselateLevel + 1), 
                                        TRUE,
                                        &pMeshOut,
                                        bSkipAdjacencyAndEdges ? NULL : &pAdjacencyOut);
                if (FAILED(hr))
                {
                    // if a tesselate has succeeded, then we probably just overran the
                    //   16 to 32bit limit, so continue on at the current level
                    if (pmc->pSWTesselatedMesh != NULL)
                    {
                        hr = S_OK;
                    }

                    goto e_Exit;
                }

                if (!m_bHWNPatches)
                {
                    GXRELEASE(pmc->pMesh);
                    GXRELEASE(pmc->ptmDrawMesh);

                    // now replace the old mesh with the new one
                    pmc->pMesh = pMeshOut;
                    pmc->ptmDrawMesh = pMeshOut;
                    pMeshOut->AddRef();
                    pMeshOut->AddRef();
                }

                // keep a SW copy for view/intersection purposes
                GXRELEASE(pmc->pSWTesselatedMesh);
                pmc->pSWTesselatedMesh = pMeshOut;
            }


            if (!bSkipAdjacencyAndEdges)
            {
                // update adjacency information
                delete [] pmc->rgdwAdjacency;

                pmc->rgdwAdjacency     = new DWORD [pAdjacencyOut->GetBufferSize() / sizeof(DWORD)];
                if (pmc->rgdwAdjacency == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }

                memcpy(pmc->rgdwAdjacency, pAdjacencyOut->GetBufferPointer(), pAdjacencyOut->GetBufferSize());
                pMeshOut->OptimizeInplace(D3DXMESHOPT_ATTRSORT, pmc->rgdwAdjacency, pmc->rgdwAdjacency, NULL, NULL);

                //hr = pmc->m_aoAdjacency.Init(pmc->pMesh, pmc->rgdwAdjacency);
                //if (FAILED(hr))
                    //goto e_Exit;

                //hr = pmc->m_eoEdges.Init(pmc->pMesh, pmc->rgdwAdjacency);
                //if (FAILED(hr))
                    //goto e_Exit;

                hr = pmc->m_npoNPatchOutline.Init(pmc->pSWTesselatedMesh, pmc->cTesselateLevel + 1);
                if (FAILED(hr))
                    goto e_Exit;
            }
            // optimize the SW mesh if being used for drawing and skipping most of mesh container setup
            else if (pMeshOut != NULL)
            {
                pMeshOut->OptimizeInplace(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);
            }

            if (!bSkipAdjacencyAndEdges)
            {
                AdjustScrollbar();
            }
        }

        // display normals from the mesh to tesselate from - consistent between hw and sw n-patching
        hr = pmc->m_snNormals.Init(pmc->pMeshToTesselate, UNUSED32, m_pdeSelected->fRadius / 20.0f);
        if (FAILED(hr))
            goto e_Exit;

        pmc->ptmDrawMesh->GetAttributeTable(m_pmcSelectedMesh->m_rgaeAttributeTable, NULL);
    }


e_Exit:
    GXRELEASE(pAdjacencyOut);

    if (FAILED(hr))
    {
        pmc->bNPatchMode = FALSE;
        GXRELEASE(pmc->pMeshToTesselate);
    }
    return hr;
}

void TrivialData::ConvertSelectedMesh
    (
    DWORD dwOptions, 
    LPD3DVERTEXELEMENT9 pDecl
    )
{
    HRESULT hr;
    LPD3DXPMESH pPMeshTemp;
    LPD3DXMESH pMeshTemp;
    D3DVERTEXELEMENT9 pDeclOrig[MAX_FVF_DECL_SIZE];

    GXASSERT(m_pmcSelectedMesh->ptmDrawMesh != NULL);
    GXASSERT(!m_pmcSelectedMesh->bSimplifyMode);

    // do not change the position type, will wreak havok with skinning
//    dwFVF &= ~D3DFVF_POSITION_MASK;
//    dwFVF |= m_pmcSelectedMesh->ptmDrawMesh->GetFVF() & D3DFVF_POSITION_MASK;

    m_pmcSelectedMesh->ptmDrawMesh->GetDeclaration(pDeclOrig);

    // if no changes, then just return
    if ((dwOptions == m_pmcSelectedMesh->ptmDrawMesh->GetOptions())
        && (BIdenticalDecls(pDecl, pDeclOrig)))
        return;

    if (m_pmcSelectedMesh->bPMMeshMode)
    {
        // make a drawable image of the current simplification mesh
        hr = m_pmcSelectedMesh->pPMMesh->ClonePMesh(dwOptions,
                                        pDecl, 
                                        m_pDevice, &pPMeshTemp);
        if (FAILED(hr))
            return;

        GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
        GXRELEASE(m_pmcSelectedMesh->pPMMesh);

        m_pmcSelectedMesh->pPMMesh = pPMeshTemp;

        m_pmcSelectedMesh->ptmDrawMesh = pPMeshTemp;
        pPMeshTemp->AddRef();
    }
    else
    {
        hr = m_pmcSelectedMesh->pMesh->CloneMesh(dwOptions,
                                        pDecl, 
                                        m_pDevice, &pMeshTemp);
        if (FAILED(hr))
            return;

        GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
        GXRELEASE(m_pmcSelectedMesh->pMesh);

        m_pmcSelectedMesh->pMesh = pMeshTemp;

        m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
        pMeshTemp->AddRef();
    } 

    hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
    if (FAILED(hr))
        return;

}

void 
TrivialData::SetNumVertices(int cVerticesChange, bool bAbsolute)
{
    if (m_pmcSelectedMesh == NULL)
        return;

    BOOL bSimpMesh = m_pmcSelectedMesh->bSimplifyMode;
    BOOL bPMesh = m_pmcSelectedMesh->bPMMeshMode;
    DWORD cVerticesNew;

    // if not a LOD type of mesh, then return
    if (!bSimpMesh && !bPMesh)
        return;

    if (bAbsolute)
    {
        cVerticesNew = cVerticesChange;
    }
    else
    {
        if (bPMesh)
            cVerticesNew = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
        else
            cVerticesNew = m_pmcSelectedMesh->pSimpMesh->GetNumVertices();

        if (cVerticesChange + (int)cVerticesNew < 1)
            cVerticesNew = 1;
        else
            cVerticesNew += cVerticesChange;
    }

    if (bPMesh)
    {
        if (cVerticesNew < m_pmcSelectedMesh->m_cMinVerticesSoft)
            cVerticesNew = m_pmcSelectedMesh->m_cMinVerticesSoft;

        if (cVerticesNew > m_pmcSelectedMesh->m_cMaxVerticesSoft)
            cVerticesNew = m_pmcSelectedMesh->m_cMaxVerticesSoft;
    }

    if (bPMesh)
        m_pmcSelectedMesh->pPMMesh->SetNumVertices(cVerticesNew);
    else
        m_pmcSelectedMesh->pSimpMesh->ReduceVertices(cVerticesNew);

    if (bSimpMesh)
    {
        GXRELEASE(m_pmcSelectedMesh->pMesh);

        // make a drawable image of the current simplification mesh
        m_pmcSelectedMesh->pSimpMesh->CloneMesh(m_pmcSelectedMesh->pSimpMesh->GetOptions(),
                                        NULL, 
                                        m_pDevice, NULL, NULL, &m_pmcSelectedMesh->pMesh);

        m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
        m_pmcSelectedMesh->pMesh->AddRef();
    }

    // make certain that the selected face/vertex is still valid
    if (bPMesh && (m_dwFaceSelected != UNUSED32))
    {
        
        if ((m_dwVertexSelected >= m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].VertexStart + m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].VertexCount)
              || (m_dwFaceSelected >= m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].FaceStart + m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].FaceCount))
        {
            m_dwVertexSelected = UNUSED32;
            m_dwFaceSelected = UNUSED32;
            m_dwFaceSelectedAttr = UNUSED32;
        }
    }

    m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(m_pmcSelectedMesh->m_rgaeAttributeTable, NULL);

    if (bPMesh)
    {
        m_pmcSelectedMesh->pPMMesh->GetAdjacency(m_pmcSelectedMesh->rgdwAdjacency);
        m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
    }

    AdjustScrollbar();
}

void 
TrivialData::SetNumFaces(int cFacesChange, bool bAbsolute)
{
    BOOL bSimpMesh = m_pmcSelectedMesh->bSimplifyMode;
    BOOL bPMesh = m_pmcSelectedMesh->bPMMeshMode;
    DWORD cFacesNew;

    // if not a LOD type of mesh, then return
    if (!bSimpMesh && !bPMesh)
        return;

    if (bAbsolute)
    {
        cFacesNew = cFacesChange;
    }
    else
    {
        if (bPMesh)
            cFacesNew = m_pmcSelectedMesh->pPMMesh->GetNumFaces();
        else
            cFacesNew = m_pmcSelectedMesh->pSimpMesh->GetNumFaces();

        if (cFacesChange + (int)cFacesNew < 1)
            cFacesNew = 1;
        else
            cFacesNew += cFacesChange;
    }

    if (bPMesh)
    {
        m_pmcSelectedMesh->pPMMesh->SetNumFaces(cFacesNew);

        DWORD cVertices = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
        if (cVertices < m_pmcSelectedMesh->m_cMinVerticesSoft)
            m_pmcSelectedMesh->pPMMesh->SetNumVertices(m_pmcSelectedMesh->m_cMinVerticesSoft);
        if (cVertices > m_pmcSelectedMesh->m_cMaxVerticesSoft)
            m_pmcSelectedMesh->pPMMesh->SetNumVertices(m_pmcSelectedMesh->m_cMaxVerticesSoft);
    }
    else
        m_pmcSelectedMesh->pSimpMesh->ReduceFaces(cFacesNew);


    if (bSimpMesh)
    {
        GXRELEASE(m_pmcSelectedMesh->pMesh);

        // make a drawable image of the current simplification mesh
        m_pmcSelectedMesh->pSimpMesh->CloneMesh(m_pmcSelectedMesh->pSimpMesh->GetOptions(),
                                        NULL, 
                                        m_pDevice, NULL, NULL, &m_pmcSelectedMesh->pMesh);

        m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
        m_pmcSelectedMesh->pMesh->AddRef();
    }

    m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(m_pmcSelectedMesh->m_rgaeAttributeTable, NULL);

    AdjustScrollbar();
}

// control window to work with per vertex component weight inputs
LRESULT CALLBACK
DlgProcSimplifyInputs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[1024];
    char *pTmp;
    HRESULT hr;
    LPD3DXATTRIBUTEWEIGHTS pAttributeWeights = (LPD3DXATTRIBUTEWEIGHTS)g_pData->pvDialogData;

    GXASSERT(pAttributeWeights != NULL);

    switch (message)
    {
    case WM_SETFONT:
        return TRUE;

    case WM_INITDIALOG:
        sprintf(szBuf, "%f", pAttributeWeights->Position);
        SetDlgItemText(hDlg, IDC_POSITIONWEIGHT, szBuf);

        sprintf(szBuf, "%f", pAttributeWeights->Boundary);
        SetDlgItemText(hDlg, IDC_BOUNDARYWEIGHT, szBuf);

        sprintf(szBuf, "%f", pAttributeWeights->Normal);
        SetDlgItemText(hDlg, IDC_NORMALWEIGHT, szBuf);

        sprintf(szBuf, "%f", pAttributeWeights->Diffuse);
        SetDlgItemText(hDlg, IDC_DIFFUSEWEIGHT, szBuf);

        sprintf(szBuf, "%f", pAttributeWeights->Specular);
        SetDlgItemText(hDlg, IDC_SPECULARWEIGHT, szBuf);

        sprintf(szBuf, "%f", pAttributeWeights->Texcoord[0]);
        SetDlgItemText(hDlg, IDC_TEXTUREWEIGHT, szBuf);

        return TRUE;

    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            memset(pAttributeWeights, 0, sizeof(D3DXATTRIBUTEWEIGHTS));

            GetDlgItemText(hDlg, IDC_POSITIONWEIGHT, szBuf, 256);
            pAttributeWeights->Position = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_BOUNDARYWEIGHT, szBuf, 256);
            pAttributeWeights->Boundary = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_NORMALWEIGHT, szBuf, 256);
            pAttributeWeights->Normal = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_DIFFUSEWEIGHT, szBuf, 256);
            pAttributeWeights->Diffuse = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_SPECULARWEIGHT, szBuf, 256);
            pAttributeWeights->Specular = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_TEXTUREWEIGHT, szBuf, 256);
            pAttributeWeights->Texcoord[0] = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            // set all texture weights to the same value
            pAttributeWeights->Texcoord[1] = pAttributeWeights->Texcoord[0];
            pAttributeWeights->Texcoord[2] = pAttributeWeights->Texcoord[3] = pAttributeWeights->Texcoord[0];
            pAttributeWeights->Texcoord[4] = pAttributeWeights->Texcoord[5] = pAttributeWeights->Texcoord[0];
            pAttributeWeights->Texcoord[6] = pAttributeWeights->Texcoord[7] = pAttributeWeights->Texcoord[0];

            if (nId == IDOK)
            {
                EndDialog(hDlg, 0);
            }

            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

e_Exit:
    MessageBox(NULL, "Please enter a valid number", "Simplify", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

HRESULT 
TrivialData::ConvertMeshToSimplify()
{
    HRESULT hr = S_OK;
    LPD3DXMESH pMeshFixed;
    D3DXATTRIBUTEWEIGHTS AttributeWeights;
    INT_PTR dwRet;

    ID3DXSPMesh *ptmSimplifyMeshNew = NULL;

    GXASSERT(!m_pmcSelectedMesh->bSimplifyMode && !m_pmcSelectedMesh->bPMMeshMode);

    memset(&AttributeWeights, 0, sizeof(AttributeWeights));
    AttributeWeights.Position = 1.0f;
    AttributeWeights.Boundary = 1.0f;
    AttributeWeights.Normal = 1.0f;

    // first figure out the AttributeWeights
    pvDialogData = &AttributeWeights;
    dwRet = DialogBox(m_hInstance, (LPCTSTR) IDD_SIMPLIFYINPUTS, m_hwnd, (DLGPROC) DlgProcSimplifyInputs);
    if (dwRet != 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // cleanup the mesh before simplification
    CleanMesh();

    hr = D3DXCreateSPMesh(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &AttributeWeights, NULL, &ptmSimplifyMeshNew);
    if (FAILED(hr))
        goto e_Exit;

    // once successful, then set the simplification mesh as the one to draw

    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
    GXRELEASE(m_pmcSelectedMesh->pPMMesh);
    GXRELEASE(m_pmcSelectedMesh->pMesh);

    m_pmcSelectedMesh->pSimpMesh = ptmSimplifyMeshNew;
    ptmSimplifyMeshNew = NULL;

    //hr = m_pmcSelectedMesh->pPMMesh->QueryInterface(IID_IGXTri3DrawMesh, (PVOID*)&m_pmcSelectedMesh->ptmDrawMesh);
    //if (FAILED(hr))
        //goto e_Exit;
        
    m_pmcSelectedMesh->bPMMeshMode = false;
    m_pmcSelectedMesh->bSimplifyMode = true;

    // the maximum number of vertices changes for a simplification mesh, so
    //    remember it now so that we can give the user an idea of how far they
    //    have gone on the scrollbar
    //hr = m_pmcSelectedMesh->pPMMesh->GetMaxVertices(&m_pmcSelectedMesh->m_cMaxVertices);
    //if (FAILED(hr))
        //goto e_Exit;

    //hr = m_pmcSelectedMesh->pPMMesh->GetMinVertices(&m_pmcSelectedMesh->m_cMinVertices);
    //if (FAILED(hr))
        //goto e_Exit;

    //AdjustScrollbar();

    // make a drawable image of the current simplification mesh
    m_pmcSelectedMesh->pSimpMesh->CloneMesh(m_pmcSelectedMesh->pSimpMesh->GetOptions(),
                                    NULL, 
                                    m_pDevice, NULL, NULL, &m_pmcSelectedMesh->pMesh);

    m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
    m_pmcSelectedMesh->pMesh->AddRef();

e_Exit:
    GXRELEASE(ptmSimplifyMeshNew);

    return hr;
}


// simplify the mesh as far as it will go, and then generate a PM
HRESULT 
TrivialData::GeneratePM()
{
    HRESULT hr = S_OK;
    ID3DXPMesh *pPMeshTemp;

    // if not in simplification mode yet, then switch to it
    if (!m_pmcSelectedMesh->bSimplifyMode)
    {
        hr = ConvertMeshToSimplify();
        if (FAILED(hr))
            goto e_Exit;
    }

    // simplify the mesh
    hr = m_pmcSelectedMesh->pSimpMesh->ReduceVertices(1);
    if (FAILED(hr))
        goto e_Exit;

    // actually generate the pSimpMesh
    hr = m_pmcSelectedMesh->pSimpMesh->ClonePMesh(
                        m_pmcSelectedMesh->pSimpMesh->GetOptions(), 
                        NULL,
                        m_pDevice,
                        NULL, NULL, &pPMeshTemp);
    if (FAILED(hr))
        goto e_Exit;

    GXRELEASE(m_pmcSelectedMesh->pMesh);
    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
    GXRELEASE(m_pmcSelectedMesh->pPMMesh);
    GXRELEASE(m_pmcSelectedMesh->pSimpMesh);

    m_pmcSelectedMesh->pPMMesh = pPMeshTemp; 
    
    m_pmcSelectedMesh->ptmDrawMesh = pPMeshTemp;
    pPMeshTemp->AddRef();

    m_pmcSelectedMesh->bPMMeshMode = true;
    m_pmcSelectedMesh->bSimplifyMode = false;

    m_dwFaceSelected = UNUSED32;
    m_dwVertexSelected = UNUSED32;
    UpdateSelectionInfo();

    m_pmcSelectedMesh->m_cNumVertices = pPMeshTemp->GetNumVertices();
    m_pmcSelectedMesh->m_cMaxVerticesSoft = pPMeshTemp->GetMaxVertices();
    m_pmcSelectedMesh->m_cMinVerticesSoft = pPMeshTemp->GetMinVertices();

    // update the attribute table
    m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(m_pmcSelectedMesh->m_rgaeAttributeTable, NULL);

    m_pmcSelectedMesh->pPMMesh->GetAdjacency(m_pmcSelectedMesh->rgdwAdjacency);
    m_pmcSelectedMesh->UpdateViews(m_pdeSelected);

    // setup the scroll bar for the PM
    AdjustScrollbar();

e_Exit:
    return hr;
}


// simplify the mesh as far as it will go, and then generate a PM
HRESULT 
TrivialData::RemoveBackToBackTris()
{
    HRESULT hr = S_OK;
    DWORD iFace;
    DWORD cFaces;
    WORD *pwFaces = NULL;
    LPD3DXMESH pMeshFixed;
    DWORD iFaceBad;
    DWORD iAdjFace;
    DWORD iPoint;
    DWORD iPointInner;
    DWORD *rgdwAdjacency;
    DWORD cFacesRemoved = 0;
    char szBuf[80];

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        // not setup to handle 32bit meshes
        if (m_pmcSelectedMesh->pMesh->GetOptions() & D3DXMESH_32BIT)
        {
            hr = E_NOTIMPL;
            goto e_Exit;
        }

        hr = m_pmcSelectedMesh->pMesh->LockIndexBuffer(0, (LPVOID*)&pwFaces);
        if (FAILED(hr))
            goto e_Exit;

        cFaces = m_pmcSelectedMesh->pMesh->GetNumFaces();
        rgdwAdjacency = m_pmcSelectedMesh->rgdwAdjacency;
        for (iFace = 0; iFace < cFaces; iFace++)
        {
            if (((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 1]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                || ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                || ((rgdwAdjacency[iFace*3 + 1] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 1] != UNUSED32)))
            {
                if ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 1]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                    iFaceBad = rgdwAdjacency[iFace*3 + 0];
                else if ((rgdwAdjacency[iFace*3 + 0] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 0] != UNUSED32))
                    iFaceBad = rgdwAdjacency[iFace*3 + 0];
                else // if  ((rgdwAdjacency[iFace*3 + 1] == rgdwAdjacency[iFace*3 + 2]) && (rgdwAdjacency[iFace*3 + 1] != UNUSED32)))
                    iFaceBad = rgdwAdjacency[iFace*3 + 1];

                for (iPoint = 0; iPoint < 3; iPoint++)
                {
                    iAdjFace = rgdwAdjacency[iFaceBad*3 +iPoint];

                    pwFaces[iFaceBad*3 +iPoint] = UNUSED16;
                    rgdwAdjacency[iFaceBad*3 +iPoint] = UNUSED32;

                    for (iPointInner = 0; iPointInner < 3; iPointInner++)
                    {
                        if (rgdwAdjacency[iAdjFace*3 + iPointInner] == iFaceBad)
                        {
                            rgdwAdjacency[iAdjFace*3 + iPointInner] = UNUSED32;
                        }
                    }
                }

                cFacesRemoved += 1;
            }
        }

        Optimize(D3DXMESHOPT_ATTRSORT);

        sprintf(szBuf, "Removed %d back to back tris\n", cFacesRemoved);
        OutputDebugString(szBuf);
    }

e_Exit:
    if (pwFaces != NULL)
    {
        m_pmcSelectedMesh->pMesh->UnlockIndexBuffer();
    }

    return hr;
}

HRESULT
TrivialData::RemoveAllMeshesExceptSelectedFromFrame(SFrame *pframeCur)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;

    if ((pframeCur->pmcMesh != NULL) && (pframeCur->pmcMesh != m_pmcSelectedMesh))
    {
        delete pframeCur->pmcMesh;
        pframeCur->pmcMesh = NULL;
    }

    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = RemoveAllMeshesExceptSelectedFromFrame(pframeChild);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}

HRESULT 
TrivialData::RemoveAllMeshesExceptSelected()
{
    SDrawElement *pdeCur;
    HRESULT hr = S_OK;

    pdeCur = m_pdeHead;
    while (pdeCur != NULL)
    {
        hr = RemoveAllMeshesExceptSelectedFromFrame(pdeCur->pframeRoot);
        if (FAILED(hr))
            return hr;

        pdeCur = pdeCur->pdeNext;
    }

    return S_OK;
}

template <class UINT_IDX, unsigned int UNUSED>
class CFIFOIndexCacheQueue
{
public:
    CFIFOIndexCacheQueue(UINT cMaxElements)
        :m_rgiQueue(NULL), m_cElementsMax(cMaxElements) {}

    ~CFIFOIndexCacheQueue()
        {
            delete []m_rgiQueue;
        }

    HRESULT Init()
    {
        UINT iElement;

        if (m_rgiQueue == NULL)
        {
            m_rgiQueue = new UINT_IDX[m_cElementsMax];
            if (m_rgiQueue == NULL)
                return E_OUTOFMEMORY;
        }

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            m_rgiQueue[iElement] = UNUSED;
        }

        m_cElements = 0;
        m_iNextElement = 0;
        m_iHead = 0;

        return S_OK;
    }

    bool BElementPresent(UINT_IDX iElementTest)
    {
        UINT iElement;

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            if (m_rgiQueue[iElement] == iElementTest)
                return true;
        }

        return false;
    }

    void InsertElement(UINT_IDX iElementNew)
    {
        m_rgiQueue[m_iNextElement] = iElementNew;

        m_cElements += 1;

        m_iNextElement = m_iNextElement + 1;
        if (m_iNextElement == m_cElementsMax)
            m_iNextElement = 0;
    }

private:
    UINT m_iHead;
    UINT m_iNextElement;
    UINT m_cElements;
    UINT m_cElementsMax;

    UINT_IDX *m_rgiQueue;
};

template <class UINT_IDX, unsigned int UNUSED>
class CFIFOVertexCache
{
public:
    CFIFOVertexCache(UINT cVertexCacheSize): m_iqCache(cVertexCacheSize) {}

    HRESULT Clear()
    {
        return m_iqCache.Init();
    }

    // mark as using the vertex, if in the cache set bHit to true, else
    //   just add to the cache
    void AccessVertex(UINT_IDX iVertex, bool &bHit)
    {
        GXASSERT(iVertex != UNUSED);

        bHit = m_iqCache.BElementPresent(iVertex);
        if (!bHit)
        {
            m_iqCache.InsertElement(iVertex);
        }
    }

private:
    CFIFOIndexCacheQueue<UINT_IDX,UNUSED> m_iqCache;
};

template <class UINT_IDX, unsigned int UNUSED>
class CLRUIndexCacheQueue
{
public:
    CLRUIndexCacheQueue(UINT cMaxElements)
        :m_rgiQueue(NULL), m_rgiEntryTime(NULL), m_cElementsMax(cMaxElements) {}

    ~CLRUIndexCacheQueue()
        {
            delete []m_rgiQueue;
            delete []m_rgiEntryTime;
        }

    HRESULT Init()
    {
        UINT iElement;

        if (m_rgiQueue == NULL)
        {
            m_rgiQueue = new UINT_IDX[m_cElementsMax];
            m_rgiEntryTime = new DWORD[m_cElementsMax];
            if ((m_rgiQueue == NULL) || (m_rgiEntryTime == NULL))
                return E_OUTOFMEMORY;
        }

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            m_rgiQueue[iElement] = UNUSED;
            m_rgiEntryTime[iElement] = 0;
        }

        m_iCurTime = 0;

        return S_OK;
    }

    bool BElementPresent(UINT_IDX iElementTest)
    {
        UINT iElement;

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            if (m_rgiQueue[iElement] == iElementTest)
            {
                m_iCurTime += 1;
                m_rgiEntryTime[iElement] = m_iCurTime;
                return true;
            }
        }

        return false;
    }

    void InsertElement(UINT_IDX iElementNew)
    {
        UINT iElement;
        UINT iFoundElement = UNUSED32;
        UINT iFoundTime = UNUSED32;

        for (iElement = 0; iElement < m_cElementsMax; iElement++)
        {
            if (m_rgiQueue[iElement] == UNUSED)
            {
                iFoundElement = iElement;
                break;
            }
            else if (iFoundTime > m_rgiEntryTime[iElement])
            {
                iFoundTime = m_rgiEntryTime[iElement];
                iFoundElement = iElement;
            }
        }
        GXASSERT(iFoundElement <= m_cElementsMax);

        m_iCurTime += 1;
        m_rgiQueue[iFoundElement] = iElementNew;
        m_rgiEntryTime[iFoundElement] = m_iCurTime;
    }

private:
    UINT m_cElementsMax;
    DWORD m_iCurTime;

    UINT_IDX *m_rgiQueue;
    DWORD *m_rgiEntryTime;

};

template <class UINT_IDX, unsigned int UNUSED>
class CLRUVertexCache
{
public:
    CLRUVertexCache(UINT cVertexCacheSize): m_iqCache(cVertexCacheSize) {}

    HRESULT Clear()
    {
        return m_iqCache.Init();
    }

    // mark as using the vertex, if in the cache set bHit to true, else
    //   just add to the cache
    void AccessVertex(UINT_IDX iVertex, bool &bHit)
    {
        GXASSERT(iVertex != UNUSED);

        bHit = m_iqCache.BElementPresent(iVertex);
        if (!bHit)
        {
            m_iqCache.InsertElement(iVertex);
        }
    }

private:
    CLRUIndexCacheQueue<UINT_IDX,UNUSED> m_iqCache;
};

template<class UINT_IDX, unsigned int UNUSED>
HRESULT
SimulateCache(DWORD iVertexCacheSize, LPD3DXMESH pMesh, BOOL bLRU, char szBuf[80], UINT_IDX Dummy)
{
    HRESULT hr = S_OK;
    CFIFOVertexCache<UINT_IDX, UNUSED> svcFIFO(iVertexCacheSize);
    CLRUVertexCache<UINT_IDX, UNUSED> svcLRU(iVertexCacheSize);
    UINT iIndex;
    UINT cIndices;
    UINT iPoint;
    UINT_IDX *pwFaces;
    UINT cMisses;
    bool bHit;

    hr = svcFIFO.Clear();
    if (FAILED(hr))
        goto e_Exit;
    hr = svcLRU.Clear();
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh->LockIndexBuffer(0, (LPVOID*)&pwFaces);
    if (FAILED(hr))
        goto e_Exit;

    cIndices = pMesh->GetNumFaces() * 3;

    cMisses = 0;
    for (iIndex = 0; iIndex < cIndices; iIndex++)
    {
        if (bLRU)
            svcLRU.AccessVertex(pwFaces[iIndex], bHit);
        else
            svcFIFO.AccessVertex(pwFaces[iIndex], bHit);

        cMisses += !bHit;
    }

    pMesh->UnlockIndexBuffer();

    if (bLRU)
        sprintf(szBuf, "LRU  Cache Size: %d, Misses: %d, Miss Rate: %f\r\n", iVertexCacheSize, cMisses, (float)cMisses / (float)pMesh->GetNumVertices());
    else
        sprintf(szBuf, "FIFO Cache Size: %d, Misses: %d, Miss Rate: %f\r\n", iVertexCacheSize, cMisses, (float)cMisses / (float)pMesh->GetNumVertices());

e_Exit:
    return hr;
}

HRESULT
GenerateCacheInfo(LPD3DXMESH pMesh, char **pszBuffer)
{
    HRESULT hr = S_OK;
    char szBuf[120];
    const DWORD x_iMaxCacheSize = 16;
    const DWORD x_iMinCacheSize = 3;
    const DWORD x_cCacheSizes = x_iMaxCacheSize - x_iMinCacheSize + 1;
    UINT16 Dummy16 = 0;
    UINT32 Dummy32 = 0;
    BOOL b16BitMesh = FALSE;
    DWORD iCacheSize;

    char *szOutput = NULL;

    GXASSERT(pMesh != NULL);
    GXASSERT(pszBuffer != NULL);

    b16BitMesh = !(pMesh->GetOptions() & D3DXMESH_32BIT);

    szOutput = new char[120 * (x_cCacheSizes*2 + 2)];
    if (szOutput == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    sprintf(szOutput, "Miss rates for using optimized mesh with FIFO caches (Mesh optimized for %d entries)\r\n",12);
    for (iCacheSize = x_iMaxCacheSize; iCacheSize >= x_iMinCacheSize; iCacheSize--)
    {
        if (b16BitMesh)
            SimulateCache<UINT16,UNUSED16>(iCacheSize, pMesh, FALSE /*FIFO*/, szBuf, Dummy16);
        else
            SimulateCache<UINT32,UNUSED32>(iCacheSize, pMesh, FALSE /*FIFO*/, szBuf, Dummy32);

        strcat(szOutput, szBuf);
    }


    sprintf(szBuf, "\r\nMiss rates for using optimized mesh with LRU caches (Mesh optimized for %d entries)\r\n",12);
    strcat(szOutput, szBuf);

    for (iCacheSize = x_iMaxCacheSize; iCacheSize >= x_iMinCacheSize; iCacheSize--)
    {
        if (b16BitMesh)
            SimulateCache<UINT16,UNUSED16>(iCacheSize, pMesh, TRUE /*LRU*/, szBuf, Dummy16);
        else
            SimulateCache<UINT32,UNUSED32>(iCacheSize, pMesh, TRUE /*LRU*/, szBuf, Dummy32);

        strcat(szOutput, szBuf);
    }

    *pszBuffer = szOutput;
    szOutput = NULL;

e_Exit:
    delete []szOutput;
    return hr;
}

HRESULT 
TrivialData::SimulateCacheStuff()
{
    DWORD iHWVertexCacheSize;
    DWORD iOptVertexCacheSize;
    BOOL b16BitMesh;
    UINT32 Dummy32 = 0;
    UINT16 Dummy16 = 0;
    char szBuf[120];
    char *szOutput = NULL;
    HRESULT hr;
    LPD3DXMESH pMeshTemp;
    const DWORD x_iOptMaxCacheSize = 16;
    const DWORD x_iOptMinCacheSize = 3;
    const DWORD x_cOptCacheSizes = x_iOptMaxCacheSize - x_iOptMinCacheSize + 1;
    const DWORD x_iHWMaxCacheSize = 16;
    const DWORD x_iHWMinCacheSize = 3;
    const DWORD x_cHWCacheSizes = x_iHWMaxCacheSize - x_iHWMinCacheSize + 1;


    if ((m_pmcSelectedMesh == NULL) || m_pmcSelectedMesh->bPMMeshMode || (m_pmcSelectedMesh->pMesh == NULL))
        return S_OK;

    szOutput = new char[120 * ((x_cOptCacheSizes+3)*x_cHWCacheSizes*2)];
    if (szOutput == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    szOutput[0] = '\0';

    b16BitMesh = !(m_pmcSelectedMesh->pMesh->GetOptions() & D3DXMESH_32BIT);

    //for (iOptVertexCacheSize = 18; iOptVertexCacheSize >= 10; iOptVertexCacheSize--)
    for (iOptVertexCacheSize = x_iOptMaxCacheSize; iOptVertexCacheSize >= x_iOptMinCacheSize; iOptVertexCacheSize--)
    {
        // set the vertex cache size (works in debug only)
        sprintf(szBuf, "%d", iOptVertexCacheSize);
        //sprintf(szBuf, "%d", 3);
        SetEnvironmentVariable("D3DXVertexCacheSize", szBuf);

        //sprintf(szBuf, "%d", iOptVertexCacheSize-1);
        //SetEnvironmentVariable("D3DXOptMagicNumber", szBuf);

        sprintf(szBuf, "Optimized for vertex cache of %d entries\r\n", iOptVertexCacheSize);
        strcat(szOutput, szBuf);

        // optimize for that size
        hr = m_pmcSelectedMesh->pMesh->Optimize(D3DXMESHOPT_VERTEXCACHE, m_pmcSelectedMesh->rgdwAdjacency, 
            NULL, NULL, NULL, &pMeshTemp);
        if (FAILED(hr))
            return hr;

        // first do FIFO
        sprintf(szBuf, "\r\nMiss rates for using optimized mesh with FIFO caches (Mesh optimized for %d entries)\r\n",iOptVertexCacheSize);
        strcat(szOutput, szBuf);
        for (iHWVertexCacheSize = x_iHWMaxCacheSize; iHWVertexCacheSize >= x_iHWMinCacheSize; iHWVertexCacheSize--)
        {
            if (b16BitMesh)
                SimulateCache<UINT16,UNUSED16>(iHWVertexCacheSize, pMeshTemp, FALSE /*FIFO*/, szBuf, Dummy16);
            else
                SimulateCache<UINT32,UNUSED32>(iHWVertexCacheSize, pMeshTemp, FALSE /*FIFO*/, szBuf, Dummy32);
            strcat(szOutput, szBuf);
        }

        // next do LRU
        sprintf(szBuf, "\r\nMiss rates for using optimized mesh with LRU caches (Mesh optimized for %d entries)\r\n",iOptVertexCacheSize);
        strcat(szOutput, szBuf);
        for (iHWVertexCacheSize = x_iHWMaxCacheSize; iHWVertexCacheSize >= x_iHWMinCacheSize; iHWVertexCacheSize--)
        {
            if (b16BitMesh)
                SimulateCache<UINT16,UNUSED16>(iHWVertexCacheSize, pMeshTemp, TRUE /*LRU*/, szBuf, Dummy16);
            else
                SimulateCache<UINT32,UNUSED32>(iHWVertexCacheSize, pMeshTemp, TRUE /*LRU*/, szBuf, Dummy32);

            strcat(szOutput, szBuf);
        }

        strcat(szOutput, "\r\n");

        GXRELEASE(pMeshTemp);
    }

    // display info
    pvDialogData = (PVOID)szOutput;
    DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);


e_Exit:
    delete []szOutput;

    return S_OK;
}


template<class UINT_IDX, unsigned int UNUSED>
HRESULT
BuildHistogram(LPD3DXMESH pMesh, UINT_IDX Dummy)
{
    HRESULT hr = S_OK;
    DWORD *rgdwDistances = NULL;
    DWORD *rgdwPrevIndex = NULL;
    DWORD cNonRepeatedIndices;
    DWORD cVertices;
    DWORD cFaces;
    DWORD cIndices;
    UINT_IDX *pwIndices = NULL;
    DWORD iIndex;
    DWORD wCurIndex;
    DWORD iDistance;
    char szBuf[120];
    DWORD cSum;

    cVertices = pMesh->GetNumVertices();
    cFaces = pMesh->GetNumFaces();
    cIndices = cFaces * 3;

    hr = pMesh->LockIndexBuffer(0, (LPVOID*)&pwIndices);
    if (FAILED(hr))
        goto e_Exit;

    rgdwPrevIndex = new DWORD[cVertices];
    rgdwDistances = new DWORD[cIndices];
    if ((rgdwDistances == NULL) || (rgdwDistances == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memset(rgdwDistances, 0, sizeof(DWORD) * cIndices);
    memset(rgdwPrevIndex, 0xff, sizeof(DWORD) * cVertices);

    // first find the distances between the repeats
    for (iIndex = 0; iIndex < cIndices; iIndex++)
    {
        wCurIndex = pwIndices[iIndex];

        if (rgdwPrevIndex[wCurIndex] != UNUSED32)
        {
            iDistance = iIndex - rgdwPrevIndex[wCurIndex];
            rgdwDistances[iDistance] += 1;
        }

        rgdwPrevIndex[wCurIndex] = iIndex;
    }

    // next find the number of indices that only ocurred once
    cNonRepeatedIndices = cIndices;
    for (iIndex = 0; iIndex < cIndices; iIndex++)
    {
        cNonRepeatedIndices -= rgdwDistances[iIndex];
    }

    OutputDebugString("Cache Independent Histogram: (First n entries only)\n");
    sprintf(szBuf, "Total Indices: %d, Unique Indices: %d\n", cIndices, cNonRepeatedIndices);
    OutputDebugString(szBuf);


    OutputDebugString("Bin   Frequency\n");
    cSum = 0;
    for (iIndex = 1; iIndex < min(64, cVertices); iIndex++)
    {
        cSum += rgdwDistances[iIndex];
        sprintf(szBuf, "%d   %d    %f\n", iIndex, rgdwDistances[iIndex], (float)cSum / (float)cIndices);
        OutputDebugString(szBuf);
    }

e_Exit:
    delete []rgdwDistances;
    delete []rgdwPrevIndex;

    if (pwIndices != NULL)
    {
        pMesh->UnlockIndexBuffer();
    }

    return hr;
}

HRESULT 
TrivialData::BuildCacheHistogram()
{
    DWORD iHWVertexCacheSize;
    DWORD iOptVertexCacheSize;
    BOOL b16BitMesh;
    UINT32 Dummy32 = 0;
    UINT16 Dummy16 = 0;
    char szBuf[120];
    HRESULT hr;
    LPD3DXMESH pMeshTemp;

    if ((m_pmcSelectedMesh == NULL) || m_pmcSelectedMesh->bPMMeshMode || (m_pmcSelectedMesh->pMesh == NULL))
        return S_OK;

    b16BitMesh = !(m_pmcSelectedMesh->pMesh->GetOptions() & D3DXMESH_32BIT);

    //for (iOptVertexCacheSize = 18; iOptVertexCacheSize >= 10; iOptVertexCacheSize--)
    for (iOptVertexCacheSize = 12; iOptVertexCacheSize >= 12; iOptVertexCacheSize--)
    {
        // set the vertex cache size (works in debug only)
        //sprintf(szBuf, "%d", iOptVertexCacheSize);
        //sprintf(szBuf, "%d", 3);
        //SetEnvironmentVariable("D3DXVertexCacheSize", szBuf);

        //sprintf(szBuf, "%d", iOptVertexCacheSize-1);
        //SetEnvironmentVariable("D3DXOptMagicNumber", szBuf);

        sprintf(szBuf, "Optimized for vertex cache of %d entries\n", iOptVertexCacheSize);
        OutputDebugString(szBuf);

        // optimize for that size
        hr = m_pmcSelectedMesh->pMesh->Optimize(D3DXMESHOPT_VERTEXCACHE, m_pmcSelectedMesh->rgdwAdjacency, 
            NULL, NULL, NULL, &pMeshTemp);
        if (FAILED(hr))
            return hr;

        // first do FIFO
        sprintf(szBuf, "\nMiss rates for using optimized mesh with FIFO caches (Mesh optimized for %d entries)\n",iOptVertexCacheSize);
        OutputDebugString(szBuf);

        if (b16BitMesh)
            BuildHistogram<UINT16,UNUSED16>(pMeshTemp, Dummy16);
        else
            BuildHistogram<UINT32,UNUSED32>(pMeshTemp, Dummy32);

        OutputDebugString("\n");

        GXRELEASE(pMeshTemp);
    }

    return S_OK;
}


LRESULT CALLBACK
DlgProcWeldVertices(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[1024];
    char *pTmp;
    HRESULT hr;
    float fSkinWeightsEspilon;
    float fNormalEspilon;
    float fTextureEspilon;
    BOOL bRemoveBackToBackTris;
    BOOL bRegenerateAdjacency;
    DWORD cPreWeldVertices;
    DWORD cPostWeldVertices;
    DWORD cFacesRemoved;
    D3DXWELDEPSILONS Epsilons;
    BOOL bChangedSelInfo;
    DWORD *rgdwFaceRemap = NULL;
    LPD3DXBUFFER *ppbufVertRemapParam = NULL;
    LPD3DXBUFFER pbufVertRemap = NULL;
    DWORD dwFlags;

    switch (message)
    {
    case WM_SETFONT:
        return TRUE;

    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_WELDCONSOLE, "<no weld performed yet>");

        sprintf(szBuf, "%f", 0.01f);
        SetDlgItemText(hDlg, IDC_NORMALSEPSILON, szBuf);

        sprintf(szBuf, "%f", 0.01f);
        SetDlgItemText(hDlg, IDC_SKINWEIGHTSEPSILON, szBuf);

        sprintf(szBuf, "%f", 0.01f);
        SetDlgItemText(hDlg, IDC_TEXTUREEPSILON, szBuf);

        SendDlgItemMessage(hDlg, IDC_REMOVEBACKTOBACK, BM_SETCHECK, BST_UNCHECKED, 0);
        SendDlgItemMessage(hDlg, IDC_REGENERATEADJACENCY, BM_SETCHECK, BST_UNCHECKED, 0);
        SendDlgItemMessage(hDlg, IDC_PARTIALWELD, BM_SETCHECK, BST_UNCHECKED, 0);
        SendDlgItemMessage(hDlg, IDC_DONOTREMOVEVERTICES, BM_SETCHECK, BST_UNCHECKED, 0);
        SendDlgItemMessage(hDlg, IDC_WELDALLVERTICES, BM_SETCHECK, BST_UNCHECKED, 0);
        return TRUE;

    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case ID_APPLY:
        case IDOK:
            GetDlgItemText(hDlg, IDC_SKINWEIGHTSEPSILON, szBuf, 256);
            fSkinWeightsEspilon = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_NORMALSEPSILON, szBuf, 256);
            fNormalEspilon = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_TEXTUREEPSILON, szBuf, 256);
            fTextureEspilon = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            memset(&Epsilons, 0, sizeof(Epsilons));
            Epsilons.Position = 1.0e-6f;
            Epsilons.Normal = fNormalEspilon;
            Epsilons.BlendWeights = fSkinWeightsEspilon;
            Epsilons.Texcoord[0] = Epsilons.Texcoord[1]  = fTextureEspilon;
            Epsilons.Texcoord[2] = Epsilons.Texcoord[3]  = fTextureEspilon;
            Epsilons.Texcoord[4] = Epsilons.Texcoord[5]  = fTextureEspilon;
            Epsilons.Texcoord[6] = Epsilons.Texcoord[7]  = fTextureEspilon;    
            dwFlags = 0;
            
            bRemoveBackToBackTris = (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_REMOVEBACKTOBACK, BM_GETCHECK, 0, 0));
            bRegenerateAdjacency = (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_REGENERATEADJACENCY, BM_GETCHECK, 0, 0));

            if (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_PARTIALWELD, BM_GETCHECK, 0, 0))
                dwFlags |= D3DXWELDEPSILONS_WELDPARTIALMATCHES;

            if (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_DONOTREMOVEVERTICES, BM_GETCHECK, 0, 0))
                dwFlags |= D3DXWELDEPSILONS_DONOTREMOVEVERTICES;

            if (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_WELDALLVERTICES, BM_GETCHECK, 0, 0))
                dwFlags |= D3DXWELDEPSILONS_WELDALL;

            if (bRemoveBackToBackTris)
            {
                cFacesRemoved = g_pData->m_pmcSelectedMesh->pMesh->GetNumFaces();
                g_pData->RemoveBackToBackTris();
                cFacesRemoved -= g_pData->m_pmcSelectedMesh->pMesh->GetNumFaces();
            }
            else if (bRegenerateAdjacency)
            {
                g_pData->m_pmcSelectedMesh->pMesh->GenerateAdjacency(1.0e-6f, g_pData->m_pmcSelectedMesh->rgdwAdjacency);
            }

            cPreWeldVertices = g_pData->m_pmcSelectedMesh->pMesh->GetNumVertices();

            if (g_pData->m_dwFaceSelected != UNUSED32)
            {
                rgdwFaceRemap = new DWORD[g_pData->m_pmcSelectedMesh->pMesh->GetNumFaces()];
                if (rgdwFaceRemap == NULL)
                {
                    // if it failed, continue, but unselect the face/vertex
                    g_pData->m_dwFaceSelected = UNUSED32;
                    g_pData->m_dwVertexSelected = UNUSED32;
                }
            }

            if (g_pData->m_dwVertexSelected != UNUSED32)
            {
                ppbufVertRemapParam = &pbufVertRemap;
            }

            hr = D3DXWeldVertices(g_pData->m_pmcSelectedMesh->pMesh, dwFlags, &Epsilons,  g_pData->m_pmcSelectedMesh->rgdwAdjacency, g_pData->m_pmcSelectedMesh->rgdwAdjacency, rgdwFaceRemap, ppbufVertRemapParam);
            if (FAILED(hr))
                goto e_Exit;
    
            if (g_pData->m_dwFaceSelected != UNUSED32)
            {
                // if the face was moved, then find it
                if (rgdwFaceRemap[g_pData->m_dwFaceSelected] != g_pData->m_dwFaceSelected)
                {
                    g_pData->m_dwFaceSelected = FindDWORD(g_pData->m_dwFaceSelected, rgdwFaceRemap, g_pData->m_pmcSelectedMesh->pMesh->GetNumFaces());

                    bChangedSelInfo = TRUE;
                }
            }

            if (g_pData->m_dwVertexSelected != UNUSED32)
            {
                // if the face was moved, then find it
                if (((DWORD*)(pbufVertRemap->GetBufferPointer()))[g_pData->m_dwVertexSelected] != g_pData->m_dwVertexSelected)
                {
                    g_pData->m_dwVertexSelected = FindDWORD(g_pData->m_dwVertexSelected, 
                                            (DWORD*)(pbufVertRemap->GetBufferPointer()), 
                                            g_pData->m_pmcSelectedMesh->pMesh->GetNumVertices());

                    bChangedSelInfo = TRUE;
                }
            }

            if (bChangedSelInfo)
            {
                g_pData->UpdateSelectionInfo();
            }

            hr = g_pData->m_pmcSelectedMesh->UpdateViews(g_pData->m_pdeSelected);
            if (FAILED(hr))
                goto e_Exit;

            cPostWeldVertices = g_pData->m_pmcSelectedMesh->pMesh->GetNumVertices();

            if (bRemoveBackToBackTris)
                sprintf(szBuf, "Removed %d BackToBack triangles\r\nBefore weld: %d vertices\r\nAfter weld: %d vertices\r\n", cFacesRemoved, cPreWeldVertices, cPostWeldVertices);
            else
                sprintf(szBuf, "Before weld: %d vertices\r\nAfter weld: %d vertices\r\n", cPreWeldVertices, cPostWeldVertices);

            SetDlgItemText(hDlg, IDC_WELDCONSOLE, szBuf);


            delete []rgdwFaceRemap;
            GXRELEASE(pbufVertRemap);

            if (nId == IDOK)
            {
                EndDialog(hDlg, 0);
            }

            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

e_Exit:
    delete []rgdwFaceRemap;
    GXRELEASE(pbufVertRemap);
    MessageBox(NULL, "Please enter a valid number", "WeldVertices", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

HRESULT 
TrivialData::WeldVertices()
{
    
    if ((m_pmcSelectedMesh == NULL) || (m_pmcSelectedMesh->pMesh == NULL))
        goto e_Exit;

    DialogBox(m_hInstance, (LPCTSTR) IDD_WELDVERTICES, m_hwnd, (DLGPROC) DlgProcWeldVertices);

e_Exit:
    return S_OK;
}


// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcTestSimplify(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[65];
    char *pTmp;
    DWORD cTestInterval;

    switch (message)
    {
    case WM_INITDIALOG:
        GXASSERT(g_pData->m_pmcSelectedMesh->bSimplifyMode);
        GXASSERT(g_pData->m_pmcSelectedMesh->pSimpMesh != NULL);

        SetDlgItemText(hDlg, IDC_TESTINTERVAL, "10");

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_TESTINTERVAL, szBuf, 256);
            cTestInterval = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            *(DWORD*)g_pData->pvDialogData = cTestInterval;

            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

e_Exit:
    MessageBox(NULL, "Please enter a valid number", "Simplify", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

HRESULT 
TrivialData::TestSimplify()
{
    INT_PTR dwRet;
    DWORD cTestInterval = 0;
    DWORD cNumVertices;
    HRESULT hr = S_OK;
    
    if (m_pmcSelectedMesh == NULL) 
        goto e_Exit;

    if (!m_pmcSelectedMesh->bSimplifyMode && !m_pmcSelectedMesh->bPMMeshMode && (m_pmcSelectedMesh->pMesh != NULL))
    {
        hr = ConvertMeshToSimplify();
        if (FAILED(hr))
            goto e_Exit;
    }

    if ((!m_pmcSelectedMesh->bSimplifyMode) || (m_pmcSelectedMesh->pSimpMesh == NULL))
        goto e_Exit;

    pvDialogData = &cTestInterval;
    dwRet = DialogBox(m_hInstance, (LPCTSTR) IDD_TESTSIMPLIFY, m_hwnd, (DLGPROC) DlgProcTestSimplify);
    if (dwRet != 0)
        goto e_Exit;
    
    while (1)
    {
        cNumVertices = m_pmcSelectedMesh->pSimpMesh->GetNumVertices();

        hr = m_pmcSelectedMesh->pSimpMesh->ReduceVertices(cNumVertices - cTestInterval);
        if (FAILED(hr))
            goto e_Exit;

        AdjustScrollbar();

        m_pmcSelectedMesh->m_cNumVertices = m_pmcSelectedMesh->pSimpMesh->GetNumVertices();

        // if we hit the bottom, stop
        if (cNumVertices == m_pmcSelectedMesh->m_cNumVertices)
            break;
    }

    GXRELEASE(m_pmcSelectedMesh->pMesh);

    // make a drawable image of the current simplification mesh
    m_pmcSelectedMesh->pSimpMesh->CloneMesh(m_pmcSelectedMesh->pSimpMesh->GetOptions(),
                                    NULL, 
                                    m_pDevice, NULL, NULL, &m_pmcSelectedMesh->pMesh);

    m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
    m_pmcSelectedMesh->pMesh->AddRef();

e_Exit:
    return S_OK;
}


// optimize the mesh with the given types of optimization
void 
TrivialData::Optimize(DWORD dwFlags)
{
    LPD3DXMESH pMeshNew;
    HRESULT hr;
    DWORD *pdwSwap;
    LPD3DXBUFFER *ppbufVertRemapParam = NULL;
    LPD3DXBUFFER pbufVertRemap = NULL;
    DWORD *rgdwFaceRemap = NULL;
    BOOL bChangedSelInfo = FALSE;

    if ((m_pmcSelectedMesh == NULL) || m_pmcSelectedMesh->bPMMeshMode || (m_pmcSelectedMesh->pMesh == NULL))
        return;

    if (m_dwFaceSelected != UNUSED32)
    {
        rgdwFaceRemap = new DWORD[m_pmcSelectedMesh->pMesh->GetNumFaces()];
        if (rgdwFaceRemap == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
    }

    if (m_dwVertexSelected != UNUSED32)
    {
        ppbufVertRemapParam = &pbufVertRemap;
    }

    DWORD *rgdwAdjTemp = new DWORD[m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces()*3];
    memcpy(rgdwAdjTemp, m_pmcSelectedMesh->rgdwAdjacency, sizeof(DWORD)*m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces()*3);

    // actually perform the optimization
    hr = m_pmcSelectedMesh->pMesh->OptimizeInplace(dwFlags, m_pmcSelectedMesh->rgdwAdjacency, 
        m_pmcSelectedMesh->rgdwAdjacency, rgdwFaceRemap, ppbufVertRemapParam);
    delete []rgdwAdjTemp;
    if (FAILED(hr))
        goto e_Exit;


    if (m_dwFaceSelected != UNUSED32)
    {
        // if the face was moved, then find it
        if (rgdwFaceRemap[m_dwFaceSelected] != m_dwFaceSelected)
        {
            m_dwFaceSelected = FindDWORD(m_dwFaceSelected, rgdwFaceRemap, m_pmcSelectedMesh->pMesh->GetNumFaces());

            bChangedSelInfo = TRUE;
        }
    }

    if (m_dwVertexSelected != UNUSED32)
    {
        // if the face was moved, then find it
        if (((DWORD*)(pbufVertRemap->GetBufferPointer()))[m_dwVertexSelected] != m_dwVertexSelected)
        {
            m_dwVertexSelected = FindDWORD(m_dwVertexSelected, 
                                    (DWORD*)(pbufVertRemap->GetBufferPointer()), 
                                    m_pmcSelectedMesh->pMesh->GetNumVertices());

            bChangedSelInfo = TRUE;
        }
    }

    if (bChangedSelInfo)
    {
        UpdateSelectionInfo();
    }


    hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
    if (FAILED(hr))
        goto e_Exit;

    // if doing strip reorder or vertex cache optimization, report on cache behavior
    if (dwFlags & (D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_STRIPREORDER))
    {
        char *szBuffer = NULL;
        hr = GenerateCacheInfo(m_pmcSelectedMesh->pMesh, &szBuffer);
        if (FAILED(hr))
            goto e_Exit;

        pvDialogData = (PVOID)szBuffer;

        DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

        delete []szBuffer;
    }

e_Exit:
    delete []rgdwFaceRemap;
    GXRELEASE(pbufVertRemap);

    return;
}

HRESULT 
TrivialData::DisplayCacheBehavior()
{
    HRESULT hr = S_OK;
    char *szBuffer = NULL;

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        hr = GenerateCacheInfo(m_pmcSelectedMesh->pMesh, &szBuffer);
        if (FAILED(hr))
            return hr;

        pvDialogData = (PVOID)szBuffer;

        DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

        delete []szBuffer;
    }

    return hr;
}

void 
TrivialData::SetSoftMinLOD()
{
    // skip if not a PMesh
    if ((m_pmcSelectedMesh == NULL) || !(m_pmcSelectedMesh->bPMMeshMode))
    {
        return;
    }

    m_pmcSelectedMesh->m_cMinVerticesSoft = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
}

void 
TrivialData::SetSoftMaxLOD()
{
    // skip if not a PMesh
    if ((m_pmcSelectedMesh == NULL) || !(m_pmcSelectedMesh->bPMMeshMode))
    {
        return;
    }

    m_pmcSelectedMesh->m_cMaxVerticesSoft = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
}

void 
TrivialData::ResetSoftMinLOD()
{
    // skip if not a PMesh
    if ((m_pmcSelectedMesh == NULL) || !(m_pmcSelectedMesh->bPMMeshMode))
    {
        return;
    }

    m_pmcSelectedMesh->m_cMinVerticesSoft = m_pmcSelectedMesh->pPMMesh->GetMinVertices();
}

void 
TrivialData::ResetSoftMaxLOD()
{
    // skip if not a PMesh
    if ((m_pmcSelectedMesh == NULL) || !(m_pmcSelectedMesh->bPMMeshMode))
    {
        return;
    }

    m_pmcSelectedMesh->m_cMaxVerticesSoft = m_pmcSelectedMesh->pPMMesh->GetMaxVertices();
}

void 
TrivialData::TrimPMeshToSoftLimits()
{
    // skip if not a PMesh
    if ((m_pmcSelectedMesh == NULL) || !(m_pmcSelectedMesh->bPMMeshMode))
    {
        return;
    }

    m_pmcSelectedMesh->pPMMesh->TrimByVertices(m_pmcSelectedMesh->m_cMinVerticesSoft, m_pmcSelectedMesh->m_cMaxVerticesSoft, NULL, NULL);

    AdjustScrollbar();
}

void
TrivialData::SnapshotSelected()
{
    LPD3DXMESH pMeshTemp;
    HRESULT hr;

    if (m_pmcSelectedMesh != NULL)
    {
        if (m_pmcSelectedMesh->bSimplifyMode)
        {
            LPD3DXPMESH pPMesh = m_pmcSelectedMesh->pPMMesh;

            hr = m_pmcSelectedMesh->pSimpMesh->CloneMesh(m_pmcSelectedMesh->pSimpMesh->GetOptions(), NULL, m_pDevice, m_pmcSelectedMesh->rgdwAdjacency, NULL, &pMeshTemp);
            GXASSERT(!FAILED(hr));

            GXRELEASE(m_pmcSelectedMesh->pSimpMesh);
            GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);

            m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
            m_pmcSelectedMesh->pMesh = pMeshTemp;
            pMeshTemp->AddRef();

            m_pmcSelectedMesh->bSimplifyMode = FALSE;
            AdjustScrollbar();
 
            hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
            if (FAILED(hr))
                return;
        }
        else if (m_pmcSelectedMesh->bPMMeshMode)
        {
            LPD3DXPMESH pPMesh = m_pmcSelectedMesh->pPMMesh;

            hr = m_pmcSelectedMesh->pPMMesh->Optimize(D3DXMESH_MANAGED|D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT, m_pmcSelectedMesh->rgdwAdjacency, NULL, NULL, &pMeshTemp);
            GXASSERT(!FAILED(hr));

            GXRELEASE(m_pmcSelectedMesh->pPMMesh);
            GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);

            m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
            m_pmcSelectedMesh->pMesh = pMeshTemp;
            pMeshTemp->AddRef();

            m_pmcSelectedMesh->bPMMeshMode = FALSE;
            AdjustScrollbar();
 
            hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
            if (FAILED(hr))
                return;
        }
        else if (m_pmcSelectedMesh->bNPatchMode)
        {          
            // if HW, swap in the software tesselated mesh
            if (m_bHWNPatches)
            {
                GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                GXRELEASE(m_pmcSelectedMesh->pMesh);
                m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pSWTesselatedMesh;
                m_pmcSelectedMesh->pMesh = m_pmcSelectedMesh->pSWTesselatedMesh;
                m_pmcSelectedMesh->pMesh->AddRef();
                m_pmcSelectedMesh->pMesh->AddRef();
            }

            GXRELEASE(m_pmcSelectedMesh->pSWTesselatedMesh);
            GXRELEASE(m_pmcSelectedMesh->pMeshToTesselate);
            delete []m_pmcSelectedMesh->rgdwAdjacencyTesselate;
            m_pmcSelectedMesh->rgdwAdjacencyTesselate = NULL;

            m_pmcSelectedMesh->cTesselateLevel = 0;
            m_pmcSelectedMesh->bNPatchMode = FALSE;

            hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
            if (FAILED(hr))
                return;

            AdjustScrollbar();
        }
        else if (m_pmcSelectedMesh->bTesselateMode)
        {          
            GXRELEASE(m_pmcSelectedMesh->pPatchMesh);

            m_pmcSelectedMesh->cTesselateLevel = 0;
            m_pmcSelectedMesh->bTesselateMode = FALSE;

            hr = m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
            if (FAILED(hr))
                return;

            AdjustScrollbar();
        }
    }
}


//HRESULT CreateMaterialBuffer(LPD3DXMATERIAL rgmat, DWORD cmat, LPD3DXBUFFER *ppbufMaterials);
static HRESULT
CreateMaterialBuffer(LPD3DXMATERIAL rgmat, DWORD cmat, LPD3DXBUFFER *ppbufMaterials)
{
    HRESULT hr = S_OK;
    DWORD cbTotalStringSize;
    DWORD iCurOffset;
    DWORD imat;
    LPD3DXBUFFER pbufMaterialsOut = NULL;
    LPD3DXMATERIAL rgmatOut;
    DWORD cbName;

    // first calculate the amount of memory needed for the string buffers
    cbTotalStringSize = 0;
    for (imat = 0; imat < cmat; imat++)
    {
        if (rgmat[imat].pTextureFilename != NULL)
        {
            cbTotalStringSize += strlen(rgmat[imat].pTextureFilename) + 1;
        }
    }

    hr = D3DXCreateBuffer(sizeof(D3DXMATERIAL) * cmat + cbTotalStringSize, &pbufMaterialsOut);
    if (FAILED(hr))
        goto e_Exit;

    rgmatOut = (LPD3DXMATERIAL)pbufMaterialsOut->GetBufferPointer();

    // fist copy the materials info into the new array (note: string pointers are now incorrect)
    memcpy(rgmatOut, rgmat, sizeof(D3DXMATERIAL) * cmat);

    // start allocating strings just after the last material
    iCurOffset = sizeof(D3DXMATERIAL) * cmat;
    for (imat = 0; imat < cmat; imat++)
    {
        if (rgmat[imat].pTextureFilename != NULL)
        {
            rgmatOut[imat].pTextureFilename = ((char*)rgmatOut) + iCurOffset;

            cbName = strlen(rgmat[imat].pTextureFilename) + 1;
            memcpy(rgmatOut[imat].pTextureFilename, rgmat[imat].pTextureFilename, cbName);

            iCurOffset += cbName;
        }
    }

    GXASSERT(iCurOffset == sizeof(D3DXMATERIAL) * cmat + cbTotalStringSize);

    *ppbufMaterials = pbufMaterialsOut;
    pbufMaterialsOut = NULL;

e_Exit:
    GXRELEASE(pbufMaterialsOut);
    return hr;
}


static HRESULT
MergeMaterialBuffers
    (
    D3DXMATERIAL *pMat1, 
    DWORD cmat1, 
    D3DXMATERIAL *pMat2, 
    DWORD cmat2, 
    LPD3DXBUFFER *ppbufMaterials
    )
{
    HRESULT hr = S_OK;
    D3DXMATERIAL *rgmatTemp = NULL;
    D3DXMATERIAL *rgmat1;
    D3DXMATERIAL *rgmat2;

    if (pMat1 == NULL)
    {
        if (pMat2 == NULL)
        {
            *ppbufMaterials = NULL;
        }
        else
        {
            hr = CreateMaterialBuffer(pMat2, cmat2, ppbufMaterials);
            if (FAILED(hr))
                goto e_Exit;
        }
    }
    else if (pMat2 == NULL)
    {
        hr = CreateMaterialBuffer(pMat1, cmat1, ppbufMaterials);
        if (FAILED(hr))
            goto e_Exit;
    }
    else  // both have materials
    {
        // make an array contains the color info and pointers to the original strings
        //   that is the two arrays combined
        rgmatTemp = new D3DXMATERIAL[cmat1 + cmat2];
        if (rgmatTemp == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        memcpy(rgmatTemp, pMat1, sizeof(D3DXMATERIAL) * cmat1);
        memcpy(rgmatTemp + cmat1, pMat2, sizeof(D3DXMATERIAL) * cmat2);

        // then use the CreateMaterialBuffer call to take and make a buffer out of the "merged" array
        hr = CreateMaterialBuffer(rgmatTemp, cmat1 + cmat2, ppbufMaterials);
        if (FAILED(hr))
            goto e_Exit;
    }

e_Exit:
    delete []rgmatTemp;
    return hr;

}

static DWORD DwCombineFVFs(DWORD dwFVF1, DWORD dwFVF2)
{
    DXCrackFVF cfvf1(dwFVF1);
    DXCrackFVF cfvf2(dwFVF2);
    DWORD dwWeights;
    DWORD dwTex;
    DWORD dwOut;

    dwWeights = max(cfvf1.CWeights(), cfvf2.CWeights());
    if (dwWeights > 0)
    {
        dwWeights *= 2;
        dwWeights += 4;
        dwOut = dwWeights;
    }
    else
    {
        dwOut = D3DFVF_XYZ;
    }

    if (cfvf1.BNormal() || cfvf2.BNormal())
        dwOut |= D3DFVF_NORMAL;

    if (cfvf1.BDiffuse() || cfvf2.BDiffuse())
        dwOut |= D3DFVF_DIFFUSE;

    if (cfvf1.BSpecular() || cfvf2.BSpecular())
        dwOut |= D3DFVF_SPECULAR;

    dwTex = max(cfvf1.CTexCoords(), cfvf2.CTexCoords());
    if (dwTex > 0)
    {
        dwOut |= (dwTex << D3DFVF_TEXCOUNT_SHIFT);
    }

    return dwOut;
}

HRESULT D3DXMergeMeshes
    (
    LPD3DXMESH pMesh1,
    PDWORD rgdwAdjacency1,
    D3DXMATERIAL *pMaterials1,
    DWORD cmat1,
    LPD3DXMESH pMesh2,
    PDWORD rgdwAdjacency2,
    D3DXMATERIAL *pMaterials2,
    DWORD cmat2,
    LPDIRECT3DDEVICE9 pD3DDevice,
    LPD3DXMESH *ppMeshOut,
    LPD3DXBUFFER *ppbufAdjacencyOut,
    LPD3DXBUFFER *ppbufMaterialsOut,
    LPDWORD pcmatOut
    )
{
    HRESULT hr = S_OK;
    DWORD cVertices1;
    DWORD cVertices2;
    DWORD cVerticesOut;
    DWORD cFaces1;
    DWORD cFaces2;
    DWORD cFacesOut;
    LPD3DXMESH ptmMeshOut = NULL;
    LPD3DXBUFFER pbufAdjacencyOut = NULL;
    PDWORD rgdwAdjacencyOut;
    PDWORD rgdwAttribs1 = NULL;
    PDWORD rgdwAttribs2 = NULL;
    BOOL b16BitIndex;
    PBYTE pFacesOut = NULL;
    PDWORD rgdwAttribsOut = NULL;
    UINT iIndex;
    UINT iIndex2;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    PBYTE pFaces1 = NULL;
    PBYTE pFaces2 = NULL;
    PBYTE pvPointsOut = NULL;
    PBYTE pvPoints1 = NULL;
    PBYTE    pvPoints2 = NULL;
    UINT cBytesPerIndex;
    LPD3DXBUFFER pbufMaterialsOut = NULL;
    DWORD dwFVF;
    DWORD dwOptions;
    LPD3DXMESH pTempMesh1 = NULL;
    LPD3DXMESH pTempMesh2 = NULL;

    // need all mesh pointers
    if ((pMesh1 == NULL) || (pMesh2 == NULL) || (ppMeshOut == NULL))
    {
        DPF(0, "Internal error - D3DXMergeMesh: Invalid pMesh1, pMesh2 or ppMeshOut pointers");
        return D3DXERR_INVALIDDATA;
    }

    // number of vertices
    cVertices1 = pMesh1->GetNumVertices();
    cVertices2 = pMesh2->GetNumVertices();
    cVerticesOut = cVertices1 + cVertices2;

    // number of faces
    cFaces1 = pMesh1->GetNumFaces();
    cFaces2 = pMesh2->GetNumFaces();
    cFacesOut = cFaces1 + cFaces2;

    dwOptions = pMesh1->GetOptions();

    if ((cFacesOut >= UNUSED16) 
        || (cVerticesOut >= UNUSED16)
        || (pMesh2->GetOptions() & D3DXMESH_32BIT))
    {
        dwOptions |= D3DXMESH_32BIT;
    }
    b16BitIndex = !(dwOptions & D3DXMESH_32BIT);

    if (pMesh1->GetFVF() != pMesh2->GetFVF())
    {
        dwFVF = DwCombineFVFs(pMesh1->GetFVF(), pMesh2->GetFVF());
    }
    else
    {
        dwFVF = pMesh1->GetFVF();
    }

    if ((pMesh1->GetOptions() != dwOptions) 
        || (pMesh1->GetFVF() != dwFVF))
    {
        hr = pMesh1->CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, &pTempMesh1);
        if (FAILED(hr))
            return hr;

        // NOTE: don't release pMesh1, owned by caller, just release pTempMesh1 on exit
        pMesh1 = pTempMesh1;
    }

    if ((pMesh2->GetOptions() != dwOptions) 
        || (pMesh2->GetFVF() != dwFVF))
    {
        hr = pMesh2->CloneMeshFVF(dwOptions, dwFVF, pD3DDevice, &pTempMesh2);
        if (FAILED(hr))
            return hr;
        
        // NOTE: don't release pMesh2, owned by caller, just release pTempMesh2 on exit
        pMesh2 = pTempMesh2;
    }


    // had better have the same options
    GXASSERT((pMesh1->GetOptions() == pMesh2->GetOptions()) && (pMesh1->GetFVF() == pMesh2->GetFVF()));
    GXASSERT((pMesh1->GetOptions() == dwOptions) && (pMesh1->GetFVF() == dwFVF));
    GXASSERT(!b16BitIndex || ((cFacesOut < UNUSED16) && (cVerticesOut < UNUSED16)));
    GXASSERT((rgdwAdjacency1 != NULL) && (rgdwAdjacency2 != NULL) && (ppbufAdjacencyOut != NULL));


    hr = pMesh1->LockAttributeBuffer(D3DLOCK_READONLY, &rgdwAttribs1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockAttributeBuffer(D3DLOCK_READONLY, &rgdwAttribs2);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh1->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pFaces1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pFaces2);
    if (FAILED(hr))
        goto e_Exit;

    if (b16BitIndex)
        cBytesPerIndex = sizeof(UINT16);
    else
        cBytesPerIndex = sizeof(UINT32);

    // generate a material buffer that is two material sets concated together
    hr = MergeMaterialBuffers(pMaterials1, cmat1, pMaterials2, cmat2, &pbufMaterialsOut);
    if (FAILED(hr))
        goto e_Exit;

    //create the mesh to fill
    hr = D3DXCreateMeshFVF(cFacesOut, cVerticesOut,
                                dwOptions, dwFVF,
                                pD3DDevice, &ptmMeshOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMeshOut->LockIndexBuffer(0, (LPVOID*)&pFacesOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMeshOut->LockAttributeBuffer(0, &rgdwAttribsOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = D3DXCreateBuffer(cFacesOut * sizeof(DWORD) * 3, &pbufAdjacencyOut);
    if (FAILED(hr))
        goto e_Exit;
    rgdwAdjacencyOut = (PDWORD)pbufAdjacencyOut->GetBufferPointer();

    // now copy and remap the face data

    memcpy(pFacesOut, pFaces1, cBytesPerIndex * cFaces1 * 3);
    memcpy(rgdwAttribsOut, rgdwAttribs1, sizeof(DWORD) * cFaces1);
    for (iIndex = 0; iIndex < cFaces1*3; iIndex++)
    {
        if (rgdwAdjacency1 != NULL)
        {
            rgdwAdjacencyOut[iIndex] = rgdwAdjacency1[iIndex];
        }
        else
        {
            rgdwAdjacencyOut[iIndex] = UNUSED32;
        }
    }

    memcpy(pFacesOut + cBytesPerIndex * cFaces1 * 3, pFaces2, cBytesPerIndex * cFaces2 * 3);
    memcpy(rgdwAttribsOut + cFaces1, rgdwAttribs2, sizeof(DWORD) * cFaces2);
    for (iIndex = cFaces1 * 3, iIndex2 = 0; iIndex < cFacesOut*3; iIndex++, iIndex2++)
    {
        if (rgdwAdjacency2 != NULL)
        {
            if (rgdwAdjacency2[iIndex2] == UNUSED32)
                rgdwAdjacencyOut[iIndex] = UNUSED32;
            else
                rgdwAdjacencyOut[iIndex] = rgdwAdjacency2[iIndex2] + cFaces1;
        }
        else
        {
            rgdwAdjacencyOut[iIndex] = UNUSED32;
        }
    }


    if (b16BitIndex)
    {
        UINT16 *pFacesTemp;
        pFacesTemp = (UINT16*)pFacesOut;

        for (iIndex = cFaces1 * 3; iIndex < cFacesOut * 3; iIndex++)
        {
            if (pFacesTemp[iIndex] != UNUSED16)
                pFacesTemp[iIndex] += (UINT16)cVertices1;
        }
    }
    else
    {
        UINT32 *pFacesTemp;
        pFacesTemp = (UINT32*)pFacesOut;

        for (iIndex = cFaces1 * 3; iIndex < cFacesOut * 3; iIndex++)
        {
            if (pFacesTemp[iIndex] != UNUSED32)
                pFacesTemp[iIndex] += cVertices1;
        }
    }

    for (iIndex = cFaces1; iIndex < cFacesOut; iIndex++)
    {
        rgdwAttribsOut[iIndex] += cmat1;
    }

    // now copy the vertex data from the two buffers into the one

    hr = ptmMeshOut->LockVertexBuffer(0, (LPVOID*)&pvPointsOut);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh1->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pvPoints1);
    if (FAILED(hr))
        goto e_Exit;

    hr = pMesh2->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pvPoints2);
    if (FAILED(hr))
        goto e_Exit;


    cfvf = DXCrackFVF(dwFVF);

    // finish by copying the two sets of vertex data into the destination buffer
    memcpy(pvPointsOut, pvPoints1, cfvf.m_cBytesPerVertex * cVertices1);
    memcpy((PBYTE)pvPointsOut + cfvf.m_cBytesPerVertex * cVertices1, pvPoints2,
                            cfvf.m_cBytesPerVertex * cVertices2);

    // all finished, setup return values
    *ppMeshOut = ptmMeshOut;
    ptmMeshOut->AddRef();
    if (ppbufAdjacencyOut != NULL)
    {
        *ppbufAdjacencyOut = pbufAdjacencyOut;
        pbufAdjacencyOut = NULL;
    }
    if (ppbufMaterialsOut != NULL)
    {
        *ppbufMaterialsOut = pbufMaterialsOut;
        pbufMaterialsOut = NULL;
    }
    if (pcmatOut != NULL)
        *pcmatOut = cmat1 + cmat2;

//    GXASSERT(CheckAdjacency(rgdwAdjacencyOut, cFacesOut));

e_Exit:
    if (pFacesOut != NULL)
    {
        ptmMeshOut->UnlockIndexBuffer();
    }
    if (rgdwAttribsOut != NULL)
    {
        ptmMeshOut->UnlockAttributeBuffer();
    }
    if (rgdwAttribs1 != NULL)
    {
        pMesh1->UnlockAttributeBuffer();
    }
    if (rgdwAttribs2 != NULL)
    {
        pMesh2->UnlockAttributeBuffer();
    }

    if (pFaces1 != NULL)
    {
        pMesh1->UnlockIndexBuffer();
    }
    if (pFaces2 != NULL)
    {
        pMesh2->UnlockIndexBuffer();
    }

    if (pvPointsOut != NULL)
    {
        ptmMeshOut->UnlockVertexBuffer();
    }
    if (pvPoints1 != NULL)
    {
        pMesh1->UnlockVertexBuffer();
    }
    if (pvPoints2 != NULL)
    {
        pMesh2->UnlockVertexBuffer();
    }

    GXRELEASE(ptmMeshOut);
    GXRELEASE(pbufAdjacencyOut);
    GXRELEASE(pbufMaterialsOut);

    GXRELEASE(pTempMesh1);
    GXRELEASE(pTempMesh2);

    return hr;
}


HRESULT
MergeMeshes
    (
    SFrame *pframe, 
    LPDIRECT3DDEVICE9 pDevice,    
    LPD3DXMESH *ppMeshMerged, 
    LPD3DXBUFFER *ppbufAdjacencyMerged,
    LPD3DXBUFFER *ppbufMaterialsMerged,
    DWORD *pcMaterialsMerged
    )
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcCur = pframe->pmcMesh;
    SFrame *pframeCur;
    DWORD cFaces;
    LPD3DXMESH pMeshOut;
    LPD3DXBUFFER pbufAdjacencyOut;
    LPD3DXBUFFER pbufMaterialsOut;
    DWORD cMatOut;
    DWORD cVertices;
    DWORD iVert;
    DXCrackFVF cfvf(D3DFVF_XYZ);
    PBYTE pvPoints;
    PBYTE pvPoint;
    D3DXVECTOR3 *pvVertexCur;
    LPD3DXMESH pMeshTemp = NULL;
    LPDIRECT3DDEVICE9 pD3DDevice;

    while (pmcCur != NULL)
    {
        if (pmcCur->pMesh != NULL)
        {
            cfvf = DXCrackFVF(pmcCur->pMesh->GetFVF());

            hr = pmcCur->pMesh->GetDevice(&pD3DDevice);
            if (FAILED(hr))
                goto e_Exit;
            pD3DDevice->Release();

            // make a temp mesh to modify
            hr = pmcCur->pMesh->CloneMeshFVF(pmcCur->pMesh->GetOptions(), cfvf.m_dwFVF, pD3DDevice, &pMeshTemp);
            if (FAILED(hr))
                goto e_Exit;

    // first transform into world space
            hr = pMeshTemp->LockVertexBuffer(0, (LPVOID*)&pvPoints);
            if (FAILED(hr))
                goto e_Exit;

            cVertices = pMeshTemp->GetNumVertices();
            for (iVert = 0; iVert < cVertices; iVert++)
            {
                pvPoint = cfvf.GetArrayElem(pvPoints, iVert);

                pvVertexCur = cfvf.PvGetPosition(pvPoint);

                D3DXVec3TransformCoord(pvVertexCur, pvVertexCur, &pframe->matCombined);

                if (cfvf.BNormal())
                {
                    pvVertexCur = cfvf.PvGetNormal(pvPoint);

                    D3DXVec3TransformNormal(pvVertexCur, pvVertexCur, &pframe->matCombined);
                    D3DXVec3Normalize(pvVertexCur, pvVertexCur);
                }
            }
            pMeshTemp->UnlockVertexBuffer();

    // now merge with the other mesh data - either first - just addref
            // or if nth, use merge function

            if (*ppMeshMerged == NULL)
            {
                *ppMeshMerged = pMeshTemp;
                pMeshTemp->AddRef();

                cFaces = pMeshTemp->GetNumFaces();

                hr = D3DXCreateBuffer(cFaces * sizeof(DWORD) * 3, ppbufAdjacencyMerged);
                if (FAILED(hr))
                    goto e_Exit;
                memcpy((*ppbufAdjacencyMerged)->GetBufferPointer(), pmcCur->rgdwAdjacency, sizeof(DWORD)*3*cFaces);

                hr = CreateMaterialBuffer(pmcCur->rgMaterials, pmcCur->NumMaterials, ppbufMaterialsMerged);
                if (FAILED(hr))
                    goto e_Exit;
                *pcMaterialsMerged = pmcCur->NumMaterials;
            }
            else
            {
                hr = D3DXMergeMeshes
                    (
                    pMeshTemp,
                    pmcCur->rgdwAdjacency,
                    pmcCur->rgMaterials,
                    pmcCur->NumMaterials,
                    *ppMeshMerged,
                    (DWORD*)(*ppbufAdjacencyMerged)->GetBufferPointer(),
                    (D3DXMATERIAL*)(*ppbufMaterialsMerged)->GetBufferPointer(),
                    *pcMaterialsMerged,
                    pDevice,
                    &pMeshOut,
                    &pbufAdjacencyOut,
                    &pbufMaterialsOut,
                    &cMatOut
                    );
                if (FAILED(hr))
                    goto e_Exit;

                GXRELEASE(*ppMeshMerged);
                GXRELEASE(*ppbufAdjacencyMerged);
                GXRELEASE(*ppbufMaterialsMerged);

                *ppMeshMerged = pMeshOut;
                *ppbufAdjacencyMerged = pbufAdjacencyOut;
                *ppbufMaterialsMerged = pbufMaterialsOut;
                *pcMaterialsMerged = cMatOut;
            }

            GXRELEASE(pMeshTemp);
        }


        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = MergeMeshes(pframeCur, pDevice, ppMeshMerged, ppbufAdjacencyMerged, ppbufMaterialsMerged, pcMaterialsMerged);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    GXRELEASE(pMeshTemp);
    return hr;
}

void
TrivialData::MergeMeshes()
{
    HRESULT hr = S_OK;;
    LPD3DXMESH pMeshMerged = NULL;
    LPD3DXBUFFER pbufAdjacencyMerged = NULL;
    LPD3DXBUFFER pbufMaterialsMerged = NULL;
    DWORD cMaterialsMerged;

    if (m_pdeSelected != NULL)
    {
        hr = ::MergeMeshes(m_pdeSelected->pframeRoot, m_pDevice,    
                                &pMeshMerged, &pbufAdjacencyMerged,
                                &pbufMaterialsMerged, &cMaterialsMerged);
        if (FAILED(hr))
            goto e_Exit;
        if (pMeshMerged == NULL)
        {
            MessageBox(m_hwnd, "No mesh data found to collapse, data might be present as PMeshes that cannot be collapsed.  Snapshot must be called first.", "CollapseMeshes Failed", MB_OK);
            goto e_Exit;
        }

        hr = AddMeshToDrawList("", pMeshMerged, NULL, pbufAdjacencyMerged, pbufMaterialsMerged, NULL, cMaterialsMerged);
        if (FAILED(hr))
            goto e_Exit;

        m_dwFaceSelected = UNUSED32;
        m_dwVertexSelected = UNUSED32;
        UpdateSelectionInfo();
    }

e_Exit:
    GXRELEASE(pMeshMerged);
    GXRELEASE(pbufAdjacencyMerged);
    GXRELEASE(pbufMaterialsMerged);

    return;
}
HRESULT
SMeshContainer::UpdateSkinInfo()
{
    HRESULT hr = S_OK;
    LPD3DXSKININFO pSkinInfoTemp = NULL;
    LPDIRECT3DDEVICE9 pD3DDevice = NULL;
    DWORD dwFVFNoWeights;
    UINT iBone, cBones;
    LPD3DXBONECOMBINATION pBoneComb;
    DWORD *rgdwAttribs;
    UINT iAttrib;
    UINT iFaceStart;
    UINT iFaceEnd;
    UINT iFace;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    // the SkinInfo matches the mesh if doing software skinning
    if (m_Method == SOFTWARE)
        goto e_Exit;

    hr = pMesh->GetDevice(&pD3DDevice);
    if (FAILED(hr))
        goto e_Exit;

    GXRELEASE(m_pOrigMesh);

    pMesh->GetDeclaration(pDecl);

    // drop all the weights from the decl
    RemoveDeclElement(D3DDECLUSAGE_BLENDWEIGHT, 0, pDecl);
    RemoveDeclElement(D3DDECLUSAGE_BLENDINDICES, 0, pDecl);

    hr = pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, pDecl, pD3DDevice, &m_pOrigMesh);
    if (FAILED(hr))
        goto e_Exit;

    m_pOrigMesh->LockAttributeBuffer(0, &rgdwAttribs);

    pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationBuf->GetBufferPointer());
    for (iAttrib = 0; iAttrib < m_cAttributeGroups; iAttrib++)
    {
        iFaceStart = pBoneComb[iAttrib].FaceStart;
        iFaceEnd = iFaceStart + pBoneComb[iAttrib].FaceCount;
        for (iFace = iFaceStart; iFace < iFaceEnd; iFace++)
        {
            rgdwAttribs[iFace] = pBoneComb[iAttrib].AttribId;
        }
    }

    m_pOrigMesh->UnlockAttributeBuffer();

    hr = D3DXCreateSkinInfoFromBlendedMesh(
                            pMesh, 
                            pSkinInfo->GetNumBones(), 
                            (LPD3DXBONECOMBINATION)(m_pBoneCombinationBuf->GetBufferPointer()), 
                            &pSkinInfoTemp);
    if (FAILED(hr))
        goto e_Exit;

    // copy the bone names and offset matrices from one skin info to the other
    cBones = pSkinInfo->GetNumBones();
    for (iBone = 0; iBone < cBones; iBone++)
    {
        pSkinInfoTemp->SetBoneName(iBone, pSkinInfo->GetBoneName(iBone));
        pSkinInfoTemp->SetBoneOffsetMatrix(iBone, pSkinInfo->GetBoneOffsetMatrix(iBone));
    }

    // swap from the old to the new skin info
    pSkinInfo->Release();
    pSkinInfo = pSkinInfoTemp;
    pSkinInfoTemp = NULL;

e_Exit:
    GXRELEASE(pSkinInfoTemp);
    GXRELEASE(pD3DDevice);
    return hr;
}

HRESULT
SMeshContainer::UpdateViews
    (
    SDrawElement *pde
    )
{
    HRESULT hr;
    DWORD iTexCoord;
    DWORD ipattr;
    LPD3DXBONECOMBINATION rgBoneComb;
    LPD3DXSKININFO pSkinInfoTemp;
    LPDIRECT3DDEVICE9 pD3DDevice;


    // first update the skin mesh if present
    if ((pSkinInfo != NULL) && (m_Method != SOFTWARE) && (pMesh != NULL))
    {
		ptmDrawMesh->GetAttributeTable(m_rgaeAttributeTable, NULL);

		// some ops want to know if there is a skinned mesh somewhere
        pde->bSkinnedMeshInHeirarchy = TRUE;

        // now update the bone combination table
        rgBoneComb = (LPD3DXBONECOMBINATION)(m_pBoneCombinationBuf->GetBufferPointer());
        for (ipattr = 0; ipattr < m_cAttributeGroups; ipattr++)
        {
            rgBoneComb[ipattr].FaceStart = m_rgaeAttributeTable[ipattr].FaceStart;
            rgBoneComb[ipattr].FaceCount = m_rgaeAttributeTable[ipattr].FaceCount;
            rgBoneComb[ipattr].VertexStart = m_rgaeAttributeTable[ipattr].VertexStart;
            rgBoneComb[ipattr].VertexCount= m_rgaeAttributeTable[ipattr].VertexCount;
        }

        hr = UpdateSkinInfo();
        if (FAILED(hr))
            return hr;

        GXRELEASE(m_pSkinnedMesh);


        hr = m_pOrigMesh->GetDevice(&pD3DDevice);
        if (FAILED(hr))
            return hr;

        // for picking purposes, we always need a sysmem mesh to use software skinning with
        //   same as orig mesh.  orig mesh is skinned into this mesh for picking
        hr = m_pOrigMesh->CloneMesh(D3DXMESH_SYSTEMMEM, NULL,
                                              pD3DDevice, &m_pSkinnedMesh);
        pD3DDevice->Release();
        if (FAILED(hr))
            return hr;
    }

    hr = m_aoAdjacency.Init(ptmDrawMesh, rgdwAdjacency);
    if (FAILED(hr))
        return hr;

    hr = m_eoEdges.Init(ptmDrawMesh, rgdwAdjacency);
    if (FAILED(hr))
        return hr;

    hr = m_snNormals.Init(ptmDrawMesh, UNUSED32, pde->fRadius / 20.0f);
    if (FAILED(hr))
        return hr;

    hr = m_soStrips.Init(ptmDrawMesh);
    if (FAILED(hr))
        return hr;

    for (iTexCoord = 0; iTexCoord < x_cpsnTexCoords; iTexCoord++)
    {
        // if already created (viewed at some point), or now viewed, but not created
        if ((m_rgpsnTexCoords[iTexCoord] != NULL) || (pde->dwTexCoordsCalculated & (1<<iTexCoord)))
        {
            // allocate a view if one is not present already
            if (m_rgpsnTexCoords[iTexCoord] == NULL)
            {
                m_rgpsnTexCoords[iTexCoord] = new CShowNormals();
                if (m_rgpsnTexCoords[iTexCoord] == NULL)
                    return E_OUTOFMEMORY;
            }


            hr = m_rgpsnTexCoords[iTexCoord]->Init(ptmDrawMesh, iTexCoord, pde->fRadius / 20.0f);
            if (FAILED(hr))
                return hr;            
        }
    }

    // if npatching, initialize npatch edge mode view
    if (bNPatchMode)
    {
        hr = m_npoNPatchOutline.Init(pSWTesselatedMesh, cTesselateLevel+1);
        if (FAILED(hr))
            return hr;
    }
    else
    {
        hr = m_npoNPatchOutline.CreateEmptyOutline();
        if (FAILED(hr))
            return hr;
    }
 
    return S_OK;
}

void
TrivialData::ComputeNormals()
{
    HRESULT hr = S_OK;

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->ptmDrawMesh != NULL))
    {
        hr = D3DXComputeNormals(m_pmcSelectedMesh->ptmDrawMesh, NULL);
        GXASSERT(!FAILED(hr));

        m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
    }
}

void
TrivialData::ValidateMesh()
{
    HRESULT hr = S_OK;
    LPD3DXMESH pMeshFixed = NULL;
    LPD3DXBUFFER pbufOutput = NULL;
    char *pbOutput;

    if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
    {
        hr = D3DXValidMesh(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &pbufOutput);
        if (pbufOutput)
        {
            ConvertCarriageReturns((char*)pbufOutput->GetBufferPointer(), &pbOutput);
            pvDialogData = (PVOID)pbOutput;

            DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

            delete []pbOutput;

            // if failed, then ask if clean mesh should be called
            if (IDYES == MessageBox(m_hwnd, "The selected mesh failed D3DXValidMesh, do you want to call D3DXCleanMesh?", "Valid Mesh failed", MB_YESNO) )
            {
                GXRELEASE(pbufOutput);
                hr = D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency, &pMeshFixed, m_pmcSelectedMesh->rgdwAdjacency, &pbufOutput);
                if (pbufOutput)
                {
                    ConvertCarriageReturns((char*)pbufOutput->GetBufferPointer(), &pbOutput);
                    pvDialogData = (PVOID)pbOutput;

                    DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);

                    delete []pbOutput;
                    GXRELEASE(pbufOutput);
                }

                if (FAILED(hr))
                {
                    MessageBox(m_hwnd, "Clean mesh failed without errors/warnings - likely due to out of memory", "Clean Mesh failed", MB_OK);
                }
                else
                {
                    GXRELEASE(m_pmcSelectedMesh->pMesh);
                    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                    m_pmcSelectedMesh->ptmDrawMesh = pMeshFixed;
                    m_pmcSelectedMesh->pMesh = pMeshFixed;
                    pMeshFixed->AddRef();
                    pMeshFixed = NULL;
                }
            }
        }
        else if (FAILED(hr))
        {   
            MessageBox(m_hwnd, "Valid mesh failed without errors/warnings - likely due to invalid parameters", "Valid Mesh failed!", MB_OK);
        }
        else
        {
            MessageBox(m_hwnd, "The selected mesh is valid.", "Valid Mesh succeeded!", MB_OK);
        }

    }

    GXRELEASE(pbufOutput);
}
