#ifndef __XEXPORTER_H__
#define __XEXPORTER_H__

//required to compile under vc7
//#define REQUIRE_IOSTREAM

#include "PreviewPipeline.h"
#include "MayaInterface.h"
#include "device.h"
#include "viewer.h"



#include <vector>
#include <map>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include <maya/MGlobal.h>
#include <maya/MStringArray.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MStatus.h>
#include <maya/MFnMesh.h>
#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MDagMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MArgList.h>
#include <maya/MFloatArray.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFloatMatrix.h>

using namespace std;
///////////////////////////////////////////////////////////////////////////////////////
// DEBUG_CONTROL
///////////////////////////////////////////////////////////////////////////////////////
//#define MAYA_ATTRIBUTE_DEBUG

///////////////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////////
class DCCTag;
class DCCTagContainer;
class CMayaPreviewPipeline;
class CMayaPreviewPipelineDevice;
class DirectXShader;

#define DX_UPDATE_FREQUENCY			(1000.0f/15.0f)





extern CRITICAL_SECTION DeviceAndViewerSection; 
extern HANDLE DeviceCreatedEvent; 
extern HANDLE AddOrUpdateMutex; 


//typedef struct FrameUserData		*LPFRAMEUSERDATA;
//LPFRAMEUSERDATA	FrameGetUserData(LPDXCCFRAME pFrame, LPDXCCMANAGER pManager);					
//LPFRAMEUSERDATA	FrameCreateUserData(LPDXCCRESOURCE pRes);				
typedef struct	MeshUserData			*LPMESHUSERDATA;
LPMESHUSERDATA	MeshGetUserData(LPDXCCMESH pMesh, LPDXCCMANAGER pManager);			
LPMESHUSERDATA	MeshCreateUserData(LPDXCCRESOURCE pRes, UINT cVerticesMaya, UINT cVerticesD3D);
void			MeshCleanUserData(	LPMESHUSERDATA dxMeshUserData, 	UINT cVerticesMaya,	UINT cVerticesD3D)	;

bool	SkinMeshGetOriginalMesh(MFnMesh& fnMesh);
HRESULT CalculateMeshSize(MFnMesh& fnMesh, UINT& cTriangles, UINT& cVertices);


bool	DAGIsVisible(MDagPath& dagPath, MStatus* status);
bool	NodeIsVisible(MObject& node, MStatus* _status);

UINT	MayaGetFPS();

void ConvertLocalMatrix(D3DXMATRIX& ToD3D, const MMatrix& FromMaya);
void ConvertLocalMatrix(D3DXMATRIX& ToD3D, const MFloatMatrix& FromMaya);
void ConvertWorldMatrix(D3DXMATRIX& ToD3D, const MMatrix& FromMaya);
void ConvertWorldMatrix(D3DXMATRIX& ToD3D, const MFloatMatrix& FromMaya);



///////////////////////////////////////////////////////////////////////////////////////
// EXTERNS
///////////////////////////////////////////////////////////////////////////////////////
extern CMayaPreviewPipeline g_PreviewPipeline;

///////////////////////////////////////////////////////////////////////////////////////
// DEFINITONS
///////////////////////////////////////////////////////////////////////////////////////

struct DCCVertexInfo
{
	MPoint			position;
	MVector			normal;
	MColor			color;
	UINT			cUVs;
	MStringArray	UVNames;
	MFloatArray		Ucoords;
	MFloatArray		Vcoords;

	DCCVertexInfo()
	{
		cUVs=0;
	}
};

struct DCCMeshInfo
{

	UINT 			cD3DTriangles; 
	UINT 			cD3DVertices;

	DCCVertexInfo	VertexInfo;

	DCCMeshInfo()
	{
		cD3DTriangles=0; 
		cD3DVertices=0;
	}
};


struct DXCCTimeStamp
{
	_timeb			Coarse;
	clock_t			Fine;

	DXCCTimeStamp();
	void AcquireTime();
	bool RoughlyEqual(DXCCTimeStamp &test);
};


class DCCTag
{
	friend DCCTagContainer;
	friend CMayaPreviewPipeline;
protected:
	~DCCTag();
	DCCTag() {};
	DCCTag(DCCTagContainer* Owner, MDagPath& path);
public:

	void			SetPath(MDagPath& newPath);
	MDagPath&		GetPath();

	bool			IsValid();

	bool			IsInitialized();
	bool			SetInitialized(bool bInitializeNow);
	bool			CanInitialize();



	//DCCTag*			Parent;
	//vector<DCCTag*>	Dependants;//

#ifdef MAYA_ATTRIBUTE_DEBUG
	MStringArray	AttributeChanged;
#endif

