/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include "BasisDrawer.h"
#include <Camera.h>

void BasisDrawer::Init()
{
    Meshes::VertexMetadata vertexMetadata = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    shaders.vs.Load(L"../Resources/Shaders/Basis.vs", "ProcessVertex", vertexMetadata);
    shaders.ps.Load(L"../Resources/Shaders/Basis.ps", "ProcessPixel");

    shaders.vs.CreateVariable<Matrix4x4>("worldViewProj", 0, 0);
    shaders.vs.CreateVariable<Matrix4x4>("worldInvTrans", 0, 1);

    shaders.ps.CreateVariable<ColorF>("color", 0, 0);
}

void BasisDrawer::BeginDraw(const Scene::IObject *Object, const Meshes::IMesh *Mesh, const Camera::ICamera * Camera)
{
    const Matrix4x4 &worldMatrix = Object->GetWorldMatrix();

    shaders.vs.UpdateVariable("worldViewProj", worldMatrix * Camera->GetViewMatrix() * Camera->GetProjMatrix());

    shaders.vs.ApplyVariables();

    shaders.vs.Apply();
    shaders.ps.Apply();
}

void BasisDrawer::ProcessMaterial(const Scene::IObject *Object, const Meshes::MaterialData &Material)
{
    shaders.ps.UpdateVariable("color", Material.colors.ambient);
    shaders.ps.ApplyVariables();
}
