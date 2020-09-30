/*//////////////////////////////////////////////////////////////////////////////
//
// File: mdraw.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/
 
#include "mviewpch.h"

const WORD x_rglineBBox[24] = { 0, 1,  0, 2,  0, 4,  7, 6,  7, 5,  7, 3,
                                1, 3,  1, 5,  2, 3,  2, 6,  4, 5,  4, 6 };

D3DTRANSFORMSTATETYPE worldMatHandles[] = { D3DTS_WORLD,
                                            D3DTS_WORLD1,
                                            D3DTS_WORLD2,
                                            D3DTS_WORLD3 };
HRESULT GenerateMesh(SMeshContainer *pmcMesh);

HRESULT 
TrivialData::DrawSubset
    (
    LPD3DXBASEMESH pMesh, 
    LPD3DXEFFECT pEffect, 
    SEffectInfo *pEffectInfo, 
    DWORD ipattr, 
    SMeshContainer *pmcMesh, 
    BOOL bSelected, 
    D3DXMATRIX *rgmIndexedMatrices, 
    DWORD cIndexedMatrices
    )
{
    static LPCSTR mWd[8] = { "mWd0", "mWd1", "mWd2", "mWd3",  "mWd4", "mWd5", "mWd6", "mWd7"};

    HRESULT hr = S_OK;
    UINT iPass;
    UINT cPasses;
    LPD3DXEFFECT pfx;
    DWORD iae;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
    LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
    DWORD cBytesPerVertex;

    if (pmcMesh->bNPatchMode && m_bHWNPatches)
    {
        float fSegments;
        fSegments = (float)pmcMesh->cTesselateLevel + 1.0f;
        m_pDevice->SetNPatchMode(fSegments);
    }

    // Strip support is currently disabled
#if 0
    if (pmcMesh->m_rgpStrips != NULL)
    {
        LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
        DWORD dwFVF;
        DWORD cBytesPerVertex;

        dwFVF = pMesh->GetFVF();
        cBytesPerVertex = D3DXGetFVFVertexSize(dwFVF);
        pMesh->GetVertexBuffer(&pVertexBuffer);

        m_pDevice->SetFVF(dwFVF);
        m_pDevice->SetStreamSource(0, pVertexBuffer, 0, cBytesPerVertex);
        m_pDevice->SetIndices(pmcMesh->m_rgpStrips[ipattr]);

        if (pmcMesh->m_rgcStripIndices != NULL)
        {
            hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 
                                         0, 0, pMesh->GetNumVertices(),
                                         0, pmcMesh->m_rgcStripIndices[ipattr] - 2);
            if (FAILED(hr))
                goto e_Exit;
        }
        else
        {
            DWORD iStripStart;
            DWORD iStrip;

            iStripStart = 0;
            for (iStrip = 0; iStrip < pmcMesh->m_rgcStrips[ipattr]; iStrip++)
            {
                hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 
                                             0, 0, pMesh->GetNumVertices(),
                                             iStripStart, m_pmcSelectedMesh->m_rgrgcStripCounts[ipattr][iStrip]);
                if (FAILED(hr))
                    goto e_Exit;

                iStripStart += 2 + m_pmcSelectedMesh->m_rgrgcStripCounts[ipattr][iStrip];
            }
        }

        pVertexBuffer->Release();
    }
    else //if ((pEffect != NULL))// && !pEffect->BDefaultEffect())
#endif
    {
        GXASSERT(pEffect != NULL);

        D3DXMATRIX matView, matProj, matWorld, matTot;
        
        m_pDevice->GetTransform(D3DTS_VIEW,&matView);
        m_pDevice->GetTransform(D3DTS_PROJECTION,&matProj);

        hr = SetEffectMatrices(pEffect, pEffectInfo, &matProj, &matView, rgmIndexedMatrices, cIndexedMatrices,  NULL, 0, m_pdeHead->fCurTime);
        if (FAILED(hr))
            goto e_Exit;

        pEffect->Begin(&cPasses, 0);

        for (iPass = 0; iPass < cPasses; iPass++)
        {

            pEffect->BeginPass(iPass);

            hr = pMesh->DrawSubset(ipattr);
            if (FAILED(hr))
                goto e_Exit;

            pEffect->EndPass();
        }
        pEffect->End();
    }

    if (pmcMesh->bNPatchMode && m_bHWNPatches)
    {
        m_pDevice->SetNPatchMode(0);
    }

    if ((bSelected && m_bFaceSelectionMode && (m_dwFaceSelected != UNUSED32)) 
        || (bSelected && m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32)) 
        || m_bEdgeMode || m_bNPatchEdgeMode || m_bStripMode || m_bCreaseMode || m_bNormalsMode || m_bAdjacencyMode || m_dwTexCoordsShown)
    {
        DWORD dwFillMode;

        D3DXMATRIX mTrans(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);
        D3DXMATRIX mTransOrig;

        m_pDevice->GetRenderState( D3DRS_FILLMODE, &dwFillMode );
        m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );


        hr = m_pDevice->GetTransform(D3DTS_VIEW, &mTrans);
        if(FAILED(hr))
                return hr;

        mTransOrig = mTrans;

        mTrans._43 -= (m_pdeSelected == NULL ? m_pdeHead : m_pdeSelected)->fRadius / 800.0f;

        hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTrans);
        if(FAILED(hr))
            goto e_Exit;

        if (m_bAdjacencyMode && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)) && (!pmcMesh->bNPatchMode))
        {
            hr = pmcMesh->m_aoAdjacency.Draw(m_pDevice, ipattr, m_pfxSkinnedAdjacency, m_pfxFaceSelect, 0xff004f4f, rgmIndexedMatrices, cIndexedMatrices);
            if(FAILED(hr))
                return hr;

            // offset view some more in case of strip mode.  so strip mode will show up on top
            mTrans._43 -= (m_pdeSelected == NULL ? m_pdeHead : m_pdeSelected)->fRadius / 800.0f;

            hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTrans);
            if(FAILED(hr))
                    return hr;
        }

        if (m_bStripMode && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)) && (!pmcMesh->bNPatchMode))
        {
            hr = pmcMesh->m_soStrips.Draw(m_pDevice, ipattr, m_pfxSkinnedAdjacency, m_pfxFaceSelect, 0xff40a0ff, rgmIndexedMatrices, cIndexedMatrices);
            if(FAILED(hr))
                return hr;
        }

        // do edge mode first, so selected face and other options are above it
        if (m_bEdgeMode)
        {
            D3DXCOLOR color(0.0f, 0.0f, 0.0f, 1.0f);

            if (pmcMesh->bNPatchMode && m_bHWNPatches)
            {
                float fSegments;
                fSegments = (float)pmcMesh->cTesselateLevel + 1.0f;
                m_pDevice->SetNPatchMode(fSegments);
            }

            m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
            m_pfxFaceSelect->Begin(&cPasses, 0);

            for (iPass = 0; iPass < cPasses; iPass++)
            {
                m_pfxFaceSelect->BeginPass(iPass);

                hr = pMesh->DrawSubset( ipattr );
                if(FAILED(hr))
                    goto e_Exit;

                m_pfxFaceSelect->EndPass();
            }
            m_pfxFaceSelect->End();

            if (pmcMesh->bNPatchMode && m_bHWNPatches)
            {
                m_pDevice->SetNPatchMode(0);
            }

            // if edge mode, move the view transform again, so that the later options will be on top
            mTrans._43 -= (m_pdeSelected == NULL ? m_pdeHead : m_pdeSelected)->fRadius / 800.0f;

            hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTrans);
            if(FAILED(hr))
                goto e_Exit;
        }

        // do npatch edge mode next, so selected face and other options are above it
        if (m_bNPatchEdgeMode && pmcMesh->bNPatchMode)
        {
            D3DXCOLOR color(1.0f, 0.0f, 1.0f, 1.0f);

            m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
            m_pfxFaceSelect->Begin(&cPasses, 0);

            for (iPass = 0; iPass < cPasses; iPass++)
            {
                m_pfxFaceSelect->BeginPass(iPass);

                hr = pmcMesh->m_npoNPatchOutline.Draw(pmcMesh->pSWTesselatedMesh, ipattr);
                if(FAILED(hr))
                    goto e_Exit;

                m_pfxFaceSelect->EndPass();
            }
            m_pfxFaceSelect->End();

            // if edge mode, move the view transform again, so that the later options will be on top
            mTrans._43 -= (m_pdeSelected == NULL ? m_pdeHead : m_pdeSelected)->fRadius / 800.0f;

            hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTrans);
            if(FAILED(hr))
                goto e_Exit;
        }

        if (bSelected && (m_dwFaceSelectedAttr == ipattr))
        {
            LPD3DXBASEMESH pMeshCur;
            
            if (pmcMesh->bNPatchMode)
                pMeshCur = pmcMesh->pMeshToTesselate;
            else // skinned, pm, etc
                pMeshCur = pmcMesh->ptmDrawMesh;

            if (m_bFaceSelectionMode && (m_dwFaceSelected != UNUSED32))
            {
                D3DXCOLOR color(1.0f, 1.0f, 0.0f, 1.0f);

                if (!pmcMesh->bNPatchMode)
                {
                    DWORD dwFVF;
                    DWORD cBytesPerVertex;
                    PBYTE pbVertices;
                    PBYTE pbCur;
                    PBYTE pbVerticesSrc;
                    DWORD *pwFace;
        
                    //pMeshCur = pmcMesh->m_pSkinnedMesh != NULL ? pmcMesh->m_pSkinnedMesh : pMesh;

                    dwFVF = pMeshCur->GetFVF();
                    cBytesPerVertex = D3DXGetFVFVertexSize(dwFVF);
                    pMeshCur->GetVertexBuffer(&pVertexBuffer);
                    pMeshCur->GetIndexBuffer(&pIndexBuffer);

                    pbVertices = (PBYTE)_alloca(cBytesPerVertex * 3);

                    m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
                    m_pfxFaceSelect->Begin(&cPasses, 0);

                    for (iPass = 0; iPass < cPasses; iPass++)
                    {
                        m_pfxFaceSelect->BeginPass(iPass);

                        m_pDevice->SetFVF(dwFVF);

                        if (pMeshCur->GetOptions() & D3DXMESH_32BIT)
                        {
                            pMeshCur->LockVertexBuffer(0, (PVOID*)&pbVerticesSrc);
                            pMeshCur->LockIndexBuffer(0, (PVOID*)&pwFace);
                            pwFace += m_dwFaceSelected * 3;

                            pbCur = pbVertices;
                            memcpy(pbCur, pbVerticesSrc + pwFace[0] * cBytesPerVertex, cBytesPerVertex);
                            pbCur += cBytesPerVertex;
                            memcpy(pbCur, pbVerticesSrc + pwFace[1] * cBytesPerVertex, cBytesPerVertex);
                            pbCur += cBytesPerVertex;
                            memcpy(pbCur, pbVerticesSrc + pwFace[2] * cBytesPerVertex, cBytesPerVertex);
                            pbCur += cBytesPerVertex;

                            pMeshCur->UnlockVertexBuffer();
                            pMeshCur->UnlockIndexBuffer();

                            hr = m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pbVertices, cBytesPerVertex);
                            if (FAILED(hr))
                                goto e_Exit;                
                        }
                        else
                        {
                            m_pDevice->SetStreamSource(0, pVertexBuffer, 0, cBytesPerVertex);
                            m_pDevice->SetIndices(pIndexBuffer);

                            hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
                                                         0, 0, pMeshCur->GetNumVertices(),
                                                         m_dwFaceSelected * 3, 1);
                            if(FAILED(hr))
                                goto e_Exit;
                        }

                        m_pfxFaceSelect->EndPass();
                    }

                    m_pfxFaceSelect->End();

                    pVertexBuffer->Release();
                    pIndexBuffer->Release();
                }
                else // show npatch selected face
                {
                    m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
                    m_pfxFaceSelect->Begin(&cPasses, 0);

                    for (iPass = 0; iPass < cPasses; iPass++)
                    {
                        m_pfxFaceSelect->BeginPass(iPass);

                        hr = pmcMesh->m_npoNPatchOutline.DrawFace(pmcMesh->pSWTesselatedMesh, m_dwFaceSelected, ipattr);
                        if(FAILED(hr))
                            goto e_Exit;

                        m_pfxFaceSelect->EndPass();
                    }
                    m_pfxFaceSelect->End();
                }
            }

            if (m_bVertexSelectionMode && (m_dwVertexSelected != UNUSED32)) 
            {
                DWORD dwFVF;
                DWORD cBytesPerVertex;
                LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
                LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
                //pMeshCur = pmcMesh->m_pSkinnedMesh != NULL ? pmcMesh->m_pSkinnedMesh : pMesh;

                dwFVF = pMeshCur->GetFVF();
                cBytesPerVertex = D3DXGetFVFVertexSize(dwFVF);
                pMeshCur->GetVertexBuffer(&pVertexBuffer);

                GXASSERT(m_dwVertexSelected < pMeshCur->GetNumVertices());

                m_pfxVertSelect->SetVector("vcol", &D3DXVECTOR4(1.0, 0.0, 0.0f, 1.0f));
                m_pfxVertSelect->Begin(&cPasses, 0);

                for (iPass = 0; iPass < cPasses; iPass++)
                {
                    m_pfxVertSelect->BeginPass(iPass);

                    m_pDevice->SetFVF(dwFVF);
                    m_pDevice->SetStreamSource(0, pVertexBuffer, 0, cBytesPerVertex);
                    m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVertexSelected, 1);
                 
                    m_pfxVertSelect->EndPass();
                }

                m_pfxVertSelect->End();
                pVertexBuffer->Release();

                // view all normals
                if (m_bNormalsMode && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)))
                {
                    hr = pmcMesh->m_snNormals.Draw(ipattr, m_dwVertexSelected, m_pfxShowNormals, m_pfxFaceSelect, 0xffffff00, rgmIndexedMatrices, cIndexedMatrices);
                    if(FAILED(hr))
                        return hr;
                }

                if ((m_dwTexCoordsShown != 0) && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)))
                {
                    for (DWORD iTexCoord = 0; iTexCoord < x_cpsnTexCoords; iTexCoord++)
                    {
                        if (m_dwTexCoordsShown & (1<<iTexCoord))
                        {
                            hr = pmcMesh->m_rgpsnTexCoords[iTexCoord]->Draw(ipattr, m_dwVertexSelected, m_pfxShowNormals, m_pfxFaceSelect, 0xffffff00, rgmIndexedMatrices, cIndexedMatrices);
                            if(FAILED(hr))
                                return hr;
                        }
                    }
                }
            }
        }

        // UNDONE UNDONE - Crease mode does not work if the mesh is split
        if (m_bCreaseMode && (!pmcMesh->bNPatchMode))
        {
            D3DXCOLOR color(1.0f, 2.0f/3.0f, 0.0f, 1.0f);

            m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
            m_pfxFaceSelect->Begin(&cPasses, 0);

            for (iPass = 0; iPass < cPasses; iPass++)
            {
                m_pfxFaceSelect->BeginPass(iPass);

                hr = pmcMesh->m_eoEdges.Draw(pMesh, ipattr);
                if(FAILED(hr))
                    return hr;

                m_pfxFaceSelect->EndPass();
            }

            m_pfxFaceSelect->End();
        }

        // view all normals and/or texture coords
        if (!m_bVertexSelectionMode )
        {
            if (m_bNormalsMode && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)))
            {
                hr = pmcMesh->m_snNormals.Draw(ipattr, UNUSED32, m_pfxShowNormals, m_pfxFaceSelect, 0xffffff00, rgmIndexedMatrices, cIndexedMatrices);
                if(FAILED(hr))
                    return hr;
            }

            if ((m_dwTexCoordsShown != 0) && ((pmcMesh->pSkinInfo == NULL) || (pmcMesh->m_Method != SOFTWARE)))
            {
                for (DWORD iTexCoord = 0; iTexCoord < x_cpsnTexCoords; iTexCoord++)
                {
                    if (m_dwTexCoordsShown & (1<<iTexCoord))
                    {
                        hr = pmcMesh->m_rgpsnTexCoords[iTexCoord]->Draw(ipattr, UNUSED32, m_pfxShowNormals, m_pfxFaceSelect, 0xffffff00, rgmIndexedMatrices, cIndexedMatrices);
                        if(FAILED(hr))
                            return hr;
                    }
                }
            }
        }

        m_pDevice->SetRenderState( D3DRS_FILLMODE, dwFillMode );

        // restore the view transformation
        hr = m_pDevice->SetTransform(D3DTS_VIEW, &mTransOrig);
        if(FAILED(hr))
            goto e_Exit;
    
    }

    if (m_bShowMeshSelectionMode && bSelected && (ipattr == 0))
    {
        D3DXVECTOR3 rgvPoints[8];
        D3DXCOLOR color(1.0f, 1.0f, 0.0f, 1.0f);
        DWORD dwIndexedVertexBlend, dwVertexBlend;

        rgvPoints[0] = pmcMesh->m_vBoundingBoxMin;
        rgvPoints[1] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMin.x, pmcMesh->m_vBoundingBoxMin.y, pmcMesh->m_vBoundingBoxMax.z);
        rgvPoints[2] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMin.x, pmcMesh->m_vBoundingBoxMax.y, pmcMesh->m_vBoundingBoxMin.z);
        rgvPoints[3] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMin.x, pmcMesh->m_vBoundingBoxMax.y, pmcMesh->m_vBoundingBoxMax.z);
        rgvPoints[4] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMax.x, pmcMesh->m_vBoundingBoxMin.y, pmcMesh->m_vBoundingBoxMin.z);
        rgvPoints[5] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMax.x, pmcMesh->m_vBoundingBoxMin.y, pmcMesh->m_vBoundingBoxMax.z);
        rgvPoints[6] = D3DXVECTOR3(pmcMesh->m_vBoundingBoxMax.x, pmcMesh->m_vBoundingBoxMax.y, pmcMesh->m_vBoundingBoxMin.z);
        rgvPoints[7] = pmcMesh->m_vBoundingBoxMax;

        m_pDevice->GetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, &dwIndexedVertexBlend);
        m_pDevice->GetRenderState(D3DRS_VERTEXBLEND, &dwVertexBlend);
        m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

        m_pfxFaceSelect->SetVector("vClr", (D3DXVECTOR4*)&color);
        m_pfxFaceSelect->Begin(&cPasses, 0);

        for (iPass = 0; iPass < cPasses; iPass++)
        {
            m_pfxFaceSelect->BeginPass(iPass);

            m_pDevice->SetFVF(D3DFVF_XYZ);
            m_pDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, (void*)x_rglineBBox, D3DFMT_INDEX16, rgvPoints, 12);

            m_pfxFaceSelect->EndPass();
        }
        m_pfxFaceSelect->End();

        m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, dwIndexedVertexBlend);
        m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, dwVertexBlend);
    }

e_Exit:
    return hr;
}

HRESULT
TrivialData::DrawMeshContainer
    (
    SMeshContainer *pmcMesh,
    D3DXMATRIX *pmatWorld
    )
{
    UINT ipattr;
    DWORD cAttr;
    HRESULT hr;
    LPD3DXBONECOMBINATION pBoneComb;
    DWORD AttribIdPrev;
    UINT iPass;
    UINT cPasses;
    
    if (pmcMesh->pSkinInfo)
    {
        D3DCAPS9    caps;

        m_pDevice->GetDeviceCaps(&caps);

        if ((m_method != pmcMesh->m_Method) && !pmcMesh->m_bOverrideMethod)
        {
            pmcMesh->m_Method = m_method;
            GenerateMesh(pmcMesh);

            pmcMesh->UpdateViews(m_pdeHead);
        }

        if (pmcMesh->m_Method == D3DNONINDEXED)
        {
            AttribIdPrev = UNUSED32; 
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());

            // Draw using default vtx processing of the device (typically HW)
            for (ipattr = 0; ipattr < pmcMesh->m_cAttributeGroups; ipattr++)
            {
                DWORD numBlend = 0;
                for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
                {
                    if (pBoneComb[ipattr].BoneId[i] != UINT_MAX)
                    {
                        numBlend = i;
                    }
                }

                if (m_Caps.MaxVertexBlendMatrices >= numBlend + 1)
                {
                    for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
                    {
                        DWORD matid = pBoneComb[ipattr].BoneId[i];
                        if (matid != UINT_MAX)
                        {
                            D3DXMatrixMultiply(&m_mMatrices[i], &pmcMesh->pBoneOffsetMatrices[matid], pmcMesh->m_pBoneMatrix[matid]);

                            m_pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &m_mMatrices[i]);
                        }
                    }

                    m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, numBlend);

                    hr = DrawSubset(pmcMesh->ptmDrawMesh, pmcMesh->m_rgpfxAttributes[pBoneComb[ipattr].AttribId], &pmcMesh->m_rgEffectInfo[pBoneComb[ipattr].AttribId], ipattr, pmcMesh, pmcMesh == m_pmcSelectedMesh, m_mMatrices, pmcMesh->m_maxFaceInfl);
                    if(FAILED(hr))
                        return hr;
                }

            }

            // If necessary, draw parts that HW could not handle using SW
            if (pmcMesh->iAttrSplit < pmcMesh->m_cAttributeGroups)
            {
                AttribIdPrev = UNUSED32; 
                if (!m_bSoftwareVP)
                {
                    m_pDevice->SetSoftwareVertexProcessing(TRUE);
                }

                for (ipattr = pmcMesh->iAttrSplit; ipattr < pmcMesh->m_cAttributeGroups; ipattr++)
                {
                    DWORD numBlend = 0;
                    for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
                    {
                        if (pBoneComb[ipattr].BoneId[i] != UINT_MAX)
                        {
                            numBlend = i;
                        }
                    }

                    if (m_Caps.MaxVertexBlendMatrices < numBlend + 1)
                    {
                        for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
                        {
                            DWORD matid = pBoneComb[ipattr].BoneId[i];
                            if (matid != UINT_MAX)
                            {
                                D3DXMatrixMultiply(&m_mMatrices[i], &pmcMesh->pBoneOffsetMatrices[matid], pmcMesh->m_pBoneMatrix[matid]);

                                m_pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &m_mMatrices[i]);
                            }
                        }

                        m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, numBlend);

                        hr = DrawSubset(pmcMesh->ptmDrawMesh, pmcMesh->m_rgpfxAttributes[pBoneComb[ipattr].AttribId], &pmcMesh->m_rgEffectInfo[pBoneComb[ipattr].AttribId], ipattr, pmcMesh, pmcMesh == m_pmcSelectedMesh, m_mMatrices, pmcMesh->m_maxFaceInfl);
                        if(FAILED(hr))
                            return hr;
                    }
                }
                if (!m_bSoftwareVP)
                    m_pDevice->SetSoftwareVertexProcessing(FALSE);
            }
            m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

        }
        else if (pmcMesh->m_Method == D3DINDEXED)
        {
           if ((caps.MaxVertexBlendMatrixIndex / 2 /*div2 because we need the inverse transpose for the normals*/) < pmcMesh->pSkinInfo->GetNumBones())
            {
                // UNDONE UNDONE - need to know when vertex shaders are doing the skinning
                if ((pmcMesh->m_iPaletteSize == x_iDefaultSkinningPaletteSize) && (!m_bSoftwareVP))
                {
                    m_pDevice->SetSoftwareVertexProcessing(TRUE);
                }
            }

            m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, pmcMesh->m_maxFaceInfl - 1);
            if (pmcMesh->m_maxFaceInfl - 1)
                m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
            for (ipattr = 0; ipattr < pmcMesh->m_cAttributeGroups; ipattr++)
            {
                for (DWORD i = 0; i < pmcMesh->m_iPaletteSize/*paletteSize*/; ++i)
                {
                    DWORD matid = pBoneComb[ipattr].BoneId[i];
                    if (matid != UINT_MAX)
                    {
                        D3DXMatrixMultiply(&m_mMatrices[i], &pmcMesh->pBoneOffsetMatrices[matid], pmcMesh->m_pBoneMatrix[matid]);

                        m_pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &m_mMatrices[i]);
                    }
                }
                
                hr = DrawSubset(pmcMesh->ptmDrawMesh, pmcMesh->m_rgpfxAttributes[pBoneComb[ipattr].AttribId], &pmcMesh->m_rgEffectInfo[pBoneComb[ipattr].AttribId], ipattr, pmcMesh, pmcMesh == m_pmcSelectedMesh, m_mMatrices, pmcMesh->m_iPaletteSize);
                if(FAILED(hr))
                    return hr;
            }
            m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
            m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

            if (caps.MaxVertexBlendMatrixIndex < pmcMesh->pSkinInfo->GetNumBones())
            {
                if ((pmcMesh->m_iPaletteSize == x_iDefaultSkinningPaletteSize) && !m_bSoftwareVP)
                {
                    m_pDevice->SetSoftwareVertexProcessing(FALSE);
                }
            }
        }
        else if (pmcMesh->m_Method == SOFTWARE)
        {
            D3DXMATRIX  Identity;
            PBYTE       pbVerticesSrc;
            PBYTE       pbVerticesDest;

            DWORD       cBones  = pmcMesh->pSkinInfo->GetNumBones();

            // set up bone transforms

            D3DXMATRIX* rgBoneMatrices  = NULL;
            
            rgBoneMatrices  = new D3DXMATRIX[cBones];

            if (rgBoneMatrices == NULL)
            {
                hr = E_OUTOFMEMORY;

                goto e_ExitSOFTWARE;
            }

            {
            for (DWORD iBone = 0; iBone < cBones; ++iBone)
            {
                D3DXMatrixMultiply
                (
                    &rgBoneMatrices[iBone],                 // output
                    &pmcMesh->pBoneOffsetMatrices[iBone], 
                    pmcMesh->m_pBoneMatrix[iBone]
                );
            }
            }

            // set world transform
            D3DXMatrixIdentity(&Identity);

            hr = m_pDevice->SetTransform(D3DTS_WORLD, &Identity);

            if (FAILED(hr))
                goto e_ExitSOFTWARE;

            pmcMesh->m_pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (PVOID*)&pbVerticesSrc);
            pmcMesh->pMesh->LockVertexBuffer(D3DLOCK_DISCARD, (PVOID*)&pbVerticesDest);

            // generate skinned mesh
            hr = pmcMesh->pSkinInfo->UpdateSkinnedMesh(rgBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest);
            pmcMesh->m_pOrigMesh->UnlockVertexBuffer();
            pmcMesh->pMesh->UnlockVertexBuffer();

            if (FAILED(hr))
                goto e_ExitSOFTWARE;

            for (ipattr = 0; ipattr < pmcMesh->m_cAttributeGroups; ipattr++)
            {
                hr = DrawSubset(pmcMesh->ptmDrawMesh, pmcMesh->m_rgpfxAttributes[ipattr], &pmcMesh->m_rgEffectInfo[ipattr], ipattr, pmcMesh, pmcMesh == m_pmcSelectedMesh, m_mMatrices, 1);
                if (FAILED(hr))
                    goto e_ExitSOFTWARE;
            }

