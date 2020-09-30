//--------------------------------------------------------------------------------------
// File: main.cpp
//
// This sample demonstrates how use D3DXSHPRTSimulation(), a per vertex  
// precomputed radiance transfer (PRT) simulator that uses low-order 
// spherical harmonics (SH) and records the results to a file. The sample 
// then demonstrates how compress the results with clustered principal 
// component analysis (CPCA) and view the compressed results with arbitrary 
// lighting in real time with a vs_1_1 vertex shader
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include <msxml.h>
#include "PRTMesh.h"
#include "PRTSimulator.h"
#include "PRTOptionsDlg.h"
#include "skybox.h"
#include "resource.h"

// Enable extra D3D debugging in debug builds.  This makes D3D objects work well
// in the debugger watch window, but slows down performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#define D3D_DEBUG_INFO
#endif

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
CModelViewerCamera      g_Camera;               // A model viewing camera
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_StartUpUI;            // dialog for startup
CDXUTDialog             g_StartUpUI2;           // dialog for startup
CDXUTDialog             g_SimulatorRunningUI;   // dialog for while simulator running
CDXUTDialog             g_RenderingUI;          // dialog for while PRT rendering 
CDXUTDialog             g_RenderingUI2;         // dialog for while PRT rendering 
CDXUTDialog             g_RenderingUI3;         // dialog for while PRT rendering 
CDXUTDialog             g_CompressionUI;        // dialog for PRT compression settings
CPRTSimulator           g_Simulator;
CPRTMesh                g_PRTMesh;
bool                    g_bRenderEnvMap = true;
bool                    g_bRenderUI = true;
bool                    g_bRenderArrows = true;
bool                    g_bRenderMesh = true;
bool                    g_bRenderText = true;
bool                    g_bRenderWithAlbedoTexture = true;
bool                    g_bRenderCompressionUI = false;
float                   g_fCurObjectRadius = -1.0f;
bool                    g_bWireframe = false;
bool                    g_bRenderSHProjection = false;
DWORD                   g_dwTechnique = 0;
float                   g_fLightScaleForPRT     = 0.0f;
float                   g_fLightScaleForSHIrrad = 0.0f;
float                   g_fLightScaleForNdotL   = 1.0f;