	DXCCTimeStamp	TimeStamp;
	MCallbackId		PathCallback;
	IDXCCResource*	Resource;
	IUnknown*		Interface;
	
	//bool			DirtyAnimationFlag;


	MDagPath			Path;
	DCCTagContainer*	pContainer;
	bool				Dirty;
	bool				Initialized;

};

class DCCTagContainer
{
	friend DCCTag;
public:
	DCCTagContainer() {};
	DCCTagContainer(CMayaPreviewPipeline* owner) 
	{
		pPreview= owner;
		GlobalArray.clear();
		DirtyArray.clear();
	};

	~DCCTagContainer() 
	{
		EraseAll();
	};

	bool		Add(MObject& objAllPathsTo );
	bool		Add(MDagPath& path, UINT *pGlobalTagIndex, DCCTag** ppTag );

	UINT		Size();
	bool		Get(UINT iGlobalTagIndex, DCCTag* &tag );
	bool		Find(DCCTag* pFind, UINT *pGlobalTagIndex);
	bool		Find(MDagPath& path, UINT *pGlobalTagIndex, DCCTag** ppTag );

	UINT		SizeFromDirtyList();
	bool		GetFromDirtyList(UINT iGlobalTagIndex, DCCTag* &tag); 
	bool		FindFromDirtyList(DCCTag* pFind, UINT *pDirtyListIndex);
	bool		FindFromDirtyList(MDagPath& path, UINT *pGlobalTagIndex, DCCTag** ppTag );

	bool		Erase(UINT iGlobalTagIndex);
	bool		Erase_SLOW(DCCTag* tag);
	void		EraseAll();	
	void		EraseInvalidTags();

	void		MarkAsDirty(UINT iGlobalListIndex);
	void		MarkAsDirty(DCCTag* tag);
	void		MarkAsClean(UINT iDirtyListIndex);
	void		MarkAsClean_SLOW(DCCTag* tag);
	bool		IsDirty(DCCTag* tag);

	DXCCTimeStamp	TimeStamp;
protected:
	CMayaPreviewPipeline* pPreview;
	vector<DCCTag*> DirtyArray;
	vector<DCCTag*> GlobalArray;

};

//class DCCObject
//{
//	DCCObject(MObject& obj);
//	DCCTagContainer* GetTagContainer();
//	D3DXMATRIX GetLocalMatrix();
//}


class CMayaPreviewPipeline : public CPreviewPipeline
{
	friend DCCTagContainer;
	friend DirectXShader;
public:
						CMayaPreviewPipeline();
	virtual				~CMayaPreviewPipeline(); 

	virtual HRESULT		Create();
	virtual HRESULT		Destroy();

	static void			Callback_MayaExiting( void* clientData );
	static void			Callback_SceneSave(void* clientData );
	static void			Callback_ScenePreload(void* clientData );
	static void			Callback_ScenePostload(void* clientData );
	static void			Callback_SceneIdle( void* clientData );
	static void			Callback_NodeAdded( MObject & node, void* clientData );
	static void			Callback_NodeRemoved( MObject & node, void* clientData );
	static void			Callback_NodeNameChange( MObject & node, void* clientData );
	static void			Callback_NodeAttributeChanged( MNodeMessage::AttributeMessage msg, MPlug & plug, MPlug & otherPlug, void* clientData );
	static void			Callback_NodeParentingChange( MDagMessage::DagMessage dagMsg, MDagPath &child, MDagPath &parent, void * clientData );
	//static void		Callback_AnimationEdit( MObjectArray &editedCurves, void *clientData );//for realtime attempts at tracking animation.//not using this approach yet

	void				Scene_Register();
	void				Scene_Deregister();
	void				Scene_Update(bool bForceUpdate);
	void				Scene_Update_Tags(bool bForceUpdate, DXCCTimeStamp& TimeStamp, MFn::Type type);
	void				Scene_Update_Materials(bool bForceUpdate, DXCCTimeStamp& TimeStamp);
	void				Scene_Update_Visibility(bool bForceUpdate, DXCCTimeStamp& TimeStamp);
	void				Scene_Update_Animation();
	HRESULT				Scene_Export(const char* file, const char* options, MPxFileTranslator::FileAccessMode mode);

	HRESULT 			Synchronize_Node(MObject & node); //Multi-instance DCC sync
	HRESULT 			Synchronize_DagPath(MDagPath &path); //single instance DCC sync
	HRESULT 			Synchronize_Tag(DCCTag *pTag);//single instance pipeline sync
	HRESULT 			Synchronize_Frame(DCCTag *pTag);//typed sync
	HRESULT 			Synchronize_Mesh(DCCTag *pTag);//typed sync
	HRESULT 			Synchronize_PerspectiveCamera(); 
	HRESULT 			Synchronize_Material(DXCCHANDLE hDirtyShader); //Multi-instance DCC sync

