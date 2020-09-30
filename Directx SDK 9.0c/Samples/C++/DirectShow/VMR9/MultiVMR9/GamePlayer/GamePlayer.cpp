//------------------------------------------------------------------------------
// File: GamePlayer.cpp
//
// Desc: DirectShow sample code - MultiVMR9 GamePlayer
// Defines the class behaviors for the application
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "GamePlayer.h"
#include "StartDialog.h"
// #include "dxutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HINSTANCE g_hInstance = NULL;

// CGamePlayerApp

BEGIN_MESSAGE_MAP(CGamePlayerApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGamePlayerApp construction

CGamePlayerApp::CGamePlayerApp()
{
}


// The one and only CGamePlayerApp object

CGamePlayerApp theApp;


//----------------------------------------------------------------------------
//  VerifyVMR9
// 
//  Verifies that VMR9 COM objects exist on the system and that the VMR9
//  can be instantiated.
//
//  Returns: FALSE if the VMR9 can't be created
//----------------------------------------------------------------------------

BOOL CGamePlayerApp::VerifyVMR9(void)
{
    HRESULT hr;

    // Verify that the VMR exists on this system
    IBaseFilter* pBF = NULL;
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL,
                          CLSCTX_INPROC,
                          IID_IBaseFilter,
                          (LPVOID *)&pBF);
    if(SUCCEEDED(hr))
    {
        pBF->Release();
        return TRUE;
    }
    else
    {
        MessageBox(NULL, 
            TEXT("This application requires the Video Mixing Renderer, which is present\r\n")
            TEXT("only on DirectX 9 systems with hardware video acceleration enabled.\r\n\r\n")

            TEXT("The Video Mixing Renderer (VMR9) is not enabled when viewing a \r\n")
            TEXT("remote Windows XP machine through a Remote Desktop session.\r\n")
            TEXT("You can run VMR-enabled applications only on your local machine.\r\n\r\n")

            TEXT("To verify that hardware acceleration is enabled on a Windows XP\r\n")
            TEXT("system, follow these steps:\r\n")
            TEXT("-----------------------------------------------------------------------\r\n")
            TEXT(" - Open 'Display Properties' in the Control Panel\r\n")
            TEXT(" - Click the 'Settings' tab\r\n")
            TEXT(" - Click on the 'Advanced' button at the bottom of the page\r\n")
            TEXT(" - Click on the 'Troubleshooting' tab in the window that appears\r\n")
            TEXT(" - Verify that the 'Hardware Acceleration' slider is at the rightmost position\r\n")

            TEXT("\r\nThis sample will now exit."),

            TEXT("Video Mixing Renderer (VMR9) capabilities are required"), MB_OK);

        return FALSE;
    }
}

// CGamePlayerApp initialization

