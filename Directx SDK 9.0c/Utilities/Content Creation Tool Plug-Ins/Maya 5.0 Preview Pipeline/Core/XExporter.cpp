

#include "dxstdafx.h"

#include "d3d9.h"
#include "d3dx9.h"

#include "MayaInterface.h"
#include "XExporter.h"


#include "PreviewPipeline.h"
#include "MayaInterface.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

// Maya API
#include <maya/MSelectionList.h>

//#include <maya/MFn.h>
//#include <maya/MFnDependencyNode.h>
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlug.h>
#include <maya/MItDag.h>

#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTime.h>

#include <maya/MFnMeshData.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlugArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MSceneMessage.h>
#include <maya/MDGMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MDagMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MFnCamera.h>
#include <maya/MAnimMessage.h>

//#include <maya/MSimple.h>
//#include <maya/MEulerRotation.h>
//#include <maya/MIkSystem.h>

#include <maya/MAnimControl.h>
#include <maya/MAnimUtil.h>

#include <maya/MFnTransform.h>
#include <maya/MFnIKJoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnSubd.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnSet.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnStringData.h>

#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshFaceVertex.h>


CRITICAL_SECTION DeviceAndViewerSection; 
HANDLE DeviceCreatedEvent; 
HANDLE AddOrUpdateMutex; 
//HANDLE CreationSemaphore;
//HANDLE CreationThread;
//HANDLE UpdateThread;

CMayaPreviewPipeline g_PreviewPipeline;

DWORD*					MeshUserData::D3DPointReps= 0;
DWORD					MeshUserData::D3DPointRepsLength= 0;
VertexD3DToMayaType*	MeshUserData::pVertexD3DToMaya= 0;
DWORD					MeshUserData::VertexD3DToMayaLength= 0;
VertexMayaToD3DType*	MeshUserData::pVertexMayaToD3D= 0;
DWORD					MeshUserData::VertexMayaToD3DLength= 0;


HRESULT DirectXShader::OnPostLoad( 
	HWND hDialog, 
	LPDIRECT3DDEVICE9 pDevice,
	LPDXCCMANAGER pManager,
	LPDXCCRESOURCE pResource,//recycle me
	LPCSTR szNewEffectFilename) 
{
	CPreviewPipelineLock SceneLock;

	g_PreviewPipeline.SceneReadLock(true, SceneLock);

	bool FoundInDirtyBin= false;
	for(UINT i = 0; i < g_PreviewPipeline.DirtyShaders.size(); i++)
	{
		if(g_PreviewPipeline.DirtyShaders[i]==pResource->GetHandle())
		{
			FoundInDirtyBin=true;
			break;
		}
	}
	if(!FoundInDirtyBin)
	{
		g_PreviewPipeline.DirtyShaders.push_back(pResource->GetHandle());
	}

	g_PreviewPipeline.SceneReadUnlock(SceneLock);

	return S_OK;
}

HRESULT DirectXShader::OnClose(  
	HWND hDialog, 
 	LPDIRECT3DDEVICE9 pDevice,
 	LPDXCCMANAGER pManager,
 	LPDXCCRESOURCE pResource)
{
	SetEvent(DirectXShader::FxDialogEvent);

	return S_OK; 
}

CMayaPreviewPipeline::CMayaPreviewPipeline()
{
	new(&TagContainer) DCCTagContainer(this);
};

CMayaPreviewPipeline::~CMayaPreviewPipeline()
{
}; 

HRESULT CMayaPreviewPipeline::Synchronize_Material(DXCCHANDLE hDirtyShader)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	MObject node= Convert_DXCCResourceToDGNode(hDirtyShader);
	if(!node.isNull())
	{
		for(MItDependencyGraph FindEngine(node, MFn::kShadingEngine, MItDependencyGraph::kDownstream );
			!FindEngine.isDone();
			FindEngine.next())
		{
			MObject dgShadingEngine= FindEngine.thisNode();

			if(!dgShadingEngine.isNull())
			{



				for(MItDependencyGraph FindMesh(dgShadingEngine, MFn::kMesh, MItDependencyGraph::kUpstream);
					!FindMesh.isDone();
					FindMesh.next())
				{
					MObject dgMesh= FindMesh.thisNode();
					if(!dgMesh.isNull())
					{
						MDagPathArray PathArray;

						MDagPath::getAllPathsTo(dgMesh, PathArray);
						for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
						{
							DCCTag* tag= NULL;
							if(TagContainer.Find(PathArray[iPath], NULL, &tag))
							{
								TagContainer.MarkAsDirty(tag);
							}
						}
					}
				}


			}
		}
	}
	return hr;
}

MObject CMayaPreviewPipeline::Convert_DXCCResourceToDGNode(DXCCHANDLE matchHandle)
{
	for(MItDependencyNodes iter; !iter.isDone(); iter.next())
	{
		MFnDependencyNode depNode(iter.item());
		if(depNode.typeId() == DirectXShader::id)
		{
			int iHandle;
			DXCCHANDLE hHandle; 

			MPlug plugResource= depNode.findPlug(DirectXShader::aDXCCHandle);

			plugResource.getValue(iHandle);
			hHandle= (DXCCHANDLE)iHandle;

			if(hHandle == matchHandle)
			{
				return iter.item();
			}
		}
	}

	return MObject();
}


DXCCHANDLE CMayaPreviewPipeline::Convert_DGNodeToDXCCResource(MObject matchNode)
{
	MFnDependencyNode depNode(matchNode);
	if(depNode.typeId() == DirectXShader::id)
	{
		int iHandle;
		DXCCHANDLE hHandle; 

		MPlug plugResource= depNode.findPlug(DirectXShader::aDXCCHandle);

		plugResource.getValue(iHandle);
		hHandle= (DXCCHANDLE)iHandle;

		return hHandle;
	}

	return NULL;
}




HRESULT CDXCCSaveEffectDefaults::Set(LPDXCCMANAGER pResourceManager, /*MObject& attribParameters,*/ MObject& pxNode)
{
	if(!pResourceManager)
		return E_INVALIDARG;

	if(pxNode.isNull())
		return E_INVALIDARG;

	//if(attribParameters.isNull())
	//	return E_INVALIDARG;

	pManager= pResourceManager;
	oNode= pxNode;
	//aEffectParameters= attribParameters;
	iParameter= 0;

	return S_OK;
};

HRESULT CDXCCSaveEffectDefaults::EnumParameter(LPD3DXEFFECT pEffect, D3DXHANDLE handle, LPCSTR name)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	bool Valid= false;
	MPlug plugName;
	D3DXPARAMETER_DESC ParamDesc;
	MFnTypedAttribute aParamName;
	MFnTypedAttribute aParamValue;
	MObject oParamValue;
	MFnDependencyNode depNode(oNode);
	FLOAT fArray[16];

	if(!pManager)
		return E_UNEXPECTED;

	if(oNode.isNull())
		return E_INVALIDARG;


	MString ShortParamName( MString(DX_SHORT_FXPARAM_NAME) + iParameter);
	MString LongParamName( MString(DX_LONG_FXPARAM_NAME) + iParameter);
	MString ShortDataName( MString(DX_SHORT_FXPARAM_DATA) + iParameter);
	MString LongDataName( MString(DX_LONG_FXPARAM_DATA) + iParameter);


	plugName= depNode.findPlug(LongParamName);
	if(plugName.isNull())
	{
		MObject oParamName= aParamName.create(LongParamName, ShortParamName, MFnData::kString, MObject::kNullObj , &stat);
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

		CHECK_MSTATUS( aParamName.setHidden( true ) );

		stat = depNode.addAttribute(oParamName);
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

		plugName= depNode.findPlug(LongParamName, &stat);
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}


	hr= pEffect->GetParameterDesc(handle, &ParamDesc);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	switch(ParamDesc.Class)
	{
	case D3DXPC_SCALAR:
	case D3DXPC_VECTOR:
	case D3DXPC_MATRIX_ROWS:
	case D3DXPC_MATRIX_COLUMNS:
		{
			DXCC_ASSERT_EXCEPTIONS_BEGIN()
			{

				MFnDoubleArrayData dArrayData;
				MDoubleArray dArray;
				MObject oData;

				hr= pEffect->GetFloatArray(handle, fArray, 16);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);

				oData= dArrayData.create(&stat);
				dArray= dArrayData.array();
				switch(ParamDesc.Class)
				{
					case D3DXPC_SCALAR:
						{
							dArray.setLength(1);
							dArray.set(fArray[0], 0);
						}
						break;
					case D3DXPC_VECTOR:
						{
							dArray.setLength(4);
							for(UINT i= 0; i < 4; i++)
								dArray.set(fArray[i], i);
						}
						break;				
					case D3DXPC_MATRIX_ROWS:
					case D3DXPC_MATRIX_COLUMNS:
						{
							dArray.setLength(16);
							for(UINT i= 0; i < 16; i++)
								dArray.set(fArray[i], i);
						}
						break;	
				};

				MPlug plugData= depNode.findPlug(LongDataName);
				if(plugData.isNull())
				{
					oParamValue= aParamValue.create(LongDataName, ShortDataName, MFnData::Type::kDoubleArray, MObject::kNullObj, &stat);
					if (!stat)
					{
						stat.perror("Failed to create Parameter Attribute");
						DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
					}
					CHECK_MSTATUS( aParamValue.setHidden( true ) );
					depNode.addAttribute(oParamValue);
					plugData= depNode.findPlug(LongDataName);
				}
				plugData.setValue(oData);
				Valid= true;


			}
			DXCC_ASSERT_EXCEPTIONS_END()
		}
		break;
	case D3DXPC_OBJECT:
		switch(ParamDesc.Type)
		{
		case D3DXPT_STRING:
			{

				DXCC_ASSERT_EXCEPTIONS_BEGIN()
				{

					MFnStringData StringData;
					MObject oData;
					LPCSTR pStr= NULL;
		
					hr= pEffect->GetString(handle, &pStr);
					if(DXCC_FAILED(hr))
						DXCC_GOTO_EXIT(e_Exit, TRUE);
					
					if(!pStr)
						pStr= "";

					oData= StringData.create(MString(pStr), &stat);
					if (!stat)
					{
						stat.perror("Failed to create StringData");
						DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
					}

					MPlug plugData= depNode.findPlug(LongDataName);
					if(plugData.isNull())
					{
						oParamValue= aParamValue.create(LongDataName, ShortDataName, MFnData::Type::kString, MObject::kNullObj, &stat);
						if (!stat)
						{
							stat.perror("Failed to create Parameter Attribute");
							DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
						}
						CHECK_MSTATUS( aParamValue.setHidden( true ) );
						depNode.addAttribute(oParamValue);
						plugData= depNode.findPlug(LongDataName);
					}
					plugData.setValue(oData);
					Valid= true;

				}
				DXCC_ASSERT_EXCEPTIONS_END()

			}
			break;
		case D3DXPT_TEXTURE:
		case D3DXPT_TEXTURE1D:
		case D3DXPT_TEXTURE2D:
		case D3DXPT_TEXTURE3D:
		case D3DXPT_TEXTURECUBE:
			{
				DXCC_ASSERT_EXCEPTIONS_BEGIN()
				{

					MFnStringData StringData;
					MObject oData;
					LPDXCCRESOURCE pRes= NULL;
					LPDIRECT3DBASETEXTURE9 pTexture= NULL;

					hr= pEffect->GetTexture(handle, &pTexture);
					if(DXCC_FAILED(hr))
						DXCC_GOTO_EXIT(e_Exit, TRUE);
					
					if(pTexture 
						&& DXCC_SUCCEEDED(pManager->FindResourceByPointer(pTexture, NULL, &pRes)))
					{
						MPlug plugData= depNode.findPlug(LongDataName);
						LPCSTR pStr= pRes->GetResourcePath();

						if(!pStr)
							pStr= "";

						oData= StringData.create(MString(pStr), &stat);
						if (!stat)
						{
							stat.perror("Failed to create StringData");
							DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
						}

						if(plugData.isNull())
						{
							oParamValue= aParamValue.create(LongDataName, ShortDataName, MFnData::Type::kString, MObject::kNullObj, &stat);
							if (!stat)
							{
								stat.perror("Failed to create Parameter Attribute");
								DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
							}
							CHECK_MSTATUS( aParamValue.setHidden( true ) );
							depNode.addAttribute(oParamValue);
							plugData= depNode.findPlug(LongDataName);
						}
						plugData.setValue(oData);
						Valid= true;
					}
				}
				DXCC_ASSERT_EXCEPTIONS_END()
			}
			break;
		default:
			break;
		};
		break;
		//get a string
		break;
	case D3DXPC_STRUCT:
		break;
	default:
		break;
	};


	if(Valid)
	{
		plugName.setValue(name);
		iParameter++;
	}


	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:	


	return hr;
}

bool 
DAGIsVisible(
	MDagPath& dagPath, 
	MStatus* status)
{
	bool result= true;
	MDagPath path= dagPath;
	MFnDagNode dagNode(path);

	if(path.node().hasFn(MFn::kManipulator3D))
		return FALSE;

	do
	{
		if( !NodeIsVisible( path.node(), status))
			return false;
	} while( path.pop());

	return true;


	return result;
}

bool 
NodeIsVisible(
	MObject& node,
	MStatus* status)
{
	HRESULT	hr= S_OK;
	MStatus TempStatus;

	if(!status)
		status= &TempStatus;
	
		
	MFnDagNode DagNode(node); 

	bool	bIsVisible= false;

	bool	bPlugVisibility= true, 
			bPlugLodVisibility= true, 
			bPlugOverrideEnabled= false, 
			bPlugOverrideVisibility= true,
			bPlugIntermediate= false,
			bPlugUnderworld= false;

	MPlug	mpVisibility,
			mpLodVisibility,
			mpOverrideEnabled, 
			mpOverrideVisibility;
			//mpIntermediate;

	//VISIBILITY
	mpVisibility = DagNode.findPlug("visibility", status); // intermediateObject //template (these too?)
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	*status = mpVisibility.getValue(bPlugVisibility);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	//LOD VISIBILITY
	mpLodVisibility = DagNode.findPlug("lodVisibility", status);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	*status = mpLodVisibility.getValue(bPlugLodVisibility);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	//Intermediate Object
	//mpIntermediate = DagNode.findPlug("intermediateObject", status);
	//if(!*status)
	//	DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	//*status = mpIntermediate.getValue(bPlugIntermediate);
	//if(!*status)
	//	DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	bPlugIntermediate= DagNode.isIntermediateObject();
	bPlugUnderworld= DagNode.inUnderWorld();

	//OVERRIDE ENABLED
	mpOverrideEnabled = DagNode.findPlug("overrideEnabled", status);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	*status = mpOverrideEnabled.getValue(bPlugOverrideEnabled);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	//OVERRIDE VISIBILITY
	mpOverrideVisibility = DagNode.findPlug("overrideVisibility", status);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	*status = mpOverrideVisibility.getValue(bPlugOverrideVisibility);
	if(!*status)
		DXCC_STATUS_EXIT(*status, MS::kFailure, e_Exit, FALSE);

	bIsVisible= bPlugVisibility
		&& bPlugLodVisibility 
		&& !bPlugIntermediate
		&& !bPlugUnderworld 
		&& !(bPlugOverrideEnabled && !bPlugOverrideVisibility);

e_Exit:

	return bIsVisible;	
};

