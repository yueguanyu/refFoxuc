//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//  File:       crackdecl.h
//  Content:    Used to access vertex data using Declarations or FVFs
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __CRACKDECL_H__
#define __CRACKDECL_H__

#include <d3dx9.h>

//----------------------------------------------------------------------------
// CD3DXCrackDecl
//----------------------------------------------------------------------------

class CD3DXCrackDecl
{
protected:
    
    //just a pointer to the Decl! No data stored!
    CONST D3DVERTEXELEMENT9 *pElements;
    DWORD dwNumElements;

    //still need the stream pointer though
    //and the strides
    LPBYTE pStream[16];
    DWORD  dwStride[16];

public:

    CD3DXCrackDecl();
   

    HRESULT SetDeclaration( CONST D3DVERTEXELEMENT9 *pDecl);
    HRESULT SetStreamSource(UINT Stream, LPVOID pData,UINT Stride);

    // Get
    inline UINT GetVertexStride(UINT Stream);

    
    inline UINT GetFields(CONST D3DVERTEXELEMENT9*);

    inline LPBYTE GetVertex(UINT Stream,  UINT Index);
    inline LPBYTE GetElementPointer(CONST D3DVERTEXELEMENT9 *Element,UINT Index);


    inline CONST D3DVERTEXELEMENT9 *GetSemanticElement(UINT Usage,UINT UsageIndex);
    //simple function that gives part of the decl back 
    inline CONST D3DVERTEXELEMENT9 *GetIndexElement(UINT Index);

    // Encode/Decode
    VOID Decode(CONST D3DVERTEXELEMENT9 *, UINT Index, FLOAT* pData, UINT cData);
    VOID Encode(CONST D3DVERTEXELEMENT9 *, UINT Index, CONST FLOAT* pData, UINT cData);

    inline VOID DecodeSemantic(UINT Usage,UINT UsageIndex,UINT VertexIndex, FLOAT *pData,UINT cData);
    inline VOID EncodeSemantic(UINT Usage,UINT UsageIndex,UINT VertexIndex, FLOAT *pData,UINT cData);

    inline CONST D3DVERTEXELEMENT9 *GetElements() { return pElements; };
    inline DWORD GetNumElements() { return dwNumElements; };
};


//----------------------------------------------------------------------------
// CD3DXCrackDecl inline methods
//----------------------------------------------------------------------------
inline CONST D3DVERTEXELEMENT9*
CD3DXCrackDecl::GetIndexElement(UINT Index)
{
    if(Index < dwNumElements)
        return &pElements[Index];
    else
        return NULL;
}

inline CONST D3DVERTEXELEMENT9*
CD3DXCrackDecl::GetSemanticElement(UINT Usage,UINT UsageIndex)
{
    CONST D3DVERTEXELEMENT9 *pPlace = pElements;
    while(pPlace->Stream != 0xFF)
    {
        if(pPlace->Usage == Usage &&
           pPlace->UsageIndex == UsageIndex)
           return pPlace;
        pPlace++;
    }
    return NULL;
}

inline VOID 
CD3DXCrackDecl::DecodeSemantic(UINT Usage,UINT UsageIndex,UINT VertexIndex, FLOAT *pData,UINT cData)
{
    Decode(GetSemanticElement(Usage,UsageIndex),VertexIndex,pData,cData);
}

inline VOID 
CD3DXCrackDecl::EncodeSemantic(UINT Usage,UINT UsageIndex,UINT VertexIndex, FLOAT *pData,UINT cData)
{
    Encode(GetSemanticElement(Usage,UsageIndex),VertexIndex,pData,cData);
}


inline UINT 
CD3DXCrackDecl::GetVertexStride(UINT Stream)
{
    return dwStride[Stream];
}

