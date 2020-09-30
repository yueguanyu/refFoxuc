///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8dbg.h
//  Content:    D3DX debugging functions
//
///////////////////////////////////////////////////////////////////////////


#ifndef __D3DX8DBG_H__
#define __D3DX8DBG_H__


//
// DPF
//

#if DBG

    void cdecl D3DXDebugPrintfMView(UINT lvl, LPSTR szFormat, ...);
    void cdecl D3DXDebugPrintfHRMView(UINT lvl, HRESULT hr, LPSTR szFormat, ...);

    #define DPF D3DXDebugPrintfMView
    #define DPFHR D3DXDebugPrintfHRMView

#else // !DBG

    #define DPF
    #define DPFHR

#endif // !DBG




//
// D3DXASSERT
//

#if DBG

    int WINAPI D3DXDebugAssertMView(LPCSTR szFile, int nLine, LPCSTR szCondition);

    #define D3DXASSERT(condition) \
        do { if(!(condition) && D3DXDebugAssertMView(__FILE__, __LINE__, #condition)) DebugBreak(); } while(0)

#else // !DBG

    #define D3DXASSERT(condition) 0

#endif // !DBG


//
// Memory leak checking
//
#ifdef DBG
//#define MEM_DEBUG
#endif

#ifdef MEM_DEBUG

    BOOL WINAPI D3DXDumpUnfreedMemoryInfo();
    void *operator new(size_t stAllocateBlock, const UINT32 uiLineNumber, const char *szFilename);
    void operator delete(void *pvMem, size_t stAllocateBlock, const UINT32 uiLineNumber, const char *szFilename);
    //void operator delete( void *pvMem );


#define new new(__LINE__, __FILE__)

#else // !DBG

    BOOL WINAPI D3DXDumpUnfreedMemoryInfo();

#define New new

#endif // !DBG

#endif // __D3DX8DBG_H__
