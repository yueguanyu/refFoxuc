// RenderView.cpp : implementation of the CRenderView class
//

#include "stdafx.h"
#include "EffectEdit.h"

#include "EffectDoc.h"
#include "UIElements.h"
#include "RenderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Custom vertex type for the flat-texture background
struct FLATBACKGROUNDVERTEX
{
    float x,y,z,w;
    float u,v;

    static const DWORD FVF;
};
const DWORD FLATBACKGROUNDVERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;


// Custom vertex type for the cubemap-texture background
struct CUBEBACKGROUNDVERTEX
{ 
    float x,y,z; 
    float u,v,w; 

    static const DWORD FVF;
};
const DWORD CUBEBACKGROUNDVERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

/////////////////////////////////////////////////////////////////////////////
// CRenderView

static CRenderView* s_pRenderView = NULL;


IMPLEMENT_DYNCREATE(CRenderView, CFormView)

BEGIN_MESSAGE_MAP(CRenderView, CFormView)
    //{{AFX_MSG_MAP(CRenderView)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_COMMAND(ID_VIEW_RENDER, OnRender)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderView construction/destruction

CRenderView::CRenderView()
    : CFormView(CRenderView::IDD)
{
    //{{AFX_DATA_INIT(CRenderView)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // TODO: add construction code here
    m_bShowStats = TRUE;
    m_pFont = NULL;
    m_pEffect = NULL;
    m_pTextSprite = NULL;
    m_pBackgroundTexture = NULL;
    m_pEnvironmentTexture = NULL;
    m_dwBackgroundColor = D3DCOLOR_ARGB(0, 0, 0, 255);
    m_pVBBackground = NULL;
    m_iTechnique = -1;
    m_iPass = -1;
    m_strMesh[0] = 0;
    m_pMesh = NULL;
    m_pbufMaterials = NULL;
    m_dwNumMaterials = 0;
    m_ppTextures = NULL;
    m_strBackgroundTexture[0] = 0;
    m_vObjectCenter = D3DXVECTOR3(0, 0, 0);
    m_fObjectRadius = 1.0f;
    m_pMeshSphere = NULL;
    m_bWireframe = FALSE;
    m_bNoTextures = FALSE;
    m_bSelectedPassOnly = FALSE;
    m_bUpToSelectedPassOnly = FALSE;
    m_bUINeedsReset = TRUE;
}

CRenderView::~CRenderView()
{
    SAFE_RELEASE( m_pEffect );
    DXUTShutdown();
}

void CRenderView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRenderView)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BOOL CRenderView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CFormView::PreCreateWindow(cs);
}


// DXUT Callbacks
void CRenderView::OnFrameMoveCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    if( s_pRenderView )
        s_pRenderView->OnFrameMove( pd3dDevice, fTime, fElapsedTime );
}


void CRenderView::OnFrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    if( s_pRenderView )
        s_pRenderView->OnFrameRender( pd3dDevice, fTime, fElapsedTime );
}


HRESULT CRenderView::OnCreateDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    if( s_pRenderView )
        return s_pRenderView->OnCreateDevice( pd3dDevice, pBackBufferSurfaceDesc );
    return S_OK;
}


HRESULT CRenderView::OnResetDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    if( s_pRenderView )
        return s_pRenderView->OnResetDevice( pd3dDevice, pBackBufferSurfaceDesc );
    return S_OK;
}


void CRenderView::OnLostDeviceCB()
{
    if( s_pRenderView )
        s_pRenderView->OnLostDevice();
}


void CRenderView::OnDestroyDeviceCB()
{
    if( s_pRenderView )
        s_pRenderView->OnDestroyDevice();
}


//-----------------------------------------------------------------------------
// Name: FullScreenWndProc()
// Desc: The WndProc funtion used when the app is in fullscreen mode. This is
//       needed simply to trap the ESC key.
//-----------------------------------------------------------------------------
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if( msg == WM_CREATE )
    {
        s_pRenderView = (CRenderView*)((CREATESTRUCT*)lParam)->lpCreateParams;
    }
    else if( msg == WM_CLOSE )
    {
        // User wants to exit, so go back to windowed mode and exit for real
        s_pRenderView->OnToggleFullscreen();
        s_pRenderView->GetActiveWindow()->PostMessage( WM_CLOSE, 0, 0 );
        //g_App.GetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
    }
    else if( msg == WM_SETCURSOR )
    {
        SetCursor( NULL );
    }
    else if( ( msg == WM_KEYUP && wParam == VK_ESCAPE ) ||
             ( msg == WM_SYSCHAR && wParam == VK_RETURN ) )
    {
        // User wants to leave fullscreen mode
        s_pRenderView->OnToggleFullscreen();
        return 0;
    }
    else if ( msg == WM_LBUTTONDOWN ||
              msg == WM_LBUTTONUP ||
              msg == WM_RBUTTONDOWN ||
              msg == WM_RBUTTONUP ||
              msg == WM_MOUSEMOVE )
    {
        // Map the coordinates from fullscreen window to
        // RenderView window.
        POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
        ::MapWindowPoints( hWnd, s_pRenderView->GetSafeHwnd(), &pt, 1 );
        lParam = MAKELPARAM( pt.x, pt.y );

        // forward mouse messages to the render view window
        s_pRenderView->PostMessage( msg, wParam, lParam );
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


void CRenderView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();

    // Save static reference to the render window
    m_hwndRenderWindow = GetDlgItem(IDC_RENDERWINDOW)->GetSafeHwnd();

    // Register a class for a fullscreen window
    WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, FullScreenWndProc, 0, 0, NULL,
                          NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                          _T("Fullscreen Window") };
    RegisterClass( &wndClass );

    // We create the fullscreen window (not visible) at startup, so it can
    // be the focus window.  The focus window can only be set at CreateDevice
    // time, not in a Reset, so ToggleFullscreen wouldn't work unless we have
    // already set up the fullscreen focus window.
    m_hwndRenderFullScreen = CreateWindow( _T("Fullscreen Window"), NULL,
                                           WS_POPUP, 0, 0, 100, 100,
                                           GetTopLevelParent()->GetSafeHwnd(), 0L, NULL, this );

    // If we haven't initialized the D3D framework, do so now
    if( !DXUTGetD3DDevice() )
    {
        CRect rc;
        ::GetClientRect( m_hwndRenderWindow, &rc );
        if( rc.Width() == 0 || rc.Height() == 0 )
        {
            MessageBox( TEXT("The render view must be visible when EffectEdit starts.  Please change the window size or splitters and start EffectEdit again."),
                TEXT("EffectEdit") );
        }
        else
        {
            // Set up the DXUT callbacks
            DXUTSetCallbackDeviceCreated( OnCreateDeviceCB );
            DXUTSetCallbackDeviceReset( OnResetDeviceCB );
            DXUTSetCallbackDeviceLost( OnLostDeviceCB );
            DXUTSetCallbackDeviceDestroyed( OnDestroyDeviceCB );
            DXUTSetCallbackFrameRender( OnFrameRenderCB );
            DXUTSetCallbackFrameMove( OnFrameMoveCB );

            // Show the cursor and clip it when in full screen
            DXUTSetCursorSettings( true, true );

            DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

            // Note that for the MFC samples, the device window and focus window
            // are not the same.
            DXUTSetWindow( m_hwndRenderFullScreen, m_hwndRenderFullScreen, m_hwndRenderWindow, false );
            ::GetClientRect( m_hwndRenderWindow, &m_rcWindowClient );
            HRESULT hr = DXUTCreateDevice( D3DADAPTER_DEFAULT, true, m_rcWindowClient.right - m_rcWindowClient.left,
                                           m_rcWindowClient.bottom - m_rcWindowClient.top, NULL, NULL );
            if( FAILED( hr ) )
            {
                // If D3D is not functional, do not continue.  End the application now.
                CFrameWnd *pTop = this->GetTopLevelFrame();
                if( pTop )
                    pTop->PostMessage( WM_CLOSE, 0, 0 );
            }
        }
    }
    GetDocument()->Compile();
}

