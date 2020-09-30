/*//////////////////////////////////////////////////////////////////////////////
//
// File: mview.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"
#include <direct.h>
#include "dxerr8.h"

TrivialData *g_pData = NULL;

//BOOL WINAPI D3DXDumpUnfreedMemoryInfo();

//-----------------------------------------------------------------------------
// Name: D3DUtil_InitMaterial()
// Desc: Helper function called to build a D3DMATERIAL structure
//-----------------------------------------------------------------------------
VOID D3DUtil_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r, FLOAT g, FLOAT b )
{
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = r;
    mtrl.Diffuse.g = mtrl.Ambient.g = g;
    mtrl.Diffuse.b = mtrl.Ambient.b = b;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    mtrl.Emissive.r = 0;
    mtrl.Emissive.g = 0;
    mtrl.Emissive.b = 0;
}

TrivialData::TrivialData( )
{
    m_bTextureMode = TRUE;
    m_bEdgeMode = FALSE;
    m_bNPatchEdgeMode = FALSE;
    m_bStripMode = FALSE;
    m_bAdjacencyMode = FALSE;
    m_bCreaseMode = FALSE;
    m_bNormalsMode = FALSE;
    m_bWireframeMode = FALSE;
    m_bFaceSelectionMode = FALSE;
    m_bVertexSelectionMode = FALSE;
    m_bShowMeshSelectionMode = FALSE;
    m_bAnimPaused = FALSE;
    m_bLighting = TRUE;
    m_bUpdatedNormalDuringMouseMove = FALSE;
    m_dwTexCoordsShown = 0;

    m_hMenu = NULL;
    m_hMeshMenu = NULL;
    m_hAnimationsMenu = NULL;

    m_pmcSelectedMesh = NULL;
    m_pframeSelected = NULL;
    m_dwFaceSelected = UNUSED32;
    m_dwVertexSelected = UNUSED32;
    m_pdeHead = NULL;
    m_pdeSelected = NULL;

    m_pfxFaceSelect = NULL;
    m_pfxVertSelect = NULL;
    m_pfxShowNormals = NULL;
    m_pfxSkinnedAdjacency = NULL;

    m_pfvTreeView = NULL;

    m_dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1;

    m_sizeClient.cx = 0;
    m_sizeClient.cy = 0;

	m_ptcTextureCacheHead = NULL;
    m_method = D3DNONINDEXED;
    m_iPaletteSize = x_iDefaultSkinningPaletteSize;

    m_rgbFontColor = 0;
    m_hFontText = NULL;
    m_fExtrusion = 0.1f;
    m_fDeviation = 0.001f;
    strcpy(m_szText, "Hello");

    m_bDisableHWNPatches = FALSE;
    m_bForceSoftwareMode = FALSE;
    m_bForceRefRast = FALSE;
    m_bInitialFilename = FALSE;

    m_dwCullMode = D3DCULL_CCW;

    //m_fTicksPerSecond = 30.0f;
    m_fTicksPerSecond = 4800.0f;

    m_fFramesPerSec = 0.0f;
    m_fFramesPerSecCur = 0.0f;
    m_fLastUpdateTime = 0.0f;
    m_fTime = 0.0f;
}

TrivialData::~TrivialData( )
{
#if 0
    ReleaseFontAndTextBuffers();
#endif

    delete m_pdeHead;

    delete m_pfvTreeView;
}

HRESULT FrameViewSelectionCallback(PVOID pvCallbackData, PVOID pvItemId)
{
    TrivialData *pData = (TrivialData*)pvCallbackData;
    SFrame *pframeSelected = (SFrame*)pvItemId;

    return pData->SelectFrame(pframeSelected, NULL);
}

HRESULT TrivialData::SelectFrame(SFrame *pframeNew, SMeshContainer *pmcMesh)
{
    if (pframeNew != NULL)
    {
        m_pfvTreeView->SelectItem(pframeNew->m_hTreeHandle);

        // if selecting a new frame, unselect the face
        if (pframeNew != m_pframeSelected)
        {
            SelectFace(UNUSED32);
            SelectVertex(UNUSED32);
        }

        m_pframeSelected = pframeNew;

        if (pmcMesh == NULL)
            m_pmcSelectedMesh = pframeNew->pmcMesh;
        else 
            m_pmcSelectedMesh = pmcMesh;

        AdjustScrollbar();
    }

    return S_OK;
}

// mark a face as selected
HRESULT TrivialData::SelectFace(DWORD dwFace)
{
    DWORD cAttr;
    DWORD iAttr;
    D3DXATTRIBUTERANGE *rgaeTable;

    if (dwFace == UNUSED32)
    {
        m_dwFaceSelected = UNUSED32;
        m_dwFaceSelectedAttr = 0;
    }
    else
    {
        GXASSERT(m_pmcSelectedMesh != NULL);
        GXASSERT(m_pmcSelectedMesh->ptmDrawMesh != NULL);

        m_dwFaceSelected = dwFace;
        m_dwFaceSelectedAttr = 0;

        m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(NULL, &cAttr);
        if (cAttr > 0)
        {
            rgaeTable = (D3DXATTRIBUTERANGE*)_alloca(sizeof(D3DXATTRIBUTERANGE) * cAttr);

            // if tesselating, the attribute table must be the one tesselated from since the
            //   selected face is from there
            if (m_pmcSelectedMesh->pMeshToTesselate == NULL)
                m_pmcSelectedMesh->ptmDrawMesh->GetAttributeTable(rgaeTable, NULL);
            else
                m_pmcSelectedMesh->pMeshToTesselate->GetAttributeTable(rgaeTable, NULL);

            for (iAttr = 0; iAttr < cAttr; iAttr++)
            {
                if ((rgaeTable[iAttr].FaceStart <= dwFace) && ((rgaeTable[iAttr].FaceStart + rgaeTable[iAttr].FaceCount) > dwFace))
                {
                    m_dwFaceSelectedAttr = iAttr;
                    break;
                }
            }
        }
    }

    return S_OK;
}


// mark a face as selected
HRESULT TrivialData::SelectVertex(DWORD dwVertex)
{
    if (dwVertex == UNUSED32)
    {
        m_dwVertexSelected = UNUSED32;
    }
    else
    {
        GXASSERT(m_pmcSelectedMesh != NULL);
        GXASSERT(m_pmcSelectedMesh->ptmDrawMesh != NULL);

        m_dwVertexSelected = dwVertex;
    }

    return S_OK;
}

BOOL TrivialData::AreCapsSufficient(D3DCAPS9 *pCaps, DWORD dwBehaviorFlags)
{
    if(pCaps->AdapterOrdinal != 0)
        return FALSE;

	if(m_bForceRefRast && (pCaps->DeviceType != D3DDEVTYPE_REF))
		return FALSE;

    //if (dwBehaviorFlags == D3DCREATE_HARDWARE_VERTEXPROCESSING)
        //return FALSE;
        //return TRUE;
    if (dwBehaviorFlags == D3DCREATE_MIXED_VERTEXPROCESSING)
        return !m_bForceSoftwareMode;

    if (dwBehaviorFlags == D3DCREATE_SOFTWARE_VERTEXPROCESSING)
        return TRUE;

    return FALSE;//TRUE;    // TODO
}

void
TrivialData::SetupMenu()
{
    BOOL bSelectedContainer = FALSE;
    BOOL bPMMode = FALSE;
    BOOL bSelectedMesh = FALSE;
    BOOL bTessellateMode = FALSE;
    BOOL bNPatchMode = FALSE;
    BOOL bAnimPresent = FALSE;
    BOOL bSoftwareSkinning = (m_method == SOFTWARE);

    if (m_pmcSelectedMesh != NULL)
    {
        bSelectedContainer = TRUE;

        // first pmesh only menu items
        bPMMode =  m_pmcSelectedMesh->bPMMeshMode;
        bTessellateMode = m_pmcSelectedMesh->bTesselateMode;
        bNPatchMode = m_pmcSelectedMesh->bNPatchMode;

        bSelectedMesh = (m_pmcSelectedMesh->pMesh != NULL) && !m_pmcSelectedMesh->bPMMeshMode && !m_pmcSelectedMesh->bSimplifyMode && !m_pmcSelectedMesh->bTesselateMode && !m_pmcSelectedMesh->bNPatchMode;
    }

    if (m_pdeHead != NULL)
    {
        bAnimPresent = (m_pdeHead->m_pAnimMixer != NULL);
    }

    // enable/gray menu items requiring something loaded
    EnableMenuItem(m_hMenu, ID_FILE_CLOSEMESH, bSelectedContainer ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_FILE_CLOSENONSELECTED, bSelectedContainer ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_RESETMATRICES, bSelectedContainer ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_PROPERTIES, bSelectedContainer ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_FILE_SAVEMESH, (m_pdeHead != NULL) ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(m_hMenu, ID_MESHOPS_COLLAPSE, (m_pdeSelected != NULL) ? MF_ENABLED : MF_GRAYED);

    // enable/gray menu items requiring a "normal" mesh selected
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_GENERATEPM, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_COMPACT, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_ATTRSORT, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_STRIPREORDER, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_VERTEXCACHE, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIMIZE_SIMULATE, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_WELDVERTICES, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_NPATCHFRAME, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_SPLITMESH, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_OPTIONS_APPLYDISPLACEMENT, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_FILE_SAVEMESHTOM, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_MESHOPS_COMPUTENORMALS, bSelectedMesh ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_MESHOPS_VALIDATEMESH, bSelectedMesh ? MF_ENABLED : MF_GRAYED);

    // enable/gray pmesh menu items
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_SIMPLIFY, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_SIMPLIFYFACES, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_SETSOFTMIN, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_SETSOFTMAX, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_RESETSOFTMIN, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_RESETSOFTMAX, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_TRIM, bPMMode ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_SIMPLIFY_SNAPSHOT, bPMMode ? MF_ENABLED : MF_GRAYED);

    // enable/gray tessellate menu items
    //  UNDONE UNDONE - need to add patch menus instead of piggybacking on npatches
    EnableMenuItem(m_hMenu, ID_NPATCHES_SNAPSHOT, (bNPatchMode || bTessellateMode) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_NPATCHES_EDGEMODE, bNPatchMode ? MF_ENABLED : MF_GRAYED);
    CheckMenuItem(m_hMenu, ID_NPATCHES_EDGEMODE, m_bNPatchEdgeMode ? MF_CHECKED : MF_UNCHECKED);

    // setup check marks on menu's and toolbars
    CheckMenuItem(m_hMenu, ID_D3D_EDGEMODE, m_bEdgeMode ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_D3D_WIREFRAME, m_bWireframeMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_EDGEMODE, MAKELONG(m_bEdgeMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_WIREFRAME, MAKELONG(m_bWireframeMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_SOLID, MAKELONG((!m_bEdgeMode && !m_bWireframeMode), 0));

    CheckMenuItem(m_hMenu, ID_OPTIONS_VERTEXSELECTION, m_bVertexSelectionMode ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_OPTIONS_FACESELECTION, m_bFaceSelectionMode ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_OPTIONS_MESHSELECTION, m_bShowMeshSelectionMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_VERTEXSELECTION, MAKELONG(m_bVertexSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_FACESELECTION, MAKELONG(m_bFaceSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_MESHSELECTION, MAKELONG(m_bShowMeshSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_NOSELECTION, MAKELONG((!m_bFaceSelectionMode && !m_bVertexSelectionMode && !m_bShowMeshSelectionMode), 0));

    EnableMenuItem(m_hMenu, ID_D3D_ADJACENCYMODE, !bSoftwareSkinning && !bTessellateMode ? MF_ENABLED : MF_GRAYED);
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_D3D_ADJACENCYMODE, MAKELONG(!(bSoftwareSkinning||bTessellateMode), 0));
    CheckMenuItem(m_hMenu, ID_D3D_ADJACENCYMODE, m_bAdjacencyMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_ADJACENCYMODE, MAKELONG(m_bAdjacencyMode, 0));

    EnableMenuItem(m_hMenu, ID_D3D_CREASEMODE, !bSoftwareSkinning && !bTessellateMode ? MF_ENABLED : MF_GRAYED);
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_D3D_CREASEMODE, MAKELONG(!(bSoftwareSkinning||bTessellateMode), 0));
    CheckMenuItem(m_hMenu, ID_D3D_CREASEMODE, m_bCreaseMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_CREASEMODE, MAKELONG(m_bCreaseMode, 0));

    EnableMenuItem(m_hMenu, ID_D3D_STRIPMODE, !bSoftwareSkinning && !bTessellateMode ? MF_ENABLED : MF_GRAYED);
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_D3D_STRIPMODE, MAKELONG(!(bSoftwareSkinning||bTessellateMode), 0));
    CheckMenuItem(m_hMenu, ID_D3D_STRIPMODE, m_bStripMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_STRIPMODE, MAKELONG(m_bStripMode, 0));

    EnableMenuItem(m_hMenu, ID_D3D_SHOWNORMALS, !bSoftwareSkinning ? MF_ENABLED : MF_GRAYED);
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_D3D_SHOWNORMALS, MAKELONG(!bSoftwareSkinning, 0));
    CheckMenuItem(m_hMenu, ID_D3D_SHOWNORMALS, m_bNormalsMode ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_SHOWNORMALS, MAKELONG(m_bNormalsMode, 0));


    CheckMenuItem(m_hMenu, ID_D3D_LIGHTING, m_bLighting ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_D3D_CULL, (m_dwCullMode == D3DCULL_CCW) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_OPTIONS_TEXTURE, m_bTextureMode ? MF_CHECKED : MF_UNCHECKED);

    CheckMenuItem(m_hMenu, ID_D3D_SOFTWARESKIN, (m_method == SOFTWARE) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_D3D_INDEXED,      (m_method == D3DINDEXED) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_D3D_NONINDEXED,   (m_method == D3DNONINDEXED) ? MF_CHECKED : MF_UNCHECKED);


    CheckMenuItem(m_hMenu, ID_VIEW_PLAYBACKSPEED, (m_fTicksPerSecond == 4800.0f) ? MF_CHECKED : MF_UNCHECKED);
    EnableMenuItem(m_hMenu, ID_VIEW_PLAYANIM, (bAnimPresent && m_bAnimPaused) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(m_hMenu, ID_VIEW_PAUSEANIM, (bAnimPresent && !m_bAnimPaused) ? MF_ENABLED : MF_GRAYED);
    CheckMenuItem(m_hMenu, ID_VIEW_PLAYANIM, (bAnimPresent && !m_bAnimPaused) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_VIEW_PAUSEANIM, (bAnimPresent && m_bAnimPaused) ? MF_CHECKED : MF_UNCHECKED);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_VIEW_PLAYANIM, MAKELONG(bAnimPresent && !m_bAnimPaused, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_VIEW_PAUSEANIM, MAKELONG(bAnimPresent && m_bAnimPaused, 0));
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_VIEW_PLAYANIM, MAKELONG(bAnimPresent, 0));
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, ID_VIEW_PAUSEANIM, MAKELONG(bAnimPresent, 0));
}


// go to each mesh container and reset all the effects
HRESULT
ResetEffects(SFrame *pframe, TrivialData *pApp)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcCur = pframe->pmcMesh;
    SFrame *pframeCur;
    DWORD ifx;

    while (pmcCur != NULL)
    {
        for (ifx = 0; ifx < pmcCur->NumMaterials; ifx++)
        {
            if (pmcCur->m_rgpfxAttributes[ifx] != NULL)
            {
                pmcCur->m_rgpfxAttributes[ifx]->OnResetDevice();
            }
        }

        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = ResetEffects(pframeCur, pApp);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    return hr;
}

HRESULT TrivialData::OnResetDevice()
{
    HRESULT hr = S_OK;
    RECT r;
    D3DLIGHT9 light;
    D3DCAPS9 Caps;

    SetupMenu();

    GetClientRect( m_hwnd, &r );

    // setup the client rect size, used for frame rate info
    m_sizeClient.cx = r.right - r.left;
    m_sizeClient.cy = r.bottom - r.top;

    m_abArcBall.SetWindow(r.right, r.bottom, 0.85f);

    if (m_pdeSelected != NULL)
        SetProjectionMatrix();

    //m_FPSMeter.OnResetDevice(m_hwnd, m_pDevice);    //

    m_pDevice->GetDeviceCaps(&Caps);
    m_bHWVertexShaders = Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
    m_bHWNPatches = !m_bDisableHWNPatches && (Caps.DevCaps & D3DDEVCAPS_NPATCHES);   
    m_bSoftwareVP = FAILED(m_pDevice->SetSoftwareVertexProcessing(FALSE));


    m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    m_pDevice->SetRenderState( D3DRS_AMBIENT,  0xffffffff);
    m_pDevice->SetNPatchMode( 0 );
    m_pDevice->SetRenderState( D3DRS_CULLMODE, m_dwCullMode );
    m_pDevice->SetRenderState( D3DRS_LIGHTING, m_bLighting );

    m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR  );
    m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR  );
    m_pDevice->SetRenderState( D3DRS_FILLMODE, m_bWireframeMode ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    light.Type        = D3DLIGHT_DIRECTIONAL;

    light.Diffuse.r = 1.0;
    light.Diffuse.g = 1.0;
    light.Diffuse.b = 1.0;
    light.Specular.r = 0;
    light.Specular.g = 0;
    light.Specular.b = 0;
    //light.Ambient.r = 0.25;
    //light.Ambient.g = 0.25;
    //light.Ambient.b = 0.25;
    light.Ambient.r = 1;
    light.Ambient.g = 1;
    light.Ambient.b = 1;

    light.Position     = D3DXVECTOR3(0.0f, 0.0f, -20.0f);
    light.Direction    = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;
    light.Range = ((float)sqrt(FLT_MAX));

    switch( light.Type )
    {
        case D3DLIGHT_POINT:
            light.Attenuation0 = 1.0f;
            break;
        case D3DLIGHT_DIRECTIONAL:
            light.Position     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            break;
        case D3DLIGHT_SPOT:
            light.Position     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            light.Range        =   1.0f;
            light.Falloff      = 100.0f;
            light.Theta        =   0.8f;
            light.Phi          =   1.0f;
            light.Attenuation2 =   1.0f;
    }

    hr = m_pDevice->SetLight(0, &light );
    if (FAILED(hr))
        return E_FAIL;

    light.Diffuse.r = 0.5;
    light.Diffuse.g = 0.5;
    light.Diffuse.b = 0.5;

    hr = m_pDevice->SetLight(1, &light );
    if (FAILED(hr))
        return E_FAIL;

    hr = m_pDevice->LightEnable(0, TRUE);
    if (FAILED(hr))
        return E_FAIL;

    hr = m_pDevice->LightEnable(1, FALSE);
    if (FAILED(hr))
        return E_FAIL;

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.  
    // Debugging vertex shaders requires either REF or software vertex processing, and debugging 
    // pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug 
    // experience in the shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile against the next 
    // higher available software target, which ensures that the unoptimized shaders do not exceed 
    // the shader model limitations.  Setting these flags will cause slower rendering since the shaders 
    // will be unoptimized and forced into software.  See the DirectX documentation for more information 
    // about using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    hr = D3DXCreateEffectFromResource(m_pDevice, NULL, MAKEINTRESOURCE(IDD_FACESELECT), NULL, NULL, dwShaderFlags, NULL, &m_pfxFaceSelect, NULL);
    if (FAILED(hr))
        return hr;

    hr = D3DXCreateEffectFromResource(m_pDevice, NULL, MAKEINTRESOURCE(IDD_VERTSELECT), NULL, NULL, dwShaderFlags, NULL, &m_pfxVertSelect, NULL);
    if (FAILED(hr))
        return hr;

    hr = D3DXCreateEffectFromResource(m_pDevice, NULL, MAKEINTRESOURCE(IDD_SHOWNORMALS), NULL, NULL, dwShaderFlags, NULL, &m_pfxShowNormals, NULL);
    if (FAILED(hr))
        return hr;

    //hr = D3DXCreateEffectFromFile(m_pDevice, "adjacency.fx", NULL, NULL, dwShaderFlags, NULL, &m_pfxSkinnedAdjacency, NULL);
    hr = D3DXCreateEffectFromResource(m_pDevice, NULL, MAKEINTRESOURCE(IDD_ADJACENCY), NULL, NULL, dwShaderFlags, NULL, &m_pfxSkinnedAdjacency, NULL);
    if (FAILED(hr))
        return hr;

    if (m_pdeHead != NULL)
    {
        ResetEffects(m_pdeHead->pframeRoot, this);
    }

    // a file was requested to be loaded on startup
    if (m_bInitialFilename)
    {
        hr = LoadMeshHierarchyFromFile(m_szInitialFilename);
        m_bInitialFilename = FALSE;

        if (FAILED(hr))
        {
            MessageBox( m_hwnd, "Unabled to load the specified file.", "Load failed!", MB_OK);
            hr = S_OK;
        }
    }

    return S_OK;
}

HRESULT TrivialData::OnDestroyDevice()
{
    //m_FPSMeter.OnDestroyDevice();   //

    delete m_pdeHead;
    m_pdeHead = NULL;

	delete m_ptcTextureCacheHead;
    m_ptcTextureCacheHead = NULL;

    return S_OK;
}

HRESULT
OnLostEffects(SFrame *pframe)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcCur = pframe->pmcMesh;
    SFrame *pframeCur;
    DWORD ifx;

    while (pmcCur != NULL)
    {
        for (ifx = 0; ifx < pmcCur->NumMaterials; ifx++)
        {
            if (pmcCur->m_rgpfxAttributes[ifx] != NULL)
            {
                pmcCur->m_rgpfxAttributes[ifx]->OnLostDevice();
            }
        }

        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    pframeCur = pframe->pframeFirstChild;
    while (pframeCur != NULL)
    {
        hr = OnLostEffects(pframeCur);
        if (FAILED(hr))
            goto e_Exit;

        pframeCur = pframeCur->pframeSibling;
    }

e_Exit:
    return hr;
}

HRESULT TrivialData::OnLostDevice()
{
    //m_FPSMeter.OnLostDevice();  //

    GXRELEASE(m_pfxFaceSelect);
    GXRELEASE(m_pfxVertSelect);
    GXRELEASE(m_pfxShowNormals);
    GXRELEASE(m_pfxSkinnedAdjacency);

    if (m_pdeHead != NULL)
    {
        OnLostEffects(m_pdeHead->pframeRoot);
    }

    return S_OK;
}

HRESULT TrivialData::OnCreateDevice()
{
    HRESULT hr = S_OK;

    AdjustScrollbar();
    SetProjectionMatrix();

    D3DCAPS9 caps;
    hr = m_pDevice->GetDeviceCaps(&caps);
    if (hr != S_OK)
        return hr;
    m_maxFaceInflHW = caps.MaxVertexBlendMatrices;

    if (m_pfvTreeView == NULL)
    {
        RECT rTreeView;

        GetWindowRect( m_hwnd, &rTreeView );
        rTreeView.right =  rTreeView.left + (rTreeView.right - rTreeView.left) / 3;

        hr = CreateFrameView(rTreeView, m_hwnd, m_hInstance,
                                    FrameViewSelectionCallback, (PVOID)this,
                                    TrivialData::WndProcCallback,
                                    &m_pfvTreeView);
        if (FAILED(hr))
            goto e_Exit;

        SetFocus(m_hwnd);
    }

    m_ShowArcball.Init(m_pDevice, 64);

    // prepare

e_Exit:
    return hr;
}

void TrivialData::ToggleTreeView()
{
    BOOL bVisible;
    RECT rTreeView;
    RECT rWindowRect;

    // toggle visibility of tree view
    bVisible = m_pfvTreeView->ToggleVisible();

    CheckMenuItem(m_hMenu, ID_OPTIONS_TREEVIEW, bVisible ? MF_CHECKED : MF_UNCHECKED);

    // if visible, make sure that the two windows don't overlap each other
    if (bVisible)
    {
        m_pfvTreeView->GetWindowRect(&rTreeView);
        GetWindowRect( m_hwnd, &rWindowRect );

        // if overlapping, move 
        if (rWindowRect.left == rTreeView.left)
        {
            rWindowRect.left += (rTreeView.right - rTreeView.left);
            rWindowRect.right += (rTreeView.right - rTreeView.left);

            SetWindowPos( m_hwnd, HWND_TOP, rWindowRect.left, rWindowRect.top,
                                        rWindowRect.right - rWindowRect.left,
                                        rWindowRect.bottom - rWindowRect.top, SWP_NOZORDER);
        }
    }
}

void
TrivialData::PauseDrawing( )
{
    //bPaused = true;

#if 0
    if ( bFullScreen )
    {
        pDXMgr->FlipToGDISurface();
        DrawMenuBar(m_hwnd);
        RedrawWindow(m_hwnd, NULL, NULL, RDW_FRAME);
    }
#endif
}

void
TrivialData::RestartDrawing( )
{
    //bPaused = false;
}

static BOOL CALLBACK
EnumFirstResource(HINSTANCE hInstance, LPCTSTR lpszType, LPTSTR lpszName, LPARAM lParam)
{
    *((LPSTR *) lParam) = lpszName;
    return FALSE;
}

void
TrivialData::InitializeHelper()
{
    HMENU hAnimMenu;
    m_hMenu = GetMenu(m_hwnd);

    // get the animation sub-sub-menu
    hAnimMenu = GetSubMenu(m_hMenu, 3);
    m_hAnimationsMenu = GetSubMenu(hAnimMenu, 5);

    m_hFontText = CreateFont(
        12,
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        VARIABLE_PITCH,
        "Arial" );

    InitCommonControls();

    // Create the status bar. 
    m_hwndStatus = CreateWindowEx( 
        0,                       // no extended styles 
        STATUSCLASSNAME,                // name of status bar class 
        (LPCTSTR) NULL,          // no text when first created 
        WS_CHILD | WS_VISIBLE,                // creates a child window 
        0, 0, 0, 0,              // ignores size and position 
        m_hwnd,                  // handle to parent window 
        (HMENU) 0,               // child window identifier 
        m_hInstance,             // handle to application instance 
        NULL);                   // no window creation data 


    RECT r;
    INT lpParts[x_cStatusBarSizes+1];// = {280, 370, 430, 500, 560, 640};
    DWORD iCur;
    INT iPart;

    GetClientRect( m_hwnd, &r );
    iCur = r.right - r.left;
    for (iPart = x_cStatusBarSizes - 1; iPart >= 0; iPart--)
    {
        lpParts[iPart+1] = iCur;
        iCur -= x_rgStatusBarSizes[iPart];
    }
    lpParts[0] = iCur;

    SendMessage(m_hwndStatus, SB_SETPARTS, (WPARAM) x_cStatusBarSizes+1, (LPARAM) lpParts); 


    const DWORD cButtons = 18;
    TBBUTTON Buttons[cButtons];

    Buttons[0].iBitmap = 0; 
    Buttons[0].idCommand = ID_OPTIONS_NOSELECTION; 
    Buttons[0].fsState = TBSTATE_ENABLED; 
    Buttons[0].fsStyle = TBSTYLE_BUTTON; 
    Buttons[0].dwData = 0; 
    Buttons[0].iString = 0; 

    Buttons[1].iBitmap = 13; 
    Buttons[1].idCommand = ID_OPTIONS_MESHSELECTION; 
    Buttons[1].fsState = TBSTATE_ENABLED; 
    Buttons[1].fsStyle = TBSTYLE_BUTTON; 
    Buttons[1].dwData = 0; 
    Buttons[1].iString = 0; 

    Buttons[2].iBitmap = 1; 
    Buttons[2].idCommand = ID_OPTIONS_FACESELECTION; 
    Buttons[2].fsState = TBSTATE_ENABLED; 
    Buttons[2].fsStyle = TBSTYLE_BUTTON; 
    Buttons[2].dwData = 0; 
    Buttons[2].iString = 0; 

    Buttons[3].iBitmap = 2; 
    Buttons[3].idCommand = ID_OPTIONS_VERTEXSELECTION; 
    Buttons[3].fsState = TBSTATE_ENABLED; 
    Buttons[3].fsStyle = TBSTYLE_BUTTON; 
    Buttons[3].dwData = 0; 
    Buttons[3].iString = 0; 

    Buttons[4].iBitmap = STD_HELP; 
    Buttons[4].idCommand = 0; 
    Buttons[4].fsState = TBSTATE_ENABLED; 
    Buttons[4].fsStyle = TBSTYLE_SEP; 
    Buttons[4].dwData = 0; 
    Buttons[4].iString = 0; 

    Buttons[5].iBitmap = 3; 
    Buttons[5].idCommand = ID_D3D_SOLID; 
    Buttons[5].fsState = TBSTATE_ENABLED; 
    Buttons[5].fsStyle = TBSTYLE_BUTTON; 
    Buttons[5].dwData = 0; 
    Buttons[5].iString = 0; 

    Buttons[6].iBitmap = 4; 
    Buttons[6].idCommand = ID_D3D_WIREFRAME; 
    Buttons[6].fsState = TBSTATE_ENABLED; 
    Buttons[6].fsStyle = TBSTYLE_BUTTON; 
    Buttons[6].dwData = 0; 
    Buttons[6].iString = 0; 

    Buttons[7].iBitmap = 5; 
    Buttons[7].idCommand = ID_D3D_EDGEMODE; 
    Buttons[7].fsState = TBSTATE_ENABLED; 
    Buttons[7].fsStyle = TBSTYLE_BUTTON; 
    Buttons[7].dwData = 0; 
    Buttons[7].iString = 0; 

    Buttons[8].iBitmap = STD_HELP; 
    Buttons[8].idCommand = 0; 
    Buttons[8].fsState = TBSTATE_ENABLED; 
    Buttons[8].fsStyle = TBSTYLE_SEP; 
    Buttons[8].dwData = 0; 
    Buttons[8].iString = 0; 

    Buttons[9].iBitmap = 6; 
    Buttons[9].idCommand = ID_D3D_ADJACENCYMODE; 
    Buttons[9].fsState = TBSTATE_ENABLED; 
    Buttons[9].fsStyle = TBSTYLE_BUTTON; 
    Buttons[9].dwData = 0; 
    Buttons[9].iString = 0; 

    Buttons[10].iBitmap = 7; 
    Buttons[10].idCommand = ID_D3D_STRIPMODE; 
    Buttons[10].fsState = TBSTATE_ENABLED; 
    Buttons[10].fsStyle = TBSTYLE_BUTTON; 
    Buttons[10].dwData = 0; 
    Buttons[10].iString = 0; 

    Buttons[11].iBitmap = 8; 
    Buttons[11].idCommand = ID_D3D_CREASEMODE; 
    Buttons[11].fsState = TBSTATE_ENABLED; 
    Buttons[11].fsStyle = TBSTYLE_BUTTON; 
    Buttons[11].dwData = 0; 
    Buttons[11].iString = 0; 

    Buttons[12].iBitmap = 9; 
    Buttons[12].idCommand = ID_D3D_SHOWNORMALS; 
    Buttons[12].fsState = TBSTATE_ENABLED; 
    Buttons[12].fsStyle = TBSTYLE_BUTTON; 
    Buttons[12].dwData = 0; 
    Buttons[12].iString = 0; 

    Buttons[13].iBitmap = STD_HELP; 
    Buttons[13].idCommand = 0; 
    Buttons[13].fsState = TBSTATE_ENABLED; 
    Buttons[13].fsStyle = TBSTYLE_SEP; 
    Buttons[13].dwData = 0; 
    Buttons[13].iString = 0; 

    Buttons[14].iBitmap = 10; 
    Buttons[14].idCommand = ID_OPTIONS_INFO; 
    Buttons[14].fsState = TBSTATE_ENABLED; 
    Buttons[14].fsStyle = TBSTYLE_BUTTON; 
    Buttons[14].dwData = 0; 
    Buttons[14].iString = 0; 

    Buttons[15].iBitmap = STD_HELP; 
    Buttons[15].idCommand = 0; 
    Buttons[15].fsState = TBSTATE_ENABLED; 
    Buttons[15].fsStyle = TBSTYLE_SEP; 
    Buttons[15].dwData = 0; 
    Buttons[15].iString = 0; 

    Buttons[16].iBitmap = 11; 
    Buttons[16].idCommand = ID_VIEW_PLAYANIM; 
    Buttons[16].fsState = TBSTATE_ENABLED; 
    Buttons[16].fsStyle = TBSTYLE_BUTTON; 
    Buttons[16].dwData = 0; 
    Buttons[16].iString = 0; 

    Buttons[17].iBitmap = 12; 
    Buttons[17].idCommand = ID_VIEW_PAUSEANIM; 
    Buttons[17].fsState = TBSTATE_ENABLED; 
    Buttons[17].fsStyle = TBSTYLE_BUTTON; 
    Buttons[17].dwData = 0; 
    Buttons[17].iString = 0; 


    m_hwndToolbar = CreateToolbarEx(m_hwnd, WS_CHILD | WS_VISIBLE, 1, 15, m_hInstance, IDB_BITMAP1/*HINST_COMMCTRL, IDB_STD_SMALL_COLOR*/, Buttons, cButtons, 18, 17, 16, 15, sizeof(TBBUTTON));

    GetClientRect(m_hwndToolbar, &r);
}

