/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <VisualDebug.h>
#include <Meshes.h>
#include <Camera.h>
#include <Utils/DirectX.h>
#include <Utils/ToString.h>
#include <MathHelpers.h>
#include <RenderStatesManager.h>
#include <CommonParams.h>

namespace VisualDebug
{

template<typename TData>
static void AddData(const TData &NewData,
                    std::vector<TData> &Storage,
                    Utils::SharedCOM<ID3D11Buffer> &VertexBuffer,
                    UINT &MaxVerts,
                    UINT InitialVertsCnt)
{
    Storage.push_back(NewData);

    if(Storage.size() == MaxVerts){
        MaxVerts += InitialVertsCnt;

        VertexBuffer = Utils::DirectX::CreateBuffer(sizeof(TData) * MaxVerts, 
                                        D3D11_BIND_VERTEX_BUFFER, 
                                        D3D11_USAGE_DYNAMIC, 
                                        D3D11_CPU_ACCESS_WRITE);

        Utils::DirectX::Map<TData>(VertexBuffer, [&](TData* Data)
        {
            std::copy(Storage.begin(), Storage.end(), Data);
        },
        D3D11_MAP_WRITE_NO_OVERWRITE);

    }else
        Utils::DirectX::Map<TData>(VertexBuffer, [&](TData* Data)
        {
            Data[Storage.size() - 1] = NewData;
        },
        D3D11_MAP_WRITE_NO_OVERWRITE);
}

void Points3DContainer::Init(const Vector3 &Scale, const RangeF &Size) throw (Exception)
{
    Meshes::VertexMetadata meta = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    shaders.vs.Load(L"../Resources/Shaders/Points.vs", "ProcessVertex", meta);
    shaders.ps.Load(L"../Resources/Shaders/Points.ps", "ProcessPixel");
    shaders.gs.Load(L"../Resources/Shaders/Points.gs", "ProcessVertex");

    shaders.gs.CreateVariable<Matrix4x4>("invView", 0, 0);
    shaders.gs.CreateVariable<Matrix4x4>("viewProg", 0, 1);

    shaders.gs.CreateVariable<Vector4>("eyePosW", 1, 0);

    shaders.gs.CreateVariable<float>("pointScaleA", 2, 0, Scale.x);
    shaders.gs.CreateVariable<float>("pointScaleB", 2, 1, Scale.y);
    shaders.gs.CreateVariable<float>("pointScaleC", 2, 2, Scale.z);
    shaders.gs.CreateVariable<float>("pointScaleMin", 2, 3, Size.minVal);
    shaders.gs.CreateVariable<float>("pointScaleMax", 2, 4, Size.maxVal);
    shaders.gs.CreateVariable<Vector3>("padding", 2, 5);

    vertexBuffer = Utils::DirectX::CreateBuffer(sizeof(PointDescription) * maxVerts, 
                                D3D11_BIND_VERTEX_BUFFER,
                                D3D11_USAGE_DYNAMIC,
                                D3D11_CPU_ACCESS_WRITE);

    D3D11_DEPTH_STENCIL_DESC sDesc = {};
    sDesc.DepthEnable = true;
    sDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    sDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    RenderStates::Manager::GetInstance()->CreateRenderState("DisableDepthWritingForPoints", {sDesc, 0});
}

void Points3DContainer::AddPoint(const Point3F &Pos, const ColorF &Color, float Size) throw (Exception)
{
    if(Size <= 0.0f)
        throw Points3DContainerException("Invalid point size");

    PointDescription newDesc;
    newDesc.pos = Pos;
    newDesc.col = Color;
    newDesc.size = Size;
    AddData(newDesc, points, vertexBuffer, maxVerts, InitialVertsCnt);
}

void Points3DContainer::Draw(const Camera::ICamera &Camera)
{
    Matrix4x4 viewProg = Camera.GetViewMatrix() * Camera.GetProjMatrix();

    shaders.gs.UpdateVariable("invView", Matrix4x4::Inverse(Camera.GetViewMatrix()));
    shaders.gs.UpdateVariable("viewProg", viewProg);
    shaders.gs.UpdateVariable("eyePosW", Vector4(Camera.GetPos(), 1.0f));

    Utils::DirectX::SetPrimitiveStream({vertexBuffer}, nullptr, {sizeof(PointDescription)}, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    Shaders::Apply(shaders, [&]()
    {
        if(disableDepthWriting)
            RenderStates::Manager::GetInstance()->ProcessWithStates({"DisableDepthWritingForPoints"}, [&]()
            {
                DeviceKeeper::GetDeviceContext()->Draw(points.size(), 0);
            });
        else
            DeviceKeeper::GetDeviceContext()->Draw(points.size(), 0);
    });
}

}