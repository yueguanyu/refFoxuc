/*//////////////////////////////////////////////////////////////////////////////
//
// File: mviewpch.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

// identifier was truncated to '255' characters in the browser information
#pragma warning(disable: 4786) 

//==========================================================================;
//
//  Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.
//
//  File:       trivwin.cpp
//  Content:    Trivial windowed application that uses the 
//              DirectX Initialization API.
//
//**************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <tchar.h>
#include <commctrl.h>
#include <shellapi.h>

#include <objbase.h>
#include <initguid.h>


#include <malloc.h> // _alloca
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "common.h"
#include "resource.h"
#include "filestrm.h"
#include "gxcrackfvf.h"
#include "d3dArcball.h"
#include "StripOutline.h"
#include "AdjOutline.h"
#include "EdgeOutline.h"
#include "ShowNormal.h"
#include "showarcball.h"
#include "treeview.h"
#include "d3dxapp.h"
#include "npatchoutline.h"

#include "skin.h"

#include "d3dx9anim.h"

#include "util.h"

#include "gxu.h"
#include "mview.h"

#ifdef _DEBUG
#define MEM_DEBUG 1
#endif

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 
