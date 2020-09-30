//--------------------------------------------------------------------------------------
// File: PRTSimulator.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "prtmesh.h"
#include "prtsimulator.h"
#include "prtoptionsdlg.h"

CPRTSimulator* g_pSimulator;

//--------------------------------------------------------------------------------------
CPRTSimulator::CPRTSimulator(void)
{
    m_nCurPass = 1;
    m_nNumPasses = 1;
    g_pSimulator = this;
    InitializeCriticalSection( &m_cs );   

    m_bStopSimulator = false;
    m_bRunning = false;
    m_bFailed = false;
    m_pPRTEngine = NULL;

    m_hThreadId = NULL; 
    m_dwThreadId = 0;
    m_fPercentDone = 0.0f;
    wcscpy( m_strCurPass, L"" );
}


//--------------------------------------------------------------------------------------
CPRTSimulator::~CPRTSimulator(void)
{
    DeleteCriticalSection( &m_cs );  
    SAFE_RELEASE( m_pPRTEngine );
}


//--------------------------------------------------------------------------------------
HRESULT CPRTSimulator::Run( IDirect3DDevice9* pd3dDevice, SIMULATOR_OPTIONS* pOptions, CPRTMesh* pPRTMesh )
{
    if( IsRunning() ) 
        return E_FAIL;

    memcpy( &m_Options, pOptions, sizeof(SIMULATOR_OPTIONS) );
    m_pd3dDevice = pd3dDevice;
    m_pPRTMesh = pPRTMesh;

    m_bRunning = true;
    m_bFailed = false;
    m_bStopSimulator = false;
    m_fPercentDone = 0.0f;

    // Launch the PRT simulator on another thread cause it'll 
    // likely take a while and the UI would be unresponsive otherwise
    m_hThreadId = CreateThread( NULL, 0, StaticPRTSimulationThreadProc, 
                                this, 0, &m_dwThreadId );

    return S_OK;
}


//--------------------------------------------------------------------------------------
bool CPRTSimulator::IsRunning()
{
    if( m_hThreadId )
    {
        // Ask to stop the PRT simulator if it's running in the other thread
        DWORD dwResult = WaitForSingleObject( m_hThreadId, 0 );
        if( dwResult == WAIT_TIMEOUT )
            return true;
    }
    return false;
}


