/*//////////////////////////////////////////////////////////////////////////////
//
// File: mload.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

HRESULT GenerateMesh(SMeshContainer *pmcMesh);
HRESULT
CalculateBoundingSphereMC(SMeshContainer *pmcMesh, LPD3DXVECTOR3 pCenter, FLOAT *pfRadius);

HRESULT AllocateName(LPCSTR Name, LPTSTR *pNewName)
{
    UINT cbLength;

    if (Name != NULL)
    {
        cbLength = strlen(Name)+1;
        *pNewName = new TCHAR[cbLength];
        if (*pNewName == NULL)
        {
            return E_OUTOFMEMORY;
        }

        memcpy(*pNewName, Name, cbLength*sizeof(TCHAR));
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

// Make a copy of one effect default into another
HRESULT CopyEffectDefault
    (
    LPD3DXEFFECTDEFAULT pDefaultSrc,
    LPD3DXEFFECTDEFAULT pDefaultDest
    )
{
    HRESULT hr;

    hr = AllocateName(pDefaultSrc->pParamName, &pDefaultDest->pParamName);
    if (FAILED(hr))
        return hr;

    pDefaultDest->Type = pDefaultSrc->Type;
    pDefaultDest->NumBytes = pDefaultSrc->NumBytes;
    pDefaultDest->pValue = new BYTE[pDefaultSrc->NumBytes];
    if (pDefaultDest->pValue == NULL)
        return E_OUTOFMEMORY;

    memcpy(pDefaultDest->pValue, pDefaultSrc->pValue, pDefaultDest->NumBytes);

    return hr;
}

// Make a copy of one effect instance into another
HRESULT CopyEffectInstance
    (
    CONST D3DXEFFECTINSTANCE *pEffectSrc,
    LPD3DXEFFECTINSTANCE pEffectDest
    )
{
    HRESULT hr = S_OK;
    UINT iDefault;

    hr = AllocateName(pEffectSrc->pEffectFilename, &pEffectDest->pEffectFilename);
    if (FAILED(hr))
        goto e_Exit;

    pEffectDest->NumDefaults = pEffectSrc->NumDefaults;
    if (pEffectDest->NumDefaults > 0)
    {
        pEffectDest->pDefaults = new D3DXEFFECTDEFAULT[pEffectSrc->NumDefaults];
        if (pEffectDest->pDefaults == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        for (iDefault = 0; iDefault < pEffectSrc->NumDefaults; iDefault++)
        {
            hr = CopyEffectDefault(&pEffectSrc->pDefaults[iDefault], &pEffectDest->pDefaults[iDefault]);
            if (FAILED(hr))
                goto e_Exit;
        }
    }

e_Exit:
    return hr;
}

HRESULT AddNormal
    (
    CD3DXCrackDecl1 &cd,
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE]
    )
{
    UINT iInsert;
    D3DVERTEXELEMENT9 NormalElement;

    iInsert = 0;

    // try to insert the normal in a place that would satisfy FVF conventions
    if (cd.BPosition())
        iInsert += 1;
    if (cd.BWeights())
        iInsert += 1;
    if (cd.BIndexedWeights())
        iInsert += 1;

    if (D3DXGetDeclLength(pDecl) + 1 == MAX_FVF_DECL_SIZE)
    {
        /// UNDONE UNDONE
        // need to add a message box here that we can't load the mesh due to number of vertex elements
        return E_FAIL;
    }

    // Insert a normal element into the decl 
    NormalElement.Stream = 0;
    NormalElement.Type = D3DDECLTYPE_FLOAT3;
    NormalElement.Usage = D3DDECLUSAGE_NORMAL;
    NormalElement.UsageIndex = 0;
    NormalElement.Method = D3DDECLMETHOD_DEFAULT;
    InsertDeclElement(iInsert, &NormalElement, pDecl);

    // update the crack decl with the new normal added
    cd.SetDeclaration(pDecl);              

    return S_OK;
}

HRESULT AddTangent
    (
    CD3DXCrackDecl1 &cd,
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE]
    )
{
    UINT iInsert;
    D3DVERTEXELEMENT9 NormalElement;

    iInsert = 0;

    // try to insert the normal in a place that would satisfy FVF conventions
    if (cd.BPosition())
        iInsert += 1;
    if (cd.BWeights())
        iInsert += 1;
    if (cd.BIndexedWeights())
        iInsert += 1;

    if (D3DXGetDeclLength(pDecl) + 1 == MAX_FVF_DECL_SIZE)
    {
        /// UNDONE UNDONE
        // need to add a message box here that we can't load the mesh due to number of vertex elements
        return E_FAIL;
    }

    // Insert a normal element into the decl 
    NormalElement.Stream = 0;
    NormalElement.Type = D3DDECLTYPE_FLOAT3;
    NormalElement.Usage = D3DDECLUSAGE_TANGENT;
    NormalElement.UsageIndex = 0;
    NormalElement.Method = D3DDECLMETHOD_DEFAULT;
    InsertDeclElement(iInsert, &NormalElement, pDecl);

    // update the crack decl with the new normal added
    cd.SetDeclaration(pDecl);              

    return S_OK;
}

//=============================================================================
//
//  CAllocateHierarchy - Allocate nodes for the frame hierarchy tree
//
//=============================================================================
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:

    // ID3DXInterpolator
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, 
                            LPD3DXFRAME *ppNewFrame)
    {
        HRESULT hr = S_OK;
        SFrame *pFrame;

        *ppNewFrame = NULL;

        pFrame = new SFrame;
        if (pFrame == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        hr = AllocateName(Name, &pFrame->szName);
        if (FAILED(hr))
            goto e_Exit;

        *ppNewFrame = (LPD3DXFRAME)pFrame;
        pFrame = NULL;
e_Exit:
        delete pFrame;
        return hr;
    }

    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData, 
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffects, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer) 
    {
        HRESULT hr;
        SMeshContainer *pMeshContainer = NULL;
        UINT iBone, cBones;
        UINT NumFaces;
        UINT iMaterial;
        LPDIRECT3DDEVICE9 pd3dDevice = NULL;
        LPDIRECT3DTEXTURE9 ptex;
        LPD3DXEFFECT pEffect;
        D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
        CD3DXCrackDecl1 cd;
        DWORD dwSkinningSupport;
        LPD3DXBUFFER pbufErrors;
        D3DXVECTOR3 vMeshCenter;
        FLOAT fMeshRadius;
        BOOL bUsesTangents = FALSE;

        LPD3DXMESH pMesh = NULL;
        LPD3DXPMESH pPMesh = NULL;
        LPD3DXPATCHMESH pPatchMesh = NULL;

        if (pMeshData->Type == D3DXMESHTYPE_PATCHMESH)
            pPatchMesh = pMeshData->pPatchMesh;
        else if (pMeshData->Type == D3DXMESHTYPE_PMESH)
            pPMesh = pMeshData->pPMesh;
        else
            pMesh = pMeshData->pMesh;

        *ppNewMeshContainer = NULL;

        pMeshContainer = new SMeshContainer;
        if (pMeshContainer == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        pMeshContainer->m_Method = g_pData->m_method;
        pMeshContainer->m_iPaletteSize = g_pData->m_iPaletteSize;

        hr = AllocateName(Name, &pMeshContainer->Name);
        if (FAILED(hr))
            goto e_Exit;
        
        // if a patch mesh is found, enable tesselate mode for this node and then generate a mesh via tesselation
        if (pPatchMesh != NULL)
        {
            pPatchMesh->GetDevice(&pd3dDevice);

            pMeshContainer->pPatchMesh = pPatchMesh;
            pPatchMesh->AddRef();

            hr = g_pData->Tesselate(pMeshContainer, FALSE);
            if(FAILED(hr))
                goto e_Exit;

            pPatchMesh->GetDeclaration(pDecl);

            pMeshContainer->bTesselateMode = TRUE;
        }
        // if a pmesh is found, setup this node to enable pmeshes and allocate an adjacency array large enough for the max LOD
        else if (pPMesh != NULL)
        {
            pPMesh->GetDevice(&pd3dDevice);

            pMeshContainer->bPMMeshMode = true;

            pMeshContainer->m_cNumVertices = pPMesh->GetNumVertices();
            pMeshContainer->m_cMaxVerticesSoft = pPMesh->GetMaxVertices();
            pMeshContainer->m_cMinVerticesSoft = pPMesh->GetMinVertices();

            pMeshContainer->pPMMesh = pPMesh;
            pMeshContainer->pPMMesh->AddRef();

            NumFaces = pMeshContainer->pPMMesh->GetMaxFaces();

            pMeshContainer->rgdwAdjacency = new DWORD[NumFaces*3];
            if (pMeshContainer->rgdwAdjacency == NULL) 
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            // get the adjacency from the pmesh rather than the input
            pMeshContainer->pPMMesh->GetAdjacency(pMeshContainer->rgdwAdjacency);

            pMeshContainer->pPMMesh->GetDeclaration(pDecl);
        }
        // standard mesh found, allocate an adjacency buffer of the same size
        else
        {

            pMesh->GetDevice(&pd3dDevice);

            pMeshContainer->pMesh = pMesh;
            pMeshContainer->pMesh->AddRef();

            NumFaces = pMeshContainer->pMesh->GetNumFaces();
            
            pMeshContainer->rgdwAdjacency = new DWORD[NumFaces*3];
            if (pMeshContainer->rgdwAdjacency == NULL) 
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            memcpy(pMeshContainer->rgdwAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);

            pMeshContainer->pMesh->GetDeclaration(pDecl);
        }

        //.now that we have data for the vertex layout, create a crack decl
        cd.SetDeclaration(pDecl);

        // if no materials are found, then generate a "default" material of gray
        if ((pMaterials == NULL) || (NumMaterials == 0))
        {
            pMeshContainer->NumMaterials = 1;
            pMeshContainer->m_cAttributeGroups = 1;
            pMeshContainer->m_rgpfxAttributes = new LPD3DXEFFECT[pMeshContainer->NumMaterials];
            pMeshContainer->m_rgEffectInfo = new SEffectInfo[pMeshContainer->NumMaterials];
            pMeshContainer->rgMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
            if ((pMeshContainer->rgMaterials == NULL) || (pMeshContainer->m_rgEffectInfo == NULL) || (pMeshContainer->m_rgpfxAttributes == NULL))
            {
                pMeshContainer->NumMaterials = 0;
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            memset(pMeshContainer->rgMaterials, 0, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);
            memset(pMeshContainer->m_rgpfxAttributes, 0, sizeof(LPD3DXEFFECT) * pMeshContainer->NumMaterials);

            D3DMATERIAL9 mat;
            memset(&mat, 0, sizeof(D3DMATERIAL9));
            mat.Diffuse.r = 0.5f;
            mat.Diffuse.g = 0.5f;
            mat.Diffuse.b = 0.5f;
            mat.Specular = mat.Diffuse;

            pMeshContainer->rgMaterials[0].MatD3D = mat;
            pMeshContainer->rgMaterials[0].pTextureFilename = NULL;

            D3DXEFFECTINSTANCE EffectInstance;

            memset(&EffectInstance, 0, sizeof(D3DXEFFECTINSTANCE));

            // create effect instance will create a "default" effect when pEffectFilename is NULL
            hr = D3DXCreateEffectInstance(&EffectInstance, pd3dDevice, &pMeshContainer->m_rgpfxAttributes[0], NULL);
            if (FAILED(hr))
                goto e_Exit;

            //hr = GenerateEffectInfo(pMeshContainer->m_rgpfxAttributes[0], &pMeshContainer->m_rgEffectInfo[0]);
            //if (FAILED(hr))
                //goto e_Exit;

            // set our "default" material colors
            pMeshContainer->m_rgpfxAttributes[0]->SetVector("Ambient", (LPD3DXVECTOR4)&mat.Ambient);
            pMeshContainer->m_rgpfxAttributes[0]->SetVector("Diffuse", (LPD3DXVECTOR4)&mat.Diffuse);
            pMeshContainer->m_rgpfxAttributes[0]->SetVector("Specular", (LPD3DXVECTOR4)&mat.Specular);            
        }
        // materials found, so save them off
        else
        {
            pMeshContainer->m_rgpfxAttributes = new LPD3DXEFFECT[NumMaterials];
            pMeshContainer->m_rgEffectInfo = new SEffectInfo[NumMaterials];
            pMeshContainer->rgMaterials = new D3DXMATERIAL[NumMaterials];
            pMeshContainer->pEffects = new D3DXEFFECTINSTANCE[NumMaterials];
            pMeshContainer->NumMaterials = NumMaterials;
            pMeshContainer->m_cAttributeGroups = NumMaterials;
            if ((pMeshContainer->rgMaterials == NULL) || (pMeshContainer->m_rgEffectInfo == NULL) || (pMeshContainer->m_rgpfxAttributes == NULL))
            {
                pMeshContainer->NumMaterials = 0;
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            memset(pMeshContainer->rgMaterials, 0, sizeof(D3DXMATERIAL) * NumMaterials);
            memset(pMeshContainer->m_rgpfxAttributes, 0, sizeof(LPD3DXEFFECT) * NumMaterials);
            memset(pMeshContainer->pEffects, 0, sizeof(D3DXEFFECTINSTANCE) * NumMaterials);

            for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
            {
                hr = CopyEffectInstance(&pEffects[iMaterial], &pMeshContainer->pEffects[iMaterial]);
                if (FAILED(hr))
                    goto e_Exit;

                pMeshContainer->rgMaterials[iMaterial].MatD3D = pMaterials[iMaterial].MatD3D;

                ptex = NULL;
                if (pMaterials[iMaterial].pTextureFilename != NULL)
                {
                    DWORD cchFilename = strlen(pMaterials[iMaterial].pTextureFilename) + 1;
                    pMeshContainer->rgMaterials[iMaterial].pTextureFilename = new char[cchFilename];
                    if (pMeshContainer->rgMaterials[iMaterial].pTextureFilename == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        goto e_Exit;
                    }
                    memcpy(pMeshContainer->rgMaterials[iMaterial].pTextureFilename,
                                    pMaterials[iMaterial].pTextureFilename, sizeof(char) * cchFilename);
                }

                hr = D3DXCreateEffectInstance(&pEffects[iMaterial], pd3dDevice, &pMeshContainer->m_rgpfxAttributes[iMaterial], &pbufErrors);

                if (pbufErrors)
                {
                    MessageBox(NULL, (LPSTR)pbufErrors->GetBufferPointer(), "Effect file errors", MB_OK);
                    GXRELEASE(pbufErrors);
                }

                if (FAILED(hr))
                    goto e_Exit;

                //pMeshContainer->m_rgpfxAttributes[iMaterial]->SetTechnique((LPCSTR)1);

                //hr = GenerateEffectInfo(pMeshContainer->m_rgpfxAttributes[iMaterial], &pMeshContainer->m_rgEffectInfo[iMaterial]);
                //if (FAILED(hr))
                  //  goto e_Exit;
            }
        }


        // if skinning info is found, then we need to setup all info for skinning this mesh container
        if (pSkinInfo != NULL)
        {
            pMeshContainer->pSkinInfo = pSkinInfo;
            pSkinInfo->AddRef();
            pMeshContainer->m_pOrigMesh = pMesh;
            pMesh->AddRef();

            cBones = pSkinInfo->GetNumBones();

            pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
            pMeshContainer->m_pBoneMatrix = new D3DXMATRIX*[cBones];
            if ((pMeshContainer->m_pBoneMatrix == NULL) || (pMeshContainer->pBoneOffsetMatrices == NULL))
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }


            for (iBone = 0; iBone < cBones; iBone++)
            {
                pMeshContainer->pBoneOffsetMatrices[iBone] = *(pSkinInfo->GetBoneOffsetMatrix(iBone));
            }

            hr = GenerateMesh(pMeshContainer);
            if (FAILED(hr))
                goto e_Exit;

            dwSkinningSupport = D3DXST_3WEIGHT;
        }
        else  // non-skinned case, unskinned required
        {
            dwSkinningSupport = D3DXST_UNSKINNED;
        }

        pd3dDevice->SetSoftwareVertexProcessing(TRUE);
        pMeshContainer->m_dwSkinningType = dwSkinningSupport;

        pd3dDevice->SetFVF(D3DFVF_XYZ);

        // now that the skinning mode has been selected, select the first technique to match it and validate
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            hr = SelectTechnique(pMeshContainer->m_rgpfxAttributes[iMaterial], dwSkinningSupport);
            if (FAILED(hr))
            {
                DPF(0, "CreateMeshContainer: No technique found that will validate");
                goto e_Exit;
            }

            hr = GenerateEffectInfo(pMeshContainer->m_rgpfxAttributes[iMaterial], &pMeshContainer->m_rgEffectInfo[iMaterial]);
            if (FAILED(hr))
                goto e_Exit;

            D3DXTECHNIQUE_DESC TechniqueDesc;
            D3DXPASS_DESC PassDesc;
            D3DXHANDLE hPass;
            D3DXHANDLE hTechnique;
            hTechnique = pMeshContainer->m_rgpfxAttributes[iMaterial]->GetCurrentTechnique();
            pMeshContainer->m_rgpfxAttributes[iMaterial]->GetTechniqueDesc(hTechnique, &TechniqueDesc);
            for( UINT iPass = 0; iPass < TechniqueDesc.Passes; iPass++ )
            {
                hPass = pMeshContainer->m_rgpfxAttributes[iMaterial]->GetPass( hTechnique, iPass );
                pMeshContainer->m_rgpfxAttributes[iMaterial]->GetPassDesc( hPass, &PassDesc );

                UINT NumVSSemanticsUsed;
                D3DXSEMANTIC pVSSemantics[MAXD3DDECLLENGTH];

                if( !PassDesc.pVertexShaderFunction ||
                    FAILED( D3DXGetShaderInputSemantics( PassDesc.pVertexShaderFunction, pVSSemantics, &NumVSSemanticsUsed ) ) )
                {
                    continue;
                }

                for( UINT iSem = 0; iSem < NumVSSemanticsUsed; iSem++ )
                {
                    if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_TANGENT )
                    {
                        bUsesTangents = TRUE;
                        break;
                    }
                }
            }
        }

        pd3dDevice->SetSoftwareVertexProcessing(FALSE);

        // handle the case where there is a mesh without normals
        if (pMeshContainer->pMesh != NULL)
        {
            /* if no normals in the mesh, then clone the mesh to add the normals and then compute them */
            if (cd.GetSemanticElement(D3DDECLUSAGE_NORMAL, 0) == NULL)
            {
                LPD3DXMESH pMeshTemp = pMeshContainer->pMesh;

                AddNormal(cd, pDecl);

                pMeshContainer->pMesh = NULL;

                // clone in the space for the new normal
                hr = pMeshTemp->CloneMesh(pMeshTemp->GetOptions(), pDecl, pd3dDevice, &pMeshContainer->pMesh);
    
                GXRELEASE(pMeshTemp);

                if (FAILED(hr))
                    goto e_Exit;
    
                hr = D3DXComputeNormals(pMeshContainer->pMesh, NULL);
                if (FAILED(hr))
                    goto e_Exit;

            }            

            if (bUsesTangents && (NULL == cd.GetSemanticElement(D3DDECLUSAGE_TANGENT, 0)))
            {
                LPD3DXMESH pMeshTemp = pMeshContainer->pMesh;

                AddTangent(cd, pDecl);

                pMeshContainer->pMesh = NULL;

                // clone in the space for the new normal
                hr = pMeshTemp->CloneMesh(pMeshTemp->GetOptions(), pDecl, pd3dDevice, &pMeshContainer->pMesh);
    
                GXRELEASE(pMeshTemp);

                if (FAILED(hr))
                    goto e_Exit;
    
                hr = D3DXComputeTangent( pMeshContainer->pMesh, 0, 0, D3DX_DEFAULT, TRUE, NULL );
                if (FAILED(hr))
                    goto e_Exit;
            }

            // down cast to the base mesh type (could also QI for it)
            GXRELEASE(pMeshContainer->ptmDrawMesh);
            pMeshContainer->ptmDrawMesh = pMeshContainer->pMesh;
            pMeshContainer->ptmDrawMesh->AddRef();

        }
        // handle the case where there is a pmesh without normals
        else if (pMeshContainer->pPMMesh != NULL)
        {
            /* if no normals in the mesh, then clone the mesh to add the normals and then compute them */
            if (cd.GetSemanticElement(D3DDECLUSAGE_NORMAL, 0) == NULL)
            {
                LPD3DXPMESH pPMeshTemp = pMeshContainer->pPMMesh;
                pMeshContainer->pPMMesh = NULL;

                AddNormal(cd, pDecl);

                hr = pPMeshTemp->ClonePMesh(pPMeshTemp->GetOptions(), pDecl, pd3dDevice, &pMeshContainer->pPMMesh);
    
                GXRELEASE(pPMeshTemp);

                if (FAILED(hr))
                    goto e_Exit;
    
                hr = D3DXComputeNormals(pMeshContainer->pPMMesh, NULL);
                if (FAILED(hr))
                    goto e_Exit;

            }

            // need compute tangent to take a base mesh
            if (bUsesTangents && (NULL == cd.GetSemanticElement(D3DDECLUSAGE_TANGENT, 0)))
            {
                LPD3DXMESH pMeshTemp;
                LPD3DXPMESH pPMeshTemp = pMeshContainer->pPMMesh;
                pMeshContainer->pPMMesh = NULL;

                AddTangent(cd, pDecl);

                hr = pPMeshTemp->ClonePMesh(pPMeshTemp->GetOptions(), pDecl, pd3dDevice, &pMeshContainer->pPMMesh);
    
                GXRELEASE(pPMeshTemp);

                if (FAILED(hr))
                    goto e_Exit;

             // normal mesh required for compute tangent, so clone one that uses the same VB and do the work there

                // set to Max LOD so that all vertices will be touched
                pMeshContainer->pPMMesh->SetNumVertices(0xffffffff);

                // clone the mesh NOTE: SHARING the VB
                hr = pMeshContainer->pPMMesh->CloneMesh(pMeshContainer->pPMMesh->GetOptions() | D3DXMESH_VB_SHARE, NULL, pd3dDevice, &pMeshTemp);
                if (FAILED(hr))
                    goto e_Exit;
    
                // compute the tangents on the mesh with the shared VB
                hr = D3DXComputeTangent( pMeshTemp, 0, 0, D3DX_DEFAULT, TRUE, NULL );
                GXRELEASE (pMeshTemp);

                if (FAILED(hr))
                    goto e_Exit;
            }

            // down cast to the base mesh type (could also QI for it)
            GXRELEASE(pMeshContainer->ptmDrawMesh);
            pMeshContainer->ptmDrawMesh = pMeshContainer->pPMMesh;
            pMeshContainer->ptmDrawMesh->AddRef();
        }

        // setup attribute table
        pMeshContainer->ptmDrawMesh->GetAttributeTable(NULL, &pMeshContainer->m_caeAttributeTable);

        // The load function will attribute sort the mesh, just make sure that there is one
        GXASSERT(pMeshContainer->m_caeAttributeTable > 0);

        // allocate an attribute table for convenience purposes
        delete []pMeshContainer->m_rgaeAttributeTable;
        pMeshContainer->m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->m_caeAttributeTable];
        if (pMeshContainer->m_rgaeAttributeTable == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        pMeshContainer->ptmDrawMesh->GetAttributeTable(pMeshContainer->m_rgaeAttributeTable, &pMeshContainer->m_caeAttributeTable);


        hr = CalculateBoundingSphereMC(pMeshContainer, &vMeshCenter, &fMeshRadius);
        if (FAILED(hr))
            goto e_Exit;

        // set the mesh radius/center into the effect
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            SetEffectMeshInfo(pMeshContainer->m_rgpfxAttributes[iMaterial], &vMeshCenter, fMeshRadius);
        }

        *ppNewMeshContainer = (LPD3DXMESHCONTAINER)pMeshContainer;
        pMeshContainer = NULL;