#define NUM_SKY_BOXES 5
CSkybox                 g_Skybox[NUM_SKY_BOXES];   
float                   g_fSkyBoxLightSH[NUM_SKY_BOXES][3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
int                     g_dwSkyBoxA = (0 % NUM_SKY_BOXES);
int                     g_dwSkyBoxB = (1 % NUM_SKY_BOXES);

#define MAX_LIGHTS 10
CDXUTDirectionWidget    g_LightControl[MAX_LIGHTS];
int                     g_nNumActiveLights;
int                     g_nActiveLight;

enum APP_STATE
{
    APP_STATE_STARTUP,
    APP_STATE_SIMULATOR_OPTIONS,
    APP_STATE_SIMULATOR_RUNNING,
    APP_STATE_LOAD_PRT_BUFFER,
    APP_STATE_RENDER_SCENE,
};

APP_STATE               g_AppState = APP_STATE_STARTUP;         // State of app



//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC              0
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4

#define IDC_NUM_LIGHTS          6
#define IDC_NUM_LIGHTS_STATIC   7
#define IDC_ACTIVE_LIGHT        8
#define IDC_LIGHT_SCALE         9
#define IDC_LIGHT_SCALE_STATIC  10

#define IDC_LOAD_PRTBUFFER      12
#define IDC_SIMULATOR           13
#define IDC_STOP_SIMULATOR      14

#define IDC_ENVIRONMENT_1_SCALER 15
#define IDC_ENVIRONMENT_2_SCALER 16
#define IDC_DIRECTIONAL_SCALER   17
#define IDC_ENVIRONMENT_BLEND_SCALER 18
#define IDC_ENVIRONMENT_A        19
#define IDC_ENVIRONMENT_B        20

#define IDC_RENDER_UI            21
#define IDC_RENDER_MAP           22
#define IDC_RENDER_ARROWS        23
#define IDC_RENDER_MESH          24
#define IDC_SIM_STATUS           25
#define IDC_SIM_STATUS_2         26

#define IDC_NUM_PCA              27
#define IDC_NUM_PCA_TEXT         28
#define IDC_NUM_CLUSTERS         29
#define IDC_NUM_CLUSTERS_TEXT    30
#define IDC_MAX_CONSTANTS        31
#define IDC_CUR_CONSTANTS        32
#define IDC_CUR_CONSTANTS_STATIC 34

#define IDC_APPLY                33
#define IDC_LIGHT_ANGLE_STATIC   35
#define IDC_LIGHT_ANGLE          36
#define IDC_RENDER_TEXTURE       37
#define IDC_WIREFRAME            38
#define IDC_RESTART              40
#define IDC_COMPRESSION          41
#define IDC_SH_PROJECTION        42

#define IDC_TECHNIQUE_PRT        43
#define IDC_TECHNIQUE_SHIRRAD    44
#define IDC_TECHNIQUE_NDOTL      45

#define IDC_SCENE_1              46
#define IDC_SCENE_2              47
#define IDC_SCENE_3              48
#define IDC_SCENE_4              49
#define IDC_SCENE_5              50



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
void    RenderText();
void    UpdateLightingEnvironment();
void    UpdateConstText();
void    LoadScene( IDirect3DDevice9* pd3dDevice, WCHAR* strInputMesh, WCHAR* strResultsFile );
void    LoadSceneAndOptGenResults( IDirect3DDevice9* pd3dDevice, WCHAR* strInputMesh, WCHAR* strResultsFile, int nNumRays, int nNumBounces, bool bSubSurface );


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
    DXUTCreateWindow( L"PRTPerVertex" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 800, 600, IsDeviceAcceptable, ModifyDeviceSettings );

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
struct SHCubeProj 
{
    float *pRed,*pGreen,*pBlue;
    int iOrderUse; // order to use
    float fConvCoeffs[6]; // convolution coefficients

    void InitDiffCubeMap(float *pR, float *pG, float *pB)
    {
        pRed = pR;
        pGreen = pG;
        pBlue = pB;

        iOrderUse = 3; // go to 5 is a bit more accurate...
        fConvCoeffs[0] = 1.0f;
        fConvCoeffs[1] = 2.0f/3.0f;
        fConvCoeffs[2] = 1.0f/4.0f;
        fConvCoeffs[3] = fConvCoeffs[5] = 0.0f;
        fConvCoeffs[4] = -6.0f/144.0f; // 
    }

    void Init(float *pR, float *pG, float *pB)
    {
        pRed = pR;
        pGreen = pG;
        pBlue = pB;

        iOrderUse = 6;
        for(int i=0;i<6;i++) fConvCoeffs[i] = 1.0f;
    }
};


//--------------------------------------------------------------------------------------
void WINAPI SHCubeFill(D3DXVECTOR4* pOut, 
                       CONST D3DXVECTOR3* pTexCoord, 
                       CONST D3DXVECTOR3* pTexelSize, 
                       LPVOID pData)
{
    SHCubeProj* pCP = (SHCubeProj*) pData;
    D3DXVECTOR3 vDir;

    D3DXVec3Normalize(&vDir,pTexCoord);

    float fVals[36];
    D3DXSHEvalDirection( fVals, pCP->iOrderUse, &vDir );

    (*pOut) = D3DXVECTOR4(0,0,0,0); // just clear it out...

    int l, m, uIndex = 0;
    for( l=0; l<pCP->iOrderUse; l++ ) 
    {
        const float fConvUse = pCP->fConvCoeffs[l];
        for( m=0; m<2*l+1; m++ ) 
        {
            pOut->x += fConvUse*fVals[uIndex]*pCP->pRed[uIndex];
            pOut->y += fConvUse*fVals[uIndex]*pCP->pGreen[uIndex];
            pOut->z += fConvUse*fVals[uIndex]*pCP->pBlue[uIndex];
            pOut->w = 1;

            uIndex++;
        }
    }
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    for( int i=0; i<MAX_LIGHTS; i++ )
    {
        g_LightControl[i].SetLightDirection( D3DXVECTOR3( sinf(D3DX_PI*2*i/MAX_LIGHTS-D3DX_PI/6), 0, -cosf(D3DX_PI*2*i/MAX_LIGHTS-D3DX_PI/6) ) );
        g_LightControl[i].SetButtonMask( MOUSE_MIDDLE_BUTTON );
    }

    g_nActiveLight = 0;
    g_nNumActiveLights = 1;

    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

    // Title font for comboboxes
    g_StartUpUI.SetFont( 1, L"Arial", 24, FW_BOLD );
    CDXUTElement* pElement = g_StartUpUI.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    pElement->iFont = 1;
    pElement->dwTextFormat = DT_CENTER | DT_BOTTOM;

    g_StartUpUI.SetCallback( OnGUIEvent ); iY = 10; 
    g_StartUpUI.AddStatic( -1, L"What would you like to do?", 0, 10, 300, 22 );
    g_StartUpUI.AddButton( IDC_SIMULATOR, L"Run PRT simulator", 90, 42, 125, 40 );
    g_StartUpUI.AddButton( IDC_LOAD_PRTBUFFER, L"View saved results", 90, 84, 125, 40 );

    g_StartUpUI2.SetCallback( OnGUIEvent ); iY = 10; 
    g_StartUpUI2.AddButton( IDC_SCENE_1, L"Demo Scene 1 (Z)", 0, iY += 24, 125, 22 );
    g_StartUpUI2.AddButton( IDC_SCENE_2, L"Demo Scene 2 (X)", 0, iY += 24, 125, 22 );
    g_StartUpUI2.AddButton( IDC_SCENE_3, L"Demo Scene 3 (C)", 0, iY += 24, 125, 22 );
    g_StartUpUI2.AddButton( IDC_SCENE_4, L"Demo Scene 4 (V)", 0, iY += 24, 125, 22 );
    g_StartUpUI2.AddButton( IDC_SCENE_5, L"Demo Scene 5 (B)", 0, iY += 24, 125, 22 );

    g_SimulatorRunningUI.SetFont( 1, L"Arial", 24, FW_BOLD );
    pElement = g_SimulatorRunningUI.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    pElement->iFont = 1;
    pElement->dwTextFormat = DT_CENTER | DT_BOTTOM;

    g_SimulatorRunningUI.SetCallback( OnGUIEvent ); iY = 10; 
    g_SimulatorRunningUI.AddStatic( IDC_SIM_STATUS, L"", 0, 0, 600, 22 );
    g_SimulatorRunningUI.AddStatic( IDC_SIM_STATUS_2, L"", 0, 30, 600, 22 );
    g_SimulatorRunningUI.AddButton( IDC_STOP_SIMULATOR, L"Stop PRT simulator", 240, 60, 125, 40 );

    // Title font for comboboxes
    g_RenderingUI2.SetFont( 1, L"Arial", 14, FW_BOLD );
    pElement = g_RenderingUI.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    pElement->iFont = 1;
    pElement->dwTextFormat = DT_LEFT | DT_BOTTOM;

    g_RenderingUI2.SetCallback( OnGUIEvent ); iY = 10; 
    g_RenderingUI2.AddCheckBox( IDC_RENDER_UI, L"Show UI (4)", 35, iY += 24, 125, 22, g_bRenderUI, '4' );
    g_RenderingUI2.AddCheckBox( IDC_RENDER_MAP, L"Background (5)", 35, iY += 24, 125, 22, g_bRenderEnvMap, '5' );
    g_RenderingUI2.AddCheckBox( IDC_RENDER_ARROWS, L"Arrows (6)", 35, iY += 24, 125, 22, g_bRenderArrows, '6' );  
    g_RenderingUI2.AddCheckBox( IDC_RENDER_MESH, L"Mesh (7)", 35, iY += 24, 125, 22, g_bRenderMesh, '7' );   
    g_RenderingUI2.AddCheckBox( IDC_RENDER_TEXTURE, L"Texture (8)", 35, iY += 24, 125, 22, g_bRenderWithAlbedoTexture, '8' );   
    g_RenderingUI2.AddCheckBox( IDC_WIREFRAME, L"Wireframe (W)", 35, iY += 24, 125, 22, g_bWireframe );   
    g_RenderingUI2.AddCheckBox( IDC_SH_PROJECTION, L"SH Projection (H)", 35, iY += 24, 125, 22, g_bRenderSHProjection, 'H' );   
    
    WCHAR sz[100];

    iY += 10;
    _snwprintf( sz, 100, L"Light scale: %0.2f", 0.0f ); sz[99] = 0;
    g_RenderingUI2.AddStatic( IDC_LIGHT_SCALE_STATIC, sz, 35, iY += 24, 125, 22 );
    g_RenderingUI2.AddSlider( IDC_LIGHT_SCALE, 50, iY += 24, 100, 22, 0, 200, (int)(g_fLightScaleForPRT * 100.0f) );

    _snwprintf( sz, 100, L"# Lights: %d", g_nNumActiveLights ); sz[99] = 0;
    g_RenderingUI2.AddStatic( IDC_NUM_LIGHTS_STATIC, sz, 35, iY += 24, 125, 22 );
    g_RenderingUI2.AddSlider( IDC_NUM_LIGHTS, 50, iY += 24, 100, 22, 1, MAX_LIGHTS, g_nNumActiveLights );

    _snwprintf( sz, 100, L"Cone Angle: 45" ); sz[99] = 0;
    g_RenderingUI2.AddStatic( IDC_LIGHT_ANGLE_STATIC, sz, 35, iY += 24, 125, 22 );
    g_RenderingUI2.AddSlider( IDC_LIGHT_ANGLE, 50, iY += 24, 100, 22, 1, 180, 45 );

    iY += 5;
    g_RenderingUI2.AddButton( IDC_ACTIVE_LIGHT, L"Change active light (K)", 35, iY += 24, 125, 22, 'K' );

    iY += 5;
    g_RenderingUI2.AddButton( IDC_COMPRESSION, L"Compression Settings", 35, iY += 24, 125, 22 );

    iY += 5;
    g_RenderingUI2.AddButton( IDC_RESTART, L"Restart", 35, iY += 24, 125, 22 );

    bool bEnable = (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() != 0 );
    g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->SetEnabled( bEnable );
    g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->SetEnabled( bEnable );
    g_RenderingUI2.GetStatic( IDC_LIGHT_ANGLE_STATIC )->SetEnabled( bEnable );
    g_RenderingUI2.GetStatic( IDC_NUM_LIGHTS_STATIC )->SetEnabled( bEnable );
    g_RenderingUI2.GetButton( IDC_ACTIVE_LIGHT )->SetEnabled( bEnable );

    // Title font for comboboxes
    g_RenderingUI3.SetFont( 1, L"Arial", 14, FW_BOLD );
    pElement = g_RenderingUI3.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    pElement->iFont = 1;
    pElement->dwTextFormat = DT_LEFT | DT_BOTTOM;

    g_RenderingUI3.SetCallback( OnGUIEvent ); iY = 0; 
    g_RenderingUI3.AddStatic( IDC_STATIC, L"(T)echnique", 0, iY, 150, 25 );
    g_RenderingUI3.AddRadioButton( IDC_TECHNIQUE_PRT, 1, L"(1) PRT", 0, iY += 24, 150, 22, true, '1' );   
    g_RenderingUI3.AddRadioButton( IDC_TECHNIQUE_SHIRRAD, 1, L"(2) SHIrradEnvMap", 0, iY += 24, 150, 22, false, '2' );   
    g_RenderingUI3.AddRadioButton( IDC_TECHNIQUE_NDOTL, 1, L"(3) N dot L", 0, iY += 24, 150, 22, false, '3' );   

    // Title font for comboboxes
    g_RenderingUI.SetFont( 1, L"Arial", 14, FW_BOLD );
    pElement = g_RenderingUI.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
    pElement->iFont = 1;
    pElement->dwTextFormat = DT_LEFT | DT_BOTTOM;

    g_RenderingUI.SetCallback( OnGUIEvent ); 

    int iStartY = 0;
    int iX = 10;
    iY = iStartY; 
    g_RenderingUI.AddStatic( IDC_STATIC, L"(F)irst Light Probe", iX, iY += 24, 150, 25 );
    g_RenderingUI.AddComboBox( IDC_ENVIRONMENT_A, iX, iY += 24, 150, 22, 'F' );   
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A)->SetDropHeight( 30 );
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A )->AddItem( L"Eucalyptus Grove", (void*)0 );
#if NUM_SKY_BOXES > 1
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A )->AddItem( L"The Uffizi Gallery", (void*)1 );
#endif
#if NUM_SKY_BOXES > 2
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A )->AddItem( L"Galileo's Tomb", (void*)2 );
#endif
#if NUM_SKY_BOXES > 3
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A )->AddItem( L"Grace Cathedral", (void*)3 );
#endif
#if NUM_SKY_BOXES > 4
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A )->AddItem( L"St. Peter's Basilica", (void*)4 );
#endif
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A)->SetSelectedByData( IntToPtr(g_dwSkyBoxA) );
    
    g_RenderingUI.AddStatic( IDC_STATIC, L"First Light Probe Scaler", iX, iY += 24, 150, 25 );
    g_RenderingUI.AddSlider( IDC_ENVIRONMENT_1_SCALER, iX, iY += 24, 150, 22, 0, 100, 50 );

    iX += 175;
    iY = iStartY + 30; 
    g_RenderingUI.AddStatic( IDC_STATIC, L"Light Probe Blend", iX, iY += 24, 100, 25 );
    g_RenderingUI.AddSlider( IDC_ENVIRONMENT_BLEND_SCALER, iX, iY += 24, 100, 22, 0, 100, 0 );   

    iX += 125;
    iY = iStartY; 
    g_RenderingUI.AddStatic( IDC_STATIC, L"(S)econd Light Probe", iX, iY += 24, 150, 25 );
    g_RenderingUI.AddComboBox( IDC_ENVIRONMENT_B, iX, iY += 24, 150, 22, 'S' );   
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->SetDropHeight( 30 );
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->AddItem( L"Eucalyptus Grove", (void*)0 );
#if NUM_SKY_BOXES > 1
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->AddItem( L"The Uffizi Gallery", (void*)1 );
#endif
#if NUM_SKY_BOXES > 2
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->AddItem( L"Galileo's Tomb", (void*)2 );
#endif
#if NUM_SKY_BOXES > 3
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->AddItem( L"Grace Cathedral", (void*)3 );
#endif
#if NUM_SKY_BOXES > 4
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->AddItem( L"St. Peter's Basilica", (void*)4 );
#endif
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B )->SetSelectedByData( IntToPtr(g_dwSkyBoxB) );

    g_RenderingUI.AddStatic( IDC_STATIC, L"Second Light Probe Scaler", iX, iY += 24, 150, 25 );
    g_RenderingUI.AddSlider( IDC_ENVIRONMENT_2_SCALER, iX, iY += 24, 150, 22, 0, 100, 50 );

    g_CompressionUI.SetCallback( OnGUIEvent ); iY = 10;

    SIMULATOR_OPTIONS& options = GetGlobalOptions();

    g_CompressionUI.AddStatic( IDC_NUM_PCA_TEXT, L"Number of PCA: 24", 30, iY += 24, 120, 22 );
    g_CompressionUI.AddSlider( IDC_NUM_PCA, 30, iY += 24, 120, 22, 1, 6, options.dwNumPCA/4 );
    g_CompressionUI.AddStatic( IDC_NUM_CLUSTERS_TEXT, L"Number of Clusters: 1", 30, iY += 24, 120, 22 );
    g_CompressionUI.AddSlider( IDC_NUM_CLUSTERS, 30, iY += 24, 120, 22, 1, 40, options.dwNumClusters );
    g_CompressionUI.AddStatic( IDC_MAX_CONSTANTS, L"Max VS Constants: 0", 30, iY += 24, 120, 22 );
    g_CompressionUI.AddStatic( IDC_CUR_CONSTANTS_STATIC, L"Cur VS Constants:", 30, iY += 24, 120, 22 );
    g_CompressionUI.AddStatic( IDC_CUR_CONSTANTS, L"0", -10, iY += 12, 200, 22 );
    g_CompressionUI.AddButton( IDC_APPLY, L"Apply", 30, iY += 24, 120, 24 );

    swprintf( sz, L"Number of PCA: %d", options.dwNumPCA );
    g_CompressionUI.GetStatic( IDC_NUM_PCA_TEXT )->SetText( sz );           
    swprintf( sz, L"Number of Clusters: %d", options.dwNumClusters );
    g_CompressionUI.GetStatic( IDC_NUM_CLUSTERS_TEXT )->SetText( sz );           
    UpdateConstText();
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

    // The PRT simulator runs on a seperate thread.  If you are just 
    // loading simulator results, then this isn't needed
    pDeviceSettings->BehaviorFlags |= D3DCREATE_MULTITHREADED;

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
    WCHAR* str;

    for(int i=0; i<NUM_SKY_BOXES; ++i )
    {
        switch( i )
        {
            case 0: str = L"Light Probes\\rnl_cross.dds"; break;
            case 1: str = L"Light Probes\\uffizi_cross.dds"; break;
            case 2: str = L"Light Probes\\galileo_cross.dds"; break;
            case 3: str = L"Light Probes\\grace_cross.dds"; break;
            case 4: 
            default: str = L"Light Probes\\stpeters_cross.dds"; break;
        }

        g_Skybox[i].OnCreateDevice( pd3dDevice, 50, str, L"SkyBox.fx" );

        V( D3DXSHProjectCubeMap( 6, g_Skybox[i].GetEnvironmentMap(), g_fSkyBoxLightSH[i][0], g_fSkyBoxLightSH[i][1], g_fSkyBoxLightSH[i][2] ) );

        // now compute the SH projection of the skybox...
        LPDIRECT3DCUBETEXTURE9 pSHCubeTex=NULL;
        V( D3DXCreateCubeTexture( pd3dDevice, 256, 1, 0, D3DFMT_A16B16G16R16F, D3DPOOL_MANAGED, &pSHCubeTex ) );
 
        SHCubeProj projData;
        projData.Init(g_fSkyBoxLightSH[i][0],g_fSkyBoxLightSH[i][1],g_fSkyBoxLightSH[i][2]);

        V( D3DXFillCubeTexture(pSHCubeTex,SHCubeFill,&projData));
/*
        switch( i )
        {
            case 0: str = L"diff_rnl_cross.dds"; break;
            case 1: str = L"diff_uffizi_cross.dds"; break;
            case 2: str = L"diff_galileo_cross.dds"; break;
            case 3: str = L"diff_grace_cross.dds"; break;
            case 4: 
            default: str = L"diff_stpeters_cross.dds"; break;
        }
        V( D3DXSaveTextureToFile( str, D3DXIFF_DDS, pSHCubeTex, NULL ) ); 
*/
        g_Skybox[i].InitSH(pSHCubeTex);
    }

    V_RETURN( g_PRTMesh.OnCreateDevice( pd3dDevice ) );

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );
    V_RETURN( CDXUTDirectionWidget::StaticOnCreateDevice( pd3dDevice ) );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    g_Camera.SetViewParams( &vecEye, &vecAt );

    DWORD dwMaxVertexShaderConst = DXUTGetDeviceCaps()->MaxVertexShaderConst;
    WCHAR sz[256];
    int nMax = (dwMaxVertexShaderConst - 4) / 2;
    g_CompressionUI.GetSlider( IDC_NUM_CLUSTERS )->SetRange( 1, nMax );

    swprintf( sz, L"Max VS Constants: %d", dwMaxVertexShaderConst );
    g_CompressionUI.GetStatic( IDC_MAX_CONSTANTS )->SetText( sz );
    UpdateConstText();

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

    for(int i=0; i<NUM_SKY_BOXES; ++i )
        g_Skybox[i].OnResetDevice( pBackBufferSurfaceDesc ); 

    V( g_PRTMesh.OnResetDevice() );

    if( g_pFont )
        V( g_pFont->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    for( int i=0; i<MAX_LIGHTS; i++ )
        g_LightControl[i].OnResetDevice( pBackBufferSurfaceDesc  );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 1.0f, 10000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_RIGHT_BUTTON );
    g_Camera.SetAttachCameraToModel( true );

    if( g_PRTMesh.GetMesh() != NULL )
    {
        // Update camera's viewing radius based on the object radius
        float fObjectRadius = g_PRTMesh.GetObjectRadius();
        if( g_fCurObjectRadius != fObjectRadius )
        {
            g_fCurObjectRadius = fObjectRadius;
            g_Camera.SetRadius( fObjectRadius*3.0f, fObjectRadius*0.1f, fObjectRadius*20.0f );
        }
        g_Camera.SetModelCenter( g_PRTMesh.GetObjectCenter() );
        for( int i=0; i<MAX_LIGHTS; i++ )
            g_LightControl[i].SetRadius(fObjectRadius);
    }

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 100 );
    g_StartUpUI.SetLocation( (pBackBufferSurfaceDesc->Width-300)/2, (pBackBufferSurfaceDesc->Height-200)/2 );
    g_StartUpUI.SetSize( 300, 200 );
    g_StartUpUI2.SetLocation( 50, (pBackBufferSurfaceDesc->Height-200) );
    g_StartUpUI2.SetSize( 300, 200 );
    g_CompressionUI.SetLocation( 0, 150 );
    g_CompressionUI.SetSize( 200, 200 );
    g_SimulatorRunningUI.SetLocation( (pBackBufferSurfaceDesc->Width-600)/2, (pBackBufferSurfaceDesc->Height-100)/2 );
    g_SimulatorRunningUI.SetSize( 600, 100 );
    g_RenderingUI.SetLocation( 0, pBackBufferSurfaceDesc->Height-125 );
    g_RenderingUI.SetSize( pBackBufferSurfaceDesc->Width, 125 );
    g_RenderingUI2.SetLocation( pBackBufferSurfaceDesc->Width-170, 100 );
    g_RenderingUI2.SetSize( 170, 400 );
    g_RenderingUI3.SetLocation( 10, 30 );
    g_RenderingUI3.SetSize( 200, 100 );

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
}