e_ExitSOFTWARE:

            delete[] rgBoneMatrices;

            return hr;
        }
    }
    else
    {
        GXASSERT(pmcMesh->m_cAttributeGroups == pmcMesh->NumMaterials);

        m_pDevice->SetTransform(D3DTS_WORLD, pmatWorld);
        m_mMatrices[0] = *pmatWorld;

        for (ipattr = 0; ipattr < pmcMesh->m_cAttributeGroups; ipattr++)
        {
            hr = DrawSubset(pmcMesh->ptmDrawMesh, pmcMesh->m_rgpfxAttributes[ipattr], &pmcMesh->m_rgEffectInfo[ipattr], ipattr, pmcMesh, pmcMesh == m_pmcSelectedMesh, m_mMatrices, 1);
            if(FAILED(hr))
                return hr;
        }

#if 0
        D3DXMATRIX matOrig;
        D3DXMATRIX matNew;

        m_pDevice->GetTransform(D3DTS_WORLD, &matOrig);

        D3DXMatrixAffineTransformation(&matNew, -m_pdeHead->fRadius, NULL, NULL, &m_pdeHead->vCenter);
        D3DXMatrixMultiply(&matNew, &matNew, &matOrig);

        m_pDevice->SetTransform(D3DTS_WORLD, &matNew);

        m_ShowArcball.Draw(m_pfxFaceSelect, 0xffffff00);

        m_pDevice->SetTransform(D3DTS_WORLD, &matOrig);
#endif

    }

    return S_OK;
}