BOOL CGamePlayerApp::InitInstance()
{
    // InitCommonControls() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCommonControls();

    CWinApp::InitInstance();

    g_hInstance = m_hInstance;

    HRESULT hr = CoInitialize(NULL);

    // Verify that the VMR9 is present on this system
    if(!VerifyVMR9())
    {
        CoUninitialize();
        return FALSE;
    }

    CStartDialog dlg;
    m_pMainWnd = &dlg;

    dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

///////////////////////////////////// CGamePlayerSession //////////////////////////
/*********************************************************************************\
* CGamePlayerSession
* constructor
\*********************************************************************************/
CGamePlayerSession::CGamePlayerSession()
    : CMultigraphSession()
    , m_pMixer( NULL )
    , m_pUI( NULL )
{
}

/*********************************************************************************\
* ~CGamePlayerSession
* destructor
\*********************************************************************************/
CGamePlayerSession::~CGamePlayerSession()
{
    Terminate();
    RELEASE( m_pMixer );
    RELEASE( m_pUI );
}

/*********************************************************************************\
* Initialize
* loads and initializes custom mixer control and UI Layer
\*********************************************************************************/
HRESULT CGamePlayerSession::Initialize()
{
    HRESULT hr = S_OK;
    CComPtr<IMultiVMR9RenderEngine> pRenderEngine;

    try
    {
        if( m_pWizard )
            throw VFW_E_WRONG_STATE;

        // create wizard
        hr = CoCreateInstance(  CLSID_MultiVMR9Wizard,
                                NULL,
                                CLSCTX_INPROC_SERVER,
                                IID_IMultiVMR9Wizard,
                                (void**)&m_pWizard);
        CHECK_HR( hr, DbgMsg("CGamePlayerSession::Initialize: Failed to create instance of MultiVMR9Wizard, hr = 0x%08x", hr));

        // create render engine
        hr = CoCreateInstance(  CLSID_MultiVMR9RenderEngine,
                                NULL,
                                CLSCTX_INPROC_SERVER,
                                IID_IMultiVMR9RenderEngine,
                                (void**)&(pRenderEngine.p));
        CHECK_HR( hr, DbgMsg("CGamePlayerSession::Initialize: Failed to create instance of MultiVMR9RenderEngine, hr = 0x%08x", hr));

        // create video window
        CHECK_HR(
            hr = CreateVideoWindow_(800,600,WS_OVERLAPPED|WS_BORDER),
            DbgMsg("CGamePlayerSession::Initialize: failed to create video window"));

        // create mixer control
        m_pMixer = new CGameMixer(NULL, &hr);

        CHECK_HR(
            FAILED(hr) ? hr : (!m_pMixer ? E_OUTOFMEMORY : S_OK),
            DbgMsg("CGamePlayerSession::Initialize: failed to create custom mixer control"));

        m_pMixer->AddRef();

        // create UI layer
        m_pUI = new CGameUILayer( NULL, &hr);

        CHECK_HR(
            FAILED(hr) ? hr : (!m_pUI ? E_OUTOFMEMORY : S_OK),
            DbgMsg("CGamePlayerSession::Initialize: failed to create custom UI layer"));

        m_pUI->AddRef();

        CHECK_HR(
            hr = m_pUI->SetMixer( m_pMixer),
            DbgMsg("CGamePlayerSession::Initialize: failed to sync mixer and UI layer"));

        // initialize render engine with custom mixer control and UI layer
        CHECK_HR(
            hr = pRenderEngine->Initialize( m_hwndVideo, NULL, m_pMixer, m_pUI),
            DbgMsg("CGamePlayerSession::Initialize: failed to initialize customized render engine, hr = 0x%08x", hr));

        // initialize wizard with customized render engine
        CHECK_HR(
            hr = m_pWizard->Initialize(NULL, m_hwndVideo, pRenderEngine),
            DbgMsg("CMultiPlayerSession::Initialize: failed to initialize the wizard, hr = 0x%08x", hr));

        // get interfaces
        CHECK_HR(
            hr = m_pWizard->GetRenderEngine( &m_pRenderEngine),
            DbgMsg("CMultiPlayerSession::Initialize: failed to get Render Engine, hr = 0x%08x", hr));

        CHECK_HR(
            hr = m_pRenderEngine->GetMixerControl( &m_pMixerControl),
            DbgMsg("CMultiPlayerSession::Initialize: failed to get Mixer Control, hr = 0x%08x", hr));

        CHECK_HR(
            hr = m_pRenderEngine->GetUILayer( &m_pUILayer),
            DbgMsg("CMultiPlayerSession::Initialize: failed to get UILayer, hr = 0x%08x", hr));

        CHECK_HR(
            hr = m_pRenderEngine->SetFrameRate( 5000 ),
            DbgMsg("CMultiPlayerSession::Initialize: failed to set frame rate, hr = 0x%08x", hr));

        ::ShowWindow( m_hwndVideo, SW_SHOW);
    }
    catch( HRESULT hr1 )
    {

        if( pRenderEngine )
        {
            pRenderEngine->Terminate();
        }

        hr = hr1;
    }

    return hr;
}
/*********************************************************************************\
* Terminate
* 
\*********************************************************************************/
HRESULT CGamePlayerSession::Terminate()
{
	HRESULT hr = S_OK;

	hr = CMultigraphSession::Terminate();
	return hr;
}


HRESULT GetDXSDKMediaPathCch( TCHAR* strDest, int cchDest )
{
    if( strDest == NULL || cchDest < 1 )
        return E_INVALIDARG;

    lstrcpy( strDest, TEXT("") );

    // Open the appropriate registry key
    HKEY hKey;
    LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                 TEXT("Software\\Microsoft\\DirectX SDK"),
                                 0, KEY_READ, &hKey );
    if( ERROR_SUCCESS != lResult ) 
        return E_FAIL;

    DWORD dwType;
    DWORD dwSize = cchDest * sizeof(WCHAR);
    lResult = RegQueryValueEx( hKey, TEXT("DX9S4SDK Samples Path"), NULL,
                               &dwType, (BYTE*)strDest, &dwSize );
    strDest[cchDest-1] = 0; // RegQueryValueEx doesn't NULL term if buffer too small
    RegCloseKey( hKey );

    if( ERROR_SUCCESS != lResult )
        return E_FAIL;

    const TCHAR* strMedia = TEXT("\\Media\\");
    if( lstrlen(strDest) + lstrlen(strMedia) < cchDest )
        _tcscat( strDest, strMedia );
    else
        return E_INVALIDARG;

    return S_OK;
}

