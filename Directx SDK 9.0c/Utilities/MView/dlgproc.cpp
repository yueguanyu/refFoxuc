/*//////////////////////////////////////////////////////////////////////////////
//
// File: dlgproc.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcSimplify(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[256];
    char *pTmp;
    DWORD cVertices;

    switch (message)
    {
    case WM_INITDIALOG:
        GXASSERT(g_pData->m_pmcSelectedMesh->bSimplifyMode 
            || g_pData->m_pmcSelectedMesh->bPMMeshMode);

        if (g_pData->m_pmcSelectedMesh->bPMMeshMode)
            cVertices = g_pData->m_pmcSelectedMesh->pPMMesh->GetNumVertices();                   
        else
            cVertices = g_pData->m_pmcSelectedMesh->pSimpMesh->GetNumVertices();                   

        sprintf(szBuf, "%d", cVertices);

        SetDlgItemText(hDlg, IDC_VERTEXCOUNT, szBuf);

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_VERTEXCOUNT, szBuf, 255);
            cVertices = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            g_pData->SetNumVertices(cVertices, true /*absolute*/);                   

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
    MessageBox(NULL, "Please enter a valid number", "Simplify", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcSimplifyFaces(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    char szBuf[256];
    char *pTmp;
    DWORD cFaces;

    switch (message)
    {
    case WM_INITDIALOG:
        GXASSERT(g_pData->m_pmcSelectedMesh->bSimplifyMode 
            || g_pData->m_pmcSelectedMesh->bPMMeshMode);

        if (g_pData->m_pmcSelectedMesh->bPMMeshMode)
            cFaces = g_pData->m_pmcSelectedMesh->pPMMesh->GetNumFaces();                   
        else
            cFaces = g_pData->m_pmcSelectedMesh->pSimpMesh->GetNumFaces();                   

        sprintf(szBuf, "%d", cFaces);

        SetDlgItemText(hDlg, IDC_VERTEXCOUNT, szBuf);

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_VERTEXCOUNT, szBuf, 255);
            cFaces = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                goto e_Exit;

            g_pData->SetNumFaces(cFaces, true /*absolute*/);                   

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
    MessageBox(NULL, "Please enter a valid number", "Simplify", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}

void
SetCheck(HWND hDlg, UINT id, BOOL bChecked)
{
    HWND hwndTemp;
    int nCheck = bChecked ? BST_CHECKED : BST_UNCHECKED; 

    hwndTemp = GetDlgItem(hDlg, id);
    SendMessage(hwndTemp, BM_SETCHECK, nCheck, 0);
}

bool
GetCheck(HWND hDlg, UINT id)
{
    HWND hwndTemp;

    hwndTemp = GetDlgItem(hDlg, id);
    return BST_CHECKED == SendMessage(hwndTemp, BM_GETCHECK, 0, 0);
}

LRESULT CALLBACK 
DlgProcProperties(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0, nNotifyCode = 0;
    DWORD dwFVF;
    DWORD dwOptions;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    switch (message)
    {
    case WM_INITDIALOG:

        dwFVF = g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetFVF();
        dwOptions = g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetOptions();

        SetCheck(hDlg, IDC_XYZ, true);
        SetCheck(hDlg, IDC_NORMAL, dwFVF & D3DFVF_NORMAL);
        SetCheck(hDlg, IDC_DIFFUSE, dwFVF & D3DFVF_DIFFUSE);
        SetCheck(hDlg, IDC_SPECULAR, dwFVF & D3DFVF_SPECULAR);
        SetCheck(hDlg, IDC_TEX1, dwFVF & D3DFVF_TEX1);

        SetCheck(hDlg, IDC_32BITINDEX, dwOptions & D3DXMESH_32BIT);

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        nNotifyCode = HIWORD(wParam);
        switch (nId)
        {
        case IDOK:
            dwFVF = g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetFVF() | D3DFVF_XYZ;
            dwOptions = g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetOptions();

            if (GetCheck(hDlg, IDC_NORMAL))
                dwFVF |= D3DFVF_NORMAL;
            else
                dwFVF &= ~D3DFVF_NORMAL;

            if (GetCheck(hDlg, IDC_DIFFUSE))
                dwFVF |= D3DFVF_DIFFUSE;
            else
                dwFVF &= ~D3DFVF_DIFFUSE;

            if (GetCheck(hDlg, IDC_SPECULAR))
                dwFVF |= D3DFVF_SPECULAR;
            else
                dwFVF &= ~D3DFVF_SPECULAR;

            if (GetCheck(hDlg, IDC_TEX1))
                dwFVF |= D3DFVF_TEX1;
            else
                dwFVF &= ~D3DFVF_TEX1;

            if (GetCheck(hDlg, IDC_32BITINDEX))
                dwOptions |= D3DXMESH_32BIT;
            else
                dwOptions &= ~D3DXMESH_32BIT;

            D3DXDeclaratorFromFVF(dwFVF, pDecl);
            g_pData->ConvertSelectedMesh(dwOptions, pDecl);

            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;

//e_Exit:
    MessageBox(NULL, "Please enter a valid number", "Simplify", MB_SYSTEMMODAL | MB_OK );
    return TRUE;
}


// Mesage handler for about box.
LRESULT CALLBACK DlgProcAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            DWORD dwVersionMS = 0;
            DWORD dwVersionLS = 0;

            TCHAR tsz[256];
            TCHAR tszVersion[256];
            ::GetDlgItemText( hDlg, IDC_VERSION, tsz, 256 );
            // Retrieve the version number
            TCHAR tszFileName[MAX_PATH];
            ::GetModuleFileName( NULL, tszFileName, MAX_PATH );
            DWORD dwVerSize, dwHandle;
            dwVerSize = ::GetFileVersionInfoSize( tszFileName, &dwHandle );
            if( dwVerSize )
            {
                LPVOID pData = malloc( dwVerSize );
                if( pData )
                {
                    ::GetFileVersionInfo( tszFileName, 0, dwVerSize, pData );
                    VS_FIXEDFILEINFO *pffi;
                    UINT uLen;
                    ::VerQueryValue( pData, _T("\\"), (LPVOID*)&pffi, &uLen );
                    dwVersionMS = pffi->dwFileVersionMS;
                    dwVersionLS = pffi->dwFileVersionLS;
                    free( pData );
                }
            }
            _sntprintf( tszVersion, 256, tsz, HIWORD( dwVersionMS ), LOWORD( dwVersionMS ),
                                              HIWORD( dwVersionLS ), LOWORD( dwVersionLS ) );
            tszVersion[255] = _T('\0');
            ::SetDlgItemText( hDlg, IDC_VERSION, tszVersion );
            return TRUE;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcOutput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0;

    switch (message)
    {
    case WM_INITDIALOG:
        GXASSERT(g_pData->pvDialogData != NULL);

        SetDlgItemText(hDlg, IDC_CONSOLE, (char*)g_pData->pvDialogData);

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        switch (nId)
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

const char *x_rgszUsageStrings[] = 
{  
    "Position",    // D3DDECLUSAGE_POSITION = 0,
    "BlendWeight", // D3DDECLUSAGE_BLENDWEIGHT,   // 1
    "BlendIndices",// D3DDECLUSAGE_BLENDINDICES,  // 2
    "Normal",      // D3DDECLUSAGE_NORMAL,        // 3
    "PSize",       // D3DDECLUSAGE_PSIZE,         // 4
    "Texcoord",    // D3DDECLUSAGE_TEXCOORD,      // 5
    "Tangent",     // D3DDECLUSAGE_TANGENT,       // 6
    "Binormal",    // D3DDECLUSAGE_BINORMAL,      // 7
    "TessFactor",  // D3DDECLUSAGE_TESSFACTOR,    // 8
    "PositionT",   // D3DDECLUSAGE_POSITIONT,     // 9
    "Color",       // D3DDECLUSAGE_COLOR,         // 10
    "Fog",         // D3DDECLUSAGE_FOG,           // 11
    "Depth",       // D3DDECLUSAGE_DEPTH,         // 12
    "Sample"       // D3DDECLUSAGE_SAMPLE,        // 13
};


GatherVertexInfo(
    DWORD dwVertex, 
    PBYTE pbPoints, 
    LPD3DVERTEXELEMENT9 pDecl,
    char **pszOutput)
{
    char szBuf[120];
    char * szOutput = NULL;
    HRESULT hr = S_OK;
    CD3DXCrackDecl1 cd(pDecl);
    PBYTE pbPoint;
    D3DXVECTOR3 *pvTemp;
    DWORD iWeight;
    DWORD cWeights;
    float *pfWeights;
    D3DXCOLOR colorTemp;
    float *pfTexCoords;
    DWORD iTexCoord;
    DWORD cComponents;
    DWORD iComponent;
    FLOAT rgfValues[4];
    const char *szUsage;
    LPD3DVERTEXELEMENT9 pDeclCur;
    DWORD iValue;
    DWORD cValues;

    cd.SetStreamSource(0, pbPoints, cd.m_cBytesPerVertex);

    szOutput = new char[120 * 30/*vertex info*/];
    if (szOutput == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    sprintf(szOutput, "Information for Vertex %d:\r\n", dwVertex);

    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff)
    {

        szUsage = x_rgszUsageStrings[pDeclCur->Usage];

        sprintf(szBuf, "\t%s%d (", szUsage, pDeclCur->UsageIndex);
        strcat(szOutput, szBuf);

        cd.Decode(pDeclCur, dwVertex, rgfValues, 4);

        cValues = x_rgcbFields[pDeclCur->Type];
        for (iValue = 0; iValue < cValues; iValue++)
        {
            if (iValue == (cValues-1))
                sprintf(szBuf, "%f", rgfValues[iValue]);
            else
                sprintf(szBuf, "%f,", rgfValues[iValue]);

            strcat(szOutput, szBuf);
        }

        strcat(szOutput, ")\r\n");

        pDeclCur++;
    }

    *pszOutput = szOutput;
    szOutput = NULL;

e_Exit:
    delete []szOutput;
    return hr;
}

HRESULT 
GatherVertexInfoHelper(LPD3DXBASEMESH pMesh, DWORD dwVertex, char **pszOutput)
{
    HRESULT hr = S_OK;
    char *szOutput = NULL;
    PBYTE pbPoints;
    PBYTE pIndices;
    DWORD dwFVF;
    BOOL b16BitMesh;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    pMesh->LockVertexBuffer(D3DLOCK_READONLY, (PVOID*)&pbPoints);

    pMesh->GetDeclaration(pDecl);
    hr = GatherVertexInfo(dwVertex, pbPoints, pDecl, &szOutput);
    if (FAILED(hr))
        goto e_Exit;

    *pszOutput = szOutput;
    szOutput = NULL;

e_Exit:
    if (pbPoints != NULL)
    {
        pMesh->UnlockVertexBuffer();
    }

    delete []szOutput;
    return hr;
}

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcVertexInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0;
    char szBuf[120];
    char *pTmp;
    char *szOutput = NULL;
    DWORD dwVertex;

    switch (message)
    {
    case WM_INITDIALOG:
        sprintf(szBuf, "%d", g_pData->m_dwVertexSelected);
        SetDlgItemText(hDlg, IDC_SELECTEDVERTEX, szBuf);

        GatherVertexInfoHelper(g_pData->m_pmcSelectedMesh->ptmDrawMesh, g_pData->m_dwVertexSelected, &szOutput);
        SetDlgItemText(hDlg, IDC_CONSOLE, szOutput);

        delete []szOutput;

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        switch (nId)
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        case IDC_GETVERTEXINFO:

            GetDlgItemText(hDlg, IDC_SELECTEDVERTEX, szBuf, 120);
            dwVertex = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                return TRUE;

            if (dwVertex < g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumVertices())
            {
                GatherVertexInfoHelper(g_pData->m_pmcSelectedMesh->ptmDrawMesh, dwVertex, &szOutput);
                g_pData->m_dwVertexSelected = dwVertex;
            }
            else
            {
                szOutput = new char[120];
                if (szOutput == NULL)
                    return FALSE;

                sprintf(szOutput, "Index %d is not a valid index.  There are %d vertices in the selected mesh", dwVertex, g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumVertices());
            }

            SetDlgItemText(hDlg, IDC_CONSOLE, szOutput);

            delete []szOutput;

            return TRUE;
        }
        break;
    }
    return FALSE;
}



template<class UINT_IDX, unsigned int UNUSED>
HRESULT
GatherFaceInfo(
    LPD3DXBASEMESH pMesh, 
    DWORD dwFace, 
    UINT_IDX *pwFaces, 
    DWORD *rgdwAdjacency,
    PBYTE pbPoints, 
    LPD3DVERTEXELEMENT9 pDecl, 
    char **pszOutput, 
    UINT_IDX Dummy)
{
    char szBuf[120];
    char * szOutput = NULL;
    HRESULT hr = S_OK;
    DWORD iIndex;
    char *szVertexInfo;

    szOutput = new char[120 * (10 /*face info*/+ 60/*vertex info*/)];
    if (szOutput == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    sprintf(szOutput, "Information for Face %d:\r\n\r\n", dwFace);

    sprintf(szBuf, "Vertex Indices:\r\n");
    strcat(szOutput, szBuf);
    for (iIndex = 0; iIndex < 3; iIndex++)
    {
        sprintf(szBuf, "Index %d: %d\r\n", iIndex, pwFaces[dwFace*3 + iIndex]);
        strcat(szOutput, szBuf);
    }

    sprintf(szBuf, "\r\nAdjacent Face Indices:\r\n");
    strcat(szOutput, szBuf);
    for (iIndex = 0; iIndex < 3; iIndex++)
    {
        if (rgdwAdjacency[dwFace*3 + iIndex] != UNUSED32)
            sprintf(szBuf, "AdjIndex %d: %d\r\n", iIndex, rgdwAdjacency[dwFace*3 + iIndex]);
        else
            sprintf(szBuf, "AdjIndex %d: UNUSED\r\n", iIndex);

        strcat(szOutput, szBuf);
    }

    strcat(szOutput, "\r\n");
    for (iIndex = 0; iIndex < 3; iIndex++)
    {
        // gather the vertex info to include with the face info
        hr = GatherVertexInfo(pwFaces[dwFace*3 + iIndex], pbPoints, pDecl, &szVertexInfo);
        if (FAILED(hr))
            goto e_Exit;

        strcat(szOutput, szVertexInfo);
        strcat(szOutput, "\r\n");
        delete []szVertexInfo;
    }


    *pszOutput = szOutput;
    szOutput = NULL;

e_Exit:
    delete []szOutput;
    return hr;
}

HRESULT
GatherFaceInfoHelper(
    LPD3DXBASEMESH pMesh, 
    DWORD dwFace, 
    DWORD *rgdwAdjacency,
    char **pszOutput)
{
    HRESULT hr = S_OK;
    char *szOutput = NULL;
    PBYTE pbPoints;
    PBYTE pIndices;
    UINT16 Dummy16 = 0;
    UINT32 Dummy32 = 0;
    DWORD dwFVF;
    BOOL b16BitMesh;
    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];

    pMesh->LockIndexBuffer(D3DLOCK_READONLY, (PVOID*)&pIndices);
    pMesh->LockVertexBuffer(D3DLOCK_READONLY, (PVOID*)&pbPoints);

    pMesh->GetDeclaration(pDecl);
    b16BitMesh = !(pMesh->GetOptions() & D3DXMESH_32BIT);

    // first gather the face info
    if (b16BitMesh)
        hr = GatherFaceInfo<UINT16,UNUSED16>(pMesh, dwFace, (UINT16*)pIndices, rgdwAdjacency, pbPoints, pDecl, &szOutput, Dummy16);
    else
        hr = GatherFaceInfo<UINT32,UNUSED32>(pMesh, dwFace, (UINT32*)pIndices, rgdwAdjacency, pbPoints, pDecl, &szOutput, Dummy32);

    if (FAILED(hr))
        goto e_Exit;

    *pszOutput = szOutput;
    szOutput = NULL;

e_Exit:
    if (pIndices != NULL)
    {
        pMesh->UnlockIndexBuffer();
    }

    if (pbPoints != NULL)
    {
        pMesh->UnlockVertexBuffer();
    }

    delete []szOutput;
    return hr;
}

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcFaceInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0;
    char szBuf[120];
    char *pTmp;
    char *szOutput = NULL;
    DWORD dwFace;

    switch (message)
    {
    case WM_INITDIALOG:
        sprintf(szBuf, "%d", g_pData->m_dwFaceSelected);
        SetDlgItemText(hDlg, IDC_SELECTEDFACE, szBuf);

        GatherFaceInfoHelper(g_pData->m_pmcSelectedMesh->ptmDrawMesh, g_pData->m_dwFaceSelected, 
                                g_pData->m_pmcSelectedMesh->rgdwAdjacency, &szOutput);
        SetDlgItemText(hDlg, IDC_CONSOLE, szOutput);
        
        delete []szOutput;

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        switch (nId)
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        case IDC_GETFACEINFO:

            {

            GetDlgItemText(hDlg, IDC_SELECTEDFACE, szBuf, 120);
            dwFace = (long) strtoul(szBuf, &pTmp, 10);
            if (pTmp && (*pTmp != '\0'))
                return TRUE;

            if (dwFace < g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces())
            {
                GatherFaceInfoHelper(g_pData->m_pmcSelectedMesh->ptmDrawMesh, dwFace, 
                                        g_pData->m_pmcSelectedMesh->rgdwAdjacency, &szOutput);

                g_pData->m_dwFaceSelected = dwFace;
            }
            else
            {
                szOutput = new char[120];
                if (szOutput == NULL)
                    return FALSE;

                sprintf(szOutput, "Index %d is not a valid index.  There are %d faces in the selected mesh", dwFace, g_pData->m_pmcSelectedMesh->ptmDrawMesh->GetNumFaces());
            }

            // now display it
            SetDlgItemText(hDlg, IDC_CONSOLE, szOutput);

            delete []szOutput;
            }

            return TRUE;
        }
        break;
    }
    return FALSE;
}


HRESULT
GatherMeshInfo
    (
    SMeshContainer *pmc, 
    char **pszOutput
    )
{
    HRESULT hr = S_OK;
    char *szOutput = NULL;
    char szBuf[120];
    DWORD cVertices;
    DWORD cFaces;

    szOutput = new char[120 * 20];
    if (szOutput == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    cVertices = pmc->ptmDrawMesh->GetNumVertices();
    cFaces = pmc->ptmDrawMesh->GetNumFaces();

    if (pmc->pPMMesh != NULL)
    {
        sprintf(szOutput, "Information about selected progressive mesh:\r\n");

        sprintf(szBuf, "\tMin number of faces   : %d\r\n", pmc->pPMMesh->GetMinFaces());
        strcat(szOutput, szBuf);
        sprintf(szBuf, "\tCur number of faces   : %d\r\n", cFaces);
        strcat(szOutput, szBuf);
        sprintf(szBuf, "\tMax number of faces   : %d\r\n\r\n", pmc->pPMMesh->GetMaxFaces());
        strcat(szOutput, szBuf);

        sprintf(szBuf, "\tMin number of vertices: %d\r\n", pmc->pPMMesh->GetMinVertices());
        strcat(szOutput, szBuf);
        sprintf(szBuf, "\tCur number of vertices: %d\r\n", cVertices);
        strcat(szOutput, szBuf);
        sprintf(szBuf, "\tMax number of vertices: %d\r\n", pmc->pPMMesh->GetMaxVertices());
        strcat(szOutput, szBuf);
    }
    else
    {
        sprintf(szOutput, "Information about selected mesh:\r\n");

        sprintf(szBuf, "\tNumber of faces   : %d\r\n", cFaces);
        strcat(szOutput, szBuf);
        sprintf(szBuf, "\tNumber of vertices: %d\r\n", cVertices);
        strcat(szOutput, szBuf);
    }

    *pszOutput = szOutput;
    szOutput = NULL;

e_Exit:
    delete []szOutput;
    return hr;
}

// Mesage handler for about box.
LRESULT CALLBACK 
DlgProcMeshInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD nId = 0;
    char szBuf[120];
    char *pTmp;
    char *szOutput = NULL;
    DWORD dwVertex;

    switch (message)
    {
    case WM_INITDIALOG:
        GatherMeshInfo(g_pData->m_pmcSelectedMesh, &szOutput);
        SetDlgItemText(hDlg, IDC_CONSOLE, szOutput);

        delete []szOutput;

        return TRUE;
        
    case WM_COMMAND:
        nId = LOWORD(wParam);
        switch (nId)
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 1);
            return TRUE;
        }
    }
    return FALSE;
}