e_Exit:
        GXRELEASE(pd3dDevice);

        // call Destroy function to properly clean up the memory allocated 
        if (pMeshContainer != NULL)
        {
            DestroyMeshContainer((LPD3DXMESHCONTAINER)pMeshContainer);
        }

        return hr;
    }


    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) 
    {
        SFrame *pFrame = (SFrame*)pFrameToFree;

        pFrame->pframeSibling = NULL;
        pFrame->pframeFirstChild = NULL;
        pFrame->pmcMesh = NULL; 
        delete pFrame;
        return S_OK; 
    }

    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase)
    {
        SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

        // DON"T traverse the list of mesh containers to free them in the destructor
        pMeshContainer->pNextMeshContainer = NULL;
        delete pMeshContainer;

        return S_OK;
    }
};

HRESULT
InitViews
    (
    SFrame *pframe, 
    SDrawElement *pde
    )
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcCur = pframe->pmcMesh;
    SFrame *pframeCur;

    while (pmcCur != NULL)
    {
        hr = pmcCur->UpdateViews(pde);
        if (FAILED(hr))
            goto e_Exit;

        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = InitViews(pframeCur, pde);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    return hr;
}

HRESULT
SetupOrigMatrices
    (
    SFrame *pframe
    )
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcCur = pframe->pmcMesh;
    SFrame *pframeCur;

    // remember the starting matrix for saving later
    pframe->matRotOrig = pframe->matRot;

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = SetupOrigMatrices(pframeCur);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    return hr;
}

