///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx9dbg.cpp
//  Content:    D3DX debugging functions
//
///////////////////////////////////////////////////////////////////////////

#include "pchgxu.h"


#if DBG

static BOOL g_bDebugMute = FALSE;


//
// DPF
//

void cdecl D3DXDebugPrintfMView(UINT lvl, LPSTR szFormat, ...)
{
    static UINT uDebugLevel = (UINT) -1;

    char strA[256];
    char strB[256];

    if((UINT) -1 == uDebugLevel)
        uDebugLevel = GetProfileInt("Direct3D", "debug", 0);

    if((lvl > uDebugLevel) || g_bDebugMute)
        return;

    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(strA, sizeof(strA), szFormat, ap);
    strA[255] = '\0';
    va_end(ap);

    _snprintf(strB, sizeof(strB), "D3DX: %s\r\n", strA);
    strB[255] = '\0';

    OutputDebugStringA(strB);
}


//
// D3DXASSERT
//

int WINAPI D3DXDebugAssertMView(LPCSTR szFile, int nLine, LPCSTR szCondition)
{
    typedef BOOL (*PFNBV)(VOID);

    static DWORD dwValue = 0;
    static BOOL bInit = FALSE;
    static PFNBV pIsDebuggerPresent = NULL;

    LONG err;
    char str[256];


    // Print message to debug console
    DPF(0, "Assertion failure! (%s %d): %s", szFile, nLine, szCondition);


    // Initialize stuff
    if(!bInit)
    {
        HKEY hkey;
        DWORD dwType;
        DWORD cbValue = sizeof(DWORD);
        HINSTANCE hinst;

        bInit = TRUE;

        // Get IsDebuggerPresent entry point
        if((hinst = (HINSTANCE) GetModuleHandle("kernel32.dll")) ||
           (hinst = (HINSTANCE) LoadLibrary("kernel32.dll")))
        {
            pIsDebuggerPresent = (PFNBV) GetProcAddress(hinst, "IsDebuggerPresent");
        }

        // Get debug level from registry
        err = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Direct3D", &hkey);

        if(ERROR_SUCCESS != err)
            return 0;

        err = RegQueryValueEx(hkey, "D3DX", NULL, &dwType, (LPBYTE) &dwValue, &cbValue);

        RegCloseKey(hkey);

        if(ERROR_SUCCESS != err || REG_DWORD != dwType || cbValue != sizeof(DWORD))
            return 0;
    }

    if(0 == dwValue)
        return 0;

    if(1 == dwValue)
        return 1;


    // Display a message box if no debugger is present
    if((dwValue == 3) || (pIsDebuggerPresent && !pIsDebuggerPresent()))
    {
        _snprintf(str, sizeof(str), "File:\t %s\nLine:\t %d\nAssertion:\t%s\n\nDo you want to invoke the debugger?", szFile, nLine, szCondition);
	    err = MessageBox(NULL, str, "D3DX Assertion Failure", MB_SYSTEMMODAL | MB_YESNOCANCEL);

        switch(err)
        {
        case IDYES:     return 1;
        case IDNO:      return 0;
        case IDCANCEL:  FatalAppExit(0, "D3DX Assertion Failure.. Application terminated"); return 1;
        }
    }

	return 0;
}


#endif // DBG

#if 0
//
// DPF_MUTE
//

typedef void (*LPDEBUGSETMUTE)(BOOL);

void D3DXDebugMute(BOOL bMute)
{
    static LPDEBUGSETMUTE pDSM_D3D9  = NULL;
    static LPDEBUGSETMUTE pDSM_D3D9D = NULL;


    // Mute D3DX
#if DBG
    g_bDebugMute = bMute;
#endif


    // Mute D3D9
    if(!pDSM_D3D9 && !pDSM_D3D9D)
    {
        HINSTANCE hInst;

        if(GetModuleHandle("d3d9.dll"))
        {
            if((hInst = (HINSTANCE) LoadLibrary("d3d9.dll")))
                pDSM_D3D9 = (LPDEBUGSETMUTE) GetProcAddress(hInst, "DebugSetMute");
        }

        if(GetModuleHandle("d3d9d.dll"))
        {
            if((hInst = (HINSTANCE) LoadLibrary("d3d9d.dll")))
                pDSM_D3D9D = (LPDEBUGSETMUTE) GetProcAddress(hInst, "DebugSetMute");
        }
    }


    if(pDSM_D3D9)
        pDSM_D3D9(bMute);

    if(pDSM_D3D9D)
        pDSM_D3D9D(bMute);
}
#endif