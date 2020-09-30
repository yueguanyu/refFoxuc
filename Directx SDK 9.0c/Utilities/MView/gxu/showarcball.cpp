/*//////////////////////////////////////////////////////////////////////////////
//
// File: stripoutline.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "pchgxu.h"

#include "ShowArcball.h"

#define PI 3.141592f

CShowArcball::CShowArcball()
  :m_pDevice(NULL),
    m_pVertexBuffer(NULL),
    m_cSegments(0)
{
}

CShowArcball::~CShowArcball()
{
    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDevice);
}

HRESULT
CShowArcball::Init
    (
    LPDIRECT3DDEVICE9 pDevice,
    DWORD cSegments
    )
{
    HRESULT hr;
    DWORD iSegment;
    D3DXVECTOR3 *rgvDest = NULL;

    hr = CreateEmptyOutline();
    if (FAILED(hr))
        goto e_Exit;

    m_pDevice = pDevice;
    m_pDevice->AddRef();

    m_cSegments = cSegments;

    hr = m_pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * (cSegments+1) * 3, 
                        D3DUSAGE_WRITEONLY, 
                        D3DFVF_XYZ, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    if (FAILED(hr))
        goto e_Exit;


    hr = m_pVertexBuffer->Lock(0,0, (PVOID*)&rgvDest, D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
        goto e_Exit;
    
    for (iSegment = 0; iSegment < cSegments+1; iSegment++)
    {
        rgvDest[iSegment].x = cosf(PI*2 * ((float)iSegment/(float)cSegments));
        rgvDest[iSegment].y = sinf(PI*2 * ((float)iSegment/(float)cSegments));
        rgvDest[iSegment].z = 0.0f;
    }

    rgvDest += cSegments+1;
    for (iSegment = 0; iSegment < cSegments+1; iSegment++)
    {
        rgvDest[iSegment].x = 0.0f;
        rgvDest[iSegment].y = cosf(PI*2 * ((float)iSegment/(float)cSegments));
        rgvDest[iSegment].z = sinf(PI*2 * ((float)iSegment/(float)cSegments));
    }

    rgvDest += cSegments+1;
    for (iSegment = 0; iSegment < cSegments+1; iSegment++)
    {
        rgvDest[iSegment].x = cosf(PI*2 * ((float)iSegment/(float)cSegments));
        rgvDest[iSegment].y = 0.0f;
        rgvDest[iSegment].z = sinf(PI*2 * ((float)iSegment/(float)cSegments));
    }

e_Exit:
    if (rgvDest != NULL)
    {
        m_pVertexBuffer->Unlock();
    }

    return hr;
}

HRESULT
CShowArcball::Draw
    (
    LPD3DXEFFECT pfxColor, 
    DWORD dwColor
    )
{
    HRESULT hr;
    UINT iPass;
    UINT cPasses;
    D3DXCOLOR vClr(dwColor);

    pfxColor->Begin(&cPasses, 0);

    for (iPass = 0; iPass < cPasses; iPass++)
    {
        m_pDevice->SetFVF(D3DFVF_XYZ);

        pfxColor->BeginPass(iPass);

        m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(D3DXVECTOR3));

        vClr = D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f);
        pfxColor->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       
        hr = m_pDevice->DrawPrimitive(D3DPT_LINESTRIP, 
                                     0, m_cSegments);

        vClr = D3DXCOLOR(0.2f, 1.0f, 0.2f, 1.0f);
        pfxColor->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       
        hr = m_pDevice->DrawPrimitive(D3DPT_LINESTRIP, 
                                     m_cSegments + 1, m_cSegments);

        vClr = D3DXCOLOR(0.2f, 0.2f, 1.0f, 1.0f);
        pfxColor->SetVector("vClr", (D3DXVECTOR4*)&vClr);                       
        hr = m_pDevice->DrawPrimitive(D3DPT_LINESTRIP, 
                                     m_cSegments * 2 + 2, m_cSegments);
        pfxColor->EndPass();
    }

    pfxColor->End();

    return S_OK;
}

HRESULT
CShowArcball::CreateEmptyOutline()
{
    m_cSegments = 0;

    GXRELEASE(m_pVertexBuffer);
    GXRELEASE(m_pDevice);

    return S_OK;
}