UINT MayaGetFPS()
{
	UINT nFPS;	// num frames per second

	// calculate the frames per second
	switch(MTime::uiUnit()) 
	{
		case MTime::kSeconds:		// 1 fps
			nFPS = 1;
			break;
		case MTime::kMilliseconds:	// 1000 fps
			nFPS = 1000;
			break;
		case MTime::kGames:			// 15 fps
			nFPS = 15;
			break;
		case MTime::kFilm:			// 24 fps
			nFPS = 24;
			break;
		case MTime::kPALFrame:		// 25 fps
			nFPS = 25;
			break;
		case MTime::kNTSCFrame:		// 30 fps
			nFPS = 30;
			break;
		case MTime::kShowScan:		// 48 fps
			nFPS = 48;
			break;
		case MTime::kPALField:		// 50 fps
			nFPS = 50;
			break;
		case MTime::kNTSCField:		// 60 fps
			nFPS = 60;
			break;
		default:
			nFPS = 1;
			break;
	};

	return nFPS;
}

void ConvertLocalMatrix(D3DXMATRIX& ToD3D, const MFloatMatrix& FromMaya)
{
	for (UINT i = 0; i < 4; i++)
	{
		for (UINT j = 0; j < 4; j++)
		{
			ToD3D(i, j)= (FLOAT)FromMaya(i, j);
		}
	}
}

void ConvertLocalMatrix(D3DXMATRIX& ToD3D, const MMatrix& FromMaya)
{
	MFloatMatrix FromMayaFloat(FromMaya.matrix);
	ConvertLocalMatrix(ToD3D, FromMayaFloat);
}

void ConvertWorldMatrix(D3DXMATRIX& ToD3D, const MFloatMatrix& FromMaya)
{
	D3DXMATRIX Local;
	ConvertLocalMatrix(Local, FromMaya);

	D3DXMATRIX RtoL;
	D3DXMatrixScaling(&RtoL, 1.0f, 1.0f, -1.0f);
	D3DXMatrixMultiply(&ToD3D, &Local, &RtoL);
}

void ConvertWorldMatrix(D3DXMATRIX& ToD3D, const MMatrix& FromMaya)
{
	MFloatMatrix FromMayaFloat(FromMaya.matrix);
	ConvertWorldMatrix(ToD3D, FromMayaFloat);
}

bool DCCTagContainer::Add(MObject& objAllPathsTo )
{
	MDagPathArray PathArray;

	if(objAllPathsTo.isNull())
		return false;

	MDagPath::getAllPathsTo(objAllPathsTo, PathArray);
	for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
	{
		Add(PathArray[iPath], NULL, NULL);
	}

	return true;
}

bool DCCTagContainer::Add(MDagPath& path, UINT *pIndex, DCCTag** ppTag )
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()
	//GetSingleObject(true, AddOrUpdateMutex);

	if(Find(path, pIndex, ppTag))
		return false;

	if(!DAGIsVisible(path, NULL))
		return false;

	DCCTag* pTag= new DCCTag(this, path);
	if(!pTag)
		return false;

	GlobalArray.push_back(pTag);

	MarkAsDirty(Size()-1);



	if(pIndex)
	{
		*pIndex= Size()-1;
	}

	if(ppTag)
	{
		*ppTag= pTag;
	}

	DXCC_ASSERT_EXCEPTIONS_END()

	return true;
}

DCCTag::DCCTag(DCCTagContainer* Owner, MDagPath& path)
{
	pContainer= Owner;
	Dirty= false;
	Initialized= false;
	PathCallback= NULL;
	Resource= NULL;
	Interface= NULL;

	SetPath(path);
	pContainer->pPreview->AccessManager()->CreateResource(NULL, IID_NULL, true, &Resource);

	DXCC_DPFA_REPORT("TAG: %s [%s]",  path.fullPathName ().asChar(), path.node().apiTypeStr());
};

DCCTag::~DCCTag()
{
	if(PathCallback != NULL)
		MMessage::removeCallback(PathCallback);
	
	if(Resource)
	{
		if(Interface)
		{
			if(Resource->GetIID() == IID_IDXCCFrame)
			{
				LPDXCCFRAME pFrame= (LPDXCCFRAME)Interface;

				pFrame->SetParent(NULL);

				while(pFrame->NumChildren() > 0)
				{
					pFrame->RemoveChild(pFrame->NumChildren()-1);
				}
				
				while(pFrame->NumMembers() > 0)
				{
					pFrame->RemoveMember(pFrame->NumMembers()-1);
				}
			}
			else if(Resource->GetIID() == IID_IDXCCMesh)
			{
				LPDXCCMESH pMesh= (LPDXCCMESH)Interface;

				if(pMesh->NumBones() > 0)
					pMesh->CreateBones(0);
			}

			DXCC_RELEASE(Interface);



		}

		if(Resource->DetachObject())
		{
			if(pContainer
				&& pContainer->pPreview
				&& pContainer->pPreview->AccessManager())
				pContainer->pPreview->AccessManager()->RemoveResource(Resource->GetHandle());
		}

		DXCC_RELEASE(Resource);
	}


	
	DXCC_RELEASE(Resource);
	DXCC_RELEASE(Interface);
};


void DCCTag::SetPath(MDagPath& newPath)
{
	Path.set(newPath);
}

MDagPath& DCCTag::GetPath()
{
	if(!Path.isValid())
		Path= MDagPath();

	return Path;
}

bool DCCTag::IsInitialized()
{
	return (Initialized && IsValid() && Resource && Interface);
}

bool DCCTag::IsValid()
{
	return (Path.isValid());
};

void DCCTagContainer::EraseInvalidTags()
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	for(UINT i= 0; 
		i < Size(); 
		i++)
	{
		DCCTag* pTag= NULL;
		Get(i, pTag);

		if(!pTag->IsValid())
		{
			Erase(i);
			i--;
		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()

	return;
}


bool DCCTagContainer::Get(UINT index, DCCTag* &pTag)
{
	if(index < Size())
	{
		pTag= GlobalArray[index];
		return true;
	}
	else
		return false;
}

bool DCCTagContainer::GetFromDirtyList(UINT index, DCCTag* &pTag)
{
	if(index < Size())
	{
		pTag= DirtyArray[index];
		return true;
	}
	else
		return false;
}

UINT DCCTagContainer::SizeFromDirtyList()
{
	return (UINT)DirtyArray.size();
}

bool DCCTagContainer::FindFromDirtyList(DCCTag* pFind, UINT *pDirtyListIndex)
{
	for(UINT i= 0; 
		i < SizeFromDirtyList(); 
		i++)
	{
		DCCTag* pTag= NULL;
		GetFromDirtyList(i, pTag);

		if(pTag == pFind)
		{
			*pDirtyListIndex= i;
			return true;
		}
	}

	return false;
}

bool DCCTagContainer::Find(DCCTag* pFind, UINT *pIndex)
{
	for(UINT i= 0; 
		i < Size(); 
		i++)
	{
		DCCTag* pTag= NULL;
		Get(i, pTag);

		if(pTag == pFind)
		{
			*pIndex= i;
			return true;
		}
	}

	return false;
}

bool DCCTagContainer::Find(MDagPath& path, UINT *pIndex, DCCTag** ppTag)
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	for(UINT i= 0; 
		i < Size(); 
		i++)
	{
		DCCTag* pTag= NULL;
		Get(i, pTag);

		if(pTag->GetPath() == path)
		{
			if(pIndex)
				*pIndex= i;
			if(ppTag)
				*ppTag= pTag;

			return true;
		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()

	return false;
}


bool DCCTagContainer::FindFromDirtyList(MDagPath& path, UINT *pIndex, DCCTag** ppTag)
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	for(UINT i= 0; 
		i < SizeFromDirtyList(); 
		i++)
	{
		DCCTag* pTag= NULL;
		GetFromDirtyList(i, pTag);

		if(pTag->GetPath() == path)
		{
			if(pIndex)
				*pIndex= i;
			if(ppTag)
				*ppTag= pTag;

			return true;
		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()

	return false;
}


UINT DCCTagContainer::Size()
{
	return (UINT)GlobalArray.size();
}


void DCCTagContainer::EraseAll()
{
	if(GlobalArray.size() > 0)
	{
		for(UINT i= 0; i < GlobalArray.size(); i++)
		{
			DCCTag* Element= GlobalArray[i];
			delete Element;
		}
		GlobalArray.clear();
	}
	DirtyArray.clear();
}

bool DCCTagContainer::Erase_SLOW(DCCTag* tag)
{
	UINT i= (UINT)-1;
	if(Find(NULL, &i))
	{
		return Erase(i);
	}
	return false;
}

bool DCCTagContainer::Erase(UINT index)
{
	if(index < GlobalArray.size())
	{
		DCCTag* &Element= GlobalArray[index];

		MarkAsClean_SLOW(Element);

		delete Element;

		GlobalArray.erase(GlobalArray.begin()+index);

		return true;
	}
	return false;
}

HRESULT	
CMayaPreviewPipeline::Scene_Export(
			const char* file,		// save file object
			const char* options,	// options string
			MPxFileTranslator::FileAccessMode mode)
{
 	HRESULT hr = S_OK;
	MStatus stat = MS::kSuccess;
	CPreviewPipelineLock SceneLock;

	SceneWriteLock(true, SceneLock);

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	DXCC_DPF(TEXT("Maya XFile Exporter"));
	DXCC_DPF(TEXT("Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved."));
	DXCC_DPFA("Exporting to \"%s\"...", file);

#ifdef _XBOX_
	Scene_Deregister();
	Scene_Register();
#endif

	hr= DXCCSaveScene(
			file, 
			D3DXF_FILEFORMAT_TEXT,//D3DXF_FILEFORMAT_BINARY,//
			AccessManager(), 
			AccessRoot(), 
			NULL, 
			NULL, 
			NULL);
	if (DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:

	SceneWriteUnlock(SceneLock);

	return hr;
}

//HRESULT	
//CMayaPreviewPipeline::Export_Options(
//			const char* file,		// save file object
//			const char* options,	// options string
//			MPxFileTranslator::FileAccessMode mode)
//{
//		//OPTIONS INITIALIZE
//		//Options.Hierarchy= TRUE;
//		//Options.HierarchyCentered= FALSE;
//		//Options.HierarchyScale.x= 1.0f;
//		//Options.HierarchyScale.y= 1.0f;
//		//Options.HierarchyScale.z= 1.0f;
//
//		//Options.Mesh= TRUE;
//		//Options.MeshSkinned= TRUE;
//
//		//Options.Material= TRUE;
//		//Options.MaterialFX= TRUE; 
//		//Options.MaterialUnqualifyPath= FALSE; 
//
//		//Options.Animation= TRUE;
//		//Options.AnimationController= NULL;
//		//strcpy(Options.AnimationName, "default");
//		//Options.AnimationKeysOnly= FALSE;
//		//Options.AnimationPlaybackType= D3DXPLAY_ONCE;
//		//Options.AnimationPlayFPS= Options.AnimationSampleFPS= MayaGetFPS();
//		//Options.AnimationStartTime= MAnimControl::minTime().value(); 
//		//Options.AnimationEndTime= (DOUBLE)MAnimControl::maxTime().value();
//
//		//Options.OutputFormat= D3DXF_FILEFORMAT_BINARY;//D3DXF_FILEFORMAT_TEXT;//
//		//strcpy(Options.OutputSaveAs, file);
//	}//END//OPTIONS INITIALIZE
//
//
//
//	//hr= DXCCShowSaveDialog(&Options);
//	//if (DXCC_FAILED(hr))
//	//	DXCC_GOTO_EXIT(e_Exit, TRUE);
//}




void CMayaPreviewPipeline::Scene_Update(bool bForceUpdate)
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()


	DXCCTimeStamp TimeStamp;
	CPreviewPipelineLock SceneLock;	

	if(!bForceUpdate)
	{
		if(TimeStamp.RoughlyEqual(TagContainer.TimeStamp))
			return;
	}

	//always update the camera...when enough time has passed
	Synchronize_PerspectiveCamera();

	if(	TagContainer.SizeFromDirtyList() > 0 || 
		DirtyShaders.size() > 0)
	{
		SceneWriteLock(true, SceneLock);
		MGlobal::displayInfo("DXCC BEGINNING TO COPY SCENE");

		//update materials since dirty materials will make dirty meshes
		Scene_Update_Materials(bForceUpdate, TimeStamp);

		//remove any invisible nodes which are dirty
		Scene_Update_Visibility(bForceUpdate, TimeStamp);

		//update the frames since meshes rely on them
		Scene_Update_Tags(bForceUpdate, TimeStamp, MFn::kTransform);

		//update meshes 
		Scene_Update_Tags(bForceUpdate, TimeStamp, MFn::kMesh);

		MGlobal::displayInfo("DXCC FINISHED COPYING SCENE");
		SceneWriteUnlock(SceneLock);
	}

	//always update the timeso that we know it's been long enough 
	//between updates ....NOTE this is at the end to garantee usable time 
	//rather than maintaining interactive rates
	//TODO: track and avg time of updates and and throttle the values in RoughlyEquals 
	//instead to smooth out interaction.
	TimeStamp.AcquireTime();

	DXCC_ASSERT_EXCEPTIONS_END()

	return;
}


void CMayaPreviewPipeline::Scene_Update_Animation()
{
	HRESULT hr= S_OK;

	for(UINT iDirty= 0; 
		iDirty < TagContainer.Size();
		iDirty++)
	{
		DCCTag* pTag= NULL;

		if(!TagContainer.Get(iDirty, pTag))
			continue;

		//for realtime attempts at tracking animation.//not using this approach
		//if(pTag->DirtyAnimationFlag)
		//we're just going to regather everything regardless of being dirty
		//pTag->DirtyAnimationFlag= false;

		Frame_GatherAnimation(pTag);//ok if this fails

	}

//e_Exit:
	return;
}

void CMayaPreviewPipeline::Scene_Update_Visibility(bool bForceUpdate, DXCCTimeStamp& TimeStamp)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	UINT CleanedCount= 0;
	UINT DirtyCount= TagContainer.SizeFromDirtyList();
	for(UINT i= 0; 
		i < DirtyCount;
		i++)
	{
		DCCTag* pTag= NULL;

		UINT iDirty= i-CleanedCount;

		if(!TagContainer.GetFromDirtyList(iDirty, pTag))
			continue;

		if(!bForceUpdate)
		{
			if(TimeStamp.RoughlyEqual(pTag->TimeStamp))
				continue;
		}

		if(pTag->IsInitialized())
		{
			MDagPath path= pTag->GetPath();

			if(!DAGIsVisible(path, &stat))
			{
				Tag_Remove(path.node());
				CleanedCount++;
			}
		}
	}
}

void CMayaPreviewPipeline::Scene_Update_Tags(bool bForceUpdate, DXCCTimeStamp& TimeStamp, MFn::Type type)
{
	UINT CleanedCount= 0;
	UINT DirtyCount= TagContainer.SizeFromDirtyList();
	for(UINT i= 0; 
		i < DirtyCount;
		i++)
	{
		DCCTag* pTag= NULL;

		UINT iDirty= i-CleanedCount;

		if(!TagContainer.GetFromDirtyList(iDirty, pTag))
			continue;

		//skip tags that to recent unless we are forced to do so
		if(!bForceUpdate)
		{
			if(TimeStamp.RoughlyEqual(pTag->TimeStamp))
				continue;
		}
	
		//skip tags that maya throws exception for when i try to initialize
		if(pTag->CanInitialize())		
		{
			bool InitializeMe= !pTag->IsInitialized();
			MDagPath dagTag= pTag->GetPath();

			//skip tags that are not of the desired type
			switch(type)
			{
			case MFn::kMesh:
				if(!dagTag.hasFn(MFn::kMesh))
					continue;
				break;
			case MFn::kTransform:
				if(dagTag.hasFn(MFn::kMesh))//in future kShape
					continue;
				break;
			};
			
			if(InitializeMe)
				DXCC_DPFA_REPORT("INITIALIZE TAG: %s", dagTag.fullPathName().asChar());


			Synchronize_Tag(pTag);

			//parent because we arent in the tree yet!
			//rechild because we cant garantee the ordering from maya.
			//EX FOR RECHILD: the child may be added and the parent may not exist at that time
			//so it gets added the the global parent.
			//then the parent gets added and if you dont rechild the hierarchy is wrong.
			if(InitializeMe)
			{
				MDagPath dagParent= dagTag;
				dagParent.pop();

				for(UINT iChild= 0; iChild < dagTag.childCount(); iChild++)
				{
					MDagPath dagChild= dagTag;
					dagChild.push(dagTag.child(iChild));

					Tag_Reparent(MDagMessage::DagMessage::kParentAdded,dagChild, dagTag, false);
				}
				Tag_Reparent(MDagMessage::DagMessage::kParentAdded,dagTag, dagParent, false);

				pTag->SetInitialized(true);
			}

			//We cleaned the tag!
			TagContainer.MarkAsClean(iDirty);

			CleanedCount++;
		}
	}
}

void CMayaPreviewPipeline::Scene_Update_Materials(bool bForceUpdate, DXCCTimeStamp& TimeStamp)
{
	if(DirtyShaders.size() > 0)
	{

		for(UINT iDirty = 0; iDirty < DirtyShaders.size(); iDirty++)
		{
			//materials should respond quickly so we are not throttling them by RoughlyEquals

			Synchronize_Material(DirtyShaders[iDirty]);
		}
		DirtyShaders.clear();
	}
}

bool DCCTag::CanInitialize()
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;
	MDagPath TagDP= GetPath();
	MFnDependencyNode depNode(TagDP.node());

	if(TagDP.node().hasFn(MFn::kMesh))
	{
		try
		{
			MFnMesh mesh(TagDP.node(), &stat);
			if(MAYA_FAILED(stat))//don't place hooks until this stops 'passing'
				return false;
		}
		catch (...)
		{
			return false;
		}
	}
	else if(TagDP.node().hasFn(MFn::kTransform))
	{
		try
		{
			MFnTransform transform(TagDP.node(), &stat);
			if(MAYA_FAILED(stat))//don't place hooks until this stops 'passing'
				return false;
		}
		catch (...)
		{
			return false;
		}
	}

	return true;
}


bool DCCTag::SetInitialized(bool bInitializeNow)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	if(IsInitialized())
	{
		if(bInitializeNow)
		{
			return true;
		}
		else
		{
			MMessage::removeCallback(PathCallback);
			PathCallback=0;
			Initialized= false;
			return false;
		}
	}
	else
	{
		if(!bInitializeNow)
		{
			return true;
		}
		else if( CanInitialize() )
		{
			Initialized= true;

			return true;
		}
	}

	return false;
}