HRESULT
TrivialData::LoadMeshHierarchyFromFile(char *szFilename)
{
    HRESULT hr = S_OK;
    SDrawElement *pdeMesh = NULL;
    DWORD dwOptions;
    int cchFileName;
    char szPath[256];
    char *szTemp;
    DWORD cchPath;
    CAllocateHierarchy Alloc;

    cchPath = GetFullPathName(szFilename, sizeof(szPath), szPath, &szTemp);
    if ((cchPath == 0) || (sizeof(szPath) <= cchPath))
        return 1;

    // remove the filename from the path part
    szPath[szTemp - szPath] = '\0';

    // set the current directory, so that textures will be searched from there
    SetCurrentDirectory(szPath);

    pdeMesh = new SDrawElement();
    if (pdeMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    pdeMesh->dwTexCoordsCalculated = m_dwTexCoordsShown;

    cchFileName = strlen(szFilename);
    if (cchFileName < 2)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // reset skinning palette size to 255 when loading
    m_iPaletteSize = x_iDefaultSkinningPaletteSize;

    pdeMesh->pframeRoot = new SFrame();
    if (pdeMesh->pframeRoot == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    hr = D3DXLoadMeshHierarchyFromX(szFilename, D3DXMESH_MANAGED, m_pDevice, &Alloc, NULL, (LPD3DXFRAME*)&(pdeMesh->pframeRoot->pframeFirstChild), &pdeMesh->m_pAnimMixer);
    if (FAILED(hr))
        goto e_Exit;

    // the root cannot have any meshes, it is there just for
    //   mouse stuff (rot/tran)
    if (pdeMesh->pframeRoot->pmcMesh != NULL)
    {
        SFrame *pframeNew;

        pframeNew = new SFrame();
        if (pframeNew == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        pframeNew->pframeFirstChild = pdeMesh->pframeRoot;
        pdeMesh->pframeRoot = pframeNew;
    }

    hr = FindBones(pdeMesh->pframeRoot, pdeMesh);
    if (FAILED(hr))
        goto e_Exit;

    // replace the head, only one scene loaded at a time
    delete m_pdeHead;
    m_pdeHead = pdeMesh;


    delete []pdeMesh->szName;
    pdeMesh->szName = new char[cchFileName+1];
    if (pdeMesh->szName == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memcpy(pdeMesh->szName, szFilename, cchFileName+1);

    SelectDrawElement(pdeMesh);
    SelectFrame(pdeMesh->pframeRoot, pdeMesh->pframeRoot->pmcMesh);

    hr = CalculateBoundingSphere(pdeMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = InitViews(pdeMesh->pframeRoot, pdeMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = SetupOrigMatrices(pdeMesh->pframeRoot);
    if (FAILED(hr))
        goto e_Exit;

    SetProjectionMatrix();

    m_pdeSelected->fCurTime = 0.0f;
    m_pdeSelected->fMaxTime = 200.0f;

    D3DXMatrixTranslation(&m_pdeSelected->pframeRoot->matRot,
                                -pdeMesh->vCenter.x, -pdeMesh->vCenter.y, -pdeMesh->vCenter.z);
    m_pdeSelected->pframeRoot->matRotOrig = m_pdeSelected->pframeRoot->matRot;

e_Exit:

    if (FAILED(hr))
    {
                delete pdeMesh;
    }

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
    );

namespace GXU
{
HRESULT WINAPI
    LoadMeshFromM(
        IStream *pstream,
        DWORD options,
        DWORD fvf,
        LPDIRECT3DDEVICE9 pD3D,
        LPD3DXMESH *ppMesh,
        LPD3DXBUFFER *ppbufAdjacency);
}

HRESULT
CalculateSum(SFrame *pframe, D3DXMATRIX *pmatCur, D3DXVECTOR3 *pvCenter, UINT *pcVertices)
{
    HRESULT hr = S_OK;
    PBYTE pbPoints = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
    UINT cVerticesLocal = 0;
    PBYTE pbCur;
    D3DXVECTOR3 *pvCur;
    D3DXVECTOR3 vTransformedCur;
    UINT iPoint;
    SMeshContainer *pmcCur;
    SFrame *pframeCur;
    UINT cVertices;
    D3DXMATRIX matLocal;
    CD3DXCrackDecl1 cd;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    D3DXMatrixMultiply(&matLocal, &pframe->matRot, pmatCur);

    pmcCur = pframe->pmcMesh;
    while (pmcCur != NULL)
    {
        pmcCur->ptmDrawMesh->GetDeclaration(pDecl);
        cd.SetDeclaration(pDecl);

        cVertices = pmcCur->ptmDrawMesh->GetNumVertices();

            hr = pmcCur->ptmDrawMesh->GetVertexBuffer(&pVertexBuffer);
        if (FAILED(hr))
            goto e_Exit;

        hr = pVertexBuffer->Lock(0,0, (PVOID*)&pbPoints, D3DLOCK_NOSYSLOCK );
        if (FAILED(hr))
            goto e_Exit;

        for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += cd.m_cBytesPerVertex )
        {
            pvCur = cd.PvGetPosition(pbCur);

            if ((pvCur->x != 0.0) || (pvCur->y != 0.0) || (pvCur->z != 0.0))
            {
                cVerticesLocal++;

                D3DXVec3TransformCoord(&vTransformedCur, pvCur, &matLocal);

                pvCenter->x += vTransformedCur.x;
                pvCenter->y += vTransformedCur.y;
                pvCenter->z += vTransformedCur.z;
            }
        }



        // calculate the bounding box after calculating part of the global sphere calculation
        hr = D3DXComputeBoundingBox((D3DXVECTOR3*)pbPoints, 
                        cVertices, 
                        cd.m_cBytesPerVertex, 
                        &pmcCur->m_vBoundingBoxMin,
                        &pmcCur->m_vBoundingBoxMax);
        if (FAILED(hr))
            goto e_Exit;

        pVertexBuffer->Unlock();
        GXRELEASE(pVertexBuffer);
        pbPoints = NULL;

        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    *pcVertices += cVerticesLocal;

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = CalculateSum(pframeCur, &matLocal, pvCenter, pcVertices);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    if (pbPoints != NULL)
    {
        GXASSERT(pVertexBuffer != NULL);
        pVertexBuffer->Unlock();
    }
    GXRELEASE(pVertexBuffer);

    return hr;
}

HRESULT
CalculateRadius(SFrame *pframe, D3DXMATRIX *pmatCur, D3DXVECTOR3 *pvCenter, float *pfRadiusSq)
{
    HRESULT hr = S_OK;
    PBYTE pbPoints = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
    PBYTE pbCur;
    D3DXVECTOR3 *pvCur;
    D3DXVECTOR3 vDist;;
    UINT iPoint;
    UINT cVertices;
    SMeshContainer *pmcCur;
    SFrame *pframeCur;
    float fRadiusLocalSq;
    float fDistSq;
    D3DXMATRIX matLocal;
    CD3DXCrackDecl1 cd;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    D3DXMatrixMultiply(&matLocal, &pframe->matRot, pmatCur);

    pmcCur = pframe->pmcMesh;
    fRadiusLocalSq = *pfRadiusSq;
    while (pmcCur != NULL)
    {
        pmcCur->ptmDrawMesh->GetDeclaration(pDecl);
        cd.SetDeclaration(pDecl);

        cVertices = pmcCur->ptmDrawMesh->GetNumVertices();

            hr = pmcCur->ptmDrawMesh->GetVertexBuffer(&pVertexBuffer);
        if (FAILED(hr))
            goto e_Exit;

        hr = pVertexBuffer->Lock(0,0, (PVOID*)&pbPoints, D3DLOCK_NOSYSLOCK );
        if (FAILED(hr))
            goto e_Exit;

        for( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += cd.m_cBytesPerVertex )
        {
            pvCur = cd.PvGetPosition(pbCur);

            if ((pvCur->x == 0.0) && (pvCur->y == 0.0) && (pvCur->z == 0.0))
                continue;

            D3DXVec3TransformCoord(&vDist, pvCur, &matLocal);

            vDist -= *pvCenter;

            fDistSq = D3DXVec3LengthSq(&vDist);

            if( fDistSq > fRadiusLocalSq )
                fRadiusLocalSq = fDistSq;
        }


        pVertexBuffer->Unlock();
        pbPoints = NULL;
        GXRELEASE(pVertexBuffer);

        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    *pfRadiusSq = fRadiusLocalSq;

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = CalculateRadius(pframeCur, &matLocal, pvCenter, pfRadiusSq);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    if (pbPoints != NULL)
    {
        GXASSERT(pVertexBuffer != NULL);
        pVertexBuffer->Unlock();
    }

    GXRELEASE(pVertexBuffer);

    return hr;
}

HRESULT
CalculateBoundingSphere(SDrawElement *pdeCur)
{
    HRESULT hr = S_OK;
    D3DXVECTOR3 vCenter(0,0,0);
    UINT cVertices = 0;
    float fRadiusSq = 0;
    D3DXMATRIX matCur;

    D3DXMatrixIdentity(&matCur);
    hr = CalculateSum(pdeCur->pframeRoot, &matCur, &vCenter, &cVertices);
    if (FAILED(hr))
        goto e_Exit;

    if (cVertices > 0)
    {
        vCenter /= (float)cVertices;

        D3DXMatrixIdentity(&matCur);
        hr = CalculateRadius(pdeCur->pframeRoot, &matCur, &vCenter, &fRadiusSq);
        if (FAILED(hr))
            goto e_Exit;
    }

    pdeCur->fRadius = (float)sqrt((double)fRadiusSq);;
    pdeCur->vCenter = vCenter;
e_Exit:
    return hr;
}

HRESULT
CalculateBoundingSphereMC(SMeshContainer *pmcMesh, LPD3DXVECTOR3 pCenter, FLOAT *pfRadius)
{
    LPVOID pbVertices;

    pmcMesh->ptmDrawMesh->LockVertexBuffer(D3DLOCK_READONLY, &pbVertices);

    // UNDONE UNDONE - to handle general FVFs, this needs to add the offset of the position into pbVertices
    //   this only works with FLOAT3 positions anyhow, so this function isn't useful in some cases
    D3DXComputeBoundingSphere((D3DXVECTOR3*)pbVertices, pmcMesh->ptmDrawMesh->GetNumVertices(), pmcMesh->ptmDrawMesh->GetNumBytesPerVertex(), pCenter, pfRadius);

    pmcMesh->ptmDrawMesh->UnlockVertexBuffer();

    return S_OK;;
}

HRESULT
TrivialData::DeleteSelectedMesh()
{
    if (m_pdeSelected != NULL)
    {
        SDrawElement *pdeCur = m_pdeHead;
        SDrawElement *pdePrev = NULL;
        while ((pdeCur != NULL) && (pdeCur != m_pdeSelected))
        {
            pdePrev = pdeCur;
            pdeCur = pdeCur->pdeNext;
        }

        GXASSERT(pdeCur != NULL);
        GXASSERT(pdeCur == m_pdeSelected);

        if (pdePrev == NULL)
        {
            m_pdeHead = m_pdeHead->pdeNext;
        }
        else
        {
            pdePrev->pdeNext = pdeCur->pdeNext;
        }

        m_pdeSelected->pdeNext = NULL;
        delete m_pdeSelected;

        m_dwVertexSelected = UNUSED32;
        m_dwFaceSelected = UNUSED32;

        SelectDrawElement(m_pdeHead);

        UpdateMeshMenu();

        if (m_pdeHead == NULL)
        {
            SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "Nothing Selected"); 
            SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 2, (LPARAM) ""); 
            SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 3, (LPARAM) ""); 
            SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 4, (LPARAM) ""); 
            SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 5, (LPARAM) ""); 
        }
    }

    return S_OK;
}

char *SkipWhiteSpace(char *sz)
{
    while ((*sz != '\0') && ((*sz == ' ') || (*sz == '\t') || (*sz == '\n')))
        sz++;

    return sz;
}

HRESULT
TrivialData::AddMeshToDrawList
(
    char *szName,
    LPD3DXMESH ptmMesh,
    LPD3DXPMESH ptmPMesh,
    LPD3DXBUFFER pbufAdjacency,
    LPD3DXBUFFER pbufMaterials,
    LPD3DXBUFFER pbufEffectInstances,
    UINT cMaterials
)
{
    HRESULT hr = S_OK;
    SDrawElement *pdeMesh = NULL;
    SMeshContainer *pmcMesh;
    UINT cFaces;
    LPD3DXMATERIAL rgMaterials;
    LPD3DXEFFECTINSTANCE rgEffects = NULL;
    UINT iMaterial;
    LPDIRECT3DTEXTURE9 ptex;
    UINT cchName;
    UINT cchFilename;
    D3DXMATRIX matRHtoLH;
    SFrame *pframeMesh;
    LPD3DXEFFECT pEffect;
    D3DXEFFECTINSTANCE EffectInstance;
    BOOL bUsesTangents = FALSE;

    GXASSERT((ptmMesh != NULL) || (ptmPMesh != NULL));
    GXASSERT((ptmMesh == NULL) || (ptmPMesh == NULL));

    pdeMesh = new SDrawElement();
    if (pdeMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pdeMesh->dwTexCoordsCalculated = m_dwTexCoordsShown;

    pdeMesh->pframeRoot = new SFrame();
    if (pdeMesh->pframeRoot == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // add a second level to be similar to mesh hierarchys
    //   always a frame even if the root one (mouse stuff) is pulled
    pdeMesh->pframeRoot->pframeFirstChild = new SFrame();
    if (pdeMesh->pframeRoot->pframeFirstChild == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    pframeMesh = pdeMesh->pframeRoot->pframeFirstChild;

    pframeMesh->pmcMesh = new SMeshContainer();
    if (pframeMesh->pmcMesh == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    pmcMesh = pframeMesh->pmcMesh;

    if ((pbufMaterials == NULL) || (cMaterials == 0))
    {
        pmcMesh->m_cAttributeGroups = 1;
        pmcMesh->NumMaterials = 1;

        pmcMesh->m_rgpfxAttributes = new LPD3DXEFFECT[pmcMesh->NumMaterials];
        pmcMesh->m_rgEffectInfo = new SEffectInfo[pmcMesh->NumMaterials];
        pmcMesh->rgMaterials = new D3DXMATERIAL[pmcMesh->NumMaterials];
        if ((pmcMesh->m_rgpfxAttributes == NULL) || (pmcMesh->m_rgEffectInfo == NULL) || (pmcMesh->rgMaterials == NULL))
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        D3DMATERIAL9 mat;
        memset(&mat, 0, sizeof(D3DMATERIAL9));
        mat.Diffuse.r = 0.5f;
        mat.Diffuse.g = 0.5f;
        mat.Diffuse.b = 0.5f;
        mat.Specular = mat.Diffuse;

        pmcMesh->rgMaterials[0].MatD3D = mat;
        pmcMesh->rgMaterials[0].pTextureFilename = NULL;

        memset(&EffectInstance, 0, sizeof(D3DXEFFECTINSTANCE));

        // create effect instance will create a "default" effect when pEffectFilename is NULL
        hr = D3DXCreateEffectInstance(&EffectInstance, m_pDevice, &pmcMesh->m_rgpfxAttributes[0], NULL);
        if (FAILED(hr))
            goto e_Exit;

        hr = GenerateEffectInfo(pmcMesh->m_rgpfxAttributes[0], &pmcMesh->m_rgEffectInfo[0]);
        if (FAILED(hr))
            goto e_Exit;

        // set our "default" material colors
        pmcMesh->m_rgpfxAttributes[0]->SetVector("Ambient", (LPD3DXVECTOR4)&mat.Ambient);
        pmcMesh->m_rgpfxAttributes[0]->SetVector("Diffuse", (LPD3DXVECTOR4)&mat.Diffuse);
        pmcMesh->m_rgpfxAttributes[0]->SetVector("Specular", (LPD3DXVECTOR4)&mat.Specular);            
    }
    else
    {
        pmcMesh->m_cAttributeGroups = cMaterials;
        pmcMesh->NumMaterials = cMaterials;

        pmcMesh->m_rgpfxAttributes = new LPD3DXEFFECT[pmcMesh->NumMaterials];
        pmcMesh->m_rgEffectInfo = new SEffectInfo[pmcMesh->NumMaterials];
        pmcMesh->rgMaterials = new D3DXMATERIAL[pmcMesh->NumMaterials];
        if ((pmcMesh->m_rgpfxAttributes == NULL) || (pmcMesh->m_rgEffectInfo == NULL) || (pmcMesh->rgMaterials == NULL))
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        memset(pmcMesh->rgMaterials, 0, sizeof(D3DXMATERIAL) * cMaterials);
        memset(pmcMesh->m_rgpfxAttributes, 0, sizeof(LPD3DXEFFECT) * cMaterials);

        rgMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();

        if (pbufEffectInstances != NULL)
        {
            rgEffects = (LPD3DXEFFECTINSTANCE)pbufEffectInstances->GetBufferPointer();
        }

        for (iMaterial = 0; iMaterial < cMaterials; iMaterial++)
        {
            pmcMesh->rgMaterials[iMaterial].MatD3D = rgMaterials[iMaterial].MatD3D;

            if (rgMaterials[iMaterial].pTextureFilename != NULL)
            {
                cchFilename = strlen(rgMaterials[iMaterial].pTextureFilename) + 1;
                pmcMesh->rgMaterials[iMaterial].pTextureFilename = new char[cchFilename];
                if (pmcMesh->rgMaterials[iMaterial].pTextureFilename == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                memcpy(pmcMesh->rgMaterials[iMaterial].pTextureFilename,
                                rgMaterials[iMaterial].pTextureFilename, sizeof(char) * cchFilename);
            }

            if (rgEffects != NULL)
            {
                hr = D3DXCreateEffectInstance(&rgEffects[iMaterial], m_pDevice, &pmcMesh->m_rgpfxAttributes[iMaterial], NULL);
                if (FAILED(hr))
                    goto e_Exit;
            }
            else  // only used for text and other shape type things
            {
                // should not be used for anything using a texture filename
                GXASSERT(pmcMesh->rgMaterials[iMaterial].pTextureFilename == NULL);

                memset(&EffectInstance, 0, sizeof(D3DXEFFECTINSTANCE));

                // create effect instance will create a "default" effect when pEffectFilename is NULL
                hr = D3DXCreateEffectInstance(&EffectInstance, m_pDevice, &pmcMesh->m_rgpfxAttributes[iMaterial], NULL);
                if (FAILED(hr))
                    goto e_Exit;

                // set our "default" material colors
                pmcMesh->m_rgpfxAttributes[iMaterial]->SetVector("Ambient", (LPD3DXVECTOR4)&pmcMesh->rgMaterials[iMaterial].MatD3D.Ambient);
                pmcMesh->m_rgpfxAttributes[iMaterial]->SetVector("Diffuse", (LPD3DXVECTOR4)&pmcMesh->rgMaterials[iMaterial].MatD3D.Diffuse);
                pmcMesh->m_rgpfxAttributes[iMaterial]->SetVector("Specular", (LPD3DXVECTOR4)&pmcMesh->rgMaterials[iMaterial].MatD3D.Specular);            
            }

            hr = GenerateEffectInfo(pmcMesh->m_rgpfxAttributes[iMaterial], &pmcMesh->m_rgEffectInfo[iMaterial]);
            if (FAILED(hr))
                goto e_Exit;

            D3DXTECHNIQUE_DESC TechniqueDesc;
            D3DXPASS_DESC PassDesc;
            D3DXHANDLE hPass;
            D3DXHANDLE hTechnique;
            hTechnique = pmcMesh->m_rgpfxAttributes[iMaterial]->GetCurrentTechnique();
            pmcMesh->m_rgpfxAttributes[iMaterial]->GetTechniqueDesc(hTechnique, &TechniqueDesc);
            for( UINT iPass = 0; iPass < TechniqueDesc.Passes; iPass++ )
            {
                hPass = pmcMesh->m_rgpfxAttributes[iMaterial]->GetPass( hTechnique, iPass );
                pmcMesh->m_rgpfxAttributes[iMaterial]->GetPassDesc( hPass, &PassDesc );

                UINT NumVSSemanticsUsed;
                D3DXSEMANTIC pVSSemantics[MAXD3DDECLLENGTH];

                if( !PassDesc.pVertexShaderFunction ||
                    FAILED( D3DXGetShaderInputSemantics( PassDesc.pVertexShaderFunction, pVSSemantics, &NumVSSemanticsUsed ) ) )
                {
                    continue;
                }

                for( UINT iSem = 0; iSem < NumVSSemanticsUsed; iSem++ )
                {
                    if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_TANGENT )
                    {
                        bUsesTangents = TRUE;
                        break;
                    }
                }
            }
        }
    }


    if (ptmMesh != NULL)
    {
        D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
        CD3DXCrackDecl1 cd;

        // down cast to the base mesh type (could also QI for it)
        pmcMesh->pMesh = ptmMesh;
        pmcMesh->pMesh->AddRef();

        hr = ptmMesh->GetDeclaration(pDecl);
        if (FAILED(hr))
            goto e_Exit;
        
        cd.SetDeclaration(pDecl);

        if (bUsesTangents && (NULL == cd.GetSemanticElement(D3DDECLUSAGE_TANGENT, 0)))
        {
            LPD3DXMESH pMeshTemp = pmcMesh->pMesh;

            AddTangent(cd, pDecl);

            pmcMesh->pMesh = NULL;

            // clone in the space for the new normal
            hr = pMeshTemp->CloneMesh(pMeshTemp->GetOptions(), pDecl, m_pDevice, &pmcMesh->pMesh);

            GXRELEASE(pMeshTemp);

            if (FAILED(hr))
                goto e_Exit;

            hr = D3DXComputeTangent( pmcMesh->pMesh, 0, 0, D3DX_DEFAULT, TRUE, NULL );
            if (FAILED(hr))
                goto e_Exit;
        }

        pmcMesh->ptmDrawMesh = pmcMesh->pMesh;
        pmcMesh->ptmDrawMesh->AddRef();

        pmcMesh->bPMMeshMode = false;
        pmcMesh->bSimplifyMode = false;
    }
    else
    {
        GXASSERT(ptmPMesh != NULL);

        pmcMesh->pPMMesh = ptmPMesh;
        pmcMesh->pPMMesh->AddRef();
        pmcMesh->ptmDrawMesh = ptmPMesh;
        pmcMesh->ptmDrawMesh->AddRef();

        pmcMesh->m_cMinVerticesSoft = ptmPMesh->GetMinVertices();
        pmcMesh->m_cMaxVerticesSoft = ptmPMesh->GetMaxVertices();

        pmcMesh->bPMMeshMode = true;
        pmcMesh->bSimplifyMode = false;
    }

#if 0
    hr = ApplyEffectsToMesh(pmcMesh);
    if (FAILED(hr))
        goto e_Exit;
#endif

    cFaces = pmcMesh->ptmDrawMesh->GetNumFaces();

    // allocate a local buffer to put the adjacency data in
    pmcMesh->rgdwAdjacency = new DWORD[cFaces * 3];
    if (pmcMesh->rgdwAdjacency == NULL) 
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    if (pbufAdjacency != NULL)
    {
        GXASSERT(cFaces * 3 * sizeof(DWORD) == pbufAdjacency->GetBufferSize());

        // copy the adjacency data into the local buffer, could keep the buffer around, but not needed
        memcpy(pmcMesh->rgdwAdjacency, pbufAdjacency->GetBufferPointer(),
                                pbufAdjacency->GetBufferSize());
    }
    else
    {
        memset(pmcMesh->rgdwAdjacency, 0xff, sizeof(DWORD) * 3 * cFaces);
    }

#if 0
    // then link into the draw list
    pdeMesh->pdeNext = m_pdeHead;
    m_pdeHead = pdeMesh;
#else
    // replace the head, only one scene loaded at a time
    delete m_pdeHead;
    m_pdeHead = pdeMesh;
#endif

    cchName = strlen(szName) + 1;
    pdeMesh->szName = new char[cchName];
    if (pdeMesh->szName == NULL)
    {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
    }
    memcpy(pdeMesh->szName, szName, cchName);

    // select the new pde, pmc and pframe as current
    SelectDrawElement(pdeMesh);
    SelectFrame(pframeMesh, NULL);
    GXASSERT(m_pmcSelectedMesh == pmcMesh);

    Optimize(D3DXMESHOPT_ATTRSORT);

    hr = CalculateBoundingSphere(pdeMesh);
    if (FAILED(hr))
        goto e_Exit;

    // set the mesh radius/center into the effect
    for (iMaterial = 0; iMaterial < pmcMesh->NumMaterials; iMaterial++)
    {
        SetEffectMeshInfo(pmcMesh->m_rgpfxAttributes[iMaterial], &pdeMesh->vCenter, pdeMesh->fRadius);
    }

    D3DXMatrixTranslation(&pdeMesh->pframeRoot->matRot,
                                -pdeMesh->vCenter.x, -pdeMesh->vCenter.y, -pdeMesh->vCenter.z);
    pdeMesh->pframeRoot->matRotOrig = pdeMesh->pframeRoot->matRot;

    if (pmcMesh->pMesh != NULL)
    {
        // make sure there is an attribute table
        Optimize(D3DXMESHOPT_ATTRSORT);

        hr = pmcMesh->UpdateViews(pdeMesh);
        if (FAILED(hr))
            goto e_Exit;
    }

    SetProjectionMatrix();

    // setup attribute table
    pmcMesh->ptmDrawMesh->GetAttributeTable(NULL, &pmcMesh->m_caeAttributeTable);

    delete pmcMesh->m_rgaeAttributeTable;
    pmcMesh->m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[pmcMesh->m_caeAttributeTable];
    if (pmcMesh->m_rgaeAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    pmcMesh->ptmDrawMesh->GetAttributeTable(pmcMesh->m_rgaeAttributeTable, &pmcMesh->m_caeAttributeTable);

e_Exit:

    if (FAILED(hr))
        delete pdeMesh;

    return hr;
}

struct SLoadMeshData
{
    BOOL bFlattenHierarchy;
};

UINT_PTR CALLBACK DlgProcLoadMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    SLoadMeshData *plmdData;
    LPOPENFILENAME lpOFN;

    switch (message)
    {
        case WM_INITDIALOG:
            // Save off the long pointer to the OPENFILENAME structure.
            SetWindowLongPtr(hDlg, DWLP_USER, lParam);

            lpOFN = (LPOPENFILENAME)lParam;
            plmdData = (SLoadMeshData *)lpOFN->lCustData;
            SendDlgItemMessage(hDlg, IDC_FLATTENHIERARCHY, BM_SETCHECK, plmdData->bFlattenHierarchy ? BST_CHECKED : BST_UNCHECKED, 0);
            break;

        case WM_DESTROY:
            lpOFN = (LPOPENFILENAME)GetWindowLongPtr(hDlg, DWLP_USER);
            plmdData = (SLoadMeshData *)lpOFN->lCustData;

            plmdData->bFlattenHierarchy = (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_FLATTENHIERARCHY, BM_GETCHECK, 0, 0));
            break;

        default:
            return FALSE;
    }
    return TRUE;
}

HRESULT
TrivialData::LoadNewMesh()
{
    HRESULT hr = S_OK;
    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );
    SLoadMeshData lmdData;
    static TCHAR file[256];
    static TCHAR fileTitle[256];
    static TCHAR filter[] =
                           TEXT("Mesh files (*.x,*.m)\0*.x;*.m\0")
                           TEXT("X files (*.x)\0*.x\0")
                           TEXT("MESH files (*.m)\0*.m\0")
                           TEXT("All Files (*.*)\0*.*\0");
    _tcscpy( file, TEXT(""));
    _tcscpy( fileTitle, TEXT(""));

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hwnd;
    ofn.hInstance         = m_hInstance;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT("*.x");
    ofn.lCustData         = (LPARAM)&lmdData;

    ofn.lpfnHook          = DlgProcLoadMesh;
    ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_LOADMESHEX);
    lmdData.bFlattenHierarchy = FALSE;

    ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
    if ( ! GetOpenFileName( &ofn) )
    {
        char s[40];
        DWORD dwErr = CommDlgExtendedError();
        if ( 0 != dwErr )
        {
            memset( &ofn, 0, sizeof(ofn) );
            ofn.lStructSize       = sizeof(OPENFILENAME_NT4W);
            ofn.hwndOwner         = m_hwnd;
            ofn.hInstance         = m_hInstance;
            ofn.lpstrFilter       = filter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter    = 0L;
            ofn.nFilterIndex      = 1L;
            ofn.lpstrFile         = file;
            ofn.nMaxFile          = sizeof(file);
            ofn.lpstrFileTitle    = fileTitle;
            ofn.nMaxFileTitle     = sizeof(fileTitle);
            ofn.lpstrInitialDir   = NULL;
            ofn.nFileOffset       = 0;
            ofn.nFileExtension    = 0;
            ofn.lpstrDefExt       = TEXT("*.x");
            ofn.lCustData         = (LPARAM)&lmdData;

            ofn.lpfnHook          = DlgProcLoadMesh;
            ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_LOADMESHEX);
            lmdData.bFlattenHierarchy = FALSE;

            ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

        // if the first one failed, retry with old dialog style
            if ( ! GetOpenFileName( &ofn) )
            {
                dwErr = CommDlgExtendedError();
                if ( 0 != dwErr )
                {
                    sprintf( s, "GetOpenFileName failed with %x", dwErr );
                    MessageBox( m_hwnd, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
                }

                goto e_Exit;
            }
        }
        else
        {
            goto e_Exit;
        }
    }

    hr = LoadMesh( file, lmdData.bFlattenHierarchy );

e_Exit:
    return hr;
}


HRESULT
TrivialData::LoadMesh( TCHAR* file, BOOL bFlattenHierarchy )
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXMESH ptmMeshTemp = NULL;
    HRESULT hr = S_OK;
    bool b3dsFile;
    bool bXFile;
    IStream *pstream = NULL;
    DWORD dwOptions;
    int cchFileName;
    int cchFilePath;
    LPD3DXBUFFER pbufAdjacency = NULL;
    LPD3DXBUFFER pbufMaterials = NULL;
    LPD3DXBUFFER pbufEffectInstances = NULL;
    DWORD cMaterials = 0;
    char szPath[256];
    char *szTemp;
    DWORD cchPath;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    CD3DXCrackDecl1 cd;

    // create a mesh of the correct type
    dwOptions = D3DXMESH_MANAGED;

    cchFileName = strlen(file);
    if (cchFileName < 2)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    b3dsFile = true;
    bXFile = false;
    if ((toupper(file[cchFileName - 1]) == 'M') && (file[cchFileName - 2] == '.'))
    {
        b3dsFile = false;
        bXFile = false;
    }
    else if ((toupper(file[cchFileName - 1]) == 'X') && (file[cchFileName - 2] == '.'))
    {
        b3dsFile = false;
        bXFile = true;
    }

    static TCHAR szFilepath[256];
    memcpy(szFilepath, file, sizeof(file));
    cchFileName = strlen(szFilepath);
    int i;
    for (i=cchFileName; i > 0; i--)
    {
        if (szFilepath[i] == '\\')
        {
            szFilepath[i] = '\0';
            break;
        }
    }
    cchFilePath = strlen(szFilepath);

    // if loading the hierarchy and it is an xfile (can have one)
    //   call off to hierarchy loader
    if (!bFlattenHierarchy && bXFile)
    {
        hr = LoadMeshHierarchyFromFile(file);
        if (FAILED(hr))
            goto e_Exit;
    }
    else  // either Flatten specified, or the file is already flat
    {           // do a "normal" load without the hierarchy

        if (bXFile)
        {
            hr = D3DXLoadMeshFromX(file, dwOptions, m_pDevice, &pbufAdjacency, &pbufMaterials, &pbufEffectInstances, &cMaterials, &ptmMesh);
            if (FAILED(hr))
                goto e_Exit;
        }
        else if (!b3dsFile)
        {
            pstream = new CFileStream(file, true, false, &hr);
            if (FAILED(hr))
                goto e_Exit;

            hr = GXU::LoadMeshFromM(pstream, dwOptions, m_dwFVF, m_pDevice, &ptmMesh, &pbufAdjacency);
            if (FAILED(hr))
                goto e_Exit;
        }
        else 
        {
            hr = E_INVALIDARG;
            goto e_Exit;
        }

        cchPath = GetFullPathName(file, sizeof(szPath), szPath, &szTemp);
        if ((cchPath == 0) || (sizeof(szPath) <= cchPath))
        {
            hr = E_FAIL;
            goto e_Exit;
        }

        // remove the filename from the path part
        szPath[szTemp - szPath] = '\0';

        // set the current directory, so that textures will be searched from there
        SetCurrentDirectory(szPath);

        ptmMesh->GetDeclaration(pDecl);
        cd.SetDeclaration(pDecl);

        // if no normals in the mesh, then clone the mesh to add the normals and then compute them
        if (cd.GetSemanticElement(D3DDECLUSAGE_NORMAL, 0) == NULL)
        {
            ptmMeshTemp = ptmMesh;
            ptmMesh = NULL;

            AddNormal(cd, pDecl);

            hr = ptmMeshTemp->CloneMesh(ptmMeshTemp->GetOptions(), pDecl, m_pDevice, &ptmMesh);
            if (FAILED(hr))
                goto e_Exit;

            hr = D3DXComputeNormals(ptmMesh, NULL);
            if (FAILED(hr))
                goto e_Exit;
        }

        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "Polygon Mode"); 

        hr = AddMeshToDrawList(file, ptmMesh, NULL, pbufAdjacency, pbufMaterials, pbufEffectInstances, cMaterials);
        if (FAILED(hr))
            goto e_Exit;
    }

    UpdateAnimationsMenu();

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(ptmMeshTemp);
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pbufMaterials);
    GXRELEASE(pbufEffectInstances);

    if (pstream != NULL)
    {
        pstream->Release();
    }

    if (FAILED(hr))
    {
        MessageBox( m_hwnd, "Unabled to load the specified file.", "Load failed!", MB_OK);
    }

    return hr;
}

