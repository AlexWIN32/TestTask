/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <Exception.h>
#include <Vector2.h>
#include <MeshesFwd.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <stdint.h>
#include <Utils/VertexArray.h>
#include <Utils/SharedCOM.h>

namespace Meshes
{

DECLARE_EXCEPTION(VertexMetadataException);

class IMesh
{
public:
	virtual ~IMesh(){}
	virtual void Release() = 0;
	virtual void Draw(INT SubsetNumber = -1) const throw (Exception) = 0;
	virtual INT GetSubsetCount() const = 0;
	virtual const MaterialData &GetSubsetMaterial(size_t SubsetNumber) const throw (Exception) = 0;
	virtual const VertexMetadata &GetVertexMetadata() const throw (Exception) = 0;
	virtual void SetSubsetMaterial(size_t SubsetNumber, const MaterialData &Material) throw (Exception) = 0;
};

class Triangle : public IMesh
{
public:
    struct VertexDefinition
    {
        Point3F pos = {0.0f, 0.0f, 0.0f};
        ColorF color = {1.0f, 1.0f, 1.0f, 1.0f};
        VertexDefinition(){}
        VertexDefinition(const Point3F &Pos, const ColorF &Color) : pos(Pos), color(Color) {}
    };
private:
    VertexMetadata vertexMetadata;
    Utils::SharedCOM<ID3D11Buffer> vertexBuffer;
    MaterialData material;
    UINT vertexSize = 0;
public:
    Triangle(){}
    virtual ~Triangle(){}
    void Init(const VertexDefinition &A, const VertexDefinition &B, const VertexDefinition &C) throw (Exception);
    virtual void Release();
    virtual void Draw(INT SubsetNumber = -1) const throw (Exception);
    virtual INT GetSubsetCount() const {return 1;}
    virtual const VertexMetadata &GetVertexMetadata() const throw (Exception){return vertexMetadata;}
    virtual const MaterialData &GetSubsetMaterial(size_t SubsetNumber) const throw (Exception);
    virtual void SetSubsetMaterial(size_t SubsetNumber, const MaterialData &Material) throw (Exception);
};

}