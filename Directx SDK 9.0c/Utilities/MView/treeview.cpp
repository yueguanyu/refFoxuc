#include "mviewpch.h"

#include <commdlg.h>
#include <tchar.h>
#include <commctrl.h>

#include "common.h"
#include "treeview.h"

#ifdef _DEBUG
static bool g_bInstanceCreated = false;
#endif


bool
InitFrameViewClass(HINSTANCE hInstance)
{
    WNDCLASS wndclass;

    GXASSERT(!g_bInstanceCreated);

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = (WNDPROC) CFrameView::WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof(void *);
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = (HICON) NULL;
    wndclass.hCursor       = (HCURSOR) NULL;
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "FrameViewWindow";
    if (! RegisterClass(&wndclass) )
    {
        return false;
    }

#ifdef _DEBUG
    g_bInstanceCreated = true;
#endif

    return true;
}


HRESULT
CreateFrameView(RECT &rTreeView, HWND hwndParent, HINSTANCE hInst, 
                PFNSELECTED pfnSelected, PVOID pvCallbackData, WNDPROC pfnKeyPress, CFrameView **ppfvNew)
{
    HRESULT hr = S_OK;
    CFrameView *pfvNew = NULL;
    HWND hwndFrame;

    GXASSERT(g_bInstanceCreated);

    pfvNew = new CFrameView();
    if (pfvNew == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	hwndFrame = CreateWindow("FrameViewWindow", NULL,
								WS_POPUP | WS_CAPTION | WS_CLIPCHILDREN | WS_SIZEBOX | WS_VISIBLE,
								rTreeView.left, rTreeView.top,rTreeView.right - rTreeView.left, rTreeView.bottom - rTreeView.top,
								hwndParent, NULL, hInst, NULL);
	if (hwndFrame == NULL) 
    {
		MessageBox (NULL, "Failed to create the TreeView Frame window!", NULL, MB_OK );
		return FALSE;
	}
    SetWindowLongPtr(hwndFrame, 0, (LONG_PTR)pfvNew);

    hr = pfvNew->Init(hwndFrame, hInst, pfnSelected, pfnKeyPress, pvCallbackData);
    if (FAILED(hr))
        goto e_Exit;

    *ppfvNew = pfvNew;
    pfvNew = NULL;

    ShowWindow(hwndFrame, SW_HIDE);
    //ShowWindow(hwndFrame, SW_SHOW);
e_Exit:
    delete pfvNew;

    return hr;
}

BOOL
CFrameView::ToggleVisible()
{
    m_bVisible = !m_bVisible;    
    ShowWindow(m_hwndFrame, m_bVisible ? SW_SHOW : SW_HIDE );
    ShowWindow(m_hwndTree, m_bVisible ? SW_SHOW : SW_HIDE );

    return m_bVisible;
}

void
CFrameView::GetWindowRect(RECT *pRect)
{
    ::GetWindowRect(m_hwndFrame, pRect);
}

HRESULT
CFrameView::Init(HWND hwndFrame, HINSTANCE hInst, PFNSELECTED pfnSelected, WNDPROC pfnKeyPress, PVOID pvCallbackData)
{
    HRESULT hr = S_OK;
    RECT rTreeView;

    GetClientRect(hwndFrame, &rTreeView);

	InitCommonControls();

    m_hwndFrame = hwndFrame;
    m_bVisible = FALSE;

    m_bCurrentlyDeleting = false;
    m_iWidth = rTreeView.right - rTreeView.left;
    m_iHeight = rTreeView.bottom - rTreeView.top;
    m_pfnSelected = pfnSelected;    
    m_pvCallbackData = pvCallbackData;
    m_pfnKeyPress = pfnKeyPress;

	m_hwndTree = CreateWindowEx(0, WC_TREEVIEW, "Hierarchy",
					  /*WS_VISIBLE | */WS_CHILD | WS_DLGFRAME | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP/*| TVS_EDITLABELS */| TVS_SHOWSELALWAYS,
					  rTreeView.left, rTreeView.top, rTreeView.right, rTreeView.bottom,
					  hwndFrame, (HMENU) NULL, hInst, NULL);
    if (m_hwndTree == NULL)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

	//InitTreeViewImageLists(hwndTV);

e_Exit:
    return hr;
}

CFrameView::~CFrameView()
{
    SetWindowLong(m_hwndFrame, 0, 0);

    DestroyWindow(m_hwndFrame);
}

HTREEITEM
CFrameView::InsertIntoTree(HTREEITEM htreeParent, char *szString, PVOID pvItemId)
{
	TV_INSERTSTRUCT ins;

	ins.hParent = htreeParent;
	ins.hInsertAfter = TVI_LAST;
	ins.item.mask = TVIF_TEXT | TVIF_PARAM;
    ins.item.lParam = (LPARAM)pvItemId;
	ins.item.cchTextMax = strlen(szString);
    ins.item.pszText = szString;

	return TreeView_InsertItem(m_hwndTree, &ins);
}

void
CFrameView::SelectItem(HTREEITEM htreeSelect)
{
	TreeView_SelectItem(m_hwndTree, htreeSelect);
}


HRESULT
CFrameView::RemoveAllItems()
{
    m_bCurrentlyDeleting = true;
    TreeView_DeleteAllItems(m_hwndTree);
    m_bCurrentlyDeleting = false;

    return S_OK;
}

















/*
 * Handle messages for the viewer treeview frame window
 */
LRESULT WINAPI
CFrameView::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	CFrameView *pfv;

	pfv = (CFrameView *) GetWindowLongPtr(hwnd, 0);

    if (pfv != NULL)
    {

	    switch (msg) {
	    case WM_NOTIFY:
		    {
			    LPNMHDR pnmh = (LPNMHDR) lparam;
			    if (pnmh->hwndFrom == pfv->m_hwndTree) {
				    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW) lparam;
				    switch (pnmh->code) 
                    {
				    case TVN_SELCHANGED:
					    {
                            if (!pfv->m_bCurrentlyDeleting)
                            {
                                pfv->m_pfnSelected(pfv->m_pvCallbackData, (PVOID)lpnmtv->itemNew.lParam);
                            }
					    }
					    break;
                    case TVN_KEYDOWN:
                        NMTVKEYDOWN *tvKeyDown = (LPNMTVKEYDOWN)lparam;
                        pfv->m_pfnKeyPress(hwnd, WM_KEYDOWN, tvKeyDown->wVKey, 0);
                        return 1;

				    //case TVN_ITEMEXPANDED:
					    //TreeViewExpandItem(&(lpnmtv->itemNew), lpnmtv->action & TVE_EXPAND);
					    //break;
				    }
			    }
		    }
		    break;
        case WM_KEYDOWN:
            return pfv->m_pfnKeyPress(hwnd, msg, wparam, lparam);

        case WM_KEYUP:
            return pfv->m_pfnKeyPress(hwnd, msg, wparam, lparam);

	    case WM_WINDOWPOSCHANGING:
    #if 0
		    {
			    WINDOWPOS *pos = (WINDOWPOS *)lparam;
			    RECT rc;
			    GetClientRect(m_hwndParent, &rc);
			    if (pos->x > 0)
				    pos->cx = pfv->m_iWidth;
			    pos->x = 0;
			    pos->y = toolBarHt;
			    if (pos->cy < (rc.bottom - rc.top - toolBarHt))
				    pos->cy = rc.bottom - rc.top - toolBarHt;
		    }
            return 0;
    #endif
		    break;
	    case WM_SIZE:
		    {
			    if (pfv == NULL)
				    return 0;
                UINT iWidth = LOWORD(lparam);
                UINT iHeight = HIWORD(lparam);
			    BOOL bSizeChanged = FALSE;

			    if (iWidth != pfv->m_iWidth) 
                {
				    pfv->m_iWidth = iWidth;
				    bSizeChanged = TRUE;
			    }

			    if (iHeight != pfv->m_iHeight) 
                {
				    pfv->m_iHeight = iHeight;
				    bSizeChanged = TRUE;
			    }

			    if (bSizeChanged) 
                {
				    SetWindowPos(pfv->m_hwndTree, NULL, 0, 0, iWidth, iHeight, SWP_NOZORDER);
			    }
		    }
		    break;

	    case WM_DESTROY:
        default:

		    break;
        
        }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}
