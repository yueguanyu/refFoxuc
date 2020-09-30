//--------------------------------------------------------------------------------------
// File: Viewer.cpp
//
// Creates a window for viewing a standard semantics and annotations mesh 
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "PreviewPipeline.h"
#include "XExporter.h"
#include "viewer.h"


#include <process.h>


#include<maya/MDagPath.h>
#include<maya/M3dView.h>
#include<maya/MFnCamera.h>
#include<maya/MFloatMatrix.h>
#include<maya/MMatrix.h>
#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>


CGrowableArray< CViewer* >& CViewer::GetViewerList()
{
    // Using an accessor function gives control of the construction order
    static CGrowableArray< CViewer* > ViewerList;
    return ViewerList;
}


//-------------------------------------------------------------------------------------
UINT __stdcall CViewer::StaticRunThread( void* pParam )
{
    CViewer* pViewer = (CViewer*) pParam;
    return pViewer->RunThread();
}


//-------------------------------------------------------------------------------------
LRESULT CALLBACK CViewer::StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // Loop through all known viewers and see if this message is intended for the owned
    // window
    CGrowableArray< CViewer* >& Viewers = GetViewerList();

    for( int i=0; 
        i < Viewers.GetSize(); 
        i++ )
    {
        CViewer* pViewer = Viewers[ i ];

        if( pViewer->GetRenderWindow() == hWnd )
            return pViewer->MsgProc( hWnd, uMsg, wParam, lParam );
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}





//-------------------------------------------------------------------------------------
CViewer::CViewer()
{
    // Add this instance to the global list
    GetViewerList().Add( this );

    // TODO: flush this out
    m_pFont = NULL;
    m_pTextSprite = NULL;
    m_bCreated = false;
    m_bVisible = false;
    m_hWndBound = NULL;

    m_dwWidth = 0;
    m_dwHeight = 0;
}


//-------------------------------------------------------------------------------------
CViewer::~CViewer()
{
    // Remove this instance from the global list
    GetViewerList().Remove( GetViewerList().IndexOf( this ) );
}


