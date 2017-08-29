/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Meshes.h>
#include <Texture.h>
#include <DeviceKeeper.h>
#include <Utils/ToString.h>
#include <Utils/DirectX.h>
#include <Utils/Algorithm.h>
#include <Vector2.h>
#include <cstdio>
#include <memory>
#include <algorithm>

namespace Meshes
{

void VertexMetadata::Clear()
{
    meta.clear();
    semantics.clear();
}

VertexMetadata::VertexMetadata(const VertexMetadata &Val)
{
    Clear();

    meta = Val.meta;

    ResetNames();
}

VertexMetadata &VertexMetadata::operator = (const VertexMetadata &Val)
{
    Clear();

    meta = Val.meta;

    ResetNames();

    return *this;
}

void VertexMetadata::ResetNames()
{
    std::vector<SemanticData> tmpSemantics;
    
    for(Element &elem : meta)
        tmpSemantics.push_back({elem.SemanticName, elem.SemanticIndex});

    semantics = tmpSemantics;

    for(size_t i = 0; i < meta.size(); i++)
        meta[i].SemanticName = semantics[i].name.c_str();
}

VertexMetadata::VertexMetadata(const std::initializer_list<Element> &Elements)
{
    Clear();

    meta.insert(meta.end(), Elements.begin(), Elements.end());

    ResetNames();
}

VertexMetadata VertexMetadata::Concatenate(const VertexMetadata &Meta) const
{
    std::vector<Element> newElems = Utils::Concatenate(meta, Meta.meta);

    VertexMetadata newMeta;
    newMeta.meta = newElems;
    newMeta.ResetNames();

    return newMeta;
}

void VertexMetadata::Add(const Element &NewElement)
{
    meta.push_back(NewElement);

    ResetNames();
}

void VertexMetadata::Add(const Element &NewElement, size_t Pos)
{
    if( Pos > meta.size())
        throw VertexMetadataException("invalid tangent pos in vertex");

    meta.insert(meta.begin() + Pos, NewElement);

    ResetNames();
}

void VertexMetadata::Remove(const SemanticData &ElementToRemove)
{
    auto it = std::find_if(meta.begin(), meta.end(), [&](const Element &V)
    {
        return std::string(V.SemanticName) == ElementToRemove.name && V.SemanticIndex == ElementToRemove.index;
    });

    if(it != meta.end())
        meta.erase(it);

    ResetNames();
}

void Triangle::Init(const VertexDefinition &A, const VertexDefinition &B, const VertexDefinition &C) throw (Exception)
{
    vertexMetadata =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    Vector3 d1 = A.pos - B.pos, d2 = C.pos - B.pos;

    Vector3 normal = Vector3::Normalize(Vector3::Cross(d1, d2));

    Utils::DirectX::VertexArray verts;
    verts.Init(vertexMetadata, 3);

    verts.Set({"POSITION0", "NORMAL0", "COLOR0"}, 0, A.pos, A.color, normal);
    verts.Set({"POSITION0", "NORMAL0", "COLOR0"}, 1, B.pos, B.color, normal);
    verts.Set({"POSITION0", "NORMAL0", "COLOR0"}, 2, C.pos, C.color, normal);

    vertexBuffer = Utils::DirectX::CreateBuffer(verts);

    vertexSize = verts.GetVertixSize();
}

void Triangle::Release()
{
    vertexBuffer.Release();
    vertexMetadata.Clear();
}

void Triangle::Draw(INT SubsetNumber) const throw (Exception)
{
    if(SubsetNumber != -1 && SubsetNumber != 0)
        throw MeshException("Invalid subset number");

    Utils::DirectX::SetPrimitiveStream({vertexBuffer}, nullptr, {vertexSize});

    DeviceKeeper::GetDeviceContext()->Draw(3, 0);
}

const MaterialData &Triangle::GetSubsetMaterial(size_t SubsetNumber) const throw (Exception)
{
    if(SubsetNumber != 0)
        throw MeshException("Invalid subset number");

    return material;
}

void Triangle::SetSubsetMaterial(size_t SubsetNumber, const MaterialData &Material) throw (Exception)
{
    if(SubsetNumber != 0)
        throw MeshException("Invalid subset number");

    material = Material;
}

}