//--------------------------------------------------------------------------------------
// File: PRTOptionsDlg.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "prtmesh.h"
#include <stdio.h>

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 



//--------------------------------------------------------------------------------------
CPRTMesh::CPRTMesh(void)
{
    m_pMesh          = NULL;
    m_pPRTBuffer     = NULL;
    m_pPRTCompBuffer = NULL;
    m_pPRTEffect    = NULL;       
    m_pSHIrradEnvMapEffect = NULL;       
    m_pNDotLEffect  = NULL;       
    m_fObjectRadius = 0.0f;
    m_vObjectCenter = D3DXVECTOR3(0,0,0);
    m_aClusterBases = NULL;
    m_dwOrder       = 0;
    m_aPRTConstants = NULL;
    m_pMaterials    = NULL;
    m_dwNumMaterials = 0;

    ZeroMemory( &m_ReloadState, sizeof(RELOAD_STATE) );
}


//--------------------------------------------------------------------------------------
CPRTMesh::~CPRTMesh(void)
{
    Cleanup();
}


//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( m_ReloadState.bUseReloadState )
    {
        LoadMesh( pd3dDevice, m_ReloadState.strMeshFileName );
        if( m_ReloadState.bLoadCompressed )
        {
            LoadCompPRTBufferFromFile( m_ReloadState.strPRTBufferFileName );
        }
        else
        {
            LoadPRTBufferFromFile( m_ReloadState.strPRTBufferFileName );
            CompressBuffer( m_ReloadState.quality, m_ReloadState.dwNumClusters, m_ReloadState.dwNumPCA );
        }
        ExtractCompressedDataForPRTShader();
        LoadEffects( pd3dDevice, DXUTGetDeviceCaps() );
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::OnResetDevice()
{
    HRESULT hr;
    if( m_pPRTEffect )
        V( m_pPRTEffect->OnResetDevice() );
    if( m_pSHIrradEnvMapEffect )
        V( m_pSHIrradEnvMapEffect->OnResetDevice() );
    if( m_pNDotLEffect )
        V( m_pNDotLEffect->OnResetDevice() );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing 
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strMeshFileName )
{
    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Release any previous mesh object
    SAFE_RELEASE(m_pMesh);
    SAFE_RELEASE(m_pMaterialBuffer);
    for(int i=0; i<m_pAlbedoTextures.GetSize(); i++ )
    {
        SAFE_RELEASE( m_pAlbedoTextures[i] );
    }
    m_pAlbedoTextures.RemoveAll();

    // Load the mesh object
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strMeshFileName ) );
    wcscpy( m_ReloadState.strMeshFileName, str );
    V_RETURN( D3DXLoadMeshFromX( str, D3DXMESH_MANAGED, pd3dDevice, NULL, 
                                 &m_pMaterialBuffer, NULL, &m_dwNumMaterials, &m_pMesh) );
    m_pMaterials = (D3DXMATERIAL*)m_pMaterialBuffer->GetBufferPointer();

    // Change the current directory to the mesh's directory so we can
    // find the textures.
    WCHAR* pLastSlash = wcsrchr( str, L'\\' );
    if( pLastSlash )
        *(pLastSlash + 1) = 0;
    WCHAR strCWD[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, strCWD );
    SetCurrentDirectory( str );

    // Lock the vertex buffer to get the object's radius & center
    // simply to help position the camera a good distance away from the mesh.
    IDirect3DVertexBuffer9* pVB = NULL;
    void* pVertices;
    V_RETURN( m_pMesh->GetVertexBuffer( &pVB ) );
    V_RETURN( pVB->Lock( 0, 0, &pVertices, 0 ) );

    D3DVERTEXELEMENT9 Declaration[MAXD3DDECLLENGTH + 1];
    m_pMesh->GetDeclaration( Declaration );
    DWORD dwStride = D3DXGetDeclVertexSize( Declaration, 0 );
    V_RETURN( D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, m_pMesh->GetNumVertices(), 
                                          dwStride, &m_vObjectCenter, 
                                          &m_fObjectRadius ) );

    pVB->Unlock();
    SAFE_RELEASE( pVB );

    // Make the mesh have a known decl in order to pass per vertex CPCA 
    // data to the shader
    V_RETURN( AdjustMeshDecl( pd3dDevice, &m_pMesh ) );

    // Optimize the mesh for this graphics card's vertex cache 
    // so when rendering the mesh's triangle list the vertices will 
    // cache hit more often so it won't have to re-execute the vertex shader 
    // on those vertices so it will improve perf.     
    DWORD *rgdwAdjacency = NULL;
    rgdwAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( m_pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( m_pMesh->OptimizeInplace( D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_IGNOREVERTS, 
                                 rgdwAdjacency, NULL, NULL, NULL) );
    delete []rgdwAdjacency;

    for(UINT i=0; i<m_dwNumMaterials; i++ )
    {
        // First attempt to look for texture in the same folder as the input folder.
        WCHAR strTextureTemp[MAX_PATH];
        MultiByteToWideChar( CP_ACP, 0, m_pMaterials[i].pTextureFilename, -1, strTextureTemp, MAX_PATH );
        strTextureTemp[MAX_PATH-1] = 0;

        // Create the mesh texture from a file
        if( SUCCEEDED( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strTextureTemp ) ) )
        {
            IDirect3DTexture9* pAlbedoTexture;
            V( D3DXCreateTextureFromFileEx( pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
                                            D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
                                            D3DX_DEFAULT, D3DX_DEFAULT, 0, 
                                            NULL, NULL, &pAlbedoTexture ) );
            m_pAlbedoTextures.Add( pAlbedoTexture );
        }
        else
        {
            m_pAlbedoTextures.Add( NULL );
        }
    }
   
    SetCurrentDirectory( strCWD );

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::SetMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMesh )
{
    HRESULT hr;

    // Release any previous mesh object
    SAFE_RELEASE(m_pMesh);

    m_pMesh = pMesh;

    V( AdjustMeshDecl( pd3dDevice, &m_pMesh ) );

    // Sort the attributes
    DWORD *rgdwAdjacency = NULL;
    rgdwAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( m_pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency) );
    V( m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_IGNOREVERTS, 
                                rgdwAdjacency, NULL, NULL, NULL) );
    delete []rgdwAdjacency;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Make the mesh have a known decl in order to pass per vertex CPCA 
