//--------------------------------------------------------------------------------------
// File: Viewer.h
//
// Creates a window for viewing a standard semantics and annotations mesh 
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef VIEWER_H
#define VIEWER_H


struct CreateArgs
{
    D3DPRESENT_PARAMETERS PresentParams;
    WCHAR strWindowTitle[MAX_PATH+1];
    HINSTANCE hInstance;
    HICON hIcon;
    HMENU hMenu;
    int width, height;
    int x, y;
};


class CViewer : public CPreviewPipelineViewer, public CPreviewPipelineEngineEvents, public CPreviewPipelineManagerEvents, public CPreviewPipelineViewerEvents
{
public:
    CViewer();
    ~CViewer();

    HRESULT Create( CPreviewPipeline* pPreviewPipeline,	
					D3DPRESENT_PARAMETERS* pPresentationParameters,
                    const WCHAR* strWindowTitle = L"Direct3D Preview Pipeline", 
					HINSTANCE hInstance = NULL, 
					HICON hIcon = NULL, 
					HMENU hMenu = NULL,
                    int width = 640,
                    int height = 480,
					int x = CW_USEDEFAULT, 
					int y = CW_USEDEFAULT );
    HRESULT Destroy();

	HRESULT Refresh();

	void BindToWindow( HWND hWnd, BOOL bVisible=true );
	void CheckForResize( BOOL bForceResize=false );
    
    void Hide();
    void Show();

    UINT Run(){ return m_nPauseCount = 0; } //allow core logic to start/continue
    UINT GetPauseCount(){ return m_nPauseCount; } //is the core logic running?
    UINT Pause(){ return m_nPauseCount = 1; } //pause the core logic
	
    HWND	GetRenderWindow(){ return m_hWnd; } //get window this view will render to
    HWND    GetShellWindow(){ return m_hWndShell; }
    HRESULT	GetD3DSwapChain( IDirect3DSwapChain9** ppSwapChain );

    HRESULT RegisterEffectsInFrame( IDXCCFrame* pFrame );
    HRESULT RegisterEffectsInMesh( IDXCCMesh* pMesh );

    void    DrawFrame( IDXCCFrame* pFrame );
    void    DrawMesh( IDXCCMesh* pMesh );

	// CPreviewPipelineEngineEvents interface
    HRESULT OnD3DDeviceCreate();
    HRESULT OnD3DDeviceReset();
    HRESULT OnD3DDeviceLost();
    HRESULT OnD3DDeviceDestroy();

	// CPreviewPipelineManagerEvents interface
	HRESULT OnReload(){ return Refresh(); };
	HRESULT OnResourceAdd(IDXCCResource* pRes){ return Refresh(); };
	HRESULT OnResourceRecycle(IDXCCResource* pRes){ return Refresh(); };
	HRESULT OnResourceRemove(IDXCCResource* pRes){ return Refresh(); };

	// CPreviewPipelineViewerEvents interface
	HRESULT OnFrameChildAdded(IDXCCFrame* pParent, IDXCCFrame* pChild) { return Refresh(); };
	HRESULT OnFrameChildRemoved(IDXCCFrame* pParent, IDXCCFrame* pChild) { return Refresh(); };
	HRESULT OnFrameMemberAdded(IDXCCFrame* pParent, IUnknown* pChild) { return Refresh(); };
	HRESULT OnFrameMemberRemoved(IDXCCFrame* pParent, IUnknown* pChild) { return Refresh(); };
	HRESULT OnMeshChange(IDXCCMesh* pMesh){ return Refresh(); };
	HRESULT OnMeshDeclarationChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh){ return Refresh(); };
	HRESULT OnMeshTopologyChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh){ return Refresh(); };
	HRESULT OnMeshVertexChange(IDXCCMesh* pMesh, UINT vertMin, UINT vertMax){ return Refresh(); };
	HRESULT OnMeshSubVertexChange(IDXCCMesh* pMesh, D3DDECLUSAGE Usage, UINT UsageIndex, UINT vertMin, UINT vertMax){ return Refresh(); };
	HRESULT OnMeshAttributeChange(IDXCCMesh* pMesh, UINT faceMin, UINT faceMax){ return Refresh(); };
	HRESULT OnMeshMaterialChange(IDXCCMesh* pMesh, DXCCATTRIBUTE Attrib, ID3DXEffect* pOldMaterial){ return Refresh(); };
	HRESULT OnMaterialEffectChange(LPD3DXEFFECT pMaterial, LPD3DXEFFECT pOldEffect){ return Refresh(); };
	HRESULT OnMaterialParameterChange(LPD3DXEFFECT pMaterial, D3DXHANDLE hParameter){ return Refresh(); };

	static CGrowableArray< CViewer* >& GetViewerList();
    
private:
    void FrameMove();
    void Render();

    void DeviceLock( CPreviewPipelineLock& Lock );
    void DeviceUnlock( CPreviewPipelineLock& Lock );

    static LRESULT CALLBACK StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    static UINT __stdcall StaticRunThread( void* pParam );
    UINT __stdcall RunThread();
    
    void RenderText();

private:
	bool m_bCreated;
    UINT m_nThreadID;
    uintptr_t m_hThread;

	IDirect3DSurface9* m_pDepthStencil;
    IDirect3DSwapChain9* m_pSwapChain;
    CPreviewPipeline* m_pPreviewPipeline;

    float m_fTime;
    float m_fPreviousTime;
    float m_fElapsedTime;

	DWORD m_dwWidth;
	DWORD m_dwHeight;

    int m_nPauseCount;
    CreateArgs m_CreateArgs;
	CDXUTDirectionWidget m_LightWidget;
    CModelViewerCamera  m_Camera; 
    ID3DXFont*          m_pFont;         // Font for drawing text
    ID3DXSprite*        m_pTextSprite;   // Sprite for batching draw text calls
    BOOL                m_bVisible;
    HWND                m_hWnd;    
	HWND                m_hWndBound;
	HWND                m_hWndShell;

    CDXUTEffectMap      m_EffectMap;

	ID3DXEffect*        m_pDefaultEffect;
};

#endif