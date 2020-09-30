//------------------------------------------------------------------------------
// File: BGMusic.cpp
//
// Desc: A simple playback applicaiton that plays a cyclic set of media
//       files of the same type. This is the code required to use DirectShow
//       to play compressed audio in the background of your title in a
//       seamless manner.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//------------------------------------------------------------------------------

#include <windows.h>
#include <dshow.h>
#include <tchar.h>
#include <stdio.h>
#include <atlbase.h>
#include "resource.h"


//------------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------------
HRESULT GraphInit(void);
HRESULT SwapSourceFilter(void); 
HWND AppInit(HINSTANCE hInstance);

void AppMessageLoop(void);
void AppCleanUp(void);
void ShowCurrentFile(HWND hWnd);
HRESULT DXUtil_GetDXSDKMediaPathCch( TCHAR* strDest, int cchDest );
HRESULT DXUtil_FindMediaFileCch( TCHAR* strDestPath, int cchDest, LPCTSTR strFilename );
bool DXUtil_FindMediaSearchTypicalDirs( TCHAR* strSearchPath, int cchSearch, LPCTSTR strLeaf, TCHAR* strExePath, TCHAR* strExeName, TCHAR* strMediaDir );
bool DXUtil_FindMediaSearchParentDirs( TCHAR* strSearchPath, int cchSearch, TCHAR* strStartAt, TCHAR* strLeafName );

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID CALLBACK MyTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
void AddAboutMenuItem(HWND hWnd);
LRESULT CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define CLASSNAME       TEXT("BGMusicPlayer\0")
#define APPNAME         TEXT("BGMusic Player\0")
#define APPWIDTH        200
#define APPHEIGHT       100
#define MEDIA_TIMEOUT   (10 * 1000)  // 10 seconds, represented in milliseconds

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------
// DirectShow Graph, Filter & Pins used
IGraphBuilder *g_pGraphBuilder  = NULL;
IMediaControl *g_pMediaControl  = NULL;
IMediaSeeking *g_pMediaSeeking  = NULL;
IBaseFilter   *g_pSourceCurrent = NULL;
IBaseFilter   *g_pSourceNext    = NULL;
TCHAR          g_szCurrentFile[128];
HWND           g_hwndApp=0;
HINSTANCE      g_hInstance=0;

// File names & variables to track current file
// These files are installed with the DirectX SDK into the samples media folder
LPCTSTR pstrFiles[] = 
{
    TEXT("track1.mp3\0"),
    TEXT("track2.mp3\0"),
    TEXT("track3.mp3\0"),
};

int g_iNumFiles = 3, g_iNextFile = 0;


//------------------------------------------------------------------------------
// Name: WinMain()
// Desc: Main Entry point for the app. Calls the Initialization routines and
//       then calls the main processing loop.
//------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // Initialize application window
    if(! AppInit(hInstance))
        return 0;

    // Initialize DirectShow components and build initial graph
    if(SUCCEEDED(GraphInit()))
    {
        // Main Message Loop
        AppMessageLoop();
    }

    // Clean up
    AppCleanUp();
    return 0;
}


//------------------------------------------------------------------------------
// Name: GraphInit()
// Desc: Initialization of DirectShow components and initial graph
//------------------------------------------------------------------------------
HRESULT GraphInit(void)
{
    HRESULT hr;
    // Initialize COM
    if(FAILED(hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        return hr;

    // Create DirectShow Graph
    if(FAILED(hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC, IID_IGraphBuilder,
        reinterpret_cast<void **>(&g_pGraphBuilder))))
        return hr;

    // Get the IMediaControl Interface
    if(FAILED(hr = g_pGraphBuilder->QueryInterface(IID_IMediaControl,
        reinterpret_cast<void **>(&g_pMediaControl))))
        return hr;

    // Get the IMediaControl Interface
    if(FAILED(hr = g_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
        reinterpret_cast<void **>(&g_pMediaSeeking))))
        return hr;

    // Create Source Filter for first file
    g_iNextFile = 0;

    // Create the intial graph
    if(FAILED(hr = SwapSourceFilter()))
        return hr;

    // Set a timer for switching the sources
    if(!SetTimer(g_hwndApp, 0, MEDIA_TIMEOUT, (TIMERPROC) MyTimerProc))
        return E_FAIL;

    return S_OK;
}