/////////////////////////////////////////////////////////////////////////////
// CRenderView diagnostics

#ifdef _DEBUG
void CRenderView::AssertValid() const
{
    CFormView::AssertValid();
}


void CRenderView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}


CEffectDoc* CRenderView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectDoc)));
    return (CEffectDoc*)m_pDocument;
}
#endif //_DEBUG


void CRenderView::OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
     // Setup world matrix
    D3DXMatrixTranslation( &m_matWorld, -m_vObjectCenter.x,
                                      -m_vObjectCenter.y,
                                      -m_vObjectCenter.z );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, m_ArcBallMesh.GetRotationMatrix() );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, m_ArcBallMesh.GetTranslationMatrix() );
    pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    D3DXVECTOR3 vFrom( 0, 0, -2*m_fObjectRadius );
    D3DXVECTOR3 vAt( 0, 0, 0 );
    D3DXVECTOR3 vUp( 0, 1, 0 );
    D3DXMatrixLookAtLH( &m_matView, &vFrom, &vAt, &vUp );
    D3DXMatrixMultiply( &m_matView, m_ArcBallView.GetRotationMatrix(), &m_matView );
    D3DXMatrixMultiply( &m_matView, &m_matView, m_ArcBallView.GetTranslationMatrix() );
    pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    
    m_UIElements.SetMatView( &m_matView );
    m_UIElements.SetObjectRadius( m_fObjectRadius );
}

// Vertex data for a cube that is used to draw the environment
//   3D Positon and 3D texture coordinate for lookup in cube map
static CUBEBACKGROUNDVERTEX EnvironmentVerts[] =
{
    { 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    { 1.0f,  1.0f,-1.0f, 1.0f, 1.0f,-1.0f},
    { 1.0f, -1.0f, 1.0f, 1.0f,-1.0f, 1.0f},
    { 1.0f, -1.0f,-1.0f, 1.0f,-1.0f,-1.0f},
    {-1.0f,  1.0f, 1.0f,-1.0f, 1.0f, 1.0f},
    {-1.0f,  1.0f,-1.0f,-1.0f, 1.0f,-1.0f},
    {-1.0f, -1.0f, 1.0f,-1.0f,-1.0f, 1.0f},
    {-1.0f, -1.0f,-1.0f,-1.0f,-1.0f,-1.0f},
};

//  index data for the cube to draw the environment with
static WORD EnvironmentIndices[] =
{
    0, 1, 3,  0, 3, 2,   4, 7, 5,  4, 6, 7,
    0, 2, 6,  0, 6, 4,   1, 7, 3,  1, 5, 7,
    0, 5, 1,  0, 4, 5,   2, 3, 7,  2, 7, 6
};

void CRenderView::OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;

    // Clear the viewport
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       m_dwBackgroundColor, 1.0f, 0L );
    if( SUCCEEDED( hr = pd3dDevice->BeginScene() ) )
    {
        if( m_pEffect != NULL )
        {
            if (m_pEnvironmentTexture != NULL)
            {
                // set the rotation of the environment
                // Note: EffectEdit always shows the environment cubemap aligned with world
                // space (world matrix is identity), but some apps might want to support
                // a rotated cubemap.
                D3DXMATRIXA16 matWorldEnvMap;
                D3DXMatrixIdentity(&matWorldEnvMap);
                pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldEnvMap);
                pd3dDevice->SetTransform( D3DTS_VIEW, m_ArcBallView.GetRotationMatrix() );

                // Render background image
                pd3dDevice->SetTexture(0, m_pEnvironmentTexture);
                pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
                pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
                pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

                pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

                pd3dDevice->SetFVF( CUBEBACKGROUNDVERTEX::FVF);
                pd3dDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 12, EnvironmentIndices, 
                    D3DFMT_INDEX16, EnvironmentVerts, sizeof(CUBEBACKGROUNDVERTEX));
                
                // restore the device state
                pd3dDevice->SetTexture(0, NULL);
                pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE);
                pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
                pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
            }
            else if( m_pBackgroundTexture != NULL )
            {
                // Render background image
                pd3dDevice->SetTexture(0, m_pBackgroundTexture);
                pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
                pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
                pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

                pd3dDevice->SetFVF( FLATBACKGROUNDVERTEX::FVF);
                pd3dDevice->SetStreamSource( 0, m_pVBBackground, 0, 6*sizeof(float) );
                pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
                
                pd3dDevice->SetTexture(0, NULL);
                pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            }

            // Render the mesh with the current effect
            if( m_pEffect != NULL )
            {
                if( m_MatWorldEffectHandle != NULL )
                    m_pEffect->SetMatrix( m_MatWorldEffectHandle, &m_matWorld );

                if( m_MatViewEffectHandle != NULL )
                    m_pEffect->SetMatrix( m_MatViewEffectHandle, &m_matView );

                if( m_MatProjEffectHandle != NULL )
                    m_pEffect->SetMatrix( m_MatProjEffectHandle, &m_matProj );

                if( m_MatWorldViewEffectHandle != NULL )
                {
                    D3DXMATRIX matWorldView = m_matWorld * m_matView;
                    m_pEffect->SetMatrix( m_MatWorldViewEffectHandle, &matWorldView );
                }

                if( m_MatViewProjEffectHandle != NULL )
                {
                    D3DXMATRIX matViewProj = m_matView * m_matProj;
                    m_pEffect->SetMatrix( m_MatViewProjEffectHandle, &matViewProj );
                }

                if( m_MatWorldViewProjEffectHandle != NULL )
                {
                    D3DXMATRIX matWorldViewProj = m_matWorld * m_matView * m_matProj;
                    m_pEffect->SetMatrix( m_MatWorldViewProjEffectHandle, &matWorldViewProj );
                }

                if( m_VecCameraPosEffectHandle != NULL )
                {
                    D3DXMATRIXA16 matViewInv;
                    D3DXMatrixInverse( &matViewInv, NULL, &m_matView );
                    D3DXVECTOR4 vecPosition( matViewInv._41, matViewInv._42, matViewInv._43, 1.0f );
                    m_pEffect->SetVector( m_VecCameraPosEffectHandle, &vecPosition );
                }

                if( m_TimeEffectHandle != NULL )
                    m_pEffect->SetFloat( m_TimeEffectHandle, float(fTime) );

                if( m_MeshRadiusEffectHandle != NULL )
                    m_pEffect->SetFloat( m_MeshRadiusEffectHandle, m_fObjectRadius );

                m_UIElements.SetEffectParameters( m_pEffect );

                D3DCAPS9 d3dCaps;
                pd3dDevice->GetDeviceCaps( &d3dCaps );

                UINT numPasses;
                hr = m_pEffect->Begin( &numPasses, 0 );

                if( SUCCEEDED( hr ) )
                {
                    for( UINT iPass = 0; iPass < numPasses; iPass++ )
                    {
                        if( m_bSelectedPassOnly )
                            iPass = m_iPass;

                        hr = m_pEffect->BeginPass( iPass );
                        
                        if( m_bWireframe )
                            pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
                        else
                            pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
                        
                        if( m_bNoTextures )
                        {
                            for( UINT iStage = 0; iStage < d3dCaps.MaxTextureBlendStages; iStage++ )
                                pd3dDevice->SetTexture( iStage, NULL );
                        }

                        if( m_pMesh == NULL )
                        {
                            m_pMeshSphere->DrawSubset( 0 );
                        }
                        else
                        {
                            for( UINT i = 0; i < m_dwNumMaterials; i++ )
                            {
                                if( m_MaterialAmbientEffectHandle != NULL )
                                    m_pEffect->SetVector( m_MaterialAmbientEffectHandle, (LPD3DXVECTOR4)&m_pMaterials[i].MatD3D.Ambient );
                                if( m_MaterialDiffuseEffectHandle != NULL )
                                    m_pEffect->SetVector( m_MaterialDiffuseEffectHandle, (LPD3DXVECTOR4)&m_pMaterials[i].MatD3D.Diffuse );
                                if( m_MaterialSpecularEffectHandle != NULL )
                                    m_pEffect->SetVector( m_MaterialSpecularEffectHandle, (LPD3DXVECTOR4)&m_pMaterials[i].MatD3D.Specular );
                                if( m_MaterialSpecularPowerEffectHandle != NULL )
                                    m_pEffect->SetFloat( m_MaterialSpecularPowerEffectHandle, m_pMaterials[i].MatD3D.Power );
                                m_pEffect->CommitChanges();                           
                                m_pMesh->DrawSubset( i );
                            }
                        }

                        hr = m_pEffect->EndPass();

                        if( m_bSelectedPassOnly || ( m_bUpToSelectedPassOnly && (int)iPass == m_iPass ) )
                            break;
                    }

                    hr = m_pEffect->End();
                }

                // Render the UI objects
                m_UIElements.Render( pd3dDevice );

            }

        }

        if( m_bShowStats )
        {
            CDXUTTextHelper txtHelper( m_pFont, m_pTextSprite, 15 );

            txtHelper.Begin();
            // Output statistics
            txtHelper.SetInsertionPos( 5, 5 );
            txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
            txtHelper.DrawTextLine( DXUTGetFrameStats() );
            txtHelper.DrawTextLine( DXUTGetDeviceStats() );
            if( m_pEffect == NULL )
            {
                txtHelper.SetForegroundColor( D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f ) );
                txtHelper.DrawTextLine( L"Error in effect file" );
            }

            if( !m_bTechniqueValidValid )
            {
                m_bTechniqueValid = SUCCEEDED( m_pEffect->ValidateTechnique( m_pEffect->GetCurrentTechnique() ) );
                m_bTechniqueValidValid = TRUE;
            }

            if( !m_bTechniqueValid )
            {
                txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
                txtHelper.SetInsertionPos( 2, 45 );
                txtHelper.DrawTextLine( L"Warning: technique not valid with current device settings" );
            }

            if( !((CEffectEditApp*)AfxGetApp())->AppActivated() )
            {
                txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
                txtHelper.SetInsertionPos( 2, 60 );
                txtHelper.DrawTextLine( L"Note: sleeping between frames because app is not frontmost, fps may be lower" );
            }

            txtHelper.End();
        }
        // End the scene.
        pd3dDevice->EndScene();
    }
}


