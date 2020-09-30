/*//////////////////////////////////////////////////////////////////////////////
//
// File: mshape.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"


HRESULT
TrivialData::SelectTextFont()
{
    LOGFONT lf;
    GetObject( m_hFontText, sizeof(LOGFONT), &lf );
    CHOOSEFONT cf;

    memset(&cf, 0, sizeof(CHOOSEFONT));
    cf.lStructSize = sizeof(CHOOSEFONT);
    cf.hwndOwner = m_hwnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = m_rgbFontColor;

    cf.Flags = CF_SCREENFONTS | CF_TTONLY | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;

    if (ChooseFont(&cf))
    {
        DeleteObject(m_hFontText);

        m_hFontText = CreateFontIndirect(&lf);
        GXASSERT(m_hFontText != NULL);

        m_rgbFontColor = cf.rgbColors;
    }
    DWORD dw = CommDlgExtendedError();

    return S_OK;
}

LRESULT CALLBACK
DlgProcCreateText(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[256];
    char *pTmp;
    HDC hDC;
    LPD3DXMESH ptmMesh = NULL;
    HRESULT hr;
    float fExtrusion;
    float fDeviation;
    LPD3DXBUFFER pbufAdjacency = NULL;

    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_TEXT, g_pData->m_szText);

        sprintf(szBuf, "%f", g_pData->m_fExtrusion);
        SetDlgItemText(hDlg, IDC_EXTRUSION, szBuf);

        sprintf(szBuf, "%f", g_pData->m_fDeviation);
        SetDlgItemText(hDlg, IDC_DEVIATION, szBuf);

        return TRUE;

    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_EXTRUSION, szBuf, 256);
            fExtrusion = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_DEVIATION, szBuf, 256);
            fDeviation = (float)strtod(szBuf, &pTmp);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            GetDlgItemText(hDlg, IDC_TEXT, szBuf, 256);

            hDC = GetDC(hDlg);

            SelectObject(hDC, g_pData->m_hFontText);

            hr = D3DXCreateTextA(g_pData->m_pDevice, hDC, szBuf, fDeviation, fExtrusion, &ptmMesh, &pbufAdjacency, NULL);
            if (FAILED(hr))
                goto e_Fail;

            ReleaseDC(hDlg, hDC);

            hr = g_pData->AddMeshToDrawList(szBuf, ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
            GXRELEASE(ptmMesh);
            GXRELEASE(pbufAdjacency);

            if (FAILED(hr))
                goto e_Fail;


            D3DMATERIAL9 mat;
            memset(&mat, 0, sizeof(D3DMATERIAL9));
            mat.Diffuse.r = (float)(g_pData->m_rgbFontColor & 0xff) / 0xff;
            mat.Diffuse.g = (float)((g_pData->m_rgbFontColor >> 8) & 0xff) / 0xff;
            mat.Diffuse.b = (float)((g_pData->m_rgbFontColor >> 16) & 0xff) / 0xff;
            mat.Specular = mat.Diffuse;

            g_pData->m_pmcSelectedMesh->m_rgpfxAttributes[0]->SetVector("Diffuse", (LPD3DXVECTOR4)&mat.Diffuse);
            g_pData->m_pmcSelectedMesh->m_rgpfxAttributes[0]->SetVector("Specular", (LPD3DXVECTOR4)&mat.Specular);

            strcpy(g_pData->m_szText, szBuf);
            g_pData->m_fExtrusion = fExtrusion;
            g_pData->m_fDeviation = fDeviation;

            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

e_Exit:
    MessageBox(NULL, "Please enter a valid number", "CreateText", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
e_Fail:
    MessageBox(NULL, "Unable to create desired text", "CreateText", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

HRESULT
TrivialData::CreateText()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;

    if (m_hFontText == NULL)
    {
        MessageBox(NULL, "No font selected", "Error", MB_OK);
        return E_INVALIDARG;
    }

    DialogBox(m_hInstance, (LPCTSTR) IDD_CREATETEXT, m_hwnd, (DLGPROC) DlgProcCreateText);

    return S_OK;
}

HRESULT
TrivialData::CreatePolygon()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreatePolygon(m_pDevice, 1.0f, 8, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Polygon", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateBox()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateBox(m_pDevice, 1.0f, 1.0f, 1.0f, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Box", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateCylinder()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateCylinder(m_pDevice, 1.0f, 1.0f, 3.0f, 20, 2, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Cylinder", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateCone()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateCylinder(m_pDevice, 0.0f, 1.0f, 3.0f, 6, 3, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Cone", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateTorus()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateTorus(m_pDevice, 2.0f, 3.0f, 10, 10, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Torus", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateTeapot()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateTeapot(m_pDevice, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Teapot", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}

HRESULT
TrivialData::CreateSphere()
{
    LPD3DXMESH ptmMesh = NULL;
    LPD3DXBUFFER pbufAdjacency = NULL;
    HRESULT hr = S_OK;

    hr = D3DXCreateSphere(m_pDevice, 1.0f, 10, 10, &ptmMesh, &pbufAdjacency);
    if (FAILED(hr))
        goto e_Exit;

    hr = AddMeshToDrawList("Sphere", ptmMesh, NULL, pbufAdjacency, NULL, NULL, 0);
    if (FAILED(hr))
        goto e_Exit;

e_Exit:
    GXRELEASE(ptmMesh);
    GXRELEASE(pbufAdjacency);
    return hr;
}
