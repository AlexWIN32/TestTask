/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Vector2.h>
#include <windows.h>

class Circle
{
private:
    Point2F pos;
    ColorUC color;
    float radius = 0.0f;
public:
    virtual ~Circle(){}
    const Point2F &GetPos() const {return pos;}
    const ColorUC &GetColor() const {return color;}
    void SetColor(const ColorUC &NewValue) {color = NewValue;}
    float GetRadius() const {return radius;}
    void SetRadius(float NewValue) {radius = NewValue;}
    void SetPos(const Point2F &NewValue) {pos = NewValue;}
    void Draw(HDC BackBufferHdc) const;
};