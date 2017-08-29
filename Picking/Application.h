/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once

#include <SceneManagement.h>
#include <Meshes.h>
#include <Camera.h>
#include <VisualDebug.h>
#include <Timer.h>
#include "BasisDrawer.h"

namespace GUI
{

class Font;
class Label;

};

class Application
{
private:
    GUI::Label *fpsLabel = nullptr;
    GUI::Label *infoLabel = nullptr;
    GUI::Font *fpsFont = nullptr;
    Scene::Object3D triangleObj;
    Meshes::Triangle triangle;
    float triangleRotAng = 0.0f;
    Scene::DrawingContainer drawingContainer;
    BasisDrawer drawer;
    Camera::EyeCamera camera;
    Time::Timer timer;
    VisualDebug::Points3DContainer points;
    Point3F p1, p2, p3;
public:
    void Init();
    void Invalidate();
    void Draw();
    void Release();
};