/*********************************************************************************\
* FindMediaFile
* Searches for a media file in (a) current directory, (b) DirectX SDK media folder,
* (c) resources
\*********************************************************************************/
HRESULT FindMediaFile(  TCHAR* achPath, 
                        DWORD size, 
                        TCHAR* achName, 
                        LPCTSTR lpResourceName, 
                        LPCTSTR lpResourceType )
{
    HRESULT hr = S_OK;
    
    TCHAR achCurDirPath[ MAX_PATH ];
    TCHAR achShortName[ MAX_PATH ];
    TCHAR *pch = NULL;

    bool bSuccess = false;  
    HGLOBAL hResMem = NULL;
    DWORD dwSize = 0L;

    if( !achPath || !achName /*|| !lpResourceName */ )
    {
        return E_POINTER;
    }


    pch = _tcsrchr( achName, _T('\\'));
    if( pch )
    {
        _tcsncpy( achShortName, pch+1, MAX_PATH );
    }
    else
    {
        _tcsncpy( achShortName, achName, MAX_PATH );
    }
    
    // first, try current directory
    {
        GetCurrentDirectory( MAX_PATH, achCurDirPath );
        _tcsncat( achCurDirPath, TEXT("\\"), MAX_PATH);
        _tcsncat( achCurDirPath, achShortName, MAX_PATH);

        if( INVALID_FILE_ATTRIBUTES != ::GetFileAttributes( achCurDirPath ))
        {
            // file is in the current folder
            if( size < _tcslen( achCurDirPath ))
                return E_OUTOFMEMORY;

            _tcsncpy( achPath, achCurDirPath, size );
            return S_OK;
        }
    }

    // second, try DXSDK media folder
    {
        TCHAR achPathFull[ MAX_PATH ];
        TCHAR achDXSDK_Media_Path[ MAX_PATH ];

        hr = GetDXSDKMediaPathCch( achDXSDK_Media_Path, MAX_PATH );
        if( S_OK == hr )
        {
            _tcsncpy( achPathFull, achDXSDK_Media_Path, MAX_PATH );
            // _tcsncat( achPathFull, TEXT("\\") , MAX_PATH );
            _tcsncat( achPathFull, achName, MAX_PATH );
            
            if( INVALID_FILE_ATTRIBUTES != ::GetFileAttributes( achPathFull ))
            {
                // file is in the DXSDK media folder
                if( size < _tcslen( achPathFull ))
                    return E_OUTOFMEMORY;

                _tcsncpy( achPath, achPathFull, size );
                return S_OK;
            }
        }
    }

    // finally, try to load file from resources
    if( lpResourceName )
    {
        HANDLE hFile = NULL;
        HRSRC hRsrc = NULL;

        hFile = ::CreateFile( achCurDirPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if( INVALID_HANDLE_VALUE == hFile )
        {
            return E_FAIL;
        }

        hRsrc = FindResource( AfxGetInstanceHandle(), lpResourceName, lpResourceType);
        if( NULL != hRsrc )
        {
            // if the resource is a bitmap
            if( lpResourceType == RT_BITMAP )
            {
                HBITMAP hBitmap = NULL;
                hBitmap = LoadBitmap( AfxGetInstanceHandle(), lpResourceName);
                if( hBitmap )
                {
                    CBitmap* pBitmap = NULL;
                    BITMAP bitmap;
                    BITMAPINFOHEADER bmpHdr;
                    LPVOID pData = NULL;
                    BITMAPFILEHEADER bmpFHdr;
                    DWORD dwWritten = 0L;

                    pBitmap = CBitmap::FromHandle( hBitmap );
                    pBitmap->GetBitmap( &bitmap );

                    ZeroMemory( &bmpHdr, sizeof(BITMAPINFOHEADER));

                    bmpHdr.biSize = sizeof(BITMAPINFOHEADER);
                    bmpHdr.biWidth = bitmap.bmWidth;
                    bmpHdr.biHeight = -bitmap.bmHeight;
                    bmpHdr.biPlanes = bitmap.bmPlanes;
                    bmpHdr.biBitCount = bitmap.bmBitsPixel;
                    bmpHdr.biCompression = bitmap.bmType;
                    bmpHdr.biSizeImage = bitmap.bmHeight * bitmap.bmWidthBytes;

                    pData = malloc(bmpHdr.biSizeImage);
                    pBitmap->GetBitmapBits( bmpHdr.biSizeImage, pData );
                    
                    bmpFHdr.bfType = static_cast<WORD>(L'B'|(L'M'<<8));
                    bmpFHdr.bfSize = 14 + sizeof(BITMAPINFOHEADER) + bmpHdr.biSizeImage;
                    bmpFHdr.bfOffBits = 14 + bmpHdr.biSize;
                    bmpFHdr.bfReserved1    = 0;
                    bmpFHdr.bfReserved2    = 0;

                    if( TRUE == WriteFile(hFile, static_cast<LPVOID>(&bmpFHdr), 14, &dwWritten, NULL) )
                    {
                        if( TRUE == WriteFile(hFile, static_cast<LPVOID>(&bmpHdr), bmpHdr.biSize, &dwWritten, NULL))
                        {
                            if( TRUE == WriteFile(hFile, static_cast<LPVOID>(pData), bmpHdr.biSizeImage, &dwWritten, NULL))
                            {
                                bSuccess = true;
                            }
                        }
                    }
                    free( pData );
                    pData = NULL;
                }
            }
        }
        CloseHandle( hFile );
    }

    if( false == bSuccess )
    {
        return E_FAIL;
    }

    if( size < _tcslen( achCurDirPath ))
        return E_OUTOFMEMORY;

    _tcsncpy( achPath, achCurDirPath, size );
    return S_OK;
}

/*********************************************************************************\
* DbgMsg
* debug tracing function
\*********************************************************************************/

#ifdef _DEBUG
    void DbgMsg( char* szMessage, ... )
    {
        char szFullMessage[MAX_PATH];
        char szFormatMessage[MAX_PATH];
        DWORD dwWritten = 0L;
        size_t len = 0;

        // format message
        va_list ap;
        va_start(ap, szMessage);
        _vsnprintf( szFormatMessage, MAX_PATH, szMessage, ap);
        va_end(ap);
        len = strlen( szFormatMessage );
        strncat( szFormatMessage, "\n", MAX_PATH - len);
        strcpy( szFullMessage, "GamePlayer: ");
        len = strlen( szFullMessage );
        strncat( szFullMessage, szFormatMessage, MAX_PATH - len );
        OutputDebugStringA( szFullMessage );
    }
#else
    void DbgMsg( char* szMessage, ... ){;}
#endif

