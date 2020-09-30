//--------------------------------------------------------------------------------------
// File: PreviewPipeline.h
//
// Defines the interfaces for the preview pipeline
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef PREVIEWPIPELINE_H
#define PREVIEWPIPELINE_H

#include "dxcc.h"

#include <vector>
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

// Forward declaration
class CPreviewPipelineLock;
class CPreviewPipelineEngineEvents;
class CPreviewPipelineManagerEvents;
class CPreviewPipelineViewerEvents;
class CPreviewPipelineEngineEventsTrigger;
class CPreviewPipelineManagerEventsTrigger;
class CPreviewPipelineViewerEventsTrigger;
class CPreviewPipeline;
class CPreviewPipelineEngine;
class CPreviewPipelineViewer;

#define GetSingleObject(WaitForLock, obj) (WAIT_OBJECT_0 == ((WaitForLock) ? WaitForSingleObject(obj, INFINITE) :   WaitForSingleObject(obj, 0)))


class CPreviewPipelineEngineEvents
{
public:

	virtual HRESULT OnD3DDeviceCreate(){ return S_OK; };//on Adapter Change recovery
	virtual HRESULT OnD3DDeviceReset(){ return S_OK; };//on Device Lost recovery
	virtual HRESULT OnD3DDeviceLost(){ return S_OK; };//on Device Lost
	virtual HRESULT OnD3DDeviceDestroy(){ return S_OK; };//on Adapter Change
};

class CPreviewPipelineManagerEvents
{
public:
	CPreviewPipelineManagerEvents();
	~CPreviewPipelineManagerEvents();

	//Set the thread-dependant exclusivity status
	virtual bool SetExclusiveMode(bool WaitToAcquire, bool PauseState);

	virtual HRESULT OnReload(){ return S_OK; };//expect that everything is new
	virtual HRESULT OnResourceAdd(IDXCCResource* pRes){ return S_OK; };
	virtual HRESULT OnResourceRecycle(IDXCCResource* pRes){ return S_OK; };
	virtual HRESULT OnResourceUpdate(IDXCCResource* pRes){ return S_OK; };
	virtual HRESULT OnResourceRemove(IDXCCResource* pRes){ return S_OK; };

private:
	HANDLE ExclusiveModeMutex;
};

class CPreviewPipelineViewerEvents
{
public:

	virtual HRESULT OnFrameChildAdded(IDXCCFrame* pParent, IDXCCFrame* pChild) { return S_OK; };
	virtual HRESULT OnFrameChildRemoved(IDXCCFrame* pParent, IDXCCFrame* pChild) { return S_OK; };

	virtual HRESULT OnFrameMemberAdded(IDXCCFrame* pParent, IUnknown* pChild) { return S_OK; };
	virtual HRESULT OnFrameMemberRemoved(IDXCCFrame* pParent, IUnknown* pChild) { return S_OK; };

	virtual HRESULT OnMeshChange(IDXCCMesh* pMesh){ return S_OK; };

	virtual HRESULT OnMeshDeclarationChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh){ return S_OK; };
	//topology has change, regenerate mesh
	virtual HRESULT OnMeshTopologyChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh){ return S_OK; };
	//vertices has changed so updates that range with the given Usage info
	virtual HRESULT OnMeshVertexChange(IDXCCMesh* pMesh, UINT vertMin, UINT vertMax){ return S_OK; };
	virtual HRESULT OnMeshSubVertexChange(IDXCCMesh* pMesh, D3DDECLUSAGE Usage, UINT UsageIndex, UINT vertMin, UINT vertMax){ return S_OK; };
	//the attributes used to associate a material to the mesh have changed
	virtual HRESULT OnMeshAttributeChange(IDXCCMesh* pMesh, UINT faceMin, UINT faceMax){ return S_OK; };
	//the materials associated to the attributes have changed
	virtual HRESULT OnMeshMaterialChange(IDXCCMesh* pMesh, DXCCATTRIBUTE Attrib, ID3DXEffect* pOldMaterial){ return S_OK; };
	//The material's Effect interface has changed (commonly used to unbind from Standard Semantics detabase)
	virtual HRESULT OnMaterialEffectChange(LPD3DXEFFECT pMaterial, LPD3DXEFFECT pOldEffect){ return S_OK; };
	//The material's Effect interface has changed (commonly used to unbind from Standard Semantics detabase)
	virtual HRESULT OnMaterialParameterChange(LPD3DXEFFECT pMaterial, D3DXHANDLE hParameter){ return S_OK; };
};