const BYTE x_rgcbFields[] = 
{
    1, // D3DDECLTYPE_FLOAT1,        // 1D float expanded to (value, 0., 0., 1.)
    2, // D3DDECLTYPE_FLOAT2,        // 2D float expanded to (value, value, 0., 1.)
    3, // D3DDECLTYPE_FLOAT3,       / 3D float expanded to (value, value, value, 1.)
    4, // D3DDECLTYPE_FLOAT4,       / 4D float
    4, // D3DDECLTYPE_D3DCOLOR,      // 4D packed unsigned bytes mapped to 0. to 1. range
    //                      // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    4, // D3DDECLTYPE_UBYTE4,        // 4D unsigned byte
    2, // D3DDECLTYPE_SHORT2,        // 2D signed short expanded to (value, value, 0., 1.)
    4, // D3DDECLTYPE_SHORT4         // 4D signed short

    4, // D3DDECLTYPE_UBYTE4N,       // Each of 4 bytes is normalized by dividing to 255.0
    2, // D3DDECLTYPE_SHORT2N,       // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    4, // D3DDECLTYPE_SHORT4N,       // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    2, // D3DDECLTYPE_USHORT2N,      // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    4, // D3DDECLTYPE_USHORT4N,      // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    3, // D3DDECLTYPE_UDEC3,         // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    3, // D3DDECLTYPE_DEC3N,         // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    2, // D3DDECLTYPE_FLOAT16_2,     // 2D 16 bit float expanded to (value, value, 0, 1 )
    4, // D3DDECLTYPE_FLOAT16_4,     // 4D 16 bit float 
    0, // D3DDECLTYPE_UNKNOWN,       // Unknown
};


inline UINT 
CD3DXCrackDecl::GetFields(CONST D3DVERTEXELEMENT9 *pElement)
{
    if(pElement->Type <= D3DDECLTYPE_FLOAT16_4)
        return x_rgcbFields[pElement->Type];

    return 0;
}



inline LPBYTE 
CD3DXCrackDecl::GetVertex(UINT Stream, UINT Index)
{
    return pStream[Stream] + dwStride[Stream] * Index;
}


inline LPBYTE 
CD3DXCrackDecl::GetElementPointer(CONST D3DVERTEXELEMENT9 *pElement, UINT Index)
{
    return GetVertex(pElement->Stream, Index) + pElement->Offset;
}


//----------------------------------------------------------------------------
// CD3DXCrackDecl1 - Single stream enhanced crackdecl (combine CrackFVF and CrackDecl
//----------------------------------------------------------------------------

class CD3DXCrackDecl1 : public CD3DXCrackDecl
{
public:
    
    CONST D3DVERTEXELEMENT9 *pPositionElement;
    CONST D3DVERTEXELEMENT9 *pWeightElement;
    CONST D3DVERTEXELEMENT9 *pIndexElement;
    CONST D3DVERTEXELEMENT9 *pNormalElement;
    CONST D3DVERTEXELEMENT9 *pDiffuseElement;
    CONST D3DVERTEXELEMENT9 *pSpecularElement;

    CONST D3DVERTEXELEMENT9 *rgpTextureElements[8];

    DWORD m_cBytesPerVertex;

    BOOL BPosition()                                 { return pPositionElement != NULL; }
    BOOL BWeights()                                  { return pWeightElement != NULL; }
    BOOL BIndexedWeights()                           { return pIndexElement != NULL; }
    BOOL BNormal()                                   { return pNormalElement != NULL; }
    BOOL BDiffuse()                                  { return pDiffuseElement != NULL; }
    BOOL BSpecular()                                 { return pSpecularElement != NULL; }
    BOOL BTexCoord(UINT iTexCoord)                   { return rgpTextureElements[iTexCoord] != NULL; }
                                                 
    BOOL CWeights()                                  { return pWeightElement == NULL ? 0 : (pWeightElement->Type - D3DDECLTYPE_FLOAT1) + 1; }

    LPD3DXVECTOR3 PvGetPosition(PBYTE pvPoint)       { return (LPD3DXVECTOR3)(pvPoint + pPositionElement->Offset); }
    LPD3DXVECTOR3 PvGetNormal(PBYTE pvPoint)         { return (LPD3DXVECTOR3)(pvPoint + pNormalElement->Offset); }
    FLOAT *PfGetWeights(PBYTE pvPoint)               { return (FLOAT*)(pvPoint + pWeightElement->Offset); }
    LPDWORD PdwGetIndices(PBYTE pvPoint)             { return (LPDWORD)(pvPoint + pIndexElement->Offset); }
    FLOAT FGetWeight(PBYTE pvPoint, UINT iWeight)    { return ((FLOAT*)(pvPoint + pWeightElement->Offset))[iWeight]; }
    LPD3DXVECTOR2 PuvGetTex1(PBYTE pvPoint)          { return (LPD3DXVECTOR2)(pvPoint + rgpTextureElements[0]->Offset); }