//------------------------------------------------------------------------------
// Name: AppInit()
// Desc: Initialization of application window
//------------------------------------------------------------------------------
HWND AppInit(HINSTANCE hInstance)
{
    WNDCLASS wc;

    // Register the window class
    ZeroMemory(&wc, sizeof wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASSNAME;
    wc.lpszMenuName  = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BGMUSIC));
    if(!RegisterClass(&wc))
        return 0;

    // Create the main window without support for resizing
    g_hwndApp = CreateWindow(CLASSNAME, APPNAME,
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                                           | WS_MINIMIZEBOX | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             APPWIDTH, APPHEIGHT,
                             0, 0, hInstance, 0);

    // Add a menu item to the app's system menu
    AddAboutMenuItem(g_hwndApp);
    g_hInstance = hInstance;

    return g_hwndApp;
}


//------------------------------------------------------------------------------
// Name: WndProcLoop()
// Desc: Main Message Processor for the Application
//------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_PAINT:
            ShowCurrentFile(hWnd);
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case IDM_ABOUTBOX:
                    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, 
                             (DLGPROC) AboutDlgProc);
            }
            break;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


//------------------------------------------------------------------------------
// Name: AppMessageLoop()
// Desc: Main Message Loop for the Application
//------------------------------------------------------------------------------
void AppMessageLoop(void)
{
    MSG msg;
    BOOL bRet = 0;

    // Main message loop:
    while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if(bRet == -1)     // Error in GetMessage
            break;

        TranslateMessage(&msg) ;
        DispatchMessage(&msg) ;
    }

    return;
}


//------------------------------------------------------------------------------
// Name: AppCleanUp)
// Desc: Clean up the application
//------------------------------------------------------------------------------
void AppCleanUp(void)
{
    // Stop playback
    if(g_pMediaControl)
        g_pMediaControl->Stop();

    // Release all remaining pointers
    SAFE_RELEASE(g_pSourceNext);
    SAFE_RELEASE(g_pSourceCurrent);
    SAFE_RELEASE(g_pMediaSeeking);
    SAFE_RELEASE(g_pMediaControl);
    SAFE_RELEASE(g_pGraphBuilder);

    // Clean up COM
    CoUninitialize();
    return;
}


//------------------------------------------------------------------------------
// MyTimerProc - Callback when the timer goes off
//------------------------------------------------------------------------------
VOID CALLBACK MyTimerProc(
                          HWND hwnd,     // handle to window
                          UINT uMsg,     // WM_TIMER message
                          UINT idEvent,  // timer identifier
                          DWORD dwTime   // current system time
)
{
    SwapSourceFilter();

    // Update the "current file" text message
    RECT rc;
    GetWindowRect(hwnd, &rc);
    InvalidateRect(hwnd, &rc, TRUE);
}


//------------------------------------------------------------------------------
// Name: ShowCurrentFile()
// Desc: Display the currently playing media file in the main window
//------------------------------------------------------------------------------
void ShowCurrentFile(HWND hWnd)
{
    PAINTSTRUCT ps;
    RECT rc;
    TCHAR szMsg[128];

    BeginPaint(hWnd, &ps);
    HDC hdc = GetDC(hWnd);
    GetWindowRect(hWnd, &rc);

    // Set the text color to bright green against black background
    SetTextColor(hdc, RGB(80, 255, 80));
    SetBkColor(hdc, RGB(0,0,0));

    // Decide where to place the text (centered in window)
    int X = (rc.right - rc.left) / 2;
    int Y = (rc.bottom - rc.top) / 3;
    SetTextAlign(hdc, TA_CENTER | VTA_CENTER);

    // Update the text string
    wsprintf(szMsg, _T("Playing %s\0"), g_szCurrentFile);
    ExtTextOut(hdc, X, Y, (UINT) ETO_OPAQUE, NULL, 
               szMsg, (UINT) _tcslen(szMsg), 0);

    EndPaint(hWnd, &ps);
}


