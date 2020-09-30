#include "dxstdafx.h"
#include "PreviewPipeline.h"


HRESULT CPreviewPipeline::Create() 
{ 
	LPDXCCRESOURCE pResource= NULL;
	pPreviewEngine= NULL;
	fTime= 0;
	DXCCCreateManager(&pDXCCManager);
	DXCCCreateFrame(&pDXCCRoot);
	pDXCCManager->CreateResource(pDXCCRoot, IID_IDXCCFrame, TRUE, &pResource);
	pResource->SetName("DXCC_ROOT");
	DXCC_RELEASE(pResource);

	ReadWriteMutex= CreateMutexA(NULL, FALSE, "DXCCPreviewPipeline_SceneReadWriteLock");
	DeviceMutex= CreateMutexA(NULL, FALSE, "DXCCPreviewPipeline_DeviceLock");
	InitializeCriticalSection(&ZeroSceneReadersSection);
	SceneReadersCount= 0;
	ZeroSceneReadersEvent= CreateEventA(NULL, TRUE, TRUE, "DXCCPreviewPipeline_ZeroReaders");

	return S_OK; 
}

HRESULT CPreviewPipeline::Destroy() 
{ 

	DXCC_RELEASE(pDXCCRoot);
	DXCC_RELEASE(pDXCCManager);
	CloseHandle(DeviceMutex);
	DeviceMutex= NULL;
	CloseHandle(ReadWriteMutex);
	ReadWriteMutex= NULL;
	CloseHandle(ZeroSceneReadersEvent);
	ZeroSceneReadersEvent= NULL;
	DeleteCriticalSection(&ZeroSceneReadersSection);
		
	return S_OK; 
}

HRESULT CPreviewPipeline::SetEngine( CPreviewPipelineEngine* pPreviewPipelineEngine )  
{ 
	TriggerDeviceEvent()->OnD3DDeviceDestroy();
	pPreviewEngine= pPreviewPipelineEngine; 
	TriggerDeviceEvent()->OnD3DDeviceCreate();
	return S_OK;
}



HRESULT CPreviewPipeline::SetTime(float time) 
{
	fTime= time;
	return S_OK;
}

float CPreviewPipeline::GetTime() 
{
	return fTime;
}

CPreviewPipelineEngine* 
CPreviewPipeline::AccessEngine()
{
	return pPreviewEngine;
}

LPDXCCMANAGER
CPreviewPipeline::AccessManager()
{
	return pDXCCManager;
}

LPDXCCFRAME 
CPreviewPipeline::AccessRoot()
{
	return pDXCCRoot;
}

bool CPreviewPipeline::DeviceStateLock(BOOL WaitForLock, CPreviewPipelineLock& Lock)
{
	if(Lock.isLocked()==false)
	{
		Lock.Locked=true;
		Lock.Type=CPreviewPipelineLock::TYPE_DEVICE_STATE;

		return GetSingleObject(WaitForLock, DeviceMutex);
	}
	else
		return false;
}

void CPreviewPipeline::DeviceStateUnlock(CPreviewPipelineLock& Lock)
{
	if(Lock.isLocked()==true && Lock.ofType()==CPreviewPipelineLock::TYPE_DEVICE_STATE)
	{
		Lock.Locked=false;
		ReleaseMutex(DeviceMutex);
	}
}


bool CPreviewPipeline::SceneWriteLock(BOOL WaitForLock, CPreviewPipelineLock& Lock) 
{
	if(Lock.isLocked()==false)
	{
		Lock.Locked=true;
		Lock.Type=CPreviewPipelineLock::TYPE_SCENE_WRITE;
		Lock.pPreviewPipeline=this;

		if(GetSingleObject(WaitForLock, ReadWriteMutex))
		{
			if(GetSingleObject(WaitForLock, ZeroSceneReadersEvent))
			{
				return true;
			}
			else
			{
				ReleaseMutex(ReadWriteMutex);
				return false;
			}
		}
		else
			return false;
	}
	else
		return false;
}
	
void CPreviewPipeline::SceneWriteUnlock(CPreviewPipelineLock& Lock) 
{
	if(Lock.isLocked()==true && Lock.ofType()==CPreviewPipelineLock::TYPE_SCENE_WRITE)
	{
		Lock.Locked=false;
		Lock.pPreviewPipeline=NULL;

		ReleaseMutex(ReadWriteMutex);
	}
}
CPreviewPipelineLock::CPreviewPipelineLock(bool AutoUnlock)
{
	Type=TYPE_NONE; 
	Locked=false; 
	UnlockOnDestuction= AutoUnlock;
	pPreviewPipeline=NULL; 
};

CPreviewPipelineLock::~CPreviewPipelineLock()
{
	if(Locked && UnlockOnDestuction && pPreviewPipeline)
	{
		switch(Type)
		{
		case TYPE_DEVICE_STATE:
			pPreviewPipeline->DeviceStateUnlock(*this);
			break;
		case TYPE_SCENE_WRITE:
			pPreviewPipeline->SceneWriteUnlock(*this);
			break;
		case TYPE_SCENE_READ:
			pPreviewPipeline->SceneReadUnlock(*this);
			break;
		};
	}
}