BOOL WINAPI D3DXDumpUnfreedMemoryInfo();

int APIENTRY
WinMain( HINSTANCE hinst,
         HINSTANCE hPrevInstance,
         LPSTR lpCmdLine,
         int nCmdShow )
{
    HRESULT hr;
    WNDCLASS wndclass;
    LPSTR szIcon = NULL;
    char *szFilename;
    char szFilenameBuffer[256];
    BOOL bFilenameProvided = FALSE;
    BOOL bForceSoftwareMode = FALSE;
    BOOL bForceRefRast = FALSE;
    DWORD dwWidth = 640;
    DWORD dwHeight = 480;
    BOOL bDisableHWNPatches = FALSE;
    BOOL bAdjacencyMode = FALSE;
    BOOL bStripMode = FALSE;
    BOOL bNormalsMode = FALSE;
    BOOL bCreaseMode = FALSE;
                    
    // Parse command line.
    if(lpCmdLine)
    {
        LPCSTR pchMin, pchLim;
        pchLim = lpCmdLine;

        for(;;)
        {
            for(pchMin = pchLim; *pchMin == ' '; pchMin++);
            for(pchLim = pchMin; *pchLim != ' ' && *pchLim != '\0'; pchLim++)
            {
                // check for quotes to ignore spaces
                if (*pchLim == '"')
                {
                    pchLim++;
                    while (*pchLim != '"' && *pchLim != '\0')
                    {
                        pchLim++;
                    }
                }
            }

            if(*pchMin == '\0')
                break;
            if(*pchMin == '-' || *pchMin == '/')
            {
                char szToken[16];
                int cchToken = 0;

                for(pchMin++; pchMin < pchLim && cchToken < 15; pchMin++)
                    szToken[cchToken++] = static_cast<char>(tolower(*pchMin));

                szToken[cchToken] = '\0';

                if ((strcmp(szToken, "sw") == 0) || (strcmp(szToken, "software") == 0))
                {
                    bForceSoftwareMode = TRUE;
                }
                else if ((strcmp(szToken, "ref") == 0) || (strcmp(szToken, "reference") == 0))
                {
                    bForceRefRast = TRUE;
                }
                else if (!memcmp(szToken, "w:", 2) || !memcmp(szToken, "width:", 6))
                {
                    dwWidth = atoi(strchr(szToken, ':') + 1);
                }

                else if (!memcmp(szToken, "h:", 2) || !memcmp(szToken, "height:", 7))
                {
                    dwHeight = atoi(strchr(szToken, ':') + 1);
                }
                else if ((strcmp(szToken, "hd") == 0) || (strcmp(szToken, "hwnpatchesdisable") == 0))
                {
                    bDisableHWNPatches = TRUE;
                }
                else if (strcmp(szToken, "a") == 0)
                {
                    bAdjacencyMode = TRUE;
                }
                else if (strcmp(szToken, "s") == 0)
                {
                    bStripMode = TRUE;
                }
                else if (strcmp(szToken, "n") == 0)
                {
                    bNormalsMode = TRUE;
                }
                else if (strcmp(szToken, "r") == 0)
                {
                    bCreaseMode = TRUE;
                }

#if 0


                if(!strcmp(szToken, "hal"))
                {
                    m_pData->m_Options.m_bForceDevType = TRUE;
                    m_pData->m_Options.m_DevType = D3DDEVTYPE_HAL;
                }

                else if(!strcmp(szToken, "emu") || !strcmp(szToken, "emulation") || !strcmp(szToken, "rgb"))
                {
                    m_pData->m_Options.m_bForceDevType = TRUE;
                    m_pData->m_Options.m_DevType = D3DDEVTYPE_SW;
                }

                else if(!strcmp(szToken, "ref") || !strcmp(szToken, "reference"))
                {
                    m_pData->m_Options.m_bForceDevType = TRUE;
                    m_pData->m_Options.m_DevType = D3DDEVTYPE_REF;
                }

                else if(!strcmp(szToken, "hw") || !strcmp(szToken, "hardware"))
                {
                    m_pData->m_Options.m_bForceBehavior = TRUE;
                    m_pData->m_Options.m_dwBehavior |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                }

                else if(!strcmp(szToken, "pure"))
                {
                    m_pData->m_Options.m_bForceBehavior = TRUE;
                    m_pData->m_Options.m_dwBehavior |= D3DCREATE_PUREDEVICE;
                }

#endif
            }
            else
            {
                DWORD ich;
                szFilename = szFilenameBuffer;
                for(ich=0; pchMin < pchLim; pchMin++,ich++)
                    szFilename[ich] = *pchMin;

                szFilename[ich] = '\0';
                bFilenameProvided = TRUE;

                // if there are quotes, then remove them
                if ((szFilename[0] == '"') && (szFilename[ich-1] == '"'))
                {
                    szFilename[ich-1] = '\0';
                    szFilename++;
                }
            }
        }
    }

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = (WNDPROC) CD3DXApplication::WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hinst;
    wndclass.hIcon         = LoadIcon(hinst, MAKEINTRESOURCE(IDI_MAINICON));
    wndclass.hCursor       = CopyCursor(LoadCursor(NULL, IDC_ARROW));
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName = "MeshView";
    wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
    if (! RegisterClass(&wndclass) )
    {
        return FALSE;
    }

    InitFrameViewClass(hinst);

    // indent so that App will get destroyed before memory check
    {
        TrivialData App;
        g_pData = &App;

        if(FAILED(hr = App.Initialize(hinst, "MeshView", "MeshView", dwWidth, dwHeight)))
        {
            MessageBox(NULL, DXGetErrorString8(hr), "Error", MB_OK);
        }

        App.InitializeHelper();

        if (bFilenameProvided)
        {
            if( szFilename[1] != ':' &&
                !(szFilename[0] == '\\' && szFilename[1] == '\\') )
            {
			    GetCurrentDirectory(sizeof(App.m_szInitialFilename), App.m_szInitialFilename);
			    strcat(App.m_szInitialFilename, "\\");
			    strcat(App.m_szInitialFilename, szFilename);
            }
            else
            {
                strcpy(App.m_szInitialFilename, szFilename);
            }

            App.m_bInitialFilename = TRUE;
        }

        App.m_bForceSoftwareMode = bForceSoftwareMode;
        App.m_bForceRefRast = bForceRefRast;
        App.m_bDisableHWNPatches = bDisableHWNPatches;
        App.m_bAdjacencyMode = bAdjacencyMode;
        App.m_bStripMode = bStripMode;
        App.m_bNormalsMode = bNormalsMode;
        App.m_bCreaseMode = bCreaseMode;

        if (FAILED(hr = App.Run()))
        {
            MessageBox(NULL, DXGetErrorString8(hr), "Error", MB_OK);
        }
    }

//  GXASSERT(!D3DXDumpUnfreedMemoryInfo());

    D3DXDumpUnfreedMemoryInfo();

    return 0;
}



