/*//////////////////////////////////////////////////////////////////////////////
//
// File: texture.cpp
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "mviewpch.h"

HRESULT
TrivialData::NewTexture(char *szFilename, LPDIRECT3DTEXTURE9 *ppTexture)
{
 	HRESULT hr = S_OK;
	STextureCache *ptcCur;
	DWORD cchFilename;
	DWORD cchPath;
	char szPath[1024];
	char *szName;

	// first search the loaded textures
	ptcCur = m_ptcTextureCacheHead;
	while (ptcCur != NULL)
	{
		// if found just return that texture
		if (strcmp(ptcCur->szFilename, szFilename) == 0)
		{
			*ppTexture = (LPDIRECT3DTEXTURE9)ptcCur->pTexture;

            if (ptcCur->pTexture != NULL)
			    ptcCur->pTexture->AddRef();

			goto e_Exit;
		}

		ptcCur = ptcCur->pNext;
	}

	// if not found, load the texture and add an entry to the cache
	ptcCur = new STextureCache;
	if (ptcCur == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	// copy the filename
	cchFilename = strlen(szFilename) + 1;
	ptcCur->szFilename = new char[cchFilename];
	if (ptcCur->szFilename == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memcpy(ptcCur->szFilename, szFilename, cchFilename);

	// load the file, if it fails, just set to NULL
    cchPath = GetFullPathName(szFilename, sizeof(szPath), szPath, &szName);
    if ((cchPath == 0) || (sizeof(szPath) <= cchPath))
        return E_FAIL;

	hr = D3DXCreateTextureFromFile(m_pDevice, szName, (LPDIRECT3DTEXTURE9*)&ptcCur->pTexture);
	if (FAILED(hr))
		ptcCur->pTexture = NULL;

    // if that failed try the current directory
    if (FAILED(hr))
    {
		hr = D3DXCreateTextureFromFile(m_pDevice, szFilename, (LPDIRECT3DTEXTURE9*)&ptcCur->pTexture);
		if (FAILED(hr))
			ptcCur->pTexture = NULL;
    }

	// be sure to keep an addref for the cache
	*ppTexture = (LPDIRECT3DTEXTURE9)ptcCur->pTexture;

	if (ptcCur->pTexture != NULL)
		ptcCur->pTexture->AddRef();

	// link into cache
	ptcCur->pNext = m_ptcTextureCacheHead;
	m_ptcTextureCacheHead = ptcCur;



e_Exit:
	return hr;
}


HRESULT
TrivialData::NewCubeTexture(char *szFilename, LPDIRECT3DCUBETEXTURE9 *ppTexture)
{
 	HRESULT hr = S_OK;
	STextureCache *ptcCur;
	DWORD cchFilename;
	DWORD cchPath;
	char szPath[1024];
	char *szName;

	// first search the loaded textures
	ptcCur = m_ptcTextureCacheHead;
	while (ptcCur != NULL)
	{
		// if found just return that texture
		if (strcmp(ptcCur->szFilename, szFilename) == 0)
		{
			*ppTexture = (LPDIRECT3DCUBETEXTURE9)ptcCur->pTexture;
			ptcCur->pTexture->AddRef();
			goto e_Exit;
		}

		ptcCur = ptcCur->pNext;
	}

	// if not found, load the texture and add an entry to the cache
	ptcCur = new STextureCache;
	if (ptcCur == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	// copy the filename
	cchFilename = strlen(szFilename) + 1;
	ptcCur->szFilename = new char[cchFilename];
	if (ptcCur->szFilename == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memcpy(ptcCur->szFilename, szFilename, cchFilename);

	// load the file, if it fails, just set to NULL
    cchPath = GetFullPathName(szFilename, sizeof(szPath), szPath, &szName);
    if ((cchPath == 0) || (sizeof(szPath) <= cchPath))
        return E_FAIL;

	hr = D3DXCreateCubeTextureFromFile(m_pDevice, szName, (LPDIRECT3DCUBETEXTURE9*)&ptcCur->pTexture);
	if (FAILED(hr))
		ptcCur->pTexture = NULL;

    // if that failed try the current directory
    if (FAILED(hr))
    {
		hr = D3DXCreateCubeTextureFromFile(m_pDevice, szFilename, (LPDIRECT3DCUBETEXTURE9*)&ptcCur->pTexture);
		if (FAILED(hr))
			ptcCur->pTexture = NULL;
    }

	// be sure to keep an addref for the cache
	*ppTexture = (LPDIRECT3DCUBETEXTURE9)ptcCur->pTexture;

	if (ptcCur->pTexture != NULL)
		ptcCur->pTexture->AddRef();

	// link into cache
	ptcCur->pNext = m_ptcTextureCacheHead;
	m_ptcTextureCacheHead = ptcCur;



e_Exit:
	return hr;
}


