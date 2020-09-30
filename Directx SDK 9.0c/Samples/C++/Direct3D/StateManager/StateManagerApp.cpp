//--------------------------------------------------------------------------------------
// File: StateManagerApp.cpp
//
// This sample shows an example implementation of the ID3DXEffectStateManager interface.
// This inteface can be used to implement custom state-change handling for the D3DX
// Effects system.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"


//--------------------------------------------------------------------------------------
// Defines custom interface to a sample implementation of the ID3DXEffectStateManager
// interface
//--------------------------------------------------------------------------------------
#include "EffectStateManager.h"


//--------------------------------------------------------------------------------------
// Contains (along with renderables.cpp) support code for sample.  This code is used
// to manage Mesh, Effect, and Texture resources in a general manner.  Because elements
// of this sample have performance implications (eg, redundant state filtering on
// PURE device), it is desirable to handle renderable objects (mesh subsets) in a
// such a general manner.  This allows lists of renderables to be re-sorted in arbitrary
// orders, to minimize the number of state changes required to render the scene.
//--------------------------------------------------------------------------------------
#include "renderables.h"


//--------------------------------------------------------------------------------------
// The scene is loaded from an x-file, that has been extended to include templates for
// specifying mesh filenames, and cameras.
//--------------------------------------------------------------------------------------
#include "LoadSceneFromX.h"



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;             // Font for drawing text
ID3DXSprite*            g_pSprite = NULL;           // Sprite for batching draw text calls
bool                    g_bShowHelp = true;         // If true, it renders the UI control text
CFirstPersonCamera      g_Camera;                   // A model viewing camera
CDXUTDialog             g_HUD;                      // manages the 3D UI
CDXUTDialog             g_SampleUI;                 // dialog for sample specific controls
CStateManagerInterface* g_pStateManager = NULL;     // The current ID3DXEffectStateManager implementation
bool                    g_bSortByMaterial = true;   // Sort by Material/Effect (versus mesh instance)
bool                    g_bSortOrderDirty = true;   // Causes the Renderable Objects to be re-sorted
bool                    g_bAppIsDebug = false;      // The application build is debug
bool                    g_bRuntimeDebug = false;    // The D3D Debug Runtime was detected
bool                    g_bStateManger  = true;     // Enable custom-implemented d3dx effect state management
bool                    g_bResetCamera = true;      // Causes camera to be (re)loaded from scene
vector<CInstance*>      g_vecInstances;             // Contains the group of mesh instances composing the scene
typedef vector<CRenderable> pass;                   // A render-pass is composed of individual items to be rendered
vector<pass>            g_passes;                   // Contains a group of rendering passes
UINT                    g_nBindCPU = 0;             // Add extra CPU workload, to (hopefully) become CPU-bound if not already
UINT                    g_nMaxRocksToRender = 200;  // The initial maximum number of rocks to be rendered

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_NUM_ROCKS_STATIC    5
#define IDC_NUM_ROCKS           6
#define IDC_TOGGLEPURE          7
#define IDC_STATEMANAGER        8
#define IDC_MATERIALSORT        9
#define IDC_BIND_CPU_STATIC     10
#define IDC_BIND_CPU            11


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl );
void    CALLBACK OnLostDevice();
void    CALLBACK OnDestroyDevice();