/////////////////////////////////////////////////////////////////////////////
// CRenderView message handlers

void CRenderView::OnDestroy()
{
    ::DestroyWindow( m_hwndRenderFullScreen );
}


void CRenderView::OnSize(UINT nType, int cx, int cy) 
{
    SetScrollSizes( MM_TEXT, CSize(cx, cy) );

    CFormView::OnSize(nType, cx, cy);
    
    CWnd* pGroup = GetDlgItem(IDC_GROUPBOX);
    CWnd* pRenderWnd = GetDlgItem(IDC_RENDERWINDOW);
    CWnd* pRenderText = GetDlgItem(IDC_RENDERTEXT);

    if( pGroup != NULL && 
        pRenderWnd != NULL &&
        pRenderText != NULL )
    {
        CRect rc;
        INT textHeight;
        pRenderText->GetClientRect(&rc);
        textHeight = rc.Height();

        pRenderText->SetWindowPos(NULL, 0, cy - textHeight - 5, cx, textHeight, SWP_NOZORDER);
        pRenderText->Invalidate();
        pGroup->SetWindowPos(NULL, 7, 2, cx - 7 - 4, cy - textHeight - 2 - 10, SWP_NOZORDER);

        pGroup->GetClientRect(&rc);
        pGroup->MapWindowPoints(this, &rc);


        RECT rcClientOld = m_rcWindowClient;
        
        rc.InflateRect( -10, -17, -10, -10 );
        pRenderWnd->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);

        if( DXUTGetD3DDevice() != NULL )
        {
            pRenderWnd->GetClientRect( &m_rcWindowClient );
            DXUTStaticWndProc( GetSafeHwnd(), WM_SIZE, nType, MAKELPARAM( cx, cy ) );
            DXUTStaticWndProc( GetSafeHwnd(), WM_EXITSIZEMOVE, 0, 0 );
        }
    }
}


void CRenderView::OnRender() 
{
    if( DXUTGetD3DDevice() != NULL )
        DXUTRender3DEnvironment();
}


HRESULT CRenderView::OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    // Initialize the font
    if( FAILED( hr = D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                                     OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                     _T("Arial"), &m_pFont ) ) )
        return hr;

    if( FAILED( hr = D3DXCreateSphere( pd3dDevice, 1.0f, 40, 40, &m_pMeshSphere, NULL ) ) )
        return hr;

    if( FAILED( hr = m_UIElements.OnCreateDevice( pd3dDevice ) ) )
        return hr;

    if( GetDocument()->GetCode().GetLength() > 0 )
        GetDocument()->Compile( true );

    return S_OK;
}

void CRenderView::OnDestroyDevice()
{
    SAFE_RELEASE( m_pFont );
    SAFE_RELEASE( m_pBackgroundTexture );
    SAFE_RELEASE( m_pEnvironmentTexture );
    SAFE_RELEASE( m_pMeshSphere );
    m_UIElements.OnDestroyDevice();
    UnloadMesh();
    SAFE_RELEASE( m_pEffect );
}