DXCCTimeStamp::DXCCTimeStamp() 
{
	AcquireTime();
} 

void 
DXCCTimeStamp::AcquireTime()
{
	_ftime(&Coarse);
	Fine= clock();
}

bool 
DXCCTimeStamp::RoughlyEqual(DXCCTimeStamp &test)
{
	if(this->Coarse.time == test.Coarse.time)
	{
		float micro= test.Coarse.millitm - this->Coarse.millitm;
		if(micro <= DX_UPDATE_FREQUENCY)
		{
			return true;
		}
	}
	return false;
}


void CMayaPreviewPipeline::UI_SetPreviewState(bool bPreview)
{
	MStatus stat= MS::kSuccess;
	if(bPreview == false && CallbackId_PreviewArray.length() > 0)//NOT PREVIEWING
	{
		MMessage::removeCallbacks(CallbackId_PreviewArray); 	
		CallbackId_PreviewArray.clear();
	}
	else if(bPreview == true && CallbackId_PreviewArray.length() == 0)//PREVIEWING
	{
		CallbackId_PreviewArray.append(MSceneMessage::addCallback( MSceneMessage::Message::kBeforeOpen, CMayaPreviewPipeline::Callback_ScenePreload, NULL, &stat));
		CallbackId_PreviewArray.append(MSceneMessage::addCallback( MSceneMessage::Message::kBeforeSave, CMayaPreviewPipeline::Callback_SceneSave, NULL, &stat));
		CallbackId_PreviewArray.append(MEventMessage::addEventCallback("idle", CMayaPreviewPipeline::Callback_SceneIdle, NULL, &stat));
		CallbackId_PreviewArray.append(MDGMessage::addNodeAddedCallback( CMayaPreviewPipeline::Callback_NodeAdded, kDefaultNodeType, NULL, &stat)); 
		CallbackId_PreviewArray.append(MDGMessage::addNodeRemovedCallback( CMayaPreviewPipeline::Callback_NodeRemoved, kDefaultNodeType, NULL, &stat)); 
		//CallbackId_PreviewArray.append(MAnimMessage::addAnimCurveEditedCallback( CMayaPreviewPipeline::Callback_AnimationEdit, NULL, &stat)); 
	}
}

bool CMayaPreviewPipeline::UI_GetPreviewState()
{	
	int state= 0;
	MGlobal::executeCommand ("DirectX_GetPreviewState()",  state);
	//MGlobal::executeCommand ("menuItem -q -checkBox  $g_dxPreviewToggle;",  state);

	return (bool)(state==0?false:true);
}

bool CMayaPreviewPipeline::UI_GetSkinState()
{	
	int state= 0;
	MGlobal::executeCommand ("DirectX_GetSkinState()",  state);
	//MGlobal::executeCommand ("menuItem -q -checkBox  $g_dxSkinToggle;",  state);

	return (bool)(state==0?false:true);
}


HRESULT CMayaPreviewPipeline::Create()
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;
	MItDag FindRoot;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()
	
	hr= CPreviewPipeline::Create();
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	//-----------------------INITIALIZATION ------------------------//
	FindRoot.getPath(MayaRoot);

	DxRoot.Interface= AccessRoot();
	DxRoot.Interface->AddRef();

	(*(LPD3DXMATRIX)(void*)AccessRoot()->GetLocalMatrix())(2,2)= -1.0f;
	(*(LPD3DXMATRIX)(void*)AccessRoot()->GetWorldMatrix())(2,2)= -1.0f;


	hr= AccessManager()->FindResourceByPointer(DxRoot.Interface, NULL, &DxRoot.Resource );
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	AddOrUpdateMutex= CreateMutexA(NULL, FALSE, "DXCCPreviewPipeline_MayaInterface_AddOrUpdate");
	DeviceCreatedEvent= CreateEventA(NULL, FALSE, FALSE, "DXCCPreviewPipeline_DeviceCreated");
	InitializeCriticalSection(&DeviceAndViewerSection);



	g_Engine.Create(this);
	SetEngine(&g_Engine);

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}

void CMayaPreviewPipeline::Scene_Register()
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	CPreviewPipelineLock SceneLock;
	SceneWriteLock(true, SceneLock);

	MStatus stat= MS::kSuccess;

	UI_SetPreviewState(UI_GetPreviewState());

	for(MItDependencyNodes iter; !iter.isDone(); iter.next())
	{
		Tag_Add( iter.item() );
	}

	Scene_Update(true);

	SceneWriteUnlock(SceneLock);
	DXCC_ASSERT_EXCEPTIONS_END()
}

HRESULT CMayaPreviewPipeline::Destroy()
{
	CPreviewPipelineLock SceneLock;
	SceneWriteLock(true, SceneLock);

	Scene_Deregister();
	
	CPreviewPipeline::Destroy();

	g_Engine.Destroy();



	MMessage::removeCallback( CallbackId_Exiting);
	CallbackId_Exiting=0;
	MMessage::removeCallback( CallbackId_AfterOpen);
	CallbackId_AfterOpen=0;
	MMessage::removeCallbacks(CallbackId_PreviewArray);
	MMessage::removeCallbacks(CallbackId_NodeArray);


	CloseHandle(AddOrUpdateMutex);
	AddOrUpdateMutex= NULL;
	CloseHandle(DeviceCreatedEvent);
	DeviceCreatedEvent= NULL;
	DeleteCriticalSection(&DeviceAndViewerSection);
	//DeviceAndViewerSection= NULL;

	SceneWriteUnlock(SceneLock);

	return S_OK;
}

void CMayaPreviewPipeline::Scene_Deregister()
{
	DXCC_ASSERT_EXCEPTIONS_BEGIN()
	LPDXCCFRAME pRoot= NULL;
	CPreviewPipelineLock SceneLock;
	SceneWriteLock(true, SceneLock);

	UI_SetPreviewState(false);

	pRoot= AccessRoot();
	pRoot->AddRef();

	while(pRoot->NumChildren() > 0)
	{
		pRoot->RemoveChild(0);
	}

	while(pRoot->NumMembers() > 0)
	{
		pRoot->RemoveMember(0);
	}

	DXCC_RELEASE(pRoot);

	TagContainer.EraseAll();

	MMessage::removeCallbacks(CallbackId_NodeArray);

	SceneWriteUnlock(SceneLock);
	DXCC_ASSERT_EXCEPTIONS_END()
}



HRESULT CMayaPreviewPipeline::Tag_Remove( MObject & node )
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MObject oAttr;
	int cid;	
	MDagPathArray PathArray;
	MFnDependencyNode depNode(node);

	MPlug plugNameCallback= depNode.findPlug(DX_LONG_NAME_CALLBACKID);
	if(!plugNameCallback.isNull())
	{
		stat= plugNameCallback.getValue(cid);
		MMessage::removeCallback((MCallbackId)cid);
	}

	MPlug plugAttributeCallback= depNode.findPlug(DX_LONG_ATTRIB_CALLBACKID);
	if(!plugAttributeCallback.isNull())
	{
		stat= plugAttributeCallback.getValue(cid);
		MMessage::removeCallback((MCallbackId)cid);
	}

	//TagContainer.EraseInvalidTags();

	stat= MDagPath::getAllPathsTo(node, PathArray);
	for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
	{
		UINT iRemove;
	
		MItDag itDag;
		for (itDag.reset(PathArray[iPath]); !itDag.isDone(); itDag.next())
		{
			UINT iRemove;
			MDagPath leaf;
			itDag.getPath(leaf);

			MFnDagNode leafDN(leaf);
			if(TagContainer.Find(leaf, &iRemove, NULL))
			{
				DXCC_DPFA_REPORT("Erase NODE: %s", leaf.fullPathName().asChar());
				if(!TagContainer.Erase(iRemove))
					DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);	
			}

		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}


HRESULT CMayaPreviewPipeline::Tag_Add( MObject & node )
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;
	MFnNumericAttribute attr;
	MCallbackId cid;
	MPlug plugNameCallback;
	MPlug plugAttributeCallback;
	const char* typeStr= NULL;
	const char* nameStr= NULL;


	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MFnDependencyNode depNode(node);

	if(node.hasFn(MFn::kManipulator3D)
	//|| node.hasFn(MFn::kGroundPlane)
	|| node.hasFn(MFn::kCamera)
	)
		return hr;


	typeStr= node.apiTypeStr();
	nameStr= depNode.name().asChar();

	if(node.hasFn(MFn::kDagNode)
		//||
		//node.hasFn(MFn::kMesh)|| 
		//node.hasFn(MFn::kTransform)
		)
	{
		if(TagContainer.Add(node))
		{
			cid= MNodeMessage::addNameChangedCallback( node, CMayaPreviewPipeline::Callback_NodeNameChange, NULL, &stat);
			CallbackId_NodeArray.append(cid);
			plugNameCallback= depNode.findPlug(DX_LONG_NAME_CALLBACKID);
			if(plugNameCallback.isNull())
			{
				MObject oAttr= attr.create(DX_LONG_NAME_CALLBACKID, DX_SHORT_NAME_CALLBACKID, MFnNumericData::Type::kInt, 0, &stat);
				CHECK_MSTATUS( attr.setHidden( true ) );
				CHECK_MSTATUS( attr.setStorable( false ) );
				stat= depNode.addAttribute(oAttr, MFnDependencyNode::MAttrClass::kLocalDynamicAttr);
			}
			plugNameCallback.setValue((INT)cid);


			cid= MNodeMessage::addAttributeChangedCallback( node, CMayaPreviewPipeline::Callback_NodeAttributeChanged, NULL, &stat);
			CallbackId_NodeArray.append(cid);
			plugAttributeCallback= depNode.findPlug(DX_LONG_ATTRIB_CALLBACKID);
			if(plugAttributeCallback.isNull())
			{
				MObject oAttr= attr.create(DX_LONG_ATTRIB_CALLBACKID, DX_SHORT_ATTRIB_CALLBACKID, MFnNumericData::Type::kInt, 0, &stat);
				CHECK_MSTATUS( attr.setHidden( true ) );
				CHECK_MSTATUS( attr.setStorable( false ) );
				stat= depNode.addAttribute(oAttr, MFnDependencyNode::MAttrClass::kLocalDynamicAttr);
			}
			plugAttributeCallback.setValue((INT)cid);
		}
	}


	DXCC_ASSERT_EXCEPTIONS_END()

//e_Exit:
	return hr;
}

HRESULT CMayaPreviewPipeline::Tag_Rename( MObject & node)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MFnDependencyNode depNode(node);

	MDagPathArray PathArray;
	stat= MDagPath::getAllPathsTo(node, PathArray);
	for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
	{
		DCCTag* tag= NULL;
		
		if(TagContainer.Find(PathArray[iPath], NULL, &tag)
			&& tag->Resource != NULL)
		{
			DXCC_DPFA_REPORT("NAME: %s",  PathArray[iPath].fullPathName ().asChar());
			
			hr= tag->Resource->SetName(MakeNameExportable(tag->GetPath().partialPathName(&stat)).asChar());
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);
		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}

