/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Vector2.h>
#include <vector>
#include <functional>
#include <Shader.h>
#include <Utils/ToString.h>
#include <Utils/SharedCOM.h>
#include <SceneManagement.h>

DECLARE_EXCEPTION(InvalidLodException);

class BasisDrawer : public Scene::IMeshDrawManager
{
private:
    Shaders::ShadersSet shaders;
public:
    void Init();
    void BeginDraw(const Scene::IObject *Object, const Meshes::IMesh *Mesh, const Camera::ICamera * Camera);
    void ProcessMaterial(const Scene::IObject *Object, const Meshes::MaterialData &Material);
};