class CPreviewPipelineEngineEventsTrigger : public CPreviewPipelineEngineEvents
{
public:
	std::vector<CPreviewPipelineEngineEvents*> Events;

	virtual HRESULT OnD3DDeviceCreate();
	virtual HRESULT OnD3DDeviceReset();
	virtual HRESULT OnD3DDeviceLost();
	virtual HRESULT OnD3DDeviceDestroy();
};

class CPreviewPipelineManagerEventsTrigger : public CPreviewPipelineManagerEvents
{
public:
	std::vector<CPreviewPipelineManagerEvents*> Events;


	virtual bool SetExclusiveMode(bool WaitToAcquire, bool PauseState);

	virtual HRESULT OnReload(); 
	virtual HRESULT OnResourceAdd(IDXCCResource* pRes);//EMITTED
	virtual HRESULT OnResourceRecycle(IDXCCResource* pRes);//EMITTED
	virtual HRESULT OnResourceRemove(IDXCCResource* pRes);
};

class CPreviewPipelineViewerEventsTrigger : public CPreviewPipelineViewerEvents
{
public:
	std::vector<CPreviewPipelineViewerEvents*> Events;

	virtual HRESULT OnFrameChildAdded(IDXCCFrame* pParent, IDXCCFrame* pChild);//EMITTED
	virtual HRESULT OnFrameChildRemoved(IDXCCFrame* pParent, IDXCCFrame* pChild) ;//EMITTED
	virtual HRESULT OnFrameMemberAdded(IDXCCFrame* pParent, IUnknown* pChild);//EMITTED
	virtual HRESULT OnFrameMemberRemoved(IDXCCFrame* pParent, IUnknown* pChild);//EMITTED
	virtual HRESULT OnMeshChange(IDXCCMesh* pMesh);//EMITTED
	virtual HRESULT OnMeshDeclarationChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh);
	virtual HRESULT OnMeshTopologyChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh);
	virtual HRESULT OnMeshVertexChange(IDXCCMesh* pMesh, UINT vertMin, UINT vertMax);
	virtual HRESULT OnMeshSubVertexChange(IDXCCMesh* pMesh, D3DDECLUSAGE Usage, UINT UsageIndex, UINT vertMin, UINT vertMax);
	virtual HRESULT OnMeshAttributeChange(IDXCCMesh* pMesh, UINT faceMin, UINT faceMax);
	virtual HRESULT OnMeshMaterialChange(IDXCCMesh* pMesh, DXCCATTRIBUTE Attrib, LPD3DXEFFECT pOldMaterial);
	virtual HRESULT OnMaterialEffectChange(LPD3DXEFFECT pMaterial, LPD3DXEFFECT pOldEffect);
	virtual HRESULT OnMaterialParameterChange(LPD3DXEFFECT pMaterial, D3DXHANDLE hParameter);
};


class CPreviewPipeline
{
	friend CPreviewPipelineLock;
public:

	virtual HRESULT Create();
	virtual HRESULT Destroy();

	virtual bool IsValid();

	virtual CPreviewPipelineEngine* AccessEngine();
	virtual LPDXCCMANAGER AccessManager();
	virtual LPDXCCFRAME AccessRoot();

    virtual float GetTime();
    virtual HRESULT SetTime(float time);
    virtual HRESULT SetEngine( CPreviewPipelineEngine* pPreviewEngine );

	//use these to prevent user conflicts
	virtual bool DeviceStateLock(BOOL WaitForLock, CPreviewPipelineLock& Lock);
	virtual void DeviceStateUnlock(CPreviewPipelineLock& Lock);

    virtual bool SceneWriteLock(BOOL WaitForLock, CPreviewPipelineLock& Lock); //returns true if you got the lock
	virtual void SceneWriteUnlock(CPreviewPipelineLock& Lock);

	virtual bool SceneReadLock(BOOL WaitForLock, CPreviewPipelineLock& Lock); //returns true if you got the lock
	virtual void SceneReadUnlock(CPreviewPipelineLock& Lock);