    void ColorGetDiffuse(PBYTE pvPoint, LPD3DXCOLOR pColor)
    {
        if (pDiffuseElement->Type == D3DDECLTYPE_D3DCOLOR)
        {
            *pColor = D3DXCOLOR(*(D3DCOLOR*)(pvPoint + pDiffuseElement->Offset));
        }
        else
        {
            *pColor = *(LPD3DXCOLOR)(pvPoint + pDiffuseElement->Offset);
        }
    }

    void ColorGetSpecular(PBYTE pvPoint, LPD3DXCOLOR pColor)
    {
        if (pSpecularElement->Type == D3DDECLTYPE_D3DCOLOR)
        {
            *pColor = D3DXCOLOR(*(D3DCOLOR*)(pvPoint + pSpecularElement->Offset));
        }
        else
        {
            *pColor = *(LPD3DXCOLOR)(pvPoint + pSpecularElement->Offset);
        }
    }

    PBYTE GetArrayElem(PBYTE pvPoints, UINT iVertex) { return pvPoints + m_cBytesPerVertex * iVertex; }

    void SetNormal(PBYTE pvPoint, LPD3DXVECTOR3 pvNormal)
    {
        if (pNormalElement != NULL)
        {
            *(LPD3DXVECTOR3)(pvPoint + pNormalElement->Offset) = *pvNormal;
        }
    }

    // no check set position
    void SetPositionN(PBYTE pvPoint, LPD3DXVECTOR3 pvNormal)
    {
        *(LPD3DXVECTOR3)(pvPoint + pPositionElement->Offset) = *pvNormal;
    }
    // no check set normal
    void SetNormalN(PBYTE pvPoint, LPD3DXVECTOR3 pvNormal)
    {
        *(LPD3DXVECTOR3)(pvPoint + pNormalElement->Offset) = *pvNormal;
    }

    CD3DXCrackDecl1()
        :CD3DXCrackDecl(),
         pPositionElement(NULL),
         pWeightElement(NULL),
         pIndexElement(NULL),
         pNormalElement(NULL),
         pDiffuseElement(NULL),
         pSpecularElement(NULL)
        {
        memset(rgpTextureElements, 0, sizeof(LPD3DVERTEXELEMENT9) * 8);
        }
  
    CD3DXCrackDecl1(CONST D3DVERTEXELEMENT9 *pElementsNew)
        :CD3DXCrackDecl(),
         pPositionElement(NULL),
         pWeightElement(NULL),
         pIndexElement(NULL),
         pNormalElement(NULL),
         pDiffuseElement(NULL),
         pSpecularElement(NULL)
    {
        memset(rgpTextureElements, 0, sizeof(LPD3DVERTEXELEMENT9) * 8);
        SetDeclaration(pElementsNew);
    }