//-------------------------------------------------------------------------------------
UINT __stdcall CViewer::RunThread()
{
    EnterCriticalSection(&DeviceAndViewerSection);
    SetExclusiveMode(true, true);

    HRESULT hr= S_OK;

    // Register the window class
    WNDCLASS wndClass;
    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = StaticMsgProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = m_CreateArgs.hInstance;
    wndClass.hIcon = m_CreateArgs.hIcon;
    wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"Direct3DWindowClass";

    if( !RegisterClass( &wndClass ) )
    {
        DWORD dwError = GetLastError();
        if( dwError != ERROR_CLASS_ALREADY_EXISTS )
            return HRESULT_FROM_WIN32(dwError);
    }

    // Set the window's initial style.  It is invisible initially since it might
    // be resized later
    DWORD dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                            WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    
    // Create the render window
    m_hWndShell = (HWND) M3dView::applicationShell();
    m_hWnd = CreateWindow( L"Direct3DWindowClass", m_CreateArgs.strWindowTitle, dwWindowStyle,
                            m_CreateArgs.x, m_CreateArgs.y, m_CreateArgs.width, m_CreateArgs.height, M3dView::applicationShell(),
                            m_CreateArgs.hMenu, m_CreateArgs.hInstance, 0 );
    if( m_hWnd == NULL )
    {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32( dwError );
    }

    Hide();
    BindToWindow( NULL, false );

    // Start a timer to check for resize
    SetTimer( m_hWnd, 0xffff, 200, NULL );
            

	SetExclusiveMode(true, false);

    LeaveCriticalSection(&DeviceAndViewerSection);

    // Now that the window has been created start the message loop
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
  
		// Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
  			if(SetExclusiveMode(false, true))
			{
				// Render a frame during idle time (no messages are waiting)
				if( m_nPauseCount == 0 )
				{
					//LARGE_INTEGER perfCounter;
					//QueryPerformanceCounter( &perfCounter );

					//m_fTime = perfCounter / 1000.0f;
					m_fTime = (float) DXUTGetGlobalTimer()->GetAbsoluteTime();
					m_fElapsedTime = m_fTime - m_fPreviousTime;
					m_fPreviousTime = m_fTime;

					FrameMove();

					if(m_pSwapChain)
						Render();
				}

				SetExclusiveMode(false, false);
			}
            Sleep( 60 );
        }





		//// Use PeekMessage() so we can use idle time to render the scene. 
		//while(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 
		//&& WM_QUIT != msg.message)
		//{
		//	// Translate and dispatch the message
		//	TranslateMessage( &msg );
		//	DispatchMessage( &msg );
		//}

		//if(SetExclusiveMode(false, true))
		//{
		//	// Render a frame during idle time (no messages are waiting)
		//	if( m_nPauseCount == 0 )
		//	{
		//		m_fTime = (float) DXUTGetGlobalTimer()->GetAbsoluteTime();
		//		m_fElapsedTime = m_fTime - m_fPreviousTime;
		//		m_fPreviousTime = m_fTime;

		//		FrameMove();

		//		if(m_pSwapChain)
		//			Render();
		//	}

		//	SetExclusiveMode(false, false);
		//}
		//Sleep( 60 );

    }

    return 0;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::Create( CPreviewPipeline* pPreviewPipeline,    
                         D3DPRESENT_PARAMETERS* pPresentationParameters,
                         const WCHAR* strWindowTitle, 
                         HINSTANCE hInstance, 
                         HICON hIcon, 
                         HMENU hMenu,
                         int width,
                         int height,
                         int x, 
                         int y )
{
    // Start clean
    Destroy();
    
    m_dwWidth = width;
    m_dwHeight = height;

    m_pPreviewPipeline = pPreviewPipeline;

    // Register to receive device state notification
    m_pPreviewPipeline->RegisterDeviceEvents( this );
    m_pPreviewPipeline->RegisterManagerEvents( this );
    m_pPreviewPipeline->RegisterViewerEvents( this );

    if( hInstance == NULL )
        hInstance = GetModuleHandle( NULL );

    ZeroMemory( &m_CreateArgs, sizeof(CreateArgs) );
    m_CreateArgs.PresentParams = *pPresentationParameters;
    wcsncpy( m_CreateArgs.strWindowTitle, strWindowTitle, MAX_PATH );
    m_CreateArgs.hInstance = hInstance;
    m_CreateArgs.hIcon = hIcon;
    m_CreateArgs.hMenu = hMenu;
    m_CreateArgs.width = width;
    m_CreateArgs.height = height;
    m_CreateArgs.x = x;
    m_CreateArgs.y = y;

    // Launch the thread which will create the window and run the message/render loop
    m_hThread = _beginthreadex( NULL, 0, StaticRunThread, this, 0, &m_nThreadID );
    if( m_hThread == NULL )
        return E_FAIL;

    Run();
    return S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::Destroy()
{
    if( m_pPreviewPipeline )
    {
        m_pPreviewPipeline->UnregisterDeviceEvents( this );
        m_pPreviewPipeline->UnregisterManagerEvents( this );
        m_pPreviewPipeline->UnregisterViewerEvents( this );
    }

    if( m_hWnd != NULL )
        SendMessage( m_hWnd, WM_QUIT, 0, 0 );

    OnD3DDeviceLost();
	OnD3DDeviceDestroy();

    WaitForSingleObject( (HANDLE)m_hThread, 100 );
	TerminateThread((HANDLE)m_hThread, 0);
    CloseHandle( (HANDLE)m_hThread );

    m_hThread = 0;
    m_nThreadID = 0;

    return S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::Refresh()
{
    HRESULT hr = S_OK;
    
    LPDXCCFRAME pFrameRoot = m_pPreviewPipeline->AccessRoot();
    pFrameRoot->AddRef();

    
    m_EffectMap.Reset();
    
    // Traverse the frame tree to register the effects
    hr = RegisterEffectsInFrame( pFrameRoot );
    if( FAILED(hr) )
        goto LCleanReturn;


    m_EffectMap.AddEffect( m_pDefaultEffect );

    hr = S_OK;
    
LCleanReturn:
    SAFE_RELEASE( pFrameRoot );
    return S_OK;
}



//-----------------------------------------------------------------------------
HRESULT CViewer::RegisterEffectsInFrame( IDXCCFrame* pFrame)
{
    HRESULT hr = S_OK;

    for( UINT iMesh=0; iMesh < pFrame->NumMembers(); iMesh++ )
    {
        IDXCCMesh* pMesh = NULL;
        hr = pFrame->QueryMember( iMesh, IID_IDXCCMesh, (void**) &pMesh );
        if( FAILED(hr) )
            continue;

        V_RETURN( RegisterEffectsInMesh( pMesh ) );

        SAFE_RELEASE( pMesh );
    }

    for( UINT iChild=0; iChild < pFrame->NumChildren(); iChild++ )
    {
        IDXCCFrame* pChild = NULL;
        hr = pFrame->GetChild( iChild, &pChild );
        if( FAILED(hr) )
            continue;

        V_RETURN( RegisterEffectsInFrame( pChild ) );

        SAFE_RELEASE( pChild );
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::RegisterEffectsInMesh( IDXCCMesh* pMesh )
{
    HRESULT hr;

    ID3DXBuffer* pAttribs = NULL;
    DWORD dwNumAttribs = 0;
    V_RETURN( pMesh->GetAttributedMaterialList( &pAttribs, &dwNumAttribs ) );
    
    DWORD* pdwAttrib = NULL;
    if( pAttribs )
         pdwAttrib = (DWORD*) pAttribs->GetBufferPointer();

    for ( UINT iMaterial = 0; iMaterial < dwNumAttribs; iMaterial++ )
    {
        DWORD dwAttrib = *( pdwAttrib + iMaterial );

        ID3DXEffect* pEffect = NULL;
        hr = pMesh->GetAttributedMaterial( dwAttrib, &pEffect );
        if( FAILED(hr) )
            continue;
    
		if( pEffect )
		{
			hr= m_EffectMap.AddEffect( pEffect );
			if(DXCC_FAILED(hr))
			{
				SAFE_RELEASE( pEffect );
				return hr;
			}
		}
        
        SAFE_RELEASE( pEffect );
    }

    SAFE_RELEASE( pAttribs );

    return S_OK;
}


//-------------------------------------------------------------------------------------
void CViewer::BindToWindow( HWND hWnd, BOOL bVisible  )
{
    DWORD dwWindowStyle = 0;
    
    if( hWnd != NULL )
    {
        // Docking
        dwWindowStyle |= WS_CHILD;
    }
    else
    {
        // Undocking
        dwWindowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

        if( bVisible )
            dwWindowStyle |= WS_VISIBLE;


        SetWindowLongPtr( m_hWnd, GWLP_HWNDPARENT, (LONG_PTR)(LONG)(HWND)M3dView::applicationShell() );


    }
 
    // Switch window styles
#ifdef _WIN64
    LONG_PTR nResult = SetWindowLongPtr( m_hWnd, GWL_STYLE, (LONG_PTR)dwWindowStyle );
#else
    LONG_PTR nResult = SetWindowLongPtr( m_hWnd, GWL_STYLE, (LONG_PTR)(LONG)dwWindowStyle );
#endif 
 
    if( nResult == 0 )
    {
        DXUT_ERR_MSGBOX( L"SetWindowLongPtr", HRESULT_FROM_WIN32(GetLastError()) );
        return;
    }

    SetParent( m_hWnd, hWnd );
    
    /*
    WINDOWPLACEMENT placement = {0};
    placement.length = sizeof(WINDOWPLACEMENT);
    placement.showCmd = hWnd ? SW_MAXIMIZE : SW_NORMAL;
    SetWindowPlacement( DXUTGetHWND(), &placement );
    */

    nResult = SetWindowPos( m_hWnd, hWnd ? hWnd : 0, 0, 0, m_CreateArgs.width, m_CreateArgs.height, SWP_FRAMECHANGED | SWP_NOMOVE );
    if( nResult == 0 )
    {
        DXTRACE_ERR( L"SetWindowPos", HRESULT_FROM_WIN32(GetLastError()) );
        return;
    }

    m_hWndBound = hWnd;
    CheckForResize( true );
    
    // Calling show will in turn call Run(). We're also forcing Maya to completely repaint here
    // to avoid artifacts
    if( bVisible )
    {
        Show();
        ShowWindow( m_hWndShell, SW_HIDE );
        ShowWindow( m_hWndShell, SW_SHOW );
    }
}


//-------------------------------------------------------------------------------------
void CViewer::CheckForResize( BOOL bForceResize )
{
    HRESULT hr;

    RECT rcClient;
    GetClientRect( m_hWndBound ? m_hWndBound : m_hWnd, &rcClient );
    
    DWORD dwWindowWidth = rcClient.right - rcClient.left;
    DWORD dwWindowHeight = rcClient.bottom - rcClient.top;

	if(dwWindowWidth != 0 && dwWindowHeight != 0)
	{    
		if( bForceResize || (m_dwWidth != dwWindowWidth) || (m_dwHeight != dwWindowHeight))
		{
			m_dwWidth = dwWindowWidth;
			m_dwHeight = dwWindowHeight;
		    
			if( m_hWndBound )
			{
				SetWindowPos( m_hWnd, HWND_TOP, 
							0, 0, m_dwWidth, m_dwHeight, 
							( m_bVisible && m_dwWidth > 0 ) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW );
			}

			// Flush the resources and recreate the swap chain using the new dimensions
			V( OnD3DDeviceLost() );
			V( OnD3DDeviceDestroy() );
			V( OnD3DDeviceCreate() );
			V( OnD3DDeviceReset() );
		}
	}
}


//-------------------------------------------------------------------------------------
void CViewer::Hide()
{
    m_bVisible = false;
    ShowWindow( m_hWnd, SW_HIDE );
    Pause();
}


//-------------------------------------------------------------------------------------
void CViewer::Show()
{
    m_bVisible = true;
    ShowWindow( m_hWnd, SW_SHOW );
    Run();
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::GetD3DSwapChain( IDirect3DSwapChain9** ppSwapChain )
{
    *ppSwapChain = m_pSwapChain;

    if( m_pSwapChain == NULL )
        return E_FAIL;

    return S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::OnD3DDeviceCreate()
{
    CPreviewPipelineLock Lock;
    DeviceLock( Lock );

    HRESULT hr;
    IDirect3DDevice9* pd3dDevice = NULL;
    
    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    m_Camera.SetViewParams( &vecEye, &vecAt );

    CPreviewPipelineEngine* pEngine = m_pPreviewPipeline->AccessEngine();
    hr = pEngine->GetD3DDevice( &pd3dDevice );

    if( FAILED(hr) || pd3dDevice == NULL )
        goto LCleanReturn;

    // Initialize the font
    V( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &m_pFont ) );
    if( FAILED(hr) )
        goto LCleanReturn;

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

    // Find the default effect file
    WCHAR strPath[MAX_PATH+1];
    V_RETURN( DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"UI\\Default.fx" ) );
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, strPath, NULL, NULL, dwShaderFlags, NULL, &m_pDefaultEffect, NULL ) );

    hr = Refresh();
    if( FAILED(hr) )
        goto LCleanReturn;

    // Read the mesh
    //m_Viewer.Create( pd3dDevice, L"tiger\\tiger.x", 0, dwShaderFlags );
    m_LightWidget.StaticOnCreateDevice( pd3dDevice );
   
    m_LightWidget.SetLightDirection( D3DXVECTOR3(1.0f, 0.0f, -1.0f) );

    // TODO: move this to the load function and rely on the object radius
    m_Camera.SetRadius( 3.0f, 0.5f, 10.0f );
    m_LightWidget.SetRadius( 0.5f );

    hr = S_OK;

LCleanReturn:
    SAFE_RELEASE( pd3dDevice );
    
    m_bCreated = true;

    DeviceUnlock( Lock );
    return hr;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::OnD3DDeviceReset()
{
    CPreviewPipelineLock Lock;
    DeviceLock( Lock );

    HRESULT hr;

    CPreviewPipelineEngine* pEngine = m_pPreviewPipeline->AccessEngine();
    IDirect3DSurface9* pBackBuffer = NULL;
    IDirect3DDevice9* pd3dDevice = NULL;
    IDirect3DSurface9* pSurfDeviceDepthStencil = NULL;
    
	hr = pEngine->GetD3DDevice( &pd3dDevice );
    if( FAILED(hr) || pd3dDevice == NULL )
        goto LCleanReturn;

    // Create the render chain
    D3DPRESENT_PARAMETERS pp;
    pp = m_CreateArgs.PresentParams;
    pp.BackBufferWidth = m_dwWidth;
    pp.BackBufferHeight = m_dwHeight;
    V( pd3dDevice->CreateAdditionalSwapChain( &pp, &m_pSwapChain ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    // Create the depth stencil surface
    V( pd3dDevice->GetDepthStencilSurface( &pSurfDeviceDepthStencil ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    D3DSURFACE_DESC desc;
    V( pSurfDeviceDepthStencil->GetDesc( &desc ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    V( pd3dDevice->CreateDepthStencilSurface( m_dwWidth, m_dwHeight, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality, false, &m_pDepthStencil, NULL ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    V( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    D3DSURFACE_DESC BackBufferDesc;
    V( pBackBuffer->GetDesc( &BackBufferDesc ) );
    SAFE_RELEASE( pBackBuffer );

    if( FAILED(hr) )
        goto LCleanReturn;

    if( m_pDefaultEffect )
        m_pDefaultEffect->OnResetDevice();

    m_LightWidget.OnResetDevice( &BackBufferDesc );

    if( m_pFont )
    {
        V( m_pFont->OnResetDevice() );
        if( FAILED(hr) )
            goto LCleanReturn;
    }
    
    // Create a sprite to help batch calls when drawing many lines of text
    V( D3DXCreateSprite( pd3dDevice, &m_pTextSprite ) );
    if( FAILED(hr) )
        goto LCleanReturn;

    // Setup the camera's projection parameters
    float fAspectRatio;
    fAspectRatio = BackBufferDesc.Width / (FLOAT)BackBufferDesc.Height;
    
    m_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
    m_Camera.SetWindow( BackBufferDesc.Width, BackBufferDesc.Height );
    m_Camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON );

    hr = S_OK;

LCleanReturn:
    SAFE_RELEASE( pBackBuffer );
    SAFE_RELEASE( pd3dDevice );
    SAFE_RELEASE( pSurfDeviceDepthStencil );

    Run();

    DeviceUnlock( Lock );
    return hr;
}



//--------------------------------------------------------------------------------------
// Before handling window messages, the sample framework passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then the sample framework will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CViewer::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // Pass windows messages to camera so it can respond to user input
    m_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    m_LightWidget.HandleMessages( hWnd, uMsg, wParam, lParam );

    switch( uMsg )
    {
        case WM_CLOSE:
            Hide();
            return 0;

        case WM_PAINT:
            Render();
            break;

        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_ESCAPE:
                    Destroy();
                    break;
            }

            break;
        }

        case WM_TIMER:
            CheckForResize();
            break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}



//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CViewer::FrameMove()
{
    // Update the camera's position based on user input 
    m_Camera.FrameMove( m_fElapsedTime );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CViewer::Render()
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;

    
    if( m_bCreated == false || m_pSwapChain == NULL)
        return;

    // Get read lock
    CPreviewPipelineLock DeviceLock;
    CPreviewPipelineLock SceneLock;
	m_pPreviewPipeline->SceneReadLock( true , SceneLock);
	m_pPreviewPipeline->DeviceStateLock( true , DeviceLock);


    CPreviewPipelineEngine* pEngine = m_pPreviewPipeline->AccessEngine();
    IDirect3DDevice9* pd3dDevice = NULL;
    hr = pEngine->GetD3DDevice( &pd3dDevice );
    if( FAILED(hr) || pd3dDevice == NULL )
        return;

    IDirect3DSurface9* pSurfOldRenderTarget = NULL;
    V( pd3dDevice->GetRenderTarget( 0, &pSurfOldRenderTarget ) );

    IDirect3DSurface9* pSurfOldDepthStencil = NULL;
    V( pd3dDevice->GetDepthStencilSurface( &pSurfOldDepthStencil ) );
    V( pd3dDevice->SetDepthStencilSurface( m_pDepthStencil ) );

    IDirect3DSurface9* pSurfNewRenderTarget = NULL;
    hr= m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSurfNewRenderTarget );
    V( pd3dDevice->SetRenderTarget( 0, pSurfNewRenderTarget ) );
   	SAFE_RELEASE( pSurfNewRenderTarget );
 
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 100), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // Get the projection & view matrix from the camera class
        mWorld = *m_Camera.GetWorldMatrix();
        mView = *m_Camera.GetViewMatrix();
        mProj = *m_Camera.GetProjMatrix();

		mView= g_PreviewPipeline.PerspectiveCamera_GetView();
 		mProj= g_PreviewPipeline.PerspectiveCamera_GetProjection();
        
        // Adjust for aspect ratio
        mProj._22 *= (float)m_dwWidth / m_dwHeight;

        m_EffectMap.SetWorldMatrix( &mWorld );
        m_EffectMap.SetViewMatrix(&mView ); 
        m_EffectMap.SetProjectionMatrix( &mProj ); 

        m_EffectMap.SetStandardParameter( DXUT_Time, DXUT_Geometry, 0, (float*) &m_fTime, 1 );  
        m_EffectMap.SetStandardParameter( DXUT_Direction, DXUT_Light, 0, (float*) &m_LightWidget.GetLightDirection(), 3 );

        //debug
        D3DXVECTOR4 vDiffuse( 1, 1, 1, 1 );
        m_EffectMap.SetStandardParameter( DXUT_Diffuse, DXUT_Light, 0, (float*) &vDiffuse, 4 );

        // debug

        // Set the time parameter
        m_EffectMap.SetStandardParameter( DXUT_Time, DXUT_Geometry, 0, &m_fTime, 1 );

        
        LPDXCCFRAME pFrameRoot = m_pPreviewPipeline->AccessRoot();
		pFrameRoot->AddRef();
        
        if( pFrameRoot != NULL )
            DrawFrame( pFrameRoot );
        
        SAFE_RELEASE( pFrameRoot );


        //TODO: get the eye point from the maya camera
        //V( m_LightWidget.OnRender( D3DCOLOR_ARGB(255, 255, 255, 255), &mView, &mProj, m_Camera.GetEyePt() ) );
        
        //if( m_pFont && m_pTextSprite )
        //    RenderText();

        V( pd3dDevice->EndScene() );

		if(m_pSwapChain)
			V( m_pSwapChain->Present( NULL, NULL, m_hWnd, NULL, 0 ) );

    }

    pd3dDevice->SetRenderTarget( 0, pSurfOldRenderTarget );
	SAFE_RELEASE( pSurfOldRenderTarget );
    pd3dDevice->SetDepthStencilSurface( pSurfOldDepthStencil );
	SAFE_RELEASE( pSurfOldDepthStencil );

    SAFE_RELEASE( pd3dDevice );
	
	m_pPreviewPipeline->SceneReadUnlock(SceneLock);
	m_pPreviewPipeline->DeviceStateUnlock(DeviceLock);

}


//-----------------------------------------------------------------------------
// Name: DrawFrame()
// Desc: Called to render a frame in the hierarchy
//-----------------------------------------------------------------------------
void CViewer::DrawFrame( IDXCCFrame* pFrame)
{
    HRESULT hr = S_OK;

    if(pFrame->NumMembers() > 0)
    {
        D3DXMATRIXA16 mWorld;
        D3DXMATRIXA16 mView;
        D3DXMATRIXA16 mProj;
        D3DXMATRIXA16 mWorldViewProjection;


        mWorld= D3DXMATRIXA16(*pFrame->GetWorldMatrix());

        m_EffectMap.SetWorldMatrix(&mWorld );
    }

    for( UINT iMesh=0; iMesh < pFrame->NumMembers(); iMesh++ )
    {
        IDXCCMesh* pMesh = NULL;
        hr = pFrame->QueryMember( iMesh, IID_IDXCCMesh, (void**) &pMesh );
        if( FAILED(hr) )
            continue;

        DrawMesh( pMesh );

		SAFE_RELEASE(pMesh);
    }

    for( UINT iChild=0; iChild < pFrame->NumChildren(); iChild++ )
    {
        IDXCCFrame* pChild = NULL;
        hr = pFrame->GetChild( iChild, &pChild );
        if( FAILED(hr) )
            continue;

        DrawFrame( pChild );

		SAFE_RELEASE(pChild);
    }
}


//--------------------------------------------------------------------------------------
void CViewer::DrawMesh( IDXCCMesh* pMesh )
{
    HRESULT hr;

    ID3DXBuffer* pAttribs = NULL;
    DWORD dwNumAttribs = 0;
    hr = pMesh->GetAttributedMaterialList( &pAttribs, &dwNumAttribs );
    if( FAILED(hr) )
        return;

    DWORD* pdwAttrib = NULL;
    if( pAttribs )
         pdwAttrib = (DWORD*) pAttribs->GetBufferPointer();

    if( dwNumAttribs > 0 )
    {
        for( UINT iMaterial = 0; iMaterial < dwNumAttribs; iMaterial++ )
        {
            DWORD dwAttrib = *( pdwAttrib + iMaterial );

            ID3DXEffect* pEffect = NULL;
            hr = pMesh->GetAttributedMaterial( dwAttrib, &pEffect );
            if( FAILED(hr))
                continue;
			
			if(!pEffect)
			{
				m_pDefaultEffect->AddRef();
				pEffect=m_pDefaultEffect;
			}
        
            D3DXHANDLE hTechnique;
            V( pEffect->FindNextValidTechnique( NULL, &hTechnique ) );
            V( pEffect->SetTechnique( hTechnique ) );

            UINT numPasses;
            V( pEffect->Begin(&numPasses, 0) );

            for( UINT iPass = 0; iPass < numPasses; iPass++ )
            {
                V( pEffect->BeginPass(iPass) );

                // The effect interface queues up the changes and performs them 
                // with the CommitChanges call. You do not need to call CommitChanges if 
                // you are not setting any parameters between the BeginPass and EndPass.
                // V( pEffect->CommitChanges() );

                // Render the mesh with the applied technique
                V( pMesh->DrawSubset( dwAttrib ) );
                
                V( pEffect->EndPass() );
            }
            V( pEffect->End() );

			SAFE_RELEASE(pEffect);
        }
    }
    else
    {   
        if( m_pDefaultEffect )
        {
            D3DXHANDLE hTechnique;
            V( m_pDefaultEffect->FindNextValidTechnique( NULL, &hTechnique ) );
            V( m_pDefaultEffect->SetTechnique( hTechnique ) );

            UINT numPasses;
            V( m_pDefaultEffect->Begin(&numPasses, 0) );

            for( UINT iPass = 0; iPass < numPasses; iPass++ )
            {
                V( m_pDefaultEffect->BeginPass(iPass) );

                // The effect interface queues up the changes and performs them 
                // with the CommitChanges call. You do not need to call CommitChanges if 
                // you are not setting any parameters between the BeginPass and EndPass.
                // V( pEffect->CommitChanges() );

                // Render the mesh with the applied technique
                V( pMesh->DrawSubset( 0 ) );
                
                V( m_pDefaultEffect->EndPass() );
            }
            V( m_pDefaultEffect->End() );
        }
      
    }

    SAFE_RELEASE( pAttribs );
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void CViewer::RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( m_pFont, m_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );

    WCHAR strBuffer[MAX_PATH+1] = {0};
    _snwprintf( strBuffer, MAX_PATH, L"Time: %f", m_fTime );
    txtHelper.DrawTextLine( strBuffer );
    
    txtHelper.End();
}


//-------------------------------------------------------------------------------------
void CViewer::DeviceLock( CPreviewPipelineLock& Lock )
{
    if( NULL != m_pPreviewPipeline )
    {
        m_pPreviewPipeline->DeviceStateLock( true , Lock);
    }
}


//-------------------------------------------------------------------------------------
void CViewer::DeviceUnlock( CPreviewPipelineLock& Lock )
{
    if( NULL != m_pPreviewPipeline )
    {
        m_pPreviewPipeline->DeviceStateUnlock( Lock );
    }
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::OnD3DDeviceLost()
{ 
    CPreviewPipelineLock Lock;
    DeviceLock( Lock );

    UINT ref;

	Pause();

    m_LightWidget.StaticOnLostDevice();
    
    if( m_pFont )
        m_pFont->OnLostDevice();
    
    if( m_pDefaultEffect )
        m_pDefaultEffect->OnLostDevice();

    ref= DXCC_RELEASE( m_pDepthStencil );
    ref= DXCC_RELEASE( m_pTextSprite );
    ref= DXCC_RELEASE( m_pSwapChain );

    DeviceUnlock( Lock );
    return S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT CViewer::OnD3DDeviceDestroy()
{
    CPreviewPipelineLock Lock;
    DeviceLock( Lock );

	UINT ref;
    m_bCreated = false;

    m_EffectMap.Reset();
	m_LightWidget.StaticOnDestroyDevice();
    ref= DXCC_RELEASE( m_pFont );
    ref= DXCC_RELEASE( m_pTextSprite );
    ref= DXCC_RELEASE( m_pDefaultEffect );

    DeviceUnlock( Lock );
    return S_OK;
}