HRESULT
TrivialData::UpdateFrames(SFrame *pframeCur, D3DXMATRIX &matCur)
{
    HRESULT hr = S_OK;
    pframeCur->matCombined = matCur;
    D3DXMatrixMultiply(&pframeCur->matCombined, &pframeCur->matRot, &matCur);
    D3DXMatrixMultiply(&pframeCur->matCombined, &pframeCur->matCombined, &pframeCur->matTrans );
    SFrame *pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = UpdateFrames(pframeChild, pframeCur->matCombined);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }
    return S_OK;
}

HRESULT
TrivialData::DrawFrames(SFrame *pframeCur, UINT &cTriangles, UINT &cVertices)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;

    if (pframeCur->pmcMesh != NULL)
    {
        if(FAILED(hr))
            return hr;
    }

    pmcMesh = pframeCur->pmcMesh;
    while (pmcMesh != NULL)
    {
        if (pmcMesh->ptmDrawMesh != NULL)
        {
            cTriangles += pmcMesh->ptmDrawMesh->GetNumFaces();
            cVertices += pmcMesh->ptmDrawMesh->GetNumVertices();

            hr = DrawMeshContainer(pmcMesh, &pframeCur->matCombined);
            if (FAILED(hr))
                return hr;
        }

        pmcMesh = (SMeshContainer*)pmcMesh->pNextMeshContainer;
    }

    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = DrawFrames(pframeChild, cTriangles, cVertices);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}


