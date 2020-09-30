/*//////////////////////////////////////////////////////////////////////////////
//
// File: wndproc.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

// callbacks defined in DlgProc.cpp
LRESULT CALLBACK DlgProcAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcSimplify(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcSimplifyFaces(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcProperties(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcFaceInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcVertexInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcMeshInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProcOutput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT GenerateMesh(SMeshContainer *pmcMesh);

LRESULT
TrivialData::WndProcCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return g_pData->OnMessage(g_pData->m_hwnd, msg, wparam, lparam);
}

LRESULT
TrivialData::OnMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HRESULT hr;
    const float x_fSlackDelta = 0.05f;

    RECT r;
    switch (msg)
    {
        case WM_CREATE:
            // UNDONE UNDONE , it would be really nice to get this message
        return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            return 0;

        case WM_SETCURSOR:
#if 0
            if (
                 bFullScreen //&&
                 //bActive &&
                 //! bPaused
                 )
            {
                SetCursor( NULL );
                return 1;
            }
#endif
            break;

        case WM_TIMER:
            break;

        case WM_KEYDOWN:
            if ( VK_ESCAPE == (int) wparam )
                return 1;
#if 0
            else if ( 'k' == wparam || 'K' == wparam )
            {
                FixNPatchCreases();
            }
#endif
            else if ('1' == wparam)
            {
#if 0
                if (m_pmcSelectedMesh != NULL && m_pmcSelectedMesh->pMesh != NULL)
                {
                    D3DXATTRIBUTERANGE rgAttribTable[10];
                    DWORD caeAttribTable;

                    m_pmcSelectedMesh->pMesh->GetAttributeTable(rgAttribTable, &caeAttribTable);

                    m_pmcSelectedMesh->pMesh->SetAttributeTable(rgAttribTable, caeAttribTable);
                }

                D3DVERTEXELEMENT9 pDecl1[18];
                D3DVERTEXELEMENT9 pDecl2[18];
                D3DVERTEXELEMENT9 *pCur;
                LPD3DXMESH pMeshTemp;

                if (m_pmcSelectedMesh != NULL && m_pmcSelectedMesh->pMesh != NULL)
                {
                    m_pmcSelectedMesh->pMesh->GetDeclaration(pDecl1);
                    m_pmcSelectedMesh->pMesh->GetDeclaration(pDecl2);

                    pCur = pDecl2;
                    while (pCur->Stream != 0xff)
                    {
                        if (pCur->Usage == D3DDECLUSAGE_NORMAL)
                        {
                            pCur->Type = D3DDECLTYPE_SHORT4N;
                        }
                    pCur++;                   
                    }

                    // convert normals to different format
                    m_pmcSelectedMesh->pMesh->CloneMesh(
                                        m_pmcSelectedMesh->pMesh->GetOptions(), 
                                        pDecl2,
                                        m_pDevice, &pMeshTemp);

                    // now convert them back to Float3
                    GXRELEASE(m_pmcSelectedMesh->pMesh);
                    pMeshTemp->CloneMesh(
                                        pMeshTemp->GetOptions(), 
                                        pDecl1,
                                        m_pDevice, &m_pmcSelectedMesh->pMesh);

                    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                    m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
                    m_pmcSelectedMesh->ptmDrawMesh->AddRef();
                }
#endif
                if ((m_pdeSelected != NULL) && (m_pdeSelected->m_pAnimMixer != NULL))
                {
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(0, 2.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(1, 2.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                }
            }
            else if ('2' == wparam)
            {
                if ((m_pdeSelected != NULL) && (m_pdeSelected->m_pAnimMixer != NULL))
                {
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(0, 1.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(1, 1.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                }
            }
            else if ('3' == wparam)
            {
                if ((m_pdeSelected != NULL) && (m_pdeSelected->m_pAnimMixer != NULL))
                {
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(0, 0.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                    m_pdeSelected->m_pAnimMixer->KeyTrackSpeed(1, 0.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 1.0f, D3DXTRANSITION_LINEAR);
                }
            }
            else if ('4' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->KeyTrackWeight(0, 0.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 0.2f, D3DXTRANSITION_LINEAR);
                m_pdeSelected->m_pAnimMixer->KeyTrackWeight(1, 1.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 0.2f, D3DXTRANSITION_LINEAR);
            }
            else if ('5' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->KeyTrackWeight(0, 1.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 0.2f, D3DXTRANSITION_LINEAR);
                m_pdeSelected->m_pAnimMixer->KeyTrackWeight(1, 0.0f, m_pdeSelected->m_pAnimMixer->GetTime(), 0.2f, D3DXTRANSITION_LINEAR);
            }
            else if ('6' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->SetTrackEnable(1, TRUE);
                m_pdeSelected->m_pAnimMixer->SetTrackSpeed(1, 0.0f);
            }
            else if ('7' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->SetTrackWeight(0, 1.0f);
                m_pdeSelected->m_pAnimMixer->SetTrackWeight(1, 0.0001f);
            }
            else if ('8' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->SetTrackWeight(0, 1.0f);
                m_pdeSelected->m_pAnimMixer->SetTrackWeight(1, 0.0f);
            }
            else if ('9' == wparam)
            {
                m_pdeSelected->m_pAnimMixer->SetTrackPosition(0, 0.0f);
            }
            else if ('0' == wparam)
            {
                //m_pdeSelected->m_pAnimMixer->SetTrackAnimTime(0, (FLOAT)m_pdeSelected->m_rgpAnimSets[0]->m_pAnimSet->GetPeriod()-0.00001f);
            }
            else if (('h' == wparam) || ('H' == wparam))
            {
                D3DVERTEXELEMENT9 pDeclaration[20];
                DWORD dwFVF;
                DWORD dwFVFOrig = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

                D3DXDeclaratorFromFVF(dwFVFOrig, pDeclaration);
                D3DXFVFFromDeclarator(pDeclaration, &dwFVF);
                GXASSERT(dwFVF == dwFVFOrig);
            }


#if 0
            else if ( 'h' == wparam || 'H' == wparam )
            {
                DWORD cIndices;
                LPD3DXBUFFER pbufTemp;

                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
                {
                    if (0)
                    {
                        m_pmcSelectedMesh->m_rgcStripIndices = new DWORD[m_pmcSelectedMesh->m_cAttributeGroups];
                        m_pmcSelectedMesh->m_rgpStrips = new LPDIRECT3DINDEXBUFFER9[m_pmcSelectedMesh->m_cAttributeGroups];
                        DWORD iAttrib;
                        for (iAttrib = 0; iAttrib < m_pmcSelectedMesh->m_cAttributeGroups; iAttrib++)
                        {
                            hr = D3DXConvertMeshSubsetToSingleStrip(m_pmcSelectedMesh->pMesh, iAttrib, D3DXMESH_IB_MANAGED, &(m_pmcSelectedMesh->m_rgpStrips[iAttrib]), &(m_pmcSelectedMesh->m_rgcStripIndices[iAttrib]));
                            GXASSERT(!FAILED(hr));
                        }
                    }
                    else
                    {
                        m_pmcSelectedMesh->m_rgrgcStripCounts = new DWORD*[m_pmcSelectedMesh->m_cAttributeGroups];
                        m_pmcSelectedMesh->m_rgcStrips = new DWORD[m_pmcSelectedMesh->m_cAttributeGroups];
                        m_pmcSelectedMesh->m_rgpStrips = new LPDIRECT3DINDEXBUFFER9[m_pmcSelectedMesh->m_cAttributeGroups];
                        DWORD iAttrib;
                        for (iAttrib = 0; iAttrib < m_pmcSelectedMesh->m_cAttributeGroups; iAttrib++)
                        {
                            hr = D3DXConvertMeshSubsetToStrips(m_pmcSelectedMesh->pMesh, iAttrib, D3DXMESH_IB_MANAGED, &(m_pmcSelectedMesh->m_rgpStrips[iAttrib]), NULL, &pbufTemp, &(m_pmcSelectedMesh->m_rgcStrips[iAttrib]));
                            GXASSERT(!FAILED(hr));

                            m_pmcSelectedMesh->m_rgrgcStripCounts[iAttrib] = new DWORD[m_pmcSelectedMesh->m_rgcStrips[iAttrib]];

                            memcpy(m_pmcSelectedMesh->m_rgrgcStripCounts[iAttrib], pbufTemp->GetBufferPointer(), sizeof(DWORD)*m_pmcSelectedMesh->m_rgcStrips[iAttrib]);

                            GXRELEASE(pbufTemp);
                            
                        }
                    }
                }

            }
#endif
            else if ( VK_SPACE == (int) wparam )
            {
                ChangeAnimMode(!m_bAnimPaused);
            }
            else if ( VK_DELETE == (int) wparam )
            {
                DeleteSelectedMesh();
            }
            else if ( 'e' == wparam || 'E' == wparam )
            {
                ToggleEdgeMode();
            }
            else if ( 'w' == wparam || 'W' == wparam )
            {
                ToggleWireframeMode();
            }
            else if ( 'c' == wparam || 'C' == wparam )
            {
                ToggleCullMode();
            }
            else if ( 't' == wparam || 'T' == wparam )
            {
                ToggleTextureMode();
            }
            else if ( 's' == wparam || 'S' == wparam )
            {
                ToggleStripMode();
            }
            else if ( 'a' == wparam || 'A' == wparam )
            {
                ToggleAdjacencyMode();
            }
            else if ( 'r' == wparam || 'R' == wparam )
            {
                ToggleCreaseMode();
            }
            else if ( 'n' == wparam || 'N' == wparam )
            {
                ToggleNormalsMode();
            }
            else if ( 'f' == wparam || 'F' == wparam )
            {
                ToggleFaceSelectionMode();
            }
            else if ( 'v' == wparam || 'V' == wparam )
            {
                ToggleVertexSelectionMode();
            }
#if 0
            else if ( 'd' == wparam || 'D' == wparam )
            {
                GXRELEASE(m_pmcSelectedMesh->pMesh);
                GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                D3DXCreateMeshFVF(30000, 3, D3DXMESH_MANAGED, D3DFVF_XYZ, m_pDevice, &m_pmcSelectedMesh->pMesh);
                m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
                m_pmcSelectedMesh->pMesh->AddRef();

                delete m_pmcSelectedMesh->rgdwAdjacency;
                m_pmcSelectedMesh->rgdwAdjacency = NULL;
                //m_pmcSelectedMesh->rgdwAdjacency = new DWORD[30000 * 3];
                //memset(m_pmcSelectedMesh->rgdwAdjacency, 0xff, 30000 * 3 * sizeof(DWORD));

                UINT16 *pbIndices;
                PBYTE pbVertices;
                DWORD iFace;
                m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pbVertices);
                m_pmcSelectedMesh->pMesh->LockIndexBuffer(0, (LPVOID*)&pbIndices);

                *((D3DXVECTOR3*)pbVertices) = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                pbVertices += sizeof(D3DXVECTOR3);
                *((D3DXVECTOR3*)pbVertices) = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
                pbVertices += sizeof(D3DXVECTOR3);
                *((D3DXVECTOR3*)pbVertices) = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
                pbVertices += sizeof(D3DXVECTOR3);

                for (iFace = 0; iFace < 30000; iFace++)
                {
                    if ((iFace % 1) == 0)
                    {
                        *pbIndices = 0;
                        pbIndices++;

                        *pbIndices = 1;
                        pbIndices++;

                        *pbIndices = 2;
                        pbIndices++;
                    }
                    else
                    {
                        *pbIndices = 0;
                        pbIndices++;

                        *pbIndices = 2;
                        pbIndices++;

                        *pbIndices = 1;
                        pbIndices++;
                    }

                }

                m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
                m_pmcSelectedMesh->pMesh->UnlockIndexBuffer();
            }
            else if ( 'd' == wparam || 'D' == wparam )
            {
                Displace();
            }
            else if ( '1' == wparam )
            {
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
                {
                    DWORD cFaces;
                    DWORD *rgdwAttribs;
                    DWORD iFace;

                    cFaces = m_pmcSelectedMesh->pMesh->GetNumFaces();
                    
                    m_pmcSelectedMesh->m_rgpfxAttributes = new LPD3DXEFFECT[cFaces];
                    m_pmcSelectedMesh->m_cAttributeGroups = cFaces;
                    m_pmcSelectedMesh->NumMaterials = cFaces;

                    memset(m_pmcSelectedMesh->m_rgpfxAttributes, 0, sizeof(LPD3DXEFFECT) * cFaces);

                    m_pmcSelectedMesh->pMesh->LockAttributeBuffer(0, &rgdwAttribs);

                    for (iFace = 0; iFace < cFaces; iFace++)
                    {
                        rgdwAttribs[iFace] = iFace;
                    }

                    m_pmcSelectedMesh->pMesh->UnlockAttributeBuffer();

                    Optimize(D3DXMESHOPT_ATTRSORT);
                }
            }
            else if ('3' == wparam)
            {
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
                {
                    DXCrackFVF cfvf(m_pmcSelectedMesh->pMesh->GetFVF());
                    PBYTE pbPoints;
                    PBYTE pbCurVert;
                    DWORD iVertex;

                    m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pbPoints);

                    pbCurVert = pbPoints;
                    for (iVertex = 0; iVertex < m_pmcSelectedMesh->pMesh->GetNumVertices(); iVertex++)
                    {
                        memcpy(cfvf.PvGetPosition(pbCurVert), cfvf.PuvGetTex1(pbCurVert) + 1, sizeof(D3DXVECTOR3));

                        pbCurVert += cfvf.m_cBytesPerVertex;
                    }

                    m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
                }
            }
            else if ('2' == wparam)
            {
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
                {
                    LPD3DXMESH pMeshTemp;
                    HRESULT hr;
                    DWORD dwFVF;

                    dwFVF = m_pmcSelectedMesh->ptmDrawMesh->GetFVF();
                    dwFVF &= ~D3DFVF_TEXCOUNT_MASK;
                    dwFVF |= D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1);

                    hr = m_pmcSelectedMesh->ptmDrawMesh->CloneMeshFVF(
                                        m_pmcSelectedMesh->ptmDrawMesh->GetOptions(), 
                                        dwFVF,
                                        m_pDevice, &pMeshTemp);
                    GXASSERT(!FAILED(hr));

                    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                    GXRELEASE(m_pmcSelectedMesh->pMesh);
                    m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
                    m_pmcSelectedMesh->pMesh = pMeshTemp;
                    pMeshTemp->AddRef();

                    DXCrackFVF cfvf(dwFVF);
                    PBYTE pbPoints;
                    PBYTE pbCurVert;
                    DWORD iVertex;

                    m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pbPoints);

                    pbCurVert = pbPoints;
                    for (iVertex = 0; iVertex < m_pmcSelectedMesh->pMesh->GetNumVertices(); iVertex++)
                    {
                        memcpy(cfvf.PuvGetTex1(pbCurVert) + 1, cfvf.PvGetPosition(pbCurVert), sizeof(D3DXVECTOR3));
                        memset(cfvf.PvGetPosition(pbCurVert), 0, sizeof(D3DXVECTOR3));

                        pbCurVert += cfvf.m_cBytesPerVertex;
                    }

                    m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
                }
#if 0
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->bNPatchMode))
                {
                    DWORD cFaces;

                    cFaces = m_pmcSelectedMesh->pMesh->GetNumFaces();

                    GXRELEASE(m_pmcSelectedMesh->pMeshToTesselate);
                    m_pmcSelectedMesh->bNPatchMode = FALSE;
                    
                    delete []m_pmcSelectedMesh->rgdwAdjacency;
                    delete []m_pmcSelectedMesh->rgdwAdjacencyTesselate;

                    m_pmcSelectedMesh->rgdwAdjacency = new DWORD[cFaces * 3];

                    m_pmcSelectedMesh->pMesh->GenerateAdjacency(0.0f, m_pmcSelectedMesh->rgdwAdjacency);

                    AdjustScrollbar();

                    hr = m_pmcSelectedMesh->m_aoAdjacency.Init(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency);
                    hr = m_pmcSelectedMesh->m_eoEdges.Init(m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgdwAdjacency);
                }
#endif
            }
            else if ('U' == wparam)
            {
                if (m_pmcSelectedMesh != NULL)
                {
                    if (m_pmcSelectedMesh->bPMMeshMode)
                    {
                        m_pmcSelectedMesh->pPMMesh->OptimizeBaseLOD(D3DXMESHOPT_VERTEXCACHE, NULL);
                        hr = m_pmcSelectedMesh->m_soStrips.Init(m_pmcSelectedMesh->ptmDrawMesh);
                    }
                }
            }
            else if ('Y' == wparam)
            {
                if (m_pmcSelectedMesh != NULL)
                {
                    LPD3DXMESH pMeshTemp;
                    LPDIRECT3DVERTEXBUFFER9 pVB1;
                    LPDIRECT3DVERTEXBUFFER9 pVB2;

                    if (m_pmcSelectedMesh->bPMMeshMode)
                    {
                        if (0)
                        {
                            DWORD dwMinVertices = 1000, dwMaxVertices = 2000;

                            m_pmcSelectedMesh->pPMMesh->TrimByVertices(dwMinVertices, dwMaxVertices, NULL, NULL);

                            AdjustScrollbar();
                        }
                        else
                        {
                            LPD3DXPMESH pPMesh = m_pmcSelectedMesh->pPMMesh;

                            hr = m_pmcSelectedMesh->pPMMesh->Optimize(D3DXMESH_VB_WRITEONLY|D3DXMESH_MANAGED|D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT/*|D3DXMESHOPT_VERTEXCACHE*/, m_pmcSelectedMesh->rgdwAdjacency, NULL, NULL, &pMeshTemp);
                            //hr = m_pmcSelectedMesh->ptmDrawMesh->CloneMeshFVF(
                            //                    m_pmcSelectedMesh->ptmDrawMesh->GetOptions() /*| D3DXMESH_VB_SHARE*/, 
                            //                    m_pmcSelectedMesh->ptmDrawMesh->GetFVF(),
                            //                    m_pDevice, &pMeshTemp);
                            GXASSERT(!FAILED(hr));

                            GXRELEASE(m_pmcSelectedMesh->pPMMesh);
                            GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);

                            m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
                            m_pmcSelectedMesh->pMesh = pMeshTemp;
                            pMeshTemp->AddRef();

                            m_pmcSelectedMesh->bPMMeshMode = false;
                            AdjustScrollbar();

                            hr = m_pmcSelectedMesh->m_soStrips.Init(m_pmcSelectedMesh->pMesh);
                        }
                    }
                    else
                    {
                        hr = m_pmcSelectedMesh->ptmDrawMesh->CloneMesh(
                                            m_pmcSelectedMesh->ptmDrawMesh->GetOptions() | D3DXMESH_VB_SHARE, 
                                            NULL,
                                            m_pDevice, &pMeshTemp);
                        GXASSERT(!FAILED(hr));

                        m_pmcSelectedMesh->ptmDrawMesh->GetVertexBuffer(&pVB1);
                        pMeshTemp->GetVertexBuffer(&pVB2);

                        GXASSERT(pVB1 == pVB2);

                        GXRELEASE(pVB1);
                        GXRELEASE(pVB2);
                        GXRELEASE(pMeshTemp);
                    }
                }
            }
            else if (('y' == wparam) || ('Y' == wparam))
            {
                if (m_pmcSelectedMesh != NULL)
                {
                    LPD3DXMESH pMeshTemp;
                    if (m_pmcSelectedMesh->bPMMeshMode)
                    {
                        LPD3DXPMESH pPMesh = m_pmcSelectedMesh->pPMMesh;

                        hr = pPMesh->CloneMes(pPMesh->GetOptions(), NULL,
                                                m_pDevice, &pMeshTemp);
                        GXASSERT(!FAILED(hr));

                        GXRELEASE(m_pmcSelectedMesh->pPMMesh);
                        GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);

                        m_pmcSelectedMesh->ptmDrawMesh = pMeshTemp;
                        m_pmcSelectedMesh->pMesh = pMeshTemp;
                        pMeshTemp->AddRef();

                        m_pmcSelectedMesh->bPMMeshMode = false;
                        AdjustScrollbar();

                        hr = D3DXComputeNormals(pMeshTemp, NULL);
                        GXASSERT(!FAILED(hr));
                    }
                    else
                    {
                        pMeshTemp = m_pmcSelectedMesh->pMesh;

                        hr = D3DXComputeNormals(pMeshTemp, m_pmcSelectedMesh->rgdwAdjacency);
                        GXASSERT(!FAILED(hr));
                    }

                }

                m_pmcSelectedMesh->m_snNormals.Init(m_pmcSelectedMesh->pMesh, UNUSED32, m_pdeSelected->fRadius / 20.0f);
            }
            else if ('4' == wparam)
            {
                if ((m_pmcSelectedMesh != NULL) && m_pmcSelectedMesh->bSimplifyMode)
                {
                    char szBuf[1024];
                    D3DXATTRIBUTEWEIGHTS AttribWeights;

                    m_pmcSelectedMesh->pSimpMesh->GetVertexAttributeWeights(&AttribWeights);

                    sprintf(szBuf, "Position: %f\r\n"
                                   "Boundary: %f\r\n"
                                   "Normal: %f\r\n"
                                   "Diffuse: %f\r\n"
                                   "Specular: %f\r\n"
                                   "Tex[0]: %f\r\n"
                                   "Tex[1]: %f\r\n"
                                   "Tex[2]: %f\r\n"
                                   "Tex[3]: %f\r\n"
                                   "Tex[4]: %f\r\n"
                                   "Tex[5]: %f\r\n"
                                   "Tex[6]: %f\r\n"
                                   "Tex[7]: %f\r\n", 
                                   AttribWeights.Position, AttribWeights.Boundary,
                                   AttribWeights.Normal, AttribWeights.Diffuse, AttribWeights.Specular,
                                   AttribWeights.Tex[0], AttribWeights.Tex[1],
                                   AttribWeights.Tex[2], AttribWeights.Tex[3],
                                   AttribWeights.Tex[4], AttribWeights.Tex[5],
                                   AttribWeights.Tex[6], AttribWeights.Tex[7]);

                    pvDialogData = szBuf;
                    DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcOutput);
                }
            }
            else if ('p' == wparam || 'P' == wparam )
            {
                if ((m_pmcSelectedMesh != NULL) && m_pmcSelectedMesh->bSimplifyMode)
                {
                    ID3DXPMesh *pPMesh;
                    hr = m_pmcSelectedMesh->pSimpMesh->ClonePMesh(
                                            m_pmcSelectedMesh->pSimpMesh->GetOptions(),
                                            NULL,
                                            m_pDevice,
                                            NULL, &pPMesh);
                    GXASSERT(!FAILED(hr));
                    if (pPMesh != NULL)
                    {
                        m_pmcSelectedMesh->m_cMaxVerticesSoft = pPMesh->GetMaxVertices();
                        m_pmcSelectedMesh->m_cMinVerticesSoft = pPMesh->GetMinVertices();
                        m_pmcSelectedMesh->m_cNumVertices = pPMesh->GetNumVertices();

                        pPMesh->OptimizeBaseLOD(D3DXMESHOPT_VERTEXCACHE, NULL);

                        GXRELEASE(m_pmcSelectedMesh->pSimpMesh);
                        m_pmcSelectedMesh->pPMMesh = pPMesh;

                        m_pmcSelectedMesh->ptmDrawMesh = pPMesh;
                        pPMesh->AddRef();

                        m_pmcSelectedMesh->bSimplifyMode = false;
                        m_pmcSelectedMesh->bPMMeshMode = true;

                        AdjustScrollbar();
                    }

                }
            }