HRESULT
TrivialData::SetProjectionMatrix()
{
    D3DXMATRIX mat;
    RECT r;

    if (m_pdeHead == NULL)
        return S_OK;

    GetClientRect( m_hwnd, &r );
    r.top += 28;
    //r.bottom -= 48;
    r.bottom -= 20;

    float fAspect = (float)(r.right - r.left) / (float)(r.bottom - r.top);

    D3DXMatrixPerspectiveFovLH(&mat, 0.25f*3.141592654f, fAspect, m_pdeSelected->fRadius / 64, m_pdeSelected->fRadius * 200);
    return m_pDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&mat );
}


void
TrivialData::AdjustScrollbar()
{
    SCROLLINFO sbInfo;
    DWORD min,max,cur;
    sbInfo.cbSize = sizeof(SCROLLINFO);
    sbInfo.fMask = SIF_ALL;

    if (m_pmcSelectedMesh == NULL)
    {
        min = 0;
        max = 0;
        cur = 0;
        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "Nothing Selected"); 
    }
    else if (m_pmcSelectedMesh->bPMMeshMode)
    {
        min = m_pmcSelectedMesh->pPMMesh->GetMinVertices();
        max = m_pmcSelectedMesh->pPMMesh->GetMaxVertices();
        cur = m_pmcSelectedMesh->pPMMesh->GetNumVertices();
        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "PMesh Mode"); 
    }
    else if (m_pmcSelectedMesh->bSimplifyMode)
    {
        min = 1;
        max = m_pmcSelectedMesh->pSimpMesh->GetMaxVertices();
        cur = m_pmcSelectedMesh->pSimpMesh->GetNumVertices();
        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "Simplify Mode"); 
    }
    else if (m_pmcSelectedMesh->bTesselateMode || m_pmcSelectedMesh->bNPatchMode)
    {
        min = 0;
        max = 32;
        cur = m_pmcSelectedMesh->cTesselateLevel;

        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "N-Patch Mode"); 
    }
    else
    {
        min = 0;
        max = 0;
        cur = 0;
        SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 1, (LPARAM) "Polygon Mode"); 
    }

    cur = min + max - cur;

    sbInfo.nMin = min;
    sbInfo.nMax = max;
    sbInfo.nPage = 1;
    sbInfo.nPos = cur;
    sbInfo.nTrackPos = sbInfo.nMin;

    SetScrollInfo(m_hwnd, SB_VERT, &sbInfo, TRUE);

    SetupMenu();
}

