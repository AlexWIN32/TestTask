/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Utils/VertexArray.h>
#include <Utils/SemanticSize.h>
#include <Utils/ToString.h>
#include <Utils/Algorithm.h>
#include <algorithm>
#include <numeric>
#include <stdint.h>
#include <Meshes.h>

namespace Utils
{

namespace DirectX
{

void VertexArray::SetElementRawData(const std::string &SemanticName, UINT Index, const char *Data) throw (Exception)
{
    if(Index >= verticesCount)
        throw IndexOutOfRangeException("index " + Utils::ToString(Index) + " is out of range");

    const Element &elem = Utils::Find(vertexElements, SemanticName, SemanticNotFoundException(SemanticName + " semantic not found"));

    int offset = Index * vertexSize + elem.offset;

    memcpy(&rawData[offset], Data, elem.size);
}

char *VertexArray::GetElementRawData(const std::string &SemanticName, UINT Index) const throw (Exception)
{
    if(Index >= verticesCount)
        throw IndexOutOfRangeException("index " + Utils::ToString(Index) + " is out of range");

    const Element &elem = Utils::Find(vertexElements, SemanticName, SemanticNotFoundException(SemanticName + " semantic not found"));

    return &rawData[Index * vertexSize + elem.offset];
}

void VertexArray::ChangelDataStorage(const Meshes::VertexMetadata &NewVertexMeta, uint32_t NewVertexSize)
{
    if(vertexMetadata.Size() == 0)
        rawData.resize(NewVertexSize * verticesCount);
    else{

        std::vector<char> newData(NewVertexSize * verticesCount);

        for(uint32_t v = 0; v < verticesCount; v++)
            for(const D3D11_INPUT_ELEMENT_DESC &elem : NewVertexMeta){
                
                auto ci = std::find_if(vertexMetadata.begin(), vertexMetadata.end(), [&](const D3D11_INPUT_ELEMENT_DESC &V)
                {
                    return std::string(V.SemanticName) == elem.SemanticName && V.SemanticIndex == elem.SemanticIndex;
                });

                if(ci != vertexMetadata.end()){

                    uint32_t semanticSize = GetSemanticByteSize(ci->Format);

                    uint32_t newDataOffset = v * NewVertexSize + elem.AlignedByteOffset;
                    uint32_t oldDataOffset = v * vertexSize + ci->AlignedByteOffset;

                    std::copy(rawData.begin() + oldDataOffset,
                              rawData.begin() + oldDataOffset + semanticSize, 
                              newData.begin() + newDataOffset);
                }
            }

        rawData = newData;
    }
}

void VertexArray::SetVertexMetadata(const Meshes::VertexMetadata &Metadata) throw (Exception)
{
    if(!Metadata.Size())
        throw VertexArrayException("empty vertex metadata");

    uint32_t newVertexSize = 0;

    for(const D3D11_INPUT_ELEMENT_DESC &elem : Metadata){

        if(elem.AlignedByteOffset == D3D11_APPEND_ALIGNED_ELEMENT)
            throw InvalidSemanticTypeException("APPEND_ALIGNED_ELEMENT not supported");

        if(GetSemanticByteSize(elem.Format) == 0)
            throw VertexArrayException("invalid vertex metadata: vertex size is equal to zero");

        Element newElem;
        newElem.offset = elem.AlignedByteOffset;
        newElem.size = GetSemanticByteSize(elem.Format);
        
        std::string semanticName = Utils::ToString(elem.SemanticName, elem.SemanticIndex);

        vertexElements.insert({semanticName, newElem});
        
        newVertexSize += newElem.size;
    }

    ChangelDataStorage(Metadata, newVertexSize);

    vertexMetadata = Metadata;
    vertexSize = newVertexSize;
}

void VertexArray::Init(const Meshes::VertexMetadata &Metadata, UINT VerticesCount) throw (Exception)
{
    verticesCount = VerticesCount;

    SetVertexMetadata(Metadata);
}

void VertexArray::Clear()
{
    rawData.clear();
    verticesCount = 0;
}

void VertexArray::ChangeCount(UINT NewCount)
{
    rawData.resize(vertexSize * NewCount);
    verticesCount = NewCount;
}

}

}