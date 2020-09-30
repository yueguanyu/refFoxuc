#pragma once

#ifndef __GXATTRIB_H__
#define __GXATTRIB_H__

/*//////////////////////////////////////////////////////////////////////////////
//
// File: gxattrib.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

// {EBEC8816-BDE9-4822-B7F4-4014EC9CD04F}
DEFINE_GUID(IID_IGXAttributeBundle, 
0xebec8816, 0xbde9, 0x4822, 0xb7, 0xf4, 0x40, 0x14, 0xec, 0x9c, 0xd0, 0x4f);

// {79B52C0D-1287-432e-B3AF-82C53881EF79}
DEFINE_GUID(IID_IGXAttributeBundleDX7, 
0x79b52c0d, 0x1287, 0x432e, 0xb3, 0xaf, 0x82, 0xc5, 0x38, 0x81, 0xef, 0x79);

// {38E7B518-9D45-4227-9C5B-B1F4E3CAB725}
DEFINE_GUID(IID_IGXAttributeBundleSwitch, 
0x38e7b518, 0x9d45, 0x4227, 0x9c, 0x5b, 0xb1, 0xf4, 0xe3, 0xca, 0xb7, 0x25);



class IGXAttributeBundle : public IUnknown
{
public:

    // punch attribute set to device
    virtual HRESULT SetAttributesToDevice( LPDIRECT3DDEVICE8 pDevice ) PURE;

    // user defined data
    virtual HRESULT SetUserData( PVOID pvUserData ) PURE;
    virtual HRESULT GetUserData( PVOID *ppvUserData ) PURE;

    // user defined sort index for preferred order to draw attribute bundle sets in
    virtual HRESULT SetSortIndex( DWORD dwSortIndex ) PURE;
    virtual HRESULT GetSortIndex( PDWORD pdwSortIndex ) PURE;
};

class IGXAttributeBundleDX7 : public IGXAttributeBundle
{
public:
    virtual HRESULT SetMaterial( D3DMATERIAL8 *pmat ) PURE;

    // set texture in a given stage
    virtual HRESULT SetTexture( UINT Stage, LPDIRECT3DBASETEXTURE8 pTexture ) PURE;
        // AddRef's and caches given texture pointer (then Release's old texture pointer, if it was non-NULL.)
        // (make sure to Release after AddRef'ing in case before and after interface pointers are the same
        // setting a given texture pointer to NULL disables preceding stages
        // note, these semantics are quite similar to IDirect3DDevice3::SetTexture
    virtual HRESULT GetTexture( UINT Stage , LPDIRECT3DBASETEXTURE8 *pOut) PURE;
        // passes back NULL if no stage, otherwise AddRef's and passes back corresponding texture pointer
    virtual HRESULT GetTextureCount( PUINT pcTextures ) PURE;
        // passes back number of textures in cascade for this attribute bundle, or 0 if no textures.

    // the remaining entry points maintain DWORD pairs of state-to-change/new value. I am morally certain
    // that DirectX ISVs maintain data structures that look much like this. Common attribute bundles can
    // be initialized from static data in the application.
    virtual HRESULT SetTextureStageStates( UINT Stage, LPDWORD pdwTSSPairs, UINT cTSSPairs ) PURE;
    virtual HRESULT GetTextureStageStates( UINT Stage, LPDWORD pdwTSSPairs, PUINT pcTSSPairs ) PURE;
        // NOTE for the Get* entry points: either or both of pdwTSSPairs or pcTSS can be NULL.
        // If pdwTSSPairs is NULL, the client probably wants to know how many TSS pairs there are
        // so it can do an allocation on its end. If pcTSS is NULL, the client probably knows how
        // many states there are (or something). It's probably not interesting to check for both-NULL
        // and return an error if that condition pertains.
    virtual HRESULT SetRenderStates( LPDWORD pdwRSPairs, UINT cRSPairs ) PURE;
    virtual HRESULT GetRenderStates( LPDWORD pdwRSPairs, PUINT pcRSPairs ) PURE;

};

class IGXAttributeBundleSwitch : public IGXAttributeBundle
{
public:

    // set the i'th attribute bundle to resolve to
    virtual HRESULT SetAttributeBundle(UINT iAttributeBundle, IGXAttributeBundle *pattr) PURE;

    // get the i'th attribute bundle that would be resolved to
    virtual HRESULT GetAttributeBundle(UINT iAttributeBundle, IGXAttributeBundle **ppattr) PURE;

    // get the total number of attribute bundles that can be resolved to
    virtual HRESULT GetAttributeBundleCount( PUINT pcAttributeBundles ) PURE;

    // set the current attribute bundle to resolve to
    virtual HRESULT SetCurrentAttributeBundle( UINT iAttributeBundle ) PURE;
};


HRESULT GXCreateAttributeBundleDX7(IGXAttributeBundleDX7 **ppattrNew);
HRESULT GXCreateAttributeBundleSwitch(IGXAttributeBundleSwitch **ppattrNew, UINT cpattrResolveMax);

#endif
