#include "mviewpch.h"




// find the first valid technique that also matches the desired skinning characteristics
HRESULT
D3DXCalcEffectSkinningSupport
    (
    LPD3DXEFFECT pEffect, 
    DWORD *pSkinningTypesSupported
    )
{
    HRESULT hr = S_OK;
    D3DXEFFECT_DESC EffectDesc;
    DWORD dwTechBlendPaletteSize;
    DWORD cTechBlendWeights;
    UINT iTech;
    char szBuf[256];
    D3DXTECHNIQUE_DESC TechDesc;
    D3DXHANDLE hOrigTechnique, hTech;
    DWORD dwOut;
    INT iTemp;

    if ((pEffect == NULL) || (pSkinningTypesSupported == NULL))
    {
#ifdef DBG
        if (pEffect == NULL)
            DPF(0, "D3DXCalcEffectSkinningSupport: pEffect cannot be NULL");
        else if (pSkinningTypesSupported == NULL)
            DPF(0, "D3DXCalcEffectSkinningSupport: pSkinningTypesSupported cannot be NULL");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    *pSkinningTypesSupported = 0;
    dwOut = 0;

    pEffect->GetDesc(&EffectDesc);
    if (NULL == (hOrigTechnique = pEffect->GetCurrentTechnique()))
    {
        DPF(0, "D3DXCalcEffectSkinningSupport: Could not get current technique");
        hr = E_FAIL;
        goto e_Exit;
    }

    for (iTech = 0; iTech < EffectDesc.Techniques; iTech++)
    {
        hTech = pEffect->GetTechnique(iTech);

        hr = pEffect->SetTechnique(hTech);
        if (FAILED(hr))
            goto e_Exit;

        // Validate the effect, otherwise we will include info that will not be valid
        hr = pEffect->ValidateTechnique(NULL);
        if (FAILED(hr))
        {
            hr = S_OK;
            continue;
        }

        // set the technique as current to try and validate it
        hr = pEffect->GetTechniqueDesc(hTech, &TechDesc);
        if (FAILED(hr))
            goto e_Exit;

        if (TechDesc.Name != NULL)
        {
            _snprintf(szBuf, 256, "%s_BlendWeights", TechDesc.Name);
            hr = pEffect->GetInt(szBuf, &iTemp);
            cTechBlendWeights = (DWORD)iTemp;
            if (FAILED(hr))
            {
                // set to all blend weights valid special value if not defined
                cTechBlendWeights = UNUSED32;
            }
            else if (cTechBlendWeights > 3)
            {
                DPF(0, "D3DXCalcEffectSkinningSupport: More than 3 blend weights not supported. ignoring technique %s", TechDesc.Name);
                continue;
            }

            _snprintf(szBuf, 256, "%s_BlendPaletteSize", TechDesc.Name);
            hr = pEffect->GetInt(szBuf, &iTemp);
            dwTechBlendPaletteSize = (DWORD)iTemp;
            if (FAILED(hr))
            {
                // set to no blend palette value if not defined
                dwTechBlendPaletteSize = 0;
            }

            if (dwTechBlendPaletteSize == 0)
            {
                // Non-Indexed blending technique with n weights
                dwOut |= 1 << (cTechBlendWeights + D3DXST_NONINDEXEDSHIFT);
            }
            else  // dwTechBlendPaletteSize > 0
            {
                // Indexed blending technique with n weights
                dwOut |= 1 << (cTechBlendWeights + D3DXST_INDEXEDSHIFT);
            }
        }
        else  // if the technique is not named... just asume that all non-indexed modes supported
        {
            dwOut |= D3DXST_NONINDEXEDMASK;
        }
    }

    pEffect->SetTechnique(hOrigTechnique);

    *pSkinningTypesSupported = dwOut;
    
e_Exit:
    return hr;
}

// find the first valid technique that also matches the desired skinning characteristics
HRESULT
SelectTechnique
    (
    LPD3DXEFFECT pEffect, 
    DWORD dwSkinningMode
    )
{
    HRESULT hr = S_OK;
    D3DXEFFECT_DESC EffectDesc;
    D3DXTECHNIQUE_DESC TechDesc;
    DWORD dwTechBlendPaletteSize;
    DWORD cTechBlendWeights;
    UINT iTech;
    char szBuf[256];
    BOOL bFound = FALSE;
    INT iTemp;
    D3DXHANDLE hTech;

    pEffect->GetDesc(&EffectDesc);

    for (iTech = 0; iTech < EffectDesc.Techniques; iTech++)
    {
        hTech = pEffect->GetTechnique(iTech);

        // set the technique as current to try and validate it
        hr = pEffect->SetTechnique(hTech);
        if (FAILED(hr))
            goto e_Exit;

        // get the name of the technique to check for validity for skinning
        hr = pEffect->GetTechniqueDesc(NULL, &TechDesc);
        if (FAILED(hr))
            goto e_Exit;

        if (TechDesc.Name != NULL)
        {
            D3DXHANDLE Handle = pEffect->GetAnnotationByName(hTech, "BlendWeights");
            if (Handle == NULL)
            {
                // set to all blend weights valid special value if not defined
                cTechBlendWeights = UNUSED32;
                hr = S_OK;
            }
            else 
            {
                hr = pEffect->GetInt(Handle, &iTemp);
                cTechBlendWeights = (DWORD)iTemp;

                if (FAILED(hr))
                {
                    // set to all blend weights valid special value if not defined
                    cTechBlendWeights = UNUSED32;
                    hr = S_OK;
                }
                else if (cTechBlendWeights > 3)
                {
                    DPF(0, "MView: More than 3 blend weights not supported. ignoring technique %s", TechDesc.Name);
                    continue;
                }
            }

            Handle = pEffect->GetAnnotationByName(hTech, "BlendPaletteSize");
            if (Handle == NULL)
            {
                // set to all blend indices valid special value if not defined
                dwTechBlendPaletteSize = 0;
            }
            else
            {
                hr = pEffect->GetInt(Handle, &iTemp);
                dwTechBlendPaletteSize = (DWORD)iTemp;
                if (FAILED(hr))
                {
                    // set to all blend indices valid special value if not defined
                    dwTechBlendPaletteSize = 0;
                }
            }

            // if indexed skinning technique
            if (dwTechBlendPaletteSize > 0)
            {
                // check for the specific skinning weight mode
                if ( !(dwSkinningMode & (1 << (cTechBlendWeights + D3DXST_INDEXEDSHIFT)) ))
                {
                    continue;
                }
            }
            // if non-indexed skinning technique
            else
            {
                // check for the specific skinning weight mode
                if ( (cTechBlendWeights != UNUSED32) && !(dwSkinningMode & (1 << (cTechBlendWeights + D3DXST_NONINDEXEDSHIFT)) ))
                {
                    continue;
                }
            }
        }
        else  // if the technique is not named... just assume that non-indexed skinning is supported
        {
            // check for non-indexed skinning and go on if not supported
            if (!(dwSkinningMode & D3DXST_NONINDEXEDMASK))
                continue;
        }

        // Validate the effect 
        hr = pEffect->ValidateTechnique(NULL);
        if (FAILED(hr))
        {
            hr = S_OK;
            continue;
        }

        // found one that works!
        bFound = TRUE;
        break;
    }

    if (!bFound)
    {
        hr = E_FAIL;
    }
    
e_Exit:
    return hr;
}

DWORD GetNumInfl
    (
    DWORD dwSkinningSupport
    )
{

    switch(dwSkinningSupport)
    {

        case D3DXST_UNSKINNED:
            return 1;
        case D3DXST_1WEIGHT:        
            return 2;
        case D3DXST_2WEIGHT:
            return 3;
        case D3DXST_3WEIGHT:        
            return 4;

        case D3DXST_0WEIGHTINDEXED: 
            return 1;
        case D3DXST_1WEIGHTINDEXED: 
            return 2;
        case D3DXST_2WEIGHTINDEXED: 
            return 3;
        case D3DXST_3WEIGHTINDEXED: 
            return 4;

        default:
        
            // more than one bit is set, so fail
            GXASSERT(0);
            return 0;
    }
}

HRESULT
SetEffectMeshInfo
    (
    LPD3DXEFFECT pEffect, 
    LPD3DXVECTOR3 pCenter,
    FLOAT fRadius
    )
{
    UINT iParam;
    D3DXHANDLE hParam;

    D3DXEFFECT_DESC EffectDesc;
    D3DXPARAMETER_DESC ParamDesc;

    pEffect->GetDesc(&EffectDesc);

    // first find all matrix parameters that are present and used that we know about
    for (iParam = 0; iParam < EffectDesc.Parameters; iParam++)
    {
        hParam = pEffect->GetParameter(NULL, iParam);

        pEffect->GetParameterDesc(hParam, &ParamDesc);

        if (ParamDesc.Semantic == NULL)
            continue;

        if (ParamDesc.Class != D3DXPC_VECTOR)
        {
            if (lstrcmpi("meshcenter", ParamDesc.Semantic) == 0)
            {
                D3DXVECTOR4 vCenter = *pCenter;
                pEffect->SetVector(hParam, &vCenter);
            }
        }
        else if (ParamDesc.Class != D3DXPC_SCALAR)
        {
            if (lstrcmpi("meshradius", ParamDesc.Semantic) == 0)
            {
                pEffect->SetFloat(hParam, fRadius);
            }
        }

    }

    return S_OK;
}

const char *x_szWorld = "WORLD";
const UINT x_cchWorld = 5;
const char *x_szWorldInvTrans = "WorldInvTrans";
const UINT x_cchWorldInvTrans = 14;

HRESULT GenerateEffectInfo
    (
    LPD3DXEFFECT pEffect, 
    SEffectInfo *pEffectInfo
    )
{
    HRESULT hr = S_OK;
    UINT iParam;
    char *szNumber;
    char *szEnd;
    char szBuf[80];
    D3DXEFFECT_DESC EffectDesc;
    D3DXPARAMETER_DESC ParamDesc;
    BOOL bAParamNotFound;
    D3DXHANDLE hParam;

    if ((pEffectInfo == NULL) || (pEffect == NULL))
    {
#ifdef DBG
        if (pEffectInfo == NULL)
            DPF(0, "D3DXGenerateEffectInfo: pEffectInfo cannot be NULL");
        else if (pEffect == NULL)
            DPF(0, "D3DXGenerateEffectInfo: pEffect cannot be NULL");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    // intialize all indicess to be all 0xff (meaning not used)
    memset(pEffectInfo, 0, sizeof(SEffectInfo));

    pEffectInfo->cWorlds = 0;
    pEffectInfo->cWorldInvTrans = 0;

    pEffect->GetDesc(&EffectDesc);

    // first find all matrix parameters that are present and used that we know about
    for (iParam = 0; iParam < EffectDesc.Parameters; iParam++)
    {
        hParam = pEffect->GetParameter(NULL, iParam);

        // only worry about a parameter if this technique uses it
        if (!pEffect->IsParameterUsed(hParam, NULL))
            continue;

        pEffect->GetParameterDesc(hParam, &ParamDesc);

        // only look at variables with semantics
        if (ParamDesc.Semantic == NULL)
            continue;

        if (ParamDesc.Class == D3DXPC_SCALAR)
        {
            if (lstrcmpi("time", ParamDesc.Semantic) == 0)
            {
                pEffectInfo->hTime = hParam;
            }

            continue;
        }
        // world semantics only work with matrices
        else if (ParamDesc.Class != D3DXPC_MATRIX_ROWS && ParamDesc.Class != D3DXPC_MATRIX_COLUMNS)
            continue;

        if (lstrcmpi("worldview", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iWorldView = hParam;
        }
        else if (lstrcmpi("WorldViewProjection", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iWorldViewProjection = hParam;
        }
        else if (lstrcmpi("View", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iView = hParam;
        }
        else if (lstrcmpi("ViewProjection", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iViewProjection = hParam;
        }
        else if (lstrcmpi("Projection", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iProjection = hParam;
        }
        else if (lstrcmpi("WorldArray", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iWorld1 = hParam;
            pEffectInfo->cWorlds = ParamDesc.Elements;
        }
        else if (lstrcmpi("WorldInvTransArray", ParamDesc.Semantic) == 0)
        {
            pEffectInfo->iWorldInvTrans1 = hParam;
            pEffectInfo->cWorldInvTrans = ParamDesc.Elements;
        }
    }


e_Exit:
    return hr;
}

HRESULT
SetEffectMatrices
    (
    LPD3DXEFFECT pEffect, 
    SEffectInfo *pEffectInfo, 
    LPD3DXMATRIX pProjection, 
    LPD3DXMATRIX pView, 
    LPD3DXMATRIX pWorlds, 
    DWORD cWorlds,
    LPD3DXMATRIX pWorldInvTrans, 
    DWORD cWorldInvTrans,
    FLOAT fTime
    )
{
    HRESULT hr = S_OK;
    D3DXMATRIX matTemp;
    UINT iWorld;

    if ((pEffect == NULL) || (pEffectInfo == NULL))
    {
#ifdef DBG
        if (pEffectInfo == NULL)
            DPF(0, "D3DXGenerateEffectInfo: pEffectInfo cannot be NULL");
        else if (pEffect == NULL)
            DPF(0, "D3DXGenerateEffectInfo: pEffect cannot be NULL");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

    if (pEffectInfo->hTime)
    {
        pEffect->SetFloat(pEffectInfo->hTime, fTime);
    }

    // first work out worldview and worldviewprojection
    if ((pEffectInfo->iWorldView != NULL) || (pEffectInfo->iWorldViewProjection != NULL))
    {
        D3DXMatrixMultiply(&matTemp, pWorlds, pView);

        if (pEffectInfo->iWorldView != NULL)
        {
            pEffect->SetMatrix(pEffectInfo->iWorldView, &matTemp);
        }

        if (pEffectInfo->iWorldViewProjection != NULL)
        {
            D3DXMatrixMultiply(&matTemp, &matTemp, pProjection);
            pEffect->SetMatrix(pEffectInfo->iWorldViewProjection, &matTemp);
        }
    }

    // add the view and projection matrices if used
    if (pEffectInfo->iProjection != NULL)
    {
        pEffect->SetMatrix(pEffectInfo->iProjection, pProjection);
    }

    if (pEffectInfo->iView != NULL)
    {
        pEffect->SetMatrix(pEffectInfo->iView, pView);
    }

    // generate the view projection if used
    if (pEffectInfo->iViewProjection != NULL)
    {
        D3DXMatrixMultiply(&matTemp, pView, pProjection);
        pEffect->SetMatrix(pEffectInfo->iViewProjection, &matTemp);
    }

    // set all world matrices used
    if (pEffectInfo->cWorlds > 0)
    {
        if (pEffectInfo->cWorlds > cWorlds)
        {
            DPF(0, "More worlds in effect file than specified in pWorlds");
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        // if indices can be used, use them
        if (pEffectInfo->iWorld1 != NULL)
        {
            pEffect->SetMatrixArray(pEffectInfo->iWorld1, pWorlds, min(cWorlds, pEffectInfo->cWorlds));
        }
    }

    // set all world inverse matrices used
    if (pEffectInfo->cWorldInvTrans > 0)
    {
        if ((cWorldInvTrans == NULL) && (cWorlds > 0))
        {
            pWorldInvTrans = (LPD3DXMATRIX)_alloca(sizeof(D3DXMATRIX) * cWorlds);
            cWorldInvTrans = cWorlds;

            for (iWorld = 0; iWorld < cWorlds; iWorld++)
            {
                D3DXMatrixInverse(pWorldInvTrans + iWorld, NULL, pWorlds + iWorld);
                D3DXMatrixTranspose(pWorldInvTrans, pWorldInvTrans);
            }
        }

        if (pEffectInfo->cWorldInvTrans > cWorldInvTrans)
        {
            DPF(0, "D3DX:  More worlds in effect file than specified in pWorldInvTrans");
            hr = D3DERR_INVALIDCALL;
            goto e_Exit;
        }

        // if indices can be used, use them
        if (pEffectInfo->iWorldInvTrans1 != NULL)
        {
            pEffect->SetMatrixArray(pEffectInfo->iWorldInvTrans1, pWorldInvTrans, min(cWorlds, pEffectInfo->cWorldInvTrans));
        }
    }


e_Exit:
    return hr;
}

//============================================================================================================
//   UNDONE UNDONE The following "function" still needs work.  Needs some sort of filename resolver
//============================================================================================================

//-----------------------------------------------------------------------------
// Name: DXUtil_GetDXSDKMediaPath()
// Desc: Returns the DirectX SDK media path
//-----------------------------------------------------------------------------
const TCHAR* DXUtil_GetDXSDKMediaPath()
{
    static TCHAR strNull[2] = _T("");
    static TCHAR strPath[MAX_PATH];
    DWORD dwType;
    DWORD dwSize = MAX_PATH;
    HKEY  hKey;

    // Open the appropriate registry key
    LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Microsoft\\DirectX SDK"),
                                0, KEY_READ, &hKey );
    if( ERROR_SUCCESS != lResult )
        return strNull;

    lResult = RegQueryValueEx( hKey, _T("DX81SDK Samples Path"), NULL,
                              &dwType, (BYTE*)strPath, &dwSize );
    RegCloseKey( hKey );

    if( ERROR_SUCCESS != lResult )
        return strNull;

    _tcscat( strPath, _T("\\Media\\") );

    return strPath;
}


//-----------------------------------------------------------------------------
// Name: DXUtil_FindMediaFile()
// Desc: Returns a valid path to a DXSDK media file
//-----------------------------------------------------------------------------
HRESULT DXUtil_FindMediaFile( TCHAR* strPath, TCHAR* strFilename )
{
    HANDLE file;
    TCHAR strFullPath[1024];
    TCHAR *strShortName;
    DWORD cchPath;

    if( NULL==strFilename || NULL==strPath )
        return E_INVALIDARG;

    // Build full path name from strFileName (strShortName will be just the leaf filename)
    cchPath = GetFullPathName(strFilename, sizeof(strFullPath)/sizeof(TCHAR), strFullPath, &strShortName);
    if ((cchPath == 0) || (sizeof(strFullPath)/sizeof(TCHAR) <= cchPath))
        return E_FAIL;

    // first try to find the filename given a full path
    file = CreateFile( strFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        _tcscpy( strPath, strFullPath );
        CloseHandle( file );
        return S_OK;
    }
    
    // next try to find the filename in the current working directory (path stripped)
    file = CreateFile( strShortName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        _tcscpy( strPath, strShortName );
        CloseHandle( file );
        return S_OK;
    }
    
    // last, check if the file exists in the media directory
    _stprintf( strPath, _T("%s%s"), DXUtil_GetDXSDKMediaPath(), strShortName );

    file = CreateFile( strPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, 0, NULL );
    if( INVALID_HANDLE_VALUE != file )
    {
        CloseHandle( file );
        return S_OK;
    }

    // On failure, just return the file as the path
    _tcscpy( strPath, strFilename );
    return E_FAIL;
}

HRESULT WINAPI D3DXCreateEffectInstance
    (
    CONST D3DXEFFECTINSTANCE *pEffectInstance, 
    LPDIRECT3DDEVICE9 pDevice, 
    LPD3DXEFFECT *ppEffect, 
    LPD3DXBUFFER *ppCompilationErrors
    )
{
    HRESULT hr = S_OK;
    LPD3DXEFFECT pEffect = NULL;
    UINT iDefault;
    LPD3DXEFFECTDEFAULT pDefault;
    LPDIRECT3DBASETEXTURE9 pTex;
    D3DXEFFECT_DESC EffectDesc;
    D3DXPARAMETER_DESC ParamDesc;
    D3DXPARAMETER_DESC AnnotDesc;
    LPCTSTR pTextureFilename;

    if ((pEffectInstance == NULL) || (pDevice == NULL) || (ppEffect == NULL))
    {
#ifdef DBG
        if (pEffectInstance == NULL)
            DPF(0, "D3DXCreateEffectInstance: pEffectInstance cannot be NULL");
        else if (pDevice == NULL)
            DPF(0, "D3DXCreateEffectInstance: pDevice cannot be NULL");
        else if (ppEffect == NULL)
            DPF(0, "D3DXCreateEffectInstance: ppEffect cannot be NULL");
#endif
        hr = D3DERR_INVALIDCALL;
        goto e_Exit;
    }

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

    // if file based effect, then load it
    if (pEffectInstance->pEffectFilename != NULL)
    {
        hr = D3DXCreateEffectFromFileA(pDevice, pEffectInstance->pEffectFilename, NULL, NULL, dwShaderFlags, NULL, &pEffect, ppCompilationErrors);
        if (FAILED(hr))
            goto e_Exit;
    }
    else  // generate the default effect AKA texture1.fx
    {
        hr = D3DXCreateEffectFromResource(pDevice, NULL, MAKEINTRESOURCE(IDD_TEXTURE1), NULL, NULL, dwShaderFlags, NULL, &pEffect, ppCompilationErrors);
        if (FAILED(hr))
            return hr;
    }

    for (iDefault = 0; iDefault < pEffectInstance->NumDefaults; iDefault++)
    {
        D3DXPARAMETER_DESC ParamDesc;

        pDefault = &pEffectInstance->pDefaults[iDefault];
        switch (pDefault->Type)
        {
            case D3DXEDT_STRING:

                hr = pEffect->SetString(pDefault->pParamName, (char*)pDefault->pValue);
                if (FAILED(hr))
                {
                    // handle the currently broken Name case... if setting a string to an interface type, add the @Name
                    hr = pEffect->GetParameterDesc(pDefault->pParamName, &ParamDesc);
                    if (SUCCEEDED(hr))
                    {
                        if ((ParamDesc.Type >= D3DXPT_TEXTURE) || (ParamDesc.Type <= D3DXPT_PIXELSHADER))
                        {
                            char szBuf[1024];
                            sprintf(szBuf, "%s%s", pDefault->pParamName, "@Name");
                            pEffect->SetString(szBuf, (char*)pDefault->pValue);
                        }
                    }
                }

                break;
            case D3DXEDT_FLOATS:
                switch (pDefault->NumBytes / 4)
                {
                case 1:
                    pEffect->SetFloat(pDefault->pParamName, *(FLOAT*)pDefault->pValue);
                    break;
                case 4:
                    pEffect->SetVector(pDefault->pParamName, (LPD3DXVECTOR4)pDefault->pValue);
                    break;
                case 16:
                    pEffect->SetMatrix(pDefault->pParamName, (LPD3DXMATRIX)pDefault->pValue);
                    break;
                default:
                    DPF(0, "D3DXCreateEffectInstance: Unknown number of floats specified on parameter: %s  (Skipping)", pDefault->pParamName);
                    break;
                }
                break;
            case D3DXEDT_DWORD:
                pEffect->SetInt(pDefault->pParamName, (INT)*(DWORD*)pDefault->pValue);
                break;
        }
    }

    pEffect->GetDesc(&EffectDesc);

    for(UINT iParam = 0; iParam < EffectDesc.Parameters; iParam++)
    {
        CHAR szBuf[1024];
        D3DXHANDLE hParam, hAnnot;

        LPCSTR pstrName = NULL;
        LPCSTR pstrFunction = NULL;
        LPCSTR pstrTarget = NULL;
        LPCSTR pstrTextureType = NULL;
        INT Width = D3DX_DEFAULT;
        INT Height= D3DX_DEFAULT;
        INT Depth = D3DX_DEFAULT;



        hParam = pEffect->GetParameter(NULL, iParam);
        
        pEffect->GetParameterDesc(hParam, &ParamDesc);

        // only load textures
        if (D3DXPT_TEXTURE != ParamDesc.Type) 
            continue;
        
        for( UINT iAnnot = 0; iAnnot < ParamDesc.Annotations; iAnnot++ )
        {
            hAnnot = pEffect->GetAnnotation ( hParam, iAnnot );
            pEffect->GetParameterDesc( hAnnot, &AnnotDesc );
            if( lstrcmpi( AnnotDesc.Name, "name" ) == 0 )
                pEffect->GetString( hAnnot, &pstrName );
            else if ( lstrcmpi( AnnotDesc.Name, "function" ) == 0 )
                pEffect->GetString( hAnnot, &pstrFunction );
            else if ( lstrcmpi( AnnotDesc.Name, "target" ) == 0 )
                pEffect->GetString( hAnnot, &pstrTarget );
            else if ( lstrcmpi( AnnotDesc.Name, "width" ) == 0 )
                pEffect->GetInt( hAnnot, &Width );
            else if ( lstrcmpi( AnnotDesc.Name, "height" ) == 0 )
                pEffect->GetInt( hAnnot, &Height );
            else if ( lstrcmpi( AnnotDesc.Name, "depth" ) == 0 )
                pEffect->GetInt( hAnnot, &Depth );
            else if( lstrcmpi( AnnotDesc.Name, "type" ) == 0 )
                pEffect->GetString( hAnnot, &pstrTextureType );

        }

        if (pstrName != NULL)
        {
            TCHAR strTexturePath[512] = _T("");
            DXUtil_FindMediaFile( strTexturePath, (TCHAR*)pstrName );

            if (pstrTextureType != NULL) 
            {
                if( lstrcmpi( pstrTextureType, "volume" ) == 0 )
                {
                    LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateVolumeTextureFromFileEx( pDevice, strTexturePath, 
                        Width, Height, Depth, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pVolumeTex ) ) )
                    {
                        pTex = pVolumeTex;
                    }
                }
                else if( lstrcmpi( pstrTextureType, "cube" ) == 0 )
                {
                    LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateCubeTextureFromFileEx( pDevice, strTexturePath, 
                        Width, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubeTex ) ) )
                    {
                        pTex = pCubeTex;
                    }
                }
            }
            else
            {
                LPDIRECT3DTEXTURE9 p2DTex = NULL;
                if( SUCCEEDED( hr = D3DXCreateTextureFromFileEx( pDevice, strTexturePath, 
                    Width, Height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &p2DTex ) ) )
                {
                    pTex = p2DTex;
                }
            }

            if (FAILED(hr) || (pTex == NULL))
                hr = S_OK;
            else
            {
                pEffect->SetTexture( hParam, pTex );
                GXRELEASE( pTex );
            }
        }
        else if( pstrFunction != NULL )
        {
            LPD3DXBUFFER pFunction = NULL;
            LPD3DXTEXTURESHADER pTextureShader = NULL;

            if( pstrTarget == NULL )
                pstrTarget = "tx_1_0";
    
            if( SUCCEEDED( hr = D3DXCompileShaderFromFile( pEffectInstance->pEffectFilename, NULL, 
                NULL, pstrFunction, pstrTarget, 0, &pFunction, NULL, NULL ) ) )
            {
                if( SUCCEEDED( hr = D3DXCreateTextureShader( (DWORD *)pFunction->GetBufferPointer(), &pTextureShader ) ) )
                {
                    if( Width == D3DX_DEFAULT )
                        Width = 64;
                    if( Height == D3DX_DEFAULT )
                        Height = 64;
                    if( Depth == D3DX_DEFAULT )
                        Depth = 64;

                    if (pstrTextureType != NULL) 
                    {
                        if( lstrcmpi( pstrTextureType, "volume" ) == 0 )
                        {
                            LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
                            if( SUCCEEDED( hr = D3DXCreateVolumeTexture( pDevice, 
                                Width, Height, Depth, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pVolumeTex) ) )
                            {
                                if( SUCCEEDED( hr = D3DXFillVolumeTextureTX( pVolumeTex, pTextureShader ) ) )
                                    pTex = pVolumeTex;
                            }
                        }
                        else if( lstrcmpi( pstrTextureType, "cube" ) == 0 )
                        {
                            LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
                            if( SUCCEEDED( hr = D3DXCreateCubeTexture( pDevice, 
                                Width, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pCubeTex) ) )
                            {
                                if( SUCCEEDED( hr = D3DXFillCubeTextureTX( pCubeTex, pTextureShader ) ) )
                                    pTex = pCubeTex;
                            }
                        }
                    }
                    else
                    {
                        LPDIRECT3DTEXTURE9 p2DTex = NULL;
                        if( SUCCEEDED( hr = D3DXCreateTexture( pDevice, Width, Height, 
                            D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &p2DTex) ) )
                        {
                            if( SUCCEEDED( hr = D3DXFillTextureTX( p2DTex, pTextureShader ) ) )
                                pTex = p2DTex;
                        }
                    }
                    pEffect->SetTexture( hParam, pTex );
                    GXRELEASE(pTex);
                    GXRELEASE(pTextureShader);
                }
                GXRELEASE(pFunction);
            }
        }
    }


    *ppEffect = pEffect;
    pEffect = NULL;
e_Exit:
    GXRELEASE(pEffect);
    return hr;
}

//-----------------------------------------------------------------------------
// Name: D3DUtil_InitLight()
// Desc: Initializes a D3DLIGHT structure, setting the light position. The
//       diffuse color is set to white; specular and ambient are left as black.
//-----------------------------------------------------------------------------
VOID D3DUtil_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
                        FLOAT x, FLOAT y, FLOAT z )
{
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type        = ltType;
    light.Diffuse.r   = 1.0f;
    light.Diffuse.g   = 1.0f;
    light.Diffuse.b   = 1.0f;
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &D3DXVECTOR3(x, y, z) );
    light.Position.x   = x;
    light.Position.y   = y;
    light.Position.z   = z;
    light.Range        = 1000.0f;
}

