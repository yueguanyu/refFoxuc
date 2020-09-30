/*//////////////////////////////////////////////////////////////////////////////
//
// File: mode.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

HRESULT
InitViews
    (
    SFrame *pframe, 
    SDrawElement *pde
    );

void
TrivialData::ToggleNPatchEdgeMode()
{
    m_bNPatchEdgeMode = !m_bNPatchEdgeMode;
    CheckMenuItem(m_hMenu, ID_NPATCHES_EDGEMODE, m_bNPatchEdgeMode ? MF_CHECKED : MF_UNCHECKED);
}

void
TrivialData::ToggleSolidMode()
{
    if (m_bWireframeMode)
    {
        GXASSERT(!m_bEdgeMode);

        ToggleWireframeMode();
    }

    if (m_bEdgeMode)
    {
        GXASSERT(!m_bWireframeMode);

        ToggleEdgeMode();
    }
}

void
TrivialData::ToggleEdgeMode()
{
    DWORD dwOption;
    if (m_bWireframeMode)
    {
        GXASSERT(!m_bEdgeMode);

        ToggleWireframeMode();
    }

    m_bEdgeMode = !m_bEdgeMode;
    dwOption = m_bEdgeMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_EDGEMODE, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_EDGEMODE, MAKELONG(m_bEdgeMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_SOLID, MAKELONG((!m_bEdgeMode && !m_bWireframeMode), 0));
}

void
TrivialData::ToggleNoSelectionMode()
{
    DWORD dwOption;

    // if Vertex Selection mode is on, disable it
    if (m_bVertexSelectionMode)
    {
        ToggleVertexSelectionMode();
    }

    // if Face selection mode is on, disable it
    if (m_bFaceSelectionMode)
    {
        ToggleFaceSelectionMode();
    }

    if (m_bShowMeshSelectionMode)
    {
        ToggleMeshSelectionMode();
    }
}

void
TrivialData::ToggleMeshSelectionMode()
{
    DWORD dwOption;

    m_bShowMeshSelectionMode = !m_bShowMeshSelectionMode;
    dwOption = m_bShowMeshSelectionMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_OPTIONS_MESHSELECTION, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_MESHSELECTION, MAKELONG(m_bShowMeshSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_NOSELECTION, MAKELONG((!m_bFaceSelectionMode && !m_bVertexSelectionMode && !m_bShowMeshSelectionMode), 0));

    UpdateSelectionInfo();
}

void
TrivialData::ToggleFaceSelectionMode()
{
    DWORD dwOption;
#if 0
    // if Vertex Selection mode is on, disable it
    if (m_bVertexSelectionMode)
    {
        GXASSERT(!m_bFaceSelectionMode);

        ToggleVertexSelectionMode();
    }
#endif

    m_bFaceSelectionMode = !m_bFaceSelectionMode;
    dwOption = m_bFaceSelectionMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_OPTIONS_FACESELECTION, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_FACESELECTION, MAKELONG(m_bFaceSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_NOSELECTION, MAKELONG((!m_bFaceSelectionMode && !m_bVertexSelectionMode && !m_bShowMeshSelectionMode), 0));

    UpdateSelectionInfo();
}

void
TrivialData::ToggleVertexSelectionMode()
{
    DWORD dwOption;

#if 0
    // if Vertex Selection mode is on, disable it
    if (m_bFaceSelectionMode)
    {
        GXASSERT(!m_bVertexSelectionMode);

        ToggleFaceSelectionMode();
    }
#endif

    m_bVertexSelectionMode = !m_bVertexSelectionMode;
    dwOption = m_bVertexSelectionMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_OPTIONS_VERTEXSELECTION, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_VERTEXSELECTION, MAKELONG(m_bVertexSelectionMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_OPTIONS_NOSELECTION, MAKELONG((!m_bFaceSelectionMode && !m_bVertexSelectionMode && !m_bShowMeshSelectionMode), 0));

    UpdateSelectionInfo();
}

void
TrivialData::ToggleStripMode()
{
    DWORD dwOption;

    m_bStripMode = !m_bStripMode;
    dwOption = m_bStripMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_STRIPMODE, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_STRIPMODE, MAKELONG(m_bStripMode, 0));
}

void
TrivialData::ToggleAdjacencyMode()
{
    DWORD dwOption;

    m_bAdjacencyMode = !m_bAdjacencyMode;
    dwOption = m_bAdjacencyMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_ADJACENCYMODE, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_ADJACENCYMODE, MAKELONG(m_bAdjacencyMode, 0));
}

void
TrivialData::ToggleCreaseMode()
{
    DWORD dwOption;

    m_bCreaseMode = !m_bCreaseMode;
    dwOption = m_bCreaseMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_CREASEMODE, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_CREASEMODE, MAKELONG(m_bCreaseMode, 0));
}

void
TrivialData::ToggleNormalsMode()
{
    DWORD dwOption;

    m_bNormalsMode = !m_bNormalsMode;
    dwOption = m_bNormalsMode ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_SHOWNORMALS, dwOption);
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_SHOWNORMALS, MAKELONG(m_bNormalsMode, 0));
}

void
TrivialData::ToggleShowTexCoord(DWORD iTexCoord)
{
    DWORD dwOption;
    HRESULT hr;

    // if currently showing, stop showing
    if (m_dwTexCoordsShown & (1<<iTexCoord))
    {
        m_dwTexCoordsShown &= ~(1<<iTexCoord);
    }
    else
    {
        m_dwTexCoordsShown |= (1<<iTexCoord);
    }

    // if there is a model, and we haven't calculated the texture coord view yet
    if ((m_pdeHead != NULL) && !(m_pdeHead->dwTexCoordsCalculated & (1<<iTexCoord)))
    {
        // update the tex coord views to keep current
        m_pdeHead->dwTexCoordsCalculated |= (1<<iTexCoord);

        // calculate the new texture coord views
        hr = InitViews(m_pdeHead->pframeRoot, m_pdeHead);
        if (FAILED(hr))
            return;
    }

    dwOption = (m_dwTexCoordsShown & (1<<iTexCoord)) ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_D3D_SHOWTEX0+iTexCoord, dwOption);
}

void
TrivialData::TogglePlaybackSpeed()
{
    DWORD dwOption;

    if (m_fTicksPerSecond == 4800.0f)
        m_fTicksPerSecond = 30.0f;
    else
        m_fTicksPerSecond = 4800.0f;

    dwOption = (m_fTicksPerSecond == 4800.0f) ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(m_hMenu, ID_VIEW_PLAYBACKSPEED, dwOption);
}

void
TrivialData::ChangeSkinningMode(METHOD method)
{
    m_method = method;

    // software skinning changes a number of things, so call the generic menu stuff
    SetupMenu();
}



void
TrivialData::ToggleWireframeMode()
{
    DWORD dw;
    DWORD dwOption;

    // if edge mode enabled, then disable it
    if (m_bEdgeMode)
    {
        GXASSERT(!m_bWireframeMode);

        ToggleEdgeMode();
    }

    if ( m_bWireframeMode )
    {
        m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        dwOption = MF_UNCHECKED;
        m_bWireframeMode = false;
    }
    else
    {
        m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        dwOption = MF_CHECKED;
        m_bWireframeMode = true;
    }

    CheckMenuItem(m_hMenu, ID_D3D_WIREFRAME, dwOption);

    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_WIREFRAME, MAKELONG(m_bWireframeMode, 0));
    SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_D3D_SOLID, MAKELONG((!m_bEdgeMode && !m_bWireframeMode), 0));
}

void
TrivialData::ToggleLightMode()
{
    DWORD dw;
    DWORD dwOption;

    m_bLighting = !m_bLighting;
    dwOption = m_bLighting ? MF_CHECKED : MF_UNCHECKED;

    m_pDevice->SetRenderState( D3DRS_LIGHTING, m_bLighting );
    CheckMenuItem(m_hMenu, ID_D3D_LIGHTING, dwOption);
}

void
TrivialData::ToggleCullMode()
{
    DWORD dw;
    DWORD dwOption;

    if ( D3DCULL_NONE == m_dwCullMode )
    {
        m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
        m_dwCullMode = D3DCULL_CCW;
        dwOption = MF_CHECKED;
    }
    else
    {
        m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        m_dwCullMode = D3DCULL_NONE;
        dwOption = MF_UNCHECKED;
    }

    CheckMenuItem(m_hMenu, ID_D3D_CULL, dwOption);
}

void
TrivialData::ToggleTextureMode()
{
    DWORD dwOption;

    // toggle the mode
    m_bTextureMode = !m_bTextureMode;

    dwOption = m_bTextureMode ? MF_CHECKED : MF_UNCHECKED;
    CheckMenuItem(m_hMenu, ID_OPTIONS_TEXTURE, dwOption);
}

void 
TrivialData::ChangeAnimMode(BOOL bPause)
{
    m_bAnimPaused = bPause;

    // only update buttons if 
    if ((m_pdeHead != NULL) && (m_pdeHead->m_pAnimMixer != NULL))
    {
        EnableMenuItem(m_hMenu, ID_VIEW_PLAYANIM, m_bAnimPaused ? MF_ENABLED : MF_GRAYED);
        EnableMenuItem(m_hMenu, ID_VIEW_PAUSEANIM, !m_bAnimPaused ? MF_ENABLED : MF_GRAYED);
        CheckMenuItem(m_hMenu, ID_VIEW_PLAYANIM, !m_bAnimPaused ? MF_CHECKED : MF_UNCHECKED);
        CheckMenuItem(m_hMenu, ID_VIEW_PAUSEANIM, m_bAnimPaused ? MF_CHECKED : MF_UNCHECKED);
        SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_VIEW_PLAYANIM, MAKELONG(!m_bAnimPaused, 0));
        SendMessage(m_hwndToolbar, TB_CHECKBUTTON, ID_VIEW_PAUSEANIM, MAKELONG(m_bAnimPaused, 0));
    }
}