#endif
#ifdef OLD
            else if ('3' == wparam)
            {
                DXCrackFVF cfvf(D3DFVF_XYZ);
                DWORD dwFVF;
                LPD3DXMESH pMeshTemp;
                DWORD iVertex;
                PBYTE pvPoint;
                DWORD cVertices;
                PBYTE pvPoints;

                // output mesh info to a temp file in a c format

                // nothing to do, just return
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->pMesh != NULL))
                {
                    GXASSERT( !(m_pmcSelectedMesh->pMesh->GetOptions() & D3DXMESH_32BIT) );
                    DWORD *rgdwPointReps;
                    FILE *file;
                    WORD *pwIndices;
                    DWORD iFace;
                    DWORD cFaces;

                    file = fopen("c:\\tempmesh.txt", "w+");
                    GXASSERT(file != NULL);

                    cVertices = m_pmcSelectedMesh->pMesh->GetNumVertices();
                    cFaces= m_pmcSelectedMesh->pMesh->GetNumFaces();
                    rgdwPointReps = new DWORD[cVertices];
                    GXASSERT(rgdwPointReps != NULL);

                    cfvf = DXCrackFVF(m_pmcSelectedMesh->pMesh->GetFVF());

                    m_pmcSelectedMesh->pMesh->ConvertAdjacencyToPointReps(m_pmcSelectedMesh->rgdwAdjacency, rgdwPointReps);
                
                    m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pvPoints);
                    m_pmcSelectedMesh->pMesh->LockIndexBuffer(0, (LPVOID*)&pwIndices);

                    fprintf(file, "static D3DXVECTOR3 teapotPositions[%d] = \n{\n", cVertices);
                    for (iVertex = 0; iVertex < cVertices; iVertex++)
                    {
                        pvPoint = cfvf.GetArrayElem(pvPoints, iVertex);

                        fprintf(file, "    { %f, %f, %f },\n", cfvf.PvGetPosition(pvPoint)->x,
                                                        cfvf.PvGetPosition(pvPoint)->y,
                                                        cfvf.PvGetPosition(pvPoint)->z);
                    }
                    fprintf(file, "};\n");

                    fprintf(file, "static D3DXVECTOR3 teapotNormals[%d] = \n{\n", cVertices);
                    for (iVertex = 0; iVertex < cVertices; iVertex++)
                    {
                        pvPoint = cfvf.GetArrayElem(pvPoints, iVertex);

                        fprintf(file, "    { %f, %f, %f },\n", cfvf.PvGetNormal(pvPoint)->x,
                                                        cfvf.PvGetNormal(pvPoint)->y,
                                                        cfvf.PvGetNormal(pvPoint)->z);
                    }
                    fprintf(file, "};\n");

                    fprintf(file, "static DWORD teapotPointReps[%d] = \n{\n", cVertices);
                    for (iVertex = 0; iVertex < cVertices; iVertex++)
                    {
                        fprintf(file, "    %d,\n", rgdwPointReps[iVertex]);
                    }
                    fprintf(file, "};\n");

                    fprintf(file, "static WORD teapotIndices[%d] = \n{\n", cFaces * 3);
                    for (iFace = 0; iFace < cFaces; iFace++)
                    {
                        fprintf(file, "    %d, %d, %d,\n", *(pwIndices), *(pwIndices + 1), *(pwIndices + 2));
                        pwIndices += 3;
                    }
                    fprintf(file, "};\n");

                    m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();
                    m_pmcSelectedMesh->pMesh->UnlockIndexBuffer();

                    delete []rgdwPointReps;

                    fclose(file);
                }
            }
