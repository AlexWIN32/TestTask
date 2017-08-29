/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include "Application.h"
#include <GUI/Theme.h>
#include <GUI/Manager.h>
#include <GUI/Font.h>
#include <GUI/Label.h>
#include <MathHelpers.h>
#include <Utils/ToString.h>
#include <DirectInput.h>
#include <CommonParams.h>
#include <Collision.h>

void Application::Init()
{
    timer.Init(60);

    p1 = {-1.0f, -1.0f, 0.0f};
    p2 = {0.0f, 1.0f, 0.0f};
    p3 = {1.0f, -1.0f, 0.0f};

    triangle.Init({p1, {0.0f, 0.9f, 0.0f, 1.0f}},
                  {p2, {0.0f, 0.9f, 0.0f, 1.0f}},
                  {p3, {0.0f, 0.9f, 0.0f, 1.0f}});

    Meshes::MaterialData mat = triangle.GetSubsetMaterial(0);
    mat.colors.ambient = {0.0f, 0.9f, 0.0f, 1.0f};
    triangle.SetSubsetMaterial(0, mat);

    drawer.Init();

    drawingContainer.SetDrawingManager(&triangle, &drawer);
    drawingContainer.AddObject(&triangleObj, &triangle);

    Matrix4x4 projMatrix = Matrix4x4::PerspectiveFovLH(0.25f * Pi, 0.1f, 1000.0f, CommonParams::GetWidthOverHeight());

    camera.SetFlyingMode(true);
    camera.SetProjMatrix(projMatrix);
    camera.SetSpeed(0.5f);
    camera.SetPos({0.0f, 0.0f, -3.0f});
    camera.SetDir({0.0f, 0.0f, 1.0f});

    GUI::Theme theme;
    theme.Load("GuiTheme1.xml");

    GUI::Manager::GetInstance()->Init();
    GUI::Manager::GetInstance()->AddExternalTheme(theme);
    GUI::Manager::GetInstance()->SetTheme("GuiTheme1");

    fpsFont = new GUI::Font();
    fpsFont->Init("ArialBlack20.fnt");

    fpsLabel = new GUI::Label();
    fpsLabel->Init(fpsFont);
    GUI::Manager::GetInstance()->AddControl(fpsLabel);

    infoLabel = new GUI::Label();
    infoLabel->Init(&GUI::Manager::GetInstance()->GetTheme().GetFont());
    infoLabel->SetPos({0.0f, 0.05f});
    infoLabel->SetCaption(L"to move camera hold left shift and use standart FPS controls");

    GUI::Manager::GetInstance()->AddControl(infoLabel);

    points.Init();
}

void Application::Invalidate()
{
    timer.Invalidate();

    float tf = (float)timer.GetTimeFactor();

    fpsLabel->SetCaption(Utils::ToWString(timer.GetFps()));

    GUI::Manager::GetInstance()->SetEplasedTime((float)timer.GetEplasedTime());

    GUI::Manager::GetInstance()->Invalidate(tf);

    points.Clear();

    triangleRotAng += tf * 0.01f;
    triangleObj.SetRotation({0.0f, triangleRotAng, 0.0f});
    triangleObj.SetPos(Math::SphericalToDec(triangleRotAng * 0.5f, Pi * 0.5f, 10.0f));

    if(DirectInput::GetInsance()->IsKeyboardDown(DIK_LSHIFT))
        camera.Invalidate(tf);

    POINT cursorPs = DirectInput::GetInsance()->GetCursorPos();

    Point2F cursorPos;
    cursorPos.x = (FLOAT)cursorPs.x / CommonParams::GetScreenWidth();
    cursorPos.y = (FLOAT)cursorPs.y / CommonParams::GetScreenHeight();

    Point2F ndc;
    ndc.x = -1.0f + cursorPos.x * 2.0f;
    ndc.y =  1.0f - cursorPos.y * 2.0f;

    Matrix4x4 invWorlViewProj = Matrix4x4::Inverse(triangleObj.GetWorldMatrix() *
                                                   camera.GetViewMatrix() *
                                                   camera.GetProjMatrix());

    Point3F eyePosL = Matrix4x4::Transform(invWorlViewProj, Point3F{ndc.x, ndc.y, 0.0f}, true);
    Point3F cursorPtL = Matrix4x4::Transform(invWorlViewProj, Point3F{ndc.x, ndc.y, 1.0f}, true);

    bool isCollision = false;

    Point3F interPoint;
    if(Collision::TriangleVsLine(p1, p2, p3, eyePosL, cursorPtL, interPoint)){

        Point3F interPointW = Matrix4x4::Transform(triangleObj.GetWorldMatrix(), interPoint);

        points.AddPoint(interPointW, {1.0f, 0.0f, 0.0f, 0.0f}, 0.01f);

        isCollision = true;
    }

    if(DirectInput::GetInsance()->IsMousePress(0)){

        std::ostringstream os_;
        os_ << ((isCollision) ? "you clicked at triangle" : "you clicked at empty space") << std::endl;
        OutputDebugStringA( os_.str().c_str() );
    }
}

void Application::Draw()
{
    drawingContainer.Draw(&camera);

    points.Draw(camera);

    GUI::Manager::GetInstance()->Draw();
    GUI::Manager::GetInstance()->DrawSystemControls();
}

void Application::Release()
{
    delete fpsFont;
    delete fpsLabel;
    delete infoLabel;
}