//--------------------------------------------------------------------------------------
HRESULT CPRTSimulator::Stop()
{
    if( IsRunning() )
    {
        EnterCriticalSection( &m_cs );
        m_bStopSimulator = true;
        LeaveCriticalSection( &m_cs );

        // Wait for it to close
        DWORD dwResult = WaitForSingleObject( m_hThreadId, 10000 );
        if( dwResult == WAIT_TIMEOUT )
            return E_FAIL;

        m_bStopSimulator = false;
        m_hThreadId = NULL;
        m_dwThreadId = 0;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// static helper function
//-----------------------------------------------------------------------------
DWORD WINAPI CPRTSimulator::StaticPRTSimulationThreadProc( LPVOID lpParameter )
{   
    CPRTSimulator* pSim = (CPRTSimulator*)lpParameter;
    return pSim->PRTSimulationThreadProc();
}


//-----------------------------------------------------------------------------
// Load the mesh and start the simluator and save the results to a file
//-----------------------------------------------------------------------------
DWORD CPRTSimulator::PRTSimulationThreadProc()
{
    HRESULT hr;

    // Reset precent complete
    m_fPercentDone = 0.0f;

    if( !m_pPRTMesh->IsMeshLoaded() )    
        return 1;

    ID3DXPRTBuffer* pDataTotal = NULL;
    ID3DXPRTBuffer* pBufferA = NULL;
    ID3DXPRTBuffer* pBufferB = NULL;
    D3DXSHMATERIAL* pMatPtr = NULL;

    m_nNumPasses = m_Options.dwNumBounces;
    if( m_Options.bSubsurfaceScattering )
        m_nNumPasses *= 2;
    if( m_Options.bAdaptive && m_Options.bRobustMeshRefine )
        m_nNumPasses++;

    m_nNumPasses += 2;

    m_nCurPass = 1;
    m_fPercentDone = -1.0f;
    wcscpy( m_strCurPass, L"Initializing PRT engine" );

    ID3DXMesh* pMesh = m_pPRTMesh->GetMesh();

    bool bExtractUVs = false;
    if( m_Options.bAdaptive && m_pPRTMesh->GetAlbedoTexture() )
        bExtractUVs = true;

    V( D3DXCreatePRTEngine( pMesh, bExtractUVs, NULL, &m_pPRTEngine ) );

    V( m_pPRTEngine->SetCallBack( StaticPRTSimulatorCB, 0.001f, NULL ) );
    V( m_pPRTEngine->SetSamplingInfo( m_Options.dwNumRays, FALSE, TRUE, FALSE, 0.0f ) );

    if( m_Options.bAdaptive && m_pPRTMesh->GetAlbedoTexture() )
    {
        V( m_pPRTEngine->SetPerTexelAlbedo( m_pPRTMesh->GetAlbedoTexture(), 
                                            m_Options.dwNumChannels, NULL ) );
    }

    // Note that the alpha value is ignored for the Diffuse, Absorption, 
    // and ReducedScattering parameters of the material.
    D3DXSHMATERIAL shMat[1];
    ZeroMemory( &shMat[0], sizeof(D3DXSHMATERIAL) );
    shMat[0].Diffuse = m_Options.Diffuse;
    shMat[0].bMirror = false;
    shMat[0].bSubSurf = m_Options.bSubsurfaceScattering;
    shMat[0].RelativeIndexOfRefraction  = m_Options.fRelativeIndexOfRefraction;
    shMat[0].Absorption = m_Options.Absoption;
    shMat[0].ReducedScattering = m_Options.ReducedScattering;

    DWORD dwNumMeshes = 0;
    V( pMesh->GetAttributeTable(NULL,&dwNumMeshes) );

    // This sample treats all subsets as having the same 
    // material properties but they don't have too
    D3DXMATERIAL* pd3dxMaterial = m_pPRTMesh->GetMaterials();
    pMatPtr = new D3DXSHMATERIAL[dwNumMeshes];
    for( DWORD i=0; i<dwNumMeshes; ++i )
    {
        ZeroMemory( &pMatPtr[i], sizeof(D3DXSHMATERIAL) );
        pMatPtr[i].Diffuse = m_Options.Diffuse;
        pMatPtr[i].bMirror = false;
        pMatPtr[i].bSubSurf = m_Options.bSubsurfaceScattering;
        pMatPtr[i].RelativeIndexOfRefraction  = m_Options.fRelativeIndexOfRefraction;
        pMatPtr[i].Absorption = m_Options.Absoption;
        pMatPtr[i].ReducedScattering = m_Options.ReducedScattering;

        pMatPtr[i].Diffuse = pd3dxMaterial[i].MatD3D.Diffuse;
    }

    D3DXSHMATERIAL** pMatPtrArray = new D3DXSHMATERIAL*[dwNumMeshes];
    for( DWORD i=0; i<dwNumMeshes; ++i )
    {
        pMatPtrArray[i] = &pMatPtr[i];
    }

    bool bSetAlbedoFromMaterial = true;
    if( m_Options.bAdaptive && m_pPRTMesh->GetAlbedoTexture() )
        bSetAlbedoFromMaterial = false;

    V( m_pPRTEngine->SetMeshMaterials( (const D3DXSHMATERIAL**)pMatPtrArray, dwNumMeshes, 
                                       m_Options.dwNumChannels, 
                                       bSetAlbedoFromMaterial, m_Options.fLengthScale ) );

    if( !m_Options.bSubsurfaceScattering )
    {
        // Not doing subsurface scattering

        if( m_Options.bAdaptive && m_Options.bRobustMeshRefine ) 
        {
            m_nCurPass++;
            m_fPercentDone = -1.0f;
            wcscpy( m_strCurPass, L"Robust Mesh Refine" );
            V( m_pPRTEngine->RobustMeshRefine( m_Options.fRobustMeshRefineMinEdgeLength, m_Options.dwRobustMeshRefineMaxSubdiv ) );
        }

        DWORD dwNumSamples = m_pPRTEngine->GetNumVerts();
        V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                m_Options.dwNumChannels, &pDataTotal ) );

        m_nCurPass++;
        wcscpy( m_strCurPass, L"Computing Direct Lighting" );
        m_fPercentDone = 0.0f;
        if( m_Options.bAdaptive && m_Options.bAdaptiveDL )
        {
            hr = m_pPRTEngine->ComputeDirectLightingSHAdaptive( m_Options.dwOrder, 
                                                                m_Options.fAdaptiveDLThreshold, m_Options.fAdaptiveDLMinEdgeLength, m_Options.dwAdaptiveDLMaxSubdiv, 
                                                                pDataTotal );
            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 
        }
        else
        {
            hr = m_pPRTEngine->ComputeDirectLightingSH( m_Options.dwOrder, pDataTotal );
            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 
        }

        if( m_Options.dwNumBounces > 1 )
        {
            dwNumSamples = m_pPRTEngine->GetNumVerts();
            V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                    m_Options.dwNumChannels, &pBufferA ) );
            V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                    m_Options.dwNumChannels, &pBufferB ) );
            V( pBufferA->AddBuffer( pDataTotal ) );
        }

        for( UINT iBounce=1; iBounce<m_Options.dwNumBounces; ++iBounce )
        {
            m_nCurPass++;
            swprintf( m_strCurPass, L"Computing Bounce %d Lighting", iBounce+1 );
            m_fPercentDone = 0.0f;
            if( m_Options.bAdaptive && m_Options.bAdaptiveBounce )
                hr = m_pPRTEngine->ComputeBounceAdaptive( pBufferA, m_Options.fAdaptiveBounceThreshold, m_Options.fAdaptiveBounceMinEdgeLength, m_Options.dwAdaptiveBounceMaxSubdiv, pBufferB, pDataTotal );
            else
                hr = m_pPRTEngine->ComputeBounce( pBufferA, pBufferB, pDataTotal );

            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 

            // Swap pBufferA and pBufferB
            ID3DXPRTBuffer* pPRTBufferTemp = NULL;
            pPRTBufferTemp = pBufferA;
            pBufferA = pBufferB;
            pBufferB = pPRTBufferTemp;
        }

        if( m_Options.bAdaptive )
        {
            V( m_pPRTEngine->GetAdaptedMesh( m_pd3dDevice, NULL, NULL, NULL, &pMesh ) );
            m_pPRTMesh->SetMesh( m_pd3dDevice, pMesh );

            DWORD dwFormat = 0;
            if( m_Options.bBinaryOutputXFile )
                dwFormat = D3DXF_FILEFORMAT_BINARY;
            else 
                dwFormat = D3DXF_FILEFORMAT_TEXT;

            // Save the mesh
            V( D3DXSaveMeshToX( m_Options.strOutputMesh, m_pPRTMesh->GetMesh(), NULL, 
                                m_pPRTMesh->GetMaterials(), NULL, m_pPRTMesh->GetNumMaterials(), 
                                dwFormat ) );
        }

        SAFE_RELEASE( pBufferA );
        SAFE_RELEASE( pBufferB );
    }
    else
    {
        // Doing subsurface scattering

        if( m_Options.bAdaptive && m_Options.bRobustMeshRefine ) 
            V( m_pPRTEngine->RobustMeshRefine( m_Options.fRobustMeshRefineMinEdgeLength, m_Options.dwRobustMeshRefineMaxSubdiv ) );

        DWORD dwNumSamples = m_pPRTEngine->GetNumVerts();
        V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                m_Options.dwNumChannels, &pBufferA ) );
        V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                m_Options.dwNumChannels, &pBufferB ) );
        V( D3DXCreatePRTBuffer( dwNumSamples, m_Options.dwOrder*m_Options.dwOrder, 
                                m_Options.dwNumChannels, &pDataTotal ) );

        m_nCurPass = 1;
        wcscpy( m_strCurPass, L"Computing Direct Lighting" );
        m_fPercentDone = 0.0f;
        if( m_Options.bAdaptive && m_Options.bAdaptiveDL )
        {
            hr = m_pPRTEngine->ComputeDirectLightingSHAdaptive( m_Options.dwOrder, 
                                                                m_Options.fAdaptiveDLThreshold, m_Options.fAdaptiveDLMinEdgeLength, m_Options.dwAdaptiveDLMaxSubdiv, 
                                                                pBufferA );
            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 
        }
        else
        {
            hr = m_pPRTEngine->ComputeDirectLightingSH( m_Options.dwOrder, pBufferA );
            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 
        }

        m_nCurPass++;
        wcscpy( m_strCurPass, L"Computing Subsurface Direct Lighting" );
        hr = m_pPRTEngine->ComputeSS( pBufferA, pBufferB, pDataTotal );
        if( FAILED(hr ) )
            goto LEarlyExit; // handle user aborting simulator via callback 

        for( UINT iBounce=1; iBounce<m_Options.dwNumBounces; ++iBounce )
        {
            m_nCurPass++;
            swprintf( m_strCurPass, L"Computing Bounce %d Lighting", iBounce+1 );
            m_fPercentDone = 0.0f;
            if( m_Options.bAdaptive && m_Options.bAdaptiveBounce )
                hr = m_pPRTEngine->ComputeBounceAdaptive( pBufferB, m_Options.fAdaptiveBounceThreshold, m_Options.fAdaptiveBounceMinEdgeLength, m_Options.dwAdaptiveBounceMaxSubdiv, pBufferA, NULL );
            else
                hr = m_pPRTEngine->ComputeBounce( pBufferB, pBufferA, NULL );

            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 

            m_nCurPass++;
            swprintf( m_strCurPass, L"Computing Subsurface Bounce %d Lighting", iBounce+1 );
            hr = m_pPRTEngine->ComputeSS( pBufferB, pBufferA, pDataTotal );
            if( FAILED(hr ) )
                goto LEarlyExit; // handle user aborting simulator via callback 
        }

        if( m_Options.bAdaptive )
        {
            V( m_pPRTEngine->GetAdaptedMesh( m_pd3dDevice, NULL, NULL, NULL, &pMesh ) );
            m_pPRTMesh->SetMesh( m_pd3dDevice, pMesh );

            DWORD dwFormat = 0;
            if( m_Options.bBinaryOutputXFile )
                dwFormat = D3DXF_FILEFORMAT_BINARY; 
            else 
                dwFormat = D3DXF_FILEFORMAT_TEXT;

            // Save the mesh
            V( D3DXSaveMeshToX( m_Options.strOutputMesh, m_pPRTMesh->GetMesh(), NULL, 
                                m_pPRTMesh->GetMaterials(), NULL, m_pPRTMesh->GetNumMaterials(), 
                                dwFormat ) );
        }

        SAFE_RELEASE( pBufferA );
        SAFE_RELEASE( pBufferB );
    }

    m_nCurPass++;
    wcscpy( m_strCurPass, L"Compressing Buffer" );
    m_fPercentDone = -1.0f;

    SetCurrentDirectory( m_Options.strInitialDir );
    m_pPRTMesh->SetPRTBuffer( pDataTotal, m_Options.strResultsFile );
    m_pPRTMesh->CompressBuffer( D3DXSHCQUAL_FASTLOWQUALITY, 1, 24 );