#endif
#if 0
            else if ('<' == wparam  || ',' == wparam )
            {
                if (m_pmcSelectedMesh != NULL)
                {
                    SimulateCacheStuff();
                }
            }
            else if ('+' == wparam)
            {
                if (m_pmcSelectedMesh != NULL) 
                {
                    RemoveAllMeshesExceptSelected();
                }
            }
#endif
            else if ('z' == wparam || 'Z' == wparam )
            {
                SetNumVertices(1, true /*absolute*/);
            }
            else if ('x' == wparam || 'X' == wparam )
            {
                SetNumVertices(0x7fffffff, true /*absolute*/);
            }
            else if ( 'q' == wparam || 'Q' == wparam )
            {
                ToggleNPatchEdgeMode();
            }
#if 0
            else if ( '8' == wparam )
            {
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->bPMMeshMode))
                {
                    unsigned short *pbIndices;

                    DWORD cFaces = m_pmcSelectedMesh->pPMMesh->GetMaxFaces();
                    DWORD *rgdwAdjacencySrc = new DWORD[cFaces * 3];
                    DWORD *rgdwAdjacencyNew = new DWORD[cFaces * 3];
                    DWORD *rgdwPReps = new DWORD[m_pmcSelectedMesh->pPMMesh->GetMaxVertices()];
                    m_pmcSelectedMesh->pPMMesh->GetAdjacency(rgdwAdjacencySrc);

                    m_pmcSelectedMesh->pPMMesh->ConvertAdjacencyToPointReps(rgdwAdjacencySrc, rgdwPReps);
                    m_pmcSelectedMesh->pPMMesh->ConvertPointRepsToAdjacency(rgdwPReps, rgdwAdjacencyNew);

                    m_pmcSelectedMesh->pPMMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbIndices);

                    for (DWORD iFace = 0; iFace < cFaces; iFace++)
                    {
                        for (DWORD iPoint = 0; iPoint < 3; iPoint++)
                        {
                            GXASSERT(rgdwAdjacencySrc[iFace * 3 + iPoint] == rgdwAdjacencyNew[iFace * 3 + iPoint]);
                        }
                    }

                    m_pmcSelectedMesh->pPMMesh->UnlockIndexBuffer();
                }
                
            }
            else if ('9' == wparam)
            {
                if (m_pmcSelectedMesh != NULL)
                {
                    unsigned short *pwIndices;
                    D3DXVECTOR3 *rgvVertices;

                    GXRELEASE(m_pmcSelectedMesh->ptmDrawMesh);
                    GXRELEASE(m_pmcSelectedMesh->pMesh);
                    D3DXCreateMeshFVF(4, 6, D3DXMESH_MANAGED, D3DFVF_XYZ, m_pDevice, &m_pmcSelectedMesh->pMesh);

                    m_pmcSelectedMesh->ptmDrawMesh = m_pmcSelectedMesh->pMesh;
                    m_pmcSelectedMesh->pMesh->AddRef();

                    m_pmcSelectedMesh->pMesh->LockIndexBuffer(0, (LPVOID*)&pwIndices);
                    m_pmcSelectedMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&rgvVertices);

                    rgvVertices[0] = D3DXVECTOR3(-1.0, 1.0, 0.0);
                    rgvVertices[1] = D3DXVECTOR3(-1.0, 0.0, 0.0);
                    rgvVertices[2] = D3DXVECTOR3( 0.0, 1.0, 0.0);
                    rgvVertices[3] = D3DXVECTOR3( 0.0, 0.0, 0.0);
                    rgvVertices[4] = D3DXVECTOR3( 1.0, 1.0, 0.0);
                    rgvVertices[5] = D3DXVECTOR3( 1.0, 0.0, 0.0);

                    pwIndices[0] = 0;
                    pwIndices[1] = 1;
                    pwIndices[2] = 2;
                    pwIndices[3] = 2;
                    pwIndices[4] = 1;
                    pwIndices[5] = 3;

                    pwIndices[6] = 2;
                    pwIndices[7] = 3;
                    pwIndices[8] = 4;
                    pwIndices[9] = 4;
                    pwIndices[10] = 3;
                    pwIndices[11] = 5;

                    m_pmcSelectedMesh->pMesh->UnlockIndexBuffer();
                    m_pmcSelectedMesh->pMesh->UnlockVertexBuffer();


                    m_pmcSelectedMesh->rgdwAdjacency[0] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[1] = 1;
                    m_pmcSelectedMesh->rgdwAdjacency[2] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[3] = 0;
                    m_pmcSelectedMesh->rgdwAdjacency[4] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[5] = UNUSED32;

                    m_pmcSelectedMesh->rgdwAdjacency[6] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[7] = 3;
                    m_pmcSelectedMesh->rgdwAdjacency[8] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[9] = 2;
                    m_pmcSelectedMesh->rgdwAdjacency[10] = UNUSED32;
                    m_pmcSelectedMesh->rgdwAdjacency[11] = UNUSED32;

                    m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
                }
            }
            else if ('8' == wparam)
            {
                TesselateFrame(m_pframeSelected);
            }
            else if ('9' == wparam)
            {
                SplitMesh();
            }