HRESULT
TrivialData::FindBones(SFrame *pframeCur, SDrawElement *pde)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;

    pmcMesh = pframeCur->pmcMesh;
    while (pmcMesh != NULL)
    {
        if (pmcMesh->pSkinInfo)
        {
            for (DWORD i = 0; i < pmcMesh->pSkinInfo->GetNumBones(); ++i)
            {
                SFrame* pFrame = pde->FindFrame((char*)pmcMesh->pSkinInfo->GetBoneName(i));
                GXASSERT(pFrame);
                pmcMesh->m_pBoneMatrix[i] = &(pFrame->matCombined);
            }
        }
        pmcMesh = (SMeshContainer*)pmcMesh->pNextMeshContainer;
    }

    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        hr = FindBones(pframeChild, pde);
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}


HRESULT GenerateMesh(SMeshContainer *pmcMesh)
{
    // ASSUMPTION:  pmcMesh->rgdwAdjacency contains the current adjacency

    HRESULT hr  = S_OK;
    LPDIRECT3DDEVICE9       pDevice = NULL;        

    DWORD   cFaces  = pmcMesh->m_pOrigMesh->GetNumFaces();

    GXRELEASE(pmcMesh->pMesh);
    GXRELEASE(pmcMesh->m_pSkinnedMesh);
    GXRELEASE(pmcMesh->m_pBoneCombinationBuf);

    hr  = pmcMesh->m_pOrigMesh->GetDevice(&pDevice);
    if (FAILED(hr))
        goto e_ExitNONINDEXED;

    pmcMesh->pMesh      = NULL;
    
    if (pmcMesh->m_Method == D3DNONINDEXED)
    {
        LPD3DXBONECOMBINATION   rgBoneCombinations;
        D3DCAPS9                caps;

        hr  = pDevice->GetDeviceCaps(&caps);
        if (FAILED(hr))
            goto e_ExitNONINDEXED;

        // UNDONE UNDONE fix to only be Managed, once only one mesh is required
        //          NOTE: might break crease mode!
        hr = pmcMesh->pSkinInfo->ConvertToBlendedMesh
                                   (
                                       pmcMesh->m_pOrigMesh,
                                       D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
                                       pmcMesh->rgdwAdjacency, 
                                       pmcMesh->rgdwAdjacency,  
                                       NULL, NULL, 
                                       &pmcMesh->m_maxFaceInfl,
                                       &pmcMesh->m_cAttributeGroups, 
                                       &pmcMesh->m_pBoneCombinationBuf, 
                                       &pmcMesh->pMesh
                                   );
        if (FAILED(hr))
            goto e_ExitNONINDEXED;

        /* If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
           Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
           drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing. */
        if (caps.MaxVertexBlendMatrices == 2)       
        {    
            // calculate the index of the attribute table to split on
            rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
            for (pmcMesh->iAttrSplit = 0; pmcMesh->iAttrSplit < pmcMesh->m_cAttributeGroups; pmcMesh->iAttrSplit++)
            {
                DWORD   cInfl   = 0;

                for (DWORD iInfl = 0; iInfl < pmcMesh->m_maxFaceInfl; iInfl++)
                {
                    if (rgBoneCombinations[pmcMesh->iAttrSplit].BoneId[iInfl] != UINT_MAX)
                    {
                        ++cInfl;
                    }
                }

                if (cInfl > 2)
                {
                    break;
                }
            }

            // if there is both HW and SW, add the Software Processing flag
            if (pmcMesh->iAttrSplit < pmcMesh->m_cAttributeGroups)
            {
                LPD3DXMESH pMeshTmp;

                hr = pmcMesh->pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|pmcMesh->pMesh->GetOptions(), 
                                                    pmcMesh->pMesh->GetFVF(),
                                                    pDevice, &pMeshTmp);
                if (FAILED(hr))
                {
                    goto e_Exit;
                }

                pmcMesh->pMesh->Release();
                pmcMesh->pMesh = pMeshTmp;
                pMeshTmp = NULL;
            }
        }


e_ExitNONINDEXED:

        if (FAILED(hr))
            goto e_Exit;

    }
    else if (pmcMesh->m_Method == D3DINDEXED)
    {
        hr = pmcMesh->pSkinInfo->ConvertToIndexedBlendedMesh
                                (
                                pmcMesh->m_pOrigMesh, 
                                pmcMesh->m_iPaletteSize == x_iDefaultSkinningPaletteSize ? D3DXMESH_SYSTEMMEM : D3DXMESH_MANAGED, 
                                g_pData->m_iPaletteSize, 
                                pmcMesh->rgdwAdjacency, 
                                pmcMesh->rgdwAdjacency,
                                NULL, NULL, 
                                &pmcMesh->m_maxFaceInfl,
                                &pmcMesh->m_cAttributeGroups, 
                                &pmcMesh->m_pBoneCombinationBuf, 
                                &pmcMesh->pMesh);
        if (FAILED(hr))
            goto e_Exit;

    }
    else if (pmcMesh->m_Method == SOFTWARE)
    {
        GXRELEASE(pmcMesh->m_pBoneCombinationBuf);

        hr = pmcMesh->m_pOrigMesh->CloneMeshFVF(/*D3DXMESH_WRITEONLY | */D3DXMESH_DYNAMIC, pmcMesh->m_pOrigMesh->GetFVF(),
                                              pDevice, &pmcMesh->pMesh);
        if (FAILED(hr))
            goto e_Exit;

        if (FAILED(hr = pmcMesh->pMesh->OptimizeInplace(D3DXMESHOPT_IGNOREVERTS | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_DONOTSPLIT, pmcMesh->rgdwAdjacency, pmcMesh->rgdwAdjacency, NULL, NULL)))
            goto e_Exit;

        hr = pmcMesh->pMesh->GetAttributeTable(NULL, &pmcMesh->m_cAttributeGroups);
        if (FAILED(hr))
            goto e_Exit;
    }

    GXRELEASE(pmcMesh->m_pSkinnedMesh);

    hr = pmcMesh->m_pOrigMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, pmcMesh->m_pOrigMesh->GetFVF(),
                                          pDevice, &pmcMesh->m_pSkinnedMesh);
    if (FAILED(hr))
        goto e_Exit;

    GXRELEASE(pmcMesh->ptmDrawMesh);
    pmcMesh->ptmDrawMesh = pmcMesh->pMesh;
    pmcMesh->ptmDrawMesh->AddRef();

    pmcMesh->UpdateSkinInfo();

    pmcMesh->ptmDrawMesh->GetAttributeTable(0, &pmcMesh->m_cAttributeGroups);

    delete []pmcMesh->m_rgaeAttributeTable;
    pmcMesh->m_rgaeAttributeTable = new D3DXATTRIBUTERANGE[pmcMesh->m_cAttributeGroups];
    if (pmcMesh->m_rgaeAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    pmcMesh->ptmDrawMesh->GetAttributeTable(pmcMesh->m_rgaeAttributeTable, 0);

e_Exit:
    GXRELEASE(pDevice);

    return hr;
}






