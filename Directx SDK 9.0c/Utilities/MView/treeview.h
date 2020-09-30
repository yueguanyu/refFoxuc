#pragma once

#ifndef TREEVIEW_H
#define TREEVIEW_H

/*//////////////////////////////////////////////////////////////////////////////
//
// File: Treeview.h
//
// Copyright (C) 1999 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

typedef HRESULT (* PFNSELECTED)(PVOID pvCallbackData, PVOID pvItemId);


class CFrameView
{
    friend HRESULT CreateFrameView(RECT &rTreeView, HWND hwndParent, HINSTANCE hInst, 
                        PFNSELECTED pfnSelected, PVOID pvCallbackData,
                        WNDPROC pfnKeyPress,
                        CFrameView **ppfvNew);
    friend bool InitFrameViewClass(HINSTANCE hInstance);


public:
    HTREEITEM InsertIntoTree(HTREEITEM htreeParent, char *szString, PVOID pvItemId);
    void SelectItem(HTREEITEM htreeSelect);
    HRESULT RemoveAllItems();

    BOOL ToggleVisible();
    void GetWindowRect(RECT *pRect);

    ~CFrameView();
private:    
    HRESULT Init(HWND hwndFrame, HINSTANCE hInst, PFNSELECTED pfnSelected, WNDPROC pfnKeyPress, PVOID pvCallbackData);

    static LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    HWND m_hwndFrame;
    HWND m_hwndTree;
    UINT m_iWidth;
    UINT m_iHeight;

    BOOL m_bCurrentlyDeleting;
    BOOL m_bVisible;

    PFNSELECTED m_pfnSelected;
    WNDPROC m_pfnKeyPress;
    PVOID m_pvCallbackData;
};

bool InitFrameViewClass(HINSTANCE hInstance);
HRESULT CreateFrameView(RECT &rTreeView, HWND hwndParent, HINSTANCE hInst, 
                        PFNSELECTED pfnSelected, PVOID pvCallbackData,
                        WNDPROC pfnKeyPress,
                        CFrameView **ppfvNew);

#endif