HRESULT CRenderView::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    if( FAILED( hr = m_pFont->OnResetDevice() ) )
        return hr;

    // Create a sprite to help batch calls when drawing many lines of text
    if( FAILED( hr = D3DXCreateSprite( pd3dDevice, &m_pTextSprite ) ) )
        return hr;

    if( FAILED( hr = m_UIElements.OnResetDevice( pd3dDevice ) ) )
        return hr;

    // Build background image vertex buffer
    if ( FAILED( hr = pd3dDevice->CreateVertexBuffer( sizeof(FLATBACKGROUNDVERTEX)*4, 0,
        FLATBACKGROUNDVERTEX::FVF, D3DPOOL_MANAGED, &m_pVBBackground, NULL ) ) )
    {
        return hr;
    }

    //set up a set of points which represents the screen
    static struct { float x,y,z,w; float u,v; } s_Verts[] =
    {
        {750.0f,  -0.5f, 0.5f, 1.0f, 1,0},
        {750.0f, 750.0f, 0.5f, 1.0f, 1,1},
        { -0.5f,  -0.5f, 0.5f, 1.0f, 0,0},
        { -0.5f, 750.0f, 0.5f, 1.0f, 0,1},
    };

    s_Verts[0].x = (float)pBackBufferSurfaceDesc->Width - 0.5f;
    s_Verts[1].x = (float)pBackBufferSurfaceDesc->Width - 0.5f;
    s_Verts[1].y = (float)pBackBufferSurfaceDesc->Height - 0.5f;
    s_Verts[3].y = (float)pBackBufferSurfaceDesc->Height - 0.5f; 
 
    //copy them into the buffer
    void *pVerts;
    if ( FAILED(hr = m_pVBBackground->Lock( 0, sizeof(s_Verts), (void**)&pVerts, 0 )) )
        return hr;

    memcpy( pVerts, s_Verts, sizeof(s_Verts) );

    m_pVBBackground->Unlock();

    if( m_pEffect != NULL )
        m_pEffect->OnResetDevice();

    // Setup the arcball parameters
    m_ArcBallMesh.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f );
    m_ArcBallView.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f );

    // Setup the projection matrix
    FLOAT      fAspect = (FLOAT)pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 
                                m_fObjectRadius/64.0f, m_fObjectRadius*64.0f );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    m_UIElements.SetInfo( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, &m_matProj );

    return S_OK;
}


void CRenderView::OnLostDevice()
{
    m_pFont->OnLostDevice();

    m_UIElements.OnLostDevice();

    SAFE_RELEASE( m_pVBBackground );

    if( m_pEffect != NULL )
        m_pEffect->OnLostDevice();

    SAFE_RELEASE( m_pTextSprite );
}


// Helper class for D3DXCreateEffect() to be able to load include files
class CIncludeManager : public ID3DXInclude
{
protected:
    CRenderView* m_pView;

public:
    CIncludeManager( CRenderView* pView ) { m_pView = pView; }
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(LPCVOID pData);
};


HRESULT CIncludeManager::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
    CFile f;
    WCHAR strFile[MAX_PATH];
    WCHAR strFileFull[MAX_PATH];

    MultiByteToWideChar( CP_ACP, 0, pName, -1, strFile, MAX_PATH );

    if( FAILED( m_pView->EE_FindMediaFileCch( strFileFull, MAX_PATH, strFile ) ) )
        return E_FAIL;

    if( !f.Open( strFileFull, CFile::modeRead ) ) 
        return E_FAIL;

    UINT size = (UINT)f.GetLength();

    BYTE* pData = new BYTE[size];
    if( pData == NULL )
        return E_OUTOFMEMORY;

    f.Read( pData, size );

    *ppData = pData;
    *pBytes = size;

    return S_OK;
}


HRESULT CIncludeManager::Close(LPCVOID pData)
{
    BYTE* pData2 = (BYTE*)pData;
    SAFE_DELETE_ARRAY( pData2 );
    return S_OK;
}


HRESULT CRenderView::CompileEffect(CString strEffect, BOOL bUseShaderOptimizations, BOOL bFromFile, CString &strErrors, 
                                   CStringList& techniqueNameList, int* piTechnique, BOOL* pbTryLater)
{
    HRESULT hr = S_OK;
    LPD3DXBUFFER pBufferErrors = NULL;
    CIncludeManager includeManager(this);

    *pbTryLater = FALSE;

    if( DXUTGetD3DDevice() == NULL )
    {
        *pbTryLater = TRUE;
        return E_FAIL;
    }

    SAFE_RELEASE( m_pEffect );
    m_MatWorldEffectHandle = NULL;
    m_MatViewEffectHandle = NULL;
    m_MatProjEffectHandle = NULL;
    m_MatWorldViewEffectHandle = NULL;
    m_MatViewProjEffectHandle = NULL;
    m_MatWorldViewProjEffectHandle = NULL;
    m_VecCameraPosEffectHandle = NULL;
    m_MaterialAmbientEffectHandle = NULL;
    m_MaterialDiffuseEffectHandle = NULL;
    m_MaterialSpecularEffectHandle = NULL;
    m_MaterialSpecularPowerEffectHandle = NULL;
    m_TimeEffectHandle = NULL;
    m_MeshRadiusEffectHandle = NULL;

    if( m_bUINeedsReset )
    {
        m_UIElements.DeleteAllElements();
        ResetCamera();
        UnloadMesh();
        m_fObjectRadius = 1.0f;
        m_vObjectCenter.x = 0;
        m_vObjectCenter.y = 0;
        m_vObjectCenter.z = 0;
        // Adjust near/far clip planes of projection matrix based on the object radius
        const D3DSURFACE_DESC *pBackBufferDesc = DXUTGetBackBufferSurfaceDesc();
        FLOAT fAspect = (FLOAT)pBackBufferDesc->Width / (FLOAT)pBackBufferDesc->Height;
        D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 
                                    m_fObjectRadius/64.0f, m_fObjectRadius*64.0f );
        DXUTGetD3DDevice()->SetTransform( D3DTS_PROJECTION, &m_matProj );
        m_bUINeedsReset = FALSE;
    }
    else
    {
        // Delete all effect handles inside UI elements, but keep their other state
        m_UIElements.DeleteEffectObjects();
    }

    techniqueNameList.RemoveAll();
    GetDlgItem(IDC_RENDERTEXT)->SetWindowText( TEXT("(nothing to render)") );

    DWORD dwFlags = D3DXSHADER_DEBUG;
    if( !bUseShaderOptimizations )
        dwFlags |= D3DXSHADER_SKIPOPTIMIZATION;

    if( !bFromFile )
    {
        UINT EffectLength = strEffect.GetLength();
        char* strEffectA = new char[EffectLength + 1];
        if( strEffectA == NULL )
            return E_OUTOFMEMORY;
        WideCharToMultiByte( CP_ACP, 0, strEffect, -1, strEffectA, EffectLength + 1, NULL, NULL );
        
        hr = D3DXCreateEffect( DXUTGetD3DDevice(), strEffectA, EffectLength, NULL, 
                               &includeManager, dwFlags, NULL, &m_pEffect, &pBufferErrors );

        delete[] strEffectA;
    }
    else
    {
        hr = D3DXCreateEffectFromFile( DXUTGetD3DDevice(), strEffect, NULL, &includeManager, dwFlags, NULL, &m_pEffect, &pBufferErrors );
    }

    if( pBufferErrors != NULL )
        strErrors = (CHAR*)pBufferErrors->GetBufferPointer();
    else
        strErrors.Empty();

    SAFE_RELEASE( pBufferErrors );

    if( m_pEffect != NULL )
    {
        m_pEffect->GetDesc( &m_EffectDesc );

        ParseParameters( strErrors );

        // Look for UI elements
        m_UIElements.InitEffectObjects( m_pEffect );

        if( m_EffectDesc.Techniques == 0 )
            m_iTechnique = -1;
        else if( (UINT)m_iTechnique >= m_EffectDesc.Techniques )
            m_iTechnique = 0;
        else if( m_iTechnique == -1 )
            m_iTechnique = 0;
        *piTechnique = m_iTechnique;

        for( UINT iTech = 0; iTech < m_EffectDesc.Techniques; iTech++ )
        {
            D3DXTECHNIQUE_DESC desc;
            m_pEffect->GetTechniqueDesc( m_pEffect->GetTechnique( iTech ), &desc );
            techniqueNameList.AddTail(desc.Name);
        }
    }

    if( strErrors.IsEmpty() )
        strErrors = TEXT("Effect compilation successful\n");

    return hr;
}