HRESULT CMayaPreviewPipeline::Tag_Reparent( MDagMessage::DagMessage dagMsg, MDagPath &child, MDagPath &parent, bool bFixInstances)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MFnDependencyNode depNode(child.node());

	DCCTag* ChildTag=NULL;
	DCCTag* ParentTag=NULL;
	char* Msg= "";

	switch(dagMsg)
	{
	case MDagMessage::DagMessage::kParentAdded:
		Msg="ParentAdded";
		break;
	case MDagMessage::DagMessage::kChildAdded:
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);	
		//Msg="ChildAdded";
		//break;
	case MDagMessage::DagMessage::kParentRemoved:
		Msg="ParentRemoved";
		break;
	case MDagMessage::DagMessage::kChildRemoved:
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);	
		//Msg="ChildRemoved";
		//break;
	};


	if(bFixInstances)
	{
		if(dagMsg == MDagMessage::DagMessage::kParentAdded)
		{
			MItDag itDag;
			for (itDag.reset(child, MItDag::kBreadthFirst); !itDag.isDone() && (itDag.depth() <= 1) ; itDag.next())
			{
				MDagPath leaf;
				itDag.getPath(leaf);
				
				if(leaf.isInstanced())
				{
					TagContainer.Add(leaf, NULL, NULL);//will fail if already exists
				}
			}
		}
		else if(dagMsg == MDagMessage::DagMessage::kParentRemoved)
		{
			MItDag itDag;
			for (itDag.reset(child); !itDag.isDone(); itDag.next())
			{
				UINT iRemove;
				MDagPath leaf;
				itDag.getPath(leaf);
				
				if(TagContainer.Find(leaf, &iRemove, NULL))
				{			
					TagContainer.Erase(iRemove);
				}
			}
		}
	}

	if(parent.hasFn(MFn::kWorld))//parent == DxRoot.GetPath())
		ParentTag= &DxRoot;
	else if(!TagContainer.Find(parent, NULL, &ParentTag))
	{
		const char* parenttype= parent.node().apiTypeStr();
		return S_OK;
	}

	if(child.hasFn(MFn::kWorld))//parent == DxRoot.GetPath())
		return S_OK;//ChildTag= &DxRoot;
	else if(!TagContainer.Find(child, NULL, &ChildTag))
	{
		const char* childtype= child.node().apiTypeStr();
		return S_OK;
	}
	else if(ChildTag == ParentTag)
	{
		const char* parenttype= parent.node().apiTypeStr();
		const char* childtype= child.node().apiTypeStr();
		ParentTag= &DxRoot;
	}


	if(    ChildTag
		&& ParentTag
		&&ChildTag->Interface  
		&& ParentTag->Interface)
	{
		DXCC_DPFA_REPORT("PARENTING: (%s) %s <==> %s", Msg, parent.fullPathName().asChar(), child.fullPathName().asChar());

		if( ParentTag->Resource->GetIID() == IID_IDXCCFrame 
			&& ChildTag->Resource->GetIID() == IID_IDXCCFrame)
		{
			if(dagMsg == MDagMessage::DagMessage::kParentAdded)
			{
				hr=((LPDXCCFRAME)ChildTag->Interface)->SetParent(((LPDXCCFRAME)ParentTag->Interface));
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);

				TriggerViewerEvent()->OnFrameChildAdded((LPDXCCFRAME)ParentTag->Interface, (LPDXCCFRAME)ChildTag->Interface);

			}
			else if(dagMsg == MDagMessage::DagMessage::kParentRemoved)
			{
				hr=((LPDXCCFRAME)ChildTag->Interface)->SetParent(NULL);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);

				TriggerViewerEvent()->OnFrameChildRemoved((LPDXCCFRAME)ParentTag->Interface, (LPDXCCFRAME)ChildTag->Interface);
			}
		}
		else if( ParentTag->Resource->GetIID() == IID_IDXCCFrame 
			&& ChildTag->Resource->GetIID() == IID_IDXCCMesh)
		{
			if(dagMsg == MDagMessage::DagMessage::kParentAdded )
			{
				hr=((LPDXCCFRAME)ParentTag->Interface)->AddMember((LPDXCCMESH)ChildTag->Interface);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);

				TriggerViewerEvent()->OnFrameMemberAdded((LPDXCCFRAME)ParentTag->Interface, (LPDXCCMESH)ChildTag->Interface);
			}
			else if(dagMsg == MDagMessage::DagMessage::kParentRemoved )
			{
				UINT iRemove;
				if(DXCC_SUCCEEDED(((LPDXCCFRAME)ParentTag->Interface)->FindMemberByPointer(ChildTag->Interface, &iRemove)))
				{
					hr=((LPDXCCFRAME)ParentTag->Interface)->RemoveMember(iRemove);
					if(DXCC_FAILED(hr))
						DXCC_GOTO_EXIT(e_Exit, TRUE);
				
					TriggerViewerEvent()->OnFrameMemberRemoved((LPDXCCFRAME)ParentTag->Interface, (LPDXCCMESH)ChildTag->Interface);
				}
			}
		}
	}

	DXCC_ASSERT_EXCEPTIONS_END()	
e_Exit:
	return hr;
}

void DPF_DEPNodeAttributeChanged(	MNodeMessage::AttributeMessage msg, 
									MPlug & plug,
									MPlug & otherPlug )
{
	DXCC_DPF_HEADER(TEXT("INFO"));
	DXCC_DPFA_SHORT("ATTRIBUTE: (");
	if(msg & MNodeMessage::AttributeMessage::kConnectionMade)
		DXCC_DPFA_SHORT("kConnectionMade|");
	if(msg & MNodeMessage::AttributeMessage::kConnectionBroken)
		DXCC_DPFA_SHORT("kConnectionBroken|");
	if(msg & MNodeMessage::AttributeMessage::kAttributeEval)
		DXCC_DPFA_SHORT("kAttributeEval|");
	if(msg & MNodeMessage::AttributeMessage::kAttributeSet)
		DXCC_DPFA_SHORT("kAttributeSet|");
	if(msg & MNodeMessage::AttributeMessage::kAttributeLocked)
		DXCC_DPFA_SHORT("kAttributeLocked|");
	if(msg & MNodeMessage::AttributeMessage::kAttributeUnlocked)
		DXCC_DPFA_SHORT("kAttributeUnlocked|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeAdded)
		DXCC_DPFA_SHORT("kAttributeAdded|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeRemoved)
		DXCC_DPFA_SHORT("kAttributeRemoved|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeRenamed)
		DXCC_DPFA_SHORT("kAttributeRenamed|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeKeyable)
		DXCC_DPFA_SHORT("kAttributeKeyable|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeUnkeyable)
		DXCC_DPFA_SHORT("kAttributeUnkeyable|");	
	if(msg & MNodeMessage::AttributeMessage::kIncomingDirection)
		DXCC_DPFA_SHORT("kIncomingDirection|");	
	if(msg & MNodeMessage::AttributeMessage::kAttributeArrayAdded)
		DXCC_DPFA_SHORT("kAttributeArrayAdded|");
	if(msg & MNodeMessage::AttributeMessage::kAttributeArrayRemoved)
		DXCC_DPFA_SHORT("kAttributeArrayRemoved|");
	if(msg & MNodeMessage::AttributeMessage::kOtherPlugSet)
		DXCC_DPFA_SHORT("kOtherPlugSet|");
	DXCC_DPFA_MESSAGE(") %s <==> %s", plug.name().asChar(), otherPlug.name().asChar());
};

HRESULT CMayaPreviewPipeline::Tag_Dirty(MNodeMessage::AttributeMessage msg, 
															MPlug & plug,
															MPlug & otherPlug)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	const char* attrName= MFnAttribute(plug.attribute()).name().asChar();
	//DPF_DEPNodeAttributeChanged(msg, plug, otherPlug );

	if(msg&MNodeMessage::AttributeMessage::kAttributeEval
	|| msg&MNodeMessage::AttributeMessage::kAttributeSet
	|| msg&MNodeMessage::AttributeMessage::kAttributeAdded
	|| msg&MNodeMessage::AttributeMessage::kAttributeRemoved
	|| msg&MNodeMessage::AttributeMessage::kAttributeRenamed
	|| msg&MNodeMessage::AttributeMessage::kAttributeArrayAdded
	|| msg&MNodeMessage::AttributeMessage::kAttributeArrayRemoved
	|| msg&MNodeMessage::AttributeMessage::kOtherPlugSet
	|| msg&MNodeMessage::AttributeMessage::kConnectionMade)
	{
		if((attrName == strstr(attrName, "visibility"))//visibility
		|| (attrName == strstr(attrName, "lodVisibility"))//visibility
		|| (attrName == strstr(attrName, "overrideVisibility"))//visibility
		|| (attrName == strstr(attrName, "overrideEnabled"))//visibility
		|| (attrName == strstr(attrName, "intermediateObject")))//visibility
		{
			MItDag itDag;
			MDagPathArray PathArray;

			stat= MDagPath::getAllPathsTo(plug.node(), PathArray);
			for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
			{
				for( itDag.reset(PathArray[iPath]); !itDag.isDone(); itDag.next())
				{
					DCCTag* tag= NULL;
					MDagPath dagLeaf;
					itDag.getPath(dagLeaf);

					if(!DAGIsVisible(dagLeaf, &stat))
					{						
						UINT iRemove;
						if(TagContainer.Find(dagLeaf, &iRemove, &tag))
						{
							MDagPath dagParent= dagLeaf;
							dagParent.pop();
							Tag_Reparent(MDagMessage::DagMessage::kParentRemoved, dagLeaf, dagParent, false);
							TagContainer.Erase(iRemove);
						}
					}
					else
					{
						TagContainer.Add(dagLeaf, NULL, NULL);
					}		
				}
			}
		}
		else 
		//((0 == lstrcmpA(attrName, "outMesh"))//generic mesh update
		//|| (attrName == strstr(attrName, "pnts"))//vertex movement
		//|| (attrName == strstr(attrName, "instObjGroups"))//material assignment
		//|| (attrName == strstr(attrName, "translate"))//transform
		//|| (attrName == strstr(attrName, "rotate"))//transform
		//|| (attrName == strstr(attrName, "scale")))//transform
		{
			MDagPathArray PathArray;

			stat= MDagPath::getAllPathsTo(plug.node(), PathArray);
			for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
			{
				DCCTag* tag= NULL;
				if(TagContainer.Find(PathArray[iPath], NULL, &tag))
				{
#ifdef MAYA_ATTRIBUTE_DEBUG
					tag->AttributeChanged.append(MFnAttribute(plug.attribute()).name());
#endif
					TagContainer.MarkAsDirty(tag);
				}
			}
		}
		 
	}

	DXCC_ASSERT_EXCEPTIONS_END()

//e_Exit:
	return hr;
}

D3DXMATRIX& CMayaPreviewPipeline::PerspectiveCamera_GetView()
{
	return PerspectiveCamera_View;
}

D3DXMATRIX& CMayaPreviewPipeline::PerspectiveCamera_GetProjection()
{
	return PerspectiveCamera_Projection;
}

HRESULT CMayaPreviewPipeline::Synchronize_PerspectiveCamera()
{
    MDagPath MayaCamera;
    M3dView panel;
    for(UINT iView= 0; iView < M3dView::numberOf3dViews(); iView++)
    {
        D3DXMATRIXA16 mCamera;
        M3dView::get3dView(iView, panel);
        panel.getCamera(MayaCamera);
        MayaCamera.pop();
        //const char* cameraName= MayaCamera.partialPathName().asChar();
        if(MayaCamera.partialPathName() == MString("persp"))
        {
            MayaCamera.extendToShape();
            MFloatMatrix fView(MayaCamera.inclusiveMatrix().matrix );

            ConvertWorldMatrix(mCamera, fView);

            panel.getCamera(MayaCamera);
            MFnCamera fnMayaCamera(MayaCamera.node());

            MVector mUp= fnMayaCamera.upDirection();
            MVector mAt= fnMayaCamera.viewDirection();
            MPoint mEye= fnMayaCamera.eyePoint(MSpace::kWorld);
            
            D3DXVECTOR3 dxEye( (float)-mEye.x, (float)mEye.y, (float)mEye.z );
            D3DXVECTOR3 dxAt( (float)-mAt.x, (float)mAt.y, (float)mAt.z );
            D3DXVECTOR3 dxUp( (float)-mUp.x, (float)mUp.y, (float)mUp.z );
            D3DXVECTOR4 fEye;
            D3DXVECTOR4 fAt;
            D3DXVECTOR3 fUp;

            D3DXVec3Transform(&fEye, &dxEye,(D3DXMATRIX*)&mCamera);
            D3DXVec3Transform(&fAt, &dxAt,(D3DXMATRIX*)&mCamera);
            D3DXVec3TransformNormal(&fUp, &dxUp,(D3DXMATRIX*)&mCamera);
            
            
            D3DXMatrixLookAtLH(&PerspectiveCamera_View, 
                (D3DXVECTOR3*)&fEye,
                (D3DXVECTOR3*)&fAt,
                &fUp);          

            // Projection matrix
            float zNear = fnMayaCamera.nearClippingPlane();
            float zFar = fnMayaCamera.farClippingPlane();
            float hFOV = fnMayaCamera.horizontalFieldOfView();
            float f = 1.0f / tan( hFOV / 2 );

            ZeroMemory( &PerspectiveCamera_Projection, sizeof(PerspectiveCamera_Projection) );
            PerspectiveCamera_Projection._11 = f;
            PerspectiveCamera_Projection._22 = f;
            PerspectiveCamera_Projection._33 = (zFar+zNear) / (zFar-zNear);
            PerspectiveCamera_Projection._34 = 1.0f;
            PerspectiveCamera_Projection._43 = -2 * (zFar*zNear)/(zFar-zNear);
            
            break;
        }
    }
	return S_OK;
}


HRESULT CMayaPreviewPipeline::Synchronize_Node(MObject & node)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MDagPathArray PathArray;

	stat= MDagPath::getAllPathsTo(node, PathArray);
	for(UINT iPath= 0; iPath < PathArray.length(); iPath++)
	{
		hr= Synchronize_DagPath(PathArray[iPath]);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);
	}
	
	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}

HRESULT CMayaPreviewPipeline::Synchronize_DagPath(MDagPath &path)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	if(path.node().hasFn(MFn::kMesh)
		|| path.node().hasFn(MFn::kTransform))
	{
		DCCTag *tag= NULL;
		
		if(!TagContainer.Add(path, NULL, &tag))
			DXCC_STATUS_EXIT(hr, S_OK, e_Exit, FALSE);


		hr= Synchronize_Tag(tag);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);	
	}

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return S_OK;
}

HRESULT CMayaPreviewPipeline::Synchronize_Tag(DCCTag *pTag)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

#ifdef MAYA_ATTRIBUTE_DEBUG
	DXCC_DPF_HEADER(TEXT("INFO"));
	DXCC_DPFA_MESSAGE("ATTRIBUTES: ");
	for(UINT i= 0; i < pTag->AttributeChanged.length(); i++)
	{
		DXCCDebugPrintfA(pTag->AttributeChanged[i].asChar());
		DXCCDebugPrintfA(" && ");
	}
	DXCC_DPFA_MESSAGE("");
	pTag->AttributeChanged.clear();
#endif

	if(pTag->GetPath().node().hasFn(MFn::kMesh))	
	{
		return Synchronize_Mesh(pTag);
	}
	else// if(pTag->Path.node().hasFn(MFn::kTransform))
	{
		return Synchronize_Frame(pTag);
	}

	DXCC_ASSERT_EXCEPTIONS_END()

//e_Exit:
	return S_OK;
}

HRESULT
CMayaPreviewPipeline::Synchronize_Frame(DCCTag *pTag)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;
	D3DXMATRIX d3dMatrix;

	if(!pTag)
		return E_INVALIDARG;



	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MDagPath dagTag= pTag->GetPath();

	//Create the frame???
	if(!pTag->IsInitialized())
	{
		hr= DXCCCreateFrame((LPDXCCFRAME*)&pTag->Interface);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= pTag->Resource->Recycle(pTag->Interface, IID_IDXCCFrame, TRUE, FALSE, FALSE, FALSE);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= pTag->Resource->SetName(MakeNameExportable(dagTag.partialPathName()).asChar());
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		TriggerManagerEvent()->OnResourceRecycle(pTag->Resource);
	}

	if(pTag->Resource != NULL 
		&& pTag->Resource->GetIID() != IID_IDXCCFrame)
	{
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	DXCC_DPFA("FRAME_SYNC: %s", dagTag.fullPathName().asChar());

	D3DXMatrixIdentity(&d3dMatrix);
	if(dagTag.node().hasFn(MFn::kTransform))
	{
		MMatrix localMatrix;
		MFnTransform fnTransform(dagTag.node(), &stat);
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

		localMatrix= fnTransform.transformation(&stat).asMatrix();
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

		ConvertLocalMatrix(d3dMatrix, localMatrix);

	}

	hr= ((LPDXCCFRAME)pTag->Interface)->SetLocalMatrix(&d3dMatrix);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
};