//-----------------------------------------------------------------------------
// Name: AboutDlgProc()
// Desc: Message handler for About box
//-----------------------------------------------------------------------------
LRESULT CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (wParam == IDOK)
            {
                EndDialog(hWnd, TRUE);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Name: AddAboutMenuItem()
// Desc: Adds a menu item to the end of the app's system menu
//-----------------------------------------------------------------------------
void AddAboutMenuItem(HWND hWnd)
{
    // Add About box menu item
    HMENU hwndMain = GetSystemMenu(hWnd, FALSE);

    // Add separator
    BOOL rc = AppendMenu(hwndMain, MF_SEPARATOR, 0, NULL);

    // Add menu item
    rc = AppendMenu(hwndMain, MF_STRING | MF_ENABLED, 
                    IDM_ABOUTBOX,
                    TEXT("About BGMusic...\0"));
}


//------------------------------------------------------------------------------
// Name: SwapSourceFilter()
// Desc: This routine is used to change the source file in the current graph.
//       First the graph is stopped, then the current source filter is removed.
//       The new source filter is added, the output pin on this filter is
//       rendered, and playback is restarted.
//
//       When this routine is called during initialization, there is no
//       currently running graph. In that case, Stop becomes a no-op. The source
//       filter is added to an empty graph. Then during the render call, all
//       necessary filters required to play this source are added to the graph.
//
//       On subsequent calls, Stopping the graph allows filters to be removed.
//       When the old source filter is removed, all other filters are still
//       left in the graph. The new source filter is added, and then the render
//       operation reconnects the graph. Since all of the necessary filters for
//       playback are already in the graph (if the two files have the same file
//       type), these filters are reused. Existing filters in the graph are
//       always used first, if possible, during a Render operation. This avoids
//       having to create new filter instances with each change.
//------------------------------------------------------------------------------
HRESULT SwapSourceFilter(void)
{
    HRESULT hr = S_OK;
    IPin *pPin = NULL;
    int nNextFileIndex=0;
    TCHAR szFilename[MAX_PATH];
    WCHAR wFileName[MAX_PATH];

    // Determine the file to load based on DirectX Media path (from SDK)
    nNextFileIndex = g_iNextFile % g_iNumFiles;
    DXUtil_FindMediaFileCch( szFilename, MAX_PATH, pstrFiles[nNextFileIndex] );

    // Make sure that this file exists
    DWORD dwAttr = GetFileAttributes(szFilename);
    if(dwAttr == (DWORD) -1)
    {
        TCHAR sz[MAX_PATH];
        wsprintf( sz, TEXT("Players\\BGMusic\\%s"), pstrFiles[nNextFileIndex] );
        DXUtil_FindMediaFileCch( szFilename, MAX_PATH, sz );

        dwAttr = GetFileAttributes(szFilename);
        if(dwAttr == (DWORD) -1)
        {
            TCHAR szMsg[MAX_PATH + 64];
            wsprintf(szMsg, TEXT("Can't find the media file [%s].\0"), szFilename);
            MessageBox(NULL, szMsg, TEXT("BGMusic Sample Error"), MB_OK | MB_ICONEXCLAMATION);
            return E_FAIL;
        }
    }

    _tcsncpy(g_szCurrentFile, pstrFiles[nNextFileIndex], NUMELMS(g_szCurrentFile));
    g_iNextFile++;

    USES_CONVERSION;
    wcsncpy(wFileName, T2W(szFilename), MAX_PATH);

    // OPTIMIZATION OPPORTUNITY
    // This will open the file, which is expensive. To optimize, this
    // should be done earlier, ideally as soon as we knew this was the
    // next file to ensure that the file load doesn't add to the
    // filter swapping time & cause a hiccup.
    //
    // Add the new source filter to the graph. (Graph can still be running)
    hr = g_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &g_pSourceNext);

    // Get the first output pin of the new source filter. Audio sources 
    // typically have only one output pin, so for most audio cases finding 
    // any output pin is sufficient.
    if(SUCCEEDED(hr))
    {
        hr = g_pSourceNext->FindPin(L"Output", &pPin);  
    }

    // Stop the graph
    if(SUCCEEDED(hr))
    {
        hr = g_pMediaControl->Stop();
    }

    // Break all connections on the filters. You can do this by adding 
    // and removing each filter in the graph
    if(SUCCEEDED(hr))
    {
        IEnumFilters *pFilterEnum = NULL;

        if(SUCCEEDED(hr = g_pGraphBuilder->EnumFilters(&pFilterEnum)))
        {
            int iFiltCount = 0;
            int iPos = 0;

            // Need to know how many filters. If we add/remove filters during the
            // enumeration we'll invalidate the enumerator
            while(S_OK == pFilterEnum->Skip(1))
            {
                iFiltCount++;
            }

            // Allocate space, then pull out all of the 
            IBaseFilter **ppFilters = reinterpret_cast<IBaseFilter **>
                                      (_alloca(sizeof(IBaseFilter *) * iFiltCount));
            pFilterEnum->Reset();

            while(S_OK == pFilterEnum->Next(1, &(ppFilters[iPos++]), NULL));

            SAFE_RELEASE(pFilterEnum);

            for(iPos = 0; iPos < iFiltCount; iPos++)
            {
                g_pGraphBuilder->RemoveFilter(ppFilters[iPos]);

                // Put the filter back, unless it is the old source
                if(ppFilters[iPos] != g_pSourceCurrent)
                {
                    g_pGraphBuilder->AddFilter(ppFilters[iPos], NULL);
                }
                SAFE_RELEASE(ppFilters[iPos]);
            }
        }
    }

    // We have the new output pin. Render it
    if(SUCCEEDED(hr))
    {
        // Release the old source filter, if it exists
        SAFE_RELEASE(g_pSourceCurrent)

        hr = g_pGraphBuilder->Render(pPin);
        g_pSourceCurrent = g_pSourceNext;
        g_pSourceNext = NULL;
    }

    SAFE_RELEASE(pPin);
    SAFE_RELEASE(g_pSourceNext); // In case of errors

    // Re-seek the graph to the beginning
    if(SUCCEEDED(hr))
    {
        LONGLONG llPos = 0;
        hr = g_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                           &llPos, AM_SEEKING_NoPositioning);
    } 

    // Start the graph
    if(SUCCEEDED(hr))
    {
        hr = g_pMediaControl->Run();
    }

    return S_OK;   
}