HRESULT
TrivialData::LoadNewProgressiveMesh( )
{
    HRESULT hr = S_OK;
    OPENFILENAME ofn;
    CFileStream *pstream = NULL;
    LPD3DXPMESH ptmPMesh = NULL;
    LPD3DXBUFFER pbufMaterials = NULL;
    LPD3DXBUFFER pbufEffectInstances = NULL;
    DWORD cMaterials;

    memset( &ofn, 0, sizeof(ofn) );
    static TCHAR file[256];
    static TCHAR fileTitle[256];
    static TCHAR filter[] = TEXT("PMESH files (*.x)\0*.x\0")
                           TEXT("All Files (*.*)\0*.*\0");
    _tcscpy( file, TEXT(""));
    _tcscpy( fileTitle, TEXT(""));

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hwnd;
    ofn.hInstance         = m_hInstance;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT("*.m");
    ofn.lCustData         = 0;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if ( ! GetOpenFileName( &ofn) )
    {
        char s[40];
        DWORD dwErr = CommDlgExtendedError();

        // if the first one failed, retry with old dialog style
        if ( 0 != dwErr )
        {
            memset( &ofn, 0, sizeof(ofn) );
            ofn.lStructSize       = sizeof(OPENFILENAME_NT4W);
            ofn.hwndOwner         = m_hwnd;
            ofn.hInstance         = m_hInstance;
            ofn.lpstrFilter       = filter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter    = 0L;
            ofn.nFilterIndex      = 1L;
            ofn.lpstrFile         = file;
            ofn.nMaxFile          = sizeof(file);
            ofn.lpstrFileTitle    = fileTitle;
            ofn.nMaxFileTitle     = sizeof(fileTitle);
            ofn.lpstrInitialDir   = NULL;
            ofn.nFileOffset       = 0;
            ofn.nFileExtension    = 0;
            ofn.lpstrDefExt       = TEXT("*.m");
            ofn.lCustData         = 0;

            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

            if ( ! GetOpenFileName( &ofn) )
            {
                dwErr = CommDlgExtendedError();
                if ( 0 != dwErr )
                {
                    sprintf( s, "GetOpenFileName failed with %x", dwErr );
                    MessageBox( m_hwnd, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
                }

                goto e_Exit;

            }
        }
        else
        {
            goto e_Exit;
        }
    }

    pstream = new CFileStream(ofn.lpstrFile, true, false, &hr);
    if ((pstream == NULL) || FAILED(hr))
        goto e_Exit;

    hr = D3DXCreatePMeshFromStream(pstream, D3DXMESH_MANAGED, m_pDevice, &pbufMaterials, &pbufEffectInstances, &cMaterials, &ptmPMesh);
    if (FAILED(hr))
        goto e_Exit;

    // go to max lod so that bounding sphere stuff works, etc.
    hr = ptmPMesh->SetNumVertices(0xffffffff);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList(ofn.lpstrFile, NULL, ptmPMesh, NULL, pbufMaterials, pbufEffectInstances, cMaterials);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(pstream);
    GXRELEASE(ptmPMesh);
    GXRELEASE(pbufMaterials);
    GXRELEASE(pbufEffectInstances);

    return hr;
}

HRESULT SaveMeshToM
    (
    char *szFilename, 
    LPD3DXMESH pMesh,
    D3DXMATERIAL *rgMaterials
    )
{
    DWORD iFace;
    DWORD iVertex;
    D3DXVECTOR3 *pvPos;
    D3DXVECTOR3 *pvNormal;
    WORD *pwFace;
    FILE *file;
    WORD *pwFaces = NULL;
    PBYTE pvPoints = NULL;
    DWORD cVertices;
    DWORD cFaces;
    DWORD *rgdwAttributes;
    CD3DXCrackDecl1 cd;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    pMesh->GetDeclaration(pDecl);
    cd.SetDeclaration(pDecl);    

    file = fopen(szFilename, "w+");
    if (file == NULL)
        return E_FAIL;

    cVertices = pMesh->GetNumVertices();
    cFaces = pMesh->GetNumFaces();

    pMesh->LockVertexBuffer(0, (LPVOID*)&pvPoints);
    pMesh->LockIndexBuffer(0, (LPVOID*)&pwFaces);
    pMesh->LockAttributeBuffer(0, &rgdwAttributes);

    for (iVertex = 0; iVertex < cVertices; iVertex++)
    {
        pvPos = cd.PvGetPosition(cd.GetArrayElem(pvPoints, iVertex));
        pvNormal = cd.PvGetNormal(cd.GetArrayElem(pvPoints, iVertex));

        fprintf(file, "Vertex %d %f %f %f {normal=(%f %f %f)}\n",
                    iVertex+1, pvPos->x, pvPos->y, pvPos->z, 
                             pvNormal->x, pvNormal->y, pvNormal->z);
    }

    pwFace = pwFaces;
    for (iFace = 0; iFace < cFaces; iFace++)
    {
        fprintf(file, "Face %d %d %d %d {rgb=(%f %f %f)}\n",
                    iFace+1, pwFace[0]+1, pwFace[1]+1, pwFace[2]+1,
                    rgMaterials[rgdwAttributes[iFace]].MatD3D.Diffuse.r,
                    rgMaterials[rgdwAttributes[iFace]].MatD3D.Diffuse.g,
                    rgMaterials[rgdwAttributes[iFace]].MatD3D.Diffuse.b);

        pwFace += 3;
    }

    fclose(file);

    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();
    pMesh->UnlockAttributeBuffer();

    return S_OK;
}

struct SSaveMeshData
{
    BOOL bSaveSelectedOnly;
    BOOL bSaveHierarchy;
    BOOL bSaveAnimation;
    DWORD xFormat;
};

UINT_PTR CALLBACK DlgProcSaveMesh(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    SSaveMeshData *psmdData;
    LPOPENFILENAME lpOFN;

    switch (message)
    {
        case WM_INITDIALOG:
            // Save off the long pointer to the OPENFILENAME structure.
            SetWindowLongPtr(hDlg, DWLP_USER, lParam);

            lpOFN = (LPOPENFILENAME)lParam;
            psmdData = (SSaveMeshData *)lpOFN->lCustData;
            SendDlgItemMessage(hDlg, IDC_SAVESELECTEDONLY, BM_SETCHECK, psmdData->bSaveSelectedOnly ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(hDlg, IDC_SAVEHIERARCHY, BM_SETCHECK, psmdData->bSaveHierarchy ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(hDlg, IDC_SAVEANIMATION, BM_SETCHECK, psmdData->bSaveAnimation ? BST_CHECKED : BST_UNCHECKED, 0);

            switch (psmdData->xFormat)
            {
                case D3DXF_FILEFORMAT_BINARY:
                    CheckRadioButton(hDlg,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_BINARY);
                    break;

                case D3DXF_FILEFORMAT_TEXT:
                    CheckRadioButton(hDlg,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_TEXT);
                    break;

                case D3DXF_FILEFORMAT_BINARY | D3DXF_FILEFORMAT_COMPRESSED:
                    CheckRadioButton(hDlg,IDC_TEXT,IDC_BINARYCOMPRESSED,IDC_BINARYCOMPRESSED);
                    break;
            }

            break;

        case WM_DESTROY:
            lpOFN = (LPOPENFILENAME)GetWindowLongPtr(hDlg, DWLP_USER);
            psmdData = (SSaveMeshData *)lpOFN->lCustData;

            psmdData->bSaveSelectedOnly = (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_SAVESELECTEDONLY));
            psmdData->bSaveHierarchy = (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_SAVEHIERARCHY));
            psmdData->bSaveAnimation = (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_SAVEANIMATION));

            if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_TEXT))
                psmdData->xFormat = D3DXF_FILEFORMAT_TEXT;
            else if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_BINARY))
                psmdData->xFormat = D3DXF_FILEFORMAT_BINARY;
            else if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_BINARYCOMPRESSED))
                psmdData->xFormat = D3DXF_FILEFORMAT_BINARY | D3DXF_FILEFORMAT_COMPRESSED;

            break;

        default:
            return FALSE;
    }
    return TRUE;
}