//    m_pPRTMesh->CompressBuffer( m_Options.Quality, m_Options.dwNumClusters, m_Options.dwNumPCA );

    if( m_Options.bSaveCompressedResults )
    {
        ID3DXPRTCompBuffer* pCompBuffer = m_pPRTMesh->GetCompBuffer();
        V( D3DXSavePRTCompBufferToFile( m_Options.strResultsFile, pCompBuffer ) );        
    }
    else
    {
        V( D3DXSavePRTBufferToFile( m_Options.strResultsFile, pDataTotal ) );
    }
    m_pPRTMesh->ExtractCompressedDataForPRTShader();

    m_bRunning = false;
    m_fPercentDone = 1.0f;

    SAFE_RELEASE( m_pPRTEngine );
    SAFE_DELETE_ARRAY( pMatPtr );

    return 1;

LEarlyExit:

    // Usually fails becaused user stoped the simulator
    m_bFailed = true;
    m_bRunning = false;
    SAFE_RELEASE( m_pPRTEngine );
    SAFE_RELEASE( pBufferA );
    SAFE_RELEASE( pBufferB );
    SAFE_RELEASE( pDataTotal );
    SAFE_DELETE_ARRAY( pMatPtr );

    // It returns E_FAIL if the simulation was aborted from the callback
    if( hr == E_FAIL ) 
        return 0;

    DXTRACE_ERR( TEXT("D3DXSHPRTSimulation"), hr );
    return 1;
}


//-----------------------------------------------------------------------------
// static helper function
//-----------------------------------------------------------------------------
HRESULT WINAPI CPRTSimulator::StaticPRTSimulatorCB( float fPercentDone, LPVOID pParam )
{
    return g_pSimulator->PRTSimulatorCB( fPercentDone );
}


//-----------------------------------------------------------------------------
// records the percent done and stops the simulator if requested
//-----------------------------------------------------------------------------
HRESULT CPRTSimulator::PRTSimulatorCB( float fPercentDone )
{
    EnterCriticalSection( &m_cs );
    m_fPercentDone = fPercentDone;

    HRESULT hr = S_OK;
    // In this callback, returning anything except S_OK will stop the simulator
    if( m_bStopSimulator )
        hr = E_FAIL; 

    LeaveCriticalSection( &m_cs );

    return hr;
}