void    InitApp();
HRESULT BuildSceneFromX( LPDIRECT3DDEVICE9 pd3dDevice );
void    RenderText( double fTime );
void    QueueAndSortRenderables();
void    SortRenderables();
void    SetStateManager( );
HRESULT CreateInstance( LPDIRECT3DDEVICE9 pDevice, LPCWSTR wszFileName,
                        D3DXMATRIX* pWorld, UINT nRenderPass, CInstance** ppInstance = NULL );


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    try
    {
        // Set the callback functions. These functions allow the sample framework to notify
        // the application about device changes, user input, and windows messages.  The 
        // callbacks are optional so you need only set callbacks for events you're interested 
        // in. However, if you don't handle the device reset/lost callbacks then the sample 
        // framework won't be able to reset your device since the application must first 
        // release all device resources before resetting.  Likewise, if you don't handle the 
        // device created/destroyed callbacks then the sample framework won't be able to 
        // recreate your device resources.
        DXUTSetCallbackDeviceCreated( OnCreateDevice );
        DXUTSetCallbackDeviceReset( OnResetDevice );
        DXUTSetCallbackDeviceLost( OnLostDevice );
        DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
        DXUTSetCallbackMsgProc( MsgProc );
        DXUTSetCallbackKeyboard( KeyboardProc );
        DXUTSetCallbackFrameRender( OnFrameRender );
        DXUTSetCallbackFrameMove( OnFrameMove );

        // Show the cursor and clip it when in full screen
        DXUTSetCursorSettings( true, true );

        InitApp();

        // Initialize the sample framework and create the desired Win32 window and Direct3D 
        // device for the application. Calling each of these functions is optional, but they
        // allow you to set several options which control the behavior of the framework.
        DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
        DXUTCreateWindow( L"StateManager" );
        DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

        // Performance observations should not be compared against dis-similar builds (debug v retail)
    #if defined(DEBUG) | defined(_DEBUG)
        g_bAppIsDebug = true;
    #endif

        // Performance observations should not be compared against dis-similar d3d runtimes (debug v retail)
        if( GetModuleHandleW( L"d3d9d.dll" ) )
            g_bRuntimeDebug = true;

        // Pass control to the sample framework for handling the message pump and 
        // dispatching render calls. The sample framework will call your FrameMove 
        // and FrameRender callback when there is idle time between handling window messages.
        DXUTMainLoop();

        // Perform any application-level cleanup here. Direct3D device resources are released within the
        // appropriate callback functions and therefore don't require any cleanup code here.
    }
    catch( exception e )
    {
        WCHAR wsz[256];
        MultiByteToWideChar( CP_ACP, 0, e.what(), -1, wsz, 256 );
        wsz[ 255 ] = 0;
        DXUTTrace( __FILE__, (DWORD)__LINE__, E_FAIL, wsz, true );
    }
    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Set the application initial viewpoint
    D3DXVECTOR3 vecEye(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, 0.0f);
    g_Camera.SetViewParams( &vecEye, &vecAt );

    // Set the camera speed
    g_Camera.SetScalers( 0.01f, 10.0f );
    
    // Constrain the camera to movement within the horizontal plane
    g_Camera.SetEnableYAxisMovement( false );

    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10; 

    WCHAR sz[100];
    iY += 24;
    _snwprintf( sz, 100, L"# Rocks: %d", g_nMaxRocksToRender ); sz[99] = 0;
    g_SampleUI.AddStatic( IDC_NUM_ROCKS_STATIC, sz, 35, iY += 24, 125, 22 );
    g_SampleUI.AddSlider( IDC_NUM_ROCKS, 50, iY += 24, 100, 22, 1, g_nMaxRocksToRender, g_nMaxRocksToRender );

    g_SampleUI.AddButton( IDC_TOGGLEPURE, L"Toggle PURE", 35, iY += 24, 125, 22 );
    g_SampleUI.AddCheckBox( IDC_STATEMANAGER, L"Use State Manager", 35, iY += 24, 125, 22, g_bStateManger );
    g_SampleUI.AddCheckBox( IDC_MATERIALSORT, L"Sort By Material", 35, iY += 24, 125, 22, g_bSortByMaterial );

    iY += 24;
    _snwprintf( sz, 100, L"Bind CPU: %d", g_nBindCPU ); sz[99] = 0;
    g_SampleUI.AddStatic( IDC_BIND_CPU_STATIC, sz, 35, iY += 24, 125, 22 );
    g_SampleUI.AddSlider( IDC_BIND_CPU, 50, iY += 24, 100, 22, 0, 20, g_nBindCPU );

}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning E_FAIL.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // No fallback defined by this app, so reject any device that 
    // doesn't support at least ps1.4
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,4) )
        return false;

    // Skip devices that do not support cubemaps
    if( 0 == (pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP ) )
        return false;
    
    // Skip non-TL devices
    if( 0 == (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) )
        return false;

    // Skip devices that do not enable PURE device
    if (0 == (pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// the sample framework will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
{
    // This sample runs only against devices that support Pure Hardware TL.
    pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                              L"Arial", &g_pFont ) );

    V_RETURN( BuildSceneFromX( pd3dDevice ) );

    // Establish a sort-order for the render queue
    QueueAndSortRenderables();

    // Set a state manager for all loaded effects
    SetStateManager();

    return S_OK;
}



