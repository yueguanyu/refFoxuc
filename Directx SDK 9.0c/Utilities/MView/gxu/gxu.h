#pragma once

#ifndef __GXU_H
#define __GXU_H

#ifndef WIN_TYPES
#define WIN_TYPES(itype, ptype) typedef interface itype *LP##ptype, **LPLP##ptype
#endif
interface IDirectXFileData;
WIN_TYPES(IDirectXFileData,             DIRECTXFILEDATA);

/*//////////////////////////////////////////////////////////////////////////////
//
// File: gxu.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
// Description:
//    This header file is used to turn on asserts and tracing even in release
//  mode.
//
//
//////////////////////////////////////////////////////////////////////////////*/
#include "d3dx8dbg.h"

#if 0
// templates generate names > 255 chars in debug info
// 4786: identifier was truncated to 'number' characters in the debug information
#pragma warning(disable : 4786)

#if defined(DBG) || defined(DEBUG) || defined (_DEBUG)
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #ifndef DBG
        #define DBG 1
    #endif
#endif

#if defined(WIN32) || defined(_WIN32)
    #ifndef WIN32
        #define WIN32
    #endif
    #ifndef _WIN32
        #define _WIN32
    #endif
#endif // #if defined(WIN32) || defined(_WIN32)


#define MACSTART do {
#define MACEND } while (0)

#if defined(_WINDOWS) || defined(WIN32)
    #ifndef _INC_WINDOWS
        #ifndef WIN32_EXTRA_LEAN
            #define WIN32_EXTRA_LEAN
        #endif
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #include <windows.h>
        #include <wtypes.h>
    #endif
    #ifndef _WINBASE_
        #include <winbase.h>
    #endif
    #include <winerror.h>
#endif

// Macro: GXRELEASE
//    Safe release for COM objects
// ***this code should never change - there is stuff that relies on the pointer being
//    set to NULL after being released
#ifndef GXRELEASE
    #define GXRELEASE(_p) MACSTART if ((_p) != NULL) {(_p)->Release(); (_p) = NULL;} MACEND
#endif
#ifndef CHECK_HR
    #define CHECK_HR(__hr) MACSTART if (FAILED(__hr)) goto e_Exit; MACEND
#endif
#ifndef CHECK_MEM
    #define CHECK_MEM(__p) MACSTART if ((__p) == NULL) { hr = E_OUTOFMEMORY; goto e_Exit; } MACEND
#endif

#ifndef MAKE_USERERROR
    #define MAKE_USERERROR(code)    MAKE_HRESULT(1,FACILITY_ITF,code)
#endif
#ifndef E_NOTINITIALIZED
    #define E_NOTINITIALIZED        MAKE_USERERROR(0xFFFC)
#endif
#ifndef E_ALREADYINITIALIZED
    #define E_ALREADYINITIALIZED    MAKE_USERERROR(0xFFFB)
#endif
#ifndef E_NOTFOUND
    #define E_NOTFOUND              MAKE_USERERROR(0xFFFA)
#endif
#ifndef E_INSUFFICIENTDATA
    #define E_INSUFFICIENTDATA      MAKE_USERERROR(0xFFF9)
#endif

//
// DEBUG STUFF
//

#ifdef _DEBUG
    #include <stdio.h>
    #include <crtdbg.h>
    inline void __cdecl
        _GXTrace(const char *szFmt, ...)
    {
        char szMsgBuf[1024];
        va_list alist;
        va_start( alist, szFmt );
        _vsnprintf(szMsgBuf, 1024 - 1, szFmt, alist );
    #if defined(_WINDOWS) || defined(WIN32) || defined(_WIN32)
        OutputDebugString(szMsgBuf);
    #endif // #if defined(_WINDOWS) || defined(WIN32) || defined(_WIN32)
        fprintf(stderr, "%s", szMsgBuf);
        fflush(stderr);
    }

    #define GXASSERT(exp)       ((void) 0)
    #define GXASSERT(exp)       D3DXASSERT(exp)
    #define GXVERIFY(exp)       GXASSERT(exp)
    #define GXDEBUG_ONLY(exp)   (exp)
    #define GXTRACE             ::_GXTrace
#else // #ifdef _DEBUG
    inline void __cdecl _GXTrace(const char *szFmt, ...) {}
    #define GXASSERT(exp)       ((void) 0)
    #define GXVERIFY(exp)       ((void) (exp))
    #define GXDEBUG_ONLY(exp)   ((void) 0)
    #define GXTRACE             1 ? (void) 0 : ::_GXTrace
#endif // #else // #ifdef _DEBUG
#endif


#endif // #ifndef __GXTESTS_H
