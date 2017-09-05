/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <windows.h>
#include <Timer.h>
#include <vector>
#include "Circle.h"

class Application
{
private:
    Vector2 circleDir;
    float circleSpeed = 2.0f;
    Circle mainCircle;
    std::vector<Circle> obstacles;
    Time::Timer timer;
    HWND wnd = nullptr;
    bool lButtonDown = false;
    std::vector<Vector2> areaEdgesNormals;
    void CheckEdgesCollision();
    void CheckObstaclesCollision();
    void Draw(HWND Hwnd);
    void ProcessClick(const Point2 &Coords);
public:
    void Init(HWND Wnd);
    void Invalidate();
    void Release();
    LRESULT ProcessMessage(HWND Hwnd,
                           UINT Msg,
                           WPARAM WParam,
                           LPARAM LParam);
};