HRESULT CMayaPreviewPipeline::Mesh_GatherMeshInfo(MFnMesh& fnMesh, DCCMeshInfo& MayaMeshInfo)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	//CALCULATE TRIANGLES AND VERTICES
	MItMeshPolygon itMeshPoly(MObject::kNullObj);
	MItMeshVertex itMeshVert(MObject::kNullObj);

	DXCC_ASSERT(!fnMesh.object().isNull());

	stat = itMeshPoly.reset(fnMesh.object());
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Could not reset Poly iterator."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}
	for (; !itMeshPoly.isDone(); itMeshPoly.next())
	{
		int cPolyTriCount;
		UINT cPolyonVerticesMaya;
		
		cPolyonVerticesMaya= itMeshPoly.polygonVertexCount(&stat);
		if(MAYA_FAILED(stat))
		{
			DXCC_DPF_ERROR(TEXT("polygonVertexCount"));
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
		}

		stat= itMeshPoly.numTriangles(cPolyTriCount);
		if(MAYA_FAILED(stat))
		{
			DXCC_DPF_ERROR(TEXT("numTriangles"));
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
		}

		MayaMeshInfo.cD3DTriangles+= cPolyTriCount;
		MayaMeshInfo.cD3DVertices+= cPolyonVerticesMaya;
	}
	//END//CALCULATE TRIANGLES AND VERTICES

	hr= Mesh_GatherVertexInfo(fnMesh, MayaMeshInfo.VertexInfo);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}

HRESULT CMayaPreviewPipeline::Mesh_GatherVertexInfo(MFnMesh& fnMesh, DCCVertexInfo& MayaVertexInfo)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	//CALCULATE UV SET QUANTITY
	MayaVertexInfo.cUVs= fnMesh.numUVSets(&stat);
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Unable to retrieve UV count."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	stat= fnMesh.getUVSetNames ( MayaVertexInfo.UVNames ); 
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Unable to retrieve UV set names."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	stat= MayaVertexInfo.Ucoords.setLength(MayaVertexInfo.cUVs);
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Unable to resize UV UCoord Array."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	stat= MayaVertexInfo.Vcoords.setLength(MayaVertexInfo.cUVs);
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Unable to resize UV VCoord Array."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;

}

HRESULT	
CMayaPreviewPipeline::Mesh_CreateOrRecycle(DCCTag *pTag, MFnMesh &fnMesh, DCCMeshInfo &MayaMeshInfo)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	LPDXCCDECLARATION	dxDeclaration= NULL; 
	LPDXCCMESH			dxMesh= NULL;	
	LPMESHUSERDATA		dxMeshUserData= NULL;
	LPDIRECT3DDEVICE9	dxDevice= NULL;

	if(!pTag)
		return E_INVALIDARG;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()
	
	MDagPath dagTag= pTag->GetPath();

	hr= pTag->Resource->GetUserData(NULL, (void**) &dxMeshUserData, NULL);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	if( pTag->IsInitialized()
		&& dxMeshUserData != NULL 
		&& dxMeshUserData->cMayaPolygons == fnMesh.numPolygons()
		&& dxMeshUserData->cMayaEdges == fnMesh.numEdges()
		&& dxMeshUserData->cMayaFaceVertices == fnMesh.numFaceVertices()
		&& dxMeshUserData->cMayaUVs == fnMesh.numUVs()
		&& dxMeshUserData->cMayaUVSets == fnMesh.numUVSets()
		&& dxMeshUserData->cMayaVertices == fnMesh.numVertices())
	{
		pTag->Interface->AddRef();
		dxMesh= (LPDXCCMESH)pTag->Interface;	

		dxMesh->RemoveAllAttributedMaterials();

		MayaMeshInfo.cD3DVertices=  dxMesh->NumVertices();
		MayaMeshInfo.cD3DTriangles=  dxMesh->NumFaces();

		DXCC_RELEASE(dxMesh);

		hr= Mesh_GatherVertexInfo(fnMesh, MayaMeshInfo.VertexInfo);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		MeshCleanUserData(dxMeshUserData, fnMesh.numVertices(), MayaMeshInfo.cD3DVertices );
	}
	else
	{
		//must come before MeshCreateUserData to fill in MayaMeshInfo.cD3DVertices
		hr= Mesh_GatherMeshInfo(fnMesh, MayaMeshInfo);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		//must come after Mesh_GatherMeshInfo to fill in MayaMeshInfo.cD3DVertices
		dxMeshUserData= MeshCreateUserData(pTag->Resource, fnMesh.numVertices(), MayaMeshInfo.cD3DVertices);
		if(!dxMeshUserData)
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

		dxMeshUserData->cMayaPolygons= fnMesh.numPolygons();
		dxMeshUserData->cMayaEdges= fnMesh.numEdges();
		dxMeshUserData->cMayaFaceVertices= fnMesh.numFaceVertices();
		dxMeshUserData->cMayaUVs= fnMesh.numUVs();
		dxMeshUserData->cMayaUVSets= fnMesh.numUVSets();
		dxMeshUserData->cMayaVertices= fnMesh.numVertices();
	

		hr= DXCCCreateDeclaration(&dxDeclaration);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= dxDeclaration->InsertPositionElement(dxDeclaration->NumElements());
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= dxDeclaration->InsertNormalElement(dxDeclaration->NumElements());
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= dxDeclaration->InsertDiffuseElement(dxDeclaration->NumElements());
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		for(UINT iTex= 0; iTex < MayaMeshInfo.VertexInfo.cUVs; iTex++)
		{
			hr=dxDeclaration->InsertTexcoordElement(dxDeclaration->NumElements(), iTex, 2, MayaMeshInfo.VertexInfo.UVNames[iTex].asChar() );
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);
		}

		hr= AccessEngine()->GetD3DDevice(&dxDevice);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		if(pTag->IsInitialized())
		{
			pTag->Interface->AddRef();
			dxMesh= (LPDXCCMESH)pTag->Interface;	


			hr= ((LPDXCCMESH)pTag->Interface)->Recycle(
					MayaMeshInfo.cD3DTriangles,
					MayaMeshInfo.cD3DVertices,
					dxDevice,
					dxDeclaration->GetElements());
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

		}
		else
		{
//			DXCC_DPFA("MESH_INIT: %s", dagTag.fullPathName().asChar());

			hr= DXCCCreateMesh(MayaMeshInfo.cD3DTriangles, MayaMeshInfo.cD3DVertices, dxDevice, dxDeclaration->GetElements(), (LPDXCCMESH*)&pTag->Interface);
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			hr= pTag->Resource->Recycle(pTag->Interface, IID_IDXCCMesh, TRUE, FALSE, FALSE, FALSE);
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			hr= pTag->Resource->SetName(MakeNameExportable(dagTag.partialPathName()).asChar());
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			TriggerManagerEvent()->OnResourceRecycle(pTag->Resource);
		}
	}



	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	DXCC_RELEASE(dxDevice);
	DXCC_RELEASE(dxDeclaration);
	DXCC_RELEASE(dxMesh);

	return hr;
}


HRESULT	
CMayaPreviewPipeline::Synchronize_Mesh(DCCTag *pTag)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;
	
	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MFnSkinCluster SkinCluster;
	MFnMesh fnMesh;

	MItMeshPolygon itMeshPoly(MObject::kNullObj);
	MItMeshVertex itMeshVert(MObject::kNullObj);
	MObjectArray Shaders;
	MIntArray Attributes; 
	MDagPath dagTag= pTag->GetPath();
	MDagPath dagParent= dagTag;
	DCCTag *pTagParent= NULL;
	dagParent.pop();


	DCCMeshInfo MayaMeshInfo;
	UINT iVertexD3D=0;
	UINT iTriangleD3D= 0;
	FLOAT fDeterminant=-1;
	DXCCFLOAT4 f4;
	LPDXCCVERTEXBUNDLER dxVertexBundler= NULL;
	LPDXCCMESH dxMesh= NULL;	
	LPMESHUSERDATA	dxMeshUserData= NULL;

	DXCC_DPFA("MESH_SYNC: %s", dagTag.fullPathName().asChar());


	if(TagContainer.Find(dagParent, NULL, &pTagParent)
		&& pTagParent->Interface)
	{
		fDeterminant= D3DXMatrixDeterminant(((LPDXCCFRAME)pTagParent->Interface)->GetWorldMatrix());
	}
	
	stat = fnMesh.setObject(dagTag.node());
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Object could not be read as mesh."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, FALSE);
	}

	hr= Mesh_CreateOrRecycle(pTag, fnMesh, MayaMeshInfo);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= pTag->Resource->GetUserData(NULL, (void**) &dxMeshUserData, NULL);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);
	pTag->Interface->AddRef();
	dxMesh= (LPDXCCMESH)pTag->Interface;

	//AND ATTRIBUTE BUFFER

	UINT instNum= dagTag.instanceNumber();
	stat= fnMesh.getConnectedShaders(dagTag.instanceNumber(), Shaders, Attributes);
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Could not get Mesh Shaders"));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}
	UINT NumShaders= Shaders.length();
	UINT NumAttributes= Attributes.length();

	hr= Mesh_GatherMaterials(Shaders, dxMesh);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT( e_Exit, TRUE);

	if(UI_GetSkinState())
	{
		hr= Mesh_FindSkin(fnMesh, SkinCluster);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT( e_Exit, TRUE);
	}

	hr= dxMesh->LockFaces();
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= dxMesh->GetVertexBundler(&dxVertexBundler);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= dxVertexBundler->LockAll();
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);
		
	stat = itMeshPoly.reset(fnMesh.object());
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Could not reset Poly iterator."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	stat = itMeshVert.reset(fnMesh.object());
	if(MAYA_FAILED(stat))
	{
		DXCC_DPF_ERROR(TEXT("Could not reset Poly Vertex iterator."));
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
	}

	//PolygonLoop
	for (int iPolygonMaya = 0; 
		!itMeshPoly.isDone(); 
		iPolygonMaya++, itMeshPoly.next())
	{
		MVector			PolygonNormalMaya;
		D3DXVECTOR3		PolygonNormalD3D;
		int				iPolygonVertexStartD3D= iVertexD3D;
		int				cPolyonTrianglesMaya;
		UINT			cPolyonVerticesMaya= itMeshPoly.polygonVertexCount(&stat);
		if(MAYA_FAILED(stat))
		{
			DXCC_DPF_ERROR(TEXT("polygonVertexCount"));
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
		}

		stat= itMeshPoly.getNormal(PolygonNormalMaya, MSpace::kObject); 
		if(MAYA_FAILED(stat))
		{
			DXCC_DPF_ERROR(TEXT("Could not retrieve polygon(%d) normal."), iPolygonMaya);
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
		}
		f4.Encode(&PolygonNormalMaya.x, 3);
		f4.Decode(&PolygonNormalD3D);

		//PolygonVertexLoop
		for(UINT iPolygonVertexMaya= 0; 
			iPolygonVertexMaya < cPolyonVerticesMaya; 
			iPolygonVertexMaya++, iVertexD3D++)
			//,itPolyVert.next()
		{
			int iOldVertexMaya;
			int iMeshVertexMaya= itMeshPoly.vertexIndex(iPolygonVertexMaya, &stat);
			if(MAYA_FAILED(stat))
			{
				DXCC_DPF_ERROR(TEXT("Could not retrieve mesh vertex from polygon(%d) vertex(%d)."), iPolygonMaya, iPolygonVertexMaya);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
			}

			stat= itMeshVert.setIndex(iMeshVertexMaya, iOldVertexMaya);
			if(MAYA_FAILED(stat))
			{
				DXCC_DPF_ERROR(TEXT("Could not retrieve vertex(%d) iterator"), iMeshVertexMaya);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
			}


			if(dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DFirstPointRep == (UINT)-1)
			{//link to D3DFirstPointRep&D3DLastPointRep if not hooked up
				dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DFirstPointRep= iVertexD3D;
				dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DLastPointRep= iVertexD3D;
			}
			else
			{//link to D3DNextPointRep&D3DLastPointRep
				dxMeshUserData->pVertexD3DToMaya[dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DLastPointRep].D3DNextPointRep= iVertexD3D;	
				dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DLastPointRep= iVertexD3D;
			}

			dxMeshUserData->pVertexD3DToMaya[iVertexD3D].MayaPolygon= iPolygonMaya;
			dxMeshUserData->pVertexD3DToMaya[iVertexD3D].MayaVertex= iMeshVertexMaya;	
			dxMeshUserData->D3DPointReps[iVertexD3D]= dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DFirstPointRep;





			MayaMeshInfo.VertexInfo.position= itMeshVert.position ( MSpace::kObject, &stat); 
			if(MAYA_FAILED(stat))
			{
				DXCC_DPF_ERROR(TEXT("Could not retrieve polygon(%d) Vertex(%d)"), iPolygonMaya, iPolygonVertexMaya);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
			}

			hr= dxVertexBundler->SetPosition(iVertexD3D, f4.Encode(&MayaMeshInfo.VertexInfo.position.x, 4));
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			stat= itMeshVert.getNormal(MayaMeshInfo.VertexInfo.normal, iPolygonMaya, MSpace::kObject); 
			if(MAYA_FAILED(stat))
			{
				MayaMeshInfo.VertexInfo.normal= PolygonNormalMaya;
				stat= MStatus::MStatusCode::kSuccess;
			}

			hr= dxVertexBundler->SetNormal(iVertexD3D, f4.Encode(&MayaMeshInfo.VertexInfo.normal.x, 3));
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);


			for(UINT iUV= 0; iUV < MayaMeshInfo.VertexInfo.UVNames.length(); iUV++)
			{
				float2 uvPoint;
				if(MAYA_SUCCEEDED(itMeshVert.getUV(iPolygonMaya, uvPoint,  &MayaMeshInfo.VertexInfo.UVNames[iUV])))
				{
					MayaMeshInfo.VertexInfo.Ucoords[iUV]= uvPoint[0];
					MayaMeshInfo.VertexInfo.Vcoords[iUV]= uvPoint[1];
				}
				else
				{
					MayaMeshInfo.VertexInfo.Ucoords[iUV]= 0.0f;
					MayaMeshInfo.VertexInfo.Vcoords[iUV]= 0.0f;
				}

				hr= dxVertexBundler->SetTexcoord(iVertexD3D, iUV, f4.Encode(&D3DXVECTOR2(MayaMeshInfo.VertexInfo.Ucoords[iUV], 1.0f-MayaMeshInfo.VertexInfo.Vcoords[iUV])));
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);
			}

			if(itMeshVert.hasColor())
			{
				if(!MAYA_FAILED(itMeshVert.getColor(MayaMeshInfo.VertexInfo.color, iPolygonMaya)))
				{
					hr= dxVertexBundler->SetDiffuse(iVertexD3D, f4.Encode(&MayaMeshInfo.VertexInfo.color.r, 4));
					if(DXCC_FAILED(hr))
						DXCC_GOTO_EXIT(e_Exit, TRUE);
				}
			}

		}//END//PolygonVertexLoop
		
		stat= itMeshPoly.numTriangles(cPolyonTrianglesMaya);
		if(MAYA_FAILED(stat))
		{
			DXCC_DPF_ERROR(TEXT("Could not retrieve number of triangles."));
			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
		}

		//PolygonTriangleLoop
		for (int iPolyonTriangleMaya = 0; 
			iPolyonTriangleMaya < cPolyonTrianglesMaya; 
			iPolyonTriangleMaya++, iTriangleD3D++)
		{
			MPointArray			TrianglePointsMaya;
			MIntArray			TriangleVertexListMaya; //these are local to polygon
			D3DXVECTOR3			v0to1;
			D3DXVECTOR3			v0to2;
			D3DXVECTOR3			TriangleNormalD3D;
			FLOAT				fDot;
			BOOL				bReverse= FALSE;

			stat= itMeshPoly.getTriangle(iPolyonTriangleMaya,  
										 TrianglePointsMaya, //these are local to polygon
										 TriangleVertexListMaya, 
										 MSpace::Space::kObject ); 	
			if(MAYA_FAILED(stat))
			{
				DXCC_DPF_ERROR(TEXT("Could not retrieve polygon(%d) triangle(%d)"), iPolygonMaya, iPolyonTriangleMaya);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
			}

			if(TriangleVertexListMaya.length() != 3)
			{
				DXCC_DPF_ERROR(TEXT("Polygon(%d) Triangle(%d) Size != 3"), iPolygonMaya, iPolyonTriangleMaya);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
			}

			(MFloatVector(TrianglePointsMaya[1])-MFloatVector(TrianglePointsMaya[0])).get((FLOAT*)v0to1);
			(MFloatVector(TrianglePointsMaya[2])-MFloatVector(TrianglePointsMaya[0])).get((FLOAT*)v0to2);
			D3DXVec3Normalize(&v0to1, &v0to1);
			D3DXVec3Normalize(&v0to2, &v0to2);
			D3DXVec3Cross( &TriangleNormalD3D, &v0to1, &v0to2);
			fDot= D3DXVec3Dot(&TriangleNormalD3D, &PolygonNormalD3D);

			if((fDot >= 0.0f && fDeterminant < 0.0f) || (fDot < 0.0f && fDeterminant >= 0.0f))
			{
				int swap= TriangleVertexListMaya[0];
				TriangleVertexListMaya[0]= TriangleVertexListMaya[2];
				TriangleVertexListMaya[2]=swap;
			}

			//TriangleVertexLoop
			for(UINT iTriangleVertexMaya= 0; 
				iTriangleVertexMaya < TriangleVertexListMaya.length(); 
				iTriangleVertexMaya++)
			{
				int iMeshVertexMaya;
				int iPolygonVertexD3D;

				iMeshVertexMaya= TriangleVertexListMaya[iTriangleVertexMaya];
				iPolygonVertexD3D= dxMeshUserData->pVertexMayaToD3D[iMeshVertexMaya].D3DLastPointRep;

				hr= dxMesh->SetFaceVertex(iTriangleD3D, iTriangleVertexMaya, iPolygonVertexD3D);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);
			}

			int attrib= Attributes[iPolygonMaya];
			if(attrib < 0 && attrib > NumShaders)
				attrib= NumShaders;

			hr= dxMesh->SetFaceAttribute(iTriangleD3D, attrib );
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

		}//END//PolygonTriangleLoop
	}//END//PolygonLoop

	hr= dxVertexBundler->UnlockAll();
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= dxMesh->UnlockFaces();
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= dxMesh->ConvertPointRepsToAdjacency(dxMeshUserData->D3DPointReps);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);


	if(UI_GetSkinState())
	{
		hr= Mesh_GatherSkin(pTag, SkinCluster);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT( e_Exit, TRUE);
	}


	TriggerViewerEvent()->OnMeshChange(dxMesh);


