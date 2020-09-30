#ifndef __MAYAINTERFACE_H__
#define __MAYAINTERFACE_H__

//required to compile under vc7
//#define REQUIRE_IOSTREAM  


//#define _XBOX_

#include "XExporter.h"
#include "viewer.h"

#include <maya/MPxFileTranslator.h>
#include <maya/MString.h>

#include <maya/MNodeMessage.h>
#include <maya/MFileObject.h>
#include <maya/MStatus.h>
#include <maya/MPxNode.h>
#include <maya/MArgList.h>
#include <maya/MPxCommand.h>
#include <maya/M3dView.h>
#include <maya/MDagMessage.h>
#include <maya/MSelectionList.h>
#include <maya/MObjectArray.h>

#define DX_LONG_FXHANDLE			"DXCC_Fx_Handle"
#define DX_SHORT_FXHANDLE			"DFxH"
#define DX_LONG_FXFILE_NAME			"File"
#define DX_SHORT_FXFILE_NAME		"DxFxFileName"
#define DX_LONG_FXFILE_PATH			"DXCC_FxFile_Path"
#define DX_SHORT_FXFILE_PATH		"DxFxFilePath"
#define DX_LONG_FXPARAM_COUNT		"DXCC_FxParam_Count"
#define DX_SHORT_FXPARAM_COUNT		"DxFxCount"
#define DX_LONG_FXPARAM_NAME		"DXCC_FxParam_Name"
#define DX_SHORT_FXPARAM_NAME		"DxFxName"
#define DX_LONG_FXPARAM_DATA		"DXCC_FxParam_Data"
#define DX_SHORT_FXPARAM_DATA		"DxFxData"
#define DX_LONG_ATTRIB_CALLBACKID	"DXCC_NodeAttribute_CallbackID"
#define DX_SHORT_ATTRIB_CALLBACKID	"DXca"
#define DX_LONG_NAME_CALLBACKID		"DXCC_NodeName_CallbackID"
#define DX_SHORT_NAME_CALLBACKID	"DXcn"


#define MAKE_INPUT(attr)								\
    attr.setKeyable(false);  attr.setStorable(true);	\
    attr.setReadable(true); attr.setWritable(true);

#define MAKE_OUTPUT(attr)								\
    attr.setKeyable(false); attr.setStorable(false);	\
    attr.setReadable(true); attr.setWritable(false);

#define MAYA_FAILED(_stat)		((_stat).error() ? (DXCC_DPFA_ERROR((_stat).errorString().asChar()), TRUE) : FALSE )
#define MAYA_SUCCEEDED(_stat)	((_stat).error() ? FALSE : TRUE )


MString MakeNameExportable(MString &in);

//-----------------------------------------------------------------------------
// Name: xfileTranslator (class)
// Desc: An interface class for our Maya XFile Exporter
//-----------------------------------------------------------------------------
class XFileTranslator: public MPxFileTranslator 
{
public:
	static void*		creator();
	virtual				~XFileTranslator ();	
	virtual MStatus		reader(const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode);
	virtual MStatus		writer(const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode);
	virtual bool		haveReadMethod() const;
	virtual bool		haveWriteMethod() const;
	virtual MString 	defaultExtension() const;
	virtual MString 	filter() const;
	virtual MFileKind	identifyFile( const MFileObject& fileName, const char* buffer, short size) const;
};


class DirectXShader : public MPxNode, public IDXCCEffectDialogCallbacks
{
public:
					DirectXShader(){};
					virtual			~DirectXShader(){};


	STDMETHOD(OnPostLoad)(THIS_  
	 	HWND hDialog, 
 		LPDIRECT3DDEVICE9 pDevice,
 		LPDXCCMANAGER pManager,
 		LPDXCCRESOURCE pResource,//recycle me
 		LPCSTR szNewEffectFilename);

	STDMETHOD(OnClose)(THIS_  
	 	HWND hDialog, 
 		LPDIRECT3DDEVICE9 pDevice,
 		LPDXCCMANAGER pManager,
 		LPDXCCRESOURCE pResource);