//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;
   
    // Notify loaded resources that the device has been Created or Reset
    V_RETURN( CTexture::ResetDevice() );
    V_RETURN( CEffect::ResetDevice() );
    V_RETURN( CMeshObject::ResetDevice() );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );


    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 500.f );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-300 );
    g_SampleUI.SetSize( 170, 300 );

    // Device state does not persist across a reset.
    // Any state that is cached within the state manager (for state filtering) will then
    // become invalid. In turn, this can cause the state manager to initially filter some
    // states that are mistakenly recognized as redundant.
    // Reset the state manager by asking it to discard its cached states.
    if( g_pStateManager )
        g_pStateManager->DirtyCachedValues();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );

    // If Extra CPU work is requested, add it
    // State changes tend to involve significantly more CPU work than graphics hardware work.
    // However, the application may be continually waiting on the graphics hardware.
    // In this case, the overall application performance will not be greatly impacted until the
    // CPU workload exceeds the graphics hardware workload.
    D3DXMATRIX mat;
    D3DXMatrixIdentity( &mat );
    for( UINT i = 0; i< g_nBindCPU; i++ )
        for( UINT n = 0; n < 10000; n++ )
            D3DXMatrixMultiply( &mat, &mat, &mat );

}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;
    D3DXMATRIXA16 matProj, matView;
    static UINT nFrameTimeStamp = 0;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.f,1.f,1.f,0.f), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Get the projection & view matrix from the camera class
        matProj = *g_Camera.GetProjMatrix();       
        matView = *g_Camera.GetViewMatrix();

        // Sort the scene by either material or mesh instance
        SortRenderables();

        DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"Draw Code" );

        // Render each scene-level pass in turn
        // A scene-level pass in different than an effects pass.
        // Scene-level passes ensure that particular rendering orders are enforced, such as
        // allowing for all transparent objects to be drawn last.
        for( vector<pass>::iterator it_pass = g_passes.begin();
             it_pass != g_passes.end();
             it_pass++ )
        {
            pass& renderPass = *it_pass;

            // iterators, used to access the render queue for this pass
            pass::iterator it = renderPass.begin();
            pass::iterator it_begin;
            pass::iterator it_end;

            while( renderPass.end() != it )
            {
                UINT nPasses = 0;
                CEffect* pEffect = (*it).getEffect();
                
                // Render various materials using the same effect technique in batches.
                // Scan the list ahead to the next renderable not in this batch.
                for( it_end = it_begin = it; 
                    (it_end != renderPass.end()) && ((*it_end).getEffect() == (*it).getEffect());
                    it_end++ )
                {
                    // no action needed here
                    // following the loop, it_end will point to the element beyond the last
                    // renderable sharing the same effect
                }

                // Initialize the effect
                pEffect->getPointer()->Begin( &nPasses, D3DXFX_DONOTSAVESTATE );

                CInstance *pInstance, *pLastInstance = NULL;
                CEffectInstance *pEffectInstance, *pLastEffectInstance = NULL;
                
                // All materials sharing the same effect will be grouped into the effect pass.
                // As-needed, the effect will be updated with material-specific (effect instance)
                // or mesh/object instance (transformation matrices).
                // Each time ID3DXEffect::BeginPass is invoked, state change commands to support
                // the effect are invoked.  
                for( UINT i = 0; i< nPasses; i++ )
                {
                    // Effect-wide globals (scene-level info such as lighting, etc) should be
                    // set here, before the pass begins
                    //

                    // Initialize the pass
                    pEffect->getPointer()->BeginPass( i );
                    
                    // Iterate across all renderables determined to share the same effect/technique 
                    for( it = it_begin; it != it_end; it++ )
                    {
                        // The instance is needed to retrieve the world matrix for this object
                        pInstance = (*it).getInstance();
                        pEffectInstance = (*it).getEffectInstance();

                        // If the mesh object instance changed, the new object matrices
                        // must be sent to the effect
                        // The timestamp ensures that view and project matrices are not updated redundantly
                        if( pLastInstance != pInstance )
                            pEffect->SetMatrices( pInstance->getMatrix(), &matView, &matProj, nFrameTimeStamp );

                        // If the EffectInstance changed, the effect must be updated with the new
                        // parameter values
                        if( pLastEffectInstance != pEffectInstance )
                            pEffectInstance->apply();

                        // Changing Effect Parameters mid-pass requires notifying ID3DXEffect
                        pEffect->getPointer()->CommitChanges();

                        // The Effect has been fully updated -- Render the mesh subset!
                        pInstance->getMeshObject()->getPointer()->DrawSubset( (*it).getSubset() );

                        pLastInstance = pInstance;
                        pLastEffectInstance = pEffectInstance;
                    }
                    pEffect->getPointer()->EndPass( );
                }
                pEffect->getPointer()->End();
            }
        }

        DXUT_EndPerfEvent(); // end of draw code

        // To prevent invalidating any cached state values within the state manager
        // the application may directly use its interface to change state.
        // This ensures that the state manager is able to cache the
        // last known state, in turn ensuring that a true state change is not mistakenly
        // handled as if it were redundant.
        if( g_pStateManager )
        {
            // The Effect State Manager is active, use it to set some states
            g_pStateManager->SetPixelShader( NULL );
        }
        else
        {
            // Otherwise, it is safe to use the runtime directly
            pd3dDevice->SetPixelShader( NULL );
        }

        {
            CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
            V( g_HUD.OnRender( fElapsedTime ) ); 
            V( g_SampleUI.OnRender( fElapsedTime ) );
            
            RenderText( fTime );
        }
        
        // If CDXUTDialog::OnRender or RenderText failed to restore any changed states
        // (that were cached by the custom implementation of ID3DXEffectStateManager),
        // it would be necessary to notifiy the state manager that the device state has
        // been dirtied.  This would be done prior to rendering with the next effect.
        // See CBaseStateManager::DirtyCachedValues.
        // In this case, it may be safely assumed that the device state has been
        // correctly restored between rendering of effects.

        V( pd3dDevice->EndScene() );
    }
    
    nFrameTimeStamp++;
}




