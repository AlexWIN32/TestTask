/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <D3DHeaders.h>
#include <InitFunctions.h>
#include <DeviceKeeper.h>
#include <GUI/Manager.h>
#include <DirectInput.h>
#include <RenderStatesManager.h>
#include <SamplerStatesManager.h>
#include <CommonParams.h>
#include "Application.h"

Application app;

static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg){
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    const LONG width = 800;
    const LONG height = 600;

    D3DData d3d;

    try{
        HWND hWindow = InitWindow(hInstance, width, height, L"Picking test", MsgProc);

        D3DParams params;
        params.clientWidth = width;
        params.clientHeight = height;
        params.isWindowed = true;
        params.window = hWindow;

        d3d = InitD3D(params);

        DeviceKeeper::SetDevice(d3d.device);
        DeviceKeeper::SetDeviceContext(d3d.context);
        DeviceKeeper::SetDepthStencilView(d3d.depthStencilView);
        DeviceKeeper::SetRenderTargetView(d3d.renderTargetView);
        DeviceKeeper::SetSwapChain(d3d.swapChain);
        DeviceKeeper::SetMultisampleCount(params.multisampleCount);
        DeviceKeeper::SetMultisampleQuality(d3d.msaaQuality);

        CommonParams::SetScreenSize(width, height);
        CommonParams::SetWindow(hWindow);

        DirectInput::GetInsance()->Init(hInstance, hWindow);

        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;

        RenderStates::Manager::GetInstance()->CreateRenderState("NoCull", rasterDesc);
        RenderStates::Manager::GetInstance()->ApplyState("NoCull");

        app.Init();

    }catch(const Exception &ex){

        app.Release();

        GUI::Manager::ReleaseInstance();
        RenderStates::Manager::ReleaseInstance();
        SamplerStates::Manager::ReleaseInstance();
        DeviceKeeper::ReleaseResources();

        MessageBoxA(0, ex.What().c_str(), 0, 0);
        return 0;
    }

    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }else{
            DirectInput::GetInsance()->Poll();

            if(DirectInput::GetInsance()->IsKeyboardPress(DIK_ESCAPE)){
                PostQuitMessage(0);
                break;
            }

            app.Invalidate();

            float color[4];
            color[0] = 1.0f;//Red
            color[1] = 1.0f;//Green
            color[2] = 1.0f;//Blue
            color[3] = 1.0f;//Alpha

            DeviceKeeper::GetDeviceContext()->ClearRenderTargetView(DeviceKeeper::GetRenderTargetView(), color);
            DeviceKeeper::GetDeviceContext()->ClearDepthStencilView(DeviceKeeper::GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            app.Draw();

            HR(DeviceKeeper::GetSwapChain()->Present(0, 0));
        }

    app.Release();

    GUI::Manager::ReleaseInstance();
    RenderStates::Manager::ReleaseInstance();
    SamplerStates::Manager::ReleaseInstance();
    DeviceKeeper::ReleaseResources();

    return (int)msg.wParam;	

}