//-----------------------------------------------------------------------------
// Name: DXUtil_GetDXSDKMediaPath()
// Desc: Returns the DirectX SDK media path
//-----------------------------------------------------------------------------
HRESULT DXUtil_GetDXSDKMediaPathCch( TCHAR* strDest, int cchDest )
{
    if( strDest == NULL || cchDest < 1 )
        return E_INVALIDARG;

    lstrcpy( strDest, TEXT("") );

    // Open the appropriate registry key
    HKEY  hKey;
    LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Microsoft\\DirectX SDK"),
                                0, KEY_READ, &hKey );
    if( ERROR_SUCCESS != lResult )
        return E_FAIL;

    DWORD dwType;
    DWORD dwSize = cchDest * sizeof(TCHAR);
    lResult = RegQueryValueEx( hKey, _T("DX9S4SDK Samples Path"), NULL,
                              &dwType, (BYTE*)strDest, &dwSize );
    strDest[cchDest-1] = 0; // RegQueryValueEx doesn't NULL term if buffer too small
    RegCloseKey( hKey );

    if( ERROR_SUCCESS != lResult )
        return E_FAIL;

    const TCHAR* strMedia = _T("\\Media\\");
    if( lstrlen(strDest) + lstrlen(strMedia) < cchDest )
        _tcscat( strDest, strMedia );
    else
        return E_INVALIDARG;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DXUtil_FindMediaFileCch()
