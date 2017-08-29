/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Exception.h>
#include <D3DHeaders.h>
#include <LightiningFwd.h>
#include <Utils/SharedCOM.h>
#include <vector>
#include <map>
#include <string>

namespace Meshes
{

DECLARE_EXCEPTION(MeshException);
DECLARE_EXCEPTION(MeshesContainerException);

enum MeshType
{
    MT_COLLADA_BINARY,
    MT_OBJ,
    MT_3DS
};

struct MaterialData
{
    std::map<std::string, Utils::SharedCOM<ID3D11SamplerState>> samplerStates;
    Utils::SharedCOM<ID3D11ShaderResourceView> colorSRV;
    Utils::SharedCOM<ID3D11ShaderResourceView> normalSRV;
    Lightining::Material colors;
    float specularPower = 10.0f;
};

struct SubsetIndexCoords
{
    INT startIndex = 0, indicesCnt = 0;
};

struct SubsetData
{
    MaterialData material;
    SubsetIndexCoords indexCoords;
};

typedef std::vector<SubsetData> SubsetsStorage;

class VertexMetadata
{
public:
    typedef D3D11_INPUT_ELEMENT_DESC Element;
    struct SemanticData
    {
        std::string name;
        uint32_t index = 0;
        SemanticData(){}
        SemanticData(const std::string &Name, uint32_t Index)
            : name(Name), index(Index) {}
    };
private:
    std::vector<Element> meta;
    std::vector<SemanticData> semantics;
    void ResetNames();
public:
    VertexMetadata(const VertexMetadata &Val);
    VertexMetadata &operator = (const VertexMetadata &Val);
    VertexMetadata(){}
    VertexMetadata(const std::initializer_list<Element> &Elements);
    void Add(const Element &NewElement);
    void Add(const Element &NewElement, size_t Pos);
    VertexMetadata Concatenate(const VertexMetadata &Meta) const;
    void Remove(const SemanticData &SemanticToRemove);
    void Clear();
    size_t Size() const {return meta.size();}
    operator const std::vector<Element>& () const {return meta;}
    std::vector<Element>::iterator begin() {return meta.begin();}
    std::vector<Element>::iterator end() {return meta.end();}
    std::vector<Element>::const_iterator begin() const {return meta.begin();}
    std::vector<Element>::const_iterator end() const {return meta.end();}
};

typedef std::vector<UINT> IndicesStorage;

struct VertexAdjacency
{
    IndicesStorage vertices;
    IndicesStorage indices;
};

typedef std::vector<VertexAdjacency> AdjacencyStorage;

typedef LONG MeshId;

}