    HRESULT SetDeclaration(CONST D3DVERTEXELEMENT9 *pElementsNew)
    {
        CONST D3DVERTEXELEMENT9 *pPlace;

        CD3DXCrackDecl::SetDeclaration(pElementsNew);

#ifdef DEBUG
        // this class only works for single stream data types
        pPlace = pElementsNew;
        while(pPlace->Stream != 0xFF)
        {
            GXASSERT(pPlace->Stream == 0);
            pPlace++;
        }
#endif
        // should be pointing into local copy of declaration
        pPlace = pElements;

        while(pPlace->Stream != 0xFF)
        {
            if (pPlace->UsageIndex == 0 || pPlace->Usage == D3DDECLUSAGE_TEXCOORD || 
                pPlace->Usage == D3DDECLUSAGE_COLOR )
            {
                switch (pPlace->Usage)
                {
                case D3DDECLUSAGE_POSITION:
                    if (pPlace->Type == D3DDECLTYPE_FLOAT3)
                    {
                        pPositionElement = pPlace;
                    }
                    break;

                case D3DDECLUSAGE_BLENDWEIGHT:
                    if ((pPlace->Type >= D3DDECLTYPE_FLOAT1) && (pPlace->Type <= D3DDECLTYPE_FLOAT4))
                    {
                        pWeightElement = pPlace;
                    }
                    break;

                case D3DDECLUSAGE_BLENDINDICES:
                    if ((pPlace->Type == D3DDECLTYPE_UBYTE4) || (pPlace->Type == D3DDECLTYPE_D3DCOLOR))
                    {
                        pIndexElement = pPlace;
                    }
                    break;

                case D3DDECLUSAGE_NORMAL:
                    if (pPlace->Type == D3DDECLTYPE_FLOAT3)
                    {
                        pNormalElement = pPlace;
                    }
                    break;

                case D3DDECLUSAGE_COLOR:
                    if (pPlace->UsageIndex == 0)
                    {
                        if ((pPlace->Type == D3DDECLTYPE_FLOAT4) || (pPlace->Type == D3DDECLTYPE_D3DCOLOR))
                        {
                            pDiffuseElement = pPlace;
                        }
                    }
                    else
                    if (pPlace->UsageIndex == 1)
                    {
                        if ((pPlace->Type == D3DDECLTYPE_FLOAT4) || (pPlace->Type == D3DDECLTYPE_D3DCOLOR))
                        {
                            pSpecularElement = pPlace;
                        }
                    }
                    break;

                case D3DDECLUSAGE_TEXCOORD:
                    if ((pPlace->Type >= D3DDECLTYPE_FLOAT1) && (pPlace->Type <= D3DDECLTYPE_FLOAT4) && (pPlace->UsageIndex < 8))
                    {
                        rgpTextureElements[pPlace->UsageIndex] = pPlace;
                    }
                    break;
                }
            }

            pPlace++;
        }

        m_cBytesPerVertex = D3DXGetDeclVertexSize(pElements,0);

        return S_OK;
    }


};

//----------------------------------------------------------------------------
// Very Useful decl functions!!
//----------------------------------------------------------------------------
// 

// struct used to contain state for cross fvf vertex copies
struct SVertexCopyContext
{
    CD3DXCrackDecl1 cdSrc;
    CD3DXCrackDecl1 cdDest;

    DWORD cDeclsToCopy;
    CONST D3DVERTEXELEMENT9 *rgpCopyDeclSrc[MAX_FVF_DECL_SIZE];
    CONST D3DVERTEXELEMENT9 *rgpCopyDeclDest[MAX_FVF_DECL_SIZE];

    // setup info required for fvf conversion
    SVertexCopyContext(CONST D3DVERTEXELEMENT9 *pDeclSrc, CONST D3DVERTEXELEMENT9 *pDeclDest)
        :cdSrc(pDeclSrc), cdDest(pDeclDest), cDeclsToCopy(0)
    {
        CONST D3DVERTEXELEMENT9 *pPlace = pDeclSrc;
        CONST D3DVERTEXELEMENT9 *pDecl;

        // look through all the source decls
        while(pPlace->Stream != 0xFF)
        {
            // see if there is a destination decl that matches this ones usage
            pDecl = cdDest.GetSemanticElement(pPlace->Usage, pPlace->UsageIndex);

            // if one was found, save off the pair of vertex elements to copy between
            //      NOTE: the type may not match, but encode and decode will make that work out
            // D3DDECLMETHOD_UV is fantom data - it has no input, just output
            if ((pDecl != NULL) && (pPlace->Method != D3DDECLMETHOD_UV) && (pDecl->Method != D3DDECLMETHOD_UV))
            {
                rgpCopyDeclSrc[cDeclsToCopy] = pPlace;
                rgpCopyDeclDest[cDeclsToCopy] = pDecl;
                cDeclsToCopy += 1;
            }

            pPlace++;
        }
    }

    void SetStreamSource(PBYTE pbSrc, PBYTE pbDest)
    {
        cdSrc.SetStreamSource(0, pbSrc, 0);
        cdDest.SetStreamSource(0, pbDest, 0);
    }