// data to the shader
//-----------------------------------------------------------------------------
HRESULT CPRTMesh::AdjustMeshDecl( IDirect3DDevice9* pd3dDevice, ID3DXMesh** ppMesh )
{
    HRESULT hr;
    LPD3DXMESH pInMesh = *ppMesh;
    LPD3DXMESH pOutMesh = NULL;

    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE] = 
    {
        {0,  0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0,  32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        {0,  36, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1},
        {0,  52, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 2},
        {0,  68, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 3},
        {0,  84, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 4},
        {0, 100, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 5},
        {0, 116, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 6},
        D3DDECL_END()
    };

    // To do CPCA, we need to store (g_dwNumPCAVectors + 1) scalers per vertex, so 
    // make the mesh have a known decl to store this data.  Since we can't do 
    // skinning and PRT at once, we use D3DDECLUSAGE_BLENDWEIGHT[0] 
    // to D3DDECLUSAGE_BLENDWEIGHT[6] to store our per vertex data needed for PRT.
    // Notice that D3DDECLUSAGE_BLENDWEIGHT[0] is a float1, and
    // D3DDECLUSAGE_BLENDWEIGHT[1]-D3DDECLUSAGE_BLENDWEIGHT[6] are float4.  This allows 
    // up to 24 PCA weights and 1 float that gives the vertex shader 
    // an index into the vertex's cluster's data
    V( pInMesh->CloneMesh( pInMesh->GetOptions(), decl, pd3dDevice, &pOutMesh ) );

    // Make sure there are normals which are required for lighting
    if( !(pInMesh->GetFVF() & D3DFVF_NORMAL) )
        V( D3DXComputeNormals( pOutMesh, NULL ) );

    SAFE_RELEASE( pInMesh );

    *ppMesh = pOutMesh;

    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::LoadPRTBufferFromFile( WCHAR* strFile )
{
    HRESULT hr;
    SAFE_RELEASE( m_pPRTBuffer );
    SAFE_RELEASE( m_pPRTCompBuffer );

    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFile ) );
    wcscpy( m_ReloadState.strPRTBufferFileName, str );

    V( D3DXLoadPRTBufferFromFile( str, &m_pPRTBuffer ) );
    m_dwOrder = GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CPRTMesh::LoadCompPRTBufferFromFile( WCHAR* strFile )
{
    HRESULT hr;
    SAFE_RELEASE( m_pPRTBuffer );
    SAFE_RELEASE( m_pPRTCompBuffer );

    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFile ) );
    wcscpy( m_ReloadState.strPRTBufferFileName, str );

    V( D3DXLoadPRTCompBufferFromFile( str, &m_pPRTCompBuffer ) );
    m_ReloadState.bUseReloadState = true;
    m_ReloadState.bLoadCompressed = true;
    m_dwOrder = GetOrderFromNumCoeffs( m_pPRTCompBuffer->GetNumCoeffs() );
    return S_OK;
}