// this function tries to generate point reps using the adjacency in the mesh container
//          if it fails, the user is asked if they would like to skip saving it to disk
DWORD *CheckAdjacencyForSaving
    (
    SMeshContainer *pmcMesh
    )
{
    HRESULT hr;
    DWORD cVertices;
    DWORD *rgdwPointReps;

    if ((pmcMesh->pMesh != NULL) && (pmcMesh->rgdwAdjacency != NULL))
    {
        cVertices = pmcMesh->pMesh->GetNumVertices();

        rgdwPointReps = new DWORD[cVertices];

        // if allocation failed, just assume the adjacency is correct, will be checked again by D3DX function
        if (rgdwPointReps == NULL)
            return pmcMesh->rgdwAdjacency;

        hr = pmcMesh->pMesh->ConvertAdjacencyToPointReps(pmcMesh->rgdwAdjacency, rgdwPointReps);
        delete []rgdwPointReps;

        // if the function failed, there is something wrong with the adjacency, ask user if they want to skip saving it
        if (FAILED(hr))
        {
            // return NULL if user wants to skip adjacency, will not be saved to file as point reps then
            if (IDYES == MessageBox(NULL, "The adjacency for the current mesh is invalid for saving, do you want to skip saving it?  NOTE: you can try to fix the adjaceny by doing Validate Mesh", "Adjaceny invalid!", MB_YESNO) )
            {
                return NULL;
            }
        }
    }

    return pmcMesh->rgdwAdjacency;

}

