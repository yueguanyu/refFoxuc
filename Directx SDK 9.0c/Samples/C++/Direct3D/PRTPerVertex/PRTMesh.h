//--------------------------------------------------------------------------------------
// File: PRTOptionsDlg.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

class CPRTMesh
{
public:
    CPRTMesh(void);
    ~CPRTMesh(void);

    HRESULT OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );  
    HRESULT OnResetDevice(); 
    void    OnLostDevice(); 
    void    OnDestroyDevice(); 

    void    RenderWithPRT( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, bool bRenderWithAlbedoTexture );
    void    RenderWithSHIrradEnvMap( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, bool bRenderWithAlbedoTexture );
    void    RenderWithNdotL( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, D3DXMATRIX* pmWorldInv, bool bRenderWithAlbedoTexture, CDXUTDirectionWidget* aLightControl, int nNumLights, float fLightScale );

    HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strMeshFileName );
    HRESULT SetMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMesh );
    DWORD   GetNumVertices()    { return m_pMesh->GetNumVertices(); }
    ID3DXMesh* GetMesh()        { return m_pMesh; }
    D3DXMATERIAL* GetMaterials()  { return m_pMaterials; }
    DWORD GetNumMaterials()  { return m_dwNumMaterials; }
    bool    IsMeshLoaded()      { return (m_pMesh != NULL); }
    IDirect3DTexture9* GetAlbedoTexture() { return m_pAlbedoTextures[0]; }
    float   GetObjectRadius()   { return m_fObjectRadius; }
    const D3DXVECTOR3& GetObjectCenter() { return m_vObjectCenter; }

    HRESULT LoadPRTBufferFromFile( WCHAR* strFile );
    HRESULT LoadCompPRTBufferFromFile( WCHAR* strFile );
    void    SetPRTBuffer( ID3DXPRTBuffer* pPRTBuffer, WCHAR* strFile );
    void    CompressBuffer( D3DXSHCOMPRESSQUALITYTYPE Quality, UINT NumClusters, UINT NumPCA );
    DWORD   GetOrder() { return m_dwOrder; }
    bool    IsUncompressedBufferLoaded() { return (m_pPRTBuffer != NULL); }
    bool    IsCompBufferLoaded() { return (m_pPRTCompBuffer != NULL); }
    ID3DXPRTCompBuffer* GetCompBuffer() { return m_pPRTCompBuffer; }

    void    ExtractCompressedDataForPRTShader();
    bool    IsShaderDataExtracted() { return (m_aClusterBases != NULL); }

    HRESULT LoadEffects( IDirect3DDevice9* pd3dDevice, const D3DCAPS9* pDeviceCaps );
    bool    IsPRTEffectLoaded() { return (m_pPRTEffect != NULL); }
   
    void    ComputeSHIrradEnvMapConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue );
    void    ComputeShaderConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue, DWORD dwNumCoeffsPerChannel );

    void    Cleanup()       { SAFE_RELEASE(m_pMesh); SAFE_RELEASE(m_pPRTBuffer); SAFE_RELEASE(m_pPRTCompBuffer); }

protected:
    struct RELOAD_STATE
    {
        bool  bUseReloadState;
        bool  bLoadCompressed;
        WCHAR strMeshFileName[MAX_PATH];
        WCHAR strPRTBufferFileName[MAX_PATH];
        D3DXSHCOMPRESSQUALITYTYPE quality;
        UINT dwNumClusters;
        UINT dwNumPCA;
    } m_ReloadState;

    ID3DXMesh*          m_pMesh;
    CGrowableArray<IDirect3DTexture9*> m_pAlbedoTextures;
    D3DXMATERIAL*       m_pMaterials;
    ID3DXBuffer*        m_pMaterialBuffer;
    DWORD               m_dwNumMaterials;
    float               m_fObjectRadius;
    D3DXVECTOR3         m_vObjectCenter;

    ID3DXPRTBuffer*     m_pPRTBuffer;
    ID3DXPRTCompBuffer* m_pPRTCompBuffer;
    ID3DXEffect*        m_pPRTEffect;       
    ID3DXEffect*        m_pSHIrradEnvMapEffect;       
    ID3DXEffect*        m_pNDotLEffect;       
    DWORD               m_dwOrder;

    // The basis buffer is a large array of floats where 
    // Call D3DXSHPRTCompExtractBasis() to extract the basis 
    // for every cluster.  The basis for a cluster is an array of
    // (NumPCAVectors+1)*(NumChannels*Order^2) floats. 
    // The "1+" is for the cluster mean.
    float* m_aClusterBases;

    // m_aPRTConstants stores the incident radiance dotted with the transfer function.
    // Each cluster has an array of floats which is the size of 
    // 4+MAX_NUM_CHANNELS*NUM_PCA_VECTORS. This number comes from: there can 
    // be up to 3 channels (R,G,B), and each channel can 
    // have up to NUM_PCA_VECTORS of PCA vectors.  Each cluster also has 
    // a mean PCA vector which is described with 4 floats (and hence the +4).
    float* m_aPRTConstants;

    HRESULT AdjustMeshDecl( IDirect3DDevice9* pd3dDevice, ID3DXMesh** ppMesh );

    UINT GetOrderFromNumCoeffs( UINT dwNumCoeffs );
};