    // copy a vertex doing the fvf conversion in the process
    void CopyVertex(UINT iVertSrc, UINT iVertDest, BOOL bInit)
    {
        DWORD iDecl;
        FLOAT rgfTemp[4];
        UINT iWeight;
        UINT cWeights;
        FLOAT fSum;

        // some usages may already have initialized other vertex elements
        if (bInit)
        {
            memset(cdDest.GetVertex(0, iVertDest), 0, cdDest.m_cBytesPerVertex);
        }

        for (iDecl = 0; iDecl < cDeclsToCopy; iDecl++)
        {
            cdSrc.Decode(rgpCopyDeclSrc[iDecl], iVertSrc, rgfTemp, 4);

            // special case blend weights, need to generate implied weight
            if (rgpCopyDeclSrc[iDecl]->Usage == D3DDECLUSAGE_BLENDWEIGHT)
            {
                // UNDONE UNDONE - is there any way to handle other datatypes in this case?


                // NOTE: it is valid to bypass this in the case of FLOAT4!!! there is no implied weight then
                if ((rgpCopyDeclSrc[iDecl]->Type >= D3DDECLTYPE_FLOAT1) && (rgpCopyDeclSrc[iDecl]->Type <= D3DDECLTYPE_FLOAT3))
                {
                    cWeights = (rgpCopyDeclSrc[iDecl]->Type - D3DDECLTYPE_FLOAT1) + 1;

                    // calculate the sum of the specified weights
                    fSum = 0.0f;
                    for (iWeight = 0; iWeight < cWeights; iWeight++)
                    {
                        fSum += rgfTemp[iWeight];
                    }

                    GXASSERT((cWeights > 0) && cWeights <= 3);

                    // set the implied weight
                    rgfTemp[cWeights] = 1.0f - fSum;
                }
            }

            cdDest.Encode(rgpCopyDeclDest[iDecl], iVertDest, rgfTemp, 4);
        }
    }
};

inline BOOL BIdenticalDecls(CONST D3DVERTEXELEMENT9 *pDecl1, CONST D3DVERTEXELEMENT9 *pDecl2)
{
    CONST D3DVERTEXELEMENT9 *pCurSrc = pDecl1;
    CONST D3DVERTEXELEMENT9 *pCurDest = pDecl2;
    while ((pCurSrc->Stream != 0xff) && (pCurDest->Stream != 0xff))
    {
//        if (memcmp(pCurSrc, pCurDest, sizeof(D3DVERTEXELEMENT9)) != 0)
//            break;
        if ((pCurDest->Stream != pCurSrc->Stream) 
            && (pCurDest->Offset != pCurSrc->Offset) 
            || (pCurDest->Type != pCurSrc->Type) 
            || (pCurDest->Method != pCurSrc->Method) 
            || (pCurDest->Usage != pCurSrc->Usage) 
            || (pCurDest->UsageIndex != pCurSrc->UsageIndex))
            break;

        pCurSrc++;
        pCurDest++;
    }

    // it is the same decl if reached the end at the same time on both decls
    return ((pCurSrc->Stream == 0xff) && (pCurDest->Stream == 0xff));
}

inline void CopyDecls(D3DVERTEXELEMENT9 *pDest, CONST D3DVERTEXELEMENT9 *pSrc)
{
    while (pSrc->Stream != 0xff)
    {
        memcpy(pDest, pSrc, sizeof(D3DVERTEXELEMENT9));

        pSrc++;
        pDest++;
    }
    memcpy(pDest, pSrc, sizeof(D3DVERTEXELEMENT9));
}

const UINT x_cTypeSizes = D3DDECLTYPE_FLOAT16_4;

