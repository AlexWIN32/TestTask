/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <windows.h>
#include <InitWindow.h>
#include "Application.h"
#include <CommonParams.h>

Application app;

static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return app.ProcessMessage(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    const LONG width = 800;
    const LONG height = 600;

    try{
        HWND hWindow = InitWindow(hInstance,
                                  width,
                                  height,
                                  L"GDI",
                                  MsgProc,
                                  (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX));

        CommonParams::SetScreenSize(width, height);

        app.Init(hWindow);

    }catch(const Exception &ex){

        MessageBoxA(0, ex.What().c_str(), 0, 0);
        return 0;
    }

    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }else
            app.Invalidate();
}