HRESULT
TrivialData::UpdateTreeInfo(SFrame *pframeCur, HTREEITEM htreeParent)
{
    HRESULT hr = S_OK;
    SMeshContainer *pmcMesh;
    SFrame *pframeChild;
    char *szNameCur;
    HTREEITEM htreeCur;

    if (pframeCur->szName == NULL)
    {
        szNameCur = "Unnamed Frame";
    }
    else
    {
        szNameCur = pframeCur->szName;
    }

    pframeCur->m_hTreeHandle = m_pfvTreeView->InsertIntoTree(htreeParent, szNameCur, (PVOID)pframeCur);

    pmcMesh = pframeCur->pmcMesh;
    while (pmcMesh != NULL)
    {
        if (pmcMesh->Name == NULL)
        {
            szNameCur = "Unnamed Mesh";
        }
        else
        {
            szNameCur = pmcMesh->Name;
        }

        m_pfvTreeView->InsertIntoTree(pframeCur->m_hTreeHandle, szNameCur, (PVOID)pframeCur);

        pmcMesh = (SMeshContainer*)pmcMesh->pNextMeshContainer;
    }

    pframeChild = pframeCur->pframeFirstChild;
    while (pframeChild != NULL)
    {
        UpdateTreeInfo(pframeChild, pframeCur->m_hTreeHandle);

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}

HRESULT
TrivialData::UpdateMeshMenu()
{
    HRESULT hr;

#if 0
    SDrawElement *pdeCur;
    UINT cMeshes;
    UINT iMesh;

    cMeshes = GetMenuItemCount(m_hMeshMenu);
    for (iMesh = 0; iMesh < cMeshes; iMesh++)
    {
        RemoveMenu(m_hMeshMenu, 0, MF_BYPOSITION);
    }

    iMesh = 0;
    pdeCur = m_pdeHead;
    while ((pdeCur != NULL) && (iMesh < 10))
    {
        if (pdeCur->szName != NULL)
            AppendMenu(m_hMeshMenu, MF_STRING, ID_MESH_1 + iMesh, pdeCur->szName);
        else
            AppendMenu(m_hMeshMenu, MF_STRING, ID_MESH_1 + iMesh, "<No Mesh Open>");

        pdeCur = pdeCur->pdeNext;
        iMesh += 1;
    }
#endif

    if (m_pfvTreeView != NULL)
    {
        // now update the tree control
        m_pfvTreeView->RemoveAllItems();

        if (m_pframeSelected != NULL)
        {
            hr = UpdateTreeInfo(m_pframeSelected, NULL);
            if (FAILED(hr))
                return hr;
        }
    }

    return S_OK;
}

HRESULT
TrivialData::UpdateAnimationsMenu()
{
    HRESULT hr;

    UINT cAnimSets;
    UINT iAnimSet;
    LPD3DXANIMATIONSET pAnimSet;
    LPCTSTR szName;

    cAnimSets = GetMenuItemCount(m_hAnimationsMenu);
    for (iAnimSet = 0; iAnimSet < cAnimSets; iAnimSet++)
    {
        RemoveMenu(m_hAnimationsMenu, 0, MF_BYPOSITION);
    }

    if (m_pdeHead == NULL)
        return S_OK;

    if (m_pdeHead->m_pAnimMixer == NULL)
    {
        AppendMenu(m_hAnimationsMenu, MF_STRING, ID_ANIMATION_1 + iAnimSet, "No Animation");
    }

    if (m_pdeHead->m_pAnimMixer != NULL)
        cAnimSets = min(m_pdeHead->m_pAnimMixer->GetNumAnimationSets(), 50);
    else
        cAnimSets = 0;

    for (iAnimSet = 0; iAnimSet < cAnimSets; iAnimSet++)
    {
        m_pdeHead->m_pAnimMixer->GetAnimationSet(iAnimSet, &pAnimSet);

        szName = pAnimSet->GetName();

        if (szName != NULL)
            AppendMenu(m_hAnimationsMenu, MF_STRING, ID_ANIMATION_1 + iAnimSet, szName);
        else
            AppendMenu(m_hAnimationsMenu, MF_STRING, ID_ANIMATION_1 + iAnimSet, "<No Animation Name>");

        GXRELEASE(pAnimSet);
    }

    CheckMenuItem(m_hMenu, ID_ANIMATION_1 + m_pdeHead->m_iSelectedAnimSet, MF_CHECKED);

    return S_OK;
}

void
TrivialData::SelectDrawElement(SDrawElement *pdeNew)
{
    HRESULT hr;

    if (m_pdeSelected != pdeNew)
    {
        if (pdeNew != NULL)
        {
            m_pdeSelected = pdeNew;

            SelectFrame(pdeNew->pframeRoot, NULL);
            SelectFace(UNUSED32);
            SelectVertex(UNUSED32);
        }
        else
        {
            m_pdeSelected = NULL;
            m_pmcSelectedMesh = NULL;
            m_pframeSelected = NULL;
        }

        AdjustScrollbar();
        m_abArcBall.Reset();

        hr = UpdateMeshMenu();
        if (FAILED(hr))
            return;
    }
}

void
TrivialData::SwitchToAnimationSet(UINT iAnimSet)
{
    LPD3DXANIMATIONSET pAnim;

    GXASSERT(iAnimSet <= 50);

    CheckMenuItem(m_hMenu, ID_ANIMATION_1 + m_pdeHead->m_iSelectedAnimSet, MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_ANIMATION_1 + iAnimSet, MF_CHECKED);

    m_pdeHead->m_iSelectedAnimSet = iAnimSet;

    // should only be NULL if a user clicked on a placeholder
    if (m_pdeHead->m_pAnimMixer != NULL)
    {
        m_pdeHead->m_pAnimMixer->GetAnimationSet(iAnimSet, &pAnim);
        m_pdeHead->m_pAnimMixer->SetTrackAnimationSet(0, pAnim);
        GXRELEASE(pAnim);
    }
}

#if 0
void
TrivialData::SwitchToMeshID(UINT iMeshID)
{
    UINT iMesh;
    UINT iCurMesh;
    SDrawElement *pdeCur;

    iMesh = iMeshID - ID_MESH_1;
    GXASSERT(iMesh <= 8);

    iCurMesh = 0;
    pdeCur = m_pdeHead;
    while ((pdeCur != NULL) && (iCurMesh < iMesh))
    {
            pdeCur = pdeCur->pdeNext;
            iCurMesh++;
    }
    GXASSERT(iCurMesh == iMesh);
    GXASSERT(pdeCur != NULL);

    SelectDrawElement(pdeCur);
}
#endif

DWORD GetVertexIndexFromBarycentric(float fU, float fV)
{
    float fOther = 1.0f - fU - fV;

    if (fU > fV)
    {
        if (fU > fOther)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else 
    {
        if (fV > fOther)
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
}

BOOL BIntersectMeshContainer
    (
    SMeshContainer *pmcMesh,
    DWORD dwX, 
    DWORD dwY,
    D3DVIEWPORT9 *pViewport,
    D3DXMATRIX *pmatProjection,
    D3DXMATRIX *pmatView,
    D3DXMATRIX *pmatCombined,
    float *pfDistMin,
    SMeshContainer **ppmcHit,
    DWORD *pdwFaceHit,
    DWORD *pdwVertHit
    )
{
    HRESULT hr = S_OK;
    D3DXVECTOR3 vProjected;
    D3DXVECTOR3 vRayPos;
    D3DXVECTOR3 vRayDirection;
    float fDist= 0.0f;
    BOOL bHit= FALSE;
    BOOL bFound = FALSE;
    float fRayLength= 0.0f;
    DWORD dwFace= 0;
    float fU= 0.0f, fV= 0.0f;
    DWORD iVertexIndex= 0;
    PBYTE pbIndices= NULL;
    LPD3DXBASEMESH pMeshCur= NULL;
    PBYTE       pbVerticesSrc= NULL;
    PBYTE       pbVerticesDest= NULL;
	D3DXMATRIXA16* rgBoneMatrices= NULL;

    if ((pmcMesh != NULL) && (pmcMesh->m_pSkinnedMesh != NULL))  // there's a skinned mesh
    {
        // calculate ray position in world space
        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 0.0f);
        D3DXVec3Unproject(&vRayPos, &vProjected, pViewport, pmatProjection, pmatView, NULL);


        // calculate ray direction in world space
        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 1.0f);
        D3DXVec3Unproject(&vRayDirection, &vProjected, pViewport, pmatProjection, pmatView, NULL);
        vRayDirection -= vRayPos;


        // get the bone count

        DWORD   cBones  = pmcMesh->pSkinInfo->GetNumBones();


        // allocate bone transform array

        rgBoneMatrices  = new D3DXMATRIXA16[cBones];

        if (!rgBoneMatrices)
            return FALSE;


        // set up bone transforms
        
        for (DWORD iBone = 0; iBone < cBones; ++iBone)
        {
            D3DXMatrixMultiply
            (
                &rgBoneMatrices[iBone],                         // output (world)
                &pmcMesh->pBoneOffsetMatrices[iBone],   // bone offset matrix (world)
                pmcMesh->m_pBoneMatrix[iBone]        // bone current transform (world)
            );
        }

        hr= pmcMesh->m_pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
        if (FAILED(hr))
		{
 			pmcMesh->m_pOrigMesh->UnlockVertexBuffer();
			goto e_exit;
		}
		hr= pmcMesh->m_pSkinnedMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);
        if (FAILED(hr))
		{
  			pmcMesh->m_pOrigMesh->UnlockVertexBuffer();
			pmcMesh->m_pSkinnedMesh->UnlockVertexBuffer();
            goto e_exit;
		}
        // generate skinned mesh, use the system memory copy
        hr = pmcMesh->pSkinInfo->UpdateSkinnedMesh(rgBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest);
		pmcMesh->m_pOrigMesh->UnlockVertexBuffer();
		pmcMesh->m_pSkinnedMesh->UnlockVertexBuffer();
		if (FAILED(hr))
        {
            delete[] rgBoneMatrices;
			rgBoneMatrices= NULL;
            goto e_exit;
        }

        // free bone transform array
        delete[] rgBoneMatrices;
		rgBoneMatrices= NULL;

        // perform ray-mesh intersection
        hr = D3DXIntersect(
                pmcMesh->m_pSkinnedMesh,
                &vRayPos, 
                &vRayDirection, 
                &bHit, 
                &dwFace,
                &fU,
                &fV,
                &fDist,
                NULL,
                NULL);
        if (FAILED(hr))
            return FALSE;
        if (bHit)       // intersection found
        {
            // normalize intersection distance
            fDist  /= D3DXVec3Length(&vRayDirection);

            if (fDist < *pfDistMin)     // intersection distance is the smallest seen so far
            {
                // update smallest intersection distance & intersected frame
                *pfDistMin  = fDist;
                *ppmcHit = pmcMesh;

#if 0
                *pdwFaceHit = UNUSED32;
                *pdwVertHit = UNUSED32;
#else
                *pdwFaceHit = dwFace;

                iVertexIndex = GetVertexIndexFromBarycentric(fU, fV);
                pmcMesh->m_pSkinnedMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbIndices);

                if (pmcMesh->m_pSkinnedMesh->GetOptions() & D3DXMESH_32BIT)
                    *pdwVertHit = ((DWORD*)pbIndices)[dwFace * 3 + iVertexIndex];
                else
                    *pdwVertHit = ((WORD*)pbIndices)[dwFace * 3 + iVertexIndex];

                pmcMesh->m_pSkinnedMesh->UnlockIndexBuffer();
#endif
                // indicate that we found a new smallest intersection distance
                bFound      = TRUE;
            }
        }
    }
    else if ((pmcMesh != NULL) && (pmcMesh->ptmDrawMesh != NULL) && (pmcMesh->pSkinInfo == NULL))
    {
        pMeshCur = (pmcMesh->bNPatchMode) ? pmcMesh->pMeshToTesselate : pmcMesh->ptmDrawMesh;

        // calculate two points 
        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 0.0f);
        D3DXVec3Unproject(&vRayPos, &vProjected, pViewport, pmatProjection, pmatView, pmatCombined);

        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 1.0f);
        D3DXVec3Unproject(&vRayDirection, &vProjected, pViewport, pmatProjection, pmatView, pmatCombined);

        // subtract
        vRayDirection -= vRayPos;

