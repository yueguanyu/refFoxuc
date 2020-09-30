#pragma once

#ifndef __DXCRACKFVF_H__
#define __DXCRACKFVF_H__

/*//////////////////////////////////////////////////////////////////////////////
//
// File: DXCrackFVF.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/


// -------------------------------------------------------------------------------
//  class DXCrackFVF
//          used to simplify retreiving and storing data to FVF formats.
//          on construction, it creates a set of offsets in the FVF format
//          that it uses in the the methods to access the correct members
//
class DXCrackFVF
{
public:
    inline DXCrackFVF(DWORD dwFVF);

    inline PBYTE GetArrayElem(PVOID pvPoints, DWORD iElem);

    inline void         SetPosition    (PVOID pvPoint, D3DXVECTOR3 *pvPos);
    inline void         SetNormal      (PVOID pvPoint, D3DXVECTOR3 *pvNormal);
    inline void         SetPointSize   (PVOID pvPoint, float fPointSize);
    inline void         SetDiffuse     (PVOID pvPoint, D3DCOLOR color);
    inline void         SetSpecular    (PVOID pvPoint, D3DCOLOR color);
    inline void         SetTex1        (PVOID pvPoint, D3DXVECTOR2 *puvTex1);

    inline void         SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR2 *puvTex1);
    inline void         SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR3 *puvTex1);
    inline void         SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR4 *puvTex1);
    inline void         SetTexCoord    (PVOID pvPoint, UINT iTexCoord, float *);
  
    inline void         SetWeight      (PVOID pvPoint, UINT iWeight,   float fWeight);
    inline void         SetIndices     (PVOID pvPoint, DWORD dwIndices);


    inline D3DXVECTOR3 *PvGetPosition    (PVOID pvPoint);
    inline D3DXVECTOR3 *PvGetNormal      (PVOID pvPoint);
    inline float        FGetPointSize    (PVOID pvPoint);
    inline D3DCOLOR     ColorGetDiffuse  (PVOID pvPoint);
    inline D3DCOLOR     ColorGetSpecular (PVOID pvPoint);
    inline D3DXVECTOR2 *PuvGetTex1       (PVOID pvPoint);
	inline float		*PfGetWeights	 (PVOID pvPoint);
    inline DWORD        DwGetIndices     (PVOID pvPoint);

    inline float       *PxGetTexCoord   (PVOID pvPoint,UINT iTexCoord);
    inline D3DXVECTOR2 *PxyGetTexCoord  (PVOID pvPoint,UINT iTexCoord);
    inline D3DXVECTOR3 *PxyzGetTexCoord (PVOID pvPoint,UINT iTexCoord);
    inline D3DXVECTOR4 *PxyzwGetTexCoord(PVOID pvPoint,UINT iTexCoord);

    inline float        FGetWeight     (PVOID pvPoint, UINT iWeight);

    inline bool         BPosition()     { return true; }
    inline bool         BNormal()       { return m_oNormal > 0; }
    inline bool         BPointSize()    { return m_oPointSize > 0; }
    inline bool         BDiffuse()      { return m_oDiffuse > 0; }
    inline bool         BSpecular()     { return m_oSpecular > 0; }
    inline bool         BTex1()         { return m_oTex[0] > 0; }

    inline DWORD        CWeights()      { return m_cWeights; }
    inline DWORD        CTexCoords()    { return m_cTexCoords; }
    inline DWORD        CbTexCoordSize(DWORD iTexCoord);
    inline void         GetTexCoordSizes(DWORD *rgdwTexCoordSizes);

    inline BOOL         BIndexedWeights() { return (m_dwFVF & D3DFVF_LASTBETA_UBYTE4|D3DFVF_LASTBETA_D3DCOLOR) && m_cWeights; }


    DWORD m_dwFVF;
    DWORD m_cBytesPerVertex; 
    DWORD m_oNormal;
    DWORD m_oPointSize;
    DWORD m_oDiffuse;
    DWORD m_oSpecular;
    DWORD m_oTex[8];
    DWORD m_cWeights;
    DWORD m_cTexCoords;
};


// -------------------------------------------------------------------------------
//  class DXCrackFVF inline functions - used to simplify access to the FVF 
//

const DWORD x_rgdwTexCoordSize[] = { 2 * sizeof(float), 3 * sizeof(float), 4 * sizeof(float), sizeof(float)};

// -------------------------------------------------------------------------------
//  constructor for DXCrackFVF, generates offsets used to access members
//      of the given FVF
//
inline 
DXCrackFVF::DXCrackFVF(DWORD dwFVF)
:m_dwFVF(dwFVF),
m_oNormal(0),
m_oPointSize(0),
m_oDiffuse(0),
m_oSpecular(0),
m_cWeights(0),
m_cTexCoords(0)
{
    DWORD oCur = 0;
    DWORD dwPosition;
    DWORD dwTexCoordInfo;
    DWORD iTexCoord;
    DWORD dwTexCoordSize;
    m_oTex[0] = 0;

    dwPosition = dwFVF & D3DFVF_POSITION_MASK;

    if (dwPosition == D3DFVF_XYZ)
    {
        oCur += 3 * sizeof(float);
    }
    else if (dwPosition == D3DFVF_XYZRHW)
    {
        oCur += 4 * sizeof(float);
    }
    else if (dwPosition >= D3DFVF_XYZB1)
    {
        m_cWeights = (dwPosition >> 1) - 2;
        oCur += (3 + m_cWeights) * sizeof(float);
    }

    if ( dwFVF & D3DFVF_NORMAL )
    {
        m_oNormal = oCur;
        oCur += 3 * sizeof(float);
    }

    if ( dwFVF & D3DFVF_PSIZE )
    {
        m_oPointSize = oCur;
        oCur += sizeof(float);
    }

    if ( dwFVF & D3DFVF_DIFFUSE )
    {
        m_oDiffuse = oCur;
        oCur += sizeof(D3DCOLOR);
    }

    if ( dwFVF & D3DFVF_SPECULAR )
    {
        m_oSpecular = oCur;
        oCur += sizeof(D3DCOLOR);
    }

    if ( dwFVF & D3DFVF_TEXCOUNT_MASK )
    {
        m_oTex[0] = oCur;

        // only one tu/tv set currently supported

        m_cTexCoords = ((dwFVF & D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);

        if(m_cTexCoords > 8)
            m_cTexCoords = 8;

        // all 2d, then go quick route
        //if ((dwFVF & 0xffff0000) == 0)
        //{
            //oCur += sizeof(float) * 2 * m_cTexCoords;
        //}
        //else
        {
            dwTexCoordSize = (dwFVF & 0xffff0000) >> 16;
            for (iTexCoord = 0; iTexCoord < m_cTexCoords; iTexCoord++)
            {
                m_oTex[iTexCoord] = oCur;
                oCur += x_rgdwTexCoordSize[dwTexCoordSize & 0x3];
                dwTexCoordSize = dwTexCoordSize >> 2;
            }
        }
    }

    m_cBytesPerVertex = oCur;
}


// -------------------------------------------------------------------------------
//  function - GetArrayElem,  gets the ith element of the given FVF array
//
inline PBYTE 
DXCrackFVF::GetArrayElem(PVOID pvPoints, DWORD iElem)
{ 
    return ((PBYTE)pvPoints + (iElem * m_cBytesPerVertex)); 
}

// -------------------------------------------------------------------------------
//  function - SetPosition, sets the given position into the given FVF point
//
inline void 
DXCrackFVF::SetPosition    (PVOID pvPoint, D3DXVECTOR3 *pvPos)
{
    memcpy(pvPoint, pvPos, sizeof(D3DXVECTOR3));
}

// -------------------------------------------------------------------------------
//  function - SetNormal, sets the given normal into the given FVF point
//                  if no normal, this call is a no-op
//
inline void 
DXCrackFVF::SetNormal      (PVOID pvPoint, D3DXVECTOR3 *pvNormal)
{
    if (m_oNormal)
    {
        memcpy(((PBYTE)pvPoint) + m_oNormal, pvNormal, sizeof(D3DXVECTOR3));
    }
}

// -------------------------------------------------------------------------------
//  function - SetPointsize, sets the given point size into the given FVF point
//                  if no normal, this call is a no-op
//
inline void 
DXCrackFVF::SetPointSize   (PVOID pvPoint, float fPointSize)
{
    if (m_oPointSize)
    {
        memcpy(((PBYTE)pvPoint) + m_oPointSize, &fPointSize, sizeof(float));
    }
}

// -------------------------------------------------------------------------------
//  function - SetDiffuse, sets the given diffuse color into the given FVF point
//                  if no diffuse color, this call is a no-op
//
inline void 
DXCrackFVF::SetDiffuse     (PVOID pvPoint, D3DCOLOR color)
{
    if (m_oDiffuse)
    {
        memcpy(((PBYTE)pvPoint) + m_oDiffuse, &color, sizeof(D3DCOLOR));
    }
}

// -------------------------------------------------------------------------------
//  function - SetSpecular, sets the given specular color into the given FVF point
//                  if no specular color, this call is a no-op
//
inline void 
DXCrackFVF::SetSpecular    (PVOID pvPoint, D3DCOLOR color)
{
    if (m_oSpecular)
    {
        memcpy(((PBYTE)pvPoint) + m_oSpecular, &color, sizeof(D3DCOLOR));
    }
}

// -------------------------------------------------------------------------------
//  function - SetTex1, sets the given Tex1 UV coords into the given FVF point
//                  if no Tex1 coords, this call is a no-op
//
inline void 
DXCrackFVF::SetTex1        (PVOID pvPoint, D3DXVECTOR2 *puvTex1)
{
    if (m_oTex[0])
    {
        memcpy(((PBYTE)pvPoint) + m_oTex[0], puvTex1, sizeof(D3DXVECTOR2));
    }
}


//above function is skecthy, won't work for variable size textore coordinates, replaced
//with following family
inline void 
DXCrackFVF::SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR2 *pvTexCoord)
{
    if (iTexCoord < m_cTexCoords)
    {
        memcpy(((PBYTE)pvPoint) + m_oTex[iTexCoord], pvTexCoord, sizeof(D3DXVECTOR2));
    }
}

inline void 
DXCrackFVF::SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR3 *pvTexCoord)
{
    if (iTexCoord < m_cTexCoords)
    {
        memcpy(((PBYTE)pvPoint) + m_oTex[iTexCoord], pvTexCoord, sizeof(D3DXVECTOR3));
    }
}

inline void 
DXCrackFVF::SetTexCoord    (PVOID pvPoint, UINT iTexCoord, D3DXVECTOR4 *pvTexCoord)
{
    if (iTexCoord < m_cTexCoords)
    {
        memcpy(((PBYTE)pvPoint) + m_oTex[iTexCoord], pvTexCoord, sizeof(D3DXVECTOR4));
    }
}

inline void 
DXCrackFVF::SetTexCoord(PVOID pvPoint, UINT iTexCoord, float *pvTexCoord)
{
    if (iTexCoord < m_cTexCoords)
    {
        memcpy(((PBYTE)pvPoint) + m_oTex[iTexCoord], pvTexCoord, sizeof(float));
    }
}




// -------------------------------------------------------------------------------

//  function - SetWeight, sets the given blend weight into the given FVF point
//                  if the specified blend weight is not present, just return
//
inline void 
DXCrackFVF::SetWeight     (PVOID pvPoint, UINT iWeight, float fWeight)
{
    if (iWeight < m_cWeights)
    {
        memcpy(((PBYTE)pvPoint) + sizeof(D3DXVECTOR3) + sizeof(float) * iWeight, &fWeight, sizeof(float));
    }
}

// -------------------------------------------------------------------------------

//  function - SetWeight, sets the given blend weight into the given FVF point
//                  if the specified blend weight is not present, just return
//
inline float
DXCrackFVF::FGetWeight     (PVOID pvPoint, UINT iWeight)
{
    return *(float*)(((PBYTE)pvPoint) + sizeof(D3DXVECTOR3) + sizeof(float) * iWeight);
}

// -------------------------------------------------------------------------------
//  function - SetIndices, sets the given indices into the given FVF point
//                  if no indices, this call is a no-op
//
inline void
DXCrackFVF::SetIndices    (PVOID pvPoint, DWORD dwIndices)
{
    if (BIndexedWeights())
    {
        *((LPDWORD)(((PBYTE)pvPoint) + sizeof(D3DXVECTOR3) + sizeof(float) * (CWeights() - 1))) = dwIndices;
    }
}

// -------------------------------------------------------------------------------
//  function - PvGetPosition, returns a pointer to the position in the given FVF vertex
//
inline D3DXVECTOR3  *
DXCrackFVF::PvGetPosition    (PVOID pvPoint)
{
    return (D3DXVECTOR3*)pvPoint;
}

// -------------------------------------------------------------------------------
//  function - PvGetNormal, returns a pointer to the normal in the given FVF vertex
//                              NOTE: the normal HAS to be present in the FVF format
//
inline D3DXVECTOR3 *
DXCrackFVF::PvGetNormal      (PVOID pvPoint)
{
    return (D3DXVECTOR3*)  (((PBYTE)pvPoint) + m_oNormal);
}

// -------------------------------------------------------------------------------
//  function - FGetPointSize, returns a the pointsize in the given FVF vertex
//                              NOTE: the point size HAS to be present in the FVF format
//
inline float
DXCrackFVF::FGetPointSize    (PVOID pvPoint)
{
    return *( (float*)  (((PBYTE)pvPoint) + m_oPointSize) );
}

// -------------------------------------------------------------------------------
//  function - ColorGetDiffuse, returns the diffuse color in the given FVF vertex
//                              NOTE: the color HAS to be present in the FVF format
//
inline D3DCOLOR  
DXCrackFVF::ColorGetDiffuse  (PVOID pvPoint)
{
    return *(D3DCOLOR*)  (((PBYTE)pvPoint) + m_oDiffuse);
}

// -------------------------------------------------------------------------------
//  function - ColorGetSpecular, returns the specular color in the given FVF vertex
//                              NOTE: the color HAS to be present in the FVF format
//
inline D3DCOLOR  
DXCrackFVF::ColorGetSpecular (PVOID pvPoint)
{
    return *(D3DCOLOR*)  (((PBYTE)pvPoint) + m_oSpecular);
}

// -------------------------------------------------------------------------------
//  function - PuvGetTex1, returns a pointer to the tex1 position in the given FVF vertex
//                              NOTE: the tex1 HAS to be present in the FVF format
//
inline D3DXVECTOR2  *
DXCrackFVF::PuvGetTex1       (PVOID pvPoint)
{
    return (D3DXVECTOR2*)  (((PBYTE)pvPoint) + m_oTex[0]);
}



//family of functions that cast the texture stage to a D3DXVector
//of the right size

inline float*
DXCrackFVF::PxGetTexCoord(PVOID pvPoint,UINT index)
{
    return (float*) (((PBYTE)pvPoint) + m_oTex[index]);
}

inline D3DXVECTOR2  *
DXCrackFVF::PxyGetTexCoord(PVOID pvPoint,UINT index)
{
    return (D3DXVECTOR2*) (((PBYTE)pvPoint) + m_oTex[index]);
}


inline D3DXVECTOR3  *
DXCrackFVF::PxyzGetTexCoord(PVOID pvPoint,UINT index)
{
    return (D3DXVECTOR3*) (((PBYTE)pvPoint) + m_oTex[index]);
}


inline D3DXVECTOR4  *
DXCrackFVF::PxyzwGetTexCoord(PVOID pvPoint,UINT index)
{
    return (D3DXVECTOR4*) (((PBYTE)pvPoint) + m_oTex[index]);
}
// -------------------------------------------------------------------------------
//  function - PuvGetWeights, returns a pointer to the first weight 
//                              NOTE: the weights HAS to be present in the FVF format
//
inline float  *
DXCrackFVF::PfGetWeights       (PVOID pvPoint)
{
    return (float*)(((PBYTE)pvPoint) + sizeof(D3DXVECTOR3));
}

// -------------------------------------------------------------------------------
//  function - CbTexCoordSize, returns the size of the given set of texture coordinates
//
inline DWORD
DXCrackFVF::CbTexCoordSize       (DWORD iTexCoord)
{
    if (iTexCoord < m_cTexCoords)
        return x_rgdwTexCoordSize[(m_dwFVF >> (16 + iTexCoord * 2)) & 0x3];
    else
        return 0;
}

// -------------------------------------------------------------------------------
//  function - CbTexCoordSize, returns the sizes of all 8 texture coords (0 if not present)
//
inline void
DXCrackFVF::GetTexCoordSizes       (DWORD *rgdwTexCoordSizes)
{
    DWORD iTexCoord;

    for (iTexCoord = 0; iTexCoord < m_cTexCoords; iTexCoord++)
    {
       
        rgdwTexCoordSizes[iTexCoord] = x_rgdwTexCoordSize[(m_dwFVF >> (16 + iTexCoord * 2)) & 0x3];

 
    }

    // set the rest to 0
    for (;iTexCoord < 8; iTexCoord++)
    {
        rgdwTexCoordSizes[iTexCoord] = 0;
    }
}


    inline void         GetTexCoordSizes(DWORD *rgdwTexCoordSizes);

// -------------------------------------------------------------------------------
//  function - DwGetIndices, returns the indices in the given FVF vertex
//                              NOTE: the indices HAS to be present in the FVF format
//
inline DWORD
DXCrackFVF::DwGetIndices    (PVOID pvPoint)
{
    return *( (DWORD*)  (((PBYTE)pvPoint) + sizeof(D3DXVECTOR3) + sizeof(float) * (CWeights() - 1)) );
}

#endif //__DXCRACKFVF_H__
