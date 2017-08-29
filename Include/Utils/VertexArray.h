/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <vector>
#include <string>
#include <map>
#include <Exception.h>
#include <Vector2.h>
#include <MeshesFwd.h>

namespace Utils
{

namespace DirectX
{

DECLARE_EXCEPTION(VertexArrayException);
DECLARE_CHILD_EXCEPTION(SemanticNotFoundException, VertexArrayException);
DECLARE_CHILD_EXCEPTION(InvalidSemanticIndexException, VertexArrayException);
DECLARE_CHILD_EXCEPTION(InvalidDataException, VertexArrayException);
DECLARE_CHILD_EXCEPTION(IndexOutOfRangeException, VertexArrayException);
DECLARE_CHILD_EXCEPTION(InvalidSemanticTypeException, VertexArrayException);

class VertexArray final
{
public:
    typedef std::vector<std::string> SemanticNamesStorage;
private:
    struct Element
    {
        UINT offset = 0;
        UINT size = 0;
    };
    class PackedData
    {
    private:
        std::vector<char> rawData;
        UINT dataSize = 0;
        template <class TVar>
        void Init(const TVar &Var)
        {
             dataSize = sizeof(TVar);
             rawData.resize(dataSize);

             const char *ptr = reinterpret_cast<const char*>(&Var);

             std::copy(ptr, ptr + dataSize, rawData.begin());
        }
    public:
        PackedData(FLOAT Float){Init(Float);}
        PackedData(const D3DXVECTOR2 &Vec2){Init(Vec2);}
        PackedData(const D3DXVECTOR3 &Vec3){Init(Vec3);}
        PackedData(const D3DXVECTOR4 &Vec4){Init(Vec4);}
        PackedData(const D3DXCOLOR &Col){Init(Col);}
        PackedData(const Vector2 &Vec2){Init(Vec2);}
        PackedData(const Point2F &Pt2){Init(Pt2);}
        PackedData(const Vector3 &Vec3){Init(Vec3);}
        PackedData(const Point3F &Pt3){Init(Pt3);}
        PackedData(const Point4F &Pt4){Init(Pt4);}
        PackedData(const ColorF &Col){Init(Col);}
        const char* GetData() const {return &rawData[0];}
        UINT GetDataSize() const {return dataSize;}
    };
    typedef std::map<std::string, Element> VertexElementsStorage;
    VertexElementsStorage vertexElements;
    Meshes::VertexMetadata vertexMetadata;
    mutable std::vector<char> rawData;
    UINT vertexSize = 0, verticesCount = 0;
    void SetElementRawData(const std::string &SemanticName, UINT Index, const char *Data) throw (Exception);
    char *GetElementRawData(const std::string &SemanticName, UINT Index) const throw (Exception);
    void ChangelDataStorage(const Meshes::VertexMetadata &NewVertexMeta, uint32_t NewVertexSize);
public:
    void Init(const Meshes::VertexMetadata &Metadata, UINT VerticesCount = 0) throw (Exception);
    void SetVertexMetadata(const Meshes::VertexMetadata &Metadata) throw (Exception);
    template<class TData>
    const TData &Get(const std::string &SemanticName, UINT Index) const throw (Exception)
    {
        const char* rawData = GetElementRawData(SemanticName, Index);
        return *reinterpret_cast<const TData*>(rawData);
    }
    template<class TData>
    TData &Get(const std::string &SemanticName, UINT Index) throw (Exception)
    {
        char* rawData = GetElementRawData(SemanticName, Index);
        return *reinterpret_cast<TData*>(rawData);
    }
    template<class TData>
    void Set(const std::string &SemanticName, UINT Index, const TData &Data) throw (Exception)
    {
        const char* rawData = reinterpret_cast<const char*>(&Data);
        SetElementRawData(SemanticName, Index, rawData);
    }
    template<typename... Args>
    void Set(const SemanticNamesStorage &SemanticNames, UINT Index, const Args&... Data) throw (Exception)
    {
        std::vector<PackedData> packedData = {Data...};

        for(size_t i = 0; i < SemanticNames.size(); i++)
            SetElementRawData(SemanticNames[i], Index, packedData[i].GetData());
    }
    const char* GetRawData() const {return &rawData[0];}
    char* GetRawData() {return &rawData[0];}
    UINT GetVerticesCount() const {return verticesCount;}
    UINT GetVertixSize() const {return vertexSize;}
    const Meshes::VertexMetadata &GetVertexMetadata() const {return vertexMetadata;}
    void ChangeCount(UINT NewCount);
    void Clear();
};

}

}