	static  void *	creator();
	virtual void	postConstructor(); 

	static  MStatus	initialize();

	virtual MStatus	compute( const MPlug&, MDataBlock& );
    virtual bool	setInternalValue( const MPlug&, const MDataHandle&);

	static HANDLE	FxDialogEvent;

	//DXCC STATICS
	static MObject  aDXCCCommand;
	static MObject  aDXCCHandle;
	//MAYA STATICS
	static MTypeId  id;// The IFF type id
	static MObject  aColor;// Surface color
	static MObject  aColorR;// Red component of surface color
	static MObject  aColorG;// Green component of surface color
	static MObject  aColorB;// Blue component of surface color
	static MObject  aOutColor;// Output color
	static MObject  aOutColorR;// Red component of output color
	static MObject  aOutColorG;// Green component of output color
	static MObject  aOutColorB;// Blue component of output color
	static MObject  aNormalCamera;// Surface normal
	static MObject  aNormalCameraX;// X component of surface normal
	static MObject  aNormalCameraY;// Y component of surface normal
	static MObject  aNormalCameraZ;// Z component of surface normal
};

class DXCCRebuildSceneCommand : public MPxCommand 
{							
public:															
	DXCCRebuildSceneCommand() {};								
	virtual MStatus	doIt ( const MArgList& args);
	static void*	creator(){ return new DXCCRebuildSceneCommand; }									
};		

class DCCRebuildSelectedCommand : public MPxCommand 
{							
public:															
	DCCRebuildSelectedCommand() {};								
	static void*	creator(){ return new DCCRebuildSelectedCommand; }									
	virtual MStatus	doIt ( const MArgList& args);			
};		

class DXCCExportSceneCommand : public MPxCommand 
{							
public:															
	DXCCExportSceneCommand() {};								
	static void*	creator(){ return new DXCCExportSceneCommand; }									
	virtual MStatus	doIt ( const MArgList& args);				
};	

class DXCCExportSelectedCommand : public MPxCommand 
{							
public:															
	DXCCExportSelectedCommand() {};								
	static void*	creator(){ return new DXCCExportSelectedCommand; }									
	virtual MStatus	doIt ( const MArgList& args);		
};	

class DXCCPreviewToggleCommand : public MPxCommand 
{							
public:															
	DXCCPreviewToggleCommand() {};								
	static void*	creator(){ return new DXCCPreviewToggleCommand; }									
	virtual MStatus	doIt ( const MArgList& args);				
};	

class DXCCGatherAnimationCommand : public MPxCommand 
{							
public:															
	DXCCGatherAnimationCommand() {};								
	static void*	creator(){ return new DXCCGatherAnimationCommand; }									
	virtual MStatus	doIt ( const MArgList& args);				
};	

class DXCCFrontViewCommand : public MPxCommand 
{							
public:															
	DXCCFrontViewCommand() {};								
	static void*	creator(){ return new DXCCFrontViewCommand; }									
	virtual MStatus	doIt ( const MArgList& args);			
};	

class DXCCSideViewCommand : public MPxCommand 
{							
public:															
	DXCCSideViewCommand() {};								
	static void*	creator(){ return new DXCCSideViewCommand; }									
	virtual MStatus	doIt ( const MArgList& args);			
};

class DXCCTopViewCommand : public MPxCommand 
{							
public:															
	DXCCTopViewCommand() {};								
	static void*	creator(){ return new DXCCTopViewCommand; }									
	virtual MStatus	doIt ( const MArgList& args);
};

class DXCCPerspectiveViewCommand : public MPxCommand 
{							
public:															
	DXCCPerspectiveViewCommand() {};								
	static void*	creator(){ return new DXCCPerspectiveViewCommand; }									
	virtual MStatus	doIt ( const MArgList& args);		
};

class DXCCFloatingViewCommand : public MPxCommand 
{							
public:															
	DXCCFloatingViewCommand() {};								
	static void*	creator(){ return new DXCCFloatingViewCommand; }									
	virtual MStatus	doIt ( const MArgList& args);
};


#endif //__MAYAINTERFACE_H__