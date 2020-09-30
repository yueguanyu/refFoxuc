//
// D3DX Application Framework
// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//
#include "mviewpch.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>
#include <mmsystem.h>
#include <multimon.h>
#include "d3dxapp.h"
#include "dxerr8.h"

#define RELEASE(x) if(x) { x->Release(); x = NULL; } else 0
typedef BOOL (WINAPI* LPGETMONITORINFO)(HMONITOR, LPMONITORINFO);   


// ---------------------------------------------------------------------------
//
// Structures
//
// ---------------------------------------------------------------------------

struct D3DX_DEVICE
{
    UINT       m_uAdapter;
    D3DDEVTYPE m_DevType;
    DWORD      m_dwBehavior;
};


struct D3DX_TIMER
{
    float m_fTicksPerSec;
    float m_fFramesPerSec;
    float m_fSecsPerFrame;

    UINT64 m_qwTicks;
    UINT64 m_qwTicksPerSec;
    UINT64 m_qwTicksPerFrame;
};


struct D3DX_APPLICATION_DATA
{
    BOOL m_bActive        ;
    BOOL m_bRunning       ;
    BOOL m_bStopping      ;
    BOOL m_bDrawing       ;
    BOOL m_bFullscreen    ;
    BOOL m_bInSizeMove    ;
    BOOL m_bInReset       ;
    BOOL m_bCoverWindow   ;
    BOOL m_bNeedReset     ;
    BOOL m_bNeedDraw      ;
    BOOL m_bDeviceCreated ;
    BOOL m_bDeviceReset   ;

    D3DX_TIMER            m_Timer;
    D3DX_DEVICE           m_Device;
    D3DPRESENT_PARAMETERS m_Params;
    D3DPRESENT_PARAMETERS m_UserParams;

    float m_fWindowAspect;
    float m_fScreenAspect;

    HACCEL           m_hAccel;
    RECT             m_WindowRect;
    WINDOWPLACEMENT  m_WindowPlacement;
    LPGETMONITORINFO m_pfnGetMonitorInfo;

    UINT  m_cAdapter;
    UINT* m_pAdapter;
};



// ---------------------------------------------------------------------------
//
// Debug printf functions
//
// ---------------------------------------------------------------------------

#if DBG
/*
static void
DPF(const char *szFmt, ...)
{
    char sz[256], szMsg[256];

    va_list va;
    va_start(va, szFmt);
    _vsnprintf(szMsg, sizeof(szMsg), szFmt, va);
    va_end(va);

    _snprintf(sz, sizeof(sz), "d3dxapp: %s\r\n", szMsg);

    sz[sizeof(sz)-1] = '\0';
    OutputDebugString(sz);
}
*/


static void
DPF_HR(HRESULT hr, const char *szFmt, ...)
{
    char sz[256], szMsg[256];
    const char *szHr;


    va_list va;
    va_start(va, szFmt);
    _vsnprintf(szMsg, sizeof(szMsg), szFmt, va);
    va_end(va);

    szHr = DXGetErrorString8(hr);

    _snprintf(sz, sizeof(sz), "d3dxapp: %s (%s)\r\n", szMsg, szHr);
    sz[sizeof(sz)-1] = '\0';
    OutputDebugString(sz);
}
#else

#pragma warning(disable:4002)
#define DPF_HR() 0

#endif

// ---------------------------------------------------------------------------
//
// Timer functions
//
// ---------------------------------------------------------------------------


static UINT64
D3DXTimer_GetTicks()
{
    UINT64 qw;

    do
    {
        // Use QueryPerformanceCounter to get high-res timer. Fall back on 
        // timeGetTime if QPC is not supported.

        if(!QueryPerformanceCounter((LARGE_INTEGER *) &qw))
            return (UINT64) timeGetTime();
    }
    while(!qw);

    return qw;
}


static void
D3DXTimer_Initialize(D3DX_TIMER *pTimer)
{
    // Use QueryPerformanceFrequency to get frequency of timer.  If QPF is not 
    // supported, use 1000, since timeGetTime returns milliseconds.

    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &pTimer->m_qwTicksPerSec))
        pTimer->m_qwTicksPerSec = 1000;

    pTimer->m_fTicksPerSec  = (float) (__int64) pTimer->m_qwTicksPerSec;
}


static void 
D3DXTimer_Start(D3DX_TIMER *pTimer, float fFramesPerSec)
{
    if(fFramesPerSec < 1.0f)
        fFramesPerSec = 1.0f;
    
    pTimer->m_fFramesPerSec = fFramesPerSec;
    pTimer->m_fSecsPerFrame = 1.0f / fFramesPerSec;
    pTimer->m_qwTicksPerFrame = pTimer->m_qwTicksPerSec / (__int64) fFramesPerSec;
    pTimer->m_qwTicks = D3DXTimer_GetTicks();
}


static void 
D3DXTimer_OnFrame(D3DX_TIMER *pTimer)
{
    UINT64 qw;
    qw = D3DXTimer_GetTicks();


    if(qw != pTimer->m_qwTicks)
    {
        pTimer->m_qwTicksPerFrame = qw - pTimer->m_qwTicks;
        pTimer->m_qwTicks = qw;

        pTimer->m_fFramesPerSec = 
            pTimer->m_fTicksPerSec / (float) (__int64) pTimer->m_qwTicksPerFrame;

        if(pTimer->m_fFramesPerSec < 1.0f)
            pTimer->m_fFramesPerSec = 1.0f;

        pTimer->m_fSecsPerFrame = 1.0f / pTimer->m_fFramesPerSec;
    }
}




// ---------------------------------------------------------------------------
//
// EnumFirstResource
//
// ---------------------------------------------------------------------------