HRESULT CRenderView::ParseParameters(CString &strErrors)
{
    HRESULT hr;
    D3DXPARAMETER_DESC Desc;

    if( m_pEffect == NULL )
        return E_FAIL;

    //look for background Texture
    const char* pstrBackTexture;
    SAFE_RELEASE( m_pBackgroundTexture );
    if( NULL != m_pEffect->GetParameterByName( NULL, "BIMG" ) &&
        SUCCEEDED( m_pEffect->GetParameterDesc( "BIMG", &Desc ) ) &&
        Desc.Type == D3DXPT_STRING )
    {
        m_pEffect->GetString("BIMG", &pstrBackTexture);
        WCHAR strBackTexture[MAX_PATH];
        MultiByteToWideChar( CP_ACP, 0, pstrBackTexture, -1, strBackTexture, MAX_PATH );
        TCHAR strPath[MAX_PATH];
        EE_FindMediaFileCch( strPath, MAX_PATH, strBackTexture );
        if( FAILED( hr = D3DXCreateTextureFromFile( DXUTGetD3DDevice(), strPath, &m_pBackgroundTexture) ) )
        {
            strErrors += TEXT("Could not load background image texture ");
            strErrors += strBackTexture;
            strErrors += TEXT("\n");
        }
    }
    // look for background environment Texture
    SAFE_RELEASE( m_pEnvironmentTexture );
    if( NULL != m_pEffect->GetParameterByName( NULL, "Environment" ) &&
        SUCCEEDED( m_pEffect->GetParameterDesc( "Environment", &Desc ) ) &&
        Desc.Type == D3DXPT_STRING )
    {
        m_pEffect->GetString("Environment", &pstrBackTexture);
        TCHAR strBackTexture[MAX_PATH];
        MultiByteToWideChar( CP_ACP, 0, pstrBackTexture, -1, strBackTexture, MAX_PATH );
        TCHAR strPath[MAX_PATH];
        EE_FindMediaFileCch( strPath, MAX_PATH, strBackTexture );
        if( FAILED( hr = D3DXCreateCubeTextureFromFile( DXUTGetD3DDevice(), strPath, &m_pEnvironmentTexture) ) )
        {
            strErrors += TEXT("Could not load environment cube texture ");
            strErrors += strBackTexture;
            strErrors += TEXT("\n");
        }
    }

    // Look at parameters for semantics and annotations that we know how to interpret
    D3DXPARAMETER_DESC ParamDesc;
    D3DXPARAMETER_DESC AnnotDesc;
    D3DXHANDLE hParam;
    D3DXHANDLE hAnnot;
    TCHAR strPath[MAX_PATH];
    LPDIRECT3DBASETEXTURE9 pTex = NULL;

    for( UINT iParam = 0; iParam < m_EffectDesc.Parameters; iParam++ )
    {
        LPCSTR pstrName = NULL;
        LPCSTR pstrFunction = NULL;
        LPCSTR pstrTarget = NULL;
        LPCSTR pstrTextureType = NULL;
        INT Width = D3DX_DEFAULT;
        INT Height= D3DX_DEFAULT;
        INT Depth = D3DX_DEFAULT;

        hParam = m_pEffect->GetParameter ( NULL, iParam );
        m_pEffect->GetParameterDesc( hParam, &ParamDesc );
        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_MATRIX_ROWS || ParamDesc.Class == D3DXPC_MATRIX_COLUMNS ) )
        {
            if( _strcmpi( ParamDesc.Semantic, "world" ) == 0 )
                m_MatWorldEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "view" ) == 0 )
                m_MatViewEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "projection" ) == 0 )
                m_MatProjEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldview" ) == 0 )
                m_MatWorldViewEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "viewprojection" ) == 0 )
                m_MatViewProjEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldviewprojection" ) == 0 )
                m_MatWorldViewProjEffectHandle = hParam;
        }

        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_VECTOR ))
        {
            if( _strcmpi( ParamDesc.Semantic, "materialambient" ) == 0 )
                m_MaterialAmbientEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "materialdiffuse" ) == 0 )
                m_MaterialDiffuseEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "materialspecular" ) == 0 )
                m_MaterialSpecularEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "cameraposition" ) == 0 )
                m_VecCameraPosEffectHandle = hParam;
        }

        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_SCALAR ))
        {
            if( _strcmpi( ParamDesc.Semantic, "materialpower" ) == 0 )
                m_MaterialSpecularPowerEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "time" ) == 0 )
                m_TimeEffectHandle = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "meshradius" ) == 0 )
                m_MeshRadiusEffectHandle = hParam;
        }

        for( UINT iAnnot = 0; iAnnot < ParamDesc.Annotations; iAnnot++ )
        {
            hAnnot = m_pEffect->GetAnnotation ( hParam, iAnnot );
            m_pEffect->GetParameterDesc( hAnnot, &AnnotDesc );
            if( _strcmpi( AnnotDesc.Name, "name" ) == 0 )
                m_pEffect->GetString( hAnnot, &pstrName );
            else if ( _strcmpi( AnnotDesc.Name, "function" ) == 0 )
                m_pEffect->GetString( hAnnot, &pstrFunction );
            else if ( _strcmpi( AnnotDesc.Name, "target" ) == 0 )
                m_pEffect->GetString( hAnnot, &pstrTarget );
            else if ( _strcmpi( AnnotDesc.Name, "width" ) == 0 )
                m_pEffect->GetInt( hAnnot, &Width );
            else if ( _strcmpi( AnnotDesc.Name, "height" ) == 0 )
                m_pEffect->GetInt( hAnnot, &Height );
            else if ( _strcmpi( AnnotDesc.Name, "depth" ) == 0 )
                m_pEffect->GetInt( hAnnot, &Depth );
            else if( _strcmpi( AnnotDesc.Name, "type" ) == 0 )
                m_pEffect->GetString( hAnnot, &pstrTextureType );

        }
        if( pstrName != NULL )
        {
            pTex = NULL;
            TCHAR strName[MAX_PATH];
            MultiByteToWideChar( CP_ACP, 0, pstrName, -1, strName, MAX_PATH );
            EE_FindMediaFileCch( strPath, MAX_PATH, strName );
            if (pstrTextureType != NULL) 
            {
                if( _strcmpi( pstrTextureType, "volume" ) == 0 )
                {
                    LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateVolumeTextureFromFileEx( DXUTGetD3DDevice(), strPath, 
                        Width, Height, Depth, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pVolumeTex ) ) )
                    {
                        pTex = pVolumeTex;
                    }
                    else
                    {
                        strErrors += TEXT("Could not load volume texture ");
                        strErrors += strName;
                        strErrors += TEXT("\n");
                    }
                }
                else if( _strcmpi( pstrTextureType, "cube" ) == 0 )
                {
                    LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateCubeTextureFromFileEx( DXUTGetD3DDevice(), strPath, 
                        Width, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubeTex ) ) )
                    {
                        pTex = pCubeTex;
                    }
                    else
                    {
                        strErrors += TEXT("Could not load cube texture ");
                        strErrors += strName;
                        strErrors += TEXT("\n");
                    }
                }
            }
            else
            {
                LPDIRECT3DTEXTURE9 p2DTex = NULL;
                if( SUCCEEDED( hr = D3DXCreateTextureFromFileEx( DXUTGetD3DDevice(), strPath, 
                    Width, Height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &p2DTex ) ) )
                {
                    pTex = p2DTex;
                }
                else
                {
                    strErrors += TEXT("Could not load texture ");
                    strErrors += strName;
                    strErrors += TEXT("\n");
                }
            }

            // Apply successfully loaded texture to effect
            if( SUCCEEDED(hr) && pTex != NULL ) 
            {
                m_pEffect->SetTexture( m_pEffect->GetParameter( NULL, iParam ), pTex );
                SAFE_RELEASE( pTex );
            }
        }
        else if( pstrFunction != NULL )
        {
            LPD3DXBUFFER pFunction = NULL;
            LPD3DXTEXTURESHADER pTextureShader = NULL;
            CIncludeManager includeManager(this);
            CString strEffect = GetDocument()->GetCode();
            UINT EffectLength = strEffect.GetLength();
            char* strEffectA = new char[EffectLength + 1];
            if( strEffectA == NULL )
                return E_OUTOFMEMORY;
            WideCharToMultiByte( CP_ACP, 0, strEffect, -1, strEffectA, EffectLength + 1, NULL, NULL );

            if( pstrTarget == NULL )
                pstrTarget = "tx_1_0";

            LPD3DXBUFFER pBufferErrors = NULL;
            if( SUCCEEDED( hr = D3DXCompileShader( strEffectA, (UINT)strlen(strEffectA), NULL, 
                &includeManager, pstrFunction, pstrTarget, 0, &pFunction, &pBufferErrors, NULL ) ) )
            {
                if( SUCCEEDED( hr = D3DXCreateTextureShader((DWORD *)pFunction->GetBufferPointer(), &pTextureShader) ) )
                {
                    pTextureShader->SetDefaults();

                    if( Width == D3DX_DEFAULT )
                        Width = 64;
                    if( Height == D3DX_DEFAULT )
                        Height = 64;
                    if( Depth == D3DX_DEFAULT )
                        Depth = 64;

                    if (pstrTextureType != NULL) 
                    {
                        if( _strcmpi( pstrTextureType, "volume" ) == 0 )
                        {
                            LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
                            if( SUCCEEDED( hr = D3DXCreateVolumeTexture( DXUTGetD3DDevice(), 
                                Width, Height, Depth, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pVolumeTex) ) )
                            {
                                if( SUCCEEDED( hr = D3DXFillVolumeTextureTX( pVolumeTex, pTextureShader ) ) ) 
                                    pTex = pVolumeTex;
                            }
                        }
                        else if( _strcmpi( pstrTextureType, "cube" ) == 0 )
                        {
                            LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
                            if( SUCCEEDED( hr = D3DXCreateCubeTexture( DXUTGetD3DDevice(), 
                                Width, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pCubeTex) ) )
                            {
                                if( SUCCEEDED( hr = D3DXFillCubeTextureTX( pCubeTex, pTextureShader ) ) )
                                    pTex = pCubeTex;
                            }
                        }
                    }
                    else
                    {
                        LPDIRECT3DTEXTURE9 p2DTex = NULL;
                        if( SUCCEEDED( hr = D3DXCreateTexture( DXUTGetD3DDevice(), Width, Height, 
                            D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &p2DTex) ) )
                        {
                            if( SUCCEEDED( hr = D3DXFillTextureTX( p2DTex, pTextureShader ) ) )
                                pTex = p2DTex;
                        }
                    }
                    m_pEffect->SetTexture( m_pEffect->GetParameter( NULL, iParam ), pTex );
                    SAFE_RELEASE(pTex);
                    SAFE_RELEASE(pTextureShader);
                }
                SAFE_RELEASE(pFunction);
            }
            if( pBufferErrors != NULL )
            {
                strErrors += (CHAR*)pBufferErrors->GetBufferPointer();
                pBufferErrors->Release();
            }

            delete[] strEffectA;
        }
    }

    //look for background Color
    if( NULL != m_pEffect->GetParameterByName( NULL, "BCLR" ) &&
        SUCCEEDED( m_pEffect->GetParameterDesc( "BCLR", &Desc ) ) )
        m_pEffect->GetInt("BCLR", (INT *)&m_dwBackgroundColor);
    else
        m_dwBackgroundColor = D3DCOLOR_ARGB(255, 0, 0, 255);


    // look for mesh
    if( NULL != m_pEffect->GetParameterByName( NULL, "XFile" ) &&
        SUCCEEDED( m_pEffect->GetParameterDesc( "XFile", &Desc ) ) )
    {
        const char *fileName;
    
        m_pEffect->GetString("XFile",&fileName);
        if( _strcmpi( fileName, m_strMesh ) != 0 || m_pMesh == NULL )
        {
            UnloadMesh();
            strcpy( m_strMesh, fileName ); 
            
            //load the mesh
            LoadMesh( strErrors );
        }
    }
    else
    {
        if( strlen(m_strMesh) > 0 )
        {
            // There used to be an XFile, but no longer
            UnloadMesh();
            m_strMesh[0] = NULL;
        }
    }
    BOOL bUsesTangents = FALSE;

    // Look for tangents semantic
    D3DXEFFECT_DESC EffectDesc;
    D3DXHANDLE hTechnique;
    D3DXTECHNIQUE_DESC TechniqueDesc;
    D3DXHANDLE hPass;
    D3DXPASS_DESC PassDesc;

    m_pEffect->GetDesc( &EffectDesc );
    for( UINT iTech = 0; iTech < EffectDesc.Techniques; iTech++ )
    {
        hTechnique = m_pEffect->GetTechnique( iTech );
        m_pEffect->GetTechniqueDesc( hTechnique, &TechniqueDesc );
        for( UINT iPass = 0; iPass < TechniqueDesc.Passes; iPass++ )
        {
            hPass = m_pEffect->GetPass( hTechnique, iPass );
            m_pEffect->GetPassDesc( hPass, &PassDesc );

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
                    goto DoneLooking;
                }
            }
        }
    }