const BYTE x_rgcbTypeSizes[] = 
{  
    4, // D3DDECLTYPE_FLOAT1,        // 1D float expanded to (value, 0., 0., 1.)
    8, // D3DDECLTYPE_FLOAT2,        // 2D float expanded to (value, value, 0., 1.)
    12, // D3DDECLTYPE_FLOAT3,       / 3D float expanded to (value, value, value, 1.)
    16, // D3DDECLTYPE_FLOAT4,       / 4D float
    4, // D3DDECLTYPE_D3DCOLOR,      // 4D packed unsigned bytes mapped to 0. to 1. range
    //                      // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    4, // D3DDECLTYPE_UBYTE4,        // 4D unsigned byte
    4, // D3DDECLTYPE_SHORT2,        // 2D signed short expanded to (value, value, 0., 1.)
    8, // D3DDECLTYPE_SHORT4         // 4D signed short

    4, // D3DDECLTYPE_UBYTE4N,       // Each of 4 bytes is normalized by dividing to 255.0
    4, // D3DDECLTYPE_SHORT2N,       // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    8, // D3DDECLTYPE_SHORT4N,       // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    4, // D3DDECLTYPE_USHORT2N,      // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    8, // D3DDECLTYPE_USHORT4N,      // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    4, // D3DDECLTYPE_UDEC3,         // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    4, // D3DDECLTYPE_DEC3N,         // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    4, // D3DDECLTYPE_FLOAT16_2,     // 2D 16 bit float expanded to (value, value, 0, 1 )
    8, // D3DDECLTYPE_FLOAT16_4,     // 4D 16 bit float 
    0, // D3DDECLTYPE_UNUSED,        // Unused
};


inline CONST D3DVERTEXELEMENT9* GetDeclElement(CONST D3DVERTEXELEMENT9 *pDecl, BYTE Usage, BYTE UsageIndex)
{
    while (pDecl->Stream != 0xff)
    {
        if ((pDecl->Usage == Usage) && (pDecl->UsageIndex == UsageIndex))
        {
            return pDecl;
        }

        pDecl++;
    }

    return NULL;
}

inline void RemoveDeclElement(BYTE Usage, BYTE UsageIndex, D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE])
{
    LPD3DVERTEXELEMENT9 pCur;
    LPD3DVERTEXELEMENT9 pPrev;
    BYTE cbElementSize;

    pCur = pDecl;
    while (pCur->Stream != 0xff)
    {
        if ((pCur->Usage == Usage) && (pCur->UsageIndex == UsageIndex))
        {
            break;
        }
        pCur++;
    }

    //. if we found one to remove, then remove it
    if (pCur->Stream != 0xff)
    {
        cbElementSize = x_rgcbTypeSizes[pCur->Type];;

        pPrev = pCur;
        pCur++;
        while (pCur->Stream != 0xff)
        {
            memcpy(pPrev, pCur, sizeof(D3DVERTEXELEMENT9));

            pPrev->Offset -= cbElementSize;

            pPrev++;
            pCur++;
        }

        // copy the end of stream down one
        memcpy(pPrev, pCur, sizeof(D3DVERTEXELEMENT9));
    }

}

// NOTE: size checking of array should happen OUTSIDE this function!
inline void AppendDeclElement(CONST D3DVERTEXELEMENT9 *pNew, D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE])
{
    LPD3DVERTEXELEMENT9 pCur;
    BYTE cbOffset;
    BYTE cbNewOffset;
    D3DVERTEXELEMENT9   TempElement1;
    D3DVERTEXELEMENT9   TempElement2;
    UINT iCur;

    pCur = pDecl;
    cbOffset = 0;
    while ((pCur->Stream != 0xff))
    {
        cbOffset += x_rgcbTypeSizes[pCur->Type];

        pCur++;
    }
    
    // NOTE: size checking of array should happen OUTSIDE this function!
    GXASSERT(pCur - pDecl + 1 < MAX_FVF_DECL_SIZE);

    // move the end of the stream down one
    memcpy(pCur+1, pCur, sizeof(D3DVERTEXELEMENT9));

    // copy the new element in and update the offset
    memcpy(pCur, pNew, sizeof(D3DVERTEXELEMENT9));
    pCur->Offset = cbOffset;
}