static BOOL CALLBACK 
EnumFirstResource(HMODULE hInstance, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    *((LPSTR *) lParam) = lpszName;
    return FALSE;
}


// ---------------------------------------------------------------------------
//
// GetClientScreenRect - Get non-minimized client rect on screen
//
// ---------------------------------------------------------------------------

static BOOL
GetClientScreenRect(HWND hwnd, LPRECT pRect)
{
    if(!pRect)
        return FALSE;

    WINDOWPLACEMENT wp;
    GetWindowPlacement(hwnd, &wp);

    if(SW_SHOWMINIMIZED == wp.showCmd)
    {
        RECT rectBorder;
        memset(&rectBorder, 0x00, sizeof(RECT));

        AdjustWindowRectEx(
            &rectBorder, 
            GetWindowLong(hwnd, GWL_STYLE),
            NULL != GetMenu(hwnd),
            GetWindowLong(hwnd, GWL_EXSTYLE));

        pRect->left   = wp.rcNormalPosition.left   - rectBorder.left;
        pRect->top    = wp.rcNormalPosition.top    - rectBorder.top;
        pRect->right  = wp.rcNormalPosition.right  - rectBorder.right;
        pRect->bottom = wp.rcNormalPosition.bottom - rectBorder.bottom;
    }
    else
    {
        GetClientRect(hwnd, pRect);
        ClientToScreen(hwnd, (LPPOINT) pRect);

        pRect->right += pRect->left;
        pRect->bottom += pRect->top;

        pRect->top += 28;
        pRect->bottom -= 20;
    }

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// CD3DXApplication
//
// ---------------------------------------------------------------------------

CD3DXApplication::CD3DXApplication()
{
    m_hInstance  = NULL;
    m_hwnd       = NULL;
    m_pD3D       = NULL;
    m_pDevice = NULL;
    m_pData      = NULL;
}


CD3DXApplication::~CD3DXApplication()
{
    RELEASE(m_pDevice);
    RELEASE(m_pD3D);

    if(m_hwnd)
        DestroyWindow(m_hwnd);

    if(m_pData)
    {
        if(m_pData->m_pAdapter)
            delete [] m_pData->m_pAdapter;

        delete m_pData;
    }
}


HRESULT 
CD3DXApplication::Initialize(HINSTANCE hInstance, LPCSTR szWindowName, 
     LPCSTR szClassName, UINT uWidth, UINT uHeight)
{
    HRESULT hr;
    RECT rect;

    if(m_pData)
    {
        DPF_HR(0, "Application already initialized");
        return S_OK;
    }

    if (!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION))
        return E_FAIL;

    // Create D3D
    if(!(m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        DPF_HR(0, "Direct3DCreate9 failed");
        goto LFail;
    }


    // Initialize private data
    if(!(m_pData = new D3DX_APPLICATION_DATA))
    {
        DPF_HR(0, "Could not allocate private data");
        hr = E_OUTOFMEMORY;
        goto LError;
    }

    memset(m_pData, 0x00, sizeof(D3DX_APPLICATION_DATA));

    D3DXTimer_Initialize(&m_pData->m_Timer);
    D3DXTimer_Start(&m_pData->m_Timer, 30.0f);

    m_hInstance = hInstance;



    // Create window class
    if(!szWindowName)
        szWindowName = "D3DX Application";

    if(!szClassName)
    {
        WNDCLASS wc;

        // Use first icon resource as window icon.
        LPSTR szIcon;
        szIcon = NULL;
        EnumResourceNames(m_hInstance, RT_GROUP_ICON, EnumFirstResource, (LONG_PTR)&szIcon);
 
        wc.style         = 0;
        wc.lpfnWndProc   = (WNDPROC) WndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = m_hInstance;
        wc.hIcon         = szIcon ? LoadIcon(m_hInstance, szIcon) : NULL;
        wc.hCursor       = CopyCursor(LoadCursor(NULL, IDC_ARROW));
        wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = szClassName = szWindowName;

        RegisterClass(&wc);
    }



    // Create window (exact size and style will be overridden later)
    if(0 == uWidth || 0 == uHeight)
    {
        uWidth  = 640;
        uHeight = 480;
    }

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = uWidth;
    rect.bottom = uHeight;

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(szClassName, szWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
        CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, (HWND) NULL, 
        (HMENU) NULL, m_hInstance, (LPVOID) NULL);

    if(!m_hwnd)
    {
        DPF_HR(0, "Failed to create window");
        goto LFail;
    }

    DragAcceptFiles( m_hwnd, TRUE );

    // Save 'this' in the window's USERDATA for WndProc.
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);


    // Find first accelerator resource
    LPSTR szAccel;
    szAccel = NULL;
    EnumResourceNames(m_hInstance, RT_ACCELERATOR, EnumFirstResource, (LONG_PTR) &szAccel);

    if(szAccel)
        m_pData->m_hAccel = LoadAccelerators(m_hInstance, szAccel);


    // Default presentation parameters
    m_pData->m_Params.BackBufferWidth        = 0;
    m_pData->m_Params.BackBufferHeight       = 0;
    m_pData->m_Params.BackBufferFormat       = D3DFMT_UNKNOWN;
    m_pData->m_Params.BackBufferCount        = 0;
    m_pData->m_Params.MultiSampleType        = D3DMULTISAMPLE_NONE;
    m_pData->m_Params.SwapEffect             = D3DSWAPEFFECT_COPY;
    m_pData->m_Params.hDeviceWindow          = m_hwnd;
    m_pData->m_Params.Windowed               = TRUE;
    m_pData->m_Params.EnableAutoDepthStencil = TRUE;
    m_pData->m_Params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    m_pData->m_Params.Flags                  = 0;

    m_pData->m_UserParams = m_pData->m_Params;
    m_PresentParameters   = m_pData->m_Params;


    // Get GetMonitorInfo entrypoint
    HINSTANCE hUser32;

    if((hUser32 = (HINSTANCE) GetModuleHandle("user32.dll")) ||
       (hUser32 = (HINSTANCE) LoadLibrary("user32.dll")))
    {
        m_pData->m_pfnGetMonitorInfo = 
           (LPGETMONITORINFO) GetProcAddress(hUser32, "GetMonitorInfoA");
    }


    // Default adapter ordering
    UINT uAdapter;

    m_pData->m_cAdapter = m_pD3D->GetAdapterCount();
    
    if(!(m_pData->m_pAdapter = new UINT[m_pData->m_cAdapter]))
    {
        hr = E_OUTOFMEMORY;
        goto LError;
    }

    for(uAdapter = 0; uAdapter < m_pData->m_cAdapter; uAdapter++)
    {
        m_pData->m_pAdapter[uAdapter] = uAdapter;
    }



    return S_OK;


LFail:
    hr = E_FAIL;
    goto LError;

LError:
    RELEASE(m_pD3D);

    if(m_hwnd)
        DestroyWindow(m_hwnd);

    if(m_pData)
    {
        if(m_pData->m_pAdapter)
            delete m_pData->m_pAdapter;

        delete m_pData;
    }

    m_hwnd = NULL;
    m_pData = NULL;

    return hr;
}