DoneLooking:
    if( bUsesTangents && m_pMesh != NULL )
    {
        D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
        D3DVERTEXELEMENT9 End = D3DDECL_END();
        int iElem;
     
        m_pMesh->GetDeclaration( Declaration );
     
        BOOL bHasTangents = FALSE;
        for( iElem=0; Declaration[iElem].Stream != End.Stream; iElem++ )
        {   
            if( Declaration[iElem].Usage == D3DDECLUSAGE_TANGENT )
            {
                bHasTangents = TRUE;
                break;
            }
        }
     
        // Update Mesh Semantics if changed
        if( !bHasTangents ) 
        {
            Declaration[iElem].Stream = 0;
            Declaration[iElem].Offset = (WORD)m_pMesh->GetNumBytesPerVertex();
            Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
            Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
            Declaration[iElem].Usage = D3DDECLUSAGE_TANGENT;
            Declaration[iElem].UsageIndex = 0;
            Declaration[iElem+1] = End;
            LPD3DXMESH pTempMesh;
            hr = m_pMesh->CloneMesh( D3DXMESH_MANAGED, Declaration, DXUTGetD3DDevice(), &pTempMesh );
            if( SUCCEEDED( hr ) )
            {
                SAFE_RELEASE( m_pMesh );
                m_pMesh = pTempMesh;
                hr = D3DXComputeTangent( m_pMesh, 0, 0, D3DX_DEFAULT, TRUE, NULL );
            }
        }
    }

    return S_OK;
}