//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText( double fTime )
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work fine however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves perf.
    CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 2, 0 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );
    if( g_bRuntimeDebug )
        txtHelper.DrawTextLine( L"WARNING (perf): DEBUG D3D runtime detected!" );
    if( g_bAppIsDebug )
        txtHelper.DrawTextLine( L"WARNING (perf): DEBUG application build detected!" );
    
    
    // Draw help
    if( g_bShowHelp )
    {
        // Comparison of framerates is made fair by hiding help -- this avoids rendering unequal
        // amounts of text depending on the state manager statistics
        txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( g_bSortByMaterial ? L"Sorted By:  Material" : L"Sorted By:  Instance" );
        if( g_pStateManager )
            txtHelper.DrawTextLine( g_pStateManager->EndFrameStats() );

        const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
        txtHelper.SetInsertionPos( 2, pd3dsdBackBuffer->Height-15*6 );
        txtHelper.SetForegroundColor( D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f ) );

        txtHelper.DrawTextLine( L"Controls:" );
        txtHelper.DrawTextLine( L"Look: Left drag mouse\n"
                                L"Move: A,W,S,D or Arrow Keys\n" );

        txtHelper.SetInsertionPos( 250, pd3dsdBackBuffer->Height-15*5 );
        txtHelper.DrawTextLine( L"Hide help: F1\n" 
                                L"Quit: ESC\n" );
    }
    else
    {
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Press F1 for help" );
    }

    txtHelper.End();
}




//--------------------------------------------------------------------------------------
// Before handling window messages, the sample framework passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then the sample framework will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing )
{
    // Give the dialogs a chance to handle the message first
    if( g_HUD.MsgProc( hWnd, uMsg, wParam, lParam ) )
        return 0;
    if( g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam ) )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, the sample framework inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Re-creates the device as being PURE HWVP, or (non-pure) HWVP, depending on the