HRESULT 
CD3DXApplication::Reset(D3DPRESENT_PARAMETERS *pPresentParameters)
{
    HRESULT hr;

    if(!m_pData)
    {
        DPF_HR(0, "Application not initialized");
        return E_FAIL;
    }

    if(m_pData->m_bDrawing)
    {
        DPF_HR(0, "Reset cannot be called from within OnDraw");
        return E_FAIL;
    }

    if(m_pData->m_bInReset)
    {
        // Reset called from within reset
        return S_OK;
    }



    // If we are not running, we just save off the parameters and return.
    // (The device will be created when we begin running.)
    if(pPresentParameters)
        m_pData->m_UserParams = *pPresentParameters;

    if(!m_pData->m_bRunning)
        return S_OK;




    // Ok.. so we are actually going to actually do work..
    m_pData->m_bInReset = TRUE;

    D3DX_DEVICE Device = m_pData->m_Device;
    D3DPRESENT_PARAMETERS Params = m_pData->m_UserParams;


    // If we are switching from fullscreen to windowed, we need to destroy
    // the device, so that video mode gets restored before we try to figure
    // out which monitor the window is primarily on.
    if(Params.Windowed && m_pData->m_bFullscreen)
    {
        if(m_pData->m_bDeviceReset)
        {
            m_pData->m_bDeviceReset = FALSE;

            if(FAILED(hr = OnLostDevice()))
                DPF_HR(hr, "OnLostDevice failed");
        }

        if(m_pData->m_bDeviceCreated)
        {
            m_pData->m_bDeviceCreated = FALSE;

            if(FAILED(hr = OnDestroyDevice()))
                DPF_HR(hr, "OnDestroyDevice failed");
        }

        RELEASE(m_pDevice);
    }


    // Compute window client rect (in screen coordinates)
    if(!m_pData->m_bCoverWindow)
    {
        m_pData->m_WindowPlacement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(m_hwnd, &m_pData->m_WindowPlacement);
        GetClientScreenRect(m_hwnd, &m_pData->m_WindowRect);
    }

    RECT WindowRect = m_pData->m_WindowRect;


    // Default values for width and height
    if(D3DX_DEFAULT == Params.BackBufferWidth)
        Params.BackBufferWidth = 0;

    if(D3DX_DEFAULT == Params.BackBufferHeight)
        Params.BackBufferHeight = 0;

    if(!Params.BackBufferWidth || !Params.BackBufferHeight)
    {
        if(Params.Windowed)
        {
            Params.BackBufferWidth  = (WindowRect.right > WindowRect.left) ? (WindowRect.right - WindowRect.left) : 1;
            Params.BackBufferHeight = (WindowRect.bottom > WindowRect.top) ? (WindowRect.bottom - WindowRect.top) : 1;
        }
        else
        {
            Params.BackBufferWidth  = 640;
            Params.BackBufferHeight = 480;
        }
    }

    if(Params.Windowed)
    {
        Params.BackBufferCount = 1;
        Params.FullScreen_RefreshRateInHz      = 0;
        Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    Params.hDeviceWindow = m_hwnd;


    // Default values for backbuffer count
    UINT uBBCMax;
    UINT uBBCMin;

    if(0 != Params.BackBufferCount && D3DX_DEFAULT != Params.BackBufferCount)
    {
        uBBCMax = Params.BackBufferCount;
        uBBCMin = Params.BackBufferCount;
    }
    else if(!Params.Windowed && D3DSWAPEFFECT_DISCARD == Params.SwapEffect)
    {
        uBBCMax = 2;
        uBBCMin = 1;
    }
    else
    {
        uBBCMax = 1;
        uBBCMin = 1;
    }




    // Device type list
    static D3DDEVTYPE DevTypeList[] = 
    {
        D3DDEVTYPE_HAL,
        D3DDEVTYPE_REF,
    };

    D3DDEVTYPE *pDevType;
    D3DDEVTYPE *pDevTypeMin = DevTypeList;
    D3DDEVTYPE *pDevTypeLim = DevTypeList + (sizeof(DevTypeList) / sizeof(DevTypeList[0]));




    // Adapter list. This list is sorted by area covered by window.  This 
    // is done so that as the window gets dragged from monitor to monitor, 
    // the adapter which the window is mostly on will be considered first.

    UINT  cAdapter    = m_pData->m_cAdapter;
    UINT *pAdapter;
    UINT *pAdapterMin = m_pData->m_pAdapter;
    UINT *pAdapterLim = pAdapterMin + cAdapter;

    if(!m_pData->m_bFullscreen && m_pData->m_pfnGetMonitorInfo && cAdapter > 1)
    {
        HMONITOR    hMonitor;
        MONITORINFO minfo;

        UINT *pAreaMin = (UINT *) _alloca(cAdapter * sizeof(UINT));
        UINT iAdapter;

        // Clip client area to adapter rect
        for(iAdapter = 0; iAdapter < cAdapter; iAdapter++)
        {
            minfo.cbSize = sizeof(MONITORINFO);

            hMonitor = m_pD3D->GetAdapterMonitor(pAdapterMin[iAdapter]);

            m_pData->m_pfnGetMonitorInfo(hMonitor, &minfo);


            if(minfo.rcWork.left < WindowRect.left)
                minfo.rcWork.left = WindowRect.left;

            if(minfo.rcWork.right > WindowRect.right)
                minfo.rcWork.right = WindowRect.right;

            if(minfo.rcWork.right < minfo.rcWork.left)
                minfo.rcWork.right = minfo.rcWork.left;


            if(minfo.rcWork.top < WindowRect.top)
                minfo.rcWork.top = WindowRect.top;

            if(minfo.rcWork.bottom > WindowRect.bottom)
                minfo.rcWork.bottom = WindowRect.bottom;

            if(minfo.rcWork.bottom < minfo.rcWork.top)
                minfo.rcWork.bottom = minfo.rcWork.top;


            pAreaMin[iAdapter] = (UINT) ((minfo.rcWork.right - minfo.rcWork.left) * (minfo.rcWork.bottom - minfo.rcWork.top));
        }


        // Sort adapters by area
        for(;;)
        {
            for(iAdapter = 1; iAdapter < cAdapter; iAdapter++)
            {
                if(pAreaMin[iAdapter] > pAreaMin[iAdapter - 1])
                {
                    UINT u;

                    u = pAreaMin[iAdapter]; 
                    pAreaMin[iAdapter] = pAreaMin[iAdapter - 1]; 
                    pAreaMin[iAdapter - 1] = u;

                    u = pAdapterMin[iAdapter]; 
                    pAdapterMin[iAdapter] = pAdapterMin[iAdapter - 1]; 
                    pAdapterMin[iAdapter - 1] = u;

                    break;
                }
            }

            if(iAdapter == cAdapter)
                break;
        }
    }




    // Search for suitable device
    enum { NONE, EXISTING, RESET, CREATE } Action = NONE;

    for(pDevType = pDevTypeMin; pDevType < pDevTypeLim; pDevType++)
    {
        Device.m_DevType = *pDevType;

        for(pAdapter = pAdapterMin; pAdapter < pAdapterLim; pAdapter++)
        {
            Device.m_uAdapter = *pAdapter;
            D3DFORMAT PrimaryFormat;


            // Resolve unknown backbuffer formats
            if(D3DFMT_UNKNOWN == m_pData->m_UserParams.BackBufferFormat)
            {
                if(Params.Windowed)
                {
                    // Use current display mode
                    D3DDISPLAYMODE mode;

                    if(FAILED(m_pD3D->GetAdapterDisplayMode(Device.m_uAdapter , &mode)))
                        continue;

                    PrimaryFormat = mode.Format;
                    Params.BackBufferFormat = mode.Format;

                    if(FAILED(m_pD3D->CheckDeviceType(Device.m_uAdapter, Device.m_DevType, 
                        PrimaryFormat, Params.BackBufferFormat, FALSE)))
                    {
                        continue;
                    }

                }
                else
                {
                    // Fallbacks
                    for(UINT i = 0; i < 3; i++)
                    {
                        switch(i)
                        {
                        case 0: Params.BackBufferFormat = D3DFMT_R5G6B5;   break;
                        case 1: Params.BackBufferFormat = D3DFMT_X1R5G5B5; break;
                        case 2: Params.BackBufferFormat = D3DFMT_X8R8G8B8; break;
                        }

                        PrimaryFormat = Params.BackBufferFormat;
        
                        if(FAILED(m_pD3D->CheckDeviceType(Device.m_uAdapter, Device.m_DevType, 
                            PrimaryFormat, Params.BackBufferFormat, FALSE)))
                        {
                            continue;
                        }

                        break;
                    }

                    if(i == 3)
                        continue;
                }
            }
            else
            {
                // Try to pick primary format without alpha
                switch(Params.BackBufferFormat)
                {
                case D3DFMT_A8R8G8B8: PrimaryFormat = D3DFMT_X8R8G8B8;         break;
                case D3DFMT_A1R5G5B5: PrimaryFormat = D3DFMT_X1R5G5B5;         break;
                case D3DFMT_A4R4G4B4: PrimaryFormat = D3DFMT_X4R4G4B4;         break;  
                default:              PrimaryFormat = Params.BackBufferFormat; break;
                }

                if(FAILED(m_pD3D->CheckDeviceType(Device.m_uAdapter, Device.m_DevType, 
                    PrimaryFormat, Params.BackBufferFormat, FALSE)))
                {
                    continue;
                }
            }


            // Resolve unknown depth/stencil formats
            if(D3DFMT_UNKNOWN == m_pData->m_UserParams.AutoDepthStencilFormat)
            {
                static D3DFORMAT s_ZS16[] = 
                { 
                    D3DFMT_D16_LOCKABLE,
                    D3DFMT_D16,
                    D3DFMT_D15S1,
                    D3DFMT_D32,
                    D3DFMT_D24S8,
                    D3DFMT_D24X4S4,
                    D3DFMT_D24X8
                };

                static D3DFORMAT s_ZS32[] = 
                { 
                    D3DFMT_D32,
                    D3DFMT_D24S8,
                    D3DFMT_D24X4S4,
                    D3DFMT_D24X8,
                    D3DFMT_D16_LOCKABLE,
                    D3DFMT_D16,
                    D3DFMT_D15S1
                };

                D3DFORMAT *pZS;

                switch(Params.BackBufferFormat)
                {
                case D3DFMT_R5G6B5:
                case D3DFMT_X1R5G5B5:
                case D3DFMT_A1R5G5B5:
                case D3DFMT_A4R4G4B4:
                case D3DFMT_A8R3G3B2:
                case D3DFMT_X4R4G4B4:
                    pZS = s_ZS16;
                    break;

                case D3DFMT_R8G8B8:
                case D3DFMT_A8R8G8B8:
                case D3DFMT_X8R8G8B8:
                default:
                    pZS = s_ZS32;
                    break;
                }

                for(UINT i = 0; i < 7; i++)
                {
                    Params.AutoDepthStencilFormat = pZS[i];

                    if(FAILED(m_pD3D->CheckDeviceFormat(Device.m_uAdapter, Device.m_DevType, 
                        PrimaryFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, Params.AutoDepthStencilFormat)))
                    {
                        continue;
                    }

                    if(FAILED(m_pD3D->CheckDepthStencilMatch(Device.m_uAdapter, Device.m_DevType, 
                        PrimaryFormat, Params.BackBufferFormat, Params.AutoDepthStencilFormat)))
                    {
                        continue;
                    }

                    break;
                }

                if(i == 7)
                    continue;
            }



            // Check device caps
            BOOL bCapsSufficient;

            bCapsSufficient = FALSE;

            if(FAILED(m_pD3D->GetDeviceCaps(Device.m_uAdapter, Device.m_DevType, &m_Caps)))
                continue;

            if(m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            {
                if(m_Caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
                {
                    // Try Pure HW
                    Device.m_dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
                    bCapsSufficient = AreCapsSufficient(&m_Caps, Device.m_dwBehavior);
                }

                if(!bCapsSufficient)
                {
                    // Try HW
                    Device.m_dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    bCapsSufficient = AreCapsSufficient(&m_Caps, Device.m_dwBehavior);
                }

                if(!bCapsSufficient)
                {
                    // Try Mixed
                    Device.m_dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
                    bCapsSufficient = AreCapsSufficient(&m_Caps, Device.m_dwBehavior);
                }
            }

            if(!bCapsSufficient)
            {
                // Try SW
                Device.m_dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

                m_Caps.VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 |
                    D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER;

                m_Caps.MaxActiveLights           = 0xffffffff;
                m_Caps.MaxUserClipPlanes         = 6;
                m_Caps.MaxVertexBlendMatrices    = 4;
                m_Caps.MaxVertexBlendMatrixIndex = 255;

                if(0.0f == m_Caps.MaxPointSize)
                    m_Caps.MaxPointSize = 64.0f;

                m_Caps.MaxPrimitiveCount         = 0xffffffff;
                m_Caps.MaxVertexIndex            = 0xffffffff;
                m_Caps.MaxStreams                = 16;
                m_Caps.MaxStreamStride           = 0xffffffff;
                m_Caps.VertexShaderVersion       = D3DVS_VERSION(1, 1);
                m_Caps.MaxVertexShaderConst      = 96;

                bCapsSufficient = AreCapsSufficient(&m_Caps, Device.m_dwBehavior);
            }

            if(!bCapsSufficient)
                continue;


            // Iterate thru backbuffer counts
            for(Params.BackBufferCount = uBBCMax; Params.BackBufferCount >= uBBCMin; Params.BackBufferCount--)
            {

                // Try to reuse current device
                if(m_pDevice)
                {
                    if(!memcmp(&m_pData->m_Device, &Device, sizeof(D3DX_DEVICE)) &&
                       !memcmp(&m_pData->m_Params, &Params, sizeof(D3DPRESENT_PARAMETERS)))
                    {
                        // Use existing device
                        Action = EXISTING;
                        break;
                    }

                    if(m_pData->m_bDeviceReset)
                    {
                        m_pData->m_bDeviceReset = FALSE;

                        if(FAILED(hr = OnLostDevice()))
                            DPF_HR(hr, "OnLostDevice failed");
                    }

                    if(!memcmp(&m_pData->m_Device, &Device, sizeof(D3DX_DEVICE)))
                    {
                        // Reset existing device
                        if(FAILED(hr = m_pDevice->Reset(&Params)))
                        {
                            DPF_HR(hr, "IDirect3DDevice9::Reset failed");
                            continue;
                        }

                        Action = RESET;
                        break;
                    }

                    if(m_pData->m_bDeviceCreated)
                    {
                        m_pData->m_bDeviceCreated = FALSE;

                        if(FAILED(hr = OnDestroyDevice()))
                            DPF_HR(hr, "OnDestroyDevice failed");
                    }

                    RELEASE(m_pDevice);
                }

                // Try to create device
                if(FAILED(m_pD3D->CreateDevice(Device.m_uAdapter, Device.m_DevType, m_hwnd, 
                    Device.m_dwBehavior, &Params, &m_pDevice)))
                {
                    continue;
                }

                Action = CREATE;
                break;
            }

            if(Params.BackBufferCount < uBBCMin)
                continue;

            break;
        }

        if(pAdapter == pAdapterLim)
            continue;

        break;
    }


    // No suitable device found
    if(NONE == Action)
    {
        if(m_pDevice)
        {
            if(m_pData->m_bDeviceReset)
            {
                m_pData->m_bDeviceReset = FALSE;

                if(FAILED(hr = OnLostDevice()))
                    DPF_HR(hr, "OnLostDevice failed");
            }

            if(m_pData->m_bDeviceCreated)
            {
                m_pData->m_bDeviceCreated = FALSE;

                if(FAILED(hr = OnDestroyDevice()))
                    DPF_HR(hr, "OnDestroyDevice failed");
            }

            RELEASE(m_pDevice);
        }

        DPF_HR(0, "No suitable device found");
        m_pData->m_bInReset = FALSE;
        return E_FAIL;
    }


    // Device found!
    if(EXISTING != Action)
    {
        m_pData->m_Device   = Device;
        m_pData->m_Params   = Params;
        m_PresentParameters = Params;


#if DBG
        char szDbg[256];
        char sz[256];

        switch(Action)
        {
        case RESET:  strcpy(szDbg, "reset "); break;
        case CREATE: strcpy(szDbg, "create "); break;
        default:     strcpy(szDbg, "??? "); break;
        }

        sprintf(sz, "(card%d ", Device.m_uAdapter);
        strcat(szDbg, sz);

        if(Device.m_dwBehavior & D3DCREATE_PUREDEVICE)
            strcat(szDbg, "pure ");

        if(Device.m_dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
            strcat(szDbg, "sw ");
        else if(Device.m_dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING)
            strcat(szDbg, "hw ");
        else if(Device.m_dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING)
            strcat(szDbg, "mixed ");

        if(D3DDEVTYPE_HAL == Device.m_DevType)
            strcat(szDbg, "hal ");
        else if(D3DDEVTYPE_SW == Device.m_DevType)
            strcat(szDbg, "emulation ");
        else if(D3DDEVTYPE_REF == Device.m_DevType)
            strcat(szDbg, "ref ");

        sprintf(sz, "%dx%d) ", Params.BackBufferWidth, Params.BackBufferHeight);
        strcat(szDbg, sz);

        DPF_HR(0, szDbg);
#endif


        BOOL bWasFullscreen = m_pData->m_bFullscreen;
        m_pData->m_bFullscreen = !Params.Windowed;


   
        // Set initial framerate estimate to half the monitor refresh rate.  This
        // is completely arbitrary, but it seems reasonable.
        D3DDISPLAYMODE AdapterMode;

        if(FAILED(m_pD3D->GetAdapterDisplayMode(Device.m_uAdapter, &AdapterMode)))
            AdapterMode.RefreshRate = 0;

        if(AdapterMode.RefreshRate)
            D3DXTimer_Start(&m_pData->m_Timer, (float) AdapterMode.RefreshRate * 0.5f);
        else
            D3DXTimer_Start(&m_pData->m_Timer, 30.0f);



        // Update window
        DWORD dwSWP = SWP_NOMOVE | SWP_NOSIZE;

        if(m_pData->m_bFullscreen)
        {
            dwSWP |= SWP_NOZORDER | SWP_SHOWWINDOW;

            // Remove border from window
            if(!m_pData->m_bCoverWindow)
            {
                SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP);
                m_pData->m_bCoverWindow = TRUE;
                dwSWP |= SWP_FRAMECHANGED;
            }


            // Calculate screen aspect
            m_pData->m_fScreenAspect = (4.0f * Params.BackBufferHeight) / (3.0f * Params.BackBufferWidth);
        }
        else
        {
            dwSWP |= SWP_SHOWWINDOW;

            // Add border to window, move back to orig position
            if(m_pData->m_bCoverWindow)
            {
                SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowPlacement(m_hwnd, &m_pData->m_WindowPlacement);
                InvalidateRect(NULL, NULL, TRUE);
                m_pData->m_bCoverWindow = FALSE;

                dwSWP |= SWP_FRAMECHANGED;
            }


            // Calculate screen aspect from current display mode
            D3DDISPLAYMODE mode;

            if(SUCCEEDED(m_pD3D->GetAdapterDisplayMode(Device.m_uAdapter, &mode)))
                m_pData->m_fScreenAspect = (4.0f * mode.Height) / (3.0f * mode.Width);
            else
                m_pData->m_fScreenAspect = 1.0f;
        }

        SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, dwSWP);
    }



    // Compute window aspect ratio
    if(!Params.Windowed)
        m_pData->m_fWindowAspect = (float) (Params.BackBufferWidth) / (float) (Params.BackBufferHeight);
    else if(WindowRect.right > WindowRect.left)
        m_pData->m_fWindowAspect = (float) (WindowRect.right - WindowRect.left) / (float) (WindowRect.bottom - WindowRect.top);
    else
        m_pData->m_fWindowAspect = 1.0f;



    // Create and Reset callbacks
    if(!m_pData->m_bDeviceCreated)
    {
        m_pData->m_bDeviceCreated = TRUE;
        m_pData->m_bDeviceReset = FALSE;

        if(FAILED(hr = OnCreateDevice()))
        {
            DPF_HR(hr, "OnCreateDevice failed");
            m_pData->m_bInReset = FALSE;
            return hr;
        }
    }

    if(!m_pData->m_bDeviceReset)
    {
        m_pData->m_bDeviceReset = TRUE;

        if(FAILED(hr = OnResetDevice()))
        {
            DPF_HR(hr, "OnResetDevice failed");
            m_pData->m_bInReset = FALSE;
            return hr;
        }
    }

    m_pData->m_bInReset = FALSE;
    return S_OK;
}


HRESULT
CD3DXApplication::Run()
{
    MSG msg;
    HRESULT hr;

    if(!m_pData)
    {
        DPF_HR(0, "Application not initialized");
        return E_FAIL;
    }

    if(m_pData->m_bRunning)
    {
        DPF_HR(0, "Application is already running");
        return E_FAIL;
    }

    m_pData->m_bRunning    = TRUE;
    m_pData->m_bStopping   = FALSE;
    m_pData->m_bNeedReset  = TRUE;
    m_pData->m_bNeedDraw   = FALSE;



    // Main applcation loop
    while(!m_pData->m_bStopping)
    {
        // Reset the device
        if(m_pData->m_bNeedReset)
        {
            if(FAILED(hr = Reset(NULL)))
            {
                m_pData->m_bRunning = FALSE;
                break;
            }

            D3DXTimer_Start(&m_pData->m_Timer, m_pData->m_Timer.m_fFramesPerSec);
            m_pData->m_bNeedReset = FALSE;
        }

        // Look for messages in queue.  Application will dutifully process all 
        // incoming messages before trying to process a frame.

        if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if(WM_QUIT == msg.message)
                break;

            if(!TranslateAccelerator(m_hwnd, m_pData->m_hAccel, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } 
        else
        {
            hr = m_pDevice->TestCooperativeLevel();

            if(D3D_OK == hr)
            {
                // Device is available!  If the application is currently active,
                // we will update and draw the scene.  If not, we will just idle.

                if(m_pData->m_bActive)
                {
                    if(FAILED(hr = Update()))
                        break;

                    if(m_pData->m_bStopping)
                        break;
                }

                if(m_pData->m_bActive || m_pData->m_bNeedDraw)
                {
                    if(FAILED(hr = Draw()))
                        break;

                    m_pData->m_bNeedDraw = FALSE;

                    if(m_pData->m_bStopping)
                        break;
                }

                if(!m_pData->m_bActive)
                {
                    if(FAILED(hr = OnIdle()))
                    {
                        DPF_HR(hr, "OnIdle failed");
                        break;
                    }
                }
            }
            else if(D3DERR_DEVICENOTRESET == hr)
            {   
                // The device is available, but needs to be Reset.
                m_pData->m_bDeviceReset = FALSE;

                if(FAILED(hr = OnLostDevice()))
                {
                    DPF_HR(hr, "OnLostDevice failed");
                    break;
                }

                if(m_pData->m_bStopping)
                    break;

                if(SUCCEEDED(hr = m_pDevice->Reset(&m_pData->m_Params)))
                {
                    m_pData->m_bDeviceReset = TRUE;

                    if(FAILED(hr = OnResetDevice()))
                    {
                        DPF_HR(hr, "OnResetDevice failed");
                        break;
                    }
                }
                else
                {
                    // Reset failed for some reason.  As a last-ditch effort,
                    // we will destroy the device and attempt to recreate, since
                    // there might be another adapter/devtype which will still work.
                    m_pData->m_bDeviceCreated = FALSE;

                    hr = OnDestroyDevice();
                    RELEASE(m_pDevice);

                    if(FAILED(hr))
                    {
                        DPF_HR(hr, "OnDestroyDevice failed");
                        break;
                    }

                    m_pData->m_bNeedReset = TRUE;
                }
            }
            else if(D3DERR_DEVICELOST == hr)
            {
                // Device is not currently available.  We will just idle until
                // the device becomes available again.

                if(FAILED(hr = OnIdle()))
                {
                    DPF_HR(hr, "OnIdle failed");
                    break;
                }
            }
        }
    }


    // Destroy the device, if one exists.
    if(m_pDevice)
    {
        HRESULT hrT;

        if(m_pData->m_bDeviceReset)
        {
            m_pData->m_bDeviceReset = FALSE;

            if(FAILED(hrT = OnLostDevice()))
            {
                DPF_HR(hrT, "OnLostDevice failed");
                hr = FAILED(hr) ? hr : hrT;
            }
        }

        if(m_pData->m_bDeviceCreated)
        {
            m_pData->m_bDeviceCreated = FALSE;

            if(FAILED(hrT = OnDestroyDevice()))
            {
                DPF_HR(hrT, "OnDestroyDevice failed");
                hr = FAILED(hr) ? hr : hrT;
            }
        }

        RELEASE(m_pDevice);
    }

    m_pData->m_bRunning = FALSE;
    return hr;
}


HRESULT 
CD3DXApplication::Stop()
{
    if(!m_pData)
    {
        DPF_HR(0, "Application not initialized");
        return E_FAIL;
    }

    if(!m_pData->m_bRunning)
    {
        DPF_HR(0, "Application is not running");
        return E_FAIL;
    }

    m_pData->m_bStopping = TRUE;
    return S_OK;
}


BOOL 
CD3DXApplication::IsRunning()
{
    return m_pData && m_pData->m_bRunning;
}


BOOL 
CD3DXApplication::IsFullscreen()
{
    return m_pData && m_pData->m_bFullscreen;
}


HRESULT
CD3DXApplication::Update()
{
    HRESULT hr;

    if(!m_pData)
    {
        DPF_HR(0, "Application not initialized");
        return E_FAIL;
    }

    if(!m_pData->m_bRunning)
    {
        DPF_HR(0, "Application is not running");
        return E_FAIL;
    }

    if(FAILED(hr = OnUpdate(m_pData->m_Timer.m_fSecsPerFrame)))
    {
        DPF_HR(hr, "OnUpdate failed");
        return hr;
    }

    if(m_pData->m_bActive)
        D3DXTimer_OnFrame(&m_pData->m_Timer);

    return S_OK;
}


HRESULT
CD3DXApplication::Draw()
{
    HRESULT hr;
    BOOL bRecreate = FALSE;

    if(!m_pData)
    {
        DPF_HR(0, "Application not initialized");
        return E_FAIL;
    }

    if(!m_pData->m_bRunning)
    {
        DPF_HR(0, "Application is not running");
        return E_FAIL;
    }

    if(!m_pDevice || m_pData->m_bDrawing || !m_pData->m_bDeviceCreated || !m_pData->m_bDeviceReset)
        return S_OK;


    // Calculate aspect ratio
    float fAspect = m_pData->m_fWindowAspect * m_pData->m_fScreenAspect;


    // Draw scene
    m_pDevice->BeginScene();

    m_pData->m_bDrawing = TRUE;
    hr = OnDraw(fAspect);
    m_pData->m_bDrawing = FALSE;

    m_pDevice->EndScene();

    if(FAILED(hr))
    {
        DPF_HR(hr, "OnDraw failed");
        return hr;
    }

    RECT r;
    GetClientRect(m_hwnd, &r);
    r.top += 28;
    r.bottom -= 20;
    hr = m_pDevice->Present(NULL, &r, NULL, NULL);
    if (FAILED(hr))
        m_pData->m_bActive = FALSE;
    return S_OK;
}


BOOL 
CD3DXApplication::AreCapsSufficient(D3DCAPS9 *pCaps, DWORD dwBehavior)
{
    // (Default implementation - please override)
    return TRUE;
}


LRESULT 
CD3DXApplication::OnMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // (Default implementation - please override)

    switch(uMsg)
    {
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_ESCAPE: // Exit app
            PostQuitMessage(0);
            break;
        }

        break;

    case WM_SETCURSOR:
        if(IsFullscreen())
        {
            SetCursor(NULL);
            return 0;
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


HRESULT 
CD3DXApplication::OnUpdate(float fSecsPerFrame)
{
    // (Default implementation - please override)
    return S_OK;
}


HRESULT 
CD3DXApplication::OnDraw(float fAspectRatio)
{
    // (Default implementation - please override)
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);

    // Just to be nice
    Sleep(1);

    return S_OK;
}


HRESULT 
CD3DXApplication::OnIdle()
{
    // (Default implementation - please override)
    WaitMessage();
    return S_OK;
}


HRESULT 
CD3DXApplication::OnCreateDevice()
{
    // (Default implementation - please override)
    return S_OK;
}


HRESULT 
CD3DXApplication::OnResetDevice()
{
    // (Default implementation - please override)
    return S_OK;
}


HRESULT 
CD3DXApplication::OnLostDevice()
{
    // (Default implementation - please override)
    return S_OK;
}


HRESULT 
CD3DXApplication::OnDestroyDevice()
{
    // (Default implementation - please override)
    return S_OK;
}


LRESULT CALLBACK 
CD3DXApplication::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    CD3DXApplication *pApp = (CD3DXApplication *) (LPVOID) GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if(WM_CLOSE == uMsg)
        PostQuitMessage(0);

    if(!pApp)
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    if(pApp->m_pData)
    {
        switch(uMsg)
        {
        case WM_ACTIVATEAPP:
            if(!LOWORD(wParam) && pApp->m_pData->m_bActive)
            {
                pApp->m_pData->m_bActive = FALSE;
            }
            else if(LOWORD(wParam) && !pApp->m_pData->m_bActive)
            {
                pApp->m_pData->m_bActive = TRUE;
                D3DXTimer_Start(&pApp->m_pData->m_Timer, pApp->m_pData->m_Timer.m_fFramesPerSec);
            }
            if (HIWORD(wParam))
            {
                pApp->m_pData->m_bActive = FALSE;
            }

            if (IsIconic(pApp->m_hwnd))
                pApp->m_pData->m_bActive = FALSE;


            break;


        case WM_ENTERSIZEMOVE:
            if(!pApp->m_pData->m_bFullscreen)
                pApp->m_pData->m_bInSizeMove = TRUE;

            break;

        case WM_SIZE:
            if(!pApp->m_pData->m_bFullscreen && !pApp->m_pData->m_bInSizeMove && !pApp->m_pData->m_bInReset)
                pApp->m_pData->m_bNeedReset = TRUE;
            
            if(!pApp->m_pData->m_bFullscreen && LOWORD(lParam))
                pApp->m_pData->m_fWindowAspect = (float) LOWORD(lParam) / (float) HIWORD(lParam);

            break;
            
        case WM_EXITSIZEMOVE:
            if(!pApp->m_pData->m_bFullscreen)
            {
                pApp->m_pData->m_bInSizeMove = FALSE;
                pApp->m_pData->m_bNeedReset  = TRUE;
                pApp->m_pData->m_bNeedDraw   = TRUE;
            }
            break;
            
        case WM_PAINT:
            BeginPaint(hwnd, &ps);

            RECT r;
            GetClientRect(pApp->m_hwnd, &r);
            r.top += 28;
            r.bottom -= 20;
            pApp->m_pDevice->Present(NULL, &r, NULL, NULL);

            //pApp->Draw();
            pApp->m_pData->m_bNeedDraw = TRUE;

            EndPaint(hwnd, &ps);
            return 0L;

        case WM_ERASEBKGND:
            return 0L;
        }
    }

    return pApp->OnMessage(hwnd, uMsg, wParam, lParam);
}