e_Exit:
	DXCC_RELEASE(dxVertexBundler);
	DXCC_RELEASE(dxMesh);


	DXCC_ASSERT_EXCEPTIONS_END()

	return hr;
}

HRESULT	
CMayaPreviewPipeline::Mesh_GatherMaterials( 
	MObjectArray& Shaders,  //shadingEngine nodes
	LPDXCCMESH pMesh)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;

	LPDXCCRESOURCE pRes= NULL;
	ID3DXEffect* pD3DMaterial= NULL;

	//SHADER
	if(Shaders.length() > 0)
	{
		for(UINT iShader= 0; iShader < Shaders.length(); iShader++) //shadingEngine nodes
		{
			MFnDependencyNode shadingEngine(Shaders[iShader]);
			MPlug ShaderPlug;
			MPlugArray ShaderPlugArray;

			DXCC_RELEASE(pRes);
			DXCC_RELEASE(pD3DMaterial);

			BOOL bFoundShader= false;

			//TODO displacement or volume shaders too!
			ShaderPlug = shadingEngine.findPlug("surfaceShader", &stat); 
			if (MAYA_SUCCEEDED(stat) && !ShaderPlug.isNull()) //we have to find the plug but it doesnt have to be connected
			{
				ShaderPlug.connectedTo(ShaderPlugArray, true, false); //the t
	
				if (ShaderPlugArray.length() == 1) 
				{
					MFnDependencyNode shader(ShaderPlugArray[0].node());
					int iHandle;
					DXCCHANDLE hHandle;
					MPlug plugResource= shader.findPlug(DirectXShader::aDXCCHandle);
					plugResource.getValue(iHandle);
					hHandle= (DXCCHANDLE)iHandle;

					if(hHandle != NULL)
					{
						if(DXCC_SUCCEEDED(AccessManager()->FindResourceByHandle(hHandle, &pRes)))
						{ 
							//if(DXCC_SUCCEEDED(pRes->GetObject( (LPUNKNOWN*) &pD3DMaterial)))
							if(DXCC_SUCCEEDED(pRes->QueryObject( IID_ID3DXEffect, (void**) &pD3DMaterial)))
							{
								if(DXCC_SUCCEEDED(pMesh->SetAttributedMaterial(iShader, pD3DMaterial)))
								{
									bFoundShader= true;
								}
								DXCC_RELEASE(pD3DMaterial);
							}
							DXCC_RELEASE(pRes);
						}
					}
				}
			}

			if(!bFoundShader)
			{
				hr= pMesh->SetAttributedMaterial(iShader, NULL);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT( e_Exit, TRUE);
			}
		}

		hr= pMesh->SetAttributedMaterial(Shaders.length(), NULL);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT( e_Exit, TRUE);
	}
	else
	{
		hr= pMesh->SetAttributedMaterial(0, NULL);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT( e_Exit, TRUE);
	}


e_Exit:
	DXCC_RELEASE(pRes);
	DXCC_RELEASE(pD3DMaterial);

	return hr;
}


HRESULT		
CMayaPreviewPipeline::Material_Save(MObject &node)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;
	MFnDependencyNode depNode(node);
	const char *pEffectStr= NULL;
	MPlug plugResource= depNode.findPlug(DirectXShader::aDXCCHandle);
	LPDXCCRESOURCE pResource= NULL;
	LPD3DXEFFECT pEffect= NULL;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MObject oFxParamList;
	DXCCHANDLE hHandle;
	int iHandle;
	CDXCCSaveEffectDefaults SaveParams;
	CPreviewPipelineLock SceneLock;

	SceneReadLock(true, SceneLock);

	plugResource.getValue(iHandle);
	hHandle= (DXCCHANDLE)iHandle;

	hr= AccessManager()->FindResourceByHandle(hHandle, &pResource);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);		

	if(DXCC_FAILED(pResource->GetObject((LPUNKNOWN*)&pEffect)
		|| (pEffect == NULL)))
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);		

	{//FILE PATH
		MPlug plugFilePath= depNode.findPlug(DX_LONG_FXFILE_PATH);
		if(plugFilePath.isNull())
		{
			MObject oFxFilePath= tAttr.create(DX_LONG_FXFILE_PATH, DX_SHORT_FXFILE_PATH, MFnData::Type::kString);
			CHECK_MSTATUS( tAttr.setHidden( true ) );
			stat= depNode.addAttribute(oFxFilePath, MFnDependencyNode::kLocalDynamicAttr );
			plugFilePath= depNode.findPlug(DX_LONG_FXFILE_PATH, &stat);
		}
	
		LPCSTR resourcepath= pResource->GetResourcePath();
		if(resourcepath)
		{
			CHECK_MSTATUS(plugFilePath.setValue(MString(resourcepath)));
		}
		else
		{
			CHECK_MSTATUS(plugFilePath.setValue(MString("")));
		}

	}//END//FILE PATH

	{//FILE NAME
		MPlug plugFileName= depNode.findPlug(DX_LONG_FXFILE_NAME);
		if(plugFileName.isNull())
		{
			MObject oFxFileName= tAttr.create(DX_LONG_FXFILE_NAME, DX_SHORT_FXFILE_NAME, MFnData::Type::kString);
			CHECK_MSTATUS( tAttr.setStorable( false ) );
			stat= depNode.addAttribute(oFxFileName, MFnDependencyNode::kLocalDynamicAttr );
			plugFileName= depNode.findPlug(DX_LONG_FXFILE_NAME, &stat);
		}

		LPCSTR resourcepath= strrchr(pResource->GetResourcePath(), '\\');
		if(resourcepath)
			resourcepath++;
		else
			resourcepath= pResource->GetResourcePath();

		CHECK_MSTATUS(plugFileName.setValue(MString(resourcepath)));
	}//END//FILE NAME

	{//PARAMTER COUNT
		MPlug plugParamCount= depNode.findPlug(DX_LONG_FXPARAM_COUNT);
		if(!plugParamCount.isNull())
		{
			int count;
			plugParamCount.getValue(count);

			for(int p= 0; p < count; p++)
			{
				MObject oName= depNode.attribute(MString(DX_LONG_FXPARAM_NAME)+p, &stat);
				MObject oData= depNode.attribute(MString(DX_LONG_FXPARAM_DATA)+p, &stat);
				
				if(!oName.isNull())
					stat= depNode.removeAttribute(oName);
				if(!oData.isNull())
					stat= depNode.removeAttribute(oData);
			}
		}
		else
		{
			oFxParamList= nAttr.create(DX_LONG_FXPARAM_COUNT, DX_SHORT_FXPARAM_COUNT,MFnNumericData::Type::kInt, SaveParams.iParameter+1, &stat);
			CHECK_MSTATUS( nAttr.setHidden( true ) );
			CHECK_MSTATUS(depNode.addAttribute(oFxParamList, MFnDependencyNode::kLocalDynamicAttr));
		}
		plugParamCount= depNode.findPlug(DX_LONG_FXPARAM_COUNT);

		hr= SaveParams.Set(AccessManager(), node);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		hr= DXCCEnumEffectParameters(pEffect, &SaveParams);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

		CHECK_MSTATUS(plugParamCount.setValue((int)(SaveParams.iParameter+1)));
	}//END//PARAMTER COUNT
	
e_Exit:
	DXCC_RELEASE(pResource);
	DXCC_RELEASE(pEffect);
	
	SceneReadUnlock(SceneLock);

	return hr;
}

