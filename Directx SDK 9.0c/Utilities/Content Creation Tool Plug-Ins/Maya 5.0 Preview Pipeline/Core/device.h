//--------------------------------------------------------------------------------------
// File: Device.h
//
// Encapsulates a Direct3D device which exists in a separate thread
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ENGINE_H
#define ENGINE_H

class CEngine : public CPreviewPipelineEngine, public CPreviewPipelineManagerEvents
{
public:
	virtual HRESULT Create( CPreviewPipeline* pPreviewPipeline );
	virtual HRESULT Destroy();

	virtual HRESULT GetD3DObject( IDirect3D9** ppObject );
	virtual HRESULT GetD3DDevice( IDirect3DDevice9** ppDevice );

private:
    UINT __stdcall RunThread();
    static UINT __stdcall StaticRunThread( void* pParam );

    static bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
    static void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );

private:
    bool m_bDestroyCalled;
    UINT m_nThreadID;
    uintptr_t m_hThread;
	HWND m_hwndFocus;

    IDirect3DDevice9* m_pd3dDevice;
    CPreviewPipeline* m_pPreviewPipeline;
};

#endif //ENGINE_H