#if 1
        LPD3DXBUFFER pbufAllHits;
        DWORD cHits;

        hr = D3DXIntersect(pMeshCur,
            &vRayPos, &vRayDirection, &bHit, &dwFace, &fU, &fV, &fDist, &pbufAllHits, &cHits);
        if (FAILED(hr))
            goto e_exit;

        GXRELEASE(pbufAllHits);
#else
        DWORD iAttrib;
        for (iAttrib = 0; iAttrib < pmcMesh->m_cAttributeGroups; iAttrib++)
        {
            hr = D3DXIntersectSubset(pMeshCur, iAttrib,
                &vRayPos, &vRayDirection, &bHit, &dwFace, &fU, &fV, &fDist, NULL, NULL);
            if (FAILED(hr))
	           goto e_exit;

            if (bHit)
                break;
        }
#endif
        if (bHit)
        {
            // normalize the distance to be based on the length of the ray
            //  used to ignore scale factors in world matrices
            fRayLength = D3DXVec3Length(&vRayDirection);
            fDist /= fRayLength;

            if (fDist < *pfDistMin)
            {
                *pfDistMin = fDist;
                *ppmcHit = pmcMesh;
                *pdwFaceHit = dwFace;

                iVertexIndex = GetVertexIndexFromBarycentric(fU, fV);
                pMeshCur->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbIndices);

                if (pMeshCur->GetOptions() & D3DXMESH_32BIT)
                    *pdwVertHit = ((DWORD*)pbIndices)[dwFace * 3 + iVertexIndex];
                else
                    *pdwVertHit = ((WORD*)pbIndices)[dwFace * 3 + iVertexIndex];

                pMeshCur->UnlockIndexBuffer();

                bFound = TRUE;
            }
        }
    }
