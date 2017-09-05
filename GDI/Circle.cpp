#include "Circle.h"
#include <stdint.h>

void Circle::Draw(HDC BackBufferHdc) const
{
    SelectObject(BackBufferHdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(BackBufferHdc, RGB(color.r,color.g,color.b));
    SelectObject(BackBufferHdc, GetStockObject(DC_PEN));
    SetDCPenColor(BackBufferHdc, RGB(color.r,color.g,color.b));

    int32_t left = static_cast<int32_t>(pos.x - radius);
    int32_t right = static_cast<int32_t>(pos.x + radius);
    int32_t bottom = static_cast<int32_t>(pos.y + radius);
    int32_t top = static_cast<int32_t>(pos.y - radius);

    Ellipse(BackBufferHdc, left, top, right, bottom);
}