HRESULT
TrivialData::OnDraw(float fAspect)
{
    UINT cTriangles = 0;
    UINT cVertices = 0;
    UINT cSelectedTriangles = 0;
    UINT cSelectedVertices = 0;
    HRESULT hr;
        SDrawElement *pdeCur;
    D3DXMATRIX matD3D;
    D3DXMATRIX mOut;
    D3DXMATRIX mCur;
    D3DXVECTOR3 vTemp;
    D3DXMATRIX mObject;
    D3DXMATRIX mTrans;
    D3DLIGHT9 light;
    D3DXVECTOR3 vLightDirection;

    if (m_pdeHead == NULL)
    {
        if(m_pDevice)
            m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(120,120,120), 1.0f, 0 );
        return S_OK;
    }

    D3DXMatrixTranslation(&mTrans, 0, 0, (m_pdeSelected == NULL ? m_pdeHead : m_pdeSelected)->fRadius * 2.8f);

    hr = m_pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&mTrans);
    if(FAILED(hr))
        return hr;

    //m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(89,135,179), 1.0f, 0 );
    m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(120,120,120), 1.0f, 0 );

    pdeCur = m_pdeHead;
    while (pdeCur != NULL)
    {
        hr = m_pDevice->GetLight(0, &light );
        if (FAILED(hr))
            return E_FAIL;

        vLightDirection = D3DXVECTOR3( 0.0f, 0.0f, -1.0f);

        D3DXVec3TransformNormal(&vLightDirection, &vLightDirection, &pdeCur->pframeRoot->matRot);
        light.Direction    = vLightDirection;

        //hr = m_pDevice->SetLight(0, &light );
        if (FAILED(hr))
            return E_FAIL;

        D3DXMatrixIdentity(&mCur);

        hr = UpdateFrames(pdeCur->pframeRoot, mCur);
        if (FAILED(hr))
            return hr;
        hr = DrawFrames(pdeCur->pframeRoot, cTriangles, cVertices);
        if (FAILED(hr))
            return hr;

        pdeCur = pdeCur->pdeNext;
    }

    if (m_pmcSelectedMesh != NULL)
    {
        cSelectedTriangles = m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces();
        cSelectedVertices = m_pmcSelectedMesh->ptmDrawMesh->GetNumVertices();
    }
    else
    {
        cSelectedTriangles = 0;
        cSelectedVertices = 0;
    }

    UpdateStatusBar(cTriangles, cSelectedTriangles, cSelectedVertices);
    return S_OK;
}