// this function allows for the user not specifying the saving of "bad" adjacency
//   and MORE IMPORTANTLY sets up the MeshData structure for saving purpose
void 
SetupForSaveMeshHierarchy(SFrame *pFrame)
{
    SMeshContainer *pmcCur;
    DWORD *rgdwAdjacencySave;

    pmcCur = pFrame->pmcMesh;
    while (pmcCur != NULL)
    {
        // check to see if we should save the adjacency
        rgdwAdjacencySave = CheckAdjacencyForSaving(pmcCur);

        // if CheckAdjacency returned NULL, then swap out the adjacency and DON'T save it
        if (rgdwAdjacencySave == NULL)
        {
            pmcCur->m_rgdwAdjacencyBackup = pmcCur->rgdwAdjacency;
            pmcCur->rgdwAdjacency = NULL;
        }

        // if doing skinning (the orig mesh is not NULL), then we need to SAVE that mesh with the skin info
        //   instead of saving the HW friendly mesh with the skin info, so swap the pMesh out for the save
        if (pmcCur->m_pOrigMesh != NULL)
        {
            GXASSERT(pmcCur->m_pMeshBackup == NULL);

            pmcCur->m_pMeshBackup = pmcCur->pMesh;
            pmcCur->pMesh = pmcCur->m_pOrigMesh;
            pmcCur->pMesh->AddRef();
        }

        // NOTE!!! This needs to be done after handling m_pOrigMesh
        if (pmcCur->bPMMeshMode)
        {
            pmcCur->MeshData.Type = D3DXMESHTYPE_PMESH;
            pmcCur->MeshData.pPMesh = pmcCur->pPMMesh;
        }
        else if (pmcCur->bTesselateMode)
        {
            pmcCur->MeshData.Type = D3DXMESHTYPE_PATCHMESH;
            pmcCur->MeshData.pPatchMesh = pmcCur->pPatchMesh;
        }
        else // standard mesh case
        {
            pmcCur->MeshData.Type = D3DXMESHTYPE_MESH;
            pmcCur->MeshData.pMesh = pmcCur->pMesh;
        }

        pmcCur = pmcCur->pNextMeshContainer;
    }

    if (pFrame->pframeSibling != NULL)
    {
        SetupForSaveMeshHierarchy(pFrame->pframeSibling);
    }

    if (pFrame->pframeFirstChild != NULL)
    {
        SetupForSaveMeshHierarchy(pFrame->pframeFirstChild);
    }
}

void 
RestoreAfterSaveMeshHierarchy(SFrame *pFrame)
{
    SMeshContainer *pmcCur;

    pmcCur = pFrame->pmcMesh;
    while (pmcCur != NULL)
    {
        // if we didn't save the adjacency (swapped it out), swap it back in
        if (pmcCur->m_rgdwAdjacencyBackup != NULL)
        {
            GXASSERT(pmcCur->rgdwAdjacency == NULL);
            pmcCur->rgdwAdjacency = pmcCur->m_rgdwAdjacencyBackup;
            pmcCur->m_rgdwAdjacencyBackup = NULL;
        }

        // if skinning required the orig mesh to be in pMesh, swap the backed up pMesh back in
        if (pmcCur->m_pMeshBackup != NULL)
        {
            GXRELEASE(pmcCur->pMesh);
            pmcCur->pMesh = pmcCur->m_pMeshBackup;
            pmcCur->m_pMeshBackup = NULL;
        }

        pmcCur = pmcCur->pNextMeshContainer;
    }

    if (pFrame->pframeSibling != NULL)
    {
        RestoreAfterSaveMeshHierarchy(pFrame->pframeSibling);
    }

    if (pFrame->pframeFirstChild != NULL)
    {
        RestoreAfterSaveMeshHierarchy(pFrame->pframeFirstChild);
    }
}