HRESULT CRenderView::LoadMesh( CString &strErrors )
{
    if( strlen(m_strMesh) == 0 )
        return S_OK;

    LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
    void*      pVertices = NULL;
    LPD3DXMESH pTempMesh;
    WCHAR      strMeshW[MAX_PATH];
    WCHAR      strMeshPath[MAX_PATH];
    HRESULT    hr;
    LPD3DXBUFFER pbufAdjacency;

    // Load the mesh from the specified file
    MultiByteToWideChar( CP_ACP, 0, m_strMesh, -1, strMeshW, MAX_PATH );
    if( FAILED( hr = EE_FindMediaFileCch( strMeshPath, MAX_PATH, strMeshW ) ) )
    {
        strErrors += L"Could not find XFile ";
        strErrors += strMeshW;
        strErrors += L"\n";
        return hr;
    }

    hr = D3DXLoadMeshFromX( strMeshPath, D3DXMESH_MANAGED, DXUTGetD3DDevice(), 
                            &pbufAdjacency, &m_pbufMaterials, NULL, &m_dwNumMaterials, 
                            &m_pMesh );
    if( FAILED(hr) )
    {
        SAFE_RELEASE(pbufAdjacency);
        strErrors += L"Could not load XFile ";
        strErrors += strMeshW;
        strErrors += L"\n";
        return hr;
    }

    // Initialize the mesh directory string
    WCHAR *pwszLastBSlash = wcsrchr( strMeshPath, L'\\' );
    if( pwszLastBSlash )
        *pwszLastBSlash = L'\0';
    else
        lstrcpyW( strMeshPath, L"." );

    hr = m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, (DWORD*)pbufAdjacency->GetBufferPointer(), NULL, NULL, NULL);
    if ( FAILED(hr) )
    {
        SAFE_RELEASE(pbufAdjacency);
        return hr;
    }

    SAFE_RELEASE(pbufAdjacency);

    // Lock the vertex buffer, to generate a simple bounding sphere
    hr = m_pMesh->GetVertexBuffer( &pVB );
    if( FAILED(hr) )
        return hr;

    hr = pVB->Lock( 0, 0, &pVertices, 0 );
    if( FAILED(hr) )
    {
        SAFE_RELEASE( pVB );
        return hr;
    }

    hr = D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, m_pMesh->GetNumVertices(), 
                                    D3DXGetFVFVertexSize(m_pMesh->GetFVF()), &m_vObjectCenter, 
                                    &m_fObjectRadius );
    if( FAILED(hr) )
    {
        pVB->Unlock();
        SAFE_RELEASE( pVB );
        return hr;
    }

    // Adjust near/far clip planes of projection matrix based on the object radius
    const D3DSURFACE_DESC *pBackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
    FLOAT fAspect = (FLOAT)pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 
                                m_fObjectRadius/64.0f, m_fObjectRadius*64.0f );
    DXUTGetD3DDevice()->SetTransform( D3DTS_PROJECTION, &m_matProj );

    m_ArcBallMesh.SetTranslationRadius( m_fObjectRadius );
    m_ArcBallView.SetTranslationRadius( m_fObjectRadius );

    if( 0 == m_dwNumMaterials )
    {
        pVB->Unlock();
        SAFE_RELEASE( pVB );
        return E_INVALIDARG;
    }

    // Get the array of materials out of the returned buffer, allocate a
    // texture array, and load the textures
    m_pMaterials = (D3DXMATERIAL*)m_pbufMaterials->GetBufferPointer();
    m_ppTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

    // Change the current directory to the .x's directory so
    // that the search can find the texture files.
    WCHAR wszWorkingDir[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, wszWorkingDir );
    wszWorkingDir[MAX_PATH - 1] = L'\0';
    SetCurrentDirectory( strMeshPath );

    for( UINT i=0; i<m_dwNumMaterials; i++ )
    {
        m_ppTextures[i] = NULL;
        TCHAR strTextureName[MAX_PATH];
        TCHAR strTexturePath[MAX_PATH] = _T("");
        if( m_pMaterials[i].pTextureFilename != NULL && strlen( m_pMaterials[i].pTextureFilename ) != 0 )
        {
            MultiByteToWideChar( CP_ACP, 0, m_pMaterials[i].pTextureFilename, -1, strTextureName, MAX_PATH );
            if( FAILED( hr = EE_FindMediaFileCch( strTexturePath, MAX_PATH, strTextureName ) ) )
            {
                strErrors += TEXT("Could not find texture ");
                strErrors += strTextureName;
                strErrors += TEXT(" referenced by XFile ");
                strErrors += strMeshW;
                strErrors += TEXT("\n");
            }
            else if( FAILED( hr = D3DXCreateTextureFromFile( DXUTGetD3DDevice(), strTexturePath, &m_ppTextures[i] ) ) )
            {
                strErrors += TEXT("Could not load texture ");
                strErrors += strTextureName;
                strErrors += TEXT(" referenced by XFile ");
                strErrors += strMeshW;
                strErrors += TEXT("\n");
            }
        }
    }

    SetCurrentDirectory( wszWorkingDir );

    pVB->Unlock();
    SAFE_RELEASE( pVB );

    // Make sure there are normals, which are required for the tesselation
    // enhancement
    if( !(m_pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        hr = m_pMesh->CloneMeshFVF( m_pMesh->GetOptions(), 
                                          m_pMesh->GetFVF() | D3DFVF_NORMAL, 
                                          DXUTGetD3DDevice(), &pTempMesh );
        if( FAILED(hr) )
            return hr;

        D3DXComputeNormals( pTempMesh, NULL );

        SAFE_RELEASE( m_pMesh );
        m_pMesh = pTempMesh;
    }

    return S_OK;
}


