//--------------------------------------------------------------------------------------
// File: CustomUI.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
CDXUTMesh               g_Mesh;                 // Background mesh
D3DXMATRIXA16           g_mView;
CModelViewerCamera      g_Camera;               // A model viewing camera
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_EDITBOX1            5
#define IDC_EDITBOX2            6
#define IDC_ENABLEIME           7
#define IDC_DISABLEIME          8
#define IDC_COMBOBOX            9
#define IDC_STATIC              10
#define IDC_OUTPUT              12
#define IDC_SLIDER              13
#define IDC_CHECKBOX            14
#define IDC_CLEAREDIT           15
#define IDC_RADIO1A             16
#define IDC_RADIO1B             17
#define IDC_RADIO1C             18
#define IDC_RADIO2A             19
#define IDC_RADIO2B             20
#define IDC_RADIO2C             21
#define IDC_LISTBOX             22
#define IDC_LISTBOXM            23


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
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
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
    DXUTCreateWindow( L"CustomUI" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

    g_SampleUI.SetCallback( OnGUIEvent );
    g_SampleUI.SetFont( 1, L"Comic Sans MS", 24, FW_NORMAL );
    g_SampleUI.SetFont( 2, L"Courier New", 16, FW_NORMAL );

    // Static
    g_SampleUI.AddStatic( IDC_STATIC, L"This is a static control.", 0, 0, 200, 30 );
    g_SampleUI.AddStatic( IDC_OUTPUT, L"This static control provides feedback for your action.  It will change as you interact with the UI controls.", 20, 50, 620, 300 );
    g_SampleUI.GetStatic( IDC_OUTPUT )->SetTextColor( D3DCOLOR_ARGB( 255, 255, 0, 0 ) ); // Change color to red
    g_SampleUI.GetStatic( IDC_STATIC )->SetTextColor( D3DCOLOR_ARGB( 255, 0, 255, 0 ) ); // Change color to green
    g_SampleUI.GetControl( IDC_OUTPUT )->GetElement( 0 )->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
    g_SampleUI.GetControl( IDC_OUTPUT )->GetElement( 0 )->iFont = 2;
    g_SampleUI.GetControl( IDC_STATIC )->GetElement( 0 )->dwTextFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK;

    // Buttons
    g_SampleUI.AddButton( IDC_ENABLEIME, L"Enable IME", 10, 390, 80, 35 );
    g_SampleUI.AddButton( IDC_DISABLEIME, L"Disable IME", 10, 430, 80, 35 );

    // Edit box
    g_SampleUI.AddEditBox( IDC_EDITBOX1, L"Edit control with default styles. Type text here and press Enter", 20, 440, 600, 32 );

    // IME-enabled edit box
    CDXUTIMEEditBox *pIMEEdit;
    if( SUCCEEDED( g_SampleUI.AddIMEEditBox( IDC_EDITBOX2, L"IME-capable edit control with custom styles. Type and press Enter", 20, 390, 600, 45, false, &pIMEEdit ) ) )
    {
        pIMEEdit->GetElement( 0 )->iFont = 1;
        pIMEEdit->GetElement( 1 )->iFont = 1;
        pIMEEdit->GetElement( 9 )->iFont = 1;
        pIMEEdit->GetElement( 0 )->TextureColor.Init( D3DCOLOR_ARGB( 128, 255, 255, 255 ) );  // Transparent center
        pIMEEdit->SetBorderWidth( 7 );
        pIMEEdit->SetTextColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
        pIMEEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
        pIMEEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
        pIMEEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 40, 72, 72 ) );
    }

    // Slider
    g_SampleUI.AddSlider( IDC_SLIDER, 200, 450, 200, 24, 0, 100, 50, false );

    // Checkbox
    g_SampleUI.AddCheckBox( IDC_CHECKBOX, L"This is a checkbox with hotkey. Press 'C' to toggle the check state.",
                            150, 450, 350, 24, false, L'C', false );
    g_SampleUI.AddCheckBox( IDC_CLEAREDIT, L"This checkbox controls whether edit control text is cleared when Enter is pressed.",
                            150, 460, 430, 24, false, 0, false );

    // Combobox
    CDXUTComboBox *pCombo;
    g_SampleUI.AddComboBox( IDC_COMBOBOX, 0, 0, 200, 24, 0, false, &pCombo );
    if( pCombo )
    {
        pCombo->SetDropHeight( 100 );
        pCombo->AddItem( L"Combobox item", (LPVOID)0x11111111 );
        pCombo->AddItem( L"Placeholder", (LPVOID)0x12121212 );
        pCombo->AddItem( L"One more", (LPVOID)0x13131313 );
        pCombo->AddItem( L"I can't get enough", (LPVOID)0x14141414 );
        pCombo->AddItem( L"Ok, last one, I promise", (LPVOID)0x15151515 );
    }

    // Radio buttons
    g_SampleUI.AddRadioButton( IDC_RADIO1A, 1, L"Radio group 1 Amy", 0, 50, 200, 24 );
    g_SampleUI.AddRadioButton( IDC_RADIO1B, 1, L"Radio group 1 Brian", 0, 50, 200, 24 );
    g_SampleUI.AddRadioButton( IDC_RADIO1C, 1, L"Radio group 1 Clark", 0, 50, 200, 24 );

    g_SampleUI.AddRadioButton( IDC_RADIO2A, 2, L"Single", 0, 50, 70, 24 );
    g_SampleUI.AddRadioButton( IDC_RADIO2B, 2, L"Double", 0, 50, 70, 24 );
    g_SampleUI.AddRadioButton( IDC_RADIO2C, 2, L"Triple", 0, 50, 70, 24 );

    // List box
    g_SampleUI.AddListBox( IDC_LISTBOX, 30, 400, 200, 150, 0 );
    for( int i = 0; i < 15; ++i )
    {
        WCHAR wszText[50];
        swprintf( wszText, L"Single-selection listbox item %d", i );
        g_SampleUI.GetListBox( IDC_LISTBOX )->AddItem( wszText, (LPVOID)(size_t)i );
    }
    g_SampleUI.AddListBox( IDC_LISTBOXM, 30, 400, 200, 150, CDXUTListBox::MULTISELECTION );
    for( int i = 0; i < 30; ++i )
    {
        WCHAR wszText[50];
        swprintf( wszText, L"Multi-selection listbox item %d", i );
        g_SampleUI.GetListBox( IDC_LISTBOXM )->AddItem( wszText, (LPVOID)(size_t)i );
    }
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Must support vertex shader 1.1
    if( pCaps->VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        return false;

    // Must support pixel shader 1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
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
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    else
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    // This application is designed to work on a pure device by not using 
    // IDirect3D9::Get*() methods, so create a pure device if supported and using HWVP.
    if ((pCaps->DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 && 
        (pDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 )
        pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif
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
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"CustomUI.fx" ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &g_pEffect, NULL ) );

    g_Mesh.Create( pd3dDevice, L"misc\\cell.x" );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 1.5f, -7.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.2f, 0.0f);
    D3DXVECTOR3 vecUp (0.0f, 1.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );
    D3DXMatrixLookAtLH( &g_mView, &vecEye, &vecAt, &vecUp );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing 
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
{
    ID3DXMesh* pMesh = NULL;
    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
    // sample we'll ignore the X file's embedded materials since we know 
    // exactly the model we're loading.  See the mesh samples such as
    // "OptimizedMesh" for a more generic mesh loading example.
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );

    V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        ID3DXMesh* pTempMesh;
        V( pMesh->CloneMeshFVF( pMesh->GetOptions(), 
                                  pMesh->GetFVF() | D3DFVF_NORMAL, 
                                  pd3dDevice, &pTempMesh ) );
        V( D3DXComputeNormals( pTempMesh, NULL ) );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improve perf.     
    rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
    delete []rgdwAdjacency;

    *ppMesh = pMesh;

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

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( 0, 0 );
    g_SampleUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_SampleUI.GetControl( IDC_STATIC )->SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height * 6 / 10 );
    g_SampleUI.GetControl( IDC_OUTPUT )->SetSize( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height / 4 );
    g_SampleUI.GetControl( IDC_EDITBOX1 )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 230 );
    g_SampleUI.GetControl( IDC_EDITBOX1 )->SetSize( pBackBufferSurfaceDesc->Width - 40, 32 );
    g_SampleUI.GetControl( IDC_EDITBOX2 )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 280 );
    g_SampleUI.GetControl( IDC_EDITBOX2 )->SetSize( pBackBufferSurfaceDesc->Width - 40, 45 );
    g_SampleUI.GetControl( IDC_ENABLEIME )->SetLocation( 110, pBackBufferSurfaceDesc->Height - 80 );
    g_SampleUI.GetControl( IDC_DISABLEIME )->SetLocation( 200, pBackBufferSurfaceDesc->Height - 80 );
    g_SampleUI.GetControl( IDC_SLIDER )->SetLocation( 10, pBackBufferSurfaceDesc->Height - 140 );
    g_SampleUI.GetControl( IDC_CHECKBOX )->SetLocation( 100, pBackBufferSurfaceDesc->Height - 50 );
    g_SampleUI.GetControl( IDC_CLEAREDIT )->SetLocation( 100, pBackBufferSurfaceDesc->Height - 25 );
    g_SampleUI.GetControl( IDC_COMBOBOX )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 180 );
    g_SampleUI.GetControl( IDC_RADIO1A )->SetLocation( pBackBufferSurfaceDesc->Width - 140, 100 );
    g_SampleUI.GetControl( IDC_RADIO1B )->SetLocation( pBackBufferSurfaceDesc->Width - 140, 124 );
    g_SampleUI.GetControl( IDC_RADIO1C )->SetLocation( pBackBufferSurfaceDesc->Width - 140, 148 );
    g_SampleUI.GetControl( IDC_RADIO2A )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 100 );
    g_SampleUI.GetControl( IDC_RADIO2B )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 76 );
    g_SampleUI.GetControl( IDC_RADIO2C )->SetLocation( 20, pBackBufferSurfaceDesc->Height - 52 );
    g_SampleUI.GetControl( IDC_LISTBOX )->SetLocation( pBackBufferSurfaceDesc->Width - 400, pBackBufferSurfaceDesc->Height - 180 );
    g_SampleUI.GetControl( IDC_LISTBOX )->SetSize( 190, 96 );
    g_SampleUI.GetControl( IDC_LISTBOXM )->SetLocation( pBackBufferSurfaceDesc->Width - 200, pBackBufferSurfaceDesc->Height - 180 );
    g_SampleUI.GetControl( IDC_LISTBOXM )->SetSize( 190, 124 );
    g_Mesh.RestoreDeviceObjects( pd3dDevice );

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
    D3DXMATRIXA16 m;
    D3DXMatrixRotationY( &m, D3DX_PI * fElapsedTime / 40.0f );
    D3DXMatrixMultiply( &g_mView, &m, &g_mView );
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
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;
    
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Get the projection & view matrix from the camera class
        mWorld = *g_Camera.GetWorldMatrix();       
        mProj = *g_Camera.GetProjMatrix();       
        mView = g_mView;

        mWorldViewProjection = mWorld * mView * mProj;

        // Update the effect's variables.  Instead of using strings, it would 
        // be more efficient to cache a handle to the parameter by calling 
        // ID3DXEffect::GetParameterByName
        V( g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
        V( g_pEffect->SetMatrix( "g_mWorld", &mWorld ) );
        V( g_pEffect->SetFloat( "g_fTime", (float)fTime ) );

        g_pEffect->SetTechnique( "RenderScene" );
        UINT cPasses;
        g_pEffect->Begin( &cPasses, 0 );
        ID3DXMesh *pMesh = g_Mesh.GetLocalMesh();
        for( UINT p = 0; p < cPasses; ++p )
        {
            g_pEffect->BeginPass( p );
            for( UINT m = 0; m < g_Mesh.m_dwNumMaterials; ++m )
            {
                g_pEffect->SetTexture( "g_txScene", g_Mesh.m_pTextures[m] );
                g_pEffect->CommitChanges();
                pMesh->DrawSubset( m );
            }
            g_pEffect->EndPass();
        }
        g_pEffect->End();

        RenderText();
        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    
    // Draw help
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    txtHelper.DrawTextLine( L"Press ESC to quit" );
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
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
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
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
    WCHAR wszOutput[1024];

    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;
        case IDC_ENABLEIME:
            CDXUTIMEEditBox::EnableImeSystem( true );
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( L"You clicked the 'Enable IME' button.\nIME text input is enabled for IME-capable edit controls." );
            break;
        case IDC_DISABLEIME:
            CDXUTIMEEditBox::EnableImeSystem( false );
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( L"You clicked the 'Disable IME' button.\nIME text input is disabled for IME-capable edit controls." );
            break;
        case IDC_EDITBOX1:
        case IDC_EDITBOX2:
            switch( nEvent )
            {
                case EVENT_EDITBOX_STRING:
                {
                    _snwprintf( wszOutput, 1024, L"You have pressed Enter in edit control (ID %u).\nThe content of the edit control is:\n\"%s\"",
                                nControlID, ((CDXUTEditBox*)pControl)->GetText() );
                    wszOutput[1023] = 0;
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );

                    // Clear the text if needed
                    if( g_SampleUI.GetCheckBox( IDC_CLEAREDIT )->GetChecked() )
                        ((CDXUTEditBox*)pControl)->SetText( L"" );
                    break;
                }

                case EVENT_EDITBOX_CHANGE:
                {
                    _snwprintf( wszOutput, 1024, L"You have changed the content of an edit control (ID %u).\nIt is now:\n\"%s\"",
                                nControlID, ((CDXUTEditBox*)pControl)->GetText() );
                    wszOutput[1023] = 0;
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );

                    break;
                }
            }
            break;
        case IDC_SLIDER:
            _snwprintf( wszOutput, 1024, L"You adjusted the slider control.\nThe new value reported is %d",
                        ((CDXUTSlider*)pControl)->GetValue() );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;
        case IDC_CHECKBOX:
            _snwprintf( wszOutput, 1024, L"You %s the upper check box.",
                ((CDXUTCheckBox*)pControl)->GetChecked() ? L"checked" : L"cleared" );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;
        case IDC_CLEAREDIT:
            _snwprintf( wszOutput, 1024, L"You %s the lower check box.\nNow edit controls will %s",
                        ((CDXUTCheckBox*)pControl)->GetChecked() ? L"checked" : L"cleared",
                        ((CDXUTCheckBox*)pControl)->GetChecked() ? L"be cleared when you press Enter to send the text" : L"retain the text context when you press Enter to send the text" );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;
        case IDC_COMBOBOX:
        {
            DXUTComboBoxItem *pItem = ((CDXUTComboBox*)pControl)->GetSelectedItem();
            _snwprintf( wszOutput, 1024, L"You selected a new item in the combobox.\nThe new item is \"%s\" and the associated data value is 0x%p",
                        pItem->strText, pItem->pData );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;
        }
        case IDC_RADIO1A:
        case IDC_RADIO1B:
        case IDC_RADIO1C:
            _snwprintf( wszOutput, 1024, L"You selected a new radio button in the UPPER radio group.\nThe new button is \"%s\"",
                ((CDXUTRadioButton*)pControl)->GetText() );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;
        case IDC_RADIO2A:
        case IDC_RADIO2B:
        case IDC_RADIO2C:
            _snwprintf( wszOutput, 1024, L"You selected a new radio button in the LOWER radio group.\nThe new button is \"%s\"",
                ((CDXUTRadioButton*)pControl)->GetText() );
            wszOutput[1023] = L'\0';
            g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
            break;

        case IDC_LISTBOX:
            switch( nEvent )
            {
                case EVENT_LISTBOX_ITEM_DBLCLK:
                {
                    DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );

                    _snwprintf( wszOutput, 1024, L"You double clicked an item in the left list box.  The item is\n\"%s\"",
                        pItem ? pItem->strText : L"" );
                    wszOutput[1023] = L'\0';
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
                    break;
                }

                case EVENT_LISTBOX_SELECTION:
                {
                    _snwprintf( wszOutput, 1024, L"You changed the selection in the left list box.  The selected item is %d", ((CDXUTListBox *)pControl)->GetSelectedIndex() );
                    wszOutput[1023] = L'\0';
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
                    break;
                }
            }
            break;

        case IDC_LISTBOXM:
            switch( nEvent )
            {
                case EVENT_LISTBOX_ITEM_DBLCLK:
                {
                    DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );

                    _snwprintf( wszOutput, 1024, L"You double clicked an item in the right list box.  The item is\n\"%s\"",
                        pItem ? pItem->strText : L"" );
                    wszOutput[1023] = L'\0';
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
                    break;
                }

                case EVENT_LISTBOX_SELECTION:
                {
                    _snwprintf( wszOutput, 1024, L"You changed the selection in the right list box.  The selected item(s) are\n" );
                    wszOutput[1023] = L'\0';
                    int nSelected = -1;
                    while( ( nSelected = ((CDXUTListBox *)pControl)->GetSelectedIndex( nSelected ) ) != -1 )
                    {
                        _snwprintf( wszOutput + lstrlenW( wszOutput ), 1024 - lstrlenW( wszOutput ), L"%d,", nSelected );
                    }
                    // Remove the trailing comma if one exists.
                    if( wszOutput[lstrlenW(wszOutput)-1] == L',' )
                        wszOutput[lstrlenW(wszOutput)-1] = L'\0';
                    g_SampleUI.GetStatic( IDC_OUTPUT )->SetText( wszOutput );
                    break;
                }
            }
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
    g_Mesh.InvalidateDeviceObjects();

    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    SAFE_RELEASE(g_pTextSprite);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
    g_Mesh.Destroy();

    SAFE_RELEASE(g_pEffect);
    SAFE_RELEASE(g_pFont);
}



