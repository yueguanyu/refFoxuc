#include "stdafx.h"
#include "MyCompositor9.h"
#include "resource.h"
#include <stdlib.h>

extern CComPtr<CMyCompositor9> g_compositor;

LRESULT OnXPos( HWND wnd, int pos )
{
    if( g_compositor == NULL ) {
        SetWindowText( GetDlgItem( wnd, IDC_X ), _T("--") );
        return 0;
    }

    TCHAR caption[32];
    _itot( pos, caption, 10 );
    SetWindowText( GetDlgItem( wnd, IDC_X ), caption );
    g_compositor->ChangeX( pos );
    return 0;
}

LRESULT OnYPos( HWND wnd, int pos )
{
    if( g_compositor == NULL ) {
        SetWindowText( GetDlgItem( wnd, IDC_Y ), _T("--") );
        return 0;
    }

    TCHAR caption[32];
    _itot( pos, caption, 10 );
    SetWindowText( GetDlgItem( wnd, IDC_Y ), caption );
    g_compositor->ChangeY( pos );
    return 0;
}

LRESULT OnInitDialog( HWND wnd )
{
    SendMessage(GetDlgItem( wnd, IDC_SLIDER_X), TBM_SETRANGE, TRUE, MAKELONG(0, (WORD)(1000)));
    SendMessage(GetDlgItem( wnd, IDC_SLIDER_X), TBM_SETPOS, TRUE, (LPARAM)500);
    SendMessage(GetDlgItem( wnd, IDC_SLIDER_Y), TBM_SETRANGE, TRUE, MAKELONG(0, (WORD)(1000)));
    SendMessage(GetDlgItem( wnd, IDC_SLIDER_Y), TBM_SETPOS, TRUE, (LPARAM)500);
    OnXPos(wnd, 0);
    OnYPos(wnd, 0);
    return 0;
}

LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int pos; 
    switch( message ) {
    case WM_INITDIALOG:
        return OnInitDialog( hWnd );
    case WM_HSCROLL:
        pos = (int)SendMessage( GetDlgItem( hWnd, IDC_SLIDER_X), TBM_GETPOS, 0, 0);
        pos -= 500;
        return OnXPos(hWnd, pos );        
    case WM_VSCROLL:
        pos = (int)SendMessage( GetDlgItem( hWnd, IDC_SLIDER_Y), TBM_GETPOS, 0, 0);
        pos -= 500;
        return OnYPos(hWnd, pos );    
    case WM_CLOSE:
        DestroyWindow( hWnd );
        break;
    }
    return FALSE;
}