//--------------------------------------------------------------------------------------
void CPRTMesh::CompressBuffer( D3DXSHCOMPRESSQUALITYTYPE Quality, UINT NumClusters, UINT NumPCA )
{ 
    HRESULT hr;
    assert( m_pPRTBuffer != NULL );
    SAFE_RELEASE(m_pPRTCompBuffer);
    V( D3DXCreatePRTCompBuffer( Quality, NumClusters, NumPCA, m_pPRTBuffer, &m_pPRTCompBuffer ) );
    m_ReloadState.quality = Quality;
    m_ReloadState.dwNumClusters = NumClusters;
    m_ReloadState.dwNumPCA = NumPCA;
    m_ReloadState.bUseReloadState = true;
    m_ReloadState.bLoadCompressed = false;
    m_dwOrder = GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::SetPRTBuffer( ID3DXPRTBuffer* pPRTBuffer, WCHAR* strFile ) 
{ 
    SAFE_RELEASE(m_pPRTBuffer);
    SAFE_RELEASE(m_pPRTCompBuffer);
    m_pPRTBuffer = pPRTBuffer;
    m_dwOrder = GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
    wcsncpy( m_ReloadState.strPRTBufferFileName, strFile, MAX_PATH );
    m_ReloadState.strPRTBufferFileName[MAX_PATH - 1] = L'\0';
}


//-----------------------------------------------------------------------------
HRESULT CPRTMesh::LoadEffects( IDirect3DDevice9* pd3dDevice, const D3DCAPS9* pDeviceCaps )
{
    HRESULT hr;

    UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
    UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
    UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

    // The number of vertex consts need by the shader can't exceed the 
    // amount the HW can support
    DWORD dwNumVConsts = dwNumClusters * (1 + dwNumChannels*dwNumPCA/4) + 4;
    if( dwNumVConsts > pDeviceCaps->MaxVertexShaderConst )
        return E_FAIL;

    SAFE_RELEASE( m_pPRTEffect );
    SAFE_RELEASE( m_pSHIrradEnvMapEffect );
    SAFE_RELEASE( m_pNDotLEffect );

    D3DXMACRO aDefines[3];
    CHAR szMaxNumClusters[64];
    sprintf( szMaxNumClusters, "%d", dwNumClusters );
    szMaxNumClusters[63] = 0;
    CHAR szMaxNumPCA[64];
    sprintf( szMaxNumPCA, "%d", dwNumPCA );
    szMaxNumPCA[63] = 0;
    aDefines[0].Name       = "NUM_CLUSTERS";
    aDefines[0].Definition = szMaxNumClusters;
    aDefines[1].Name       = "NUM_PCA";
    aDefines[1].Definition = szMaxNumPCA;
    aDefines[2].Name       = NULL;
    aDefines[2].Definition = NULL;

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.  
    // Debugging vertex shaders requires either REF or software vertex processing, and debugging 
    // pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug 
    // experience in the shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile against the next 
    // higher available software target, which ensures that the unoptimized shaders do not exceed 
    // the shader model limitations.  Setting these flags will cause slower rendering since the shaders 
    // will be unoptimized and forced into software.  See the DirectX documentation for more information 
    // about using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif
        
    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, TEXT("PRTColorLights.fx") ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V( D3DXCreateEffectFromFile( pd3dDevice, str, aDefines, NULL, 
                                 dwShaderFlags, NULL, &m_pPRTEffect, NULL ) );

    // Make sure the technique works on this card
    hr = m_pPRTEffect->ValidateTechnique( "RenderWithPRTColorLights" );
    if( FAILED( hr ) )
        return DXTRACE_ERR( TEXT("ValidateTechnique"), hr );

    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, TEXT("SimpleLighting.fx") ) );
    V( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, 
                                 dwShaderFlags, NULL, &m_pNDotLEffect, NULL ) );

    V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, TEXT("SHIrradianceEnvMap.fx") ) );
    V( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, 
                                 dwShaderFlags, NULL, &m_pSHIrradEnvMapEffect, NULL ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
void CPRTMesh::ExtractCompressedDataForPRTShader() 
{ 
    HRESULT hr;

    // First call ID3DXPRTCompBuffer::NormalizeData.  This isn't nessacary, 
    // but it makes it easier to use data formats that have little presision.
    // It normalizes the PCA weights so that they are between [-1,1]
    // and modifies the basis vectors accordingly.  
    V( m_pPRTCompBuffer->NormalizeData() );

    UINT dwNumSamples  = m_pPRTCompBuffer->GetNumSamples();
    UINT dwNumCoeffs   = m_pPRTCompBuffer->GetNumCoeffs();
    UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
    UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
    UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

    // With clustered PCA, each vertex is assigned to a cluster.  To figure out 
    // which vertex goes with which cluster, call ID3DXPRTCompBuffer::ExtractClusterIDs.
    // This will return a cluster ID for every vertex.  Simply pass in an array of UINTs
    // that is the size of the number of vertices (which also equals the number of samples), and 
    // the cluster ID for vertex N will be at puClusterIDs[N].
    UINT* pClusterIDs = new UINT[ dwNumSamples ];
    assert( pClusterIDs );
    V( m_pPRTCompBuffer->ExtractClusterIDs( pClusterIDs ) );

    D3DVERTEXELEMENT9 declCur[MAX_FVF_DECL_SIZE];
    m_pMesh->GetDeclaration( declCur );

    // Now use this cluster ID info to store a value in the mesh in the 
    // D3DDECLUSAGE_BLENDWEIGHT[0] which is declared in the vertex decl to be a float1
    // This value will be passed into the vertex shader to allow the shader 
    // use this number as an offset into an array of shader constants.  
    // The value we set per vertex is based on the cluster ID and the stride 
    // of the shader constants array.  
    BYTE* pV = NULL;
    V( m_pMesh->LockVertexBuffer( 0, (void**) &pV ) );
    UINT uStride = m_pMesh->GetNumBytesPerVertex();
    BYTE* pClusterID = pV + 32; // 32 == D3DDECLUSAGE_BLENDWEIGHT[0] offset
    for( UINT uVert = 0; uVert < dwNumSamples; uVert++ ) 
    {
        float fArrayOffset = (float)(pClusterIDs[uVert] * (1+3*(dwNumPCA/4))); 
        memcpy(pClusterID, &fArrayOffset, sizeof(float));
        pClusterID += uStride;
    }
    m_pMesh->UnlockVertexBuffer();
    SAFE_DELETE_ARRAY(pClusterIDs);

    // Now we also need to store the per vertex PCA weights.  Earilier when
    // the mesh was loaded, we changed the vertex decl to make room to store these
    // PCA weights.  In this sample, we will use D3DDECLUSAGE_BLENDWEIGHT[1] to 
    // D3DDECLUSAGE_BLENDWEIGHT[6].  Using D3DDECLUSAGE_BLENDWEIGHT intead of some other 
    // usage was an arbritatey decision.  Since D3DDECLUSAGE_BLENDWEIGHT[1-6] were 
    // declared as float4 then we can store up to 6*4 PCA weights per vertex.  They don't
    // have to be declared as float4, but its a reasonable choice.  So for example, 
    // if dwNumPCAVectors=16 the function will write data to D3DDECLUSAGE_BLENDWEIGHT[1-4]
    V( m_pPRTCompBuffer->ExtractToMesh( dwNumPCA, D3DDECLUSAGE_BLENDWEIGHT, 1, m_pMesh ) );

    // Extract the cluster bases into a large array of floats.  
    // ID3DXPRTCompBuffer::ExtractBasis will extract the basis 
    // for a single cluster.  
    //
    // A single cluster basis is an array of
    // (NumPCA+1)*NumCoeffs*NumChannels floats
    // The "1+" is for the cluster mean.
    int nClusterBasisSize = (dwNumPCA+1) * dwNumCoeffs * dwNumChannels;  
    int nBufferSize       = nClusterBasisSize * dwNumClusters; 

    SAFE_DELETE_ARRAY( m_aClusterBases );
    m_aClusterBases = new float[nBufferSize];
    assert( m_aClusterBases );

    for( DWORD iCluster = 0; iCluster < dwNumClusters; iCluster++ ) 
    {
        // ID3DXPRTCompBuffer::ExtractBasis() extracts the basis for a single cluster at a time.
        V( m_pPRTCompBuffer->ExtractBasis( iCluster, &m_aClusterBases[iCluster * nClusterBasisSize] ) );
    }

    SAFE_DELETE_ARRAY( m_aPRTConstants );
    m_aPRTConstants = new float[dwNumClusters*(4+dwNumChannels*dwNumPCA)];
    assert( m_aPRTConstants );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::ComputeSHIrradEnvMapConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue )
{ 
    HRESULT hr;

    float* fLight[3] = { pSHCoeffsRed, pSHCoeffsGreen, pSHCoeffsBlue };

    // Lighting environment coefficients
    D3DXVECTOR4 vCoefficients[3];

    // These constants are described in the article by Peter-Pike Sloan titled 
    // "Efficient Evaluation of Irradiance Environment Maps" in the book 
    // "ShaderX 2 - Shader Programming Tips and Tricks" by Wolfgang F. Engel.
    static const float s_fSqrtPI = ((float)sqrtf(D3DX_PI));
    const float fC0 = 1.0f/(2.0f*s_fSqrtPI);
    const float fC1 = (float)sqrt(3.0f)/(3.0f*s_fSqrtPI);
    const float fC2 = (float)sqrt(15.0f)/(8.0f*s_fSqrtPI);
    const float fC3 = (float)sqrt(5.0f)/(16.0f*s_fSqrtPI);
    const float fC4 = 0.5f*fC2;

    for( int iChannel=0; iChannel<3; iChannel++ )
    {
        vCoefficients[iChannel].x = -fC1*fLight[iChannel][3];
        vCoefficients[iChannel].y = -fC1*fLight[iChannel][1];
        vCoefficients[iChannel].z =  fC1*fLight[iChannel][2];
        vCoefficients[iChannel].w =  fC0*fLight[iChannel][0] - fC3*fLight[iChannel][6];
    }

    V( m_pSHIrradEnvMapEffect->SetVector( "cAr", &vCoefficients[0] ) );
    V( m_pSHIrradEnvMapEffect->SetVector( "cAg", &vCoefficients[1] ) );
    V( m_pSHIrradEnvMapEffect->SetVector( "cAb", &vCoefficients[2] ) );

    for( iChannel=0; iChannel<3; iChannel++ )
    {
        vCoefficients[iChannel].x =      fC2*fLight[iChannel][4];
        vCoefficients[iChannel].y =     -fC2*fLight[iChannel][5];
        vCoefficients[iChannel].z = 3.0f*fC3*fLight[iChannel][6];
        vCoefficients[iChannel].w =     -fC2*fLight[iChannel][7];
    }

    V( m_pSHIrradEnvMapEffect->SetVector( "cBr", &vCoefficients[0] ) );
    V( m_pSHIrradEnvMapEffect->SetVector( "cBg", &vCoefficients[1] ) );
    V( m_pSHIrradEnvMapEffect->SetVector( "cBb", &vCoefficients[2] ) );

    vCoefficients[0].x = fC4*fLight[0][8];
    vCoefficients[0].y = fC4*fLight[1][8];
    vCoefficients[0].z = fC4*fLight[2][8];
    vCoefficients[0].w = 1.0f;

    V( m_pSHIrradEnvMapEffect->SetVector( "cC", &vCoefficients[0] ) );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::ComputeShaderConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue, DWORD dwNumCoeffsPerChannel )
{
    HRESULT hr;
    assert( dwNumCoeffsPerChannel == m_pPRTCompBuffer->GetNumCoeffs() );

    UINT dwNumCoeffs   = m_pPRTCompBuffer->GetNumCoeffs();
    UINT dwOrder       = m_dwOrder;
    UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
    UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
    UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

    //
    // With compressed PRT, a single diffuse channel is caluated by:
    //       R[p] = (M[k] dot L') + sum( w[p][j] * (B[k][j] dot L');
    // where the sum runs j between 0 and # of PCA vectors
    //       R[p] = exit radiance at point p
    //       M[k] = mean of cluster k 
    //       L' = source radiance approximated with SH coefficients
    //       w[p][j] = the j'th PCA weight for point p
    //       B[k][j] = the j'th PCA basis vector for cluster k
    //
    // Note: since both (M[k] dot L') and (B[k][j] dot L') can be computed on the CPU, 
    // these values are passed as constants using the array m_aPRTConstants.   
    // 
    // So we compute an array of floats, m_aPRTConstants, here.
    // This array is the L' dot M[k] and L' dot B[k][j].
    // The source radiance is the lighting environment in terms of spherical
    // harmonic coefficients which can be computed with D3DXSHEval* or D3DXSHProjectCubeMap.  
    // M[k] and B[k][j] are also in terms of spherical harmonic basis coefficients 
    // and come from ID3DXPRTCompBuffer::ExtractBasis().
    //
    DWORD dwClusterStride = dwNumChannels*dwNumPCA + 4;
    DWORD dwBasisStride = dwNumCoeffs*dwNumChannels*(dwNumPCA + 1);  

    for( DWORD iCluster = 0; iCluster < dwNumClusters; iCluster++ ) 
    {
        // For each cluster, store L' dot M[k] per channel, where M[k] is the mean of cluster k
        m_aPRTConstants[iCluster*dwClusterStride + 0] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 0*dwNumCoeffs], pSHCoeffsRed );
        m_aPRTConstants[iCluster*dwClusterStride + 1] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 1*dwNumCoeffs], pSHCoeffsGreen );
        m_aPRTConstants[iCluster*dwClusterStride + 2] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 2*dwNumCoeffs], pSHCoeffsBlue );
        m_aPRTConstants[iCluster*dwClusterStride + 3] = 0.0f;

        // Then per channel we compute L' dot B[k][j], where B[k][j] is the jth PCA basis vector for cluster k
        float* pPCAStart = &m_aPRTConstants[iCluster*dwClusterStride + 4];
        for( DWORD iPCA = 0; iPCA < dwNumPCA; iPCA++ ) 
        {
            int nOffset = iCluster*dwBasisStride + (iPCA+1)*dwNumCoeffs*dwNumChannels;

            pPCAStart[0*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 0*dwNumCoeffs], pSHCoeffsRed );
            pPCAStart[1*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 1*dwNumCoeffs], pSHCoeffsGreen );
            pPCAStart[2*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 2*dwNumCoeffs], pSHCoeffsBlue );
        }
    }

    V( m_pPRTEffect->SetFloatArray( "aPRTConstants", (float*)m_aPRTConstants, dwNumClusters*(4+dwNumChannels*dwNumPCA) ) );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::RenderWithPRT( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, bool bRenderWithAlbedo )
{
    HRESULT hr;
    UINT iPass, cPasses;

    m_pPRTEffect->SetMatrix( "g_mWorldViewProjection", pmWorldViewProj );

    bool bHasAlbedoTexture = false;
    for(int i=0; i<m_pAlbedoTextures.GetSize(); i++ )
    {
        if( m_pAlbedoTextures.GetAt(i) != NULL )
            bHasAlbedoTexture = true;
    }
    if( !bHasAlbedoTexture )
        bRenderWithAlbedo = false;

    if( bRenderWithAlbedo )
    {
        V( m_pPRTEffect->SetTechnique( "RenderWithPRTColorLights" ) );
    }
    else
    {
        V( m_pPRTEffect->SetTechnique( "RenderWithPRTColorLightsNoAlbedo" ) );
    }

    if( !bRenderWithAlbedo )
    {
        D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
        V( m_pPRTEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
    }

    V( m_pPRTEffect->Begin(&cPasses, 0) );

    for (iPass = 0; iPass < cPasses; iPass++)
    {
        V( m_pPRTEffect->BeginPass(iPass) );

        DWORD dwAttribs = 0;
        V( m_pMesh->GetAttributeTable( NULL, &dwAttribs ) );
        for( DWORD i=0; i<dwAttribs; i++ )
        {            
            if( bRenderWithAlbedo )
            {
                if( m_pAlbedoTextures.GetSize() > (int) i )
                    V( m_pPRTEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures.GetAt(i) ) );

                V( m_pPRTEffect->SetValue("MaterialDiffuseColor", &m_pMaterials[i].MatD3D.Diffuse, sizeof(D3DCOLORVALUE) ) );
                V( m_pPRTEffect->CommitChanges() );
            }
            V( m_pMesh->DrawSubset(i) );
        }

        V( m_pPRTEffect->EndPass() );
    }

    V( m_pPRTEffect->End() );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::RenderWithSHIrradEnvMap( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, bool bRenderWithAlbedo )
{
    HRESULT hr;
    UINT iPass, cPasses;

    m_pSHIrradEnvMapEffect->SetMatrix( "g_mWorldViewProjection", pmWorldViewProj );

    bool bHasAlbedoTexture = false;
    for(int i=0; i<m_pAlbedoTextures.GetSize(); i++ )
    {
        if( m_pAlbedoTextures.GetAt(i) != NULL )
            bHasAlbedoTexture = true;
    }
    if( !bHasAlbedoTexture )
        bRenderWithAlbedo = false;

    if( bRenderWithAlbedo )
    {
        V( m_pSHIrradEnvMapEffect->SetTechnique( "RenderWithSHIrradEnvMap" ) );
    }
    else
    {
        V( m_pSHIrradEnvMapEffect->SetTechnique( "RenderWithSHIrradEnvMapNoAlbedo" ) );
    }

    if( !bRenderWithAlbedo )
    {
        D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
        V( m_pSHIrradEnvMapEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
    }

    V( m_pSHIrradEnvMapEffect->Begin(&cPasses, 0) );

    for (iPass = 0; iPass < cPasses; iPass++)
    {
        V( m_pSHIrradEnvMapEffect->BeginPass(iPass) );

        DWORD dwAttribs = 0;
        V( m_pMesh->GetAttributeTable( NULL, &dwAttribs ) );
        for( DWORD i=0; i<dwAttribs; i++ )
        {
            if( bRenderWithAlbedo )
            {
                if( m_pAlbedoTextures.GetSize() > (int) i )
                    V( m_pSHIrradEnvMapEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures.GetAt(i) ) );
                V( m_pSHIrradEnvMapEffect->SetValue("MaterialDiffuseColor", &m_pMaterials[i].MatD3D.Diffuse, sizeof(D3DCOLORVALUE) ) );
                V( m_pSHIrradEnvMapEffect->CommitChanges() );
            }
            V( m_pMesh->DrawSubset(i) );
        }

        V( m_pSHIrradEnvMapEffect->EndPass() );
    }

    V( m_pSHIrradEnvMapEffect->End() );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::RenderWithNdotL( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pmWorldViewProj, D3DXMATRIX* pmWorldInv, bool bRenderWithAlbedo, CDXUTDirectionWidget* aLightControl, int nNumLights, float fLightScale )
{
    HRESULT hr;
    UINT iPass, cPasses;

    m_pNDotLEffect->SetMatrix( "g_mWorldViewProjection", pmWorldViewProj );
    m_pNDotLEffect->SetMatrix( "g_mWorldInv", pmWorldInv );

    D3DXVECTOR4 vLightDir[10];
    D3DXVECTOR4 vLightsDiffuse[10];
    D3DXVECTOR4 lightOn(1,1,1,1);
    D3DXVECTOR4 lightOff(0,0,0,0);
    lightOn *= fLightScale;

    for( int i=0; i<nNumLights; i++ )
        vLightDir[i] = D3DXVECTOR4( aLightControl[i].GetLightDirection(), 0 );
    for( int i=0; i<10; i++ )
        vLightsDiffuse[i] = (nNumLights > i) ? lightOn : lightOff;

    bool bHasAlbedoTexture = false;
    for(int i=0; i<m_pAlbedoTextures.GetSize(); i++ )
    {
        if( m_pAlbedoTextures.GetAt(i) != NULL )
            bHasAlbedoTexture = true;
    }
    if( !bHasAlbedoTexture )
        bRenderWithAlbedo = false;

    if( bRenderWithAlbedo )
    {
        V( m_pNDotLEffect->SetTechnique( "RenderWithNdotL" ) );
    }
    else
    {
        V( m_pNDotLEffect->SetTechnique( "RenderWithNdotLNoAlbedo" ) );
    }

    if( !bRenderWithAlbedo )
    {
        D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
        V( m_pNDotLEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
    }

    V( m_pNDotLEffect->Begin(&cPasses, 0) );

    for (iPass = 0; iPass < cPasses; iPass++)
    {
        V( m_pNDotLEffect->BeginPass(iPass) );

        V( pd3dDevice->SetVertexShaderConstantF( 10, (float*)vLightDir, nNumLights ) );
        V( pd3dDevice->SetVertexShaderConstantF( 20, (float*)vLightsDiffuse, 10 ) );

        DWORD dwAttribs = 0;
        V( m_pMesh->GetAttributeTable( NULL, &dwAttribs ) );
        for( DWORD i=0; i<dwAttribs; i++ )
        {
            if( bRenderWithAlbedo )
            {
                if( m_pAlbedoTextures.GetSize() > (int) i )
                    V( m_pNDotLEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures.GetAt(i) ) );
                V( m_pNDotLEffect->SetValue("MaterialDiffuseColor", &m_pMaterials[i].MatD3D.Diffuse, sizeof(D3DCOLORVALUE) ) );
                V( m_pNDotLEffect->CommitChanges() );
            }
            V( m_pMesh->DrawSubset(i) );
        }

        V( m_pNDotLEffect->EndPass() );
    }

    V( m_pNDotLEffect->End() );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::OnLostDevice()
{
    HRESULT hr;
    if( m_pPRTEffect )
        V( m_pPRTEffect->OnLostDevice() );
    if( m_pSHIrradEnvMapEffect )
        V( m_pSHIrradEnvMapEffect->OnLostDevice() );
    if( m_pNDotLEffect )
        V( m_pNDotLEffect->OnLostDevice() );
}


//--------------------------------------------------------------------------------------
void CPRTMesh::OnDestroyDevice()
{
    if( !m_ReloadState.bUseReloadState )
        ZeroMemory( &m_ReloadState, sizeof(RELOAD_STATE) );

    SAFE_RELEASE( m_pMesh );
    for(int i=0; i<m_pAlbedoTextures.GetSize(); i++ )
    {
        SAFE_RELEASE( m_pAlbedoTextures[i] );
    }
    m_pAlbedoTextures.RemoveAll();
    SAFE_RELEASE( m_pMaterialBuffer );
    SAFE_RELEASE( m_pPRTBuffer );
    SAFE_RELEASE( m_pPRTCompBuffer );
    SAFE_RELEASE( m_pPRTEffect );
    SAFE_RELEASE( m_pSHIrradEnvMapEffect );
    SAFE_RELEASE( m_pNDotLEffect );
}


//--------------------------------------------------------------------------------------
UINT CPRTMesh::GetOrderFromNumCoeffs( UINT dwNumCoeffs )
{
    UINT dwOrder=1; 
    while(dwOrder*dwOrder < dwNumCoeffs) 
        dwOrder++;

    return dwOrder;
}


