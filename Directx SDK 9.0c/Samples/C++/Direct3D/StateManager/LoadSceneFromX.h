//--------------------------------------------------------------------------------------
// File: LoadSceneFromX.h
//
// Enables the sample to build a scene from an x-file ('scene.x').
// The x-file has been extended to include custom templates for specifying mesh filenames
// and camera objects within frames.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------
// STL dependancy
//--------------------------------------------------------------------------------------
#pragma warning ( push )
#pragma warning ( disable : 4702 ) // unreachable code
#include <vector>
#pragma warning ( pop )
using std::vector;


//--------------------------------------------------------------------------------------
// Contains a reference to a mesh file.
// Multiple meshes may exist within a given frame.
//--------------------------------------------------------------------------------------
typedef struct _MESH_REFERENCE
{
    DWORD dwRenderPass;
    CHAR szFileName[MAX_PATH];
    _MESH_REFERENCE(DWORD _dwRenderPass, LPCSTR pszFileName )
    {
        dwRenderPass = _dwRenderPass;
        strncpy( szFileName, pszFileName, MAX_PATH );
        szFileName[MAX_PATH-1] = 0;
    }
} MESH_REFERENCE;


//--------------------------------------------------------------------------------------
// Contains a reference to a camera object
//--------------------------------------------------------------------------------------
typedef struct _CAMERA_REFERENCE
{
   FLOAT fRotationScaler;   // Camera rotation speed
   FLOAT fMoveScaler;       // Camera translation speed
   _CAMERA_REFERENCE( FLOAT _fRotationScaler, FLOAT _fMoveScaler )
   {
        fRotationScaler = _fRotationScaler;
        fMoveScaler = _fMoveScaler;
   }
} CAMERA_REFERENCE;


//--------------------------------------------------------------------------------------
// Contains a node for a frame within the hierarchy.
//--------------------------------------------------------------------------------------
typedef struct _FRAMENODE
{
    D3DXMATRIX mat;                     // Holds the collapsed matrix value for the frame
    vector<MESH_REFERENCE>   meshes;    // Each Frame may contain multiple meshes
    vector<CAMERA_REFERENCE> cameras;   // Each Frame may contain multiple cameras
} FRAMENODE;



//--------------------------------------------------------------------------------------
// Reads the scene x-file, and adds the collapsed frame hierarchy to the vecFrameNodes Hierarchy.
//--------------------------------------------------------------------------------------
HRESULT LoadSceneFromX( vector<FRAMENODE>& vecFrameNodes, LPWSTR wszFileName );