	virtual HRESULT RegisterDeviceEvents(CPreviewPipelineEngineEvents* pCall);
	virtual HRESULT UnregisterDeviceEvents(CPreviewPipelineEngineEvents* pCall);
	virtual CPreviewPipelineEngineEvents* TriggerDeviceEvent();

	virtual HRESULT RegisterManagerEvents(CPreviewPipelineManagerEvents* pCall);
	virtual HRESULT UnregisterManagerEvents(CPreviewPipelineManagerEvents* pCall);
	virtual CPreviewPipelineManagerEvents* TriggerManagerEvent();

	virtual HRESULT RegisterViewerEvents(CPreviewPipelineViewerEvents* pCall);
	virtual HRESULT UnregisterViewerEvents(CPreviewPipelineViewerEvents* pCall);
	virtual CPreviewPipelineViewerEvents* TriggerViewerEvent();


private:
	//everyone must gain the Access to play fair
	//Write not release until unlock so that no reads come though
	//Read releases immediately to support multiple writes
	//Read incriments ReadCount on gain of AccessMutex and decriment on finishing
	//Read sets ZeroEvent when  ReadCount is zero and resets when non-zero
	//write waits on ZeroEvent after it has gained AccessMutex to ensure that reads are finished
	HANDLE	ReadWriteMutex;
	HANDLE	DeviceMutex;
	CRITICAL_SECTION ZeroSceneReadersSection; 
	UINT	SceneReadersCount;
	HANDLE	ZeroSceneReadersEvent;	
	
	CPreviewPipelineEngine* pPreviewEngine;
	LPDXCCMANAGER pDXCCManager;	
	LPDXCCFRAME pDXCCRoot;
	float fTime;
	CPreviewPipelineEngineEventsTrigger DeviceEventsTrigger;
	CPreviewPipelineManagerEventsTrigger ManagerEventsTrigger;
	CPreviewPipelineViewerEventsTrigger ViewerEventsTrigger;
};


//we create a CPreviewPipelineEngine in it's own thread but you don't have to
//this prevents abnormal crashing caused by living in the same thread as OpenGL
class CPreviewPipelineEngine
{
public:
	virtual HRESULT Create(CPreviewPipeline* pPreviewPipeline) = 0;
    virtual HRESULT Destroy() = 0;

	virtual HRESULT GetD3DObject( IDirect3D9** ppObject ) = 0;
	virtual HRESULT GetD3DDevice( IDirect3DDevice9** ppDevice ) = 0;
};



//it is suggested that the viewers use StateBlocks to ensure that multiple viewers do not collide
class CPreviewPipelineViewer 
{
public:
	//get the device from the pPreviewPipeline and use CreateAdditionalSwapChain
	virtual HRESULT Create( CPreviewPipeline* pPreviewPipeline,	
							D3DPRESENT_PARAMETERS* pPresentationParameters,
							const WCHAR* strWindowTitle = L"Direct3D Preview Pipeline", 
							HINSTANCE hInstance = NULL, 
							HICON hIcon = NULL, 
							HMENU hMenu = NULL,
							int width = 640,
							int height = 480,
							int x = CW_USEDEFAULT, 
							int y = CW_USEDEFAULT) = 0;

    virtual HRESULT Destroy() = 0; //destroy thread

	virtual UINT Run() = 0; //allow core logic to start/continue
	virtual UINT GetPauseCount() = 0; //is the core logic running?
	virtual UINT Pause() = 0; //pause the core logic
	
    virtual HWND	GetRenderWindow() = 0; //get window this view will render to
    virtual HWND    GetShellWindow() = 0;//get the highest-level window you'd like docked inside a DCC
	virtual HRESULT	GetD3DSwapChain(IDirect3DSwapChain9** ppSwapChain) = 0;
};

class CPreviewPipelineLock
{
	friend CPreviewPipeline;
public:
	CPreviewPipelineLock(bool AutoUnlock= true);
	~CPreviewPipelineLock();

	long ofType(){ return Type; }
	bool isLocked(){ return Locked; }

private:
	enum
	{
		TYPE_NONE= 0,
		TYPE_DEVICE_STATE,
		TYPE_SCENE_WRITE,
		TYPE_SCENE_READ,
		NUMBER_OF_TYPES
	}					Type;
	bool				Locked;
	bool				UnlockOnDestuction;
	CPreviewPipeline*	pPreviewPipeline;
};






#endif //PREVIEWPIPELINE_H