// current state.
//--------------------------------------------------------------------------------------
void TogglePure()
{
    DXUTDeviceSettings settings = DXUTGetDeviceSettings();
    if( settings.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
    {
        settings.BehaviorFlags ^= D3DCREATE_PUREDEVICE;
        DXUTCreateDeviceFromSettings( &settings, true );
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
    WCHAR sz[100];
    sz[0] = 0;

    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;


        case IDC_NUM_ROCKS:
            g_nMaxRocksToRender =  g_SampleUI.GetSlider( IDC_NUM_ROCKS )->GetValue();

            _snwprintf( sz, 100, L"# Rocks: %d", g_nMaxRocksToRender ); sz[99] = 0;
            g_SampleUI.GetStatic( IDC_NUM_ROCKS_STATIC )->SetText( sz );
            QueueAndSortRenderables();
            break;


        case IDC_TOGGLEPURE:
            TogglePure();
            SetStateManager();
            break;
        
        case IDC_STATEMANAGER:
            g_bStateManger = g_SampleUI.GetCheckBox( IDC_STATEMANAGER )->GetChecked();
            SetStateManager();
            break;

        case IDC_MATERIALSORT:
            g_bSortByMaterial = g_SampleUI.GetCheckBox( IDC_MATERIALSORT )->GetChecked();
            g_bSortOrderDirty = true;
            break;

        case IDC_BIND_CPU:
            UINT n = g_SampleUI.GetSlider( IDC_BIND_CPU )->GetValue();
            
            // square the slider to provide an exponential response
            g_nBindCPU = n*n; 

            _snwprintf( sz, 100, L"Bind CPU: %d", g_nBindCPU ); sz[99] = 0;
            g_SampleUI.GetStatic( IDC_BIND_CPU_STATIC )->SetText( sz );
            break;
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice()
{
    // Notify device resources that the device has been lost
    CTexture::LostDevice();
    CEffect::LostDevice();
    CMeshObject::LostDevice();

    if( g_pFont )
        g_pFont->OnLostDevice();

    SAFE_RELEASE(g_pSprite);
    
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
    SAFE_RELEASE( g_pStateManager );
    SAFE_RELEASE(g_pFont);

    while( !g_vecInstances.empty() )
    {
        CInstance* pInstance = g_vecInstances.back();
        vector<CInstance*>::iterator it_inst = remove( g_vecInstances.begin(), g_vecInstances.end(), pInstance );
        g_vecInstances.erase( it_inst, g_vecInstances.end() );
        SAFE_DELETE( pInstance );
    }
}



//--------------------------------------------------------------------------------------
// Builds the scene from an x-file ('scene.x').
// This x-file has been extended to supply mesh filenames and camera identifiers within
// the frame hierarchy.
//--------------------------------------------------------------------------------------
HRESULT BuildSceneFromX( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;

    // LoadSceneFromX will populate vecFrameNodes with a list of frames and associated
    // objects (meshes, cameras) to be instantiated.
    vector<FRAMENODE> vecFrameNodes;
    
    WCHAR wszPath[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( wszPath, MAX_PATH, L"scene.x" ) );

    // Objects can be added to the scene by invoking:
    // CreateInstance( device, meshfilename, transform matrix, render pass );
    // Here, the scene is read in from an x-file that has been extended to support embedding of mesh filenames.
    // Following the LoadSceneFromFile, container vecFrameNodes will contain the collapsed hierarchy.
    V_RETURN( LoadSceneFromX( vecFrameNodes, wszPath ) );

    // Having loaded (and collapsed) the frame hierarchy, any required meshes are instantiated.
    for( vector<FRAMENODE>::iterator it = vecFrameNodes.begin();
         it != vecFrameNodes.end();
         it++ )
    {
        // retrieve the mesh sub-container for the frame
        vector<MESH_REFERENCE>& vecMeshes = (*it).meshes;

        // each mesh found within the frame is iterated through
        for( vector<MESH_REFERENCE>::iterator it_meshes = vecMeshes.begin();
             it_meshes != vecMeshes.end();
             it_meshes++ )
        {
            // The filename string must be converted
            WCHAR wszFileName[MAX_PATH];
            MultiByteToWideChar( CP_ACP, 0, (*it_meshes).szFileName, -1, wszFileName, MAX_PATH );
            wszFileName[ MAX_PATH -1 ] = 0;
            
            // Create in instance of the mesh.
            // The resource-management classes ensure that multiple references to the same
            // mesh are not loaded more than once.
            // (The same is true for resources used by the mesh -- textures and effects)
            V_RETURN( CreateInstance( pd3dDevice, wszFileName, &(*it).mat, (*it_meshes).dwRenderPass ) );
        }
        
        // If a camera reference was found, Reset the App camera
        if( !(*it).cameras.empty() && g_bResetCamera )
        {
            // Extract the camera class view params from the camera world matrix
            // Note: Assumes NO pitch, NO roll
            D3DXVECTOR3 Eye( (*it).mat._41, (*it).mat._42, (*it).mat._43 );
            D3DXVECTOR3 At( Eye );
            At = Eye - D3DXVECTOR3( (*it).mat._13, (*it).mat._23, (*it).mat._33 ) ;

            g_Camera.SetViewParams( &Eye, &At );
            g_Camera.SetScalers( (*it).cameras[0].fRotationScaler, (*it).cameras[0].fMoveScaler );

            // Do not reset the camera if re-building the scene across a defice change
            g_bResetCamera = false;
        }
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Renderable sub-items (mesh subsets with their associated materials) are maintained
// in a render queue.  This queue allows for sorting by material or instance.
//--------------------------------------------------------------------------------------
void QueueAndSortRenderables()
{
    // All render queue will be regenerated - ensure that the old queues are cleared
    g_passes.clear();

    UINT nRocksCount = 0;

    // Add each mesh/object instance into the correct render queue
    for( vector<CInstance*>::iterator it = g_vecInstances.begin();
         it!= g_vecInstances.end();
         it++ )
    {
        CInstance* pInstance = *it;
        UINT nRenderPass = pInstance->getRenderPass();

        // ensure that enough render passes exist
        if( nRenderPass >= g_passes.size() )
            g_passes.resize( nRenderPass+1 );

        // Constrain the number of rocks in the scene on the maximum limit
        if( !_wcsicmp(pInstance->getMeshObject()->getName().c_str(), L"Rock.x")
            && ++nRocksCount > g_nMaxRocksToRender )
            continue;

        // Add this object's renderables to the render queue
        DWORD dwSubsets = (DWORD) pInstance->getMeshObject()->getSubsetCount();
        for( DWORD dw = 0; dw< dwSubsets; dw++ )
        {
            g_passes[nRenderPass].push_back( CRenderable( pInstance, dw ) );
        }
    }

    if( g_nMaxRocksToRender > nRocksCount )
        g_nMaxRocksToRender = nRocksCount;

    // Update the slider to reflect the true number of rocks
    WCHAR sz[100];
    g_SampleUI.GetSlider( IDC_NUM_ROCKS )->SetRange( 0, nRocksCount );
    _snwprintf( sz, 100, L"# Rocks: %d", g_nMaxRocksToRender ); sz[99] = 0;
    g_SampleUI.GetStatic( IDC_NUM_ROCKS_STATIC )->SetText( sz );

    g_bSortOrderDirty = true;
}


//--------------------------------------------------------------------------------------
// Re-sort the list of renderables (mesh subsets)
// It should be more efficient to render groups of similar materials in batches
// (g_bSortByMaterial).  This should serve to minimize the number of effect changes,
// which in turn should minimize the number of state changes required to render the
// scene.
//--------------------------------------------------------------------------------------
void SortRenderables()
{
    if( g_bSortOrderDirty )
    {
        // Each queue is sorted by material (effect) or instance
        for( vector<pass>::iterator it = g_passes.begin();
            it != g_passes.end();
            it++ )
        {
            sort( (*it).begin(),
                (*it).end(),
                g_bSortByMaterial ? greaterMaterial : greaterInstance );
        }
    }

    g_bSortOrderDirty = false;
}


//--------------------------------------------------------------------------------------
// Helper to create an instance of a mesh to be rendered in the scene
//--------------------------------------------------------------------------------------
HRESULT CreateInstance( LPDIRECT3DDEVICE9 pDevice, LPCWSTR wszFileName,
                        D3DXMATRIX* pWorld, UINT nRenderPass, CInstance** ppInstance )
{
    HRESULT hr;

    CMeshObject* pMesh = NULL;
    V_RETURN( CMeshObject::Create( pDevice, wszFileName, &pMesh ) );

    CInstance* pInstance = new CInstance( pMesh, pWorld, nRenderPass );
    if( NULL == pInstance )
        throw bad_alloc();

    g_vecInstances.push_back( pInstance );

    if( ppInstance )
        *ppInstance = pInstance;

    return S_OK;
}



//--------------------------------------------------------------------------------------
// If g_gStateManger is 'true', a custom handler for D3DX Effect state changes will be
// activated.  Otherwise, any active custom handler will be de-activated.
//--------------------------------------------------------------------------------------
void SetStateManager( )
{
    // Release any reference count on the current custom state manager.
    // If there is no current custom state manager, g_pStateManager will be NULL.
    SAFE_RELEASE( g_pStateManager );

    // If the application no longer wishes to implement state management, it should
    // invoke SetStateManger(NULL) for each effect.
    // This value (NULL) becomes the default case, depending on the application settings.
    CStateManagerInterface* pManager = NULL;
    
    // Create the state manager
    if( g_bStateManger )
        pManager = CStateManagerInterface::Create( DXUTGetD3DDevice() );

    // Set this state manager as active across all loaded effects
    // D3DX will release the old state manager for each effect, and invoke AddRef() to
    // increment the reference count of the new state manager.
    CEffect::SetStateManager( pManager );

    // Because D3DX maintains a reference count on this state manager while in use,
    // it is possible to release the app's reference here.  Doing so allows the app
    // to 'forget' about the state manager and let d3dx manager the lifetime of it.
    // SAFE_RELEASE( pManager );

    // However, in this case it is desireable to maintain the pointer to the state
    // manager, such that it can be queried for statistics while in use.
    g_pStateManager = pManager;
}