//--------------------------------------------------------------------------------------
void RenderStartup( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;
    
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        RenderText();
        if( g_bRenderUI )
        {
            D3DXMATRIXA16 mWorld;
            D3DXMATRIXA16 mView;
            D3DXMATRIXA16 mProj;
            D3DXMATRIXA16 mWorldViewProjection;
            D3DXMATRIXA16 mViewProjection;
            mWorld = *g_Camera.GetWorldMatrix();       
            mProj = *g_Camera.GetProjMatrix();       
            mView = *g_Camera.GetViewMatrix();
            mViewProjection = mView * mProj;        

            V( g_HUD.OnRender( fElapsedTime ) );
            V( g_StartUpUI.OnRender( fElapsedTime ) );
            V( g_StartUpUI2.OnRender( fElapsedTime ) );
        }

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
void RenderSimulatorRunning( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;
    
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        RenderText();
        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SimulatorRunningUI.OnRender( fElapsedTime ) );

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
void RenderPRT( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;
    D3DXMATRIXA16 mViewProjection;
    
    pd3dDevice->SetRenderState( D3DRS_FILLMODE, (g_bWireframe) ? D3DFILL_WIREFRAME : D3DFILL_SOLID ); 

    // Clear the render target and the zbuffer 
//    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        mWorld = *g_Camera.GetWorldMatrix();       
        mProj = *g_Camera.GetProjMatrix();       
        mView = *g_Camera.GetViewMatrix();

        mWorldViewProjection = mWorld * mView * mProj;

        if( g_bRenderEnvMap && !g_bWireframe )
        {
            float fEnv1Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_1_SCALER )->GetValue() / 100.0f);
            float fEnv2Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_2_SCALER )->GetValue() / 100.0f);

            float fEnvBlendScaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_BLEND_SCALER )->GetValue() / 100.0f);
            pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

            g_Skybox[g_dwSkyBoxA].SetDrawSH( g_bRenderSHProjection );
            g_Skybox[g_dwSkyBoxB].SetDrawSH( g_bRenderSHProjection );

            mViewProjection = mView * mProj;        
            g_Skybox[g_dwSkyBoxA].Render( &mViewProjection, (1.0f-fEnvBlendScaler), fEnv1Scaler );
            g_Skybox[g_dwSkyBoxB].Render( &mViewProjection, (fEnvBlendScaler), fEnv2Scaler );

            pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        }

        float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
        if( g_bRenderArrows && fLightScale > 0.0f )
        {
            // Render the light spheres so the user can 
            // visually see the light dir
            for( int i=0; i<g_nNumActiveLights; i++ )
            {
                D3DXCOLOR arrowColor = ( i == g_nActiveLight ) ? D3DXCOLOR(1,1,0,1) : D3DXCOLOR(1,1,1,1);
                V( g_LightControl[i].OnRender( arrowColor, &mView, &mProj, g_Camera.GetEyePt() ) );
            }
        }

        if( g_bRenderMesh )
        {
            switch( g_dwTechnique )
            {
                case 0: // PRT
                    g_PRTMesh.RenderWithPRT( pd3dDevice, &mWorldViewProjection, g_bRenderWithAlbedoTexture );
                    break;

                case 1: // SHIrradEnvMap
                    g_PRTMesh.RenderWithSHIrradEnvMap( pd3dDevice, &mWorldViewProjection, g_bRenderWithAlbedoTexture );
                    break;

                case 2: // N dot L
                {
                    D3DXMATRIX mWorldInv;
                    D3DXMatrixInverse(&mWorldInv, NULL, g_Camera.GetWorldMatrix() );
                    g_PRTMesh.RenderWithNdotL( pd3dDevice, &mWorldViewProjection, &mWorldInv, g_bRenderWithAlbedoTexture, g_LightControl, g_nNumActiveLights, fLightScale );
                    break;
                }
            }


        }

        RenderText();
        if( g_bRenderUI )
        {
            V( g_HUD.OnRender( fElapsedTime ) );
            V( g_RenderingUI.OnRender( fElapsedTime ) );
            V( g_RenderingUI2.OnRender( fElapsedTime ) );
            V( g_RenderingUI3.OnRender( fElapsedTime ) );
            if( g_bRenderCompressionUI )
                V( g_CompressionUI.OnRender( fElapsedTime ) );
        }

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    switch( g_AppState )
    {
        case APP_STATE_STARTUP: 
            RenderStartup( pd3dDevice, fTime, fElapsedTime ); 
            break;

        case APP_STATE_SIMULATOR_OPTIONS:
        {
            CPRTOptionsDlg dlg;
            bool bResult = dlg.Show();
            if( bResult )
            {                
                SIMULATOR_OPTIONS* pOptions = dlg.GetOptions();
                g_PRTMesh.LoadMesh( pd3dDevice, pOptions->strInputMesh );

                // Update camera's viewing radius based on the object radius
                float fObjectRadius = g_PRTMesh.GetObjectRadius();
                if( g_fCurObjectRadius != fObjectRadius )
                {
                    g_fCurObjectRadius = fObjectRadius;
                    g_Camera.SetRadius( fObjectRadius*3.0f, fObjectRadius*0.1f, fObjectRadius*20.0f );
                }
                g_Camera.SetModelCenter( g_PRTMesh.GetObjectCenter() );
                for( int i=0; i<MAX_LIGHTS; i++ )
                    g_LightControl[i].SetRadius(fObjectRadius);

                g_Simulator.Run( pd3dDevice, pOptions, &g_PRTMesh );
                g_AppState = APP_STATE_SIMULATOR_RUNNING;
            }
            else
            {
                g_AppState = APP_STATE_STARTUP;                
            }
            break;
        }

        case APP_STATE_SIMULATOR_RUNNING:
        {
            WCHAR sz[256];
            if( g_Simulator.GetPercentComplete() >= 0.0f )
                swprintf( sz, L"Step %d of %d: %0.1f%% done", g_Simulator.GetCurrentPass(), g_Simulator.GetNumPasses(), g_Simulator.GetPercentComplete() );
            else
                swprintf( sz, L"Step %d of %d (progress n/a)", g_Simulator.GetCurrentPass(), g_Simulator.GetNumPasses() );
            g_SimulatorRunningUI.GetStatic( IDC_SIM_STATUS )->SetText( sz );
            g_SimulatorRunningUI.GetStatic( IDC_SIM_STATUS_2 )->SetText( g_Simulator.GetCurrentPassName() );

            RenderSimulatorRunning( pd3dDevice, fTime, fElapsedTime );
            Sleep(50); // Yield time to simulator thread

            if( !g_Simulator.IsRunning() )
            {
                g_PRTMesh.LoadEffects( pd3dDevice, DXUTGetDeviceCaps() );
                g_AppState = APP_STATE_RENDER_SCENE;                
                g_bRenderCompressionUI = false;
            }
            break;
        }
           
        case APP_STATE_LOAD_PRT_BUFFER:
        {
            CPRTLoadDlg dlg;
            bool bResult = dlg.Show();
            if( bResult )
            {
                SIMULATOR_OPTIONS* pOptions = dlg.GetOptions();
                if( !pOptions->bAdaptive )
                    LoadScene( pd3dDevice, pOptions->strInputMesh, pOptions->strResultsFile );
                else
                    LoadScene( pd3dDevice, pOptions->strOutputMesh, pOptions->strResultsFile );
            }
            else
            {
                g_AppState = APP_STATE_STARTUP;                
            }
            break;
        }

        case APP_STATE_RENDER_SCENE: 
            UpdateLightingEnvironment();
            RenderPRT( pd3dDevice, fTime, fElapsedTime ); 
            break;
    
        default:
            assert(false);
            break;
    }
}