// Desc: Trys to find the location of a SDK media file
//       cchDest is the size in TCHARs of strDestPath.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds.
//-----------------------------------------------------------------------------
HRESULT DXUtil_FindMediaFileCch( TCHAR* strDestPath, int cchDest, LPCTSTR strFilename )
{
    bool bFound;
    TCHAR strSearchFor[MAX_PATH];
    
    if( NULL==strFilename || strFilename[0] == 0 || NULL==strDestPath || cchDest < 10 )
        return E_INVALIDARG;

    // Get the DirectX SDK's media dir if the SDK is installed
    TCHAR strMediaDir[MAX_PATH] = {0};
    DXUtil_GetDXSDKMediaPathCch( strMediaDir, MAX_PATH );

    // Get the exe name, and exe path
    TCHAR strExePath[MAX_PATH] = {0};
    TCHAR strExeName[MAX_PATH] = {0};
    TCHAR* strLastSlash = NULL;
    GetModuleFileName( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH-1]=0;
    strLastSlash = _tcsrchr( strExePath, TEXT('\\') );
    if( strLastSlash )
    {
        lstrcpyn( strExeName, &strLastSlash[1], MAX_PATH );

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = _tcsrchr( strExeName, TEXT('.') );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    // Typical directories:
    //      .\
    //      ..\
    //      ..\..\
    //      %EXE_DIR%\
    //      %EXE_DIR%\..\
    //      %EXE_DIR%\..\..\
    //      %EXE_DIR%\..\%EXE_NAME%
    //      %EXE_DIR%\..\..\%EXE_NAME%
    //      DXSDK media path

    // Typical directory search
    bFound = DXUtil_FindMediaSearchTypicalDirs( strDestPath, cchDest, strFilename, strExePath, strExeName, strMediaDir );
    if( bFound )
        return S_OK;

    // Typical directory search again, but also look in a subdir called "\media\" 
    _sntprintf( strSearchFor, MAX_PATH, TEXT("media\\%s"), strFilename ); strSearchFor[MAX_PATH-1] = 0;
    bFound = DXUtil_FindMediaSearchTypicalDirs( strDestPath, cchDest, strSearchFor, strExePath, strExeName, strMediaDir );
    if( bFound )
        return S_OK;

    TCHAR strLeafName[MAX_PATH] = {0};

    // Search all parent directories starting at .\ and using strFilename as the leaf name
    _tcsncpy( strLeafName, strFilename, MAX_PATH ); strLeafName[MAX_PATH-1] = 0;
    bFound = DXUtil_FindMediaSearchParentDirs( strDestPath, cchDest, TEXT("."), strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at the exe's dir and using strFilename as the leaf name
    bFound = DXUtil_FindMediaSearchParentDirs( strDestPath, cchDest, strExePath, strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at .\ and using "media\strFilename" as the leaf name
    _sntprintf( strLeafName, MAX_PATH, TEXT("media\\%s"), strFilename ); strLeafName[MAX_PATH-1] = 0;
    bFound = DXUtil_FindMediaSearchParentDirs( strDestPath, cchDest, TEXT("."), strLeafName );
    if( bFound )
        return S_OK;

    // Search all parent directories starting at the exe's dir and using "media\strFilename" as the leaf name
    bFound = DXUtil_FindMediaSearchParentDirs( strDestPath, cchDest, strExePath, strLeafName );
    if( bFound )
        return S_OK;

    // On failure, return the file as the path but also return an error code
    _tcsncpy( strDestPath, strFilename, cchDest );
    strDestPath[cchDest-1] = 0;

    return E_FAIL;
}



//--------------------------------------------------------------------------------------
// Search a set of typical directories
//--------------------------------------------------------------------------------------
bool DXUtil_FindMediaSearchTypicalDirs( TCHAR* strSearchPath, int cchSearch, LPCTSTR strLeaf, 
                                        TCHAR* strExePath, TCHAR* strExeName, TCHAR* strMediaDir )
{
    // Typical directories:
    //      .\
    //      ..\
    //      ..\..\
    //      %EXE_DIR%\
    //      %EXE_DIR%\..\
    //      %EXE_DIR%\..\..\
    //      %EXE_DIR%\..\%EXE_NAME%
    //      %EXE_DIR%\..\..\%EXE_NAME%
    //      DXSDK media path

    // Search in .\  
    _tcsncpy( strSearchPath, strLeaf, cchSearch ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\  
    _sntprintf( strSearchPath, cchSearch, TEXT("..\\%s"), strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\..\ 
    _sntprintf( strSearchPath, cchSearch, TEXT("..\\..\\%s"), strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in ..\..\ 
    _sntprintf( strSearchPath, cchSearch, TEXT("..\\..\\%s"), strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\ 
    _sntprintf( strSearchPath, cchSearch, TEXT("%s\\%s"), strExePath, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\..\ 
    _sntprintf( strSearchPath, cchSearch, TEXT("%s\\..\\%s"), strExePath, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in the %EXE_DIR%\..\..\ 
    _sntprintf( strSearchPath, cchSearch, TEXT("%s\\..\\..\\%s"), strExePath, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in "%EXE_DIR%\..\%EXE_NAME%\".  This matches the DirectX SDK layout
    _sntprintf( strSearchPath, cchSearch, TEXT("%s\\..\\%s\\%s"), strExePath, strExeName, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in "%EXE_DIR%\..\..\%EXE_NAME%\".  This matches the DirectX SDK layout
    _sntprintf( strSearchPath, cchSearch, TEXT("%s\\..\\..\\%s\\%s"), strExePath, strExeName, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    // Search in DirectX SDK's media dir 
    _sntprintf( strSearchPath, cchSearch, TEXT("%s%s"), strMediaDir, strLeaf ); strSearchPath[cchSearch-1] = 0;
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        return true;

    return false;
}



//--------------------------------------------------------------------------------------
// Search parent directories starting at strStartAt, and appending strLeafName
// at each parent directory.  It stops at the root directory.
//--------------------------------------------------------------------------------------
bool DXUtil_FindMediaSearchParentDirs( TCHAR* strSearchPath, int cchSearch, TCHAR* strStartAt, TCHAR* strLeafName )
{
    TCHAR strFullPath[MAX_PATH] = {0};
    TCHAR strFullFileName[MAX_PATH] = {0};
    TCHAR strSearch[MAX_PATH] = {0};
    TCHAR* strFilePart = NULL;

    GetFullPathName( strStartAt, MAX_PATH, strFullPath, &strFilePart );
    if( strFilePart == NULL )
        return false;
   
    while( strFilePart != NULL )
    {
        _sntprintf( strFullFileName, MAX_PATH, TEXT("%s\\%s"), strFullPath, strLeafName ); strFullFileName[MAX_PATH-1] = 0;       
        if( GetFileAttributes( strFullFileName ) != 0xFFFFFFFF )
        {
            _tcsncpy( strSearchPath, strFullFileName, cchSearch ); strSearchPath[cchSearch-1] = 0; 
            return true;
        }

        _sntprintf( strSearch, MAX_PATH, TEXT("%s\\.."), strFullPath ); strSearch[MAX_PATH-1] = 0;
        GetFullPathName( strSearch, MAX_PATH, strFullPath, &strFilePart );
    }

    return false;
}
