/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"

#include "ShowNormal.h"

#define SKINNEDFVF D3DFVF_XYZB4|D3DFVF_NORMAL
#define SKINNEDFVFSTRIDE 40

#define INDEXSKINNEDFVF D3DFVF_XYZB5|D3DFVF_NORMAL|D3DFVF_LASTBETA_D3DCOLOR
#define INDEXSKINNEDFVFSTRIDE 44

CShowNormals::CShowNormals()
  :m_pDevice(NULL),
    m_pVertexBuffer(NULL),
    m_bSkinning(FALSE),
    m_bIndexedSkinning(FALSE),
    m_bHWVertexShaders(FALSE),
    m_rgaeAttributeTable(NULL),
    m_caeAttributeTable(0)
{
}

CShowNormals::~CShowNormals()
{
    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDevice);

    delete []m_rgaeAttributeTable;
}

    // dwTexStage is the texture stage to use for drawing vectors, 
    //                  if 0xffffffff then the normal field will be used
HRESULT
CShowNormals::Init
    (
    ID3DXBaseMesh *ptmMesh, 
    DWORD dwTexStage,
    float fLength
    )
{
    HRESULT hr = S_OK;
    PBYTE pbSrc = NULL;
    PBYTE pbDest = NULL;
    DWORD iVertex;
    DWORD cVertices;
    DXCrackFVF cfvfDest(D3DFVF_XYZ);
    CD3DXCrackDecl1 cdSrc;
    D3DXVECTOR3 vPos;
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 vZero(0.0f, 0.0f, 0.0f);
    float fSum;
    float fWeight;
    DWORD iWeight;
    D3DCAPS9 Caps;
    DWORD cbSizeOfVector;
    DWORD cWeights;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    UINT iAttrib;

    m_fScale = fLength;

    hr = CreateEmptyOutline();
    if (FAILED(hr))
        goto e_Exit;

    if (ptmMesh == NULL)
        goto e_Exit;

    cVertices = ptmMesh->GetNumVertices();

    ptmMesh->GetDeclaration(pDecl);
    cdSrc.SetDeclaration(pDecl);;

    // if the texture stage is 
    if ((dwTexStage != UNUSED32) && !cdSrc.BTexCoord(dwTexStage))
        goto e_Exit;

    if (dwTexStage != UNUSED32)
        cbSizeOfVector = x_rgcbTypeSizes[cdSrc.rgpTextureElements[dwTexStage]->Type];

    if ((dwTexStage == UNUSED32) && !cdSrc.BNormal())
        goto e_Exit;

    ptmMesh->GetDevice(&m_pDevice);

    m_pDevice->GetDeviceCaps(&Caps);
    m_bHWVertexShaders = Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);

    hr = ptmMesh->GetAttributeTable(NULL, &m_caeAttributeTable);
    if (FAILED(hr))
        goto e_Exit;

    if (m_caeAttributeTable == 0)
    {
        // not attribute sorted!  just return for now
        goto e_Exit;
    }

    m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[m_caeAttributeTable];
    if (m_rgaeAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    if ((cdSrc.CWeights() > 0) && !cdSrc.BIndexedWeights())
    {
        m_bSkinning = TRUE;
        m_bIndexedSkinning = FALSE;
        cfvfDest = DXCrackFVF(SKINNEDFVF);
    }
    else if (cdSrc.CWeights() > 0)
    {
        m_bSkinning = TRUE;
        m_bIndexedSkinning = TRUE;
        cfvfDest = DXCrackFVF(INDEXSKINNEDFVF);
    }
    else
    {
        m_bSkinning = FALSE;
    }

    hr = ptmMesh->GetAttributeTable(m_rgaeAttributeTable, NULL);
    if (FAILED(hr))
        goto e_Exit;

    cVertices = 0;
    for (iAttrib = 0; iAttrib < m_caeAttributeTable; iAttrib++)
    {
        cVertices = max(cVertices, m_rgaeAttributeTable[iAttrib].VertexStart + m_rgaeAttributeTable[iAttrib].VertexCount);
    }
    // get the maximum vertex index

    hr = m_pDevice->CreateVertexBuffer(cVertices * 2 * cfvfDest.m_cBytesPerVertex, 
                    D3DUSAGE_WRITEONLY|(m_bHWVertexShaders ? 0:D3DUSAGE_SOFTWAREPROCESSING), 
                    cfvfDest.m_dwFVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);

    hr = ptmMesh->LockVertexBuffer(D3DLOCK_READONLY, (PVOID*)&pbSrc);
    if (FAILED(hr))
    {
        hr = CreateEmptyOutline();
        goto e_Exit;   
    }

    hr = m_pVertexBuffer->Lock(0, 0, (PVOID*)&pbDest, D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
        goto e_Exit;   

    if (m_bSkinning)
    {
        // first figure out the counts for the number of creases per attribute group
        for (iVertex = 0; iVertex < cVertices; iVertex++)
        {
            vPos = *cdSrc.PvGetPosition(pbSrc);

            // if use the normal, easy
            if (dwTexStage == UNUSED32)
            {
                vNormal = *cdSrc.PvGetNormal(pbSrc);
            }
            else  // else grab a texture coordinate as a vector
            {
                // set all parts of the vector to zero
                memset(&vNormal, 0, sizeof(D3DXVECTOR3));

                // copy the parts found in the mesh
                memcpy(&vNormal, pbSrc + cdSrc.rgpTextureElements[dwTexStage]->Offset, cbSizeOfVector);

            }
            D3DXVec3Normalize(&vNormal, &vNormal);


            // first copy the weights that exist in the source
            fSum = 0.0f;
            cWeights = cdSrc.BIndexedWeights() ? cdSrc.CWeights() - 1: cdSrc.CWeights();
            for (iWeight = 0; iWeight < cWeights; iWeight++)
            {
                fWeight = cdSrc.FGetWeight(pbSrc, iWeight);
                fSum += fWeight;

                cfvfDest.SetWeight(pbDest, iWeight, fWeight);
                cfvfDest.SetWeight(pbDest + cfvfDest.m_cBytesPerVertex, iWeight, fWeight);
            }

            // now set the implied weight - always exists in dest
            cfvfDest.SetWeight(pbDest, iWeight, 1.0f - fSum);
            cfvfDest.SetWeight(pbDest + cfvfDest.m_cBytesPerVertex, iWeight, 1.0f - fSum);
            iWeight += 1;

            // set any other weights that are unused to 0.0f
            cWeights = cdSrc.BIndexedWeights() ? cfvfDest.CWeights() - 1: cfvfDest.CWeights();
            for (; iWeight < cWeights; iWeight++)
            {
                cfvfDest.SetWeight(pbDest, iWeight, 0.0f);
                cfvfDest.SetWeight(pbDest + cfvfDest.m_cBytesPerVertex, iWeight, 0.0f);
            }

            // if indexed skinning, copy the indices
            if (cdSrc.BIndexedWeights())
            {
                cfvfDest.SetIndices(pbDest, *cdSrc.PdwGetIndices(pbSrc));
                cfvfDest.SetIndices(pbDest + cfvfDest.m_cBytesPerVertex, *cdSrc.PdwGetIndices(pbSrc));
            }

            // set one normal to zero and the other to the correct value
            //   both points have the same position though.  the normal
            //   is added post skinning 

            // normal to zero
            cfvfDest.SetPosition(pbDest, &vPos);
            cfvfDest.SetNormal(pbDest, &vZero);
            pbDest += cfvfDest.m_cBytesPerVertex;

            // correct normal
            cfvfDest.SetPosition(pbDest, &vPos);
            cfvfDest.SetNormal(pbDest, &vNormal);
            pbDest += cfvfDest.m_cBytesPerVertex;

            pbSrc += cdSrc.m_cBytesPerVertex;
        }
    }
    else
    {
        // first figure out the counts for the number of creases per attribute group
        for (iVertex = 0; iVertex < cVertices; iVertex++)
        {
            vPos = *cdSrc.PvGetPosition(pbSrc);
            // show binormals

            // if use the normal, easy
            if (dwTexStage == UNUSED32)
            {
                vNormal = *cdSrc.PvGetNormal(pbSrc);
            }
            else  // else grab a texture coordinate as a vector
            {
                // set all parts of the vector to zero
                memset(&vNormal, 0, sizeof(D3DXVECTOR3));

                // copy the parts found in the mesh
                memcpy(&vNormal, pbSrc + cdSrc.rgpTextureElements[dwTexStage]->Offset, cbSizeOfVector);

            }
            D3DXVec3Normalize(&vNormal, &vNormal);
            vNormal *= fLength;
            pbSrc += cdSrc.m_cBytesPerVertex;

            cfvfDest.SetPosition(pbDest, &vPos);
            pbDest += cfvfDest.m_cBytesPerVertex;

            vPos += vNormal;
            cfvfDest.SetPosition(pbDest, &vPos);
            pbDest += cfvfDest.m_cBytesPerVertex;
        }
    }

e_Exit:

    if (pbSrc != NULL)
        ptmMesh->UnlockVertexBuffer();

    if (pbDest != NULL)
        m_pVertexBuffer->Unlock();

    return hr;
}

// matrix names for variables in FX file
const char *x_rgszMatrix[] =
{
 "mWd1", "mWd2", "mWd3", "mWd4", "mWd5", "mWd6", "mWd7", "mWd8", "mWd9",
 "mWd10", "mWd11", "mWd12", "mWd13", "mWd14", "mWd15", "mWd16", "mWd17", "mWd18", "mWd19",
 "mWd20", "mWd21", "mWd22", "mWd23", "mWd24", "mWd25", "mWd26", "mWd27", "mWd28"
};

HRESULT
CShowNormals::Draw
    (
    DWORD iAttrib, 
    DWORD iVertex,
    LPD3DXEFFECT pfxShowNormals, 
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
    D3DXVECTOR4 vScale(m_fScale, 0.0f, 0.0f,0.0f);
    D3DXCOLOR vClr(dwColor);
	BOOL dwSoftwareMode;
    DWORD cLines;
    DWORD cLinesCur;
    DWORD iVertexStart;
    DWORD iMatrix;
    BOOL bChangedSWMode = FALSE;
    DWORD iSubset;

    if (m_bSkinning)
    {
        if (!m_bIndexedSkinning)
        {
            pfxCurrent = pfxShowNormals;
            dwFVF = SKINNEDFVF;
            cBytesPerVertex = SKINNEDFVFSTRIDE;
            pfxShowNormals->SetTechnique(pfxShowNormals->GetTechnique(0));

            m_pDevice->GetTransform(D3DTS_VIEW,&matView);
            m_pDevice->GetTransform(D3DTS_PROJECTION,&matProj);
            m_pDevice->GetTransform(D3DTS_WORLD,&matWorld);

            //D3DXMatrixMultiply(&matTot, &matWorld,&matView);
            D3DXMatrixMultiply(&matTot,&matView,&matProj);
            pfxShowNormals->SetMatrix("mTot", &matTot);                       

            m_pDevice->GetTransform(D3DTS_WORLD,&matWorld);
            pfxShowNormals->SetMatrix("mWd1", &matWorld);                       

            m_pDevice->GetTransform(D3DTS_WORLD1,&matWorld);
            pfxShowNormals->SetMatrix("mWd2", &matWorld);                       

            m_pDevice->GetTransform(D3DTS_WORLD2,&matWorld);
            pfxShowNormals->SetMatrix("mWd3", &matWorld);                       

            m_pDevice->GetTransform(D3DTS_WORLD3,&matWorld);
            pfxShowNormals->SetMatrix("mWd4", &matWorld);                       

        }
        else
        {
            dwFVF = INDEXSKINNEDFVF;
            pfxCurrent = pfxShowNormals;
            cBytesPerVertex = INDEXSKINNEDFVFSTRIDE;
            pfxShowNormals->SetTechnique(pfxShowNormals->GetTechnique(1));

            m_pDevice->GetTransform(D3DTS_VIEW,&matView);
            m_pDevice->GetTransform(D3DTS_PROJECTION,&matProj);

            D3DXMatrixMultiply(&matTot,&matView,&matProj);
            pfxShowNormals->SetMatrix("mTot", &matTot);                       

            pfxShowNormals->SetMatrixArray("mWorlds", rgmIndexedMatrices, 28);

            //for (iMatrix = 0; iMatrix < cIndexedMatrices; iMatrix++)
            //{
              //  pfxShowNormals->SetMatrix(x_rgszMatrix[iMatrix], &rgmIndexedMatrices[iMatrix]);
            //}
        }

        pfxShowNormals->SetVector("vScl", &vScale);                       
        pfxShowNormals->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       

        dwSoftwareMode = m_pDevice->GetSoftwareVertexProcessing();
        if (!m_bHWVertexShaders)
        {
            bChangedSWMode = TRUE;
            m_pDevice->SetSoftwareVertexProcessing(TRUE);
        }
        // if in the wrong mode, then switch
        else if (dwSoftwareMode && m_bHWVertexShaders)
        {
            bChangedSWMode = TRUE;
            m_pDevice->SetSoftwareVertexProcessing(FALSE);
        }
    }
    else
    {
        pfxCurrent = pfxColor;
        dwFVF = D3DFVF_XYZ;
        cBytesPerVertex = 12;

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
        if (m_rgaeAttributeTable[iSubset].VertexCount > 0)
        {
            pfxCurrent->Begin(&cPasses, 0);

            for (iPass = 0; iPass < cPasses; iPass++)
            {
                m_pDevice->SetFVF(dwFVF);

                pfxCurrent->BeginPass(iPass);

                m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, cBytesPerVertex);

                if (iVertex == UNUSED32)
                {
                    iVertexStart = m_rgaeAttributeTable[iSubset].VertexStart;
                    cLines = m_rgaeAttributeTable[iSubset].VertexCount;

                    while (cLines > 0)
                    {
                        cLinesCur = min(cLines, (0xffff / 2));

                        hr = m_pDevice->DrawPrimitive(D3DPT_LINELIST, 
                                                     iVertexStart * 2, 
                                                     cLinesCur);
                        if (FAILED(hr))
                            return hr;

                        iVertexStart += cLinesCur * 2;
                        cLines -= cLinesCur;
                    }
                }
                else
                {
                    hr = m_pDevice->DrawPrimitive(D3DPT_LINELIST, 
                                                 iVertex * 2, 1);
                }

                pfxCurrent->EndPass();
            }

            pfxCurrent->End();

        }
    }

    if (bChangedSWMode)
    {
        m_pDevice->SetSoftwareVertexProcessing(dwSoftwareMode);
    }

    return S_OK;
}

HRESULT
CShowNormals::CreateEmptyOutline()
{
    delete []m_rgaeAttributeTable;
    m_rgaeAttributeTable = NULL;
    m_caeAttributeTable = 0;

    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDevice);

    return S_OK;
}
