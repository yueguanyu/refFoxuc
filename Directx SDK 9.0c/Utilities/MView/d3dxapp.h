//
// D3DX Application Framework
// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//

#ifndef __D3DXAPP_H__
#define __D3DXAPP_H__

#include <d3dx9.h>
struct D3DX_APPLICATION_DATA;


// ---------------------------------------------------------------------------
//
// CD3DXApplication
//
// This class can be used to quickly create single-context fullscreen or
// windowed D3DX applications.  The framework manages the window and context
// objects for the application.
//
// ---------------------------------------------------------------------------

class CD3DXApplication
{
public:
    CD3DXApplication();
    virtual ~CD3DXApplication();


    // Initialize
    //     NOTE: If szWindowName is NULL, "D3DX Application" is used.
    //           If szClassName is NULL, a default window class is created.

    HRESULT Initialize(HINSTANCE hInstance, LPCSTR szWindowName, 
        LPCSTR szClassName, UINT uWidth, UINT uHeight);


    // Reset
    //     Selects a suitable device given a set of presentation parameters.
    //     AreCapsSufficient is called for each device under consideration, 
    //     and must pass for that device to be selected.  Reset cannot 
    //     be called from within OnDraw.

    HRESULT Reset(D3DPRESENT_PARAMETERS *pPresentParameters);


    // Run
    //     Run the message loop.  While running, OnMessage is called for any 
    //     windows messages received.  In addition, when the app is active, 
    //     Update and Draw are called each frame.  When the app is inactive, 
    //     OnIdle is called.  Run uses the application's first accelerator table 
    //     resource to translate messages.
    //
    // Stop
    //     Causes Run to terminate, once done with current frame.  If Stop is
    //     called during OnUpdate, OnDraw will not be called.

    HRESULT Run();
    HRESULT Stop();


    // IsRunning
    //     Is the application running?
    //
    // IsFullscreen
    //     Are we in fullscreen mode?

    BOOL IsRunning();
    BOOL IsFullscreen();


    // WndProc
    //     This is the window proc which gets used by default.  If you use a custom 
    //     window class, please make sure that this window proc gets called.

    static LRESULT CALLBACK 
        WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


protected:
    // An application generally should not keep its own copies of these
    // objects since the device may be deleted and recreated at any time.
    // (When this happens, the app will be notified using OnDestoryDevice
    // and OnCreateDevice.)

    HINSTANCE             m_hInstance;
    HWND                  m_hwnd;
    LPDIRECT3D9           m_pD3D;
    LPDIRECT3DDEVICE9     m_pDevice;
    D3DCAPS9              m_Caps;
    D3DPRESENT_PARAMETERS m_PresentParameters;


    // Update
    //     Calls OnUpdate and updates timer.  Update can only be called
    //     while application is running.
    //
    // Draw
    //     Calls BeginScene, OnDraw, EndScene, and then Presents the
    //     rendered image to the front buffer.  If the video mode has 
    //     changed on a windowed app, Draw attemtps to recreate the context.
    //     Draw can only be called while the application is running.

    HRESULT Update();
    HRESULT Draw();


    // AreCapsSufficient
    //     Examine device capabilities to determine if a given device is
    //     sufficient for the needs of this application.

    virtual BOOL AreCapsSufficient(D3DCAPS9 *pCaps, DWORD dwBehaviorFlags);


    // OnMessage
    //     Handle Windows messages.  The default implementation handles ESC
    //     keypresses.
    //
    // OnUpdate
    //     Update the scene.  All simulation should be performed here.  
    //
    // OnDraw
    //     Draw the scene.  By the time OnDraw is called, BeginScene has already
    //     been called.  OnDraw does not need to worry about calling BeginScene,
    //     EndScene, or Present.  Reset may not be called from within 
    //     OnDraw.  The default implementation clears the back buffer to black.
    //
    // OnIdle
    //     Perform idle processing.  When possible, idle processing should be 
    //     done in small chunks, so as not to interfere with incoming messages.
    //     When no more idle processing is required, OnIdle should simply call
    //     WaitMessage.  The default implementation just calls WaitMessage.

    virtual LRESULT OnMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual HRESULT OnUpdate(float fSecsPerFrame);
    virtual HRESULT OnDraw(float fAspectRatio);
    virtual HRESULT OnIdle();


    // OnCreateDevice
    //     Called after device creation.  This is a good place to create
    //     managed surfaces.
    //
    // OnResetDevice
    //     Called after reset.  This is a good place to setup device state, and
    //     create volatile resources, like vidmem textures.  OnResetDevice
    //     is always called after OnCreateDevice.
    //
    // OnLostDevice
    //     Called after device is lost.  This is a good place to release 
    //     volatile resources, like vidmem textures.  OnLostDevice is always
    //     called before OnDestroyDevice.
    //
    // OnDestroyDevice
    //     Called before device is released.  This is a good place to release
    //     managed surfaces.

    virtual HRESULT OnCreateDevice();
    virtual HRESULT OnResetDevice();
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnDestroyDevice();


private:
    D3DX_APPLICATION_DATA *m_pData;
};


#endif // __D3DXAPP_H__
