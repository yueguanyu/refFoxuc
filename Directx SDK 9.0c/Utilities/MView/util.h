typedef enum _D3DXSKINNINGTYPE
{
    // non-indexed types
    D3DXST_UNSKINNED        = 0x01,
    D3DXST_1WEIGHT          = 0x02,
    D3DXST_2WEIGHT          = 0x04,
    D3DXST_3WEIGHT          = 0x08,

    // indexed types
    D3DXST_0WEIGHTINDEXED   = 0x10,
    D3DXST_1WEIGHTINDEXED   = 0x20,
    D3DXST_2WEIGHTINDEXED   = 0x40,
    D3DXST_3WEIGHTINDEXED   = 0x80,

    D3DXST_NONINDEXEDMASK   = 0x0f,
    D3DXST_NONINDEXEDSHIFT  = 0x00,

    D3DXST_INDEXEDMASK      = 0xf0,
    D3DXST_INDEXEDSHIFT     = 0x04,

    D3DXST_FORCEDWORD = 0x7fffffff,
} D3DXSKINNINGTYPE;

HRESULT
D3DXCalcEffectSkinningSupport
    (
    LPD3DXEFFECT pEffect, 
    DWORD *pSkinningTypesSupported
    );

HRESULT
SelectTechnique
    (
    LPD3DXEFFECT pEffect, 
    DWORD dwSkinningMode
    );

DWORD GetNumInfl
    (
    DWORD dwSkinningSupport
    );

struct SEffectInfo
{
    D3DXHANDLE iWorldView;
    D3DXHANDLE iWorldViewProjection;
    D3DXHANDLE iView;
    D3DXHANDLE iViewProjection;
    D3DXHANDLE iProjection;

    D3DXHANDLE iWorld1;
    DWORD cWorlds;

    D3DXHANDLE iWorldInvTrans1;
    DWORD cWorldInvTrans;

    D3DXHANDLE hTime;
};

HRESULT GenerateEffectInfo
    (
    LPD3DXEFFECT pEffect, 
    SEffectInfo *pEffectInfo
    );


HRESULT
SetEffectMeshInfo
    (
    LPD3DXEFFECT pEffect, 
    LPD3DXVECTOR3 pCenter,
    FLOAT fRadius
    );

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
    );

HRESULT WINAPI D3DXCreateEffectInstance
    (
    CONST D3DXEFFECTINSTANCE *pEffectInstance, 
    LPDIRECT3DDEVICE9 pDevice, 
    LPD3DXEFFECT *ppEffect, 
    LPD3DXBUFFER *ppCompilationErrors
    );

HRESULT DXUtil_FindMediaFile( TCHAR* strPath, TCHAR* strFilename );
VOID D3DUtil_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType,
                        FLOAT x, FLOAT y, FLOAT z );
