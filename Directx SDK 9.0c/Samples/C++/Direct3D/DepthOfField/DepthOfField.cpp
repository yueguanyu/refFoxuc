//--------------------------------------------------------------------------------------
// File: DepthOfField.cpp
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
// Vertex format
//--------------------------------------------------------------------------------------
struct VERTEX 
{
    D3DXVECTOR4 pos;
    DWORD       clr;
    D3DXVECTOR2 tex1;
    D3DXVECTOR2 tex2;
    D3DXVECTOR2 tex3;
    D3DXVECTOR2 tex4;
    D3DXVECTOR2 tex5;
    D3DXVECTOR2 tex6;

    static const DWORD FVF;
};
const DWORD VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX6;


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
CFirstPersonCamera      g_Camera;               // A model viewing camera
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls

VERTEX                  g_Vertex[4];

LPDIRECT3DTEXTURE9      g_pFullScreenTexture;
LPD3DXRENDERTOSURFACE   g_pRenderToSurface;
LPDIRECT3DSURFACE9      g_pFullScreenTextureSurf;

LPD3DXMESH              g_pScene1Mesh;
LPDIRECT3DTEXTURE9      g_pScene1MeshTexture;
LPD3DXMESH              g_pScene2Mesh;
LPDIRECT3DTEXTURE9      g_pScene2MeshTexture;
int                     g_nCurrentScene;
LPD3DXEFFECT            g_pEffect;

D3DXVECTOR4             g_vFocalPlane;
double                  g_fChangeTime;
int                     g_nShowMode;
DWORD                   g_dwBackgroundColor;

D3DVIEWPORT9            g_ViewportFB;
D3DVIEWPORT9            g_ViewportOffscreen;

FLOAT                   g_fBlurConst;
DWORD                   g_TechniqueIndex;

D3DXHANDLE              g_hFocalPlane;
D3DXHANDLE              g_hWorld;
D3DXHANDLE              g_hWorldView;
D3DXHANDLE              g_hWorldViewProjection;
D3DXHANDLE              g_hMeshTexture;
D3DXHANDLE              g_hTechWorldWithBlurFactor;
D3DXHANDLE              g_hTechShowBlurFactor;
D3DXHANDLE              g_hTechShowUnmodified;
D3DXHANDLE              g_hTech[5];

static CHAR* g_TechniqueNames[] = { "UsePS20ThirteenLookups",
                                    "UsePS20SevenLookups",
                                    "UsePS20SixTexcoords",
                                    "UsePS11FourTexcoordsNoRings",
                                    "UsePS11FourTexcoordsWithRings" };
const DWORD g_TechniqueCount = sizeof(g_TechniqueNames)/sizeof(LPCSTR);



