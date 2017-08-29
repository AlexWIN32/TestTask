/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <CommonParams.h>

FLOAT CommonParams::screenWidth = 0.0f;
FLOAT CommonParams::screenHeight = 0.0f;
FLOAT CommonParams::widthOverHeight = 0.0f;
FLOAT CommonParams::heightOverWidth = 0.0f;
D3DXVECTOR2 CommonParams::pixelSize;
HWND CommonParams::window;

 void CommonParams::SetScreenSize(UINT Width, UINT Height)
{
    screenWidth = (float)Width;
    screenHeight = (float)Height;

    widthOverHeight = screenWidth / screenHeight;
    heightOverWidth = screenHeight / screenWidth;

    pixelSize.x = 1.0f / screenWidth;
    pixelSize.y = 1.0f / screenHeight;
}