/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <DeviceKeeper.h>

ID3D11Device *DeviceKeeper::device = nullptr;
ID3D11DeviceContext *DeviceKeeper::context = nullptr;
ID3D11RenderTargetView* DeviceKeeper::renderTargetView = nullptr;
ID3D11DepthStencilView* DeviceKeeper::depthStencilView = nullptr;
IDXGISwapChain* DeviceKeeper::swapChain = nullptr;
UINT DeviceKeeper::multisampleCount = 0;
UINT DeviceKeeper::multisampleQuality = 0;

void DeviceKeeper::ReleaseResources()
{
    ReleaseCOM(device);
    ReleaseCOM(context);
    ReleaseCOM(renderTargetView);
    ReleaseCOM(depthStencilView);
    ReleaseCOM(swapChain);
}