bool CPreviewPipeline::SceneReadLock(BOOL WaitForLock, CPreviewPipelineLock& Lock)
{
	if(Lock.isLocked()==false)
	{
		Lock.Locked=true;
		Lock.Type=CPreviewPipelineLock::TYPE_SCENE_READ;
		Lock.pPreviewPipeline=this;

		if(GetSingleObject(WaitForLock, ReadWriteMutex))
		{
			//EnterCriticalSection(&ZeroSceneReadersSection); //using ReadWriteMutex instead
			if(0==SceneReadersCount)
				ResetEvent(ZeroSceneReadersEvent);
			++SceneReadersCount; //InterlockedIncrement(&SceneReadersCount);//using ReadWriteMutex instead
			//LeaveCriticalSection(&ZeroSceneReadersSection); //using ReadWriteMutex instead

			ReleaseMutex(ReadWriteMutex);

			return true;
		}
		else
			return false;
	}
	else
		return false;

}


void CPreviewPipeline::SceneReadUnlock(CPreviewPipelineLock& Lock)
{
	if(Lock.isLocked()==true && Lock.ofType()==CPreviewPipelineLock::TYPE_SCENE_READ)
	{
		EnterCriticalSection(&ZeroSceneReadersSection); 

		Lock.Locked=false;
		Lock.pPreviewPipeline=NULL;
		
		if(SceneReadersCount > 0
			&& 0== --SceneReadersCount)
		{
			SetEvent(ZeroSceneReadersEvent);
		}
		
		LeaveCriticalSection(&ZeroSceneReadersSection);
	}
}

bool CPreviewPipeline::IsValid()
{
	return (pPreviewEngine && pDXCCManager && pDXCCRoot);
}

HRESULT CPreviewPipeline::RegisterDeviceEvents(CPreviewPipelineEngineEvents* pCall)
{
	for(UINT i= 0; 
		i < DeviceEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineEngineEvents* &event= DeviceEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			return E_INVALIDARG;
		}
	}

	DeviceEventsTrigger.Events.push_back(pCall);

	return S_OK;
}

HRESULT CPreviewPipeline::UnregisterDeviceEvents(CPreviewPipelineEngineEvents* pCall) 
{
	for(UINT i= 0; 
		i < DeviceEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineEngineEvents* &event= DeviceEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			DeviceEventsTrigger.Events.erase( DeviceEventsTrigger.Events.begin( ) + i);
			return S_OK;
		}
	}
	
	return E_INVALIDARG;
}

CPreviewPipelineEngineEvents* CPreviewPipeline::TriggerDeviceEvent()
{
	return &DeviceEventsTrigger;
}

HRESULT CPreviewPipeline::RegisterManagerEvents(CPreviewPipelineManagerEvents* pCall)
{
	for(UINT i= 0; 
		i < ManagerEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineManagerEvents* &event= ManagerEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			return E_INVALIDARG;
		}
	}

	ManagerEventsTrigger.Events.push_back(pCall);

	return S_OK;
}

HRESULT CPreviewPipeline::UnregisterManagerEvents(CPreviewPipelineManagerEvents* pCall)
{
	for(UINT i= 0; 
		i < ManagerEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineManagerEvents* &event= ManagerEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			ManagerEventsTrigger.Events.erase( ManagerEventsTrigger.Events.begin( ) + i);
			return S_OK;
		}
	}
	
	return E_INVALIDARG;
}

CPreviewPipelineManagerEvents* CPreviewPipeline::TriggerManagerEvent()
{
	return &ManagerEventsTrigger;
}


HRESULT CPreviewPipeline::RegisterViewerEvents(CPreviewPipelineViewerEvents* pCall)
{
	for(UINT i= 0; 
		i < ViewerEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineViewerEvents* &event= ViewerEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			return E_INVALIDARG;
		}
	}

	ViewerEventsTrigger.Events.push_back(pCall);

	return S_OK;
}

HRESULT CPreviewPipeline::UnregisterViewerEvents(CPreviewPipelineViewerEvents* pCall)
{
	for(UINT i= 0; 
		i < ViewerEventsTrigger.Events.size(); 
		i++)
	{
		const CPreviewPipelineViewerEvents* &event= ViewerEventsTrigger.Events.at(i);
		if(event==pCall)
		{
			ViewerEventsTrigger.Events.erase( ViewerEventsTrigger.Events.begin( ) + i);
			return S_OK;
		}
	}
	
	return E_INVALIDARG;
}

CPreviewPipelineViewerEvents* CPreviewPipeline::TriggerViewerEvent()
{
	return &ViewerEventsTrigger;
}



HRESULT CPreviewPipelineEngineEventsTrigger::OnD3DDeviceCreate()
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineEngineEvents* &event= Events.at(i);
		hr= event->OnD3DDeviceCreate();
	}
	return hr;
};