e_exit:

    return bFound;
}


BOOL BIntersectFrame
    (
    SFrame *pframeCur,
    DWORD dwX, 
    DWORD dwY,
    D3DVIEWPORT9 *pViewport,
    D3DXMATRIX *pmatProjection,
    D3DXMATRIX *pmatView,
    float *pfDistMin,
    SFrame **ppframeHit,
    SMeshContainer **ppmcHit,
    DWORD *pdwFaceHit,
    DWORD *pdwVertHit
    )
{
    BOOL bFound = FALSE;
    SMeshContainer *pmcCur;

    pmcCur = pframeCur->pmcMesh;

    while (pmcCur != NULL)
    {
        if (BIntersectMeshContainer(pmcCur, dwX, dwY, pViewport, pmatProjection, pmatView, &pframeCur->matCombined, pfDistMin, ppmcHit, pdwFaceHit, pdwVertHit))
        {
            *ppframeHit = pframeCur;
            bFound = TRUE;
        }
        pmcCur = (SMeshContainer*)pmcCur->pNextMeshContainer;
    }

    if (pframeCur->pframeSibling != NULL)
    {
        if (BIntersectFrame(pframeCur->pframeSibling, dwX, dwY, pViewport, pmatProjection, pmatView, pfDistMin, ppframeHit, ppmcHit, pdwFaceHit, pdwVertHit))
        {
            bFound = TRUE;
        }
    }
    
    if (pframeCur->pframeFirstChild != NULL)
    {
        if (BIntersectFrame(pframeCur->pframeFirstChild, dwX, dwY, pViewport, pmatProjection, pmatView, pfDistMin, ppframeHit, ppmcHit, pdwFaceHit, pdwVertHit))
        {
            bFound = TRUE;
        }
    }

    return bFound;
}