// NOTE: size checking of array should happen OUTSIDE this function!
inline void InsertDeclElement(UINT iInsertBefore, CONST D3DVERTEXELEMENT9 *pNew, D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE])
{
    LPD3DVERTEXELEMENT9 pCur;
    BYTE cbOffset;
    BYTE cbNewOffset;
    D3DVERTEXELEMENT9   TempElement1;
    D3DVERTEXELEMENT9   TempElement2;
    UINT iCur;

    pCur = pDecl;
    cbOffset = 0;
    iCur = 0;
    while ((pCur->Stream != 0xff) && (iCur < iInsertBefore))
    {
        cbOffset += x_rgcbTypeSizes[pCur->Type];

        pCur++;
        iCur++;
    }

    // NOTE: size checking of array should happen OUTSIDE this function!
    GXASSERT(pCur - pDecl + 1 < MAX_FVF_DECL_SIZE);

    // if we hit the end, just append
    if (pCur->Stream == 0xff)
    {
        // move the end of the stream down one
        memcpy(pCur+1, pCur, sizeof(D3DVERTEXELEMENT9));

        // copy the new element in and update the offset
        memcpy(pCur, pNew, sizeof(D3DVERTEXELEMENT9));
        pCur->Offset = cbOffset;

    }
    else  // insert in the middle
    {
        // save off the offset for the new decl
        cbNewOffset = cbOffset;

        // calculate the offset for the first element shifted up
        cbOffset += x_rgcbTypeSizes[pNew->Type];

        // save off the first item to move, data so that we can copy the new element in
        memcpy(&TempElement1, pCur, sizeof(D3DVERTEXELEMENT9));

        // copy the new element in
        memcpy(pCur, pNew, sizeof(D3DVERTEXELEMENT9));
        pCur->Offset = cbNewOffset;

        // advance pCur one because we effectively did an iteration of the loop adding the new element
        pCur++;

        while (pCur->Stream != 0xff)
        {
            // save off the current element
            memcpy(&TempElement2, pCur, sizeof(D3DVERTEXELEMENT9));

            // update the current element with the previous's value which was stored in TempElement1
            memcpy(pCur, &TempElement1, sizeof(D3DVERTEXELEMENT9));

            // move the current element's value into TempElement1 for the next iteration
            memcpy(&TempElement1, &TempElement2, sizeof(D3DVERTEXELEMENT9));

            pCur->Offset = cbOffset;
            cbOffset += x_rgcbTypeSizes[pCur->Type];

            pCur++;
        }

    // now we exited one element, need to move the end back one and copy in the last element
        // move the end element back one
        memcpy(pCur + 1, pCur, sizeof(D3DVERTEXELEMENT9));

        // copy the prev element's data out of the temp and into the last element
        memcpy(pCur, &TempElement1, sizeof(D3DVERTEXELEMENT9));
        pCur->Offset = cbOffset;
    }
}

// this function always adds the weights after the first element.  It DOESN'T matter if that is position or not,
//   but if it is, then it will tend to still be able to generate an FVF
inline HRESULT AddWeights(UINT cWeights, BOOL bIndices, D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE])
{
    UINT cElements;
    D3DVERTEXELEMENT9 Element;

    // remove any weights already present (makes adding them easier)
    RemoveDeclElement(D3DDECLUSAGE_BLENDWEIGHT, 0, pDecl);
    RemoveDeclElement(D3DDECLUSAGE_BLENDINDICES, 0, pDecl);

    // check the length of the new decl before trying to create it
    cElements = D3DXGetDeclLength(pDecl);
    if (cElements + (bIndices ? 1 : 0) + (cWeights ? 1 : 0) >= MAX_FVF_DECL_SIZE)
    {
        DPF(0, "AddWeightsVertexElements:  Cannot add weight vertex elements.  Too many vertex elements");
        return D3DERR_INVALIDCALL;
    }

    // first insert the indices (otherwise we would need to do it variable based on whether weights are present or not
    if (bIndices)
    {
        Element.Stream = 0;
        Element.Method = D3DDECLMETHOD_DEFAULT;
        Element.Usage = D3DDECLUSAGE_BLENDINDICES;
        Element.UsageIndex = 0;
        Element.Type = D3DDECLTYPE_UBYTE4;
        
        InsertDeclElement(1, &Element, pDecl);
    }

    // now add the weights
    if (cWeights)
    {
        Element.Stream = 0;
        Element.Method = D3DDECLMETHOD_DEFAULT;
        Element.Usage = D3DDECLUSAGE_BLENDWEIGHT;
        Element.UsageIndex = 0;
        Element.Type = D3DDECLTYPE_FLOAT1 + (cWeights - 1);
        
        InsertDeclElement(1, &Element, pDecl);
    }

    return S_OK;
}



#endif // __CRACKDECL_H__