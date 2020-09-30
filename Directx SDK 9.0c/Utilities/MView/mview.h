#pragma once

#ifndef __MVIEW_H__
#define __MVIEW_H__

/*//////////////////////////////////////////////////////////////////////////////
//
// File: mview.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

class TrivialData;
struct SDrawElement;
struct SFrame;

const DWORD x_typeTri = 3;
const DWORD x_typeQuad = 4;

enum METHOD {
    D3DNONINDEXED,
    D3DINDEXED,
    SOFTWARE,
    NONE
};

const int x_cpsnTexCoords = 8;

struct SMeshContainer //: D3DXMESHCONTAINER
{
    LPTSTR                  Name;

    D3DXMESHDATA            MeshData;

    //LPD3DXMATERIAL        pMaterials;
    LPD3DXMATERIAL          rgMaterials;
    LPD3DXEFFECTINSTANCE    pEffects;

    DWORD                   NumMaterials;
    //DWORD                  *pAdjacency;
    DWORD *rgdwAdjacency;

    LPD3DXSKININFO          pSkinInfo;

    //_D3DXMESHCONTAINER     *pNextMeshContainer;
    SMeshContainer         *pNextMeshContainer;


    ID3DXBaseMesh *ptmDrawMesh;
    LPD3DXMESH              pMesh;
    LPD3DXPMESH             pPMMesh;
    LPD3DXPATCHMESH         pPatchMesh;

    // arrays of strips - if present, then m_cAttributeGroups sized
    LPDIRECT3DINDEXBUFFER9 *m_rgpStrips;
    DWORD *m_rgcStripIndices;
    DWORD **m_rgrgcStripCounts;
    DWORD *m_rgcStrips;

    ID3DXSPMesh *pSimpMesh;

    ID3DXMesh *pMeshToTesselate;
    ID3DXMesh *pSWTesselatedMesh;

    DWORD *rgdwAdjacencyTesselate;

    D3DXATTRIBUTERANGE *m_rgaeAttributeTable;
    DWORD m_caeAttributeTable;

    LPD3DXEFFECT *m_rgpfxAttributes;
    SEffectInfo *m_rgEffectInfo;
    
    DWORD m_cAttributeGroups;

    DWORD iAttrSplit;

    BOOL bPMMeshMode;
    BOOL bSimplifyMode;
    BOOL bTesselateMode;
    BOOL bNPatchMode;

    ULONG m_cNumVertices;
    ULONG m_cMaxVerticesSoft;
    ULONG m_cMinVerticesSoft;

    // used for looking at strips
    CStripOutline m_soStrips;
    CAdjacencyOutline m_aoAdjacency;
    CEdgeOutline m_eoEdges;
    CShowNormals m_snNormals;
    CShowNormals *m_rgpsnTexCoords[x_cpsnTexCoords];
    CNPatchOutline m_npoNPatchOutline;

    D3DXVECTOR3 m_vBoundingBoxMin;
    D3DXVECTOR3 m_vBoundingBoxMax;

    // Skin info
    LPD3DXMESH  m_pSkinnedMesh;         // stores the mesh after skinning has been applied
    DWORD m_dwSkinningType;
    LPD3DXMESH  m_pOrigMesh;
    D3DXMATRIX *pBoneOffsetMatrices;
    D3DXMATRIX** m_pBoneMatrix;
    DWORD m_numBoneComb;
    DWORD m_maxFaceInfl;
    LPD3DXBUFFER m_pBoneCombinationBuf;
    METHOD  m_Method;
    BOOL  m_bOverrideMethod;
    DWORD m_iPaletteSize;

    UINT cTesselateLevel;


    // variables used during save to store off some information that normally is in the D3DXMESHCONTAINER data section
    //    but cannot be there during the save call because it would save the wrong data
    LPD3DXMESH m_pMeshBackup;
    DWORD     *m_rgdwAdjacencyBackup;


    SMeshContainer()
        :ptmDrawMesh(NULL),
            pPMMesh(NULL),
            m_rgpStrips(NULL),
            m_rgcStripIndices(0),
            pSimpMesh(NULL),
            pMeshToTesselate(NULL),
            pSWTesselatedMesh(NULL),
            pMesh(NULL),
            pPatchMesh(NULL),
            rgdwAdjacency(NULL),
            rgdwAdjacencyTesselate(NULL),
            m_rgaeAttributeTable(NULL),
            m_caeAttributeTable(0),
            m_rgpfxAttributes(NULL),
            m_rgEffectInfo(NULL),
            rgMaterials(NULL),
            pEffects(NULL),
            m_cAttributeGroups(0),
            iAttrSplit(0),
            bPMMeshMode(FALSE),
            bSimplifyMode(FALSE),
            bTesselateMode(FALSE),
            bNPatchMode(FALSE),
            m_cNumVertices(0),
            m_cMaxVerticesSoft(0),
            m_cMinVerticesSoft(0),
            m_pSkinnedMesh(NULL),
            m_dwSkinningType(D3DXST_UNSKINNED),
            m_pOrigMesh(NULL),
            pBoneOffsetMatrices(NULL),
            m_pBoneMatrix(NULL),
            m_numBoneComb(0),
            m_maxFaceInfl(0),
            m_pBoneCombinationBuf(NULL),
            m_Method(NONE),
            m_bOverrideMethod(FALSE),
            m_iPaletteSize(0),
            cTesselateLevel(1),
            m_pMeshBackup(NULL),
            m_rgdwAdjacencyBackup(NULL)
    {
        memset(this, 0, sizeof(D3DXMESHCONTAINER));

        // initialize mesh data structure, only used for saving
        MeshData.Type = D3DXMESHTYPE_MESH;
        MeshData.pPMesh = 0;

        memset(m_rgpsnTexCoords,0, sizeof(CShowNormals*)*x_cpsnTexCoords);
    }

    ~SMeshContainer()
    {
        DWORD cBones, iBone;

        for (DWORD iMaterial = 0; m_rgpfxAttributes != NULL && iMaterial < NumMaterials; iMaterial++)
        {
            GXRELEASE(m_rgpfxAttributes[iMaterial]);
        }

        for (DWORD iMaterial = 0; rgMaterials != NULL && iMaterial < NumMaterials; iMaterial++)
        {
            delete []rgMaterials[iMaterial].pTextureFilename;
        }

        for (DWORD iMaterial = 0; pEffects != NULL && iMaterial < NumMaterials; iMaterial++)
        {
            delete []pEffects[iMaterial].pEffectFilename;
            for (DWORD iDefault = 0; iDefault < pEffects[iMaterial].NumDefaults; iDefault++)
            {
                delete []pEffects[iMaterial].pDefaults[iDefault].pParamName;
                delete []pEffects[iMaterial].pDefaults[iDefault].pValue;
            }

            delete []pEffects[iMaterial].pDefaults;
        }

        for (DWORD iMaterial = 0; (m_rgpStrips != NULL) && iMaterial < m_cAttributeGroups; iMaterial++)
        {
            GXRELEASE(m_rgpStrips[iMaterial]);
        }

        // delete any allocated tex coord
        for (DWORD iTexCoord = 0; iTexCoord < x_cpsnTexCoords; iTexCoord++)
        {
            delete m_rgpsnTexCoords[iTexCoord];
            m_rgpsnTexCoords[iTexCoord] = NULL;
        }

        delete []m_rgpStrips;
        delete []m_rgcStripIndices;

        delete []pBoneOffsetMatrices;

        GXRELEASE(ptmDrawMesh);
        GXRELEASE(pMesh);
        GXRELEASE(pPatchMesh);
        GXRELEASE(pPMMesh);
        GXRELEASE(pSimpMesh);
        GXRELEASE(m_pOrigMesh);
        GXRELEASE(pMeshToTesselate);
        GXRELEASE(pSWTesselatedMesh);
        GXRELEASE(pSkinInfo);
        GXRELEASE(m_pSkinnedMesh);
        GXRELEASE(m_pBoneCombinationBuf);

        delete [] m_pBoneMatrix;

        delete []rgdwAdjacency;
        delete []rgdwAdjacencyTesselate;

        delete []rgMaterials;
        delete []m_rgEffectInfo;
        delete []pEffects;
        delete []m_rgpfxAttributes;
        delete []m_rgaeAttributeTable;

        delete []Name;

        delete []m_rgdwAdjacencyBackup;
        GXRELEASE(m_pMeshBackup);

        // UNDONE UNDONE - once frame destroy used, remove this
        delete (SMeshContainer*)pNextMeshContainer;
    }

    HRESULT UpdateViews(SDrawElement *pde);
    HRESULT UpdateSkinInfo();
};

struct SFrame //: D3DXFRAME --- NOTE: SFrame data members overlap with D3DXFRAME!!
{
    //LPTSTR                  Name;
    char *szName;
    //D3DXMATRIX              TransformationMatrix;
    D3DXMATRIX matRot;

    //LPD3DXMESHCONTAINER     pMeshContainer;
    SMeshContainer *pmcMesh;

    //_D3DXFRAME             *pFrameSibling;
    SFrame *pframeSibling;
    //_D3DXFRAME             *pFrameFirstChild;
    SFrame *pframeFirstChild;

    D3DXMATRIX matTrans;
    D3DXMATRIX matRotOrig;
    D3DXMATRIX matCombined;


    HTREEITEM m_hTreeHandle;

    SFrame()
        :   pmcMesh(NULL),
            pframeSibling(NULL),
            pframeFirstChild(NULL),
            szName(NULL),
            m_hTreeHandle(NULL)
    {
        D3DXMatrixIdentity(&matRot);
        D3DXMatrixIdentity(&matRotOrig);
        D3DXMatrixIdentity(&matTrans);
    }

    ~SFrame()
    {
        delete []szName;
        delete pmcMesh;        

        SFrame *pToDelete;

        // non-recursive delete
        while (NULL != pframeFirstChild)
        {
            pToDelete = pframeFirstChild;
            pframeFirstChild = pToDelete->pframeFirstChild;
            pToDelete->pframeFirstChild = NULL;

            delete pToDelete;
        }

        // non-recursive delete
        while (NULL != pframeSibling)
        {
            pToDelete = pframeSibling;
            pframeSibling = pToDelete->pframeSibling;
            pToDelete->pframeSibling = NULL;

            delete pToDelete;
        }
    }

    SFrame *FindFrame(char *szFrame)
    {
        SFrame *pframe;

        if ((szName != NULL) && (strcmp(szName, szFrame) == 0))
            return this;

        if (pframeFirstChild != NULL)
        {
            pframe = pframeFirstChild->FindFrame(szFrame);
            if (pframe != NULL)
                return pframe;
        }

        if (pframeSibling != NULL)
        {
            pframe = pframeSibling->FindFrame(szFrame);
            if (pframe != NULL)
                return pframe;
        }

        return NULL;
    }

    BOOL PMeshPresent()
    {
        BOOL bResult;

        SMeshContainer *pmc = pmcMesh;
        while (pmc != NULL)
        {
            if (pmc->pPMMesh != NULL)
                return TRUE;

            pmc = pmc->pNextMeshContainer;
        }

        if (pframeFirstChild != NULL)
        {
            bResult = pframeFirstChild->PMeshPresent();
            if (bResult)
                return bResult;
        }

        if (pframeSibling != NULL)
        {
            bResult = pframeSibling->PMeshPresent();
            if (bResult)
                return bResult;
        }

        return FALSE;
    }

    void ResetMatrix()
    {
        matRot = matRotOrig;
        D3DXMatrixIdentity(&matTrans);        

        if (pframeFirstChild != NULL)
        {
            pframeFirstChild->ResetMatrix();
        }

        if (pframeSibling != NULL)
        {
            pframeSibling->ResetMatrix();
        }
    }

    void AddFrame(SFrame *pframe)
    {
        GXASSERT(pframe != NULL);

        if (pframeFirstChild == NULL)
        {
            pframeFirstChild = pframe;
        }
        else
        {
            pframe->pframeSibling = pframeFirstChild->pframeSibling;
            pframeFirstChild->pframeSibling = pframe;
        }
    }

    void AddMesh(SMeshContainer *pmc)
    {
        GXASSERT(pmc != NULL);

        pmc->pNextMeshContainer = pmcMesh;
        pmcMesh = (SMeshContainer*)pmc;
    }
};

struct SDrawElement
{
    SFrame *pframeRoot;

    D3DXVECTOR3 vCenter;
    float fRadius;

    // name of element for selection purposes
    char *szName;

    // animation list
    LPD3DXANIMATIONCONTROLLER m_pAnimMixer;
    DWORD m_iSelectedAnimSet;

    // next element in list
    SDrawElement *pdeNext;

    float fCurTime;
    float fMaxTime;
    DWORD dwTexCoordsCalculated;

    BOOL bSkinnedMeshInHeirarchy;

        SDrawElement()
            :vCenter(0.0,0.0,0.0),
                fRadius(1.0),
                szName(NULL),
                pframeRoot(NULL),
                m_pAnimMixer(NULL),
                m_iSelectedAnimSet(0),
                pdeNext(NULL),
                dwTexCoordsCalculated(0),
                bSkinnedMeshInHeirarchy(FALSE)
    {
    }
    
    ~SDrawElement()
    {
        delete pframeRoot;
        delete pdeNext;

        delete []szName;

        GXRELEASE(m_pAnimMixer);
    }

    SFrame *FindFrame(char *szName)
    {
        if (pframeRoot == NULL)
            return NULL;
        else
            return pframeRoot->FindFrame(szName);
    }
};

HRESULT CalculateBoundingSphere(SDrawElement *pdeCur);

struct STextureCache
{
    char                    *szFilename;
    LPDIRECT3DBASETEXTURE9  pTexture;

    STextureCache           *pNext;

    STextureCache()
        :szFilename(NULL), pTexture(NULL), pNext(NULL) {}

    ~STextureCache()
    {
        delete []szFilename;
        GXRELEASE(pTexture);

        delete pNext;
    }
};

const DWORD x_iDefaultSkinningPaletteSize = 28;

class TrivialData : public CD3DXApplication
{
    friend LRESULT CALLBACK DlgProcCreateText(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
public:
    TrivialData( );
    ~TrivialData( );

    void InitializeHelper();

    // Don't hurt anything, just stop drawing
    void PauseDrawing( );
    void RestartDrawing( );

    // D3DX App functions
    virtual BOOL AreCapsSufficient(D3DCAPS9 *pCaps, DWORD dwBehaviorFlags);

    virtual LRESULT OnMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT WINAPI WndProcCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    virtual HRESULT OnUpdate(float fSecsPerFrame);
    HRESULT UpdateAnimation(SDrawElement *pde, float fSecsPerFrame);


    virtual HRESULT OnDraw(float fAspect);
    HRESULT DrawSubset(LPD3DXBASEMESH pMesh, LPD3DXEFFECT pEffect, SEffectInfo *pEffectInfo, DWORD ipattr, SMeshContainer *pmcMesh, BOOL bSelected, D3DXMATRIX *rgmIndexedMatrices, DWORD cIndexedMatrices);
    HRESULT DrawMeshContainer(SMeshContainer *pmcMesh, D3DXMATRIX *pmatWorld);
    HRESULT DrawFrames(SFrame *pframeCur, UINT &cTriangles, UINT &cVertices);
    HRESULT UpdateFrames(SFrame *pframeCur, D3DXMATRIX &matCur);

    HRESULT FindBones(SFrame *pframeCur, SDrawElement *pde);

    //virtual HRESULT OnCreateSurfaces();
    virtual HRESULT OnCreateDevice();
    virtual HRESULT OnResetDevice();
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnDestroyDevice();

    HRESULT RestoreSurfaces( );
    HRESULT ReloadTextures( );
    HRESULT ReleaseTextures( );

    HRESULT LoadNewMesh( );
    HRESULT LoadMesh( TCHAR* file, BOOL bFlattenHierarchy );
    HRESULT LoadMeshHierarchyFromFile(char *szFilename);
    HRESULT SaveMeshHierarchyToFile(SDrawElement *pde, SFrame *pframeRoot, char *szFilename, DWORD xFormat);
    HRESULT LoadNewProgressiveMesh( );
    HRESULT SaveMesh( );
    HRESULT SavePMesh( );  //No longer needed with save hierarchy supporting pmeshes
    HRESULT SetProjectionMatrix();

    HRESULT GeneratePM(void);

    HRESULT NewTexture(char *szTexName, LPDIRECT3DTEXTURE9 *pptex);
    HRESULT NewCubeTexture(char *szTexName, LPDIRECT3DCUBETEXTURE9 *pptex);

    HRESULT ConvertMeshToSimplify();

    void NormalizeNormals();
    HRESULT RemoveBackToBackTris();
    HRESULT RemoveAllMeshesExceptSelectedFromFrame(SFrame *pframeCur);
    HRESULT RemoveAllMeshesExceptSelected();

    HRESULT SimulateCacheStuff();
    HRESULT DisplayCacheBehavior();
    HRESULT BuildCacheHistogram();


    void ToggleCullMode();
    void ToggleLightMode();
    void ToggleEdgeMode();
    void ToggleSolidMode();
    void ToggleNPatchEdgeMode();
    void ToggleNoSelectionMode();
    void ToggleFaceSelectionMode();
    void ToggleVertexSelectionMode();
    void ToggleMeshSelectionMode();
    void ToggleShowMeshSelectionMode();
    void ToggleStripMode();
    void ToggleAdjacencyMode();
    void ToggleCreaseMode();
    void ToggleNormalsMode();
    void ToggleShowTexCoord(DWORD iTexCoord);
    void ToggleWireframeMode();
    void ChangeSkinningMode(METHOD method);
    void ToggleTreeView();
    void ChangeAnimMode(BOOL bPause);
    void TogglePlaybackSpeed();


    void ToggleTextureMode();

    void AdjustScrollbar();
    void SetNumVertices(int cVerticesChange, bool bAbsolute);
    void SetNumFaces(int cFacesChange, bool bAbsolute);
    void ConvertSelectedMesh(DWORD dwOptions, LPD3DVERTEXELEMENT9 pDecl);

    void SetSoftMinLOD();
    void SetSoftMaxLOD();
    void ResetSoftMinLOD();
    void ResetSoftMaxLOD();
    void TrimPMeshToSoftLimits();
    void SnapshotSelected();
    void MergeMeshes();

    HRESULT Tesselate(SMeshContainer *pmc, BOOL bSkipAdjacencyAndEdges);
    HRESULT TesselateFrame(SFrame *pframe);
    //void FixNPatchCreases();
    HRESULT SplitMesh();
    HRESULT ComputeSecondaryNormals();

    HRESULT WeldVertices();
    HRESULT TestSimplify();

#if 0
    HRESULT ApplyEffectsToMesh(SMeshContainer *pmcMesh);
#endif

public:
//private:
    HRESULT CreateText();
    HRESULT CreatePolygon();
    HRESULT CreateBox();
    HRESULT CreateCylinder();
    HRESULT CreateTorus();
    HRESULT CreateTeapot();
    HRESULT CreateSphere();
    HRESULT CreateCone();

    HRESULT SelectTextFont();

    void Optimize(DWORD dwFlags);
    void CleanMesh();

    void ValidateMesh();
    void ComputeNormals();

    void Displace();

    HRESULT UpdateAnimationsMenu();
    void SwitchToAnimationSet(UINT iAnimSet);
    void AddAnimation();

    HRESULT UpdateMeshMenu();
    HRESULT UpdateTreeInfo(SFrame *pframeCur, HTREEITEM htreeParent);

    HRESULT SelectFrame(SFrame *pframeNew, SMeshContainer *pmcMesh);
    HRESULT SelectFace(DWORD dwFace);
    HRESULT SelectVertex(DWORD dwVertexs);
    void SelectDrawElement(SDrawElement *pdeNew);
    //void SwitchToMeshID(UINT iMeshID);
    void SelectionChange(DWORD dwX, DWORD dwY);
    void UpdateSelectionInfo();
    void SetupMenu();

    HRESULT AddMeshToDrawList(char *szName, LPD3DXMESH ptmMesh, LPD3DXPMESH ptmPMesh, LPD3DXBUFFER pbufAdjacency, LPD3DXBUFFER pbufMaterials, LPD3DXBUFFER pbufEffectInstances, UINT cMaterials);
    HRESULT DeleteSelectedMesh();

    // Objects created by Initialize and retained
    HMENU m_hMenu;
    HMENU m_hMeshMenu;
    HMENU m_hAnimationsMenu;

    HRESULT UpdateFrameRate(float fSecsPerFrame);
    HRESULT UpdateStatusBar(DWORD cTotalTriangles, DWORD cTriangles, DWORD cVertices);


    // Transient objects that get affected by PrepareToDraw
public:
    BOOL m_bHWVertexShaders;
    BOOL m_bHWNPatches;
    BOOL m_bSoftwareVP;

    BOOL m_bTextureMode;
    BOOL m_bEdgeMode;
    BOOL m_bNPatchEdgeMode;
    BOOL m_bStripMode;
    BOOL m_bAdjacencyMode;
    BOOL m_bCreaseMode;
    BOOL m_bNormalsMode;
    BOOL m_bWireframeMode;
    BOOL m_bFaceSelectionMode;
    BOOL m_bVertexSelectionMode;
    BOOL m_bShowMeshSelectionMode;
    BOOL m_bAnimPaused;
    BOOL m_bLighting;
    BOOL m_bUpdatedNormalDuringMouseMove;
    DWORD m_dwTexCoordsShown;       // bit field for currently visible tex coords 0 - texcoord 0, 1 - 1, etc.

    DWORD m_dwCullMode;

    DWORD m_dwFVF;

    // member variables for frame rate display
    HFONT m_hFontText;
    DWORD m_rgbFontColor;
    char  m_szText[256];
    float m_fExtrusion;
    float m_fDeviation;
    SIZE m_sizeClient;

    D3DXVECTOR2 m_vCurMousePosition;       // previous mouse position    
    GXArcBall m_abArcBall;

    DWORD m_dwFaceSelected;
    DWORD m_dwFaceSelectedAttr;
    DWORD m_dwVertexSelected;
    SMeshContainer *m_pmcSelectedMesh;
    SFrame *m_pframeSelected;
    SDrawElement *m_pdeSelected;
    SDrawElement *m_pdeHead;

    LPD3DXEFFECT m_pfxFaceSelect;
    LPD3DXEFFECT m_pfxVertSelect;
    LPD3DXEFFECT m_pfxShowNormals;
    LPD3DXEFFECT m_pfxSkinnedAdjacency;

    CFrameView *m_pfvTreeView;
    STextureCache *m_ptcTextureCacheHead;

    PVOID pvDialogData;

    DWORD m_maxFaceInflHW;
    METHOD m_method;
    DWORD   m_iPaletteSize;

    HWND m_hwndStatus;
    HWND m_hwndToolbar;

    BOOL m_bDisableHWNPatches;
    BOOL m_bForceSoftwareMode;
    BOOL m_bForceRefRast;
    BOOL m_bInitialFilename;
    char m_szInitialFilename[256];

    FLOAT m_fTicksPerSecond;

    FLOAT m_fFramesPerSecCur;
    FLOAT m_fFramesPerSec;
    FLOAT m_fLastUpdateTime;
    FLOAT m_fTime;

    CShowArcball m_ShowArcball;

    // temporary memory for use at Draw time
    D3DXMATRIX m_mMatrices[x_iDefaultSkinningPaletteSize];
};

const DWORD x_rgStatusBarSizes[5] = {90, 60, 70, 60, 80};//{280, 370, 430, 500, 560, 640};
const DWORD x_cStatusBarSizes = 5;
extern TrivialData *g_pData;

#endif
