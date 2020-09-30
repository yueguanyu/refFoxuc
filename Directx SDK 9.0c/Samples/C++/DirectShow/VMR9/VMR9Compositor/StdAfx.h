// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Windows Header Files:
#include <objbase.h>
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <comdef.h>
#include <atlbase.h>
#include <commdlg.h>
#include <Commctrl.h>
#include <tchar.h>

// DirectShow Header Files
#include <dshow.h>
#include <streams.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include <vmr9.h>

// Local Header Files

// TODO: reference additional headers your program requires here
#define FAIL_RET(x) do {if( FAILED( hr = ( x  ) ) ) { \
                            TCHAR achMsg[MAX_PATH];/*ASSERT( SUCCEEDED( hr ) );*/ \
                            _stprintf( achMsg, TEXT("Error code 0x%08x\r\n"), hr); \
                            OutputDebugString( achMsg ); \
                        return hr; \
                    }} while(0)