void CRenderView::UnloadMesh()
{
    for( UINT i = 0; i < m_dwNumMaterials; i++ )
        SAFE_RELEASE( m_ppTextures[i] );
    SAFE_DELETE_ARRAY( m_ppTextures );
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pbufMaterials );
    m_dwNumMaterials = 0L;
    m_strMesh[0] = 0;
}

LRESULT CRenderView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    // If this is a mouse message, we need to map the coordinates
    // to relative to the render window.
    if( WM_LBUTTONDOWN == message ||
        WM_LBUTTONDBLCLK == message ||
        WM_LBUTTONUP == message ||
        WM_MBUTTONDOWN == message ||
        WM_MBUTTONDBLCLK == message ||
        WM_MBUTTONUP == message ||
        WM_RBUTTONDOWN == message ||
        WM_RBUTTONDBLCLK == message ||
        WM_RBUTTONUP == message ||
        WM_XBUTTONDOWN == message ||
        WM_XBUTTONDBLCLK == message ||
        WM_XBUTTONUP == message ||
        WM_MOUSEMOVE == message ||
        WM_MOUSEWHEEL == message )
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        // Map point to parent's coordinates.
        ::MapWindowPoints( GetSafeHwnd(), DXUTGetHWND(), &pt, 1 );
        lParam = MAKELPARAM( pt.x, pt.y );
    }

    if( DXUTGetShowSettingsDialog() )
    {
        // Prevent reentrant
        static bool bProcessing;

        if( !bProcessing )
        {
            bProcessing = true;
            DXUTStaticWndProc( DXUTGetHWND(), message, wParam, lParam );
            bProcessing = false;
        }
    }

    switch( message )
    {
        case WM_LBUTTONDOWN:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            BOOL bSetCapture;
            m_UIElements.HandleMouseDown( pt.x, pt.y, &bSetCapture );
            if( bSetCapture )
                SetCapture();
            break;
        }

        case WM_MOUSEMOVE:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_UIElements.HandleMouseMove( pt.x, pt.y );
            break;
        }

        case WM_LBUTTONUP:
        {
            m_UIElements.HandleMouseUp();
            break;
        }
    }

    if( !m_UIElements.IsDragging() )
    {
        UINT bControl;
        bControl = GetAsyncKeyState(VK_CONTROL) & 0x8000;

        // Pass mouse messages to the ArcBall so it can build internal matrices
        if (!bControl)
            m_ArcBallMesh.HandleMessages( GetSafeHwnd(), message, wParam, lParam );
        else
            m_ArcBallView.HandleMessages( GetSafeHwnd(), message, wParam, lParam );
    }

    return CFormView::WindowProc(message, wParam, lParam);
}

void CRenderView::ChangeDevice() 
{
    DXUTPause( true, true );
    DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() );
    DXUTPause( false, false );
}


void CRenderView::SetTechnique( int iTech, CString strTechName )
{
    CString str;
    str.Format( TEXT("Rendering using technique '%s'"), strTechName );
    m_iTechnique = iTech;
    GetDlgItem(IDC_RENDERTEXT)->SetWindowText( str );
    D3DXHANDLE hTechnique = m_pEffect->GetTechnique( m_iTechnique );
    m_pEffect->SetTechnique( hTechnique );
    m_bTechniqueValidValid = FALSE;
}


void CRenderView::SetPass( int iPass, CString strPassName )
{
    m_iPass = iPass;
}

void CRenderView::GetPassNameList( int iTech, CStringList& passNameList )
{
    passNameList.RemoveAll();

    if( m_pEffect == NULL )
        return;

    D3DXTECHNIQUE_DESC techniqueDesc;
    D3DXHANDLE hTech;
    hTech = m_pEffect->GetTechnique( iTech );
    m_pEffect->GetTechniqueDesc( hTech, &techniqueDesc );
    for( UINT iPass = 0; iPass < techniqueDesc.Passes; iPass++ )
    {
        D3DXPASS_DESC desc;
        m_pEffect->GetPassDesc( m_pEffect->GetPass( hTech, iPass ), &desc );
        passNameList.AddTail(desc.Name);
    }
}


void CRenderView::ResetCamera()
{
    m_ArcBallMesh.Reset();
    m_ArcBallMesh.SetTranslationRadius( m_fObjectRadius );

    m_ArcBallView.Reset();
    m_ArcBallView.SetTranslationRadius( m_fObjectRadius );
}


BOOL FileExists( CString& strPath )
{
    HANDLE file;

    file = CreateFile( (LPCTSTR)strPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        CloseHandle( file );
        return TRUE;
    }
    return FALSE;
}

HRESULT CRenderView::EE_FindMediaFileCb( TCHAR* szDestPath, int cbDest, TCHAR* strFilename )
{
    return EE_FindMediaFileCch( szDestPath, cbDest / sizeof(TCHAR), strFilename );

}

// 1. Look in same dir as effects file (EffectFileDir)
// 2. Look in EffectFileDir\Media
// 3. Look in EffectFileDir\..\Media
// 4. Look in usual DxUtil_FindMediaFile paths
HRESULT CRenderView::EE_FindMediaFileCch( TCHAR* szDestPath, int cchDest, TCHAR* strFilename )
{
    CString strDoc = GetDocument()->GetPathName();
    CString strDocT;
    int iChar = strDoc.ReverseFind( TEXT('\\') );
    if( iChar >= 0 )
    {
        strDoc = strDoc.Left( iChar );

        strDocT = strDoc + TEXT("\\") + strFilename;
        if( FileExists( strDocT) )
        {
            _tcsncpy( szDestPath, strDocT, cchDest );
            szDestPath[cchDest-1] = 0; // _tcsncpy doesn't NULL term if it runs out of space
            return S_OK;
        }

        strDocT = strDoc + TEXT("\\Media\\") + strFilename;
        if( FileExists( strDocT) )
        {
            _tcsncpy( szDestPath, strDocT, cchDest );
            szDestPath[cchDest-1] = 0; // _tcsncpy doesn't NULL term if it runs out of space
            return S_OK;
        }

        strDocT = strDoc + TEXT("\\..\\Media\\" + strFilename);
        if( FileExists( strDocT) )
        {
            _tcsncpy( szDestPath, strDocT, cchDest );
            szDestPath[cchDest-1] = 0; // _tcsncpy doesn't NULL term if it runs out of space
            return S_OK;
        }
    }
    return DXUTFindDXSDKMediaFileCch( szDestPath, cchDest, strFilename );
}