	HRESULT 			Tag_Add(MObject & node);
	HRESULT 			Tag_Remove(MObject & node);
	HRESULT 			Tag_Rename( MObject & node);
	HRESULT 			Tag_Dirty(MNodeMessage::AttributeMessage msg, MPlug & plug, MPlug & otherPlug);
	HRESULT 			Tag_Reparent(MDagMessage::DagMessage dagMsg, MDagPath &child, MDagPath &parent, bool bFixInstances);
	
	HRESULT 			Frame_GatherAnimation(DCCTag* pTag)	;

	HRESULT				Mesh_CreateOrRecycle(DCCTag *pTag, MFnMesh &fnMesh, DCCMeshInfo &MayaMeshInfo);
	HRESULT 			Mesh_GatherMeshInfo(MFnMesh& fnMesh, DCCMeshInfo& MayaMeshInfo);
	HRESULT 			Mesh_GatherVertexInfo(MFnMesh& fnMesh, DCCVertexInfo& MayaVertexInfo);
	HRESULT				Mesh_GatherMaterials( MObjectArray& Shaders,  LPDXCCMESH pMesh);
	HRESULT				Mesh_FindSkin(MFnMesh& InOutMesh, MFnSkinCluster& SkinCluster);
	HRESULT				Mesh_GatherSkin(DCCTag *pTag, MFnSkinCluster& SkinCluster);

	HRESULT				Material_Save(MObject &node);
	HRESULT				Material_Restore(MObject &node);
	
	D3DXMATRIX& 		PerspectiveCamera_GetView();
	D3DXMATRIX& 		PerspectiveCamera_GetProjection();

	bool				UI_GetPreviewState();
	bool				UI_GetSkinState();
	void				UI_SetPreviewState(bool bPreview);

	DXCCHANDLE			Convert_DGNodeToDXCCResource(MObject matchNode);
	MObject				Convert_DXCCResourceToDGNode(DXCCHANDLE matchHandle);

////////////////////////VARIABLES/////////////////////
	DCCTagContainer TagContainer;
	vector<DXCCHANDLE> DirtyShaders;
	DCCTag DxRoot;
	MDagPath MayaRoot;


	MCallbackId CallbackId_Exiting;
	MCallbackId CallbackId_AfterOpen;
	MIntArray	CallbackId_PreviewArray;
	MIntArray	CallbackId_NodeArray;

	CEngine g_Engine;
	CViewer g_Viewer;

	D3DXMATRIX PerspectiveCamera_View;
	D3DXMATRIX PerspectiveCamera_Projection;

protected:


};

struct VertexD3DToMayaType
{
	VertexD3DToMayaType()
	{
		MayaPolygon= (UINT)-1;
		MayaVertex= (UINT)-1;
		D3DNextPointRep= (UINT)-1;
	};
	UINT MayaPolygon;
	UINT MayaVertex;
	UINT D3DNextPointRep;
}; 

struct VertexMayaToD3DType
{
	VertexMayaToD3DType()
	{
		D3DFirstPointRep= (UINT)-1;
		D3DLastPointRep= (UINT)-1;
	};
	UINT D3DFirstPointRep;
	UINT D3DLastPointRep;
}; 	

struct MeshUserData
{

	UINT 		cMayaPolygons;
	UINT 		cMayaEdges;
	UINT 		cMayaFaceVertices;
	UINT 		cMayaUVs;
	UINT 		cMayaUVSets;
	UINT 		cMayaVertices;

	//LPDXCCFRAME parent;
	_timeb		lastUpdate;

	static DWORD*		D3DPointReps;
	static DWORD		D3DPointRepsLength;
	static VertexD3DToMayaType *pVertexD3DToMaya;
	static DWORD				VertexD3DToMayaLength;
	static VertexMayaToD3DType *pVertexMayaToD3D;
	static DWORD				VertexMayaToD3DLength;


	MeshUserData() //:
		//parent(NULL),
		//D3DPointReps(NULL),
		//pVertexD3DToMaya(NULL),
		//pVertexMayaToD3D(NULL)
		{};

};


class CDXCCSaveEffectDefaults : public IDXCCEnumEffectParameters
{
public:
	HRESULT Set(LPDXCCMANAGER pResourceManager, /*MObject& attribParameters,*/ MObject& pxNode);

	STDMETHOD(EnumParameter)(THIS_  LPD3DXEFFECT pEffect, D3DXHANDLE handle, LPCSTR name);

	LPDXCCMANAGER pManager;
	MObject oNode;
	UINT iParameter;
};



#endif //__XEXPORTER_H__