#define FPS_FILTER 0.20
#define FPS_UPDATE 10

HRESULT
TrivialData::UpdateFrameRate(float fSecsPerFrame)
{
    float fFramesPerSecNew;
    DWORD dwLastUpdateTime;
    DWORD dwCurTime;

    m_fTime += fSecsPerFrame;

    // first calculate the current fps
    fFramesPerSecNew = 1.0f / fSecsPerFrame;
    if (m_fFramesPerSecCur == 0.0f)
        m_fFramesPerSecCur = fFramesPerSecNew;
    else
        m_fFramesPerSecCur = (float)(fFramesPerSecNew * (1.0 - FPS_FILTER) + m_fFramesPerSecCur * FPS_FILTER);

    // now see if we should update the counter displayed
    dwLastUpdateTime = (DWORD) (m_fLastUpdateTime * 2);
    dwCurTime = (DWORD) (m_fTime * 2);

    if (dwCurTime > dwLastUpdateTime)
    {
        m_fLastUpdateTime = m_fTime;

        m_fFramesPerSec = m_fFramesPerSecCur;
    }

    return S_OK;
}

HRESULT
TrivialData::UpdateStatusBar(DWORD cTotalTriangles, DWORD cTriangles, DWORD cVertices)
{
    char szBuf[256];

    sprintf(szBuf, "%d.%01d fps",
                        (int)( m_fFramesPerSec * 10 ) / 10,
                        (int)( m_fFramesPerSec * 10 ) % 10);
    SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 2, (LPARAM) szBuf); 

    sprintf(szBuf, "%ld tps",
                (DWORD)(m_fFramesPerSec * cTotalTriangles)); //fTrisPerSecCur);
    SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 3, (LPARAM) szBuf); 

    sprintf(szBuf, "%ld tri",
                cTriangles);
    SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 4, (LPARAM) szBuf); 
    
    sprintf(szBuf, "%ld vert",
                cVertices);
    SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 5, (LPARAM) szBuf); 

    return S_OK;
}

HRESULT
TrivialData::UpdateAnimation(SDrawElement *pde, float fSecsPerFrame)
{
    SFrame *pframeCur;

    //pde->fCurTime += fSecsPerFrame * 4800;
    pde->fCurTime += fSecsPerFrame;// * m_fTicksPerSecond;
    if (pde->fCurTime > 1.0e15f)
    {
        pde->fCurTime = 0;

        if (pde->m_pAnimMixer != NULL)
            pde->m_pAnimMixer->ResetTime();
    }

    if (pde->m_pAnimMixer != NULL)
        pde->m_pAnimMixer->AdvanceTime(pde->fCurTime - pde->m_pAnimMixer->GetTime(), NULL);

    return S_OK;
}

HRESULT
TrivialData::OnUpdate(float fSecsPerFrame)
{
    SDrawElement *pdeCur;

    UpdateFrameRate(fSecsPerFrame);

    if (m_bAnimPaused)
        return S_OK;

        pdeCur = m_pdeHead;
        while (pdeCur != NULL)
        {
            UpdateAnimation(pdeCur, fSecsPerFrame);

            pdeCur = pdeCur->pdeNext;
        }

    return S_OK;
}