//--------------------------------------------------------------------------------------
void UpdateLightingEnvironment()
{
    HRESULT hr;

    float fEnv1Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_1_SCALER )->GetValue() / 100.0f);
    float fEnv2Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_2_SCALER )->GetValue() / 100.0f);
    float fEnvBlendScaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_BLEND_SCALER )->GetValue() / 100.0f);
    float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
    float fConeRadius = (float) ( ( D3DX_PI * (float)g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->GetValue()) / 180.0f );    

    float fLight[MAX_LIGHTS][3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  

    D3DXCOLOR lightColor(1.0f, 1.0f, 1.0f, 1.0f);        
    lightColor *= fLightScale;

    DWORD dwOrder = g_PRTMesh.GetOrder();

    // Pass in the light direction, the intensity of each channel, and it returns
    // the source radiance as an array of order^2 SH coefficients for each color channel.  
    D3DXVECTOR3 lightDirObjectSpace;
    D3DXMATRIX mWorldInv;
    D3DXMatrixInverse(&mWorldInv, NULL, g_Camera.GetWorldMatrix() );

    int i;
    for( i=0; i<g_nNumActiveLights; i++ )
    {
        // Transform the world space light dir into object space
        // Note that if there's multiple objects using PRT in the scene, then
        // for each object you need to either evaulate the lights in object space
        // evaulate the lights in world and rotate the light coefficients 
        // into object space.
        D3DXVECTOR3 vLight = g_LightControl[i].GetLightDirection();
        D3DXVec3TransformNormal( &lightDirObjectSpace, &vLight, &mWorldInv );

        // This sample uses D3DXSHEvalDirectionalLight(), but there's other 
        // types of lights provided by D3DXSHEval*.  Pass in the 
        // order of SH, color of the light, and the direction of the light 
        // in object space.
        // The output is the source radiance coefficients for the SH basis functions.  
        // There are 3 outputs, one for each channel (R,G,B). 
        // Each output is an array of m_dwOrder^2 floats.  
        V( D3DXSHEvalConeLight( dwOrder, &lightDirObjectSpace, fConeRadius,
                                lightColor.r, lightColor.g, lightColor.b,
                                fLight[i][0], fLight[i][1], fLight[i][2] ) );
    }

    float fSum[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
    ZeroMemory( fSum, 3*D3DXSH_MAXORDER*D3DXSH_MAXORDER*sizeof(float) );

    // For multiple lights, just them sum up using D3DXSHAdd().
    for( i=0; i<g_nNumActiveLights; i++ )
    {
        // D3DXSHAdd will add Order^2 floats.  There are 3 color channels, 
        // so call it 3 times.
        D3DXSHAdd( fSum[0], dwOrder, fSum[0], fLight[i][0] );
        D3DXSHAdd( fSum[1], dwOrder, fSum[1], fLight[i][1] );
        D3DXSHAdd( fSum[2], dwOrder, fSum[2], fLight[i][2] );
    }

    float fSkybox1[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
    float fSkybox1Rot[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
    D3DXSHScale( fSkybox1[0], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][0], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
    D3DXSHScale( fSkybox1[1], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][1], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
    D3DXSHScale( fSkybox1[2], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][2], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
    D3DXSHRotate( fSkybox1Rot[0], dwOrder, &mWorldInv, fSkybox1[0] );
    D3DXSHRotate( fSkybox1Rot[1], dwOrder, &mWorldInv, fSkybox1[1] );
    D3DXSHRotate( fSkybox1Rot[2], dwOrder, &mWorldInv, fSkybox1[2] );
    D3DXSHAdd( fSum[0], dwOrder, fSum[0], fSkybox1Rot[0] );
    D3DXSHAdd( fSum[1], dwOrder, fSum[1], fSkybox1Rot[1] );
    D3DXSHAdd( fSum[2], dwOrder, fSum[2], fSkybox1Rot[2] );

    float fSkybox2[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
    float fSkybox2Rot[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
    D3DXSHScale( fSkybox2[0], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][0], fEnv2Scaler*fEnvBlendScaler );
    D3DXSHScale( fSkybox2[1], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][1], fEnv2Scaler*fEnvBlendScaler );
    D3DXSHScale( fSkybox2[2], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][2], fEnv2Scaler*fEnvBlendScaler );
    D3DXSHRotate( fSkybox2Rot[0], dwOrder, &mWorldInv, fSkybox2[0] );
    D3DXSHRotate( fSkybox2Rot[1], dwOrder, &mWorldInv, fSkybox2[1] );
    D3DXSHRotate( fSkybox2Rot[2], dwOrder, &mWorldInv, fSkybox2[2] );
    D3DXSHAdd( fSum[0], dwOrder, fSum[0], fSkybox2Rot[0] );
    D3DXSHAdd( fSum[1], dwOrder, fSum[1], fSkybox2Rot[1] );
    D3DXSHAdd( fSum[2], dwOrder, fSum[2], fSkybox2Rot[2] );

    g_PRTMesh.ComputeShaderConstants( fSum[0], fSum[1], fSum[2], dwOrder*dwOrder );
    g_PRTMesh.ComputeSHIrradEnvMapConstants( fSum[0], fSum[1], fSum[2] );
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    if( !g_bRenderText ) 
        return;

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
    if( !g_bRenderUI )
        txtHelper.DrawFormattedTextLine( L"Press '4' to show UI" );

    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, the sample framework passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then the sample framework will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing )
{
    if( uMsg == WM_KEYDOWN && wParam == VK_F8 )
        *pbNoFurtherProcessing = true;

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    switch( g_AppState )
    {
        case APP_STATE_STARTUP: 
            *pbNoFurtherProcessing = g_StartUpUI.MsgProc( hWnd, uMsg, wParam, lParam );
            if( *pbNoFurtherProcessing )
                return 0;
            *pbNoFurtherProcessing = g_StartUpUI2.MsgProc( hWnd, uMsg, wParam, lParam );
            if( *pbNoFurtherProcessing )
                return 0;
            break;

        case APP_STATE_SIMULATOR_RUNNING:
            *pbNoFurtherProcessing = g_SimulatorRunningUI.MsgProc( hWnd, uMsg, wParam, lParam );
            if( *pbNoFurtherProcessing )
                return 0;
            break;

        case APP_STATE_RENDER_SCENE: 
            if( g_bRenderUI || uMsg == WM_KEYDOWN || uMsg == WM_KEYUP )
            {
                *pbNoFurtherProcessing = g_RenderingUI.MsgProc( hWnd, uMsg, wParam, lParam );
                if( *pbNoFurtherProcessing )
                    return 0;
                *pbNoFurtherProcessing = g_RenderingUI2.MsgProc( hWnd, uMsg, wParam, lParam );
                if( *pbNoFurtherProcessing )
                    return 0;
                *pbNoFurtherProcessing = g_RenderingUI3.MsgProc( hWnd, uMsg, wParam, lParam );
                if( *pbNoFurtherProcessing )
                    return 0;
                if( g_bRenderCompressionUI )
                {
                    *pbNoFurtherProcessing = g_CompressionUI.MsgProc( hWnd, uMsg, wParam, lParam );
                    if( *pbNoFurtherProcessing )
                        return 0;
                }
            }

            g_LightControl[g_nActiveLight].HandleMessages( hWnd, uMsg, wParam, lParam );

            // Pass all remaining windows messages to camera so it can respond to user input
            g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
            break;

        default:
            break;
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Load a mesh and optionally generate the PRT results file if they aren't already cached
//--------------------------------------------------------------------------------------
void LoadSceneAndOptGenResults( IDirect3DDevice9* pd3dDevice, WCHAR* strInputMesh, WCHAR* strResultsFile,
                                int nNumRays, int nNumBounces, bool bSubSurface )
{
    HRESULT hr;
    WCHAR strResults[MAX_PATH];
    WCHAR strMesh[MAX_PATH];
    V( DXUTFindDXSDKMediaFileCch( strMesh, MAX_PATH, strInputMesh ) );

    hr = DXUTFindDXSDKMediaFileCch( strResults, MAX_PATH, strResultsFile );
    if( SUCCEEDED(hr) )
    {
        LoadScene( pd3dDevice, strInputMesh, strResultsFile );
    }
    else
    {
        SIMULATOR_OPTIONS options;
        ZeroMemory( &options, sizeof(SIMULATOR_OPTIONS) );

        wcscpy( options.strInputMesh, strInputMesh );
        DXUTGetDXSDKMediaPathCch( options.strInitialDir, MAX_PATH );
        wcscat( options.strInitialDir, L"" );
        DXUTFindDXSDKMediaFileCch( options.strInitialDir, MAX_PATH, options.strInputMesh );
        WCHAR* pLastSlash =  wcsrchr( options.strInitialDir, L'\\' );
        if( pLastSlash )
            *pLastSlash = 0;
        pLastSlash = wcsrchr( strResultsFile, L'\\' );
        if( pLastSlash )
            wcscpy( options.strResultsFile, pLastSlash + 1 );
        else
            wcscpy( options.strResultsFile, strResultsFile );
        options.dwOrder = 6;
        options.dwNumRays = nNumRays;
        options.dwNumBounces = nNumBounces;
        options.bSubsurfaceScattering = bSubSurface;
        options.fLengthScale = 25.0f;
        options.dwNumChannels = 3;

        options.dwPredefinedMatIndex = 0;
        options.fRelativeIndexOfRefraction = 1.3f;
        options.Diffuse = D3DXCOLOR( 1.00f, 1.00f, 1.00f, 1.0f );
        options.Absoption = D3DXCOLOR( 0.0030f, 0.0030f, 0.0460f, 1.0f );
        options.ReducedScattering = D3DXCOLOR( 2.00f, 2.00f, 2.00f, 1.0f );

        options.bAdaptive = false;
        options.bRobustMeshRefine = true;
        options.fRobustMeshRefineMinEdgeLength = 0.0f;
        options.dwRobustMeshRefineMaxSubdiv = 2;
        options.bAdaptiveDL = true;
        options.fAdaptiveDLMinEdgeLength = 0.03f;
        options.fAdaptiveDLThreshold = 8e-5f;
        options.dwAdaptiveDLMaxSubdiv = 3;
        options.bAdaptiveBounce = false;
        options.fAdaptiveBounceMinEdgeLength = 0.03f;
        options.fAdaptiveBounceThreshold = 8e-5f;
        options.dwAdaptiveBounceMaxSubdiv = 3;
        wcscpy( options.strOutputMesh, L"shapes1_adaptive.x" );
        options.bBinaryOutputXFile = true;

        options.bSaveCompressedResults = true;
        options.Quality = D3DXSHCQUAL_SLOWHIGHQUALITY;
        options.dwNumPCA = 24;
        options.dwNumClusters = 1;

        g_PRTMesh.LoadMesh( pd3dDevice, strInputMesh );

        // Update camera's viewing radius based on the object radius
        float fObjectRadius = g_PRTMesh.GetObjectRadius();
        if( g_fCurObjectRadius != fObjectRadius )
        {
            g_fCurObjectRadius = fObjectRadius;
            g_Camera.SetRadius( fObjectRadius*3.0f, fObjectRadius*0.1f, fObjectRadius*20.0f );
        }
        g_Camera.SetModelCenter( g_PRTMesh.GetObjectCenter() );
        for( int i=0; i<MAX_LIGHTS; i++ )
            g_LightControl[i].SetRadius(fObjectRadius);

        g_Simulator.Run( pd3dDevice, &options, &g_PRTMesh );
        g_AppState = APP_STATE_SIMULATOR_RUNNING;
    }
}


//--------------------------------------------------------------------------------------
void LoadScene( IDirect3DDevice9* pd3dDevice, WCHAR* strInputMesh, WCHAR* strResultsFile )
{
    HRESULT hr;
    WCHAR strResults[MAX_PATH];
    WCHAR strMesh[MAX_PATH];
    V( DXUTFindDXSDKMediaFileCch( strMesh, MAX_PATH, strInputMesh ) );
    V( DXUTFindDXSDKMediaFileCch( strResults, MAX_PATH, strResultsFile ) );

    g_PRTMesh.LoadMesh( pd3dDevice, strMesh );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    g_Camera.Reset();
    g_Camera.SetViewQuat( D3DXQUATERNION(0,0,0,1) );
    g_Camera.SetWorldQuat( D3DXQUATERNION(0,0,0,1) );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    // Update camera's viewing radius based on the object radius
    float fObjectRadius = g_PRTMesh.GetObjectRadius();
    g_fCurObjectRadius = fObjectRadius;
    g_Camera.SetRadius( fObjectRadius*3.0f, fObjectRadius*0.1f, fObjectRadius*20.0f );
    g_Camera.SetModelCenter( g_PRTMesh.GetObjectCenter() );

    for( int i=0; i<MAX_LIGHTS; i++ )
        g_LightControl[i].SetRadius(fObjectRadius);

    WCHAR* pLastDot =  wcsrchr( strResults, L'.' );
    bool bLoadCompressed = false;
    if( pLastDot )
    {
        if( wcscmp(pLastDot, L".prt") == 0 )
            bLoadCompressed = false;
        if( wcscmp(pLastDot, L".pca") == 0 )
            bLoadCompressed = true;
    }

    if( bLoadCompressed )
    {
        g_PRTMesh.LoadCompPRTBufferFromFile( strResults );
    }
    else
    {
        g_PRTMesh.LoadPRTBufferFromFile( strResults );
        g_PRTMesh.CompressBuffer( D3DXSHCQUAL_FASTLOWQUALITY, 1, 24 );
    }

    g_PRTMesh.ExtractCompressedDataForPRTShader();
    g_PRTMesh.LoadEffects( pd3dDevice, DXUTGetDeviceCaps() );

    g_AppState = APP_STATE_RENDER_SCENE;                
    g_bRenderCompressionUI = false;
}

//--------------------------------------------------------------------------------------
void ResetUI()
{
    g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetRange( 0, 200 );

    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_A)->SetSelectedByData( IntToPtr(0) );
    g_RenderingUI.GetComboBox(IDC_ENVIRONMENT_B)->SetSelectedByData( IntToPtr(2) );
    g_RenderingUI.GetSlider(IDC_ENVIRONMENT_1_SCALER)->SetValue( 50 ); 
    g_RenderingUI.GetSlider(IDC_ENVIRONMENT_2_SCALER)->SetValue( 50 ); 
    g_RenderingUI.GetSlider(IDC_ENVIRONMENT_BLEND_SCALER)->SetValue( 0 ); 

    g_RenderingUI2.GetCheckBox( IDC_RENDER_UI )->SetChecked( true );
    g_RenderingUI2.GetCheckBox( IDC_RENDER_MAP )->SetChecked( true );
    g_bRenderEnvMap = true;
    g_RenderingUI2.GetCheckBox( IDC_RENDER_ARROWS )->SetChecked( true );
    g_RenderingUI2.GetCheckBox( IDC_RENDER_MESH )->SetChecked( true );
    g_RenderingUI2.GetCheckBox( IDC_RENDER_TEXTURE )->SetChecked( true );
    g_RenderingUI2.GetCheckBox( IDC_WIREFRAME )->SetChecked( false );
    g_RenderingUI2.GetCheckBox( IDC_SH_PROJECTION )->SetChecked( false );

    g_RenderingUI3.GetRadioButton( IDC_TECHNIQUE_NDOTL )->SetChecked( true, true );
    g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( 100 );
    g_RenderingUI3.GetRadioButton( IDC_TECHNIQUE_SHIRRAD )->SetChecked( true, true );
    g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( 0 );
    g_RenderingUI3.GetRadioButton( IDC_TECHNIQUE_PRT )->SetChecked( true, true );
    g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( 0 );
    g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->SetValue( 1 );

    WCHAR sz[100];
    _snwprintf( sz, 100, L"# Lights: %d", g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->GetValue() ); sz[99] = 0;
    g_RenderingUI2.GetStatic( IDC_NUM_LIGHTS_STATIC )->SetText( sz );
    g_nNumActiveLights = g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->GetValue();
    g_nActiveLight %= g_nNumActiveLights;

    g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->SetValue( 45 );
    int nLightAngle = g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->GetValue();
    _snwprintf( sz, 100, L"Cone Angle: %d", nLightAngle ); sz[99] = 0;
    g_RenderingUI2.GetStatic( IDC_LIGHT_ANGLE_STATIC )->SetText( sz );

    g_RenderingUI3.GetRadioButton( IDC_TECHNIQUE_PRT )->SetChecked( true, true );
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
            // Demo hotkeys
            case 'Z': 
            {
                LoadSceneAndOptGenResults( DXUTGetD3DDevice(), L"PRT Demo\\cube_on_plane.x", 
                        L"PRT Demo\\cube_on_plane_1k_6b_prtresults.pca", 1024, 6, false ); 

                g_RenderingUI.EnableNonUserEvents( true );
                g_RenderingUI2.EnableNonUserEvents( true );
                g_RenderingUI3.EnableNonUserEvents( true );
                ResetUI();
                g_RenderingUI2.GetCheckBox( IDC_RENDER_TEXTURE )->SetChecked( false );
                g_RenderingUI.EnableNonUserEvents( false );
                g_RenderingUI2.EnableNonUserEvents( false );
                g_RenderingUI3.EnableNonUserEvents( false );
                break;
            }

            case 'X': 
            {
                LoadSceneAndOptGenResults( DXUTGetD3DDevice(), L"PRT Demo\\LandShark.x", 
                        L"PRT Demo\\02_LandShark_1k_prtresults.pca", 1024, 1, false ); 

                g_RenderingUI.EnableNonUserEvents( true );
                g_RenderingUI2.EnableNonUserEvents( true );
                g_RenderingUI3.EnableNonUserEvents( true );
                ResetUI();
                g_RenderingUI.EnableNonUserEvents( false );
                g_RenderingUI2.EnableNonUserEvents( false );
                g_RenderingUI3.EnableNonUserEvents( false );
                break;
            }

            case 'C': 
            {
                LoadSceneAndOptGenResults( DXUTGetD3DDevice(), L"PRT Demo\\wall_with_pillars.x", 
                    L"PRT Demo\\wall_with_pillars_1k_prtresults.pca", 1024, 1, false ); 

                g_RenderingUI.EnableNonUserEvents( true );
                g_RenderingUI2.EnableNonUserEvents( true );
                g_RenderingUI3.EnableNonUserEvents( true );
                ResetUI();
                g_RenderingUI.EnableNonUserEvents( false );
                g_RenderingUI2.EnableNonUserEvents( false );
                g_RenderingUI3.EnableNonUserEvents( false );
                break;
            }

            case 'V': 
            {
                LoadSceneAndOptGenResults( DXUTGetD3DDevice(), L"PRT Demo\\Head_Sad.x", 
                    L"PRT Demo\\Head_Sad_1k_prtresults.pca", 1024, 1, false ); 

                g_RenderingUI.EnableNonUserEvents( true );
                g_RenderingUI2.EnableNonUserEvents( true );
                g_RenderingUI3.EnableNonUserEvents( true );
                ResetUI();
                g_RenderingUI.EnableNonUserEvents( false );
                g_RenderingUI2.EnableNonUserEvents( false );
                g_RenderingUI3.EnableNonUserEvents( false );
                break;
            }

            case 'B': 
            {
                LoadSceneAndOptGenResults( DXUTGetD3DDevice(), L"PRT Demo\\Head_Big_Ears.x", 
                    L"PRT Demo\\Head_Big_Ears_1k_subsurf_prtresults.pca", 1024, 1, true ); 

                g_RenderingUI.EnableNonUserEvents( true );
                g_RenderingUI2.EnableNonUserEvents( true );
                g_RenderingUI3.EnableNonUserEvents( true );
                ResetUI();
                g_RenderingUI.GetSlider(IDC_ENVIRONMENT_1_SCALER)->SetValue( 0 ); 
                g_RenderingUI.GetSlider(IDC_ENVIRONMENT_2_SCALER)->SetValue( 0 ); 
                g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetRange( 0, 1000 );
                g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( 200 );
                g_RenderingUI.EnableNonUserEvents( false );
                g_RenderingUI2.EnableNonUserEvents( false );
                g_RenderingUI3.EnableNonUserEvents( false );
                break;
            }

            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
            case VK_F8:
            case 'W': g_bWireframe = !g_bWireframe; g_RenderingUI2.GetCheckBox( IDC_WIREFRAME )->SetChecked(g_bWireframe); break;
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

        case IDC_SCENE_1: KeyboardProc( 'Z', true, false ); break;
        case IDC_SCENE_2: KeyboardProc( 'X', true, false ); break;
        case IDC_SCENE_3: KeyboardProc( 'C', true, false ); break;
        case IDC_SCENE_4: KeyboardProc( 'V', true, false ); break;
        case IDC_SCENE_5: KeyboardProc( 'B', true, false ); break;

        case IDC_ACTIVE_LIGHT:
            if( !g_LightControl[g_nActiveLight].IsBeingDragged() )
            {
                g_nActiveLight++;
                g_nActiveLight %= g_nNumActiveLights;
            }
            break;

        case IDC_NUM_LIGHTS:
            if( !g_LightControl[g_nActiveLight].IsBeingDragged() )
            {
                WCHAR sz[100];
                _snwprintf( sz, 100, L"# Lights: %d", g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->GetValue() ); sz[99] = 0;
                g_RenderingUI2.GetStatic( IDC_NUM_LIGHTS_STATIC )->SetText( sz );

                g_nNumActiveLights = g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->GetValue();
                g_nActiveLight %= g_nNumActiveLights;
            }
            break;

        case IDC_LIGHT_SCALE: 
        {
            WCHAR sz[100];
            float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
            _snwprintf( sz, 100, L"Light scale: %0.2f", fLightScale ); sz[99] = 0;
            g_RenderingUI2.GetStatic( IDC_LIGHT_SCALE_STATIC )->SetText( sz );

            bool bEnable = (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() != 0 );
            g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->SetEnabled( bEnable );
            g_RenderingUI2.GetSlider( IDC_NUM_LIGHTS )->SetEnabled( bEnable );
            g_RenderingUI2.GetStatic( IDC_LIGHT_ANGLE_STATIC )->SetEnabled( bEnable );
            g_RenderingUI2.GetStatic( IDC_NUM_LIGHTS_STATIC )->SetEnabled( bEnable );
            g_RenderingUI2.GetButton( IDC_ACTIVE_LIGHT )->SetEnabled( bEnable );
            break;
        }

        case IDC_LIGHT_ANGLE:
        {
            WCHAR sz[100];
            int nLightAngle = g_RenderingUI2.GetSlider( IDC_LIGHT_ANGLE )->GetValue();
            _snwprintf( sz, 100, L"Cone Angle: %d", nLightAngle ); sz[99] = 0;
            g_RenderingUI2.GetStatic( IDC_LIGHT_ANGLE_STATIC )->SetText( sz );
            break;
        }

        case IDC_SIMULATOR:
            g_AppState = APP_STATE_SIMULATOR_OPTIONS;
            break;

        case IDC_LOAD_PRTBUFFER:
            g_AppState = APP_STATE_LOAD_PRT_BUFFER;
            break;

        case IDC_STOP_SIMULATOR:
            g_Simulator.Stop();
            g_AppState = APP_STATE_STARTUP;
            break;

        case IDC_ENVIRONMENT_A:
            g_dwSkyBoxA = PtrToInt( g_RenderingUI.GetComboBox( IDC_ENVIRONMENT_A )->GetSelectedData() );
            break;

        case IDC_ENVIRONMENT_B:
            g_dwSkyBoxB = PtrToInt( g_RenderingUI.GetComboBox( IDC_ENVIRONMENT_B )->GetSelectedData() );
            break;

        case IDC_TECHNIQUE_PRT:     
        {
            float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
            switch( g_dwTechnique ) 
            { 
                case 0: g_fLightScaleForPRT = fLightScale; break; 
                case 1: g_fLightScaleForSHIrrad = fLightScale; break; 
                case 2: g_fLightScaleForNdotL = fLightScale; break; 
            }
            g_dwTechnique = 0; 

            g_RenderingUI2.GetCheckBox( IDC_RENDER_MAP )->SetChecked( true );
            g_bRenderEnvMap = true;

            g_RenderingUI2.EnableNonUserEvents( true );
            g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( (int)(g_fLightScaleForPRT*100.0f) ); 
            g_RenderingUI2.EnableNonUserEvents( false );
            break;
        }

        case IDC_TECHNIQUE_SHIRRAD: 
        {
            float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
            switch( g_dwTechnique ) 
            { 
                case 0: g_fLightScaleForPRT = fLightScale; break; 
                case 1: g_fLightScaleForSHIrrad = fLightScale; break; 
                case 2: g_fLightScaleForNdotL = fLightScale; break; 
            }
            g_dwTechnique = 1; 
            g_RenderingUI2.GetCheckBox( IDC_RENDER_MAP )->SetChecked( true );
            g_bRenderEnvMap = true;
            g_RenderingUI2.EnableNonUserEvents( true );
            g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( (int)(g_fLightScaleForSHIrrad*100.0f) ); 
            g_RenderingUI2.EnableNonUserEvents( false );
            break;
        }

        case IDC_TECHNIQUE_NDOTL:   
        {
            float fLightScale = (float) (g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f);
            switch( g_dwTechnique ) 
            { 
                case 0: g_fLightScaleForPRT = fLightScale; break; 
                case 1: g_fLightScaleForSHIrrad = fLightScale; break; 
                case 2: g_fLightScaleForNdotL = fLightScale; break; 
            }
            g_dwTechnique = 2; 
            g_RenderingUI2.GetCheckBox( IDC_RENDER_MAP )->SetChecked( false );
            g_bRenderEnvMap = false;
            g_RenderingUI2.EnableNonUserEvents( true );
            g_RenderingUI2.GetSlider( IDC_LIGHT_SCALE )->SetValue( (int)(g_fLightScaleForNdotL*100.0f) ); 
            g_RenderingUI2.EnableNonUserEvents( false );
            break;
        }

        case IDC_COMPRESSION:
            if( g_PRTMesh.IsUncompressedBufferLoaded() )
            {
                g_bRenderCompressionUI = !g_bRenderCompressionUI;
            }
            else
            {
                MessageBox( DXUTGetHWND(), L"To change compression settings during rendering, please load an uncompressed buffer.  To make one use the simulator settings dialog to save an uncompressed buffer.", L"PRTPerVertex", MB_OK );
            }
            break;

        case IDC_RESTART:
            g_AppState = APP_STATE_STARTUP;
            break;

        case IDC_RENDER_UI: g_bRenderUI = g_RenderingUI2.GetCheckBox( IDC_RENDER_UI )->GetChecked(); break;
        case IDC_RENDER_MAP: g_bRenderEnvMap = g_RenderingUI2.GetCheckBox( IDC_RENDER_MAP )->GetChecked(); break;
        case IDC_RENDER_ARROWS: g_bRenderArrows = g_RenderingUI2.GetCheckBox( IDC_RENDER_ARROWS )->GetChecked(); break;
        case IDC_RENDER_MESH: g_bRenderMesh = g_RenderingUI2.GetCheckBox( IDC_RENDER_MESH )->GetChecked(); break;
        case IDC_WIREFRAME: g_bWireframe = g_RenderingUI2.GetCheckBox( IDC_WIREFRAME )->GetChecked(); break;
        case IDC_RENDER_TEXTURE: g_bRenderWithAlbedoTexture = g_RenderingUI2.GetCheckBox( IDC_RENDER_TEXTURE )->GetChecked(); break;
        case IDC_SH_PROJECTION: g_bRenderSHProjection = g_RenderingUI2.GetCheckBox( IDC_SH_PROJECTION )->GetChecked(); break;

        case IDC_NUM_PCA:
        {
            WCHAR sz[256];
            DWORD dwNumPCA = g_CompressionUI.GetSlider( IDC_NUM_PCA )->GetValue() * 4;
            swprintf( sz, L"Number of PCA: %d", dwNumPCA );
            g_CompressionUI.GetStatic( IDC_NUM_PCA_TEXT )->SetText( sz );           

            UpdateConstText();
            break;
        }

        case IDC_NUM_CLUSTERS:
        {
            WCHAR sz[256];
            DWORD dwNumClusters = g_CompressionUI.GetSlider( IDC_NUM_CLUSTERS )->GetValue();
            swprintf( sz, L"Number of Clusters: %d", dwNumClusters );
            g_CompressionUI.GetStatic( IDC_NUM_CLUSTERS_TEXT )->SetText( sz );           

            UpdateConstText();
            break;
        }

        case IDC_APPLY:
        {
            DWORD dwNumPCA = g_CompressionUI.GetSlider( IDC_NUM_PCA )->GetValue() * 4;
            DWORD dwNumClusters = g_CompressionUI.GetSlider( IDC_NUM_CLUSTERS )->GetValue();

            SIMULATOR_OPTIONS& options = GetGlobalOptions();
            options.dwNumClusters = dwNumClusters;
            options.dwNumPCA = dwNumPCA;
            GetGlobalOptionsFile().SaveOptions();

            g_PRTMesh.CompressBuffer( options.Quality, options.dwNumClusters, options.dwNumPCA );
            g_PRTMesh.ExtractCompressedDataForPRTShader();
            g_PRTMesh.LoadEffects( DXUTGetD3DDevice(), DXUTGetDeviceCaps() );
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
    for(int i=0; i<NUM_SKY_BOXES; ++i )
        g_Skybox[i].OnLostDevice();
    CDXUTDirectionWidget::StaticOnLostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );
    g_PRTMesh.OnLostDevice();
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
    for(int i=0; i<NUM_SKY_BOXES; ++i )
        g_Skybox[i].OnDestroyDevice();
    g_PRTMesh.OnDestroyDevice();
    CDXUTDirectionWidget::StaticOnDestroyDevice();
    SAFE_RELEASE( g_pFont );
    g_Simulator.Stop();
}



//-----------------------------------------------------------------------------
// update the dlg's text & controls
//-----------------------------------------------------------------------------
void UpdateConstText()
{
    WCHAR sz[256];
    DWORD dwNumClusters = g_CompressionUI.GetSlider( IDC_NUM_CLUSTERS )->GetValue();
    DWORD dwNumPCA = g_CompressionUI.GetSlider( IDC_NUM_PCA )->GetValue() * 4;
    DWORD dwMaxVertexShaderConst = DXUTGetDeviceCaps()->MaxVertexShaderConst;
    DWORD dwNumVConsts = dwNumClusters * (1 + 3*dwNumPCA/4) + 4;
    swprintf( sz, L"%d * (1 + (3*%d/4)) + 4 = %d", dwNumClusters, dwNumPCA, dwNumVConsts );
    g_CompressionUI.GetStatic( IDC_CUR_CONSTANTS )->SetText( sz );

    bool bEnable = ( dwNumVConsts < dwMaxVertexShaderConst );
    g_CompressionUI.GetButton( IDC_APPLY )->SetEnabled( bEnable );
}