void
TrivialData::SelectionChange
    (
    DWORD dwX, 
    DWORD dwY
    )
{
    HRESULT hr = S_OK;
 
	D3DXVECTOR3 vProjected;
    D3DXVECTOR3 vRayPos;
    D3DXVECTOR3 vRayDirection;
    D3DXVECTOR3 vVertHitPos;
    D3DVIEWPORT9 Viewport;
    D3DXMATRIX matProjection, matView, matWorld;
    SDrawElement *pdeCur= NULL;
    SDrawElement *pdeMin= NULL;
    SFrame *pframeMin= NULL;
    SMeshContainer *pmcMin= NULL;
    float fDistMin= 0.0f;
    DWORD dwFaceHit= 0;
    DWORD dwVertHit= 0;

    // account for toolbar
    dwY -= 28;

    m_pDevice->GetViewport(&Viewport);
    m_pDevice->GetTransform(D3DTS_VIEW, &matView);
    m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjection);

    pdeMin = NULL;
    pdeCur = m_pdeHead;
    fDistMin = (float)HUGE_VAL;
    while (pdeCur != NULL)
    {
        // calculate two points 
        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 0.0f);
        D3DXVec3Unproject(&vRayPos, &vProjected, &Viewport, &matProjection, &matView, &pdeCur->pframeRoot->matCombined);

        vProjected = D3DXVECTOR3((float)dwX, (float)dwY, 1.0f);
        D3DXVec3Unproject(&vRayDirection, &vProjected, &Viewport, &matProjection, &matView, &pdeCur->pframeRoot->matCombined);

        // subtract
        vRayDirection -= vRayPos;

        // TODO:  the following check needs to be removed
        //if (1 || D3DXSphereBoundProbe(&pdeCur->vCenter, pdeCur->fRadius, &vRayPos, &vRayDirection))
        {
            if (BIntersectFrame(pdeCur->pframeRoot, dwX, dwY, &Viewport, &matProjection, &matView, &fDistMin, &pframeMin, &pmcMin, &dwFaceHit, &dwVertHit))
            {
                pdeMin = pdeCur;
            }
        }

        pdeCur = pdeCur->pdeNext;
    }

    // if pdeMin != NULL then switch to that mesh, otherwise switch to no selection
    if (pdeMin != NULL)
        SelectDrawElement(pdeMin);

    if ((pdeMin != NULL) && (pframeMin != NULL))
    {        
        SelectFrame(pframeMin, pmcMin);
        SelectFace(dwFaceHit);
        SelectVertex(dwVertHit);
    }

    UpdateSelectionInfo();
}

