///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8dbg.cpp
//  Content:    D3DX debugging functions
//
///////////////////////////////////////////////////////////////////////////

#include "pchgxu.h"
#if 0
//#if DBG


//
// DPF
//

void cdecl D3DXDebugPrintf(UINT lvl, LPSTR szFormat, ...)
{
    char strA[256];
    char strB[256];

//    if(lvl > (UINT) g_dwDebugLevel)
//        return;

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
// DPFHR
//

void cdecl D3DXDebugPrintfHR(UINT lvl, HRESULT hr, LPSTR szFormat, ...)
{
    char strA[256];
    char strB[256];

    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(strA, sizeof(strA), szFormat, ap);
    strA[255] = '\0';
    va_end(ap);

    D3DXGetErrorStringA(hr, strB, sizeof(strB));
    D3DXDebugPrintf(lvl, "%s: %s", strA, strB);
}



//
// D3DXASSERT
//

int WINAPI D3DXDebugAssert(LPCSTR szFile, int nLine, LPCSTR szCondition)
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