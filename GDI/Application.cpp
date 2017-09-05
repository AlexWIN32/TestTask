/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include "Application.h"
#include <MathHelpers.h>
#include <CommonParams.h>
#include <WindowsX.h>
#include <ctime>


void Application::CheckEdgesCollision()
{
    const Point2F &pos = mainCircle.GetPos();
    float radius = mainCircle.GetRadius();

    if(pos.x - radius <= 0.0f ||
       pos.x + radius >= CommonParams::GetScreenWidth() ||
       pos.y - radius <= 0.0f ||
       pos.y + radius >= CommonParams::GetScreenHeight())
    {
        Point2F newPos = mainCircle.GetPos();

        Vector2 norm;

        if(pos.x - radius <= 0){

            norm = areaEdgesNormals[0];

            newPos.x = radius;

        }else if(pos.x + radius >= CommonParams::GetScreenWidth()){

            norm = areaEdgesNormals[2];

            newPos.x = CommonParams::GetScreenWidth() - radius;

        }else if(pos.y - radius <= 0){

            norm = areaEdgesNormals[3];

            newPos.y = radius;

        }else{
            norm = areaEdgesNormals[1];

            newPos.y = CommonParams::GetScreenHeight() - radius;
        }

        mainCircle.SetPos(newPos);

        circleDir = circleDir - norm * 2.0f * Vector2::Dot(circleDir, norm);

        Vector2 right = {-circleDir.y, circleDir.x};

        circleDir = Vector2::Normalize(circleDir + right * Math::Rand(0.0f, 0.5f));
    }
}

void Application::CheckObstaclesCollision()
{
    std::vector<const Circle *> collidedObstacles;

    for(const Circle &obstacle : obstacles){

        Vector2 toObstacle = obstacle.GetPos() - mainCircle.GetPos();

        float len = toObstacle.Lenght();

        float testDepth = obstacle.GetRadius() + mainCircle.GetRadius();

        if(len <= testDepth){

            Vector2 contactNormal = -Vector2::Normalize(toObstacle);
            
            float penetrationDepth = testDepth - len;

            mainCircle.SetPos(mainCircle.GetPos() + contactNormal * penetrationDepth);

            circleDir = contactNormal;

            circleSpeed = Math::Rand(1.0f, 10.0f);

            collidedObstacles.push_back(&obstacle);
        }
    }

    std::vector<Circle> newObstacles;

    for(const Circle &o : obstacles){
        bool found = false;

        for(const Circle *cO : collidedObstacles)
            if(&o == cO){
                found = true;
                break;
            }

        if(!found)
            newObstacles.push_back(o);
    }

    obstacles = newObstacles;
}

void Application::Draw(HWND Hwnd)
{
    PAINTSTRUCT pData;
    HDC hdc = BeginPaint(Hwnd, &pData);

    RECT rect;
    GetClientRect(Hwnd, &rect);

    HDC backbuffDC = CreateCompatibleDC(hdc);

    HBITMAP backbuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

    int savedDC = SaveDC(backbuffDC);

    SelectObject(backbuffDC, backbuffer);

    HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));
    FillRect(backbuffDC,&rect,hBrush);
    DeleteObject(hBrush);

    mainCircle.Draw(backbuffDC);

    for(const Circle &obstacle : obstacles)
        obstacle.Draw(backbuffDC);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, backbuffDC, 0, 0, SRCCOPY);

    RestoreDC(backbuffDC, savedDC);

    DeleteObject(backbuffer);
    DeleteDC(backbuffDC);

    EndPaint(Hwnd, &pData);
}

void Application::ProcessClick(const Point2 &Coords)
{
    Circle newCircle;

    newCircle.SetPos(Cast<Point2F>(Coords));
    newCircle.SetRadius(Math::Rand(10.0f, 50.0f));
    newCircle.SetColor({0, 255, 0, 0});

    obstacles.push_back(newCircle);
}

void Application::Init(HWND Wnd)
{
    wnd = Wnd;

    timer.Init(60);

    srand(time(nullptr));

    circleDir = Vector2::Normalize({Math::Rand(-1.0f, 1.0f), Math::Rand(-1.0f, 1.0f)});

    mainCircle.SetPos({CommonParams::GetScreenWidth() * 0.5f, CommonParams::GetScreenHeight() * 0.5f});
    mainCircle.SetRadius(10.0f);
    mainCircle.SetColor({255, 0, 0, 0});

    areaEdgesNormals = {
        {1.0f, 0.0f},//left
        {0.0f, -1.0f},//right
        {-1.0f, 0.0f},//bottom
        {0.0f, 1.0f},//top
    };
}

void Application::Invalidate()
{
    timer.Invalidate();

    float Tf = (float)timer.GetTimeFactor();

    mainCircle.SetPos(mainCircle.GetPos() + circleDir * circleSpeed * Tf);

    CheckEdgesCollision();
    CheckObstaclesCollision();

    InvalidateRect(wnd, nullptr, FALSE);
}

void Application::Release()
{
}

LRESULT Application::ProcessMessage(HWND Hwnd,
                                    UINT Msg,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    switch (Msg){
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        Draw(Hwnd);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        lButtonDown = true;
        return 0;
    }
    case WM_LBUTTONUP:
    {
        if(lButtonDown){

            ProcessClick({GET_X_LPARAM(LParam), GET_Y_LPARAM(LParam)});

            lButtonDown = false;
        }
        return 0;
    }
    case WM_SIZE :
    {
        CommonParams::SetScreenSize(static_cast<float>(GET_X_LPARAM(LParam)),
                                    static_cast<float>(GET_Y_LPARAM(LParam)));
        return 0;
    }
    default:
        return DefWindowProc(Hwnd, Msg, WParam, LParam);
    }
}