void
TrivialData::UpdateSelectionInfo()
{
    char szBuf[80];
    PBYTE pbFaces;
    char rgszAdj[3][30];
    DWORD rgdwFace[3];
    DWORD iPoint;
    BOOL b16BitIndex;

    if (m_bVertexSelectionMode)
    {
        if (m_dwVertexSelected != UNUSED32)
        {
            if (m_pmcSelectedMesh->ptmDrawMesh != NULL)
            {
                sprintf(szBuf, "Vertex %d", m_dwVertexSelected);
            }
            else
            {
                sprintf(szBuf, "Vertex %d", m_dwVertexSelected);
            }
        }
        else
        {
            sprintf(szBuf, "No vertex selected");
        }
    }
    else if (m_bFaceSelectionMode)
    {
        if (m_dwFaceSelected != UNUSED32)
        {
            if (m_pmcSelectedMesh->ptmDrawMesh != NULL)
            {
                b16BitIndex = !(m_pmcSelectedMesh->ptmDrawMesh->GetOptions() & D3DXMESH_32BIT);
                m_pmcSelectedMesh->ptmDrawMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbFaces);

                for (iPoint = 0; iPoint < 3; iPoint++)
                {
                    if (b16BitIndex)
                        rgdwFace[iPoint] = ((UINT16*)pbFaces)[m_dwFaceSelected * 3 + iPoint];
                    else
                        rgdwFace[iPoint] = ((UINT32*)pbFaces)[m_dwFaceSelected * 3 + iPoint];

                    if (m_pmcSelectedMesh->rgdwAdjacency != NULL)
                    {
                        if (m_pmcSelectedMesh->rgdwAdjacency[m_dwFaceSelected * 3 + iPoint] == UNUSED32)
                            sprintf(rgszAdj[iPoint], "UNUSED");
                        else
                            sprintf(rgszAdj[iPoint], "%d", m_pmcSelectedMesh->rgdwAdjacency[m_dwFaceSelected * 3 + iPoint]);
                    }
                    else
                    {
                        sprintf(rgszAdj[iPoint], "UNUSED");
                    }
                }

                m_pmcSelectedMesh->ptmDrawMesh->UnlockIndexBuffer();

                sprintf(szBuf, "Face %d  Ind (%d, %d, %d) Adj (%s, %s, %s)\n", m_dwFaceSelected, 
                                            rgdwFace[0], rgdwFace[1], rgdwFace[2],
                                             rgszAdj[0],  rgszAdj[1],  rgszAdj[2]);
            }
            else
            {
                sprintf(szBuf, "Face %d\n", m_dwFaceSelected);
            }
        }
        else
        {
            sprintf(szBuf, "No face selected");
        }
    }
    else
    {
        if ((m_pmcSelectedMesh != NULL) && (m_pmcSelectedMesh->Name != NULL))
        {
            strncpy(szBuf, m_pmcSelectedMesh->Name, sizeof(szBuf));
            szBuf[sizeof(szBuf)-1] = '\0';
        }
        else
        {
            szBuf[0] = '\0';
        }
    }

    SendMessage(m_hwndStatus, SB_SETTEXT , (WPARAM) 0, (LPARAM) szBuf); 
}