HRESULT
TrivialData::SaveMesh( )
{
    HRESULT hr = S_OK;
    LPD3DXMESH pMeshMerged = NULL;
    LPD3DXBUFFER pbufAdjacencyMerged = NULL;
    LPD3DXBUFFER pbufMaterialsMerged = NULL;
    DWORD cMaterialsMerged;
    DWORD *rgdwAdjacency;

    SSaveMeshData smdData;
    OPENFILENAME ofn;
    BOOL bXFile;
    DWORD cchFileName;
    static TCHAR file[256];
    static TCHAR szFilepath[256];
    static TCHAR fileTitle[256];
    static TCHAR filter[] =
                           TEXT("X files (*.x)\0*.x\0")
                           TEXT("M files (*.m)\0*.m\0")
                           TEXT("All Files (*.*)\0*.*\0");
    _tcscpy( file, TEXT(""));
    _tcscpy( fileTitle, TEXT(""));

    memset( &ofn, 0, sizeof(ofn) );
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hwnd;
    ofn.hInstance         = m_hInstance;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrDefExt       = NULL;
    ofn.lCustData         = (LPARAM)&smdData;
    ofn.lpfnHook          = DlgProcSaveMesh;
    ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_SAVEMESHEX);
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

    smdData.bSaveSelectedOnly = FALSE;
    smdData.bSaveHierarchy = TRUE;
    smdData.bSaveAnimation = TRUE;
    smdData.xFormat = D3DXF_FILEFORMAT_TEXT;


    if ( ! GetSaveFileName( &ofn) )
    {
        char s[40];
        DWORD dwErr = CommDlgExtendedError();
        if ( 0 != dwErr )
        {
            memset( &ofn, 0, sizeof(OPENFILENAME) );
            ofn.lStructSize       = sizeof(OPENFILENAME_NT4W);
            ofn.hwndOwner         = m_hwnd;
            ofn.hInstance         = m_hInstance;
            ofn.lpstrFilter       = filter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter    = 0L;
            ofn.nFilterIndex      = 1L;
            ofn.lpstrFile         = file;
            ofn.nMaxFile          = sizeof(file);
            ofn.lpstrFileTitle    = fileTitle;
            ofn.nMaxFileTitle     = sizeof(fileTitle);
            ofn.lpstrInitialDir   = NULL;
            ofn.lpstrDefExt       = NULL;
            ofn.lCustData         = (LPARAM)&smdData;
            ofn.lpfnHook          = DlgProcSaveMesh;
            ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_SAVEMESHEX);
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

            if ( ! GetSaveFileName( &ofn) )
            {
                dwErr = CommDlgExtendedError();
                if ( 0 != dwErr )
                {
                    sprintf( s, "GetOpenFileName failed with %x", dwErr );
                    MessageBox( m_hwnd, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
                }

                goto e_Exit;
            }
        }
        else  // just a cancel, return
        {
            goto e_Exit;
        }
    }
    
    if (smdData.bSaveSelectedOnly && ((m_pmcSelectedMesh == NULL) || (m_pmcSelectedMesh->pMesh == NULL)))
    {
        MessageBox(m_hwnd, "No Mesh selected to save!", "No Selection", MB_OK);
        goto e_Exit;
    }

    bXFile = TRUE;
    cchFileName = strlen(ofn.lpstrFile);
    if ((toupper(ofn.lpstrFile[cchFileName - 1]) == 'M') && (ofn.lpstrFile[cchFileName - 2] == '.'))
    {
        bXFile = FALSE;
    }

    if (smdData.bSaveHierarchy && bXFile)
    {
        // UNDONE UNDONE - doesn't support subset of info yet!
        GXASSERT(smdData.bSaveAnimation);
        GXASSERT(!smdData.bSaveSelectedOnly);

        // UNDONE UNDONE Need to check adjacency before saving!!!!

        SetupForSaveMeshHierarchy(m_pdeHead->pframeRoot);

        hr = D3DXSaveMeshHierarchyToFile(ofn.lpstrFile, smdData.xFormat, (LPD3DXFRAME)m_pdeHead->pframeRoot->pframeFirstChild, m_pdeHead->m_pAnimMixer, NULL);

        RestoreAfterSaveMeshHierarchy(m_pdeHead->pframeRoot);

        if (FAILED(hr))
            goto e_Exit;
    }
    else
    {

        if (smdData.bSaveSelectedOnly)
        {
            if (m_pmcSelectedMesh->pSkinInfo)
            {
                MessageBox(m_hwnd, "Skinned Meshes cannot be flattened!", "Skinned Mesh Found", MB_OK);
                hr = E_FAIL;
                goto e_Exit;
            }
            else if (m_pmcSelectedMesh->pPMMesh)
            {
                MessageBox(m_hwnd, "Progressive Meshes cannot be flattened!  Take a snapshot before saving.", "Progressive Mesh Found", MB_OK);
                hr = E_FAIL;
                goto e_Exit;
            }

            if (bXFile)
            {
                rgdwAdjacency = CheckAdjacencyForSaving(m_pmcSelectedMesh);

                hr = D3DXSaveMeshToX(ofn.lpstrFile, m_pmcSelectedMesh->pMesh, rgdwAdjacency,
                                        m_pmcSelectedMesh->rgMaterials, m_pmcSelectedMesh->pEffects, m_pmcSelectedMesh->NumMaterials, smdData.xFormat);
                if (FAILED(hr))
                    goto e_Exit;
            }
            else  // save out the M file
            {
                hr = SaveMeshToM(ofn.lpstrFile, m_pmcSelectedMesh->pMesh, m_pmcSelectedMesh->rgMaterials);
                if (FAILED(hr))
                    goto e_Exit;
            }
        }
        else
        {
            if (m_pdeHead->pframeRoot->PMeshPresent())
            {
                MessageBox(m_hwnd, "Progressive Meshes cannot be flattened!  Take a snapshot before saving.", "Progressive Mesh Found", MB_OK);
                hr = E_FAIL;
                goto e_Exit;
            }
            else if (m_pdeHead->bSkinnedMeshInHeirarchy)
            {
                MessageBox(m_hwnd, "Skinned Meshes cannot be flattened!", "Skinned Mesh Found", MB_OK);
                hr = E_FAIL;
                goto e_Exit;
            }

            D3DXMATRIX matRot;
            D3DXMATRIX matTrans;
            D3DXMATRIX matIdent;

            // recalculate matCombined, without rot/trans in root
            matRot = m_pdeHead->pframeRoot->matRot;
            matTrans = m_pdeHead->pframeRoot->matTrans;
            D3DXMatrixIdentity(&m_pdeHead->pframeRoot->matRot);
            D3DXMatrixIdentity(&m_pdeHead->pframeRoot->matTrans);
            D3DXMatrixIdentity(&matIdent);

            hr = UpdateFrames(m_pdeHead->pframeRoot, matIdent);
            if (FAILED(hr))
                return hr;

            hr = ::MergeMeshes(m_pdeHead->pframeRoot, m_pDevice,    
                                    &pMeshMerged, &pbufAdjacencyMerged,
                                    &pbufMaterialsMerged, &cMaterialsMerged);
            if (FAILED(hr))
                goto e_Exit;

            // put the original rot/trans back 
            m_pdeHead->pframeRoot->matRot = matRot;
            m_pdeHead->pframeRoot->matTrans = matTrans;
            D3DXMatrixIdentity(&matIdent);

            hr = UpdateFrames(m_pdeHead->pframeRoot, matIdent);
            if (FAILED(hr))
                return hr;

            if (bXFile)
            {
                hr = D3DXSaveMeshToX(ofn.lpstrFile, pMeshMerged, (DWORD*)(pbufAdjacencyMerged->GetBufferPointer()),
                           (D3DXMATERIAL*)(pbufMaterialsMerged->GetBufferPointer()), NULL, cMaterialsMerged, smdData.xFormat);
                if (FAILED(hr))
                    goto e_Exit;
            }
            else  // save out the M file
            {
                hr = SaveMeshToM(ofn.lpstrFile, pMeshMerged, (D3DXMATERIAL*)(pbufMaterialsMerged->GetBufferPointer()));
                if (FAILED(hr))
                    goto e_Exit;
            }
        }
    }

e_Exit:
    GXRELEASE(pMeshMerged);
    GXRELEASE(pbufAdjacencyMerged);
    GXRELEASE(pbufMaterialsMerged);

    if (FAILED(hr))
    {
        MessageBox( m_hwnd, "Unabled to save the specified file!", "Save failed!", MB_OK);
    }

    return hr;
}

#if 1
HRESULT
TrivialData::SavePMesh( )
{
    HRESULT hr = S_OK;
    IStream *pstream = NULL;

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );
    static TCHAR file[256];
    static TCHAR szFilepath[256];
    static TCHAR fileTitle[256];
    static TCHAR filter[] =
                           TEXT("X files (*.x)\0*.x\0")
                           TEXT("All Files (*.*)\0*.*\0");
    _tcscpy( file, TEXT(""));
    _tcscpy( fileTitle, TEXT(""));

    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = m_hwnd;
    ofn.hInstance         = m_hInstance;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT("*.m");
    ofn.lCustData         = 0;

    if ((m_pmcSelectedMesh == NULL) || !m_pmcSelectedMesh->bPMMeshMode )
        return S_OK;

    ofn.Flags = OFN_OVERWRITEPROMPT;
    if ( ! GetSaveFileName( &ofn) )
    {
        char s[40];
        DWORD dwErr = CommDlgExtendedError();
        if ( 0 != dwErr )
        {
            sprintf( s, "GetOpenFileName failed with %x", dwErr );
            MessageBox( m_hwnd, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
        }
        goto e_Exit;
    }

    pstream = new CFileStream(ofn.lpstrFile, false, true, &hr);
    if (pstream == NULL || FAILED(hr))
        goto e_Exit;

    hr = m_pmcSelectedMesh->pPMMesh->Save(pstream, m_pmcSelectedMesh->rgMaterials, m_pmcSelectedMesh->pEffects, m_pmcSelectedMesh->NumMaterials);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    if (FAILED(hr))
    {
        MessageBox( m_hwnd, "Unabled to save the specified file!", "Save failed!", MB_OK);
    }

    GXRELEASE(pstream);
    return hr;
}
#endif

void
TrivialData::AddAnimation()
{
    OPENFILENAME ofn;
    HRESULT hr = S_OK;
    memset( &ofn, 0, sizeof(ofn) );
    static TCHAR file[256];
    static TCHAR szFilepath[256];
    static TCHAR fileTitle[256];
    static TCHAR filter[] =
                           TEXT("Mesh files (*.x,*.m)\0*.x;*.m\0")
                           TEXT("X files (*.x)\0*.x\0")
                           TEXT("MESH files (*.m)\0*.m\0")
                           TEXT("All Files (*.*)\0*.*\0");
    _tcscpy( file, TEXT(""));
    _tcscpy( fileTitle, TEXT(""));
    int cchFileName;
    int cchFilePath;

    SFrame *pFrameToDelete = NULL;
    LPD3DXANIMATIONCONTROLLER pAnimMixerToDelete = NULL;
    LPD3DXANIMATIONCONTROLLER pAnimMixer = NULL;
    LPD3DXANIMATIONSET pAnimSet;
    CAllocateHierarchy Alloc;
    UINT iAnimSet;
    UINT cAnimSets;

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hwnd;
    ofn.hInstance         = m_hInstance;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1L;
    ofn.lpstrFile         = file;
    ofn.nMaxFile          = sizeof(file);
    ofn.lpstrFileTitle    = fileTitle;
    ofn.nMaxFileTitle     = sizeof(fileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT("*.x");
    ofn.lCustData         = NULL;

    ofn.lpfnHook          = DlgProcLoadMesh;
    ofn.lpTemplateName    = NULL;

    ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if ( ! GetOpenFileName( &ofn) )
    {
        char s[40];
        DWORD dwErr = CommDlgExtendedError();
        if ( 0 != dwErr )
        {
            memset( &ofn, 0, sizeof(ofn) );
            ofn.lStructSize       = sizeof(OPENFILENAME_NT4W);
            ofn.hwndOwner         = m_hwnd;
            ofn.hInstance         = m_hInstance;
            ofn.lpstrFilter       = filter;
            ofn.lpstrCustomFilter = NULL;
            ofn.nMaxCustFilter    = 0L;
            ofn.nFilterIndex      = 1L;
            ofn.lpstrFile         = file;
            ofn.nMaxFile          = sizeof(file);
            ofn.lpstrFileTitle    = fileTitle;
            ofn.nMaxFileTitle     = sizeof(fileTitle);
            ofn.lpstrInitialDir   = NULL;
            ofn.nFileOffset       = 0;
            ofn.nFileExtension    = 0;
            ofn.lpstrDefExt       = TEXT("*.x");
            ofn.lCustData         = NULL;

            ofn.lpfnHook          = DlgProcLoadMesh;
            ofn.lpTemplateName    = NULL;

            ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

        // if the first one failed, retry with old dialog style
            if ( ! GetOpenFileName( &ofn) )
            {
                dwErr = CommDlgExtendedError();
                if ( 0 != dwErr )
                {
                    sprintf( s, "GetOpenFileName failed with %x", dwErr );
                    MessageBox( m_hwnd, s, "TexWin", MB_OK | MB_SYSTEMMODAL );
                }

                goto e_Exit;
            }
        }
        else
        {
            goto e_Exit;
        }
    }

    // load the file to get the animation data
    hr = D3DXLoadMeshHierarchyFromX(file, D3DXMESH_SYSTEMMEM, m_pDevice, &Alloc, NULL, (LPD3DXFRAME*)&pFrameToDelete, &pAnimMixerToDelete);
    if (FAILED(hr))
        goto e_Exit;

    if (pAnimMixerToDelete == NULL)
    {
        MessageBox(NULL, "No animation data in file!", "Warning", MB_OK);
        goto e_Exit;
    }

    cAnimSets = pAnimMixerToDelete->GetNumAnimationSets();

    hr = m_pdeHead->m_pAnimMixer->CloneAnimationController(
                                m_pdeHead->m_pAnimMixer->GetMaxNumAnimationOutputs(),
                                m_pdeHead->m_pAnimMixer->GetMaxNumAnimationSets() + cAnimSets,
                                m_pdeHead->m_pAnimMixer->GetMaxNumTracks(),
                                m_pdeHead->m_pAnimMixer->GetMaxNumEvents(),
                                &pAnimMixer);
    if (FAILED(hr))
        goto e_Exit;

    GXRELEASE(m_pdeHead->m_pAnimMixer);
    m_pdeHead->m_pAnimMixer = pAnimMixer;
    pAnimMixer = NULL;
    

    for (iAnimSet = 0; iAnimSet < cAnimSets; iAnimSet++)
    {
        pAnimMixerToDelete->GetAnimationSet(iAnimSet, &pAnimSet);
        m_pdeHead->m_pAnimMixer->RegisterAnimationSet(pAnimSet);

        GXRELEASE(pAnimSet);
    }
    
    m_pdeHead->m_pAnimMixer->AdvanceTime(m_pdeHead->fCurTime - m_pdeHead->m_pAnimMixer->GetTime(), NULL);

    UpdateAnimationsMenu();
e_Exit:
    //D3DXFrameDestroy(pFrameToDelete, &Alloc);
    GXRELEASE(pAnimMixerToDelete);

    delete pFrameToDelete;

    return;
}