#endif
            else if ('i' == wparam || 'I' == wparam)
            {
                if (m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32))
                {
                    DialogBox(m_hInstance, (LPCTSTR) IDD_VERTEXINFO, m_hwnd, (DLGPROC) DlgProcVertexInfo);
                }
                else if (m_bFaceSelectionMode && (m_dwFaceSelected != UNUSED32))
                {
                    DialogBox(m_hInstance, (LPCTSTR) IDD_FACEINFO, m_hwnd, (DLGPROC) DlgProcFaceInfo);
                }
                else if (m_pmcSelectedMesh != NULL) // show mesh stats
                {
                    DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcMeshInfo);
                }
            }
            return 0;

        case WM_VSCROLL:
            {
                if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->bTesselateMode || m_pmcSelectedMesh->bNPatchMode))
                {
                    int min, max;
                    int cLevelNew = m_pmcSelectedMesh->cTesselateLevel;
                    GetScrollRange(hwnd, SB_VERT, &min, &max);

                    int delta = 1;

                    switch (LOWORD(wparam))
                    {
                    case SB_THUMBPOSITION:
                        cLevelNew = min + max - HIWORD(wparam);
                        break;
                    case SB_LINEUP:
                        cLevelNew += delta;
                        break;
                    case SB_LINEDOWN:
                        if (cLevelNew > delta)
                            cLevelNew -= delta;
                        else
                            cLevelNew = 0;
                        break;
                    case SB_PAGEUP:
                        cLevelNew += 2*delta;
                        break;
                    case SB_PAGEDOWN:
                        if (cLevelNew > 2*delta)
                            cLevelNew -= 2*delta;
                        else
                            cLevelNew = 0;
                        break;
                    case SB_THUMBTRACK:
                        cLevelNew = min + max - HIWORD(wparam);
                        break;

                    case SB_ENDSCROLL:
                        return 0;

                    case SB_BOTTOM:
                        //vertices = min;
                        //break;
                    case SB_TOP:
                        //vertices = max;
                        //break;
                    default:
                        return 0;
                    }

                    SMeshContainer *pmc = m_pframeSelected->pmcMesh;

                    while (pmc != NULL)
                    {
                        pmc->cTesselateLevel = cLevelNew;
                        Tesselate(pmc, FALSE);

                        pmc = (SMeshContainer*)pmc->pNextMeshContainer;
                    }
                }

                if ((m_pmcSelectedMesh != NULL) && m_pmcSelectedMesh->pPMMesh && m_pmcSelectedMesh->bPMMeshMode)
                {
                    DWORD vertices;
                    int min, max;
                    DWORD cMaxVertices, cMinVertices;
                    BOOL bUpdateViews = TRUE;

                    GetScrollRange(hwnd, SB_VERT, &min, &max);
                    vertices = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
//                    DWORD delta = (max - min) / 500;
                    DWORD delta = (max - min) / 500;
                    if (delta == 0)
                        delta = 1;

                    switch (LOWORD(wparam))
                    {
                    case SB_THUMBPOSITION:
                        vertices = min + max - HIWORD(wparam);
                        break;
                    case SB_LINEUP:
                        vertices += delta;
                        break;
                    case SB_LINEDOWN:
                        if (vertices > delta)
                            vertices -= delta;
                        else
                            vertices = 0;
                        break;
                    case SB_PAGEUP:
                        vertices += 5*delta;
                        break;
                    case SB_PAGEDOWN:
                        if (vertices > 5*delta)
                            vertices -= 5*delta;
                        else
                            vertices = 0;
                        break;
                    case SB_THUMBTRACK:
                        vertices = min + max - HIWORD(wparam);
                        bUpdateViews = FALSE;
                        break;

                    case SB_ENDSCROLL:
                        return 0;

                    case SB_BOTTOM:
                        //vertices = min;
                        //break;
                    case SB_TOP:
                        //vertices = max;
                        //break;
                    default:
                        return 0;
                    }
                    if (vertices < (DWORD)min) vertices=min;
                    if (vertices > (DWORD)max) vertices=max;
                    cMaxVertices = m_pmcSelectedMesh->pPMMesh->GetMaxVertices();
                    cMinVertices = m_pmcSelectedMesh->pPMMesh->GetMinVertices();
                    if (vertices < cMinVertices) vertices=cMinVertices;
                    if (vertices > cMaxVertices) vertices=cMaxVertices;

                    if (vertices < m_pmcSelectedMesh->m_cMinVerticesSoft) 
                        vertices = m_pmcSelectedMesh->m_cMinVerticesSoft;

                    if (vertices > m_pmcSelectedMesh->m_cMaxVerticesSoft) 
                        vertices = m_pmcSelectedMesh->m_cMaxVerticesSoft;

                    m_pmcSelectedMesh->pPMMesh->SetNumVertices(vertices);
                    m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(m_pmcSelectedMesh->m_rgaeAttributeTable, NULL);
                    SetScrollPos(hwnd, SB_VERT, min + max - vertices, TRUE);

                    if (bUpdateViews)
                    {
                        m_pmcSelectedMesh->pPMMesh->GetAdjacency(m_pmcSelectedMesh->rgdwAdjacency);
                        m_pmcSelectedMesh->UpdateViews(m_pdeSelected);
                    }

                    if (m_dwFaceSelected != UNUSED32)
                    {
        
                        if ((m_dwVertexSelected >= m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].VertexStart + m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].VertexCount)
                              || (m_dwFaceSelected >= m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].FaceStart + m_pmcSelectedMesh->m_rgaeAttributeTable[m_dwFaceSelectedAttr].FaceCount))
                        {
                            m_dwVertexSelected = UNUSED32;
                            m_dwFaceSelected = UNUSED32;
                            m_dwFaceSelectedAttr = UNUSED32;
                        }
                    }

                    Draw();
                }
            }
            return 0;
        case WM_ACTIVATEAPP:
//            bActive = (wparam) ? true : false;
            break;

        case WM_SIZE:
            GetClientRect( hwnd, &r );

            // setup the client rect size, used for frame rate info
            m_sizeClient.cx = r.right - r.left;
            m_sizeClient.cy = r.bottom - r.top;

            m_abArcBall.SetWindow(r.right, r.bottom, 0.85f);

            if (m_pdeSelected != NULL)
                SetProjectionMatrix();

            SendMessage(m_hwndStatus, WM_SIZE, 0, 0);
            SendMessage(m_hwndToolbar, WM_SIZE, 0, 0);

            INT lpParts[x_cStatusBarSizes + 1];// = {280, 370, 430, 500, 560, 640};
            DWORD iCur;
            INT iPart;
            iCur = m_sizeClient.cx;
            for (iPart = x_cStatusBarSizes - 1; iPart >= 0; iPart--)
            {
                lpParts[iPart+1] = iCur;
                iCur -= x_rgStatusBarSizes[iPart];
            }
            lpParts[0] = iCur;

            SendMessage(m_hwndStatus, SB_SETPARTS, (WPARAM) x_cStatusBarSizes+1, (LPARAM) lpParts); 

            break;

        case WM_COMMAND:
            if ( 1 == HIWORD(wparam) )
            {
#if 0
                switch ( LOWORD(wparam) )
                {
                    case ID_FULLSCREEN:
                        bFullScreen = ! bFullScreen;
                        bInitiatedModeChange = true;

                        if ( bFullScreen )
                        {
                            // going to fullscreen
                            GetWindowRect( hwnd, &m_rWindowedRect );

                        }
                        else
                        {
                            bInitiatedModeChange = true;
                        }

                        ReleaseTextures();

                        GXRELEASE( m_pDevice );
                        Initialize( hwnd );
                        PrepareToDraw( );

                        ReloadTextures( );

                        if ( ! bFullScreen )
                        {
                            RECT& r = m_rWindowedRect;
                            SetWindowPos(hwnd, HWND_NOTOPMOST, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOACTIVATE );

                        }

                        bInitiatedModeChange = false;
                        break;
                }
#endif
            }
            if ( 0 == HIWORD(wparam) )
            {
                if ((LOWORD(wparam) >= ID_ANIMATION_1) && (LOWORD(wparam) <= ID_ANIMATION_50))
                {
                        SwitchToAnimationSet(LOWORD(wparam) - ID_ANIMATION_1);
                }
#if 0
                if ((LOWORD(wparam) >= ID_MESH_1) && (LOWORD(wparam) <= ID_MESH_9))
                {
                        SwitchToMeshID(LOWORD(wparam));
                }
#endif

                switch ( LOWORD(wparam) )
                {
                    case ID_FILE_EXIT:
                        PostQuitMessage(0);
                        return 0;

                    case ID_FILE_OPENMESH:
                        LoadNewMesh();
                        return 0;

                    case ID_FILE_LOADPMESH:
                        LoadNewProgressiveMesh();
                        return 0;

                    case ID_FILE_SAVEMESH:
                        SaveMesh();
                        //SavePMesh();
                        return 0;

                    case ID_FILE_CLOSEMESH:
                        DeleteSelectedMesh();
                        break;

                    case ID_FILE_CLOSENONSELECTED:
                        if (m_pmcSelectedMesh != NULL) 
                        {
                            RemoveAllMeshesExceptSelected();
                        }
                        break;


                    case ID_SIMPLIFY_GENERATEPM:
                        // as long as it is not a PMesh, try to generate a PM
                        //   normal meshes will get converted to a simplification mesh
                        //   before attempting simplification
                        if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->bSimplifyMode || (m_pmcSelectedMesh->pMesh != NULL)))
                        {
                            GeneratePM();
                        }
                        return 0;

                    case ID_SIMPLIFY_SIMPLIFY:
                        if (m_pmcSelectedMesh != NULL)
                        {
                            if (!m_pmcSelectedMesh->bSimplifyMode && !m_pmcSelectedMesh->bPMMeshMode && (m_pmcSelectedMesh->pMesh != NULL))
                            {
                                hr = ConvertMeshToSimplify();
                                if (FAILED(hr)) // check for cancel or other failures
                                    break;
                            }

                            if (m_pmcSelectedMesh->bSimplifyMode || m_pmcSelectedMesh->bPMMeshMode)
                            {
                                DialogBox(m_hInstance, (LPCTSTR) IDD_SIMPLIFY, hwnd, (DLGPROC) DlgProcSimplify);
                            }
                        }
                        break;

                    case ID_SIMPLIFY_SIMPLIFYFACES:
                        if (m_pmcSelectedMesh != NULL)
                        {
                            if (!m_pmcSelectedMesh->bSimplifyMode && !m_pmcSelectedMesh->bPMMeshMode && (m_pmcSelectedMesh->pMesh != NULL))
                            {
                                hr = ConvertMeshToSimplify();
                                if (FAILED(hr)) // check for cancel or other failures
                                    break;
                            }

                            if (m_pmcSelectedMesh->bSimplifyMode || m_pmcSelectedMesh->bPMMeshMode)
                            {
                                DialogBox(m_hInstance, (LPCTSTR) IDD_SIMPLIFYFACES, hwnd, (DLGPROC) DlgProcSimplifyFaces);
                            }
                        }
                        break;
                    case ID_SIMPLIFY_TESTSIMPLIFY:
                        TestSimplify();
                        break;

                    case ID_SIMPLIFY_SETSOFTMIN:
                        SetSoftMinLOD();
                        break;

                    case ID_SIMPLIFY_SETSOFTMAX:
                        SetSoftMaxLOD();
                        break;

                    case ID_SIMPLIFY_RESETSOFTMIN:
                        ResetSoftMinLOD();
                        break;

                    case ID_SIMPLIFY_RESETSOFTMAX:
                        ResetSoftMaxLOD();
                        break;

                    case ID_SIMPLIFY_TRIM:
                        TrimPMeshToSoftLimits();
                        break;

                    case ID_SIMPLIFY_SNAPSHOT:
                        SnapshotSelected();
                        break;

                    case ID_OPTIONS_PROPERTIES:
                        if (m_pmcSelectedMesh != NULL)
                            DialogBox(m_hInstance, (LPCTSTR) IDD_MESHPROPERTIES, hwnd, (DLGPROC) DlgProcProperties);
                        break;

                    case ID_OPTIONS_COMPACT:
                        Optimize(D3DXMESHOPT_COMPACT);
                        break;

                    case ID_OPTIONS_ATTRSORT:
                        Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT);
                        break;

                    case ID_OPTIONS_STRIPREORDER:
                        Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_STRIPREORDER);
                        break;

                    case ID_OPTIONS_VERTEXCACHE:
                        Optimize(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE);
                        break;

                    case ID_OPTIMIZE_SIMULATE:
                        DisplayCacheBehavior();
                        break;

                    case ID_OPTIONS_TEXTURE:
                        ToggleTextureMode();
                        break;

                    case ID_OPTIONS_NOSELECTION:
                        ToggleNoSelectionMode();
                        break;

                    case ID_OPTIONS_FACESELECTION:
                        ToggleFaceSelectionMode();
                        break;

                    case ID_OPTIONS_VERTEXSELECTION:
                        ToggleVertexSelectionMode();
                        break;

                    case ID_OPTIONS_MESHSELECTION:
                        ToggleMeshSelectionMode();
                        break;

                    case ID_OPTIONS_WELDVERTICES:
                        WeldVertices();
                        break;

                    case ID_NPATCHES_NPATCH:
                        TesselateFrame(m_pframeSelected);
                        break;

                    case ID_NPATCHES_SNAPSHOT:
                        SnapshotSelected();
                        break;

                    case ID_NPATCHES_EDGEMODE:
                        ToggleNPatchEdgeMode();
                        break;

                    case ID_OPTIONS_SPLITMESH:
                        SplitMesh();
                        break;

                    case ID_MESHOPS_COLLAPSE:
                        MergeMeshes();
                        break;

                    case ID_MESHOPS_COMPUTENORMALS:
                        ComputeNormals();
                        break;

                    case ID_MESHOPS_VALIDATEMESH:
                        ValidateMesh();
                        break;

                    case ID_OPTIONS_APPLYDISPLACEMENT:
                        Displace();
                        break;

                    case ID_OPTIONS_RESETMATRICES:
                        if ((m_pdeSelected != NULL) && (m_pdeSelected->pframeRoot != NULL))
                        {
                            m_pdeSelected->pframeRoot->ResetMatrix();
                        }

                        break;

                    case ID_OPTIONS_TREEVIEW:
                        ToggleTreeView();
                        break;

                    case ID_D3D_LIGHTING:
                        ToggleLightMode();
                        break;

                    case ID_D3D_CULL:
                        ToggleCullMode();
                        break;

                    case ID_D3D_EDGEMODE:
                        ToggleEdgeMode();
                        break;

                    case ID_D3D_STRIPMODE:
                        ToggleStripMode();
                        break;

                    case ID_D3D_ADJACENCYMODE:
                        ToggleAdjacencyMode();
                        break;

                    case ID_D3D_CREASEMODE:
                        ToggleCreaseMode();
                        break;

                    case ID_D3D_SHOWNORMALS:
                        ToggleNormalsMode();
                        break;

                    case ID_D3D_SHOWTEX0:
                    case ID_D3D_SHOWTEX1:
                    case ID_D3D_SHOWTEX2:
                    case ID_D3D_SHOWTEX3:
                    case ID_D3D_SHOWTEX4:
                    case ID_D3D_SHOWTEX5:
                    case ID_D3D_SHOWTEX6:
                    case ID_D3D_SHOWTEX7:
                        ToggleShowTexCoord(LOWORD(wparam)-ID_D3D_SHOWTEX0);
                        break;

                    case ID_D3D_WIREFRAME:
                        ToggleWireframeMode();
                        break;

                    case ID_D3D_SOLID:
                        ToggleSolidMode();
                        break;

                    case ID_ANIMATION_LOAD:
                        AddAnimation();
                        break;

                    case ID_VIEW_PLAYANIM:
                        ChangeAnimMode(FALSE);
                        break;

                    case ID_VIEW_PAUSEANIM:
                        ChangeAnimMode(TRUE);
                        break;
                    case ID_VIEW_PLAYBACKSPEED:
                        TogglePlaybackSpeed();
                        break;

                    case ID_D3D_SOFTWARESKIN:
                        ChangeSkinningMode(SOFTWARE);
                        break;

                    case ID_D3D_NONINDEXED:
                        ChangeSkinningMode(D3DNONINDEXED);
                        break;

                    case ID_D3D_INDEXED:
                        ChangeSkinningMode(D3DINDEXED);
                        break;

                    case ID_SHAPE_SELECTFONT:
                        SelectTextFont();
                        break;

                    case ID_SHAPE_TEXT:
                        CreateText();
                        break;

                    case ID_SHAPE_POLYGON:
                        CreatePolygon();
                        break;

                    case ID_SHAPE_BOX:
                        CreateBox();
                        break;

                    case ID_SHAPE_CYLINDER:
                        CreateCylinder();
                        break;

                    case ID_SHAPE_TORUS:
                        CreateTorus();
                        break;

                    case ID_SHAPE_TEAPOT:
                        CreateTeapot();
                        break;

                    case ID_SHAPE_SPHERE:
                        CreateSphere();
                        break;

                    case ID_SHAPE_CONE:
                        CreateCone();
                        break;

                    case ID_HELP_ABOUT:
                        DialogBox(m_hInstance, (LPCTSTR) IDD_ABOUTBOX, hwnd, (DLGPROC) DlgProcAbout);
                        break;

                    case ID_OPTIONS_INFO:
                        if (m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32))
                        {
                            DialogBox(m_hInstance, (LPCTSTR) IDD_VERTEXINFO, m_hwnd, (DLGPROC) DlgProcVertexInfo);
                        }
                        else if (m_bFaceSelectionMode && (m_dwFaceSelected != UNUSED32))
                        {
                            DialogBox(m_hInstance, (LPCTSTR) IDD_FACEINFO, m_hwnd, (DLGPROC) DlgProcFaceInfo);
                        }
                        else if (m_pmcSelectedMesh != NULL) // show mesh stats
                        {
                            DialogBox(m_hInstance, (LPCTSTR) IDD_INFO, m_hwnd, (DLGPROC) DlgProcMeshInfo);
                        }
                        break;

                }
            }
            break;
        /*
         * Pause and unpause the app when entering/leaving the menu
         */
        case WM_ENTERMENULOOP:
            PauseDrawing();
            break;
        case WM_EXITMENULOOP:
            RestartDrawing();
            break;
        case WM_ACTIVATE:
            {
                DWORD fActive = LOWORD(wparam);

                if (fActive == WA_INACTIVE)
                    PauseDrawing();
                else
                    RestartDrawing();
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            m_vCurMousePosition.x = (float)LOWORD(lparam);  // horizontal position of cursor
            m_vCurMousePosition.y = (float)HIWORD(lparam);  // vertical position of cursor

            break;

        case WM_LBUTTONDOWN:
            UINT bControl;
            bControl = GetAsyncKeyState(VK_CONTROL) & 0x8000;

            m_abArcBall.BeginDrag(LOWORD(lparam), HIWORD(lparam));

            // update selection change if not editing normals
            if (!(m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32) 
                            && m_bNormalsMode && bControl && (wparam & MK_LBUTTON)))
            {
                SelectionChange(LOWORD(lparam), HIWORD(lparam));
            }

            SetCapture(m_hwnd);

            break;

        case WM_LBUTTONUP:

            // if re-tesselated during mouse move due to normal modification.  Re-tesselate now to re-init views
            if (m_bUpdatedNormalDuringMouseMove && (m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->bNPatchMode))
            {
                Tesselate(m_pmcSelectedMesh, FALSE);
            }
            m_bUpdatedNormalDuringMouseMove = FALSE;

            m_abArcBall.EndDrag();

            ReleaseCapture();

            break;

        {
            SDrawElement *pdeCur;
            bool bChange = false;
            D3DXMATRIX mat;
            D3DXVECTOR2 vDelta;

            
    
            UINT bAllFrames = GetAsyncKeyState(VK_SHIFT) & 0x8000;
            UINT bMoveRootFrame = !(GetAsyncKeyState(VK_CONTROL) & 0x8000);
            bControl = (GetAsyncKeyState(VK_CONTROL) & 0x8000);

            vDelta = D3DXVECTOR2(0,10);

            // normalize based on size of window
            vDelta.y /= m_sizeClient.cy;

            // now resize based on size of bounding sphere
            vDelta *= m_pdeSelected->fRadius * 5;

            D3DXVECTOR3 vNewDelta;
            vNewDelta.x = 0;
            vNewDelta.y = 0;
            vNewDelta.z = vDelta.y;
            bChange = true;

            if (bChange)
            {
                D3DXMatrixTranslation(&mat, vNewDelta.x, vNewDelta.y, vNewDelta.z);

                if (bAllFrames)
                {
                    pdeCur = m_pdeHead;
                    while (pdeCur != NULL)
                    {
                        //pdeCur->pframeRoot->m_vTranslation += vNewDelta;
                        D3DXMatrixMultiply(&pdeCur->pframeRoot->matTrans, &pdeCur->pframeRoot->matTrans, &mat);
                        pdeCur = pdeCur->pdeNext;
                    }
                }
                else if (m_pframeSelected != NULL)
                {
                    if (bMoveRootFrame)
                    {
                        D3DXMatrixMultiply(&m_pdeSelected->pframeRoot->matTrans, &m_pdeSelected->pframeRoot->matTrans, &mat);
                    }
                    else
                    {
                        D3DXMatrixMultiply(&m_pframeSelected->matTrans, &m_pframeSelected->matTrans, &mat);
                    }
                    //m_pframeSelected->m_vTranslation += vNewDelta;
                }
                
            }
            break;
        }

        case WM_DROPFILES:
        {
            HDROP hDropInfo = (HDROP) wparam;

            char szFilePath[MAX_PATH];
            UINT nFilesDropped = DragQueryFile( hDropInfo, 0xFFFFFFFF, szFilePath, MAX_PATH );
            if( nFilesDropped > 0 )
            {
                UINT nBytesCopied = DragQueryFile( hDropInfo, 0, szFilePath, MAX_PATH );
                LoadMesh( szFilePath, false );
            }
            DragFinish(hDropInfo); 
            break;
        }

        case WM_MOUSEWHEEL:
        case WM_MOUSEMOVE:
        {
            D3DXVECTOR2 vNewPosition;
            D3DXVECTOR3 vNewDelta;
            SDrawElement *pdeCur;
            bool bChange = false;
            D3DXMATRIX mat;

            UINT bAllFrames = GetAsyncKeyState(VK_SHIFT) & 0x8000;
            UINT bMoveRootFrame = !(GetAsyncKeyState(VK_CONTROL) & 0x8000);
            bControl = (GetAsyncKeyState(VK_CONTROL) & 0x8000);

            // if shift is down and no buttons are pressed, do a continual select
            if (bAllFrames && !(wparam & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
            {
                SelectionChange(LOWORD(lparam), HIWORD(lparam));
            }

            if (m_pdeSelected == NULL)
                break;

            // if in normal modification mode and control and left button are down, then modify the
            //    selected normal
            if ( msg == WM_MOUSEMOVE )
            {
                if( m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32) 
                    && m_bNormalsMode && bControl && (wparam & MK_LBUTTON))
                {
                    LPD3DXMESH pMeshCur = m_pmcSelectedMesh->bNPatchMode ? m_pmcSelectedMesh->pMeshToTesselate : m_pmcSelectedMesh->pMesh;

                    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

                    pMeshCur->GetDeclaration(pDecl);
                    CD3DXCrackDecl1 cd(pDecl);

                    PBYTE pvPoints;
                    D3DXVECTOR3 *pvNormal;

                    m_abArcBall.Mouse(LOWORD(lparam), HIWORD(lparam));
                    m_abArcBall.GetMat(&mat);

                    D3DXMatrixTranspose(&mat, &mat);

                    pMeshCur->LockVertexBuffer(0, (LPVOID*)&pvPoints);

                    pvNormal = cd.PvGetNormal(cd.GetArrayElem(pvPoints, m_dwVertexSelected));

                    D3DXVec3TransformNormal(pvNormal, pvNormal, &mat);
                    D3DXVec3Normalize(pvNormal, pvNormal);

                    pMeshCur->UnlockVertexBuffer();

                    if (m_pmcSelectedMesh->bNPatchMode)
                    {
                        Tesselate(m_pmcSelectedMesh, TRUE);
                    }
                    else
                    {
                        // UNDONE UNDONE - could just update the normal modified
                        m_pmcSelectedMesh->m_snNormals.Init(m_pmcSelectedMesh->pMesh, UNUSED32, m_pdeSelected->fRadius / 20.0f);
                    }

                    m_abArcBall.EndDrag();
                    m_abArcBall.Reset();
                    m_abArcBall.BeginDrag(LOWORD(lparam), HIWORD(lparam));
                    m_bUpdatedNormalDuringMouseMove = TRUE;
                    break;
                }

                if (MK_LBUTTON & wparam)
                {

                    m_abArcBall.Mouse(LOWORD(lparam), HIWORD(lparam));
                            m_abArcBall.GetMat(&mat);

                    D3DXMatrixTranspose(&mat, &mat);

                    if (bAllFrames)
                    {
                        pdeCur = m_pdeHead;
                        while (pdeCur != NULL)
                        {
                            D3DXMatrixMultiply(&pdeCur->pframeRoot->matRot, &pdeCur->pframeRoot->matRot, &mat);
                            pdeCur = pdeCur->pdeNext;
                        }
                    }
                    else if (m_pframeSelected != NULL)
                    {
                        if (bMoveRootFrame)
                        {
                            D3DXMatrixMultiply(&m_pdeSelected->pframeRoot->matRot, &m_pdeSelected->pframeRoot->matRot, &mat);
                        }
                        else
                        {
                            D3DXMatrixMultiply(&m_pframeSelected->matRot, &m_pframeSelected->matRot, &mat);
                        }
                    }

                    m_abArcBall.EndDrag();
                    m_abArcBall.Reset();
                    m_abArcBall.BeginDrag(LOWORD(lparam), HIWORD(lparam));
                    break;
                }

                vNewPosition.x = (float)LOWORD(lparam);  // horizontal position of cursor
                vNewPosition.y = (float)HIWORD(lparam);  // vertical position of cursor

                if (MK_RBUTTON & wparam)
                {
                    D3DXVECTOR2 vDelta;

                    vDelta = m_vCurMousePosition - vNewPosition;

                    // normalize based on size of window
                    vDelta.x /= m_sizeClient.cx;
                    vDelta.y /= m_sizeClient.cy;

                    // now resize based on size of bounding sphere
                    vDelta *= m_pdeSelected->fRadius * 2;
                    vDelta.x *= -1;

                    vNewDelta.x = vDelta.x;
                    vNewDelta.y = vDelta.y;
                    vNewDelta.z = 0;
                    bChange = true;
                }

                if (MK_MBUTTON & wparam)
                {
                    D3DXVECTOR2 vDelta;

                    vDelta = m_vCurMousePosition - vNewPosition;

                    // normalize based on size of window
                    vDelta.y /= m_sizeClient.cy;

                    // now resize based on size of bounding sphere
                    vDelta *= m_pdeSelected->fRadius * 5;

                    vNewDelta.x = 0;
                    vNewDelta.y = 0;
                    vNewDelta.z = vDelta.y;
                    bChange = true;
                }
            }
            else if( msg == WM_MOUSEWHEEL )
            {
                int nMouseWheelDelta = (short) HIWORD(wparam);

                // now resize based on size of bounding sphere
                float fDelta = (float)-nMouseWheelDelta/120.0f * m_pdeSelected->fRadius * 0.1f;

                vNewDelta.x = 0;
                vNewDelta.y = 0;
                vNewDelta.z = fDelta;
                bChange = true;
            }

            if (bChange)
            {
                D3DXMatrixTranslation(&mat, vNewDelta.x, vNewDelta.y, vNewDelta.z);

                if (bAllFrames)
                {
                    pdeCur = m_pdeHead;
                    while (pdeCur != NULL)
                    {
                        //pdeCur->pframeRoot->m_vTranslation += vNewDelta;
                        D3DXMatrixMultiply(&pdeCur->pframeRoot->matTrans, &pdeCur->pframeRoot->matTrans, &mat);
                        pdeCur = pdeCur->pdeNext;
                    }
                }
                else if (m_pframeSelected != NULL)
                {
                    if (bMoveRootFrame)
                    {
                        D3DXMatrixMultiply(&m_pdeSelected->pframeRoot->matTrans, &m_pdeSelected->pframeRoot->matTrans, &mat);
                    }
                    else
                    {
                        D3DXMatrixMultiply(&m_pframeSelected->matTrans, &m_pframeSelected->matTrans, &mat);
                    }
                    //m_pframeSelected->m_vTranslation += vNewDelta;
                }
            }

            if( msg == WM_MOUSEMOVE ) 
            {
                //update the current mouse position
                m_vCurMousePosition = vNewPosition;
            }
            break;
        }
    }

    return CD3DXApplication::OnMessage(hwnd, msg, wparam, lparam);
}