HRESULT		
CMayaPreviewPipeline::Material_Restore(MObject &node)
{
	HRESULT hr= S_OK;
	MStatus stat= MS::kSuccess;
	
	MFnDependencyNode depNode(node);

	LPCSTR pFXFilePath= NULL;
	LPDIRECT3DDEVICE9 pd3dDevice= NULL;
	LPDXCCRESOURCE pResource= NULL;
	LPD3DXEFFECT pEffect= NULL;

	LPDXCCRESOURCE pResTexture= NULL;
	LPDIRECT3DCUBETEXTURE9 pTextureCube= NULL;
	LPDIRECT3DVOLUMETEXTURE9 pTexture3D= NULL;
	LPDIRECT3DTEXTURE9 pTexture2D= NULL;
	LPD3DXBUFFER pErrors= NULL;
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	LPCSTR pFXFileFound= NULL;

	{//FILEPATH FILENAME
		MObject oFilePath;
		MPlug plugFilePath= depNode.findPlug(DX_LONG_FXFILE_PATH);
		if(MAYA_FAILED( plugFilePath.getValue(oFilePath)))
			return S_OK;

		MFnStringData sFilePathData(oFilePath);
		pFXFilePath= sFilePathData.string().asChar();
	}//END//FILEPATH

	MPlug plugParamCount= depNode.findPlug(DX_LONG_FXPARAM_COUNT);

	if(pFXFilePath && (pFXFilePath[0] != '\0') && !plugParamCount.isNull())
	{
		INT cParamCount;
		int iHandle= NULL;
		DXCCHANDLE hHandle= NULL;
		LPCSTR pFXFileName= NULL;

		{//FILENAME
			MPlug plugFileName= depNode.findPlug(DX_LONG_FXFILE_NAME);
	
			pFXFileName= strrchr(pFXFilePath, '\\');
			if(pFXFileName)
				pFXFileName++;
			else
				pFXFileName= pFXFilePath;

			plugFileName.setValue(MString(pFXFileName));
		}//END//FILENAME

		MPlug plugResource= depNode.findPlug(DirectXShader::aDXCCHandle);
		if(MAYA_SUCCEEDED(plugResource.getValue(iHandle)))
		{
			hHandle= (DXCCHANDLE)iHandle;

			if(!DXCC_SUCCEEDED(AccessManager()->FindResourceByHandle(hHandle, &pResource)))
			{
				hr= AccessManager()->CreateResource(NULL, IID_ID3DXEffect, true, &pResource);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);
			}

			AccessEngine()->GetD3DDevice(&pd3dDevice);
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);


			hFind= FindFirstFileA(pFXFilePath, &FindFileData);
			if(INVALID_HANDLE_VALUE != hFind)
			{
				FindClose(hFind);
				pFXFileFound= pFXFilePath;
			}
			else 
			{
				hFind= FindFirstFileA(pFXFileName, &FindFileData);

				if(INVALID_HANDLE_VALUE != hFind)
				{
					FindClose(hFind);
					pFXFileFound= pFXFileName;
				}
				else
				{	
					MString ErrorMsg= depNode.name() + MString(" cannot load shader - ") + MString(pFXFilePath);
					MessageBoxA(NULL, "Shader FX file cannot be found!" , ErrorMsg.asChar(), MB_ICONEXCLAMATION);
					DXCC_STATUS_EXIT(hr, S_OK, e_Exit, FALSE);
				}
			}

			if(!DXCC_SUCCEEDED(D3DXCreateEffectFromFileA(
					pd3dDevice,
					pFXFileFound,
					NULL,
					NULL,
					NULL,
					NULL,
					&pEffect,
					&pErrors)))
			{
				MessageBoxA(NULL,
							(LPCSTR)pErrors->GetBufferPointer(),
							"Effect Load Error",
							MB_ICONEXCLAMATION);
				DXCC_RELEASE(pErrors);
				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, FALSE);
			}

			hr= pResource->Recycle( pEffect, IID_ID3DXEffect, TRUE, FALSE, FALSE, FALSE); 
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			hr= pResource->SetName(MakeNameExportable(depNode.name()).asChar());
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			hr= pResource->SetResourcePath(pFXFilePath);
			if(DXCC_FAILED(hr))
				DXCC_GOTO_EXIT(e_Exit, TRUE);

			
			CHECK_MSTATUS(plugParamCount.getValue(cParamCount));

			for(UINT i= 0; i < cParamCount; i++)
			{
				MObject oData;
				MObject oName;
				MString szName;
				
				DXCC_RELEASE(pResTexture);
				DXCC_RELEASE(pTexture2D);
				DXCC_RELEASE(pTexture3D);
				DXCC_RELEASE(pTextureCube);

				MPlug plugName= depNode.findPlug(MString(DX_LONG_FXPARAM_NAME)+i, &stat);
				if(!MAYA_SUCCEEDED( stat ))
					continue;
				MPlug plugData= depNode.findPlug(MString(DX_LONG_FXPARAM_DATA)+i, &stat);
				if(!MAYA_SUCCEEDED( stat ))
					continue;

				stat= plugName.getValue(szName);
				if(!MAYA_SUCCEEDED( stat ))
					continue;
				
				stat= plugData.getValue(oData);
				if(!MAYA_SUCCEEDED( stat ))
					continue;

				D3DXHANDLE hHandle= pEffect->GetParameterByName(NULL, szName.asChar());
				if(hHandle != NULL)
				{
					D3DXPARAMETER_DESC ParamDesc;
					if(DXCC_SUCCEEDED(pEffect->GetParameterDesc(hHandle, &ParamDesc )))
					{
						FLOAT fArray[16];
						switch(ParamDesc.Class)
						{
						case D3DXPC_SCALAR:
						case D3DXPC_VECTOR:
						case D3DXPC_MATRIX_ROWS:
						case D3DXPC_MATRIX_COLUMNS:
							{
								MFnDoubleArrayData dArrayData(oData);
								MDoubleArray dArray= dArrayData.array();

								for(UINT i= 0; i < dArray.length(); i++)
									fArray[i]= (FLOAT)dArray[i];

								hr= pEffect->SetFloatArray(hHandle, fArray, dArray.length());
								if(DXCC_FAILED(hr))
									DXCC_GOTO_EXIT(e_Exit, TRUE);
							}
							break;
						case D3DXPC_OBJECT:
							{
								switch(ParamDesc.Type)
								{
								case D3DXPT_STRING:
									{
										MFnStringData StringData(oData);
		
										hr= pEffect->SetString(hHandle, StringData.string().asChar());
										if(DXCC_FAILED(hr))
											DXCC_GOTO_EXIT(e_Exit, TRUE);
									}
									break;
								case D3DXPT_TEXTURE:
								case D3DXPT_TEXTURE1D:
								case D3DXPT_TEXTURE2D:
								case D3DXPT_TEXTURE3D:
								case D3DXPT_TEXTURECUBE:
									{
										D3DXIMAGE_INFO info;
										MFnStringData ParamStringData(oData);
										const char* pParamStr= ParamStringData.string().asChar();

										if(DXCC_SUCCEEDED(D3DXGetImageInfoFromFileA( pParamStr, &info)))
										{
											switch(info.ResourceType)
											{
											case D3DRTYPE_TEXTURE:
												{
													hr= D3DXCreateTextureFromFileA(         
															pd3dDevice,
															pParamStr,
															&pTexture2D);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= AccessManager()->CreateResource(
															pTexture2D, 
															IID_IDirect3DTexture9, 
															TRUE, 
															&pResTexture);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pResTexture->SetResourcePath(pParamStr);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pEffect->SetTexture(hHandle, pTexture2D);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

												}
												break;
											case D3DRTYPE_VOLUMETEXTURE:
												{
													hr= D3DXCreateVolumeTextureFromFileA(         
															pd3dDevice,
															pParamStr,
															&pTexture3D);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= AccessManager()->CreateResource(
															pTexture3D, 
															IID_IDirect3DVolumeTexture9, 
															TRUE, 
															&pResTexture);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pResTexture->SetResourcePath(pParamStr);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pEffect->SetTexture(hHandle, pTexture3D);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);
												}
												break;
											case D3DRTYPE_CUBETEXTURE:
												{
													hr= D3DXCreateCubeTextureFromFileA(         
															pd3dDevice,
															pParamStr,
															&pTextureCube);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= AccessManager()->CreateResource(
															pTextureCube, 
															IID_IDirect3DCubeTexture9, 
															TRUE, 
															&pResTexture);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pResTexture->SetResourcePath(pParamStr);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);

													hr= pEffect->SetTexture(hHandle, pTextureCube);
													if(DXCC_FAILED(hr))
														DXCC_GOTO_EXIT(e_Exit, TRUE);
												}
												break;
											};//end switch info.ResourceType
										}//end D3DXGetImageInfoFromFile
									}//end case textures
									break;
								};//end switch object type
							}//end case object
							break;
						case D3DXPC_STRUCT:
						default:
							DXCC_ASSERT(FALSE);
							break;
						}; //end switch ParamDesc.Class
					}//end GetParameterDesc
				}//end hHandle
			}//end MFnCompoundAttribute child loop


			Synchronize_Material(hHandle);

		}//end FindResource
	}//end aPath is !null

e_Exit:

	DXCC_RELEASE(pResTexture);
	DXCC_RELEASE(pTexture2D);
	DXCC_RELEASE(pTexture3D);
	DXCC_RELEASE(pTextureCube);

	DXCC_RELEASE(pd3dDevice);
	DXCC_RELEASE(pResource);
	DXCC_RELEASE(pEffect);
	DXCC_RELEASE(pErrors);

	return hr;
}


void	
MeshCleanUserData(
	LPMESHUSERDATA dxMeshUserData, 
	UINT cVerticesMaya,
	UINT cVerticesD3D)	
{ 

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	if(new(dxMeshUserData) MeshUserData)
	{
		if(cVerticesD3D > MeshUserData::D3DPointRepsLength)
		{
			if(MeshUserData::D3DPointReps)
				delete [] MeshUserData::D3DPointReps;

			MeshUserData::D3DPointReps= new DWORD[cVerticesD3D];
			MeshUserData::D3DPointRepsLength= cVerticesD3D;


			if(MeshUserData::pVertexD3DToMaya)
				delete [] MeshUserData::pVertexD3DToMaya;

			MeshUserData::pVertexD3DToMaya= new VertexD3DToMayaType[cVerticesD3D];
			MeshUserData::VertexD3DToMayaLength= cVerticesD3D;
		}
		//FillMemory(MeshUserData::D3DPointReps, cVerticesD3D*sizeof(DWORD), 0xff);//fill minimum needed
		//new(MeshUserData::pVertexD3DToMaya) VertexD3DToMayaType[cVerticesD3D];//fill minimum needed
		FillMemory(MeshUserData::pVertexD3DToMaya, sizeof(VertexD3DToMayaType)*cVerticesD3D, 0xff);


		if(cVerticesMaya > MeshUserData::VertexMayaToD3DLength)
		{
			if(MeshUserData::pVertexMayaToD3D)
				delete [] MeshUserData::pVertexMayaToD3D;

			MeshUserData::pVertexMayaToD3D= new VertexMayaToD3DType[cVerticesMaya];
			MeshUserData::VertexMayaToD3DLength= cVerticesMaya;
		}
		//new(MeshUserData::pVertexMayaToD3D) VertexMayaToD3DType[cVerticesMaya];
		FillMemory(MeshUserData::pVertexMayaToD3D, sizeof(VertexMayaToD3DType)*cVerticesMaya, 0xff);

	}


	DXCC_ASSERT_EXCEPTIONS_END()

	return;
}

LPMESHUSERDATA	
MeshCreateUserData(
	LPDXCCRESOURCE pRes, 
	UINT cVerticesMaya,
	UINT cVerticesD3D)	
{ 
	//HRESULT hr;
	LPMESHUSERDATA dxMeshUserData= NULL;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	DWORD size= sizeof(MeshUserData);
		//+sizeof(DWORD)*cVerticesD3D
		//+sizeof(MeshUserData::VertexD3DToMayaType)*cVerticesD3D
		//+sizeof(MeshUserData::VertexMayaToD3DType)*cVerticesMaya;
		
	if(DXCC_SUCCEEDED(pRes->CreateUserData(size))
		&& DXCC_SUCCEEDED(pRes->GetUserData(NULL, (void**) &dxMeshUserData, NULL)))
	{
		MeshCleanUserData(dxMeshUserData, cVerticesMaya, cVerticesD3D);
	}

	DXCC_ASSERT_EXCEPTIONS_END()

	return dxMeshUserData;
}


LPMESHUSERDATA	
MeshGetUserData(LPDXCCMESH pMesh, LPDXCCMANAGER pDXCCManager)						
{ 
	LPDXCCRESOURCE pRes= NULL;
	LPMESHUSERDATA dxMeshUserData= NULL;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	if(DXCC_SUCCEEDED(pDXCCManager->FindResourceByPointer(pMesh, NULL, &pRes)))
		pRes->GetUserData(NULL, (void**) &dxMeshUserData, NULL);

	DXCC_ASSERT_EXCEPTIONS_END()

	DXCC_RELEASE(pRes);
	return dxMeshUserData;  
}

void DCCTagContainer::MarkAsDirty(DCCTag* tag)
{
	if(!tag->Dirty)
	{
		tag->Dirty= true;
		DirtyArray.push_back(tag);
	}
}


void DCCTagContainer::MarkAsClean_SLOW(DCCTag* tag)
{
	if(tag->Dirty)
	{
		for(UINT i= 0; i < SizeFromDirtyList(); i++)
		{
			DCCTag* found= NULL;
			if(	GetFromDirtyList(i, found)&& 
				found == tag)
			{
				MarkAsClean(i);
				return;
			}
		}
	}
}


void DCCTagContainer::MarkAsDirty(UINT iGlobalTagIndex)
{
	DCCTag* tag= NULL;
	if(Get(iGlobalTagIndex, tag))
	{
		MarkAsDirty(tag);
	}
}

void DCCTagContainer::MarkAsClean(UINT iDirtyListIndex)
{
	DCCTag* tag= NULL;
	
	if(GetFromDirtyList(iDirtyListIndex, tag))
	{
		if(tag->Dirty)
		{
			tag->Dirty=false;
			DirtyArray.erase(DirtyArray.begin()+iDirtyListIndex);
		}
	}
}


bool DCCTagContainer::IsDirty(DCCTag* tag)
{
	return tag->Dirty;
}

HRESULT	CMayaPreviewPipeline::Mesh_FindSkin(MFnMesh& InOutMesh, MFnSkinCluster& SkinCluster)
{
	HRESULT hr = S_OK;
	MStatus stat = MS::kSuccess;

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	MItDependencyGraph FindSkin(	InOutMesh.object(), 
									MFn::kSkinClusterFilter, 
									MItDependencyGraph::Direction::kUpstream,
									MItDependencyGraph::Traversal::kBreadthFirst,
									MItDependencyGraph::Level::kNodeLevel, 
									&stat);
	if(MAYA_SUCCEEDED(stat))
	{
		MItDependencyGraph FindMesh(	FindSkin.thisNode(), 
										MFn::kMesh, 
										MItDependencyGraph::Direction::kUpstream,
										MItDependencyGraph::Traversal::kBreadthFirst,
										MItDependencyGraph::Level::kNodeLevel, 
										&stat);
		if(MAYA_FAILED(stat))
			DXCC_STATUS_EXIT(hr, S_OK, e_Exit, TRUE);
		//Exit if we couldnt find the original Mesh

		SkinCluster.setObject(FindSkin.thisNode());
		
		InOutMesh.setObject(FindMesh.thisNode());
	}

	DXCC_ASSERT_EXCEPTIONS_END()

e_Exit:
	return hr;
}

HRESULT	CMayaPreviewPipeline::Mesh_GatherSkin(DCCTag *pTag, MFnSkinCluster& SkinCluster)
{
	HRESULT hr = S_OK;
	MStatus stat = MS::kSuccess;
	LPDXCCMESH dxMesh= NULL;
	DCCTag *pParentTag= NULL;
	LPMESHUSERDATA	dxMeshUserData= NULL;
	MMatrix			MatrixToMesh;						

	DXCC_ASSERT_EXCEPTIONS_BEGIN()

	if(SkinCluster.object().isNull())
		return hr;

	MDagPath dagParent= pTag->GetPath();
	dagParent.pop();

	if(!TagContainer.Find(dagParent, NULL, &pParentTag))
		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);

	MatrixToMesh= dagParent.exclusiveMatrix();

	hr= pTag->Resource->GetUserData(NULL, (void**) &dxMeshUserData, NULL);
	if(DXCC_SUCCEEDED(hr))
	{
		pTag->Interface->AddRef();
		dxMesh= (LPDXCCMESH)pTag->Interface;

		if(MAYA_SUCCEEDED(stat))
		{
			MDagPathArray bones;
			UINT cBones;

			cBones= SkinCluster.influenceObjects(bones, &stat);
			if(cBones > 0)
			{
				hr= dxMesh->CreateBones(cBones);
				if(DXCC_FAILED(hr))
					DXCC_GOTO_EXIT(e_Exit, TRUE);

				for(UINT iBone= 0; iBone < cBones; iBone++)
				{
					MSelectionList	SelectionInfo;
					MFloatArray		Weights;
					MFnTransform	fnBone;
					DCCTag*			pBoneTag= NULL;

					//MObject			VertexSets;
					//UINT			iWeight;
					MPlug			PlugToBindPose;
					MObject			MObjectToBindPose;
					//MMatrix			WorldWorldBindMatrix;


					DXCC_DPFA("Processing SkinCluster Bone: %s", bones[iBone].partialPathName().asChar());

					//Get d3d bone
					if(!TagContainer.Find(bones[iBone], NULL, &pBoneTag))
					{
						DXCC_DPF_ERROR(TEXT("Could not find bone"));
						DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
					}

					//Get maya bone
					stat = fnBone.setObject(bones[iBone]);
					if(MAYA_FAILED(stat))
					{
						DXCC_DPF_ERROR(TEXT("Could not create transform"));
						DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
					}

					//get influences
					stat = SkinCluster.getPointsAffectedByInfluence(bones[iBone], SelectionInfo, Weights);
					if(MAYA_FAILED(stat))
					{
						DXCC_DPF(TEXT("Ignoring bone because failure on getPointsAffectedByInfluence."));
						DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
					}

					{//BIND POSE

						MMatrix			MatrixToBindPose;
						MMatrix			MatrixOfBoneOffset;
						MFnMatrixData	MatrixDataToBindPose;
						D3DXMATRIX		matBoneOffset;

						MObjectToBindPose = MObject::kNullObj;

						PlugToBindPose = fnBone.findPlug("bindPose", &stat);
						if(MAYA_FAILED(stat)) // success
						{
							DXCC_DPF_ERROR(TEXT("Could not get Pose Plug"));
							DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
						}

						stat = PlugToBindPose.getValue(MObjectToBindPose);
						if(MAYA_FAILED(stat))
						{
							DXCC_DPF_ERROR(TEXT("Could not get Pose Plug WorldWorldBindMatrix MObject"));
							DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
						}

						stat = MatrixDataToBindPose.setObject(MObjectToBindPose);
						if(MAYA_FAILED(stat))
						{
							DXCC_DPF_ERROR(TEXT("Could not get WorldWorldBindMatrix FnMatrixData interface"));
							DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
						}

						MatrixToBindPose= MatrixDataToBindPose.matrix();

						MatrixOfBoneOffset= MatrixToMesh * MatrixToBindPose.inverse();

						ConvertLocalMatrix(matBoneOffset, MatrixOfBoneOffset);
					
						dxMesh->SetBoneFrame(iBone, (LPDXCCFRAME)pBoneTag->Interface, NULL);
						dxMesh->SetBoneOffsetMatrix(iBone, &matBoneOffset);
					}//END//BIND POSE


					//WEIGHTS
					if(SelectionInfo.length() == 0)
					{
						for (UINT iWeight = 0; iWeight < dxMesh->NumVertices(); iWeight++)
						{
							dxMesh->SetBoneInfluence(iBone, iWeight, 0.0f);
						};
					}
					else
					{
						for(UINT iInfluenceComponents= 0; iInfluenceComponents < SelectionInfo.length(); iInfluenceComponents++)
						{
							MDagPath dagMesh;
							MObject components;

							//get mesh and vertices from selection data
							stat = SelectionInfo.getDagPath(iInfluenceComponents, dagMesh, components);
							if(MAYA_FAILED(stat))
							{
								DXCC_DPF_ERROR(TEXT("Could not read DAG Path from SkinCluster  =SelectioInfo"));
								DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
							}
							else
							{
								UINT iWeight = 0;
								MItGeometry itVertices(dagMesh, components);
								for (; !itVertices.isDone(); itVertices.next(), iWeight++)
								{
									FLOAT Weight= Weights[iWeight]; 

									for(DWORD iVertexD3D= dxMeshUserData->pVertexMayaToD3D[itVertices.index()].D3DFirstPointRep;
										iVertexD3D != (DWORD)-1;
										iVertexD3D= dxMeshUserData->pVertexD3DToMaya[iVertexD3D].D3DNextPointRep)
									{
										dxMesh->SetBoneInfluence(iBone, iVertexD3D, Weight);
									}
								}
							}
						}
					}//END//WEIGHTS
				}
			}
		}
	}