HRESULT CPreviewPipelineEngineEventsTrigger::OnD3DDeviceReset()
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineEngineEvents* &event= Events.at(i);
		hr= event->OnD3DDeviceReset();
	}
	return hr;
};

HRESULT CPreviewPipelineEngineEventsTrigger::OnD3DDeviceLost()
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineEngineEvents* &event= Events.at(i);
		hr= event->OnD3DDeviceLost();
	}
	return hr;
};

HRESULT CPreviewPipelineEngineEventsTrigger::OnD3DDeviceDestroy()
{
	HRESULT hr= S_OK;
	for(UINT i= 0;
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineEngineEvents* &event= Events.at(i);
		hr= event->OnD3DDeviceDestroy();
	}
	return hr;
};


HRESULT CPreviewPipelineManagerEventsTrigger::OnReload()
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineManagerEvents* &event= Events.at(i);
		hr= event->OnReload();
	}
	return hr;
};

HRESULT CPreviewPipelineManagerEventsTrigger::OnResourceAdd(IDXCCResource* pRes)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineManagerEvents* &event= Events.at(i);
		hr= event->OnResourceAdd(pRes);
	}
	return hr;
};

bool CPreviewPipelineManagerEventsTrigger::SetExclusiveMode(bool WaitToAcquire, bool PauseState)
{
	bool result= true;
	for(UINT i= 0; 
		i < Events.size();
		i++)
	{
		CPreviewPipelineManagerEvents* event= Events.at(i);
		result= result && event->SetExclusiveMode(WaitToAcquire, PauseState);
	}

	return result;
};




CPreviewPipelineManagerEvents::CPreviewPipelineManagerEvents()
{
	ExclusiveModeMutex= CreateMutex(NULL, false, NULL);
}

CPreviewPipelineManagerEvents::~CPreviewPipelineManagerEvents()
{
	CloseHandle(ExclusiveModeMutex);

}


bool CPreviewPipelineManagerEvents::SetExclusiveMode(bool WaitToAcquire, bool PauseState)
{ 
	if(PauseState == true)
	{
		return GetSingleObject(WaitToAcquire, ExclusiveModeMutex); 
	}
	else if(PauseState == false)
	{
		ReleaseMutex(ExclusiveModeMutex);
	}

	return true;
}

HRESULT CPreviewPipelineManagerEventsTrigger::OnResourceRecycle(IDXCCResource* pRes)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineManagerEvents* &event= Events.at(i);
		hr= event->OnResourceRecycle(pRes);
	}
	return hr;
};

HRESULT CPreviewPipelineManagerEventsTrigger::OnResourceRemove(IDXCCResource* pRes)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineManagerEvents* &event= Events.at(i);
		hr=event->OnResourceRemove(pRes);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnFrameChildAdded(IDXCCFrame* pParent, IDXCCFrame* pChild)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnFrameChildAdded(pParent, pChild);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnFrameChildRemoved(IDXCCFrame* pParent, IDXCCFrame* pChild) 
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnFrameChildRemoved(pParent, pChild);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnFrameMemberAdded(IDXCCFrame* pParent, IUnknown* pChild)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnFrameMemberAdded(pParent, pChild);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnFrameMemberRemoved(IDXCCFrame* pParent, IUnknown* pChild)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnFrameMemberRemoved(pParent, pChild);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshChange(IDXCCMesh* pMesh)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshChange(pMesh);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshDeclarationChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshDeclarationChange(pMesh, pOldMesh);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshTopologyChange(IDXCCMesh* pMesh, IDXCCMesh* pOldMesh)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshTopologyChange(pMesh, pOldMesh);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshVertexChange(IDXCCMesh* pMesh, UINT vertMin, UINT vertMax)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshVertexChange(pMesh, vertMin, vertMax);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshSubVertexChange(IDXCCMesh* pMesh, D3DDECLUSAGE Usage, UINT UsageIndex, UINT vertMin, UINT vertMax)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshSubVertexChange(pMesh, Usage, UsageIndex, vertMin, vertMax);
	}
	return hr;
};
HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshAttributeChange(IDXCCMesh* pMesh, UINT faceMin, UINT faceMax)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshAttributeChange(pMesh, faceMin, faceMax);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMeshMaterialChange(IDXCCMesh* pMesh, DXCCATTRIBUTE Attrib, ID3DXEffect* pOldMaterial)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMeshMaterialChange(pMesh, Attrib, pOldMaterial);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMaterialEffectChange(ID3DXEffect* pMaterial, LPD3DXEFFECT pOldEffect)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMaterialEffectChange(pMaterial, pOldEffect);
	}
	return hr;
};

HRESULT CPreviewPipelineViewerEventsTrigger::OnMaterialParameterChange(ID3DXEffect* pMaterial, D3DXHANDLE hParameter)
{
	HRESULT hr= S_OK;
	for(UINT i= 0; 
		i < Events.size() 
		&& hr == S_OK; 
		i++)
	{
		CPreviewPipelineViewerEvents* &event= Events.at(i);
		hr= event->OnMaterialParameterChange(pMaterial, hParameter);
	}
	return hr;
};





