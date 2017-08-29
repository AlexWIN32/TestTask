/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <Shader.h>
#include <Exception.h>
#include <vector>
#include <map>
#include <string>
#include <Vector2.h>
#include <GUI/Control.h>
#include <SceneManagement.h>
#include <Utils/SharedCOM.h>
#include <Meshes.h>
#include <stdint.h>

namespace Camera
{
    class ICamera;
};

namespace GUI
{
    class Label;
};

namespace VisualDebug
{

DECLARE_EXCEPTION(Points3DContainerException)

class Points3DContainer final
{
private:
    struct PointDescription
    {
        Point3F pos = {0.0f, 0.0f, 0.0f};
        ColorF col = {1.0f, 1.0f, 1.0f, 1.0f};
        float size = 1.0f;
    };
    typedef std::vector<PointDescription> PointsStorage;
    PointsStorage points;
    static const uint32_t InitialVertsCnt = 100;
    uint32_t maxVerts = InitialVertsCnt;
    Utils::SharedCOM<ID3D11Buffer> vertexBuffer;
    Shaders::ShadersSet shaders;
    bool disableDepthWriting = true;
public:
    void Init(const Vector3 &Scale = {0.0f, 0.5f, 0.0f}, const RangeF &Size = {0.01f, 2.0f}) throw (Exception);
    void AddPoint(const Point3F &Pos, const ColorF &Color, float Size) throw (Exception);
    void Draw(const Camera::ICamera &Camera);
    void Clear(){points.clear();}
    void SetDisableDepthWriting(bool NewValue){disableDepthWriting = NewValue;}
    bool GetDisableDepthWriting() const {return disableDepthWriting;}
};

}