e_Exit:
	DXCC_RELEASE(dxMesh);

	DXCC_ASSERT_EXCEPTIONS_END()

	return hr;
}

HRESULT CMayaPreviewPipeline::Frame_GatherAnimation(DCCTag* pTag)
{
	HRESULT hr= S_OK;
	MStatus stat = MS::kSuccess;

	LPDXCCFRAME pFrame= NULL;
	LPDXCCANIMATIONSTREAM pAnim= NULL;
	if(!pTag)
		return E_FAIL;

	if(!pTag->Resource)
		return E_FAIL;

	if(pTag->Resource->GetIID() != IID_IDXCCFrame)
		return E_FAIL;

	if(!pTag->Interface)
		return E_FAIL;

	MDagPath dagTag= pTag->GetPath();
	MFnDependencyNode depNode(dagTag.node());
	MPlug matrixPlug= depNode.findPlug("matrix", &stat);//xformMatrix
	UINT minTick= (UINT)MAnimControl::minTime().as(MTime::uiUnit());
	UINT maxTick= (UINT)MAnimControl::maxTime().as(MTime::uiUnit());
	UINT cTicks= maxTick-minTick;
	UINT nFPS= MayaGetFPS();

	pFrame= (LPDXCCFRAME)pTag->Interface;
	pFrame->AddRef();

	hr= pFrame->GetLocalAnimation(&pAnim);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);

	hr= pAnim->CreateTransformKeys(nFPS, D3DXPLAY_ONCE, cTicks);
	if(DXCC_FAILED(hr))
		DXCC_GOTO_EXIT(e_Exit, TRUE);
	
	for(UINT iTick= 0; iTick < cTicks; iTick++)
	{
		MObject matrixObject;
		DXCCKEY_MATRIX matrixKey;
		matrixKey.Time= (double)(minTick+iTick);

		MDGContext timeContext(MTime(matrixKey.Time, MTime::uiUnit()));
		stat= matrixPlug.getValue(matrixObject, timeContext);

		MFnMatrixData matrixData(matrixObject, &stat);

		ConvertLocalMatrix(matrixKey.Value, matrixData.matrix());

		hr= pAnim->SetTransformKeyAsMatrix(iTick, &matrixKey);
		if(DXCC_FAILED(hr))
			DXCC_GOTO_EXIT(e_Exit, TRUE);

	}

e_Exit:
	DXCC_RELEASE(pFrame);
	DXCC_RELEASE(pAnim);
	return hr;
}




//
//HRESULT 
//MayaMeshHierarchy::SceneAddAnimationFixedStep()
//{
//	HRESULT	hr = S_OK;
//	MStatus stat = MS::kSuccess;
//
//	//MAnimControl mAnimCtrl;
//	MTime mtTick, mtOriginalTime;
//	LONG nTicks;
//	LPDXCCFRAME pITFrame= NULL;
//	LPDXCCFRAMEITERATOR pIT = NULL;
//	DWORD nFPS= MayaGetFPS();
//
//	hr= DXCCCreateFrameIterator(&pIT);
//	if(DXCC_FAILED(hr))
//		DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//	mtOriginalTime = MAnimControl::currentTime();
//
//	nTicks= (LONG)(MAnimControl::maxTime()-MAnimControl::minTime()).value()+1;
//
//	//Initialize frames
//	
//	for(pIT->Reset(pRoot, DXCCITERATOR_DEPTHFIRST), pIT->Next(); !pIT->Done(); pIT->Next())
//	{
//		LPFRAMEUSERDATA pFrameUD;
//
//		hr= pIT->Get(&pITFrame);
//		if(DXCC_FAILED(hr))
//			DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//		pFrameUD= FrameGetUserData(pITFrame, pDXCCManager);
//
//		//if(MAnimUtil::isAnimated(pFrameUD->dagPath))
//		if(pFrameUD->dagPath.hasFn(MFn::kTransform))
//		{
//			
//			LPDXCCANIMATIONSTREAM pAnimation= NULL;
//			hr= pITFrame->GetLocalAnimation(&pAnimation);
//			if(DXCC_FAILED(hr))
//				DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//			hr= pAnimation->CreateTransformKeys(nFPS, D3DXPLAY_ONCE, nTicks);
//			if(DXCC_FAILED(hr))
//				DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//			DXCC_RELEASE(pAnimation);
//		}
//
//		DXCC_RELEASE(pITFrame);
//	}
//
//	for (mtTick = MAnimControl::minTime(); mtTick <= MAnimControl::maxTime(); mtTick += 1)
//	{
//		LONG iTick= (LONG)mtTick.value() - (LONG)MAnimControl::minTime().value();
//
//		stat= MAnimControl::setCurrentTime(mtTick);
//		if(MAYA_FAILED(stat))
//			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
//
//		for(pIT->Reset(pRoot, DXCCITERATOR_DEPTHFIRST), pIT->Next(); !pIT->Done(); pIT->Next())
//		{
//			LPFRAMEUSERDATA pFrameUD;
//
//			hr= pIT->Get(&pITFrame);
//			if(DXCC_FAILED(hr))
//				DXCC_GOTO_EXIT(e_Exit, TRUE);
//			
//			pFrameUD= FrameGetUserData(pITFrame, pDXCCManager);
//			if(pFrameUD->dagPath.hasFn(MFn::kTransform))
//			{
//				hr= FrameAddAnimationFixedStep(iTick, pITFrame);
//				if(DXCC_FAILED(hr))
//					DXCC_GOTO_EXIT(e_Exit, TRUE);
//			}
//
//			DXCC_RELEASE(pITFrame);
//		}
//	}
//
//e_Exit:
//	DXCC_RELEASE(pITFrame);
//	DXCC_RELEASE(pIT);
//
//	// reset current time
//	MAnimControl::setCurrentTime(mtOriginalTime);
//
//	return hr;
//}
//
//HRESULT 
//MayaMeshHierarchy::FrameAddAnimationFixedStep(
//	LONG iTick, 
//	LPDXCCFRAME _pFrame)
//{
//	HRESULT	hr = S_OK;
//	MStatus stat = MS::kSuccess;
//	MFnTransform fnTransform;
//	DXCCKEY_MATRIX KeyMatrix;
//	MMatrix MayaMatrix;
//	DWORD i, j;			// counters
//	LPDXCCANIMATIONSTREAM pAnimation= NULL;
//	DWORD nFPS= MayaGetFPS();
//	MDagPath FramePath;
//
//	LPFRAMEUSERDATA pFrameUD= FrameGetUserData(_pFrame, pDXCCManager);
//	//MDagPath::getAPathTo(pFrameUD->object, FramePath);
//	FramePath= pFrameUD->dagPath;
//
//	if(!FramePath.hasFn(MFn::kTransform))
//		DXCC_GOTO_EXIT( e_Exit, FALSE);
//
//	//if(MAnimUtil::isAnimated(pFrameUD->dagPath))
//	//	DXCC_GOTO_EXIT( e_Exit, FALSE);
//
//	hr= _pFrame->GetLocalAnimation(&pAnimation);
//	if(DXCC_SUCCEEDED(hr))
//	{
//		stat = fnTransform.setObject(FramePath);
//		if(MAYA_FAILED(stat))
//			DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
//
//
//		KeyMatrix.Time= (FLOAT)iTick;///(float)nFPS;
//
//		MayaMatrix= fnTransform.transformation().asMatrix();
//		for (i = 0; i < 4; i++)
//			for (j = 0; j < 4; j++)
//				KeyMatrix.Value[i * 4 + j]= (FLOAT)MayaMatrix(i, j);
//
//		hr= pAnimation->SetTransformKeyAsMatrix(iTick, &KeyMatrix);
//		if(DXCC_FAILED(hr))
//			DXCC_GOTO_EXIT(e_Exit, TRUE);
//	}
//
//e_Exit:
//	DXCC_RELEASE(pAnimation);
//	return hr;
//};
//
//
//HRESULT 
//MayaMeshHierarchy::SceneAddAnimationKeys()
//{
//	HRESULT	hr = S_OK;
//	MStatus stat = MS::kSuccess;
//	LPDXCCFRAME pITFrame= NULL;
//	LPDXCCFRAMEITERATOR pIT= NULL;
//	MAnimControl mAnimCtrl;
//
//	hr= DXCCCreateFrameIterator(&pIT);
//	if(DXCC_FAILED(hr))
//		DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//
//	for(pIT->Reset(pRoot, DXCCITERATOR_DEPTHFIRST), pIT->Next(); !pIT->Done(); pIT->Next())
//	{
//		hr= pIT->Get(&pITFrame);
//		if(DXCC_FAILED(hr))
//			DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//		hr= FrameAddAnimationKeys(pITFrame);
//		if(DXCC_FAILED(hr))
//			DXCC_GOTO_EXIT(e_Exit, TRUE);
//
//		DXCC_RELEASE(pITFrame);
//	}
//
//e_Exit:
//	DXCC_RELEASE(pITFrame);
//	DXCC_RELEASE(pIT);
//
//	return hr;
//}
//
//HRESULT 
//MayaMeshHierarchy::FrameAddAnimationKeys(
//	LPDXCCFRAME _pFrame)
//{
//	HRESULT	hr = S_OK;
//	MStatus stat = MS::kSuccess;
////
////	DWORD nFPS= MayaGetFPS();
////	MAnimControl mAnimCtrl;
////	MTime mtTick;
////	MTime mtOriginalTime;
////	MFnTransform fnTransform;
////	MPlugArray AnimPlugs;
////	UINT iPlug, iCurve, i, j;
////	BOOL* rgbKeyedFrames;
////	UINT iKey, nKeys, iTick, nTicks= (UINT)(MAnimControl::maxTime()-MAnimControl::minTime()).value()+1;
////	MMatrix MayaMatrix;
////	LPDXCCANIMATIONSTREAM pAnimation;
////	MDagPath FramePath;
////
////	LPFRAMEUSERDATA pFrameUD= FrameGetUserData(_pFrame);
////	//MDagPath::getAPathTo(pFrameUD->object, FramePath);
////	FramePath= pFrameUD->dagPath;
////
////	if(MAnimUtil::isAnimated(pFrameUD->dagPath))
////		DXCC_GOTO_EXIT( e_Exit, FALSE);
////
////	mtOriginalTime = mAnimCtrl.currentTime();
////
////	stat = fnTransform.setObject(FramePath);
////	if(MAYA_FAILED(stat))
////		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
////
////	MAnimUtil::findAnimatedPlugs(FramePath, AnimPlugs, false, &stat);
////	if(MAYA_FAILED(stat))
////		DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
////
////	//COUNT AND FLAG THE KEYS
////	//MAYA IS MORE COMPLICATED THAN SRT SO WE WILL JUST CAPTURE THE WHOLE MATRIX ON ANY KEY
////	rgbKeyedFrames= (BOOL*)_alloca(sizeof(BOOL)*nTicks);
////	nKeys= 0;
////	for (nKeys= 0, iPlug = 0; iPlug < AnimPlugs.length(); iPlug++)
////	{
////		MObjectArray Curves;
////
////		MAnimUtil::findAnimation(AnimPlugs[iPlug], Curves, &stat);
////		if(MAYA_FAILED(stat))
////		{
////			DXCC_DPF(TEXT("Ignoring anim-plug because could not find animation."));
////			continue;
////		}
////
////		for (iCurve = 0; iCurve < Curves.length(); iCurve++)
////		{
////			MFnAnimCurve fnCurve(Curves[iCurve], &stat);
////			if(MAYA_FAILED(stat))
////				DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
////
////			for (iKey = 0; iKey < fnCurve.numKeys(); iKey++)
////			{
////				MTime mtTime = fnCurve.time(iKey, &stat);
////				if(MAYA_FAILED(stat))
////					DXCC_STATUS_EXIT(hr, E_FAIL, e_Exit, TRUE);
////
////				rgbKeyedFrames[(LONG)mtTime.value()]= TRUE;
////				nKeys++;
////			}
////		}
////	}
////
////	hr= DXCCCreateAnimationStreamForMatrices(nKeys, &pAnimation);
////	if(DXCC_FAILED(hr))
////		DXCC_GOTO_EXIT(e_Exit, TRUE);
////
////	hr= _pFrame->SetLocalAnimation(pAnimation);
////	if(DXCC_FAILED(hr))
////		DXCC_GOTO_EXIT(e_Exit, TRUE);
////
////	for(iKey= 0, iTick = 0; iTick < nTicks; iTick++)
////	{
////		if(rgbKeyedFrames[iTick] == TRUE)
////		{
////			LPDXCCKEY_MATRIX pKeyMatrix;
////			hr= pAnimation->GetMatrix(iKey, &pKeyMatrix);
////			if(DXCC_FAILED(hr))
////				DXCC_GOTO_EXIT(e_Exit, TRUE);
////
////			pKeyMatrix->Time= iTick/(float)nFPS ;
////
////			MayaMatrix= fnTransform.transformation().asMatrix();
////			for (i = 0; i < 4; i++)
////				for (j = 0; j < 4; j++)
////					pKeyMatrix->Value[i * 4 + j]= (FLOAT)MayaMatrix(i, j);
////
////
////			iKey++;
////		}
////	}
////
////e_Exit:
////	DXCC_RELEASE(pAnimation);
////	// reset current time
////	mAnimCtrl.setCurrentTime(mtOriginalTime);
//
//	return hr;
//};