//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_CHANGE_SCENE        5
#define IDC_CHANGE_TECHNIQUE    6
#define IDC_SHOW_BLUR           7
#define IDC_CHANGE_BLUR         8
#define IDC_CHANGE_FOCAL        9
#define IDC_CHANGE_BLUR_STATIC  10
#define IDC_SHOW_UNBLURRED      11
#define IDC_SHOW_NORMAL         12
#define IDC_CHANGE_FOCAL_STATIC 13



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
void    SetupQuad( const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT UpdateTechniqueSpecificVariables( const D3DSURFACE_DESC* pBackBufferSurfaceDesc );


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
    DXUTCreateWindow( L"DepthOfField" );
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
    g_pFont = NULL;

    g_pFullScreenTexture = NULL;
    g_pFullScreenTextureSurf = NULL;
    g_pRenderToSurface = NULL;
    g_pEffect = NULL;

    g_vFocalPlane = D3DXVECTOR4(0.0f, 0.0f, 1.0f, -2.5f);
    g_fChangeTime = 0.0f;

    g_pScene1Mesh = NULL;
    g_pScene1MeshTexture = NULL;
    g_pScene2Mesh = NULL;
    g_pScene2MeshTexture = NULL;
    g_nCurrentScene = 1;

    g_nShowMode = 0;
    g_bShowHelp = TRUE;
    g_dwBackgroundColor = 0x00003F3F;

    g_fBlurConst = 4.0f;
    g_TechniqueIndex = 0;

    g_hFocalPlane = NULL;
    g_hWorld = NULL;
    g_hWorldView = NULL;
    g_hWorldViewProjection = NULL;
    g_hMeshTexture = NULL;
    g_hTechWorldWithBlurFactor = NULL;
    g_hTechShowBlurFactor = NULL;
    g_hTechShowUnmodified = NULL;
    ZeroMemory( g_hTech, sizeof(g_hTech) );

    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10; 
    g_SampleUI.AddButton( IDC_CHANGE_SCENE, L"Change Scene", 35, iY += 24, 125, 22, 'P' );
    g_SampleUI.AddButton( IDC_CHANGE_TECHNIQUE, L"Change Technique", 35, iY += 24, 125, 22, 'N' );
    g_SampleUI.AddRadioButton( IDC_SHOW_NORMAL, 1, L"Show Normal", 35, iY += 24, 125, 22, true );
    g_SampleUI.AddRadioButton( IDC_SHOW_BLUR, 1, L"Show Blur Factor", 35, iY += 24, 125, 22 );
    g_SampleUI.AddRadioButton( IDC_SHOW_UNBLURRED, 1, L"Show Unblurred", 35, iY += 24, 125, 22 );

    iY += 24;
    WCHAR sz[100];
    _snwprintf( sz, 100, L"Focal Distance: %0.2f", -g_vFocalPlane.w ); sz[99] = 0;
    g_SampleUI.AddStatic( IDC_CHANGE_FOCAL_STATIC, sz, 35, iY += 24, 125, 22 );
    g_SampleUI.AddSlider( IDC_CHANGE_FOCAL, 50, iY += 24, 100, 22, 0, 100, (int) (-g_vFocalPlane.w*10.0f) );

    iY += 24;
    _snwprintf( sz, 100, L"Blur Factor: %0.2f", g_fBlurConst ); sz[99] = 0;
    g_SampleUI.AddStatic( IDC_CHANGE_BLUR_STATIC, sz, 35, iY += 24, 125, 22 );
    g_SampleUI.AddSlider( IDC_CHANGE_BLUR, 50, iY += 24, 100, 22, 0, 100, (int) (g_fBlurConst*10.0f) );
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
    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

    // Load the meshs
    V_RETURN( LoadMesh( pd3dDevice, TEXT("tiger\\tiger.x"), &g_pScene1Mesh ) );
    V_RETURN( LoadMesh( pd3dDevice, TEXT("misc\\sphere.x"), &g_pScene2Mesh ) );

    // Load the mesh textures
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"tiger\\tiger.bmp" ) );
    V_RETURN( D3DXCreateTextureFromFile( pd3dDevice, str, &g_pScene1MeshTexture) );

    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"earth\\earth.bmp" ) );
    V_RETURN( D3DXCreateTextureFromFile( pd3dDevice, str, &g_pScene2MeshTexture) );

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
    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"DepthOfField.fx" ) );
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &g_pEffect, NULL ) );

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

    // Setup the camera with view & projection matrix
    D3DXVECTOR3 vecEye(1.3f, 1.1f, -3.3f);
    D3DXVECTOR3 vecAt (0.75f, 0.9f, -2.5f);
    g_Camera.SetViewParams( &vecEye, &vecAt );
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DXToRadian(60.0f), fAspectRatio, 0.5f, 100.0f );

    pd3dDevice->GetViewport(&g_ViewportFB);

    // Backbuffer viewport is identical to frontbuffer, except starting at 0, 0
    g_ViewportOffscreen = g_ViewportFB;
    g_ViewportOffscreen.X = 0;
    g_ViewportOffscreen.Y = 0;

    // Create fullscreen renders target texture
    hr = D3DXCreateTexture( pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 
                            1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pFullScreenTexture);
    if( FAILED(hr) )
    {
        // Fallback to a non-RT texture
        V_RETURN( D3DXCreateTexture( pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 
                                     1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pFullScreenTexture ) );
    }

    D3DSURFACE_DESC desc;
    g_pFullScreenTexture->GetSurfaceLevel(0, &g_pFullScreenTextureSurf);
    g_pFullScreenTextureSurf->GetDesc(&desc);

    // Create a ID3DXRenderToSurface to help render to a texture on cards 
    // that don't support render targets
    V_RETURN( D3DXCreateRenderToSurface( pd3dDevice, desc.Width, desc.Height, 
                                         desc.Format, TRUE, D3DFMT_D16, &g_pRenderToSurface ) );

    // clear the surface alpha to 0 so that it does not bleed into a "blurry" background
    //   this is possible because of the avoidance of blurring in a non-blurred texel
    if(SUCCEEDED(g_pRenderToSurface->BeginScene(g_pFullScreenTextureSurf, NULL)))
    {
        pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0);
        g_pRenderToSurface->EndScene( 0 );
    }

    D3DXCOLOR colorWhite(1.0f, 1.0f, 1.0f, 1.0f);
    D3DXCOLOR colorBlack(0.0f, 0.0f, 0.0f, 1.0f);
    D3DXCOLOR colorAmbient(0.25f, 0.25f, 0.25f, 1.0f);

    // Get D3DXHANDLEs to the parameters/techniques that are set every frame so 
    // D3DX doesn't spend time doing string compares.  Doing this likely won't affect
    // the perf of this simple sample but it should be done in complex engine.
    g_hFocalPlane               = g_pEffect->GetParameterByName( NULL, "vFocalPlane" );
    g_hWorld                    = g_pEffect->GetParameterByName( NULL, "mWorld" );
    g_hWorldView                = g_pEffect->GetParameterByName( NULL, "mWorldView" );
    g_hWorldViewProjection      = g_pEffect->GetParameterByName( NULL, "mWorldViewProjection" );
    g_hMeshTexture              = g_pEffect->GetParameterByName( NULL, "MeshTexture" );
    g_hTechWorldWithBlurFactor  = g_pEffect->GetTechniqueByName("WorldWithBlurFactor");
    g_hTechShowBlurFactor       = g_pEffect->GetTechniqueByName("ShowBlurFactor");
    g_hTechShowUnmodified       = g_pEffect->GetTechniqueByName("ShowUnmodified");
    for( int i=0; i<5; i++ )
        g_hTech[i] = g_pEffect->GetTechniqueByName( g_TechniqueNames[i] );

    // Set the vars in the effect that doesn't change each frame
    V_RETURN( g_pEffect->SetVector("MaterialAmbientColor", (D3DXVECTOR4*)&colorAmbient) );
    V_RETURN( g_pEffect->SetVector("MaterialDiffuseColor", (D3DXVECTOR4*)&colorWhite) );
    V_RETURN( g_pEffect->SetTexture("RenderTargetTexture", g_pFullScreenTexture) );

    // Check if the current technique is valid for the new device/settings
    // Start from the current technique, increment until we find one we can use.
    DWORD OriginalTechnique = g_TechniqueIndex;
    do
    {
        if( g_TechniqueIndex == g_TechniqueCount )
            g_TechniqueIndex = 0;

        D3DXHANDLE hTech = g_pEffect->GetTechniqueByName( g_TechniqueNames[g_TechniqueIndex] );
        if( SUCCEEDED( g_pEffect->ValidateTechnique( hTech ) ) )
            break;

        g_TechniqueIndex++;
    } 
    while( OriginalTechnique != g_TechniqueIndex );

    V_RETURN( UpdateTechniqueSpecificVariables( pBackBufferSurfaceDesc ) );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-300 );
    g_SampleUI.SetSize( 170, 250 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Certain parameters need to be specified for specific techniques
//--------------------------------------------------------------------------------------
HRESULT UpdateTechniqueSpecificVariables( const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    LPCSTR strInputArrayName, strOutputArrayName;
    int nNumKernelEntries;
    HRESULT hr;
    D3DXHANDLE hAnnotation;

    // Create the post-process quad and set the texcoords based on the blur factor
    SetupQuad( pBackBufferSurfaceDesc );

    // Get the handle to the current technique
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName(g_TechniqueNames[g_TechniqueIndex]);   
    if(hTech == NULL)
        return S_FALSE; // This will happen if the technique doesn't have this annotation

    // Get the value of the annotation int named "NumKernelEntries" inside the technique
    hAnnotation = g_pEffect->GetAnnotationByName(hTech, "NumKernelEntries");
    if( hAnnotation == NULL )
        return S_FALSE; // This will happen if the technique doesn't have this annotation
    V_RETURN( g_pEffect->GetInt(hAnnotation, &nNumKernelEntries) );

    // Get the value of the annotation string named "KernelInputArray" inside the technique
    hAnnotation = g_pEffect->GetAnnotationByName(hTech, "KernelInputArray");
    if( hAnnotation == NULL )
        return S_FALSE; // This will happen if the technique doesn't have this annotation
    V_RETURN( g_pEffect->GetString( hAnnotation, &strInputArrayName) );

    // Get the value of the annotation string named "KernelOutputArray" inside the technique
    hAnnotation = g_pEffect->GetAnnotationByName(hTech, "KernelOutputArray");
    if( hAnnotation == NULL )
        return S_FALSE; // This will happen if the technique doesn't have this annotation
    if( FAILED( hr = g_pEffect->GetString( hAnnotation, &strOutputArrayName) ) )
        return hr;

    // Create a array to store the input array
    D3DXVECTOR2* aKernel = new D3DXVECTOR2[nNumKernelEntries];
    if (aKernel == NULL)
        return E_OUTOFMEMORY;

    // Get the input array
    V_RETURN( g_pEffect->GetValue(strInputArrayName, aKernel, sizeof(D3DXVECTOR2) * nNumKernelEntries) );

    // Get the size of the texture
    D3DSURFACE_DESC desc;
    g_pFullScreenTextureSurf->GetDesc(&desc);

    // Calculate the scale factor to convert the input array to screen space
    FLOAT fWidthMod = g_fBlurConst / (FLOAT)desc.Width ;
    FLOAT fHeightMod = g_fBlurConst / (FLOAT)desc.Height;

    // Scale the effect's kernel from pixel space to tex coord space
    // In pixel space 1 unit = one pixel and in tex coord 1 unit = width/height of texture
    for( int iEntry = 0; iEntry < nNumKernelEntries; iEntry++ )
    {
        aKernel[iEntry].x *= fWidthMod;
        aKernel[iEntry].y *= fHeightMod;
    }

    // Pass the updated array values to the effect file
    V_RETURN( g_pEffect->SetValue(strOutputArrayName, aKernel, sizeof(D3DXVECTOR2) * nNumKernelEntries) );

    SAFE_DELETE_ARRAY( aKernel );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Sets up a quad to render the fullscreen render target to the backbuffer
// so it can run a fullscreen pixel shader pass that blurs based
// on the depth of the objects.  It set the texcoords based on the blur factor
//--------------------------------------------------------------------------------------
void SetupQuad( const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    D3DSURFACE_DESC desc;
    g_pFullScreenTextureSurf->GetDesc(&desc);

    FLOAT fWidth5 = (FLOAT)pBackBufferSurfaceDesc->Width - 0.5f;
    FLOAT fHeight5 = (FLOAT)pBackBufferSurfaceDesc->Height - 0.5f;

    FLOAT fHalf = g_fBlurConst;
    FLOAT fOffOne = fHalf * 0.5f;
    FLOAT fOffTwo = fOffOne * sqrtf(3.0f);

    FLOAT fTexWidth1 = (FLOAT)pBackBufferSurfaceDesc->Width / (FLOAT)desc.Width;
    FLOAT fTexHeight1 = (FLOAT)pBackBufferSurfaceDesc->Height / (FLOAT)desc.Height;

    FLOAT fWidthMod = 1.0f / (FLOAT)desc.Width ;
    FLOAT fHeightMod = 1.0f / (FLOAT)desc.Height;

    // Create vertex buffer.  
    // g_Vertex[0].tex1 == full texture coverage
    // g_Vertex[0].tex2 == full texture coverage, but shifted y by -fHalf*fHeightMod
    // g_Vertex[0].tex3 == full texture coverage, but shifted x by -fOffTwo*fWidthMod & y by -fOffOne*fHeightMod
    // g_Vertex[0].tex4 == full texture coverage, but shifted x by +fOffTwo*fWidthMod & y by -fOffOne*fHeightMod
    // g_Vertex[0].tex5 == full texture coverage, but shifted x by -fOffTwo*fWidthMod & y by +fOffOne*fHeightMod
    // g_Vertex[0].tex6 == full texture coverage, but shifted x by +fOffTwo*fWidthMod & y by +fOffOne*fHeightMod
    g_Vertex[0].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.0f, 1.0f);
    g_Vertex[0].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[0].tex1 = D3DXVECTOR2(fTexWidth1, 0.0f);
    g_Vertex[0].tex2 = D3DXVECTOR2(fTexWidth1, 0.0f - fHalf*fHeightMod);
    g_Vertex[0].tex3 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
    g_Vertex[0].tex4 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
    g_Vertex[0].tex5 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);
    g_Vertex[0].tex6 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);

    g_Vertex[1].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.0f, 1.0f);
    g_Vertex[1].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[1].tex1 = D3DXVECTOR2(fTexWidth1, fTexHeight1);
    g_Vertex[1].tex2 = D3DXVECTOR2(fTexWidth1, fTexHeight1 - fHalf*fHeightMod);
    g_Vertex[1].tex3 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
    g_Vertex[1].tex4 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
    g_Vertex[1].tex5 = D3DXVECTOR2(fTexWidth1 - fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
    g_Vertex[1].tex6 = D3DXVECTOR2(fTexWidth1 + fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);

    g_Vertex[2].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
    g_Vertex[2].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[2].tex1 = D3DXVECTOR2(0.0f, 0.0f);
    g_Vertex[2].tex2 = D3DXVECTOR2(0.0f, 0.0f - fHalf*fHeightMod);
    g_Vertex[2].tex3 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
    g_Vertex[2].tex4 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, 0.0f - fOffOne*fHeightMod);
    g_Vertex[2].tex5 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);
    g_Vertex[2].tex6 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, 0.0f + fOffOne*fHeightMod);

    g_Vertex[3].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.0f, 1.0f);
    g_Vertex[3].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[3].tex1 = D3DXVECTOR2(0.0f, fTexHeight1);
    g_Vertex[3].tex2 = D3DXVECTOR2(0.0f, fTexHeight1 - fHalf*fHeightMod);
    g_Vertex[3].tex3 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
    g_Vertex[3].tex4 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, fTexHeight1 - fOffOne*fHeightMod);
    g_Vertex[3].tex5 = D3DXVECTOR2(0.0f + fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
    g_Vertex[3].tex6 = D3DXVECTOR2(0.0f - fOffTwo*fWidthMod, fTexHeight1 + fOffOne*fHeightMod);
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
    UINT iPass, cPasses;

    // First render the world on the rendertarget g_pFullScreenTexture. 
    if( SUCCEEDED( g_pRenderToSurface->BeginScene(g_pFullScreenTextureSurf, &g_ViewportOffscreen) ) )
    {
        V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, g_dwBackgroundColor, 1.0f, 0) );

        // Get the view & projection matrix from camera
        D3DXMATRIXA16 matWorld;
        D3DXMATRIXA16 matView = *g_Camera.GetViewMatrix();
        D3DXMATRIXA16 matProj = *g_Camera.GetProjMatrix();
        D3DXMATRIXA16 matViewProj = matView * matProj;

        // Update focal plane
        g_pEffect->SetVector( g_hFocalPlane, &g_vFocalPlane);

        // Set world render technique
        V( g_pEffect->SetTechnique( g_hTechWorldWithBlurFactor ) );

        // Set the mesh texture 
        LPD3DXMESH pSceneMesh;
        int nNumObjectsInScene;
        if( g_nCurrentScene == 1 )
        {
            V( g_pEffect->SetTexture( g_hMeshTexture, g_pScene1MeshTexture) );
            pSceneMesh = g_pScene1Mesh;
            nNumObjectsInScene = 25;
        }
        else
        {
            V( g_pEffect->SetTexture( g_hMeshTexture, g_pScene2MeshTexture) );
            pSceneMesh = g_pScene2Mesh;
            nNumObjectsInScene = 3;
        }

        static const D3DXVECTOR3 mScene2WorldPos[3] = { D3DXVECTOR3(-0.5f,-0.5f,-0.5f), 
                                                        D3DXVECTOR3( 1.0f, 1.0f, 2.0f), 
                                                        D3DXVECTOR3( 3.0f, 3.0f, 5.0f) };

        for(int iObject=0; iObject < nNumObjectsInScene; iObject++)
        {
            // setup the world matrix for the current world
            if( g_nCurrentScene == 1 )
            {
                D3DXMatrixTranslation( &matWorld, -(iObject % 5)*1.0f, 0.0f, (iObject / 5)*3.0f );
            }
            else
            {
                D3DXMATRIXA16 matRot, matPos;
                D3DXMatrixRotationY(&matRot, (float)fTime * 0.66666f);
                D3DXMatrixTranslation( &matPos, mScene2WorldPos[iObject].x, mScene2WorldPos[iObject].y, mScene2WorldPos[iObject].z );
                matWorld = matRot * matPos;
            }

            // Update effect vars
            D3DXMATRIXA16 matWorldViewProj = matWorld * matViewProj;
            D3DXMATRIXA16 matWorldView = matWorld * matView;
            V( g_pEffect->SetMatrix( g_hWorld, &matWorld) );
            V( g_pEffect->SetMatrix( g_hWorldView, &matWorldView) );
            V( g_pEffect->SetMatrix( g_hWorldViewProjection, &matWorldViewProj) );

            // Draw the mesh on the rendertarget
            V( g_pEffect->Begin(&cPasses, 0) );
            for (iPass = 0; iPass < cPasses; iPass++)
            {
                V( g_pEffect->BeginPass(iPass) );
                V( pSceneMesh->DrawSubset(0) );
                V( g_pEffect->EndPass() );
            }
            V( g_pEffect->End() );
        }

        V( g_pRenderToSurface->EndScene( 0 ) );
    }

    // Clear the backbuffer 
    V( pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L ) );

    // Begin the scene, rendering to the backbuffer
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        pd3dDevice->SetViewport(&g_ViewportFB);

        // Set the post process technique
        switch( g_nShowMode )
        {
            case 0: V( g_pEffect->SetTechnique( g_hTech[g_TechniqueIndex] ) ); break;
            case 1: V( g_pEffect->SetTechnique( g_hTechShowBlurFactor ) ); break;
            case 2: V( g_pEffect->SetTechnique( g_hTechShowUnmodified ) ); break;
        }

        // Render the fullscreen quad on to the backbuffer
        V( g_pEffect->Begin(&cPasses, 0) );
        for (iPass = 0; iPass < cPasses; iPass++)
        {
            V( g_pEffect->BeginPass(iPass) );
            V( pd3dDevice->SetFVF(VERTEX::FVF) );
            V( pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Vertex, sizeof(VERTEX)) );
            V( g_pEffect->EndPass() );
        }
        V( g_pEffect->End() );

        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );

        // Render the text
        RenderText();

        // End the scene.
        pd3dDevice->EndScene();
    }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( g_pSprite, strMsg, -1, &rc, DT_NOCLIP, g_clr );
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

    switch( g_nShowMode )
    {
        case 0: txtHelper.DrawFormattedTextLine( L"Technique: %S", g_TechniqueNames[g_TechniqueIndex] ); break;
        case 1: txtHelper.DrawTextLine( L"Technique: ShowBlurFactor" ); break;
        case 2: txtHelper.DrawTextLine( L"Technique: ShowUnmodified" ); break;
    }

    txtHelper.DrawFormattedTextLine( L"Focal Plane: (%0.1f,%0.1f,%0.1f,%0.1f)", g_vFocalPlane.x, g_vFocalPlane.y, g_vFocalPlane.z, g_vFocalPlane.w );

    
    // Draw help
    if( g_bShowHelp )
    {
        const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
        txtHelper.SetInsertionPos( 2, pd3dsdBackBuffer->Height-15*6 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Controls (F1 to hide):" );

        txtHelper.SetInsertionPos( 20, pd3dsdBackBuffer->Height-15*5 );
        txtHelper.DrawTextLine( L"Look: Left drag mouse\n"
                                L"Move: A,W,S,D or Arrow Keys\n"
                                L"Move up/down: Q,E or PgUp,PgDn\n"
                                L"Reset camera: End\n" );
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
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;

        case IDC_CHANGE_TECHNIQUE: 
        {
            DWORD OriginalTechnique = g_TechniqueIndex;
            do
            {
                g_TechniqueIndex++;

                if (g_TechniqueIndex == g_TechniqueCount)
                {
                    g_TechniqueIndex = 0;
                }

                D3DXHANDLE hTech = g_pEffect->GetTechniqueByName(g_TechniqueNames[g_TechniqueIndex]);
                if (SUCCEEDED(g_pEffect->ValidateTechnique(hTech)))
                {
                    break;
                }
            } while(OriginalTechnique != g_TechniqueIndex);

            UpdateTechniqueSpecificVariables( DXUTGetBackBufferSurfaceDesc() );
            break;
        }

        case IDC_CHANGE_SCENE:     
        {
            g_nCurrentScene %= 2;
            g_nCurrentScene++;

            switch( g_nCurrentScene )
            {
                case 1:
                {
                    D3DXVECTOR3 vecEye(0.75f, 0.8f, -2.3f);
                    D3DXVECTOR3 vecAt (0.2f, 0.75f, -1.5f);
                    g_Camera.SetViewParams( &vecEye, &vecAt );
                    break;
                }

                case 2:
                {
                    D3DXVECTOR3 vecEye(0.0f, 0.0f, -3.0f);
                    D3DXVECTOR3 vecAt (0.0f, 0.0f, 0.0f);
                    g_Camera.SetViewParams( &vecEye, &vecAt );
                    break;
                }
            }
            break;
        }

        case IDC_CHANGE_FOCAL:
        {    
            WCHAR sz[100];
            g_vFocalPlane.w = -g_SampleUI.GetSlider( IDC_CHANGE_FOCAL )->GetValue() / 10.0f;
            _snwprintf( sz, 100, L"Focal Distance: %0.2f", -g_vFocalPlane.w ); sz[99] = 0;
            g_SampleUI.GetStatic( IDC_CHANGE_FOCAL_STATIC )->SetText( sz );
            UpdateTechniqueSpecificVariables( DXUTGetBackBufferSurfaceDesc() );
            break;
        }

        case IDC_SHOW_NORMAL:
            g_nShowMode = 0;
            break; 

        case IDC_SHOW_BLUR:
            g_nShowMode = 1;
            break;

        case IDC_SHOW_UNBLURRED:
            g_nShowMode = 2;
            break;

        case IDC_CHANGE_BLUR:
        {
            WCHAR sz[100];
            g_fBlurConst = g_SampleUI.GetSlider( IDC_CHANGE_BLUR )->GetValue() / 10.0f;
            _snwprintf( sz, 100, L"Blur Factor: %0.2f", g_fBlurConst ); sz[99] = 0;
            g_SampleUI.GetStatic( IDC_CHANGE_BLUR_STATIC )->SetText( sz );
            UpdateTechniqueSpecificVariables( DXUTGetBackBufferSurfaceDesc() );
            break;
        }
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
    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    SAFE_RELEASE(g_pTextSprite);
    SAFE_RELEASE(g_pFullScreenTextureSurf);
    SAFE_RELEASE(g_pFullScreenTexture);
    SAFE_RELEASE(g_pRenderToSurface);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
    SAFE_RELEASE(g_pEffect);
    SAFE_RELEASE(g_pFont);

    SAFE_RELEASE(g_pFullScreenTextureSurf);
    SAFE_RELEASE(g_pFullScreenTexture);
    SAFE_RELEASE(g_pRenderToSurface);

    SAFE_RELEASE(g_pScene1Mesh);
    SAFE_RELEASE(g_pScene1MeshTexture);
    SAFE_RELEASE(g_pScene2Mesh);
    SAFE_RELEASE(g_pScene2MeshTexture);
}



